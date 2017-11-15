/*----------------------------------------------------------------------------
  File    : dot_avx.c
  Contents: dot product (AVX-based implementations)
  Author  : Kristian Loewe
----------------------------------------------------------------------------*/
#include "dot_avx.h"

/*----------------------------------------------------------------------------
  Function Prototypes
----------------------------------------------------------------------------*/
#ifdef __FMA__
extern float  sdot_avxfma  (const float  *a, const float  *b, int n);
extern double ddot_avxfma  (const double *a, const double *b, int n);
extern double dsdot_avxfma (const float  *a, const float  *b, int n);
#else
extern float  sdot_avx     (const float  *a, const float  *b, int n);
extern double ddot_avx     (const double *a, const double *b, int n);
extern double dsdot_avx    (const float  *a, const float  *b, int n);
#endif
