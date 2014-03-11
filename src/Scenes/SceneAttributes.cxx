
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

#define __SCENE_ATTRIBUTES_DECLARE__
#include "SceneAttributes.h"
#undef __SCENE_ATTRIBUTES_DECLARE__

using namespace caret;


    
/**
 * \class caret::SceneAttributes 
 * \brief Attributes of a scene.
 * \ingroup Scene
 *
 * See the documentation in the class Scene for how to use the Scene system.
 */

/**
 * Constructor.
 */
SceneAttributes::SceneAttributes(const SceneTypeEnum::Enum sceneType)
: CaretObject(), m_sceneType(sceneType)
{
    m_restoreWindowBehaviorInSceneDisplay = RESTORE_WINDOW_POSITION_RELATIVE_TO_FIRST_AND_USE_SIZES;
    m_specFileNameSavedToScene   = true;
    m_allLoadedFilesSavedToScene = true;
}

/**
 * Destructor.
 */
SceneAttributes::~SceneAttributes()
{
    
}

/**
 * @return The type of scene.
 */
SceneTypeEnum::Enum 
SceneAttributes::getSceneType() const
{
    return m_sceneType;
}

/**
 * Set the indices of the tabs that are to be saved to the scene.
 * @param tabIndices
 *    Indices of tabs that are saved to the scene.
 */
void 
SceneAttributes::setIndicesOfTabsForSavingToScene(const std::vector<int32_t>& tabIndices)
{
    m_indicesOfTabsForSavingToScene = tabIndices;
}

/**
 * @return Indices of tabs for saving to the scene.
 */
std::vector<int32_t> 
SceneAttributes::getIndicesOfTabsForSavingToScene() const
{
    return m_indicesOfTabsForSavingToScene;
}

/**
 * Is the given tab index for saving to the scene?
 *
 * @param tabIndex
 *    The tab index.
 * @return
 *    True if saved to scene, else false.
 */
bool
SceneAttributes::isTabIndexSavedToScene(const int32_t tabIndex) const
{
    if (std::find(m_indicesOfTabsForSavingToScene.begin(),
                  m_indicesOfTabsForSavingToScene.end(),
                  tabIndex) != m_indicesOfTabsForSavingToScene.end()) {
        return true;
    }
    
    return false;
}

/**
 * @return Name of the scene file.
 * May only have a value when restoring.
 */
AString 
SceneAttributes::getSceneFileName() const
{
    return m_sceneFileName;
}

/**
 * Set the name of the scene file containing the scene
 * that is being restored.
 * @param sceneFileName
 *    Name of scene file.
 */
void 
SceneAttributes::setSceneFileName(const AString& sceneFileName)
{
    m_sceneFileName = sceneFileName;
}

/**
 * @return Name of the scene.
 * May only have a value when restoring.
 */
AString
SceneAttributes::getSceneName() const
{
    return m_sceneName;
}

/**
 * Set the name of the scene that is being restored.
 * @param sceneName
 *    Name of scene.
 */
void
SceneAttributes::setSceneName(const AString& sceneName)
{
    m_sceneName = sceneName;
}

/**
 * @return The window restoration behavior when displaying a scene.
 */
SceneAttributes::RestoreWindowBehavior 
SceneAttributes::getRestoreWindowBehaviorInSceneDisplay() const
{
    return m_restoreWindowBehaviorInSceneDisplay;
}

/**
 * Set the window restoration behavior when displaying a scene.
 * @param rwb
 *   New value for window restoration behavior.
 */
void 
SceneAttributes::setWindowRestoreBehaviorInSceneDisplay(const RestoreWindowBehavior rwb)
{
    m_restoreWindowBehaviorInSceneDisplay = rwb;
}

/**
 * @return true if spec file name is included in scene creation.
 */
bool 
SceneAttributes::isSpecFileNameSavedToScene() const
{
    return m_specFileNameSavedToScene;
}

/**
 * Set spec file name included in scene creation.
 * @param status
 *    New status of spec file name included in scene creation.
 */
void 
SceneAttributes::setSpecFileNameSavedToScene(const bool status)
{
    m_specFileNameSavedToScene = status;
}

/**
 * @return Are all loaded files saved to the scene when creating a scene, even those
 * that do not affect the scene?  Including all loaded files will increase
 * the time required to display the scene.
 */
bool
SceneAttributes::isAllLoadedFilesSavedToScene() const
{
    return m_allLoadedFilesSavedToScene;
}

/**
 * When creating a scene, should all loadedfiles be included in the scene, even
 * those that do not affect the scene?  Including all loaded  files will increase
 * the time required to display the scene.
 *
 * @param status
 *    New status.
 */
void
SceneAttributes::setAllLoadedFilesSavedToScene(const bool status)
{
    m_allLoadedFilesSavedToScene = status;
}

/**
 * @return Save modified palette settings to the scene.
 *
 * There are times the user wants to modify the palette settings
 * for a map but not save the file or the file may not be savable.
 * Thus, this option allows the user to save the palette settings
 * to the scene.
 */
bool
SceneAttributes::isModifiedPaletteSettingsSavedToScene() const
{
    return m_modifiedPaletteSettingsSavedToScene;
}

/**
 * Set modified palettes saved to scene.
 *
 * There are times the user wants to modify the palette settings
 * for a map but not save the file or the file may not be savable.
 * Thus, this option allows the user to save the palette settings
 * to the scene.
 *
 * @param status
 *     New status.
 */
void
SceneAttributes::setModifiedPaletteSettingsSavedToScene(const bool status)
{
    m_modifiedPaletteSettingsSavedToScene = status;
}


/**
 * Add a new message to the error message.  Each message is
 * separated by a newline.
 *
 * @param message
 *    New message added to the error message.
 */
void 
SceneAttributes::addToErrorMessage(const AString& message) const
{
    if (message.isEmpty()) {
        return;
    }
    
    if (m_errorMessage.isEmpty() == false) {
        m_errorMessage += "\n";
    }
    m_errorMessage += message;
}

/**
 * @return The error message.
 */
AString 
SceneAttributes::getErrorMessage() const
{
    return m_errorMessage;
}


