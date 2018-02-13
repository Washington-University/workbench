
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

#define __BROWSER_WINDOW_CONTENT_DECLARE__
#include "BrowserWindowContent.h"
#undef __BROWSER_WINDOW_CONTENT_DECLARE__

#include "CaretAssert.h"
#include "SceneClass.h"
#include "SceneClassAssistant.h"
#include "SceneIntegerArray.h"
#include "TileTabsConfiguration.h"

using namespace caret;


    
/**
 * \class caret::BrowserWindowContent 
 * \brief Information about content of a browser window.
 * \ingroup Brain
 */

/**
 * Constructor.
 */
BrowserWindowContent::BrowserWindowContent(const int32_t windowIndex)
: CaretObject(),
m_windowIndex(windowIndex)
{
    m_validFlag = false;
    reset();
    
    m_sceneAssistant = std::unique_ptr<SceneClassAssistant>(new SceneClassAssistant());
    m_sceneAssistant->add("m_validFlag", &m_validFlag);
    m_sceneAssistant->add("m_windowAspectRatioLocked", &m_windowAspectRatioLocked);
    m_sceneAssistant->add("m_windowAspectLockedRatio", &m_windowAspectLockedRatio);
    m_sceneAssistant->add("m_allTabsInWindowAspectRatioLocked", &m_allTabsInWindowAspectRatioLocked);
    m_sceneAssistant->add("m_tileTabsEnabled", &m_tileTabsEnabled);
    m_sceneAssistant->add("m_sceneGraphicsWidth", &m_sceneGraphicsWidth);
    m_sceneAssistant->add("m_sceneGraphicsHeight", &m_sceneGraphicsHeight);
    m_sceneAssistant->add("m_sceneSelectedTabIndex", &m_sceneSelectedTabIndex);
}

/**
 * Destructor.
 */
BrowserWindowContent::~BrowserWindowContent()
{
}

/**
 * @return True if this browser window content is valid.
 */
bool
BrowserWindowContent::isValid() const
{
    return m_validFlag;
}

/**
 * Set validity for this browser window content.
 *
 * @param valid
 *     New validity status.
 */
void
BrowserWindowContent::setValid(const bool valid)
{
    m_validFlag = valid;
}


/**
 * Reset the number of elements in this instance.
 */
void
BrowserWindowContent::reset()
{
    /* Note: do not change m_validFlag */
    
    m_windowAspectRatioLocked = false;
    m_windowAspectLockedRatio = 1.0f;
    m_allTabsInWindowAspectRatioLocked = false;
    m_tileTabsEnabled = false;
    m_sceneGraphicsHeight = 0;
    m_sceneGraphicsWidth  = 0;
    m_sceneTileTabsConfiguration.reset(new TileTabsConfiguration());
    m_sceneTileTabsConfiguration->setName(s_sceneTileTabsConfigurationText);
    m_sceneSelectedTabIndex = 0;
    m_sceneTabIndices.clear();
}


/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
int32_t
BrowserWindowContent::getWindowIndex() const
{
    return m_windowIndex;
}

/**
 * @return Window aspect locked status
 */
bool
BrowserWindowContent::isWindowAspectLocked() const
{
    return m_windowAspectRatioLocked;
}

/**
 * Set the window's aspect ratio locked status
 *
 * @param lockedStatus
 *     The new locked status.
 */
void
BrowserWindowContent::setWindowAspectLocked(const bool lockedStatus)
{
    m_windowAspectRatioLocked = lockedStatus;
}

/**
 * @return The window's locked aspect ratio
 */
float
BrowserWindowContent::getWindowAspectLockedRatio() const
{
    return m_windowAspectLockedRatio;
}

/**
 * Set the window's locked aspect ratio
 *
 * @param aspectRatio
 *    The aspect ratio.
 */
void
BrowserWindowContent::setWindowAspectLockedRatio(const float aspectRatio)
{
    m_windowAspectLockedRatio = aspectRatio;
}

/**
 * @return Is the aspect ratio locked for ALL TABS in the window
 */
bool
BrowserWindowContent::isAllTabsInWindowAspectRatioLocked() const
{
    return m_allTabsInWindowAspectRatioLocked;
}

/**
 * Set the aspect ratio locked for ALL TABS in the window
 *
 * @param lockedStatus
 *     The new locked status.
 */
void
BrowserWindowContent::setAllTabsInWindowAspectRatioLocked(const bool lockedStatus)
{
    m_allTabsInWindowAspectRatioLocked = lockedStatus;
}

/**
 * @return Is tile tabs enabled ?
 */
bool
BrowserWindowContent::isTileTabsEnabled() const
{
    return m_tileTabsEnabled;
}

/**
 * Set tile tabs enabled
 *
 * @param tileTabsEnabled
 *     Enabled status for tile tabs.
 */
void
BrowserWindowContent::setTileTabsEnabled(const bool tileTabsEnabled)
{
    m_tileTabsEnabled = tileTabsEnabled;
}

/**
 * @return Width of the graphics region from scene.
 */
int32_t
BrowserWindowContent::getSceneGraphicsWidth() const
{
    return m_sceneGraphicsWidth;
}

/**
 * Set the width of the graphics region for scene.
 *
 * @param width
 *     New value for width.
 */
void
BrowserWindowContent::setSceneGraphicsWidth(const int32_t width)
{
    m_sceneGraphicsWidth = width;
}

/**
 * @return Height of the graphics region from scene.
 */
int32_t
BrowserWindowContent::getSceneGraphicsHeight() const
{
    return m_sceneGraphicsHeight;
}

/**
 * Set the height of the graphics region for scene.
 *
 * @param height
 *     New value for height.
 */
void
BrowserWindowContent::setSceneGraphicsHeight(const int32_t height)
{
    m_sceneGraphicsHeight = height;
}

/**
 * @return The tile tabs configuration from the scene.
 */
TileTabsConfiguration*
BrowserWindowContent::getSceneTileTabsConfiguration() const
{
    return m_sceneTileTabsConfiguration.get();
}

/**
 * @return Index of the selected tab in the scene.
 */
int32_t
BrowserWindowContent::getSceneSelectedTabIndex() const
{
    return m_sceneSelectedTabIndex;
}

/**
 * Set index of the selected tab in the scene.
 *
 * @param selectedTabIndex
 *     Index of selected tab.
 */
void
BrowserWindowContent::setSceneSelectedTabIndex(const int32_t selectedTabIndex)
{
    m_sceneSelectedTabIndex = selectedTabIndex;
}

/**
 * @return Indices of the tabs in the scene.
 * In single tab view, this returns one index that is the same as getSceneSelectedTabIndex().
 * In tile tabs view, this contains all tabs in the window.
 */
std::vector<int32_t>
BrowserWindowContent::getSceneTabIndices() const
{
    return m_sceneTabIndices;
}

/**
 * Set indices of the tabs in the scene.
 * In single tab view, this contains one index that the displayed tab.
 * In tile tabs view, this contains all tabs in the window.
 *
 * @param sceneTabIndices
 *     Indices of the tabs.
 */
void
BrowserWindowContent::setSceneWindowTabIndices(const std::vector<int32_t>& sceneTabIndices)
{
    m_sceneTabIndices = sceneTabIndices;
}

/**
 * Copy the tile tabs configuration for writing to the scene.
 *
 * @param tileTabsConfiguration
 *     Tile tabs configuration for scene.
 */
void
BrowserWindowContent::copyTileTabsConfigurationForSavingScene(const TileTabsConfiguration* tileTabsConfiguration)
{
    m_tileTabsConfigurationForSavingScene.reset(new TileTabsConfiguration(*tileTabsConfiguration));
}

/**
 * Save information specific to this type of model to the scene.
 *
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    saving the scene.
 *
 * @param instanceName
 *    Name of instance in the scene.
 */
SceneClass*
BrowserWindowContent::saveToScene(const SceneAttributes* sceneAttributes,
                                 const AString& instanceName)
{
    SceneClass* sceneClass = new SceneClass(instanceName,
                                            "BrowserWindowContent",
                                            1);
    m_sceneAssistant->saveMembers(sceneAttributes,
                                  sceneClass);
    
    if (m_tileTabsEnabled) {
        /*
         * The browser window will provide selected tile tabs configuration
         * by calling this class' copyTileTabsConfigurationForSavingScene() method.
         * It is saved as the scene's tile tabs configuration.
         * The content of "m_sceneTileTabsConfiguration" is NOT saved to the scene
         * but it will be updated when the scene is read.
         */
        CaretAssert(m_tileTabsConfigurationForSavingScene.get());
        const AString xmlString = m_tileTabsConfigurationForSavingScene->encodeInXML();
        sceneClass->addString("m_sceneTileTabsConfiguration",
                              xmlString);
        
        /*
         * Clear tile tabs config now that it has been written and is no longer needed
         */
        m_tileTabsConfigurationForSavingScene.reset();
    }
    //sceneClass->addString("m_sceneTileTabsConfiguration", m_sceneTileTabsConfiguration->encodeInXML());
    sceneClass->addChild(new SceneIntegerArray("m_sceneTabIndices",
                                               m_sceneTabIndices));

    // Uncomment if sub-classes must save to scene
    //saveSubClassDataToScene(sceneAttributes,
    //                        sceneClass);
    
    return sceneClass;
}

/**
 * Restore information specific to the type of model from the scene.
 *
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    restoring the scene.
 *
 * @param sceneClass
 *     sceneClass from which model specific information is obtained.
 */
void
BrowserWindowContent::restoreFromScene(const SceneAttributes* sceneAttributes,
                                      const SceneClass* sceneClass)
{
    reset();
    
    if (sceneClass == NULL) {
        return;
    }
    
    m_sceneAssistant->restoreMembers(sceneAttributes,
                                     sceneClass);    
    
    const ScenePrimitiveArray* sceneTabIndicesArray = sceneClass->getPrimitiveArray("m_sceneTabIndices");
    if (sceneTabIndicesArray != NULL) {
        sceneTabIndicesArray->integerVectorValues(m_sceneTabIndices);
    }
    
    const AString stringTileTabsConfig = sceneClass->getStringValue("m_sceneTileTabsConfiguration");
    if ( ! stringTileTabsConfig.isEmpty()) {
        if (m_sceneTileTabsConfiguration->decodeFromXML(stringTileTabsConfig)) {
            m_sceneTileTabsConfiguration->setName(s_sceneTileTabsConfigurationText
                                                  + " "
                                                  + sceneAttributes->getSceneName());
        }
        else {
            sceneAttributes->addToErrorMessage("Failed to decode tile tabs configuration from BrowserWindowContent: \""
                                               + stringTileTabsConfig
                                               + "\"");
        }
    }
    
    
    //Uncomment if sub-classes must restore from scene
    //restoreSubClassDataFromScene(sceneAttributes,
    //                             sceneClass);
    
}

/**
 * Restore from BrainBrowserWindow scene from before this class was created.
 
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    restoring the scene.
 *
 * @param browserClass
 *     A BrainBrowserWindow scene.
 */
void
BrowserWindowContent::restoreFromOldBrainBrowserWindowScene(const SceneAttributes* sceneAttributes,
                                                            const SceneClass* browserClass)
{
    CaretAssert(browserClass);
    
    reset();
    
    if (browserClass->getName() != "m_brainBrowserWindows") {
        sceneAttributes->addToErrorMessage("When restoring BrowserWindowContent::restoreFromOldBrainBrowserWindowScene "
                                           "name of scene class is not m_brainBrowserWindows");
        return;
    }
    
    m_windowAspectRatioLocked = browserClass->getBooleanValue("m_windowAspectRatioLockedAction",
                                                              false);
    
    m_windowAspectLockedRatio = browserClass->getFloatValue("m_aspectRatio", 1.0f);
    
    /* not in old scene */
    m_allTabsInWindowAspectRatioLocked = false;
    
    m_tileTabsEnabled = browserClass->getBooleanValue("m_viewTileTabsAction",
                                                      false);
    
    
    const SceneClass* graphicsGeometry = browserClass->getClass("openGLWidgetGeometry");
    if (graphicsGeometry != NULL) {
        m_sceneGraphicsWidth  = graphicsGeometry->getIntegerValue("geometryWidth", -1);
        m_sceneGraphicsHeight = graphicsGeometry->getIntegerValue("geometryHeight", -1);
    }
    
    const AString tileTabsConfigString = browserClass->getStringValue("m_sceneTileTabsConfiguration");
    if ( ! tileTabsConfigString.isEmpty()) {
        m_sceneTileTabsConfiguration->decodeFromXML(tileTabsConfigString);
        m_sceneTileTabsConfiguration->setName(s_sceneTileTabsConfigurationText
                                              + " "
                                              + sceneAttributes->getSceneName());
    }
    
    const SceneClass* toolbarClass = browserClass->getClass("m_toolbar");
    if (toolbarClass != NULL) {
        m_sceneSelectedTabIndex = toolbarClass->getIntegerValue("selectedTabIndex", -1);
        
        const ScenePrimitiveArray* tabIndexArray = toolbarClass->getPrimitiveArray("tabIndices");
        if (tabIndexArray != NULL) {
            const int32_t numTabs = tabIndexArray->getNumberOfArrayElements();
            for (auto iTab = 0; iTab < numTabs; iTab++) {
                m_sceneTabIndices.push_back(tabIndexArray->integerValue(iTab));
            }
        }
    }
}


