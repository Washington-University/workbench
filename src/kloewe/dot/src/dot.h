/*----------------------------------------------------------------------------
  File    : dot.h
  Contents: dot product (cpu dispatcher)
  Author  : Kristian Loewe
----------------------------------------------------------------------------*/
#ifndef DOT_H
#define DOT_H

#ifdef __cplusplus
extern "C"
{
#endif

/*----------------------------------------------------------------------------
  Enum to encode the sets of dot implementations
----------------------------------------------------------------------------*/
enum flags {
    DOT_NAIVE  = 1,   // plain C
    DOT_SSE2   = 2,   // SSE2
    DOT_AVX    = 3,   // AVX
    DOT_AVXFMA = 4,   // AVX+FMA3
    DOT_AUTO   = 100  // automatic choice
};
// Using dot_set_impl(), these values are used to specify the set of
// implementations to be used. The values/sets are ordered chronologically wrt
// the advent of the prerequisite instruction set extensions, with DOT_NAIVE
// representing the plain C fallback implementations, and DOT_AUTO indicating
// that the best set of implementations should be chosen automatically.

/*----------------------------------------------------------------------------
  Type Definitions
----------------------------------------------------------------------------*/
typedef float  (sdot_func)  (const float  *a, const float  *b, int n);
typedef double (ddot_func)  (const double *a, const double *b, int n);
typedef double (sddot_func) (const float  *a, const float  *b, int n);

/*----------------------------------------------------------------------------
  Global Variables
----------------------------------------------------------------------------*/
extern sdot_func  *sdot_ptr;
extern ddot_func  *ddot_ptr;
extern sddot_func *sddot_ptr;

/*----------------------------------------------------------------------------
  Function Prototypes
----------------------------------------------------------------------------*/
inline float  sdot         (const float  *a, const float  *b, int n);
inline double ddot         (const double *a, const double *b, int n);
inline double sddot        (const float  *a, const float  *b, int n);

/* dot_set_impl
 * ------------
 * specify the set of implementations that is used
 *
 * If the requested set of implementations is not supported, the function
 * will select the next best set of implementations that is supported.
 *
 * parameters
 * impl  indicates which set of implementations should be used
 *       DOT_NAIVE  -> plain C implementations
 *       DOT_SSE2   -> SSE2 implementations
 *       DOT_AVX    -> AVX implementations
 *       DOT_AVXFMA -> AVX+FMA3 implementations
 *       DOT_AUTO   -> automatically choose the best available set
 *       (see also the above enum)
 *
 * returns
 * the enum value corresponding to the selected set of implementations
 */
extern int    dot_set_impl (int   impl);


extern float  sdot_select  (const float  *a, const float  *b, int n);
extern double ddot_select  (const double *a, const double *b, int n);
extern double sddot_select (const float  *a, const float  *b, int n);

#ifndef DOT_NOFMA
extern float  sdot_avxfma  (const float  *a, const float  *b, int n);
extern double ddot_avxfma  (const double *a, const double *b, int n);
extern double sddot_avxfma (const float  *a, const float  *b, int n);
#endif

extern float  sdot_avx     (const float  *a, const float  *b, int n);
extern double ddot_avx     (const double *a, const double *b, int n);
extern double sddot_avx    (const float  *a, const float  *b, int n);

extern float  sdot_sse2    (const float  *a, const float  *b, int n);
extern double ddot_sse2    (const double *a, const double *b, int n);
extern double sddot_sse2   (const float  *a, const float  *b, int n);

extern float  sdot_naive   (const float  *a, const float  *b, int n);
extern double ddot_naive   (const double *a, const double *b, int n);
extern double sddot_naive  (const float  *a, const float  *b, int n);

/*----------------------------------------------------------------------------
  Inline Functions
----------------------------------------------------------------------------*/

inline float sdot (const float *a, const float *b, int n) {
  return (*sdot_ptr)(a,b,n);
}

inline double ddot (const double *a, const double *b, int n) {
  return (*ddot_ptr)(a,b,n);
}

inline double sddot (const float *a, const float *b, int n) {
  return (*sddot_ptr)(a,b,n);
}

#ifdef __cplusplus
}
#endif

#endif  // #ifndef DOT_H
