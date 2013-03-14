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

#include "AlgorithmSurfaceAverage.h"
#include "AlgorithmException.h"

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
    
    OptionalParameter* stdevOpt = ret->createOptionalParameter(3, "-stddev", "compute 3D sample standard deviation");
    stdevOpt->addMetricOutputParameter(1, "stddev-metric-out", "the output metric for 3D sample standard deviation");
    
    OptionalParameter* uncertaintyOpt = ret->createOptionalParameter(4, "-uncertainty", "compute caret5 'uncertainty'");
    uncertaintyOpt->addMetricOutputParameter(1, "uncert-metric-out", "the output metric for uncertainty");
    
    ret->setHelpText(
        AString("The 3D sample standard deviation is computed as 'sqrt(sum(squaredlength(xyz - mean(xyz)))/(n - 1))'.\n\n") +
        "Uncertainty is a legacy measure used in caret5, and is computed as 'sum(length(xyz - mean(xyz)))/n'."
    );
    return ret;
}

void AlgorithmSurfaceAverage::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    SurfaceFile* myAvgOut = myParams->getOutputSurface(1);
    vector<const SurfaceFile*> inputSurfs;
    const vector<ParameterComponent*>& surfOpts = *(myParams->getRepeatableParameterInstances(2));
    int numSurfs = (int)surfOpts.size();
    for (int i = 0; i < numSurfs; ++i)
    {
        inputSurfs.push_back(surfOpts[i]->getSurface(1));
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
    AlgorithmSurfaceAverage(myProgObj, myAvgOut, inputSurfs, stdevOut, uncertOut);
}

AlgorithmSurfaceAverage::AlgorithmSurfaceAverage(ProgressObject* myProgObj, SurfaceFile* myAvgOut, const vector<const SurfaceFile*>& inputSurfs, MetricFile* stdevOut, MetricFile* uncertOut) : AbstractAlgorithm(myProgObj)
{
    LevelProgress myProgress(myProgObj);
    int numSurfs = (int)inputSurfs.size();
    if (numSurfs == 0) throw AlgorithmException("no input surfaces specified in AlgorithmSurfaceAverage");
    int numNodes = inputSurfs[0]->getNumberOfNodes();
    for (int i = 1; i < numSurfs; ++i)
    {
        if (!inputSurfs[i]->hasNodeCorrespondence(*(inputSurfs[0]))) throw AlgorithmException("surface '" + inputSurfs[i]->getFileName() +
                                                                                      "' does not have node correspondence to surface '" + inputSurfs[0]->getFileName() + "'");
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
        for (int j = 0; j < numSurfs; ++j)
        {
            Vector3D inpoint = inputSurfs[j]->getCoordinate(i);
            accumpoint[0] += inpoint[0];
            accumpoint[1] += inpoint[1];
            accumpoint[2] += inpoint[2];
        }
        Vector3D avgpoint;
        avgpoint[0] = accumpoint[0] / numSurfs;
        avgpoint[1] = accumpoint[1] / numSurfs;
        avgpoint[2] = accumpoint[2] / numSurfs;
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
