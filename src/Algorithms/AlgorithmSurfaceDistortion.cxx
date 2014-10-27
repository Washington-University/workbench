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

#include "AlgorithmSurfaceDistortion.h"
#include "AlgorithmException.h"

#include "AlgorithmMetricSmoothing.h"
#include "MathFunctions.h"
#include "MetricFile.h"
#include "SurfaceFile.h"
#include "TopologyHelper.h"
#include "Vector3D.h"

#include <cmath>
#include <vector>

using namespace caret;
using namespace std;

AString AlgorithmSurfaceDistortion::getCommandSwitch()
{
    return "-surface-distortion";
}

AString AlgorithmSurfaceDistortion::getShortDescription()
{
    return "MEASURE DISTORTION BETWEEN SURFACES";
}

OperationParameters* AlgorithmSurfaceDistortion::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    ret->addSurfaceParameter(1, "surface-reference", "the reference surface");
    
    ret->addSurfaceParameter(2, "surface-distorted", "the distorted surface");
    
    ret->addMetricOutputParameter(3, "metric-out", "the output distortion metric");
    
    OptionalParameter* smoothOpt = ret->createOptionalParameter(4, "-smooth", "smooth the area data");
    smoothOpt->addDoubleParameter(1, "sigma", "the smoothing kernel sigma in mm");
    
    ret->createOptionalParameter(5, "-caret5-method", "use the surface distortion method from caret5");
    
    ret->createOptionalParameter(6, "-edge-method", "calculate distortion of edge lengths rather than areas");
    
    ret->setHelpText(
        AString("This command, when not using -caret5-method or -edge-method, is equivalent to using -surface-vertex-areas on each surface, ") +
        "smoothing both output metrics with the GEO_GAUSS_EQUAL method on the surface they came from if -smooth is specified, and then using the formula " +
        "'ln(distorted/reference)/ln(2)' on the smoothed results.\n\n" +
        "When using -caret5-method, it uses the surface distortion method from caret5, which takes the base 2 log of the ratio of tile areas, " +
        "then averages those results at each vertex, and then smooths the result on the reference surface.\n\n" +
        "When using -edge-method, the -smooth option is ignored, and the output at each vertex is the average of 'abs(ln(refEdge/distortEdge)/ln(2))' over all edges " +
        "connected to the vertex."
    );
    return ret;
}

void AlgorithmSurfaceDistortion::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    SurfaceFile* referenceSurf = myParams->getSurface(1);
    SurfaceFile* distortedSurf = myParams->getSurface(2);
    MetricFile* myMetricOut = myParams->getOutputMetric(3);
    float smooth = -1.0f;
    OptionalParameter* smoothOpt = myParams->getOptionalParameter(4);
    if (smoothOpt->m_present)
    {
        smooth = (float)smoothOpt->getDouble(1);
        if (smooth <= 0.0f) throw AlgorithmException("smoothing kernel must be positive if specified");
    }
    bool caret5method = myParams->getOptionalParameter(5)->m_present;
    bool edgeMethod = myParams->getOptionalParameter(6)->m_present;
    if (caret5method && edgeMethod) throw AlgorithmException("you may not specify both -caret5-method and -edge-method");
    AlgorithmSurfaceDistortion(myProgObj, referenceSurf, distortedSurf, myMetricOut, smooth, caret5method, edgeMethod);
}

AlgorithmSurfaceDistortion::AlgorithmSurfaceDistortion(ProgressObject* myProgObj, const SurfaceFile* referenceSurf, const SurfaceFile* distortedSurf,
                                                       MetricFile* myMetricOut, const float& smooth, const bool& caret5method, const bool& edgeMethod) : AbstractAlgorithm(myProgObj)
{
    if (caret5method && edgeMethod) throw AlgorithmException("you may not use both caret5 and edge method flags");
    ProgressObject* smoothRef = NULL, *smoothDistort = NULL, *caret5Smooth = NULL;//uncomment these if you use another algorithm inside here
    if (myProgObj != NULL)
    {
        if (smooth > 0.0f && !edgeMethod)
        {
            if (caret5method)
            {
                caret5Smooth = myProgObj->addAlgorithm(AlgorithmMetricSmoothing::getAlgorithmWeight());
            } else {
                smoothRef = myProgObj->addAlgorithm(AlgorithmMetricSmoothing::getAlgorithmWeight());
                smoothDistort = myProgObj->addAlgorithm(AlgorithmMetricSmoothing::getAlgorithmWeight());
            }
        }
    }
    LevelProgress myProgress(myProgObj);
    if (!referenceSurf->hasNodeCorrespondence(*distortedSurf)) throw AlgorithmException("input surfaces must have node correspondence");
    int numNodes = referenceSurf->getNumberOfNodes();
    myMetricOut->setNumberOfNodesAndColumns(numNodes, 1);
    myMetricOut->setStructure(referenceSurf->getStructure());
    if (caret5method)
    {
        myMetricOut->setColumnName(0, "area distortion (caret5)");
        int numTiles = referenceSurf->getNumberOfTriangles();
        vector<float> tilescratch(numTiles), nodescratch(numNodes);
        const float* refCoords = referenceSurf->getCoordinateData(), *distortCoords = distortedSurf->getCoordinateData();
        for (int i = 0; i < numTiles; ++i)
        {
            const int32_t* myTile = referenceSurf->getTriangle(i);
            int32_t offset[3] = { myTile[0] * 3, myTile[1] * 3, myTile[2] * 3 };
            float refarea = MathFunctions::triangleArea(refCoords + offset[0], refCoords + offset[1], refCoords + offset[2]);
            float distortarea = MathFunctions::triangleArea(distortCoords + offset[0], distortCoords + offset[1], distortCoords + offset[2]);
            if (refarea == 0.0f)
            {
                if (distortarea == 0.0f)
                {
                    tilescratch[i] = 0.0f;
                } else {
                    tilescratch[i] = log(10000.0) / log(2.0);//thats what it says
                }
            } else {
                if (distortarea / refarea < 0.00000001)
                {
                    tilescratch[i] = log(0.00000001) / log(2.0);
                } else {
                    tilescratch[i] = log(distortarea / refarea) / log(2.0);
                }
            }
        }
        CaretPointer<TopologyHelper> myhelp = referenceSurf->getTopologyHelper();
        for (int i = 0; i < numNodes; ++i)
        {
            const vector<int32_t>& myTiles = myhelp->getNodeTiles(i);
            int tileCount = (int)myTiles.size();
            double accum = 0.0;
            for (int j = 0; j < tileCount; ++j)
            {
                accum += tilescratch[myTiles[j]];
            }
            if (tileCount == 0)
            {
                nodescratch[i] = 0.0f;//actually, caret5 uses 1 here, but that is wrong, so fix it
            } else {
                nodescratch[i] = accum / tileCount;
            }
        }
        if (smooth > 0.0f)
        {
            MetricFile tempResult;
            tempResult.setNumberOfNodesAndColumns(numNodes, 1);
            tempResult.setValuesForColumn(0, nodescratch.data());//TSC: not sure what the "best" smoothing method here is, but there isn't a "correct" one because this method has flaws
            AlgorithmMetricSmoothing(caret5Smooth, referenceSurf, &tempResult, smooth, myMetricOut, NULL, false, false, -1, NULL, MetricSmoothingObject::GEO_GAUSS_AREA);
            myMetricOut->setStructure(referenceSurf->getStructure());//just in case we change where metric smoothing gets structure from
            myMetricOut->setColumnName(0, "area distortion (caret5)");
        } else {
            myMetricOut->setValuesForColumn(0, nodescratch.data());
        }
    } else if (edgeMethod) {
        myMetricOut->setColumnName(0, "edge distortion");
        CaretPointer<TopologyHelper> myhelp = referenceSurf->getTopologyHelper();
        const float* refCoords = referenceSurf->getCoordinateData(), *distortCoords = distortedSurf->getCoordinateData();
        for (int i = 0; i < numNodes; ++i)
        {
            Vector3D refCenter = refCoords + i * 3;
            Vector3D distortCenter = distortCoords + i * 3;
            const vector<int32_t>& neighbors = myhelp->getNodeNeighbors(i);
            int numNeigh = (int)neighbors.size();
            float accum = 0.0f;
            for (int j = 0; j < numNeigh; ++j)
            {
                Vector3D refNeigh = refCoords + neighbors[j] * 3;
                Vector3D distortNeigh = distortCoords + neighbors[j] * 3;
                float refLength = (refNeigh - refCenter).length();
                float distortLength = (distortNeigh - distortCenter).length();
                float ratio;
                if (refLength < distortLength)
                {
                    ratio = distortLength / refLength;
                } else {
                    ratio = refLength / distortLength;
                }
                accum += log(ratio) / log(2.0f);
            }
            myMetricOut->setValue(i, 0, accum / numNeigh);
        }
    } else {
        myMetricOut->setColumnName(0, "area distortion");
        MetricFile refAreas, distortAreas;
        refAreas.setNumberOfNodesAndColumns(numNodes, 1);
        distortAreas.setNumberOfNodesAndColumns(numNodes, 1);
        vector<float> scratch;
        referenceSurf->computeNodeAreas(scratch);
        refAreas.setValuesForColumn(0, scratch.data());
        distortedSurf->computeNodeAreas(scratch);
        distortAreas.setValuesForColumn(0, scratch.data());
        MetricFile refSmoothed, distortSmoothed;
        const float* refData = refAreas.getValuePointerForColumn(0), *distortData = distortAreas.getValuePointerForColumn(0);
        if (smooth > 0.0f)
        {
            AlgorithmMetricSmoothing(smoothRef, referenceSurf, &refAreas, smooth, &refSmoothed, NULL, false, false, -1, NULL, MetricSmoothingObject::GEO_GAUSS_EQUAL);
            AlgorithmMetricSmoothing(smoothDistort, distortedSurf, &distortAreas, smooth, &distortSmoothed, NULL, false, false, -1, NULL, MetricSmoothingObject::GEO_GAUSS_EQUAL);
            refData = refSmoothed.getValuePointerForColumn(0);
            distortData = distortSmoothed.getValuePointerForColumn(0);
        }
        for (int i = 0; i < numNodes; ++i)
        {
            scratch[i] = log(distortData[i] / refData[i]) / log(2.0f);//should we sanity check these?
        }
        myMetricOut->setValuesForColumn(0, scratch.data());
    }
}

float AlgorithmSurfaceDistortion::getAlgorithmInternalWeight()
{
    return 1.0f;//override this if needed, if the progress bar isn't smooth
}

float AlgorithmSurfaceDistortion::getSubAlgorithmWeight()
{
    //return AlgorithmInsertNameHere::getAlgorithmWeight();//if you use a subalgorithm
    return 0.0f;
}
