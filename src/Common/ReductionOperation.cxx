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

#include "ReductionOperation.h"
#include "CaretAssert.h"
#include "CaretException.h"
#include "MathFunctions.h"

#include <algorithm>
#include <cmath>
#include <limits>
#include <vector>

using namespace caret;
using namespace std;

float ReductionOperation::reduce(const float* data, const int64_t& numElems, const ReductionEnum::Enum& type)
{
    CaretAssert(numElems > 0);
    switch (type)
    {
        case ReductionEnum::INVALID:
            throw CaretException("reduction requested with 'INVALID' method");
        case ReductionEnum::SAMPSTDEV://all of these start by taking the average, for stability
            if (numElems < 2) throw CaretException("'SAMPSTDEV' reduction on 1 element would require dividing by zero");
        case ReductionEnum::MEAN:
        case ReductionEnum::STDEV:
        case ReductionEnum::VARIANCE:
        case ReductionEnum::SUM:
        {
            double sum = 0.0;
            for (int64_t i = 0; i < numElems; ++i) sum += data[i];
            switch (type)
            {
                case ReductionEnum::SUM:
                    return sum;
                case ReductionEnum::MEAN:
                    return sum / numElems;
                default:
                {
                    float mean = sum / numElems;
                    double residsqr = 0.0;
                    for (int64_t i = 0; i < numElems; ++i)
                    {
                        float tempf = data[i] - mean;
                        residsqr += tempf * tempf;
                    }
                    switch(type)
                    {
                        case ReductionEnum::STDEV:
                            return sqrt(residsqr / numElems);
                        case ReductionEnum::SAMPSTDEV:
                            return sqrt(residsqr / (numElems - 1));
                        case ReductionEnum::VARIANCE:
                            return residsqr / numElems;
                        default:
                            CaretAssertMessage(0, "unhandled type in sum-based reduction");
                            return 0.0f;
                    }
                }
            }
        }
        case ReductionEnum::PRODUCT:
        {
            double prod = 1.0;
            for (int64_t i = 0; i < numElems; ++i) prod *= data[i];
            return prod;
        }
        case ReductionEnum::MAX:
        {
            float max = data[0];
            for (int64_t i = 1; i < numElems; ++i) if (data[i] > max) max = data[i];
            return max;
        }
        case ReductionEnum::MIN:
        {
            float min = data[0];
            for (int64_t i = 1; i < numElems; ++i) if (data[i] < min) min = data[i];
            return min;
        }
        case ReductionEnum::INDEXMAX:
        {
            float max = data[0];
            int64_t index = 0;
            for (int64_t i = 1; i < numElems; ++i)
            {
                if (data[i] > max)
                {
                    max = data[i];
                    index = i;
                }
            }
            return index + 1;//1-based, to match gui and column arguments
        }
        case ReductionEnum::INDEXMIN:
        {
            float min = data[0];
            int64_t index = 0;
            for (int64_t i = 1; i < numElems; ++i)
            {
                if (data[i] < min)
                {
                    min = data[i];
                    index = i;
                }
            }
            return index + 1;
        }
        case ReductionEnum::MEDIAN:
        {
            vector<float> dataCopy(numElems);
            for (int64_t i = 0; i < numElems; ++i) dataCopy[i] = data[i];
            sort(dataCopy.begin(), dataCopy.end());
            if ((numElems & 1) == 0)//if even, average middle two
            {
                return (dataCopy[numElems / 2 - 1] + dataCopy[numElems / 2]) / 2.0f;
            } else {
                return dataCopy[numElems / 2];//otherwise, take the center
            }
        }
        case ReductionEnum::MODE:
        {
            vector<float> dataCopy(numElems);
            for (int64_t i = 0; i < numElems; ++i) dataCopy[i] = data[i];
            sort(dataCopy.begin(), dataCopy.end());//sort to put same-value next to each other, a hash based map could be faster for large arrays, but oh well
            int bestCount = 0, curCount = 1;
            float bestval = -1.0f, curval = dataCopy[0];
            for (int64_t i = 1; i < numElems; ++i)//search for largest contiguous region
            {
                if (dataCopy[i] == curval)
                {
                    ++curCount;
                } else {
                    if (curCount > bestCount)
                    {
                        bestval = curval;
                        bestCount = curCount;
                    }
                    curval = dataCopy[i];
                    curCount = 1;
                }
            }
            if (curCount > bestCount)
            {
                bestval = curval;
                bestCount = curCount;
            }
            return bestval;
        }
        case ReductionEnum::COUNT_NONZERO:
        {
            int64_t count = 0;
            for (int64_t i = 0; i < numElems; ++i)
            {
                if (data[i] != 0.0f)
                {
                    ++count;
                }
            }
            return count;
        }
    }
    return 0.0f;
}

float ReductionOperation::reduceExcludeDev(const float* data, const int64_t& numElems, const ReductionEnum::Enum& type, const float& numDevBelow, const float& numDevAbove)
{
    CaretAssert(numElems > 0);
    double sum = 0.0;
    int64_t validNum = 0;
    for (int64_t i = 0; i < numElems; ++i)
    {
        if (MathFunctions::isNumeric(data[i]))
        {
            ++validNum;
            sum += data[i];
        }
    }
    if (validNum == 0) throw CaretException("all input values to reduceExcludeDev were non-numeric");
    float mean = sum / validNum;
    double residsqr = 0.0;
    for (int64_t i = 0; i < numElems; ++i)
    {
        if (MathFunctions::isNumeric(data[i]))
        {
            float tempf = data[i] - mean;
            residsqr += tempf * tempf;
        }
    }
    float stdev = sqrt(residsqr / validNum);
    float low = mean - numDevBelow * stdev, high = mean + numDevAbove * stdev;
    switch (type)//special case things that use indices
    {
        case ReductionEnum::INDEXMAX:
        {
            float max = 0.0f;
            int64_t index = -1;
            bool first = true;
            for (int64_t i = 0; i < numElems; ++i)
            {
                if (MathFunctions::isNumeric(data[i]) && data[i] >= low && data[i] <= high && (first || data[i] > max))
                {
                    first = false;
                    max = data[i];
                    index = i;
                }
            }
            return index + 1;//1-based, to match gui and column arguments
        }
        case ReductionEnum::INDEXMIN:
        {
            float min = 0.0f;
            int64_t index = -1;
            bool first = true;
            for (int64_t i = 0; i < numElems; ++i)
            {
                if (MathFunctions::isNumeric(data[i]) && data[i] >= low && data[i] <= high && (first || data[i] < min))
                {
                    first = false;
                    min = data[i];
                    index = i;
                }
            }
            return index + 1;//1-based, to match gui and column arguments
        }
        default:
            break;
    }
    vector<float> excluded;
    excluded.reserve(validNum);
    for (int64_t i = 0; i < numElems; ++i)
    {
        if (MathFunctions::isNumeric(data[i]) && data[i] >= low && data[i] <= high) excluded.push_back(data[i]);
    }
    if (excluded.size() == 0) throw CaretException("exclusion parameters to reduceExcludeDev resulted in no usable data");
    if (type == ReductionEnum::SAMPSTDEV && excluded.size() < 2) throw CaretException("SAMPSTDEV requested in reduceExcludeDev when only 1 element passed the exclusion parameters");
    return reduce(excluded.data(), excluded.size(), type);
}

float ReductionOperation::reduceOnlyNumeric(const float* data, const int64_t& numElems, const ReductionEnum::Enum& type)
{
    CaretAssert(numElems > 0);
    switch (type)//special case things that use indices
    {
        case ReductionEnum::INDEXMAX:
        {
            float max = 0.0f;
            int64_t index = -1;
            bool first = true;
            for (int64_t i = 0; i < numElems; ++i)
            {
                if (MathFunctions::isNumeric(data[i]) && (first || data[i] > max))
                {
                    first = false;
                    max = data[i];
                    index = i;
                }
            }
            if (first) throw CaretException("all input values to reduceOnlyNumeric were non-numeric");
            return index + 1;//1-based, to match gui and column arguments
        }
        case ReductionEnum::INDEXMIN:
        {
            float min = 0.0f;
            int64_t index = -1;
            bool first = true;
            for (int64_t i = 0; i < numElems; ++i)
            {
                if (MathFunctions::isNumeric(data[i]) && (first || data[i] < min))
                {
                    first = false;
                    min = data[i];
                    index = i;
                }
            }
            if (first) throw CaretException("all input values to reduceOnlyNumeric were non-numeric");
            return index + 1;//1-based, to match gui and column arguments
        }
        default:
            break;
    }
    vector<float> excluded;
    excluded.reserve(numElems);
    for (int64_t i = 0; i < numElems; ++i)
    {
        if (MathFunctions::isNumeric(data[i])) excluded.push_back(data[i]);
    }
    if (excluded.size() < 1) throw CaretException("all input values to reduceOnlyNumeric were non-numeric");
    if (type == ReductionEnum::SAMPSTDEV && excluded.size() < 2) throw CaretException("SAMPSTDEV requested in reduceOnlyNumeric when only 1 element is numeric");
    return reduce(excluded.data(), excluded.size(), type);
}

namespace
{
    struct ValWeight
    {//for sorting based only on value, but keeping weight associated
        float value, weight;
        ValWeight(float v, float w)
        {
            value = v;
            weight = w;
        }
        inline bool operator<(const ValWeight& rhs) const
        {
            return value < rhs.value;
        }
    };
}

float ReductionOperation::reduceWeighted(const float* data, const float* weights, const int64_t& numElems, const ReductionEnum::Enum& type)
{
    CaretAssert(numElems > 0);
    switch (type)
    {
        case ReductionEnum::INVALID:
            throw CaretException("weighted reduction requested with 'INVALID' method");
        case ReductionEnum::INDEXMAX:
        case ReductionEnum::INDEXMIN:
        case ReductionEnum::MIN:
        case ReductionEnum::MAX:
        case ReductionEnum::PRODUCT:
        case ReductionEnum::COUNT_NONZERO:
            throw CaretException("weighted reduction not supported for '" + ReductionEnum::toName(type) + "' method");
        case ReductionEnum::SAMPSTDEV://all of these start by taking the average, for stability
            if (numElems < 2) throw CaretException("'SAMPSTDEV' weighted reduction on 1 element would require dividing by zero");
        case ReductionEnum::MEAN:
        case ReductionEnum::STDEV:
        case ReductionEnum::VARIANCE:
        case ReductionEnum::SUM:
        {
            double accum = 0.0, weightsum = 0.0f;
            for (int i = 0; i < numElems; ++i)
            {
                accum += data[i] * weights[i];
                weightsum += weights[i];
            }
            if (type == ReductionEnum::SUM) return accum;
            const float mean = accum / weightsum;
            if (type == ReductionEnum::MEAN) return mean;
            accum = 0.0;
            double weightsum2 = 0.0;//for weighted sample stdev
            for (int i = 0; i < numElems; ++i)
            {
                float tempf = data[i] - mean;
                accum += weights[i] * tempf * tempf;
                weightsum2 += weights[i] * weights[i];
            }
            if (type == ReductionEnum::STDEV) return sqrt(accum / weightsum);
            if (type == ReductionEnum::VARIANCE) return accum / weightsum;
            CaretAssert(type == ReductionEnum::SAMPSTDEV);
            return sqrt(accum / (weightsum - weightsum2 / weightsum));//http://en.wikipedia.org/wiki/Weighted_arithmetic_mean#Weighted_sample_variance
        }
        case ReductionEnum::MEDIAN:
        {
            vector<ValWeight> toSort;
            toSort.reserve(numElems);
            for (int i = 0; i < numElems; ++i)
            {
                toSort.push_back(ValWeight(data[i], weights[i]));
            }
            stable_sort(toSort.begin(), toSort.end());
            vector<double> weightaccum(numElems);
            weightaccum[0] = toSort[0].weight;
            for (int i = 1; i < numElems; ++i)
            {
                weightaccum[i] = weightaccum[i - 1] + toSort[i].weight;
            }
            double target = weightaccum.back() / 2;
            int64_t index = (int64_t)(lower_bound(weightaccum.begin(), weightaccum.end(), target) - weightaccum.begin());
            if (index == numElems) --index;//deal with edge cases from things like negative weights
            if (numElems > 1 && index < (numElems - 1) && weightaccum[index] == target)//only average on exact equals, according to https://en.wikipedia.org/wiki/Weighted_median
            {//could instead always interpolate
                return (toSort[index].value + toSort[index + 1].value) / 2;
            } else {
                return toSort[index].value;
            }
        }
        case ReductionEnum::MODE:
        {
            vector<ValWeight> toSort;
            toSort.reserve(numElems);
            for (int i = 0; i < numElems; ++i)
            {
                toSort.push_back(ValWeight(data[i], weights[i]));
            }
            stable_sort(toSort.begin(), toSort.end());
            float bestweight = -numeric_limits<float>::infinity(), curweight = toSort[0].weight;
            float bestval = toSort[0].value, curval = toSort[0].value;
            for (int i = 1; i < numElems; ++i)
            {
                if (toSort[i].value == curval)
                {
                    curweight += toSort[i].weight;
                } else {
                    if (curweight > bestweight)
                    {
                        bestval = curval;
                        bestweight = curweight;
                    }
                    curval = toSort[i].value;
                    curweight = toSort[i].weight;
                }
            }
            if (curweight > bestweight)
            {
                bestval = curval;
                bestweight = curweight;
            }
            return bestval;
        }
    }
    return 0.0f;
}

float ReductionOperation::reduceWeightedOnlyNumeric(const float* data, const float* weights, const int64_t& numElems, const ReductionEnum::Enum& type)
{
    CaretAssert(numElems > 0);
    switch (type)
    {
        case ReductionEnum::INVALID:
            throw CaretException("weighted reduction requested with 'INVALID' method");
        case ReductionEnum::INDEXMAX:
        case ReductionEnum::INDEXMIN:
        case ReductionEnum::MIN:
        case ReductionEnum::MAX:
        case ReductionEnum::PRODUCT:
        case ReductionEnum::COUNT_NONZERO:
            throw CaretException("weighted reduction not supported for '" + ReductionEnum::toName(type) + "' method");
        default:
            break;
    }
    vector<float> excluded, exweights;
    excluded.reserve(numElems);
    exweights.reserve(numElems);
    for (int64_t i = 0; i < numElems; ++i)
    {
        if (MathFunctions::isNumeric(data[i]))
        {
            excluded.push_back(data[i]);
            exweights.push_back(weights[i]);
        }
    }
    if (excluded.size() < 1) throw CaretException("all input values to reduceWeightedOnlyNumeric were non-numeric");
    if (type == ReductionEnum::SAMPSTDEV && excluded.size() < 2) throw CaretException("SAMPSTDEV requested in reduceWeightedOnlyNumeric when only 1 element is numeric");
    return reduceWeighted(excluded.data(), exweights.data(), excluded.size(), type);
}

float ReductionOperation::reduceWeightedExcludeDev(const float* data, const float* weights, const int64_t& numElems, const ReductionEnum::Enum& type, const float& numDevBelow, const float& numDevAbove)
{
    CaretAssert(numElems > 0);
    switch (type)
    {
        case ReductionEnum::INVALID:
            throw CaretException("weighted reduction requested with 'INVALID' method");
        case ReductionEnum::INDEXMAX:
        case ReductionEnum::INDEXMIN:
        case ReductionEnum::MIN:
        case ReductionEnum::MAX:
        case ReductionEnum::PRODUCT:
        case ReductionEnum::COUNT_NONZERO:
            throw CaretException("weighted reduction not supported for '" + ReductionEnum::toName(type) + "' method");
        default:
            break;
    }
    double accum = 0.0, weightsum = 0.0f;//compute weighted stdev
    int64_t numValid = 0;
    for (int i = 0; i < numElems; ++i)
    {
        if (MathFunctions::isNumeric(data[i]))
        {
            accum += data[i] * weights[i];
            weightsum += weights[i];
            ++numValid;
        }
    }
    const float mean = accum / weightsum;
    accum = 0.0;
    for (int i = 0; i < numElems; ++i)
    {
        if (MathFunctions::isNumeric(data[i]))
        {
            float tempf = data[i] - mean;
            accum += weights[i] * tempf * tempf;
        }
    }
    float stdev = sqrt(accum / weightsum);
    float low = mean - stdev * numDevBelow, high = mean + stdev * numDevAbove;
    vector<float> excluded, exweights;
    excluded.reserve(numValid);
    exweights.reserve(numValid);
    for (int64_t i = 0; i < numElems; ++i)
    {
        if (MathFunctions::isNumeric(data[i]) && data[i] >= low && data[i] <= high)
        {
            excluded.push_back(data[i]);
            exweights.push_back(weights[i]);
        }
    }
    if (excluded.size() < 1) throw CaretException("all input values to reduceWeightedExcludeDev were non-numeric");
    if (type == ReductionEnum::SAMPSTDEV && excluded.size() < 2) throw CaretException("SAMPSTDEV requested in reduceWeightedExcludeDev when only 1 element is numeric");
    return reduceWeighted(excluded.data(), exweights.data(), excluded.size(), type);
}

AString ReductionOperation::getHelpInfo()
{
    AString ret;
    vector<ReductionEnum::Enum> myEnums;
    ReductionEnum::getAllEnums(myEnums);
    int numEnums = (int)myEnums.size();
    for (int i = 0; i < numEnums; ++i)
    {
        ret += ReductionEnum::toName(myEnums[i]) + ": " + ReductionEnum::toExplanation(myEnums[i]) + "\n";
    }
    return ret;
}
