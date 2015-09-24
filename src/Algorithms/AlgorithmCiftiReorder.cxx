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

#include "AlgorithmCiftiReorder.h"
#include "AlgorithmException.h"

#include "CiftiFile.h"
#include "FileInformation.h"
#include "GiftiLabelTable.h"
#include "PaletteColorMapping.h"

#include <fstream>

using namespace caret;
using namespace std;

AString AlgorithmCiftiReorder::getCommandSwitch()
{
    return "-cifti-reorder";
}

AString AlgorithmCiftiReorder::getShortDescription()
{
    return "REORDER THE PARCELS OR SCALAR/LABEL MAPS IN A CIFTI FILE";
}

OperationParameters* AlgorithmCiftiReorder::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    ret->addCiftiParameter(1, "cifti-in", "input cifti file");
    
    ret->addStringParameter(2, "direction", "which dimension to reorder along, ROW or COLUMN");
    
    ret->addStringParameter(3, "reorder-list", "a text file containing the desired order transformation");
    
    ret->addCiftiOutputParameter(4, "cifti-out", "the reordered cifti file");
    
    ret->setHelpText(
        AString("The mapping along the specified direction must be parcels, scalars, or labels.  ") +
        "For pscalar or ptseries, use COLUMN to reorder the parcels.  " +
        "For dlabel, use ROW.  " +
        "The <reorder-list> file must contain 1-based indices separated by whitespace (spaces, newlines, tabs, etc), with as many indices as <cifti-in> has along the specified dimension.  " +
        "These indices specify which current index should end up in that position, for instance, if the current order is 'A B C D', and the desired order is 'D A B C', the text file " +
        "should contain '4 1 2 3'."
    );
    return ret;
}

void AlgorithmCiftiReorder::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    CiftiFile* myCifti = myParams->getCifti(1);
    AString directionName = myParams->getString(2);
    int myDir;
    if (directionName == "ROW")
    {
        myDir = CiftiXMLOld::ALONG_ROW;
    } else if (directionName == "COLUMN") {
        myDir = CiftiXMLOld::ALONG_COLUMN;
    } else {
        throw AlgorithmException("incorrect string for direction, use ROW or COLUMN");
    }
    AString listFileName = myParams->getString(3);
    CiftiFile* myCiftiOut = myParams->getOutputCifti(4);
    FileInformation textFileInfo(listFileName);
    if (!textFileInfo.exists())
    {
        throw AlgorithmException("name list file doesn't exist");
    }
    fstream listFile(listFileName.toLocal8Bit().constData(), fstream::in);
    if (!listFile.good())
    {
        throw AlgorithmException("error reading name list file");
    }
    vector<int64_t> reorder;
    int64_t temp;
    while (listFile >> temp)
    {
        reorder.push_back(temp - 1);
    }
    AlgorithmCiftiReorder(myProgObj, myCifti, myDir, reorder, myCiftiOut);
}

AlgorithmCiftiReorder::AlgorithmCiftiReorder(ProgressObject* myProgObj, const CiftiFile* myCifti, const int& myDir, const vector<int64_t>& reorder, CiftiFile* myCiftiOut) : AbstractAlgorithm(myProgObj)
{
    LevelProgress myProgress(myProgObj);
    const CiftiXMLOld& myXML = myCifti->getCiftiXMLOld();
    CiftiXMLOld myOutXML = myXML;
    int64_t rowSize = myXML.getNumberOfColumns(), colSize = myXML.getNumberOfRows(), myDirSize;
    if (myDir == CiftiXMLOld::ALONG_ROW)
    {
        myDirSize = rowSize;
    } else if (myDir == CiftiXMLOld::ALONG_COLUMN) {
        myDirSize = colSize;
    } else {
        throw AlgorithmException("direction not supported by cifti reorder");
    }
    if ((int64_t)reorder.size() != myDirSize) throw AlgorithmException("reorder list has incorrect size, expected " + AString::number(myDirSize) + ", got " + AString::number(reorder.size()));
    vector<bool> used(myDirSize, false);
    for (int64_t i = 0; i < myDirSize; ++i)
    {
        int64_t val = reorder[i];
        if (val < 0 || val >= myDirSize) throw AlgorithmException("invalid index in reorder list");
        if (used[val]) throw AlgorithmException("index specified more than once in reorder list");
        used[val] = true;
    }
    IndicesMapToDataType myType = myXML.getMappingType(myDir);
    switch (myType)
    {
        case CIFTI_INDEX_TYPE_SCALARS:
        {
            for (int64_t i = 0; i < myDirSize; ++i)
            {
                myOutXML.setMapNameForIndex(myDir, i, myXML.getMapName(myDir, reorder[i]));
                *(myOutXML.getMapPalette(myDir, i)) = *(myXML.getMapPalette(myDir, reorder[i]));
                *(myOutXML.getMapMetadata(myDir, i)) = *(myXML.getMapMetadata(myDir, reorder[i]));
            }
            break;
        }
        case CIFTI_INDEX_TYPE_LABELS:
        {
            for (int64_t i = 0; i < myDirSize; ++i)
            {
                myOutXML.setMapNameForIndex(myDir, i, myXML.getMapName(myDir, reorder[i]));
                *(myOutXML.getMapLabelTable(myDir, i)) = *(myXML.getMapLabelTable(myDir, reorder[i]));
                *(myOutXML.getMapMetadata(myDir, i)) = *(myXML.getMapMetadata(myDir, reorder[i]));
            }
            break;
        }
        case CIFTI_INDEX_TYPE_PARCELS:
        {
            myOutXML.resetDirectionToParcels(myDir);
            vector<StructureEnum::Enum> mySurfs;
            myXML.getParcelSurfaceStructures(myDir, mySurfs);
            for (int i = 0; i < (int)mySurfs.size(); ++i)
            {
                myOutXML.addParcelSurface(myDir, myXML.getSurfaceNumberOfNodes(myDir, mySurfs[i]), mySurfs[i]);
            }
            vector<CiftiParcelElement> origParcels;
            myXML.getParcels(myDir, origParcels);
            for (int64_t i = 0; i < myDirSize; ++i)
            {
                myOutXML.addParcel(myDir, origParcels[reorder[i]]);
            }
            break;
        }
        default:
            throw AlgorithmException("mapping along specified direction must be scalars, labels, or parcels");
    }
    myCiftiOut->setCiftiXML(myOutXML);
    vector<float> scratchrow(rowSize);
    switch (myDir)
    {
        case CiftiXMLOld::ALONG_ROW:
        {
            vector<float> scratchrow2(rowSize);
            for (int64_t i = 0; i < colSize; ++i)
            {
                myCifti->getRow(scratchrow.data(), i);
                for (int64_t j = 0; j < rowSize; ++j)
                {
                    scratchrow2[j] = scratchrow[reorder[j]];
                }
                myCiftiOut->setRow(scratchrow2.data(), i);
            }
            break;
        }
        case CiftiXMLOld::ALONG_COLUMN:
        {
            for (int64_t i = 0; i < colSize; ++i)
            {
                myCifti->getRow(scratchrow.data(), reorder[i]);
                myCiftiOut->setRow(scratchrow.data(), i);
            }
            break;
        }
        default:
            CaretAssert(false);
    }
}

float AlgorithmCiftiReorder::getAlgorithmInternalWeight()
{
    return 1.0f;//override this if needed, if the progress bar isn't smooth
}

float AlgorithmCiftiReorder::getSubAlgorithmWeight()
{
    //return AlgorithmInsertNameHere::getAlgorithmWeight();//if you use a subalgorithm
    return 0.0f;
}
