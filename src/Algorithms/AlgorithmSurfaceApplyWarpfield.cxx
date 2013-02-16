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

#include "AlgorithmSurfaceApplyWarpfield.h"
#include "AlgorithmException.h"

#include "SurfaceFile.h"
#include "Vector3D.h"
#include "WarpfieldFile.h"

using namespace caret;
using namespace std;

AString AlgorithmSurfaceApplyWarpfield::getCommandSwitch()
{
    return "-surface-apply-warpfield";
}

AString AlgorithmSurfaceApplyWarpfield::getShortDescription()
{
    return "APPLY WARPFIELD TO SURFACE FILE";
}

OperationParameters* AlgorithmSurfaceApplyWarpfield::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    ret->addSurfaceParameter(1, "in-surf", "the surface to transform");
    ret->addStringParameter(2, "warpfield", "the INVERSE warpfield");
    ret->addSurfaceOutputParameter(3, "out-surf", "the output transformed surface");
    OptionalParameter* fnirtOpt = ret->createOptionalParameter(4, "-fnirt", "MUST be used if using a fnirt warpfield");
    fnirtOpt->addStringParameter(1, "forward-warp", "the forward warpfield");
    ret->setHelpText(
        AString("NOTE: warping a surface requires the INVERSE of the warpfield used to warp the volume it lines up with.  ") +
        "The header of the forward warp is needed by the -fnirt option in order to correctly interpret the displacements in the fnirt warpfield.\n\n" +
        "If the -fnirt option is not present, the warpfield must be a nifti 'world' warpfield, which can be obtained with the -convert-warpfield command."
    );
    return ret;
}

void AlgorithmSurfaceApplyWarpfield::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    SurfaceFile* mySurf = myParams->getSurface(1);
    AString warpName = myParams->getString(2);
    SurfaceFile* mySurfOut = myParams->getOutputSurface(3);
    OptionalParameter* flirtOpt = myParams->getOptionalParameter(4);
    WarpfieldFile myWarp;
    if (flirtOpt->m_present)
    {
        myWarp.readFnirt(warpName, flirtOpt->getString(1));
    } else {
        myWarp.readWorld(warpName);
    }
    AlgorithmSurfaceApplyWarpfield(myProgObj, mySurf, myWarp.getWarpfield(), mySurfOut);
}

AlgorithmSurfaceApplyWarpfield::AlgorithmSurfaceApplyWarpfield(ProgressObject* myProgObj, const SurfaceFile* mySurf, const VolumeFile* warpfield, SurfaceFile* mySurfOut) : AbstractAlgorithm(myProgObj)
{
    LevelProgress myProgress(myProgObj);
    vector<int64_t> warpDims;
    warpfield->getDimensions(warpDims);
    if (warpDims[3] != 3 || warpDims[4] != 1) throw AlgorithmException("provided warpfield volume has wrong number of subvolumes or components");
    *mySurfOut = *mySurf;//copy rather than initialize, don't currently have much in the way of modification functions
    int numNodes = mySurf->getNumberOfNodes();
    const float* coordData = mySurf->getCoordinateData();
    vector<float> coordsOut(numNodes * 3);
    for (int i = 0; i < numNodes; ++i)
    {
        int base = i * 3;
        Vector3D sample = coordData + base;
        Vector3D displacement;
        bool valid = false;
        displacement[0] = warpfield->interpolateValue(sample, VolumeFile::TRILINEAR, &valid, 0);
        if (!valid) throw AlgorithmException("surface exceeds the bounding box of the warpfield");
        displacement[1] = warpfield->interpolateValue(sample, VolumeFile::TRILINEAR, NULL, 1);
        displacement[2] = warpfield->interpolateValue(sample, VolumeFile::TRILINEAR, NULL, 2);
        Vector3D newCoord = sample + displacement;
        coordsOut[base] = newCoord[0];
        coordsOut[base + 1] = newCoord[1];
        coordsOut[base + 2] = newCoord[2];
    }
    mySurfOut->setCoordinates(coordsOut.data(), numNodes);
}

float AlgorithmSurfaceApplyWarpfield::getAlgorithmInternalWeight()
{
    return 1.0f;//override this if needed, if the progress bar isn't smooth
}

float AlgorithmSurfaceApplyWarpfield::getSubAlgorithmWeight()
{
    //return AlgorithmInsertNameHere::getAlgorithmWeight();//if you use a subalgorithm
    return 0.0f;
}
