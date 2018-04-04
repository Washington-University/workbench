#ifndef __CARET_DATA_FILE_SELECTION_MODEL_H__
#define __CARET_DATA_FILE_SELECTION_MODEL_H__

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
#include "DataFileTypeEnum.h"
#include "SceneableInterface.h"
#include "StructureEnum.h"

namespace caret {
    class Brain;
    class CaretDataFile;
    class SceneClassAssistant;

    class CaretDataFileSelectionModel : public CaretObject, public SceneableInterface {
        
    public:
        static CaretDataFileSelectionModel* newInstanceForCaretDataFileType(Brain* brain,
                                                                            const DataFileTypeEnum::Enum dataFileType);
        
        static CaretDataFileSelectionModel* newInstanceForCaretDataFileTypes(Brain* brain,
                                                                            const std::vector<DataFileTypeEnum::Enum>& dataFileTypes);
        
        static CaretDataFileSelectionModel* newInstanceForCaretDataFileTypesInStructure(Brain* brain,
                                                                             const StructureEnum::Enum structure,
                                                                             const std::vector<DataFileTypeEnum::Enum>& dataFileTypes);
        
        static CaretDataFileSelectionModel* newInstanceForChartableMatrixParcelInterface(Brain* brain);
        
        static CaretDataFileSelectionModel* newInstanceForMultiStructureBorderFiles(Brain* brain);
        
        virtual ~CaretDataFileSelectionModel();
        
        CaretDataFileSelectionModel(const CaretDataFileSelectionModel& obj);

        CaretDataFileSelectionModel& operator=(const CaretDataFileSelectionModel& obj);
        
        CaretDataFile* getSelectedFile();
        
        const CaretDataFile* getSelectedFile() const;
        
        StructureEnum::Enum getStructure() const;
        
        void setStructure(const StructureEnum::Enum structure);
        
        std::vector<CaretDataFile*> getAvailableFiles() const;
        
        void overrideAvailableDataFiles(std::vector<CaretDataFile*>& availableFiles);
        
        /**
         * @return Selected file dynamically cast to the templated
         * data file type.
         */
        template <typename T>
        inline T* getSelectedFileOfType() {
            CaretDataFile* cdf = getSelectedFile();
            if (cdf != NULL) {
                return dynamic_cast<T*>(getSelectedFile());
            }
            
            return NULL;
        }
        
        void setSelectedFile(CaretDataFile* selectedFile);
        

        // ADD_NEW_METHODS_HERE

        virtual AString toString() const;
        
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
        enum FileMode {
            FILE_MODE_CHARTABLE_MATRIX_PARCEL_INTERFACE,
            FILE_MODE_DATA_FILE_TYPE_ENUM,
            FILE_MODE_MULTI_STRUCTURE_BORDER_FILES
        };
        
        CaretDataFileSelectionModel(Brain* brain,
                                    const StructureEnum::Enum structure,
                                    const FileMode fileMode);
        
        void copyHelperCaretDataFileSelectionModel(const CaretDataFileSelectionModel& obj);

        void updateSelection() const;
        
        const FileMode m_fileMode;
        
        std::vector<CaretDataFile*> m_overrideOfAvailableFiles;
        
        bool m_overrideOfAvailableFilesValid;
        
        Brain* m_brain;
        
        StructureEnum::Enum m_structure;
        
        std::vector<DataFileTypeEnum::Enum> m_dataFileTypes;
        
        mutable CaretDataFile* m_selectedFile;
        
        SceneClassAssistant* m_sceneAssistant;

        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __CARET_DATA_FILE_SELECTION_MODEL_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __CARET_DATA_FILE_SELECTION_MODEL_DECLARE__

} // namespace
#endif  //__CARET_DATA_FILE_SELECTION_MODEL_H__
