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

#include <cmath>
#include "CaretAssert.h"
#include "CiftiXMLOld.h"
#include "DataFileContentInformation.h"
#include "DataFileException.h"
#include "FloatMatrix.h"
#include "GiftiLabelTable.h"
#include "Palette.h"
#include "PaletteColorMapping.h"

#include "CiftiXML.h"
#include "CaretLogger.h"

using namespace caret;
using namespace std;

/*void CiftiXMLOld::testNewXML(const QString& xmlString)
{
    CiftiXML test1, test2;
    CiftiXMLOld compare1, compare2;
    AString stage;
    try
    {
        stage = "1_newRead1";
        test1.readXML(AString(xmlString));
        for (int i = 0; i < test1.getNumberOfDimensions(); ++i)
        {
            if (test1.getMappingType(i) == CiftiMappingType::SERIES)
            {
                CiftiSeriesMap myMap = test1.getSeriesMap(i);
                myMap.setLength(1);//there are asserts in series map that upon writing, it must know the length, so fake it for now
                test1.setMap(i, myMap);
            }
        }
        stage = "2_newWrite2";
        AString newWritten2 = test1.writeXMLToString(CiftiVersion(1, 1));
        stage = "3_newRead2";
        test2.readXML(newWritten2);
        stage = "4_newCompare";
        if (test1.getNumberOfDimensions() == test2.getNumberOfDimensions())
        {
            for (int i = 0; i < test1.getNumberOfDimensions(); ++i)
            {
                if (*(test1.getMap(i)) != *(test2.getMap(i)))
                {
                    CaretLogWarning("comparison in new cifti xml failed, dimension " + AString::number(i) + " not equal");
                }
            }
        } else {
            CaretLogWarning("comparison in new cifti xml failed, different number of dimensions");
        }
        stage = "5_newWrite1";
        AString newWritten1 = test2.writeXMLToString(CiftiVersion(1, 0));
        stage = "6_oldRead1";
        compare1.readXML(newWritten1, false);//prevent recursively triggering test
        compare2.readXML(xmlString, false);
        if (!compare1.matchesForRows(compare2))
        {
            CaretLogWarning("comparison in old xml failed for rows");
        }
        if (!compare1.matchesForColumns(compare2))
        {
            CaretLogWarning("comparison in old xml failed for columns");
        }
    } catch (CaretException& e) {
        CaretLogWarning("error testing new xml: stage '" + stage + "', error: " + e.whatString());
    } catch (std::exception& e) {
        CaretLogWarning("error testing new xml: stage '" + stage + "', error: " + e.what());
    } catch (...) {
        CaretLogWarning("error testing new xml: stage '" + stage + "', unknown throw type");
    }
}//*/

/*void CiftiXMLOld::testNewXML(const QByteArray& xmlBytes)
{
    CiftiXML test1, test2;
    CiftiXMLOld compare1, compare2;
    AString stage;
    try
    {
        stage = "1_newRead1";
        test1.readXML(xmlBytes);
        for (int i = 0; i < test1.getNumberOfDimensions(); ++i)
        {
            if (test1.getMappingType(i) == CiftiMappingType::SERIES)
            {
                CiftiSeriesMap myMap = test1.getSeriesMap(i);
                myMap.setLength(1);//there are asserts in series map that upon writing, it must know the length, so fake it for now
                test1.setMap(i, myMap);
            }
        }
        stage = "2_newWrite2";
        QByteArray newWritten2 = test1.writeXMLToQByteArray(CiftiVersion(1, 1));
        stage = "3_newRead2";
        test2.readXML(newWritten2);
        stage = "4_newCompare";
        if (test1.getNumberOfDimensions() == test2.getNumberOfDimensions())
        {
            for (int i = 0; i < test1.getNumberOfDimensions(); ++i)
            {
                if (*(test1.getMap(i)) != *(test2.getMap(i)))
                {
                    CaretLogWarning("comparison in new cifti xml failed, dimension " + AString::number(i) + " not equal");
                }
            }
        } else {
            CaretLogWarning("comparison in new cifti xml failed, different number of dimensions");
        }
        stage = "5_newWrite1";
        QByteArray newWritten1 = test2.writeXMLToQByteArray(CiftiVersion(1, 0));
        stage = "6_oldRead1";
        compare1.readXML(newWritten1, false);//prevent recursively triggering test
        compare2.readXML(xmlBytes, false);
        if (!compare1.matchesForRows(compare2))
        {
            CaretLogWarning("comparison in old xml failed for rows");
        }
        if (!compare1.matchesForColumns(compare2))
        {
            CaretLogWarning("comparison in old xml failed for columns");
        }
    } catch (CaretException& e) {
        CaretLogWarning("error testing new xml: stage '" + stage + "', error: " + e.whatString());
    } catch (std::exception& e) {
        CaretLogWarning("error testing new xml: stage '" + stage + "', error: " + e.what());
    } catch (...) {
        CaretLogWarning("error testing new xml: stage '" + stage + "', unknown throw type");
    }
}//*/

CiftiXMLOld::CiftiXMLOld()
{
    m_dimToMapLookup.resize(2, -1);//assume matrix is 2D, for backwards compatibility with Row/Column functions
}

map<AString, AString>* CiftiXMLOld::getFileMetaData() const
{
    if (m_root.m_matrices.size() == 0) return NULL;
    return &(m_root.m_matrices[0].m_userMetaData);
}

int64_t CiftiXMLOld::getSurfaceIndex(const int64_t& node, const CiftiBrainModelElement* myElement) const
{
    if (myElement == NULL || myElement->m_modelType != CIFTI_MODEL_TYPE_SURFACE) return -1;
    if (node < 0 || node > (int64_t)(myElement->m_surfaceNumberOfNodes)) return -1;
    CaretAssertVectorIndex(myElement->m_nodeToIndexLookup, node);
    return myElement->m_nodeToIndexLookup[node];
}

int64_t CiftiXMLOld::getColumnIndexForNode(const int64_t& node, const StructureEnum::Enum& structure) const
{
    return getSurfaceIndex(node, findSurfaceModel(m_dimToMapLookup[0], structure));//a column index is an index to get an entire column, so index ALONG a row
}

int64_t CiftiXMLOld::getRowIndexForNode(const int64_t& node, const StructureEnum::Enum& structure) const
{
    return getSurfaceIndex(node, findSurfaceModel(m_dimToMapLookup[1], structure));
}

int64_t CiftiXMLOld::getVolumeIndex(const int64_t* ijk, const int& myMapIndex) const
{
    if (myMapIndex == -1 || m_root.m_matrices.size() == 0) return -1;
    CaretAssertVectorIndex(m_root.m_matrices[0].m_matrixIndicesMap, myMapIndex);
    const CiftiMatrixIndicesMapElement* myMap = &(m_root.m_matrices[0].m_matrixIndicesMap[myMapIndex]);
    if (myMap->m_indicesMapToDataType != CIFTI_INDEX_TYPE_BRAIN_MODELS)
    {
        return -1;
    }
    if (m_root.m_matrices[0].m_volume.size() == 0) return -1;
    const CiftiVolumeElement& myVol = m_root.m_matrices[0].m_volume[0];
    if (ijk[0] < 0 || ijk[0] >= (int64_t)myVol.m_volumeDimensions[0]) return -1;//some shortcuts to not search all the voxels on invalid coords
    if (ijk[1] < 0 || ijk[1] >= (int64_t)myVol.m_volumeDimensions[1]) return -1;
    if (ijk[2] < 0 || ijk[2] >= (int64_t)myVol.m_volumeDimensions[2]) return -1;
    const int64_t* test = myMap->m_voxelToIndexLookup.find(ijk);
    if (test == NULL) return -1;
    return *test;
}

int64_t CiftiXMLOld::getColumnIndexForVoxel(const int64_t* ijk) const
{
    return getVolumeIndex(ijk, m_dimToMapLookup[0]);
}

int64_t CiftiXMLOld::getRowIndexForVoxel(const int64_t* ijk) const
{
    return getVolumeIndex(ijk, m_dimToMapLookup[1]);
}

bool CiftiXMLOld::getSurfaceMap(const int& direction, vector<CiftiSurfaceMap>& mappingOut, const StructureEnum::Enum& structure) const
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

bool CiftiXMLOld::getSurfaceMapForColumns(vector<CiftiSurfaceMap>& mappingOut, const StructureEnum::Enum& structure) const
{
    return getSurfaceMap(ALONG_COLUMN, mappingOut, structure);
}

bool CiftiXMLOld::getSurfaceMapForRows(vector<CiftiSurfaceMap>& mappingOut, const StructureEnum::Enum& structure) const
{
    return getSurfaceMap(ALONG_ROW, mappingOut, structure);
}

bool CiftiXMLOld::getVolumeMap(const int& direction, vector<CiftiVolumeMap>& mappingOut) const
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

bool CiftiXMLOld::getVolumeMapForColumns(vector<CiftiVolumeMap>& mappingOut) const
{
    return getVolumeMap(ALONG_COLUMN, mappingOut);
}

bool CiftiXMLOld::getVolumeMapForRows(vector<CiftiVolumeMap>& mappingOut) const
{
    return getVolumeMap(ALONG_ROW, mappingOut);
}

void
CiftiXMLOld::getVoxelInfoInDataFileContentInformation(const int& direction,
                                                      DataFileContentInformation& dataFileInformation) const
{
    if (direction < 0 || direction >= (int)m_dimToMapLookup.size())
    {
        return;
    }
    int myMapIndex = m_dimToMapLookup[direction];
    if (myMapIndex == -1 || m_root.m_matrices.size() == 0)
    {
        return;
    }
    CaretAssertVectorIndex(m_root.m_matrices[0].m_matrixIndicesMap, myMapIndex);
    const CiftiMatrixIndicesMapElement* myMap = &(m_root.m_matrices[0].m_matrixIndicesMap[myMapIndex]);
    if (myMap->m_indicesMapToDataType != CIFTI_INDEX_TYPE_BRAIN_MODELS)
    {
        return;
    }
    
    VolumeSpace volumeSpace;
    getVolumeSpace(volumeSpace);

    const int64_t* dims = volumeSpace.getDims();
    dataFileInformation.addNameAndValue("Dimensions", AString::fromNumbers(dims, 3, ","));
    VolumeSpace::OrientTypes orientation[3];
    float spacing[3];
    float origin[3];
    volumeSpace.getOrientAndSpacingForPlumb(orientation, spacing, origin);
    dataFileInformation.addNameAndValue("Spacing", AString::fromNumbers(spacing, 3, ","));
    dataFileInformation.addNameAndValue("Origin", AString::fromNumbers(origin, 3, ","));
    
    const std::vector<std::vector<float> >& sform = volumeSpace.getSform();
    for (uint32_t i = 0; i < sform.size(); i++) {
        dataFileInformation.addNameAndValue(("sform row "
                                             + AString::number(i)),
                                            AString::fromNumbers(sform[i], ","));
    }
    
    int64_t myIndex = 0;
    for (int64_t i = 0; i < (int64_t)myMap->m_brainModels.size(); ++i)
    {
        if (myMap->m_brainModels[i].m_modelType == CIFTI_MODEL_TYPE_VOXELS)
        {
            const vector<voxelIndexType>& myVoxels = myMap->m_brainModels[i].m_voxelIndicesIJK;
            int64_t voxelArraySize = (int64_t)myVoxels.size();
            int64_t modelOffset = myMap->m_brainModels[i].m_indexOffset;
            int64_t j1 = 0;

            const AString structureName = StructureEnum::toGuiName(myMap->m_brainModels[i].m_brainStructure);
            
            for (int64_t j = 0; j < voxelArraySize; j += 3)
            {
                const int64_t ijk[3] = {
                    myVoxels[j],
                    myVoxels[j + 1],
                    myVoxels[j + 2]
                };
                float xyz[3];
                volumeSpace.indexToSpace(ijk, xyz);
                
                const AString msg = ("ijk=("
                                     + AString::fromNumbers(ijk, 3, ",")
                                     + "), xyz=("
                                     + AString::fromNumbers(xyz, 3, ", ")
                                     + "), row="
                                     + AString::number(modelOffset + j1)
                                     + "  ");
                dataFileInformation.addNameAndValue(structureName,
                                                    msg);
                ++j1;
                ++myIndex;
            }
        }
    }
}

bool CiftiXMLOld::getVolumeStructureMap(const int& direction, vector<CiftiVolumeMap>& mappingOut, const StructureEnum::Enum& structure) const
{
    mappingOut.clear();
    CaretAssertVectorIndex(m_dimToMapLookup, direction);
    int myMapIndex = m_dimToMapLookup[direction];
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

bool CiftiXMLOld::getVolumeStructureMapForColumns(vector<CiftiVolumeMap>& mappingOut, const StructureEnum::Enum& structure) const
{
    return getVolumeStructureMap(ALONG_COLUMN, mappingOut, structure);
}

bool CiftiXMLOld::getVolumeStructureMapForRows(vector<CiftiVolumeMap>& mappingOut, const StructureEnum::Enum& structure) const
{
    return getVolumeStructureMap(ALONG_ROW, mappingOut, structure);
}

bool CiftiXMLOld::getVolumeModelMappings(vector<CiftiVolumeStructureMap>& mappingsOut, const int& myMapIndex) const
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

bool CiftiXMLOld::getVolumeModelMapsForColumns(vector<CiftiVolumeStructureMap>& mappingsOut) const
{
    return getVolumeModelMappings(mappingsOut, m_dimToMapLookup[1]);
}

bool CiftiXMLOld::getVolumeModelMapsForRows(vector<CiftiVolumeStructureMap>& mappingsOut) const
{
    return getVolumeModelMappings(mappingsOut, m_dimToMapLookup[0]);
}

bool CiftiXMLOld::getStructureLists(const int& direction, vector<StructureEnum::Enum>& surfaceList, vector<StructureEnum::Enum>& volumeList) const
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

bool CiftiXMLOld::getStructureListsForColumns(vector<StructureEnum::Enum>& surfaceList, vector<StructureEnum::Enum>& volumeList) const
{
    return getStructureLists(ALONG_COLUMN, surfaceList, volumeList);
}

bool CiftiXMLOld::getStructureListsForRows(vector<StructureEnum::Enum>& surfaceList, vector<StructureEnum::Enum>& volumeList) const
{
    return getStructureLists(ALONG_ROW, surfaceList, volumeList);
}

int CiftiXMLOld::getNumberOfBrainModels(const int& direction) const
{
    CaretAssertVectorIndex(m_dimToMapLookup, direction);
    int myMapIndex = m_dimToMapLookup[direction];
    if (myMapIndex == -1 || m_root.m_matrices.size() == 0)
    {
        return -1;
    }
    CaretAssertVectorIndex(m_root.m_matrices[0].m_matrixIndicesMap, myMapIndex);
    const CiftiMatrixIndicesMapElement* myMap = &(m_root.m_matrices[0].m_matrixIndicesMap[myMapIndex]);
    if (myMap->m_indicesMapToDataType != CIFTI_INDEX_TYPE_BRAIN_MODELS)
    {
        return -1;
    }
    return (int)myMap->m_brainModels.size();//reuse of type and structure combinations not allowed, so this is limited to number of structure enum values times number of model types (2)
}

CiftiBrainModelInfo CiftiXMLOld::getBrainModelInfo(const int& direction, const int& whichModel) const
{
    CiftiBrainModelInfo ret;
    CaretAssertVectorIndex(m_dimToMapLookup, direction);
    int myMapIndex = m_dimToMapLookup[direction];
    if (myMapIndex == -1 || m_root.m_matrices.size() == 0)
    {
        return ret;
    }
    CaretAssertVectorIndex(m_root.m_matrices[0].m_matrixIndicesMap, myMapIndex);
    const CiftiMatrixIndicesMapElement* myMap = &(m_root.m_matrices[0].m_matrixIndicesMap[myMapIndex]);
    if (myMap->m_indicesMapToDataType != CIFTI_INDEX_TYPE_BRAIN_MODELS)
    {
        return ret;
    }
    CaretAssertVectorIndex(myMap->m_brainModels, whichModel);
    ret.m_type = myMap->m_brainModels[whichModel].m_modelType;
    ret.m_structure = myMap->m_brainModels[whichModel].m_brainStructure;
    return ret;
}

void CiftiXMLOld::rootChanged()
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
            if (myMap.m_appliesToMatrixDimension[j] < 0) throw DataFileException("negative value in m_appliesToMatrixDimension");
            while (m_dimToMapLookup.size() <= (size_t)myMap.m_appliesToMatrixDimension[j])
            {
                m_dimToMapLookup.push_back(-1);
            }
            m_dimToMapLookup[myMap.m_appliesToMatrixDimension[j]] = i;
            myMap.setupLookup();
        }
    }
}

int64_t CiftiXMLOld::getColumnSurfaceNumberOfNodes(const StructureEnum::Enum& structure) const
{
    return getSurfaceNumberOfNodes(ALONG_COLUMN, structure);
}

int64_t CiftiXMLOld::getRowSurfaceNumberOfNodes(const StructureEnum::Enum& structure) const
{
    return getSurfaceNumberOfNodes(ALONG_ROW, structure);
}

int64_t CiftiXMLOld::getSurfaceNumberOfNodes(const int& direction, const StructureEnum::Enum& structure) const
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

int64_t CiftiXMLOld::getVolumeIndex(const float* xyz, const int& myMapIndex) const
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

int64_t CiftiXMLOld::getColumnIndexForVoxelCoordinate(const float* xyz) const
{
    return getVolumeIndex(xyz, m_dimToMapLookup[0]);
}

int64_t CiftiXMLOld::getRowIndexForVoxelCoordinate(const float* xyz) const
{
    return getVolumeIndex(xyz, m_dimToMapLookup[1]);
}

int64_t CiftiXMLOld::getTimestepIndex(const float& seconds, const int& myMapIndex) const
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

int64_t CiftiXMLOld::getColumnIndexForTimepoint(const float& seconds) const
{
    return getTimestepIndex(seconds, m_dimToMapLookup[0]);
}

int64_t CiftiXMLOld::getRowIndexForTimepoint(const float& seconds) const
{
    return getTimestepIndex(seconds, m_dimToMapLookup[1]);
}

bool CiftiXMLOld::getTimestep(float& seconds, const int& myMapIndex) const
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

bool CiftiXMLOld::getColumnTimestep(float& seconds) const
{
    return getTimestep(seconds, m_dimToMapLookup[1]);
}

bool CiftiXMLOld::getRowTimestep(float& seconds) const
{
    return getTimestep(seconds, m_dimToMapLookup[0]);
}

bool CiftiXMLOld::getTimestart(float& seconds, const int& myMapIndex) const
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

bool CiftiXMLOld::getColumnTimestart(float& seconds) const
{
    return getTimestart(seconds, m_dimToMapLookup[1]);
}

bool CiftiXMLOld::getRowTimestart(float& seconds) const
{
    return getTimestart(seconds, m_dimToMapLookup[0]);
}

bool CiftiXMLOld::getColumnNumberOfTimepoints(int& numTimepoints) const
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

bool CiftiXMLOld::getRowNumberOfTimepoints(int& numTimepoints) const
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

bool CiftiXMLOld::getParcelsForColumns(vector<CiftiParcelElement>& parcelsOut) const
{
    return getParcels(ALONG_COLUMN, parcelsOut);
}

bool CiftiXMLOld::getParcelsForRows(vector<CiftiParcelElement>& parcelsOut) const
{
    return getParcels(ALONG_ROW, parcelsOut);
}

bool CiftiXMLOld::getParcels(const int& direction, vector< CiftiParcelElement >& parcelsOut) const
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

bool CiftiXMLOld::getParcelSurfaceStructures(const int& direction, vector<StructureEnum::Enum>& structuresOut) const
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

int64_t CiftiXMLOld::getParcelForNode(const int64_t& node, const StructureEnum::Enum& structure, const int& myMapIndex) const
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

int64_t CiftiXMLOld::getColumnParcelForNode(const int64_t& node, const StructureEnum::Enum& structure) const
{
    return getParcelForNode(node, structure, m_dimToMapLookup[1]);
}

int64_t CiftiXMLOld::getRowParcelForNode(const int64_t& node, const caret::StructureEnum::Enum& structure) const
{
    return getParcelForNode(node, structure, m_dimToMapLookup[0]);
}

int64_t CiftiXMLOld::getParcelForVoxel(const int64_t* ijk, const int& myMapIndex) const
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

int64_t CiftiXMLOld::getColumnParcelForVoxel(const int64_t* ijk) const
{
    return getParcelForVoxel(ijk, m_dimToMapLookup[1]);
}

int64_t CiftiXMLOld::getRowParcelForVoxel(const int64_t* ijk) const
{
    return getParcelForVoxel(ijk, m_dimToMapLookup[0]);
}

bool CiftiXMLOld::setColumnNumberOfTimepoints(const int& numTimepoints)
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

bool CiftiXMLOld::setRowNumberOfTimepoints(const int& numTimepoints)
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

bool CiftiXMLOld::setTimestep(const float& seconds, const int& myMapIndex)
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

bool CiftiXMLOld::setColumnTimestep(const float& seconds)
{
    return setTimestep(seconds, m_dimToMapLookup[1]);
}

bool CiftiXMLOld::setRowTimestep(const float& seconds)
{
    return setTimestep(seconds, m_dimToMapLookup[0]);
}

bool CiftiXMLOld::setTimestart(const float& seconds, const int& myMapIndex)
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

bool CiftiXMLOld::setColumnTimestart(const float& seconds)
{
    return setTimestart(seconds, m_dimToMapLookup[1]);
}

bool CiftiXMLOld::setRowTimestart(const float& seconds)
{
    return setTimestart(seconds, m_dimToMapLookup[0]);
}

bool CiftiXMLOld::getVolumeAttributesForPlumb(VolumeSpace::OrientTypes orientOut[3], int64_t dimensionsOut[3], float originOut[3], float spacingOut[3]) const
{
    VolumeSpace mySpace;
    if (!getVolumeSpace(mySpace)) return false;
    const int64_t* myDims = mySpace.getDims();
    dimensionsOut[0] = myDims[0];
    dimensionsOut[1] = myDims[1];
    dimensionsOut[2] = myDims[2];
    mySpace.getOrientAndSpacingForPlumb(orientOut, spacingOut, originOut);
    return true;
}

bool CiftiXMLOld::getVolumeDimsAndSForm(int64_t dimsOut[3], vector<vector<float> >& sformOut) const
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

void CiftiXMLOld::setVolumeDimsAndSForm(const int64_t dims[3], const vector<vector<float> >& sform)
{
    CaretAssert(sform.size() == 3 || sform.size() == 4);
    if (hasVolumeData(ALONG_COLUMN) || hasVolumeData(ALONG_ROW))
    {
        VolumeSpace tempSpace;
        if (getVolumeSpace(tempSpace))//if it fails to get a volume space when it has volume data...allow it to set it, I guess
        {
            if (!tempSpace.matches(VolumeSpace(dims, sform)))
            {
                throw DataFileException("cannot change the volume space of cifti xml that already has volume mapping(s)");
            }
            return;
        }
    }
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
    myTrans.m_transform[12] = 0.0f;//force last row to be set to 0 0 0 1 internally for sanity, even though we don't use it
    myTrans.m_transform[13] = 0.0f;
    myTrans.m_transform[14] = 0.0f;
    myTrans.m_transform[15] = 1.0f;
    myTrans.m_unitsXYZ = NIFTI_UNITS_MM;
    myVol.m_volumeDimensions[0] = dims[0];
    myVol.m_volumeDimensions[1] = dims[1];
    myVol.m_volumeDimensions[2] = dims[2];
}

bool CiftiXMLOld::getVolumeSpace(VolumeSpace& volSpaceOut) const
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

AString CiftiXMLOld::getMapName(const int& direction, const int64_t& index) const
{
    if (m_dimToMapLookup[direction] == -1 || m_root.m_matrices.size() == 0)
    {
        return "";
    }
    CaretAssertVectorIndex(m_root.m_matrices[0].m_matrixIndicesMap, m_dimToMapLookup[direction]);
    const CiftiMatrixIndicesMapElement& myMap = m_root.m_matrices[0].m_matrixIndicesMap[m_dimToMapLookup[direction]];
    if (myMap.m_indicesMapToDataType != CIFTI_INDEX_TYPE_SCALARS &&
        myMap.m_indicesMapToDataType != CIFTI_INDEX_TYPE_LABELS)
    {
        return "";
    }
    CaretAssertVectorIndex(myMap.m_namedMaps, index);
    return myMap.m_namedMaps[index].m_mapName;
}

int64_t CiftiXMLOld::getMapIndexFromNameOrNumber(const int& direction, const AString& numberOrName) const
{
    if (m_dimToMapLookup[direction] == -1 || m_root.m_matrices.size() == 0)
    {
        return -1;
    }
    CaretAssertVectorIndex(m_root.m_matrices[0].m_matrixIndicesMap, m_dimToMapLookup[direction]);
    const CiftiMatrixIndicesMapElement& myMap = m_root.m_matrices[0].m_matrixIndicesMap[m_dimToMapLookup[direction]];
    bool ok = false;
    int32_t ret = numberOrName.toInt(&ok) - 1;//compensate for 1-indexing that command line parsing uses
    if (ok)//always work for integers, even when it is something like brain models or parcels, code that cares can check the mapping type
    {
        if (ret < 0 || ret >= getDimensionLength(direction))
        {
            ret = -1;
        }
    } else {//DO NOT search by name if the string was parsed as an integer correctly, or some idiot who names their maps as integers will get confused
            //when getting map "12" out of a file after the file expands to more than 12 elements suddenly does something different
        if (myMap.m_indicesMapToDataType != CIFTI_INDEX_TYPE_SCALARS &&
            myMap.m_indicesMapToDataType != CIFTI_INDEX_TYPE_LABELS)
        {
            return -1;//if we don't have map names to look at, return early rather than repeatedly trying to match against "" (which would be incorrect anyway)
        }
        int64_t numMaps = getDimensionLength(direction);
        ret = -1;
        for (int64_t i = 0; i < numMaps; ++i)
        {
            if (getMapName(direction, i) == numberOrName)
            {
                ret = i;
                break;
            }
        }
    }
    return ret;
}

AString CiftiXMLOld::getMapNameForColumnIndex(const int64_t& index) const
{
    return getMapName(ALONG_COLUMN, index);
}

AString CiftiXMLOld::getMapNameForRowIndex(const int64_t& index) const
{
    return getMapName(ALONG_ROW, index);
}

bool CiftiXMLOld::setMapNameForIndex(const int& direction, const int64_t& index, const AString& name) const
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

bool CiftiXMLOld::setMapNameForColumnIndex(const int64_t& index, const AString& name) const
{
    return setMapNameForIndex(ALONG_COLUMN, index, name);
}

bool CiftiXMLOld::setMapNameForRowIndex(const int64_t& index, const AString& name) const
{
    return setMapNameForIndex(ALONG_ROW, index, name);
}

GiftiLabelTable* CiftiXMLOld::getMapLabelTable(const int& direction, const int64_t& index) const
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

GiftiLabelTable* CiftiXMLOld::getLabelTableForColumnIndex(const int64_t& index) const
{
    return getMapLabelTable(ALONG_COLUMN, index);
}

GiftiLabelTable* CiftiXMLOld::getLabelTableForRowIndex(const int64_t& index) const
{
    return getMapLabelTable(ALONG_ROW, index);
}

bool CiftiXMLOld::setLabelTable(const int64_t& index, const GiftiLabelTable& labelTable, const int& myMapIndex)
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

bool CiftiXMLOld::setLabelTableForColumnIndex(const int64_t& index, const GiftiLabelTable& labelTable)
{
    return setLabelTable(index, labelTable, m_dimToMapLookup[1]);
}

bool CiftiXMLOld::setLabelTableForRowIndex(const int64_t& index, const GiftiLabelTable& labelTable)
{
    return setLabelTable(index, labelTable, m_dimToMapLookup[0]);
}

bool CiftiXMLOld::hasVolumeData(const int& direction) const
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

bool CiftiXMLOld::hasRowVolumeData() const
{
    return hasVolumeData(ALONG_ROW);
}

bool CiftiXMLOld::hasColumnVolumeData() const
{
    return hasVolumeData(ALONG_COLUMN);
}

bool CiftiXMLOld::hasColumnSurfaceData(const StructureEnum::Enum& structure) const
{
    return hasSurfaceData(ALONG_COLUMN, structure);
}

bool CiftiXMLOld::hasRowSurfaceData(const StructureEnum::Enum& structure) const
{
    return hasSurfaceData(ALONG_ROW, structure);
}

bool CiftiXMLOld::hasSurfaceData(const int& direction, const StructureEnum::Enum& structure) const
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

bool CiftiXMLOld::addSurfaceModelToColumns(const int& numberOfNodes, const StructureEnum::Enum& structure, const float* roi)
{
    return addSurfaceModel(ALONG_COLUMN, numberOfNodes, structure, roi);
}

bool CiftiXMLOld::addSurfaceModelToRows(const int& numberOfNodes, const StructureEnum::Enum& structure, const float* roi)
{
    return addSurfaceModel(ALONG_ROW, numberOfNodes, structure, roi);
}

bool CiftiXMLOld::addSurfaceModel(const int& direction, const int& numberOfNodes, const StructureEnum::Enum& structure, const float* roi)
{
    CaretAssertVectorIndex(m_dimToMapLookup, direction);
    separateMaps();
    if (m_dimToMapLookup[direction] == -1 || m_root.m_matrices.size() == 0)
    {
        return false;
    }
    CaretAssertVectorIndex(m_root.m_matrices[0].m_matrixIndicesMap, m_dimToMapLookup[direction]);
    CiftiMatrixIndicesMapElement* myMap = &(m_root.m_matrices[0].m_matrixIndicesMap[m_dimToMapLookup[direction]]);
    if (myMap->m_indicesMapToDataType != CIFTI_INDEX_TYPE_BRAIN_MODELS) return false;
    if (findSurfaceModel(m_dimToMapLookup[direction], structure) != NULL) return false;
    CiftiBrainModelElement tempModel;
    tempModel.m_brainStructure = structure;
    tempModel.m_modelType = CIFTI_MODEL_TYPE_SURFACE;
    tempModel.m_indexOffset = getNewRangeStart(m_dimToMapLookup[direction]);
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

bool CiftiXMLOld::addSurfaceModelToColumns(const int& numberOfNodes, const StructureEnum::Enum& structure, const vector<int64_t>& nodeList)
{
    return addSurfaceModel(ALONG_COLUMN, numberOfNodes, structure, nodeList);
}

bool CiftiXMLOld::addSurfaceModelToRows(const int& numberOfNodes, const StructureEnum::Enum& structure, const vector<int64_t>& nodeList)
{
    return addSurfaceModel(ALONG_ROW, numberOfNodes, structure, nodeList);
}

bool CiftiXMLOld::addSurfaceModel(const int& direction, const int& numberOfNodes, const StructureEnum::Enum& structure, const vector<int64_t>& nodeList)
{
    CaretAssertVectorIndex(m_dimToMapLookup, direction);
    separateMaps();
    if (m_dimToMapLookup[direction] == -1 || m_root.m_matrices.size() == 0)
    {
        return false;
    }
    CaretAssertVectorIndex(m_root.m_matrices[0].m_matrixIndicesMap, m_dimToMapLookup[direction]);
    CaretAssertMessage(checkSurfaceNodes(nodeList, numberOfNodes), "node list has node numbers that don't exist in the surface");
    CiftiMatrixIndicesMapElement* myMap = &(m_root.m_matrices[0].m_matrixIndicesMap[m_dimToMapLookup[direction]]);//call the check function inside an assert so it never does the check in release builds
    if (myMap->m_indicesMapToDataType != CIFTI_INDEX_TYPE_BRAIN_MODELS) return false;
    if (findSurfaceModel(m_dimToMapLookup[direction], structure) != NULL) return false;
    CiftiBrainModelElement tempModel;
    tempModel.m_brainStructure = structure;
    tempModel.m_modelType = CIFTI_MODEL_TYPE_SURFACE;
    tempModel.m_indexOffset = getNewRangeStart(m_dimToMapLookup[direction]);
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

bool CiftiXMLOld::checkSurfaceNodes(const vector<int64_t>& nodeList, const int& numberOfNodes) const
{
    int listSize = (int)nodeList.size();
    for (int i = 0; i < listSize; ++i)
    {
        if (nodeList[i] < 0 || nodeList[i] >= numberOfNodes) return false;
    }
    return true;
}

bool CiftiXMLOld::addVolumeModel(const int& direction, const vector<voxelIndexType>& ijkList, const StructureEnum::Enum& structure)
{
    CaretAssertVectorIndex(m_dimToMapLookup, direction);
    separateMaps();
    if (m_dimToMapLookup[direction] == -1 || m_root.m_matrices.size() == 0)
    {
        return false;
    }
    CaretAssertVectorIndex(m_root.m_matrices[0].m_matrixIndicesMap, m_dimToMapLookup[direction]);
    CiftiMatrixIndicesMapElement* myMap = &(m_root.m_matrices[0].m_matrixIndicesMap[m_dimToMapLookup[direction]]);
    if (myMap->m_indicesMapToDataType != CIFTI_INDEX_TYPE_BRAIN_MODELS) return false;
    if (findVolumeModel(m_dimToMapLookup[direction], structure) != NULL) return false;
    CaretAssertMessage(checkVolumeIndices(ijkList), "volume voxel list doesn't match cifti volume space, do setVolumeDimsAndSForm first");
    CiftiBrainModelElement tempModel;//call the check function inside an assert so it never does the check in release builds
    tempModel.m_brainStructure = structure;
    tempModel.m_modelType = CIFTI_MODEL_TYPE_VOXELS;
    tempModel.m_indexOffset = getNewRangeStart(m_dimToMapLookup[direction]);
    tempModel.m_indexCount = ijkList.size() / 3;
    tempModel.m_voxelIndicesIJK = ijkList;
    myMap->m_brainModels.push_back(tempModel);
    return true;
}

bool CiftiXMLOld::addVolumeModelToColumns(const vector<voxelIndexType>& ijkList, const StructureEnum::Enum& structure)
{
    return addVolumeModel(ALONG_COLUMN, ijkList, structure);
}

bool CiftiXMLOld::addVolumeModelToRows(const vector<voxelIndexType>& ijkList, const StructureEnum::Enum& structure)
{
    return addVolumeModel(ALONG_ROW, ijkList, structure);
}

bool CiftiXMLOld::addParcelSurfaceToColumns(const int& numberOfNodes, const StructureEnum::Enum& structure)
{
    return addParcelSurface(ALONG_COLUMN, numberOfNodes, structure);
}

bool CiftiXMLOld::addParcelSurfaceToRows(const int& numberOfNodes, const StructureEnum::Enum& structure)
{
    return addParcelSurface(ALONG_ROW, numberOfNodes, structure);
}

bool CiftiXMLOld::addParcelSurface(const int& direction, const int& numberOfNodes, const caret::StructureEnum::Enum& structure)
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

bool CiftiXMLOld::addParcelToColumns(const CiftiParcelElement& parcel)
{
    return addParcel(ALONG_COLUMN, parcel);
}

bool CiftiXMLOld::addParcelToRows(const caret::CiftiParcelElement& parcel)
{
    return addParcel(ALONG_ROW, parcel);
}

bool CiftiXMLOld::addParcel(const int& direction, const CiftiParcelElement& parcel)
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

bool CiftiXMLOld::checkVolumeIndices(const vector<voxelIndexType>& ijkList) const
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

void CiftiXMLOld::applyColumnMapToRows()
{
    if (m_dimToMapLookup[0] == m_dimToMapLookup[1]) return;
    applyDimensionHelper(1, 0);
}

void CiftiXMLOld::applyRowMapToColumns()
{
    if (m_dimToMapLookup[0] == m_dimToMapLookup[1]) return;
    applyDimensionHelper(0, 1);
}

void CiftiXMLOld::applyDimensionHelper(const int& from, const int& to)
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

void CiftiXMLOld::resetColumnsToBrainModels()
{
    resetDirectionToBrainModels(ALONG_COLUMN);
}

void CiftiXMLOld::resetRowsToBrainModels()
{
    resetDirectionToBrainModels(ALONG_ROW);
}

void CiftiXMLOld::resetDirectionToBrainModels(const int& direction)
{
    if (m_dimToMapLookup[direction] == -1)
    {
        m_dimToMapLookup[direction] = createMap(direction);
    } else {
        separateMaps();
    }
    CiftiMatrixIndicesMapElement myMap;
    myMap.m_appliesToMatrixDimension.push_back(direction);
    myMap.m_indicesMapToDataType = CIFTI_INDEX_TYPE_BRAIN_MODELS;
    m_root.m_matrices[0].m_matrixIndicesMap[m_dimToMapLookup[direction]] = myMap;
}

void CiftiXMLOld::resetColumnsToTimepoints(const float& timestep, const int& timepoints, const float& timestart)
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

void CiftiXMLOld::resetRowsToTimepoints(const float& timestep, const int& timepoints, const float& timestart)
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

void CiftiXMLOld::resetColumnsToScalars(const int64_t& numMaps)
{
    resetDirectionToScalars(ALONG_COLUMN, numMaps);
}

void CiftiXMLOld::resetRowsToScalars(const int64_t& numMaps)
{
    resetDirectionToScalars(ALONG_ROW, numMaps);
}

void CiftiXMLOld::resetDirectionToScalars(const int& direction, const int64_t& numMaps)
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
    myMap.m_indicesMapToDataType = CIFTI_INDEX_TYPE_SCALARS;
    myMap.m_namedMaps.resize(numMaps);
    m_root.m_matrices[0].m_matrixIndicesMap[m_dimToMapLookup[direction]] = myMap;
}

void CiftiXMLOld::resetColumnsToLabels(const int64_t& numMaps)
{
    resetDirectionToLabels(ALONG_COLUMN, numMaps);
}

void CiftiXMLOld::resetRowsToLabels(const int64_t& numMaps)
{
    resetDirectionToLabels(ALONG_ROW, numMaps);
}

void CiftiXMLOld::resetDirectionToLabels(const int& direction, const int64_t& numMaps)
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
    myMap.m_indicesMapToDataType = CIFTI_INDEX_TYPE_LABELS;
    myMap.m_namedMaps.resize(numMaps);
    for (int64_t i = 0; i < numMaps; ++i)
    {
        myMap.m_namedMaps[i].m_labelTable.grabNew(new GiftiLabelTable());
    }
    m_root.m_matrices[0].m_matrixIndicesMap[m_dimToMapLookup[direction]] = myMap;
}

void CiftiXMLOld::resetColumnsToParcels()
{
    resetDirectionToParcels(ALONG_COLUMN);
}

void CiftiXMLOld::resetRowsToParcels()
{
    resetDirectionToParcels(ALONG_ROW);
}

void CiftiXMLOld::resetDirectionToParcels(const int& direction)
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

int CiftiXMLOld::createMap(int dimension)
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

void CiftiXMLOld::separateMaps()
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

int64_t CiftiXMLOld::getNewRangeStart(const int& myMapIndex) const
{
    if (myMapIndex == -1 || m_root.m_matrices.size() == 0)
    {
        CaretAssert(false);
    }
    CaretAssertVectorIndex(m_root.m_matrices[0].m_matrixIndicesMap, myMapIndex);
    const CiftiMatrixIndicesMapElement* myMap = &(m_root.m_matrices[0].m_matrixIndicesMap[myMapIndex]);
    CaretAssert(myMap != NULL && myMap->m_indicesMapToDataType == CIFTI_INDEX_TYPE_BRAIN_MODELS);
    int numModels = (int)myMap->m_brainModels.size();
    int64_t curRet = 0;
    for (int i = 0; i < numModels; ++i)
    {
        int64_t thisEnd = myMap->m_brainModels[i].m_indexOffset + myMap->m_brainModels[i].m_indexCount;
        if (thisEnd > curRet)
        {
            curRet = thisEnd;
        }
    }
    return curRet;
}

int64_t CiftiXMLOld::getDimensionLength(const int& direction) const
{
    if (direction < 0 || direction >= (int)m_dimToMapLookup.size())
    {
        throw DataFileException("getDimensionLength called on nonexistant dimension");
    }
    int myMapIndex = m_dimToMapLookup[direction];
    if (myMapIndex == -1 || m_root.m_matrices.size() == 0)
    {
        throw DataFileException("getDimensionLength called on nonexistant dimension");
    }
    CaretAssertVectorIndex(m_root.m_matrices[0].m_matrixIndicesMap, myMapIndex);
    const CiftiMatrixIndicesMapElement* myMap = &(m_root.m_matrices[0].m_matrixIndicesMap[myMapIndex]);
    if (myMap->m_indicesMapToDataType == CIFTI_INDEX_TYPE_TIME_POINTS)
    {
        return myMap->m_numTimeSteps;
    } else if (myMap->m_indicesMapToDataType == CIFTI_INDEX_TYPE_BRAIN_MODELS) {
        return getNewRangeStart(m_dimToMapLookup[direction]);
    } else if (myMap->m_indicesMapToDataType == CIFTI_INDEX_TYPE_SCALARS || myMap->m_indicesMapToDataType == CIFTI_INDEX_TYPE_LABELS) {
        return myMap->m_namedMaps.size();
    } else if (myMap->m_indicesMapToDataType == CIFTI_INDEX_TYPE_PARCELS) {
        return myMap->m_parcels.size();
    } else {
        throw DataFileException("unknown cifti mapping type");
    }
}

int64_t CiftiXMLOld::getNumberOfColumns() const
{//number of columns is LENGTH OF A ROW
    return getDimensionLength(ALONG_ROW);
}

int64_t CiftiXMLOld::getNumberOfRows() const
{
    return getDimensionLength(ALONG_COLUMN);
}

IndicesMapToDataType CiftiXMLOld::getColumnMappingType() const
{
    if (m_dimToMapLookup[1] == -1 || m_root.m_matrices.size() == 0)
    {
        return CIFTI_INDEX_TYPE_INVALID;
    }
    CaretAssertVectorIndex(m_root.m_matrices[0].m_matrixIndicesMap, m_dimToMapLookup[1]);
    const CiftiMatrixIndicesMapElement* myMap = &(m_root.m_matrices[0].m_matrixIndicesMap[m_dimToMapLookup[1]]);
    return myMap->m_indicesMapToDataType;
}

IndicesMapToDataType CiftiXMLOld::getRowMappingType() const
{
    if (m_dimToMapLookup[0] == -1 || m_root.m_matrices.size() == 0)
    {
        return CIFTI_INDEX_TYPE_INVALID;
    }
    CaretAssertVectorIndex(m_root.m_matrices[0].m_matrixIndicesMap, m_dimToMapLookup[0]);
    const CiftiMatrixIndicesMapElement* myMap = &(m_root.m_matrices[0].m_matrixIndicesMap[m_dimToMapLookup[0]]);
    return myMap->m_indicesMapToDataType;
}

IndicesMapToDataType CiftiXMLOld::getMappingType(const int& direction) const
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

const CiftiBrainModelElement* CiftiXMLOld::findSurfaceModel(const int& myMapIndex, const StructureEnum::Enum& structure) const
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
        if (myModels[i].m_brainStructure == structure && myModels[i].m_modelType == CIFTI_MODEL_TYPE_SURFACE)
        {
            return &(myModels[i]);
        }
    }
    return NULL;
}

const CiftiBrainModelElement* CiftiXMLOld::findVolumeModel(const int& myMapIndex, const StructureEnum::Enum& structure) const
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
        if (myModels[i].m_brainStructure == structure && myModels[i].m_modelType == CIFTI_MODEL_TYPE_VOXELS)
        {
            return &(myModels[i]);
        }
    }
    return NULL;
}

bool CiftiXMLOld::matchesForColumns(const CiftiXMLOld& rhs) const
{
    return mappingMatches(ALONG_COLUMN, rhs, ALONG_COLUMN);
}

bool CiftiXMLOld::matchesForRows(const CiftiXMLOld& rhs) const
{
    return mappingMatches(ALONG_ROW, rhs, ALONG_ROW);
}

bool CiftiXMLOld::mappingMatches(const int& direction, const CiftiXMLOld& other, const int& otherDirection) const
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

void CiftiXMLOld::copyMapping(const int& direction, const CiftiXMLOld& other, const int& otherDirection)
{
    CaretAssert(direction > -1 && otherDirection > -1);
    if ((int)other.m_dimToMapLookup.size() <= otherDirection || other.m_dimToMapLookup[otherDirection] == -1)
    {
        throw DataFileException("copyMapping called with nonexistant mapping to copy");
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
            if (!matchesVolumeSpace(other)) throw DataFileException("cannot copy mapping from other cifti due to volume space mismatch");
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

map<AString, AString>* CiftiXMLOld::getMapMetadata(const int& direction, const int& index) const
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

PaletteColorMapping* CiftiXMLOld::getFilePalette() const
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

PaletteColorMapping* CiftiXMLOld::getMapPalette(const int& direction, const int& index) const
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

bool CiftiXMLOld::operator==(const caret::CiftiXMLOld& rhs) const
{
    if (this == &rhs) return true;//compare pointers to skip checking object against itself
    if (m_root.m_matrices.size() != 1 || m_root.m_matrices[0].m_matrixIndicesMap.size() != 2)
    {
        throw DataFileException("old CIFTI XML implementation only supports single-matrix, 2D cifti");
    }
    if (m_root.m_matrices.size() != rhs.m_root.m_matrices.size()) return false;
    if (m_root.m_matrices[0].m_matrixIndicesMap.size() != rhs.m_root.m_matrices[0].m_matrixIndicesMap.size()) return false;
    if (!matchesVolumeSpace(rhs)) return false;
    if (!matchesForColumns(rhs)) return false;
    if (!matchesForRows(rhs)) return false;
    return true;
}

bool CiftiXMLOld::matchesVolumeSpace(const CiftiXMLOld& rhs) const
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
        throw DataFileException("has*VolumeData() and getVolumeDimsAndSForm() disagree");
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

void CiftiXMLOld::swapMappings(const int& direction1, const int& direction2)
{
    CaretAssertVectorIndex(m_dimToMapLookup, direction1);
    CaretAssertVectorIndex(m_dimToMapLookup, direction2);
    if (direction1 < 0 || direction1 >= (int)m_dimToMapLookup.size() ||
        direction2 < 0 || direction2 >= (int)m_dimToMapLookup.size())
    {
        throw DataFileException("invalid direction specified to swapMappings, notify the developers");
    }
    int mapIndex1 = m_dimToMapLookup[direction1];
    int mapIndex2 = m_dimToMapLookup[direction2];
    if (mapIndex1 == -1 || mapIndex2 == -1 || m_root.m_matrices.size() == 0)
    {
        throw DataFileException("invalid direction specified to swapMappings, notify the developers");
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


