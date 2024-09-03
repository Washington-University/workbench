#ifndef __LABEL_SELECTION_ITEM_H__
#define __LABEL_SELECTION_ITEM_H__

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


#include <array>
#include <memory>

#include <QStandardItem>

#include "SceneableInterface.h"


namespace caret {
    class SceneClassAssistant;

    class LabelSelectionItem : public QStandardItem, public SceneableInterface {
        
    public:
        /*
         * Type of item returned by override type() method
         */
        enum class ItemType : int {
            /** Item is a label that that is turned on/off by user */
            ITEM_LABEL = QStandardItem::UserType + 1,
            /** Item is in the hierarchy and IS NOT a label */
            ITEM_HIERARCHY = QStandardItem::UserType + 2
        };
        
        LabelSelectionItem(const AString& text,
                           const int32_t labelIndex,
                           const std::array<uint8_t, 4>& labelRGBA);
        
        LabelSelectionItem(const AString& text);
        
        virtual ~LabelSelectionItem();
        
        virtual QStandardItem* clone() const;
        
        virtual int type() const;

        int32_t getLabelIndex() const;
        
        void setAllChildrenChecked(const bool checked);

        Qt::CheckState setCheckStateFromChildren();
        
        std::vector<LabelSelectionItem*> getThisAndAllDescendantsOfType(const LabelSelectionItem::ItemType itemType);
        
        std::vector<LabelSelectionItem*> getThisAndAllDescendants();
        
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

    protected:
        LabelSelectionItem(const LabelSelectionItem& other);
        
        LabelSelectionItem& operator=(const LabelSelectionItem& other); 
        
    private:
        void copyHelper(const LabelSelectionItem& other);
        
        void initializeInstance();
        
        void setCheckedStatusOfAllChildren(QStandardItem* item,
                                           const Qt::CheckState checkState);
        
        const ItemType m_itemType;
        
        int32_t m_labelIndex;
        
        std::unique_ptr<SceneClassAssistant> m_sceneAssistant;

        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __LABEL_SELECTION_ITEM_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __LABEL_SELECTION_ITEM_DECLARE__

} // namespace
#endif  //__LABEL_SELECTION_ITEM_H__
