
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

#define __WB_MACRO_HELPER_DECLARE__
#include "WbMacroHelper.h"
#undef __WB_MACRO_HELPER_DECLARE__

#include "Brain.h"
#include "BrainBrowserWindow.h"
#include "CaretAssert.h"
#include "CaretLogger.h"
#include "CaretPreferences.h"
#include "EventManager.h"
#include "EventSceneActive.h"
#include "EventUserInterfaceUpdate.h"
#include "GuiManager.h"
#include "MovieRecorder.h"
#include "Scene.h"
#include "SceneFile.h"
#include "SceneInfo.h"
#include "SessionManager.h"
#include "WbMacroCustomOperationTypeEnum.h"
#include "WbMacroCustomDataTypeEnum.h"
#include "WuQMacroCommand.h"
#include "WuQMacroCommandParameter.h"
#include "WuQMacroExecutorOptions.h"
#include "WuQMacroGroup.h"
#include "WuQMessageBox.h"

using namespace caret;


    
/**
 * \class caret::WbMacroHelper
 * \brief Implementation of WuQMacroHelperInterface that provides macro groups
 * \ingroup GuiQt
 */

/**
 * Constructor.
 */
WbMacroHelper::WbMacroHelper(QObject* parent)
: WuQMacroHelperInterface(parent)
{
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_USER_INTERFACE_UPDATE);
}

/**
 * Destructor.
 */
WbMacroHelper::~WbMacroHelper()
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
WbMacroHelper::receiveEvent(Event* event)
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
WbMacroHelper::getMacroGroups()
{
    std::vector<WuQMacroGroup*> macroGroups;
    
    const bool includePreferencesFlag(false);
    if (includePreferencesFlag) {
        CaretPreferences* preferences = SessionManager::get()->getCaretPreferences();
        CaretAssert(preferences);
        macroGroups.push_back(preferences->getMacros());
    }
    
    EventSceneActive activeSceneEvent(EventSceneActive::MODE_GET);
    EventManager::get()->sendEvent(activeSceneEvent.getPointer());
    if ( ! activeSceneEvent.isError()) {
        Scene* activeScene = activeSceneEvent.getScene();
        if (activeScene != NULL) {
            macroGroups.push_back(activeScene->getMacroGroup());
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
WbMacroHelper::macroWasModified(WuQMacro* macro)
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
WbMacroHelper::macroGroupWasModified(WuQMacroGroup* macroGroup)
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

/**
 * @return Identifiers of all available windows in which macros may be run
 */
std::vector<QString>
WbMacroHelper::getMainWindowIdentifiers()
{
    std::vector<QString> identifiers;
    
    std::vector<BrainBrowserWindow*> windows = GuiManager::get()->getAllOpenBrainBrowserWindows();
    for (auto w : windows) {
        identifiers.push_back(QString::number(w->getBrowserWindowIndex() + 1));
    }
    
    return identifiers;
}

/**
 * Get the main window with the given identifier
 *
 * @param identifier
 *     Window identifier
 * @return
 *     Window with the given identifier or NULL if not available
 */
QMainWindow*
WbMacroHelper::getMainWindowWithIdentifier(const QString& identifier)
{
    const int32_t windowIndex(identifier.toInt() - 1);
    QMainWindow* window = GuiManager::get()->getBrowserWindowByWindowIndex(windowIndex);
    
    return window;
}

/**
 * Called just before executing the macro
 *
 * @param macro
 *    Macro that is run
 * @param window
 *     Widget for parent
 * @param executorOptions
 *    Executor options
 */
void
WbMacroHelper::macroExecutionStarting(const WuQMacro* /*macro*/,
                                      QWidget* /*window*/,
                                      const WuQMacroExecutorOptions* executorOptions)
{
    MovieRecorder* movieRecorder = SessionManager::get()->getMovieRecorder();
    m_savedRecordingMode = movieRecorder->getRecordingMode();
    if (executorOptions->isRecordMovieDuringExecution()) {
        movieRecorder->setRecordingMode(MovieRecorderModeEnum::AUTOMATIC);
    }
    EventManager::get()->sendSimpleEvent(EventTypeEnum::EVENT_MOVIE_RECORDING_DIALOG_UPDATE);
}

/**
 * Called just after executing the macro
 *
 * @param macro
 *    Macro that is run
 * @param window
 *     Widget for parent
 * @param executorOptions
 *    Executor options
 */
void
WbMacroHelper::macroExecutionEnding(const WuQMacro* /*macro*/,
                                    QWidget* /*window*/,
                                    const WuQMacroExecutorOptions* /*executorOptions*/)
{
    MovieRecorder* movieRecorder = SessionManager::get()->getMovieRecorder();
    movieRecorder->setRecordingMode(m_savedRecordingMode);
    EventManager::get()->sendSimpleEvent(EventTypeEnum::EVENT_MOVIE_RECORDING_DIALOG_UPDATE);
}
