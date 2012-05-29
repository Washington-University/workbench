
/*LICENSE_START*/
/* 
 *  Copyright 1995-2011 Washington University School of Medicine 
 * 
 *  http://brainmap.wustl.edu 
 * 
 *  This file is part of CARET. 
 * 
 *  CARET is free software; you can redistribute it and/or modify 
 *  it under the terms of the GNU General Public License as published by 
 *  the Free Software Foundation; either version 2 of the License, or 
 *  (at your option) any later version. 
 * 
 *  CARET is distributed in the hope that it will be useful, 
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of 
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
 *  GNU General Public License for more details. 
 * 
 *  You should have received a copy of the GNU General Public License 
 *  along with CARET; if not, write to the Free Software 
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA 
 * 
 */ 

#include <algorithm>

#define __SESSION_MANAGER_DECLARE__
#include "SessionManager.h"
#undef __SESSION_MANAGER_DECLARE__

#include "Brain.h"
#include "BrowserTabContent.h"
#include "CaretAssert.h"
#include "CaretPreferences.h"
#include "EventManager.h"
#include "EventBrowserTabDelete.h"
#include "EventBrowserTabGet.h"
#include "EventBrowserTabGetAll.h"
#include "EventBrowserTabNew.h"
#include "EventModelAdd.h"
#include "EventModelDelete.h"
#include "EventModelGetAll.h"
#include "EventModelYokingGroupGetAll.h"
#include "LogManager.h"
#include "ModelWholeBrain.h"
#include "ModelYokingGroup.h"

using namespace caret;


/**
 * Constructor.
 */
SessionManager::SessionManager()
: CaretObject(), EventListenerInterface()
{
    this->caretPreferences = new CaretPreferences();
    
    for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; i++) {
        this->browserTabs[i] = NULL;
    }
    
    
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_BROWSER_TAB_DELETE);
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_BROWSER_TAB_GET);
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_BROWSER_TAB_GET_ALL);
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_BROWSER_TAB_NEW);
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_MODEL_DISPLAY_CONTROLLER_ADD);
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_MODEL_DISPLAY_CONTROLLER_DELETE);
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_MODEL_DISPLAY_CONTROLLER_GET_ALL);
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_MODEL_DISPLAY_CONTROLLER_YOKING_GROUP_GET_ALL);
    
    Brain* brain = new Brain();
    this->brains.push_back(brain);    

    const int32_t numberOfYokingGroups = 4;
    for (int32_t i = 0; i < numberOfYokingGroups; i++) {
        const char letter = ('A' + (char)i);
        const AString yokingGroupName = AString(letter);
        ModelYokingGroup* myg = new ModelYokingGroup(brain,
                                                     yokingGroupName);
        for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; i++) {
            myg->dorsalView(i);
        }
        this->yokingGroupModels.push_back(myg);
    }
}

/**
 * Destructor.
 */
SessionManager::~SessionManager()
{
    int32_t numberOfBrains = this->getNumberOfBrains();
    for (int32_t i = (numberOfBrains - 1); i >= 0; i--) {
        delete this->brains[i];
    }
    this->brains.clear();
    
    EventManager::get()->removeAllEventsFromListener(this);
    
    const int32_t numYokingGroups = static_cast<int32_t>(this->yokingGroupModels.size());
    for (int32_t i = 0; i < numYokingGroups; i++) {
        delete this->yokingGroupModels[i];
        this->yokingGroupModels[i] = NULL;
    }
    
    delete this->caretPreferences;
}

/**
 * Create the session manager.
 * This must be called one AND ONLY one time prior to any
 * other Caret mechanisms.
 */
void 
SessionManager::createSessionManager()
{
    CaretAssertMessage((SessionManager::singletonSessionManager == NULL), 
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
    SessionManager::singletonSessionManager = new SessionManager();
}

/**
 * Delete the session manager.
 * This may only be called one time after session manager is created.
 */
void 
SessionManager::deleteSessionManager()
{
    CaretAssertMessage((SessionManager::singletonSessionManager != NULL), 
                       "Session manager does not exist, cannot delete it.");
    
    delete SessionManager::singletonSessionManager;
    SessionManager::singletonSessionManager = NULL;
    
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
    CaretAssertMessage(SessionManager::singletonSessionManager,
                       "Session manager was not created.\n"
                       "It must be created with SessionManager::createSessionManager().");
    
    return SessionManager::singletonSessionManager;
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
    return this->brains.size();
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
    CaretAssertVectorIndex(this->brains, brainIndex);
    
    return this->brains[brainIndex];
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
        
        for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; i++) {
            if (this->browserTabs[i] == NULL) {
                BrowserTabContent* tab = new BrowserTabContent(i, 
                                                               NULL);
                tab->update(this->modelDisplayControllers);
                this->browserTabs[i] = tab;
                tabEvent->setBrowserTab(tab);
                break;
            }
        }
    }
    else if (event->getEventType() == EventTypeEnum::EVENT_BROWSER_TAB_DELETE) {
        EventBrowserTabDelete* tabEvent =
        dynamic_cast<EventBrowserTabDelete*>(event);
        CaretAssert(tabEvent);
        
        
        BrowserTabContent* tab = tabEvent->getBrowserTab();
        
        for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; i++) {
            if (this->browserTabs[i] == tab) {
                delete this->browserTabs[i];
                this->browserTabs[i] = NULL;
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
        CaretAssertArrayIndex(this->browserTabs, BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS, tabNumber);
        
        tabEvent->setBrowserTab(this->browserTabs[tabNumber]);
    }
    else if (event->getEventType() == EventTypeEnum::EVENT_BROWSER_TAB_GET_ALL) {
        EventBrowserTabGetAll* tabEvent =
        dynamic_cast<EventBrowserTabGetAll*>(event);
        CaretAssert(tabEvent);
        
        tabEvent->setEventProcessed();
        
        for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; i++) {
            if (this->browserTabs[i] != NULL) {
                tabEvent->addBrowserTab(this->browserTabs[i]);
            }
        }
    }
    else if (event->getEventType() == EventTypeEnum::EVENT_MODEL_DISPLAY_CONTROLLER_ADD) {
        EventModelAdd* addModelsEvent =
        dynamic_cast<EventModelAdd*>(event);
        CaretAssert(addModelsEvent);
        
        addModelsEvent->setEventProcessed();
        
        this->modelDisplayControllers.push_back(addModelsEvent->getModel());

        this->updateBrowserTabContents();
    }
    else if (event->getEventType() == EventTypeEnum::EVENT_MODEL_DISPLAY_CONTROLLER_DELETE) {
        EventModelDelete* deleteModelsEvent =
        dynamic_cast<EventModelDelete*>(event);
        CaretAssert(deleteModelsEvent);
        
        deleteModelsEvent->setEventProcessed();
        
        Model* model = deleteModelsEvent->getModel();
        
        std::vector<Model*>::iterator iter =
        std::find(this->modelDisplayControllers.begin(),
                  this->modelDisplayControllers.end(),
                  model);
        
        CaretAssertMessage(iter != this->modelDisplayControllers.end(),
                           "Trying to delete non-existent model controller");
        
        this->modelDisplayControllers.erase(iter);
        
        this->updateBrowserTabContents();
    }
    else if (event->getEventType() == EventTypeEnum::EVENT_MODEL_DISPLAY_CONTROLLER_GET_ALL) {
        EventModelGetAll* getModelsEvent =
        dynamic_cast<EventModelGetAll*>(event);
        CaretAssert(getModelsEvent);
        
        getModelsEvent->setEventProcessed();
        
        getModelsEvent->addModels(this->modelDisplayControllers);
    }
    else if (event->getEventType() == EventTypeEnum::EVENT_MODEL_DISPLAY_CONTROLLER_YOKING_GROUP_GET_ALL) {
        EventModelYokingGroupGetAll* getYokingEvent =
            dynamic_cast<EventModelYokingGroupGetAll*>(event);
        CaretAssert(getYokingEvent);
        
        getYokingEvent->setEventProcessed();
        
        const int32_t numYokingGroups = static_cast<int32_t>(this->yokingGroupModels.size());
        for (int32_t i = 0; i < numYokingGroups; i++) {
            getYokingEvent->addYokingGroup(this->yokingGroupModels[i]);
        }
    }
}

/**
 * Update all of the browser tab contents since the models have changed.
 */
void 
SessionManager::updateBrowserTabContents()
{
    for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; i++) {
        if (this->browserTabs[i] != NULL) {
            this->browserTabs[i]->update(this->modelDisplayControllers);
        }
    }
}

/**
 * @return The caret preferences
 */
CaretPreferences* 
SessionManager::getCaretPreferences()
{
    return this->caretPreferences;
}


