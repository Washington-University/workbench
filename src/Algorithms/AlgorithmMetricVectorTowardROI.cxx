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

#include "AlgorithmMetricVectorTowardROI.h"
#include "AlgorithmException.h"

#include "CaretOMP.h"
#include "GeodesicHelper.h"
#include "MetricFile.h"
#include "SurfaceFile.h"
#include "Vector3D.h"

#include <cmath>

using namespace caret;
using namespace std;

AString AlgorithmMetricVectorTowardROI::getCommandSwitch()
{
    return "-metric-vector-toward-roi";
}

AString AlgorithmMetricVectorTowardROI::getShortDescription()
{
    return "FIND IF VECTORS POINT TOWARD AN ROI";
}

OperationParameters* AlgorithmMetricVectorTowardROI::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    
    ret->addSurfaceParameter(1, "surface", "the surface to compute on");
    
    ret->addMetricParameter(2, "target-roi", "the roi to find the shortest path to");
    
    ret->addMetricOutputParameter(3, "metric-out", "the output metric");
    
    OptionalParameter* roiOpt = ret->createOptionalParameter(4, "-roi", "don't compute for vertices outside an roi");
    roiOpt->addMetricParameter(1, "roi-metric", "the region to compute inside, as a metric");
    
    ret->setHelpText(
        AString("At each vertex, compute the vector along the start of the shortest path to the ROI.")
    );
    return ret;
}

void AlgorithmMetricVectorTowardROI::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    SurfaceFile* mySurf = myParams->getSurface(1);
    MetricFile* targetRoi = myParams->getMetric(2);
    MetricFile* myMetricOut = myParams->getOutputMetric(3);
    MetricFile* computeRoi = NULL;
    OptionalParameter* roiOpt = myParams->getOptionalParameter(4);
    if (roiOpt->m_present)
    {
        computeRoi = roiOpt->getMetric(1);
    }
    AlgorithmMetricVectorTowardROI(myProgObj, mySurf, targetRoi, myMetricOut, computeRoi);
}

AlgorithmMetricVectorTowardROI::AlgorithmMetricVectorTowardROI(ProgressObject* myProgObj, SurfaceFile* mySurf, const MetricFile* targetRoi,
                                                               MetricFile* myMetricOut, const MetricFile* computeRoi) : AbstractAlgorithm(myProgObj)
{
    LevelProgress myProgress(myProgObj);
    int numNodes = mySurf->getNumberOfNodes();
    if (targetRoi->getNumberOfNodes() != numNodes) throw AlgorithmException("target roi and surface have different number of vertices");
    const float* computeRoiCol = NULL;
    if (computeRoi != NULL)
    {
        if (computeRoi->getNumberOfNodes() != numNodes) throw AlgorithmException("compute roi and surface have different number of vertices");
        computeRoiCol = computeRoi->getValuePointerForColumn(0);
    }
    mySurf->computeNormals();
    const float* myNormals = mySurf->getNormalData();
    myMetricOut->setNumberOfNodesAndColumns(numNodes, 3);
    myMetricOut->setStructure(mySurf->getStructure());
    vector<char> roiConvert(numNodes, 0);
    const float* coordData = mySurf->getCoordinateData();
    const float* targetData = targetRoi->getValuePointerForColumn(0);
    bool haveTarget = false;
    vector<float> outCol[3];
    outCol[0].resize(numNodes, 0.0f);
    outCol[1].resize(numNodes, 0.0f);
    outCol[2].resize(numNodes, 0.0f);
    for (int i = 0; i < numNodes; ++i)
    {
        if (targetData[i] > 0.0f)
        {
            roiConvert[i] = 1;
            haveTarget = true;
        }
    }
    if (!haveTarget) throw AlgorithmException("target roi is empty");
#pragma omp CARET_PAR
    {
        CaretPointer<GeodesicHelper> myGeoHelp = mySurf->getGeodesicHelper();
#pragma omp CARET_FOR schedule(dynamic)
        for (int i = 0; i < numNodes; ++i)
        {
            if (computeRoiCol == NULL || computeRoiCol[i] > 0.0f)
            {
                Vector3D normal = Vector3D(myNormals + i * 3).normal();//ensure unit length, just in case
                vector<int32_t> pathNodes;
                vector<float> pathDists;
                myGeoHelp->getClosestNodeInRoi(i, roiConvert.data(), pathNodes, pathDists, true);//not using smooth will make noisier results, project via normal to avoid curvature problems
                if (pathNodes.size() < 2)//no path found, or node is inside the target roi
                {
                    continue;//will already be zero
                }
                Vector3D pathVec = Vector3D(coordData + pathNodes[1] * 3) - Vector3D(coordData + i * 3);
                Vector3D pathProject = (pathVec - pathVec.dot(normal) * normal).normal();//the path vector is a direction only, just normalize after projection
                outCol[0][i] = pathProject[0];
                outCol[1][i] = pathProject[1];
                outCol[2][i] = pathProject[2];
            }
        }
    }
    myMetricOut->setValuesForColumn(0, outCol[0].data());
    myMetricOut->setValuesForColumn(1, outCol[1].data());
    myMetricOut->setValuesForColumn(2, outCol[2].data());
}

float AlgorithmMetricVectorTowardROI::getAlgorithmInternalWeight()
{
    return 1.0f;//override this if needed, if the progress bar isn't smooth
}

float AlgorithmMetricVectorTowardROI::getSubAlgorithmWeight()
{
    return 0.0f;
}
