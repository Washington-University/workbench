
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

#include <numeric>

#include "AnnotationBrowserTab.h"
#include "BrowserTabContent.h"
#include "CaretAssert.h"
#include "EventBrowserWindowGetTabs.h"
#include "EventManager.h"
#include "SceneClass.h"
#include "SceneClassAssistant.h"
#include "SceneIntegerArray.h"
#include "ScenePrimitive.h"
#include "TileTabsLayoutGridConfiguration.h"

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
    m_automaticGridTileTabsConfiguration.reset(TileTabsLayoutGridConfiguration::newInstanceAutomaticGrid());
    m_customGridTileTabsConfiguration.reset(TileTabsLayoutGridConfiguration::newInstanceCustomGrid());
    m_validFlag = false;
    reset();
    
    m_sceneAssistant = std::unique_ptr<SceneClassAssistant>(new SceneClassAssistant());
    m_sceneAssistant->add("m_validFlag", &m_validFlag);
    m_sceneAssistant->add("m_windowAspectRatioLocked", &m_windowAspectRatioLocked);
    m_sceneAssistant->add("m_windowAspectLockedRatio", &m_windowAspectLockedRatio);
    m_sceneAssistant->add("m_allTabsInWindowAspectRatioLocked", &m_allTabsInWindowAspectRatioLocked);
    m_sceneAssistant->add("m_tileTabsEnabled", &m_tileTabsEnabled);
    m_sceneAssistant->add<TileTabsLayoutConfigurationTypeEnum, TileTabsLayoutConfigurationTypeEnum::Enum>("m_tileTabsConfigurationMode",
                                                                                                          &m_tileTabsConfigurationMode);
    m_sceneAssistant->add("m_sceneGraphicsWidth", &m_sceneGraphicsWidth);
    m_sceneAssistant->add("m_sceneGraphicsHeight", &m_sceneGraphicsHeight);
    m_sceneAssistant->add("m_sceneSelectedTabIndex", &m_sceneSelectedTabIndex);
    m_sceneAssistant->add("m_windowAnnotationsStackingOrder", &m_windowAnnotationsStackingOrder);
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
    m_tileTabsConfigurationMode = TileTabsLayoutConfigurationTypeEnum::AUTOMATIC_GRID;
    m_automaticGridTileTabsConfiguration->updateAutomaticConfigurationRowsAndColumns(1);
    setCustomGridConfigurationToDefault();
    m_sceneSelectedTabIndex = 0;
    m_sceneTabIndices.clear();
    m_windowAnnotationsStackingOrder = -1000;  /* way in front */
}

/**
 * Set the custom grid configuration to the default
 */
void
BrowserWindowContent::setCustomGridConfigurationToDefault()
{
    /* sets rows/columns/factors to defaults */
    m_customGridTileTabsConfiguration->updateAutomaticConfigurationRowsAndColumns(1);
    m_customGridTileTabsConfiguration->setCustomDefaultFlag(true);
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
 * @return The selected tile tabs configuration.  
 * This will be the automatic configuration when automatic is selected,
 * otherwise it is the custom configuration.
 * Will return NULL if the selected tile tabs configuration is
 * neither AUTO nor CUSTOM grid.
 */
TileTabsLayoutBaseConfiguration*
BrowserWindowContent::getSelectedTileTabsGridConfiguration()
{
    TileTabsLayoutBaseConfiguration* configMode = NULL;
    
    switch (m_tileTabsConfigurationMode) {
        case TileTabsLayoutConfigurationTypeEnum::AUTOMATIC_GRID:
            configMode = m_automaticGridTileTabsConfiguration.get();
            break;
        case TileTabsLayoutConfigurationTypeEnum::CUSTOM_GRID:
            configMode = m_customGridTileTabsConfiguration.get();
            break;
        case TileTabsLayoutConfigurationTypeEnum::MANUAL:
            return NULL;
            break;
    }
    CaretAssert(configMode);
    
    return configMode;
}

/**
 * @return The selected tile tabs configuration (const method)
 * This will be the automatic configuration when automatic is selected,
 * otherwise it is the custom configuration.
 * Will return NULL if the selected tile tabs configuration is
 * neither AUTO nor CUSTOM grid.
 */
const TileTabsLayoutBaseConfiguration*
BrowserWindowContent::getSelectedTileTabsGridConfiguration() const
{
    TileTabsLayoutBaseConfiguration* configMode = NULL;
    
    switch (m_tileTabsConfigurationMode) {
        case TileTabsLayoutConfigurationTypeEnum::AUTOMATIC_GRID:
            configMode = m_automaticGridTileTabsConfiguration.get();
            break;
        case TileTabsLayoutConfigurationTypeEnum::CUSTOM_GRID:
            configMode = m_customGridTileTabsConfiguration.get();
            break;
        case TileTabsLayoutConfigurationTypeEnum::MANUAL:
            return NULL;
            break;
    }
    CaretAssert(configMode);
    
    return configMode;
}

/**
 * @return The automatic grid tile tabs configuration
 */
TileTabsLayoutGridConfiguration*
BrowserWindowContent::getAutomaticGridTileTabsConfiguration()
{
    return m_automaticGridTileTabsConfiguration.get();
}

/**
 * @return The automatic grid tile tabs configuration (const method)
 */
const TileTabsLayoutGridConfiguration*
BrowserWindowContent::getAutomaticGridTileTabsConfiguration() const
{
    return m_automaticGridTileTabsConfiguration.get();
}

/**
 * @return The custom grid tile tabs configuration
 */
TileTabsLayoutGridConfiguration*
BrowserWindowContent::getCustomGridTileTabsConfiguration()
{
    return m_customGridTileTabsConfiguration.get();
}

/**
 * @return The custom grid tile tabs configuration (const method)
 */
const TileTabsLayoutGridConfiguration*
BrowserWindowContent::getCustomGridTileTabsConfiguration() const
{
    return m_customGridTileTabsConfiguration.get();
}

/**
 * Set the custom grid tile tabs configuration
 *
 * @param gridConfiguration
 *     Grid configuration that is copied to the custom grid
 */
void
BrowserWindowContent::setCustomGridTileTabsConfiguration(const TileTabsLayoutGridConfiguration* gridConfiguration)
{
    CaretAssert(gridConfiguration);
    if (gridConfiguration == NULL) {
        return;
    }
    
    m_customGridTileTabsConfiguration->copy(*gridConfiguration);
}

/**
 * @return The tile tabs configuration mode.
 */
TileTabsLayoutConfigurationTypeEnum::Enum
BrowserWindowContent::getTileTabsConfigurationMode() const
{
    return m_tileTabsConfigurationMode;
}

/**
 * Set the tile tabs configuration mode.
 *
 * @param configMode
 *     New value for configuration mode.
 */
void
BrowserWindowContent::setTileTabsConfigurationMode(const TileTabsLayoutConfigurationTypeEnum::Enum configMode)
{
    const TileTabsLayoutConfigurationTypeEnum::Enum previousMode = m_tileTabsConfigurationMode;
    m_tileTabsConfigurationMode = configMode;
    
    switch (m_tileTabsConfigurationMode) {
        case  TileTabsLayoutConfigurationTypeEnum::AUTOMATIC_GRID:
            break;
        case TileTabsLayoutConfigurationTypeEnum::CUSTOM_GRID:
            if (m_customGridTileTabsConfiguration->isCustomDefaultFlag()) {
                m_customGridTileTabsConfiguration->setCustomDefaultFlag(false);
                
                EventBrowserWindowGetTabs windowTabsEvent(m_windowIndex);
                EventManager::get()->sendEvent(windowTabsEvent.getPointer());
                int32_t numTabs = static_cast<int32_t>(windowTabsEvent.getBrowserTabIndices().size());
                if (numTabs > 0) {
                    m_customGridTileTabsConfiguration->updateAutomaticConfigurationRowsAndColumns(numTabs);
                }
            }
            break;
        case TileTabsLayoutConfigurationTypeEnum::MANUAL:
        {
            EventBrowserWindowGetTabs tabsEvent(m_windowIndex);
            EventManager::get()->sendEvent(tabsEvent.getPointer());
            
            std::vector<BrowserTabContent*> allTabs = tabsEvent.getBrowserTabs();
            bool allDefaultFlag(true);
            for (auto tab : allTabs) {
                if ( ! tab->isDefaultManualTabGeometryBounds()) {
                    allDefaultFlag = false;
                    break;
                }
            }
            if (allDefaultFlag) {
                switch (previousMode) {
                    case TileTabsLayoutConfigurationTypeEnum::AUTOMATIC_GRID:
                        setManualConfigurationFromGridConfiguration(getAutomaticGridTileTabsConfiguration());
                        break;
                    case TileTabsLayoutConfigurationTypeEnum::CUSTOM_GRID:
                        setManualConfigurationFromGridConfiguration(getCustomGridTileTabsConfiguration());
                        break;
                    case TileTabsLayoutConfigurationTypeEnum::MANUAL:
                        /* manual was not initialized so use automatic grid */
                        setManualConfigurationFromGridConfiguration(getAutomaticGridTileTabsConfiguration());
                        break;
                }
            }
        }
            break;
    }
}

/**
 * Set the manual configuration from the given grid configuratin
 *
 * @param gridConfiguration
 *     Grid configuration copied to manual configuration.
 */
void
BrowserWindowContent::setManualConfigurationFromGridConfiguration(TileTabsLayoutGridConfiguration* gridConfiguration)
{
    EventBrowserWindowGetTabs tabsEvent(m_windowIndex);
    EventManager::get()->sendEvent(tabsEvent.getPointer());
    
    std::vector<BrowserTabContent*> allTabs = tabsEvent.getBrowserTabs();
    const int32_t numTabs = static_cast<int32_t>(allTabs.size());

    const int32_t windowWidth(100);
    const int32_t windowHeight(100);
    std::vector<int32_t> rowHeightsInt;
    std::vector<int32_t> columnWidthsInt;
    
    if (gridConfiguration->getRowHeightsAndColumnWidthsForWindowSize(windowWidth,
                                                                     windowHeight,
                                                                     numTabs,
                                                                     gridConfiguration->getLayoutType(),
                                                                     rowHeightsInt,
                                                                     columnWidthsInt)) {
        const int32_t numRows = static_cast<int32_t>(rowHeightsInt.size());
        const int32_t numCols = static_cast<int32_t>(columnWidthsInt.size());
        
        /*
         * No longer computer the row sum as it causes problems when
         * the tabs do not fill the entire width and/or height of window
         */
        const float rowSum = 0.0;
        std::vector<float> rowHeights;
        for (int32_t iRow = 0; iRow < numRows; iRow++) {
            if (rowSum > 0.0) {
                rowHeights.push_back((rowHeightsInt[iRow] / rowSum) * 100.0);
            }
            else {
                rowHeights.push_back(rowHeightsInt[iRow]);
            }
        }
        CaretAssert(rowHeights.size() == rowHeightsInt.size());
        
        const float columnSum = 0.0;
        std::vector<float> columnWidths;
        for (int32_t iCol = 0; iCol < numCols; iCol++) {
            if (columnSum > 0.0) {
                columnWidths.push_back((columnWidthsInt[iCol] / columnSum) * 100.0);
            }
            else {
                columnWidths.push_back(columnWidthsInt[iCol]);
            }
        }
        CaretAssert(columnWidths.size() == columnWidthsInt.size());
        
        int32_t tabCounter(0);
        float yBottom(windowHeight);
        for (int32_t i = 0; i < numRows; i++) {
            CaretAssertVectorIndex(rowHeights, i);
            const float height = rowHeights[i];
            yBottom -= height;
            
            switch (gridConfiguration->getRow(i)->getContentType()) {
                case TileTabsGridRowColumnContentTypeEnum::SPACE:
                    break;
                case TileTabsGridRowColumnContentTypeEnum::TAB:
                {
                    float xLeft(0.0);
                    for (int32_t j = 0; j < numCols; j++) {
                        CaretAssertVectorIndex(columnWidths, j);
                        const float width = columnWidths[j];
                        
                        switch (gridConfiguration->getColumn(j)->getContentType()) {
                            case TileTabsGridRowColumnContentTypeEnum::SPACE:
                                break;
                            case TileTabsGridRowColumnContentTypeEnum::TAB:
                                if (tabCounter < numTabs) {
                                    CaretAssertVectorIndex(allTabs, tabCounter);
                                    BrowserTabContent* tabContent(allTabs[tabCounter]);
                                    AnnotationBrowserTab* browserTabAnnotation = tabContent->getManualLayoutBrowserTabAnnotation();
                                    CaretAssert(browserTabAnnotation);
                                    browserTabAnnotation->setBounds2D((xLeft / windowWidth) * 100.0,
                                                                      ((xLeft + width) / windowWidth) * 100.0,
                                                                      (yBottom / windowHeight) * 100.0,
                                                                      ((yBottom + height) / windowHeight) * 100.0);
                                    browserTabAnnotation->setStackingOrder(tabCounter + 1);
                                    browserTabAnnotation->setBackgroundType(TileTabsLayoutBackgroundTypeEnum::OPAQUE_BG);
                                    
                                    tabCounter++;
                                }
                                break;
                        }
                        
                        xLeft += width;
                    }
                }
                    break;
            }
        }
    }

}
/**
 * @return True if tile tabs is enabled AND a manual configuration is selected
 */
bool
BrowserWindowContent::isManualModeTileTabsConfigurationEnabled() const
{
    if (isTileTabsEnabled()) {
        switch (getTileTabsConfigurationMode()) {
            case  TileTabsLayoutConfigurationTypeEnum::AUTOMATIC_GRID:
                break;
            case TileTabsLayoutConfigurationTypeEnum::CUSTOM_GRID:
                break;
            case TileTabsLayoutConfigurationTypeEnum::MANUAL:
                return true;
                break;
        }
    }
    return false;
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
 * @return The window annotations stacking order for a manual tile tabs layout
 */
int32_t
BrowserWindowContent::getWindowAnnotationsStackingOrder() const
{
    return m_windowAnnotationsStackingOrder;
}

/**
 * Set the window annotations stacking order for a manual tile tabs layout
 * @param stackingOrder
 *    The new stacking order
 */
void
BrowserWindowContent::setWindowAnnotationsStackingOrder(const int32_t stackingOrder)
{
    m_windowAnnotationsStackingOrder = stackingOrder;
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
    /*
     * Version 2 added by WB-735 in May 2018
     */
    SceneClass* sceneClass = new SceneClass(instanceName,
                                            "BrowserWindowContent",
                                            2);
    m_sceneAssistant->saveMembers(sceneAttributes,
                                  sceneClass);
    
    sceneClass->addString("m_customTileTabsConfigurationLatest",
                          m_customGridTileTabsConfiguration->encodeInXML());
    
    /*
     * Add a tile tabs version one so older versions of wb_view
     * may still load the scene correctly
     */
    sceneClass->addString("m_customTileTabsConfiguration",
                          m_customGridTileTabsConfiguration->encodeVersionInXML(1));
    
    /*
     * Write the tile tabs configuration a second time using
     * the old name 'm_sceneTileTabsConfiguration'.  This will
     * allow the previous version of Workbench to display
     * tile tabs correctly.
     */
    sceneClass->addString("m_sceneTileTabsConfiguration",
                          m_customGridTileTabsConfiguration->encodeVersionInXML(1));

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
    
    const int32_t sceneVersion = sceneClass->getVersionNumber();
    
    m_sceneAssistant->restoreMembers(sceneAttributes,
                                     sceneClass);    
    
    const ScenePrimitiveArray* sceneTabIndicesArray = sceneClass->getPrimitiveArray("m_sceneTabIndices");
    if (sceneTabIndicesArray != NULL) {
        sceneTabIndicesArray->integerVectorValues(m_sceneTabIndices);
    }
    
    /*
     * Try restoring newest tile tabs configuration
     */
    AString tileTabsConfig = sceneClass->getStringValue("m_customTileTabsConfigurationLatest");
    if ( ! tileTabsConfig.isEmpty()) {
        /* Since latest was found, restore, but do not use, older configuration to prevent 'not found' warning */
        sceneClass->getStringValue("m_customTileTabsConfiguration");
    }
    if (tileTabsConfig.isEmpty()) {
        /* Try version one */
        tileTabsConfig = sceneClass->getStringValue("m_customTileTabsConfiguration");
    }
    if (tileTabsConfig.isEmpty()) {
        /* Restore an old name for custom configuration */
        tileTabsConfig = sceneClass->getStringValue("m_tileTabsConfiguration");
    }
    if (tileTabsConfig.isEmpty()) {
        /*
         * "m_sceneTileTabsConfiguration" is from scenes before "m_customTileTabsConfiguration" was added
         */
        tileTabsConfig = sceneClass->getStringValue("m_sceneTileTabsConfiguration");
    }
    if ( ! tileTabsConfig.isEmpty()) {
        AString errorMessage;
        TileTabsLayoutBaseConfiguration* config = TileTabsLayoutBaseConfiguration::decodeFromXML(tileTabsConfig,
                                                                                                         errorMessage);
        if (config != NULL) {
            switch (config->getLayoutType()) {
                case TileTabsLayoutConfigurationTypeEnum::AUTOMATIC_GRID:
                    CaretAssert(0);
                    break;
                case TileTabsLayoutConfigurationTypeEnum::CUSTOM_GRID:
                    m_customGridTileTabsConfiguration.reset(config->castToGridConfiguration());
                    break;
                case TileTabsLayoutConfigurationTypeEnum::MANUAL:
                    CaretAssert(0);
                    break;
            }
        }
        else {
            sceneAttributes->addToErrorMessage("Failed to decode custom tile tabs configuration with error \""
                                               + errorMessage
                                               + "\" from BrowserWindowContent: \""
                                               + tileTabsConfig
                                               + "\"");
            m_customGridTileTabsConfiguration.reset(TileTabsLayoutGridConfiguration::newInstanceCustomGrid());
        }
        
        /*
         * This statement is here to prevent a warning about an unrestored element.
         * When the scene is written after WB-735, 'm_sceneTileTabsConfiguration' is written in
         * addition to 'm_tileTabsConfiguration' to be compatible with wb_view
         * versions before before WB-735.
         */
        (void)sceneClass->getStringValue("m_sceneTileTabsConfiguration");
    }    
    
    const ScenePrimitive* oldTileTabsAutoPrimitive = sceneClass->getPrimitive("m_tileTabsAutomaticConfigurationEnabled");
    if (oldTileTabsAutoPrimitive != NULL) {
        const bool autoModeSelected = oldTileTabsAutoPrimitive->booleanValue();
        if (autoModeSelected) {
            m_tileTabsConfigurationMode = TileTabsLayoutConfigurationTypeEnum::AUTOMATIC_GRID;
        }
        else {
            m_tileTabsConfigurationMode = TileTabsLayoutConfigurationTypeEnum::CUSTOM_GRID;
        }
    }
    
    if (sceneVersion < 2) {
        /*
         * Automatic configuration added by WB-735 in May 2018
         * If tile tabs was enabled, use CUSTOM, otherwise AUTOMATIC
         */
        if (m_tileTabsEnabled) {
            m_tileTabsConfigurationMode = TileTabsLayoutConfigurationTypeEnum::CUSTOM_GRID;
        }
        else {
            m_tileTabsConfigurationMode = TileTabsLayoutConfigurationTypeEnum::AUTOMATIC_GRID;
        }
    }
    
    /*
     * If NOT grid configuration mode for tile tabs, reset the
     * grid configuration to its default.  When the user selects
     * grid configuration, it will default to the automatic grid
     * so that all tabs are displayed.
     */
    switch (m_tileTabsConfigurationMode) {
        case TileTabsLayoutConfigurationTypeEnum::AUTOMATIC_GRID:
            setCustomGridConfigurationToDefault();
            break;
        case TileTabsLayoutConfigurationTypeEnum::CUSTOM_GRID:
            break;
        case TileTabsLayoutConfigurationTypeEnum::MANUAL:
            setCustomGridConfigurationToDefault();
            break;
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
        AString errorMessage;
        TileTabsLayoutBaseConfiguration* config = TileTabsLayoutBaseConfiguration::decodeFromXML(tileTabsConfigString,
                                                                                                 errorMessage);
        if (config != NULL) {
            switch (config->getLayoutType()) {
                case TileTabsLayoutConfigurationTypeEnum::AUTOMATIC_GRID:
                    CaretAssert(0);
                    break;
                case TileTabsLayoutConfigurationTypeEnum::CUSTOM_GRID:
                    m_customGridTileTabsConfiguration.reset(config->castToGridConfiguration());
                    break;
                case TileTabsLayoutConfigurationTypeEnum::MANUAL:
                    CaretAssert(0);
                    break;
            }
        }
        else {
            sceneAttributes->addToErrorMessage("Failed to decode custom tile tabs configuration with error \""
                                               + errorMessage
                                               + "\" from BrowserWindowContent: \""
                                               + tileTabsConfigString
                                               + "\"");
            m_customGridTileTabsConfiguration.reset(TileTabsLayoutGridConfiguration::newInstanceCustomGrid());
        }
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
    
    /*
     * Automatic configuration added by WB-735 in May 2018
     * If tile tabs was enabled, use CUSTOM, otherwise AUTOMATIC
     */
    if (m_tileTabsEnabled) {
        m_tileTabsConfigurationMode = TileTabsLayoutConfigurationTypeEnum::CUSTOM_GRID;
    }
    else {
        m_tileTabsConfigurationMode = TileTabsLayoutConfigurationTypeEnum::AUTOMATIC_GRID;
    }
}


