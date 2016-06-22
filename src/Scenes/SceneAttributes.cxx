
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
    m_useSceneForgroundAndBackgroundColorsFlag = true;
    m_modifiedPaletteSettingsSavedToScene = true;//TSC: was uninitialized, bad idea
}

SceneAttributes::SceneAttributes(const SceneAttributes& rhs): CaretObject(), m_sceneType(rhs.m_sceneType)
{
    m_restoreWindowBehaviorInSceneDisplay = rhs.m_restoreWindowBehaviorInSceneDisplay;
    m_specFileNameSavedToScene = rhs.m_specFileNameSavedToScene;
    m_allLoadedFilesSavedToScene = rhs.m_allLoadedFilesSavedToScene;
    m_useSceneForgroundAndBackgroundColorsFlag = rhs.m_useSceneForgroundAndBackgroundColorsFlag;
    m_sceneName = rhs.m_sceneName;
    //leaving sceneFileName empty, as that shouldn't even be stored inside the scene, and we don't know where this scene will be put
    m_indicesOfTabsForSavingToScene = rhs.m_indicesOfTabsForSavingToScene;
    m_indicesOfWindowsForSavingToScene = rhs.m_indicesOfWindowsForSavingToScene;
    //leaving error message empty, seems to make the most sense
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
SceneAttributes::setIndicesOfTabsAndWindowsForSavingToScene(const std::vector<int32_t>& tabIndices,
                                                  const std::vector<int32_t>& windowIndices)
{
    m_indicesOfTabsForSavingToScene    = tabIndices;
    m_indicesOfWindowsForSavingToScene = windowIndices;
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
 * @return Indices of tabs for saving to the scene.
 */
std::vector<int32_t>
SceneAttributes::getIndicesOfWindowsForSavingToScene() const
{
    return m_indicesOfWindowsForSavingToScene;
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
 * Is the given window index for saving to the scene?
 *
 * @param windowIndex
 *    The window index.
 * @return
 *    True if saved to scene, else false.
 */
bool
SceneAttributes::isWindowIndexSavedToScene(const int32_t windowIndex) const
{
    if (std::find(m_indicesOfWindowsForSavingToScene.begin(),
                  m_indicesOfWindowsForSavingToScene.end(),
                  windowIndex) != m_indicesOfWindowsForSavingToScene.end()) {
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

/**
 * Clear the error message.
 */
void
SceneAttributes::clearErrorMessage()
{
    m_errorMessage.clear();
}

/**
 * @return Use the foreground and background colors from the scene?
 *
 * Note that scenes prior May 2016 may not contain foreground and
 * background colors.
 */
bool
SceneAttributes::isUseSceneForegroundAndBackgroundColors() const
{
    return m_useSceneForgroundAndBackgroundColorsFlag;
}

/**
 * Set use the foreground and background colors from the scene.
 *
 * Note that scenes prior May 2016 may not contain foreground and
 * background colors.
 *
 * @param status
 *     New status for using colors from scene.
 */
void
SceneAttributes::setUseSceneForegroundAndBackgroundColors(const bool status)
{
    m_useSceneForgroundAndBackgroundColorsFlag = status;
}


