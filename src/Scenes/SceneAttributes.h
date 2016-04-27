#ifndef __SCENE_ATTRIBUTES__H_
#define __SCENE_ATTRIBUTES__H_

/*LICENSE_START*/
/*
 *  Copyright (C) 2014  Washington University School of Medicine
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
#include "SceneTypeEnum.h"

namespace caret {
    
    class SceneAttributes : public CaretObject {
        
    public:
        enum RestoreWindowBehavior {
            RESTORE_WINDOW_USE_ALL_POSITIONS_AND_SIZES,
            RESTORE_WINDOW_POSITION_RELATIVE_TO_FIRST_AND_USE_SIZES,
            RESTORE_WINDOW_IGNORE_ALL_POSITIONS_AND_SIZES
        };
        
        SceneAttributes(const SceneTypeEnum::Enum sceneType);
        
        virtual ~SceneAttributes();
        
        SceneTypeEnum::Enum getSceneType() const;
        
        void setIndicesOfTabsAndWindowsForSavingToScene(const std::vector<int32_t>& tabIndices,
                                                        const std::vector<int32_t>& windowIndices);
        
        std::vector<int32_t> getIndicesOfTabsForSavingToScene() const;
        
        std::vector<int32_t> getIndicesOfWindowsForSavingToScene() const;
        
        bool isTabIndexSavedToScene(const int32_t tabIndex) const;
        
        bool isWindowIndexSavedToScene(const int32_t tabIndex) const;
        
        AString getSceneFileName() const;
        
        void setSceneFileName(const AString& sceneFileName);
        
        AString getSceneName() const;
        
        void setSceneName(const AString& name);
        
        RestoreWindowBehavior getRestoreWindowBehaviorInSceneDisplay() const;
        
        void setWindowRestoreBehaviorInSceneDisplay(const RestoreWindowBehavior rwb);
        
        bool isSpecFileNameSavedToScene() const;
        
        void setSpecFileNameSavedToScene(const bool status);
        
        bool isAllLoadedFilesSavedToScene() const;
        
        void setAllLoadedFilesSavedToScene(const bool status);
        
        bool isModifiedPaletteSettingsSavedToScene() const;
        
        void setModifiedPaletteSettingsSavedToScene(const bool status);
        
        void addToErrorMessage(const AString& message) const;
        
        AString getErrorMessage() const;
        
        void clearErrorMessage();
        
        bool isUseSceneForegroundAndBackgroundColors() const;
        
        void setUseSceneForegroundAndBackgroundColors(const bool status);
        
    private:
        SceneAttributes(const SceneAttributes&);

        SceneAttributes& operator=(const SceneAttributes&);
        
    public:

        // ADD_NEW_METHODS_HERE

    private:

        const SceneTypeEnum::Enum m_sceneType;
        
        std::vector<int32_t> m_indicesOfTabsForSavingToScene;
        
        std::vector<int32_t> m_indicesOfWindowsForSavingToScene;
        
        AString m_sceneFileName;
        
        AString m_sceneName;
        
        RestoreWindowBehavior m_restoreWindowBehaviorInSceneDisplay;
        
        bool m_specFileNameSavedToScene;
        
        bool m_allLoadedFilesSavedToScene;
        
        bool m_modifiedPaletteSettingsSavedToScene;
        
        bool m_useSceneForgroundAndBackgroundColorsFlag;
        
        mutable AString m_errorMessage;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __SCENE_ATTRIBUTES_DECLARE__
#endif // __SCENE_ATTRIBUTES_DECLARE__

} // namespace
#endif  //__SCENE_ATTRIBUTES__H_
