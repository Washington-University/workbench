/*LICENSE_START*/
/*
 *  Copyright (C) 2025  Washington University School of Medicine
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

#include "AlgorithmMetricInterpolateGaps.h"
#include "AlgorithmException.h"

#include "GeodesicHelper.h"
#include "MathFunctions.h"
#include "MetricFile.h"
#include "SurfaceFile.h"

#include <cmath>
#include <set>

using namespace caret;
using namespace std;

AString AlgorithmMetricInterpolateGaps::getCommandSwitch()
{
    return "-metric-interpolate-gaps";
}

AString AlgorithmMetricInterpolateGaps::getShortDescription()
{
    return "INTERPOLATE BETWEEN DISJOINT DATA";
}

OperationParameters* AlgorithmMetricInterpolateGaps::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    
    ret->addSurfaceParameter(2, "surface", "the surface to compute on");
    
    ret->addDoubleParameter(3, "distance", "limit for how large of a gap to interpolate, in mm");

    ret->addMetricOutputParameter(4, "metric-out", "the output metric");
    
    OptionalParameter* dataRoiOpt = ret->createOptionalParameter(6, "-data-roi", "specify a single-map roi, locations outside it will have their input data ignored, and will have zeros for the output values");
    dataRoiOpt->addMetricParameter(1, "roi-metric", "metric file, positive values denote vertices that have usable data");

    OptionalParameter* corrAreaOpt = ret->createOptionalParameter(7, "-corrected-areas", "vertex areas to use instead of computing them from the surface");
    corrAreaOpt->addMetricParameter(1, "area-metric", "the corrected vertex areas, as a metric");
    
    OptionalParameter* dilExponentOpt = ret->createOptionalParameter(8, "-dilate-exponent", "specify a different exponent for within-map dilation");
    dilExponentOpt->addDoubleParameter(1, "exponent", "exponent 'n' to use in (area / (distance ^ n)) as the weighting function (default 6)");
    
    ret->createOptionalParameter(9, "-dilate-nearest", "use the nearest good value instead of a weighted average for within-map dilation");
    
    ret->createOptionalParameter(10, "-dilate-linear", "fill in values with linear interpolation along strongest gradient for within-map dilation");
    
    ParameterComponent* inputOpt = ret->createRepeatableParameter(1, "-metric", "specify an input metric");
    inputOpt->addMetricParameter(1, "metric-in", "the input data");
    inputOpt->addMetricParameter(2, "bad-vertex-roi", "metric file, positive values denote vertices of the input data that don't have good data");
    
    ret->setHelpText(
        AString("Each input metric is dilated independently, and then the results are blended based on each vertices' distance to the closest vertex that has good data in each input.  ") +
        "When the closest vertex for an input is blocked by a good vertex from a different input, the blocked input is not used for that vertex.  " +
        "When only two inputs are used at a vertex, the blending between dilated inputs is a linear interpolation based on the closest distances.\n\n" +
        "Other options work as described in -metric-dilate."
    );
    return ret;
}

void AlgorithmMetricInterpolateGaps::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    vector<const MetricFile*> inputMetrics;
    vector<const MetricFile*> badRois;
    const vector<ParameterComponent*>& metricInOpts = myParams->getRepeatableParameterInstances(1);
    for (ParameterComponent* instance : metricInOpts)
    {
        inputMetrics.push_back(instance->getMetric(1));
        badRois.push_back(instance->getMetric(2));
    }
    SurfaceFile* mySurf = myParams->getSurface(2);
    float distLimit = float(myParams->getDouble(3));
    MetricFile* myMetricOut = myParams->getOutputMetric(4);
    MetricFile* dataROI = NULL;
    OptionalParameter* dataRoiOpt = myParams->getOptionalParameter(6);
    if (dataRoiOpt->m_present)
    {
        dataROI = dataRoiOpt->getMetric(1);
    }
    MetricFile* corrAreas = NULL;
    OptionalParameter* corrAreaOpt = myParams->getOptionalParameter(7);
    if (corrAreaOpt->m_present)
    {
        corrAreas = corrAreaOpt->getMetric(1);
    }
    float dilExponent = 6.0f;
    OptionalParameter* dilExponentOpt = myParams->getOptionalParameter(8);
    if (dilExponentOpt->m_present)
    {
        dilExponent = float(dilExponentOpt->getDouble(1));
    }
    AlgorithmMetricDilate::Method dilMethod = AlgorithmMetricDilate::WEIGHTED;
    bool methodSpecified = false;
    if (myParams->getOptionalParameter(9)->m_present)
    {
        methodSpecified = true;
        dilMethod = AlgorithmMetricDilate::NEAREST;
    }
    if (myParams->getOptionalParameter(10)->m_present)
    {
        if (methodSpecified) throw AlgorithmException("-nearest and -linear may not be specified together");
        methodSpecified = true;
        dilMethod = AlgorithmMetricDilate::LINEAR;
    }
    AlgorithmMetricInterpolateGaps(myProgObj, inputMetrics, mySurf, distLimit, myMetricOut, badRois, dataROI, corrAreas, dilExponent, dilMethod);
}

AlgorithmMetricInterpolateGaps::AlgorithmMetricInterpolateGaps(ProgressObject* myProgObj, const vector<const MetricFile*> inputMetrics, const SurfaceFile* mySurf, float distLimit, MetricFile* myMetricOut,
                                                               const vector<const MetricFile*> badRois, const MetricFile* dataROI, const MetricFile* corrAreas,
                                                               const float dilExponent, const AlgorithmMetricDilate::Method dilMethod) : AbstractAlgorithm(myProgObj)
{
    LevelProgress myProgress(myProgObj);
    int32_t numNodes = mySurf->getNumberOfNodes();
    int32_t numInputs = int32_t(inputMetrics.size());
    if (numInputs == 0) throw AlgorithmException("no input data specified");
    int32_t numMaps = inputMetrics[0]->getNumberOfMaps();
    CaretAssert(badRois.size() == inputMetrics.size());
    if (badRois.size() != inputMetrics.size()) throw AlgorithmException("internal error, AlgorithmMetricInterpolateGaps provided different number of input and bad vertex metrics");
    for (int32_t i = 0; i < numInputs; ++i)
    {
        if (inputMetrics[i]->getNumberOfNodes() != numNodes) throw AlgorithmException("data must have the same number of vertices as the surface");
        if (inputMetrics[i]->getNumberOfMaps() != numMaps) throw AlgorithmException("input metric '" + inputMetrics[i]->getFileName() + "' has a different number of maps than the other inputs");
        if (badRois[i]->getNumberOfNodes() != numNodes) throw AlgorithmException("bad vertex roi has a different number of vertices than the data");
    }
    if (dataROI != NULL && dataROI->getNumberOfNodes() != numNodes) throw AlgorithmException("data roi has a different number of vertices than the data");
    if (corrAreas != NULL && corrAreas->getNumberOfNodes() != numNodes) throw AlgorithmException("vertex area metric has a different number of vertices than the data");
    myMetricOut->setNumberOfNodesAndColumns(numNodes, 1);
    myMetricOut->setStructure(mySurf->getStructure());
    //don't need match maps anymore
    //AlgorithmMetricDilate(NULL, myMetric, mySurf, distLimit, &dilOut, badROI, dataROI, -1, dilMethod, dilExponent, corrAreas, false, true);
    vector<MetricFile> dilOut(numInputs);
    vector<vector<char> > charDataRois(numInputs, vector<char>(numNodes));
    vector<set<int32_t> > definedByInput(numNodes);
    const float* dataRoiPointer = NULL;
    if (dataROI != NULL) dataRoiPointer = dataROI->getValuePointerForColumn(0);
    for (int32_t input = 0; input < numInputs; ++input)
    {
        AlgorithmMetricDilate(NULL, inputMetrics[input], mySurf, distLimit, &(dilOut[input]), badRois[input], dataROI, -1, dilMethod, dilExponent, corrAreas);
        const float* badVertPointer = badRois[input]->getValuePointerForColumn(0);
        for (int32_t node = 0; node < numNodes; ++node)
        {
            bool goodVert = (!(badVertPointer[node] > 0.0f)) && (dataRoiPointer == NULL || dataRoiPointer[node] > 0.0f);
            /*if (badVertPointer != NULL)
            {
                goodVert = (!(badVertPointer[node] > 0.0f)) && (dataRoiPointer == NULL || dataRoiPointer[node] > 0.0f);
            } else {
                goodVert = dataPointer[node] != 0.0f && (dataRoiPointer == NULL || dataRoiPointer[node] > 0.0f);
            }//*/
            if (goodVert)
            {
                charDataRois[input][node] = 1;
                definedByInput[node].insert(input);
            } else {
                charDataRois[input][node] = 0;
            }
        }
    }
    vector<vector<float> > valuesOut(numMaps, vector<float>(numNodes, 0.0f));
    CaretPointer<GeodesicHelperBase> correctedBase;
    if (corrAreas != NULL)
    {
        correctedBase.grabNew(new GeodesicHelperBase(mySurf, corrAreas->getValuePointerForColumn(0)));
    }
#pragma omp CARET_PAR
    {
        CaretPointer<GeodesicHelper> myGeoHelp;
        if (corrAreas == NULL)
        {
            myGeoHelp = mySurf->getGeodesicHelper();
        } else {
            myGeoHelp.grabNew(new GeodesicHelper(correctedBase));
        }
#pragma omp CARET_FOR schedule(dynamic)
        for (int32_t node = 0; node < numNodes; ++node)
        {
            if (dataRoiPointer != NULL && ! (dataRoiPointer[node] > 0.0f)) continue;
            vector<double> sums(numMaps, 0.0);
            double weightsum = 0.0;
            for (int32_t input = 0; input < numInputs; ++input)
            {
                float targetDist = 0.0f;
                int32_t targetNode = myGeoHelp->getClosestNodeInRoi(node, charDataRois[input].data(), distLimit, targetDist, true);
                bool blocked = false;
                if (targetNode > 0)
                {
                    vector<int32_t> targetPath;
                    vector<float> pathDists;
                    myGeoHelp->getPathAlongLineSegment(node, targetNode, mySurf->getCoordinate(node), mySurf->getCoordinate(targetNode), targetPath, pathDists);
                    for (int32_t thisNode : targetPath)
                    {
                        if (definedByInput[thisNode].count(input) == 0 && definedByInput[thisNode].size() > 0) //if the path vertex wasn't already defined in this map, but was defined by some map
                        {
                            blocked = true;
                            break;
                        }
                    }
                    if (!blocked)
                    {
                        if (targetDist <= 0.0f) //was already defined in the input data, or the surface is funky
                        {
                            if (weightsum < 0.0)
                            {//already had another 0-distance success, must be defined by multiple input maps (or the surface is borked), add and "increment"
                                for (int32_t i = 0; i < numMaps; ++i)
                                {
                                    sums[i] += dilOut[input].getValue(node, i);
                                }
                                weightsum += -1.0;
                            } else {//discard anything that was at a distance
                                for (int32_t i = 0; i < numMaps; ++i)
                                {
                                    sums[i] = dilOut[input].getValue(node, i);
                                }
                                weightsum = -1.0;
                            }
                        } else {//"extend" linear interpolation to 3+ distances by using inverse distance as weights
                            float thisWeight = 1.0f / targetDist;
                            for (int32_t i = 0; i < numMaps; ++i)
                            {
                                sums[i] += thisWeight * dilOut[input].getValue(node, i);
                            }
                            weightsum += thisWeight;
                        }
                    }
                }
            }
            if (abs(weightsum) > 0.0f) //not zero, not nan, would need a denormal surface to go inf
            {
                for (int32_t i = 0; i < numMaps; ++i)
                {
                    valuesOut[i][node] = float(sums[i] / abs(weightsum)); //trick: when it is a count it is just negative, so abs works for both cases
                }
            }
        }
    }
    for (int32_t i = 0; i < numMaps; ++i)
    {
        myMetricOut->setValuesForColumn(i, valuesOut[i].data());
    }
}

float AlgorithmMetricInterpolateGaps::getAlgorithmInternalWeight()
{
    return 1.0f;//override this if needed, if the progress bar isn't smooth
}

float AlgorithmMetricInterpolateGaps::getSubAlgorithmWeight()
{
    //return AlgorithmInsertNameHere::getAlgorithmWeight();//if you use a subalgorithm
    return 0.0f;
}
