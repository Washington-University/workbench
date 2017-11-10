/*----------------------------------------------------------------------------
  File    : dot_naive.h
  Contents: dot product (naive implementations)
  Author  : Kristian Loewe, Christian Borgelt
----------------------------------------------------------------------------*/
#ifndef DOT_NAIVE_H
#define DOT_NAIVE_H

/*----------------------------------------------------------------------------
  Function Prototypes
----------------------------------------------------------------------------*/
inline float  sdot_naive  (const float  *a, const float  *b, int n);
inline double ddot_naive  (const double *a, const double *b, int n);
inline double dsdot_naive (const float  *a, const float  *b, int n);

/*----------------------------------------------------------------------------
  Inline Functions
----------------------------------------------------------------------------*/
#ifdef REAL                     // if REAL is already defined, save its
#  include "real-is-double.inc" // original definition based on the value
#  undef REAL                   // of REAL_IS_DOUBLE, then undefine it
#endif
/*--------------------------------------------------------------------------*/

// --- dot product (single precision)
#define REAL float              // (re)define REAL to be float
#define dot_naive sdot_naive
#include "dot_naive_real.h"
#undef dot_naive
#undef REAL

/*--------------------------------------------------------------------------*/
#undef DOT_NAIVE_REAL_H         // undef guard to include header a 2nd time
/*--------------------------------------------------------------------------*/

// --- dot product (double precision)
#define REAL double             // (re)define REAL to be double
#define dot_naive ddot_naive
#include "dot_naive_real.h"
#undef dot_naive
#undef REAL

/*--------------------------------------------------------------------------*/
#undef REAL                     // restore original definition of REAL
#ifdef REAL_IS_DOUBLE           // (if necessary)
#  if REAL_IS_DOUBLE
#    define REAL double
#  else
#    define REAL float
#  endif
#endif
/*--------------------------------------------------------------------------*/

// --- dot product (input: single; intermediate and output: double)
inline double dsdot_naive (const float *a, const float *b, int n)
{
  double sum = 0;
  for (int k = 0; k < n; k++)
    sum += a[k] * b[k];
  return sum;
}  // dsdot_naive()

#endif // DOT_NAIVE_H
