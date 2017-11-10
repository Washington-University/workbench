/*----------------------------------------------------------------------------
  File:     dot_sse2.h
  Contents: dot product (SSE2-based implementations)
  Authors:  Kristian Loewe, Christian Borgelt
----------------------------------------------------------------------------*/
#ifndef DOT_SSE2_H
#define DOT_SSE2_H

#ifndef __SSE2__
#  error "SSE2 is not enabled"
#endif

#include <emmintrin.h>

// alignment check
#include <stdint.h>
#define is_aligned(POINTER, BYTE_COUNT) \
  (((uintptr_t)(const void *)(POINTER)) % (BYTE_COUNT) == 0)

// horizontal sum variant
#ifdef HORZSUM_SSE3
#  ifdef __SSE3__
#    include <pmmintrin.h>
#  else
#    error "HORZSUM_SSE3 requires SSE3."
#  endif
#endif

/*----------------------------------------------------------------------------
  Function Prototypes
----------------------------------------------------------------------------*/
inline float  sdot_sse2  (const float  *a, const float  *b, int n);
inline double ddot_sse2  (const double *a, const double *b, int n);
inline double dsdot_sse2 (const float  *a, const float  *b, int n);

/*----------------------------------------------------------------------------
  Inline Functions
----------------------------------------------------------------------------*/

// --- dot product (single precision)
inline float sdot_sse2 (const float *a, const float *b, int n)
{
  // initialize total sum
  float s = 0.0f;

  // compute and add up to 3 products without SIMD to achieve alignment
  int aligned = is_aligned(a, 16) && is_aligned(b, 16);
  if (!aligned) {
    int k = 0;
    while (!aligned) {
      s += (*a) * (*b);
      n--; a++; b++;
      aligned = is_aligned(a, 16) && is_aligned(b, 16);
      if (aligned || (++k > 2) || (n == 0))
        break;
    }
  }

  // initialize 4 sums
  __m128 s4 = _mm_setzero_ps();

  // in each iteration, add 1 product to each of the 4 sums in parallel
  if (is_aligned(a, 16) && is_aligned(b, 16))
    for (int k = 0, nq = 4*(n/4); k < nq; k += 4)
      s4 = _mm_add_ps(s4, _mm_mul_ps(_mm_load_ps(a+k), _mm_load_ps(b+k)));
  else
    for (int k = 0, nq = 4*(n/4); k < nq; k += 4)
      s4 = _mm_add_ps(s4, _mm_mul_ps(_mm_loadu_ps(a+k), _mm_loadu_ps(b+k)));

  // compute horizontal sum
  #ifdef HORZSUM_SSE3
  s4 = _mm_hadd_ps(s4,s4);
  s4 = _mm_hadd_ps(s4,s4);
  #else
  s4 = _mm_add_ps(s4, _mm_movehl_ps(s4, s4));
  s4 = _mm_add_ss(s4, _mm_shuffle_ps(s4, s4, 1));
  #endif
  s += _mm_cvtss_f32(s4);  // extract horizontal sum from 1st elem.

  // add the remaining products
  for (int k = 4*(n/4); k < n; k++)
    s += a[k] * b[k];

  return s;
}  // sdot_sse2()

/*--------------------------------------------------------------------------*/

// --- dot product (double precision)
inline double ddot_sse2 (const double *a, const double *b, int n)
{
  // initialize total sum
  double s = 0.0;

  // compute and add up to 1 product without SIMD to achieve alignment
  int aligned = is_aligned(a, 16) && is_aligned(b, 16);
  if (!aligned) {
      s += (*a) * (*b);
      n--; a++; b++;
      aligned = is_aligned(a, 16) && is_aligned(b, 16);
  }

  // compute and add (the bulk of the) products using SSE2 intrinsics
  __m128d s2 = _mm_setzero_pd(); // initalize 2 sums
  if (aligned)
    for (int k = 0, nq = 2*(n/2); k < nq; k += 2)
      s2 = _mm_add_pd(s2, _mm_mul_pd(_mm_load_pd(a+k), _mm_load_pd(b+k)));
  else
    for (int k = 0, nq = 2*(n/2); k < nq; k += 2)
      s2 = _mm_add_pd(s2, _mm_mul_pd(_mm_loadu_pd(a+k), _mm_loadu_pd(b+k)));

  // compute horizontal sum
  #ifdef HORZSUM_SSE3
  s2 = _mm_hadd_pd(s2,s2);
  #else
  s2 = _mm_add_pd(s2, _mm_shuffle_pd(s2, s2, 1));
  #endif
  s += _mm_cvtsd_f64(s2); // extract horizontal sum from 1st elem.

  // add the remaining products
  for (int k = 2*(n/2); k < n; k++)
    s += a[k] * b[k];

  return s;
}  // ddot_sse2()

/*--------------------------------------------------------------------------*/

// --- dot product (input: single; intermediate and output: double)
inline double dsdot_sse2 (const float *a, const float *b, int n)
{
  // initialize total sum
  double s = 0.0;

  // compute and add up to 3 products without SIMD to achieve alignment
  int aligned = is_aligned(a, 16) && is_aligned(b, 16);
  if (!aligned) {
    int k = 0;
    while (!aligned) {
      s += (*a) * (*b);
      n--; a++; b++;
      aligned = is_aligned(a, 16) && is_aligned(b, 16);
      if (aligned || (++k > 2) || (n == 0))
        break;
    }
  }

  // compute and add (the bulk of the) products using SSE2 intrinsics
  __m128d s2  = _mm_setzero_pd(); // initalize 2 sums (aligned or unaligned)
  __m128d s2u = _mm_setzero_pd(); // initalize 2 sums (unaligned)
  // note that _mm_cvtps_pd() converts *the lower two* SPFP values
  if (aligned) {
    for (int k = 0, nq = 4*(n/4); k < nq; k += 4) {
      s2 = _mm_add_pd(s2,
        _mm_cvtps_pd(_mm_mul_ps(_mm_load_ps(a+k), _mm_load_ps(b+k))));
      s2u = _mm_add_pd(s2u,
        _mm_cvtps_pd(_mm_mul_ps(_mm_loadu_ps(a+k+2), _mm_loadu_ps(b+k+2))));
    } }
  else {
    for (int k = 0, nq = 4*(n/4); k < nq; k += 4) {
      s2 = _mm_add_pd(s2,
        _mm_cvtps_pd(_mm_mul_ps(_mm_loadu_ps(a+k), _mm_loadu_ps(b+k))));
      s2u = _mm_add_pd(s2u,
        _mm_cvtps_pd(_mm_mul_ps(_mm_loadu_ps(a+k+2), _mm_loadu_ps(b+k+2))));
    }
  }
  s2 = _mm_add_pd(s2,s2u);

  // compute horizontal sum
  #ifdef HORZSUM_SSE3
  s2 = _mm_hadd_pd(s2, s2);
  #else
  s2 = _mm_add_pd(s2, _mm_shuffle_pd(s2, s2, 1));
  #endif
  s += _mm_cvtsd_f64(s2); // extract horizontal sum from 1st elem.

  // add the remaining products
  for (int k = 4*(n/4); k < n; k++)
    s += a[k] * b[k];

  return s;
}  // dsdot_sse2()

#endif // DOT_SSE2_H
