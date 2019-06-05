#ifndef __MAP_FILE_DATA_SELECTOR_H__
#define __MAP_FILE_DATA_SELECTOR_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2017 Washington University School of Medicine
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



#include <memory>

#include "CaretObject.h"

#include "SceneableInterface.h"
#include "StructureEnum.h"

namespace caret {
    class CaretMappableDataFile;
    class SceneClassAssistant;

    class MapFileDataSelector : public CaretObject, public SceneableInterface {
        
    public:
        enum class DataSelectionType {
            INVALID,
            COLUMN_DATA,
            ROW_DATA,
            SURFACE_VERTEX,
            SURFACE_VERTICES_AVERAGE,
            VOLUME_XYZ
        };
        
        MapFileDataSelector();
        
        virtual ~MapFileDataSelector();
        
        MapFileDataSelector(const MapFileDataSelector& obj);

        MapFileDataSelector& operator=(const MapFileDataSelector& obj);
        
        bool operator==(const MapFileDataSelector& obj) const;
        
        DataSelectionType getDataSelectionType() const;
        
        static AString getDataSelectionTypeName(const DataSelectionType dataSelectionType);
        
        std::vector<int32_t> getValidTabIndices() const;
        
        void getSurfaceVertex(StructureEnum::Enum& surfaceStructure,
                              int32_t& surfaceNumberOfVertices,
                              int32_t& surfaceVertexIndex) const;
        
        void setSurfaceVertex(const StructureEnum::Enum& surfaceStructure,
                              const int32_t surfaceNumberOfVertices,
                              const int32_t surfaceVertexIndex);
        
        void getSurfaceVertexAverage(StructureEnum::Enum& surfaceStructure,
                                     int32_t& surfaceNumberOfVertices,
                                     std::vector<int32_t>& surfaceVertexIndices) const;
        
        void setSurfaceVertexAverage(const StructureEnum::Enum& surfaceStructure,
                                     const int32_t surfaceNumberOfVertices,
                                     const std::vector<int32_t>& surfaceVertexIndices);
        
        void getVolumeVoxelXYZ(float xyz[3]) const;
        
        void setVolumeVoxelXYZ(const float xyz[3]);
        
        void getColumnIndex(CaretMappableDataFile* &columnMapFile,
                            AString& columnMapFileName,
                            int32_t &columnIndex) const;
        
        void setColumnIndex(CaretMappableDataFile* columnMapFile,
                            const AString& columnMapFileName,
                            const int32_t columnIndex);
        
        void getRowIndex(CaretMappableDataFile* &rowMapFile,
                         AString& rowMapFileName,
                         int32_t &rowIndex) const;
        
        void setRowIndex(CaretMappableDataFile* rowMapFile,
                         const AString& rowMapFileName,
                         const int32_t rowIndex);
        
        void reset();

        // ADD_NEW_METHODS_HERE

        virtual AString toString() const;
        
        virtual SceneClass* saveToScene(const SceneAttributes* sceneAttributes,
                                        const AString& instanceName);

        virtual void restoreFromScene(const SceneAttributes* sceneAttributes,
                                      const SceneClass* sceneClass);

          
          
          
          
          
// If there will be sub-classes of this class that need to save
// and restore data from scenes, these pure virtual methods can
// be uncommented to force their implementation by sub-classes.
//    protected: 
//        virtual void saveSubClassDataToScene(const SceneAttributes* sceneAttributes,
//                                             SceneClass* sceneClass) = 0;
//
//        virtual void restoreSubClassDataFromScene(const SceneAttributes* sceneAttributes,
//                                                  const SceneClass* sceneClass) = 0;

    private:
        void copyHelperMapFileDataSelector(const MapFileDataSelector& obj);

        std::unique_ptr<SceneClassAssistant> m_sceneAssistant;

        DataSelectionType m_dataSelectionType = DataSelectionType::INVALID;
        
        int32_t m_surfaceNumberOfVertices = 0;
        
        StructureEnum::Enum m_surfaceStructure = StructureEnum::INVALID;
        
        std::vector<int32_t> m_surfaceVertexAverageIndices;
        
        int32_t m_surfaceVertexIndex = -1;
        
        float m_voxelXYZ[3];
        
        int32_t m_columnIndex = -1;
        
        int32_t m_rowIndex = -1;
        
        CaretMappableDataFile* m_rowMapFile;
        
        CaretMappableDataFile* m_columnMapFile;
        
        AString m_rowMapFileName;
        
        AString m_columnMapFileName;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __MAP_FILE_DATA_SELECTOR_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __MAP_FILE_DATA_SELECTOR_DECLARE__

} // namespace
#endif  //__MAP_FILE_DATA_SELECTOR_H__
