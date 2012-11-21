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

#include "AlgorithmCiftiMerge.h"
#include "AlgorithmException.h"
#include "CaretAssert.h"
#include "CaretPointer.h"
#include "CiftiFile.h"
#include "FileInformation.h"

#include <fstream>
#include <string>

using namespace caret;
using namespace std;

AString AlgorithmCiftiMerge::getCommandSwitch()
{
    return "-cifti-merge";
}

AString AlgorithmCiftiMerge::getShortDescription()
{
    return "MERGE CIFTI TIMESERIES, SCALAR, OR LABEL FILES";
}

OperationParameters* AlgorithmCiftiMerge::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    
    ret->addStringParameter(1, "cifti-list-file", "a text file containing a list of cifti filenames to concatenate");
    
    ret->addCiftiOutputParameter(2, "cifti-out", "output cifti file");
    
    ret->setHelpText(
        AString("Given a list of CIFTI files which have matching mappings along columns, and for which mappings along rows ") +
        "are the same type, all either time points, scalars, or labels, this command concatenates them horizontally (rows become longer)."
    );
    return ret;
}

void AlgorithmCiftiMerge::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
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
    AlgorithmCiftiMerge(myProgObj, ciftiList, ciftiOut);
}

AlgorithmCiftiMerge::AlgorithmCiftiMerge(ProgressObject* myProgObj, const vector<const CiftiInterface*>& ciftiList, CiftiFile* ciftiOut) : AbstractAlgorithm(myProgObj)
{
    LevelProgress myProgress(myProgObj);
    if (ciftiList.size() == 0)
    {
        throw AlgorithmException("no files specified");
    }
    CaretAssert(ciftiList[0] != NULL);
    CiftiXML baseXML = ciftiList[0]->getCiftiXML();
    int64_t rowSize = ciftiList[0]->getNumberOfColumns();
    for (int i = 1; i < (int)ciftiList.size(); ++i)
    {
        CaretAssert(ciftiList[i] != NULL);
        rowSize += ciftiList[i]->getNumberOfColumns();
        if (!baseXML.matchesForColumns(ciftiList[i]->getCiftiXML()) || baseXML.getRowMappingType() != ciftiList[i]->getCiftiXML().getRowMappingType())
        {
            throw AlgorithmException("cifti files do not match");
        }
    }
    CiftiXML newXML = baseXML;
    bool doLoop = true, isLabel = false;
    switch (baseXML.getRowMappingType())
    {
        case CIFTI_INDEX_TYPE_TIME_POINTS:
        {
            doLoop = false;
            float timestep;
            baseXML.getRowTimestep(timestep);
            newXML.resetRowsToTimepoints(timestep, rowSize);
            break;
        }
        case CIFTI_INDEX_TYPE_SCALARS:
            newXML.resetRowsToScalars(rowSize);
            break;
        case CIFTI_INDEX_TYPE_LABELS:
            isLabel = true;
            newXML.resetRowsToLabels(rowSize);
            break;
        default:
            throw AlgorithmException("cannot merge CIFTI files of this type");
    }
    if (doLoop)
    {
        int64_t curInd = 0;
        for (int i = 0; i < (int)ciftiList.size(); ++i)
        {
            int64_t mySize = ciftiList[i]->getNumberOfColumns();
            const CiftiXML& myXML = ciftiList[i]->getCiftiXML();
            for (int64_t j = 0; j < mySize; ++j)
            {
                newXML.setMapNameForRowIndex(curInd, myXML.getMapNameForRowIndex(j));
                if (isLabel)
                {
                    newXML.setLabelTableForRowIndex(curInd, *(myXML.getLabelTableForRowIndex(j)));
                }
            }
        }
    }
    ciftiOut->setCiftiXML(newXML);
    vector<float> rowscratch(rowSize);
    int64_t colSize = baseXML.getNumberOfRows();
    for (int64_t j = 0; j < colSize; ++j)
    {
        int64_t curoffset = 0;
        for (int i = 0; i < (int)ciftiList.size(); ++i)
        {
            ciftiList[i]->getRow(rowscratch.data() + curoffset, j);
            curoffset += ciftiList[i]->getNumberOfColumns();
        }
        ciftiOut->setRow(rowscratch.data(), j);
    }
}

float AlgorithmCiftiMerge::getAlgorithmInternalWeight()
{
    return 1.0f;//override this if needed, if the progress bar isn't smooth
}

float AlgorithmCiftiMerge::getSubAlgorithmWeight()
{
    //return AlgorithmInsertNameHere::getAlgorithmWeight();//if you use a subalgorithm
    return 0.0f;
}
