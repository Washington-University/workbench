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
    
    ret->addMetricParameter(2, "vector-metric", "the input vectors");
    
    ret->addMetricParameter(3, "target-roi", "the roi to find the shortest path to");
    
    ret->addMetricOutputParameter(4, "metric-out", "the output metric");
    
    ret->createOptionalParameter(5, "-normalize", "normalize the input vectors");
    
    OptionalParameter* roiOpt = ret->createOptionalParameter(6, "-roi", "don't compute for vertices outside an roi");
    roiOpt->addMetricParameter(1, "roi-metric", "the region to compute inside, as a metric");
    
    ret->setHelpText(
        AString("At each vertex, compute the dot product between the input vector and the start of the shortest path to the ROI.  ") +
        "The number of columns of the input file must be a multiple of 3, each group of 3 is interpreted as a set of vectors.  " +
        "If -normalize is used, the input vectors are normalized before the dot product is taken."
    );
    return ret;
}

void AlgorithmMetricVectorTowardROI::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    SurfaceFile* mySurf = myParams->getSurface(1);
    MetricFile* myMetric = myParams->getMetric(2);
    MetricFile* targetRoi = myParams->getMetric(3);
    MetricFile* myMetricOut = myParams->getOutputMetric(4);
    bool normalize = myParams->getOptionalParameter(5)->m_present;
    MetricFile* computeRoi = NULL;
    OptionalParameter* roiOpt = myParams->getOptionalParameter(6);
    if (roiOpt->m_present)
    {
        computeRoi = roiOpt->getMetric(1);
    }
    AlgorithmMetricVectorTowardROI(myProgObj, mySurf, myMetric, targetRoi, myMetricOut, normalize, computeRoi);
}

AlgorithmMetricVectorTowardROI::AlgorithmMetricVectorTowardROI(ProgressObject* myProgObj, SurfaceFile* mySurf, const MetricFile* myMetric, const MetricFile* targetRoi,
                                                               MetricFile* myMetricOut, const bool& normalize, const MetricFile* computeRoi) : AbstractAlgorithm(myProgObj)
{
    LevelProgress myProgress(myProgObj);
    int numNodes = mySurf->getNumberOfNodes();
    if (myMetric->getNumberOfNodes() != numNodes) throw AlgorithmException("vector metric and surface have different number of vertices");
    if (targetRoi->getNumberOfNodes() != numNodes) throw AlgorithmException("target roi and surface have different number of vertices");
    if (computeRoi != NULL && computeRoi->getNumberOfNodes() != numNodes) throw AlgorithmException("compute roi and surface have different number of vertices");
    int inColumns = myMetric->getNumberOfColumns();
    int numVectors = inColumns / 3;
    if (inColumns % 3 != 0) throw AlgorithmException("input vector number of maps is not a multiple of 3");
    mySurf->computeNormals();
    const float* myNormals = mySurf->getNormalData();
    myMetricOut->setNumberOfNodesAndColumns(numNodes, numVectors);
    myMetricOut->setStructure(mySurf->getStructure());
    vector<char> roiConvert(numNodes, 0);
    const float* coordData = mySurf->getCoordinateData();
    const float* targetData = targetRoi->getValuePointerForColumn(0);
    bool haveTarget = false;
    vector<float> outCol(numNodes, 0.0f);
    for (int i = 0; i < numNodes; ++i)
    {
        if (targetData[i] > 0.0f)
        {
            roiConvert[i] = 1;
            haveTarget = true;
        }
    }
    if (!haveTarget) throw AlgorithmException("target roi is empty");
    for (int vec = 0; vec < numVectors; ++vec)
    {
        const float* xcol = myMetric->getValuePointerForColumn(vec * 3);
        const float* ycol = myMetric->getValuePointerForColumn(vec * 3 + 1);
        const float* zcol = myMetric->getValuePointerForColumn(vec * 3 + 2);
#pragma omp CARET_PAR
        {
            CaretPointer<GeodesicHelper> myGeoHelp = mySurf->getGeodesicHelper();
#pragma omp CARET_FOR schedule(dynamic)
            for (int i = 0; i < numNodes; ++i)
            {
                Vector3D normal = Vector3D(myNormals + i * 3).normal();//ensure unit length, just in case
                Vector3D inVec = Vector3D(xcol[i], ycol[i], zcol[i]);
                if (normalize) inVec = inVec.normal();
                vector<int32_t> pathNodes;
                vector<float> pathDists;
                myGeoHelp->getClosestNodeInRoi(i, roiConvert.data(), pathNodes, pathDists, true);//not using smooth will make noisier results, but using smooth can have curvature problems...
                if (pathNodes.size() < 2)//no path found, or node is inside the target roi
                {
                    outCol[i] = 0.0f;
                    continue;
                }
                Vector3D pathVec = Vector3D(coordData + pathNodes[1] * 3) - Vector3D(coordData + i * 3);
                Vector3D vecProject = inVec.length() * (inVec - inVec.dot(normal) * normal).normal();//project and restore original vector magnitude
                Vector3D pathProject = pathVec.length() * (pathVec - pathVec.dot(normal) * normal).normal();//ditto
                outCol[i] = vecProject.dot(pathProject);
            }
        }
        myMetricOut->setValuesForColumn(vec, outCol.data());
    }
}

float AlgorithmMetricVectorTowardROI::getAlgorithmInternalWeight()
{
    return 1.0f;//override this if needed, if the progress bar isn't smooth
}

float AlgorithmMetricVectorTowardROI::getSubAlgorithmWeight()
{
    return 0.0f;
}
