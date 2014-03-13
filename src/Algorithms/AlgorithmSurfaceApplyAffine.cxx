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

#include "AlgorithmSurfaceApplyAffine.h"
#include "AlgorithmException.h"

#include "AffineFile.h"
#include "SurfaceFile.h"
#include "Vector3D.h"

using namespace caret;
using namespace std;

AString AlgorithmSurfaceApplyAffine::getCommandSwitch()
{
    return "-surface-apply-affine";
}

AString AlgorithmSurfaceApplyAffine::getShortDescription()
{
    return "APPLY AFFINE TRANSFORM TO SURFACE FILE";
}

OperationParameters* AlgorithmSurfaceApplyAffine::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    ret->addSurfaceParameter(1, "in-surf", "the surface to transform");
    ret->addStringParameter(2, "affine", "the affine file");
    ret->addSurfaceOutputParameter(3, "out-surf", "the output transformed surface");
    OptionalParameter* flirtOpt = ret->createOptionalParameter(4, "-flirt", "MUST be used if affine is a flirt affine");
    flirtOpt->addStringParameter(1, "source-volume", "the source volume used when generating the affine");
    flirtOpt->addStringParameter(2, "target-volume", "the target volume used when generating the affine");
    ret->setHelpText(
        AString("For flirt matrices, you must use the -flirt option, because flirt matrices are not a complete description of the coordinate transform they represent.  ") +
        "If the -flirt option is not present, the affine must be a nifti 'world' affine, which can be obtained with the -convert-affine command, or aff_conv from the 4dfp suite."
    );
    return ret;
}

void AlgorithmSurfaceApplyAffine::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    SurfaceFile* mySurf = myParams->getSurface(1);
    AString affineName = myParams->getString(2);
    SurfaceFile* mySurfOut = myParams->getOutputSurface(3);
    OptionalParameter* flirtOpt = myParams->getOptionalParameter(4);
    AffineFile myAffine;
    if (flirtOpt->m_present)
    {
        myAffine.readFlirt(affineName, flirtOpt->getString(1), flirtOpt->getString(2));
    } else {
        myAffine.readWorld(affineName);
    }
    AlgorithmSurfaceApplyAffine(myProgObj, mySurf, myAffine.getMatrix(), mySurfOut);
}

AlgorithmSurfaceApplyAffine::AlgorithmSurfaceApplyAffine(ProgressObject* myProgObj, const SurfaceFile* mySurf, const FloatMatrix& myMatrix, SurfaceFile* mySurfOut) : AbstractAlgorithm(myProgObj)
{
    LevelProgress myProgress(myProgObj);
    *mySurfOut = *mySurf;//copy rather than initialize, don't currently have much in the way of modification functions
    Vector3D xvec, yvec, zvec, translate;
    myMatrix.getAffineVectors(xvec, yvec, zvec, translate);
    int numNodes = mySurf->getNumberOfNodes();
    const float* coordData = mySurf->getCoordinateData();
    vector<float> coordsOut(numNodes * 3);
    for (int i = 0; i < numNodes; ++i)
    {
        int base = i * 3;
        Vector3D transformed = coordData[base] * xvec + coordData[base + 1] * yvec + coordData[base + 2] * zvec + translate;
        coordsOut[base] = transformed[0];
        coordsOut[base + 1] = transformed[1];
        coordsOut[base + 2] = transformed[2];
    }
    mySurfOut->setCoordinates(coordsOut.data());
}

float AlgorithmSurfaceApplyAffine::getAlgorithmInternalWeight()
{
    return 1.0f;//override this if needed, if the progress bar isn't smooth
}

float AlgorithmSurfaceApplyAffine::getSubAlgorithmWeight()
{
    //return AlgorithmInsertNameHere::getAlgorithmWeight();//if you use a subalgorithm
    return 0.0f;
}
