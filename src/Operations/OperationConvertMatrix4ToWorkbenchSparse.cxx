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

#include "CaretHeap.h"
#include "CaretSparseFile.h"
#include "CiftiFile.h"
#include "OxfordSparseThreeFile.h"
#include "MetricFile.h"
#include "VolumeFile.h"

#include <cmath>
#include <map>
#include <vector>
#include <fstream>
#include <string>

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
    ret->addCiftiParameter(4, "orientation-file", "the .fiberTEMP.nii file this trajectory file applies to");
    ret->addStringParameter(5, "voxel-list", "list of voxel index triplets as used in the trajectory matrix");
    ret->addMetricParameter(6, "seed-roi", "metric roi file of all nodes used in the seed space");
    ret->addStringParameter(7, "wb-sparse-out", "output - the output workbench sparse file");
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
    AString voxelFileName = myParams->getString(5);
    MetricFile* myROI = myParams->getMetric(6);
    CiftiFile* orientationFile = myParams->getCifti(4);
    AString outFileName = myParams->getString(7);
    OxfordSparseThreeFile inFile(inFile1, inFile2, inFile3);
    const int64_t* sparseDims = inFile.getDimensions();
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
    int64_t volDims[3];
    vector<vector<float> > sform;
    CiftiXML myXML = orientationFile->getCiftiXML();
    myXML.getVolumeDimsAndSForm(volDims, sform);
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
    if (!voxelFile.eof())
    {
        voxelFile.clear();
        string mystring;
        while (getline(voxelFile, mystring))
        {
            if (AString(mystring.c_str()).trimmed() != "")
            {
                throw OperationException("found non-digit, non-whitespace characters: " + AString(mystring.c_str()));
            }
        }
    }
    if ((int64_t)voxelIndices.size() != sparseDims[0] * 3) throw OperationException("voxel list file contains the wrong number of voxels, expected " +
                                                                                    AString::number(sparseDims[0] * 3) + " integers, read " + AString::number(voxelIndices.size()));
    myXML.applyColumnMapToRows();
    myXML.resetColumnsToBrainModels();
    myXML.addSurfaceModelToColumns(numNodes, myROI->getStructure(), roiData);
    CaretAssert(myXML.getNumberOfRows() == sparseDims[1]);
    vector<vector<vector<int64_t> > > voxToOutSpace(volDims[0], vector<vector<int64_t> >(volDims[1], vector<int64_t>(volDims[2], -1)));//basically, a volume file of integers initialized to -1
    vector<CiftiVolumeMap> myMap;
    myXML.getVolumeMapForRows(myMap);
    for (int64_t i = 0; i < (int64_t)myMap.size(); ++i)
    {
        voxToOutSpace[myMap[i].m_ijk[0]][myMap[i].m_ijk[1]][myMap[i].m_ijk[2]] = myMap[i].m_ciftiIndex;//now, given voxel indices, we can get the correct output index
    }
    vector<int64_t> rowReorder(sparseDims[0], -1);
    for (int64_t i = 0; i < (int64_t)voxelIndices.size(); i += 3)
    {
        int64_t tempInd = voxToOutSpace[voxelIndices[i]][voxelIndices[i + 1]][voxelIndices[i + 2]];
        if (tempInd != -1)
        {
            rowReorder[i / 3] = tempInd;
        }
    }
    int64_t outDims[2] = { myXML.getNumberOfColumns(), myXML.getNumberOfRows() };
    CaretSparseFileWriter mywriter(outFileName, outDims, myXML);//NOTE: CaretSparseFile has a different encoding of fibers, ALWAYS use getFibersRow, etc
    vector<int64_t> indicesIn, indicesOut;//this method knows about sparseness, does sorting of indexes in order to avoid scanning full rows
    vector<FiberFractions> fibersIn, fibersOut;//can be slower if matrix isn't very sparse, but that is a problem for other reasons anyway
    CaretMinHeap<FiberFractions, int64_t> myHeap;//use our heap to do heapsort, rather than coding a struct for stl sort
    for (int64_t i = 0; i < sparseDims[1]; ++i)
    {
        inFile.getFibersRowSparse(i, indicesIn, fibersIn);
        size_t numNonzero = indicesIn.size();
        myHeap.reserve(numNonzero);
        for (size_t j = 0; j < numNonzero; ++j)
        {
            int64_t newIndex = rowReorder[indicesIn[j]];//reorder
            if (newIndex != -1)
            {
                myHeap.push(fibersIn[j], newIndex);//heapify
            }
        }
        indicesOut.resize(myHeap.size());
        fibersOut.resize(myHeap.size());
        int64_t curIndex = 0;
        while (!myHeap.isEmpty())
        {
            int64_t newIndex;
            fibersOut[curIndex] = myHeap.pop(&newIndex);
            indicesOut[curIndex] = newIndex;
            ++curIndex;
        }
        mywriter.writeFibersRowSparse(i, indicesOut, fibersOut);
    }
    mywriter.finish();
}
