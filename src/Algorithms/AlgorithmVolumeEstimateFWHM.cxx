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

#include "AlgorithmVolumeEstimateFWHM.h"
#include "AlgorithmException.h"

#include <cmath>
#include <iostream>

using namespace caret;
using namespace std;

AString AlgorithmVolumeEstimateFWHM::getCommandSwitch()
{
    return "-volume-estimate-fwhm";
}

AString AlgorithmVolumeEstimateFWHM::getShortDescription()
{
    return "ESTIMATE FWHM SMOOTHNESS OF A VOLUME";
}

OperationParameters* AlgorithmVolumeEstimateFWHM::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    ret->addVolumeParameter(1, "volume", "the input volume");
    
    OptionalParameter* roiVolOpt = ret->createOptionalParameter(2, "-roi", "use only data within an ROI");
    roiVolOpt->addVolumeParameter(1, "roivol", "the volume to use as an ROI");
    
    OptionalParameter* subvolSelect = ret->createOptionalParameter(3, "-subvolume", "select a single subvolume to estimate smoothness of");
    subvolSelect->addStringParameter(1, "subvol", "the subvolume number or name");
    
    OptionalParameter* allFramesOpt = ret->createOptionalParameter(4, "-whole-file", "estimate for the whole file at once, not each subvolume separately");
    allFramesOpt->createOptionalParameter(1, "-demean", "subtract the mean image before estimating smoothness");
    
    ret->setHelpText(
        AString("Estimates the smoothness of the input volume in X, Y, and Z directions separately, printing the estimates to standard output, in mm as FWHM.  ") +
        "If -subvolume or -whole-file are not specified, each subvolume is estimated and displayed separately."
    );
    return ret;
}

void AlgorithmVolumeEstimateFWHM::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    LevelProgress myProgress(myProgObj);
    VolumeFile* myVol = myParams->getVolume(1);
    OptionalParameter* roiVolOpt = myParams->getOptionalParameter(2);
    VolumeFile* roiVol = NULL;
    if (roiVolOpt->m_present)
    {
        roiVol = roiVolOpt->getVolume(1);
        if (!roiVol->matchesVolumeSpace(myVol))
        {
            throw AlgorithmException("roi volume does not match the space of the input volume");
        }
    }
    OptionalParameter* subvolSelect = myParams->getOptionalParameter(3);
    int subvolNum = -1;
    if (subvolSelect->m_present)
    {
        subvolNum = (int)myVol->getMapIndexFromNameOrNumber(subvolSelect->getString(1));
        if (subvolNum < 0)
        {
            throw AlgorithmException("invalid subvolume specified");
        }
    }
    bool allFrames = false, demean = false;
    OptionalParameter* allFramesOpt = myParams->getOptionalParameter(4);
    if (allFramesOpt->m_present)
    {
        if (subvolSelect->m_present) throw AlgorithmException("specifying both -subvolume and -whole-file is not allowed");
        allFrames = true;
        demean = allFramesOpt->getOptionalParameter(1)->m_present;
    }
    vector<int64_t> dims;
    myVol->getDimensions(dims);
    if (allFrames)
    {
        Vector3D result = estimateFWHMAllFrames(myVol, roiVol, demean);
        cout << "FWHM: " << result[0] << ", " << result[1] << ", " << result[2] << endl;
    } else {
        if (subvolNum == -1)
        {
            for (int64_t s = 0; s < dims[3]; ++s)
            {
                for (int64_t c = 0; c < dims[4]; ++c)
                {
                    Vector3D result = estimateFWHM(myVol, roiVol, s, c);
                    if (dims[3] != 1) cout << "subvol " << s + 1 << " ";
                    if (dims[4] != 1) cout << "component " << c + 1 << " ";
                    cout << "FWHM: " << result[0] << ", " << result[1] << ", " << result[2] << endl;
                }
            }
        } else {
            for (int64_t c = 0; c < dims[4]; ++c)
            {
                Vector3D result = estimateFWHM(myVol, roiVol, subvolNum, c);
                if (dims[3] != 1) cout << "subvol " << subvolNum + 1 << " ";
                if (dims[4] != 1) cout << "component " << c + 1 << " ";
                cout << "FWHM: " << result[0] << ", " << result[1] << ", " << result[2] << endl;
            }
        }
    }
}

Vector3D AlgorithmVolumeEstimateFWHM::estimateFWHM(const VolumeFile* input, const VolumeFile* roi, const int64_t& brickIndex, const int64_t& component)
{
    if (roi != NULL && !roi->matchesVolumeSpace(input))
    {
        throw AlgorithmException("roi volume does not match the space of the input volume");
    }
    vector<int64_t> dims;
    input->getDimensions(dims);
    double globalaccum = 0.0;
    int64_t globalCount = 0;
    double diraccum[3] = {0.0, 0.0, 0.0};
    int64_t dirCount[3] = {0, 0, 0};
    for (int64_t k = 0; k < dims[2]; ++k)//2 pass method, first find means of values and of FORWARD differences only - this removes global gradient effects
    {//for derivation, see Forman, S.D., Cohen, J.D., Fitzgerald, M., Eddy, W.F., Mintun, M.A., Noll, D.C., 1995.
        for (int64_t j = 0; j < dims[1]; ++j)//Improved assessment of significant activation in functional magnetic resonance imaging (fMRI): use of a cluster-size threshold. 
        {//Magn. Reson. Med. 33, 636–647.
            for (int64_t i = 0; i < dims[0]; ++i)
            {
                if (roi == NULL || roi->getValue(i, j, k) > 0.0f)
                {
                    float center = input->getValue(i, j, k, brickIndex, component);
                    globalaccum += center;
                    ++globalCount;
                    if (i + 1 < dims[0] && (roi == NULL || roi->getValue(i + 1, j, k) > 0.0f))//use ONLY forward differences, to avoid double counting
                    {
                        float diff = center - input->getValue(i + 1, j, k, brickIndex, component);
                        diraccum[0] += diff;
                        ++dirCount[0];
                    }
                    if (j + 1 < dims[1] && (roi == NULL || roi->getValue(i, j + 1, k) > 0.0f))//use ONLY forward differences, to avoid double counting
                    {
                        float diff = center - input->getValue(i, j + 1, k, brickIndex, component);
                        diraccum[1] += diff;
                        ++dirCount[1];
                    }
                    if (k + 1 < dims[2] && (roi == NULL || roi->getValue(i, j, k + 1) > 0.0f))//use ONLY forward differences, to avoid double counting
                    {
                        float diff = center - input->getValue(i, j, k + 1, brickIndex, component);
                        diraccum[2] += diff;
                        ++dirCount[2];
                    }
                }
            }
        }
    }
    if (globalCount == 0) throw AlgorithmException("ROI is empty or volume file has no voxels");
    float dirmean[3];
    for (int i = 0; i < 3; ++i)
    {
        dirmean[i] = diraccum[i] / dirCount[i];//we will fix NaNs later
        diraccum[i] = 0.0;
    }
    float globalmean = globalaccum / globalCount;
    globalaccum = 0;
    for (int64_t k = 0; k < dims[2]; ++k)//now, find the variances
    {
        for (int64_t j = 0; j < dims[1]; ++j)
        {
            for (int64_t i = 0; i < dims[0]; ++i)
            {
                if (roi == NULL || roi->getValue(i, j, k) > 0.0f)
                {
                    float center = input->getValue(i, j, k, brickIndex, component);
                    float tempf = center - globalmean;
                    globalaccum += tempf * tempf;
                    if (i + 1 < dims[0] && (roi == NULL || roi->getValue(i + 1, j, k) > 0.0f))//use ONLY forward differences, to avoid double counting
                    {
                        float diff = center - input->getValue(i + 1, j, k, brickIndex, component);
                        tempf = diff - dirmean[0];
                        diraccum[0] += tempf * tempf;
                    }
                    if (j + 1 < dims[1] && (roi == NULL || roi->getValue(i, j + 1, k) > 0.0f))//use ONLY forward differences, to avoid double counting
                    {
                        float diff = center - input->getValue(i, j + 1, k, brickIndex, component);
                        tempf = diff - dirmean[1];
                        diraccum[1] += tempf * tempf;
                    }
                    if (k + 1 < dims[2] && (roi == NULL || roi->getValue(i, j, k + 1) > 0.0f))//use ONLY forward differences, to avoid double counting
                    {
                        float diff = center - input->getValue(i, j, k + 1, brickIndex, component);
                        tempf = diff - dirmean[2];
                        diraccum[2] += tempf * tempf;
                    }
                }
            }
        }
    }
    float dirvariance[3];
    float fwhms[3];
    float globalvariance = globalaccum / globalCount;
    const VolumeSpace& volSpace = input->getVolumeSpace();
    Vector3D spacingVecs[4];
    volSpace.getSpacingVectors(spacingVecs[0], spacingVecs[1], spacingVecs[2], spacingVecs[3]);
    for (int i = 0; i < 3; ++i)
    {
        if (dirCount[i] > 0)
        {
            dirvariance[i] = diraccum[i] / dirCount[i];
        } else {
            dirvariance[i] = 0.0;//avoid NaN for variance...however, 0 directional variance means the formula will become NaN...
        }
        fwhms[i] = spacingVecs[i].length() * sqrt(-2.0f * log(2.0f) / log(1.0f - dirvariance[i] / (2.0f * globalvariance)));
    }
    Vector3D ret;
    VolumeSpace::OrientTypes myorient[3];
    volSpace.getOrientation(myorient);
    for (int i = 0; i < 3; ++i)
    {
        switch (myorient[i])
        {
            case VolumeSpace::LEFT_TO_RIGHT:
            case VolumeSpace::RIGHT_TO_LEFT:
                ret[0] = fwhms[i];
                break;
            case VolumeSpace::POSTERIOR_TO_ANTERIOR:
            case VolumeSpace::ANTERIOR_TO_POSTERIOR:
                ret[1] = fwhms[i];
                break;
            case VolumeSpace::INFERIOR_TO_SUPERIOR:
            case VolumeSpace::SUPERIOR_TO_INFERIOR:
                ret[2] = fwhms[i];
                break;
        }
    }
    return ret;
}

Vector3D AlgorithmVolumeEstimateFWHM::estimateFWHMAllFrames(const VolumeFile* input, const VolumeFile* roi, bool demean)
{
    if (roi != NULL && !roi->matchesVolumeSpace(input))
    {
        throw AlgorithmException("roi volume does not match the space of the input volume");
    }
    const float* roiFrame = NULL;
    if (roi != NULL) roiFrame = roi->getFrame();
    vector<int64_t> dims;
    input->getDimensions(dims);
    int64_t frameSize = dims[0] * dims[1] * dims[2];
    vector<vector<double> > meanimage;
    if (demean)
    {
        meanimage.resize(dims[4], vector<double>(frameSize, 0.0));//keep components separate, I guess
        for (int64_t component = 0; component < dims[4]; ++component)
        {
            for (int64_t brickIndex = 0; brickIndex < dims[3]; ++brickIndex)
            {
                const float* frame = input->getFrame(brickIndex, component);
                for (int64_t i = 0; i < frameSize; ++i)
                {
                    if (roiFrame == NULL || roiFrame[i] > 0.0f)
                    {//only computing the mean image inside the ROI reduces the working set
                        meanimage[component][i] += frame[i];
                    }
                }
            }
            for (int64_t i = 0; i < frameSize; ++i)
            {
                if (roiFrame == NULL || roiFrame[i] > 0.0f)
                {
                    meanimage[component][i] /= dims[3];
                }
            }
        }
    }
    double globalaccum = 0.0;
    int64_t globalCount = 0;
    double diraccum[3] = {0.0, 0.0, 0.0};
    int64_t dirCount[3] = {0, 0, 0};
    vector<float> demeaned;//less reallocation
    if (demean) demeaned.resize(frameSize);
    for (int64_t component = 0; component < dims[4]; ++component)
    {
        for (int64_t brickIndex = 0; brickIndex < dims[3]; ++brickIndex)
        {
            const float* frame = input->getFrame(brickIndex, component);
            if (demean)
            {
                for (int64_t i = 0; i < frameSize; ++i)
                {
                    if (roiFrame == NULL || roiFrame[i] > 0.0f)
                    {//again, to keep working set down
                        demeaned[i] = frame[i] - meanimage[component][i];
                    }
                }
                frame = demeaned.data();
            }
            for (int64_t k = 0; k < dims[2]; ++k)
            {
                for (int64_t j = 0; j < dims[1]; ++j)
                {
                    for (int64_t i = 0; i < dims[0]; ++i)
                    {
                        if (roi == NULL || roi->getValue(i, j, k) > 0.0f)
                        {
                            float center = frame[input->getIndex(i, j, k)];
                            globalaccum += center;
                            ++globalCount;
                            if (i + 1 < dims[0] && (roi == NULL || roi->getValue(i + 1, j, k) > 0.0f))//use ONLY forward differences, to avoid double counting
                            {
                                float diff = center - frame[input->getIndex(i + 1, j, k)];
                                diraccum[0] += diff;
                                ++dirCount[0];
                            }
                            if (j + 1 < dims[1] && (roi == NULL || roi->getValue(i, j + 1, k) > 0.0f))//use ONLY forward differences, to avoid double counting
                            {
                                float diff = center - frame[input->getIndex(i, j + 1, k)];
                                diraccum[1] += diff;
                                ++dirCount[1];
                            }
                            if (k + 1 < dims[2] && (roi == NULL || roi->getValue(i, j, k + 1) > 0.0f))//use ONLY forward differences, to avoid double counting
                            {
                                float diff = center - frame[input->getIndex(i, j, k + 1)];
                                diraccum[2] += diff;
                                ++dirCount[2];
                            }
                        }
                    }
                }
            }
        }
    }
    if (globalCount == 0) throw AlgorithmException("ROI is empty or volume file has no voxels");
    float dirmean[3];
    for (int i = 0; i < 3; ++i)
    {
        dirmean[i] = diraccum[i] / dirCount[i];//we will fix NaNs later
        diraccum[i] = 0.0;
    }
    float globalmean = globalaccum / globalCount;
    globalaccum = 0.0;
    for (int64_t component = 0; component < dims[4]; ++component)
    {
        for (int64_t brickIndex = 0; brickIndex < dims[3]; ++brickIndex)
        {
            const float* frame = input->getFrame(brickIndex, component);
            if (demean)
            {
                for (int64_t i = 0; i < frameSize; ++i)
                {
                    if (roiFrame == NULL || roiFrame[i] > 0.0f)
                    {
                        demeaned[i] = frame[i] - meanimage[component][i];
                    }
                }
                frame = demeaned.data();
            }
            for (int64_t k = 0; k < dims[2]; ++k)//now, find the variances
            {
                for (int64_t j = 0; j < dims[1]; ++j)
                {
                    for (int64_t i = 0; i < dims[0]; ++i)
                    {
                        if (roi == NULL || roi->getValue(i, j, k) > 0.0f)
                        {
                            float center = frame[input->getIndex(i, j, k)];
                            float tempf = center - globalmean;
                            globalaccum += tempf * tempf;
                            if (i + 1 < dims[0] && (roi == NULL || roi->getValue(i + 1, j, k) > 0.0f))//use ONLY forward differences, to avoid double counting
                            {
                                float diff = center - frame[input->getIndex(i + 1, j, k)];
                                tempf = diff - dirmean[0];
                                diraccum[0] += tempf * tempf;
                            }
                            if (j + 1 < dims[1] && (roi == NULL || roi->getValue(i, j + 1, k) > 0.0f))//use ONLY forward differences, to avoid double counting
                            {
                                float diff = center - frame[input->getIndex(i, j + 1, k)];
                                tempf = diff - dirmean[1];
                                diraccum[1] += tempf * tempf;
                            }
                            if (k + 1 < dims[2] && (roi == NULL || roi->getValue(i, j, k + 1) > 0.0f))//use ONLY forward differences, to avoid double counting
                            {
                                float diff = center - frame[input->getIndex(i, j, k + 1)];
                                tempf = diff - dirmean[2];
                                diraccum[2] += tempf * tempf;
                            }
                        }
                    }
                }
            }
        }
    }
    float dirvariance[3];
    float fwhms[3];
    float globalvariance = globalaccum / globalCount;
    const VolumeSpace& volSpace = input->getVolumeSpace();
    Vector3D spacingVecs[4];
    volSpace.getSpacingVectors(spacingVecs[0], spacingVecs[1], spacingVecs[2], spacingVecs[3]);
    for (int i = 0; i < 3; ++i)
    {
        if (dirCount[i] > 0)
        {
            dirvariance[i] = diraccum[i] / dirCount[i];
        } else {
            dirvariance[i] = 0.0;
        }
        fwhms[i] = spacingVecs[i].length() * sqrt(-2.0f * log(2.0f) / log(1.0f - dirvariance[i] / (2.0f * globalvariance)));
    }
    Vector3D ret;
    VolumeSpace::OrientTypes myorient[3];
    volSpace.getOrientation(myorient);
    for (int i = 0; i < 3; ++i)
    {
        switch (myorient[i])
        {
            case VolumeSpace::LEFT_TO_RIGHT:
            case VolumeSpace::RIGHT_TO_LEFT:
                ret[0] = fwhms[i];
                break;
            case VolumeSpace::POSTERIOR_TO_ANTERIOR:
            case VolumeSpace::ANTERIOR_TO_POSTERIOR:
                ret[1] = fwhms[i];
                break;
            case VolumeSpace::INFERIOR_TO_SUPERIOR:
            case VolumeSpace::SUPERIOR_TO_INFERIOR:
                ret[2] = fwhms[i];
                break;
        }
    }
    return ret;
}
