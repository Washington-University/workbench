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

#include "AlgorithmFiberDotProducts.h"
#include "AlgorithmException.h"
#include "CiftiFile.h"
#include "CaretPointLocator.h"
#include "MetricFile.h"
#include "SignedDistanceHelper.h"
#include "SurfaceFile.h"

#include <cmath>
#include <vector>

using namespace caret;
using namespace std;

AString AlgorithmFiberDotProducts::getCommandSwitch()
{
    return "-fiber-dot-products";
}

AString AlgorithmFiberDotProducts::getShortDescription()
{
    return "COMPUTE DOT PRODUCTS OF FIBER ORIENTATIONS WITH SURFACE NORMALS";
}

OperationParameters* AlgorithmFiberDotProducts::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    ret->addSurfaceParameter(1, "white-surf", "the white/gray boundary surface");
    
    ret->addCiftiParameter(2, "fiber-file", "the fiber orientation file");
    
    ret->addDoubleParameter(3, "max-dist", "the maximum distance from any surface vertex a fiber population may be, in mm");
    
    ret->addStringParameter(6, "direction", "test against surface for whether a fiber population should be used");
    
    ret->addMetricOutputParameter(4, "dot-metric", "the metric of dot products");
    
    ret->addMetricOutputParameter(5, "f-metric", "a metric of the f values of the fiber distributions");
    
    ret->setHelpText(
        AString("For each vertex, this command finds the closest fiber population that satisfies the <direction> test, ") +
        "and computes the absolute value of the dot product of the surface normal and the normalized mean direction of each fiber.  " +
        "The <direction> test must be one of INSIDE, OUTSIDE, or ANY, which causes the command to only use fiber populations " +
        "that are inside the surface, outside the surface, or to not care which direction it is from the surface.  " +
        "Each fiber population is output in a separate metric column."
    );
    return ret;
}

void AlgorithmFiberDotProducts::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    SurfaceFile* mySurf = myParams->getSurface(1);
    CiftiFile* myFibers = myParams->getCifti(2);
    float maxDist = (float)myParams->getDouble(3);
    MetricFile* myDotProdOut = myParams->getOutputMetric(4);
    MetricFile* myFSampOut = myParams->getOutputMetric(5);
    AString direction = myParams->getString(6);
    Direction myTest = INSIDE;
    if (direction == "INSIDE")
    {
        myTest = INSIDE;
    } else if (direction == "OUTSIDE") {
        myTest = OUTSIDE;
    } else if (direction == "ANY") {
        myTest = ANY;
    } else {
        throw AlgorithmException("unrecognized direction test");
    }
    AlgorithmFiberDotProducts(myProgObj, mySurf, myFibers, maxDist, myTest, myDotProdOut, myFSampOut);
}

AlgorithmFiberDotProducts::AlgorithmFiberDotProducts(ProgressObject* myProgObj, const SurfaceFile* mySurf, const CiftiFile* myFibers, const float& maxDist, const Direction& myTest, MetricFile* myDotProdOut, MetricFile* myFSampOut) : AbstractAlgorithm(myProgObj)
{
    LevelProgress myProgress(myProgObj);
    CaretPointer<SignedDistanceHelper> mySignedHelp = mySurf->getSignedDistanceHelper();
    int rowSize = myFibers->getNumberOfColumns();
    if ((rowSize - 3) % 7 != 0) throw AlgorithmException("input is not a fiber orientation file");
    int numFibers = (rowSize - 3) / 7;
    int64_t numRows = myFibers->getNumberOfRows();
    vector<float> coordsInside;
    vector<int64_t> coordIndices;
    vector<float> rowScratch(rowSize);
    for (int64_t i = 0; i < numRows; ++i)
    {
        myFibers->getRow(rowScratch.data(), i);
        int closeNode = mySurf->closestNode(rowScratch.data(), maxDist);
        if (closeNode == -1) continue;//skip samples that aren't close to a surface node, for speed (signed distance is kinda slow)
        if (myTest == ANY)
        {
            coordsInside.push_back(rowScratch[0]);//add point to vector for locator
            coordsInside.push_back(rowScratch[1]);
            coordsInside.push_back(rowScratch[2]);
            coordIndices.push_back(i);//and save its cifti index
        } else {
            float signedDist = mySignedHelp->dist(rowScratch.data(), SignedDistanceHelper::EVEN_ODD);//the first 3 floats are xyz coords
            if ((myTest == INSIDE) == (signedDist <= 0.0f))//test for inside/outside surface
            {
                coordsInside.push_back(rowScratch[0]);//add point to vector for locator
                coordsInside.push_back(rowScratch[1]);
                coordsInside.push_back(rowScratch[2]);
                coordIndices.push_back(i);//and save its cifti index
            }
        }
    }
    if (coordIndices.size() == 0) throw AlgorithmException("no fiber samples passed the <max-dist> and <direction> tests");
    CaretPointLocator myLocator(coordsInside.data(), coordIndices.size());//build the locator
    int numNodes = mySurf->getNumberOfNodes();
    myDotProdOut->setNumberOfNodesAndColumns(numNodes, numFibers);
    myFSampOut->setNumberOfNodesAndColumns(numNodes, numFibers);
    myDotProdOut->setStructure(mySurf->getStructure());
    myFSampOut->setStructure(mySurf->getStructure());
    for (int i = 0; i < numFibers; ++i)
    {
        myDotProdOut->setColumnName(i, "Fiber " + AString::number(i + 1) + " dot products");
        myFSampOut->setColumnName(i, "Fiber " + AString::number(i + 1) + " population mean f");
    }
    const float* coordData = mySurf->getCoordinateData();
    for (int i = 0; i < numNodes; ++i)
    {
        int closest = myLocator.closestPoint(coordData + i * 3);
        if (closest != -1)
        {
            myFibers->getRow(rowScratch.data(), coordIndices[closest]);
            Vector3D myNormal = mySurf->getNormalVector(i);
            for (int j = 0; j < numFibers; ++j)
            {
                int base = 3 + j * 7;
                float fmean = rowScratch[base];
                float theta = rowScratch[base + 2];
                float phi = rowScratch[base + 3];
                Vector3D direction;
                direction[0] = -sin(theta) * cos(phi);//NOTE: theta, phi are polar coordinates for a RADIOLOGICAL coordinate system, so flip x so that +x = right
                direction[1] = sin(theta) * sin(phi);
                direction[2] = cos(theta);
                float dotProd = abs(myNormal.dot(direction));
                myDotProdOut->setValue(i, j, dotProd);
                myFSampOut->setValue(i, j, fmean);
            }
        } else {
            for (int j = 0; j < numFibers; ++j)
            {
                myDotProdOut->setValue(i, j, 0.0f);
                myFSampOut->setValue(i, j, 0.0f);
            }
        }
    }
}

float AlgorithmFiberDotProducts::getAlgorithmInternalWeight()
{
    return 1.0f;//override this if needed, if the progress bar isn't smooth
}

float AlgorithmFiberDotProducts::getSubAlgorithmWeight()
{
    //return AlgorithmInsertNameHere::getAlgorithmWeight();//if you use a subalgorithm
    return 0.0f;
}
