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

#include "AlgorithmVolumeWarpfieldResample.h"
#include "AlgorithmException.h"

#include "CaretLogger.h"
#include "CaretOMP.h"
#include "NiftiIO.h"
#include "Vector3D.h"
#include "WarpfieldFile.h"

using namespace caret;
using namespace std;

AString AlgorithmVolumeWarpfieldResample::getCommandSwitch()
{
    return "-volume-warpfield-resample";
}

AString AlgorithmVolumeWarpfieldResample::getShortDescription()
{
    return "RESAMPLE VOLUME USING WARPFIELD";
}

OperationParameters* AlgorithmVolumeWarpfieldResample::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    ret->addVolumeParameter(1, "volume-in", "volume to resample");
    
    ret->addStringParameter(2, "warpfield", "the warpfield to apply");
    
    ret->addStringParameter(3, "volume-space", "a volume file in the volume space you want for the output");
    
    ret->addStringParameter(4, "method", "the resampling method");
    
    ret->addVolumeOutputParameter(5, "volume-out", "the output volume");
    
    OptionalParameter* fnirtOpt = ret->createOptionalParameter(6, "-fnirt", "MUST be used if using a fnirt warpfield");
    fnirtOpt->addStringParameter(1, "source-volume", "the source volume used when generating the warpfield");
    
    ret->setHelpText(
        AString("Resample a volume file with a warpfield.  ") +
        "The recommended methods are CUBIC (cubic spline) for most data, and ENCLOSING_VOXEL for label data.  "
        "The parameter <method> must be one of:\n\n" +
        "CUBIC\nENCLOSING_VOXEL\nTRILINEAR"
    );
    return ret;
}

void AlgorithmVolumeWarpfieldResample::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    VolumeFile* inVol = myParams->getVolume(1);
    AString warpName = myParams->getString(2);
    AString refSpaceName = myParams->getString(3);
    AString method = myParams->getString(4);
    VolumeFile* outVol = myParams->getOutputVolume(5);
    OptionalParameter* fnirtOpt = myParams->getOptionalParameter(6);
    WarpfieldFile myWarpfield;
    if (fnirtOpt->m_present)
    {
        myWarpfield.readFnirt(warpName, fnirtOpt->getString(1));
    } else {
        myWarpfield.readWorld(warpName);
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
    NiftiIO refSpaceIO;
    refSpaceIO.openRead(refSpaceName);
    vector<int64_t> refDims = refSpaceIO.getDimensions();
    if (refDims.size() < 3) refDims.resize(3, 1);
    AlgorithmVolumeWarpfieldResample(myProgObj, inVol, myWarpfield.getWarpfield(), refDims.data(), refSpaceIO.getHeader().getSForm(), myMethod, outVol);
}

AlgorithmVolumeWarpfieldResample::AlgorithmVolumeWarpfieldResample(ProgressObject* myProgObj, const VolumeFile* inVol, const VolumeFile* warpfield,
                                                                   const int64_t refDims[3], const vector<vector<float> >& refSform, const VolumeFile::InterpType& myMethod, VolumeFile* outVol) : AbstractAlgorithm(myProgObj)
{
    LevelProgress myProgress(myProgObj);
    vector<int64_t> warpDims;
    warpfield->getDimensions(warpDims);
    if (warpDims[3] != 3 || warpDims[4] != 1) throw AlgorithmException("provided warpfield volume has wrong number of subvolumes or components");
    vector<int64_t> outDims = inVol->getOriginalDimensions();
    if (outDims.size() < 3) throw AlgorithmException("input must have 3 spatial dimensions");
    outDims[0] = refDims[0];
    outDims[1] = refDims[1];
    outDims[2] = refDims[2];
    int64_t numMaps = inVol->getNumberOfMaps(), numComponents = inVol->getNumberOfComponents();
    outVol->reinitialize(outDims, refSform, numComponents, inVol->getType(), inVol->m_header);
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
                        Vector3D outCoord, inCoord, displacement;
                        outVol->indexToSpace(i, j, k, outCoord);
                        bool validDisplacement = false;
                        displacement[0] = warpfield->interpolateValue(outCoord, VolumeFile::TRILINEAR, &validDisplacement, 0);
                        if (validDisplacement)
                        {
                            displacement[1] = warpfield->interpolateValue(outCoord, VolumeFile::TRILINEAR, NULL, 1);
                            displacement[2] = warpfield->interpolateValue(outCoord, VolumeFile::TRILINEAR, NULL, 2);
                            inCoord = outCoord + displacement;
                            float interpVal = inVol->interpolateValue(inCoord, myMethod, NULL, b, c);
                            outVol->setValue(interpVal, i, j, k, b, c);
                        } else {
                            outVol->setValue(VolumeFile::INVALID_INTERP_VALUE, i, j, k, b, c);
                        }
                    }
                }
            }
            if (myMethod == VolumeFile::CUBIC)
            {
                inVol->freeSpline(b, c);//release memory we no longer need, if we allocated it
            }
        }
    }
}

float AlgorithmVolumeWarpfieldResample::getAlgorithmInternalWeight()
{
    return 1.0f;//override this if needed, if the progress bar isn't smooth
}

float AlgorithmVolumeWarpfieldResample::getSubAlgorithmWeight()
{
    //return AlgorithmInsertNameHere::getAlgorithmWeight();//if you use a subalgorithm
    return 0.0f;
}
