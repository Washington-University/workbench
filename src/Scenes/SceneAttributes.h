#ifndef __SCENE_ATTRIBUTES__H_
#define __SCENE_ATTRIBUTES__H_

/*LICENSE_START*/
/*
 * Copyright 2012 Washington University,
 * All rights reserved.
 *
 * Connectome DB and Connectome Workbench are part of the integrated Connectome 
 * Informatics Platform.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    * Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *    * Neither the names of Washington University nor the
 *      names of its contributors may be used to endorse or promote products
 *      derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR  
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
 * SUBSTITUTE GOODS OR  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
 * POSSIBILITY OF SUCH DAMAGE.
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
        
        void setIndicesOfTabsForSavingToScene(const std::vector<int32_t>& tabIndices);
        
        std::vector<int32_t> getIndicesOfTabsForSavingToScene() const;
        
        bool isTabIndexSavedToScene(const int32_t tabIndex) const;
        
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
        
    private:
        SceneAttributes(const SceneAttributes&);

        SceneAttributes& operator=(const SceneAttributes&);
        
    public:

        // ADD_NEW_METHODS_HERE

    private:

        const SceneTypeEnum::Enum m_sceneType;
        
        std::vector<int32_t> m_indicesOfTabsForSavingToScene;
        
        AString m_sceneFileName;
        
        AString m_sceneName;
        
        RestoreWindowBehavior m_restoreWindowBehaviorInSceneDisplay;
        
        bool m_specFileNameSavedToScene;
        
        bool m_allLoadedFilesSavedToScene;
        
        bool m_modifiedPaletteSettingsSavedToScene;
        
        mutable AString m_errorMessage;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __SCENE_ATTRIBUTES_DECLARE__
#endif // __SCENE_ATTRIBUTES_DECLARE__

} // namespace
#endif  //__SCENE_ATTRIBUTES__H_
