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

#include "AlgorithmSurfaceAverage.h"
#include "AlgorithmException.h"

#include "CaretAssert.h"
#include "MetricFile.h"
#include "SurfaceFile.h"
#include "Vector3D.h"

#include <cmath>

using namespace caret;
using namespace std;

AString AlgorithmSurfaceAverage::getCommandSwitch()
{
    return "-surface-average";
}

AString AlgorithmSurfaceAverage::getShortDescription()
{
    return "AVERAGE SURFACE FILES TOGETHER";
}

OperationParameters* AlgorithmSurfaceAverage::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    
    ret->addSurfaceOutputParameter(1, "surface-out", "the output averaged surface");
    
    ParameterComponent* surfOpt = ret->createRepeatableParameter(2, "-surf", "specify a surface to include in the average");
    surfOpt->addSurfaceParameter(1, "surface", "a surface file to average");
    OptionalParameter* surfWeightOpt = surfOpt->createOptionalParameter(2, "-weight", "specify a weighted average");
    surfWeightOpt->addDoubleParameter(1, "weight", "the weight to use (default 1)");
    
    OptionalParameter* stdevOpt = ret->createOptionalParameter(3, "-stddev", "compute 3D sample standard deviation");
    stdevOpt->addMetricOutputParameter(1, "stddev-metric-out", "the output metric for 3D sample standard deviation");
    
    OptionalParameter* uncertaintyOpt = ret->createOptionalParameter(4, "-uncertainty", "compute caret5 'uncertainty'");
    uncertaintyOpt->addMetricOutputParameter(1, "uncert-metric-out", "the output metric for uncertainty");
    
    ret->setHelpText(
        AString("The 3D sample standard deviation is computed as 'sqrt(sum(squaredlength(xyz - mean(xyz)))/(n - 1))'.\n\n") +
        "Uncertainty is a legacy measure used in caret5, and is computed as 'sum(length(xyz - mean(xyz)))/n'.\n\n" +
        "When weights are used, the 3D sample standard deviation treats them as reliability weights."
    );
    return ret;
}

void AlgorithmSurfaceAverage::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    SurfaceFile* myAvgOut = myParams->getOutputSurface(1);
    vector<const SurfaceFile*> inputSurfs;
    const vector<ParameterComponent*>& surfOpts = myParams->getRepeatableParameterInstances(2);
    int numSurfs = (int)surfOpts.size();
    vector<float> surfWeights, *surfWeightPtr = NULL;
    for (int i = 0; i < numSurfs; ++i)
    {
        inputSurfs.push_back(surfOpts[i]->getSurface(1));
        OptionalParameter* surfWeightOpt = surfOpts[i]->getOptionalParameter(2);
        if (surfWeightOpt->m_present)
        {
            if (surfWeightPtr == NULL)
            {
                surfWeights.resize(i, 1.0f);
                surfWeightPtr = &surfWeights;
            }
            surfWeights.push_back((float)surfWeightOpt->getDouble(1));
        } else {
            if (surfWeightPtr != NULL)
            {
                surfWeights.push_back(1.0f);
            }
        }
    }
    MetricFile* stdevOut = NULL;
    OptionalParameter* stdevOpt = myParams->getOptionalParameter(3);
    if (stdevOpt->m_present)
    {
        stdevOut = stdevOpt->getOutputMetric(1);
    }
    MetricFile* uncertOut = NULL;
    OptionalParameter* uncertaintyOpt = myParams->getOptionalParameter(4);
    if (uncertaintyOpt->m_present)
    {
        uncertOut = uncertaintyOpt->getOutputMetric(1);
    }
    AlgorithmSurfaceAverage(myProgObj, myAvgOut, inputSurfs, stdevOut, uncertOut, surfWeightPtr);
}

AlgorithmSurfaceAverage::AlgorithmSurfaceAverage(ProgressObject* myProgObj, SurfaceFile* myAvgOut, const vector<const SurfaceFile*>& inputSurfs,
                                                 MetricFile* stdevOut, MetricFile* uncertOut, const vector<float>* surfWeightPtr) : AbstractAlgorithm(myProgObj)
{
    LevelProgress myProgress(myProgObj);
    int numSurfs = (int)inputSurfs.size();
    if (numSurfs == 0) throw AlgorithmException("no input surfaces specified in AlgorithmSurfaceAverage");
    if (surfWeightPtr != NULL)
    {
        CaretAssert((int)surfWeightPtr->size() == numSurfs);
    }
    int numNodes = inputSurfs[0]->getNumberOfNodes();
    for (int i = 1; i < numSurfs; ++i)
    {
        if (!inputSurfs[i]->hasNodeCorrespondence(*(inputSurfs[0]))) throw AlgorithmException("surface '" + inputSurfs[i]->getFileName() +
                                                                                      "' does not have node correspondence to surface '" + inputSurfs[0]->getFileName() + "'");
        inputSurfs[i]->clearCachedHelpers();
    }
    *myAvgOut = *(inputSurfs[0]);
    if (uncertOut != NULL)
    {
        uncertOut->setNumberOfNodesAndColumns(numNodes, 1);
        uncertOut->setStructure(inputSurfs[0]->getStructure());
        uncertOut->setColumnName(0, "SHAPE_STANDARD_UNCERTAINTY");
    }
    if (stdevOut != NULL)
    {
        stdevOut->setNumberOfNodesAndColumns(numNodes, 1);
        stdevOut->setStructure(inputSurfs[0]->getStructure());
        stdevOut->setColumnName(0, "3D sample standard deviation");
    }
    //loop across nodes first so that we can use doubles for accumulation easily
    for (int i = 0; i < numNodes; ++i)
    {
        double accumpoint[3] = {0.0, 0.0, 0.0};
        Vector3D avgpoint;
        if (surfWeightPtr == NULL)
        {
            for (int j = 0; j < numSurfs; ++j)
            {
                Vector3D inpoint = inputSurfs[j]->getCoordinate(i);
                accumpoint[0] += inpoint[0];
                accumpoint[1] += inpoint[1];
                accumpoint[2] += inpoint[2];
            }
            avgpoint[0] = accumpoint[0] / numSurfs;
            avgpoint[1] = accumpoint[1] / numSurfs;
            avgpoint[2] = accumpoint[2] / numSurfs;
        } else {
            double weightsum = 0.0;
            for (int j = 0; j < numSurfs; ++j)
            {
                Vector3D inpoint = inputSurfs[j]->getCoordinate(i);
                float thisweight = (*surfWeightPtr)[j];
                weightsum += thisweight;
                accumpoint[0] += inpoint[0] * thisweight;
                accumpoint[1] += inpoint[1] * thisweight;
                accumpoint[2] += inpoint[2] * thisweight;
            }
            avgpoint[0] = accumpoint[0] / weightsum;
            avgpoint[1] = accumpoint[1] / weightsum;
            avgpoint[2] = accumpoint[2] / weightsum;
        }
        myAvgOut->setCoordinate(i, avgpoint);
        if (uncertOut != NULL || stdevOut != NULL)
        {
            if (numSurfs == 1)
            {
                if (uncertOut != NULL)
                {
                    uncertOut->setValue(i, 0, 0.0f);
                }
                if (stdevOut != NULL)
                {
                    stdevOut->setValue(i, 0, 0.0f);
                }
            } else {
                double distAccum = 0.0;//for caret5 uncertainty
                double dist2Accum = 0.0;//for sample stdev
                if (surfWeightPtr == NULL)
                {
                    for (int j = 0; j < numSurfs; ++j)
                    {
                        Vector3D inpoint = inputSurfs[j]->getCoordinate(i);
                        float dist2 = (avgpoint - inpoint).lengthsquared();
                        dist2Accum += dist2;
                        distAccum += sqrt(dist2);
                    }
                    if (uncertOut != NULL)
                    {
                        uncertOut->setValue(i, 0, distAccum / numSurfs);
                    }
                    if (stdevOut != NULL)
                    {
                        stdevOut->setValue(i, 0, sqrt(dist2Accum / (numSurfs - 1)));
                    }
                } else {
                    double weightsum = 0.0, weight2sum = 0.0;
                    for (int j = 0; j < numSurfs; ++j)
                    {
                        Vector3D inpoint = inputSurfs[j]->getCoordinate(i);
                        float thisweight = (*surfWeightPtr)[j];
                        weightsum += thisweight;
                        weight2sum += thisweight * thisweight;
                        float dist2 = (avgpoint - inpoint).lengthsquared();
                        dist2Accum += dist2 * thisweight;
                        distAccum += sqrt(dist2) * thisweight;
                    }
                    if (uncertOut != NULL)
                    {
                        uncertOut->setValue(i, 0, distAccum / weightsum);
                    }
                    if (stdevOut != NULL)
                    {
                        stdevOut->setValue(i, 0, sqrt(dist2Accum / (weightsum - weight2sum / weightsum)));//https://en.wikipedia.org/wiki/Weighted_arithmetic_mean#Weighted_sample_variance
                    }
                }
            }
        }
    }
}

float AlgorithmSurfaceAverage::getAlgorithmInternalWeight()
{
    return 1.0f;//override this if needed, if the progress bar isn't smooth
}

float AlgorithmSurfaceAverage::getSubAlgorithmWeight()
{
    //return AlgorithmInsertNameHere::getAlgorithmWeight();//if you use a subalgorithm
    return 0.0f;
}
