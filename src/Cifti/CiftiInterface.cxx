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

#include "CiftiInterface.h"

#include "DataFileException.h"

#include <limits>
#include <vector>

using namespace caret;
using namespace std;

CiftiInterface::CiftiInterface()
{
    m_dataRangeValid = false;
}

void CiftiInterface::invalidateDataRange()
{
    m_dataRangeValid = false;
}

bool CiftiInterface::getDataRangeFromAllMaps(float& minOut, float& maxOut) const
{
    if (!m_dataRangeValid)
    {
        int64_t numRows = getNumberOfRows(), rowSize = getNumberOfColumns();
        if (numRows <= 0 || rowSize <= 0)
        {
            maxOut = numeric_limits<float>::max();
            minOut = -maxOut;
            return false;
        }
        m_dataRangeMin = numeric_limits<float>::max();
        m_dataRangeMax = -m_dataRangeMin;
        vector<float> tempRow(rowSize);
        for (int64_t row = 0; row < numRows; ++row)
        {
            getRow(tempRow.data(), row);
            for (int64_t i = 0; i < rowSize; ++i)
            {
                if (tempRow[i] > m_dataRangeMax) m_dataRangeMax = tempRow[i];
                if (tempRow[i] < m_dataRangeMin) m_dataRangeMin = tempRow[i];
            }
        }
        m_dataRangeValid = true;
    }
    minOut = m_dataRangeMin;
    maxOut = m_dataRangeMax;
    return true;
}

CiftiXMLOld CiftiInterface::getCiftiXMLOld() const
{
    CiftiXMLOld ret;
    if (m_xml.getNumberOfDimensions() != 2) throw DataFileException("can't convert to old XML because number of dimensions isn't 2");
    ret.readXML(m_xml.writeXMLToString(CiftiVersion(1, 0)));
    if (ret.getDimensionLength(CiftiXMLOld::ALONG_ROW) < 1)
    {
        ret.setRowNumberOfTimepoints(m_xml.getDimensionLength(CiftiXML::ALONG_ROW));
    }
    if (ret.getDimensionLength(CiftiXMLOld::ALONG_COLUMN) < 1)
    {
        ret.setColumnNumberOfTimepoints(m_xml.getDimensionLength(CiftiXML::ALONG_COLUMN));
    }
    return ret;
}

bool CiftiInterface::checkColumnIndex(int64_t index) const
{
    if (index < 0 || index >= getNumberOfColumns())
    {
        return false;
    }
    return true;
}

bool CiftiInterface::checkRowIndex(int64_t index) const
{
    if (index < 0 || index >= getNumberOfRows())
    {
        return false;
    }
    return true;
}

bool CiftiInterface::getColumnFromNode(float* columnOut, const int64_t node, const caret::StructureEnum::Enum structure) const
{
    int64_t myIndex = -1;
    const CiftiMappingType& myGenMap = *(m_xml.getMap(CiftiXML::ALONG_ROW));
    if (myGenMap.getType() == CiftiMappingType::BRAIN_MODELS)
    {
        const CiftiBrainModelsMap& myMap = (const CiftiBrainModelsMap&)myGenMap;
        myIndex = myMap.getIndexForNode(node, structure);
    } else if (myGenMap.getType() == CiftiMappingType::PARCELS) {
        const CiftiParcelsMap& myMap = (const CiftiParcelsMap&)myGenMap;
        myIndex = myMap.getIndexForNode(node, structure);
    }
    if (!checkColumnIndex(myIndex)) return false;
    getColumn(columnOut, myIndex);
    return true;
}

bool CiftiInterface::getColumnFromVoxel(float* columnOut, const int64_t* ijk) const
{
    int64_t myIndex = -1;
    const CiftiMappingType& myGenMap = *(m_xml.getMap(CiftiXML::ALONG_ROW));
    if (myGenMap.getType() == CiftiMappingType::BRAIN_MODELS)
    {
        const CiftiBrainModelsMap& myMap = (const CiftiBrainModelsMap&)myGenMap;
        myIndex = myMap.getIndexForVoxel(ijk);
    } else if (myGenMap.getType() == CiftiMappingType::PARCELS) {
        const CiftiParcelsMap& myMap = (const CiftiParcelsMap&)myGenMap;
        myIndex = myMap.getIndexForVoxel(ijk);
    }
    if (!checkColumnIndex(myIndex)) return false;
    getColumn(columnOut, myIndex);
    return true;
}

bool CiftiInterface::getRowFromNode(float* rowOut, const int64_t node, const caret::StructureEnum::Enum structure) const
{
    int64_t myIndex = -1;
    const CiftiMappingType& myGenMap = *(m_xml.getMap(CiftiXML::ALONG_COLUMN));
    if (myGenMap.getType() == CiftiMappingType::BRAIN_MODELS)
    {
        const CiftiBrainModelsMap& myMap = (const CiftiBrainModelsMap&)myGenMap;
        myIndex = myMap.getIndexForNode(node, structure);
    } else if (myGenMap.getType() == CiftiMappingType::PARCELS) {
        const CiftiParcelsMap& myMap = (const CiftiParcelsMap&)myGenMap;
        myIndex = myMap.getIndexForNode(node, structure);
    }
    if (!checkRowIndex(myIndex)) return false;
    getRow(rowOut, myIndex);
    return true;
}

bool CiftiInterface::getRowFromNode(float* rowOut, const int64_t node, const caret::StructureEnum::Enum structure, int64_t& rowIndexOut) const
{
    rowIndexOut = -1;
    const CiftiMappingType& myGenMap = *(m_xml.getMap(CiftiXML::ALONG_COLUMN));
    if (myGenMap.getType() == CiftiMappingType::BRAIN_MODELS)
    {
        const CiftiBrainModelsMap& myMap = (const CiftiBrainModelsMap&)myGenMap;
        rowIndexOut = myMap.getIndexForNode(node, structure);
    } else if (myGenMap.getType() == CiftiMappingType::PARCELS) {
        const CiftiParcelsMap& myMap = (const CiftiParcelsMap&)myGenMap;
        rowIndexOut = myMap.getIndexForNode(node, structure);
    }
    if (!checkRowIndex(rowIndexOut)) return false;
    getRow(rowOut, rowIndexOut);
    return true;
}

bool CiftiInterface::getRowFromVoxel(float* rowOut, const int64_t* ijk) const
{
    int64_t myIndex = -1;
    const CiftiMappingType& myGenMap = *(m_xml.getMap(CiftiXML::ALONG_COLUMN));
    if (myGenMap.getType() == CiftiMappingType::BRAIN_MODELS)
    {
        const CiftiBrainModelsMap& myMap = (const CiftiBrainModelsMap&)myGenMap;
        myIndex = myMap.getIndexForVoxel(ijk);
    } else if (myGenMap.getType() == CiftiMappingType::PARCELS) {
        const CiftiParcelsMap& myMap = (const CiftiParcelsMap&)myGenMap;
        myIndex = myMap.getIndexForVoxel(ijk);
    }
    if (!checkRowIndex(myIndex)) return false;
    getRow(rowOut, myIndex);
    return true;
}

bool CiftiInterface::getColumnFromVoxelCoordinate(float* columnOut, const float* xyz) const
{
    int64_t myIndex = -1;
    int64_t ijk[3];
    const CiftiMappingType& myGenMap = *(m_xml.getMap(CiftiXML::ALONG_ROW));
    if (myGenMap.getType() == CiftiMappingType::BRAIN_MODELS)
    {
        const CiftiBrainModelsMap& myMap = (const CiftiBrainModelsMap&)myGenMap;
        myMap.getVolumeSpace().enclosingVoxel(xyz, ijk);
        myIndex = myMap.getIndexForVoxel(ijk);
    } else if (myGenMap.getType() == CiftiMappingType::PARCELS) {
        const CiftiParcelsMap& myMap = (const CiftiParcelsMap&)myGenMap;
        myMap.getVolumeSpace().enclosingVoxel(xyz, ijk);
        myIndex = myMap.getIndexForVoxel(ijk);
    }
    if (!checkColumnIndex(myIndex)) return false;
    getColumn(columnOut, myIndex);
    return true;
}

bool CiftiInterface::getRowFromVoxelCoordinate(float* rowOut, const float* xyz) const
{
    int64_t myIndex = -1;
    int64_t ijk[3];
    const CiftiMappingType& myGenMap = *(m_xml.getMap(CiftiXML::ALONG_COLUMN));
    if (myGenMap.getType() == CiftiMappingType::BRAIN_MODELS)
    {
        const CiftiBrainModelsMap& myMap = (const CiftiBrainModelsMap&)myGenMap;
        myMap.getVolumeSpace().enclosingVoxel(xyz, ijk);
        myIndex = myMap.getIndexForVoxel(ijk);
    } else if (myGenMap.getType() == CiftiMappingType::PARCELS) {
        const CiftiParcelsMap& myMap = (const CiftiParcelsMap&)myGenMap;
        myMap.getVolumeSpace().enclosingVoxel(xyz, ijk);
        myIndex = myMap.getIndexForVoxel(ijk);
    }
    if (!checkRowIndex(myIndex)) return false;
    getRow(rowOut, myIndex);
    return true;
}

bool CiftiInterface::getRowFromVoxelCoordinate(float* rowOut, const float* xyz, int64_t& rowIndexOut) const
{
    rowIndexOut = -1;
    int64_t ijk[3];
    const CiftiMappingType& myGenMap = *(m_xml.getMap(CiftiXML::ALONG_COLUMN));
    if (myGenMap.getType() == CiftiMappingType::BRAIN_MODELS)
    {
        const CiftiBrainModelsMap& myMap = (const CiftiBrainModelsMap&)myGenMap;
        myMap.getVolumeSpace().enclosingVoxel(xyz, ijk);
        rowIndexOut = myMap.getIndexForVoxel(ijk);
    } else if (m_xml.getMappingType(CiftiXML::ALONG_ROW) == CiftiMappingType::PARCELS) {
        const CiftiParcelsMap& myMap = (const CiftiParcelsMap&)myGenMap;
        myMap.getVolumeSpace().enclosingVoxel(xyz, ijk);
        rowIndexOut = myMap.getIndexForVoxel(ijk);
    }
    if (!checkRowIndex(rowIndexOut)) return false;
    getRow(rowOut, rowIndexOut);
    return true;
}

bool CiftiInterface::getColumnFromTimepoint(float* columnOut, const float seconds) const
{
    const CiftiMappingType& myGenMap = *(m_xml.getMap(CiftiXML::ALONG_ROW));
    if (myGenMap.getType() != CiftiMappingType::SERIES) return false;
    const CiftiSeriesMap& myMap = (const CiftiSeriesMap&)myGenMap;
    int64_t myIndex = (int64_t)floor((seconds - myMap.getStart()) / myMap.getStep() + 0.5f);
    if (!checkColumnIndex(myIndex)) return false;
    getColumn(columnOut, myIndex);
    return true;
}

//column and frame are the same value currently, this function exists only
//to keep the concepts of frame and time separate from being conflated
bool CiftiInterface::getColumnFromFrame(float* columnOut, const int frame) const
{
    if(!checkColumnIndex(frame))
    {
        return false;
    }
    getColumn(columnOut, frame);
    return true;
}


bool CiftiInterface::getRowFromTimepoint(float* rowOut, const float seconds) const
{
    const CiftiMappingType& myGenMap = *(m_xml.getMap(CiftiXML::ALONG_COLUMN));
    if (myGenMap.getType() != CiftiMappingType::SERIES) return false;
    const CiftiSeriesMap& myMap = (const CiftiSeriesMap&)myGenMap;
    int64_t myIndex = (int64_t)floor((seconds - myMap.getStart()) / myMap.getStep() + 0.5f);
    if (!checkColumnIndex(myIndex)) return false;
    getRow(rowOut, myIndex);
    return true;
}

bool CiftiInterface::getSurfaceMapForRows(vector<CiftiBrainModelsMap::SurfaceMap>& mappingOut, const StructureEnum::Enum structure) const
{
    if (m_xml.getMappingType(CiftiXML::ALONG_ROW) != CiftiMappingType::BRAIN_MODELS) return false;
    mappingOut = m_xml.getBrainModelsMap(CiftiXML::ALONG_ROW).getSurfaceMap(structure);//will throw on structure missing
    return true;
}

bool CiftiInterface::getSurfaceMapForColumns(vector<CiftiBrainModelsMap::SurfaceMap>& mappingOut, const StructureEnum::Enum structure) const
{
    if (m_xml.getMappingType(CiftiXML::ALONG_COLUMN) != CiftiMappingType::BRAIN_MODELS) return false;
    mappingOut = m_xml.getBrainModelsMap(CiftiXML::ALONG_COLUMN).getSurfaceMap(structure);//will throw on structure missing
    return true;
}

bool CiftiInterface::getVolumeMapForRows(vector<CiftiBrainModelsMap::VolumeMap>& mappingOut) const
{
    if (m_xml.getMappingType(CiftiXML::ALONG_ROW) != CiftiMappingType::BRAIN_MODELS) return false;
    mappingOut = m_xml.getBrainModelsMap(CiftiXML::ALONG_ROW).getFullVolumeMap();
    return true;
}

bool CiftiInterface::getVolumeMapForColumns(vector<CiftiBrainModelsMap::VolumeMap>& mappingOut) const
{
    if (m_xml.getMappingType(CiftiXML::ALONG_COLUMN) != CiftiMappingType::BRAIN_MODELS) return false;
    mappingOut = m_xml.getBrainModelsMap(CiftiXML::ALONG_COLUMN).getFullVolumeMap();
    return true;
}

int64_t CiftiInterface::getRowSurfaceNumberOfNodes(const StructureEnum::Enum structure) const
{
    if (m_xml.getMappingType(CiftiXML::ALONG_ROW) == CiftiMappingType::BRAIN_MODELS) {
        return m_xml.getBrainModelsMap(CiftiXML::ALONG_ROW).getSurfaceNumberOfNodes(structure);//will throw on structure missing
    }
    
    if (m_xml.getMappingType(CiftiXML::ALONG_ROW) == CiftiMappingType::PARCELS) {
        return m_xml.getParcelsMap(CiftiXML::ALONG_ROW).getSurfaceNumberOfNodes(structure);
    }
    
    return -1;
    
//    if (m_xml.getMappingType(CiftiXML::ALONG_ROW) != CiftiMappingType::BRAIN_MODELS) return -1;
//    return m_xml.getBrainModelsMap(CiftiXML::ALONG_ROW).getSurfaceNumberOfNodes(structure);//will throw on structure missing
}

int64_t CiftiInterface::getColumnSurfaceNumberOfNodes(const StructureEnum::Enum structure) const
{
    if (m_xml.getMappingType(CiftiXML::ALONG_COLUMN) == CiftiMappingType::BRAIN_MODELS) {
        return m_xml.getBrainModelsMap(CiftiXML::ALONG_COLUMN).getSurfaceNumberOfNodes(structure);//will throw on structure missing
    }
    
    if (m_xml.getMappingType(CiftiXML::ALONG_COLUMN) == CiftiMappingType::PARCELS) {
        return m_xml.getParcelsMap(CiftiXML::ALONG_COLUMN).getSurfaceNumberOfNodes(structure);
    }
    
    return -1;
    
//    if (m_xml.getMappingType(CiftiXML::ALONG_COLUMN) != CiftiMappingType::BRAIN_MODELS) return -1;
//    return m_xml.getBrainModelsMap(CiftiXML::ALONG_COLUMN).getSurfaceNumberOfNodes(structure);//will throw on structure missing
}

bool CiftiInterface::getRowTimestep(float& seconds) const
{
    if (m_xml.getMappingType(CiftiXML::ALONG_ROW) != CiftiMappingType::SERIES) return false;
    seconds = m_xml.getSeriesMap(CiftiXML::ALONG_ROW).getStep();
    return true;
}

bool CiftiInterface::getColumnTimestep(float& seconds) const
{
    if (m_xml.getMappingType(CiftiXML::ALONG_COLUMN) != CiftiMappingType::SERIES) return false;
    seconds = m_xml.getSeriesMap(CiftiXML::ALONG_COLUMN).getStep();
    return true;
}

bool CiftiInterface::getVolumeAttributesForPlumb(VolumeSpace::OrientTypes orientOut[3], int64_t dimensionsOut[3], float originOut[3], float spacingOut[3]) const
{
    for (int i = 0; i < m_xml.getNumberOfDimensions(); ++i)
    {
        if (m_xml.getMappingType(i) == CiftiMappingType::BRAIN_MODELS)
        {
            const CiftiBrainModelsMap& myMap = m_xml.getBrainModelsMap(i);
            if (myMap.hasVolumeData())
            {
                if (!myMap.getVolumeSpace().isPlumb()) return false;
                myMap.getVolumeSpace().getOrientAndSpacingForPlumb(orientOut, spacingOut, originOut);
                const int64_t* dims = myMap.getVolumeSpace().getDims();
                dimensionsOut[0] = dims[0];
                dimensionsOut[1] = dims[1];
                dimensionsOut[2] = dims[2];
                return true;
            }
        } else if (m_xml.getMappingType(i) == CiftiMappingType::PARCELS) {
            const CiftiParcelsMap& myMap = m_xml.getParcelsMap(i);
            if (myMap.hasVolumeData())
            {
                if (!myMap.getVolumeSpace().isPlumb()) return false;
                myMap.getVolumeSpace().getOrientAndSpacingForPlumb(orientOut, spacingOut, originOut);
                const int64_t* dims = myMap.getVolumeSpace().getDims();
                dimensionsOut[0] = dims[0];
                dimensionsOut[1] = dims[1];
                dimensionsOut[2] = dims[2];
                return true;
            }
        }
    }
    return false;
}

bool CiftiInterface::hasRowVolumeData() const
{
    if (m_xml.getMappingType(CiftiXML::ALONG_ROW) == CiftiMappingType::BRAIN_MODELS)
    {
        return m_xml.getBrainModelsMap(CiftiXML::ALONG_ROW).hasVolumeData();
    } else if (m_xml.getMappingType(CiftiXML::ALONG_ROW) == CiftiMappingType::PARCELS) {
        return m_xml.getParcelsMap(CiftiXML::ALONG_ROW).hasVolumeData();
    }
    return false;
}

bool CiftiInterface::hasColumnVolumeData() const
{
    if (m_xml.getMappingType(CiftiXML::ALONG_COLUMN) == CiftiMappingType::BRAIN_MODELS)
    {
        return m_xml.getBrainModelsMap(CiftiXML::ALONG_COLUMN).hasVolumeData();
    } else if (m_xml.getMappingType(CiftiXML::ALONG_COLUMN) == CiftiMappingType::PARCELS) {
        return m_xml.getParcelsMap(CiftiXML::ALONG_COLUMN).hasVolumeData();
    }
    return false;
}

bool CiftiInterface::hasRowSurfaceData(const StructureEnum::Enum structure) const
{
    if (m_xml.getMappingType(CiftiXML::ALONG_ROW) == CiftiMappingType::BRAIN_MODELS)
    {
        return m_xml.getBrainModelsMap(CiftiXML::ALONG_ROW).hasSurfaceData(structure);
    } else if (m_xml.getMappingType(CiftiXML::ALONG_ROW) == CiftiMappingType::PARCELS) {
        return m_xml.getParcelsMap(CiftiXML::ALONG_ROW).hasSurfaceData(structure);
    }
    return false;
}
bool CiftiInterface::hasColumnSurfaceData(const StructureEnum::Enum structure) const
{
    if (m_xml.getMappingType(CiftiXML::ALONG_COLUMN) == CiftiMappingType::BRAIN_MODELS)
    {
        return m_xml.getBrainModelsMap(CiftiXML::ALONG_COLUMN).hasSurfaceData(structure);
    } else if (m_xml.getMappingType(CiftiXML::ALONG_COLUMN) == CiftiMappingType::PARCELS) {
        return m_xml.getParcelsMap(CiftiXML::ALONG_COLUMN).hasSurfaceData(structure);
    }
    return false;
}

AString CiftiInterface::getMapNameForColumnIndex(const int& index) const
{
    if (m_xml.getMappingType(CiftiXML::ALONG_COLUMN) == CiftiMappingType::SCALARS)
    {
        return m_xml.getScalarsMap(CiftiXML::ALONG_COLUMN).getMapName(index);
    } else if (m_xml.getMappingType(CiftiXML::ALONG_COLUMN) == CiftiMappingType::LABELS) {
        return m_xml.getLabelsMap(CiftiXML::ALONG_COLUMN).getMapName(index);
    }
    return "";
}

AString CiftiInterface::getMapNameForRowIndex(const int& index) const
{
    if (m_xml.getMappingType(CiftiXML::ALONG_ROW) == CiftiMappingType::SCALARS)
    {
        return m_xml.getScalarsMap(CiftiXML::ALONG_ROW).getMapName(index);
    } else if (m_xml.getMappingType(CiftiXML::ALONG_ROW) == CiftiMappingType::LABELS) {
        return m_xml.getLabelsMap(CiftiXML::ALONG_ROW).getMapName(index);
    }
    return "";
}

CiftiInterface::~CiftiInterface()
{
}
