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

#include "OperationSurfaceGeodesicROIs.h"
#include "OperationException.h"

#include "GeodesicHelper.h"
#include "MetricFile.h"
#include "SurfaceFile.h"

#include <cmath>
#include <fstream>
#include <string>

using namespace caret;
using namespace std;

AString OperationSurfaceGeodesicROIs::getCommandSwitch()
{
    return "-surface-geodesic-rois";
}

AString OperationSurfaceGeodesicROIs::getShortDescription()
{
    return "DRAW GEODESIC LIMITED ROIS AT VERTICES";
}

OperationParameters* OperationSurfaceGeodesicROIs::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    ret->addSurfaceParameter(1, "surface", "the surface to draw on");
    
    ret->addDoubleParameter(2, "limit", "geodesic distance limit from vertex, in mm");
    
    ret->addStringParameter(3, "vertex-list-file", "a text file containing the vertices to draw ROIs around");
    
    ret->addMetricOutputParameter(4, "metric-out", "the output metric");
    
    OptionalParameter* gaussOpt = ret->createOptionalParameter(5, "-gaussian", "generate a gaussian kernel instead of a flat ROI");
    gaussOpt->addDoubleParameter(1, "sigma", "the sigma for the gaussian kernel, in mm");
    
    OptionalParameter* overlapOpt = ret->createOptionalParameter(6, "-overlap-logic", "how to handle overlapping ROIs, default ALLOW");
    overlapOpt->addStringParameter(1, "method", "the method of resolving overlaps");
    
    OptionalParameter* namesOpt = ret->createOptionalParameter(7, "-names", "name the columns from text file");
    namesOpt->addStringParameter(1, "name-list-file", "a text file containing column names, one per line");
    
    OptionalParameter* corrAreaOpt = ret->createOptionalParameter(8, "-corrected-areas", "vertex areas to use instead of computing them from the surface");
    corrAreaOpt->addMetricParameter(1, "area-metric", "the corrected vertex areas, as a metric");

    ret->setHelpText(
        AString("For each vertex in the list file, a column in the output metric is created, and an ROI around that vertex is drawn in that column.  ") +
        "Each metric column will have zeros outside the geodesic distance spacified by <limit>, and by default will have a value of 1.0 inside it.  " +
        "If the -gaussian option is specified, the values inside the ROI will instead form a gaussian with the specified value of sigma, normalized " +
        "so that the sum of the nonzero values in the metric column is 1.0.  The <method> argument to -overlap-logic must be one of ALLOW, CLOSEST, or EXCLUDE.  " +
        "ALLOW is the default, and means that ROIs are treated independently and may overlap.  " +
        "CLOSEST means that ROIs may not overlap, and that no ROI contains vertices that are closer to a different seed vertex.  " +
        "EXCLUDE means that ROIs may not overlap, and that any vertex within range of more than one ROI does not belong to any ROI."
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
        if (sigma <= 0.0f)
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
            throw OperationException("invalid vertex number: " + AString::number(nodenum));
        }
        nodelist.push_back(nodenum);
        textFile >> nodenum;
    }
    int overlapType = 1;//ALLOW
    OptionalParameter* overlapOpt = myParams->getOptionalParameter(6);
    if (overlapOpt->m_present)
    {
        AString overlapString = overlapOpt->getString(1);
        if (overlapString == "ALLOW")
        {
            overlapType = 1;
        } else if (overlapString == "CLOSEST") {
            overlapType = 2;
        } else if (overlapString == "EXCLUDE") {
            overlapType = 3;
        } else {
            throw OperationException("unrecognized overlap method: " + overlapString);
        }
    }
    vector<AString> namesList;
    OptionalParameter* namesOpt = myParams->getOptionalParameter(7);
    if (namesOpt->m_present)
    {
        AString namesFileName = namesOpt->getString(1);
        fstream namesfile(namesFileName.toLocal8Bit().constData(), fstream::in);
        if (!namesfile.good())
        {
            throw OperationException("error opening names file for reading");
        }
        int i = 0;
        string inputline;
        getline(namesfile, inputline);
        while (namesfile && i < (int)nodelist.size())
        {
            namesList.push_back(inputline.c_str());
            getline(namesfile, inputline);
            ++i;
        }
    }
    MetricFile* corrAreas = NULL;
    OptionalParameter* corrAreaOpt = myParams->getOptionalParameter(8);
    if (corrAreaOpt->m_present)
    {
        corrAreas = corrAreaOpt->getMetric(1);
        if (corrAreas->getNumberOfNodes() != numNodes)
        {
            throw OperationException("corrected areas metric does not match surface in number of vertices");
        }
    }
    myMetricOut->setNumberOfNodesAndColumns(numNodes, (int)nodelist.size());
    myMetricOut->setStructure(mySurf->getStructure());
    float invneg2sigmasqr = -0.5f / (sigma * sigma);
    for (int i = 0; i < (int)nodelist.size(); ++i)
    {
        myMetricOut->initializeColumn(i);
        if (i < (int)namesList.size())
        {
            myMetricOut->setColumnName(i, namesList[i]);
        } else {
            const float* myCoord = mySurf->getCoordinate(i);
            myMetricOut->setColumnName(i, "Vertex " + AString::number(nodelist[i]) +
                                            " (" + AString::number(myCoord[0], 'f', 1) +
                                            ", " + AString::number(myCoord[1], 'f', 1) +
                                            ", " + AString::number(myCoord[2], 'f', 1) + ")");
        }
    }
    CaretPointer<GeodesicHelper> myhelp;
    CaretPointer<GeodesicHelperBase> mygeobase;
    if (corrAreas == NULL)
    {
        myhelp = mySurf->getGeodesicHelper();
    } else {
        mygeobase.grabNew(new GeodesicHelperBase(mySurf, corrAreas->getValuePointerForColumn(0)));
        myhelp.grabNew(new GeodesicHelper(mygeobase));
    }
    switch (overlapType)
    {
        case 1://ALLOW
            for (int i = 0; i < (int)nodelist.size(); ++i)
            {
                vector<int32_t> roinodes;
                vector<float> dists;
                myhelp->getNodesToGeoDist(nodelist[i], limit, roinodes, dists);
                if (sigma > 0.0f)
                {
                    double accum = 0.0;
                    for (int j = 0; j < (int)dists.size(); ++j)
                    {
                        dists[j] = exp(dists[j] * dists[j] * invneg2sigmasqr);//reuse the vector for weights
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
            break;
        case 2:
        case 3:
        {
            vector<int> useCounts(numNodes, 0);
            vector<int> closestSeed(numNodes, -1);
            vector<float> bestDists(numNodes, -1.0f);
            for (int i = 0; i < (int)nodelist.size(); ++i)
            {
                vector<int32_t> roinodes;
                vector<float> dists;
                myhelp->getNodesToGeoDist(nodelist[i], limit, roinodes, dists);
                for (int j = 0; j < (int)roinodes.size(); ++j)
                {
                    ++useCounts[roinodes[j]];
                    if (bestDists[roinodes[j]] < 0.0f || dists[j] < bestDists[roinodes[j]])
                    {
                        bestDists[roinodes[j]] = dists[j];
                        closestSeed[roinodes[j]] = i;//nodelist array index, not node number
                    }
                }
            }
            if (sigma > 0.0f)
            {
                vector<double> accums(nodelist.size(), 0.0);
                vector<vector<int> > roinodelists(nodelist.size());
                vector<vector<float> > weightlists(nodelist.size());
                for (int i = 0; i < numNodes; ++i)
                {
                    if (closestSeed[i] != -1 && (overlapType == 2 || useCounts[i] == 1))
                    {
                        roinodelists[closestSeed[i]].push_back(i);
                        float weight = exp(bestDists[i] * bestDists[i] * invneg2sigmasqr);
                        weightlists[closestSeed[i]].push_back(weight);
                        accums[closestSeed[i]] += weight;
                    }
                }
                for (int i = 0; i < (int)nodelist.size(); ++i)
                {
                    for (int j = 0; j < (int)roinodelists[i].size(); ++j)
                    {
                        myMetricOut->setValue(roinodelists[i][j], i, weightlists[i][j] / accums[i]);
                    }
                }
            } else {
                for (int i = 0; i < numNodes; ++i)
                {
                    if (closestSeed[i] != -1 && (overlapType == 2 || useCounts[i] == 1))
                    {
                        myMetricOut->setValue(i, closestSeed[i], 1.0f);
                    }
                }
            }
            break;
        }
        default:
            throw OperationException("something very bad happened, notify the developers");
    }
}
