#ifndef __CIFTI_INTERFACE_H__
#define __CIFTI_INTERFACE_H__

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

#include "CiftiXML.h"
#include "CiftiXMLOld.h"

namespace caret
{
    
    class CiftiInterface
    {
        mutable bool m_dataRangeValid;
        mutable float m_dataRangeMin, m_dataRangeMax;
    protected:
        CiftiXML m_xml;        
        CiftiInterface();
    public:
        bool checkRowIndex(int64_t index) const;
        bool checkColumnIndex(int64_t index) const;

        ///get a row
        virtual void getRow(float* rowOut, const int64_t& rowIndex) const = 0;
        
        ///get a column
        virtual void getColumn(float* columnOut, const int64_t& columnIndex) const = 0;
        
        ///get row size
        virtual int64_t getNumberOfColumns() const = 0;
        
        ///get column size
        virtual int64_t getNumberOfRows() const = 0;

        ///get a reference to the XML structure
        const CiftiXML& getCiftiXML() const { return m_xml; }
        
        ///get the old XML structure
        CiftiXMLOld getCiftiXMLOld() const;
        
        ///get a row by surface and node - returns false if not found in mapping
        bool getRowFromNode(float* rowOut, const int64_t node, const StructureEnum::Enum structure) const;
        
        ///get a row by surface and node - returns false if not found in mapping
        bool getRowFromNode(float* rowOut, const int64_t node, const StructureEnum::Enum structure, int64_t& rowIndexOut) const;
        
        ///get a column by surface and node - returns false if not found in mapping
        bool getColumnFromNode(float* columnOut, const int64_t node, const StructureEnum::Enum structure) const;
        
        ///get a row by voxel index - returns false if not found in mapping
        bool getRowFromVoxel(float* rowOut, const int64_t* ijk) const;
        
        ///get a column by voxel index - returns false if not found in mapping
        bool getColumnFromVoxel(float* columnOut, const int64_t* ijk) const;
        
        ///get a row by voxel coordinate - returns false if not found in mapping
        bool getRowFromVoxelCoordinate(float* rowOut, const float* xyz) const;
        
        ///get a row by voxel coordinate - returns false if not found in mapping
        bool getRowFromVoxelCoordinate(float* rowOut, const float* xyz, int64_t& rowIndexOut) const;
        
        ///get a column by voxel coordinate - returns false if not found in mapping
        bool getColumnFromVoxelCoordinate(float* columnOut, const float* xyz) const;
        
        ///get a row by timepoint
        bool getRowFromTimepoint(float* rowOut, const float seconds) const;
        
        ///get a column by timepoint
        bool getColumnFromTimepoint(float* columnOut, const float seconds) const;
        
        ///get a column by frame
        bool getColumnFromFrame(float* columnOut, const int frame) const;
        
        ///get data range
        bool getDataRangeFromAllMaps(float& minOut, float& maxOut) const;
        
        ///called when something changes the data
        void invalidateDataRange();
        
        ///get the mapping for a surface in rows, returns false and empty vector if not found
        bool getSurfaceMapForRows(std::vector<CiftiBrainModelsMap::SurfaceMap>& mappingOut, const StructureEnum::Enum structure) const;
        
        ///get the mapping for a surface in columns, returns false and empty vector if not found
        bool getSurfaceMapForColumns(std::vector<CiftiBrainModelsMap::SurfaceMap>& mappingOut, const StructureEnum::Enum structure) const;
            
        ///get the mapping for a surface in rows, returns false and empty vector if not found
        bool getVolumeMapForRows(std::vector<CiftiBrainModelsMap::VolumeMap>& mappingOut) const;
        
        ///get the mapping for a surface in columns, returns false and empty vector if not found
        bool getVolumeMapForColumns(std::vector<CiftiBrainModelsMap::VolumeMap>& mappingOut) const;
        
        ///get the original number of nodes of the surfaces used to make this cifti, for rows
        int64_t getRowSurfaceNumberOfNodes(const StructureEnum::Enum structure) const;
        
        ///get the original number of nodes of the surfaces used to make this cifti, for columns
        int64_t getColumnSurfaceNumberOfNodes(const StructureEnum::Enum structure) const;
            
        ///get the timestep for rows, returns false if not timeseries
        bool getRowTimestep(float& seconds) const;
        
        ///get the timestep for columns, returns false if not timeseries
        bool getColumnTimestep(float& seconds) const;

        ///get dimensions, spacing, origin for the volume attribute - returns false if not plumb - NOTE: only uses the volume space of the first dimension that has one
        bool getVolumeAttributesForPlumb(VolumeSpace::OrientTypes orientOut[3], int64_t dimensionsOut[3], float originOut[3], float spacingOut[3]) const;
        
        bool hasRowVolumeData() const;
        
        bool hasColumnVolumeData() const;
        
        bool hasRowSurfaceData(const StructureEnum::Enum structure) const;
        
        bool hasColumnSurfaceData(const StructureEnum::Enum structure) const;

        ///get the map name for an index along a column
        AString getMapNameForColumnIndex(const int& index) const;

        ///get the map name for an index along a row
        AString getMapNameForRowIndex(const int& index) const;

        virtual ~CiftiInterface();

    };
}

#endif //__CIFTI_INTERFACE_H__
