/*----------------------------------------------------------------------------
  File    : dot_sse2.c
  Contents: dot product (SSE2-based implementations)
  Author  : Kristian Loewe
----------------------------------------------------------------------------*/
#include "dot_sse2.h"

/*----------------------------------------------------------------------------
  Function Prototypes
----------------------------------------------------------------------------*/
extern float  sdot_sse2  (const float  *a, const float  *b, int n);
extern double ddot_sse2  (const double *a, const double *b, int n);
extern double dsdot_sse2 (const float  *a, const float  *b, int n);
