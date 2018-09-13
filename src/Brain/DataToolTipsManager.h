#ifndef __DATA_TOOL_TIPS_MANAGER_H__
#define __DATA_TOOL_TIPS_MANAGER_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2018 Washington University School of Medicine
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

#include "EventListenerInterface.h"
#include "SceneableInterface.h"


namespace caret {
    class Brain;
    class BrowserTabContent;
    class SceneClassAssistant;
    class SelectionItemSurfaceNode;
    class SelectionItemVoxel;
    class SelectionManager;

    class DataToolTipsManager : public CaretObject, public EventListenerInterface, public SceneableInterface {
        
    public:
        DataToolTipsManager();
        
        virtual ~DataToolTipsManager();
        
        DataToolTipsManager(const DataToolTipsManager&) = delete;

        DataToolTipsManager& operator=(const DataToolTipsManager&) = delete;

        AString getToolTip(const Brain* brain,
                           const BrowserTabContent* browserTab,
                           const SelectionManager* selectionManager) const;
        
        bool isEnabled() const;
        
        void setEnabled(const bool status);
        
        bool isShowSurfacePrimaryAnatomical() const;
        
        void setShowSurfacePrimaryAnatomical(const bool status);

        bool isShowSurfaceViewed() const;
        
        void setShowSurfaceViewed(const bool status);
        
        bool isShowTopLayer() const;
        
        void setShowTopLayer(const bool status);
        
        bool isShowBorder() const;
        
        void setShowBorder(const bool status);
        
        bool isShowFocus() const;
        
        void setShowFocus(const bool status);
        
        // ADD_NEW_METHODS_HERE

        virtual AString toString() const;
        
        virtual void receiveEvent(Event* event);

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
        AString getSurfaceToolTip(const Brain* brain,
                                  const BrowserTabContent* browserTab,
                                  const SelectionManager* selectionManager,
                                  const SelectionItemSurfaceNode* nodeSelection) const;
        
        AString getVolumeToolTip(const Brain* brain,
                                 const BrowserTabContent* browserTab,
                                 const SelectionManager* selectionManager,
                                 const SelectionItemVoxel* voxelSelection) const;
        
        std::unique_ptr<SceneClassAssistant> m_sceneAssistant;

        bool m_enabledFlag = true;
        
        bool m_showSurfacePrimaryAnatomicalFlag = true;
        
        bool m_showSurfaceViewedFlag = true;
        
        bool m_showTopLayerFlag = true;
        
        bool m_showBorderFlag = true;
        
        bool m_showFocusFlag = true;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __DATA_TOOL_TIPS_MANAGER_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __DATA_TOOL_TIPS_MANAGER_DECLARE__

} // namespace
#endif  //__DATA_TOOL_TIPS_MANAGER_H__
