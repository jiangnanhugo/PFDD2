/* ----------------------------------------------------------------------
PFDD -- Phase Field Dislocation Dynamics

© 2022. Triad National Security, LLC. All rights reserved.
This program was produced under U.S. Government contract 89233218CNA000001 for
Los Alamos National Laboratory (LANL), which is operated by Triad National
Security, LLC for the U.S. Department of Energy/National Nuclear Security
Administration. All rights in the program are reserved by Triad National
Security, LLC, and the U.S. Department of Energy/National Nuclear Security
Administration. The Government is granted for itself and others acting on its
behalf a nonexclusive, paid-up, irrevocable worldwide license in this material 
to reproduce, prepare derivative works, distribute copies to the public, perform
 publicly and display publicly, and to permit others to do so.
   ------------------------------------------------------------------------- */

#include "math.h"
#include "string.h"
#include "stdlib.h"
#include "app_2d.h"
#include "fft.h"
#include "solve.h"
#include "random_park.h"
#include "error.h"

using namespace PFDD_NS;;

/* ---------------------------------------------------------------------- */

App2D::App2D(PFDD_C *pfdd_p, int narg, char **arg) :
  App(pfdd_p,narg,arg)
{
  ndouble = 0;
  timestep = 1.0;
  //neighshell = 1;
  slip_systems = 1;  // The # of slip systems has to be allocated in the constructure
                      //  fot the FFT class to be able to allocate arrays
  CD = 0.5;           // Dislocation mobility
  oflag = 1;

  if(setup_flag == -1)
    setup_flag = 5; //default to 1L ortho FCC
  if(core_flag == -1)
    core_flag = 1; //default to sin^2
}

/* ---------------------------------------------------------------------- */

App2D::~App2D()
{
  delete [] sites;
}

/* ----------------------------------------------------------------------
   set site value ptrs each time iarray/darray are reallocated
   ------------------------------------------------------------------------- */

void App2D::grow_app()
{
  darray = fft->xi;
}

/* ----------------------------------------------------------------------
   initialize before each run
   check validity of site values
   ------------------------------------------------------------------------- */

void App2D::init_app()
{
  delete [] sites;
  sites = new int[1 + maxneigh];

  grow_app();
  set_slip();
  initial_sxtal();

  stopsteps = static_cast<int>(floor(stoptime/timestep));
}
/* ----------------------------------------------------------------------
   input specific for this app
   ------------------------------------------------------------------------- */
void App2D::input_app(char *command, int narg, char **arg)
{
  error->all(FLERR,"Unrecognized command");
}



void App2D::set_slip()
{
  int i, j, k;
  double s3;
  double xnf=0,ynf=0,znf=0;
  double xbf=0,ybf=0,zbf=0;

  fft->xn[0][0]= 1.0/sqrt(3);
  fft->xn[0][1]= 1.0/sqrt(3);
  fft->xn[0][2]= 1.0/sqrt(3);

  if(oflag == 0){
    fft->xb[0][0]= 1.0/2.0;
    fft->xb[0][1]= -1.0/2.0;
    fft->xb[0][2]= 0.0;
  }
  else if(oflag == 1){
    fft->xb[0][0]= 1.0/2.0;
    fft->xb[0][1]= -1.0/2.0;
    fft->xb[0][2]= 0.0;
  }
  else{
    printf("Initial configuration specified has not been developed yet");
  }
  // Rotation
  xnf = (fft->xn[0][0]*fft->four[0][0] + fft->xn[0][1]*fft->four[0][1] + fft->xn[0][2]*fft->four[0][2])
    /fft->normfour[0];
  ynf = (fft->xn[0][0]*fft->four[1][0] + fft->xn[0][1]*fft->four[1][1] + fft->xn[0][2]*fft->four[1][2])
    /fft->normfour[1];
  znf = (fft->xn[0][0]*fft->four[2][0] + fft->xn[0][1]*fft->four[2][1] + fft->xn[0][2]*fft->four[2][2])
    /fft->normfour[2];

  fft->xn[0][0] = xnf;
  fft->xn[0][1] = ynf;
  fft->xn[0][2] = znf;

  xbf = (fft->xb[0][0]*fft->four[0][0] + fft->xb[0][1]*fft->four[0][1] + fft->xb[0][2]*fft->four[0][2])
    /fft->normfour[0];
  ybf = (fft->xb[0][0]*fft->four[1][0] + fft->xb[0][1]*fft->four[1][1] + fft->xb[0][2]*fft->four[1][2])
    /fft->normfour[1];
  zbf = (fft->xb[0][0]*fft->four[2][0] + fft->xb[0][1]*fft->four[2][1] + fft->xb[0][2]*fft->four[2][2])
    /fft->normfour[2];

  fft->xb[0][0] = xbf;
  fft->xb[0][1] = ybf;
  fft->xb[0][2] = zbf;

  return;
}

/* ----------------------------------------------------------------------
   computes the xi in a given point and slip system
   ------------------------------------------------------------------------- */

double App2D::compute_mean_xi(int n, int flag)
{
  int lN1 = fft->local_x;
  int lxs = fft->local_x_start;
  int N2 = fft->local_y;
  int N3 = fft->local_z;
  int NS = fft->slip_systems;
  int slip = 0;
  int na=-1;

  int i=siteijk[n][0] - lxs;
  int j=siteijk[n][1];
  int k=siteijk[n][2];

  if(flag == 1){ // 1 slip system, real value
    slip = 0;
    na = 2*(i*N2*N3 + j*N3 + k + slip*lN1*N2*N3);
  }
  else if(flag == 2){ // 1 slip system, imaginary value
    slip = 0;
    na = 2*(i*N2*N3 + j*N3 + k + slip*lN1*N2*N3) + 1;
  }

  return fft->xi[0][na];
}

/* ----------------------------------------------------------------------
   computes the stress in a given point
   ------------------------------------------------------------------------- */

double App2D::compute_stress(int n, int flag)
{
  int lN1 = fft->local_x;
  int lxs = fft->local_x_start;
  int N2 = fft->local_y;
  int N3 = fft->local_z;
  int NS = fft->slip_systems;
  int ND = dimension;
  int slip = 0;
  int na=-1;

  int i=siteijk[n][0] - lxs;
  int j=siteijk[n][1];
  int k=siteijk[n][2];

  if(flag == 1)  // pxx
    na = 2*(i*N2*N3 + j*N3 + k + 0*lN1*N2*N3 + 0*lN1*N2*N3*ND);
  else if(flag == 2)  // pyy
    na = 2*(i*N2*N3 + j*N3 + k + 1*lN1*N2*N3 + 1*lN1*N2*N3*ND);
  else if(flag == 3)  // pzz
    na = 2*(i*N2*N3 + j*N3 + k + 2*lN1*N2*N3 + 2*lN1*N2*N3*ND);
  else if(flag == 4)  // pxy
    na = 2*(i*N2*N3 + j*N3 + k + 0*lN1*N2*N3 + 1*lN1*N2*N3*ND);
  else if(flag == 5)  // pxz
    na = 2*(i*N2*N3 + j*N3 + k + 0*lN1*N2*N3 + 2*lN1*N2*N3*ND);
  else if(flag == 6)  // pxz
    na = 2*(i*N2*N3 + j*N3 + k + 1*lN1*N2*N3 + 2*lN1*N2*N3*ND);

  return fft->data_sigma[na];
}

/* ----------------------------------------------------------------------
   computes the strain in a given point
   ------------------------------------------------------------------------- */

double App2D::compute_strain(int n, int flag)
{
  int lN1 = fft->local_x;
  int lxs = fft->local_x_start;
  int N2 = fft->local_y;
  int N3 = fft->local_z;
  int NS = fft->slip_systems;
  int ND = dimension;
  int slip = 0;
  int na=-1;

  int i=siteijk[n][0] - lxs;
  int j=siteijk[n][1];
  int k=siteijk[n][2];

  if(flag == 1)  // exx
    na = 2*(i*N2*N3 + j*N3 + k + 0*lN1*N2*N3 + 0*lN1*N2*N3*ND);
  else if(flag == 2)  // eyy
    na = 2*(i*N2*N3 + j*N3 + k + 1*lN1*N2*N3 + 1*lN1*N2*N3*ND);
  else if(flag == 3)  // ezz
    na = 2*(i*N2*N3 + j*N3 + k + 2*lN1*N2*N3 + 2*lN1*N2*N3*ND);
  else if(flag == 4)  // exy
    na = 2*(i*N2*N3 + j*N3 + k + 0*lN1*N2*N3 + 1*lN1*N2*N3*ND);
  else if(flag == 5)  // exz
    na = 2*(i*N2*N3 + j*N3 + k + 0*lN1*N2*N3 + 2*lN1*N2*N3*ND);
  else if(flag == 6)  // exz
    na = 2*(i*N2*N3 + j*N3 + k + 1*lN1*N2*N3 + 2*lN1*N2*N3*ND);

  return fft->data_eps[na];
}
/* ----------------------------------------------------------------------
   print stats
------------------------------------------------------------------------- */

void App2D::stats(char *strtmp)
{
  char *strpnt = strtmp;
  sprintf(strpnt," %10g %10d",time,steps);
  strpnt += strlen(strpnt);

  // for (int m = 0; m < nspecies; m++) {
  //   sprintf(strpnt," %d",pcount[m]);
  //   strpnt += strlen(strpnt);
  // }
}

/* ----------------------------------------------------------------------
   print stats header
------------------------------------------------------------------------- */

void App2D::stats_header(char *strtmp)
{
  char *strpnt = strtmp;
  sprintf(strpnt," %10s %10s","Time","Step");
  strpnt += strlen(strpnt);

  // for (int m = 0; m < nspecies; m++) {
  //   sprintf(strpnt," %s",sname[m]);
  //   strpnt += strlen(strpnt);
  // }
}
