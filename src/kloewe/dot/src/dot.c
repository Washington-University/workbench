/*----------------------------------------------------------------------------
  File    : dot.c
  Contents: dot product (cpu dispatcher)
  Author  : Kristian Loewe
----------------------------------------------------------------------------*/
#include "cpuinfo.h"
#include "dot.h"

/*----------------------------------------------------------------------------
  Function Prototypes
----------------------------------------------------------------------------*/
extern float  sdot  (const float  *a, const float  *b, int n);
extern double ddot  (const double *a, const double *b, int n);
extern double sddot (const float  *a, const float  *b, int n);

/*----------------------------------------------------------------------------
  Global Variables
----------------------------------------------------------------------------*/
sdot_func  *sdot_ptr  = &sdot_select;
ddot_func  *ddot_ptr  = &ddot_select;
sddot_func *sddot_ptr = &sddot_select;

/*----------------------------------------------------------------------------
  Functions
----------------------------------------------------------------------------*/

float sdot_select (const float *a, const float *b, int n) {
  dot_set_impl(DOT_AUTO);
  return (*sdot_ptr)(a,b,n);
}

double ddot_select (const double *a, const double *b, int n) {
  dot_set_impl(DOT_AUTO);
  return (*ddot_ptr)(a,b,n);
}

double sddot_select (const float *a, const float *b, int n) {
  dot_set_impl(DOT_AUTO);
  return (*sddot_ptr)(a,b,n);
}

int    dot_set_impl (int impl) {
  #ifndef DOT_NOFMA
  // the AVX-FMA implementations are currently slower than the AVX
  // implementations and are thus only used if explicitly requested
  if      (hasFMA3() && hasAVX() && (impl == DOT_AVXFMA)) { // AVX-FMA
    sdot_ptr  = &sdot_avxfma;
    ddot_ptr  = &ddot_avxfma;
    sddot_ptr = &sddot_avxfma;
    return DOT_AVXFMA; }
  else if (hasAVX()              && (impl >= DOT_AVX)) {    // AVX
  #else
  if      (hasAVX()              && (impl >= DOT_AVX)) {    // AVX
  #endif
    sdot_ptr  = &sdot_avx;
    ddot_ptr  = &ddot_avx;
    sddot_ptr = &sddot_avx;
    return DOT_AVX; }
  else if (hasSSE2()             && (impl >= DOT_SSE2)) {   // SSE2
    sdot_ptr  = &sdot_sse2;
    ddot_ptr  = &ddot_sse2;
    sddot_ptr = &sddot_sse2;
    return DOT_SSE2; }
  else {                                                    // naive
    sdot_ptr  = &sdot_naive;
    ddot_ptr  = &ddot_naive;
    sddot_ptr = &sddot_naive;
    return DOT_NAIVE;
  }
}
