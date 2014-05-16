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

#include "OperationMetricVertexSum.h"
#include "OperationException.h"

#include "MetricFile.h"
#include "SurfaceFile.h"

#include <sstream>
#include <iomanip>
#include <iostream>

using namespace caret;
using namespace std;

AString OperationMetricVertexSum::getCommandSwitch()
{
    return "-metric-vertex-sum";
}

AString OperationMetricVertexSum::getShortDescription()
{
    return "SUM VALUES ACROSS VERTICES IN A METRIC FILE";
}

OperationParameters* OperationMetricVertexSum::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    ret->addMetricParameter(1, "metric-in", "the metric to sum");
    
    OptionalParameter* integrateOpt = ret->createOptionalParameter(2, "-integrate", "integrate across a surface, rather than summing");
    integrateOpt->addSurfaceParameter(1, "surface", "the surface to integrate on");
    
    OptionalParameter* integrateMetricOpt = ret->createOptionalParameter(3, "-integrate-metric", "integrate using vertex areas from a metric file");
    integrateMetricOpt->addMetricParameter(1, "area-metric", "metric file containing vertex areas");
    
    OptionalParameter* roiOpt = ret->createOptionalParameter(4, "-roi", "only use data inside an roi");
    roiOpt->addMetricParameter(1, "roi-metric", "the roi, as a metric file");
    
    OptionalParameter* columnOpt = ret->createOptionalParameter(5, "-column", "select a single column");
    columnOpt->addStringParameter(1, "column", "the column number or name");
    
    ret->setHelpText(
        AString("For each column in <metric-in>, sum the values across all vertices, then print the sum on standard output.  ") +
        "-integrate and -integrate-metric multiply each vertex value by the vertex area before doing the sum.  " +
        "Only one of -integrate and -integrate-metric may be specified.  " +
        "Use -roi to only sum within a specific area.  " +
        "Use -column to only report for one column.  "
    );
    return ret;
}

void OperationMetricVertexSum::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    LevelProgress myProgress(myProgObj);
    MetricFile* myMetric = myParams->getMetric(1);
    int numNodes = myMetric->getNumberOfNodes();
    const float* integrateData = NULL;
    vector<float> surfNodeAreas;
    OptionalParameter* integrateOpt = myParams->getOptionalParameter(2);
    if (integrateOpt->m_present)
    {
        SurfaceFile* integrateSurf = integrateOpt->getSurface(1);
        if (integrateSurf->getNumberOfNodes() != numNodes) throw OperationException("integration surface has the wrong number of vertices");
        integrateSurf->computeNodeAreas(surfNodeAreas);
        integrateData = surfNodeAreas.data();
    }
    OptionalParameter* integrateMetricOpt = myParams->getOptionalParameter(3);
    if (integrateMetricOpt->m_present)
    {
        if (integrateOpt->m_present) throw OperationException("only one of -integrate and -integrate-metric can be specified");
        MetricFile* integrateMetric = integrateMetricOpt->getMetric(1);//we don't need to keep a pointer to it
        if (integrateMetric->getNumberOfNodes() != numNodes) throw OperationException("integration metric has the wrong number of vertices");
        integrateData = integrateMetric->getValuePointerForColumn(0);//just its data
    }
    MetricFile* myRoi = NULL;
    OptionalParameter* roiOpt = myParams->getOptionalParameter(4);
    if (roiOpt->m_present)
    {
        myRoi = roiOpt->getMetric(1);
        if (myRoi->getNumberOfNodes() != numNodes) throw OperationException("roi-metric has the wrong number of vertices");
    }
    int myColumn = -1;
    OptionalParameter* columnOpt = myParams->getOptionalParameter(5);
    if (columnOpt->m_present)
    {
        myColumn = myMetric->getMapIndexFromNameOrNumber(columnOpt->getString(1));
        if (myColumn < 0 || myColumn >= myMetric->getNumberOfMaps())
        {
            throw OperationException("invalid column specified");
        }
    }
    const float* roiData = NULL;
    if (myRoi != NULL) roiData = myRoi->getValuePointerForColumn(0);
    if (integrateData != NULL)
    {
        if (myColumn == -1)
        {
            int numCols = myMetric->getNumberOfColumns();
            for (int j = 0; j < numCols; ++j)
            {
                double accum = 0.0;
                const float* data = myMetric->getValuePointerForColumn(j);
                for (int i = 0; i < numNodes; ++i)
                {
                    if (roiData == NULL || roiData[i] > 0.0f)
                    {
                        accum += data[i] * integrateData[i];
                    }
                }
                stringstream s;
                s << std::setprecision(7) << accum;
                cout << s.str() << endl;
            }
        } else {
            double accum = 0.0;
            const float* data = myMetric->getValuePointerForColumn(myColumn);
            for (int i = 0; i < numNodes; ++i)
            {
                if (roiData == NULL || roiData[i] > 0.0f)
                {
                    accum += data[i] * integrateData[i];
                }
            }
            stringstream s;
            s << std::setprecision(7) << accum;
            cout << s.str() << endl;
        }
    } else {
        if (myColumn == -1)
        {
            int numCols = myMetric->getNumberOfColumns();
            for (int j = 0; j < numCols; ++j)
            {
                double accum = 0.0;
                const float* data = myMetric->getValuePointerForColumn(j);
                for (int i = 0; i < numNodes; ++i)
                {
                    if (roiData == NULL || roiData[i] > 0.0f)
                    {
                        accum += data[i];
                    }
                }
                stringstream s;
                s << std::setprecision(7) << accum;
                cout << s.str() << endl;
            }
        } else {
            double accum = 0.0;
            const float* data = myMetric->getValuePointerForColumn(myColumn);
            for (int i = 0; i < numNodes; ++i)
            {
                if (roiData == NULL || roiData[i] > 0.0f)
                {
                    accum += data[i];
                }
            }
            stringstream s;
            s << std::setprecision(7) << accum;
            cout << s.str() << endl;
        }
    }
}
