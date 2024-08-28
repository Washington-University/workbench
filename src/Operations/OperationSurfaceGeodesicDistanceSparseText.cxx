/*LICENSE_START*/
/*
 *  Copyright (C) 2024  Washington University School of Medicine
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

#include "OperationSurfaceGeodesicDistanceSparseText.h"
#include "OperationException.h"

#include "GeodesicHelper.h"
#include "MathFunctions.h"
#include "MetricFile.h"
#include "SurfaceFile.h"

#include <fstream>

using namespace caret;
using namespace std;

AString OperationSurfaceGeodesicDistanceSparseText::getCommandSwitch()
{
    return "-surface-geodesic-distance-sparse-text";
}

AString OperationSurfaceGeodesicDistanceSparseText::getShortDescription()
{
    return "OUTPUT GEODESIC DISTANCES AS TEXT";
}

OperationParameters* OperationSurfaceGeodesicDistanceSparseText::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    
    ret->addSurfaceParameter(1, "surface", "the surface to compute on");
    
    ret->addDoubleParameter(2, "limit", "the geodesic distance limit in mm");
    
    ret->addStringParameter(3, "text-out", "output - the output text file"); //fake the output parameter formatting
    
    OptionalParameter* corrAreaOpt = ret->createOptionalParameter(4, "-corrected-areas", "vertex areas to use to correct the distances on a group-average surface");
    corrAreaOpt->addMetricParameter(1, "area-metric", "the corrected vertex areas, as a metric");

    ret->createOptionalParameter(5, "-naive", "use only neighbors, don't crawl triangles (not recommended)");
    
    ret->setHelpText(
        AString("Similar to -geodesic-distance-all-to-all, but outputs in a text format that only lists the in-range vertices.  ") +
        "Each line in the output text file is a list of pairs, like '<vertex>,<distance>,<vertex>,<distance>,...' in csv format.\n\n" +
        "The -corrected-areas option should be used when the input is a group average surface - group average surfaces have " +
        "significantly less surface area than individual surfaces do, and therefore distances measured on them would be smaller than measuring them on individual surfaces.  " +
        "In this case, the input to this option should be a group average of the output of -surface-vertex-areas for each subject.\n\n" +
        "If -naive is not specified, the algorithm uses not just immediate neighbors, but also neighbors derived from crawling across pairs of triangles that share an edge."
    );
    return ret;
}

void OperationSurfaceGeodesicDistanceSparseText::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    LevelProgress myProgress(myProgObj);
    SurfaceFile* mySurf = myParams->getSurface(1);
    float limit = myParams->getDouble(2);
    AString textOutName = myParams->getString(3);
    CaretPointer<GeodesicHelper> myGeoHelp;
    CaretPointer<GeodesicHelperBase> myBase;
    OptionalParameter* corrAreaOpt = myParams->getOptionalParameter(4);
    if (corrAreaOpt->m_present)
    {
        MetricFile* corrAreas = corrAreaOpt->getMetric(1);
        if (corrAreas->getNumberOfNodes() != mySurf->getNumberOfNodes()) throw OperationException("corrected vertex areas metric does not match surface number of vertices");
        myBase.grabNew(new GeodesicHelperBase(mySurf, corrAreas->getValuePointerForColumn(0)));
        myGeoHelp.grabNew(new GeodesicHelper(myBase));
    } else {
        myGeoHelp = mySurf->getGeodesicHelper();
    }
    bool naive = myParams->getOptionalParameter(5)->m_present;
    if (limit < 0.0f || !MathFunctions::isNumeric(limit)) throw OperationException("geodesic distance limit must be numeric and non-negative"); //accept zero, I guess...
    fstream textOut(textOutName.toLatin1().data(), ios_base::out);
    int32_t numNodes = mySurf->getNumberOfNodes();
    vector<int32_t> nodes;
    vector<float> dists;
    for (int32_t i = 0; i < numNodes; ++i)
    {
        myGeoHelp->getNodesToGeoDist(i, limit, nodes, dists, !naive);
        bool first = true;
        for (int neigh = 0; neigh < int(nodes.size()); ++neigh)
        {
            if (!first) textOut << ",";
            first = false;
            textOut << nodes[neigh] << "," << dists[neigh];
        }
        textOut << endl;
    }
}
