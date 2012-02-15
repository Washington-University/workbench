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
    ret->addSurfaceParameter(2, "left-surface", "the left surface to smooth on");
    ret->addSurfaceParameter(3, "right-surface", "the right surface to smooth on");
    ret->addDoubleParameter(4, "surface-kernel", "the sigma for the gaussian surface smoothing kernel, in mm");
    ret->addDoubleParameter(5, "volume-kernel", "the sigma for the gaussian volume smoothing kernel, in mm");
    ret->addStringParameter(6, "direction", "which dimension to smooth along, ROW or COLUMN");
    ret->addCiftiOutputParameter(7, "cifti-out", "the output cifti");
    
    ret->createOptionalParameter(8, "-fix-zeros-volume", "treat values of zero in the volume as missing data");
    
    ret->createOptionalParameter(9, "-fix-zeros-surface", "treat values of zero on the surface as missing data");
    
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
    SurfaceFile* myLeftSurf = myParams->getSurface(2), *myRightSurf = myParams->getSurface(3);
    float surfKern = (float)myParams->getDouble(4);
    float volKern = (float)myParams->getDouble(5);
    AString directionName = myParams->getString(6);
    SmoothDirection myDir;
    if (directionName == "ROW")
    {
        myDir = ALONG_ROW;
    } else if (directionName == "COLUMN") {
        myDir = ALONG_COLUMN;
    } else {
        throw AlgorithmException("incorrect string for direction, use ROW or COLUMN");
    }
    CiftiFile* myCiftiOut = myParams->getOutputCifti(7);
    bool fixZerosVol = myParams->getOptionalParameter(8)->m_present;
    bool fixZerosSurf = myParams->getOptionalParameter(9)->m_present;
    AlgorithmCiftiSmoothing(myProgObj, myCifti, myLeftSurf, myRightSurf, surfKern, volKern, myDir, myCiftiOut, fixZerosVol, fixZerosSurf);
}

AlgorithmCiftiSmoothing::AlgorithmCiftiSmoothing(ProgressObject* myProgObj, const CiftiFile* myCifti, const SurfaceFile* myLeftSurf, const SurfaceFile* myRightSurf, const float& surfKern, const float& volKern, SmoothDirection myDir, CiftiFile* myCiftiOut, bool fixZerosVol, bool fixZerosSurf) : AbstractAlgorithm(myProgObj)
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
    CaretArray<float> rowScratch(rowSize);
    if (myDir == ALONG_ROW)
    {
        int numToSmooth = (int)myCifti->getNumberOfRows();
        vector<CiftiSurfaceMap> myMap;
        if (myXML.getSurfaceMapForRows(myMap, StructureEnum::CORTEX_LEFT))
        {
            MetricFile input, output, roi;
            int numNodes = (int)myCifti->getRowSurfaceNumberOfNodes(StructureEnum::CORTEX_LEFT);
            input.setNumberOfNodesAndColumns(numNodes, numToSmooth);
            roi.setNumberOfNodesAndColumns(numNodes, 1);
            roi.initializeColumn(0);
            int mapSize = (int)myMap.size();
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
        }
        if (myXML.getSurfaceMapForRows(myMap, StructureEnum::CORTEX_RIGHT))
        {
            MetricFile input, output, roi;
            int numNodes = (int)myCifti->getRowSurfaceNumberOfNodes(StructureEnum::CORTEX_RIGHT);
            input.setNumberOfNodesAndColumns(numNodes, numToSmooth);
            roi.setNumberOfNodesAndColumns(numNodes, 1);
            roi.initializeColumn(0);
            int mapSize = (int)myMap.size();
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
        }
        int64_t myDims[3];
        vector<vector<float> > mySForm;
        if (myXML.getVolumeDimsAndSForm(myDims, mySForm))
        {
            vector<int64_t> volDims, volDims2;
            volDims.push_back(myDims[0]);
            volDims.push_back(myDims[1]);
            volDims.push_back(myDims[2]);
            volDims2 = volDims;
            volDims.push_back(numToSmooth);
            VolumeFile input(volDims, mySForm), roi(volDims2, mySForm), output;
            for (int map = 0; map < numVolMaps; ++map)
            {
                roi.setValueAllVoxels(0.0f);
                int numVoxels = (int)myVolMaps[map].m_map.size();
                for (int i = 0; i < numVoxels; ++i)
                {
                    roi.setValue(1.0f, myVolMaps[map].m_map[i].m_ijk);
                }
                for (int subvol = 0; subvol < numToSmooth; ++subvol)
                {
                    myCifti->getRow(rowScratch, subvol);
                    for (int i = 0; i < numVoxels; ++i)
                    {
                        input.setValue(rowScratch[myVolMaps[map].m_map[i].m_ciftiIndex], myVolMaps[map].m_map[i].m_ijk, subvol);
                    }
                }
                AlgorithmVolumeSmoothing(NULL, &input, volKern, &output, &roi, fixZerosVol);
                for (int subvol = 0; subvol < numToSmooth; ++subvol)
                {
                    myCiftiOut->getRow(rowScratch, subvol);
                    for (int i = 0; i < numVoxels; ++i)
                    {
                        rowScratch[myVolMaps[map].m_map[i].m_ciftiIndex] = output.getValue(myVolMaps[map].m_map[i].m_ijk, subvol);
                    }
                    myCiftiOut->setRow(rowScratch, subvol);
                }
            }
        }
    } else {
        vector<CiftiSurfaceMap> myMap;
        if (myXML.getSurfaceMapForColumns(myMap, StructureEnum::CORTEX_LEFT))
        {
            MetricFile input, output, roi;
            int numNodes = (int)myCifti->getColumnSurfaceNumberOfNodes(StructureEnum::CORTEX_LEFT);
            input.setNumberOfNodesAndColumns(numNodes, rowSize);
            roi.setNumberOfNodesAndColumns(numNodes, 1);
            roi.initializeColumn(0);
            int mapSize = (int)myMap.size();
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
        }
        if (myXML.getSurfaceMapForColumns(myMap, StructureEnum::CORTEX_RIGHT))
        {
            MetricFile input, output, roi;
            int numNodes = (int)myCifti->getColumnSurfaceNumberOfNodes(StructureEnum::CORTEX_RIGHT);
            input.setNumberOfNodesAndColumns(numNodes, rowSize);
            roi.setNumberOfNodesAndColumns(numNodes, 1);
            roi.initializeColumn(0);
            int mapSize = (int)myMap.size();
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
        }
        int64_t myDims[3];
        vector<vector<float> > mySForm;
        if (myXML.getVolumeDimsAndSForm(myDims, mySForm))
        {
            vector<int64_t> volDims, volDims2;
            volDims.push_back(myDims[0]);
            volDims.push_back(myDims[1]);
            volDims.push_back(myDims[2]);
            volDims2 = volDims;
            volDims.push_back(rowSize);
            VolumeFile input(volDims, mySForm), roi(volDims2, mySForm), output;
            for (int map = 0; map < numVolMaps; ++map)
            {
                roi.setValueAllVoxels(0.0f);
                int numVoxels = (int)myVolMaps[map].m_map.size();
                for (int i = 0; i < numVoxels; ++i)
                {
                    roi.setValue(1.0f, myVolMaps[map].m_map[i].m_ijk);
                }
                for (int index = 0; index < numVoxels; ++index)
                {
                    myCifti->getRow(rowScratch, myVolMaps[map].m_map[index].m_ciftiIndex);
                    for (int subvol = 0; subvol < rowSize; ++subvol)
                    {
                        input.setValue(rowScratch[subvol], myVolMaps[map].m_map[index].m_ijk, subvol);
                    }
                }
                AlgorithmVolumeSmoothing(NULL, &input, volKern, &output, &roi, fixZerosVol);
                for (int index = 0; index < numVoxels; ++index)
                {
                    for (int subvol = 0; subvol < rowSize; ++subvol)
                    {
                        rowScratch[subvol] = output.getValue(myVolMaps[map].m_map[index].m_ijk, subvol);
                    }
                    myCiftiOut->setRow(rowScratch, myVolMaps[map].m_map[index].m_ciftiIndex);
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
