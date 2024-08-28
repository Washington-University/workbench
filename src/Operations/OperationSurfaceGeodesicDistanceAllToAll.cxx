/*LICENSE_START*/
/*
 *  Copyright (C) 2018  Washington University School of Medicine
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

#include "OperationSurfaceGeodesicDistanceAllToAll.h"
#include "OperationException.h"

#include "CaretOMP.h"
#include "CiftiFile.h"
#include "GeodesicHelper.h"
#include "MetricFile.h"
#include "SurfaceFile.h"

using namespace caret;
using namespace std;

AString OperationSurfaceGeodesicDistanceAllToAll::getCommandSwitch()
{
    return "-surface-geodesic-distance-all-to-all";
}

AString OperationSurfaceGeodesicDistanceAllToAll::getShortDescription()
{
    return "COMPUTE GEODESIC DISTANCES FROM ALL VERTICES";
}

OperationParameters* OperationSurfaceGeodesicDistanceAllToAll::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    
    ret->addSurfaceParameter(1, "surface", "the surface to compute on");
    
    ret->addCiftiOutputParameter(2, "cifti-out", "single-hemisphere dconn containing the distances");
    
    OptionalParameter* roiOpt = ret->createOptionalParameter(3, "-roi", "only output distances for vertices inside an ROI");
    roiOpt->addMetricParameter(1, "roi-metric", "the ROI as a metric file");
    
    OptionalParameter* limitOpt = ret->createOptionalParameter(4, "-limit", "stop at a specified distance");
    limitOpt->addDoubleParameter(1, "limit-mm", "distance in mm to stop at");
    
    OptionalParameter* corrAreaOpt = ret->createOptionalParameter(5, "-corrected-areas", "vertex areas to use to correct the distances on a group-average surface");
    corrAreaOpt->addMetricParameter(1, "area-metric", "the corrected vertex areas, as a metric");

    ret->createOptionalParameter(6, "-naive", "use only neighbors, don't crawl triangles (not recommended)");

    ret->setHelpText(
        AString("Computes geodesic distance from every vertex to every vertex, outputting a single-hemisphere dconn file.  ") +
        "If you are only interested in a few vertices, see -surface-geodesic-distance.  " +
        "When -limit is specified, any vertex beyond the limit is assigned the value -1.\n\n" +
        "The -roi option makes the output file smaller by not outputting distances to or from vertices outside the ROI, but paths are still allowed to go outside the ROI when finding distances to other vertices.\n\n" +
        "The -corrected-areas option should be used when the input is a group average surface - group average surfaces have " +
        "significantly less surface area than individual surfaces do, and therefore distances measured on them would be smaller than measuring them on individual surfaces.  " +
        "In this case, the input to this option should be a group average of the output of -surface-vertex-areas for each subject.\n\n" +
        "If -naive is not specified, the algorithm uses not just immediate neighbors, but also neighbors derived from crawling across pairs of triangles that share an edge."
    );
    return ret;
}

void OperationSurfaceGeodesicDistanceAllToAll::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    LevelProgress myProgress(myProgObj);
    SurfaceFile* mySurf = myParams->getSurface(1);
    CiftiFile* ciftiOut = myParams->getOutputCifti(2);
    const float* roiData = NULL;
    OptionalParameter* roiOpt = myParams->getOptionalParameter(3);
    if (roiOpt->m_present)
    {
        MetricFile* roiMetric = roiOpt->getMetric(1);
        if (roiMetric->getNumberOfNodes() != mySurf->getNumberOfNodes()) throw OperationException("corrected vertex areas metric does not match surface number of vertices");
        roiData = roiMetric->getValuePointerForColumn(0);
    }
    float distLimit = -1.0f;
    OptionalParameter* limitOpt = myParams->getOptionalParameter(4);
    if (limitOpt->m_present)
    {
        distLimit = limitOpt->getDouble(1);
        if (!(distLimit > 0.0f)) throw OperationException("<limit-mm> must be positive");
    }
    CaretPointer<GeodesicHelperBase> myBase;
    OptionalParameter* corrAreaOpt = myParams->getOptionalParameter(5);
    if (corrAreaOpt->m_present)
    {
        MetricFile* corrAreas = corrAreaOpt->getMetric(1);
        if (corrAreas->getNumberOfNodes() != mySurf->getNumberOfNodes()) throw OperationException("corrected vertex areas metric does not match surface number of vertices");
        myBase.grabNew(new GeodesicHelperBase(mySurf, corrAreas->getValuePointerForColumn(0)));
    }
    bool naive = myParams->getOptionalParameter(6)->m_present;
    CiftiBrainModelsMap myMap;
    StructureEnum::Enum structure = mySurf->getStructure();
    myMap.addSurfaceModel(mySurf->getNumberOfNodes(), structure, roiData);
    int64_t mapLength = myMap.getLength();
    vector<CiftiBrainModelsMap::SurfaceMap> surfMap = myMap.getSurfaceMap(structure);
    CiftiXML myXML;
    myXML.setNumberOfDimensions(2);
    myXML.setMap(CiftiXML::ALONG_ROW, myMap);
    myXML.setMap(CiftiXML::ALONG_COLUMN, myMap);
    ciftiOut->setCiftiXML(myXML);
#pragma omp CARET_PAR
    {
        CaretPointer<GeodesicHelper> privHelper;
        if (corrAreaOpt->m_present)
        {
            privHelper.grabNew(new GeodesicHelper(myBase));
        } else {
            privHelper = mySurf->getGeodesicHelper();
        }
#pragma omp CARET_FOR schedule(dynamic)
        for (int64_t i = 0; i < mapLength; ++i)
        {
            vector<float> outRow(mapLength, -1.0f), outDists;
            vector<int32_t> outNodes;
            if (distLimit > 0.0f)
            {
                privHelper->getNodesToGeoDist(surfMap[i].m_surfaceNode, distLimit, outNodes, outDists, !naive);
                for (int j = 0; j < int(outNodes.size()); ++j)
                {
                    int64_t index = myMap.getIndexForNode(outNodes[j], structure);//-1 if outside ROI
                    if (index >= 0) outRow[index] = outDists[j];
                }
            } else {
                privHelper->getGeoFromNode(surfMap[i].m_surfaceNode, outDists, !naive);
                for (int64_t j = 0; j < mapLength; ++j)
                {
                    outRow[j] = outDists[surfMap[j].m_surfaceNode];
                }
            }
#pragma omp critical
            {
                ciftiOut->setRow(outRow.data(), i);
            }
        }
    }
}
