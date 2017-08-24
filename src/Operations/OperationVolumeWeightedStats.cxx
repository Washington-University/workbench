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

#include "OperationVolumeWeightedStats.h"
#include "OperationException.h"

#include "CaretHeap.h"
#include "VolumeFile.h"

#include <cmath>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <vector>

using namespace caret;
using namespace std;

AString OperationVolumeWeightedStats::getCommandSwitch()
{
    return "-volume-weighted-stats";
}

AString OperationVolumeWeightedStats::getShortDescription()
{
    return "WEIGHTED SPATIAL STATISTICS ON A VOLUME FILE";
}

OperationParameters* OperationVolumeWeightedStats::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    
    ret->addVolumeParameter(1, "volume-in", "the input volume");
    
    OptionalParameter* weightVolumeOpt = ret->createOptionalParameter(2, "-weight-volume", "use weights from a volume file");
    weightVolumeOpt->addVolumeParameter(1, "weight-volume", "volume file containing the weights");
    weightVolumeOpt->createOptionalParameter(2, "-match-maps", "each subvolume of input uses the corresponding subvolume from the weights file");
    
    OptionalParameter* subvolOpt = ret->createOptionalParameter(3, "-subvolume", "only display output for one subvolume");
    subvolOpt->addStringParameter(1, "subvolume", "the subvolume number or name");
    
    OptionalParameter* roiOpt = ret->createOptionalParameter(4, "-roi", "only consider data inside an roi");
    roiOpt->addVolumeParameter(1, "roi-volume", "the roi, as a volume file");
    roiOpt->createOptionalParameter(2, "-match-maps", "each subvolume of input uses the corresponding subvolume from the roi file");
    
    ret->createOptionalParameter(5, "-mean", "compute weighted mean");
    
    OptionalParameter* stdevOpt = ret->createOptionalParameter(6, "-stdev", "compute weighted standard deviation");
    stdevOpt->createOptionalParameter(1, "-sample", "estimate population stdev from the sample");
    
    OptionalParameter* percentileOpt = ret->createOptionalParameter(7, "-percentile", "compute weighted percentile");
    percentileOpt->addDoubleParameter(1, "percent", "the percentile to find");
    
    ret->createOptionalParameter(8, "-sum", "compute weighted sum");
    
    ret->createOptionalParameter(9, "-show-map-name", "print map index and name before each output");
    
    ret->setHelpText(
        AString("For each subvolume of the input, a single number is printed, resulting from the specified operation.  ") +
        "If -weight-volume is not specified, each voxel's volume is used.  " +
        "Use -subvolume to only give output for a single subvolume.  " +
        "Use -roi to consider only the data within a region.  " +
        "Exactly one of -mean, -stdev, -percentile or -sum must be specified.\n\n" +
        "Using -sum without -weight-volume is equivalent to integrating with respect to volume."
    );
    return ret;
}

namespace
{
    enum OperationType
    {
        MEAN,
        STDEV,
        SAMPSTDEV,
        PERCENTILE,
        SUM
    };
    
    float doOperation(const float* data, const float* weights, const int64_t& numElements, const OperationType& myop, const float* roiData, const float& argument)
    {//argument is only used for percentile currently
        if (roiData != NULL)
        {
            bool haveData = false;
            for (int64_t i = 0; i < numElements; ++i)
            {
                if (weights[i] > 0.0f)
                {
                    haveData = true;
                    break;
                }
            }
            if (!haveData) throw OperationException("roi contains no voxels");
        }
        switch(myop)
        {
            case SUM:
            case MEAN:
            case STDEV:
            case SAMPSTDEV://these all start the same way
            {
                double accum = 0.0, weightsum = 0.0;
                for (int64_t i = 0; i < numElements; ++i)
                {
                    if (roiData == NULL || roiData[i] > 0.0f)
                    {
                        accum += data[i] * weights[i];
                        weightsum += weights[i];
                    }
                }
                if (myop == SUM) return accum;
                const float mean = accum / weightsum;
                if (myop == MEAN) return mean;
                accum = 0.0;
                double weightsum2 = 0.0;//for weighted sample stdev
                for (int64_t i = 0; i < numElements; ++i)
                {
                    if (roiData == NULL || roiData[i] > 0.0f)
                    {
                        float tempf = data[i] - mean;
                        accum += weights[i] * tempf * tempf;
                        weightsum2 += weights[i] * weights[i];
                    }
                }
                if (myop == STDEV) return sqrt(accum / weightsum);
                CaretAssert(myop == SAMPSTDEV);
                return sqrt(accum / (weightsum - weightsum2 / weightsum));//http://en.wikipedia.org/wiki/Weighted_arithmetic_mean#Weighted_sample_variance
            }
            case PERCENTILE:
            {
                CaretAssert(argument >= 0.0f && argument <= 100.0f);
                CaretSimpleMinHeap<float, float> sorter;
                double weightaccum = 0.0;//double will usually prevent adding weights in a different order from getting a different answer
                for (int64_t i = 0; i < numElements; ++i)
                {
                    if (roiData == NULL || roiData[i] > 0.0f)
                    {
                        if (weights[i] < 0.0f) throw OperationException("negative weights not allowed in weighted percentile");
                        weightaccum += weights[i];
                        sorter.push(weights[i], data[i]);//sort by value, so the key is the data
                    }
                }
                int64_t numUse = sorter.size();
                if (numUse == 1)//would need special handling anyway, so get it early
                {
                    float ret;
                    sorter.top(&ret);
                    return ret;
                }
                float targetWeight = argument / 100.0f * weightaccum;
                float lastData, nextData;
                float lastWeight = sorter.pop(&lastData);
                weightaccum = lastWeight;
                float nextWeight = sorter.top(&nextData);
                int64_t position = 1;//because the first and last sections get special treatment to not have flat ends on the function
                while (weightaccum + nextWeight * 0.5f < targetWeight && sorter.size() > 1)
                {
                    ++position;
                    sorter.pop();
                    weightaccum += nextWeight;
                    lastWeight = nextWeight;
                    lastData = nextData;
                    nextWeight = sorter.top(&nextData);
                }
                if (targetWeight < weightaccum)
                {
                    if (position == 1)
                    {//stretch interpolation at first position to the edge
                        return lastData + (nextData - lastData) * 0.5f * ((targetWeight - weightaccum) / lastWeight + 1.0f);
                    } else {
                        return lastData + (nextData - lastData) * 0.5f * ((targetWeight - weightaccum) / (lastWeight * 0.5f) + 1.0f);
                    }
                } else {
                    if (position == numUse - 1)
                    {//ditto
                        return (lastData + nextData) * 0.5f + (nextData - lastData) * 0.5f * (targetWeight - weightaccum) / nextWeight;
                    } else {
                        return (lastData + nextData) * 0.5f + (nextData - lastData) * 0.5f * (targetWeight - weightaccum) / (nextWeight * 0.5f);
                    }
                }
            }
        }
        CaretAssert(false);//make sure execution never actually reaches end of function
        throw OperationException("internal error in weighted stats");
    }

    float doOperationSingleWeight(const float* data, const float& weight, const int64_t& numElements, const OperationType& myop, const float* roiData, const float& argument)
    {//argument is only used for percentile currently
        const float* useData = data;
        int64_t numUse = numElements;
        vector<float> dataScratch;//for when we have an ROI
        if (roiData != NULL)
        {
            dataScratch.reserve(numElements);
            for (int64_t i = 0; i < numElements; ++i)
            {
                if (roiData[i] > 0.0f)
                {
                    dataScratch.push_back(data[i]);
                }
            }
            if (dataScratch.size() < 1) throw OperationException("roi contains no voxels");
            useData = dataScratch.data();
            numUse = (int64_t)dataScratch.size();
        }
        switch(myop)
        {
            case SUM:
            case MEAN:
            case STDEV:
            case SAMPSTDEV://these all start the same way
            {
                double accum = 0.0;
                for (int64_t i = 0; i < numUse; ++i)
                {
                    accum += useData[i];
                }
                if (myop == SUM) return accum * weight;//this is the only operation that needs the weight when it is the same at every location
                const float mean = accum / numUse;
                if (myop == MEAN) return mean;
                accum = 0.0;
                for (int64_t i = 0; i < numUse; ++i)
                {
                    float tempf = useData[i] - mean;
                    accum += tempf * tempf;
                }
                if (myop == STDEV) return sqrt(accum / numUse);
                CaretAssert(myop == SAMPSTDEV);
                if (numUse < 2) throw OperationException("sample standard deviation requires at least 2 elements in the roi");
                return sqrt(accum / (numUse - 1));
            }
            case PERCENTILE:
            {
                CaretAssert(argument >= 0.0f && argument <= 100.0f);//same as unweighted
                vector<float> sortCopy(useData, useData + numUse);
                sort(sortCopy.begin(), sortCopy.end());
                const double index = argument / 100.0f * (sortCopy.size() - 1);
                if (index <= 0) return sortCopy[0];
                if (index >= sortCopy.size() - 1) return sortCopy.back();
                double ipart, fpart;
                fpart = modf(index, &ipart);
                return (1.0f - fpart) * sortCopy[(int64_t)ipart] + fpart * sortCopy[((int64_t)ipart) + 1];
            }
        }
        CaretAssert(false);//make sure execution never actually reaches end of function
        throw OperationException("internal error in weighted stats");
    }

}

void OperationVolumeWeightedStats::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    LevelProgress myProgress(myProgObj);
    VolumeFile* input = myParams->getVolume(1);
    const float constWeight = input->getVolumeSpace().getVoxelVolume();//even if we don't need it
    vector<int64_t> dims = input->getDimensions();
    const int64_t frameSize = dims[0] * dims[1] * dims[2];
    if (input->getNumberOfComponents() != 1) throw OperationException("multi-component volumes are not supported in -volume-weighted-stats");
    OptionalParameter* weightVolumeOpt = myParams->getOptionalParameter(2);
    VolumeFile* myWeights = NULL;
    const float* weightData = NULL;
    bool matchSubvolWeights = false;
    if (weightVolumeOpt->m_present)
    {
        myWeights = weightVolumeOpt->getVolume(1);
        if (!myWeights->matchesVolumeSpace(input)) throw OperationException("weight volume doesn't match volume space of input");
        if (weightVolumeOpt->getOptionalParameter(2)->m_present)
        {
            if (myWeights->getDimensions()[3] != dims[3])
            {
                throw OperationException("-match-maps specified, but weights file has different number of subvolumes than input");
            }
            matchSubvolWeights = true;
        } else {
            weightData = myWeights->getFrame();
        }
    }
    int subvol = -1;
    OptionalParameter* subvolOpt = myParams->getOptionalParameter(3);
    if (subvolOpt->m_present)
    {
        subvol = input->getMapIndexFromNameOrNumber(subvolOpt->getString(1));
        if (subvol < 0) throw OperationException("invalid column specified");
    }
    bool matchSubvolMode = false;
    VolumeFile* myRoi = NULL;
    const float* roiData = NULL;
    OptionalParameter* roiOpt = myParams->getOptionalParameter(4);
    if (roiOpt->m_present)
    {
        myRoi = roiOpt->getVolume(1);
        if (!input->matchesVolumeSpace(myRoi)) throw OperationException("roi doesn't match volume space of input");
        if (roiOpt->getOptionalParameter(2)->m_present)
        {
            if (myRoi->getDimensions()[3] != dims[3])
            {
                throw OperationException("-match-maps specified, but roi file has different number of subvolumes than input");
            }
            matchSubvolMode = true;
        } else {
            roiData = myRoi->getFrame();
        }
    }
    bool haveOp = false;
    OperationType myop;
    if (myParams->getOptionalParameter(5)->m_present)
    {
        haveOp = true;
        myop = MEAN;
    }
    OptionalParameter* stdevOpt = myParams->getOptionalParameter(6);
    if (stdevOpt->m_present)
    {
        if (haveOp) throw OperationException("you may only specify one operation");
        haveOp = true;
        if (stdevOpt->getOptionalParameter(1)->m_present)
        {
            myop = SAMPSTDEV;
        } else {
            myop = STDEV;
        }
    }
    float argument = -1.0f;
    OptionalParameter* percentileOpt = myParams->getOptionalParameter(7);
    if (percentileOpt->m_present)
    {
        if (haveOp) throw OperationException("you may only specify one operation");
        haveOp = true;
        myop = PERCENTILE;
        argument = percentileOpt->getDouble(1);
        if (!(argument >= 0.0f && argument <= 100.0f)) throw OperationException("percentile must be between 0 and 100");
    }
    if (myParams->getOptionalParameter(8)->m_present)
    {
        if (haveOp) throw OperationException("you may only specify one operation");
        haveOp = true;
        myop = SUM;
    }
    if (!haveOp) throw OperationException("you must specify an operation");
    bool showMapName = myParams->getOptionalParameter(9)->m_present;
    int numMaps = input->getNumberOfMaps();
    if (subvol == -1)
    {
        for (int i = 0; i < numMaps; ++i)
        {//store result before printing anything, in case it throws while computing
            if (matchSubvolMode)
            {
                roiData = myRoi->getFrame(i);
            }
            if (matchSubvolWeights)
            {
                weightData = myWeights->getFrame(i);
            }
            float result;
            if (weightData != NULL)
            {
                result = doOperation(input->getFrame(i), weightData, frameSize, myop, roiData, argument);
            } else {
                result = doOperationSingleWeight(input->getFrame(i), constWeight, frameSize, myop, roiData, argument);
            }
            if (showMapName) cout << AString::number(i + 1) << ": " << input->getMapName(i) << ": ";
            stringstream resultsstr;
            resultsstr << setprecision(7) << result;
            cout << resultsstr.str() << endl;
        }
    } else {
        if (matchSubvolMode)
        {
            roiData = myRoi->getFrame(subvol);
        }
        if (matchSubvolWeights)
        {
            weightData = myWeights->getFrame(subvol);
        }
        float result;
        if (weightData != NULL)
        {
            result = doOperation(input->getFrame(subvol), weightData, frameSize, myop, roiData, argument);
        } else {
            result = doOperationSingleWeight(input->getFrame(subvol), constWeight, frameSize, myop, roiData, argument);
        }
        if (showMapName) cout << AString::number(subvol + 1) << ": " << input->getMapName(subvol) << ": ";
        stringstream resultsstr;
        resultsstr << setprecision(7) << result;
        cout << resultsstr.str() << endl;
    }
}
