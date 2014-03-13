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

#include "OperationFociGetProjectionVertex.h"
#include "OperationException.h"

#include "FociFile.h"
#include "Focus.h"
#include "MetricFile.h"
#include "SurfaceFile.h"
#include "SurfaceProjectedItem.h"
#include "SurfaceProjectionBarycentric.h"
#include "SurfaceProjectionVanEssen.h"

using namespace caret;
using namespace std;

AString OperationFociGetProjectionVertex::getCommandSwitch()
{
    return "-foci-get-projection-vertex";
}

AString OperationFociGetProjectionVertex::getShortDescription()
{
    return "GET PROJECTION VERTEX FOR FOCI";
}

OperationParameters* OperationFociGetProjectionVertex::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    ret->addFociParameter(1, "foci", "the foci file");

    ret->addSurfaceParameter(2, "surface", "the surface related to the foci file");

    ret->addMetricOutputParameter(3, "metric-out", "the output metric file");

    OptionalParameter* nameOpt = ret->createOptionalParameter(4, "-name", "select a focus by name");
    nameOpt->addStringParameter(1, "name", "the name of the focus");
    
    ret->setHelpText(
        AString("For each focus, a column is created in <metric-out>, and the vertex with the most influence on its projection ") +
        "is assigned a value of 1 in that column, with all other vertices 0.  " +
        "If -name is used, only one focus will be used."
    );
    return ret;
}

void OperationFociGetProjectionVertex::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    LevelProgress myProgress(myProgObj);
    const FociFile* myFoci = myParams->getFoci(1);
    const SurfaceFile* mySurf = myParams->getSurface(2);
    MetricFile* myMetricOut = myParams->getOutputMetric(3);
    OptionalParameter* nameOpt = myParams->getOptionalParameter(4);
    int numFoci = myFoci->getNumberOfFoci();
    if (numFoci < 1) throw OperationException("Foci file has no foci");
    int numNodes = mySurf->getNumberOfNodes();
    if (nameOpt->m_present)
    {
        AString name = nameOpt->getString(1);
        myMetricOut->setNumberOfNodesAndColumns(numNodes, 1);
        myMetricOut->setStructure(mySurf->getStructure());
        bool found = false;
        for (int i = 0; i < numFoci; ++i)
        {
            const Focus* myFocus = myFoci->getFocus(i);
            if (name == myFocus->getName())
            {
                found = true;
                if (myFocus->getNumberOfProjections() < 1) throw OperationException("matching focus has no projections");
                const SurfaceProjectedItem* firstItem = myFocus->getProjection(0);
                int whichNode = -1;
                if (firstItem->getBarycentricProjection()->isValid())
                {
                    const float* areas = firstItem->getBarycentricProjection()->getTriangleAreas();
                    const int32_t* nodes = firstItem->getBarycentricProjection()->getTriangleNodes();
                    int pos = 0;
                    for (int i = 1; i < 3; ++i)
                    {
                        if (areas[i] > areas[pos]) pos = i;
                    }
                    whichNode = nodes[pos];
                } else {
                    throw OperationException("barycentric projection not valid");//someone else can worry about van essen projection, if it is needed
                }
                if (whichNode >= numNodes) throw OperationException("foci file has larger node numbers than surface");
                myMetricOut->initializeColumn(0, 0.0f);
                myMetricOut->setColumnName(0, myFocus->getName());
                myMetricOut->setValue(whichNode, 0, 1.0f);
                break;
            }
        }
        if (!found) throw OperationException("specified name did not match any foci");
    } else {
        myMetricOut->setNumberOfNodesAndColumns(numNodes, numFoci);
        myMetricOut->setStructure(mySurf->getStructure());
        for (int i = 0; i < numFoci; ++i)
        {
            const Focus* myFocus = myFoci->getFocus(i);
            if (myFocus->getNumberOfProjections() < 1) throw OperationException("matching focus has no projections");
            const SurfaceProjectedItem* firstItem = myFocus->getProjection(0);
            int whichNode = -1;
            if (firstItem->getBarycentricProjection()->isValid())
            {
                const float* areas = firstItem->getBarycentricProjection()->getTriangleAreas();
                const int32_t* nodes = firstItem->getBarycentricProjection()->getTriangleNodes();
                int pos = 0;
                for (int i = 1; i < 3; ++i)
                {
                    if (areas[i] > areas[pos]) pos = i;
                }
                whichNode = nodes[pos];
            } else {
                throw OperationException("barycentric projection not valid");//someone else can worry about van essen projection, if it is needed
            }
            if (whichNode >= numNodes) throw OperationException("foci file has larger node numbers than surface");
            myMetricOut->initializeColumn(i, 0.0f);
            myMetricOut->setColumnName(i, myFocus->getName());
            myMetricOut->setValue(whichNode, i, 1.0f);
        }
    }
}
