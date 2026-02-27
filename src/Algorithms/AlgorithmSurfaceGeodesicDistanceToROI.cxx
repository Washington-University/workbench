/*LICENSE_START*/
/*
 *  Copyright (C) 2026  Washington University School of Medicine
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

#include "AlgorithmSurfaceGeodesicDistanceToROI.h"
#include "AlgorithmException.h"

#include "GeodesicHelper.h"
#include "MetricFile.h"
#include "SurfaceFile.h"

using namespace caret;
using namespace std;

AString AlgorithmSurfaceGeodesicDistanceToROI::getCommandSwitch()
{
    return "-surface-geodesic-distance-to-roi";
}

AString AlgorithmSurfaceGeodesicDistanceToROI::getShortDescription()
{
    return "GEODESIC DISTANCE TO ROI";
}

OperationParameters* AlgorithmSurfaceGeodesicDistanceToROI::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    
    ret->addSurfaceParameter(1, "surface", "the surface to compute on");
    
    ret->addMetricParameter(2, "roi", "the ROI(s), as a metric file");
    
    ret->addMetricOutputParameter(3, "metric-out", "the output metric");
    
    OptionalParameter* columnOpt = ret->createOptionalParameter(4, "-column", "select a single ROI column");
    columnOpt->addStringParameter(1, "column", "the column number or name");
    
    ret->createOptionalParameter(5, "-negatives-inside", "also compute negative distances from the inverted ROI, instead of putting zeros in the ROI - NOTE: this is not a true signed distance");
    
    ret->createOptionalParameter(6, "-naive", "use only neighbors, don't crawl triangles (not recommended)");
    
    ret->setHelpText(
        AString("For each vertex, find the shortest geodesic distance to a vertex in the ROI.  ") +
        "Each map of <roi> is treated as a single binary class, even if it is discontiguous.  " +
        "ROIs that should have distances measured separately should be separate maps.  "
    );
    return ret;
}

void AlgorithmSurfaceGeodesicDistanceToROI::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    SurfaceFile* mySurf = myParams->getSurface(1);
    MetricFile* myROI = myParams->getMetric(2);
    MetricFile* myMetricOut = myParams->getOutputMetric(3);
    OptionalParameter* columnOpt = myParams->getOptionalParameter(4);
    int column = -1;
    if (columnOpt->m_present)
    {
        column = myROI->getMapIndexFromNameOrNumber(columnOpt->getString(1));
        if (column < 0) throw AlgorithmException("invalid column specified");
    }
    bool negatives = myParams->getOptionalParameter(5)->m_present;
    bool naive = myParams->getOptionalParameter(6)->m_present;
    AlgorithmSurfaceGeodesicDistanceToROI(myProgObj, mySurf, myROI, myMetricOut, column, negatives, naive);
}

namespace
{
    vector<float> computeDist(GeodesicHelper* myHelp, const int32_t numNodes, const float* roiData, const bool negatives, const bool naive)
    {
        set<int32_t> in, out;
        for (int32_t v = 0; v < numNodes; ++v)
        {
            if (roiData[v] > 0.0f)
            {
                in.insert(v);
            } else {
                if (negatives) //avoid building the inverse ROI if we won't use it
                {
                    out.insert(v);
                }
            }
        }
        vector<float> ret;
        myHelp->getGeoFromNodeList(in, ret, NULL, !naive);
        if (negatives)
        {
            vector<float> negData;
            myHelp->getGeoFromNodeList(out, negData, NULL, !naive);
            for (auto v : in)
            {
                ret[v] = -negData[v];
            }
        }
        return ret;
    }
}

AlgorithmSurfaceGeodesicDistanceToROI::AlgorithmSurfaceGeodesicDistanceToROI(ProgressObject* myProgObj, const SurfaceFile* mySurf, const MetricFile* myROI, MetricFile* myMetricOut,
                                                                             const int32_t column, const bool negatives, const bool naive) : AbstractAlgorithm(myProgObj)
{
    LevelProgress myProgress(myProgObj);
    int32_t numNodes = mySurf->getNumberOfNodes();
    if (myROI->getNumberOfNodes() != numNodes) throw AlgorithmException("surface and roi metric have different number of vertices");
    int32_t numROIs = myROI->getNumberOfColumns();
    if (column < -1 || column >= numROIs) throw AlgorithmException("invalid column number");
    CaretPointer<GeodesicHelper> myHelp = mySurf->getGeodesicHelper();
    myMetricOut->setStructure(mySurf->getStructure());
    if (column == -1)
    {
        myMetricOut->setNumberOfNodesAndColumns(numNodes, numROIs);
        for (int32_t i = 0; i < numROIs; ++i)
        {
            vector<float> distData = computeDist(myHelp, numNodes, myROI->getValuePointerForColumn(i), negatives, naive);
            myMetricOut->setValuesForColumn(i, distData.data());
            myMetricOut->setMapName(i, myROI->getMapName(i));
        }
    } else {
        myMetricOut->setNumberOfNodesAndColumns(numNodes, 1);
        vector<float> distData = computeDist(myHelp, numNodes, myROI->getValuePointerForColumn(column), negatives, naive);
        myMetricOut->setValuesForColumn(0, distData.data());
        myMetricOut->setMapName(0, myROI->getMapName(column));
    }
}

float AlgorithmSurfaceGeodesicDistanceToROI::getAlgorithmInternalWeight()
{
    return 1.0f;//override this if needed, if the progress bar isn't smooth
}

float AlgorithmSurfaceGeodesicDistanceToROI::getSubAlgorithmWeight()
{
    //return AlgorithmInsertNameHere::getAlgorithmWeight();//if you use a subalgorithm
    return 0.0f;
}
