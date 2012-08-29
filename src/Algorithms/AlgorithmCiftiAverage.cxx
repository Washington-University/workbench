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
    
    ret->addStringParameter(1, "cifti-list-file", "a text file containing a list of cifti filenames to average together");
    
    ret->addCiftiOutputParameter(2, "cifti-out", "output cifti file");
    
    OptionalParameter* weightOpt = ret->createOptionalParameter(3, "-weights", "use per-file weights");
    weightOpt->addStringParameter(1, "weight-list-file", "a text file containing one weight for each file in cifti-list-file");
    
    ret->setHelpText(
        AString("Averages cifti files together.")
    );
    return ret;
}

void AlgorithmCiftiAverage::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    AString listFileName = myParams->getString(1);
    CiftiFile* ciftiOut = myParams->getOutputCifti(2);
    fstream textFile(listFileName.toLocal8Bit().constData(), fstream::in);
    if (!textFile.good())
    {
        throw AlgorithmException("error opening input file for reading");
    }
    vector<CaretPointer<CiftiFile> > ciftiFileList;//use this for memory management in case something throws
    vector<const CiftiInterface*> ciftiList;//this is just so that it can pass them to the algorithm
    string myline;
    while (getline(textFile, myline))
    {
        if (myline == "") continue;
        FileInformation ciftiFileInfo(myline.c_str());
        if (!ciftiFileInfo.exists())
        {
            throw AlgorithmException(AString("file does not exist: ") + myline.c_str());
        }
        ciftiFileList.push_back(CaretPointer<CiftiFile>(new CiftiFile(myline.c_str(), ON_DISK)));
        ciftiList.push_back(ciftiFileList.back());
    }
    vector<float>* weightsPtr = NULL;
    vector<float> weights;
    OptionalParameter* weightOpt = myParams->getOptionalParameter(3);
    if (weightOpt->m_present)
    {
        weightsPtr = &weights;
        AString weightFileName = weightOpt->getString(1);
        fstream weightFile(weightFileName.toLocal8Bit().constData(), fstream::in);
        float weight;
        while (weightFile >> weight)
        {
            weights.push_back(weight);
        }
    }
    AlgorithmCiftiAverage(myProgObj, ciftiList, ciftiOut, weightsPtr);//executes the algorithm
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
