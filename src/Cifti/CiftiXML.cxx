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
#include "CaretAssert.h"
#include "CiftiXML.h"
#include "CiftiFileException.h"
#include "FloatMatrix.h"
#include "GiftiLabelTable.h"
#include "Palette.h"
#include "PaletteColorMapping.h"

using namespace caret;
using namespace std;

//const int CiftiXML::ALONG_ROW;
//const int CiftiXML::ALONG_COLUMN;
//const int CiftiXML::ALONG_STACK;//even though these are initialized in the class, they need to be defined in some object code somewhere - so here they are

CiftiXML::CiftiXML()
{
    m_dimToMapLookup.resize(2, -1);//assume matrix is 2D, for backwards compatibility with Row/Column functions
}

map<AString, AString>* CiftiXML::getFileMetaData() const
{
    if (m_root.m_matrices.size() == 0) return NULL;
    return &(m_root.m_matrices[0].m_userMetaData);
}

int64_t CiftiXML::getSurfaceIndex(const int64_t& node, const CiftiBrainModelElement* myElement) const
{
    if (myElement == NULL || myElement->m_modelType != CIFTI_MODEL_TYPE_SURFACE) return -1;
    if (node < 0 || node > (int64_t)(myElement->m_surfaceNumberOfNodes)) return -1;
    CaretAssertVectorIndex(myElement->m_nodeToIndexLookup, node);
    return myElement->m_nodeToIndexLookup[node];
}

int64_t CiftiXML::getColumnIndexForNode(const int64_t& node, const StructureEnum::Enum& structure) const
{
    return getSurfaceIndex(node, findSurfaceModel(m_dimToMapLookup[0], structure));//a column index is an index to get an entire column, so index ALONG a row
}

int64_t CiftiXML::getRowIndexForNode(const int64_t& node, const StructureEnum::Enum& structure) const
{
    return getSurfaceIndex(node, findSurfaceModel(m_dimToMapLookup[1], structure));
}

int64_t CiftiXML::getVolumeIndex(const int64_t* ijk, const int& myMapIndex) const
{
    if (myMapIndex == -1 || m_root.m_matrices.size() == 0) return -1;
    CaretAssertVectorIndex(m_root.m_matrices[0].m_matrixIndicesMap, myMapIndex);
    const CiftiMatrixIndicesMapElement* myMap = &(m_root.m_matrices[0].m_matrixIndicesMap[myMapIndex]);
    if (myMap->m_indicesMapToDataType != CIFTI_INDEX_TYPE_BRAIN_MODELS)
    {
        return -1;
    }
    const CiftiVolumeElement& myVol = m_root.m_matrices[0].m_volume[0];
    if (ijk[0] < 0 || ijk[0] >= (int64_t)myVol.m_volumeDimensions[0]) return -1;//some shortcuts to not search all the voxels on invalid coords
    if (ijk[1] < 0 || ijk[1] >= (int64_t)myVol.m_volumeDimensions[1]) return -1;
    if (ijk[2] < 0 || ijk[2] >= (int64_t)myVol.m_volumeDimensions[2]) return -1;
    const int64_t* test = myMap->m_voxelToIndexLookup.find(ijk);
    if (test == NULL) return -1;
    return *test;
}

int64_t CiftiXML::getColumnIndexForVoxel(const int64_t* ijk) const
{
    return getVolumeIndex(ijk, m_dimToMapLookup[0]);
}

int64_t CiftiXML::getRowIndexForVoxel(const int64_t* ijk) const
{
    return getVolumeIndex(ijk, m_dimToMapLookup[1]);
}

bool CiftiXML::getSurfaceMap(const int& direction, vector<CiftiSurfaceMap>& mappingOut, const StructureEnum::Enum& structure) const
{
    if (direction < 0 || direction >= (int)m_dimToMapLookup.size())
    {
        mappingOut.clear();
        return false;
    }
    const CiftiBrainModelElement* myModel = findSurfaceModel(m_dimToMapLookup[direction], structure);
    if (myModel == NULL || myModel->m_modelType != CIFTI_MODEL_TYPE_SURFACE)
    {
        mappingOut.clear();
        return false;
    }
    int64_t mappingSize = myModel->m_indexCount;
    mappingOut.resize(mappingSize);
    if (myModel->m_nodeIndices.size() == 0)
    {
        CaretAssert(myModel->m_indexCount == myModel->m_surfaceNumberOfNodes);
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

bool CiftiXML::getSurfaceMapForColumns(vector<CiftiSurfaceMap>& mappingOut, const StructureEnum::Enum& structure) const
{
    return getSurfaceMap(ALONG_COLUMN, mappingOut, structure);
}

bool CiftiXML::getSurfaceMapForRows(vector<CiftiSurfaceMap>& mappingOut, const StructureEnum::Enum& structure) const
{
    return getSurfaceMap(ALONG_ROW, mappingOut, structure);
}

bool CiftiXML::getVolumeMap(const int& direction, vector<CiftiVolumeMap>& mappingOut) const
{
    mappingOut.clear();
    if (direction < 0 || direction >= (int)m_dimToMapLookup.size())
    {
        return false;
    }
    int myMapIndex = m_dimToMapLookup[direction];
    if (myMapIndex == -1 || m_root.m_matrices.size() == 0)
    {
        return false;
    }
    CaretAssertVectorIndex(m_root.m_matrices[0].m_matrixIndicesMap, myMapIndex);
    const CiftiMatrixIndicesMapElement* myMap = &(m_root.m_matrices[0].m_matrixIndicesMap[myMapIndex]);
    if (myMap->m_indicesMapToDataType != CIFTI_INDEX_TYPE_BRAIN_MODELS)
    {
        return false;
    }
    int64_t myIndex = 0;
    bool first = true;
    for (int64_t i = 0; i < (int64_t)myMap->m_brainModels.size(); ++i)
    {
        if (myMap->m_brainModels[i].m_modelType == CIFTI_MODEL_TYPE_VOXELS)
        {
            const vector<voxelIndexType>& myVoxels = myMap->m_brainModels[i].m_voxelIndicesIJK;
            int64_t voxelArraySize = (int64_t)myVoxels.size();
            int64_t modelOffset = myMap->m_brainModels[i].m_indexOffset;
            int64_t j1 = 0;
            if (first)
            {
                mappingOut.reserve(voxelArraySize / 3);//skip the tiny vector reallocs
                first = false;
            }
            for (int64_t j = 0; j < voxelArraySize; j += 3)
            {
                mappingOut.push_back(CiftiVolumeMap());//default constructor should be NOOP and get removed by compiler
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
    return getVolumeMap(ALONG_COLUMN, mappingOut);
}

bool CiftiXML::getVolumeMapForRows(vector<CiftiVolumeMap>& mappingOut) const
{
    return getVolumeMap(ALONG_ROW, mappingOut);
}

bool CiftiXML::getVolumeStructureMapping(vector<CiftiVolumeMap>& mappingOut, const StructureEnum::Enum& structure, const int& myMapIndex) const
{
    mappingOut.clear();
    const CiftiBrainModelElement* myModel = findVolumeModel(myMapIndex, structure);
    if (myModel == NULL)
    {
        return false;
    }
    int64_t size = (int64_t)myModel->m_voxelIndicesIJK.size();
    CaretAssert(size % 3 == 0);
    mappingOut.resize(size / 3);
    int64_t index = 0;
    for (int64_t i = 0; i < size; i += 3)
    {
        mappingOut[index].m_ciftiIndex = myModel->m_indexOffset + index;
        mappingOut[index].m_ijk[0] = myModel->m_voxelIndicesIJK[i];
        mappingOut[index].m_ijk[1] = myModel->m_voxelIndicesIJK[i + 1];
        mappingOut[index].m_ijk[2] = myModel->m_voxelIndicesIJK[i + 2];
        ++index;
    }
    return true;
}

bool CiftiXML::getVolumeStructureMapForColumns(vector<CiftiVolumeMap>& mappingOut, const StructureEnum::Enum& structure) const
{
    return getVolumeStructureMapping(mappingOut, structure, m_dimToMapLookup[1]);
}

bool CiftiXML::getVolumeStructureMapForRows(vector<CiftiVolumeMap>& mappingOut, const StructureEnum::Enum& structure) const
{
    return getVolumeStructureMapping(mappingOut, structure, m_dimToMapLookup[0]);
}

bool CiftiXML::getVolumeModelMappings(vector<CiftiVolumeStructureMap>& mappingsOut, const int& myMapIndex) const
{
    mappingsOut.clear();
    if (myMapIndex == -1 || m_root.m_matrices.size() == 0)
    {
        return false;
    }
    CaretAssertVectorIndex(m_root.m_matrices[0].m_matrixIndicesMap, myMapIndex);
    const CiftiMatrixIndicesMapElement* myMap = &(m_root.m_matrices[0].m_matrixIndicesMap[myMapIndex]);
    if (myMap->m_indicesMapToDataType != CIFTI_INDEX_TYPE_BRAIN_MODELS)
    {
        return false;
    }
    int numModels = (int)myMap->m_brainModels.size();
    mappingsOut.reserve(numModels);
    for (int i = 0; i < numModels; ++i)
    {
        if (myMap->m_brainModels[i].m_modelType == CIFTI_MODEL_TYPE_VOXELS)
        {
            mappingsOut.push_back(CiftiVolumeStructureMap());
            int whichMap = (int)mappingsOut.size() - 1;
            mappingsOut[whichMap].m_structure = myMap->m_brainModels[i].m_brainStructure;
            int numIndices = (int)myMap->m_brainModels[i].m_indexCount;
            mappingsOut[whichMap].m_map.resize(numIndices);
            for (int index = 0; index < numIndices; ++index)
            {
                mappingsOut[whichMap].m_map[index].m_ciftiIndex = myMap->m_brainModels[i].m_indexOffset + index;
                int64_t i3 = index * 3;
                mappingsOut[whichMap].m_map[index].m_ijk[0] = myMap->m_brainModels[i].m_voxelIndicesIJK[i3];
                mappingsOut[whichMap].m_map[index].m_ijk[1] = myMap->m_brainModels[i].m_voxelIndicesIJK[i3 + 1];
                mappingsOut[whichMap].m_map[index].m_ijk[2] = myMap->m_brainModels[i].m_voxelIndicesIJK[i3 + 2];
            }
        }
    }
    return true;
}

bool CiftiXML::getVolumeModelMapsForColumns(vector<CiftiVolumeStructureMap>& mappingsOut) const
{
    return getVolumeModelMappings(mappingsOut, m_dimToMapLookup[1]);
}

bool CiftiXML::getVolumeModelMapsForRows(vector<CiftiVolumeStructureMap>& mappingsOut) const
{
    return getVolumeModelMappings(mappingsOut, m_dimToMapLookup[0]);
}

bool CiftiXML::getStructureLists(const int& direction, vector<StructureEnum::Enum>& surfaceList, vector<StructureEnum::Enum>& volumeList) const
{
    surfaceList.clear();
    volumeList.clear();
    if (direction < 0 || direction >= (int)m_dimToMapLookup.size())
    {
        return false;
    }
    int myMapIndex = m_dimToMapLookup[direction];
    if (myMapIndex == -1 || m_root.m_matrices.size() == 0)
    {
        return false;
    }
    CaretAssertVectorIndex(m_root.m_matrices[0].m_matrixIndicesMap, myMapIndex);
    const CiftiMatrixIndicesMapElement* myMap = &(m_root.m_matrices[0].m_matrixIndicesMap[myMapIndex]);
    if (myMap->m_indicesMapToDataType != CIFTI_INDEX_TYPE_BRAIN_MODELS)
    {
        return false;
    }
    int numModels = (int)myMap->m_brainModels.size();
    for (int i = 0; i < numModels; ++i)
    {
        switch (myMap->m_brainModels[i].m_modelType)
        {
            case CIFTI_MODEL_TYPE_SURFACE:
                surfaceList.push_back(myMap->m_brainModels[i].m_brainStructure);
                break;
            case CIFTI_MODEL_TYPE_VOXELS:
                volumeList.push_back(myMap->m_brainModels[i].m_brainStructure);
                break;
            default:
                break;
        }
    }
    return true;
}

bool CiftiXML::getStructureListsForColumns(vector<StructureEnum::Enum>& surfaceList, vector<StructureEnum::Enum>& volumeList) const
{
    return getStructureLists(ALONG_COLUMN, surfaceList, volumeList);
}

bool CiftiXML::getStructureListsForRows(vector<StructureEnum::Enum>& surfaceList, vector<StructureEnum::Enum>& volumeList) const
{
    return getStructureLists(ALONG_ROW, surfaceList, volumeList);
}

void CiftiXML::rootChanged()
{
    m_dimToMapLookup.clear();//first, invalidate everything
    m_dimToMapLookup.resize(2, -1);//assume matrix is 2D, for backwards compatibility with Row/Column functions
    if (m_root.m_matrices.size() == 0)
    {
        return;//it shouldn't crash if it has no matrix, so return instead of throw
    }
    CiftiMatrixElement& myMatrix = m_root.m_matrices[0];//assume only one matrix
    int numMaps = (int)myMatrix.m_matrixIndicesMap.size();
    for (int i = 0; i < numMaps; ++i)
    {
        CiftiMatrixIndicesMapElement& myMap = myMatrix.m_matrixIndicesMap[i];
        int numDimensions = (int)myMap.m_appliesToMatrixDimension.size();
        for (int j = 0; j < numDimensions; ++j)
        {
            if (myMap.m_appliesToMatrixDimension[j] < 0) throw CiftiFileException("negative value in m_appliesToMatrixDimension");
            while (m_dimToMapLookup.size() <= (size_t)myMap.m_appliesToMatrixDimension[j])
            {
                m_dimToMapLookup.push_back(-1);
            }
            m_dimToMapLookup[myMap.m_appliesToMatrixDimension[j]] = i;
            myMap.setupLookup();
        }
    }
}

int64_t CiftiXML::getColumnSurfaceNumberOfNodes(const StructureEnum::Enum& structure) const
{
    return getSurfaceNumberOfNodes(ALONG_COLUMN, structure);
}

int64_t CiftiXML::getRowSurfaceNumberOfNodes(const StructureEnum::Enum& structure) const
{
    return getSurfaceNumberOfNodes(ALONG_ROW, structure);
}

int64_t CiftiXML::getSurfaceNumberOfNodes(const int& direction, const StructureEnum::Enum& structure) const
{
    if (direction < 0 || direction >= (int)m_dimToMapLookup.size()) return -1;
    if (m_root.m_matrices.size() == 0) return -1;
    CaretAssertVectorIndex(m_root.m_matrices[0].m_matrixIndicesMap, m_dimToMapLookup[direction]);
    const CiftiMatrixIndicesMapElement& myMap = m_root.m_matrices[0].m_matrixIndicesMap[m_dimToMapLookup[direction]];
    IndicesMapToDataType myType = myMap.m_indicesMapToDataType;
    if (myType == CIFTI_INDEX_TYPE_BRAIN_MODELS)
    {
        const CiftiBrainModelElement* myModel = findSurfaceModel(m_dimToMapLookup[direction], structure);
        if (myModel == NULL) return -1;
        return myModel->m_surfaceNumberOfNodes;
    } else if (myType == CIFTI_INDEX_TYPE_PARCELS) {
        int numSurfs = (int)myMap.m_parcelSurfaces.size();
        for (int i = 0; i < numSurfs; ++i)
        {
            if (myMap.m_parcelSurfaces[i].m_structure == structure)
            {
                return myMap.m_parcelSurfaces[i].m_numNodes;
            }
        }
    }
    return -1;
}

int64_t CiftiXML::getVolumeIndex(const float* xyz, const int& myMapIndex) const
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
    ijk[0] = (int64_t)floor(myIndices[0][0] + 0.5f);
    ijk[1] = (int64_t)floor(myIndices[1][0] + 0.5f);
    ijk[2] = (int64_t)floor(myIndices[2][0] + 0.5f);
    return getVolumeIndex(ijk, myMapIndex);
}

int64_t CiftiXML::getColumnIndexForVoxelCoordinate(const float* xyz) const
{
    return getVolumeIndex(xyz, m_dimToMapLookup[0]);
}

int64_t CiftiXML::getRowIndexForVoxelCoordinate(const float* xyz) const
{
    return getVolumeIndex(xyz, m_dimToMapLookup[1]);
}

int64_t CiftiXML::getTimestepIndex(const float& seconds, const int& myMapIndex) const
{
    if (myMapIndex == -1 || m_root.m_matrices.size() == 0)
    {
        return false;
    }
    CaretAssertVectorIndex(m_root.m_matrices[0].m_matrixIndicesMap, myMapIndex);
    const CiftiMatrixIndicesMapElement* myMap = &(m_root.m_matrices[0].m_matrixIndicesMap[myMapIndex]);
    float myStep;
    if (!getTimestep(myStep, myMapIndex))
    {
        return -1;
    }
    float rawIndex = seconds / myStep;
    int64_t ret = (int64_t)floor(rawIndex + 0.5f);
    if (ret < 0 || ret >= myMap->m_numTimeSteps) return -1;//NOTE: should this have a different error value if it is after the end of the timeseries
    return ret;
}

int64_t CiftiXML::getColumnIndexForTimepoint(const float& seconds) const
{
    return getTimestepIndex(seconds, m_dimToMapLookup[0]);
}

int64_t CiftiXML::getRowIndexForTimepoint(const float& seconds) const
{
    return getTimestepIndex(seconds, m_dimToMapLookup[1]);
}

bool CiftiXML::getTimestep(float& seconds, const int& myMapIndex) const
{
    if (myMapIndex == -1 || m_root.m_matrices.size() == 0)
    {
        return false;
    }
    CaretAssertVectorIndex(m_root.m_matrices[0].m_matrixIndicesMap, myMapIndex);
    const CiftiMatrixIndicesMapElement* myMap = &(m_root.m_matrices[0].m_matrixIndicesMap[myMapIndex]);
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
    return getTimestep(seconds, m_dimToMapLookup[1]);
}

bool CiftiXML::getRowTimestep(float& seconds) const
{
    return getTimestep(seconds, m_dimToMapLookup[0]);
}

bool CiftiXML::getTimestart(float& seconds, const int& myMapIndex) const
{
    if (myMapIndex == -1 || m_root.m_matrices.size() == 0)
    {
        return false;
    }
    CaretAssertVectorIndex(m_root.m_matrices[0].m_matrixIndicesMap, myMapIndex);
    const CiftiMatrixIndicesMapElement* myMap = &(m_root.m_matrices[0].m_matrixIndicesMap[myMapIndex]);
    if (myMap->m_indicesMapToDataType != CIFTI_INDEX_TYPE_TIME_POINTS || myMap->m_hasTimeStart == false)
    {
        return false;
    }
    switch (myMap->m_timeStepUnits)
    {
        case NIFTI_UNITS_SEC:
            seconds = myMap->m_timeStart;
            break;
        case NIFTI_UNITS_MSEC:
            seconds = myMap->m_timeStart * 0.001f;
            break;
        case NIFTI_UNITS_USEC:
            seconds = myMap->m_timeStart * 0.000001f;
            break;
        default:
            return false;
    };
    return true;
}

bool CiftiXML::getColumnTimestart(float& seconds) const
{
    return getTimestart(seconds, m_dimToMapLookup[1]);
}

bool CiftiXML::getRowTimestart(float& seconds) const
{
    return getTimestart(seconds, m_dimToMapLookup[0]);
}

bool CiftiXML::getColumnNumberOfTimepoints(int& numTimepoints) const
{
    if (m_dimToMapLookup[1] == -1 || m_root.m_matrices.size() == 0)
    {
        return false;
    }
    CaretAssertVectorIndex(m_root.m_matrices[0].m_matrixIndicesMap, m_dimToMapLookup[1]);
    const CiftiMatrixIndicesMapElement* myMap = &(m_root.m_matrices[0].m_matrixIndicesMap[m_dimToMapLookup[1]]);
    if (myMap->m_indicesMapToDataType != CIFTI_INDEX_TYPE_TIME_POINTS)
    {
        return false;
    }
    numTimepoints = myMap->m_numTimeSteps;
    return true;
}

bool CiftiXML::getRowNumberOfTimepoints(int& numTimepoints) const
{
    if (m_dimToMapLookup[0] == -1 || m_root.m_matrices.size() == 0)
    {
        return false;
    }
    CaretAssertVectorIndex(m_root.m_matrices[0].m_matrixIndicesMap, m_dimToMapLookup[0]);
    const CiftiMatrixIndicesMapElement* myMap = &(m_root.m_matrices[0].m_matrixIndicesMap[m_dimToMapLookup[0]]);
    if (myMap->m_indicesMapToDataType != CIFTI_INDEX_TYPE_TIME_POINTS)
    {
        return false;
    }
    numTimepoints = myMap->m_numTimeSteps;
    return true;
}

bool CiftiXML::getParcelsForColumns(vector<CiftiParcelElement>& parcelsOut) const
{
    return getParcels(ALONG_COLUMN, parcelsOut);
}

bool CiftiXML::getParcelsForRows(vector<CiftiParcelElement>& parcelsOut) const
{
    return getParcels(ALONG_ROW, parcelsOut);
}

bool CiftiXML::getParcels(const int& direction, vector< CiftiParcelElement >& parcelsOut) const
{
    CaretAssertVectorIndex(m_dimToMapLookup, direction);
    if (m_dimToMapLookup[direction] == -1 || m_root.m_matrices.size() == 0)
    {
        parcelsOut.clear();
        return false;
    }
    CaretAssertVectorIndex(m_root.m_matrices[0].m_matrixIndicesMap, m_dimToMapLookup[direction]);
    const CiftiMatrixIndicesMapElement* myMap = &(m_root.m_matrices[0].m_matrixIndicesMap[m_dimToMapLookup[direction]]);
    if (myMap->m_indicesMapToDataType != CIFTI_INDEX_TYPE_PARCELS)
    {
        parcelsOut.clear();
        return false;
    }
    parcelsOut = myMap->m_parcels;
    return true;
}

bool CiftiXML::getParcelSurfaceStructures(const int& direction, vector<StructureEnum::Enum>& structuresOut) const
{
    structuresOut.clear();
    if (direction < 0 || direction >= (int)m_dimToMapLookup.size()) return false;
    if (m_dimToMapLookup[direction] == -1 || m_root.m_matrices.size() == 0)
    {
        return false;
    }
    CaretAssertVectorIndex(m_root.m_matrices[0].m_matrixIndicesMap, m_dimToMapLookup[direction]);
    const CiftiMatrixIndicesMapElement& myMap = m_root.m_matrices[0].m_matrixIndicesMap[m_dimToMapLookup[direction]];
    if (myMap.m_indicesMapToDataType != CIFTI_INDEX_TYPE_PARCELS) return false;
    for (int i = 0; i < (int)myMap.m_parcelSurfaces.size(); ++i)
    {
        structuresOut.push_back(myMap.m_parcelSurfaces[i].m_structure);
    }
    return true;
}

int64_t CiftiXML::getParcelForNode(const int64_t& node, const StructureEnum::Enum& structure, const int& myMapIndex) const
{
    if (node < 0 || myMapIndex == -1 || m_root.m_matrices.size() == 0)
    {
        return -1;
    }
    CaretAssertVectorIndex(m_root.m_matrices[0].m_matrixIndicesMap, myMapIndex);
    const CiftiMatrixIndicesMapElement& myMap = m_root.m_matrices[0].m_matrixIndicesMap[myMapIndex];
    if (myMap.m_indicesMapToDataType != CIFTI_INDEX_TYPE_PARCELS)
    {
        return -1;
    }
    for (int i = 0; i < (int)myMap.m_parcelSurfaces.size(); ++i)
    {
        if (myMap.m_parcelSurfaces[i].m_structure == structure)
        {
            if (node < myMap.m_parcelSurfaces[i].m_numNodes)
            {
                return myMap.m_parcelSurfaces[i].m_lookup[node];
            } else {
                return -1;
            }
        }
    }
    return -1;
}

int64_t CiftiXML::getColumnParcelForNode(const int64_t& node, const StructureEnum::Enum& structure) const
{
    return getParcelForNode(node, structure, m_dimToMapLookup[1]);
}

int64_t CiftiXML::getRowParcelForNode(const int64_t& node, const caret::StructureEnum::Enum& structure) const
{
    return getParcelForNode(node, structure, m_dimToMapLookup[0]);
}

int64_t CiftiXML::getParcelForVoxel(const int64_t* ijk, const int& myMapIndex) const
{
    if (ijk[0] < 0 || ijk[1] < 0 || ijk[2] < 0 || myMapIndex == -1 || m_root.m_matrices.size() == 0)
    {
        return -1;
    }
    CaretAssertVectorIndex(m_root.m_matrices[0].m_matrixIndicesMap, myMapIndex);
    const CiftiMatrixIndicesMapElement& myMap = m_root.m_matrices[0].m_matrixIndicesMap[myMapIndex];
    if (myMap.m_indicesMapToDataType != CIFTI_INDEX_TYPE_PARCELS)
    {
        return -1;
    }
    const int64_t* test = myMap.m_voxelToIndexLookup.find(ijk);
    if (test == NULL) return -1;
    return *test;
}

int64_t CiftiXML::getColumnParcelForVoxel(const int64_t* ijk) const
{
    return getParcelForVoxel(ijk, m_dimToMapLookup[1]);
}

int64_t CiftiXML::getRowParcelForVoxel(const int64_t* ijk) const
{
    return getParcelForVoxel(ijk, m_dimToMapLookup[0]);
}

bool CiftiXML::setColumnNumberOfTimepoints(const int& numTimepoints)
{
    if (m_dimToMapLookup[1] == -1 || m_root.m_matrices.size() == 0)
    {
        return false;
    }
    CaretAssertVectorIndex(m_root.m_matrices[0].m_matrixIndicesMap, m_dimToMapLookup[1]);
    CiftiMatrixIndicesMapElement* myMap = &(m_root.m_matrices[0].m_matrixIndicesMap[m_dimToMapLookup[1]]);
    if (myMap->m_indicesMapToDataType != CIFTI_INDEX_TYPE_TIME_POINTS)
    {
        return false;
    }
    myMap->m_numTimeSteps = numTimepoints;
    return true;
}

bool CiftiXML::setRowNumberOfTimepoints(const int& numTimepoints)
{
    if (m_dimToMapLookup[0] == -1 || m_root.m_matrices.size() == 0)
    {
        return false;
    }
    CaretAssertVectorIndex(m_root.m_matrices[0].m_matrixIndicesMap, m_dimToMapLookup[0]);
    CiftiMatrixIndicesMapElement* myMap = &(m_root.m_matrices[0].m_matrixIndicesMap[m_dimToMapLookup[0]]);
    if (myMap->m_indicesMapToDataType != CIFTI_INDEX_TYPE_TIME_POINTS)
    {
        return false;
    }
    myMap->m_numTimeSteps = numTimepoints;
    return true;
}

bool CiftiXML::setTimestep(const float& seconds, const int& myMapIndex)
{
    if (myMapIndex == -1 || m_root.m_matrices.size() == 0)
    {
        return false;
    }
    CaretAssertVectorIndex(m_root.m_matrices[0].m_matrixIndicesMap, myMapIndex);
    CiftiMatrixIndicesMapElement* myMap = &(m_root.m_matrices[0].m_matrixIndicesMap[myMapIndex]);
    if (myMap->m_indicesMapToDataType != CIFTI_INDEX_TYPE_TIME_POINTS)
    {
        return false;
    }
    float temp = 1.0f;
    getTimestart(temp, myMapIndex);//convert to seconds
    myMap->m_timeStart = temp;
    myMap->m_timeStepUnits = NIFTI_UNITS_SEC;
    myMap->m_timeStep = seconds;
    return true;
}

bool CiftiXML::setColumnTimestep(const float& seconds)
{
    return setTimestep(seconds, m_dimToMapLookup[1]);
}

bool CiftiXML::setRowTimestep(const float& seconds)
{
    return setTimestep(seconds, m_dimToMapLookup[0]);
}

bool CiftiXML::setTimestart(const float& seconds, const int& myMapIndex)
{
    if (myMapIndex == -1 || m_root.m_matrices.size() == 0)
    {
        return false;
    }
    CaretAssertVectorIndex(m_root.m_matrices[0].m_matrixIndicesMap, myMapIndex);
    CiftiMatrixIndicesMapElement* myMap = &(m_root.m_matrices[0].m_matrixIndicesMap[myMapIndex]);
    if (myMap->m_indicesMapToDataType != CIFTI_INDEX_TYPE_TIME_POINTS)
    {
        return false;
    }
    float temp = 1.0f;
    getTimestep(temp, myMapIndex);//convert timestep to seconds
    myMap->m_timeStep = temp;
    myMap->m_timeStepUnits = NIFTI_UNITS_SEC;
    myMap->m_timeStart = seconds;
    myMap->m_hasTimeStart = true;
    return true;
}

bool CiftiXML::setColumnTimestart(const float& seconds)
{
    return setTimestart(seconds, m_dimToMapLookup[1]);
}

bool CiftiXML::setRowTimestart(const float& seconds)
{
    return setTimestart(seconds, m_dimToMapLookup[0]);
}

bool CiftiXML::getVolumeAttributesForPlumb(VolumeBase::OrientTypes orientOut[3], int64_t dimensionsOut[3], float originOut[3], float spacingOut[3]) const
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
                    orientOut[j] = (negative ? VolumeBase::RIGHT_TO_LEFT : VolumeBase::LEFT_TO_RIGHT);
                    break;
                case 1:
                    //forward/back
                    orientOut[j] = (negative ? VolumeBase::ANTERIOR_TO_POSTERIOR : VolumeBase::POSTERIOR_TO_ANTERIOR);
                    break;
                case 2:
                    //up/down
                    orientOut[j] = (negative ? VolumeBase::SUPERIOR_TO_INFERIOR : VolumeBase::INFERIOR_TO_SUPERIOR);
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

bool CiftiXML::getVolumeDimsAndSForm(int64_t dimsOut[3], vector<vector<float> >& sformOut) const
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
    sformOut = myMatrix.getMatrix();
    dimsOut[0] = myVol.m_volumeDimensions[0];
    dimsOut[1] = myVol.m_volumeDimensions[1];
    dimsOut[2] = myVol.m_volumeDimensions[2];
    return true;
}

void CiftiXML::setVolumeDimsAndSForm(const int64_t dims[3], const vector<vector<float> >& sform)
{
    CaretAssert(sform.size() == 3);
    if (m_root.m_matrices.size() == 0)
    {
        m_root.m_matrices.resize(1);
    }
    if (m_root.m_matrices[0].m_volume.size() == 0)
    {
        m_root.m_matrices[0].m_volume.resize(1);
    }
    CiftiVolumeElement& myVol = m_root.m_matrices[0].m_volume[0];
    if (myVol.m_transformationMatrixVoxelIndicesIJKtoXYZ.size() == 0)
    {
        myVol.m_transformationMatrixVoxelIndicesIJKtoXYZ.resize(1);
    }
    TransformationMatrixVoxelIndicesIJKtoXYZElement& myTrans = myVol.m_transformationMatrixVoxelIndicesIJKtoXYZ[0];//oh the humanity
    for (int i = 0; i < 3; ++i)
    {
        CaretAssert(sform[i].size() == 4);
        for (int j = 0; j < 4; ++j)
        {
            myTrans.m_transform[i * 4 + j] = sform[i][j];
        }
    }
    myTrans.m_unitsXYZ = NIFTI_UNITS_MM;
    myVol.m_volumeDimensions[0] = dims[0];
    myVol.m_volumeDimensions[1] = dims[1];
    myVol.m_volumeDimensions[2] = dims[2];
}

bool CiftiXML::getVolumeSpace(VolumeSpace& volSpaceOut) const
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
    int64_t dims[3] = { myVol.m_volumeDimensions[0], myVol.m_volumeDimensions[1], myVol.m_volumeDimensions[2] };
    volSpaceOut.setSpace(dims, myMatrix.getMatrix());
    return true;
}

AString CiftiXML::getMapName(const int& direction, const int& index) const
{
    if (m_dimToMapLookup[direction] == -1 || m_root.m_matrices.size() == 0)
    {
        return "#" + AString::number(index);
    }
    CaretAssertVectorIndex(m_root.m_matrices[0].m_matrixIndicesMap, m_dimToMapLookup[direction]);
    const CiftiMatrixIndicesMapElement& myMap = m_root.m_matrices[0].m_matrixIndicesMap[m_dimToMapLookup[direction]];
    if (myMap.m_indicesMapToDataType != CIFTI_INDEX_TYPE_SCALARS &&
        myMap.m_indicesMapToDataType != CIFTI_INDEX_TYPE_LABELS)
    {
        return "#" + AString::number(index);
    }
    CaretAssertVectorIndex(myMap.m_namedMaps, index);
    return myMap.m_namedMaps[index].m_mapName;
}

AString CiftiXML::getMapNameForColumnIndex(const int& index) const
{
    return getMapName(ALONG_COLUMN, index);
}

AString CiftiXML::getMapNameForRowIndex(const int& index) const
{
    return getMapName(ALONG_ROW, index);
}

bool CiftiXML::setMapNameForIndex(const int& direction, const int& index, const AString& name) const
{
    if (direction < 0 || direction >= (int)m_dimToMapLookup.size())
    {
        return false;
    }
    int myMapIndex = m_dimToMapLookup[direction];
    if (myMapIndex == -1 || m_root.m_matrices.size() == 0)
    {
        return false;
    }
    const CiftiMatrixIndicesMapElement& myMap = m_root.m_matrices[0].m_matrixIndicesMap[myMapIndex];
    if (myMap.m_indicesMapToDataType != CIFTI_INDEX_TYPE_SCALARS &&
        myMap.m_indicesMapToDataType != CIFTI_INDEX_TYPE_LABELS)
    {
        return false;
    }
    CaretAssertVectorIndex(myMap.m_namedMaps, index);
    myMap.m_namedMaps[index].m_mapName = name;
    return true;
}

bool CiftiXML::setMapNameForColumnIndex(const int& index, const AString& name) const
{
    return setMapNameForIndex(ALONG_COLUMN, index, name);
}

bool CiftiXML::setMapNameForRowIndex(const int& index, const AString& name) const
{
    return setMapNameForIndex(ALONG_ROW, index, name);
}

GiftiLabelTable* CiftiXML::getMapLabelTable(const int& direction, const int& index) const
{
    if (m_dimToMapLookup[direction] == -1 || m_root.m_matrices.size() == 0)
    {
        return NULL;
    }
    CaretAssertVectorIndex(m_root.m_matrices[0].m_matrixIndicesMap, m_dimToMapLookup[direction]);
    const CiftiMatrixIndicesMapElement& myMap = m_root.m_matrices[0].m_matrixIndicesMap[m_dimToMapLookup[direction]];
    if (myMap.m_indicesMapToDataType != CIFTI_INDEX_TYPE_LABELS)
    {
        return NULL;
    }
    CaretAssertVectorIndex(myMap.m_namedMaps, index);
    return myMap.m_namedMaps[index].m_labelTable;
}

GiftiLabelTable* CiftiXML::getLabelTableForColumnIndex(const int& index) const
{
    return getMapLabelTable(ALONG_COLUMN, index);
}

GiftiLabelTable* CiftiXML::getLabelTableForRowIndex(const int& index) const
{
    return getMapLabelTable(ALONG_ROW, index);
}

bool CiftiXML::setLabelTable(const int& index, const GiftiLabelTable& labelTable, const int& myMapIndex)
{
    if (myMapIndex == -1 || m_root.m_matrices.size() == 0)
    {
        return false;
    }
    CaretAssertVectorIndex(m_root.m_matrices[0].m_matrixIndicesMap, myMapIndex);
    CiftiMatrixIndicesMapElement& myMap = m_root.m_matrices[0].m_matrixIndicesMap[myMapIndex];
    if (myMap.m_indicesMapToDataType != CIFTI_INDEX_TYPE_LABELS)
    {
        return false;
    }
    CaretAssertVectorIndex(myMap.m_namedMaps, index);
    if (myMap.m_namedMaps[index].m_labelTable == NULL)//should never happen, but just in case
    {
        myMap.m_namedMaps[index].m_labelTable.grabNew(new GiftiLabelTable(labelTable));
    } else {
        *(myMap.m_namedMaps[index].m_labelTable) = labelTable;
    }
    return true;
}

bool CiftiXML::setLabelTableForColumnIndex(const int& index, const GiftiLabelTable& labelTable)
{
    return setLabelTable(index, labelTable, m_dimToMapLookup[1]);
}

bool CiftiXML::setLabelTableForRowIndex(const int& index, const GiftiLabelTable& labelTable)
{
    return setLabelTable(index, labelTable, m_dimToMapLookup[0]);
}

bool CiftiXML::hasVolumeData(const int& direction) const
{
    if (direction < 0 || direction >= (int)m_dimToMapLookup.size())
    {
        return false;
    }
    int myMapIndex = m_dimToMapLookup[direction];
    if (myMapIndex == -1 || m_root.m_matrices.size() == 0)
    {
        return false;
    }
    CaretAssertVectorIndex(m_root.m_matrices[0].m_matrixIndicesMap, myMapIndex);
    const CiftiMatrixIndicesMapElement* myMap = &(m_root.m_matrices[0].m_matrixIndicesMap[myMapIndex]);
    if (m_root.m_matrices[0].m_volume.size() == 0)
    {
        return false;
    }
    if (myMap == NULL)
    {
        return false;
    }
    if (myMap->m_indicesMapToDataType == CIFTI_INDEX_TYPE_BRAIN_MODELS)
    {
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
    } else if (myMap->m_indicesMapToDataType == CIFTI_INDEX_TYPE_PARCELS) {
        for (int64_t i = 0; i < (int64_t)myMap->m_parcels.size(); ++i)
        {
            if (myMap->m_parcels[i].m_voxelIndicesIJK.size() != 0)
            {
                return true;
            }
        }//TSC: I now think it should say true for parcels as long as there are voxels, useful for checking whether the volume XML element is needed
    }
    return false;
}

bool CiftiXML::hasRowVolumeData() const
{
    return hasVolumeData(ALONG_ROW);
}

bool CiftiXML::hasColumnVolumeData() const
{
    return hasVolumeData(ALONG_COLUMN);
}

bool CiftiXML::hasColumnSurfaceData(const StructureEnum::Enum& structure) const
{
    return hasSurfaceData(ALONG_COLUMN, structure);
}

bool CiftiXML::hasRowSurfaceData(const StructureEnum::Enum& structure) const
{
    return hasSurfaceData(ALONG_ROW, structure);
}

bool CiftiXML::hasSurfaceData(const int& direction, const StructureEnum::Enum& structure) const
{
    if (direction < 0 || direction >= (int)m_dimToMapLookup.size())
    {
        return false;
    }
    int myMapIndex = m_dimToMapLookup[direction];
    if (myMapIndex == -1 || m_root.m_matrices.size() == 0)
    {
        return false;
    }
    CaretAssertVectorIndex(m_root.m_matrices[0].m_matrixIndicesMap, myMapIndex);
    const CiftiMatrixIndicesMapElement* myMap = &(m_root.m_matrices[0].m_matrixIndicesMap[myMapIndex]);
    if (myMap->m_indicesMapToDataType == CIFTI_INDEX_TYPE_BRAIN_MODELS)
    {
        return (findSurfaceModel(m_dimToMapLookup[direction], structure) != NULL);
    } else if (myMap->m_indicesMapToDataType == CIFTI_INDEX_TYPE_PARCELS) {
        bool found = false;
        for (int i = 0; i < (int)myMap->m_parcelSurfaces.size(); ++i)
        {
            if (myMap->m_parcelSurfaces[i].m_structure == structure)
            {
                found = true;//TODO: figure out if we should just return true here
            }
        }
        if (!found) return false;
        for (int64_t i = 0; i < (int64_t)myMap->m_parcels.size(); ++i)
        {
            const CiftiParcelElement& myParcel = myMap->m_parcels[i];
            for (int j = 0; j < (int)myParcel.m_nodeElements.size(); ++j)
            {
                const CiftiParcelNodesElement& myNodes = myParcel.m_nodeElements[j];
                if (myNodes.m_structure == structure && myNodes.m_nodes.size() != 0) return true;//instead of checking that at least one parcel actually uses it
            }
        }
        return false;
    } else {
        return false;
    }
}

bool CiftiXML::addSurfaceModelToColumns(const int& numberOfNodes, const StructureEnum::Enum& structure, const float* roi)
{
    separateMaps();
    return addSurfaceModel(m_dimToMapLookup[1], numberOfNodes, structure, roi);
}

bool CiftiXML::addSurfaceModelToRows(const int& numberOfNodes, const StructureEnum::Enum& structure, const float* roi)
{
    separateMaps();
    return addSurfaceModel(m_dimToMapLookup[0], numberOfNodes, structure, roi);
}

bool CiftiXML::addSurfaceModel(const int& myMapIndex, const int& numberOfNodes, const StructureEnum::Enum& structure, const float* roi)
{
    if (myMapIndex == -1 || m_root.m_matrices.size() == 0)
    {
        return false;
    }
    CaretAssertVectorIndex(m_root.m_matrices[0].m_matrixIndicesMap, myMapIndex);
    CiftiMatrixIndicesMapElement* myMap = &(m_root.m_matrices[0].m_matrixIndicesMap[myMapIndex]);
    if (myMap->m_indicesMapToDataType != CIFTI_INDEX_TYPE_BRAIN_MODELS) return false;
    CiftiBrainModelElement tempModel;
    tempModel.m_brainStructure = structure;
    tempModel.m_modelType = CIFTI_MODEL_TYPE_SURFACE;
    tempModel.m_indexOffset = getNewRangeStart(myMapIndex);
    tempModel.m_surfaceNumberOfNodes = numberOfNodes;
    if (roi == NULL)
    {
        tempModel.m_indexCount = numberOfNodes;
    } else {
        tempModel.m_indexCount = 0;
        tempModel.m_nodeIndices.reserve(numberOfNodes);
        bool allNodes = true;
        for (int i = 0; i < numberOfNodes; ++i)
        {
            if (roi[i] > 0.0f)
            {
                tempModel.m_nodeIndices.push_back(i);
            } else {
                allNodes = false;
            }
        }
        if (allNodes)
        {
            tempModel.m_nodeIndices.clear();
            tempModel.m_indexCount = numberOfNodes;
        } else {
            tempModel.m_indexCount = (unsigned long long)tempModel.m_nodeIndices.size();
        }
    }
    myMap->m_brainModels.push_back(tempModel);
    myMap->m_brainModels.back().setupLookup(*myMap);
    return true;
}

bool CiftiXML::addSurfaceModelToColumns(const int& numberOfNodes, const StructureEnum::Enum& structure, const vector<int64_t>& nodeList)
{
    separateMaps();
    return addSurfaceModel(m_dimToMapLookup[1], numberOfNodes, structure, nodeList);
}

bool CiftiXML::addSurfaceModelToRows(const int& numberOfNodes, const StructureEnum::Enum& structure, const vector<int64_t>& nodeList)
{
    separateMaps();
    return addSurfaceModel(m_dimToMapLookup[0], numberOfNodes, structure, nodeList);
}

bool CiftiXML::addSurfaceModel(const int& myMapIndex, const int& numberOfNodes, const StructureEnum::Enum& structure, const vector<int64_t>& nodeList)
{
    if (myMapIndex == -1 || m_root.m_matrices.size() == 0)
    {
        return false;
    }
    CaretAssertVectorIndex(m_root.m_matrices[0].m_matrixIndicesMap, myMapIndex);
    CaretAssertMessage(checkSurfaceNodes(nodeList, numberOfNodes), "node list has node numbers that don't exist in the surface");
    CiftiMatrixIndicesMapElement* myMap = &(m_root.m_matrices[0].m_matrixIndicesMap[myMapIndex]);//call the check function inside an assert so it never does the check in release builds
    if (myMap->m_indicesMapToDataType != CIFTI_INDEX_TYPE_BRAIN_MODELS) return false;
    CiftiBrainModelElement tempModel;
    tempModel.m_brainStructure = structure;
    tempModel.m_modelType = CIFTI_MODEL_TYPE_SURFACE;
    tempModel.m_indexOffset = getNewRangeStart(myMapIndex);
    tempModel.m_surfaceNumberOfNodes = numberOfNodes;
    tempModel.m_indexCount = (int64_t)nodeList.size();
    if ((int)nodeList.size() == numberOfNodes)
    {
        bool sequential = true;
        for (int i = 0; i < numberOfNodes; ++i)
        {
            if (nodeList[i] != i)
            {
                sequential = false;
                break;
            }
        }
        if (!sequential)
        {
            tempModel.m_nodeIndices = nodeList;
        }
    } else {
        tempModel.m_nodeIndices = nodeList;
    }
    myMap->m_brainModels.push_back(tempModel);
    myMap->m_brainModels.back().setupLookup(*myMap);
    return true;
}

bool CiftiXML::checkSurfaceNodes(const vector<int64_t>& nodeList, const int& numberOfNodes) const
{
    int listSize = (int)nodeList.size();
    for (int i = 0; i < listSize; ++i)
    {
        if (nodeList[i] < 0 || nodeList[i] >= numberOfNodes) return false;
    }
    return true;
}

bool CiftiXML::addVolumeModel(const int& myMapIndex, const vector<voxelIndexType>& ijkList, const StructureEnum::Enum& structure)
{
    separateMaps();
    if (myMapIndex == -1 || m_root.m_matrices.size() == 0)
    {
        return false;
    }
    CaretAssertVectorIndex(m_root.m_matrices[0].m_matrixIndicesMap, myMapIndex);
    CiftiMatrixIndicesMapElement* myMap = &(m_root.m_matrices[0].m_matrixIndicesMap[myMapIndex]);
    if (myMap->m_indicesMapToDataType != CIFTI_INDEX_TYPE_BRAIN_MODELS) return false;
    CaretAssertMessage(checkVolumeIndices(ijkList), "volume voxel list doesn't match cifti volume space, do setVolumeDimsAndSForm first");
    CiftiBrainModelElement tempModel;//call the check function inside an assert so it never does the check in release builds
    tempModel.m_brainStructure = structure;
    tempModel.m_modelType = CIFTI_MODEL_TYPE_VOXELS;
    tempModel.m_indexOffset = getNewRangeStart(myMapIndex);
    tempModel.m_indexCount = ijkList.size() / 3;
    tempModel.m_voxelIndicesIJK = ijkList;
    myMap->m_brainModels.push_back(tempModel);
    return true;
}

bool CiftiXML::addVolumeModelToColumns(const vector<voxelIndexType>& ijkList, const StructureEnum::Enum& structure)
{
    return addVolumeModel(m_dimToMapLookup[1], ijkList, structure);
}

bool CiftiXML::addVolumeModelToRows(const vector<voxelIndexType>& ijkList, const StructureEnum::Enum& structure)
{
    return addVolumeModel(m_dimToMapLookup[0], ijkList, structure);
}

bool CiftiXML::addParcelSurfaceToColumns(const int& numberOfNodes, const StructureEnum::Enum& structure)
{
    return addParcelSurface(ALONG_COLUMN, numberOfNodes, structure);
}

bool CiftiXML::addParcelSurfaceToRows(const int& numberOfNodes, const StructureEnum::Enum& structure)
{
    return addParcelSurface(ALONG_ROW, numberOfNodes, structure);
}

bool CiftiXML::addParcelSurface(const int& direction, const int& numberOfNodes, const caret::StructureEnum::Enum& structure)
{
    if (direction < 0 || direction >= (int)m_dimToMapLookup.size())
    {
        return false;
    }
    separateMaps();
    if (numberOfNodes < 1 || m_dimToMapLookup[direction] == -1 || m_root.m_matrices.size() == 0)
    {
        return false;
    }
    CaretAssertVectorIndex(m_root.m_matrices[0].m_matrixIndicesMap, m_dimToMapLookup[direction]);
    CiftiMatrixIndicesMapElement& myMap = m_root.m_matrices[0].m_matrixIndicesMap[m_dimToMapLookup[direction]];
    if (myMap.m_indicesMapToDataType != CIFTI_INDEX_TYPE_PARCELS) return false;
    CiftiParcelSurfaceElement tempSurf;
    tempSurf.m_numNodes = numberOfNodes;
    tempSurf.m_structure = structure;
    myMap.m_parcelSurfaces.push_back(tempSurf);
    myMap.setupLookup();//TODO: make the lookup maintenance incremental
    return true;
}

bool CiftiXML::addParcelToColumns(const CiftiParcelElement& parcel)
{
    return addParcel(ALONG_COLUMN, parcel);
}

bool CiftiXML::addParcelToRows(const caret::CiftiParcelElement& parcel)
{
    return addParcel(ALONG_ROW, parcel);
}

bool CiftiXML::addParcel(const int& direction, const CiftiParcelElement& parcel)
{
    if (direction < 0 || direction >= (int)m_dimToMapLookup.size())
    {
        return false;
    }
    separateMaps();
    if (m_dimToMapLookup[direction] == -1 || m_root.m_matrices.size() == 0)
    {
        return false;
    }
    CaretAssertVectorIndex(m_root.m_matrices[0].m_matrixIndicesMap, m_dimToMapLookup[direction]);
    CiftiMatrixIndicesMapElement& myMap = m_root.m_matrices[0].m_matrixIndicesMap[m_dimToMapLookup[direction]];
    if (myMap.m_indicesMapToDataType != CIFTI_INDEX_TYPE_PARCELS) return false;
    if (!checkVolumeIndices(parcel.m_voxelIndicesIJK)) return false;
    myMap.m_parcels.push_back(parcel);//NOTE: setupLookup does error checking for nodes
    try
    {
        myMap.setupLookup();//TODO: make the lookup maintenance incremental, decide on throw vs bool return, separate sanity checking?
    } catch (...) {
        return false;
    }
    return true;
}

bool CiftiXML::checkVolumeIndices(const vector<voxelIndexType>& ijkList) const
{
    int64_t listSize = (int64_t)ijkList.size();
    if (listSize == 0) return true;
    if (listSize % 3 != 0) return false;
    int64_t dims[3];
    vector<vector<float> > sform;//not used, but needed by the funciton
    if (!getVolumeDimsAndSForm(dims, sform)) return false;
    for (int i = 0; i < listSize; i += 3)
    {
        if (ijkList[i] < 0 || ijkList[i] >= dims[0]) return false;
        if (ijkList[i + 1] < 0 || ijkList[i + 1] >= dims[1]) return false;
        if (ijkList[i + 2] < 0 || ijkList[i + 2] >= dims[2]) return false;
    }
    return true;
}

void CiftiXML::applyColumnMapToRows()
{
    if (m_dimToMapLookup[0] == m_dimToMapLookup[1]) return;
    applyDimensionHelper(1, 0);
}

void CiftiXML::applyRowMapToColumns()
{
    if (m_dimToMapLookup[0] == m_dimToMapLookup[1]) return;
    applyDimensionHelper(0, 1);
}

void CiftiXML::applyDimensionHelper(const int& from, const int& to)
{
    if (m_root.m_matrices.size() == 0) return;
    CiftiMatrixElement& myMatrix = m_root.m_matrices[0];//assume only one matrix
    int numMaps = (int)myMatrix.m_matrixIndicesMap.size();
    for (int i = 0; i < numMaps; ++i)
    {
        CiftiMatrixIndicesMapElement& myMap = myMatrix.m_matrixIndicesMap[i];
        int numDimensions = (int)myMap.m_appliesToMatrixDimension.size();
        for (int j = 0; j < numDimensions; ++j)
        {
            if (myMap.m_appliesToMatrixDimension[j] == to)
            {
                myMap.m_appliesToMatrixDimension.erase(myMap.m_appliesToMatrixDimension.begin() + j);
                --numDimensions;
                --j;
                break;
            }
        }
        for (int j = 0; j < numDimensions; ++j)
        {
            if (myMap.m_appliesToMatrixDimension[j] == from)
            {
                myMap.m_appliesToMatrixDimension.push_back(to);
                break;
            }
        }
        if (myMap.m_appliesToMatrixDimension.size() == 0)
        {
            myMatrix.m_matrixIndicesMap.erase(myMatrix.m_matrixIndicesMap.begin() + i);
            for (int j = 0; j < (int)m_dimToMapLookup.size(); ++j)
            {
                if (m_dimToMapLookup[j] > i) --m_dimToMapLookup[j];
            }
            --numMaps;
            --i;//make sure we don't skip a map due to an erase
        }
    }
    m_dimToMapLookup[to] = m_dimToMapLookup[from];
}

void CiftiXML::resetColumnsToBrainModels()
{
    if (m_dimToMapLookup[1] == -1)
    {
        m_dimToMapLookup[1] = createMap(1);
    } else {
        separateMaps();
    }
    CiftiMatrixIndicesMapElement myMap;
    myMap.m_appliesToMatrixDimension.push_back(1);
    myMap.m_indicesMapToDataType = CIFTI_INDEX_TYPE_BRAIN_MODELS;
    m_root.m_matrices[0].m_matrixIndicesMap[m_dimToMapLookup[1]] = myMap;
}

void CiftiXML::resetRowsToBrainModels()
{
    if (m_dimToMapLookup[0] == -1)
    {
        m_dimToMapLookup[0] = createMap(0);
    } else {
        separateMaps();
    }
    CiftiMatrixIndicesMapElement myMap;
    myMap.m_appliesToMatrixDimension.push_back(0);
    myMap.m_indicesMapToDataType = CIFTI_INDEX_TYPE_BRAIN_MODELS;
    m_root.m_matrices[0].m_matrixIndicesMap[m_dimToMapLookup[0]] = myMap;
}

void CiftiXML::resetColumnsToTimepoints(const float& timestep, const int& timepoints, const float& timestart)
{
    if (m_dimToMapLookup[1] == -1)
    {
        m_dimToMapLookup[1] = createMap(1);
    } else {
        separateMaps();
    }
    CiftiMatrixIndicesMapElement myMap;
    myMap.m_appliesToMatrixDimension.push_back(1);
    myMap.m_indicesMapToDataType = CIFTI_INDEX_TYPE_TIME_POINTS;
    myMap.m_timeStepUnits = NIFTI_UNITS_SEC;
    myMap.m_timeStep = timestep;
    myMap.m_timeStart = timestart;
    myMap.m_hasTimeStart = true;
    myMap.m_numTimeSteps = timepoints;
    m_root.m_matrices[0].m_matrixIndicesMap[m_dimToMapLookup[1]] = myMap;
}

void CiftiXML::resetRowsToTimepoints(const float& timestep, const int& timepoints, const float& timestart)
{
    if (m_dimToMapLookup[0] == -1)
    {
        m_dimToMapLookup[0] = createMap(0);
    } else {
        separateMaps();
    }
    CiftiMatrixIndicesMapElement myMap;
    myMap.m_appliesToMatrixDimension.push_back(0);
    myMap.m_indicesMapToDataType = CIFTI_INDEX_TYPE_TIME_POINTS;
    myMap.m_timeStepUnits = NIFTI_UNITS_SEC;
    myMap.m_timeStep = timestep;
    myMap.m_timeStart = timestart;
    myMap.m_hasTimeStart = true;
    myMap.m_numTimeSteps = timepoints;
    m_root.m_matrices[0].m_matrixIndicesMap[m_dimToMapLookup[0]] = myMap;
}

void CiftiXML::resetColumnsToScalars(const int& numMaps)
{
    resetDirectionToScalars(ALONG_COLUMN, numMaps);
}

void CiftiXML::resetRowsToScalars(const int& numMaps)
{
    resetDirectionToScalars(ALONG_ROW, numMaps);
}

void CiftiXML::resetDirectionToScalars(const int& direction, const int& numMaps)
{
    if (m_dimToMapLookup[direction] == -1)
    {
        m_dimToMapLookup[direction] = createMap(direction);
    } else {
        separateMaps();
    }
    CiftiMatrixIndicesMapElement myMap;
    myMap.m_appliesToMatrixDimension.push_back(direction);
    myMap.m_indicesMapToDataType = CIFTI_INDEX_TYPE_SCALARS;
    myMap.m_namedMaps.resize(numMaps);
    m_root.m_matrices[0].m_matrixIndicesMap[m_dimToMapLookup[direction]] = myMap;
}

void CiftiXML::resetColumnsToLabels(const int& numMaps)
{
    if (m_dimToMapLookup[1] == -1)
    {
        m_dimToMapLookup[1] = createMap(1);
    } else {
        separateMaps();
    }
    CiftiMatrixIndicesMapElement myMap;
    myMap.m_appliesToMatrixDimension.push_back(1);
    myMap.m_indicesMapToDataType = CIFTI_INDEX_TYPE_LABELS;
    myMap.m_namedMaps.resize(numMaps);
    for (int i = 0; i < numMaps; ++i)
    {
        myMap.m_namedMaps[i].m_labelTable.grabNew(new GiftiLabelTable());
    }
    m_root.m_matrices[0].m_matrixIndicesMap[m_dimToMapLookup[1]] = myMap;
}

void CiftiXML::resetRowsToLabels(const int& numMaps)
{
    if (m_dimToMapLookup[0] == -1)
    {
        m_dimToMapLookup[0] = createMap(0);
    } else {
        separateMaps();
    }
    CiftiMatrixIndicesMapElement myMap;
    myMap.m_appliesToMatrixDimension.push_back(0);
    myMap.m_indicesMapToDataType = CIFTI_INDEX_TYPE_LABELS;
    myMap.m_namedMaps.resize(numMaps);
    for (int i = 0; i < numMaps; ++i)
    {
        myMap.m_namedMaps[i].m_labelTable.grabNew(new GiftiLabelTable());
    }
    m_root.m_matrices[0].m_matrixIndicesMap[m_dimToMapLookup[0]] = myMap;
}

void CiftiXML::resetColumnsToParcels()
{
    resetDirectionToParcels(ALONG_COLUMN);
}

void CiftiXML::resetRowsToParcels()
{
    resetDirectionToParcels(ALONG_ROW);
}

void CiftiXML::resetDirectionToParcels(const int& direction)
{
    CaretAssertVectorIndex(m_dimToMapLookup, direction);
    if (m_dimToMapLookup[direction] == -1)
    {
        m_dimToMapLookup[direction] = createMap(direction);
    } else {
        separateMaps();
    }
    CiftiMatrixIndicesMapElement myMap;
    myMap.m_appliesToMatrixDimension.push_back(direction);
    myMap.m_indicesMapToDataType = CIFTI_INDEX_TYPE_PARCELS;
    m_root.m_matrices[0].m_matrixIndicesMap[m_dimToMapLookup[direction]] = myMap;
}

int CiftiXML::createMap(int dimension)
{
    CiftiMatrixIndicesMapElement tempMap;
    tempMap.m_appliesToMatrixDimension.push_back(dimension);
    if (m_root.m_matrices.size() == 0)
    {
        m_root.m_matrices.resize(1);
        m_root.m_numberOfMatrices = 1;//TODO: remove this variable
    }
    CiftiMatrixElement& myMatrix = m_root.m_matrices[0];//assume only one matrix
    myMatrix.m_matrixIndicesMap.push_back(tempMap);
    return myMatrix.m_matrixIndicesMap.size() - 1;
}

void CiftiXML::separateMaps()
{
    if (m_root.m_matrices.size() == 0) return;
    CiftiMatrixElement& myMatrix = m_root.m_matrices[0];//assume only one matrix
    int numMaps = (int)myMatrix.m_matrixIndicesMap.size();
    for (int i = 0; i < numMaps; ++i)//don't need to loop over newly created maps
    {
        CiftiMatrixIndicesMapElement myMap = myMatrix.m_matrixIndicesMap[i];//make a copy because we are modifying this vector
        int numDimensions = (int)myMap.m_appliesToMatrixDimension.size();
        for (int j = 1; j < numDimensions; ++j)//leave the first in place
        {
            int whichDim = myMap.m_appliesToMatrixDimension[j];
            myMatrix.m_matrixIndicesMap.push_back(myMap);
            myMatrix.m_matrixIndicesMap.back().m_appliesToMatrixDimension.resize(1);
            myMatrix.m_matrixIndicesMap.back().m_appliesToMatrixDimension[0] = whichDim;
            m_dimToMapLookup[whichDim] = myMatrix.m_matrixIndicesMap.size() - 1;
        }
        myMatrix.m_matrixIndicesMap[i].m_appliesToMatrixDimension.resize(1);//ditch all but the first, they have their own maps
    }
}

int CiftiXML::getNewRangeStart(const int& myMapIndex) const
{
    if (myMapIndex == -1 || m_root.m_matrices.size() == 0)
    {
        CaretAssert(false);
    }
    CaretAssertVectorIndex(m_root.m_matrices[0].m_matrixIndicesMap, myMapIndex);
    const CiftiMatrixIndicesMapElement* myMap = &(m_root.m_matrices[0].m_matrixIndicesMap[myMapIndex]);
    CaretAssert(myMap != NULL && myMap->m_indicesMapToDataType == CIFTI_INDEX_TYPE_BRAIN_MODELS);
    int numModels = (int)myMap->m_brainModels.size();
    int curRet = 0;
    for (int i = 0; i < numModels; ++i)
    {
        int thisEnd = myMap->m_brainModels[i].m_indexOffset + myMap->m_brainModels[i].m_indexCount;
        if (thisEnd > curRet)
        {
            curRet = thisEnd;
        }
    }
    return curRet;
}

int CiftiXML::getNumberOfColumns() const
{//number of columns is LENGTH OF A ROW
    if (m_dimToMapLookup[0] == -1)
    {
        return 0;//unspecified should be an error, probably, but be permissive
    } else {
        if (m_root.m_matrices.size() == 0)
        {
            return 0;
        }
        CaretAssertVectorIndex(m_root.m_matrices[0].m_matrixIndicesMap, m_dimToMapLookup[0]);
        const CiftiMatrixIndicesMapElement* myMap = &(m_root.m_matrices[0].m_matrixIndicesMap[m_dimToMapLookup[0]]);
        if (myMap->m_indicesMapToDataType == CIFTI_INDEX_TYPE_TIME_POINTS)
        {
            return myMap->m_numTimeSteps;
        } else if (myMap->m_indicesMapToDataType == CIFTI_INDEX_TYPE_BRAIN_MODELS) {
            return getNewRangeStart(m_dimToMapLookup[0]);
        } else if (myMap->m_indicesMapToDataType == CIFTI_INDEX_TYPE_SCALARS || myMap->m_indicesMapToDataType == CIFTI_INDEX_TYPE_LABELS) {
            return myMap->m_namedMaps.size();
        } else if (myMap->m_indicesMapToDataType == CIFTI_INDEX_TYPE_PARCELS) {
            return myMap->m_parcels.size();
        } else {
            throw CiftiFileException("unknown cifti mapping type");
        }
    }
}

int CiftiXML::getNumberOfRows() const
{
    if (m_dimToMapLookup[1] == -1)
    {
        return 0;//unspecified should be an error, probably, but be permissive
    } else {
        if (m_root.m_matrices.size() == 0)
        {
            return 0;
        }
        CaretAssertVectorIndex(m_root.m_matrices[0].m_matrixIndicesMap, m_dimToMapLookup[1]);
        const CiftiMatrixIndicesMapElement* myMap = &(m_root.m_matrices[0].m_matrixIndicesMap[m_dimToMapLookup[1]]);
        if (myMap->m_indicesMapToDataType == CIFTI_INDEX_TYPE_TIME_POINTS)
        {
            return myMap->m_numTimeSteps;
        } else if (myMap->m_indicesMapToDataType == CIFTI_INDEX_TYPE_BRAIN_MODELS) {
            return getNewRangeStart(m_dimToMapLookup[1]);
        } else if (myMap->m_indicesMapToDataType == CIFTI_INDEX_TYPE_SCALARS || myMap->m_indicesMapToDataType == CIFTI_INDEX_TYPE_LABELS) {
            return myMap->m_namedMaps.size();
        } else if (myMap->m_indicesMapToDataType == CIFTI_INDEX_TYPE_PARCELS) {
            return myMap->m_parcels.size();
        } else {
            throw CiftiFileException("unknown cifti mapping type");
        }
    }
}

IndicesMapToDataType CiftiXML::getColumnMappingType() const
{
    if (m_dimToMapLookup[1] == -1 || m_root.m_matrices.size() == 0)
    {
        return CIFTI_INDEX_TYPE_INVALID;
    }
    CaretAssertVectorIndex(m_root.m_matrices[0].m_matrixIndicesMap, m_dimToMapLookup[1]);
    const CiftiMatrixIndicesMapElement* myMap = &(m_root.m_matrices[0].m_matrixIndicesMap[m_dimToMapLookup[1]]);
    return myMap->m_indicesMapToDataType;
}

IndicesMapToDataType CiftiXML::getRowMappingType() const
{
    if (m_dimToMapLookup[0] == -1 || m_root.m_matrices.size() == 0)
    {
        return CIFTI_INDEX_TYPE_INVALID;
    }
    CaretAssertVectorIndex(m_root.m_matrices[0].m_matrixIndicesMap, m_dimToMapLookup[0]);
    const CiftiMatrixIndicesMapElement* myMap = &(m_root.m_matrices[0].m_matrixIndicesMap[m_dimToMapLookup[0]]);
    return myMap->m_indicesMapToDataType;
}

IndicesMapToDataType CiftiXML::getMappingType(const int& direction) const
{
    if (direction < 0 || direction >= (int)m_dimToMapLookup.size())
    {
        CaretAssertMessage(false, "CiftiXML::getMappingType called with invalid direction");
        return CIFTI_INDEX_TYPE_INVALID;
    }
    if (m_dimToMapLookup[direction] == -1 || m_root.m_matrices.size() == 0)
    {
        return CIFTI_INDEX_TYPE_INVALID;
    }
    CaretAssertVectorIndex(m_root.m_matrices[0].m_matrixIndicesMap, m_dimToMapLookup[direction]);
    const CiftiMatrixIndicesMapElement* myMap = &(m_root.m_matrices[0].m_matrixIndicesMap[m_dimToMapLookup[direction]]);
    return myMap->m_indicesMapToDataType;
}

const CiftiBrainModelElement* CiftiXML::findSurfaceModel(const int& myMapIndex, const StructureEnum::Enum& structure) const
{
    if (myMapIndex == -1 || m_root.m_matrices.size() == 0)
    {
        return NULL;
    }
    CaretAssertVectorIndex(m_root.m_matrices[0].m_matrixIndicesMap, myMapIndex);
    const CiftiMatrixIndicesMapElement* myMap = &(m_root.m_matrices[0].m_matrixIndicesMap[myMapIndex]);
    if (myMap->m_indicesMapToDataType != CIFTI_INDEX_TYPE_BRAIN_MODELS) return NULL;
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

const CiftiBrainModelElement* CiftiXML::findVolumeModel(const int& myMapIndex, const StructureEnum::Enum& structure) const
{
    if (myMapIndex == -1 || m_root.m_matrices.size() == 0)
    {
        return NULL;
    }
    CaretAssertVectorIndex(m_root.m_matrices[0].m_matrixIndicesMap, myMapIndex);
    const CiftiMatrixIndicesMapElement* myMap = &(m_root.m_matrices[0].m_matrixIndicesMap[myMapIndex]);
    if (myMap->m_indicesMapToDataType != CIFTI_INDEX_TYPE_BRAIN_MODELS) return NULL;
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

bool CiftiXML::matchesForColumns(const CiftiXML& rhs) const
{
    return mappingMatches(ALONG_COLUMN, rhs, ALONG_COLUMN);
}

bool CiftiXML::matchesForRows(const CiftiXML& rhs) const
{
    return mappingMatches(ALONG_ROW, rhs, ALONG_ROW);
}

bool CiftiXML::mappingMatches(const int& direction, const CiftiXML& other, const int& otherDirection) const
{
    CaretAssertVectorIndex(m_dimToMapLookup, direction);
    CaretAssertVectorIndex(other.m_dimToMapLookup, otherDirection);
    if (m_root.m_matrices.size() == 0 || m_dimToMapLookup[direction] == -1)
    {
        return (other.m_root.m_matrices.size() == 0 || other.m_dimToMapLookup[otherDirection] == -1);
    }
    CaretAssertVectorIndex(m_root.m_matrices[0].m_matrixIndicesMap, m_dimToMapLookup[direction]);
    CaretAssertVectorIndex(other.m_root.m_matrices[0].m_matrixIndicesMap, other.m_dimToMapLookup[otherDirection]);
    if (hasVolumeData(direction) && !(other.hasVolumeData(otherDirection) && matchesVolumeSpace(other))) return false;
    return (m_root.m_matrices[0].m_matrixIndicesMap[m_dimToMapLookup[direction]] == other.m_root.m_matrices[0].m_matrixIndicesMap[other.m_dimToMapLookup[otherDirection]]);
}

void CiftiXML::copyMapping(const int& direction, const CiftiXML& other, const int& otherDirection)
{
    CaretAssert(direction > -1 && otherDirection > -1);
    if ((int)other.m_dimToMapLookup.size() <= otherDirection || other.m_dimToMapLookup[otherDirection] == -1)
    {
        throw CiftiFileException("copyMapping called with nonexistant mapping to copy");
    }
    bool copyVolSpace = false, haveVoxels = false;
    for (int i = 0; i < (int)m_dimToMapLookup.size(); ++i)
    {
        if (i != direction && hasVolumeData(i))
        {
            haveVoxels = true;
        }
    }
    if (other.hasVolumeData(otherDirection))
    {
        if (haveVoxels)
        {
            if (!matchesVolumeSpace(other)) throw CiftiFileException("cannot copy mapping from other cifti due to volume space mismatch");
        } else {
            copyVolSpace = true;
        }
    } else {
        if (!haveVoxels)
        {
            m_root.m_matrices[0].m_volume.clear();
        }
    }
    if (m_dimToMapLookup[direction] == -1)
    {
        m_dimToMapLookup[direction] = createMap(direction);
    } else {
        separateMaps();
    }
    if (copyVolSpace)
    {//we have checked that this is okay because if we have any voxel data, it is in the map that is about to be replaced
        m_root.m_matrices[0].m_volume = other.m_root.m_matrices[0].m_volume;
    }
    CiftiMatrixIndicesMapElement& myMap = m_root.m_matrices[0].m_matrixIndicesMap[m_dimToMapLookup[direction]];
    myMap = other.m_root.m_matrices[0].m_matrixIndicesMap[other.m_dimToMapLookup[otherDirection]];
    myMap.m_appliesToMatrixDimension.clear();
    myMap.m_appliesToMatrixDimension.push_back(direction);//the member lookups should already be valid, copy works
}

map<AString, AString>* CiftiXML::getMapMetadata(const int& direction, const int& index) const
{
    CaretAssertVectorIndex(m_dimToMapLookup, direction);
    int myMapIndex = m_dimToMapLookup[direction];
    if (myMapIndex == -1 || m_root.m_matrices.size() == 0)
    {
        return NULL;
    }
    CaretAssertVectorIndex(m_root.m_matrices[0].m_matrixIndicesMap, myMapIndex);
    const CiftiMatrixIndicesMapElement& myMap = m_root.m_matrices[0].m_matrixIndicesMap[myMapIndex];
    if (myMap.m_indicesMapToDataType != CIFTI_INDEX_TYPE_LABELS && myMap.m_indicesMapToDataType != CIFTI_INDEX_TYPE_SCALARS)
    {
        return NULL;
    }
    CaretAssertVectorIndex(myMap.m_namedMaps, index);
    return &(myMap.m_namedMaps[index].m_mapMetaData);
}

PaletteColorMapping* CiftiXML::getFilePalette() const
{
    if (m_root.m_matrices.size() == 0) return NULL;
    if (m_root.m_matrices[0].m_palette == NULL)
    {//load from metadata
        m_root.m_matrices[0].m_palette.grabNew(new PaletteColorMapping());
        map<AString, AString>::const_iterator myIter = m_root.m_matrices[0].m_userMetaData.find("PaletteColorMapping");
        if (myIter != m_root.m_matrices[0].m_userMetaData.end())
        {
            m_root.m_matrices[0].m_palette->decodeFromStringXML(myIter->second);
        }
    /*} else {//will be needed if we make default palettes a user preference
        if (m_root.m_matrices[0].m_defaultPalette)
        {//TODO: load the current defaults into the existing palette, find some way of only doing this if the defaults were modified since last time this was called
        }//*/
    }
    return m_root.m_matrices[0].m_palette.getPointer();
}

PaletteColorMapping* CiftiXML::getMapPalette(const int& direction, const int& index) const
{
    CaretAssertVectorIndex(m_dimToMapLookup, direction);
    int myMapIndex = m_dimToMapLookup[direction];
    if (myMapIndex == -1 || m_root.m_matrices.size() == 0)
    {
        return NULL;
    }
    CaretAssertVectorIndex(m_root.m_matrices[0].m_matrixIndicesMap, myMapIndex);
    const CiftiMatrixIndicesMapElement& myMap = m_root.m_matrices[0].m_matrixIndicesMap[myMapIndex];
    if (myMap.m_indicesMapToDataType != CIFTI_INDEX_TYPE_SCALARS)
    {
        return NULL;
    }
    CaretAssertVectorIndex(myMap.m_namedMaps, index);
    const CiftiNamedMapElement& myElem = myMap.m_namedMaps[index];
    if (myElem.m_palette == NULL)
    {//load from metadata
        myElem.m_palette.grabNew(new PaletteColorMapping());
        map<AString, AString>::const_iterator myIter = myElem.m_mapMetaData.find("PaletteColorMapping");
        if (myIter != myElem.m_mapMetaData.end())
        {
            myElem.m_palette->decodeFromStringXML(myIter->second);
        }
    /*} else {//will be needed if we make default palettes a user preference
        if (m_root.m_matrices[0].m_defaultPalette)
        {//TODO: load the current defaults into the existing palette, find some way of only doing this if the defaults were modified since last time this was called
        }//*/
    }
    return myElem.m_palette;
}

bool CiftiXML::operator==(const caret::CiftiXML& rhs) const
{
    if (this == &rhs) return true;//compare pointers to skip checking object against itself
    if (m_root.m_matrices.size() != rhs.m_root.m_matrices.size()) return false;
    if (!matchesVolumeSpace(rhs)) return false;
    if (!matchesForColumns(rhs)) return false;
    if (m_root.m_matrices.size() > 1) return matchesForRows(rhs);
    return true;
}

bool CiftiXML::matchesVolumeSpace(const CiftiXML& rhs) const
{
    if (hasColumnVolumeData() || hasRowVolumeData())
    {
        if (!(rhs.hasColumnVolumeData() || rhs.hasRowVolumeData()))
        {
            return false;
        }
    } else {
        if (rhs.hasColumnVolumeData() || rhs.hasRowVolumeData())
        {
            return false;
        } else {
            return true;//don't check for matching/existing sforms if there are no voxel maps in either
        }
    }
    int64_t dims[3], rdims[3];
    vector<vector<float> > sform, rsform;
    if (!getVolumeDimsAndSForm(dims, sform) || !rhs.getVolumeDimsAndSForm(rdims, rsform))
    {//should NEVER happen
        CaretAssertMessage(false, "has*VolumeData() and getVolumeDimsAndSForm() disagree");
        throw CiftiFileException("has*VolumeData() and getVolumeDimsAndSForm() disagree");
    }
    const float TOLER_RATIO = 0.999f;//ratio a spacing element can mismatch by
    for (int i = 0; i < 3; ++i)
    {
        if (dims[i] != rdims[i]) return false;
        for (int j = 0; j < 4; ++j)
        {
            float left = sform[i][j];
            float right = rsform[i][j];
            if (left != right && (left == 0.0f || right == 0.0f || left / right < TOLER_RATIO || right / left < TOLER_RATIO)) return false;
        }
    }
    return true;
}

void CiftiXML::swapMappings(const int& direction1, const int& direction2)
{
    CaretAssertVectorIndex(m_dimToMapLookup, direction1);
    CaretAssertVectorIndex(m_dimToMapLookup, direction2);
    if (direction1 < 0 || direction1 >= (int)m_dimToMapLookup.size() ||
        direction2 < 0 || direction2 >= (int)m_dimToMapLookup.size())
    {
        throw CiftiFileException("invalid direction specified to swapMappings, notify the developers");
    }
    int mapIndex1 = m_dimToMapLookup[direction1];
    int mapIndex2 = m_dimToMapLookup[direction2];
    if (mapIndex1 == -1 || mapIndex2 == -1 || m_root.m_matrices.size() == 0)
    {
        throw CiftiFileException("invalid direction specified to swapMappings, notify the developers");
    }
    CaretAssertVectorIndex(m_root.m_matrices[0].m_matrixIndicesMap, mapIndex1);
    CaretAssertVectorIndex(m_root.m_matrices[0].m_matrixIndicesMap, mapIndex2);
    if (mapIndex1 == mapIndex2) return;//nothing to do if they refer to the same mapping
    CiftiMatrixIndicesMapElement& mapRef1 = m_root.m_matrices[0].m_matrixIndicesMap[mapIndex1];//give them shorter variable names
    CiftiMatrixIndicesMapElement& mapRef2 = m_root.m_matrices[0].m_matrixIndicesMap[mapIndex2];
    m_dimToMapLookup[direction1] = mapIndex2;//swap them by changing the lookup values
    m_dimToMapLookup[direction2] = mapIndex1;
    int numApply = (int)mapRef1.m_appliesToMatrixDimension.size(), i;//but we also need to modify the "applies to" lists
    for (i = 0; i < numApply; ++i)
    {
        if (mapRef1.m_appliesToMatrixDimension[i] == direction1)//we made the references from the old lookup values, so these are same
        {
            mapRef1.m_appliesToMatrixDimension[i] = direction2;//change the "applies to" element
            break;
        }
    }
    CaretAssert(i < numApply);//otherwise, we didn't find the element to modify, ie, something went horribly wrong
    numApply = (int)mapRef2.m_appliesToMatrixDimension.size();//and for the other mapping
    for (i = 0; i < numApply; ++i)
    {
        if (mapRef2.m_appliesToMatrixDimension[i] == direction2)
        {
            mapRef2.m_appliesToMatrixDimension[i] = direction1;
            break;
        }
    }
    CaretAssert(i < numApply);
}
