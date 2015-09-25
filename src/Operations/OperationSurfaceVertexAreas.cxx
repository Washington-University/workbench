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

#include "OperationSurfaceVertexAreas.h"
#include "OperationException.h"
#include "MetricFile.h"
#include "SurfaceFile.h"

#include <vector>

using namespace caret;
using namespace std;

AString OperationSurfaceVertexAreas::getCommandSwitch()
{
    return "-surface-vertex-areas";
}

AString OperationSurfaceVertexAreas::getShortDescription()
{
    return "MEASURE SURFACE AREA EACH VERTEX IS RESPONSIBLE FOR";
}

OperationParameters* OperationSurfaceVertexAreas::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    ret->addSurfaceParameter(1, "surface", "the surface to measure");
    ret->addMetricOutputParameter(2, "metric", "the output metric");
    ret->setHelpText(
        AString("Each vertex gets one third of the area of each triangle it is a part of.  ") +
        "Units are mm^2."
    );
    return ret;
}

void OperationSurfaceVertexAreas::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    LevelProgress myProgress(myProgObj);
    SurfaceFile* mySurf = myParams->getSurface(1);
    MetricFile* myMetricOut = myParams->getOutputMetric(2);
    int numNodes = mySurf->getNumberOfNodes();
    vector<float> areas;
    mySurf->computeNodeAreas(areas);
    myMetricOut->setNumberOfNodesAndColumns(numNodes, 1);
    myMetricOut->setStructure(mySurf->getStructure());
    myMetricOut->setColumnName(0, "vertex areas");
    myMetricOut->setValuesForColumn(0, areas.data());
}
