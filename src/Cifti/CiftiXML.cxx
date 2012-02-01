/*LICENSE_START*/
/*
 *  Copyright 1995-2011 Washington University School of Medicine
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
/*LICENSE_END*/

#include <cmath>
#include "CiftiXML.h"
#include "CiftiFileException.h"
#include "FloatMatrix.h"
#include "CaretAssert.h"

using namespace caret;
using namespace std;

int64_t CiftiXML::getSurfaceIndex(const int64_t node, const CiftiBrainModelElement* myElement) const
{
    if (myElement == NULL || myElement->m_modelType != CIFTI_MODEL_TYPE_SURFACE) return -1;
    if (node < 0 || node > (int64_t)myElement->m_surfaceNumberOfNodes) return -1;
    CaretAssertVectorIndex(myElement->m_nodeToIndexLookup, node);
    return myElement->m_nodeToIndexLookup[node];
}

int64_t CiftiXML::getColumnIndexForNode(const int64_t node, const StructureEnum::Enum structure) const
{
    return getSurfaceIndex(node, findSurfaceModel(m_colMap, structure));
}

int64_t CiftiXML::getRowIndexForNode(const int64_t node, const StructureEnum::Enum structure) const
{
    return getSurfaceIndex(node, findSurfaceModel(m_rowMap, structure));
}

int64_t CiftiXML::getVolumeIndex(const int64_t* ijk, const CiftiMatrixIndicesMapElement* myMap) const
{
    if (myMap == NULL || myMap->m_indicesMapToDataType != CIFTI_INDEX_TYPE_BRAIN_MODELS)
    {
        return -1;
    }
    const CiftiVolumeElement& myVol = m_root.m_matrices[0].m_volume[0];
    if (ijk[0] < 0 || ijk[0] >= (int64_t)myVol.m_volumeDimensions[0]) return -1;//some shortcuts to not search all the voxels on invalid coords
    if (ijk[1] < 0 || ijk[1] >= (int64_t)myVol.m_volumeDimensions[1]) return -1;
    if (ijk[2] < 0 || ijk[2] >= (int64_t)myVol.m_volumeDimensions[2]) return -1;
    for (int64_t i = 0; i < (int64_t)myMap->m_brainModels.size(); ++i)
    {
        if (myMap->m_brainModels[i].m_modelType == CIFTI_MODEL_TYPE_VOXELS)
        {
            const vector<voxelIndexType>& myVoxels = myMap->m_brainModels[i].m_voxelIndicesIJK;
            int64_t voxelArraySize = (int64_t)myVoxels.size();
            for (int64_t j = 0; j < voxelArraySize; j += 3)
            {
                if ((ijk[0] == (int64_t)myVoxels[j]) && (ijk[1] == (int64_t)myVoxels[j + 1]) && (ijk[2] == (int64_t)myVoxels[j + 2]))
                {
                    return myMap->m_brainModels[i].m_indexOffset + j / 3;
                }
            }
        }
    }
    return -1;
}

int64_t CiftiXML::getColumnIndexForVoxel(const int64_t* ijk) const
{
    return getVolumeIndex(ijk, m_colMap);
}

int64_t CiftiXML::getRowIndexForVoxel(const int64_t* ijk) const
{
    return getVolumeIndex(ijk, m_rowMap);
}

bool CiftiXML::getSurfaceMapping(vector<CiftiSurfaceMap>& mappingOut, const CiftiBrainModelElement* myModel) const
{
    if (myModel == NULL || myModel->m_modelType != CIFTI_MODEL_TYPE_SURFACE)
    {
        mappingOut.clear();
        return false;
    }
    int64_t mappingSize = (int64_t)myModel->m_indexCount;
    mappingOut.resize(mappingSize);
    if (myModel->m_nodeIndices.size() == 0)
    {
        for (int i = 0; i < mappingSize; ++i)
        {
            mappingOut[i].m_ciftiIndex = myModel->m_indexOffset + i;
            mappingOut[i].m_surfaceNode = i;
        }
    } else {
        for (int i = 0; i < mappingSize; ++i)
        {
            mappingOut[i].m_ciftiIndex = myModel->m_indexOffset + i;
            mappingOut[i].m_surfaceNode = myModel->m_nodeIndices[i];
        }
    }
    return true;
}

bool CiftiXML::getSurfaceMapForColumns(vector<CiftiSurfaceMap>& mappingOut, const StructureEnum::Enum structure) const
{
    return getSurfaceMapping(mappingOut, findSurfaceModel(m_colMap, structure));
}

bool CiftiXML::getSurfaceMapForRows(vector<CiftiSurfaceMap>& mappingOut, const StructureEnum::Enum structure) const
{
    return getSurfaceMapping(mappingOut, findSurfaceModel(m_rowMap, structure));
}

bool CiftiXML::getVolumeMapping(vector<CiftiVolumeMap>& mappingOut, const CiftiMatrixIndicesMapElement* myMap, int64_t myCount) const
{
    if (myMap == NULL || myMap->m_indicesMapToDataType != CIFTI_INDEX_TYPE_BRAIN_MODELS)
    {
        mappingOut.clear();
        return false;
    }
    mappingOut.resize(myCount);
    int64_t myIndex = 0;
    for (int64_t i = 0; i < (int64_t)myMap->m_brainModels.size(); ++i)
    {
        if (myMap->m_brainModels[i].m_modelType == CIFTI_MODEL_TYPE_VOXELS)
        {
            const vector<voxelIndexType>& myVoxels = myMap->m_brainModels[i].m_voxelIndicesIJK;
            int64_t voxelArraySize = (int64_t)myVoxels.size();
            int64_t modelOffset = myMap->m_brainModels[i].m_indexOffset;
            int64_t j1 = 0;
            for (int64_t j = 0; j < voxelArraySize; j += 3)
            {
                mappingOut[myIndex].m_ciftiIndex = modelOffset + j1;
                mappingOut[myIndex].m_ijk[0] = myVoxels[j];
                mappingOut[myIndex].m_ijk[1] = myVoxels[j + 1];
                mappingOut[myIndex].m_ijk[2] = myVoxels[j + 2];
                ++j1;
                ++myIndex;
            }
        }
    }
    return true;
}

bool CiftiXML::getVolumeMapForColumns(vector<CiftiVolumeMap>& mappingOut) const
{
    return getVolumeMapping(mappingOut, m_colMap, m_colVoxels);
}

bool CiftiXML::getVolumeMapForRows(vector<CiftiVolumeMap>& mappingOut) const
{
    return getVolumeMapping(mappingOut, m_rowMap, m_rowVoxels);
}

void CiftiXML::rootChanged()
{//and here is where the real work is done
    m_colMap = NULL;//first, invalidate everything
    m_colVoxels = 0;
    m_rowMap = NULL;
    m_rowVoxels = 0;
    if (m_root.m_matrices.size() == 0)
    {
        throw CiftiFileException("No matrices defined in cifti extension");
    }
    CiftiMatrixElement& myMatrix = m_root.m_matrices[0];//assume only one matrix
    int numMaps = (int)myMatrix.m_matrixIndicesMap.size();
    for (int i = 0; i < numMaps; ++i)
    {
        CiftiMatrixIndicesMapElement& myMap = myMatrix.m_matrixIndicesMap[i];
        int numDimensions = (int)myMap.m_appliesToMatrixDimension.size();
        for (int j = 0; j < numDimensions; ++j)
        {
            if (myMap.m_appliesToMatrixDimension[j] == 0)
            {
                if (m_rowMap != NULL)
                {
                    throw CiftiFileException("Multiple mappings on the same dimension not supported");
                }
                m_rowMap = &myMap;
                if (myMap.m_indicesMapToDataType == CIFTI_INDEX_TYPE_BRAIN_MODELS)
                {
                    int numModels = (int)myMap.m_brainModels.size();//over 2 billion models? unlikely
                    for (int k = 0; k < numModels; ++k)
                    {
                        if (myMap.m_brainModels[k].m_modelType == CIFTI_MODEL_TYPE_SURFACE)
                        {
                            myMap.m_brainModels[k].setupLookup();
                        }
                        if (myMap.m_brainModels[k].m_modelType == CIFTI_MODEL_TYPE_VOXELS)
                        {
                            m_rowVoxels += myMap.m_brainModels[k].m_indexCount;
                        }
                    }
                }
            }
            if (myMap.m_appliesToMatrixDimension[j] == 1)
            {
                if (m_colMap != NULL)
                {
                    throw CiftiFileException("Multiple mappings on the same dimension not supported");
                }
                m_colMap = &myMap;
                if (myMap.m_indicesMapToDataType == CIFTI_INDEX_TYPE_BRAIN_MODELS)
                {
                    int numModels = (int)myMap.m_brainModels.size();//over 2 billion models? unlikely
                    for (int k = 0; k < numModels; ++k)
                    {
                        if (myMap.m_brainModels[k].m_modelType == CIFTI_MODEL_TYPE_SURFACE)
                        {
                            myMap.m_brainModels[k].setupLookup();
                        }
                        if (myMap.m_brainModels[k].m_modelType == CIFTI_MODEL_TYPE_VOXELS)
                        {
                            m_colVoxels += myMap.m_brainModels[k].m_indexCount;
                        }
                    }
                }
            }
        }
    }
}

int64_t CiftiXML::getColumnSurfaceNumberOfNodes(const StructureEnum::Enum structure) const
{
    const CiftiBrainModelElement* myModel = findSurfaceModel(m_colMap, structure);
    if (myModel == NULL) return -1;
    return myModel->m_surfaceNumberOfNodes;
}

int64_t CiftiXML::getRowSurfaceNumberOfNodes(const StructureEnum::Enum structure) const
{
    const CiftiBrainModelElement* myModel = findSurfaceModel(m_rowMap, structure);
    if (myModel == NULL) return -1;
    return myModel->m_surfaceNumberOfNodes;
}

int64_t CiftiXML::getVolumeIndex(const float* xyz, const CiftiMatrixIndicesMapElement* myMap) const
{
    if (m_root.m_matrices.size() == 0)
    {
        return -1;
    }
    if (m_root.m_matrices[0].m_volume.size() == 0)
    {
        return -1;
    }
    const CiftiVolumeElement& myVol = m_root.m_matrices[0].m_volume[0];
    if (myVol.m_transformationMatrixVoxelIndicesIJKtoXYZ.size() == 0)
    {
        return -1;
    }
    const TransformationMatrixVoxelIndicesIJKtoXYZElement& myTrans = myVol.m_transformationMatrixVoxelIndicesIJKtoXYZ[0];//oh the humanity
    FloatMatrix myMatrix = FloatMatrix::zeros(4, 4);
    for (int i = 0; i < 3; ++i)//NEVER trust the fourth row of input, NEVER!
    {
        for (int j = 0; j < 4; ++j)
        {
            myMatrix[i][j] = myTrans.m_transform[i * 4 + j];
        }
    }
    switch (myTrans.m_unitsXYZ)
    {
        case NIFTI_UNITS_MM:
            break;
        case NIFTI_UNITS_METER:
            myMatrix *= 1000.0f;
            break;
        case NIFTI_UNITS_MICRON:
            myMatrix *= 0.001f;
            break;
        default:
            return -1;
    };
    myMatrix[3][3] = 1.0f;//i COULD do this by making a fake volume file, but that seems kinda hacky
    FloatMatrix toIndexSpace = myMatrix.inverse();//invert to convert the other direction
    FloatMatrix myCoord = FloatMatrix::zeros(4, 1);//column vector
    myCoord[0][0] = xyz[0];
    myCoord[1][0] = xyz[1];
    myCoord[2][0] = xyz[2];
    myCoord[3][0] = 1.0f;
    FloatMatrix myIndices = toIndexSpace * myCoord;//matrix multiply
    int64_t ijk[3];
    ijk[0] = floor(myIndices[0][0] + 0.5f);
    ijk[1] = floor(myIndices[1][0] + 0.5f);
    ijk[2] = floor(myIndices[2][0] + 0.5f);
    return getVolumeIndex(ijk, myMap);
}

int64_t CiftiXML::getColumnIndexForVoxelCoordinate(const float* xyz) const
{
    return getVolumeIndex(xyz, m_colMap);
}

int64_t CiftiXML::getRowIndexForVoxelCoordinate(const float* xyz) const
{
    return getVolumeIndex(xyz, m_rowMap);
}

int64_t CiftiXML::getTimestepIndex(const float seconds, const CiftiMatrixIndicesMapElement* myMap) const
{
    float myStep;
    if (!getTimestep(myStep, myMap))
    {
        return -1;
    }
    float rawIndex = seconds / myStep;
    int64_t ret = floor(rawIndex + 0.5f);
    if (ret < 0) return -1;//NOTE: ciftiXML doesn't know the size of a row/column, so doesn't know numberof timesteps, so we can't check that here
    return ret;
}

int64_t CiftiXML::getColumnIndexForTimepoint(const float seconds) const
{
    return getTimestepIndex(seconds, m_colMap);
}

int64_t CiftiXML::getRowIndexForTimepoint(const float seconds) const
{
    return getTimestepIndex(seconds, m_rowMap);
}

bool CiftiXML::getTimestep(float& seconds, const CiftiMatrixIndicesMapElement* myMap) const
{
    if (myMap == NULL)
    {
        return false;
    }
    if (myMap->m_indicesMapToDataType != CIFTI_INDEX_TYPE_TIME_POINTS)
    {
        return false;
    }
    switch (myMap->m_timeStepUnits)
    {
        case NIFTI_UNITS_SEC:
            seconds = myMap->m_timeStep;
            break;
        case NIFTI_UNITS_MSEC:
            seconds = myMap->m_timeStep * 0.001f;
            break;
        case NIFTI_UNITS_USEC:
            seconds = myMap->m_timeStep * 0.000001f;
            break;
        default:
            return false;
    };
    return true;
}

bool CiftiXML::getColumnTimestep(float& seconds) const
{
    return getTimestep(seconds, m_colMap);
}

bool CiftiXML::getRowTimestep(float& seconds) const
{
    return getTimestep(seconds, m_rowMap);
}

bool CiftiXML::getVolumeAttributesForPlumb(VolumeFile::OrientTypes orientOut[3], int64_t dimensionsOut[3], float originOut[3], float spacingOut[3]) const
{
    if (m_root.m_matrices.size() == 0)
    {
        return false;
    }
    if (m_root.m_matrices[0].m_volume.size() == 0)
    {
        return false;
    }
    const CiftiVolumeElement& myVol = m_root.m_matrices[0].m_volume[0];
    if (myVol.m_transformationMatrixVoxelIndicesIJKtoXYZ.size() == 0)
    {
        return false;
    }
    const TransformationMatrixVoxelIndicesIJKtoXYZElement& myTrans = myVol.m_transformationMatrixVoxelIndicesIJKtoXYZ[0];//oh the humanity
    FloatMatrix myMatrix = FloatMatrix::zeros(3, 4);//no fourth row
    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < 4; ++j)
        {
            myMatrix[i][j] = myTrans.m_transform[i * 4 + j];
        }
    }
    switch (myTrans.m_unitsXYZ)
    {
        case NIFTI_UNITS_MM:
            break;
        case NIFTI_UNITS_METER:
            myMatrix *= 1000.0f;
            break;
        case NIFTI_UNITS_MICRON:
            myMatrix *= 0.001f;
            break;
        default:
            return false;
    };
    dimensionsOut[0] = myVol.m_volumeDimensions[0];
    dimensionsOut[1] = myVol.m_volumeDimensions[1];
    dimensionsOut[2] = myVol.m_volumeDimensions[2];
    char axisUsed = 0;
    char indexUsed = 0;
    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < 3; ++j)
        {
            if (myMatrix[i][j] != 0.0f)
            {
                if (axisUsed & (1<<i))
                {
                    return false;
                }
                if (indexUsed & (1<<j))
                {
                    return false;
                }
                axisUsed &= (1<<i);
                indexUsed &= (1<<j);
                spacingOut[j] = myMatrix[i][j];
                originOut[j] = myMatrix[i][3];
                bool negative;
                if (myMatrix[i][j] > 0.0f)
                {
                    negative = true;
                } else {
                    negative = false;
                }
                switch (i)
                {
                case 0:
                    //left/right
                    orientOut[j] = (negative ? VolumeFile::RIGHT_TO_LEFT : VolumeFile::LEFT_TO_RIGHT);
                    break;
                case 1:
                    //forward/back
                    orientOut[j] = (negative ? VolumeFile::ANTERIOR_TO_POSTERIOR : VolumeFile::POSTERIOR_TO_ANTERIOR);
                    break;
                case 2:
                    //up/down
                    orientOut[j] = (negative ? VolumeFile::SUPERIOR_TO_INFERIOR : VolumeFile::INFERIOR_TO_SUPERIOR);
                    break;
                default:
                    //will never get called
                    break;
                };
            }
        }
    }
    return true;
}

bool CiftiXML::hasRowVolumeData() const
{
    return hasVolumeData(m_rowMap);
}

bool CiftiXML::hasColumnVolumeData() const
{
    return hasVolumeData(m_colMap);
}

bool CiftiXML::hasVolumeData(const CiftiMatrixIndicesMapElement* myMap) const
{
    if (m_root.m_matrices.size() == 0)
    {
        return false;
    }
    if (m_root.m_matrices[0].m_volume.size() == 0)
    {
        return false;
    }
    if (myMap == NULL || myMap->m_indicesMapToDataType != CIFTI_INDEX_TYPE_BRAIN_MODELS)
    {
        return false;
    }
    const CiftiVolumeElement& myVol = m_root.m_matrices[0].m_volume[0];
    if (myVol.m_transformationMatrixVoxelIndicesIJKtoXYZ.size() == 0)
    {
        return false;
    }
    for (int64_t i = 0; i < (int64_t)myMap->m_brainModels.size(); ++i)
    {
        if (myMap->m_brainModels[i].m_modelType == CIFTI_MODEL_TYPE_VOXELS)
        {
            return true;
        }
    }
    return false;
}

bool CiftiXML::hasColumnSurfaceData(const caret::StructureEnum::Enum structure) const
{
    return (findSurfaceModel(m_colMap, structure) != NULL);
}

bool CiftiXML::hasRowSurfaceData(const caret::StructureEnum::Enum structure) const
{
    return (findSurfaceModel(m_rowMap, structure) != NULL);
}

CiftiXML& CiftiXML::operator=(const CiftiXML& right)
{
    if (this == &right) return *this;
    m_root = right.m_root;
    rootChanged();
    return *this;
}

const CiftiBrainModelElement* CiftiXML::findSurfaceModel(const CiftiMatrixIndicesMapElement* myMap, StructureEnum::Enum structure) const
{
    if (myMap == NULL || myMap->m_indicesMapToDataType != CIFTI_INDEX_TYPE_BRAIN_MODELS) return NULL;
    const vector<CiftiBrainModelElement>& myModels = myMap->m_brainModels;
    int numModels = myModels.size();
    for (int i = 0; i < numModels; ++i)
    {
        if (myModels[i].m_modelType == CIFTI_MODEL_TYPE_SURFACE && myModels[i].m_brainStructure == structure)
        {
            return &(myModels[i]);
        }
    }
    return NULL;
}

const CiftiBrainModelElement* CiftiXML::findVolumeModel(const CiftiMatrixIndicesMapElement* myMap, StructureEnum::Enum structure) const
{
    if (myMap == NULL || myMap->m_indicesMapToDataType != CIFTI_INDEX_TYPE_BRAIN_MODELS) return NULL;
    const vector<CiftiBrainModelElement>& myModels = myMap->m_brainModels;
    int numModels = myModels.size();
    for (int i = 0; i < numModels; ++i)
    {
        if (myModels[i].m_modelType == CIFTI_MODEL_TYPE_VOXELS && myModels[i].m_brainStructure == structure)
        {
            return &(myModels[i]);
        }
    }
    return NULL;
}
