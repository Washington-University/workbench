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

#include "CiftiXML.h"
#include "CiftiFileException.h"
#include "FloatMatrix.h"
#include <cmath>

using namespace caret;
using namespace std;

int64_t CiftiXML::getSurfaceIndex(const int64_t node, const CiftiBrainModelElement* myElement, const int64_t numContig) const
{
    if (myElement == NULL) return -1;
    if (node < numContig)//NOTE: this is how an unspecified node list of size of the entire surface gets mapped!
    {
        return myElement->m_indexOffset + node;
    }
    int64_t numIndices = (int64_t)myElement->m_nodeIndices.size();
    for (int64_t i = numContig; i < numIndices; ++i)
    {
        if ((int64_t)myElement->m_nodeIndices[i] == node)
        {
            return myElement->m_indexOffset + i;
        }
    }
    return -1;
}

int64_t CiftiXML::getColumnIndexForNode(const int64_t node, const StructureEnum::Enum structure) const
{
    if (m_colMap == NULL || m_colMap->m_indicesMapToDataType != CIFTI_INDEX_TYPE_BRAIN_MODELS)
    {
        return -1;
    }
    bool left = false;
    switch (structure)
    {
    case StructureEnum::CORTEX_LEFT:
        left = true;
        break;
    case StructureEnum::CORTEX_RIGHT:
        break;
    default:
        return -1;
    };
    if (left)
    {
        return getSurfaceIndex(node, m_colLeftSurfModel, m_colLeftSurfContig);
    } else {
        return getSurfaceIndex(node, m_colRightSurfModel, m_colRightSurfContig);
    }
}

int64_t CiftiXML::getRowIndexForNode(const int64_t node, const StructureEnum::Enum structure) const
{
    if (m_rowMap == NULL || m_rowMap->m_indicesMapToDataType != CIFTI_INDEX_TYPE_BRAIN_MODELS)
    {
        return -1;
    }
    bool left = false;
    switch (structure)
    {
    case StructureEnum::CORTEX_LEFT:
        left = true;
        break;
    case StructureEnum::CORTEX_RIGHT:
        break;
    default:
        return -1;
    };
    if (left)
    {
        return getSurfaceIndex(node, m_rowLeftSurfModel, m_rowLeftSurfContig);
    } else {
        return getSurfaceIndex(node, m_rowRightSurfModel, m_rowRightSurfContig);
    }
}

int64_t CiftiXML::getVolumeIndex(const int64_t* ijk, const CiftiMatrixIndicesMapElement* myMap) const
{
    if (myMap == NULL || myMap->m_indicesMapToDataType != CIFTI_INDEX_TYPE_BRAIN_MODELS)
    {
        return -1;
    }
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

bool CiftiXML::getSurfaceMapping(vector<CiftiSurfaceMap>& mappingOut, CiftiBrainModelElement* myModel, const int64_t numContig) const
{
    if (myModel == NULL)
    {
        mappingOut.clear();
        return false;
    }
    int64_t mappingSize = (int64_t)myModel->m_indexCount;
    mappingOut.resize(mappingSize);
    for (int64_t i = 0; i < mappingSize; ++i)
    {
        mappingOut[i].m_ciftiIndex = myModel->m_indexOffset + i;
        if (i < numContig)//NOTE: this is how an unspecified node list of size of the entire surface gets mapped!
        {
            mappingOut[i].m_surfaceNode = i;
        } else {
            mappingOut[i].m_surfaceNode = myModel->m_nodeIndices[i];
        }
    }
    return true;
}

bool CiftiXML::getSurfaceMapForColumns(vector<CiftiSurfaceMap>& mappingOut, const StructureEnum::Enum structure) const
{
    bool left = false;
    switch (structure)
    {
    case StructureEnum::CORTEX_LEFT:
        left = true;
        break;
    case StructureEnum::CORTEX_RIGHT:
        break;
    default:
        mappingOut.clear();
        return false;
    };
    if (left)
    {
        return getSurfaceMapping(mappingOut, m_colLeftSurfModel, m_colLeftSurfContig);
    } else {
        return getSurfaceMapping(mappingOut, m_colRightSurfModel, m_colRightSurfContig);
    }
}

bool CiftiXML::getSurfaceMapForRows(vector<CiftiSurfaceMap>& mappingOut, const StructureEnum::Enum structure) const
{
    bool left = false;
    switch (structure)
    {
    case StructureEnum::CORTEX_LEFT:
        left = true;
        break;
    case StructureEnum::CORTEX_RIGHT:
        break;
    default:
        mappingOut.clear();
        return false;
    };
    if (left)
    {
        return getSurfaceMapping(mappingOut, m_rowLeftSurfModel, m_rowLeftSurfContig);
    } else {
        return getSurfaceMapping(mappingOut, m_rowRightSurfModel, m_colRightSurfContig);
    }
}

bool CiftiXML::getVolumeMapping(vector<CiftiVolumeMap>& mappingOut, CiftiMatrixIndicesMapElement* myMap, int64_t myCount) const
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
            vector<voxelIndexType>& myVoxels = myMap->m_brainModels[i].m_voxelIndicesIJK;
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

bool CiftiXML::getVolumeMapForRows(vector< CiftiVolumeMap >& mappingOut) const
{
    return getVolumeMapping(mappingOut, m_rowMap, m_rowVoxels);
}

void CiftiXML::rootChanged()
{//and here is where the real work is done
    m_colMap = NULL;//first, invalidate everything
    m_colLeftSurfModel = NULL;
    m_colRightSurfModel = NULL;
    m_colLeftSurfNodes = -1;
    m_colRightSurfNodes = -1;
    m_colLeftSurfContig = 0;
    m_colRightSurfContig = 0;
    m_colVoxels = 0;
    m_rowMap = NULL;
    m_rowLeftSurfModel = NULL;
    m_rowRightSurfModel = NULL;
    m_rowLeftSurfNodes = -1;
    m_rowRightSurfNodes = -1;
    m_rowLeftSurfContig = 0;
    m_rowRightSurfContig = 0;
    m_rowVoxels = 0;
    if (m_root.m_matrices.size() == 0)
    {
        throw CiftiFileException("No matrices defined in cifti extension");
    }
    CiftiMatrixElement& myMatrix = m_root.m_matrices[0];//assume only one matrix
    int numMaps = (int)myMatrix.m_matrixIndicesMap.size();
    for (int i = 0; i < numMaps; ++i)
    {
        if (myMatrix.m_matrixIndicesMap[i].m_indicesMapToDataType == CIFTI_INDEX_TYPE_BRAIN_MODELS)
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
                    int numModels = (int)myMap.m_brainModels.size();//over 2 billion models? unlikely
                    for (int k = 0; k < numModels; ++k)
                    {
                        if (myMap.m_brainModels[k].m_modelType == CIFTI_MODEL_TYPE_SURFACE)
                        {
                            int64_t thisContig = myMap.m_brainModels[k].m_indexCount;//if the loop interior never executes (empty list) it is a special case that all are contiguous
                            for (int64_t m = 0; m < (int64_t)myMap.m_brainModels[k].m_indexCount; ++m)//similarly, if the loop never encounters not equals, all are correct
                            {
                                if (m != (int64_t)myMap.m_brainModels[k].m_nodeIndices[m])
                                {
                                    thisContig = m;
                                    break;
                                }
                            }
                            if (myMap.m_brainModels[k].m_brainStructure.endsWith("_LEFT"))
                            {
                                if (m_rowLeftSurfModel != NULL)
                                {
                                    throw CiftiFileException("Multiple surfaces of the same handedness not supported");
                                }
                                m_rowLeftSurfModel = &(myMap.m_brainModels[k]);
                                m_rowLeftSurfNodes = myMap.m_brainModels[k].m_surfaceNumberOfNodes;
                                m_rowLeftSurfContig = thisContig;
                            }
                            if (myMap.m_brainModels[k].m_brainStructure.endsWith("_RIGHT"))
                            {
                                if (m_rowRightSurfModel != NULL)
                                {
                                    throw CiftiFileException("Multiple surfaces of the same handedness not supported");
                                }
                                m_rowRightSurfModel = &(myMap.m_brainModels[k]);
                                m_rowRightSurfNodes = myMap.m_brainModels[k].m_surfaceNumberOfNodes;
                                m_rowRightSurfContig = thisContig;
                            }
                        }
                        if (myMap.m_brainModels[k].m_modelType == CIFTI_MODEL_TYPE_VOXELS)
                        {
                            m_rowVoxels += myMap.m_brainModels[k].m_indexCount;
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
                    int numModels = (int)myMap.m_brainModels.size();//over 2 billion models? unlikely
                    for (int k = 0; k < numModels; ++k)
                    {
                        if (myMap.m_brainModels[k].m_modelType == CIFTI_MODEL_TYPE_SURFACE)
                        {
                            int64_t thisContig = myMap.m_brainModels[k].m_indexCount;//if the loop interior never executes (empty list) it is a special case that all are contiguous
                            for (int64_t m = 0; m < (int64_t)myMap.m_brainModels[k].m_indexCount; ++m)//similarly, if the loop never encounters not equals, all are correct
                            {
                                if (m != (int64_t)myMap.m_brainModels[k].m_nodeIndices[m])
                                {
                                    thisContig = m;
                                    break;
                                }
                            }
                            if (myMap.m_brainModels[k].m_brainStructure.endsWith("_LEFT"))
                            {
                                if (m_colLeftSurfModel != NULL)
                                {
                                    throw CiftiFileException("Multiple surfaces of the same handedness not supported");
                                }
                                m_colLeftSurfModel = &(myMap.m_brainModels[k]);
                                m_colLeftSurfNodes = myMap.m_brainModels[k].m_surfaceNumberOfNodes;
                                m_colLeftSurfContig = thisContig;
                            }
                            if (myMap.m_brainModels[k].m_brainStructure.endsWith("_RIGHT"))
                            {
                                if (m_colRightSurfModel != NULL)
                                {
                                    throw CiftiFileException("Multiple surfaces of the same handedness not supported");
                                }
                                m_colRightSurfModel = &(myMap.m_brainModels[k]);
                                m_colRightSurfNodes = myMap.m_brainModels[k].m_surfaceNumberOfNodes;
                                m_colRightSurfContig = thisContig;
                            }
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
    bool left = false;
    switch (structure)
    {
    case StructureEnum::CORTEX_LEFT:
        left = true;
        break;
    case StructureEnum::CORTEX_RIGHT:
        break;
    default:
        return -1;
    };
    if (left)
    {
        return m_colLeftSurfNodes;
    } else {
        return m_colRightSurfNodes;
    }
}

int64_t CiftiXML::getRowSurfaceNumberOfNodes(const StructureEnum::Enum structure) const
{
    bool left = false;
    switch (structure)
    {
    case StructureEnum::CORTEX_LEFT:
        left = true;
        break;
    case StructureEnum::CORTEX_RIGHT:
        break;
    default:
        return -1;
    };
    if (left)
    {
        return m_rowLeftSurfNodes;
    } else {
        return m_rowRightSurfNodes;
    }
}

int64_t CiftiXML::getVolumeIndex(const float* xyz, const CiftiMatrixIndicesMapElement* myMap) const
{
    if (m_root.m_matrices.size() == 0)
    {
        throw CiftiFileException("No matrices defined in cifti extension");
    }
    if (m_root.m_matrices[0].m_volume.size() == 0)
    {
        throw CiftiFileException("No volume element defined in cifti extension");
    }
    const CiftiVolumeElement& myVol = m_root.m_matrices[0].m_volume[0];
    if (myVol.m_transformationMatrixVoxelIndicesIJKtoXYZ.size() == 0)
    {
        throw CiftiFileException("No volume transformation defined in cifti extension");
    }
    const TransformationMatrixVoxelIndicesIJKtoXYZElement& myTrans = myVol.m_transformationMatrixVoxelIndicesIJKtoXYZ[0];//oh the humanity
    float unitScale = 1.0f;
    switch (myTrans.m_unitsXYZ)
    {
        case NIFTI_UNITS_MM:
            break;
        case NIFTI_UNITS_METER:
            unitScale = 1000.0f;
            break;
        case NIFTI_UNITS_MICRON:
            unitScale = 0.001f;
            break;
        default:
            throw CiftiFileException("Unknown units in volume transformation");
    };
    FloatMatrix myMatrix = FloatMatrix::zeros(4, 4);
    for (int i = 0; i < 3; ++i)//NEVER trust the fourth row of input, NEVER!
    {
        for (int j = 0; j < 4; ++j)
        {
            myMatrix[i][j] = myTrans.m_transform[i * 4 + j];
        }
    }
    myMatrix *= unitScale;//rescale to mm
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
    if (ijk[0] < 0 || ijk[0] >= (int64_t)myVol.m_volumeDimensions[0]) return -1;//some shortcuts to not search all the voxels on invalid coords
    if (ijk[1] < 0 || ijk[1] >= (int64_t)myVol.m_volumeDimensions[1]) return -1;//should this be added to the other voxel index function?
    if (ijk[2] < 0 || ijk[2] >= (int64_t)myVol.m_volumeDimensions[2]) return -1;
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
            throw CiftiFileException("Unknown units in cifti timestep");
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
