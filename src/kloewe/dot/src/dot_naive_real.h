/*----------------------------------------------------------------------------
  File    : dot_naive_real.h
  Contents: dot product (naive implementation)
  Author  : Kristian Loewe, Christian Borgelt
----------------------------------------------------------------------------*/
#ifndef DOT_NAIVE_REAL_H
#define DOT_NAIVE_REAL_H

/*----------------------------------------------------------------------------
  Inline Functions
----------------------------------------------------------------------------*/

inline REAL dot_naive (const REAL *a, const REAL *b, int n)
{
  REAL sum = 0;
  for (int k = 0; k < n; k++)
    sum += a[k] * b[k];
  return sum;
}  // dot_naive()

#endif // DOT_NAIVE_REAL_H
