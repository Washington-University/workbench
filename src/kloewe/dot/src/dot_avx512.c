/*----------------------------------------------------------------------------
  File    : dot_avx512.c
  Contents: dot product (AVX512-based implementations)
  Author  : Kristian Loewe
----------------------------------------------------------------------------*/
#include "dot_avx512.h"

/*----------------------------------------------------------------------------
  Function Prototypes
----------------------------------------------------------------------------*/
#ifdef __FMA__
extern float  sdot_avx512fma  (const float  *a, const float  *b, int n);
extern double ddot_avx512fma  (const double *a, const double *b, int n);
extern double dsdot_avx512fma (const float  *a, const float  *b, int n);
#else
extern float  sdot_avx512     (const float  *a, const float  *b, int n);
extern double ddot_avx512     (const double *a, const double *b, int n);
extern double dsdot_avx512    (const float  *a, const float  *b, int n);
#endif
