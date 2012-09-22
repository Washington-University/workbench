/*LICENSE_START*/
/*
 *  Copyright 1995-2002 Washington University School of Medicine
 *
 *  http://brainmap.wustl.edu
 *
 *  This file is part of CARET.
 *
 *  CARET is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  CARET is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with CARET; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include "OperationSurfaceGeodesicROIs.h"
#include "OperationException.h"

#include "GeodesicHelper.h"
#include "MetricFile.h"
#include "SurfaceFile.h"

#include <cmath>
#include <fstream>

using namespace caret;
using namespace std;

AString OperationSurfaceGeodesicROIs::getCommandSwitch()
{
    return "-surface-geodesic-rois";
}

AString OperationSurfaceGeodesicROIs::getShortDescription()
{
    return "DRAW GEODESIC LIMITED ROIS AT NODES";
}

OperationParameters* OperationSurfaceGeodesicROIs::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    ret->addSurfaceParameter(1, "surface", "the surface to draw on");
    
    ret->addDoubleParameter(2, "limit", "geodesic distance limit from node, in mm");
    
    ret->addStringParameter(3, "node-list-file", "a text file containing the nodes to draw ROIs around");
    
    ret->addMetricOutputParameter(4, "metric-out", "the output metric");
    
    OptionalParameter* gaussOpt = ret->createOptionalParameter(5, "-gaussian", "generate a gaussian kernel instead of a flat ROI");
    gaussOpt->addDoubleParameter(1, "sigma", "the sigma for the gaussian kernel, in mm");
    
    ret->setHelpText(
        AString("For each node in the list file, a column in the output metric is created, and an ROI around that node is drawn in that column.  ") +
        "Each metric column will have zeros outside the geodesic distance spacified by <limit>, and by default will have a value of 1.0 inside it.  " +
        "If the -gaussian option is specified, the values inside the ROI will instead form a gaussian with the specified value of sigma, normalized " +
        "so that the sum of the nonzero values in the metric column is 1.0."
    );
    return ret;
}

void OperationSurfaceGeodesicROIs::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    LevelProgress myProgress(myProgObj);
    SurfaceFile* mySurf = myParams->getSurface(1);
    float limit = (float)myParams->getDouble(2);
    AString nodeFileName = myParams->getString(3);
    MetricFile* myMetricOut = myParams->getOutputMetric(4);
    OptionalParameter* gaussOpt = myParams->getOptionalParameter(5);
    float sigma = -1.0f;
    if (gaussOpt->m_present)
    {//set up to use a gaussian function
        sigma = (float)gaussOpt->getDouble(1);
        if (sigma < 0.0f)
        {
            throw OperationException("invalid sigma specified");
        }
    }
    fstream textFile(nodeFileName.toLocal8Bit().constData(), fstream::in);
    if (!textFile.good())
    {
        throw OperationException("error opening list file for reading");
    }
    int nodenum, numNodes = mySurf->getNumberOfNodes();
    vector<int> nodelist;
    textFile >> nodenum;
    while (textFile)
    {
        if (nodenum < 0 || nodenum >= numNodes)
        {
            throw OperationException("invalid node number: " + AString::number(nodenum));
        }
        nodelist.push_back(nodenum);
        textFile >> nodenum;
    }
    myMetricOut->setNumberOfNodesAndColumns(numNodes, (int)nodelist.size());
    myMetricOut->setStructure(mySurf->getStructure());
    float invneg2sigmasqr = -0.5f / (sigma * sigma);
    for (int i = 0; i < (int)nodelist.size(); ++i)
    {
        myMetricOut->initializeColumn(i);
        myMetricOut->setColumnName(i, AString::number(nodelist[i]));
        CaretPointer<GeodesicHelper> myhelp = mySurf->getGeodesicHelper();
        vector<int32_t> roinodes;
        vector<float> dists;
        myhelp->getNodesToGeoDist(nodelist[i], limit, roinodes, dists);
        if (sigma > 0.0f)
        {
            double accum = 0.0;
            for (int j = 0; j < (int)dists.size(); ++j)
            {
                dists[j] = exp(dists[j] * dists[j] * invneg2sigmasqr);
                accum += dists[j];
            }
            for (int j = 0; j < (int)dists.size(); ++j)
            {
                dists[j] /= accum;
                myMetricOut->setValue(roinodes[j], i, dists[j]);
            }
        } else {
            for (int j = 0; j < (int)roinodes.size(); ++j)
            {
                myMetricOut->setValue(roinodes[j], i, 1.0f);
            }
        }
    }
}
