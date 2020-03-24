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

#include "AlgorithmCiftiAverage.h"
#include "AlgorithmException.h"
#include "CaretAssert.h"
#include "CaretLogger.h"
#include "CaretPointer.h"
#include "CiftiFile.h"
#include "MathFunctions.h"

#include <cmath>

using namespace caret;
using namespace std;

AString AlgorithmCiftiAverage::getCommandSwitch()
{
    return "-cifti-average";
}

AString AlgorithmCiftiAverage::getShortDescription()
{
    return "AVERAGE CIFTI FILES";
}

OperationParameters* AlgorithmCiftiAverage::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    
    ret->addCiftiOutputParameter(1, "cifti-out", "output cifti file");
    
    OptionalParameter* excludeOpt = ret->createOptionalParameter(2, "-exclude-outliers", "exclude outliers by standard deviation of each element across files");
    excludeOpt->addDoubleParameter(1, "sigma-below", "number of standard deviations below the mean to include");
    excludeOpt->addDoubleParameter(2, "sigma-above", "number of standard deviations above the mean to include");

    ParameterComponent* ciftiOpt = ret->createRepeatableParameter(3, "-cifti", "specify an input file");
    ciftiOpt->addCiftiParameter(1, "cifti-in", "the input cifti file");
    
    OptionalParameter* weightOpt = ciftiOpt->createOptionalParameter(1, "-weight", "give a weight for this file");
    weightOpt->addDoubleParameter(1, "weight", "the weight to use");
    
    ret->setHelpText(
        AString("Averages cifti files together.  ") +
        "Files without -weight specified are given a weight of 1.  " +
        "If -exclude-outliers is specified, at each element, the data across all files is taken as a set, its unweighted mean and sample standard deviation are found, " +
        "and values outside the specified number of standard deviations are excluded from the (potentially weighted) average at that element."
    );
    return ret;
}

void AlgorithmCiftiAverage::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    CiftiFile* ciftiOut = myParams->getOutputCifti(1);
    vector<const CiftiFile*> ciftiList;//this is just so that it can pass them to the algorithm
    vector<float> weights;
    const vector<ParameterComponent*>& myInstances = myParams->getRepeatableParameterInstances(3);
    for (int i = 0; i < (int)myInstances.size(); ++i)
    {
        ciftiList.push_back(myInstances[i]->getCifti(1));
        OptionalParameter* weightOpt = myInstances[i]->getOptionalParameter(1);
        if (weightOpt->m_present)
        {
            weights.push_back((float)weightOpt->getDouble(1));
        } else {
            weights.push_back(1.0f);
        }
    }
    OptionalParameter* excludeOpt = myParams->getOptionalParameter(2);
    if (excludeOpt->m_present)
    {
        AlgorithmCiftiAverage(myProgObj, ciftiList, excludeOpt->getDouble(1), excludeOpt->getDouble(2), ciftiOut, &weights);
    } else {
        AlgorithmCiftiAverage(myProgObj, ciftiList, ciftiOut, &weights);
    }
}

AlgorithmCiftiAverage::AlgorithmCiftiAverage(ProgressObject* myProgObj, const vector<const CiftiFile*>& ciftiList, CiftiFile* ciftiOut, const vector<float>* weightsPtr) : AbstractAlgorithm(myProgObj)
{
    LevelProgress myProgress(myProgObj);
    if (ciftiList.size() == 0)
    {
        throw AlgorithmException("no files specified");
    }
    if (weightsPtr != NULL && ciftiList.size() != weightsPtr->size())
    {
        throw AlgorithmException("number of weights doesn't match number of input cifti files");
    }
    CaretAssert(ciftiList[0] != NULL);
    CiftiXML baseXML = ciftiList[0]->getCiftiXML();
    if (baseXML.getNumberOfDimensions() != 2) throw AlgorithmException("cifti average currently only supports 2D files");
    int numRows = baseXML.getDimensionLength(CiftiXML::ALONG_COLUMN), rowSize = baseXML.getDimensionLength(CiftiXML::ALONG_ROW), numFiles = (int)ciftiList.size();
    for (int i = 1; i < numFiles; ++i)
    {
        CaretAssert(ciftiList[i] != NULL);
        if (!baseXML.approximateMatch(ciftiList[i]->getCiftiXML()))//requires at least length to match, often more restrictive
        {
            throw AlgorithmException("cifti file '" + ciftiList[i]->getFileName() + "' does not match earlier inputs");
        }
    }
    ciftiOut->setCiftiXML(baseXML);
    vector<double> accum(rowSize);
    vector<float> myrow(rowSize);
    for (int i = 0; i < numRows; ++i)
    {
        vector<double> weightaccum(rowSize, 0.0);
        for (int j = 0; j < rowSize; ++j)
        {
            accum[j] = 0.0;
        }
        for (int j = 0; j < numFiles; ++j)
        {
            ciftiList[j]->getRow(myrow.data(), i);
            if (weightsPtr == NULL)
            {
                for (int k = 0; k < rowSize; ++k)
                {
                    if (MathFunctions::isNumeric(myrow[k]))
                    {
                        weightaccum[k] += 1.0;
                        accum[k] += myrow[k];
                    }
                }
            } else {
                float weight = (*weightsPtr)[j];
                for (int k = 0; k < rowSize; ++k)
                {
                    if (MathFunctions::isNumeric(myrow[k]))
                    {
                        weightaccum[k] += weight;
                        accum[k] += myrow[k] * weight;
                    }
                }
            }
        }
        for (int k = 0; k < rowSize; ++k)
        {
            if (weightaccum[k] != 0.0)
            {
                myrow[k] = accum[k] / weightaccum[k];
            } else {
                myrow[k] = 0.0f;
            }
        }
        ciftiOut->setRow(myrow.data(), i);
    }
}

AlgorithmCiftiAverage::AlgorithmCiftiAverage(ProgressObject* myProgObj, const vector<const CiftiFile*>& ciftiList,
                                             const float& sigmaBelow, const float& sigmaAbove,
                                             CiftiFile* ciftiOut, const std::vector<float>* weightsPtr): AbstractAlgorithm(myProgObj)
{
    LevelProgress myProgress(myProgObj);
    if (ciftiList.size() < 2)
    {
        throw AlgorithmException("fewer than 2 files specified with outlier exclusion");
    }
    if (weightsPtr != NULL && ciftiList.size() != weightsPtr->size())
    {
        throw AlgorithmException("number of weights doesn't match number of input cifti files");
    }
    CaretAssert(ciftiList[0] != NULL);
    CiftiXML baseXML = ciftiList[0]->getCiftiXML();
    if (baseXML.getNumberOfDimensions() != 2) throw AlgorithmException("cifti average currently only supports 2D files");
    int numRows = baseXML.getDimensionLength(CiftiXML::ALONG_COLUMN), rowSize = baseXML.getDimensionLength(CiftiXML::ALONG_ROW), numFiles = (int)ciftiList.size();
    for (int i = 1; i < numFiles; ++i)
    {
        CaretAssert(ciftiList[i] != NULL);
        if (!baseXML.approximateMatch(ciftiList[i]->getCiftiXML()))
        {
            throw AlgorithmException("cifti files do not match");
        }
    }
    bool haveWarned = false;
    ciftiOut->setCiftiXML(baseXML);
    vector<vector<float> > myrows(numFiles, vector<float>(rowSize));
    for (int i = 0; i < numRows; ++i)
    {
        for (int j = 0; j < numFiles; ++j)
        {
            ciftiList[j]->getRow(myrows[j].data(), i);
        }
        vector<float> outrow(rowSize);
        for (int k = 0; k < rowSize; ++k)
        {
            double accum = 0.0;
            double weightaccum = 0.0;
            int nonnumeric = 0;
            for (int j = 0; j < numFiles; ++j)
            {
                if (MathFunctions::isNumeric(myrows[j][k]))
                {
                    accum += myrows[j][k];
                } else {
                    ++nonnumeric;
                }
            }
            if (nonnumeric >= numFiles - 1)
            {
                if (!haveWarned)
                {
                    CaretLogWarning("found element where less than 2 files have numeric values");
                    haveWarned = true;
                }
                outrow[k] = 0.0f;
            } else {
                float mean = accum / (numFiles - nonnumeric);
                accum = 0.0;
                for (int j = 0; j < numFiles; ++j)
                {
                    if (MathFunctions::isNumeric(myrows[j][k]))
                    {
                        float temp = myrows[j][k] - mean;
                        accum += temp * temp;
                    }
                }
                float stdev = sqrt(accum / (numFiles - 1 - nonnumeric));
                float cutoffLow = mean - sigmaBelow * stdev;
                float cutoffHigh = mean + sigmaAbove * stdev;
                accum = 0.0;
                for (int j = 0; j < numFiles; ++j)
                {
                    if (myrows[j][k] > cutoffLow && myrows[j][k] < cutoffHigh)//implicitly excludes NaN and inf
                    {
                        if (weightsPtr != NULL)
                        {
                            float weight = (*weightsPtr)[j];
                            accum += myrows[j][k] * weight;
                            weightaccum += weight;
                        } else {
                            accum += myrows[j][k];
                            weightaccum += 1.0;
                        }
                    }
                }
                if (weightaccum != 0.0)
                {
                    outrow[k] = accum / weightaccum;
                } else {
                    outrow[k] = 0.0f;
                }
            }
        }
        ciftiOut->setRow(outrow.data(), i);
    }
}

float AlgorithmCiftiAverage::getAlgorithmInternalWeight()
{
    return 1.0f;//override this if needed, if the progress bar isn't smooth
}

float AlgorithmCiftiAverage::getSubAlgorithmWeight()
{
    //return AlgorithmInsertNameHere::getAlgorithmWeight();//if you use a subalgorithm
    return 0.0f;
}
