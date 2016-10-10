#ifndef DOT_WRAPPER_H
#define DOT_WRAPPER_H

#ifdef __cplusplus
extern "C"
{
#endif

#ifdef _WIN32
#define inline __inline
#endif

#ifdef CARET_DOTFCN
#include "dot.h"
#else
inline double sddot (const float *a, const float *b, int n)
{
  double sum = 0;
  for (int k = 0; k < n; k++)
    sum += a[k] * b[k];
  return sum;
}  // sddot()
//copy enum from dot.h
enum flags {
    DOT_NAIVE  = 1,
    DOT_SSE2   = 2,
    DOT_AVX    = 3,
    DOT_AVXFMA = 4,
    DOT_AUTO   = 100
};
//and dummy implementation of dot_set_impl
inline int dot_set_impl (int)
{
    return DOT_NAIVE;
}
#endif

#ifdef __cplusplus
}
#endif

#endif
