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

#ifdef DIAG_CLASS
DiagStyle(strain,DiagStrain)
#else

#ifndef PFDD_DIAG_STRAIN_H
#define PFDD_DIAG_STRAIN_H

#include "stdio.h"
#include "diag.h"
#include <vector>

using namespace std;

namespace PFDD_NS {

  class DiagStrain : public Diag {
  public:
    DiagStrain(class PFDD_C *, int, char **);
    ~DiagStrain() {}
    void init();
  void compute();
  void stats(char *);
  void stats_header(char *);

 private:

  double **gstrain;          // Global stress and strain

};

}

#endif
#endif
