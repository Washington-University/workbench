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

#include "OperationConvertMatrix4ToWorkbenchSparse.h"
#include "OperationException.h"

#include "CaretSparseFile.h"
#include "OxfordSparseThreeFile.h"
#include "MetricFile.h"
#include "VolumeFile.h"

#include <cmath>
#include <map>
#include <vector>
#include <fstream>

using namespace caret;
using namespace std;

AString OperationConvertMatrix4ToWorkbenchSparse::getCommandSwitch()
{
    return "-convert-matrix4-to-workbench-sparse";
}

AString OperationConvertMatrix4ToWorkbenchSparse::getShortDescription()
{
    return "CONVERT A 3-FILE MATRIX4 TO A WORKBENCH SPARSE FILE";
}

OperationParameters* OperationConvertMatrix4ToWorkbenchSparse::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    ret->addStringParameter(1, "matrix4_1", "the first matrix4 file");
    ret->addStringParameter(2, "matrix4_2", "the second matrix4 file");
    ret->addStringParameter(3, "matrix4_3", "the third matrix4 file");
    ret->addVolumeParameter(4, "label-vol", "a label volume containing the desired structures for the white matter");
    ret->addStringParameter(5, "voxel-list", "list of voxel index triplets as used in the trajectory matrix");
    ret->addMetricParameter(6, "seed-roi", "metric roi file of all nodes used in the seed space");
    ret->addStringParameter(7, "wb-sparse-out", "the output workbench sparse file");
    ret->setHelpText(
        AString("This is where you set the help text.  DO NOT add the info about what the command line format is, ") +
        "and do not give the command switch, short description, or the short descriptions of parameters.  Do not indent, " +
        "add newlines, or format the text in any way other than to separate paragraphs within the help text prose."
    );
    return ret;
}

void OperationConvertMatrix4ToWorkbenchSparse::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    LevelProgress myProgress(myProgObj);
    AString inFile1 = myParams->getString(1);
    AString inFile2 = myParams->getString(2);
    AString inFile3 = myParams->getString(3);
    VolumeFile* labelVol = myParams->getVolume(4);
    AString voxelFileName = myParams->getString(5);
    MetricFile* myROI = myParams->getMetric(6);
    AString outFileName = myParams->getString(7);
    OxfordSparseThreeFile inFile(inFile1, inFile2, inFile3);
    const int64_t* sparseDims = inFile.getDimensions();
    if (labelVol->getType() != SubvolumeAttributes::LABEL)
    {
        throw OperationException("input volume is not a label volume");
    }
    int numNodes = myROI->getNumberOfNodes();
    int nodeCount = 0;
    const float* roiData = myROI->getValuePointerForColumn(0);
    for (int i = 0; i < numNodes; ++i)
    {
        if (roiData[i] > 0.0f) ++nodeCount;
    }
    if (nodeCount != sparseDims[1])
    {
        throw OperationException("roi has a different number of selected vertices than the input matrix");
    }
    fstream voxelFile(voxelFileName.toLocal8Bit().constData(), fstream::in);
    if (!voxelFile.good())
    {
        throw OperationException("failed to open voxel list file for reading");
    }
    vector<int64_t> volDims;
    labelVol->getDimensions(volDims);
    vector<int64_t> voxelIndices;
    int64_t ind1, ind2, ind3;
    while (voxelFile >> ind1 >> ind2 >> ind3)
    {
        if (min(min(ind1, ind2), ind3) < 0) throw OperationException("negative voxel index found in voxel list");
        if (ind1 >= volDims[0] || ind2 >= volDims[1] || ind3 >= volDims[2]) throw OperationException("found voxel index that exceeds dimension in voxel list");
        voxelIndices.push_back(ind1);
        voxelIndices.push_back(ind2);
        voxelIndices.push_back(ind3);
    }
    if ((int64_t)voxelIndices.size() != sparseDims[0] * 3) throw OperationException("voxel list file contains the wrong number of voxels");
    CiftiXML myXML;
    myXML.resetColumnsToBrainModels();
    myXML.resetRowsToBrainModels();
    myXML.addSurfaceModelToColumns(numNodes, myROI->getStructure(), roiData);
    myXML.setVolumeDimsAndSForm(volDims.data(), labelVol->getVolumeSpace());
    CaretAssert(myXML.getNumberOfRows() == sparseDims[1]);
    map<int, StructureEnum::Enum> labelToStruct;
    map<StructureEnum::Enum, pair<vector<voxelIndexType>, vector<int64_t> > > structToVoxList;
    GiftiLabelTable* myLabels = labelVol->getMapLabelTable(0);
    vector<int32_t> myKeys;
    myLabels->getKeys(myKeys);
    bool ok = false;
    for (int i = 0; i < (int)myKeys.size(); ++i)
    {
        StructureEnum::Enum myStruct = StructureEnum::fromName(myLabels->getLabelName(myKeys[i]), &ok);
        if (ok)
        {
            labelToStruct[myKeys[i]] = myStruct;
        }
    }
    for (int i = 0; i < (int)voxelIndices.size(); i += 3)
    {
        float myVal = labelVol->getValue(voxelIndices.data() + i);
        int labelKey = (int)floor(myVal + 0.5f);
        map<int, StructureEnum::Enum>::iterator myiter = labelToStruct.find(labelKey);
        if (myiter != labelToStruct.end())
        {
            vector<voxelIndexType>& myRef = structToVoxList[myiter->second].first;
            vector<int64_t>& myRef2 = structToVoxList[myiter->second].second;
            myRef.push_back(voxelIndices[i]);
            myRef.push_back(voxelIndices[i + 1]);
            myRef.push_back(voxelIndices[i + 2]);
            myRef2.push_back(i / 3);
        }
    }
    for (map<StructureEnum::Enum, pair<vector<voxelIndexType>, vector<int64_t> > >::iterator myiter = structToVoxList.begin(); myiter != structToVoxList.end(); ++myiter)
    {
        myXML.addVolumeModelToRows(myiter->second.first, myiter->first);
    }
    int64_t outDims[2] = { myXML.getNumberOfColumns(), myXML.getNumberOfRows() };
    vector<int64_t> rowReorderReverse(outDims[0]);
    int64_t curCount = 0;
    for (map<StructureEnum::Enum, pair<vector<voxelIndexType>, vector<int64_t> > >::iterator myiter = structToVoxList.begin(); myiter != structToVoxList.end(); ++myiter)
    {
        vector<int64_t>& myRef = myiter->second.second;
        for (int i = 0; i < (int)myRef.size(); ++i)
        {
            rowReorderReverse[curCount] = myRef[i];
            ++curCount;
        }
    }
    CaretAssert(curCount == outDims[0]);
    CaretSparseFileWriter mywriter(outFileName, outDims, myXML);
    vector<FiberFractions> inRow(sparseDims[0]), outRow(outDims[0]);
    for (int64_t i = 0; i < sparseDims[1]; ++i)
    {
        inFile.getFibersRow(inRow.data(), i);
        for (int64_t j = 0; j < outDims[0]; ++j)
        {
            if (inRow[rowReorderReverse[j]].totalCount > 0)
            {
                outRow[j] = inRow[rowReorderReverse[j]];
            } else {
                outRow[j].zero();
            }
        }
        mywriter.writeFibersRow(outRow.data(), i);
    }
    mywriter.finish();
}
