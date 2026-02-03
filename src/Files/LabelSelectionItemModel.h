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

#include <array>
#include <cstdint>
#include <map>
#include <memory>
#include <set>

#include <QStandardItemModel>

#include "CaretHierarchy.h"
#include "DisplayGroupEnum.h"
#include "LabelSelectionItem.h"
#include "SceneableInterface.h"


namespace caret {
    class ClusterContainer;
    class GiftiLabel;
    class GiftiLabelTable;
    class SceneClassAssistant;

    class LabelSelectionItemModel : public QStandardItemModel, public SceneableInterface {
        
        Q_OBJECT

    public:
        LabelSelectionItemModel(const AString& fileAndMapName,
                                GiftiLabelTable* giftiLabelTable,
                                const ClusterContainer* clusterContainer,
                                const DisplayGroupEnum::Enum displayGroup,
                                const int32_t tabIndex,
                                const bool logMismatchedLabelsFlag);
        
        virtual ~LabelSelectionItemModel();
        
        bool isValid() const;
        
        LabelSelectionItemModel(const LabelSelectionItemModel&) = delete;

        LabelSelectionItemModel& operator=(const LabelSelectionItemModel&) = delete;
        
        bool isLabelChecked(const int32_t labelKey) const;

        void setCheckedStatusOfAllItems(const bool checked);
        
        void setCheckedStatusOfAllItemsWithNames(const std::vector<AString>& names,
                                                 const bool checked);
        
        void updateCheckedStateOfAllItems();
        
        std::vector<LabelSelectionItem*> getAllDescendants() const;
        
        std::vector<LabelSelectionItem*> getAllDescendantsOfType(const LabelSelectionItem::ItemType itemType);
        
        std::vector<LabelSelectionItem*> getTopLevelItems();
        
        std::vector<LabelSelectionItem*> getItemsWithName(const AString& name) const;
        
        std::vector<LabelSelectionItem*> getItemsWithName(const AString& name);
        
        AString toFormattedString(const AString& indentation) const;
        
        void synchronizeSelectionsWithLabelTable(const bool copyToLabelTableFlag);
        
        std::vector<AString> getAllAlternativeNames() const;
        
        AString getSelectedAlternativeName() const;
        
        void setSelectedAlternativeName(const AString& name);
        
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
        void buildModel(const ClusterContainer* clusterContainer);
        
        LabelSelectionItem* buildTree(const CaretHierarchy::Item* hierarchyItem,
                                      const GiftiLabelTable* giftiLabelTable,
                                      const ClusterContainer* clusterContainer);
        
        const AString& m_fileAndMapName;
        
        GiftiLabelTable* m_giftiLabelTable;
        
        const DisplayGroupEnum::Enum m_displayGroup;
        
        const int32_t m_tabIndex;
        
        const bool m_logMismatchedLabelsFlag;
        
        std::unique_ptr<SceneClassAssistant> m_sceneAssistant;

        std::map<int32_t, LabelSelectionItem*> m_labelKeyToLabelSelectionItem;
        
        std::set<AString> m_buildTreeMissingLabelNames;
        
        std::set<AString> m_hierarchyParentNames;
        
        mutable std::vector<AString> m_allAlternativeNames;
        
        mutable bool m_allAlternativeNamesValidFlag = false;
        
        AString m_selectedAlternativeName;
        
        static const AString s_defaultAlternativeName;
        
        bool m_validFlag = false;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __LABEL_SELECTION_ITEM_MODEL_DECLARE__
    const AString LabelSelectionItemModel::s_defaultAlternativeName = "Name";
#endif // __LABEL_SELECTION_ITEM_MODEL_DECLARE__

} // namespace
#endif  //__LABEL_SELECTION_ITEM_MODEL_H__
