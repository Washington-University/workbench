/*LICENSE_START*/
/*
 *  Copyright (C) 2014  Washington University School of Medicine
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */
/*LICENSE_END*/
#include "DotTest.h"

#include "CaretAssert.h"
#include "dot_wrapper.h"

#include <cmath>
#include <cstdlib>
#include <vector>

using namespace caret;
using namespace std;

DotTest::DotTest(const AString& identifier) : TestInterface(identifier)
{
}

namespace
{
    vector<float> randVector01(const int& size)
    {
        vector<float> ret(size);
        for (int i = 0; i < size; ++i)
        {
            ret[i] = ((float)rand()) / RAND_MAX;
        }
        return ret;
    }
    
    vector<float> vectorMult(const vector<float>& vec, const float& scalar)
    {
        vector<float> ret(vec.size());
        for (int i = 0; i < (int)ret.size(); ++i)
        {
            ret[i] = vec[i] * scalar;
        }
        return ret;
    }
    
    vector<float> vectorAdd(const vector<float>& vec, const float& scalar)
    {
        vector<float> ret(vec.size());
        for (int i = 0; i < (int)ret.size(); ++i)
        {
            ret[i] = vec[i] + scalar;
        }
        return ret;
    }
    
    vector<float> vectorAdd(const vector<float>& vec1, const vector<float>& vec2)
    {
        CaretAssert(vec1.size() == vec2.size());
        vector<float> ret(vec1.size());
        for (int i = 0; i < (int)ret.size(); ++i)
        {
            ret[i] = vec1[i] + vec2[i];
        }
        return ret;
    }
    
    //we need to mean-center the inputs, so copy them on call
    float correlate(vector<float> vec1, vector<float> vec2)
    {//three-pass - mean, stdev, then dot function (under test)
        CaretAssert(vec1.size() == vec2.size());
        const int length = (int)vec1.size();
        double accum1 = 0.0, accum2 = 0.0;
        for (int i = 0; i < length; ++i)
        {
            accum1 += vec1[i];
            accum2 += vec2[i];
        }
        float mean1 = accum1 / length;
        float mean2 = accum2 / length;
        accum1 = 0.0;
        accum2 = 0.0;
        for (int i = 0; i < length; ++i)
        {
            vec1[i] -= mean1;
            vec2[i] -= mean2;
            accum1 += vec1[i] * vec1[i];//we don't do this with dot on other implementations currently, so do it naively here too
            accum2 += vec2[i] * vec2[i];
        }
        float stdev1 = sqrt(accum1);
        float stdev2 = sqrt(accum2);
        double dotval = sddot(vec1.data(), vec2.data(), length);//function under test
        return dotval / (stdev1 * stdev2);
    }
    
}

void DotTest::checkVal(const float& correct, const float& test, const AString& descrip)
{
    const float TOLER_RATIO = 0.00001f;//6 digits, seems to work, probably due to accumulation in double before cast to float
    const float TOLER_ABS = 0.0000001f;//for near-zero results, need some wiggle room, which a ratio won't work for
    if (abs(test - correct) > TOLER_ABS + TOLER_RATIO * abs(correct)) setFailed(descrip + " got " + AString::number(test) + ", expected " + AString::number(correct));
}

void DotTest::execute()
{
    dot_flags impl_in_use = dot_set_impl(DOT_NAIVE);
    if (impl_in_use != DOT_NAIVE) setFailed("failed to set implementation to NAIVE");
    const int ROWSIZE = 100000;//400KB per vector shouldn't be too bad
    vector<float> rand1 = randVector01(ROWSIZE), rand2 = randVector01(ROWSIZE), rand3 = randVector01(ROWSIZE);
    vector<float> lowsnrA = vectorAdd(rand1, vectorMult(rand2, 20.0f)), lowsnrB = vectorAdd(rand1, vectorMult(rand3, 20.0f));
    vector<float> midsnrA = vectorAdd(rand1, vectorMult(rand2, 2.0f)), midsnrB = vectorAdd(rand1, vectorMult(rand3, 2.0f));
    vector<float> highsnrA = vectorAdd(rand1, vectorMult(rand2, 0.2f)), highsnrB = vectorAdd(rand1, vectorMult(rand3, 0.2f));
    //compute all by naive first
    const float self_naive = correlate(rand1, rand1);
    checkVal(1.0f, self_naive, "naive self-correlation");//sanity check
    const float unrelated_naive = correlate(rand1, rand2);
    const float lowsnr_naive = correlate(lowsnrA, lowsnrB);
    const float midsnr_naive = correlate(midsnrA, midsnrB);
    const float highsnr_naive = correlate(highsnrA, highsnrB);
    const float cross_snr_naive = correlate(lowsnrA, highsnrB);
    //sse2
    impl_in_use = dot_set_impl(DOT_SSE2);
    if (impl_in_use == DOT_SSE2)
    {
        checkVal(self_naive, correlate(rand1, rand1), "sse2 self-correlation");
        checkVal(unrelated_naive, correlate(rand1, rand2), "sse2 unrelated correlation");
        checkVal(lowsnr_naive, correlate(lowsnrA, lowsnrB), "sse2 low snr correlation");
        checkVal(midsnr_naive, correlate(midsnrA, midsnrB), "sse2 mid snr correlation");
        checkVal(highsnr_naive, correlate(highsnrA, highsnrB), "sse2 high snr correlation");
        checkVal(cross_snr_naive, correlate(lowsnrA, highsnrB), "sse2 cross snr correlation");
    } else {
        cout << "skipping SSE2, not supported" << endl;
    }
    //avx
    impl_in_use = dot_set_impl(DOT_AVX);
    if (impl_in_use == DOT_AVX)
    {
        checkVal(self_naive, correlate(rand1, rand1), "avx self-correlation");
        checkVal(unrelated_naive, correlate(rand1, rand2), "avx unrelated correlation");
        checkVal(lowsnr_naive, correlate(lowsnrA, lowsnrB), "avx low snr correlation");
        checkVal(midsnr_naive, correlate(midsnrA, midsnrB), "avx mid snr correlation");
        checkVal(highsnr_naive, correlate(highsnrA, highsnrB), "avx high snr correlation");
        checkVal(cross_snr_naive, correlate(lowsnrA, highsnrB), "avx cross snr correlation");
    } else {
        cout << "skipping AVX, not supported" << endl;
    }
    //avx
    impl_in_use = dot_set_impl(DOT_AVXFMA);
    if (impl_in_use == DOT_AVXFMA)
    {
        checkVal(self_naive, correlate(rand1, rand1), "avxfma self-correlation");
        checkVal(unrelated_naive, correlate(rand1, rand2), "avxfma unrelated correlation");
        checkVal(lowsnr_naive, correlate(lowsnrA, lowsnrB), "avxfma low snr correlation");
        checkVal(midsnr_naive, correlate(midsnrA, midsnrB), "avxfma mid snr correlation");
        checkVal(highsnr_naive, correlate(highsnrA, highsnrB), "avxfma high snr correlation");
        checkVal(cross_snr_naive, correlate(lowsnrA, highsnrB), "avxfma cross snr correlation");
    } else {
        cout << "skipping AVXFMA, not supported" << endl;
    }
}
