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

#include "AlgorithmVolumeAffineResample.h"
#include "AffineFile.h"
#include "AlgorithmException.h"
#include "CaretLogger.h"
#include "CaretOMP.h"
#include "NiftiIO.h"
#include "Vector3D.h"

using namespace caret;
using namespace std;

AString AlgorithmVolumeAffineResample::getCommandSwitch()
{
    return "-volume-affine-resample";
}

AString AlgorithmVolumeAffineResample::getShortDescription()
{
    return "DEPRECATED: use -volume-resample";
}

OperationParameters* AlgorithmVolumeAffineResample::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    
    ret->addVolumeParameter(1, "volume-in", "volume to resample");
    
    ret->addStringParameter(2, "affine", "the affine file to apply");
    
    ret->addStringParameter(3, "volume-space", "a volume file in the volume space you want for the output");
    
    ret->addStringParameter(4, "method", "the resampling method");
    
    ret->addVolumeOutputParameter(5, "volume-out", "the output volume");
    
    OptionalParameter* flirtOpt = ret->createOptionalParameter(6, "-flirt", "MUST be used if affine is a flirt affine");
    flirtOpt->addStringParameter(1, "source-volume", "the source volume used when generating the affine");
    flirtOpt->addStringParameter(2, "target-volume", "the target volume used when generating the affine");
    
    ret->setHelpText(
        AString("DEPRECATED: this command may be removed in a future release, use -volume-resample.\n\n") +
        "Resample a volume file with an affine transformation.  " +
        "The recommended methods are CUBIC (cubic spline) for most data, and ENCLOSING_VOXEL for label data.  "
        "The parameter <method> must be one of:\n\n" +
        "CUBIC\nENCLOSING_VOXEL\nTRILINEAR"
    );
    return ret;
}

void AlgorithmVolumeAffineResample::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    VolumeFile* inVol = myParams->getVolume(1);
    AString affName = myParams->getString(2);
    AString refSpaceName = myParams->getString(3);
    AString method = myParams->getString(4);
    VolumeFile* outVol = myParams->getOutputVolume(5);
    OptionalParameter* flirtOpt = myParams->getOptionalParameter(6);
    AffineFile myAffine;
    if (flirtOpt->m_present)
    {
        myAffine.readFlirt(affName, flirtOpt->getString(1), flirtOpt->getString(2));
    } else {
        myAffine.readWorld(affName);
    }
    VolumeFile::InterpType myMethod = VolumeFile::CUBIC;
    if (method == "CUBIC")
    {
        myMethod = VolumeFile::CUBIC;
    } else if (method == "TRILINEAR") {
        myMethod = VolumeFile::TRILINEAR;
    } else if (method == "ENCLOSING_VOXEL") {
        myMethod = VolumeFile::ENCLOSING_VOXEL;
    } else {
        throw AlgorithmException("unrecognized interpolation method");
    }
    FloatMatrix affMat = FloatMatrix(myAffine.getMatrix());
    NiftiIO refSpaceIO;
    refSpaceIO.openRead(refSpaceName);
    vector<int64_t> refDims = refSpaceIO.getDimensions();
    if (refDims.size() < 3) refDims.resize(3, 1);
    AlgorithmVolumeAffineResample(myProgObj, inVol, affMat, refDims.data(), refSpaceIO.getHeader().getSForm(), myMethod, outVol);
}

AlgorithmVolumeAffineResample::AlgorithmVolumeAffineResample(ProgressObject* myProgObj, const VolumeFile* inVol, const FloatMatrix& myAffine,
                                                             const int64_t refDims[3], const vector<vector<float> >& refSform, const VolumeFile::InterpType& myMethod, VolumeFile* outVol) : AbstractAlgorithm(myProgObj)
{
    LevelProgress myProgress(myProgObj);
    int64_t affRows, affColumns;
    myAffine.getDimensions(affRows, affColumns);
    if (affRows < 3 || affRows > 4 || affColumns != 4) throw AlgorithmException("input matrix is not an affine matrix");
    vector<int64_t> outDims = inVol->getOriginalDimensions();
    if (outDims.size() < 3) throw AlgorithmException("input must have 3 spatial dimensions");
    outDims[0] = refDims[0];
    outDims[1] = refDims[1];
    outDims[2] = refDims[2];
    int64_t numMaps = inVol->getNumberOfMaps(), numComponents = inVol->getNumberOfComponents();
    outVol->reinitialize(outDims, refSform, numComponents, inVol->getType(), inVol->m_header);
    FloatMatrix targetToSource = myAffine;
    targetToSource.resize(4, 4);
    targetToSource[3][0] = 0.0f;
    targetToSource[3][1] = 0.0f;
    targetToSource[3][2] = 0.0f;
    targetToSource[3][3] = 1.0f;
    targetToSource = targetToSource.inverse();
    Vector3D xvec, yvec, zvec, offset;
    xvec[0] = targetToSource[0][0]; xvec[1] = targetToSource[1][0]; xvec[2] = targetToSource[2][0];
    yvec[0] = targetToSource[0][1]; yvec[1] = targetToSource[1][1]; yvec[2] = targetToSource[2][1];
    zvec[0] = targetToSource[0][2]; zvec[1] = targetToSource[1][2]; zvec[2] = targetToSource[2][2];
    offset[0] = targetToSource[0][3]; offset[1] = targetToSource[1][3]; offset[2] = targetToSource[2][3];
    vector<float> scratchFrame(outDims[0] * outDims[1] * outDims[2], 0.0f);
    if (inVol->isMappedWithLabelTable())
    {
        if (myMethod != VolumeFile::ENCLOSING_VOXEL)
        {
            CaretLogWarning("using interpolation type other than ENCLOSING_VOXEL on a label volume");
        }
        for (int64_t i = 0; i < numMaps; ++i)
        {
            *(outVol->getMapLabelTable(i)) = *(inVol->getMapLabelTable(i));
        }
    }
    for (int64_t i = 0; i < numMaps; ++i)
    {
        outVol->setMapName(i, inVol->getMapName(i));
    }
    for (int64_t c = 0; c < numComponents; ++c)
    {
        for (int64_t b = 0; b < numMaps; ++b)
        {
            if (myMethod == VolumeFile::CUBIC)
            {
                inVol->validateSpline(b, c);//because deconvolve is parallel, but won't execute parallel if we are already in a parallel section
            }
#pragma omp CARET_PARFOR schedule(dynamic)
            for (int64_t k = 0; k < outDims[2]; ++k)
            {
                for (int64_t j = 0; j < outDims[1]; ++j)
                {
                    for (int64_t i = 0; i < outDims[0]; ++i)
                    {
                        Vector3D outCoord, inCoord;
                        outVol->indexToSpace(i, j, k, outCoord);
                        inCoord = xvec * outCoord[0] + yvec * outCoord[1] + zvec * outCoord[2] + offset;
                        float interpVal = inVol->interpolateValue(inCoord, myMethod, NULL, b, c);
                        scratchFrame[outVol->getIndex(i, j, k)] = interpVal;
                    }
                }
            }
            outVol->setFrame(scratchFrame.data(), b, c);
            if (myMethod == VolumeFile::CUBIC)
            {
                inVol->freeSpline(b, c);//release memory we no longer need, if we allocated it
            }
        }
    }
}

float AlgorithmVolumeAffineResample::getAlgorithmInternalWeight()
{
    return 1.0f;//override this if needed, if the progress bar isn't smooth
}

float AlgorithmVolumeAffineResample::getSubAlgorithmWeight()
{
    //return AlgorithmInsertNameHere::getAlgorithmWeight();//if you use a subalgorithm
    return 0.0f;
}
