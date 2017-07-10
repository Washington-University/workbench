/*LICENSE_START*/
/*
 *  Copyright (C) 2016  Washington University School of Medicine
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

#include "OperationSurfaceSetCoordinates.h"
#include "OperationException.h"

#include "MetricFile.h"
#include "SurfaceFile.h"

using namespace caret;
using namespace std;

AString OperationSurfaceSetCoordinates::getCommandSwitch()
{
    return "-surface-set-coordinates";
}

AString OperationSurfaceSetCoordinates::getShortDescription()
{
    return "MODIFY COORDINATES OF A SURFACE";
}

OperationParameters* OperationSurfaceSetCoordinates::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    
    ret->addSurfaceParameter(1, "surface-in", "the surface to use for the topology");
    
    ret->addMetricParameter(2, "coord-metric", "the new coordinates, as a 3-column metric file");
    
    ret->addSurfaceOutputParameter(3, "surface-out", "the new surface");
    
    ret->setHelpText(
        AString("Takes the topology from an existing surface file, and uses values from a metric file as coordinates to construct a new surface file.\n\n") +
        "See -surface-coordinates-to-metric for how to get surface coordinates as a metric file, such that you can then modify them via metric commands, etc."
    );
    return ret;
}

void OperationSurfaceSetCoordinates::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    LevelProgress myProgress(myProgObj);
    SurfaceFile* inSurf = myParams->getSurface(1);
    MetricFile* coordsIn = myParams->getMetric(2);
    SurfaceFile* outSurf = myParams->getOutputSurface(3);
    int numNodes = inSurf->getNumberOfNodes();
    if (coordsIn->getNumberOfNodes() != numNodes)
    {
        throw OperationException("coordinate metric file must have the same number of vertices as the input surface");
    }
    if (coordsIn->getNumberOfColumns() != 3)
    {
        throw OperationException("coordinate metric file must have exactly 3 columns: x, y, and z");
    }
    *outSurf = *inSurf;
    vector<float> combinedCoords(numNodes * 3);
    for (int i = 0; i < 3; ++i)
    {
        const float* colData = coordsIn->getValuePointerForColumn(i);
        for (int j = 0; j < numNodes; ++j)
        {
            combinedCoords[j * 3 + i] = colData[j];
        }
    }
    outSurf->setCoordinates(combinedCoords.data());
}
