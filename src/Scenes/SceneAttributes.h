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

#include <set>

#include "CaretObject.h"
#include "SceneRestoreWarningCodesEnum.h"
#include "SceneTypeEnum.h"

namespace caret {
    
    class CaretMappableDataFile;
    class Scene;
    
    class SceneAttributes : public CaretObject {
        
    public:
        enum RestoreWindowBehavior {
            RESTORE_WINDOW_USE_ALL_POSITIONS_AND_SIZES,
            RESTORE_WINDOW_POSITION_RELATIVE_TO_FIRST_AND_USE_SIZES,
            RESTORE_WINDOW_IGNORE_ALL_POSITIONS_AND_SIZES
        };
        
        SceneAttributes(const SceneTypeEnum::Enum sceneType,
                        const Scene* scene);
        
        SceneAttributes(const SceneAttributes& rhs);

        virtual ~SceneAttributes();
        
        SceneTypeEnum::Enum getSceneType() const;
        
        const Scene* getScene() const;
        
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
        
        bool isLogFilesWithPaletteSettingsErrors() const;
        
        void setLogFilesWithPaletteSettingsErrors(const bool status);
        
        void addToMapFilesWithPaletteSettingsErrors(CaretMappableDataFile* mapFile,
                                                    const AString& dataFileName) const;
        
        std::vector<std::pair<CaretMappableDataFile*, AString>> getMapFilesWithPaletteSettingsErrors() const;
        
        void addToErrorMessage(const AString& message) const;
        
        AString getErrorMessage() const;
        
        void clearErrorMessage();
        
        bool isUseSceneForegroundAndBackgroundColors() const;
        
        void setUseSceneForegroundAndBackgroundColors(const bool status);
        
        void setSceneRestoreWarningCode(const SceneRestoreWarningCodesEnum::Enum warningCode) const;
        
        AString getSceneLoadWarningMessage() const;
        
        void setKeepAllFilesInScene(const bool status) const;
        
        bool isKeepAllFilesInScene() const;
        
    private:
        SceneAttributes& operator=(const SceneAttributes&);
        
    public:

        // ADD_NEW_METHODS_HERE

    private:

        const SceneTypeEnum::Enum m_sceneType;
        
        const Scene* m_scene;
        
        std::vector<int32_t> m_indicesOfTabsForSavingToScene;
        
        std::vector<int32_t> m_indicesOfWindowsForSavingToScene;
        
        AString m_sceneFileName;
        
        AString m_sceneName;
        
        RestoreWindowBehavior m_restoreWindowBehaviorInSceneDisplay;
        
        bool m_specFileNameSavedToScene;
        
        bool m_allLoadedFilesSavedToScene;
        
        bool m_modifiedPaletteSettingsSavedToScene;
        
        bool m_useSceneForgroundAndBackgroundColorsFlag;
        
        bool m_logFilesWithPaletteSettingsErrorsFlag;
        
        mutable std::vector<std::pair<CaretMappableDataFile*, AString>> m_mapFilesWithPaletteSettingsErrors;
        
        mutable AString m_errorMessage;
        
        mutable std::set<SceneRestoreWarningCodesEnum::Enum> m_sceneWarningCodes;
        
        mutable bool m_keepAllFilesInSceneFlag;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __SCENE_ATTRIBUTES_DECLARE__
#endif // __SCENE_ATTRIBUTES_DECLARE__

} // namespace
#endif  //__SCENE_ATTRIBUTES__H_
