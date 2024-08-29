#ifndef __LABEL_SELECTION_ITEM_MODEL_H__
#define __LABEL_SELECTION_ITEM_MODEL_H__

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

#include <cstdint>
#include <map>
#include <memory>

#include <QStandardItemModel>

#include "CaretHierarchy.h"
#include "DisplayGroupEnum.h"
#include "LabelSelectionItem.h"
#include "SceneableInterface.h"


namespace caret {
    class GiftiLabelTable;
    class SceneClassAssistant;

    class LabelSelectionItemModel : public QStandardItemModel, public SceneableInterface {
        
        Q_OBJECT

    public:
        LabelSelectionItemModel(const GiftiLabelTable* giftiLabelTable,
                                const DisplayGroupEnum::Enum displayGroup,
                                const int32_t tabIndex);
        
        virtual ~LabelSelectionItemModel();
        
        bool isValid() const;
        
        LabelSelectionItemModel(const LabelSelectionItemModel&) = delete;

        LabelSelectionItemModel& operator=(const LabelSelectionItemModel&) = delete;
        
        bool isLabelChecked(const int32_t labelKey) const;

        void setCheckedStatusOfAllItems(const bool checked);
        
        void updateCheckedStateOfAllItems();
        
        std::vector<LabelSelectionItem*> getAllDescendantsOfType(const LabelSelectionItem::ItemType itemType);
        
        // ADD_NEW_METHODS_HERE

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
        void buildModel(const GiftiLabelTable* giftiLabelTable);
        
        LabelSelectionItem* buildTree(const CaretHierarchy::Item* hierarchyItem,
                                      const GiftiLabelTable* giftiLabelTable);
        
        const DisplayGroupEnum::Enum m_displayGroup;
        
        const int32_t m_tabIndex;
        
        std::unique_ptr<SceneClassAssistant> m_sceneAssistant;

        std::map<int32_t, LabelSelectionItem*> m_labelIndexToLabelSelectionItem;
        
        bool m_validFlag = false;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __LABEL_SELECTION_ITEM_MODEL_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __LABEL_SELECTION_ITEM_MODEL_DECLARE__

} // namespace
#endif  //__LABEL_SELECTION_ITEM_MODEL_H__
