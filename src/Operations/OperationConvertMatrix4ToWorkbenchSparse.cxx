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
    ret->addStringParameter(5, "voxel-list", "list of white matter voxel index triplets as used in the trajectory matrix");
    ret->addStringParameter(6, "wb-sparse-out", "output - the output workbench sparse file");
    
    OptionalParameter* surfaceOpt = ret->createOptionalParameter(7, "-surface-seeds", "specify the surface seed space");
    surfaceOpt->addMetricParameter(1, "seed-roi", "metric roi file of all vertices used in the seed space");
    
    OptionalParameter* volumeOpt = ret->createOptionalParameter(8, "-volume-seeds", "specify the volume seed space");
    volumeOpt->addCiftiParameter(1, "cifti-template", "cifti file to use the volume mappings from");
    volumeOpt->addStringParameter(2, "direction", "dimension along the cifti file to take the mapping from, ROW or COLUMN");
    
    ret->setHelpText(
        AString("Converts the matrix 4 output of probtrackx to workbench sparse file format.  ") +
        "Exactly one of -surface-seeds and -volume-seeds must be specified."
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
    CiftiFile* orientationFile = myParams->getCifti(4);
    AString outFileName = myParams->getString(6);
    OxfordSparseThreeFile inFile(inFile1, inFile2, inFile3);
    const int64_t* sparseDims = inFile.getDimensions();
    OptionalParameter* surfaceOpt = myParams->getOptionalParameter(7);
    OptionalParameter* volumeOpt = myParams->getOptionalParameter(8);
    if (surfaceOpt->m_present == volumeOpt->m_present) throw OperationException("you must specify exactly one of -surface-seeds and -volume-seeds");//use == on booleans as xnor
    const CiftiXML& orientXML = orientationFile->getCiftiXML();
    if (orientXML.getMappingType(CiftiXML::ALONG_COLUMN) != CiftiMappingType::BRAIN_MODELS) throw OperationException("orientation file must have brain models mapping along column");
    CiftiXML myXML;
    myXML.setNumberOfDimensions(2);
    myXML.setMap(CiftiXML::ALONG_ROW, *(orientXML.getMap(CiftiXML::ALONG_COLUMN)));
    if (surfaceOpt->m_present)
    {
        MetricFile* myROI = surfaceOpt->getMetric(1);
        const float* roiData = myROI->getValuePointerForColumn(0);
        CiftiBrainModelsMap tempMap;
        tempMap.addSurfaceModel(myROI->getNumberOfNodes(), myROI->getStructure(), roiData);
        if (tempMap.getLength() != sparseDims[1])
        {
            throw OperationException("roi has a different number of selected vertices than the input matrix");
        }
        myXML.setMap(CiftiXML::ALONG_COLUMN, tempMap);
    }
    if (volumeOpt->m_present)
    {
        CiftiFile* myTemplate = volumeOpt->getCifti(1);
        AString directionName = volumeOpt->getString(2);
        int myDir = -1;
        if (directionName == "ROW")
        {
            myDir = CiftiXML::ALONG_ROW;
        } else if (directionName == "COLUMN")
        {
            myDir = CiftiXML::ALONG_COLUMN;
        } else {
            throw OperationException("direction must be ROW or COLUMN");
        }
        const CiftiXML& templateXML = myTemplate->getCiftiXML();
        if (templateXML.getMappingType(myDir) != CiftiMappingType::BRAIN_MODELS) throw OperationException("template cifti file must have brain models along specified direction");
        const CiftiBrainModelsMap& templateMap = templateXML.getBrainModelsMap(myDir);
        if (!templateMap.hasVolumeData()) throw OperationException("template cifti file has no volume data");
        vector<StructureEnum::Enum> surfList = templateMap.getSurfaceStructureList(), volList = templateMap.getVolumeStructureList();//since we only need the volume models, we don't need to loop through all models
        CiftiBrainModelsMap tempMap;
        tempMap.setVolumeSpace(templateMap.getVolumeSpace());
        for (int i = 0; i < (int)volList.size(); ++i)
        {
            vector<CiftiBrainModelsMap::VolumeMap> myMap = templateMap.getVolumeStructureMap(volList[i]);
            int64_t mapSize = (int64_t)myMap.size();
            vector<int64_t> ijkList;
            for (int64_t j = 0; j < mapSize; ++j)
            {
                ijkList.push_back(myMap[j].m_ijk[0]);
                ijkList.push_back(myMap[j].m_ijk[1]);
                ijkList.push_back(myMap[j].m_ijk[2]);
            }
            tempMap.addVolumeModel(volList[i], ijkList);
        }
        if (tempMap.getLength() != sparseDims[1]) throw OperationException("volume models in template cifti file do not match the dimension of the input file");
        myXML.setMap(CiftiXML::ALONG_COLUMN, tempMap);
    }
    CaretAssert(myXML.getDimensionLength(CiftiXML::ALONG_COLUMN) == sparseDims[1]);
    fstream voxelFile(voxelFileName.toLocal8Bit().constData(), fstream::in);
    if (!voxelFile.good())
    {
        throw OperationException("failed to open voxel list file for reading");
    }
    const CiftiBrainModelsMap& rowMap = myXML.getBrainModelsMap(CiftiXML::ALONG_ROW);//tested above, as orientationXML
    const int64_t* volDims = rowMap.getVolumeSpace().getDims();
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
    vector<int64_t> rowReorder(sparseDims[0], -1);
    for (int64_t i = 0; i < (int64_t)voxelIndices.size(); i += 3)
    {
        int64_t tempInd = rowMap.getIndexForVoxel(voxelIndices.data() + i);
        if (tempInd != -1)
        {
            rowReorder[i / 3] = tempInd;
        }
    }
    CaretSparseFileWriter mywriter(outFileName, myXML);//NOTE: CaretSparseFile has a different encoding of fibers, ALWAYS use getFibersRow, etc
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
