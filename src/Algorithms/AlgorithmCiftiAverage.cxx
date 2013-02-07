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

#include "AlgorithmCiftiAverage.h"
#include "AlgorithmException.h"
#include "CaretAssert.h"
#include "CaretPointer.h"
#include "CiftiFile.h"
#include "FileInformation.h"

#include <fstream>
#include <string>

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
    
    ParameterComponent* ciftiOpt = ret->createRepeatableParameter(2, "-cifti", "specify an input file");
    ciftiOpt->addCiftiParameter(1, "cifti-in", "the input cifti file");
    
    OptionalParameter* weightOpt = ciftiOpt->createOptionalParameter(2, "-weight", "give a weight for this file");
    weightOpt->addDoubleParameter(1, "weight", "the weight to use");
    
    ret->setHelpText(
        AString("Averages cifti files together.  Files without -weight specified are given a weight of 1.")
    );
    return ret;
}

void AlgorithmCiftiAverage::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    CiftiFile* ciftiOut = myParams->getOutputCifti(1);
    vector<const CiftiInterface*> ciftiList;//this is just so that it can pass them to the algorithm
    vector<float> weights;
    const vector<ParameterComponent*>& myInstances = *(myParams->getRepeatableParameterInstances(2));
    for (int i = 0; i < (int)myInstances.size(); ++i)
    {
        ciftiList.push_back(myInstances[i]->getCifti(1));
        OptionalParameter* weightOpt = myInstances[i]->getOptionalParameter(2);
        if (weightOpt->m_present)
        {
            weights.push_back((float)weightOpt->getDouble(1));
        } else {
            weights.push_back(1.0f);
        }
    }
    AlgorithmCiftiAverage(myProgObj, ciftiList, ciftiOut, &weights);//executes the algorithm
}

AlgorithmCiftiAverage::AlgorithmCiftiAverage(ProgressObject* myProgObj, const vector<const CiftiInterface*>& ciftiList, CiftiFile* ciftiOut, const vector<float>* weightsPtr) : AbstractAlgorithm(myProgObj)
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
    for (int i = 1; i < (int)ciftiList.size(); ++i)
    {
        CaretAssert(ciftiList[i] != NULL);
        if (baseXML != ciftiList[i]->getCiftiXML())
        {
            throw AlgorithmException("cifti files do not match");
        }
    }
    ciftiOut->setCiftiXML(baseXML);
    int numRows = baseXML.getNumberOfRows(), rowSize = baseXML.getNumberOfColumns(), numFiles = (int)ciftiList.size();
    vector<double> accum(rowSize);
    vector<float> myrow(rowSize);
    double weightaccum;
    for (int i = 0; i < numRows; ++i)
    {
        weightaccum = 0.0;
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
                    accum[k] += myrow[k];
                }
            } else {
                float weight = (*weightsPtr)[j];
                weightaccum += weight;
                for (int k = 0; k < rowSize; ++k)
                {
                    accum[k] += myrow[k] * weight;
                }
            }
        }
        if (weightsPtr == NULL)
        {
            for (int k = 0; k < rowSize; ++k)
            {
                myrow[k] = accum[k] / numFiles;
            }
        } else {
            for (int k = 0; k < rowSize; ++k)
            {
                myrow[k] = accum[k] / weightaccum;
            }
        }
        ciftiOut->setRow(myrow.data(), i);
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
