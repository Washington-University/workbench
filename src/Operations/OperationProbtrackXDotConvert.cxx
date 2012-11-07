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
        if (index[1] < rhs.index[1]) return true;//NOTE: this specifically avoids minor ordering by the other index to make the sort faster (less swapping because more "equals" cases)
        return false;//we do NOT need full ordering to do what we need
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
    
    OptionalParameter* rowVoxelOpt = ret->createOptionalParameter(3, "-row-voxels", "the mapping along a row is voxels");
    rowVoxelOpt->addStringParameter(1, "voxel-list-file", "a text file containing IJK indices for the voxels used");
    rowVoxelOpt->addVolumeParameter(2, "label-vol", "a label volume with the dimensions and sform used, with structure labels");
    
    OptionalParameter* rowSurfaceOpt = ret->createOptionalParameter(4, "-row-surface", "the mapping along a row is surface vertices");
    rowSurfaceOpt->addMetricParameter(1, "roi-metric", "a metric file with positive values on all nodes used");
    
    OptionalParameter* colVoxelOpt = ret->createOptionalParameter(5, "-col-voxels", "the mapping along a row is voxels");
    colVoxelOpt->addStringParameter(1, "voxel-list-file", "a text file containing IJK indices for the voxels used");
    colVoxelOpt->addVolumeParameter(2, "label-vol", "a label volume with the dimensions and sform used, with structure labels");
    
    OptionalParameter* colSurfaceOpt = ret->createOptionalParameter(6, "-col-surface", "the mapping along a row is surface vertices");
    colSurfaceOpt->addMetricParameter(1, "roi-metric", "a metric file with positive values on all nodes used");
    
    AString myText = AString("NOTE: exactly one -row option and one -col option must be used.\n\n") +
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
    if (rowVoxelOpt->m_present)
    {
        addVoxelMapping(rowVoxelOpt->getVolume(2), rowVoxelOpt->getString(1), myXML, CiftiXML::ALONG_ROW);
    } else {
        MetricFile* myMetric = rowSurfaceOpt->getMetric(1);
        myXML.addSurfaceModelToRows(myMetric->getNumberOfNodes(), myMetric->getStructure(), myMetric->getValuePointerForColumn(0));
    }
    if (colVoxelOpt->m_present)
    {
        addVoxelMapping(colVoxelOpt->getVolume(2), colVoxelOpt->getString(1), myXML, CiftiXML::ALONG_COLUMN);
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
    while (dotFile >> tempValue.index[0] >> tempValue.index[1] >> tempValue.value)
    {
        if (tempValue.index[0] < 1 || tempValue.index[0] >  rowSize||
            tempValue.index[1] < 1 || tempValue.index[1] > colSize)
        {
            throw OperationException("found invalid index pair in dot file: " + AString::number(tempValue.index[0]) + ", " + AString::number(tempValue.index[1]));
        }
        tempValue.index[0] -= 1;//fix for 1-indexing
        tempValue.index[1] -= 1;
        dotFileContents.push_back(tempValue);
    }
    bool sorted = true;
    int64_t numValues = (int64_t)dotFileContents.size();
    for (int64_t i = 1; i < numValues; ++i)
    {
        if (dotFileContents[i - 1].index[1] > dotFileContents[i].index[1])
        {
            sorted = false;
            CaretLogInfo("dot file indexes are not correctly sorted, sorting them will take some time...");
            break;
        }
    }
    if (!sorted) sort(dotFileContents.begin(), dotFileContents.end());
    myCiftiOut->setCiftiXML(myXML);
    int64_t cur = 0, end = (int64_t)dotFileContents.size();
    vector<float> scratchRow(myXML.getNumberOfColumns(), 0.0f);
    int64_t whichRow = 0;//set all rows, in case initial allocation doesn't give a zeroed matrix
    while (whichRow < myXML.getNumberOfRows())
    {
        int64_t next = cur;
        while (next < end && dotFileContents[next].index[1] == whichRow) ++next;
        for (int64_t i = cur; i < next; ++i) scratchRow[dotFileContents[i].index[0]] = dotFileContents[i].value;
        myCiftiOut->setRow(scratchRow.data(), whichRow);
        for (int64_t i = cur; i < next; ++i) scratchRow[dotFileContents[i].index[0]] = 0.0f;
        cur = next;
        ++whichRow;
    }
}

void OperationProbtrackXDotConvert::addVoxelMapping(const VolumeFile* myLabelVol, const AString& textFileName, CiftiXML& myXML, const int& direction)
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
    map<StructureEnum::Enum, int> componentMap;//maps structures to indexes in voxelLists
    vector<int32_t> labelKeys;
    myLabelTable->getKeys(labelKeys);
    int count = 0;
    for (int i = 0; i < (int)labelKeys.size(); ++i)
    {
        bool ok = false;
        StructureEnum::Enum thisStructure = StructureEnum::fromName(myLabelTable->getLabelName(labelKeys[i]), &ok);
        if (ok)
        {
            labelMap[labelKeys[i]] = thisStructure;
            if (componentMap.find(thisStructure) == componentMap.end())//make sure we don't already have this structure from another label
            {
                componentMap[thisStructure] = count;
                ++count;
            }
        }
    }
    voxelLists.resize(count);
    voxelIndexType vi, vj, vk;
    fstream myTextFile(textFileName.toAscii().constData(), fstream::in);
    if (!myTextFile.good())
    {
        throw OperationException("error opening text file '" + textFileName + "'");
    }
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
        }
    }
    for (map<StructureEnum::Enum, int>::const_iterator iter = componentMap.begin(); iter != componentMap.end(); ++iter)
    {
        int i = iter->second;
        if (voxelLists[i].size() != 0)
        {
            if (direction == CiftiXML::ALONG_ROW)
            {
                myXML.addVolumeModelToRows(voxelLists[i], iter->first);
            } else {
                myXML.addVolumeModelToColumns(voxelLists[i], iter->first);
            }
        }
    }
}
