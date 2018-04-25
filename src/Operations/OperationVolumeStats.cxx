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

#include "OperationVolumeStats.h"
#include "OperationException.h"

#include "ReductionOperation.h"
#include "VolumeFile.h"

#include <algorithm>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <vector>

using namespace caret;
using namespace std;

AString OperationVolumeStats::getCommandSwitch()
{
    return "-volume-stats";
}

AString OperationVolumeStats::getShortDescription()
{
    return "SPATIAL STATISTICS ON A VOLUME FILE";
}

OperationParameters* OperationVolumeStats::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    
    ret->addVolumeParameter(1, "volume-in", "the input volume");
    
    OptionalParameter* reduceOpt = ret->createOptionalParameter(2, "-reduce", "use a reduction operation");
    reduceOpt->addStringParameter(1, "operation", "the reduction operation");
    
    OptionalParameter* percentileOpt = ret->createOptionalParameter(3, "-percentile", "give the value at a percentile");
    percentileOpt->addDoubleParameter(1, "percent", "the percentile to find");
    
    OptionalParameter* subvolOpt = ret->createOptionalParameter(4, "-subvolume", "only display output for one subvolume");
    subvolOpt->addStringParameter(1, "subvolume", "the subvolume number or name");
    
    OptionalParameter* roiOpt = ret->createOptionalParameter(5, "-roi", "only consider data inside an roi");
    roiOpt->addVolumeParameter(1, "roi-volume", "the roi, as a volume file");
    roiOpt->createOptionalParameter(2, "-match-maps", "each subvolume of input uses the corresponding subvolume from the roi file");
    
    ret->createOptionalParameter(6, "-show-map-name", "print map index and name before each output");
    
    ret->setHelpText(
        AString("For each subvolume of the input, a single number is printed, resulting from the specified reduction or percentile operation.  ") +
        "Use -subvolume to only give output for a single subvolume.  " +
        "Use -roi to consider only the data within a region.  " +
        "Exactly one of -reduce or -percentile must be specified.\n\n" +
        "The argument to the -reduce option must be one of the following:\n\n" +
        ReductionOperation::getHelpInfo());
    return ret;
}

namespace
{
    float reduce(const float* data, const int64_t& numElements, const ReductionEnum::Enum& myop, const float* roiData)
    {
        if (roiData == NULL)
        {
            return ReductionOperation::reduce(data, numElements, myop);
        } else {
            vector<float> toUse;
            toUse.reserve(numElements);
            for (int64_t i = 0; i < numElements; ++i)
            {
                if (roiData[i] > 0.0f)
                {
                    toUse.push_back(data[i]);
                }
            }
            if (toUse.empty()) throw OperationException("roi contains no voxels");
            return ReductionOperation::reduce(toUse.data(), toUse.size(), myop);
        }
    }
    
    float percentile(const float* data, const int64_t& numElements, const float& percent, const float* roiData)
    {
        CaretAssert(percent >= 0.0f && percent <= 100.0f);
        vector<float> toUse;
        if (roiData == NULL)
        {
            toUse = vector<float>(data, data + numElements);
        } else {
            toUse.reserve(numElements);
            for (int64_t i = 0; i < numElements; ++i)
            {
                if (roiData[i] > 0.0f)
                {
                    toUse.push_back(data[i]);
                }
            }
        }
        if (toUse.empty()) throw OperationException("roi contains no voxels");
        sort(toUse.begin(), toUse.end());
        const double index = percent / 100.0f * (toUse.size() - 1);
        if (index <= 0) return toUse[0];
        if (index >= toUse.size() - 1) return toUse.back();
        double ipart, fpart;
        fpart = modf(index, &ipart);
        return (1.0f - fpart) * toUse[(int64_t)ipart] + fpart * toUse[((int64_t)ipart) + 1];
    }
}

void OperationVolumeStats::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    LevelProgress myProgress(myProgObj);
    VolumeFile* input = myParams->getVolume(1);
    vector<int64_t> dims = input->getDimensions();
    const int64_t frameSize = dims[0] * dims[1] * dims[2];
    if (input->getNumberOfComponents() != 1) throw OperationException("multi-component volumes are not supported in -volume-stats");
    OptionalParameter* reduceOpt = myParams->getOptionalParameter(2);
    OptionalParameter* percentileOpt = myParams->getOptionalParameter(3);
    if (reduceOpt->m_present == percentileOpt->m_present)//use == as logical xnor
    {
        throw OperationException("you must use exactly one of -reduce or -percentile");
    }
    ReductionEnum::Enum myop = ReductionEnum::INVALID;
    if (reduceOpt->m_present)
    {
        bool ok = false;
        myop = ReductionEnum::fromName(reduceOpt->getString(1), &ok);
        if (!ok) throw OperationException("unrecognized reduction operation: " + reduceOpt->getString(1));
    }
    float percent = 0.0f;
    if (percentileOpt->m_present)
    {
        percent = (float)percentileOpt->getDouble(1);//use not within range to trap NaNs, just in case
        if (!(percent >= 0.0f && percent <= 100.0f)) throw OperationException("percentile must be between 0 and 100");
    }
    int subvol = -1;
    OptionalParameter* subvolOpt = myParams->getOptionalParameter(4);
    if (subvolOpt->m_present)
    {
        subvol = input->getMapIndexFromNameOrNumber(subvolOpt->getString(1));
        if (subvol < 0) throw OperationException("invalid column specified");
    }
    bool matchSubvolMode = false;
    VolumeFile* myRoi = NULL;
    int numRoiMaps = 1;//trick: pretend there is one ROI map with no ROI file
    OptionalParameter* roiOpt = myParams->getOptionalParameter(5);
    if (roiOpt->m_present)
    {
        myRoi = roiOpt->getVolume(1);
        numRoiMaps = myRoi->getNumberOfMaps();
        if (!input->matchesVolumeSpace(myRoi)) throw OperationException("roi doesn't match volume space of input");
        if (roiOpt->getOptionalParameter(2)->m_present)
        {
            if (myRoi->getDimensions()[3] != dims[3])
            {
                throw OperationException("-match-maps specified, but roi has different number of subvolumes than input");
            }
            matchSubvolMode = true;
        }
    }
    bool showMapName = myParams->getOptionalParameter(6)->m_present;
    int numMaps = input->getNumberOfMaps();
    int startSubvol, endSubvol;
    if (subvol == -1)
    {
        startSubvol = 0;
        endSubvol = numMaps;
    } else {
        startSubvol = subvol;
        endSubvol = subvol + 1;
    }
    for (int i = startSubvol; i < endSubvol; ++i)
    {
        if (showMapName) cout << AString::number(i + 1) << ":\t" << input->getMapName(i) << ":\t";
        if (matchSubvolMode)
        {//trick: matchSubvolMode is only true when we have an roi
            const float* roiData = myRoi->getFrame(i);
            float result;
            if (reduceOpt->m_present)
            {
                result = reduce(input->getFrame(i), frameSize, myop, roiData);
            } else {
                CaretAssert(percentileOpt->m_present);
                result = percentile(input->getFrame(i), frameSize, percent, roiData);
            }
            stringstream resultsstr;
            resultsstr << setprecision(7) << result;
            cout << resultsstr.str();
        } else {
            const float* roiData = NULL;
            for (int j = 0; j < numRoiMaps; ++j)
            {
                if (myRoi != NULL) roiData = myRoi->getFrame(j);
                float result;
                if (reduceOpt->m_present)
                {
                    result = reduce(input->getFrame(i), frameSize, myop, roiData);
                } else {
                    CaretAssert(percentileOpt->m_present);
                    result = percentile(input->getFrame(i), frameSize, percent, roiData);
                }
                stringstream resultsstr;
                resultsstr << setprecision(7) << result;
                if (j != 0) cout << "\t";
                cout << resultsstr.str();
            }
        }
        cout << endl;
    }
}
