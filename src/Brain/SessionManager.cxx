
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

#include "AnnotationBrowserTab.h"
#include "AnnotationManager.h"
#include "ApplicationInformation.h"
#include "BackgroundAndForegroundColorsSceneHelper.h"
#include "Brain.h"
#include "BrowserTabContent.h"
#include "BrowserWindowContent.h"
#include "CaretAssert.h"
#include "CaretLogger.h"
#include "CaretPreferenceDataValue.h"
#include "CaretPreferences.h"
#include "CiftiConnectivityMatrixDataFileManager.h"
#include "CiftiFiberTrajectoryManager.h"
#include "DataToolTipsManager.h"
#include "ElapsedTimer.h"
#include "EventManager.h"
#include "EventBrowserTabClose.h"
#include "EventBrowserTabDelete.h"
#include "EventBrowserTabGet.h"
#include "EventBrowserTabGetAll.h"
#include "EventBrowserTabIndicesGetAll.h"
#include "EventBrowserTabNew.h"
#include "EventBrowserTabNewClone.h"
#include "EventBrowserTabReopenAvailable.h"
#include "EventBrowserTabReopenClosed.h"
#include "EventBrowserWindowContent.h"
#include "EventCaretPreferencesGet.h"
#include "EventModelAdd.h"
#include "EventModelDelete.h"
#include "EventModelGetAll.h"
#include "EventModelGetAllDisplayed.h"
#include "EventProgressUpdate.h"
#include "EventSpacerTabGet.h"
#include "ImageCaptureSettings.h"
#include "LogManager.h"
#include "MapYokingGroupEnum.h"
#include "ModelWholeBrain.h"
#include "MovieRecorder.h"
#include "Scene.h"
#include "SceneAttributes.h"
#include "SceneClass.h"
#include "SceneClassArray.h"
#include "ScenePrimitiveArray.h"
#include "SpacerTabContent.h"
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
    m_dataToolTipsManager.reset(new DataToolTipsManager(m_caretPreferences->isShowDataToolTipsEnabled()));
    
    m_browserTabs.fill(NULL);
    
    for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_WINDOWS; i++) {
        CaretAssertStdArrayIndex(m_browserWindowContent, i);
        m_browserWindowContent[i] = new BrowserWindowContent(i);
    }
    
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_BROWSER_TAB_CLOSE);
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_BROWSER_TAB_DELETE);
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_BROWSER_TAB_GET);
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_BROWSER_TAB_GET_ALL);
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_BROWSER_TAB_INDICES_GET_ALL);
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_BROWSER_TAB_NEW);
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_BROWSER_TAB_NEW_CLONE);
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_BROWSER_TAB_REOPEN_AVAILBLE);
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_BROWSER_TAB_REOPEN_CLOSED);
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_BROWSER_WINDOW_CONTENT);
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_CARET_PREFERENCES_GET);
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_MODEL_ADD);
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_MODEL_DELETE);
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_MODEL_GET_ALL);
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_MODEL_GET_ALL_DISPLAYED);
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_SPACER_TAB_GET);
    
    Brain* brain = new Brain(m_caretPreferences);
    m_brains.push_back(brain);
    m_movieRecorder.reset(new MovieRecorder());
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
    deleteAllBrowserTabs();
    
    clearSpacerTabs();
    
    std::for_each(m_browserWindowContent.begin(),
                  m_browserWindowContent.end(),
                  [](BrowserWindowContent* bwc) { if (bwc != NULL) delete bwc; } );
    
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
 * Clear all of the spacer tabs
 */
void
SessionManager::clearSpacerTabs()
{
    for (auto st : m_spacerTabsMap) {
        delete st.second;
    }
    m_spacerTabsMap.clear();
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
 * Get the maximum stack order from all tabs
 */
int32_t
SessionManager::getMaximumManualTabStackOrder() const
{
    int32_t maxStackOrder(0);
    
    for (const auto bt : m_browserTabs) {
        if (bt != NULL) {
            maxStackOrder = std::max(bt->getManualLayoutBrowserTabAnnotation()->getStackingOrder(),
                                     maxStackOrder);
        }
    }
    
    return maxStackOrder;
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
        
        BrowserTabContent* newTab = createNewBrowserTab();
        if (newTab != NULL) {
            tabEvent->setBrowserTab(newTab);
        }
        else {
            tabEvent->setErrorMessage("Workbench is unable to create tabs, all tabs are in use.");
        }
    }
    else if (event->getEventType() == EventTypeEnum::EVENT_BROWSER_TAB_NEW_CLONE) {
        EventBrowserTabNewClone* cloneTabEvent = dynamic_cast<EventBrowserTabNewClone*>(event);
        CaretAssert(cloneTabEvent);
        
        cloneTabEvent->setEventProcessed();
        
        const int32_t cloneTabIndex = cloneTabEvent->getIndexOfBrowserTabThatWasCloned();
        if ((cloneTabIndex < 0)
            || (cloneTabIndex >= BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS)) {
            cloneTabEvent->setErrorMessage("Invalid tab for cloning index=" + AString::number(cloneTabIndex));
            return;
        }
        
        BrowserTabContent* newTab = createNewBrowserTab();
        if (newTab != NULL) {
            CaretAssertStdArrayIndex(m_browserTabs, cloneTabIndex);
            newTab->cloneBrowserTabContent(m_browserTabs[cloneTabIndex]);
            cloneTabEvent->setNewBrowserTab(newTab,
                                            newTab->getTabNumber());
        }
        else {
            cloneTabEvent->setErrorMessage("Workbench is unable to create tabs, all tabs are in use.");
        }
    }
    else if (event->getEventType() == EventTypeEnum::EVENT_BROWSER_TAB_CLOSE) {
        EventBrowserTabClose* closeTabEvent =
        dynamic_cast<EventBrowserTabClose*>(event);
        CaretAssert(closeTabEvent);
        
        AString errorMessage;
        if ( ! closeBrowserTab(closeTabEvent,
                               errorMessage)) {
            closeTabEvent->setErrorMessage(errorMessage);
        }
        closeTabEvent->setEventProcessed();
    }
    else if (event->getEventType() == EventTypeEnum::EVENT_BROWSER_TAB_DELETE) {
        EventBrowserTabDelete* tabEvent =
        dynamic_cast<EventBrowserTabDelete*>(event);
        CaretAssert(tabEvent);
        
        AString errorMessage;
        if ( ! deleteBrowserTab(tabEvent,
                                errorMessage)) {
            tabEvent->setErrorMessage(errorMessage);
        }
        tabEvent->setEventProcessed();
    }
    else if (event->getEventType() == EventTypeEnum::EVENT_BROWSER_TAB_GET) {
        EventBrowserTabGet* tabEvent =
        dynamic_cast<EventBrowserTabGet*>(event);
        CaretAssert(tabEvent);
        
        tabEvent->setEventProcessed();
        
        const int32_t tabNumber = tabEvent->getTabNumber();
        CaretAssertStdArrayIndex(m_browserTabs, tabNumber);
        tabEvent->setBrowserTab(m_browserTabs[tabNumber]);
    }
    else if (event->getEventType() == EventTypeEnum::EVENT_BROWSER_TAB_GET_ALL) {
        EventBrowserTabGetAll* tabEvent =
        dynamic_cast<EventBrowserTabGetAll*>(event);
        CaretAssert(tabEvent);
        
        tabEvent->setEventProcessed();
        
        std::vector<BrowserTabContent*> activeTabs = getActiveBrowserTabs();
        for (auto bt : activeTabs) {
            tabEvent->addBrowserTab(bt);
        }
    }
    else if (event->getEventType() == EventTypeEnum::EVENT_BROWSER_TAB_INDICES_GET_ALL) {
        EventBrowserTabIndicesGetAll* tabEvent =
        dynamic_cast<EventBrowserTabIndicesGetAll*>(event);
        CaretAssert(tabEvent);
        
        tabEvent->setEventProcessed();
        
        std::vector<BrowserTabContent*> activeTabs = getActiveBrowserTabs();
        for (auto bt : activeTabs) {
            tabEvent->addBrowserTabIndex(bt->getTabNumber());
        }
    }
    else if (event->getEventType() == EventTypeEnum::EVENT_BROWSER_TAB_REOPEN_AVAILBLE) {
        EventBrowserTabReopenAvailable* tabEvent = dynamic_cast<EventBrowserTabReopenAvailable*>(event);
        CaretAssert(tabEvent);
        if ( ! m_closedBrowserTabs.empty()) {
            BrowserTabContent* tab = m_closedBrowserTabs.front();
            CaretAssert(tab);
            tabEvent->setTabIndexAndName(tab->getTabNumber(),
                                         tab->getTabName());
        }
        tabEvent->setEventProcessed();
    }
    else if (event->getEventType() == EventTypeEnum::EVENT_BROWSER_TAB_REOPEN_CLOSED) {
        EventBrowserTabReopenClosed* lastTabEvent = dynamic_cast<EventBrowserTabReopenClosed*>(event);
        CaretAssert(lastTabEvent);
        
        AString errorMessage;
        BrowserTabContent* tab = reopenLastClosedTab(errorMessage);
        if (tab != NULL) {
            lastTabEvent->setBrowserTabContent(tab,
                                               tab->getTabNumber());
        }
        else {
            lastTabEvent->setErrorMessage(errorMessage);
        }
        lastTabEvent->setEventProcessed();
    }
    else if (event->getEventType() == EventTypeEnum::EVENT_BROWSER_WINDOW_CONTENT) {
        EventBrowserWindowContent* windowEvent =
        dynamic_cast<EventBrowserWindowContent*>(event);
        CaretAssert(windowEvent);
        
        windowEvent->setEventProcessed();
        
        const int32_t windowIndex = windowEvent->getBrowserWindowIndex();
        CaretAssertStdArrayIndex(m_browserWindowContent, windowIndex);
        
        CaretAssert(m_browserWindowContent[windowIndex]);
        
        switch (windowEvent->getMode()) {
            case EventBrowserWindowContent::Mode::DELETER:
                m_browserWindowContent[windowIndex]->setValid(false);
                break;
            case EventBrowserWindowContent::Mode::GET:
                windowEvent->setBrowserWindowContent(m_browserWindowContent[windowIndex]);
                break;
            case EventBrowserWindowContent::Mode::NEW:
                m_browserWindowContent[windowIndex]->setValid(true);
                windowEvent->setBrowserWindowContent(m_browserWindowContent[windowIndex]);
                break;
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
    else if (event->getEventType() == EventTypeEnum::EVENT_MODEL_GET_ALL_DISPLAYED) {
        EventModelGetAllDisplayed* getDisplayedModelsEvent =
        dynamic_cast<EventModelGetAllDisplayed*>(event);
        CaretAssert(getDisplayedModelsEvent);
        
        std::vector<BrowserTabContent*> activeTabs = getActiveBrowserTabs();
        for (auto bt : activeTabs) {
            getDisplayedModelsEvent->addModel(bt->getModelForDisplay());
        }
        
        getDisplayedModelsEvent->setEventProcessed();
    }
    else if (event->getEventType() == EventTypeEnum::EVENT_SPACER_TAB_GET) {
        EventSpacerTabGet* spacerTabEvent = dynamic_cast<EventSpacerTabGet*>(event);
        CaretAssert(spacerTabEvent);
        
        SpacerTabContent* spacerTabContent = NULL;
        
        SpacerTabIndex spacerTabIndex(spacerTabEvent->getWindowIndex(),
                                      spacerTabEvent->getRowIndex(),
                                      spacerTabEvent->getColumnIndex());
        auto iter = m_spacerTabsMap.find(spacerTabIndex);
        if (iter != m_spacerTabsMap.end()) {
            spacerTabContent = iter->second;
            CaretLogFiner("Found Spacer Tab Content: "
                          + spacerTabContent->getTabName());
        }
        else {
            spacerTabContent = new SpacerTabContent(spacerTabEvent->getWindowIndex(),
                                                    spacerTabEvent->getRowIndex(),
                                                    spacerTabEvent->getColumnIndex());
            m_spacerTabsMap.insert(std::make_pair(spacerTabIndex,
                                                  spacerTabContent));
            CaretLogFiner("Created Spacer Tab Content: "
                          + spacerTabContent->getTabName());
        }
        
        CaretAssert(spacerTabContent);
        spacerTabEvent->setSpacerTabContent(spacerTabContent);
        spacerTabEvent->setEventProcessed();
    }
}

/**
 * Update all of the browser tab contents since the models have changed.
 */
void 
SessionManager::updateBrowserTabContents()
{
    std::vector<BrowserTabContent*> activeTabs = getActiveBrowserTabs();
    for (auto bt : activeTabs) {
        bt->update(m_models);
    }
    
    /*
     * Update the closed tabs so that they do not
     * reference and data that is no longer valid
     */
    for (auto bt : m_closedBrowserTabs) {
        bt->update(m_models);
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
    std::vector<BrowserTabContent*> validTabs = getActiveBrowserTabs();
    for (auto tab : validTabs) {
        if (std::find(tabIndicesForScene.begin(),
                      tabIndicesForScene.end(),
                      tab->getTabNumber()) != tabIndicesForScene.end()) {
            browserTabSceneClasses.push_back(tab->saveToScene(sceneAttributes,
                                                              "m_browserTabs"));
        }
    }
    sceneClass->addChild(new SceneClassArray("m_browserTabs",
                                             browserTabSceneClasses));
    
    /*
     * Save browser windows
     */
    SceneObjectMapIntegerKey* browserClassMap = new SceneObjectMapIntegerKey("browserWindowContentMap",
                                                                             SceneObjectDataTypeEnum::SCENE_CLASS);
    for (auto bw : m_browserWindowContent) {
        if (bw->isValid()) {
            browserClassMap->addClass(bw->getWindowIndex(),
                                      bw->saveToScene(sceneAttributes, "m_browserWindowContent"));
        }
    }
    sceneClass->addChild(browserClassMap);
    
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
    
    sceneClass->addClass(savePreferencesToScene(sceneAttributes,
                                                "ScenePreferenceDataValues"));

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
    m_caretPreferences->invalidateSceneDataValues();

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
            Brain* brain = new Brain(m_caretPreferences);
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
    deleteAllBrowserTabs();
    
    /*
     * Remove all spacer tabs
     */
    clearSpacerTabs();
    
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
        CaretAssertStdArrayIndex(m_browserTabs, tabIndex);
        m_browserTabs[tabIndex] = tab;
    }
    
    /*
     * Restore windows
     */
    const SceneObjectMapIntegerKey* browserClassMap = sceneClass->getMapIntegerKey("browserWindowContentMap");
    if (browserClassMap != NULL) {
        /*
         * m_browserWindowContent was added in Feb 2018 at which time the
         * aspect locking buttons for window and tab were consolidated into
         * one button.
         */
        const std::vector<int32_t> windowIndices = browserClassMap->getKeys();
        for (const auto windowIndex : windowIndices) {
            const SceneClass* windowClass = browserClassMap->classValue(windowIndex);
            CaretAssert(windowClass);
            CaretAssertStdArrayIndex(m_browserWindowContent, windowIndex);
            m_browserWindowContent[windowIndex]->restoreFromScene(sceneAttributes,
                                                                  windowClass);
            m_browserWindowContent[windowIndex]->setValid(true);
        }
    }
    else {
        /*
         * For scenes before Feb 2018, need to restore from the GUI's browser window
         * to the browser window content.
         */
        const Scene* scene = sceneAttributes->getScene();
        CaretAssert(scene);
        
        const SceneClass* guiManagerClass = scene->getClassWithName("guiManager");
        if (guiManagerClass->getName() != "guiManager") {
            sceneAttributes->addToErrorMessage("Top level scene class should be guiManager but it is: "
                                               + guiManagerClass->getName());
            return;
        }
        
        const SceneClassArray* browserWindowArray = guiManagerClass->getClassArray("m_brainBrowserWindows");
        if (browserWindowArray != NULL) {
            const int32_t numBrowserClasses = browserWindowArray->getNumberOfArrayElements();
            for (int32_t i = 0; i < numBrowserClasses; i++) {
                const SceneClass* browserClass = browserWindowArray->getClassAtIndex(i);
                CaretAssert(browserClass);
                int32_t windowIndex = browserClass->getIntegerValue("m_browserWindowIndex", -1);
                if (windowIndex < 0) {
                    windowIndex = i;
                }
                CaretAssert(windowIndex >= 0);
                CaretAssertStdArrayIndex(m_browserWindowContent, windowIndex);
                m_browserWindowContent[windowIndex]->restoreFromOldBrainBrowserWindowScene(sceneAttributes,
                                                                                           browserClass);
                m_browserWindowContent[windowIndex]->setValid(true);
            }
        }
    }
    
    const int32_t numValidBrowserWindows = std::count_if(m_browserWindowContent.begin(),
                                                         m_browserWindowContent.end(),
                                                         [](BrowserWindowContent* bwc) { return bwc->isValid(); });
    if (numValidBrowserWindows <= 0) {
        sceneAttributes->addToErrorMessage("Scene error, no browser window content was restored");
        return;
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
    
    restorePreferencesFromScene(sceneAttributes,
                                sceneClass->getClass("ScenePreferenceDataValues"));
                                
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
 * Save items in preferences to the scene.
 *
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    saving the scene.
 *
 * @param instanceName
 *    Name for the scene class
 *
 * @return Pointer to scene class containing preferences
 */
SceneClass*
SessionManager::savePreferencesToScene(const SceneAttributes* /*sceneAttributes*/,
                                       const AString& instanceName)
{
    SceneClass* sceneClass = new SceneClass(instanceName,
                                            "ScenePreferences",
                                            1);
    std::vector<CaretPreferenceDataValue*> sceneDataValues = m_caretPreferences->getPreferenceSceneDataValues();
    for (auto scv : sceneDataValues) {
        if (scv->isSavedToScenes()) {
            sceneClass->addString(scv->getName(),
                                  scv->getPreferenceValue().toString());
        }
    }
    
    return sceneClass;
}

/**
 * Restore items in preferences from the scene
 *
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    restoring the scene.
 *
 * @param sceneClass
 *     sceneClass containing the preference items.
 */
void
SessionManager::restorePreferencesFromScene(const SceneAttributes* /*sceneAttributes*/,
                                            const SceneClass* sceneClass)
{
    if (sceneClass == NULL) {
        return;
    }
    
    const QString invalidValueName("InVaLiDvAlUe");
    m_caretPreferences->invalidateSceneDataValues();
    
    std::vector<CaretPreferenceDataValue*> sceneDataValues = m_caretPreferences->getPreferenceSceneDataValues();
    for (auto scv : sceneDataValues) {
        if (scv->isSavedToScenes()) {
            const QString name = scv->getName();
            const QString value = sceneClass->getStringValue(scv->getName(),
                                                             invalidValueName);
            if (value != invalidValueName) {
                scv->setSceneValue(QVariant(value));
            }
        }
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

    clearSpacerTabs();
    
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
 * @return The data tool tips manager
 */
DataToolTipsManager*
SessionManager::getDataToolTipsManager()
{
    return m_dataToolTipsManager.get();
}

/**
 * @return The data tool tips manager (const method)
 */
const DataToolTipsManager*
SessionManager::getDataToolTipsManager() const
{
    return m_dataToolTipsManager.get();
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

/**
 * @return The movie recorder
 */
MovieRecorder*
SessionManager::getMovieRecorder()
{
    return m_movieRecorder.get();
}

/**
 * @return The movie recorder (const method)
 */
const MovieRecorder*
SessionManager::getMovieRecorder() const
{
    return m_movieRecorder.get();
}

/**
 * @return Active browser tabs in a vector
 */
std::vector<BrowserTabContent*>
SessionManager::getActiveBrowserTabs()
{
    std::vector<BrowserTabContent*> tabs;
    
    for (auto bt: m_browserTabs) {
        if (bt != NULL) {
            tabs.push_back(bt);
        }
    }
    
    return tabs;
}

/**
 * @return a new browser tab or NULL if a tab cannot be created
 */
BrowserTabContent*
SessionManager::createNewBrowserTab()
{
    BrowserTabContent* newTab(NULL);
    
    /*
     * Try to find an unused tab index
     */
    for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; i++) {
        CaretAssertStdArrayIndex(m_browserTabs, i);
        if (m_browserTabs[i] == NULL) {
            if ( ! isTabInClosedBrowserTabs(i)) {
                newTab = new BrowserTabContent(i);
                break;
            }
        }
    }
    
    /*
     * If no available tabs, remove the oldest tab in the closed tabs, delete the
     * closed tab, and use its index for a new tab
     */
    if (newTab == NULL) {
        if ( ! m_closedBrowserTabs.empty()) {
            BrowserTabContent* btc = m_closedBrowserTabs.back();
            CaretAssert(btc);
            const int32_t tabIndex = btc->getTabNumber();
            if (m_browserTabs[tabIndex] == NULL) {
                m_closedBrowserTabs.pop_back();
                delete btc;
                btc = NULL;
                
                newTab = new BrowserTabContent(tabIndex);
            }
            else {
                CaretLogSevere("Program Error: Trying to delete closed tab with index="
                               + AString::number(tabIndex)
                               + " but there is a currently open tab at that index");
            }
        }
    }
   
    if (newTab != NULL) {
        const int32_t tabIndex = newTab->getTabNumber();
        m_browserTabs[tabIndex] = newTab;
        newTab->update(m_models);
        newTab->getManualLayoutBrowserTabAnnotation()->setStackingOrder(getMaximumManualTabStackOrder() + 1);
        
        std::vector<BrowserTabContent*> tabs = getActiveBrowserTabs();
        std::vector<AnnotationBrowserTab*> anns;
        for (auto t : tabs) {
            AnnotationBrowserTab* abt = t->getManualLayoutBrowserTabAnnotation();
            CaretAssert(abt);
            anns.push_back(abt);
        }
        
        CaretAssert(m_brains[0]);
        AnnotationManager* annMan = m_brains[0]->getAnnotationManager();
        AString errorMessage;
        const bool resultFlag = annMan->moveTabOrWindowAnnotationToFront(newTab->getManualLayoutBrowserTabAnnotation(),
                                                                         errorMessage);
        if ( ! resultFlag) {
            CaretLogWarning("Moving to front after creation of new tab error: "
                            + errorMessage);
        }
    }
    
    return newTab;
}

/**
 * Close the given tab in the given window that may be reopened later
 *
 * @param closeTabEvent
 *    The delete tab event
 * @param errorMessageOut
 *    Contains error information
 * @return True if tab was closed, else false
 */
bool
SessionManager::closeBrowserTab(EventBrowserTabClose* closeTabEvent,
                                 AString& errorMessageOut)
{
    BrowserTabContent* tab = closeTabEvent->getBrowserTab();
    const int32_t windowIndex = closeTabEvent->getWindowIndex();
    CaretAssert(tab);
    CaretAssertStdArrayIndex(m_browserWindowContent, windowIndex);
    
    const int32_t tabIndex = tab->getTabNumber();
    if ((tabIndex < 0)
        || (tabIndex >= BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS)) {
        errorMessageOut = ("Tab for closing has invalid tabIndex="
                           + AString::number(tabIndex));
        return false;
    }
    
    CaretAssertStdArrayIndex(m_browserTabs, tabIndex);
    if (tab != m_browserTabs[tabIndex]) {
        errorMessageOut = ("Tab for closing with tabIndex="
                           + AString::number(tabIndex)
                           + " is not at that index in array of tabs.");
        return false;
    }
    
    tab->setClosedStatusFromSessionManager(true);
    tab->setClosedTabWindowTabBarPositionIndex(closeTabEvent->getWindowTabBarPositionIndex());
    tab->setClosedTabWindowIndex(windowIndex);
    m_closedBrowserTabs.push_front(tab);
    m_browserTabs[tabIndex] = NULL;
    
    return true;
}

/**
 * Delete the given tab in the given window
 *
 * @param deleteTabEvent
 *    The delete tab event
 * @param errorMessageOut
 *    Contains error information
 * @return True if tab was closed, else false
 */
bool
SessionManager::deleteBrowserTab(EventBrowserTabDelete* deleteTabEvent,
                                AString& errorMessageOut)
{
    BrowserTabContent* tab = deleteTabEvent->getBrowserTab();
    CaretAssert(tab);
    
    const int32_t tabIndex = tab->getTabNumber();
    if ((tabIndex < 0)
        || (tabIndex >= BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS)) {
        errorMessageOut = ("Tab for deleting has invalid tabIndex="
                           + AString::number(tabIndex));
        return false;
    }
    
    CaretAssertStdArrayIndex(m_browserTabs, tabIndex);
    if (tab != m_browserTabs[tabIndex]) {
        errorMessageOut = ("Tab for deleting with tabIndex="
                           + AString::number(tabIndex)
                           + " is not at that index in array of tabs.");
        CaretAssertMessage(0, errorMessageOut);
        return false;
    }

    delete m_browserTabs[tabIndex];
    m_browserTabs[tabIndex] = NULL;
    
    return true;
}

/**
 * Reopen that last tab closed in the  given window
 *
 * @param windowIndex
 *    Index of window containing the tab
 * @param errorMessageOut
 *    Contains error information
 * @return Tab that was reopened or NULL if no tab was reopened
 */
BrowserTabContent*
SessionManager::reopenLastClosedTab(AString& errorMessageOut)
{
    if ( ! m_closedBrowserTabs.empty()) {
        BrowserTabContent* btc = m_closedBrowserTabs.front();
        CaretAssert(btc);
        const int32_t tabIndex = btc->getTabNumber();
        if (m_browserTabs[tabIndex] == NULL) {
            m_browserTabs[tabIndex] = btc;
            m_closedBrowserTabs.pop_front();
            btc->update(m_models);
            btc->setClosedStatusFromSessionManager(false);
            return btc;
        }
        else {
            errorMessageOut = ("Program Error: Trying to reopen tab with index="
                               + AString::number(tabIndex)
                               + " but there is a currently open tab at that index");
        }
    }
    
    errorMessageOut = "No tabs are available for reopening";
    
    return NULL;
}

/**
 * Delete all browser tabs both active and recently closed
 */
void
SessionManager::deleteAllBrowserTabs()
{
    for (auto bt : m_browserTabs) {
        if (bt != NULL) {
            delete bt;
        }
    }
    m_browserTabs.fill(NULL);
    
    for (auto bt : m_closedBrowserTabs) {
        delete bt;
    }
    m_closedBrowserTabs.clear();
}

/**
 * @return True if a tab with the given index in the closed browser tabs
 * @param tabIndex
 *    Index of the browser tab
 */
bool
SessionManager::isTabInClosedBrowserTabs(const int32_t tabIndex)
{
    for (auto bt : m_closedBrowserTabs) {
        CaretAssert(bt);
        if (tabIndex == bt->getTabNumber()) {
            return true;
        }
    }
    return false;
}

