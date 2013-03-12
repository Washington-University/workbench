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

#include "AlgorithmSurfaceModifySphere.h"
#include "AlgorithmException.h"

#include "BoundingBox.h"
#include "CaretLogger.h"
#include "SurfaceFile.h"
#include "Vector3D.h"

using namespace caret;
using namespace std;

AString AlgorithmSurfaceModifySphere::getCommandSwitch()
{
    return "-surface-modify-sphere";
}

AString AlgorithmSurfaceModifySphere::getShortDescription()
{
    return "CHANGE RADIUS AND OPTIONALLY RECENTER A SPHERE";
}

OperationParameters* AlgorithmSurfaceModifySphere::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    ret->addSurfaceParameter(1, "sphere-in", "the sphere to modify");
    
    ret->addDoubleParameter(2, "radius", "the radius the output sphere should have");
    
    ret->addSurfaceOutputParameter(3, "sphere-out", "the output sphere");
    
    ret->createOptionalParameter(4, "-recenter", "recenter the sphere by means of the bounding box");
    
    //TODO: add option to supress warnings?
    ret->setHelpText(
        AString("This command may be useful if you have used -surface-resample to resample a sphere, which can suffer from problems generally not present in ") +
        "-surface-sphere-project-unproject.  If the sphere should already be centered around the origin, using -recenter may still shift it slightly before changing the radius, " +
        "which is likely to be undesireable.\n\n" +
        "If <sphere-in> is not close to spherical, or not centered around the origin and -recenter is not used, a warning is printed."
    );
    return ret;
}

void AlgorithmSurfaceModifySphere::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    SurfaceFile* mySphere = myParams->getSurface(1);
    float newRadius = (float)myParams->getDouble(2);
    SurfaceFile* outSphere = myParams->getOutputSurface(3);
    bool recenter = myParams->getOptionalParameter(4)->m_present;
    AlgorithmSurfaceModifySphere(myProgObj, mySphere, newRadius, outSphere, recenter);
}

AlgorithmSurfaceModifySphere::AlgorithmSurfaceModifySphere(ProgressObject* myProgObj, const SurfaceFile* mySphere, const float& newRadius, SurfaceFile* outSphere, const bool& recenter) : AbstractAlgorithm(myProgObj)
{
    LevelProgress myProgress(myProgObj);
    Vector3D center;//initializes to the origin
    if (recenter)
    {
        const BoundingBox* mybb = mySphere->getBoundingBox();
        mybb->getCenter(center);
    }
    int numNodes = mySphere->getNumberOfNodes();
    CaretAssert(numNodes > 1);
    int numNodes3 = numNodes * 3;
    const float* coordData = mySphere->getCoordinateData();
    float mindist, maxdist;
    bool first = true;
    const float TOLERANCE = 1.05f;//5% is a very loose tolerance, we expect some odd spheres
    *outSphere = *mySphere;
    vector<float> outCoords(numNodes3);
    for (int i = 0; i < numNodes3; i += 3)
    {
        Vector3D recenterCoord = Vector3D(coordData + i) - center;
        float tempf;
        Vector3D outCoord = recenterCoord.normal(&tempf) * newRadius;
        outCoords[i] = outCoord[0];
        outCoords[i + 1] = outCoord[1];
        outCoords[i + 2] = outCoord[2];
        if (tempf != tempf) throw CaretException("found NaN coordinate in the input sphere");
        if (first)
        {
            first = false;
            mindist = tempf;
            maxdist = tempf;
        } else {
            if (tempf < mindist)
            {
                mindist = tempf;
            }
            if (tempf > maxdist)
            {
                maxdist = tempf;
            }
        }
    }
    if (mindist * TOLERANCE < maxdist)
    {
        if (recenter)
        {
            CaretLogWarning("input sphere is unusually irregular, inspect the input");
        } else {
            CaretLogWarning("input sphere is either unusually irregular or not centered, inspect the input");
        }
    }
    outSphere->setCoordinates(outCoords.data(), numNodes);
}

float AlgorithmSurfaceModifySphere::getAlgorithmInternalWeight()
{
    return 1.0f;//override this if needed, if the progress bar isn't smooth
}

float AlgorithmSurfaceModifySphere::getSubAlgorithmWeight()
{
    //return AlgorithmInsertNameHere::getAlgorithmWeight();//if you use a subalgorithm
    return 0.0f;
}
