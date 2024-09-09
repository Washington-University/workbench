#ifndef __CARET_MAPPABLE_DATA_FILE_AND_MAP_SELECTION_MODEL_H__
#define __CARET_MAPPABLE_DATA_FILE_AND_MAP_SELECTION_MODEL_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2014 Washington University School of Medicine
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


#include "CaretObject.h"
#include "CaretVolumeExtension.h"
#include "DataFileTypeEnum.h"
#include "SceneableInterface.h"


namespace caret {
    class CaretDataFileSelectionModel;
    class CaretMappableDataFile;
    class SceneClassAssistant;

    class CaretMappableDataFileAndMapSelectionModel : public CaretObject, public SceneableInterface {
        
    public:
        CaretMappableDataFileAndMapSelectionModel(const CaretMappableDataFile* caretMappableDataFile,
                                                  const bool excludeSelfFlag = false);
        
        CaretMappableDataFileAndMapSelectionModel(const DataFileTypeEnum::Enum dataFileType);
        
        CaretMappableDataFileAndMapSelectionModel(const std::vector<DataFileTypeEnum::Enum>& dataFileTypes);
        
        CaretMappableDataFileAndMapSelectionModel(const std::vector<DataFileTypeEnum::Enum>& dataFileTypes,
                                                  const std::vector<SubvolumeAttributes::VolumeType>& volumeTypes);
               
        virtual ~CaretMappableDataFileAndMapSelectionModel();
        
        CaretMappableDataFileAndMapSelectionModel(const CaretMappableDataFileAndMapSelectionModel& obj);

        CaretMappableDataFileAndMapSelectionModel& operator=(const CaretMappableDataFileAndMapSelectionModel& obj);

        CaretDataFileSelectionModel* getCaretDataFileSelectionModel();
        
        CaretMappableDataFile* getSelectedFile();
        
        const CaretMappableDataFile* getSelectedFile() const;
        
        int32_t getSelectedMapIndex() const;
        
        std::vector<CaretMappableDataFile*> getAvailableFiles() const;
        
        void overrideAvailableDataFiles(std::vector<CaretMappableDataFile*>& availableFiles);
        
        /**
         * @return Selected file dynamically cast to the templated
         * data file type.
         */
        template <typename T>
        inline T* getSelectedFileOfType() {
            CaretMappableDataFile* cmdf = getSelectedFile();
            if (cmdf != NULL) {
                return dynamic_cast<T*>(getSelectedFile());
            }
            
            return NULL;
        }
        
        void setSelectedFile(CaretMappableDataFile* selectedFile);
        
        void setSelectedFileName(const AString& filename);
        
        void setSelectedMapIndex(const int32_t mapIndex);

        // ADD_NEW_METHODS_HERE

        virtual SceneClass* saveToScene(const SceneAttributes* sceneAttributes,
                                        const AString& instanceName);

        virtual void restoreFromScene(const SceneAttributes* sceneAttributes,
                                      const SceneClass* sceneClass);

          
          
          
          
          
// If there will be sub-classes of this class that need to save
// and restore data from scenes, these pure virtual methods can
// be uncommented to force their implemetation by sub-classes.
//    protected: 
//        virtual void saveSubClassDataToScene(const SceneAttributes* sceneAttributes,
//                                             SceneClass* sceneClass) = 0;
//
//        virtual void restoreSubClassDataFromScene(const SceneAttributes* sceneAttributes,
//                                                  const SceneClass* sceneClass) = 0;

    private:
        enum class Mode {
            MAP_TO_SAME_BRAINORDINATES,
            MAP_TO_SAME_BRAINORDINATES_EXCLUDE_SELF,
            MATCH_DATA_FILE_TYPES
        };
        
        void copyHelperCaretMappableDataFileAndMapSelectionModel(const CaretMappableDataFileAndMapSelectionModel& obj);

        void performConstruction(const std::vector<DataFileTypeEnum::Enum>& dataFileTypes,
                                 const std::vector<SubvolumeAttributes::VolumeType>& volumeTypes);

        void validateDataFileTypes();
        
        const Mode m_mode;
        
        const CaretMappableDataFile* m_mappableDataFile;
        
        SceneClassAssistant* m_sceneAssistant;

        std::vector<DataFileTypeEnum::Enum> m_dataFileTypes;
        
        std::vector<SubvolumeAttributes::VolumeType> m_volumeTypes;
        
        CaretDataFileSelectionModel* m_caretDataFileSelectionModel;
        
        mutable int32_t m_selectedMapIndex;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __CARET_MAPPABLE_DATA_FILE_AND_MAP_SELECTION_MODEL_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __CARET_MAPPABLE_DATA_FILE_AND_MAP_SELECTION_MODEL_DECLARE__

} // namespace
#endif  //__CARET_MAPPABLE_DATA_FILE_AND_MAP_SELECTION_MODEL_H__
