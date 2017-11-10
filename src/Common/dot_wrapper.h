#ifndef DOT_WRAPPER_H
#define DOT_WRAPPER_H

//workbench is strictly c++, so we don't actually need ifdef guards on this
//we also don't expose any libraries, so it doesn't really matter whether it switches to c++ name mangling when we disable SIMD
//but hey, whatever
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
inline double dsdot (const float *a, const float *b, int n)
{
  double sum = 0;
  for (int k = 0; k < n; k++)
    sum += a[k] * b[k];
  return sum;
}  // dsdot()
//copy enum from dot.h
//renamed to dot_flags in both files for less conflict chance
typedef enum {
    DOT_NAIVE     = 1,
    DOT_SSE2      = 2,
    DOT_AVX       = 3,
    DOT_AVXFMA    = 4,
    DOT_AVX512    = 5,
    DOT_AVX512FMA = 6,
    DOT_AUTO      = 100
} dot_flags;
//and dummy implementation of dot_set_impl
inline dot_flags dot_set_impl (dot_flags)
{
    return DOT_NAIVE;
}
#endif

#ifdef __cplusplus
}
#endif

//convenience helpers for the enum
#include "AString.h"
#include "CaretAssert.h"

#include <vector>

namespace caret
{
    class DotSIMDEnum
    {
    public:
        typedef dot_flags Enum;

        static inline std::vector<Enum> getAllEnums()
        {
            std::vector<Enum> ret;
            ret.push_back(DOT_NAIVE);
            ret.push_back(DOT_SSE2);
            ret.push_back(DOT_AVX);
            ret.push_back(DOT_AVXFMA);
            ret.push_back(DOT_AVX512);
            ret.push_back(DOT_AVX512FMA);
            ret.push_back(DOT_AUTO);
            return ret;
        }

        static inline Enum fromName(const AString& name, bool* isValidOut = NULL)
        {
            bool valid = false;
            Enum ret = DOT_NAIVE;
            if (name == "NAIVE")
            {
                ret = DOT_NAIVE;
                valid = true;
            } else if (name == "SSE2") {
                ret = DOT_SSE2;
                valid = true;
            } else if (name == "AVX") {
                ret = DOT_AVX;
                valid = true;
            } else if (name == "AVXFMA") {
                ret = DOT_AVXFMA;
                valid = true;
            } else if (name == "AVX512") {
                ret = DOT_AVX512;
                valid = true;
            } else if (name == "AVX512FMA") {
                ret = DOT_AVX512FMA;
                valid = true;
            } else if (name == "AUTO") {
                ret = DOT_AUTO;
                valid = true;
            }
            if (isValidOut == NULL)
            {
                CaretAssert(valid);
            } else {
                *isValidOut = valid;
            }
            return ret;
        }

        static inline AString toName(const Enum& value)
        {
            switch (value)
            {
                case DOT_NAIVE:
                    return "NAIVE";
                case DOT_SSE2:
                    return "SSE2";
                case DOT_AVX:
                    return "AVX";
                case DOT_AVXFMA:
                    return "AVXFMA";
                case DOT_AVX512:
                    return "AVX512";
                case DOT_AVX512FMA:
                    return "AVX512FMA";
                case DOT_AUTO:
                    return "AUTO";
                default:
                    CaretAssert(0);
            }
            return "";
        }
    };
}

#endif
