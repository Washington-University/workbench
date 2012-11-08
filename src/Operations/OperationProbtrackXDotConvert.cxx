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

#include "OperationProbtrackXDotConvert.h"
#include "OperationException.h"
#include "CaretLogger.h"
#include "CiftiFile.h"
#include "MetricFile.h"
#include "StructureEnum.h"
#include "VolumeFile.h"

#include <algorithm>
#include <fstream>
#include <map>
#include <vector>

using namespace caret;
using namespace std;

//specifically for the purpose of sorting the input .dot file to the order required, in case it isn't initially ordered correctly
struct SparseValue
{
    int32_t index[2];//save some memory
    float value;
    bool operator<(const SparseValue& rhs) const
    {
        return (index[1] < rhs.index[1]);//NOTE: this specifically avoids minor ordering by the other index to make the sort faster (less swapping because more "equals" cases)
    }
};

AString OperationProbtrackXDotConvert::getCommandSwitch()
{
    return "-probtrackx-dot-convert";
}

AString OperationProbtrackXDotConvert::getShortDescription()
{
    return "CONVERT A .DOT FILE FROM PROBTRACKX TO CIFTI";
}

OperationParameters* OperationProbtrackXDotConvert::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    ret->addStringParameter(1, "dot-file", "input .dot file");
    ret->addCiftiOutputParameter(2, "cifti-out", "output cifti file");
    
    OptionalParameter* rowVoxelOpt = ret->createOptionalParameter(3, "-row-voxels", "the output mapping along a row will be voxels");
    rowVoxelOpt->addStringParameter(1, "voxel-list-file", "a text file containing IJK indices for the voxels used");
    rowVoxelOpt->addVolumeParameter(2, "label-vol", "a label volume with the dimensions and sform used, with structure labels");
    
    OptionalParameter* rowSurfaceOpt = ret->createOptionalParameter(4, "-row-surface", "the output mapping along a row will be surface vertices");
    rowSurfaceOpt->addMetricParameter(1, "roi-metric", "a metric file with positive values on all nodes used");
    
    OptionalParameter* colVoxelOpt = ret->createOptionalParameter(5, "-col-voxels", "the output mapping along a row will be voxels");
    colVoxelOpt->addStringParameter(1, "voxel-list-file", "a text file containing IJK indices for the voxels used");
    colVoxelOpt->addVolumeParameter(2, "label-vol", "a label volume with the dimensions and sform used, with structure labels");
    
    OptionalParameter* colSurfaceOpt = ret->createOptionalParameter(6, "-col-surface", "the output mapping along a row will be surface vertices");
    colSurfaceOpt->addMetricParameter(1, "roi-metric", "a metric file with positive values on all nodes used");
    
    ret->createOptionalParameter(7, "-transpose", "transpose the input matrix");
    
    AString myText = AString("NOTE: exactly one -row option and one -col option must be used.\n\n") +
        "If the input file does not have its indexes sorted in the correct ordering, this command may take longer than expected.  " +
        "Specifying -transpose will transpose the input matrix before trying to put its values into the cifti file, which is currently needed for at least matrix2 " +
        "in order to display it as intended.  " +
        "How the cifti file is displayed is based on which -row option is specified: if -row-voxels is specified, then it will display data on volume slices.  " +
        "The label names in the label volume(s) must have the following names, other names are ignored:\n\n";
    vector<StructureEnum::Enum> myStructureEnums;
    StructureEnum::getAllEnums(myStructureEnums);
    for (int i = 0; i < (int)myStructureEnums.size(); ++i)
    {
        myText += "\n" + StructureEnum::toName(myStructureEnums[i]);
    }
    ret->setHelpText(myText);
    return ret;
}

void OperationProbtrackXDotConvert::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    LevelProgress myProgress(myProgObj);
    AString dotFileName = myParams->getString(1);
    CiftiFile* myCiftiOut = myParams->getOutputCifti(2);
    OptionalParameter* rowVoxelOpt = myParams->getOptionalParameter(3);
    OptionalParameter* rowSurfaceOpt = myParams->getOptionalParameter(4);
    OptionalParameter* colVoxelOpt = myParams->getOptionalParameter(5);
    OptionalParameter* colSurfaceOpt = myParams->getOptionalParameter(6);
    bool transpose = myParams->getOptionalParameter(7)->m_present;
    if (rowVoxelOpt->m_present == rowSurfaceOpt->m_present)//if both false or both true, basically using equals as a quick hack for xnor
    {
        throw OperationException("you must specify exactly one of -row-voxels and -row-surface");
    }
    if (colVoxelOpt->m_present == colSurfaceOpt->m_present)
    {
        throw OperationException("you must specify exactly one of -col-voxels and -col-surface");
    }
    CiftiXML myXML;
    myXML.resetRowsToBrainModels();
    myXML.resetColumnsToBrainModels();
    vector<int64_t> rowReorderMap, colReorderMap;
    if (rowVoxelOpt->m_present)
    {
        addVoxelMapping(rowVoxelOpt->getVolume(2), rowVoxelOpt->getString(1), myXML, rowReorderMap, CiftiXML::ALONG_ROW);
    } else {
        MetricFile* myMetric = rowSurfaceOpt->getMetric(1);
        myXML.addSurfaceModelToRows(myMetric->getNumberOfNodes(), myMetric->getStructure(), myMetric->getValuePointerForColumn(0));
    }
    if (colVoxelOpt->m_present)
    {
        addVoxelMapping(colVoxelOpt->getVolume(2), colVoxelOpt->getString(1), myXML, colReorderMap, CiftiXML::ALONG_COLUMN);
    } else {
        MetricFile* myMetric = colSurfaceOpt->getMetric(1);
        myXML.addSurfaceModelToColumns(myMetric->getNumberOfNodes(), myMetric->getStructure(), myMetric->getValuePointerForColumn(0));
    }
    fstream dotFile(dotFileName.toAscii().constData(), fstream::in);
    if (!dotFile.good())
    {
        throw OperationException("error opening text file '" + dotFileName + "'");
    }
    SparseValue tempValue;
    vector<SparseValue> dotFileContents;
    int32_t rowSize = myXML.getNumberOfColumns(), colSize = myXML.getNumberOfRows();
    if (transpose)
    {
        while (dotFile >> tempValue.index[1] >> tempValue.index[0] >> tempValue.value)//this is the only line that is different for transpose
        {
            if (tempValue.index[0] < 1 || tempValue.index[0] >  rowSize ||
                tempValue.index[1] < 1 || tempValue.index[1] > colSize)
            {
                throw OperationException("found invalid index pair in dot file: " + AString::number(tempValue.index[0]) + ", " + AString::number(tempValue.index[1]) +
                    ", perhaps you need to use -transpose");
            }
            tempValue.index[0] -= 1;//fix for 1-indexing
            tempValue.index[1] -= 1;
            dotFileContents.push_back(tempValue);
        }
    } else {
        while (dotFile >> tempValue.index[0] >> tempValue.index[1] >> tempValue.value)
        {
            if (tempValue.index[0] < 1 || tempValue.index[0] >  rowSize ||
                tempValue.index[1] < 1 || tempValue.index[1] > colSize)
            {
                throw OperationException("found invalid index pair in dot file: " + AString::number(tempValue.index[0]) + ", " + AString::number(tempValue.index[1]) +
                    ", perhaps you need to use -transpose");
            }
            tempValue.index[0] -= 1;//fix for 1-indexing
            tempValue.index[1] -= 1;
            dotFileContents.push_back(tempValue);
        }
    }
    bool sorted = true;
    int64_t numValues = (int64_t)dotFileContents.size();
    for (int64_t i = 1; i < numValues; ++i)
    {
        if (dotFileContents[i - 1].index[1] > dotFileContents[i].index[1])
        {
            sorted = false;
            CaretLogInfo("dot file indexes are not correctly sorted, sorting them may take a minute or so...");
            break;
        }
    }
    if (!sorted) sort(dotFileContents.begin(), dotFileContents.end());
    if (!sorted)
    {
        CaretLogInfo("sorting finished");
    }
    myCiftiOut->setCiftiXML(myXML);
    int64_t cur = 0, end = (int64_t)dotFileContents.size();
    vector<float> scratchRow(myXML.getNumberOfColumns(), 0.0f);
    int64_t whichRow = 0;//set all rows, in case initial allocation doesn't give a zeroed matrix
    while (whichRow < myXML.getNumberOfRows())
    {
        int64_t next = cur;
        while (next < end && dotFileContents[next].index[1] == whichRow) ++next;
        if (rowVoxelOpt->m_present)
        {
            for (int64_t i = cur; i < next; ++i) scratchRow[rowReorderMap[dotFileContents[i].index[0]]] = dotFileContents[i].value;
        } else {
            for (int64_t i = cur; i < next; ++i) scratchRow[dotFileContents[i].index[0]] = dotFileContents[i].value;
        }
        if (colVoxelOpt->m_present)
        {
            myCiftiOut->setRow(scratchRow.data(), colReorderMap[whichRow]);
        } else {
            myCiftiOut->setRow(scratchRow.data(), whichRow);
        }
        if (rowVoxelOpt->m_present)
        {
            for (int64_t i = cur; i < next; ++i) scratchRow[rowReorderMap[dotFileContents[i].index[0]]] = 0.0f;
        } else {
            for (int64_t i = cur; i < next; ++i) scratchRow[dotFileContents[i].index[0]] = 0.0f;
        }
        cur = next;
        ++whichRow;
    }
}

void OperationProbtrackXDotConvert::addVoxelMapping(const VolumeFile* myLabelVol, const AString& textFileName, CiftiXML& myXML, vector<int64_t>& reorderMapping, const int& direction)
{
    if (myLabelVol->getType() != SubvolumeAttributes::LABEL)
    {
        throw OperationException("specified volume for row voxels is not a label volume");
    }
    vector<int64_t> myDims;
    myLabelVol->getDimensions(myDims);
    myXML.setVolumeDimsAndSForm(myDims.data(), myLabelVol->getVolumeSpace());
    const GiftiLabelTable* myLabelTable = myLabelVol->getMapLabelTable(0);
    map<int, StructureEnum::Enum> labelMap;//maps label values to structures
    vector<vector<voxelIndexType> > voxelLists;//voxel lists for each volume component
    vector<vector<int64_t> > inputIndices;//index from the input space, matched to voxelLists
    map<StructureEnum::Enum, int> componentMap;//maps structures to indexes in voxelLists
    vector<int32_t> labelKeys;
    myLabelTable->getKeys(labelKeys);
    int64_t count = 0;
    for (int i = 0; i < (int)labelKeys.size(); ++i)
    {
        bool ok = false;
        StructureEnum::Enum thisStructure = StructureEnum::fromName(myLabelTable->getLabelName(labelKeys[i]), &ok);
        if (ok)
        {
            labelMap[labelKeys[i]] = thisStructure;
            if (componentMap.find(thisStructure) == componentMap.end())//make sure we don't already have this structure from another label
            {
                componentMap[thisStructure] = (int)count;
                ++count;
            }
        }
    }
    voxelLists.resize(count);
    inputIndices.resize(count);
    voxelIndexType vi, vj, vk;
    fstream myTextFile(textFileName.toAscii().constData(), fstream::in);
    if (!myTextFile.good())
    {
        throw OperationException("error opening text file '" + textFileName + "'");
    }
    count = 0;
    while (myTextFile >> vi >> vj >> vk)
    {
        if (!myLabelVol->indexValid(vi, vj, vk))
        {
            throw OperationException("invalid voxel index found in text file: " + AString::number(vi) + ", " +
                                                                                  AString::number(vj) + ", " +
                                                                                  AString::number(vk));
        }
        int myKey = (int)floor(myLabelVol->getValue(vi, vj, vk) + 0.5f);
        map<int, StructureEnum::Enum>::const_iterator myIter = labelMap.find(myKey);
        if (myIter != labelMap.end())
        {
            int myListIndex = componentMap[myIter->second];//will always exist
            voxelLists[myListIndex].push_back(vi);
            voxelLists[myListIndex].push_back(vj);
            voxelLists[myListIndex].push_back(vk);
            inputIndices[myListIndex].push_back(count);
            ++count;
        }
    }
    vector<int64_t> forwardMap;
    for (map<StructureEnum::Enum, int>::const_iterator iter = componentMap.begin(); iter != componentMap.end(); ++iter)
    {
        int i = iter->second;
        int64_t listSize = voxelLists[i].size();
        if (listSize != 0)
        {
            forwardMap.insert(forwardMap.end(), inputIndices[i].begin(), inputIndices[i].end());//append the structure's input index list, building lookup of new index->input index
            if (direction == CiftiXML::ALONG_ROW)
            {
                myXML.addVolumeModelToRows(voxelLists[i], iter->first);
            } else {
                myXML.addVolumeModelToColumns(voxelLists[i], iter->first);
            }
        }
    }
    int64_t reorderSize = (int64_t)forwardMap.size();
    reorderMapping.resize(reorderSize);
    for (int i = 0; i < reorderSize; ++i)
    {
        reorderMapping[forwardMap[i]] = i;//reverse it, building lookup from input index->new index
    }
}
