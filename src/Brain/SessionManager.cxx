
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

#include <algorithm>

#define __SESSION_MANAGER_DECLARE__
#include "SessionManager.h"
#undef __SESSION_MANAGER_DECLARE__

#include "ApplicationInformation.h"
#include "BackgroundAndForegroundColorsSceneHelper.h"
#include "Brain.h"
#include "BrowserTabContent.h"
#include "CaretAssert.h"
#include "CaretLogger.h"
#include "CaretPreferences.h"
#include "CiftiConnectivityMatrixDataFileManager.h"
#include "CiftiFiberTrajectoryManager.h"
#include "ElapsedTimer.h"
#include "EventManager.h"
#include "EventBrowserTabDelete.h"
#include "EventBrowserTabGet.h"
#include "EventBrowserTabGetAll.h"
#include "EventBrowserTabIndicesGetAll.h"
#include "EventBrowserTabNew.h"
#include "EventCaretPreferencesGet.h"
#include "EventModelAdd.h"
#include "EventModelDelete.h"
#include "EventModelGetAll.h"
#include "EventProgressUpdate.h"
#include "ImageCaptureSettings.h"
#include "LogManager.h"
#include "MapYokingGroupEnum.h"
#include "ModelWholeBrain.h"
#include "SceneAttributes.h"
#include "SceneClass.h"
#include "SceneClassArray.h"
#include "ScenePrimitiveArray.h"
#include "VolumeSurfaceOutlineSetModel.h"


using namespace caret;


/**
 * Constructor.
 */
SessionManager::SessionManager()
: CaretObject(), EventListenerInterface(), SceneableInterface()
{
    m_caretPreferences = new CaretPreferences();
    
    m_imageCaptureDialogSettings = new ImageCaptureSettings();

    m_ciftiConnectivityMatrixDataFileManager = new CiftiConnectivityMatrixDataFileManager();
    m_ciftiFiberTrajectoryManager = new CiftiFiberTrajectoryManager();
    
    for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; i++) {
        m_browserTabs[i] = NULL;
    }
    
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_BROWSER_TAB_DELETE);
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_BROWSER_TAB_GET);
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_BROWSER_TAB_GET_ALL);
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_BROWSER_TAB_INDICES_GET_ALL);
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_BROWSER_TAB_NEW);
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_CARET_PREFERENCES_GET);
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_MODEL_ADD);
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_MODEL_DELETE);
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_MODEL_GET_ALL);
    
    Brain* brain = new Brain();
    m_brains.push_back(brain);
}

/**
 * Destructor.
 */
SessionManager::~SessionManager()
{
    /*
     * Delete browser tab content.  Workbench requests deletion of tab content
     * as browser tabs are closed.  However, command line does not issue
     * commands to delete tabs so this code will do so.
     */
    for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; i++) {
        if (m_browserTabs[i] != NULL) {
            delete m_browserTabs[i];
            m_browserTabs[i] = NULL;
        }
    }
    
    int32_t numberOfBrains = getNumberOfBrains();
    for (int32_t i = (numberOfBrains - 1); i >= 0; i--) {
        delete m_brains[i];
    }
    m_brains.clear();
    
    EventManager::get()->removeAllEventsFromListener(this);
    
    delete m_ciftiConnectivityMatrixDataFileManager;
    delete m_ciftiFiberTrajectoryManager;
    
    delete m_imageCaptureDialogSettings;
    
    delete m_caretPreferences;
}

/**
 * Create the session manager.
 * This must be called one AND ONLY one time prior to any
 * other Caret mechanisms.
 *
 * @param applicationType
 *    The type of application (command line or GUI).
 */
void 
SessionManager::createSessionManager(const ApplicationTypeEnum::Enum applicationType)
{
    CaretAssertMessage((s_singletonSessionManager == NULL), 
                       "Session manager has already been created.");

    /*
     * Set the type of application.
     */
    ApplicationInformation::setApplicationType(applicationType);
    
    /*
     * Create log manager.
     */
    LogManager::createLogManager();
    
    /*
     * Create event manager first.
     */
    EventManager::createEventManager();
    
    /*
     * Create session manager.
     */
    s_singletonSessionManager = new SessionManager();
}

/**
 * Delete the session manager.
 * This may only be called one time after session manager is created.
 */
void 
SessionManager::deleteSessionManager()
{
    CaretAssertMessage((s_singletonSessionManager != NULL), 
                       "Session manager does not exist, cannot delete it.");
    
    delete s_singletonSessionManager;
    s_singletonSessionManager = NULL;
    
    /*
     * Session manager must be deleted before the event
     * manager is deleted.
     */
    EventManager::deleteEventManager();
    
    LogManager::deleteLogManager();
}

/**
 * Get the one and only session manager.
 *
 * @return  Pointer to the session manager.
 */
SessionManager* 
SessionManager::get()
{
    CaretAssertMessage(s_singletonSessionManager,
                       "Session manager was not created.\n"
                       "It must be created with SessionManager::createSessionManager().");
    
    return s_singletonSessionManager;
}

/**
 * Add a brain to this session.  
 * In most cases, there is one brain per subject.
 *
 * @param shareDisplayPropertiesFlag
 *    If true display properties are shared which 
 *    is appropriate for the GUI version of Caret.
 *    For SumsDB, false may be appropriate to keep
 *    each subject independent of the other.
 * 
 * @return New brain that was added.
 */
Brain* 
SessionManager::addBrain(const bool /*shareDisplayPropertiesFlag*/)
{
    CaretAssertMessage(0, "Adding brains not implemented at this time.");
    return NULL;
}

/**
 * Get the number of brains.
 * There is always at least one brain.
 * 
 * @return Number of brains.
 */
int32_t 
SessionManager::getNumberOfBrains() const
{
    return m_brains.size();
}

/**
 * Get the brain at the specified index.
 * There is always one brain so passing 
 * zero as the index will always work.
 * 
 * @param brainIndex
 *    Index of brain.
 * @return
 *    Brain at specified index.
 */
Brain* 
SessionManager::getBrain(const int32_t brainIndex) const
{
    CaretAssertVectorIndex(m_brains, brainIndex);
    
    return m_brains[brainIndex];
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
SessionManager::toString() const
{
    return "SessionManager";
}

/**
 * Receive events.
 *
 * @param event
 *    Event that needs to be processed.
 */
void 
SessionManager::receiveEvent(Event* event)
{
    if (event->getEventType() == EventTypeEnum::EVENT_BROWSER_TAB_NEW) {
        EventBrowserTabNew* tabEvent =
            dynamic_cast<EventBrowserTabNew*>(event);
        CaretAssert(tabEvent);
        
        tabEvent->setEventProcessed();
        
        bool createdTab = false;
        for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; i++) {
            if (m_browserTabs[i] == NULL) {
                BrowserTabContent* tab = new BrowserTabContent(i);
                tab->update(m_models);
                m_browserTabs[i] = tab;
                tabEvent->setBrowserTab(tab);
                createdTab = true;
                break;
            }
        }
        if (createdTab == false) {
            tabEvent->setErrorMessage("Workbench is exhausted.  It cannot create any more tabs.");
        }
    }
    else if (event->getEventType() == EventTypeEnum::EVENT_BROWSER_TAB_DELETE) {
        EventBrowserTabDelete* tabEvent =
        dynamic_cast<EventBrowserTabDelete*>(event);
        CaretAssert(tabEvent);
        
        
        BrowserTabContent* tab = tabEvent->getBrowserTab();
        
        for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; i++) {
            if (m_browserTabs[i] == tab) {
                delete m_browserTabs[i];
                m_browserTabs[i] = NULL;
                tabEvent->setEventProcessed();
                break;
            }
        }
    }
    else if (event->getEventType() == EventTypeEnum::EVENT_BROWSER_TAB_GET) {
        EventBrowserTabGet* tabEvent =
        dynamic_cast<EventBrowserTabGet*>(event);
        CaretAssert(tabEvent);
        
        tabEvent->setEventProcessed();
        
        const int32_t tabNumber = tabEvent->getTabNumber();
        CaretAssertArrayIndex(m_browserTabs, BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS, tabNumber);
        
        tabEvent->setBrowserTab(m_browserTabs[tabNumber]);
    }
    else if (event->getEventType() == EventTypeEnum::EVENT_BROWSER_TAB_GET_ALL) {
        EventBrowserTabGetAll* tabEvent =
        dynamic_cast<EventBrowserTabGetAll*>(event);
        CaretAssert(tabEvent);
        
        tabEvent->setEventProcessed();
        
        for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; i++) {
            if (m_browserTabs[i] != NULL) {
                tabEvent->addBrowserTab(m_browserTabs[i]);
            }
        }
    }
    else if (event->getEventType() == EventTypeEnum::EVENT_BROWSER_TAB_INDICES_GET_ALL) {
        EventBrowserTabIndicesGetAll* tabEvent =
        dynamic_cast<EventBrowserTabIndicesGetAll*>(event);
        CaretAssert(tabEvent);
        
        tabEvent->setEventProcessed();
        
        for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; i++) {
            if (m_browserTabs[i] != NULL) {
                tabEvent->addBrowserTabIndex(m_browserTabs[i]->getTabNumber());
            }
        }
    }
    else if (event->getEventType() == EventTypeEnum::EVENT_CARET_PREFERENCES_GET) {
        EventCaretPreferencesGet* preferencesEvent =
        dynamic_cast<EventCaretPreferencesGet*>(event);
        CaretAssert(preferencesEvent);
        
        preferencesEvent->setCaretPreferences(m_caretPreferences);
        preferencesEvent->setEventProcessed();
    }
    else if (event->getEventType() == EventTypeEnum::EVENT_MODEL_ADD) {
        EventModelAdd* addModelsEvent =
        dynamic_cast<EventModelAdd*>(event);
        CaretAssert(addModelsEvent);
        
        addModelsEvent->setEventProcessed();
        
        m_models.push_back(addModelsEvent->getModel());

        updateBrowserTabContents();
    }
    else if (event->getEventType() == EventTypeEnum::EVENT_MODEL_DELETE) {
        EventModelDelete* deleteModelsEvent =
        dynamic_cast<EventModelDelete*>(event);
        CaretAssert(deleteModelsEvent);
        
        deleteModelsEvent->setEventProcessed();
        
        Model* model = deleteModelsEvent->getModel();
        
        std::vector<Model*>::iterator iter =
        std::find(m_models.begin(),
                  m_models.end(),
                  model);
        
        CaretAssertMessage(iter != m_models.end(),
                           "Trying to delete non-existent model");
        
        m_models.erase(iter);
        
        updateBrowserTabContents();
    }
    else if (event->getEventType() == EventTypeEnum::EVENT_MODEL_GET_ALL) {
        EventModelGetAll* getModelsEvent =
        dynamic_cast<EventModelGetAll*>(event);
        CaretAssert(getModelsEvent);
        
        getModelsEvent->setEventProcessed();
        
        getModelsEvent->addModels(m_models);
    }
}

/**
 * Update all of the browser tab contents since the models have changed.
 */
void 
SessionManager::updateBrowserTabContents()
{
    for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; i++) {
        if (m_browserTabs[i] != NULL) {
            m_browserTabs[i]->update(m_models);
        }
    }
}

/**
 * @return The caret preferences
 */
CaretPreferences* 
SessionManager::getCaretPreferences()
{
    return m_caretPreferences;
}

/**
 * Create a scene for an instance of a class.
 *
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    saving the scene.
 *
 * @return Pointer to SceneClass object representing the state of 
 *    this object.  Under some circumstances a NULL pointer may be
 *    returned.  Caller will take ownership of returned object.
 */
SceneClass* 
SessionManager::saveToScene(const SceneAttributes* sceneAttributes,
                   const AString& instanceName)
{
    switch (sceneAttributes->getSceneType()) {
        case SceneTypeEnum::SCENE_TYPE_FULL:
            break;
        case SceneTypeEnum::SCENE_TYPE_GENERIC:
            break;
    }
    
    SceneClass* sceneClass = new SceneClass(instanceName,
                                            "SessionManager",
                                            1);
    
    {
        /*
         * Save map yoking groups.
         */
        std::vector<MapYokingGroupEnum::Enum> mapYokingGroups;
        MapYokingGroupEnum::getAllEnums(mapYokingGroups);
        const int32_t numMapYokingGroups = static_cast<int32_t>(mapYokingGroups.size());
        if (numMapYokingGroups > 0) {
            std::vector<int32_t> mapMapSelections(numMapYokingGroups, 1);
            
            CaretArray<bool> mapMapEnabled(numMapYokingGroups);
            for (int32_t i = 0; i < numMapYokingGroups; i++) {
                mapMapEnabled[i] = false;
            }
            
            for (int32_t i = 0; i < numMapYokingGroups; i++) {
                const MapYokingGroupEnum::Enum enumValue = mapYokingGroups[i];
                if (enumValue != MapYokingGroupEnum::MAP_YOKING_GROUP_OFF) {
                    const int32_t groupIndex = MapYokingGroupEnum::toIntegerCode(enumValue);
                    const int32_t mapIndex   = MapYokingGroupEnum::getSelectedMapIndex(enumValue);
                    mapMapSelections[groupIndex] = mapIndex;
                    mapMapEnabled[groupIndex] = MapYokingGroupEnum::isEnabled(enumValue);
                }
            }
            
            sceneClass->addIntegerArray("MapYokingIndexArray",
                                        &mapMapSelections[0],
                                        numMapYokingGroups);
            sceneClass->addBooleanArray("MapYokingEnabledArray",
                                        &mapMapEnabled[0],
                                        numMapYokingGroups);
        }
    }
    
    /*
     * Save brains
     */
    std::vector<SceneClass*> brainSceneClasses;
    const int32_t numBrains = m_brains.size();
    for (int32_t i = 0; i < numBrains; i++) {
        brainSceneClasses.push_back(m_brains[i]->saveToScene(sceneAttributes, 
                                                             "m_brains"));
    }
    sceneClass->addChild(new SceneClassArray("m_brains",
                                             brainSceneClasses));
    
    /*
     * Get the tabs that are to be included in the scene
     */
    const std::vector<int32_t> tabIndicesForScene = sceneAttributes->getIndicesOfTabsForSavingToScene();
    
    /*
     * Save browser tabs
     */
    std::vector<SceneClass*> browserTabSceneClasses;
    for (int32_t tabIndex = 0; tabIndex < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; tabIndex++) {
        if (std::find(tabIndicesForScene.begin(),
                      tabIndicesForScene.end(),
                      tabIndex) != tabIndicesForScene.end()) {
            BrowserTabContent* btc = m_browserTabs[tabIndex];
            if (btc != NULL) {
                browserTabSceneClasses.push_back(btc->saveToScene(sceneAttributes,
                                                                  "m_browserTabs"));
            }
        }
    }
    sceneClass->addChild(new SceneClassArray("m_browserTabs",
                                             browserTabSceneClasses));
    
    sceneClass->addChild(m_imageCaptureDialogSettings->saveToScene(sceneAttributes,
                                                                   "m_imageCaptureDialogSettings"));

    /*
     * Save the background and foreground colors to the scene
     */
    const BackgroundAndForegroundColors* colorsPointer = m_caretPreferences->getBackgroundAndForegroundColors();
    CaretAssert(colorsPointer);
    BackgroundAndForegroundColors colors(*colorsPointer);
    
    BackgroundAndForegroundColorsSceneHelper colorHelper(colors);
    sceneClass->addChild(colorHelper.saveToScene(sceneAttributes,
                                                 "backgroundAndForegroundColors"));
    
    return sceneClass;
}

/**
 * Restore the state of an instance of a class.
 * 
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    restoring the scene.
 *
 * @param sceneClass
 *     sceneClass for the instance of a class that implements
 *     this interface.  May be NULL for some types of scenes.
 */
void 
SessionManager::restoreFromScene(const SceneAttributes* sceneAttributes,
                        const SceneClass* sceneClass)
{
    /*
     * Default to user preferences for colors
     */
    m_caretPreferences->setBackgroundAndForegroundColorsMode(BackgroundAndForegroundColorsModeEnum::USER_PREFERENCES);

    if (sceneClass == NULL) {
        return;
    }

    int32_t progressCounter = 0;
    const int32_t PROGRESS_RESTORING_BRAIN = progressCounter++;
    const int32_t PROGRESS_RESTORING_TABS = progressCounter++;
    const int32_t PROGRESS_RESTORING_GUI = progressCounter++;
    const int32_t PROGRESS_RESTORING_TOTAL = progressCounter++;
    
    ElapsedTimer timer;
    timer.start();
    
    EventProgressUpdate progressEvent(0,
                                      PROGRESS_RESTORING_TOTAL,
                                      PROGRESS_RESTORING_BRAIN,
                                      "Restoring Brain");
    EventManager::get()->sendEvent(progressEvent.getPointer());
    if (progressEvent.isCancelled()) {
        resetBrains(true);
        return;
    }
    
    
    switch (sceneAttributes->getSceneType()) {
        case SceneTypeEnum::SCENE_TYPE_FULL:
            break;
        case SceneTypeEnum::SCENE_TYPE_GENERIC:
            break;
    }
    
    
    {
        /*
         * Restore map yoking groups
         */
        std::vector<MapYokingGroupEnum::Enum> mapYokingGroups;
        MapYokingGroupEnum::getAllEnums(mapYokingGroups);
        const int32_t numMapYokingGroups = static_cast<int32_t>(mapYokingGroups.size());
        if (numMapYokingGroups > 0) {
            std::vector<int32_t> mapIndexSelections(numMapYokingGroups, 1);
            
            sceneClass->getIntegerArrayValue("MapYokingIndexArray",
                                             &mapIndexSelections[0],
                                             numMapYokingGroups,
                                             1);
            
            for (int32_t i = 0; i < numMapYokingGroups; i++) {
                bool isValid = false;
                const MapYokingGroupEnum::Enum enumValue = MapYokingGroupEnum::fromIntegerCode(i,
                                                                                               &isValid);
                if (enumValue != MapYokingGroupEnum::MAP_YOKING_GROUP_OFF) {
                    if (isValid) {
                        MapYokingGroupEnum::setSelectedMapIndex(enumValue, mapIndexSelections[i]);
                    }
                    else {
                        MapYokingGroupEnum::setSelectedMapIndex(enumValue, 1);
                    }
                }
            }
            
            
            const ScenePrimitiveArray* mapEnabledArray = sceneClass->getPrimitiveArray("MapYokingEnabledArray");
            if (mapEnabledArray != NULL) {
                for (int32_t i = 0; i < numMapYokingGroups; i++) {
                    bool isValid = false;
                    const MapYokingGroupEnum::Enum enumValue = MapYokingGroupEnum::fromIntegerCode(i,
                                                                                                   &isValid);
                    if (enumValue != MapYokingGroupEnum::MAP_YOKING_GROUP_OFF) {
                        /*
                         * Enabled status was added on 10/17/2014.
                         * Previous to this data, there was no enabled status and
                         * we will assume enabled status was on.
                         */
                        bool enabledStatus = true;
                        if (isValid) {
                            if (mapEnabledArray != NULL) {
                                if (i < mapEnabledArray->getNumberOfArrayElements()) {
                                    enabledStatus = mapEnabledArray->booleanValue(i);
                                }
                            }
                        }
                        MapYokingGroupEnum::setEnabled(enumValue, enabledStatus);
                    }
                }
            }
        }
    }
    
    {
        /*
         * Restore overlay yoking groups
         * Note: Overlay yoking groups were replaced by MapYokingGroupEnum
         * So if overlay yoking group is found, the scene was created
         * before map yoking groups.
         */
        
        const ScenePrimitiveArray* overlayEnabledArray = sceneClass->getPrimitiveArray("OverlayYokingEnabledArray");
        const ScenePrimitiveArray* mapIndexArray = sceneClass->getPrimitiveArray("OverlayYokingGroupArray");
        if (mapIndexArray != NULL) {
            const int32_t numMapIndexElements = mapIndexArray->getNumberOfArrayElements();
            
            for (int32_t i = 0; i < numMapIndexElements; i++) {
                bool isValid = false;
                const MapYokingGroupEnum::Enum enumValue = MapYokingGroupEnum::fromIntegerCode(i,
                                                                                               &isValid);
                if (enumValue != MapYokingGroupEnum::MAP_YOKING_GROUP_OFF) {
                    if (isValid) {
                        MapYokingGroupEnum::setSelectedMapIndex(enumValue,
                                                                mapIndexArray->integerValue(i));
                    }
                    else {
                        MapYokingGroupEnum::setSelectedMapIndex(enumValue, 1);
                    }
                }
            }
            
            
            if (overlayEnabledArray != NULL) {
                const int32_t numMapEnabledElements = overlayEnabledArray->getNumberOfArrayElements();
                for (int32_t i = 0; i < numMapEnabledElements; i++) {
                    bool isValid = false;
                    const MapYokingGroupEnum::Enum enumValue = MapYokingGroupEnum::fromIntegerCode(i,
                                                                                                   &isValid);
                    if (enumValue != MapYokingGroupEnum::MAP_YOKING_GROUP_OFF) {
                        /*
                         * Enabled status was added on 10/17/2014.
                         * Previous to this data, there was no enabled status and
                         * we will assume enabled status was on.
                         */
                        bool enabledStatus = true;
                        if (isValid) {
                            if (overlayEnabledArray != NULL) {
                                if (i < overlayEnabledArray->getNumberOfArrayElements()) {
                                    enabledStatus = overlayEnabledArray->booleanValue(i);
                                }
                            }
                        }
                        MapYokingGroupEnum::setEnabled(enumValue, enabledStatus);
                    }
                }
            }
        }
    }
    
    /*
     * Restore brains
     */
    const SceneClassArray* brainArray = sceneClass->getClassArray("m_brains");
    const int32_t numBrainClasses = brainArray->getNumberOfArrayElements();
    for (int32_t i = 0; i < numBrainClasses; i++) {
        const SceneClass* brainClass = brainArray->getClassAtIndex(i);
        if (i < static_cast<int32_t>(m_brains.size())) {
            m_brains[i]->restoreFromScene(sceneAttributes,
                                          brainClass);
        }
        else {
            Brain* brain = new Brain();
            brain->restoreFromScene(sceneAttributes, 
                                    brainClass);
        }
    }
    
    CaretLogFine("Time to restore brain was "
                 + QString::number(timer.getElapsedTimeSeconds(), 'f', 3)
                 + " seconds");
    timer.reset();
    
    progressEvent.setProgress(PROGRESS_RESTORING_TABS,
                              "Restoring Content of Browser Tabs");
    EventManager::get()->sendEvent(progressEvent.getPointer());
    if (progressEvent.isCancelled()) {
        resetBrains(true);
        return;
    }
    
    /*
     * Remove all tabs
     */
    for (int32_t iTab = 0; iTab < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; iTab++) {
        if (m_browserTabs[iTab] != NULL) {
            delete m_browserTabs[iTab];
            m_browserTabs[iTab] = NULL;
        }
    }
    
    /*
     * Restore tabs
     */
    const SceneClassArray* browserTabArray = sceneClass->getClassArray("m_browserTabs");
    const int32_t numBrowserTabClasses = browserTabArray->getNumberOfArrayElements();
    for (int32_t i = 0; i < numBrowserTabClasses; i++) {
        const SceneClass* browserTabClass = browserTabArray->getClassAtIndex(i);
        
        BrowserTabContent* tab = new BrowserTabContent(i);
        tab->update(m_models);
        tab->getVolumeSurfaceOutlineSet()->selectSurfacesAfterSpecFileLoaded(m_brains[0], 
                                                                             false);
        tab->restoreFromScene(sceneAttributes, 
                              browserTabClass);
        const int32_t tabIndex = tab->getTabNumber();
        CaretAssert(tabIndex >= 0);
        m_browserTabs[tabIndex] = tab;
    }
    
    for (auto brainPtr : m_brains) {
        brainPtr->restoreModelChartOneToModelChartTwo();
    }
    
    /*
     * Restore foreground and background colors to scene foreground and background colors
     */
    if (sceneAttributes->isUseSceneForegroundAndBackgroundColors()) {
        BackgroundAndForegroundColors colors;
        BackgroundAndForegroundColorsSceneHelper colorHelper(colors);
        colorHelper.restoreFromScene(sceneAttributes,
                                     sceneClass->getClass("backgroundAndForegroundColors"));
        if (colorHelper.wasRestoredFromScene()) {
            m_caretPreferences->setBackgroundAndForegroundColorsMode(BackgroundAndForegroundColorsModeEnum::SCENE);
            m_caretPreferences->setSceneBackgroundAndForegroundColors(colors);
        }
        else {
            m_caretPreferences->setBackgroundAndForegroundColorsMode(BackgroundAndForegroundColorsModeEnum::USER_PREFERENCES);
        }
    }
    
    m_imageCaptureDialogSettings->restoreFromScene(sceneAttributes,
                                                   sceneClass->getClass("m_imageCaptureDialogSettings"));
    
    CaretLogFine("Time to restore browser tab content was "
                 + QString::number(timer.getElapsedTimeSeconds(), 'f', 3)
                 + " seconds");
    timer.reset();
    
    progressEvent.setProgress(PROGRESS_RESTORING_GUI,
                              "Restoring Graphical User Interface");
    EventManager::get()->sendEvent(progressEvent.getPointer());
    if (progressEvent.isCancelled()) {
        resetBrains(true);
        return;
    }    
}

/**
 * Reset the first brain and remove all other brains.
 */
void
SessionManager::resetBrains(const bool keepSceneFiles)
{
    const int32_t numBrains = static_cast<int32_t>(m_brains.size());
    for (int32_t i = 0; i < numBrains; i++) {
        if (i > 0) {
            delete m_brains[i];
        }
        else if (keepSceneFiles) {
            m_brains[i]->resetBrainKeepSceneFiles();
        }
        else {
            m_brains[i]->resetBrain();
        }
    }

    if (numBrains > 1) {
        m_brains.resize(1);
    }
}

/**
 * @param The CIFTI connectivity matrix data file manager
 */
CiftiConnectivityMatrixDataFileManager*
SessionManager::getCiftiConnectivityMatrixDataFileManager()
{
    return m_ciftiConnectivityMatrixDataFileManager;
}

/**
 * @param The CIFTI connectivity matrix data file manager
 */
const CiftiConnectivityMatrixDataFileManager*
SessionManager::getCiftiConnectivityMatrixDataFileManager() const
{
    return m_ciftiConnectivityMatrixDataFileManager;
}

/**
 * @return The CIFTI Fiber Trajectory Manager
 */
CiftiFiberTrajectoryManager*
SessionManager::getCiftiFiberTrajectoryManager()
{
    return m_ciftiFiberTrajectoryManager;
}

/**
 * @return The CIFTI Fiber Trajectory Manager (const method)
 */
const CiftiFiberTrajectoryManager*
SessionManager::getCiftiFiberTrajectoryManager() const
{
    return m_ciftiFiberTrajectoryManager;
}

/**
 * @return Image capture settings for image capture dialog.
 */
ImageCaptureSettings*
SessionManager::getImageCaptureDialogSettings()
{
    return m_imageCaptureDialogSettings;
}

/**
 * @return Image capture settings for image capture dialog (const method)
 */
const ImageCaptureSettings*
SessionManager::getImageCaptureDialogSettings() const
{
    return m_imageCaptureDialogSettings;
}


