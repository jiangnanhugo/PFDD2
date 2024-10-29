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

// define integer data types used by SPPARKS and associated size limits

// smallint = variables for on-procesor system (nlocal, nghost, etc)
// tagint = variables for site IDs (nglobal, id[], etc)
// bigint = variables for total system tallies (naccept, nreject, etc)

// smallint must be an int, as defined by C compiler
// tagint can be 32-bit or 64-bit int, must be >= smallint
// bigint can be 32-bit or 64-bit int, must be >= tagint

// MPI_SPK_TAGINT = MPI data type corresponding to a tagint
// MPI_SPK_BIGINT = MPI data type corresponding to a bigint

// NOTE: if your machine/MPI does not support "long long" ints,
//       but only "long" ints, then you will need to change
//       MPI_LONG_LONG to MPI_LONG, and atoll to atol

#ifndef PFDD_PFDDTYPE_H
#define PFDD_PFDDTYPE_H

#define __STDC_LIMIT_MACROS
#define __STDC_FORMAT_MACROS

#include "limits.h"
#include "stdint.h"
#include "inttypes.h"

namespace PFDD_NS {

// default settings, allows for models with over 2B sites
// 32-bit smallint, 64-bit tagint and bigint

typedef int smallint;
typedef int64_t tagint;
typedef int64_t bigint;

#define MAXSMALLINT INT_MAX
#define MAXTAGINT INT64_MAX
#define MAXBIGINT INT64_MAX

#define MPI_PFDD_TAGINT MPI_LONG_LONG
#define MPI_PFDD_BIGINT MPI_LONG_LONG

#define TAGINT_FORMAT "%" PRId64
#define BIGINT_FORMAT "%" PRId64

#define ATOTAGINT atoll
#define ATOBIGINT atoll


}

#endif
