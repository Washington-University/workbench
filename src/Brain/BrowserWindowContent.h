#ifndef __BROWSER_WINDOW_CONTENT_H__
#define __BROWSER_WINDOW_CONTENT_H__

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

#include "SceneableInterface.h"
#include "TileTabsConfigurationModeEnum.h"

namespace caret {
    class SceneClassAssistant;
    class TileTabsConfiguration;
    
    class BrowserWindowContent : public CaretObject, public SceneableInterface {
        
    public:
        BrowserWindowContent(const int32_t windowIndex);
        
        virtual ~BrowserWindowContent();
        
        int32_t getWindowIndex() const;
        
        bool isValid() const;
        
        void setValid(const bool valid);
        
        void reset();
        
        bool isWindowAspectLocked() const;
        
        void setWindowAspectLocked(const bool lockedStatus);
        
        float getWindowAspectLockedRatio() const;
        
        void setWindowAspectLockedRatio(const float aspectRatio);
        
        bool isAllTabsInWindowAspectRatioLocked() const;
        
        void setAllTabsInWindowAspectRatioLocked(const bool lockedStatus);

        bool isTileTabsEnabled() const;
        
        void setTileTabsEnabled(const bool tileTabsEnabled);

        TileTabsConfiguration* getSelectedTileTabsConfiguration();
        
        const TileTabsConfiguration* getSelectedTileTabsConfiguration() const;
        
        TileTabsConfiguration* getAutomaticTileTabsConfiguration();
        
        const TileTabsConfiguration* getAutomaticTileTabsConfiguration() const;
        
        TileTabsConfiguration* getCustomTileTabsConfiguration();
        
        const TileTabsConfiguration* getCustomTileTabsConfiguration() const;
        
        TileTabsConfigurationModeEnum::Enum getTileTabsConfigurationMode() const;
        
        void setTileTabsConfigurationMode(const TileTabsConfigurationModeEnum::Enum configMode);
        
        int32_t getSceneGraphicsWidth() const;
        
        void setSceneGraphicsWidth(const int32_t width);
        
        int32_t getSceneGraphicsHeight() const;
        
        void setSceneGraphicsHeight(const int32_t width);
        
        int32_t getSceneSelectedTabIndex() const;
        
        void setSceneSelectedTabIndex(const int32_t selectedTabIndex);
        
        std::vector<int32_t> getSceneTabIndices() const;
        
        void setSceneWindowTabIndices(const std::vector<int32_t>& sceneTabIndices);
        
        // ADD_NEW_METHODS_HERE

        virtual SceneClass* saveToScene(const SceneAttributes* sceneAttributes,
                                        const AString& instanceName);

        virtual void restoreFromScene(const SceneAttributes* sceneAttributes,
                                      const SceneClass* sceneClass);

        void restoreFromOldBrainBrowserWindowScene(const SceneAttributes* sceneAttributes,
                                                   const SceneClass* browserClass);
        
        
          
          
          
          
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
        BrowserWindowContent(const BrowserWindowContent&);

        BrowserWindowContent& operator=(const BrowserWindowContent&);
        
        std::unique_ptr<SceneClassAssistant> m_sceneAssistant;

        bool m_validFlag;
        
        const int32_t m_windowIndex;
        
        bool m_windowAspectRatioLocked = false;
        
        float m_windowAspectLockedRatio = 1.0f;
        
        bool m_allTabsInWindowAspectRatioLocked = false;
        
        bool m_tileTabsEnabled = false;
        
        TileTabsConfigurationModeEnum::Enum m_tileTabsConfigurationMode = TileTabsConfigurationModeEnum::AUTOMATIC;
        
        int32_t m_sceneGraphicsWidth = 0;
        
        int32_t m_sceneGraphicsHeight = 0;
        
        std::unique_ptr<TileTabsConfiguration> m_automaticTileTabsConfiguration;
        
        std::unique_ptr<TileTabsConfiguration> m_customTileTabsConfiguration;
        
        int32_t m_sceneSelectedTabIndex = 0;
        
        std::vector<int32_t> m_sceneTabIndices;
        
        friend class BrainBrowserWindow;

        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __BROWSER_WINDOW_CONTENT_DECLARE__
#endif // __BROWSER_WINDOW_CONTENT_DECLARE__

} // namespace
#endif  //__BROWSER_WINDOW_CONTENT_H__
