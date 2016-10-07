/*----------------------------------------------------------------------------
  File    : dot_naive.c
  Contents: dot product (naive implementations)
  Author  : Kristian Loewe
----------------------------------------------------------------------------*/
#include "dot_naive.h"

/*----------------------------------------------------------------------------
  Function Prototypes
----------------------------------------------------------------------------*/
extern float  sdot_naive  (const float  *a, const float  *b, int n);
extern double ddot_naive  (const double *a, const double *b, int n);
extern double sddot_naive (const float  *a, const float  *b, int n);
