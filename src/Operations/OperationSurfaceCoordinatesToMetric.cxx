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

#include "OperationSurfaceCoordinatesToMetric.h"
#include "OperationException.h"

#include "MetricFile.h"
#include "SurfaceFile.h"

using namespace caret;
using namespace std;

AString OperationSurfaceCoordinatesToMetric::getCommandSwitch()
{
    return "-surface-coordinates-to-metric";
}

AString OperationSurfaceCoordinatesToMetric::getShortDescription()
{
    return "MAKE METRIC FILE OF SURFACE COORDINATES";
}

OperationParameters* OperationSurfaceCoordinatesToMetric::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    ret->addSurfaceParameter(1, "surface", "the surface to use the coordinates of");
    
    ret->addMetricOutputParameter(2, "metric-out", "the output metric");
    
    ret->setHelpText(
        AString("Puts the coordinates of the surface into a 3-map metric file, as x, y, z.")
    );
    return ret;
}

void OperationSurfaceCoordinatesToMetric::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    LevelProgress myProgress(myProgObj);
    SurfaceFile* mySurf = myParams->getSurface(1);
    MetricFile* myMetricOut = myParams->getOutputMetric(2);
    int numNodes = mySurf->getNumberOfNodes();
    myMetricOut->setNumberOfNodesAndColumns(numNodes, 3);
    myMetricOut->setStructure(mySurf->getStructure());
    myMetricOut->setMapName(0, "x coordinate");
    myMetricOut->setMapName(1, "y coordinate");
    myMetricOut->setMapName(2, "z coordinate");
    for (int i = 0; i < numNodes; ++i)
    {
        const float* coord = mySurf->getCoordinate(i);
        myMetricOut->setValue(i, 0, coord[0]);
        myMetricOut->setValue(i, 1, coord[1]);
        myMetricOut->setValue(i, 2, coord[2]);
    }
}
