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
#include <vector>

using namespace caret;
using namespace std;

float ReductionOperation::reduce(const float* data, const int64_t& numElems, const ReductionEnum::Enum& type)
{
    CaretAssert(numElems > 0);
    switch (type)
    {
        case ReductionEnum::INVALID:
            throw CaretException("reduction requested with INVALID operator");
        case ReductionEnum::SAMPSTDEV://all of these start by taking the average, for stability
            if (numElems < 2) throw CaretException("SAMPSTDEV reduction would require dividing by zero");
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
    if (excluded.size() == 0) throw CaretException("all input values to reduceOnlyNumeric were non-numeric");
    return reduce(excluded.data(), excluded.size(), type);
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
