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

#include "AlgorithmBorderResample.h"
#include "AlgorithmException.h"

#include "Border.h"
#include "BorderFile.h"
#include "GiftiLabelTable.h"
#include "SurfaceFile.h"
#include "SurfaceProjectedItem.h"
#include "SurfaceProjectionBarycentric.h"
#include "SignedDistanceHelper.h"

using namespace caret;
using namespace std;

AString AlgorithmBorderResample::getCommandSwitch()
{
    return "-border-resample";
}

AString AlgorithmBorderResample::getShortDescription()
{
    return "RESAMPLE A BORDER FILE TO A DIFFERENT MESH";
}

OperationParameters* AlgorithmBorderResample::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    
    ret->addBorderParameter(1, "border-in", "the border file to resample");
    
    ret->addSurfaceParameter(2, "current-sphere", "a sphere surface with the mesh that the metric is currently on");
    
    ret->addSurfaceParameter(3, "new-sphere", "a sphere surface that is in register with <current-sphere> and has the desired output mesh");
    
    ret->addBorderOutputParameter(4, "border-out", "the output border file");
    
    ret->setHelpText(
        AString("Resamples a border file, given two spherical surfaces that are in register.  ")
    );
    return ret;
}

void AlgorithmBorderResample::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    BorderFile* borderIn = myParams->getBorder(1);
    SurfaceFile* curSphere = myParams->getSurface(2);
    SurfaceFile* newSphere = myParams->getSurface(3);
    BorderFile* borderOut = myParams->getOutputBorder(4);
    AlgorithmBorderResample(myProgObj, borderIn, curSphere, newSphere, borderOut);
}

AlgorithmBorderResample::AlgorithmBorderResample(ProgressObject* myProgObj, const BorderFile* borderIn, const SurfaceFile* curSphere, const SurfaceFile* newSphere, BorderFile* borderOut) : AbstractAlgorithm(myProgObj)
{
    LevelProgress myProgress(myProgObj);//TODO: error checking once we decide how new border files work
    *(borderOut->getClassColorTable()) = *(borderIn->getClassColorTable());
    *(borderOut->getNameColorTable()) = *(borderIn->getNameColorTable());
    borderOut->setStructure(newSphere->getStructure());
    //borderOut->setSurfaceNumberOfNodes(newSphere->getSurfaceNumberOfNodes());//TODO
    int numBorders = borderIn->getNumberOfBorders();
    CaretPointer<SignedDistanceHelper> myHelp = newSphere->getSignedDistanceHelper();
    for (int i = 0; i < numBorders; ++i)
    {
        const Border* inputBorder = borderIn->getBorder(i);
        CaretPointer<Border> outputBorder(new Border());//in case something throws
        outputBorder->setName(inputBorder->getName());
        outputBorder->setClassName(inputBorder->getClassName());
        int numPoints = inputBorder->getNumberOfPoints();
        for (int j = 0; j < numPoints; ++j)
        {
            CaretPointer<SurfaceProjectedItem> outPoint(new SurfaceProjectedItem());//ditto
            float coord[3];
            const SurfaceProjectedItem* myItem = inputBorder->getPoint(j);
            if (!myItem->getBarycentricProjection()->isValid()) throw AlgorithmException("input file has a border point without barycentric projection");//because we never want to use van essen projection or straight coords
            bool valid = myItem->getBarycentricProjection()->unprojectToSurface(*curSphere, coord, 0.0f, true);//should really be "from" surface - "true" makes it not use the signed distance above surface, if present
            if (!valid) throw AlgorithmException("input file has a border point that is invalid for the current sphere");
            BarycentricInfo myBaryInfo;
            myHelp->barycentricWeights(coord, myBaryInfo);
            outPoint->setStructure(newSphere->getStructure());
            outPoint->getBarycentricProjection()->setTriangleNodes(myBaryInfo.nodes);
            outPoint->getBarycentricProjection()->setTriangleAreas(myBaryInfo.baryWeights);
            outPoint->getBarycentricProjection()->setProjectionSurfaceNumberOfNodes(newSphere->getNumberOfNodes());
            outPoint->getBarycentricProjection()->setValid(true);
            outputBorder->addPoint(outPoint.releasePointer());//NOTE: addPoint currently takes ownership of a RAW POINTER - shared_ptr won't release the pointer, so this function would need to be deprecated
        }
        borderOut->addBorder(outputBorder.releasePointer());//NOTE: again, ownership of RAW POINTER
    }
}

float AlgorithmBorderResample::getAlgorithmInternalWeight()
{
    return 1.0f;//override this if needed, if the progress bar isn't smooth
}

float AlgorithmBorderResample::getSubAlgorithmWeight()
{
    //return AlgorithmInsertNameHere::getAlgorithmWeight();//if you use a subalgorithm
    return 0.0f;
}
