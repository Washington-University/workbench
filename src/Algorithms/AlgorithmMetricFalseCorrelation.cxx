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

#include "AlgorithmMetricFalseCorrelation.h"
#include "AlgorithmException.h"

#include "CaretOMP.h"
#include "CaretPointLocator.h"
#include "GeodesicHelper.h"
#include "SurfaceFile.h"
#include "MetricFile.h"

#include <cmath>
#include <fstream>

using namespace caret;
using namespace std;

AString AlgorithmMetricFalseCorrelation::getCommandSwitch()
{
    return "-metric-false-correlation";
}

AString AlgorithmMetricFalseCorrelation::getShortDescription()
{
    return "COMPARE CORRELATION LOCALLY AND ACROSS/THROUGH SULCI/GYRI";
}

OperationParameters* AlgorithmMetricFalseCorrelation::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    ret->addSurfaceParameter(1, "surface", "the surface to compute geodesic and 3D distance with");
    
    ret->addMetricParameter(2, "metric-in", "the metric to correlate");
    
    ret->addDoubleParameter(3, "3D-dist", "maximum 3D distance to check around each vertex");
    
    ret->addDoubleParameter(4, "geo-outer", "maximum geodesic distance to use for neighboring correlation");
    
    ret->addDoubleParameter(5, "geo-inner", "minimum geodesic distance to use for neighboring correlation");
    
    ret->addMetricOutputParameter(6, "metric-out", "the output metric");
    
    OptionalParameter* roiOpt = ret->createOptionalParameter(7, "-roi", "select a region of interest that has data");
    roiOpt->addMetricParameter(1, "roi-metric", "the region, as a metric file");
    
    OptionalParameter* dumpTextOpt = ret->createOptionalParameter(8, "-dump-text", "dump the raw measures used to a text file");
    dumpTextOpt->addStringParameter(1, "text-out", "the output text file");
    
    ret->setHelpText(
        AString("For each vertex, compute the average correlation within a range of geodesic distances that don't cross a sulcus/gyrus, and the correlation to the closest vertex crossing a sulcus/gyrus.  ") +
        "A vertex is considered to cross a sulcus/gyrus if the 3D distance is less than a third of the geodesic distance.  " +
        "The output file contains the ratio between these correlations, and some additional maps to help explain the ratio."
    );
    return ret;
}

void AlgorithmMetricFalseCorrelation::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    SurfaceFile* mySurf = myParams->getSurface(1);
    MetricFile* myMetric = myParams->getMetric(2);
    float max3D = (float)myParams->getDouble(3);
    float maxgeo = (float)myParams->getDouble(4);
    float mingeo = (float)myParams->getDouble(5);
    MetricFile* myMetricOut = myParams->getOutputMetric(6);
    MetricFile* myRoi = NULL;
    OptionalParameter* roiOpt = myParams->getOptionalParameter(7);
    if (roiOpt->m_present)
    {
        myRoi = roiOpt->getMetric(1);
    }
    AString textName = "";
    OptionalParameter* dumpTextOpt = myParams->getOptionalParameter(8);
    if (dumpTextOpt->m_present)
    {
        textName = dumpTextOpt->getString(1);
    }
    AlgorithmMetricFalseCorrelation(myProgObj, mySurf, myMetric, myMetricOut, max3D, maxgeo, mingeo, myRoi, textName);
}

AlgorithmMetricFalseCorrelation::AlgorithmMetricFalseCorrelation(ProgressObject* myProgObj, const SurfaceFile* mySurf, const MetricFile* myMetric, MetricFile* myMetricOut,
                                                                 const float& max3D, const float& maxgeo, const float& mingeo, const MetricFile* myRoi, const AString& textName) : AbstractAlgorithm(myProgObj)
{
    LevelProgress myProgress(myProgObj);
    if (max3D <= 0.0f || maxgeo <= 0.0f || mingeo < 0.0f) throw AlgorithmException("distance limits must not be negative, and maximums must be positive");
    int numNodes = mySurf->getNumberOfNodes();
    if (myMetric->getNumberOfNodes() != numNodes) throw AlgorithmException("surface and metric have different number of vertices");
    const float* roiCol = NULL;
    if (myRoi != NULL)
    {
        if (myRoi->getNumberOfNodes() != numNodes) throw AlgorithmException("surface and roi metric have different number of vertices");
        roiCol = myRoi->getValuePointerForColumn(0);
    }
    if (myMetric->getNumberOfColumns() < 3) throw AlgorithmException("input metric must have more than 2 columns for correlation to be meaningful");
    bool dumpRaw = false;
    ofstream rawOut;
    if (textName != "")
    {
        dumpRaw = true;
        rawOut.open(textName.toLocal8Bit().constData());
        if (!rawOut) throw AlgorithmException("failed to open text file for output");
    }
    m_toCorr = myMetric;//so we can do fancy correlation caching without rewriting this part, if we want
    setupCorr(roiCol);
    myMetricOut->setNumberOfNodesAndColumns(numNodes, 5);
    myMetricOut->setStructure(mySurf->getStructure());
    myMetricOut->setColumnName(0, "correlation ratio");
    myMetricOut->setColumnName(1, "non-neighborhood correlation");
    myMetricOut->setColumnName(2, "average neighborhood correlation");
    myMetricOut->setColumnName(3, "3D distance to non-neighborhood vertex");
    myMetricOut->setColumnName(4, "non-neighborhood vertex number");
    const AString sep1 = ",", sep2 = ";\n";
    float distRatioCutoff = 3.0f;
    CaretPointer<const CaretPointLocator> myLocator = mySurf->getPointLocator();
#pragma omp CARET_PAR
    {
        CaretPointer<GeodesicHelper> myGeo = mySurf->getGeodesicHelper();
#pragma omp CARET_FOR schedule(dynamic)
        for (int n = 0; n < numNodes; ++n)
        {
            int crossingNode = -1, neighCount = 0;
            float crossingDist = -1.0f, neighAccum = 0.0f;
            if (roiCol == NULL || roiCol[n] > 0.0f)
            {
                AString rawDumpString;//build the entire string for a single node, then write it in one call within #pragma omp critical
                Vector3D myCoord = mySurf->getCoordinate(n);
                vector<LocatorInfo> inRange = myLocator->pointsInRange(myCoord, max3D);
                int numInterested = (int)inRange.size();
                vector<int32_t> interested(numInterested);
                int counter = 0;
                for (vector<LocatorInfo>::iterator iter = inRange.begin(); iter != inRange.end(); ++iter)
                {
                    interested[counter] = iter->index;
                    ++counter;
                }
                vector<float> geoDists;
                myGeo->getGeoToTheseNodes(n, interested, geoDists);
                counter = 0;
                for (vector<LocatorInfo>::iterator iter = inRange.begin(); iter != inRange.end(); ++iter)
                {
                    if (roiCol == NULL || (roiCol[iter->index] > 0.0f))
                    {
                        float dist3D = (myCoord - iter->coords).length();
                        if (iter->index == n || geoDists[counter] / dist3D < distRatioCutoff)
                        {
                            if (maxgeo <= max3D)//otherwise, we can't trust the 3D test picking up all the points we want
                            {
                                if (dumpRaw)
                                {
                                    float thiscorr = correlate(n, iter->index);
                                    rawDumpString += AString::number(n) + sep1 + AString::number(iter->index) + sep1 + AString::number(thiscorr) + sep1 +
                                        AString::number(geoDists[counter]) + sep1 + AString::number(dist3D) + sep2;
                                    if (geoDists[counter] <= maxgeo && geoDists[counter] >= mingeo)
                                    {
                                        neighAccum += thiscorr;
                                        ++neighCount;
                                    }
                                } else {
                                    if (geoDists[counter] <= maxgeo && geoDists[counter] >= mingeo)
                                    {
                                        float thiscorr = correlate(n, iter->index);
                                        neighAccum += thiscorr;
                                        ++neighCount;
                                    }
                                }
                            }
                        } else {
                            if (dist3D < crossingDist || crossingNode == -1)
                            {
                                crossingDist = dist3D;
                                crossingNode = iter->index;
                            }
                            if (dumpRaw)
                            {
                                float thiscorr = correlate(n, iter->index);
                                rawDumpString += AString::number(n) + sep1 + AString::number(iter->index) + sep1 + AString::number(thiscorr) + sep1 +
                                    AString::number(geoDists[counter]) + sep1 + AString::number(dist3D) + sep2;
                            }
                        }
                    }
                    ++counter;
                }
                if (maxgeo > max3D)//so, we have to run geodesic separately
                {
                    myGeo->getNodesToGeoDist(n, maxgeo, interested, geoDists);//reuse interested, we don't need its previous contents
                    int numInRange = (int)interested.size();
                    for (int i = 0; i < numInRange; ++i)
                    {
                        if (roiCol != NULL && !(roiCol[interested[i]] > 0.0f)) continue;
                        float dist3D = (myCoord - Vector3D(mySurf->getCoordinate(interested[i]))).length();
                        if ((interested[i] == n || geoDists[i] / dist3D < distRatioCutoff))
                        {
                            if (dumpRaw)
                            {
                                float thiscorr = correlate(n, interested[i]);
                                rawDumpString += AString::number(n) + sep1 + AString::number(interested[i]) + sep1 + AString::number(thiscorr) + sep1 +
                                    AString::number(geoDists[i]) + sep1 + AString::number(dist3D) + sep2;
                                if (geoDists[i] >= mingeo)//we already know it is not greater than maxgeo
                                {
                                    neighAccum += thiscorr;
                                    ++neighCount;
                                }
                            } else {
                                if (geoDists[i] >= mingeo)
                                {
                                    float thiscorr = correlate(n, interested[i]);
                                    neighAccum += thiscorr;
                                    ++neighCount;
                                }
                            }
                        }
                    }
                }
                if (dumpRaw)
                {
#pragma omp critical
                    {
                        rawOut << rawDumpString;
                    }
                }
            }
            if (crossingNode != -1 && neighCount != 0)
            {
                float longCorr = correlate(n, crossingNode);
                float closeAvg = neighAccum / neighCount;
                myMetricOut->setValue(n, 0, longCorr / closeAvg);
                myMetricOut->setValue(n, 1, longCorr);
                myMetricOut->setValue(n, 2, closeAvg);
                myMetricOut->setValue(n, 3, crossingDist);
                myMetricOut->setValue(n, 4, crossingNode);
            } else {
                myMetricOut->setValue(n, 0, 0.0f);
                myMetricOut->setValue(n, 1, 0.0f);
                myMetricOut->setValue(n, 2, 0.0f);
                myMetricOut->setValue(n, 3, 0.0f);
                myMetricOut->setValue(n, 4, -1);
            }
        }
    }
}

float AlgorithmMetricFalseCorrelation::correlate(int first, int second)
{
    CaretAssert((int)m_demeanedRows[first].size() == m_rowSize);
    CaretAssert((int)m_demeanedRows[second].size() == m_rowSize);
    double accum = 0.0;
    for (int i = 0; i < m_rowSize; ++i)
    {
        accum += m_demeanedRows[first][i] * m_demeanedRows[second][i];
    }
    return accum / (m_rrs[first] * m_rrs[second]);
}

void AlgorithmMetricFalseCorrelation::setupCorr(const float* roiCol)
{
    m_rowSize = m_toCorr->getNumberOfColumns();
    int numRows = m_toCorr->getNumberOfNodes();
    m_demeanedRows.resize(numRows);
    m_rrs.resize(numRows);
    for (int i = 0; i < numRows; ++i)
    {
        if (roiCol != NULL && !(roiCol[i] > 0.0f)) continue;
        double accum = 0.0f;
        m_demeanedRows[i].resize(m_rowSize);
        for (int j = 0; j < m_rowSize; ++j)
        {
            float tempf = m_toCorr->getValue(i, j);
            m_demeanedRows[i][j] = tempf;//first, transpose the indexing - in MetricFile, a column is contiguous in memory
            accum += tempf;
        }
        float mean = accum / m_rowSize;
        accum = 0.0;
        for (int j = 0; j < m_rowSize; ++j)
        {
            float tempf = m_demeanedRows[i][j] - mean;//remove mean, calculate rrs for correlation
            m_demeanedRows[i][j] = tempf;
            accum += tempf * tempf;
        }
        m_rrs[i] = sqrt(accum);
    }
}

float AlgorithmMetricFalseCorrelation::getAlgorithmInternalWeight()
{
    return 1.0f;//override this if needed, if the progress bar isn't smooth
}

float AlgorithmMetricFalseCorrelation::getSubAlgorithmWeight()
{
    //return AlgorithmInsertNameHere::getAlgorithmWeight();//if you use a subalgorithm
    return 0.0f;
}
