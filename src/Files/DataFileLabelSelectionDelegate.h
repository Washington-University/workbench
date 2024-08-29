#ifndef __DATA_FILE_LABEL_SELECTION_DELEGATE_H__
#define __DATA_FILE_LABEL_SELECTION_DELEGATE_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2024 Washington University School of Medicine
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
#include "DisplayGroupEnum.h"
#include "SceneableInterface.h"


namespace caret {
    class CaretMappableDataFile;
    class LabelSelectionItemModel;
    class SceneClassAssistant;

    class DataFileLabelSelectionDelegate : public CaretObject, public SceneableInterface {
        
    public:
        DataFileLabelSelectionDelegate(const CaretMappableDataFile* mapFile,
                                       const int32_t mapIndex);
        
        virtual ~DataFileLabelSelectionDelegate();
        
        DataFileLabelSelectionDelegate(const DataFileLabelSelectionDelegate&) = delete;

        DataFileLabelSelectionDelegate& operator=(const DataFileLabelSelectionDelegate&) = delete;
        
        LabelSelectionItemModel* getSelectionModelForMapAndTab(const DisplayGroupEnum::Enum displayGroup,
                                                               const int32_t tabIndex);

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
        LabelSelectionItemModel* createModel(const DisplayGroupEnum::Enum displayGroup,
                                             const int32_t tabIndex);
        
        const CaretMappableDataFile* m_mapFile;
        
        const int32_t m_mapIndex;
        
        std::unique_ptr<SceneClassAssistant> m_sceneAssistant;

        std::vector<std::unique_ptr<LabelSelectionItemModel>> m_tabSelectionModels;
        
        std::vector<std::unique_ptr<LabelSelectionItemModel>> m_displayGroupSelectionModels;
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __DATA_FILE_LABEL_SELECTION_DELEGATE_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __DATA_FILE_LABEL_SELECTION_DELEGATE_DECLARE__

} // namespace
#endif  //__DATA_FILE_LABEL_SELECTION_DELEGATE_H__
