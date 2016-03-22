#ifndef __DISPLAY_GROUP_AND_TAB_ITEM_HELPER_H__
#define __DISPLAY_GROUP_AND_TAB_ITEM_HELPER_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2016 Washington University School of Medicine
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

#include "BrainConstants.h"
#include "CaretObject.h"
#include "DisplayGroupEnum.h"
#include "SceneableInterface.h"
#include "TriStateSelectionStatusEnum.h"

namespace caret {
    class DisplayGroupAndTabItemInterface;
    class SceneClassAssistant;

    class DisplayGroupAndTabItemHelper : public CaretObject, public SceneableInterface {
        
    public:
        DisplayGroupAndTabItemHelper();
        
        virtual ~DisplayGroupAndTabItemHelper();
        
        DisplayGroupAndTabItemHelper(const DisplayGroupAndTabItemHelper& obj);

        DisplayGroupAndTabItemHelper& operator=(const DisplayGroupAndTabItemHelper& obj);
        
        DisplayGroupAndTabItemInterface* getParent() const;
        
        void setParent(DisplayGroupAndTabItemInterface* itemParent);

        bool isExpanded(const DisplayGroupEnum::Enum displayGroup,
                        const int32_t tabIndex) const;
        
        void setExpanded(const DisplayGroupEnum::Enum displayGroup,
                         const int32_t tabIndex,
                         const bool status);
        
        void setExpandable(const bool expandableStatus);
        
        TriStateSelectionStatusEnum::Enum getSelected(const DisplayGroupEnum::Enum displayGroup,
                        const int32_t tabIndex) const;
        
        void setSelected(const DisplayGroupEnum::Enum displayGroup,
                         const int32_t tabIndex,
                         const TriStateSelectionStatusEnum::Enum status);
        
        TriStateSelectionStatusEnum::Enum getSelectedInWindow(const int32_t windowIndex) const;
        
        void setSelectedInWindow(const int32_t windowIndex,
                                 const TriStateSelectionStatusEnum::Enum status);
        
        bool isExpandedInWindow(const int32_t windowIndex) const;
        
        void setExpandedInWindow(const int32_t windowIndex,
                                 const bool status);
        
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
        void copyHelperDisplayGroupAndTabItemHelper(const DisplayGroupAndTabItemHelper& obj);

        void clearPrivate();
        
        void initializeNewInstance();
        
        DisplayGroupAndTabItemInterface* m_parent;
        
        SceneClassAssistant* m_sceneAssistant;

        /** Selection for each display group */
        TriStateSelectionStatusEnum::Enum m_selectedInDisplayGroup[DisplayGroupEnum::NUMBER_OF_GROUPS];
        
        /** Selection for each tab */
        TriStateSelectionStatusEnum::Enum m_selectedInTab[BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS];
        
        /** Expanded (collapsed) status in display group */
        bool m_expandedStatusInDisplayGroup[DisplayGroupEnum::NUMBER_OF_GROUPS];
        
        /** Expanded (collapsed) status in tab */
        bool m_expandedStatusInTab[BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS];
        
        /** Selection for each window */
        TriStateSelectionStatusEnum::Enum m_selectedInWindow[BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_WINDOWS];
        
        /** Expanded (collapsed) status in window */
        bool m_expandedInWindow[BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_WINDOWS];
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __DISPLAY_GROUP_AND_TAB_ITEM_HELPER_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __DISPLAY_GROUP_AND_TAB_ITEM_HELPER_DECLARE__

} // namespace
#endif  //__DISPLAY_GROUP_AND_TAB_ITEM_HELPER_H__
