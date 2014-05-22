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
        "This example would take the first column from first.dtseries.nii, followed by all columns from second.dtseries.nii, " +
        "and write these columns to out.dtseries.nii."
    );
    return ret;
}

void AlgorithmCiftiMerge::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    CiftiFile* ciftiOut = myParams->getOutputCifti(1);
    const vector<ParameterComponent*>& myInstances = *(myParams->getRepeatableParameterInstances(2));
    vector<const CiftiFile*> ciftiList;
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

AlgorithmCiftiMerge::AlgorithmCiftiMerge(ProgressObject* myProgObj, const vector<const CiftiFile*>& ciftiList, const vector<int64_t>& indexList, CiftiFile* ciftiOut) : AbstractAlgorithm(myProgObj)
{
    LevelProgress myProgress(myProgObj);
    if (ciftiList.size() == 0)
    {
        throw AlgorithmException("no files specified");
    }
    CaretAssert(ciftiList.size() == indexList.size());
    CaretAssert(ciftiList[0] != NULL);
    const CiftiXML& baseXML = ciftiList[0]->getCiftiXML();
    if (baseXML.getNumberOfDimensions() != 2) throw AlgorithmException("only 2D cifti are supported");
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
        const CiftiXML& listXML = ciftiList[i]->getCiftiXML();
        if (!baseXML.getMap(CiftiXML::ALONG_COLUMN)->approximateMatch(*(listXML.getMap(CiftiXML::ALONG_COLUMN))) || baseXML.getMappingType(CiftiXML::ALONG_ROW) != listXML.getMappingType(CiftiXML::ALONG_ROW))
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
    switch (baseXML.getMappingType(CiftiXML::ALONG_ROW))
    {
        case CiftiMappingType::SERIES:
        {
            doLoop = false;
            newXML.getSeriesMap(CiftiXML::ALONG_ROW).setLength(rowSize);
            break;
        }
        case CiftiMappingType::SCALARS:
            newXML.getScalarsMap(CiftiXML::ALONG_ROW).setLength(rowSize);
            break;
        case CiftiMappingType::LABELS:
            isLabel = true;
            newXML.getLabelsMap(CiftiXML::ALONG_ROW).setLength(rowSize);
            break;
        default:
            throw AlgorithmException("cannot merge CIFTI files of this type with this command");
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
                if (isLabel)
                {
                    const CiftiLabelsMap& listLabelMap = myXML.getLabelsMap(CiftiXML::ALONG_ROW);
                    CiftiLabelsMap& newLabelMap = newXML.getLabelsMap(CiftiXML::ALONG_ROW);
                    for (int64_t j = 0; j < mySize; ++j)
                    {
                        newLabelMap.setMapName(curInd, listLabelMap.getMapName(j));
                        *(newLabelMap.getMapLabelTable(curInd)) = *(listLabelMap.getMapLabelTable(j));
                        curInd += 1;
                    }
                } else {
                    const CiftiScalarsMap& listScalarMap = myXML.getScalarsMap(CiftiXML::ALONG_ROW);
                    CiftiScalarsMap& newScalarMap = newXML.getScalarsMap(CiftiXML::ALONG_ROW);
                    for (int64_t j = 0; j < mySize; ++j)
                    {
                        newScalarMap.setMapName(curInd, listScalarMap.getMapName(j));
                        *(newScalarMap.getMapPalette(curInd)) = *(listScalarMap.getMapPalette(j));
                        curInd += 1;
                    }
                }
            } else {
                const CiftiXML& myXML = ciftiList[i]->getCiftiXML();
                if (isLabel)
                {
                    const CiftiLabelsMap& listLabelMap = myXML.getLabelsMap(CiftiXML::ALONG_ROW);
                    CiftiLabelsMap& newLabelMap = newXML.getLabelsMap(CiftiXML::ALONG_ROW);
                    newLabelMap.setMapName(curInd, listLabelMap.getMapName(indexList[i]));
                    *(newLabelMap.getMapLabelTable(curInd)) = *(listLabelMap.getMapLabelTable(indexList[i]));
                } else {
                    const CiftiScalarsMap& listScalarMap = myXML.getScalarsMap(CiftiXML::ALONG_ROW);
                    CiftiScalarsMap& newScalarMap = newXML.getScalarsMap(CiftiXML::ALONG_ROW);
                    newScalarMap.setMapName(curInd, listScalarMap.getMapName(indexList[i]));
                    *(newScalarMap.getMapPalette(curInd)) = *(listScalarMap.getMapPalette(indexList[i]));
                }
                curInd += 1;
            }
        }
    }
    ciftiOut->setCiftiXML(newXML);
    vector<float> rowscratch(rowSize), rowscratch2(maxRowSize);
    int64_t colSize = baseXML.getDimensionLength(CiftiXML::ALONG_COLUMN);
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
