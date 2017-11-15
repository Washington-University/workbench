/*----------------------------------------------------------------------------
  File    : dot_avx512.h
  Contents: dot product (AVX512-based implementations)
  Author  : Kristian Loewe, Christian Borgelt
----------------------------------------------------------------------------*/
#ifndef DOT_AVX512_H
#define DOT_AVX512_H

#ifndef __AVX512F__
#  error "AXV512F is not enabled"
#endif

#include <immintrin.h>

/*----------------------------------------------------------------------------
  Function Prototypes
----------------------------------------------------------------------------*/
#ifdef __FMA__
inline float  sdot_avx512fma  (const float  *a, const float  *b, int n);
inline double ddot_avx512fma  (const double *a, const double *b, int n);
inline double dsdot_avx512fma (const float  *a, const float  *b, int n);
#else
inline float  sdot_avx512     (const float  *a, const float  *b, int n);
inline double ddot_avx512     (const double *a, const double *b, int n);
inline double dsdot_avx512    (const float  *a, const float  *b, int n);
#endif

/*----------------------------------------------------------------------------
  Inline Functions
----------------------------------------------------------------------------*/

// --- dot product (single precision)
#ifdef __FMA__
inline float sdot_avx512fma (const float *a, const float *b, int n)
#else
inline float sdot_avx512    (const float *a, const float *b, int n)
#endif
{
  // initialize 16 sums
  __m512 s16 = _mm512_setzero_ps();

  // in each iteration, add 1 product to each of the 16 sums in parallel
  for (int k = 0, nq = 16*(n/16); k < nq; k += 16)
    #ifdef __FMA__
    s16 = _mm512_fmadd_ps(_mm512_loadu_ps(a+k), _mm512_loadu_ps(b+k), s16);
    #else
    s16 = _mm512_add_ps(
            _mm512_mul_ps(_mm512_loadu_ps(a+k), _mm512_loadu_ps(b+k)), s16);
    #endif

  // compute horizontal sum
  float s = _mm512_reduce_add_ps(s16);

  // add the remaining products
  for (int k = 16*(n/16); k < n; k++)
    s += a[k] * b[k];

  return s;
}  // sdot_avx512()

/*--------------------------------------------------------------------------*/

// --- dot product (double precision)
#ifdef __FMA__
inline double ddot_avx512fma (const double *a, const double *b, int n)
#else
inline double ddot_avx512    (const double *a, const double *b, int n)
#endif
{
  // initialize 8 sums
  __m512d s8 = _mm512_setzero_pd();

  // in each iteration, add 1 product to each of the 8 sums in parallel
  for (int k = 0, nq = 8*(n/8); k < nq; k += 8)
    #ifdef __FMA__
    s8 = _mm512_fmadd_pd(_mm512_loadu_pd(a+k), _mm512_loadu_pd(b+k), s8);
    #else
    s8 = _mm512_add_pd(
           _mm512_mul_pd(_mm512_loadu_pd(a+k), _mm512_loadu_pd(b+k)), s8);
    #endif

  // compute horizontal sum
  double s = _mm512_reduce_add_pd(s8);

  // add the remaining products
  for (int k = 8*(n/8); k < n; k++)
    s += a[k] * b[k];

  return s;
}  // ddot_avx512()

/*--------------------------------------------------------------------------*/

// --- dot product (input: single; intermediate and output: double)
#ifdef __FMA__
inline double dsdot_avx512fma (const float *a, const float *b, int n)
#else
inline double dsdot_avx512    (const float *a, const float *b, int n)
#endif
{
  // initialize 8 sums
  __m512d s8 = _mm512_setzero_pd();

  // in each iteration, add 1 product to each of the 4 sums in parallel
  for (int k = 0, nq = 8*(n/8); k < nq; k += 8)
    #ifdef __FMA__
    s8 = _mm512_fmadd_pd(_mm512_cvtps_pd(_mm256_loadu_ps(a+k)),
                         _mm512_cvtps_pd(_mm256_loadu_ps(b+k)), s8);
    #else
    s8 = _mm512_add_pd(_mm512_cvtps_pd(
           _mm256_mul_ps(_mm256_loadu_ps(a+k), _mm256_loadu_ps(b+k))), s8);
    #endif

  // compute horizontal sum
  double s = _mm512_reduce_add_pd(s8);

  // add the remaining products
  for (int k = 8*(n/8); k < n; k++)
    s += a[k] * b[k];

  return s;
}  // dsdot_avx512()

#endif // DOT_AVX512_H
