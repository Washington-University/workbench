/*LICENSE_START*/
/*
 *  Copyright 1995-2002 Washington University School of Medicine
 *
 *  http://brainmap.wustl.edu
 *
 *  This file is part of CARET.
 *
 *  CARET is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  CARET is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with CARET; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include "ReductionOperation.h"
#include "CaretAssert.h"
#include "CaretException.h"

#include <cmath>

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
    }
    return 0.0f;
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
