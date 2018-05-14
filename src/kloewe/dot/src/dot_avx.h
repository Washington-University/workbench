/*----------------------------------------------------------------------------
  File    : dot_avx.h
  Contents: dot product (AVX-based implementations)
  Author  : Kristian Loewe, Christian Borgelt
----------------------------------------------------------------------------*/
#ifndef DOT_AVX_H
#define DOT_AVX_H

#ifndef __AVX__
#  error "AXV is not enabled"
#endif

#include <immintrin.h>
#include <assert.h>

// alignment check
#include <stdint.h>
#define align_rem(PTR, NBYTES) (((uintptr_t)(const void *)(PTR)) % (NBYTES))
#define is_aligned(PTR, NBYTES) (align_rem(PTR,NBYTES) == 0)

/*----------------------------------------------------------------------------
  Function Prototypes
----------------------------------------------------------------------------*/
#ifdef __FMA__
inline float  sdot_avxfma  (const float  *a, const float  *b, int n);
inline double ddot_avxfma  (const double *a, const double *b, int n);
inline double dsdot_avxfma (const float  *a, const float  *b, int n);
#else
inline float  sdot_avx     (const float  *a, const float  *b, int n);
inline double ddot_avx     (const double *a, const double *b, int n);
inline double dsdot_avx    (const float  *a, const float  *b, int n);
#endif

/*----------------------------------------------------------------------------
  Inline Functions
----------------------------------------------------------------------------*/

// --- dot product (single precision)
#ifdef __FMA__
inline float sdot_avxfma (const float *a, const float *b, int n)
#else
inline float sdot_avx    (const float *a, const float *b, int n)
#endif
{
  // initialize total sum
  float s = 0.0f;

  // compute and add up to 7 products without SIMD to achieve alignment
  if (align_rem(a,32) == align_rem(b,32)) {
    int aligned = is_aligned(a, 32) && is_aligned(b, 32);
    if (!aligned) {
      int k = 0;
      while (!aligned) {
        s += (*a) * (*b);
        n--; a++; b++;
        aligned = is_aligned(a, 32) && is_aligned(b, 32);
        if (aligned || (++k > 6) || (n == 0))
          break;
      }
    }
  }

  // initialize 8 sums
  __m256 s8 = _mm256_setzero_ps();

  // in each iteration, add 1 product to each of the 8 sums in parallel
  for (int k = 0, nq = 8*(n/8); k < nq; k += 8)
    #ifdef __FMA__
    s8 = _mm256_fmadd_ps(_mm256_loadu_ps(a+k), _mm256_loadu_ps(b+k), s8);
    #else
    s8 = _mm256_add_ps(
           _mm256_mul_ps(_mm256_loadu_ps(a+k), _mm256_loadu_ps(b+k)), s8);
    #endif

  // compute horizontal sum
  #if 1
  __m128 sh = _mm_add_ps(_mm256_castps256_ps128(s8),
                         _mm256_extractf128_ps(s8, 1));
  sh = _mm_add_ps(sh, _mm_movehl_ps(sh, sh));
  sh = _mm_add_ss(sh, _mm_shuffle_ps(sh, sh, 1));
  #else
  s8 = _mm256_hadd_ps(s8, s8);  // comp horizontal sums in upper
  s8 = _mm256_hadd_ps(s8, s8);  // and lower half of the register
  __m128 sh = _mm_add_ss(_mm256_castps256_ps128(s8),
                         _mm256_extractf128_ps(s8, 1));
  #endif
  s += _mm_cvtss_f32(sh);  // extract horizontal sum from 1st elem.

  // add the remaining products
  for (int k = 8*(n/8); k < n; k++)
    s += a[k] * b[k];

  return s;
}  // sdot_avx()

/*--------------------------------------------------------------------------*/

// --- dot product (double precision)
#ifdef __FMA__
inline double ddot_avxfma (const double *a, const double *b, int n)
#else
inline double ddot_avx    (const double *a, const double *b, int n)
#endif
{
  // initialize total sum
  double s = 0.0;

  // compute and add up to 3 products without SIMD to achieve alignment
  if (align_rem(a,32) == align_rem(b,32)) {
    int aligned = is_aligned(a, 32) && is_aligned(b, 32);
    if (!aligned) {
      int k = 0;
      while (!aligned) {
        s += (*a) * (*b);
        n--; a++; b++;
        aligned = is_aligned(a, 32) && is_aligned(b, 32);
        if (aligned || (++k > 2) || (n == 0))
          break;
      }
    }
  }

  // initialize 4 sums
  __m256d s4 = _mm256_setzero_pd();

  // in each iteration, add 1 product to each of the 4 sums in parallel
  for (int k = 0, nq = 4*(n/4); k < nq; k += 4)
    #ifdef __FMA__
    s4 = _mm256_fmadd_pd(_mm256_loadu_pd(a+k), _mm256_loadu_pd(b+k), s4);
    #else
    s4 = _mm256_add_pd(
           _mm256_mul_pd(_mm256_loadu_pd(a+k), _mm256_loadu_pd(b+k)), s4);
    #endif

  // compute horizontal sum
  __m128d sh = _mm_add_pd(_mm256_extractf128_pd(s4, 0),
                          _mm256_extractf128_pd(s4, 1));
  sh = _mm_add_pd(sh, _mm_shuffle_pd(sh, sh, 1));
  s += _mm_cvtsd_f64(sh);  // extract horizontal sum from 1st elem.

  // add the remaining products
  for (int k = 4*(n/4); k < n; k++)
    s += a[k] * b[k];

  return s;
}  // ddot_avx()

/*--------------------------------------------------------------------------*/

// --- dot product (input: single; intermediate and output: double)
#ifdef __FMA__
inline double dsdot_avxfma (const float *a, const float *b, int n)
#else
inline double dsdot_avx    (const float *a, const float *b, int n)
#endif
{
  // initialize total sum
  double s = 0.0;

  // compute and add up to 7 products without SIMD to achieve alignment
  if (align_rem(a,32) == align_rem(b,32)) {
    int aligned = is_aligned(a, 32) && is_aligned(b, 32);
    if (!aligned) {
      int k = 0;
      while (!aligned) {
        s += (*a) * (*b);
        n--; a++; b++;
        aligned = is_aligned(a, 32) && is_aligned(b, 32);
        if (aligned || (++k > 6) || (n == 0))
          break;
      }
    }
  }

  // initialize 4 sums
  __m256d s4 = _mm256_setzero_pd();

  // in each iteration, add 1 product to each of the 4 sums in parallel
  for (int k = 0, nq = 4*(n/4); k < nq; k += 4)
    #ifdef __FMA__
    s4 = _mm256_fmadd_pd(_mm256_cvtps_pd(_mm_loadu_ps(a+k)),
                         _mm256_cvtps_pd(_mm_loadu_ps(b+k)), s4);
    #else
    s4 = _mm256_add_pd(
      _mm256_cvtps_pd(_mm_mul_ps(_mm_loadu_ps(a+k), _mm_loadu_ps(b+k))), s4);
    #endif

  // compute horizontal sum
  __m128d sh = _mm_add_pd(_mm256_extractf128_pd(s4, 0),
                          _mm256_extractf128_pd(s4, 1));
  sh = _mm_add_pd(sh, _mm_shuffle_pd(sh, sh, 1));
  s += _mm_cvtsd_f64(sh); // extract horizontal sum from 1st elem.

  // add the remaining products
  for (int k = 4*(n/4); k < n; k++)
    s += a[k] * b[k];

  return s;
}  // dsdot_avx()

#endif // DOT_AVX_H
