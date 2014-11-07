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

#include "OperationSurfaceNormals.h"
#include "OperationException.h"

#include "MetricFile.h"
#include "SurfaceFile.h"

using namespace caret;
using namespace std;

AString OperationSurfaceNormals::getCommandSwitch()
{
    return "-surface-normals";
}

AString OperationSurfaceNormals::getShortDescription()
{
    return "OUTPUT VERTEX NORMALS AS METRIC FILE";
}

OperationParameters* OperationSurfaceNormals::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    
    ret->addSurfaceParameter(1, "surface", "the surface to output the normals of");
    
    ret->addMetricOutputParameter(2, "metric-out", "the normal vectors");
    
    ret->setHelpText(
        AString("Computes the normal vectors of the surface file, and outputs them as a 3 column metric file.")
    );
    return ret;
}

void OperationSurfaceNormals::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    LevelProgress myProgress(myProgObj);
    SurfaceFile* mySurf = myParams->getSurface(1);
    MetricFile* myMetricOut = myParams->getOutputMetric(2);
    int numNodes = mySurf->getNumberOfNodes();
    myMetricOut->setNumberOfNodesAndColumns(numNodes, 3);
    myMetricOut->setStructure(mySurf->getStructure());
    myMetricOut->setColumnName(0, "normal x-component");
    myMetricOut->setColumnName(1, "normal y-component");
    myMetricOut->setColumnName(2, "normal z-component");
    mySurf->computeNormals();
    const float* normalData = mySurf->getNormalData();
    vector<float> colScratch(numNodes);
    for (int axis = 0; axis < 3; ++axis)
    {
        for (int i = 0; i < numNodes; ++i)
        {
            colScratch[i] = normalData[i * 3 + axis];
        }
        myMetricOut->setValuesForColumn(axis, colScratch.data());
    }
}
