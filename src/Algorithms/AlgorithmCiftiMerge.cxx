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
    
    ret->addCiftiOutputParameter(1, "cifti-out", "output cifti file");
    
    ParameterComponent* ciftiOpt = ret->createRepeatableParameter(2, "-cifti", "specify an input cifti file");
    ciftiOpt->addCiftiParameter(1, "cifti-in", "a cifti file to use columns from");
    OptionalParameter* columnOpt = ciftiOpt->createOptionalParameter(2, "-column", "select a single column to use");
    columnOpt->addIntegerParameter(1, "column", "the column number (starting from 1)");
    
    ret->setHelpText(
        AString("Given input CIFTI files which have matching mappings along columns, and for which mappings along rows ") +
        "are the same type, all either time points, scalars, or labels, this command concatenates the specified columns horizontally (rows become longer).\n\n" +
        "Example: wb_command -cifti-merge out.dtseries.nii -cifti first.dtseries.nii -column 1 -cifti second.dtseries.nii\n\n" +
        "This example would take the first column from first.dtseries.nii and all columns from second.dtseries.nii, " +
        "and write these columns to out.dtseries.nii."
    );
    return ret;
}

void AlgorithmCiftiMerge::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    CiftiFile* ciftiOut = myParams->getOutputCifti(1);
    const vector<ParameterComponent*>& myInstances = *(myParams->getRepeatableParameterInstances(2));
    vector<const CiftiInterface*> ciftiList;
    vector<int64_t> indexList;
    int numCifti = (int)myInstances.size();
    for (int i = 0; i < numCifti; ++i)
    {
        ciftiList.push_back(myInstances[i]->getCifti(1));
        OptionalParameter* columnOpt = myInstances[i]->getOptionalParameter(2);
        if (columnOpt->m_present)
        {
            int64_t input = columnOpt->getInteger(1);
            if (input < 1) throw AlgorithmException("cifti column indexes must be 1 or greater");
            indexList.push_back(input - 1);
        } else {
            indexList.push_back(-1);
        }
    }
    AlgorithmCiftiMerge(myProgObj, ciftiList, indexList, ciftiOut);
}

AlgorithmCiftiMerge::AlgorithmCiftiMerge(ProgressObject* myProgObj, const vector<const CiftiInterface*>& ciftiList, const vector<int64_t>& indexList, CiftiFile* ciftiOut) : AbstractAlgorithm(myProgObj)
{
    LevelProgress myProgress(myProgObj);
    if (ciftiList.size() == 0)
    {
        throw AlgorithmException("no files specified");
    }
    CaretAssert(ciftiList.size() == indexList.size());
    CaretAssert(ciftiList[0] != NULL);
    CiftiXML baseXML = ciftiList[0]->getCiftiXML();
    int64_t rowSize = -1, maxRowSize = ciftiList[0]->getNumberOfColumns();
    if (indexList[0] < -1) throw AlgorithmException("found invalid (less than -1) index in indexList in AlgorithmCiftiMerge");
    if (indexList[0] == -1)
    {
        rowSize = ciftiList[0]->getNumberOfColumns();
    } else {
        if (indexList[0] >= ciftiList[0]->getNumberOfColumns()) throw AlgorithmException("index too large for input cifti #1");
        rowSize = 1;
    }
    for (int i = 1; i < (int)ciftiList.size(); ++i)
    {
        CaretAssert(ciftiList[i] != NULL);
        if (!baseXML.matchesForColumns(ciftiList[i]->getCiftiXML()) || baseXML.getRowMappingType() != ciftiList[i]->getCiftiXML().getRowMappingType())
        {
            throw AlgorithmException("cifti files do not match");
        }
        if (indexList[i] < -1) throw AlgorithmException("found invalid (less than -1) index in indexList in AlgorithmCiftiMerge");
        if (indexList[i] == -1)
        {
            rowSize += ciftiList[i]->getNumberOfColumns();
        } else {
            if (indexList[i] >= ciftiList[i]->getNumberOfColumns()) throw AlgorithmException("index too large for input cifti #" + AString::number(i + 1));
            rowSize += 1;
        }
        if (maxRowSize < ciftiList[i]->getNumberOfColumns()) maxRowSize = ciftiList[i]->getNumberOfColumns();
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
            if (indexList[i] == -1)
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
                    curInd += 1;
                }
            } else {
                const CiftiXML& myXML = ciftiList[i]->getCiftiXML();
                newXML.setMapNameForRowIndex(curInd, myXML.getMapNameForRowIndex(indexList[i]));
                if (isLabel)
                {
                    newXML.setLabelTableForRowIndex(curInd, *(myXML.getLabelTableForRowIndex(indexList[i])));
                }
                curInd += 1;
            }
        }
    }
    ciftiOut->setCiftiXML(newXML);
    vector<float> rowscratch(rowSize), rowscratch2(maxRowSize);
    int64_t colSize = baseXML.getNumberOfRows();
    for (int64_t j = 0; j < colSize; ++j)
    {
        int64_t curoffset = 0;
        for (int i = 0; i < (int)ciftiList.size(); ++i)
        {
            if (indexList[i] == -1)
            {
                ciftiList[i]->getRow(rowscratch.data() + curoffset, j);
                curoffset += ciftiList[i]->getNumberOfColumns();
            } else {
                ciftiList[i]->getRow(rowscratch2.data(), j);
                rowscratch[curoffset] = rowscratch2[indexList[i]];
                ++curoffset;
            }
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
