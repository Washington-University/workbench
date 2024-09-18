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
#include <vector>

#include <QStandardItem>

#include "Cluster.h"
#include "SceneableInterface.h"


namespace caret {
    class SceneClassAssistant;

    class LabelSelectionItem : public QStandardItem, public SceneableInterface {
        
    public:
        /**
         * Contains info on children center of gravity
         */
        class ChildCogInfo {
        public:
            ChildCogInfo(const Vector3D& centerOfGravity,
                         const float numberOfBrainordinates,
                         const bool validFlag)
            : m_centerOfGravity(centerOfGravity),
            m_numberOfBrainordinates(numberOfBrainordinates),
            m_validFlag(validFlag) { }
            
            Vector3D getCenterOfGravity() const { return m_centerOfGravity; }
            float getNumberOfBrainordinates() const { return m_numberOfBrainordinates; }
            bool isValid() const { return m_validFlag; }
        private:
            Vector3D m_centerOfGravity;
            float m_numberOfBrainordinates;
            bool m_validFlag;
        };
        
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
        
        std::pair<bool, Vector3D> getCenterOfGravityFromChildren() const;

        std::vector<LabelSelectionItem*> getThisAndAllDescendantsOfType(const LabelSelectionItem::ItemType itemType);
        
        std::vector<LabelSelectionItem*> getThisAndAllDescendants();
        
        AString toFormattedString(const AString& indentation) const;
        
        const std::vector<const Cluster>& getClusters() const;
        
        void setClusters(const std::vector<const Cluster*>& clusters);
        
        // ADD_NEW_METHODS_HERE

        virtual SceneClass* saveToScene(const SceneAttributes* sceneAttributes,
                                        const AString& instanceName);

        virtual void restoreFromScene(const SceneAttributes* sceneAttributes,
                                      const SceneClass* sceneClass);

    protected:
        LabelSelectionItem(const LabelSelectionItem& other);
        
        LabelSelectionItem& operator=(const LabelSelectionItem& other); 
        
    private:
        void copyHelper(const LabelSelectionItem& other);
        
        void initializeInstance();
        
        void setCheckedStatusOfAllChildren(QStandardItem* item,
                                           const Qt::CheckState checkState);
        
        ChildCogInfo setCenterOfGravityFromChildren();

        const ItemType m_itemType;
        
        int32_t m_labelIndex;
        
        std::unique_ptr<SceneClassAssistant> m_sceneAssistant;

        std::vector<const Cluster> m_clusters;
        
        bool m_centerOfGravityValidFlag = false;
        
        Vector3D m_centerOfGravity;
        
        friend class LabelSelectionItemModel;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __LABEL_SELECTION_ITEM_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __LABEL_SELECTION_ITEM_DECLARE__

} // namespace
#endif  //__LABEL_SELECTION_ITEM_H__
