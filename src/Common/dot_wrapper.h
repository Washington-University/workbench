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
#endif

#ifdef __cplusplus
}
#endif

#endif
