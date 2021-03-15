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

#include "AlgorithmException.h"
#include "AlgorithmVolumeGradient.h"
#include "AlgorithmVolumeSmoothing.h"
#include "CaretOMP.h"
#include "FloatMatrix.h"
#include "MathFunctions.h"
#include "Vector3D.h"
#include "VolumeFile.h"

#include <algorithm>
#include <cmath>

using namespace caret;
using namespace std;

AString AlgorithmVolumeGradient::getCommandSwitch()
{
    return "-volume-gradient";
}

AString AlgorithmVolumeGradient::getShortDescription()
{
    return "GRADIENT OF A VOLUME FILE";
}

OperationParameters* AlgorithmVolumeGradient::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    ret->addVolumeParameter(1, "volume-in", "the input volume");
    
    ret->addVolumeOutputParameter(2, "volume-out", "the output gradient magnitude volume");
    
    OptionalParameter* presmoothOpt = ret->createOptionalParameter(3, "-presmooth", "smooth the volume before computing the gradient");
    presmoothOpt->addDoubleParameter(1, "kernel", "the size of the gaussian smoothing kernel in mm, as sigma by default");
    presmoothOpt->createOptionalParameter(2, "-fwhm", "kernel size is FWHM, not sigma");
    
    OptionalParameter* roiOption = ret->createOptionalParameter(4, "-roi", "select a region of interest to take the gradient of");
    roiOption->addVolumeParameter(1, "roi-volume", "the region to take the gradient within");
    
    OptionalParameter* vecOption = ret->createOptionalParameter(5, "-vectors", "output vectors");
    vecOption->addVolumeOutputParameter(1, "vector-volume-out", "the vectors as a volume file");
    
    OptionalParameter* subvolSelect = ret->createOptionalParameter(6, "-subvolume", "select a single subvolume to take the gradient of");
    subvolSelect->addStringParameter(1, "subvol", "the subvolume number or name");
    
    ret->setHelpText(
        AString("Computes the gradient of the volume by doing linear regressions for each voxel, considering only its face neighbors unless too few face neighbors exist.  ") +
        "The gradient vector is constructed from the partial derivatives of the resulting linear function, and the magnitude of this vector is the output.  " +
        "If specified, the volume vector output is arranged with the x, y, and z components from a subvolume as consecutive subvolumes."
    );
    return ret;
}

void AlgorithmVolumeGradient::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    VolumeFile* volIn = myParams->getVolume(1);
    VolumeFile* volOut = myParams->getOutputVolume(2);
    float presmooth = -1.0f;
    OptionalParameter* presmoothOpt = myParams->getOptionalParameter(3);
    if (presmoothOpt->m_present)
    {
        presmooth = (float)presmoothOpt->getDouble(1);
        if (presmoothOpt->getOptionalParameter(2)->m_present)
        {
            presmooth = presmooth / (2.0f * sqrt(2.0f * log(2.0f)));
        }
    }
    VolumeFile* myRoi = NULL;
    OptionalParameter* roiOption = myParams->getOptionalParameter(4);
    if (roiOption->m_present)
    {
        myRoi = roiOption->getVolume(1);
    }
    VolumeFile* vectorsOut = NULL;
    OptionalParameter* vecOption = myParams->getOptionalParameter(5);
    if (vecOption->m_present)
    {
        vectorsOut = vecOption->getOutputVolume(1);
    }
    OptionalParameter* subvolSelect = myParams->getOptionalParameter(6);
    int subvolNum = -1;
    if (subvolSelect->m_present)
    {
        subvolNum = (int)volIn->getMapIndexFromNameOrNumber(subvolSelect->getString(1));
        if (subvolNum < 0)
        {
            throw AlgorithmException("invalid subvolume specified");
        }
    }
    AlgorithmVolumeGradient(myProgObj, volIn, volOut, presmooth, myRoi, vectorsOut, subvolNum);
}

AlgorithmVolumeGradient::AlgorithmVolumeGradient(ProgressObject* myProgObj, const VolumeFile* volIn, VolumeFile* volOut, const float& presmooth,
                                                       const VolumeFile* myRoi, VolumeFile* vectorsOut, const int& subvolNum) : AbstractAlgorithm(myProgObj)
{
    ProgressObject* smoothProgress = NULL;
    if (myProgObj != NULL && presmooth > 0.0f)
    {
        smoothProgress = myProgObj->addAlgorithm(AlgorithmVolumeSmoothing::getAlgorithmWeight());
    }
    LevelProgress myProgress(myProgObj);
    if (myRoi != NULL && !volIn->matchesVolumeSpace(myRoi))
    {
        throw AlgorithmException("roi volume space does not match input");
    }
    if (subvolNum < -1 || subvolNum > volIn->getNumberOfMaps())
    {
        throw AlgorithmException("invalid subvolume specified");
    }
    VolumeFile smoothVol;
    const VolumeFile* processVol = volIn;
    int useSubvol = subvolNum;
    if (presmooth > 0.0f)
    {
        AlgorithmVolumeSmoothing(smoothProgress, volIn, presmooth, &smoothVol, myRoi, false, subvolNum);
        processVol = &smoothVol;
        if (subvolNum != -1)
        {
            useSubvol = 0;
        }
    }
    vector<int64_t> origDims = volIn->getOriginalDimensions(), myDims;
    volIn->getDimensions(myDims);
    int stencil[] = { 0, 0, 1,
                    0, 0, -1,
                    0, 1, 0,
                    0, -1, 0,
                    1, 0, 0,
                    -1, 0, 0 };//WARNING: ordering of these neighbors is used in checking for singular regression conditions
    vector<vector<float> > volSpace = volIn->getSform();
    Vector3D ivec, jvec, kvec, origin;
    ivec[0] = volSpace[0][0]; jvec[0] = volSpace[0][1]; kvec[0] = volSpace[0][2]; origin[0] = volSpace[0][3];
    ivec[1] = volSpace[1][0]; jvec[1] = volSpace[1][1]; kvec[1] = volSpace[1][2]; origin[1] = volSpace[1][3];
    ivec[2] = volSpace[2][0]; jvec[2] = volSpace[2][1]; kvec[2] = volSpace[2][2]; origin[2] = volSpace[2][3];//TODO: special case orthogonal volumes (central difference)?
    const float* roiFrame = NULL;
    if (myRoi != NULL)
    {
        roiFrame = myRoi->getFrame();
    }
    if (subvolNum == -1)
    {
        volOut->reinitialize(origDims, volIn->getSform(), myDims[4], volIn->getType(), volIn->m_header);
        if (vectorsOut != NULL)
        {
            while (origDims.size() < 4)
            {
                origDims.push_back(1);
            }
            origDims[3] *= 3;
            vectorsOut->reinitialize(origDims, volIn->getSform(), myDims[4], volIn->getType());
        }
        for (int c = 0; c < myDims[4]; ++c)
        {
            for (int s = 0; s < myDims[3]; ++s)
            {
                const float* inFrame = processVol->getFrame(s, c);
#pragma omp CARET_PARFOR schedule(dynamic)
                for (int k = 0; k < myDims[2]; ++k)
                {
                    for (int j = 0; j < myDims[1]; ++j)
                    {
                        for (int i = 0; i < myDims[0]; ++i)
                        {
                            float magnitude;
                            Vector3D gradient;
                            if (myRoi == NULL || myRoi->getValue(i, j, k) > 0.0f)
                            {
                                float curval = processVol->getValue(i, j, k, s, c);
                                FloatMatrix regress = FloatMatrix::zeros(4, 5);
                                regress[3][3] = 1;//count the center voxel in case neighbors are missing (displacement and valdiff are zero, cancelling all other terms)
                                int dircheck = 0;
                                for (int neighbase = 0; neighbase < 18; neighbase += 3)
                                {
                                    int ikern = i + stencil[neighbase];
                                    int jkern = j + stencil[neighbase + 1];
                                    int kkern = k + stencil[neighbase + 2];
                                    if (volIn->indexValid(ikern, jkern, kkern))
                                    {
                                        int64_t kernIndex = volIn->getIndex(ikern, jkern, kkern);
                                        if (roiFrame == NULL || roiFrame[kernIndex] > 0.0f)
                                        {
                                            dircheck |= 1<<(2 - (neighbase / 6));//NOTE: uses the ordering of the neighbors to map neighbor to direction
                                            float valdiff = inFrame[kernIndex] - curval;
                                            Vector3D displacement = ivec * stencil[neighbase] + jvec * stencil[neighbase + 1] + kvec * stencil[neighbase + 2];
                                            regress[0][0] += displacement[0] * displacement[0];//note: this is the generic code, built to handle strange volumes, to get more speed, test
                                            regress[0][1] += displacement[0] * displacement[1];// for orthogonal volume, and use central differences (if no ROI, could special case for even more speed)
                                            regress[0][2] += displacement[0] * displacement[2];//but, seems reasonably fast anyway
                                            regress[0][3] += displacement[0];
                                            regress[0][4] += displacement[0] * valdiff;
                                            regress[1][1] += displacement[1] * displacement[1];
                                            regress[1][2] += displacement[1] * displacement[2];
                                            regress[1][3] += displacement[1];
                                            regress[1][4] += displacement[1] * valdiff;
                                            regress[2][2] += displacement[2] * displacement[2];
                                            regress[2][3] += displacement[2];
                                            regress[2][4] += displacement[2] * valdiff;
                                            regress[3][3] += 1;
                                            regress[3][4] += valdiff;
                                        }
                                    }
                                }
                                if (dircheck == 7)//have at least one neighbor in every index axis, continue
                                {
                                    regress[1][0] = regress[0][1];//finish the symmetric part of the matrix
                                    regress[2][0] = regress[0][2];
                                    regress[2][1] = regress[1][2];
                                    regress[3][0] = regress[0][3];
                                    regress[3][1] = regress[1][3];
                                    regress[3][2] = regress[2][3];
                                    FloatMatrix result = regress.reducedRowEchelon();
                                    gradient[0] = result[0][4];
                                    gradient[1] = result[1][4];
                                    gradient[2] = result[2][4];//[3][4] is the constant part of the regression
                                    magnitude = gradient.length();
                                    if (!MathFunctions::isNumeric(magnitude))
                                    {
                                        magnitude = 0.0f;
                                        gradient[0] = 0.0f;
                                        gradient[1] = 0.0f;
                                        gradient[2] = 0.0f;
                                    }
                                } else {//fallback 1: regression with 26-neighbors
                                    Vector3D directions[3];//track the displacements in index space for simplicity
                                    int dirUsed = 0;
                                    if (dircheck & 1)
                                    {
                                        directions[dirUsed][0] = 1;
                                        ++dirUsed;
                                    }
                                    if (dircheck & 2)
                                    {
                                        directions[dirUsed][1] = 1;
                                        ++dirUsed;
                                    }
                                    if (dircheck & 4)
                                    {
                                        directions[dirUsed][2] = 1;
                                        ++dirUsed;
                                    }
                                    int imin = max(i - 1, 0), jmin = max(j - 1, 0), kmin = max(k - 1, 0);
                                    int imax = min(i + 2, (int)myDims[0]), jmax = min(j + 2, (int)myDims[1]), kmax = min(k + 2, (int)myDims[2]);
                                    Vector3D voxelDir;
                                    for (int kkern = kmin; kkern < kmax; ++kkern)
                                    {
                                        voxelDir[2] = kkern - k;
                                        int kabs = abs(kkern - k);
                                        int64_t kindpart = kkern * myDims[1];
                                        for (int jkern = jmin; jkern < jmax; ++jkern)
                                        {
                                            int jabs = abs(jkern - j) + kabs;
                                            if (jabs > 0)//skip inner loop if it won't get any new neighbors
                                            {
                                                int64_t jindpart = (kindpart + jkern) * myDims[0];
                                                voxelDir[1] = jkern - j;
                                                for (int ikern = imin; ikern < imax; ++ikern)
                                                {
                                                    int64_t kernIndex = jindpart + ikern;
                                                    if (jabs + abs(ikern - i) > 1 && (roiFrame == NULL || roiFrame[kernIndex] > 0.0f))//only add non-face neighbors
                                                    {
                                                        voxelDir[0] = ikern - i;
                                                        if (dirUsed < 3)//check for singularity via base vectors being dependent
                                                        {
                                                            bool newDir = true;
                                                            switch (dirUsed)
                                                            {
                                                                case 0:
                                                                default:
                                                                    break;
                                                                case 1:
                                                                    if (voxelDir.cross(directions[0]).length() < 0.01f) newDir = false;
                                                                    break;
                                                                case 2:
                                                                    if (voxelDir.cross(directions[0]).cross(voxelDir.cross(directions[1])).length() < 0.01f)
                                                                        newDir = false;
                                                                    break;
                                                            }
                                                            if (newDir)
                                                            {
                                                                directions[dirUsed] = voxelDir;
                                                                ++dirUsed;
                                                            }
                                                        }
                                                        Vector3D displacement = ivec * voxelDir[0] + jvec * voxelDir[1] + kvec * voxelDir[2];
                                                        float valdiff = inFrame[kernIndex] - curval;
                                                        regress[0][0] += displacement[0] * displacement[0];
                                                        regress[0][1] += displacement[0] * displacement[1];
                                                        regress[0][2] += displacement[0] * displacement[2];
                                                        regress[0][3] += displacement[0];
                                                        regress[0][4] += displacement[0] * valdiff;
                                                        regress[1][1] += displacement[1] * displacement[1];
                                                        regress[1][2] += displacement[1] * displacement[2];
                                                        regress[1][3] += displacement[1];
                                                        regress[1][4] += displacement[1] * valdiff;
                                                        regress[2][2] += displacement[2] * displacement[2];
                                                        regress[2][3] += displacement[2];
                                                        regress[2][4] += displacement[2] * valdiff;
                                                        regress[3][3] += 1;
                                                        regress[3][4] += valdiff;
                                                    }
                                                }
                                            }
                                        }
                                    }
                                    if (dirUsed == 3)
                                    {
                                        regress[1][0] = regress[0][1];//finish the symmetric part of the matrix
                                        regress[2][0] = regress[0][2];
                                        regress[2][1] = regress[1][2];
                                        regress[3][0] = regress[0][3];
                                        regress[3][1] = regress[1][3];
                                        regress[3][2] = regress[2][3];
                                        FloatMatrix result = regress.reducedRowEchelon();
                                        gradient[0] = result[0][4];
                                        gradient[1] = result[1][4];
                                        gradient[2] = result[2][4];//[3][4] is the constant part of the regression
                                        magnitude = gradient.length();
                                        if (!MathFunctions::isNumeric(magnitude))
                                        {
                                            magnitude = 0.0f;
                                            gradient[0] = 0.0f;
                                            gradient[1] = 0.0f;
                                            gradient[2] = 0.0f;
                                        }
                                    } else {//fallback 2: average forward differences in 26-neighborhood
                                        Vector3D accum;
                                        int accumCount = 0;
                                        for (int kkern = kmin; kkern < kmax; ++kkern)
                                        {
                                            voxelDir[2] = kkern - k;
                                            int64_t kindpart = kkern * myDims[1];
                                            for (int jkern = jmin; jkern < jmax; ++jkern)
                                            {
                                                int64_t jindpart = (kindpart + jkern) * myDims[0];
                                                voxelDir[1] = jkern - j;
                                                for (int ikern = imin; ikern < imax; ++ikern)
                                                {
                                                    int64_t kernIndex = jindpart + ikern;
                                                    if (roiFrame == NULL || roiFrame[kernIndex] > 0.0f)
                                                    {
                                                        float valdiff = inFrame[kernIndex] - curval;
                                                        voxelDir[0] = ikern - i;
                                                        Vector3D displacement = ivec * voxelDir[0] + jvec * voxelDir[1] + kvec * voxelDir[2];
                                                        float length = displacement.length();
                                                        if (length > 0.0f)
                                                        {
                                                            accum += displacement * (valdiff / (length * length));//once to normalize vector, and once to find gradient magnitude
                                                            ++accumCount;
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                        if (accumCount > 0)
                                        {
                                            gradient = accum / accumCount;
                                            magnitude = gradient.length();
                                            if (!MathFunctions::isNumeric(magnitude))
                                            {
                                                magnitude = 0.0f;
                                                gradient[0] = 0.0f;
                                                gradient[1] = 0.0f;
                                                gradient[2] = 0.0f;
                                            }
                                        } else {
                                            magnitude = 0.0f;
                                            gradient[0] = 0.0f;
                                            gradient[1] = 0.0f;
                                            gradient[2] = 0.0f;
                                        }
                                    }
                                }
                            } else {
                                magnitude = 0.0f;
                                gradient[0] = 0.0f;
                                gradient[1] = 0.0f;
                                gradient[2] = 0.0f;
                            }
                            volOut->setValue(magnitude, i, j, k, s, c);
                            if (vectorsOut != NULL)
                            {
                                int subvolbase = s * 3;
                                vectorsOut->setValue(gradient[0], i, j, k, subvolbase, c);
                                vectorsOut->setValue(gradient[1], i, j, k, subvolbase + 1, c);
                                vectorsOut->setValue(gradient[2], i, j, k, subvolbase + 2, c);
                            }
                        }
                    }
                }
            }
        }
    } else {
        origDims.resize(3);
        volOut->reinitialize(origDims, volIn->getSform(), myDims[4], volIn->getType(), volIn->m_header);
        if (vectorsOut != NULL)
        {
            origDims.push_back(3);
            vectorsOut->reinitialize(origDims, volIn->getSform(), myDims[4], volIn->getType());
        }
        for (int c = 0; c < myDims[4]; ++c)
        {
            const float* inFrame = processVol->getFrame(useSubvol, c);
#pragma omp CARET_PARFOR schedule(dynamic)
            for (int k = 0; k < myDims[2]; ++k)
            {
                for (int j = 0; j < myDims[1]; ++j)
                {
                    for (int i = 0; i < myDims[0]; ++i)
                    {
                        float magnitude;
                        Vector3D gradient;
                        if (myRoi == NULL || myRoi->getValue(i, j, k) > 0.0f)
                        {
                            float curval = processVol->getValue(i, j, k, useSubvol, c);
                            FloatMatrix regress = FloatMatrix::zeros(4, 5);
                            regress[3][3] = 1;//count the center voxel in case neighbors are missing (displacement and valdiff are zero, cancelling all other terms)
                            int dircheck = 0;
                            for (int neighbase = 0; neighbase < 18; neighbase += 3)
                            {
                                int ikern = i + stencil[neighbase];
                                int jkern = j + stencil[neighbase + 1];
                                int kkern = k + stencil[neighbase + 2];
                                if (volIn->indexValid(ikern, jkern, kkern))
                                {
                                    int64_t kernIndex = volIn->getIndex(ikern, jkern, kkern);
                                    if (roiFrame == NULL || roiFrame[kernIndex] > 0.0f)
                                    {
                                        dircheck |= 1<<(2 - (neighbase / 6));//NOTE: uses the ordering of the neighbors to map neighbor to direction
                                        float valdiff = inFrame[kernIndex] - curval;
                                        Vector3D displacement = ivec * stencil[neighbase] + jvec * stencil[neighbase + 1] + kvec * stencil[neighbase + 2];
                                        regress[0][0] += displacement[0] * displacement[0];//note: this is the generic code, built to handle strange volumes, to get more speed, test
                                        regress[0][1] += displacement[0] * displacement[1];// for orthogonal volume, and use central differences (if no ROI, could special case for even more speed)
                                        regress[0][2] += displacement[0] * displacement[2];
                                        regress[0][3] += displacement[0];
                                        regress[0][4] += displacement[0] * valdiff;
                                        regress[1][1] += displacement[1] * displacement[1];
                                        regress[1][2] += displacement[1] * displacement[2];
                                        regress[1][3] += displacement[1];
                                        regress[1][4] += displacement[1] * valdiff;
                                        regress[2][2] += displacement[2] * displacement[2];
                                        regress[2][3] += displacement[2];
                                        regress[2][4] += displacement[2] * valdiff;
                                        regress[3][3] += 1;
                                        regress[3][4] += valdiff;
                                    }
                                }
                            }
                            if (dircheck == 7)//have at least one neighbor in every index axis, continue
                            {
                                regress[1][0] = regress[0][1];//finish the symmetric part of the matrix
                                regress[2][0] = regress[0][2];
                                regress[2][1] = regress[1][2];
                                regress[3][0] = regress[0][3];
                                regress[3][1] = regress[1][3];
                                regress[3][2] = regress[2][3];
                                FloatMatrix result = regress.reducedRowEchelon();
                                gradient[0] = result[0][4];
                                gradient[1] = result[1][4];
                                gradient[2] = result[2][4];//[3][4] is the constant part of the regression
                                magnitude = gradient.length();
                                if (!MathFunctions::isNumeric(magnitude))
                                {
                                    magnitude = 0.0f;
                                    gradient[0] = 0.0f;
                                    gradient[1] = 0.0f;
                                    gradient[2] = 0.0f;
                                }
                            } else {//fallback 1: regression with 26-neighbors
                                Vector3D directions[3];//track the displacements in index space for simplicity
                                int dirUsed = 0;
                                if (dircheck & 1)
                                {
                                    directions[dirUsed][0] = 1;
                                    ++dirUsed;
                                }
                                if (dircheck & 2)
                                {
                                    directions[dirUsed][1] = 1;
                                    ++dirUsed;
                                }
                                if (dircheck & 4)
                                {
                                    directions[dirUsed][2] = 1;
                                    ++dirUsed;
                                }
                                int imin = max(i - 1, 0), jmin = max(j - 1, 0), kmin = max(k - 1, 0);
                                int imax = min(i + 2, (int)myDims[0]), jmax = min(j + 2, (int)myDims[1]), kmax = min(k + 2, (int)myDims[2]);
                                Vector3D voxelDir;
                                for (int kkern = kmin; kkern < kmax; ++kkern)
                                {
                                    voxelDir[2] = kkern - k;
                                    int kabs = abs(kkern - k);
                                    int64_t kindpart = kkern * myDims[1];
                                    for (int jkern = jmin; jkern < jmax; ++jkern)
                                    {
                                        int jabs = abs(jkern - j) + kabs;
                                        if (jabs > 0)//skip inner loop if it won't get any new neighbors
                                        {
                                            int64_t jindpart = (kindpart + jkern) * myDims[0];
                                            voxelDir[1] = jkern - j;
                                            for (int ikern = imin; ikern < imax; ++ikern)
                                            {
                                                int64_t kernIndex = jindpart + ikern;
                                                if (jabs + abs(ikern - i) > 1 && (roiFrame == NULL || roiFrame[kernIndex] > 0.0f))//only add non-face neighbors
                                                {
                                                    voxelDir[0] = ikern - i;
                                                    if (dirUsed < 3)//check for singularity via base vectors being dependent
                                                    {
                                                        bool newDir = true;
                                                        switch (dirUsed)
                                                        {
                                                            case 0:
                                                            default:
                                                                break;
                                                            case 1:
                                                                if (voxelDir.cross(directions[0]).length() < 0.01f) newDir = false;
                                                                break;
                                                            case 2:
                                                                if (voxelDir.cross(directions[0]).cross(voxelDir.cross(directions[1])).length() < 0.01f)
                                                                    newDir = false;
                                                                break;
                                                        }
                                                        if (newDir)
                                                        {
                                                            directions[dirUsed] = voxelDir;
                                                            ++dirUsed;
                                                        }
                                                    }
                                                    Vector3D displacement = ivec * voxelDir[0] + jvec * voxelDir[1] + kvec * voxelDir[2];
                                                    float valdiff = inFrame[kernIndex] - curval;
                                                    regress[0][0] += displacement[0] * displacement[0];
                                                    regress[0][1] += displacement[0] * displacement[1];
                                                    regress[0][2] += displacement[0] * displacement[2];
                                                    regress[0][3] += displacement[0];
                                                    regress[0][4] += displacement[0] * valdiff;
                                                    regress[1][1] += displacement[1] * displacement[1];
                                                    regress[1][2] += displacement[1] * displacement[2];
                                                    regress[1][3] += displacement[1];
                                                    regress[1][4] += displacement[1] * valdiff;
                                                    regress[2][2] += displacement[2] * displacement[2];
                                                    regress[2][3] += displacement[2];
                                                    regress[2][4] += displacement[2] * valdiff;
                                                    regress[3][3] += 1;
                                                    regress[3][4] += valdiff;
                                                }
                                            }
                                        }
                                    }
                                }
                                if (dirUsed == 3)
                                {
                                    regress[1][0] = regress[0][1];//finish the symmetric part of the matrix
                                    regress[2][0] = regress[0][2];
                                    regress[2][1] = regress[1][2];
                                    regress[3][0] = regress[0][3];
                                    regress[3][1] = regress[1][3];
                                    regress[3][2] = regress[2][3];
                                    FloatMatrix result = regress.reducedRowEchelon();
                                    gradient[0] = result[0][4];
                                    gradient[1] = result[1][4];
                                    gradient[2] = result[2][4];//[3][4] is the constant part of the regression
                                    magnitude = gradient.length();
                                    if (!MathFunctions::isNumeric(magnitude))
                                    {
                                        magnitude = 0.0f;
                                        gradient[0] = 0.0f;
                                        gradient[1] = 0.0f;
                                        gradient[2] = 0.0f;
                                    }
                                } else {//fallback 2: average forward differences in 26-neighborhood
                                    Vector3D accum;
                                    int accumCount = 0;
                                    for (int kkern = kmin; kkern < kmax; ++kkern)
                                    {
                                        voxelDir[2] = kkern - k;
                                        int64_t kindpart = kkern * myDims[1];
                                        for (int jkern = jmin; jkern < jmax; ++jkern)
                                        {
                                            int64_t jindpart = (kindpart + jkern) * myDims[0];
                                            voxelDir[1] = jkern - j;
                                            for (int ikern = imin; ikern < imax; ++ikern)
                                            {
                                                int64_t kernIndex = jindpart + ikern;
                                                if (roiFrame == NULL || roiFrame[kernIndex] > 0.0f)
                                                {
                                                    float valdiff = inFrame[kernIndex] - curval;
                                                    voxelDir[0] = ikern - i;
                                                    Vector3D displacement = ivec * voxelDir[0] + jvec * voxelDir[1] + kvec * voxelDir[2];
                                                    float length = displacement.length();
                                                    if (length > 0.0f)
                                                    {
                                                        accum += displacement * (valdiff / (length * length));//once to normalize vector, and once to find gradient magnitude
                                                        ++accumCount;
                                                    }
                                                }
                                            }
                                        }
                                    }
                                    if (accumCount > 0)
                                    {
                                        gradient = accum / accumCount;
                                        magnitude = gradient.length();
                                        if (!MathFunctions::isNumeric(magnitude))
                                        {
                                            magnitude = 0.0f;
                                            gradient[0] = 0.0f;
                                            gradient[1] = 0.0f;
                                            gradient[2] = 0.0f;
                                        }
                                    } else {
                                        magnitude = 0.0f;
                                        gradient[0] = 0.0f;
                                        gradient[1] = 0.0f;
                                        gradient[2] = 0.0f;
                                    }
                                }
                            }
                        } else {
                            magnitude = 0.0f;
                            gradient[0] = 0.0f;
                            gradient[1] = 0.0f;
                            gradient[2] = 0.0f;
                        }
                        volOut->setValue(magnitude, i, j, k, 0, c);
                        if (vectorsOut != NULL)
                        {
                            vectorsOut->setValue(gradient[0], i, j, k, 0, c);
                            vectorsOut->setValue(gradient[1], i, j, k, 1, c);
                            vectorsOut->setValue(gradient[2], i, j, k, 2, c);
                        }
                    }
                }
            }
        }
    }
}

float AlgorithmVolumeGradient::getAlgorithmInternalWeight()
{
    return 1.0f;//override this if needed, if the progress bar isn't smooth
}

float AlgorithmVolumeGradient::getSubAlgorithmWeight()
{
    //return AlgorithmInsertNameHere::getAlgorithmWeight();//if you use a subalgorithm
    return 0.0f;
}
