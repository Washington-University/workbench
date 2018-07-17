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

#include "OperationSurfaceGeodesicDistance.h"
#include "OperationException.h"

#include "CaretAssert.h"
#include "GeodesicHelper.h"
#include "MetricFile.h"
#include "SurfaceFile.h"

using namespace caret;
using namespace std;

AString OperationSurfaceGeodesicDistance::getCommandSwitch()
{
    return "-surface-geodesic-distance";
}

AString OperationSurfaceGeodesicDistance::getShortDescription()
{
    return "COMPUTE GEODESIC DISTANCE FROM ONE VERTEX TO THE ENTIRE SURFACE";
}

OperationParameters* OperationSurfaceGeodesicDistance::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    ret->addSurfaceParameter(1, "surface", "the surface to compute on");
    
    ret->addIntegerParameter(2, "vertex", "the vertex to compute geodesic distance from");
    
    ret->addMetricOutputParameter(3, "metric-out", "the output metric");
    
    ret->createOptionalParameter(4, "-naive", "use only neighbors, don't crawl triangles (not recommended)");
    
    OptionalParameter* limitOpt = ret->createOptionalParameter(5, "-limit", "stop at a certain distance");
    limitOpt->addDoubleParameter(1, "limit-mm", "distance in mm to stop at");
    
    OptionalParameter* corrAreaOpt = ret->createOptionalParameter(6, "-corrected-areas", "vertex areas to use instead of computing them from the surface");
    corrAreaOpt->addMetricParameter(1, "area-metric", "the corrected vertex areas, as a metric");
    
    ret->setHelpText(
        AString("Unless -limit is specified, computes the geodesic distance from the specified vertex to all others.  ") +
        "The result is output as a single column metric file, with a value of -1 for vertices that the distance was not computed for.\n\n" +
        "The -corrected-areas option should be used when the input is a group average surface - group average surfaces have " +
        "significantly less surface area than individual surfaces do, and therefore distances measured on them would be smaller than measuring them on individual surfaces.  " +
        "In this case, the input to this option should be a group average of the output of -surface-vertex-areas for each subject.\n\n" +
        "If -naive is not specified, the algorithm uses not just immediate neighbors, but also neighbors derived from crawling across pairs of triangles that share an edge."
    );
    return ret;
}

void OperationSurfaceGeodesicDistance::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    LevelProgress myProgress(myProgObj);
    SurfaceFile* mySurf = myParams->getSurface(1);
    int myVertex = (int)myParams->getInteger(2);
    MetricFile* myMetricOut = myParams->getOutputMetric(3);
    bool smooth = !(myParams->getOptionalParameter(4)->m_present);
    OptionalParameter* limitOpt = myParams->getOptionalParameter(5);
    CaretPointer<GeodesicHelper> myHelp;
    CaretPointer<GeodesicHelperBase> myBase;
    OptionalParameter* corrAreaOpt = myParams->getOptionalParameter(6);
    if (corrAreaOpt->m_present)
    {
        MetricFile* corrAreas = corrAreaOpt->getMetric(1);
        if (corrAreas->getNumberOfNodes() != mySurf->getNumberOfNodes()) throw OperationException("corrected vertex areas metric does not match surface number of vertices");
        myBase.grabNew(new GeodesicHelperBase(mySurf, corrAreas->getValuePointerForColumn(0)));
        myHelp.grabNew(new GeodesicHelper(myBase));
    } else {
        myHelp = mySurf->getGeodesicHelper();
    }
    vector<float> scratch(mySurf->getNumberOfNodes(), -1.0f);//use -1 to specify invalid
    if (limitOpt->m_present)
    {
        vector<int32_t> nodes;
        vector<float> dists;
        myHelp->getNodesToGeoDist(myVertex, limitOpt->getDouble(1), nodes, dists, smooth);
        for (int i = 0; i < (int)nodes.size(); ++i)
        {
            CaretAssertVectorIndex(dists, i);
            scratch[nodes[i]] = dists[i];
        }
    } else {
        myHelp->getGeoFromNode(myVertex, scratch, smooth);
        if (scratch.size() == 0) throw OperationException("invalid vertex specified");
    }
    myMetricOut->setNumberOfNodesAndColumns(mySurf->getNumberOfNodes(), 1);
    myMetricOut->setStructure(mySurf->getStructure());
    myMetricOut->setColumnName(0, "vertex " + AString::number(myVertex) + " distance");
    myMetricOut->setValuesForColumn(0, scratch.data());
}
