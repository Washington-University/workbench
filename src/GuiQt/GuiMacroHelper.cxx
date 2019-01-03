
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

#define __GUI_MACRO_HELPER_DECLARE__
#include "GuiMacroHelper.h"
#undef __GUI_MACRO_HELPER_DECLARE__

#include "Brain.h"
#include "CaretAssert.h"
#include "CaretPreferences.h"
#include "EventManager.h"
#include "EventSceneActive.h"
#include "EventUserInterfaceUpdate.h"
#include "GuiManager.h"
#include "Scene.h"
#include "SceneFile.h"
#include "SceneInfo.h"
#include "SessionManager.h"
#include "WuQMacroGroup.h"

using namespace caret;


    
/**
 * \class caret::GuiMacroHelper 
 * \brief Implementation of WuQMacroHelperInterface that provides macro groups
 * \ingroup GuiQt
 */

/**
 * Constructor.
 */
GuiMacroHelper::GuiMacroHelper(QObject* parent)
: WuQMacroHelperInterface(parent)
{
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_USER_INTERFACE_UPDATE);
}

/**
 * Destructor.
 */
GuiMacroHelper::~GuiMacroHelper()
{
    EventManager::get()->removeAllEventsFromListener(this);
}

/**
 * Receive an event
 *
 * @param event
 *     The event.
 */
void
GuiMacroHelper::receiveEvent(Event* event)
{
    CaretAssert(event);
    if (event->getEventType() == EventTypeEnum::EVENT_USER_INTERFACE_UPDATE) {
        event->setEventProcessed();
        emit requestDialogsUpdate();
    }
}

/**
 * @return All available macro groups
 */
std::vector<WuQMacroGroup*>
GuiMacroHelper::getMacroGroups()
{
    std::vector<WuQMacroGroup*> macroGroups;
    
    CaretPreferences* preferences = SessionManager::get()->getCaretPreferences();
    CaretAssert(preferences);
    macroGroups.push_back(preferences->getMacros());
    
    EventSceneActive activeSceneEvent(EventSceneActive::MODE_GET);
    EventManager::get()->sendEvent(activeSceneEvent.getPointer());
    if ( ! activeSceneEvent.isError()) {
        Scene* activeScene = activeSceneEvent.getScene();
        if (activeScene != NULL) {
            macroGroups.push_back(activeScene->getSceneInfo()->getMacroGroup());
        }
    }
    
    return macroGroups;
}

/**
 * Is called when the given macro is modified
 *
 * @param macro
 *     Macro that is modified
 */
void
GuiMacroHelper::macroWasModified(WuQMacro* macro)
{
    /*
     * Need to write to preferences if macro is from preferences
     */
    CaretPreferences* preferences = SessionManager::get()->getCaretPreferences();
    CaretAssert(preferences);
    WuQMacroGroup* prefMacroGroup = preferences->getMacros();
    if (prefMacroGroup->containsMacro(macro)) {
        preferences->writeMacros();
    }
    
    EventManager::get()->sendEvent(EventUserInterfaceUpdate().getPointer());
}

/**
 * Is called when the given macro group is modified
 *
 * @param macroGroup
 *     Macro Group that is modified
 */
void
GuiMacroHelper::macroGroupWasModified(WuQMacroGroup* macroGroup)
{
    /**
     * Need to write to preferences if macro group is from preferences
     */
    CaretPreferences* preferences = SessionManager::get()->getCaretPreferences();
    CaretAssert(preferences);
    if (macroGroup == preferences->getMacros()) {
        preferences->writeMacros();
    }
    
    EventManager::get()->sendEvent(EventUserInterfaceUpdate().getPointer());
}
