
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
#include "EventBrowserTabNew.h"
#include "EventModelAdd.h"
#include "EventModelDelete.h"
#include "EventModelGetAll.h"
#include "EventProgressUpdate.h"
#include "LogManager.h"
#include "ModelWholeBrain.h"
#include "SceneAttributes.h"
#include "SceneClass.h"
#include "SceneClassArray.h"
#include "OverlayYokingGroupEnum.h"
#include "VolumeSurfaceOutlineSetModel.h"


using namespace caret;


/**
 * Constructor.
 */
SessionManager::SessionManager()
: CaretObject(), EventListenerInterface(), SceneableInterface()
{
    m_caretPreferences = new CaretPreferences();
    
    m_ciftiConnectivityMatrixDataFileManager = new CiftiConnectivityMatrixDataFileManager();
    m_ciftiFiberTrajectoryManager = new CiftiFiberTrajectoryManager();
    
    for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; i++) {
        m_browserTabs[i] = NULL;
    }
    
    
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_BROWSER_TAB_DELETE);
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_BROWSER_TAB_GET);
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_BROWSER_TAB_GET_ALL);
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_BROWSER_TAB_NEW);
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
    
    delete m_caretPreferences;
}

/**
 * Create the session manager.
 * This must be called one AND ONLY one time prior to any
 * other Caret mechanisms.
 */
void 
SessionManager::createSessionManager()
{
    CaretAssertMessage((s_singletonSessionManager == NULL), 
                       "Session manager has already been created.");

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
SessionManager::getBrain(const int32_t brainIndex)
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
    
    /*
     * Save overlay yoking groups.
     */
    std::vector<OverlayYokingGroupEnum::Enum> overlayYokingGroups;
    OverlayYokingGroupEnum::getAllEnums(overlayYokingGroups);
    const int32_t numOverlayYokingGroups = static_cast<int32_t>(overlayYokingGroups.size());
    if (numOverlayYokingGroups > 0) {
        std::vector<int32_t> overlayMapSelections(numOverlayYokingGroups, 1);
        for (int32_t i = 0; i < numOverlayYokingGroups; i++) {
            const OverlayYokingGroupEnum::Enum enumValue = overlayYokingGroups[i];
            if (enumValue != OverlayYokingGroupEnum::OVERLAY_YOKING_GROUP_OFF) {
                const int32_t groupIndex = OverlayYokingGroupEnum::toIntegerCode(enumValue);
                const int32_t mapIndex   = OverlayYokingGroupEnum::getSelectedMapIndex(enumValue);
                overlayMapSelections[groupIndex] = mapIndex;
            }
        }
        
        sceneClass->addIntegerArray("OverlayYokingGroupArray",
                                    &overlayMapSelections[0],
                                    numOverlayYokingGroups);
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
    
    /*
     * Restore overlay yoking groups
     */
    std::vector<OverlayYokingGroupEnum::Enum> overlayYokingGroups;
    OverlayYokingGroupEnum::getAllEnums(overlayYokingGroups);
    const int32_t numOverlayYokingGroups = static_cast<int32_t>(overlayYokingGroups.size());
    if (numOverlayYokingGroups > 0) {
        std::vector<int32_t> overlayMapSelections(numOverlayYokingGroups, 1);
        
        sceneClass->getIntegerArrayValue("OverlayYokingGroupArray",
                                         &overlayMapSelections[0],
                                         numOverlayYokingGroups,
                                         1);
        
        for (int32_t i = 0; i < numOverlayYokingGroups; i++) {
            bool isValid = false;
            const OverlayYokingGroupEnum::Enum enumValue = OverlayYokingGroupEnum::fromIntegerCode(i,
                                                                                                   &isValid);
            if (enumValue != OverlayYokingGroupEnum::OVERLAY_YOKING_GROUP_OFF) {
                if (isValid) {
                    OverlayYokingGroupEnum::setSelectedMapIndex(enumValue, overlayMapSelections[i]);
                }
                else {
                    OverlayYokingGroupEnum::setSelectedMapIndex(enumValue, 1);
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
    
    CaretLogInfo("Time to restore brain was "
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
    
    CaretLogInfo("Time to restore browser tab content was "
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



