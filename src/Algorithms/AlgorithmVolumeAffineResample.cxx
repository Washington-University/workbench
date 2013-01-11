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

#include "AlgorithmVolumeAffineResample.h"
#include "AffineFile.h"
#include "AlgorithmException.h"
#include "CaretLogger.h"
#include "CaretOMP.h"
#include "Vector3D.h"

using namespace caret;
using namespace std;

AString AlgorithmVolumeAffineResample::getCommandSwitch()
{
    return "-volume-affine-resample";
}

AString AlgorithmVolumeAffineResample::getShortDescription()
{
    return "APPLY AFFINE TRANSFORM TO VOLUME AND RESAMPLE";
}

OperationParameters* AlgorithmVolumeAffineResample::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    
    ret->addVolumeParameter(1, "volume-in", "volume to resample");
    
    ret->addStringParameter(2, "affine", "the affine file to apply");
    
    ret->addVolumeParameter(3, "volume-space", "a volume file in the volume space you want for the output");
    
    ret->addStringParameter(4, "method", "the resampling method");
    
    ret->addVolumeOutputParameter(5, "volume-out", "the output volume");
    
    OptionalParameter* flirtOpt = ret->createOptionalParameter(6, "-flirt", "MUST be used if affine is a flirt affine");
    flirtOpt->addStringParameter(1, "source-volume", "the source volume used when generating the affine");
    flirtOpt->addStringParameter(2, "target-volume", "the target volume used when generating the affine");
    
    ret->setHelpText(
        AString("This is where you set the help text.  DO NOT add the info about what the command line format is, ") +
        "and do not give the command switch, short description, or the short descriptions of parameters.  Do not indent, " +
        "add newlines, or format the text in any way other than to separate paragraphs within the help text prose."
    );
    return ret;
}

void AlgorithmVolumeAffineResample::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    VolumeFile* inVol = myParams->getVolume(1);
    AString affName = myParams->getString(2);
    VolumeFile* refSpace = myParams->getVolume(3);
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
    AlgorithmVolumeAffineResample(myProgObj, inVol, affMat, refSpace, myMethod, outVol);
}

AlgorithmVolumeAffineResample::AlgorithmVolumeAffineResample(ProgressObject* myProgObj, const VolumeFile* inVol, const FloatMatrix& myAffine,
                                                             const VolumeFile* refSpace, const VolumeFile::InterpType& myMethod, VolumeFile* outVol) : AbstractAlgorithm(myProgObj)
{
    LevelProgress myProgress(myProgObj);
    int64_t affRows, affColumns;
    myAffine.getDimensions(affRows, affColumns);
    if (affRows < 3 || affRows > 4 || affColumns != 4) throw AlgorithmException("input matrix is not an affine matrix");
    vector<int64_t> outDims = inVol->getOriginalDimensions(); const vector<int64_t>& refDims = refSpace->getOriginalDimensions();
    if (outDims.size() < 3 || refDims.size() < 3) throw AlgorithmException("input and refspace must have 3 spatial dimensions");
    outDims[0] = refDims[0];
    outDims[1] = refDims[1];
    outDims[2] = refDims[2];
    int64_t numMaps = inVol->getNumberOfMaps(), numComponents = inVol->getNumberOfComponents();
    outVol->reinitialize(outDims, refSpace->getVolumeSpace(), numComponents, inVol->getType());
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
    for (int64_t c = 0; c < numComponents; ++c)
    {
        for (int64_t b = 0; b < numMaps; ++b)
        {
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
                        outVol->setValue(interpVal, i, j, k, b, c);
                    }
                }
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
