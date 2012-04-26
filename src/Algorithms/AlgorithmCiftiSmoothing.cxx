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

#include "AlgorithmCiftiSmoothing.h"
#include "AlgorithmException.h"
#include "AlgorithmMetricSmoothing.h"
#include "AlgorithmVolumeSmoothing.h"
#include "CiftiFile.h"
#include "MetricFile.h"
#include "VolumeFile.h"
#include "SurfaceFile.h"
#include "AlgorithmCiftiSeparate.h"
#include "AlgorithmCiftiReplaceStructure.h"

using namespace caret;
using namespace std;

AString AlgorithmCiftiSmoothing::getCommandSwitch()
{
    return "-cifti-smoothing";
}

AString AlgorithmCiftiSmoothing::getShortDescription()
{
    return "SMOOTH A CIFTI FILE";
}

OperationParameters* AlgorithmCiftiSmoothing::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    ret->addCiftiParameter(1, "cifti", "the input cifti");
    ret->addDoubleParameter(2, "surface-kernel", "the sigma for the gaussian surface smoothing kernel, in mm");
    ret->addDoubleParameter(3, "volume-kernel", "the sigma for the gaussian volume smoothing kernel, in mm");
    ret->addStringParameter(4, "direction", "which dimension to smooth along, ROW or COLUMN");
    ret->addCiftiOutputParameter(5, "cifti-out", "the output cifti");
    
    OptionalParameter* leftSurfOpt = ret->createOptionalParameter(6, "-left-surface", "specify the left surface to use");
    leftSurfOpt->addSurfaceParameter(1, "surface", "the left surface file");
    
    OptionalParameter* rightSurfOpt = ret->createOptionalParameter(7, "-right-surface", "specify the right surface to use");
    rightSurfOpt->addSurfaceParameter(1, "surface", "the right surface file");
    
    OptionalParameter* cerebSurfaceOpt = ret->createOptionalParameter(8, "-cereb-surface", "specify the cerebellum surface to use");
    cerebSurfaceOpt->addSurfaceParameter(1, "surface", "the cerebellum surface file");
    
    ret->createOptionalParameter(9, "-fix-zeros-volume", "treat values of zero in the volume as missing data");
    
    ret->createOptionalParameter(10, "-fix-zeros-surface", "treat values of zero on the surface as missing data");
    
    ret->setHelpText(
        AString("The input cifti file must have a brain models mapping on the chosen dimension, columns for .dtseries, and ") +
        "either for .dconn.  The fix zeros options will treat values of zero as lack of data, " +
        "and not use that value when generating the smoothed values, but will fill zeros with extrapolated values."
    );
    return ret;
}

void AlgorithmCiftiSmoothing::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    CiftiFile* myCifti = myParams->getCifti(1);
    float surfKern = (float)myParams->getDouble(2);
    float volKern = (float)myParams->getDouble(3);
    AString directionName = myParams->getString(4);
    SmoothDirection myDir;
    if (directionName == "ROW")
    {
        myDir = ALONG_ROW;
    } else if (directionName == "COLUMN") {
        myDir = ALONG_COLUMN;
    } else {
        throw AlgorithmException("incorrect string for direction, use ROW or COLUMN");
    }
    CiftiFile* myCiftiOut = myParams->getOutputCifti(5);
    SurfaceFile* myLeftSurf = NULL, *myRightSurf = NULL, *myCerebSurf = NULL;
    OptionalParameter* leftSurfOpt = myParams->getOptionalParameter(6);
    if (leftSurfOpt->m_present)
    {
        myLeftSurf = leftSurfOpt->getSurface(1);
    }
    OptionalParameter* rightSurfOpt = myParams->getOptionalParameter(7);
    if (rightSurfOpt->m_present)
    {
        myRightSurf = rightSurfOpt->getSurface(1);
    }
    OptionalParameter* cerebSurfOpt = myParams->getOptionalParameter(8);
    if (cerebSurfOpt->m_present)
    {
        myCerebSurf = cerebSurfOpt->getSurface(1);
    }
    bool fixZerosVol = myParams->getOptionalParameter(9)->m_present;
    bool fixZerosSurf = myParams->getOptionalParameter(10)->m_present;
    AlgorithmCiftiSmoothing(myProgObj, myCifti, surfKern, volKern, myDir, myCiftiOut, myLeftSurf, myRightSurf, myCerebSurf, fixZerosVol, fixZerosSurf);
}

AlgorithmCiftiSmoothing::AlgorithmCiftiSmoothing(ProgressObject* myProgObj, const CiftiFile* myCifti, const float& surfKern, const float& volKern, SmoothDirection myDir, CiftiFile* myCiftiOut, const SurfaceFile* myLeftSurf, const SurfaceFile* myRightSurf, const SurfaceFile* myCerebSurf, bool fixZerosVol, bool fixZerosSurf) : AbstractAlgorithm(myProgObj)
{
    const CiftiXML& myXML = myCifti->getCiftiXML();
    vector<CiftiVolumeStructureMap> myVolMaps;
    if (myDir == ALONG_ROW)
    {
        if (myXML.getRowMappingType() != CIFTI_INDEX_TYPE_BRAIN_MODELS)
        {
            throw AlgorithmException("a row doesn't map to brain models, try column instead");
        }
        myXML.getVolumeParcelMapsForRows(myVolMaps);
        int surfNumNodes = myXML.getRowSurfaceNumberOfNodes(StructureEnum::CORTEX_LEFT);
        if (surfNumNodes != -1)
        {
            if (myLeftSurf == NULL || myLeftSurf->getNumberOfNodes() != surfNumNodes)
            {
                throw AlgorithmException("number of nodes doesn't match for left surface");
            }
        }
        surfNumNodes = myXML.getRowSurfaceNumberOfNodes(StructureEnum::CORTEX_RIGHT);
        if (surfNumNodes != -1)
        {
            if (myRightSurf == NULL || myRightSurf->getNumberOfNodes() != surfNumNodes)
            {
                throw AlgorithmException("number of nodes doesn't match for right surface");
            }
        }
        surfNumNodes = myXML.getRowSurfaceNumberOfNodes(StructureEnum::CEREBELLUM);
        if (surfNumNodes != -1)
        {
            if (myCerebSurf == NULL || myCerebSurf->getNumberOfNodes() != surfNumNodes)
            {
                throw AlgorithmException("number of nodes doesn't match for cerebellum surface");
            }
        }
    } else {
        if (myXML.getColumnMappingType() != CIFTI_INDEX_TYPE_BRAIN_MODELS)
        {
            throw AlgorithmException("a column doesn't map to brain models, try row instead");
        }
        myXML.getVolumeParcelMapsForColumns(myVolMaps);
        int surfNumNodes = myXML.getColumnSurfaceNumberOfNodes(StructureEnum::CORTEX_LEFT);
        if (surfNumNodes != -1)
        {
            if (myLeftSurf == NULL || myLeftSurf->getNumberOfNodes() != surfNumNodes)
            {
                throw AlgorithmException("number of nodes doesn't match for left surface");
            }
        }
        surfNumNodes = myXML.getColumnSurfaceNumberOfNodes(StructureEnum::CORTEX_RIGHT);
        if (surfNumNodes != -1)
        {
            if (myRightSurf == NULL || myRightSurf->getNumberOfNodes() != surfNumNodes)
            {
                throw AlgorithmException("number of nodes doesn't match for right surface");
            }
        }
        surfNumNodes = myXML.getColumnSurfaceNumberOfNodes(StructureEnum::CEREBELLUM);
        if (surfNumNodes != -1)
        {
            if (myCerebSurf == NULL || myCerebSurf->getNumberOfNodes() != surfNumNodes)
            {
                throw AlgorithmException("number of nodes doesn't match for right surface");
            }
        }
    }
    int numVolMaps = (int)myVolMaps.size();
    //ProgressObject* leftSurfSmooth = NULL, *rightSurfSmooth = NULL;
    //vector<ProgressObject*> volProgObjs;
    if (myProgObj != NULL)
    {//TODO: set this up depending on existence of mappings
        //leftSurfSmooth = myProgObj->addAlgorithm(AlgorithmMetricSmoothing::getAlgorithmWeight());
    }
    LevelProgress myProgress(myProgObj);
    myCiftiOut->setCiftiXML(myXML);
    int rowSize = myCifti->getNumberOfColumns();
    CaretArray<float> rowScratch(rowSize), rowScratch2(rowSize);
    if (myDir == ALONG_ROW)
    {
        int numToSmooth = (int)myCifti->getNumberOfRows();
        vector<CiftiSurfaceMap> myMap;
        if (myXML.getSurfaceMapForRows(myMap, StructureEnum::CORTEX_LEFT))
        {
            int numNodes = (int)myCifti->getRowSurfaceNumberOfNodes(StructureEnum::CORTEX_LEFT);
            int mapSize = (int)myMap.size();
            if (surfKern > 0.0f)
            {
                MetricFile input, output, roi;
                input.setNumberOfNodesAndColumns(numNodes, numToSmooth);
                roi.setNumberOfNodesAndColumns(numNodes, 1);
                roi.initializeColumn(0);
                for (int i = 0; i < mapSize; ++i)
                {
                    roi.setValue(myMap[i].m_surfaceNode, 0, 1.0f);
                }
                CaretArray<float> metricCol(numNodes);
                for (int col = 0; col < numToSmooth; ++col)
                {
                    myCifti->getRow(rowScratch, col);
                    for (int i = 0; i < mapSize; ++i)
                    {
                        metricCol[myMap[i].m_surfaceNode] = rowScratch[myMap[i].m_ciftiIndex];
                    }
                    input.setValuesForColumn(col, metricCol);
                }
                AlgorithmMetricSmoothing(NULL, myLeftSurf, &input, surfKern, &output, &roi, fixZerosSurf);
                for (int col = 0; col < numToSmooth; ++col)
                {
                    const float* outcol = output.getValuePointerForColumn(col);
                    for (int i = 0; i < mapSize; ++i)//don't need to copy the existing part of the row because this is the first step
                    {
                        rowScratch[myMap[i].m_ciftiIndex] = outcol[myMap[i].m_surfaceNode];
                    }
                    myCiftiOut->setRow(rowScratch, col);
                }
            } else {
                for (int col = 0; col < numToSmooth; ++col)
                {
                    myCifti->getRow(rowScratch, col);
                    myCiftiOut->setRow(rowScratch, col);
                }
            }
        }
        if (myXML.getSurfaceMapForRows(myMap, StructureEnum::CORTEX_RIGHT))
        {
            int numNodes = (int)myCifti->getRowSurfaceNumberOfNodes(StructureEnum::CORTEX_RIGHT);
            int mapSize = (int)myMap.size();
            if (surfKern > 0.0f)
            {
                MetricFile input, output, roi;
                input.setNumberOfNodesAndColumns(numNodes, numToSmooth);
                roi.setNumberOfNodesAndColumns(numNodes, 1);
                roi.initializeColumn(0);
                for (int i = 0; i < mapSize; ++i)
                {
                    roi.setValue(myMap[i].m_surfaceNode, 0, 1.0f);
                }
                CaretArray<float> metricCol(numNodes);
                for (int col = 0; col < numToSmooth; ++col)
                {
                    myCifti->getRow(rowScratch, col);
                    for (int i = 0; i < mapSize; ++i)
                    {
                        metricCol[myMap[i].m_surfaceNode] = rowScratch[myMap[i].m_ciftiIndex];
                    }
                    input.setValuesForColumn(col, metricCol);
                }
                AlgorithmMetricSmoothing(NULL, myRightSurf, &input, surfKern, &output, &roi, fixZerosSurf);
                for (int col = 0; col < numToSmooth; ++col)
                {
                    const float* outcol = output.getValuePointerForColumn(col);
                    myCiftiOut->getRow(rowScratch, col);//get the existing values so as to not overwrite them
                    for (int i = 0; i < mapSize; ++i)
                    {
                        rowScratch[myMap[i].m_ciftiIndex] = outcol[myMap[i].m_surfaceNode];
                    }
                    myCiftiOut->setRow(rowScratch, col);
                }
            } else {
                for (int col = 0; col < numToSmooth; ++col)
                {
                    myCifti->getRow(rowScratch, col);
                    myCiftiOut->getRow(rowScratch2, col);//ditto, though it shouldn't matter
                    for (int i = 0; i < mapSize; ++i)
                    {
                        rowScratch2[myMap[i].m_ciftiIndex] = rowScratch[myMap[i].m_ciftiIndex];
                    }
                    myCiftiOut->setRow(rowScratch2, col);
                }
            }
        }
        if (myXML.getSurfaceMapForRows(myMap, StructureEnum::CEREBELLUM))
        {
            int numNodes = (int)myCifti->getRowSurfaceNumberOfNodes(StructureEnum::CEREBELLUM);
            int mapSize = (int)myMap.size();
            if (surfKern > 0.0f)
            {
                MetricFile input, output, roi;
                input.setNumberOfNodesAndColumns(numNodes, numToSmooth);
                roi.setNumberOfNodesAndColumns(numNodes, 1);
                roi.initializeColumn(0);
                for (int i = 0; i < mapSize; ++i)
                {
                    roi.setValue(myMap[i].m_surfaceNode, 0, 1.0f);
                }
                CaretArray<float> metricCol(numNodes);
                for (int col = 0; col < numToSmooth; ++col)
                {
                    myCifti->getRow(rowScratch, col);
                    for (int i = 0; i < mapSize; ++i)
                    {
                        metricCol[myMap[i].m_surfaceNode] = rowScratch[myMap[i].m_ciftiIndex];
                    }
                    input.setValuesForColumn(col, metricCol);
                }
                AlgorithmMetricSmoothing(NULL, myCerebSurf, &input, surfKern, &output, &roi, fixZerosSurf);
                for (int col = 0; col < numToSmooth; ++col)
                {
                    const float* outcol = output.getValuePointerForColumn(col);
                    myCiftiOut->getRow(rowScratch, col);//get the existing values so as to not overwrite them
                    for (int i = 0; i < mapSize; ++i)
                    {
                        rowScratch[myMap[i].m_ciftiIndex] = outcol[myMap[i].m_surfaceNode];
                    }
                    myCiftiOut->setRow(rowScratch, col);
                }
            } else {
                for (int col = 0; col < numToSmooth; ++col)
                {
                    myCifti->getRow(rowScratch, col);
                    myCiftiOut->getRow(rowScratch2, col);//ditto, though it shouldn't matter
                    for (int i = 0; i < mapSize; ++i)
                    {
                        rowScratch2[myMap[i].m_ciftiIndex] = rowScratch[myMap[i].m_ciftiIndex];
                    }
                    myCiftiOut->setRow(rowScratch2, col);
                }
            }
        }
        int64_t myDims[3];
        vector<vector<float> > mySForm;
        if (myXML.getVolumeDimsAndSForm(myDims, mySForm))
        {
            for (int map = 0; map < numVolMaps; ++map)
            {
                int numVoxels = (int)myVolMaps[map].m_map.size();
                if (volKern > 0.0f)
                {
                    if (numVoxels > 0)
                    {//make a voxel bounding box to minimize memory usage
                        int extrema[6] = { myVolMaps[map].m_map[0].m_ijk[0],
                            myVolMaps[map].m_map[0].m_ijk[0],
                            myVolMaps[map].m_map[0].m_ijk[1],
                            myVolMaps[map].m_map[0].m_ijk[1],
                            myVolMaps[map].m_map[0].m_ijk[2],
                            myVolMaps[map].m_map[0].m_ijk[2]
                        };
                        for (int i = 1; i < numVoxels; ++i)
                        {
                            if (myVolMaps[map].m_map[i].m_ijk[0] < extrema[0]) extrema[0] = myVolMaps[map].m_map[i].m_ijk[0];
                            if (myVolMaps[map].m_map[i].m_ijk[0] > extrema[1]) extrema[1] = myVolMaps[map].m_map[i].m_ijk[0];
                            if (myVolMaps[map].m_map[i].m_ijk[1] < extrema[2]) extrema[2] = myVolMaps[map].m_map[i].m_ijk[1];
                            if (myVolMaps[map].m_map[i].m_ijk[1] > extrema[3]) extrema[3] = myVolMaps[map].m_map[i].m_ijk[1];
                            if (myVolMaps[map].m_map[i].m_ijk[2] < extrema[4]) extrema[4] = myVolMaps[map].m_map[i].m_ijk[2];
                            if (myVolMaps[map].m_map[i].m_ijk[2] > extrema[5]) extrema[5] = myVolMaps[map].m_map[i].m_ijk[2];
                        }
                        vector<int64_t> volDims, volDims2;
                        volDims.push_back(extrema[1] - extrema[0] + 1);
                        volDims.push_back(extrema[3] - extrema[2] + 1);
                        volDims.push_back(extrema[5] - extrema[4] + 1);
                        volDims2 = volDims;
                        volDims.push_back(rowSize);
                        VolumeFile input(volDims, mySForm), roi(volDims2, mySForm), output;
                        roi.setValueAllVoxels(0.0f);
                        for (int i = 0; i < numVoxels; ++i)
                        {
                            roi.setValue(1.0f, myVolMaps[map].m_map[i].m_ijk[0] - extrema[0], myVolMaps[map].m_map[i].m_ijk[1] - extrema[2], myVolMaps[map].m_map[i].m_ijk[2] - extrema[4]);
                        }
                        for (int subvol = 0; subvol < numToSmooth; ++subvol)
                        {
                            myCifti->getRow(rowScratch, subvol);
                            for (int i = 0; i < numVoxels; ++i)
                            {
                                input.setValue(rowScratch[myVolMaps[map].m_map[i].m_ciftiIndex], myVolMaps[map].m_map[i].m_ijk[0] - extrema[0], myVolMaps[map].m_map[i].m_ijk[1] - extrema[2], myVolMaps[map].m_map[i].m_ijk[2] - extrema[4], subvol);
                            }
                        }
                        AlgorithmVolumeSmoothing(NULL, &input, volKern, &output, &roi, fixZerosVol);
                        for (int subvol = 0; subvol < numToSmooth; ++subvol)
                        {
                            myCiftiOut->getRow(rowScratch, subvol);
                            for (int i = 0; i < numVoxels; ++i)
                            {
                                rowScratch[myVolMaps[map].m_map[i].m_ciftiIndex] = output.getValue(myVolMaps[map].m_map[i].m_ijk[0] - extrema[0], myVolMaps[map].m_map[i].m_ijk[1] - extrema[2], myVolMaps[map].m_map[i].m_ijk[2] - extrema[4], subvol);
                            }
                            myCiftiOut->setRow(rowScratch, subvol);
                        }
                    }
                } else {
                    for (int subvol = 0; subvol < numToSmooth; ++subvol)
                    {
                        myCifti->getRow(rowScratch, subvol);
                        myCiftiOut->getRow(rowScratch2, subvol);
                        for (int i = 0; i < numVoxels; ++i)
                        {
                            rowScratch2[myVolMaps[map].m_map[i].m_ciftiIndex] = rowScratch[myVolMaps[map].m_map[i].m_ciftiIndex];
                        }
                        myCiftiOut->setRow(rowScratch2, subvol);
                    }
                }
            }
        }
    } else {
        vector<CiftiSurfaceMap> myMap;
        if (myXML.getSurfaceMapForColumns(myMap, StructureEnum::CORTEX_LEFT))
        {
            int numNodes = (int)myCifti->getColumnSurfaceNumberOfNodes(StructureEnum::CORTEX_LEFT);
            int mapSize = (int)myMap.size();
            if (surfKern > 0.0f)
            {
                MetricFile input, output, roi;
                input.setNumberOfNodesAndColumns(numNodes, rowSize);
                roi.setNumberOfNodesAndColumns(numNodes, 1);
                roi.initializeColumn(0);
                for (int i = 0; i < mapSize; ++i)
                {
                    roi.setValue(myMap[i].m_surfaceNode, 0, 1.0f);
                }
                for (int index = 0; index < mapSize; ++index)
                {
                    myCifti->getRow(rowScratch, myMap[index].m_ciftiIndex);
                    for (int i = 0; i < rowSize; ++i)
                    {
                        input.setValue(myMap[index].m_surfaceNode, i, rowScratch[i]);
                    }
                }
                AlgorithmMetricSmoothing(NULL, myLeftSurf, &input, surfKern, &output, &roi, fixZerosSurf);
                for (int index = 0; index < mapSize; ++index)
                {
                    for (int i = 0; i < rowSize; ++i)
                    {
                        rowScratch[i] = output.getValue(myMap[index].m_surfaceNode, i);
                    }
                    myCiftiOut->setRow(rowScratch, myMap[index].m_ciftiIndex);
                }
            } else {
                for (int index = 0; index < mapSize; ++index)
                {
                    myCifti->getRow(rowScratch, myMap[index].m_ciftiIndex);
                    myCiftiOut->setRow(rowScratch, myMap[index].m_ciftiIndex);
                }
            }
        }
        if (myXML.getSurfaceMapForColumns(myMap, StructureEnum::CORTEX_RIGHT))
        {
            int numNodes = (int)myCifti->getColumnSurfaceNumberOfNodes(StructureEnum::CORTEX_RIGHT);
            int mapSize = (int)myMap.size();
            if (surfKern > 0.0f)
            {
                MetricFile input, output, roi;
                input.setNumberOfNodesAndColumns(numNodes, rowSize);
                roi.setNumberOfNodesAndColumns(numNodes, 1);
                roi.initializeColumn(0);
                for (int i = 0; i < mapSize; ++i)
                {
                    roi.setValue(myMap[i].m_surfaceNode, 0, 1.0f);
                }
                for (int index = 0; index < mapSize; ++index)
                {
                    myCifti->getRow(rowScratch, myMap[index].m_ciftiIndex);
                    for (int i = 0; i < rowSize; ++i)
                    {
                        input.setValue(myMap[index].m_surfaceNode, i, rowScratch[i]);
                    }
                }
                AlgorithmMetricSmoothing(NULL, myRightSurf, &input, surfKern, &output, &roi, fixZerosSurf);
                for (int index = 0; index < mapSize; ++index)
                {//don't need to preload the row because we set the entire row
                    for (int i = 0; i < rowSize; ++i)
                    {
                        rowScratch[i] = output.getValue(myMap[index].m_surfaceNode, i);
                    }
                    myCiftiOut->setRow(rowScratch, myMap[index].m_ciftiIndex);
                }
            } else {
                for (int index = 0; index < mapSize; ++index)
                {
                    myCifti->getRow(rowScratch, myMap[index].m_ciftiIndex);
                    myCiftiOut->setRow(rowScratch, myMap[index].m_ciftiIndex);
                }
            }
        }
        if (myXML.getSurfaceMapForColumns(myMap, StructureEnum::CEREBELLUM))
        {
            int numNodes = (int)myCifti->getColumnSurfaceNumberOfNodes(StructureEnum::CEREBELLUM);
            int mapSize = (int)myMap.size();
            if (surfKern > 0.0f)
            {
                MetricFile input, output, roi;
                input.setNumberOfNodesAndColumns(numNodes, rowSize);
                roi.setNumberOfNodesAndColumns(numNodes, 1);
                roi.initializeColumn(0);
                for (int i = 0; i < mapSize; ++i)
                {
                    roi.setValue(myMap[i].m_surfaceNode, 0, 1.0f);
                }
                for (int index = 0; index < mapSize; ++index)
                {
                    myCifti->getRow(rowScratch, myMap[index].m_ciftiIndex);
                    for (int i = 0; i < rowSize; ++i)
                    {
                        input.setValue(myMap[index].m_surfaceNode, i, rowScratch[i]);
                    }
                }
                AlgorithmMetricSmoothing(NULL, myCerebSurf, &input, surfKern, &output, &roi, fixZerosSurf);
                for (int index = 0; index < mapSize; ++index)
                {//don't need to preload the row because we set the entire row
                    for (int i = 0; i < rowSize; ++i)
                    {
                        rowScratch[i] = output.getValue(myMap[index].m_surfaceNode, i);
                    }
                    myCiftiOut->setRow(rowScratch, myMap[index].m_ciftiIndex);
                }
            } else {
                for (int index = 0; index < mapSize; ++index)
                {
                    myCifti->getRow(rowScratch, myMap[index].m_ciftiIndex);
                    myCiftiOut->setRow(rowScratch, myMap[index].m_ciftiIndex);
                }
            }
        }
        int64_t myDims[3];
        vector<vector<float> > mySForm;
        if (myXML.getVolumeDimsAndSForm(myDims, mySForm))
        {
            for (int map = 0; map < numVolMaps; ++map)
            {
                int numVoxels = (int)myVolMaps[map].m_map.size();
                if (volKern > 0.0f)
                {
                    if (numVoxels > 0)
                    {//make a voxel bounding box to minimize memory usage
                        int extrema[6] = { myVolMaps[map].m_map[0].m_ijk[0],
                            myVolMaps[map].m_map[0].m_ijk[0],
                            myVolMaps[map].m_map[0].m_ijk[1],
                            myVolMaps[map].m_map[0].m_ijk[1],
                            myVolMaps[map].m_map[0].m_ijk[2],
                            myVolMaps[map].m_map[0].m_ijk[2]
                        };
                        for (int i = 1; i < numVoxels; ++i)
                        {
                            if (myVolMaps[map].m_map[i].m_ijk[0] < extrema[0]) extrema[0] = myVolMaps[map].m_map[i].m_ijk[0];
                            if (myVolMaps[map].m_map[i].m_ijk[0] > extrema[1]) extrema[1] = myVolMaps[map].m_map[i].m_ijk[0];
                            if (myVolMaps[map].m_map[i].m_ijk[1] < extrema[2]) extrema[2] = myVolMaps[map].m_map[i].m_ijk[1];
                            if (myVolMaps[map].m_map[i].m_ijk[1] > extrema[3]) extrema[3] = myVolMaps[map].m_map[i].m_ijk[1];
                            if (myVolMaps[map].m_map[i].m_ijk[2] < extrema[4]) extrema[4] = myVolMaps[map].m_map[i].m_ijk[2];
                            if (myVolMaps[map].m_map[i].m_ijk[2] > extrema[5]) extrema[5] = myVolMaps[map].m_map[i].m_ijk[2];
                        }
                        vector<int64_t> volDims, volDims2;
                        volDims.push_back(extrema[1] - extrema[0] + 1);
                        volDims.push_back(extrema[3] - extrema[2] + 1);
                        volDims.push_back(extrema[5] - extrema[4] + 1);
                        volDims2 = volDims;
                        volDims.push_back(rowSize);
                        VolumeFile input(volDims, mySForm), roi(volDims2, mySForm), output;
                        roi.setValueAllVoxels(0.0f);
                        for (int i = 0; i < numVoxels; ++i)
                        {
                            roi.setValue(1.0f, myVolMaps[map].m_map[i].m_ijk[0] - extrema[0], myVolMaps[map].m_map[i].m_ijk[1] - extrema[2], myVolMaps[map].m_map[i].m_ijk[2] - extrema[4]);
                        }
                        for (int index = 0; index < numVoxels; ++index)
                        {
                            myCifti->getRow(rowScratch, myVolMaps[map].m_map[index].m_ciftiIndex);
                            for (int subvol = 0; subvol < rowSize; ++subvol)
                            {
                                input.setValue(rowScratch[subvol], myVolMaps[map].m_map[index].m_ijk[0] - extrema[0], myVolMaps[map].m_map[index].m_ijk[1] - extrema[2], myVolMaps[map].m_map[index].m_ijk[2] - extrema[4], subvol);
                            }
                        }
                        AlgorithmVolumeSmoothing(NULL, &input, volKern, &output, &roi, fixZerosVol);
                        for (int index = 0; index < numVoxels; ++index)
                        {
                            for (int subvol = 0; subvol < rowSize; ++subvol)
                            {
                                rowScratch[subvol] = output.getValue(myVolMaps[map].m_map[index].m_ijk[0] - extrema[0], myVolMaps[map].m_map[index].m_ijk[1] - extrema[2], myVolMaps[map].m_map[index].m_ijk[2] - extrema[4], subvol);
                            }
                            myCiftiOut->setRow(rowScratch, myVolMaps[map].m_map[index].m_ciftiIndex);
                        }
                    }
                } else {
                    for (int index = 0; index < numVoxels; ++index)
                    {
                        myCifti->getRow(rowScratch, myVolMaps[map].m_map[index].m_ciftiIndex);
                        myCiftiOut->setRow(rowScratch, myVolMaps[map].m_map[index].m_ciftiIndex);
                    }
                }
            }
        }
    }
}

float AlgorithmCiftiSmoothing::getAlgorithmInternalWeight()
{
    return 1.0f;//override this if needed, if the progress bar isn't smooth
}

float AlgorithmCiftiSmoothing::getSubAlgorithmWeight()
{
    return AlgorithmMetricSmoothing::getAlgorithmWeight() * 2 + AlgorithmVolumeSmoothing::getAlgorithmWeight();//if you use a subalgorithm
}
