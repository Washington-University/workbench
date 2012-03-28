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

#include "AlgorithmVolumeGradient.h"
#include "AlgorithmException.h"
#include "VolumeFile.h"
#include "CaretAssert.h"
#include "FloatMatrix.h"
#include "Vector3D.h"
#include "MathFunctions.h"
#include "CaretOMP.h"
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
    
    ret->addDoubleParameter(2, "kernel", "sigma for gaussian weighting function, in mm");
    
    ret->addVolumeOutputParameter(3, "volume-out", "the output gradient magnitude volume");
    
    OptionalParameter* roiOption = ret->createOptionalParameter(4, "-roi", "select a region of interest to take the gradient of");
    roiOption->addVolumeParameter(1, "roi-volume", "the region to smooth within");
    
    OptionalParameter* vecOption = ret->createOptionalParameter(5, "-vectors", "output vectors");
    vecOption->addVolumeOutputParameter(1, "vector-volume-out", "the vectors as a volume file");
    
    OptionalParameter* subvolSelect = ret->createOptionalParameter(6, "-subvolume", "select a single subvolume to take the gradient of");
    subvolSelect->addStringParameter(1, "subvol", "the subvolume number or name");
    
    ret->setHelpText(
        AString("Computes the gradient of the volume by doing weighted regressions for each voxel.  The slopes of the resulting ") +
        "linear function are considered the components of the gradient vector, and the magnitude of this vector is the output.  " +
        "If specified, the volume vector output is arranged with the x, y, and z components from a subvolume as consecutive subvolumes."
    );
    return ret;
}

void AlgorithmVolumeGradient::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    VolumeFile* volIn = myParams->getVolume(1);
    float kernel = (float)myParams->getDouble(2);
    VolumeFile* volOut = myParams->getOutputVolume(3);
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
    AlgorithmVolumeGradient(myProgObj, volIn, kernel, volOut, myRoi, vectorsOut, subvolNum);
}

AlgorithmVolumeGradient::AlgorithmVolumeGradient(ProgressObject* myProgObj, const VolumeFile* volIn, const float& kernel, VolumeFile* volOut, const VolumeFile* myRoi, VolumeFile* vectorsOut, const int& subvolNum) : AbstractAlgorithm(myProgObj)
{
    CaretAssert(volIn != NULL);
    CaretAssert(volOut != NULL);
    LevelProgress myProgress(myProgObj);
    if (myRoi != NULL && !myRoi->matchesVolumeSpace(volIn))
    {
        throw AlgorithmException("roi volume space does not match input");
    }
    if (subvolNum < -1 || subvolNum >= volIn->getNumberOfMaps())
    {
        throw AlgorithmException("invalid subvolume specified");
    }
    if (kernel <= 0.0f)
    {
        throw AlgorithmException("kernel too small");
    }
    precompute(kernel, volIn);
    int irange = (m_kernelWeights.size() - 1) / 2;//precompute should never hand back less than a 3x3x3, and always odd
    int jrange = (m_kernelWeights[0].size() - 1) / 2;
    int krange = (m_kernelWeights[0][0].size() - 1) / 2;
    vector<int64_t> origDims = volIn->getOriginalDimensions(), myDims;
    volIn->getDimensions(myDims);
    const float* roiFrame = NULL;
    if (myRoi != NULL)
    {
        roiFrame = myRoi->getFrame();
    }
    if (subvolNum == -1)
    {
        volOut->reinitialize(origDims, volIn->getVolumeSpace(), myDims[4], volIn->getType());
        if (vectorsOut != NULL)
        {
            while (origDims.size() < 4)
            {
                origDims.push_back(1);
            }
            origDims[3] *= 3;
            vectorsOut->reinitialize(origDims, volIn->getVolumeSpace(), myDims[4], volIn->getType());
        }
        for (int c = 0; c < myDims[4]; ++c)
        {
            for (int s = 0; s < myDims[3]; ++s)
            {
                const float* inFrame = volIn->getFrame(s, c);
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
                                float curval = volIn->getValue(i, j, k, s, c);
                                FloatMatrix regress = FloatMatrix::zeros(4, 5);
                                int imin = i - irange, imax = i + irange + 1;//one-after array size convention
                                if (imin < 0) imin = 0;
                                if (imax > myDims[0]) imax = myDims[0];
                                int jmin = j - jrange, jmax = j + jrange + 1;
                                if (jmin < 0) jmin = 0;
                                if (jmax > myDims[1]) jmax = myDims[1];
                                int kmin = k - krange, kmax = k + krange + 1;
                                if (kmin < 0) kmin = 0;
                                if (kmax > myDims[2]) kmax = myDims[2];
                                for (int kkern = kmin; kkern < kmax; ++kkern)
                                {
                                    int64_t kindpart = kkern * myDims[1];
                                    int kkernpart = kkern - k + krange;
                                    for (int jkern = jmin; jkern < jmax; ++jkern)
                                    {
                                        int64_t jindpart = (kindpart + jkern) * myDims[0];
                                        int jkernpart = jkern - j + jrange;
                                        for (int ikern = imin; ikern < imax; ++ikern)
                                        {
                                            int64_t thisIndex = jindpart + ikern;//somewhat optimized index computation, could remove some integer multiplies, but there aren't that many
                                            int ikernpart = ikern - i + irange;
                                            float weight = m_kernelWeights[kkernpart][jkernpart][ikernpart];//reversed indexing in order to align the linear access with the fast moving index
                                            if (weight != 0.0f && (myRoi == NULL || roiFrame[thisIndex] > 0.0f))
                                            {
                                                Vector3D displacement = m_displacements[kkernpart][jkernpart][ikernpart];//NOTE: has weight PREAPPLIED!
                                                float valdiff = (inFrame[thisIndex] - curval) * weight;//NOTE: it is VERY IMPORTANT to multiply weight in instead of the usual 1.0 constant in [3][...] and [...][3]
                                                regress[0][0] += displacement[0] * displacement[0];
                                                regress[0][1] += displacement[0] * displacement[1];
                                                regress[0][2] += displacement[0] * displacement[2];
                                                regress[0][3] += weight * displacement[0];
                                                regress[0][4] += displacement[0] * valdiff;
                                                regress[1][1] += displacement[1] * displacement[1];
                                                regress[1][2] += displacement[1] * displacement[2];
                                                regress[1][3] += weight * displacement[1];
                                                regress[1][4] += displacement[1] * valdiff;
                                                regress[2][2] += displacement[2] * displacement[2];
                                                regress[2][3] += weight * displacement[2];
                                                regress[2][4] += displacement[2] * valdiff;
                                                regress[3][3] += weight * weight;//that means square for constant times constant, too
                                                regress[3][4] += weight * valdiff;
                                            }
                                        }
                                    }
                                }
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
        volOut->reinitialize(origDims, volIn->getVolumeSpace(), myDims[4], volIn->getType());
        if (vectorsOut != NULL)
        {
            origDims.push_back(3);
            vectorsOut->reinitialize(origDims, volIn->getVolumeSpace(), myDims[4], volIn->getType());
        }
        for (int c = 0; c < myDims[4]; ++c)
        {
            const float* inFrame = volIn->getFrame(subvolNum, c);
#pragma omp CARET_PARFOR schedule(dynamic)
            for (int k = 0; k < myDims[2]; ++k)
            {
                for (int j = 0; j < myDims[1]; ++j)
                {
                    for (int i = 0; i < myDims[0]; ++i)
                    {
                        float magnitude;
                        Vector3D gradient;
                        if (myRoi->getValue(i, j, k) > 0.0f)
                        {
                            float curval = volIn->getValue(i, j, k, subvolNum, c);
                            FloatMatrix regress = FloatMatrix::zeros(4, 5);
                            int imin = i - irange, imax = i + irange + 1;//one-after array size convention
                            if (imin < 0) imin = 0;
                            if (imax > myDims[0]) imax = myDims[0];
                            int jmin = j - jrange, jmax = j + jrange + 1;
                            if (jmin < 0) jmin = 0;
                            if (jmax > myDims[1]) jmax = myDims[1];
                            int kmin = k - krange, kmax = k + krange + 1;
                            if (kmin < 0) kmin = 0;
                            if (kmax > myDims[2]) kmax = myDims[2];
                            for (int kkern = kmin; kkern < kmax; ++kkern)
                            {
                                int64_t kindpart = kkern * myDims[1];
                                int kkernpart = kkern - k + krange;
                                for (int jkern = jmin; jkern < jmax; ++jkern)
                                {
                                    int64_t jindpart = (kindpart + jkern) * myDims[0];
                                    int jkernpart = jkern - j + jrange;
                                    for (int ikern = imin; ikern < imax; ++ikern)
                                    {
                                        int64_t thisIndex = jindpart + ikern;//somewhat optimized index computation, could remove some integer multiplies, but there aren't that many
                                        int ikernpart = ikern - i + irange;
                                        float weight = m_kernelWeights[kkernpart][jkernpart][ikernpart];//reversed indexing in order to align the linear access with the fast moving index
                                        if (weight != 0.0f && (myRoi == NULL || roiFrame[thisIndex] > 0.0f))
                                        {
                                            Vector3D displacement = m_displacements[kkernpart][jkernpart][ikernpart];//NOTE: has weight PREAPPLIED!
                                            float valdiff = (inFrame[thisIndex] - curval) * weight;//NOTE: it is VERY IMPORTANT to multiply weight in instead of the usual 1.0 constant in [3][...] and [...][3]
                                            regress[0][0] += displacement[0] * displacement[0];
                                            regress[0][1] += displacement[0] * displacement[1];
                                            regress[0][2] += displacement[0] * displacement[2];
                                            regress[0][3] += weight * displacement[0];
                                            regress[0][4] += displacement[0] * valdiff;
                                            regress[1][1] += displacement[1] * displacement[1];
                                            regress[1][2] += displacement[1] * displacement[2];
                                            regress[1][3] += weight * displacement[1];
                                            regress[1][4] += displacement[1] * valdiff;
                                            regress[2][2] += displacement[2] * displacement[2];
                                            regress[2][3] += weight * displacement[2];
                                            regress[2][4] += displacement[2] * valdiff;
                                            regress[3][3] += weight * weight;//that means square for constant times constant, too
                                            regress[3][4] += weight * valdiff;
                                        }
                                    }
                                }
                            }
                            regress[1][0] = regress[0][1];//finish the symmetric part of the matrix
                            regress[2][0] = regress[0][2];
                            regress[2][1] = regress[1][2];
                            regress[3][0] = regress[0][3];
                            regress[3][1] = regress[1][3];
                            regress[3][2] = regress[2][3];
                            FloatMatrix result = regress.reducedRowEchelon();//and solve the regression
                            gradient[0] = result[0][4];
                            gradient[1] = result[1][4];
                            gradient[2] = result[2][4];//[3][4] is the constant part of the regression
                            magnitude = gradient.length();
                            if (!MathFunctions::isNumeric(magnitude))//check results for sanity
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

void AlgorithmVolumeGradient::precompute(float kernel, const caret::VolumeFile* volIn)
{
    float kernBox = 3 * kernel;//copied from volume smoothing (again)...probably should be a function in VolumeFile
    vector<vector<float> > volSpace = volIn->getVolumeSpace();
    Vector3D ivec, jvec, kvec, origin, ijorth, jkorth, kiorth;
    ivec[0] = volSpace[0][0]; jvec[0] = volSpace[0][1]; kvec[0] = volSpace[0][2]; origin[0] = volSpace[0][3];
    ivec[1] = volSpace[1][0]; jvec[1] = volSpace[1][1]; kvec[1] = volSpace[1][2]; origin[1] = volSpace[1][3];
    ivec[2] = volSpace[2][0]; jvec[2] = volSpace[2][1]; kvec[2] = volSpace[2][2]; origin[2] = volSpace[2][3];
    ijorth = ivec.cross(jvec).normal();//find the bounding box that encloses a sphere of radius kernBox
    jkorth = jvec.cross(kvec).normal();
    kiorth = kvec.cross(ivec).normal();
    int irange = (int)floor(abs(kernBox / ivec.dot(jkorth)));
    int jrange = (int)floor(abs(kernBox / jvec.dot(kiorth)));
    int krange = (int)floor(abs(kernBox / kvec.dot(ijorth)));
    if (irange < 1) irange = 1;//don't underflow
    if (jrange < 1) jrange = 1;
    if (krange < 1) krange = 1;
    int isize = irange * 2 + 1;//and construct a precomputed kernel in the box
    int jsize = jrange * 2 + 1;
    int ksize = krange * 2 + 1;
    Vector3D kscratch, jscratch, iscratch;
    m_kernelWeights.resize(ksize);
    m_displacements.resize(ksize);
    for (int k = 0; k < ksize; ++k)
    {
        kscratch = kvec * (k - krange);
        m_kernelWeights[k].resize(jsize);
        m_displacements[k].resize(jsize);
        for (int j = 0; j < jsize; ++j)
        {
            jscratch = kscratch + jvec * (j - jrange);
            m_kernelWeights[k][j].resize(isize);
            m_displacements[k][j].resize(isize);
            for (int i = 0; i < isize; ++i)
            {
                iscratch = jscratch + ivec * (i - irange);
                float tempf = iscratch.length();
                if (tempf > kernBox)
                {
                    m_kernelWeights[k][j][i] = 0.0f;//test for zero to avoid some multiplies/adds, cheaper or cleaner than checking bounds on indexes from an index list
                } else {
                    m_kernelWeights[k][j][i] = exp(-tempf * tempf / kernel / kernel / 2.0f);//optimization here isn't critical
                    m_displacements[k][j][i] = iscratch * m_kernelWeights[k][j][i];//preapply the weight to the displacement for speed
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
    return 0.0f;
}
