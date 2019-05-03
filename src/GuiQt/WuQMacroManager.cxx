
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

#define __WU_Q_MACRO_MANAGER_DECLARE__
#include "WuQMacroManager.h"
#undef __WU_Q_MACRO_MANAGER_DECLARE__

#include <set>

#include <QAbstractButton>
#include <QAction>
#include <QApplication>
#include <QFileDialog>
#include <QKeyEvent>
#include <QMessageBox>
#include <QMouseEvent>
#include <QWidget>

#include "CaretAssert.h"
#include "CaretLogger.h"
#include "DataFileException.h"
#include "EventManager.h"
#include "EventUserInterfaceUpdate.h"
#include "WuQMacro.h"
#include "WuQMacroCommand.h"
#include "WuQMacroCreateDialog.h"
#include "WuQMacroCustomOperationManagerInterface.h"
#include "WuQMacroDialog.h"
#include "WuQMacroExecutor.h"
#include "WuQMacroExecutorMonitor.h"
#include "WuQMacroFile.h"
#include "WuQMacroGroup.h"
#include "WuQMacroMouseEventInfo.h"
#include "WuQMacroHelperInterface.h"
#include "WuQMacroExecutorOptions.h"
#include "WuQMacroSignalWatcher.h"
#include "WuQMacroWidgetAction.h"

using namespace caret;


/**
 * \class caret::WuQMacroManager 
 * \brief Manages the macro system.
 * \ingroup WuQMacro
 */

/**
 * Constructor.
 *
 * @param name
 *     Name for the macro manager.
 */
WuQMacroManager::WuQMacroManager(const QString& name,
                                 QObject* parent)
: QObject(parent),
m_name(name)
{
    setObjectName(name);
    
    m_macroExecutorMonitor = new WuQMacroExecutorMonitor(this);
    m_executorOptions.reset(new WuQMacroExecutorOptions());
}

/**
 * Destructor.
 */
WuQMacroManager::~WuQMacroManager()
{
    /*
     * Note: Do not delete the executor monitor as it is a QObject
     * with 'this' as a parent so Qt will take of of deleting it
     */
    
    /*
     * Do not delete the WuQMacroSignalWatcher instances in
     * m_signalWatchers.  This WuQMacroManager is set as the
     * parent object of the WuQMacroSignalWatcher's so Qt
     * will destroy them.
     */
    m_signalWatchers.clear();
    
    if (m_customCommandManager != NULL) {
        delete m_customCommandManager;
        m_customCommandManager = NULL;
    }
    
    /*
     * If an instance is being deleted it MUST be the
     * singleton so make it NULL.
     */
    if (s_singletonMacroManager != NULL) {
        s_singletonMacroManager = NULL;
    }
}

/**
 * @return The instance of the Macro Manager.  Before calling this method,
 * the createMacroManagerSingleton() must have been called to create the
 * singleton Macro Manager.  If the singleton is not valid this method
 * will cause the application to abort.
 */
WuQMacroManager*
WuQMacroManager::instance()
{
    if (s_singletonMacroManager == NULL) {
        /*
         * 'qApp' is macro in QApplication that points to the QApplication instance
         */
        s_singletonMacroManager = new WuQMacroManager("MacroManager",
                                                      qApp);
    }
    
    return s_singletonMacroManager;
}

/**
 * Set the macro helper that provides the macro groups
 *
 * @param macroHelper
 *     The macro helper.  Ownership will be taken of macro helper
 *     and it will be destoryed when this instance is destroyed.
 */
void
WuQMacroManager::setMacroHelper(WuQMacroHelperInterface* macroHelper)
{
    m_macroHelper = macroHelper;
    
    if (macroHelper != NULL) {
        QObject::connect(macroHelper, &WuQMacroHelperInterface::requestDialogsUpdate,
                         this, &WuQMacroManager::updateNonModalDialogs);
        
        m_macroWidgetActions = macroHelper->getMacroWidgetActions();
        
        for (auto mwa : m_macroWidgetActions) {
            addMacroSupportToObject(mwa,
                                    mwa->getToolTip());
        }
    }
}

/**
 * Set the custom command manager for editing custom command parameters and
 * running custom commands
 *
 * @param customCommandManager
 *    Pointer to custom command manager
 */
void
WuQMacroManager::setCustomCommandManager(WuQMacroCustomOperationManagerInterface* customCommandManager)
{
    m_customCommandManager = customCommandManager;
}

/**
 * @return Name of this macro manager
 */
QString
WuQMacroManager::getName() const
{
    return m_name;
}

/**
 * @return The current macro mode.
 */
WuQMacroModeEnum::Enum
WuQMacroManager::getMode() const
{
    return m_mode;
}

/**
 * Set the macro mode
 *
 * @param mode
 *     New mode
 */
void
WuQMacroManager::setMode(const WuQMacroModeEnum::Enum mode)
{
    m_mode = mode;
}

/**
 * Add macro support to the given object.  When recording,
 * The object's 'value changed' signal will be monitored so
 * that the new value can be part of a macro command.
 *
 * @param object
 *     Object that is monitored.
 * @param descriptiveName
 *     Descriptive name for user
 * @param toolTipTextOverride
 *     Override of object's tooltip.  This is primarily used when
 *     an object of a particular class does not support a tooltip
 *     such as a QButtonGroup.  This can also be empty to avoid
 *     the "no tooltip" message.
 */
bool
WuQMacroManager::addMacroSupportToObjectWithToolTip(QObject* object,
                                                    const QString& descriptiveName,
                                                    const QString& toolTipOverride)
{
    CaretAssert(object);
    
    const QString name = object->objectName();
    if (name.isEmpty()) {
        CaretLogSevere("Object name is empty, will be ignored for macros\n"
                       + SystemUtilities::getBackTrace());
        return false;
    }
    if (descriptiveName.isEmpty()) {
        CaretLogSevere("Descriptive name is empty for "
                       + name
                       + "\n"
                       + SystemUtilities::getBackTrace());
    }
    
    auto existingWatcher = m_signalWatchers.find(name);
    if (existingWatcher != m_signalWatchers.end()) {
        CaretLogSevere("Object named \""
                       + name
                       + "\" has already been connected for macros\n"
                       + SystemUtilities::getBackTrace()
                       + "\n");
        return false;
    }
    
    AString errorMessage;
    WuQMacroSignalWatcher* widgetWatcher = WuQMacroSignalWatcher::newInstance(this,
                                                                              object,
                                                                              descriptiveName,
                                                                              toolTipOverride,
                                                                              errorMessage);
    if (widgetWatcher != NULL) {
        widgetWatcher->setParent(this);
        m_signalWatchers.insert(std::make_pair(name,
                                               widgetWatcher));
        return true;
    }
    else {
        CaretLogWarning(errorMessage);
        return false;
    }
}

/**
 * Add macro support to the given object.  When recording,
 * The object's 'value changed' signal will be monitored so
 * that the new value can be part of a macro command.
 *
 * @param object
 *     Object that is monitored.
 * @param descriptiveName
 *     Descriptive name for user
 */
bool
WuQMacroManager::addMacroSupportToObject(QObject* object,
                                         const QString& descriptiveName)
{
    CaretAssert(object);
    
    QString toolTipText;
    QAction* action = qobject_cast<QAction*>(object);
    if (action != NULL) {
        toolTipText = action->toolTip();
    }
    QWidget* widget = qobject_cast<QWidget*>(object);
    if (widget != NULL) {
        toolTipText = widget->toolTip();
    }
    WuQMacroWidgetAction* macroWidgetAction = qobject_cast<WuQMacroWidgetAction*>(object);
    if (macroWidgetAction != NULL) {
        toolTipText = macroWidgetAction->getToolTip();
    }
    
    const bool resultFlag = addMacroSupportToObjectWithToolTip(object,
                                                               descriptiveName,
                                                               toolTipText);
    if (resultFlag) {
        
        if (toolTipText.isEmpty()) {
            CaretLogWarning("Object named \""
                            + object->objectName()
                            + "\" is missing a tooltip");
        }
    }
    
    return resultFlag;
}

/**
 * Adds the given macro comand to the macro that is currently being
 * recorded.  If no macro is being recorded, no action is taken.
 *
 * @param macroCommand
 *    Command to add to the current macro
 * @return
 *    True if recording is on and command was added to the macro.
 *    Ownership of command will be by the macro
 *    False if recording off in which case caller is responsible
 *    to delete the macro command
 */
bool
WuQMacroManager::addMacroCommandToRecording(WuQMacroCommand* macroCommand)
{
    CaretAssert(macroCommand);
    
    switch (getMode()) {
        case WuQMacroModeEnum::OFF:
            break;
        case WuQMacroModeEnum::RECORDING_INSERT_COMMANDS:
            CaretAssert(m_macroInsertCommandBeingRecorded);
            m_macroInsertCommandBeingRecorded->insertRow(m_macroInsertCommandBeingRecordedOffset,
                                                         macroCommand);
            m_macroInsertCommandBeingRecordedOffset++;
            return true;
            break;
        case WuQMacroModeEnum::RECORDING_NEW_MACRO:
            CaretAssert(m_macroBeingRecorded);
            m_macroBeingRecorded->appendMacroCommand(macroCommand);
            return true;
            break;
        case WuQMacroModeEnum::RUNNING:
            break;
    }
    
    return false;
}

/**
 * Adds a mouse event to the macro that is currently being
 * recorded.  If no macro is being recorded, no action is taken.
 *
 * @param widget
 *     Widget where mouse event occurred
 * @param descriptiveName
 *     Descriptive name for user
 * @param me
 *     The Qt Mouse Event
 * @return 
 *     True if the mouse event was recorded or false if there is an error.
 */
bool
WuQMacroManager::addMouseEventToRecording(QWidget* widget,
                                          const QString& descriptiveName,
                                          const QMouseEvent* me)
{
    CaretAssert(widget);
    CaretAssert(me);
    
    WuQMacro* recordingMacro(NULL);
    switch (getMode()) {
        case WuQMacroModeEnum::OFF:
            break;
        case WuQMacroModeEnum::RECORDING_INSERT_COMMANDS:
            CaretAssert(m_macroInsertCommandBeingRecorded);
            recordingMacro = m_macroInsertCommandBeingRecorded;
            break;
        case WuQMacroModeEnum::RECORDING_NEW_MACRO:
            CaretAssert(m_macroBeingRecorded);
            recordingMacro = m_macroBeingRecorded;
            break;
        case WuQMacroModeEnum::RUNNING:
            break;
    }
    if (recordingMacro != NULL) {
        const QString name(widget->objectName());
        if (name.isEmpty()) {
            CaretLogSevere("Widget name is empty for recording of mouse event\n"
                           + SystemUtilities::getBackTrace());
            return false;
        }
        if (descriptiveName.isEmpty()) {
            CaretLogSevere("Descriptive name is empty for "
                           + name
                           + "\n"
                           + SystemUtilities::getBackTrace());
        }

        bool validMouseEventFlag(true);
        WuQMacroMouseEventTypeEnum::Enum mouseEventType = WuQMacroMouseEventTypeEnum::MOVE;
        switch (me->type()) {
            case QEvent::MouseButtonPress:
                mouseEventType = WuQMacroMouseEventTypeEnum::BUTTON_PRESS;
                break;
            case QEvent::MouseButtonRelease:
                mouseEventType = WuQMacroMouseEventTypeEnum::BUTTON_RELEASE;
                break;
            case QEvent::MouseButtonDblClick:
                mouseEventType = WuQMacroMouseEventTypeEnum::DOUBLE_CLICKED;
                break;
            case QEvent::MouseMove:
                mouseEventType = WuQMacroMouseEventTypeEnum::MOVE;
                
                /*
                 * Only track move events if a button is down
                 * Note: Use "buttons()" mask, not button()
                 */
                if (me->buttons() == Qt::NoButton) {
                    validMouseEventFlag = false;
                }
                break;
            default:
                CaretAssertMessage(0, ("Unknown mouse event type integer cast="
                                       + QString::number(static_cast<int>(me->type()))));
                break;
        }
        if (validMouseEventFlag) {
            WuQMacroMouseEventInfo* mouseInfo = new WuQMacroMouseEventInfo(mouseEventType,
                                                                           static_cast<uint32_t>(me->button()),
                                                                           static_cast<uint32_t>(me->buttons()),
                                                                           static_cast<uint32_t>(me->modifiers()),
                                                                           widget->width(),
                                                                           widget->height());
            mouseInfo->addLocalXY(me->localPos().x(),
                                  me->localPos().y());
        
            const int32_t versionNumber(1);
            QString errorMessage;
            WuQMacroCommand* command = WuQMacroCommand::newInstanceMouseCommand(mouseInfo,
                                                                                versionNumber,
                                                                                name,
                                                                                descriptiveName,
                                                                                "mouse operation",
                                                                                1.0,
                                                                                errorMessage);
            recordingMacro->appendMacroCommand(command);
            return true;
        }
    }
    
    return false;
}


/**
 * @return All 'active' available macro groups.
 *         Macros groups that are editible.  Other macro
 *         groups are exluded.
 */
std::vector<WuQMacroGroup*>
WuQMacroManager::getActiveMacroGroups() const
{
    std::vector<WuQMacroGroup*> macroGroups;
    if (m_macroHelper) {
        macroGroups = m_macroHelper->getActiveMacroGroups();
    }
    return macroGroups;
}

/**
 * @return All macro groups including those that are
 *         be valid (editable) at this time.
 */
std::vector<const WuQMacroGroup*>
WuQMacroManager::getAllMacroGroups() const
{
    std::vector<const WuQMacroGroup*> macroGroups;
    if (m_macroHelper) {
        macroGroups = m_macroHelper->getAllMacroGroups();
    }
    return macroGroups;
}


/**
 * Start recording a new macro using the New Macro dialog.
 *
 * @param parent
 *     Parent for dialog.
 */
void
WuQMacroManager::startRecordingNewMacro(QWidget* parent)
{
    startRecordingNewMacro(parent,
                           NULL,
                           NULL);
}

/**
 * Start recording a new macro using the New Macro dialog.
 * New macro inserted into the given macro group and after the given macro.
 *
 * @param parent
 *     Parent for dialog.
 * @param insertIntoMacroGroup
 *     Insert new macro into this macro group
 * @param insertAfterMacro
 *     Insert new macro after this macro (if NULL
 *     new macro is inserted at beginning of group)
 * @return
 *     Pointer to new macro or NULL if user cancelled
 */
WuQMacro*
WuQMacroManager::startRecordingNewMacro(QWidget* parent,
                                        WuQMacroGroup* insertIntoMacroGroup,
                                        WuQMacro* insertAfterMacro)
{
    CaretAssert(m_mode == WuQMacroModeEnum::OFF);
    
    WuQMacroCreateDialog createMacroDialog(insertIntoMacroGroup,
                                           insertAfterMacro,
                                           parent);
    if (createMacroDialog.exec() == WuQMacroCreateDialog::Accepted) {
        m_mode = WuQMacroModeEnum::RECORDING_NEW_MACRO;
        m_macroBeingRecorded = createMacroDialog.getNewMacro();
        CaretAssert(m_macroBeingRecorded);
        
        EventManager::get()->sendEvent(EventUserInterfaceUpdate().getPointer());
    }
    else {
        m_macroBeingRecorded = NULL;
    }

    return m_macroBeingRecorded;
}

/**
 * Start recording commands and insert them into the given macro after
 * the given macro command.
 *
 * @param insertIntoMacro
 *     Macro into which new commands are inserted.
 * @param insertAfterMacroCommand
 *     New commands are inserted after this command or at beginning
 *     if this command is NULL.
 */
void
WuQMacroManager::startRecordingNewCommandInsertion(WuQMacro* insertIntoMacro,
                                                   WuQMacroCommand* insertAfterMacroCommand)
{
    CaretAssert(m_mode == WuQMacroModeEnum::OFF);
    CaretAssert(insertIntoMacro);
    
    m_mode = WuQMacroModeEnum::RECORDING_INSERT_COMMANDS;
    m_macroInsertCommandBeingRecorded = insertIntoMacro;
    m_macroInsertCommandBeingRecordedOffset = 0;
    if (insertAfterMacroCommand != NULL) {
        const int32_t index = insertIntoMacro->getIndexOfMacroCommand(insertAfterMacroCommand) + 1;
        CaretAssert(index >= 0);
        m_macroInsertCommandBeingRecordedOffset = index;
    }
    EventManager::get()->sendEvent(EventUserInterfaceUpdate().getPointer());
}

/**
 * Stop recording the macro.
 */
void
WuQMacroManager::stopRecordingNewMacro()
{
    switch (m_mode) {
        case WuQMacroModeEnum::OFF:
            CaretAssert(0);
            break;
        case WuQMacroModeEnum::RECORDING_INSERT_COMMANDS:
            CaretAssert(m_macroInsertCommandBeingRecorded);
            if (m_macroHelper) {
                m_macroHelper->macroWasModified(m_macroInsertCommandBeingRecorded);
            }
            break;
        case WuQMacroModeEnum::RECORDING_NEW_MACRO:
            CaretAssert(m_macroBeingRecorded);
            if (m_macroHelper) {
                m_macroHelper->macroWasModified(m_macroBeingRecorded);
            }
            break;
        case WuQMacroModeEnum::RUNNING:
            CaretAssert(0);
            break;
    }
    m_mode = WuQMacroModeEnum::OFF;
    
    m_macroBeingRecorded = NULL;
    m_macroInsertCommandBeingRecorded = NULL;
    m_macroInsertCommandBeingRecordedOffset = -1;
    
    EventManager::get()->sendEvent(EventUserInterfaceUpdate().getPointer());
    updateNonModalDialogs();
}

/**
 * Show the macros dialog
 *
 * @param parent
 *     Parent for dialog
 */
void
WuQMacroManager::showMacrosDialog(QWidget* parent)
{
    if (m_macrosDialog == NULL) {
        m_macrosDialog = new WuQMacroDialog(parent);
    }
    m_macrosDialog->updateDialogContents();
    m_macrosDialog->show();
    m_macrosDialog->raise();
    m_macrosDialog->restorePositionAndSize();
}

/**
 * @return Vector containing all non-modal dialogs used by Macro Manager.
 * This may be useful if the parent window is closed but other parent
 * windows are available.
 */
std::vector<QWidget*>
WuQMacroManager::getNonModalDialogs()
{
    std::vector<QWidget*> nonModalDialogs;
    if (m_macrosDialog != NULL) {
        nonModalDialogs.push_back(m_macrosDialog);
    }
    return nonModalDialogs;
}


/**
 * Update non-modal dialogs in macro manager
 */
void
WuQMacroManager::updateNonModalDialogs()
{
    if (m_macrosDialog != NULL) {
        m_macrosDialog->updateDialogContents();
    }
}

/**
 * Run the given macro
 * 
 * @param widget
 *     Widget used for parent of dialogs
 * @param macroToRun
 *     Macro that is run
 * @param macroCommandToStartAt
 *     Macro command at which execution should begin.  If NULL, start
 *     with the first command in the macro
 * @param macroCommandToStopAfter
 *     Macro command that the executor may stop after, depending upon options.
 *     If NULL, end after last command is executed.
 * @return
 *     Pointer to macro that was last run.  The selected macro could change
 *     when looping is enabled.
 */
WuQMacro*
WuQMacroManager::runMacro(QWidget* widget,
                          const WuQMacro* macroToRun,
                          const WuQMacroCommand* macroCommandToStartAt,
                          const WuQMacroCommand* macroCommandToStopAfter)
{
    CaretAssert(widget);
    CaretAssert(macroToRun);
    WuQMacro* macro(const_cast<WuQMacro*>(macroToRun));
    
    bool dialogWasDisplayed(false);
    bool resultFlag(false);
    bool loopFlag(true);
    while (loopFlag) {
        loopFlag = m_executorOptions->isLooping();
        QString errorMessage;
        m_macroExecutor = new WuQMacroExecutor();
        QObject::connect(m_macroExecutor, &WuQMacroExecutor::macroCommandAboutToStart,
                         this, &WuQMacroManager::macroCommandStartingExecution);
        QObject::connect(m_macroExecutor, &WuQMacroExecutor::macroCommandHasCompleted,
                         this, &WuQMacroManager::macroCommandCompletedExecution);
        
        if (m_macrosDialog != NULL) {
            QObject::connect(m_macroExecutor, &WuQMacroExecutor::macroCommandStarting,
                             m_macrosDialog, &WuQMacroDialog::selectMacroCommand);
        }
        
        m_macroExecutorMonitor->setMode(WuQMacroExecutorMonitor::Mode::RUN);
        
        if (m_macroHelper != NULL) {
            m_macroHelper->macroExecutionStarting(macro,
                                                  widget,
                                                  m_executorOptions.get());
        }
        resultFlag = m_macroExecutor->runMacro(macro,
                                               macroCommandToStartAt,
                                               macroCommandToStopAfter,
                                               widget,
                                               m_parentObjects,
                                               m_macroExecutorMonitor,
                                               m_executorOptions.get(),
                                               errorMessage);
        if (m_macroHelper != NULL) {
            m_macroHelper->macroExecutionEnding(macro,
                                                widget,
                                                m_executorOptions.get());
        }
        
        m_macroExecutorMonitor->setMode(WuQMacroExecutorMonitor::Mode::STOP);
        
        if ( ! resultFlag) {
            QMessageBox::information(widget,
                                  "Macro Done",
                                  errorMessage,
                                  QMessageBox::Ok,
                                  QMessageBox::NoButton);
            loopFlag = false;
            dialogWasDisplayed = true;
        }
        
        /*
         * Mutex needed so stop() method does not try
         * to access an invalid pointer to executor.
         */
        QMutexLocker locker(&m_macroExecutorMutex);
        delete m_macroExecutor;
        m_macroExecutor = NULL;
        locker.unlock();
        
        if (loopFlag) {
            if (m_macroHelper != NULL) {
                QMessageBox* msgBox = new QMessageBox(QMessageBox::Information,
                                                      "Wait",
                                                      "Reloading scene",
                                                      QMessageBox::Ok,
                                                      widget);
                msgBox->button(QMessageBox::Ok)->setVisible(false);
                msgBox->show();
                
                WuQMacro* newMacro = resetMacro(widget, macro);
                if (newMacro != NULL) {
                    macro = newMacro;
                }
                else {
                    loopFlag = false;
                }
                
                msgBox->hide();
                msgBox->deleteLater();
            }
        }
    }
    
    if ( ! dialogWasDisplayed) {
        QApplication::beep();
    }
    return macro;
}

/**
 * If a macro is running, stop it
 */
void
WuQMacroManager::stopMacro()
{
    m_macroExecutorMonitor->setMode(WuQMacroExecutorMonitor::Mode::STOP);
}

/**
 * Pause or continue a macro
 */
void
WuQMacroManager::pauseContinueMacro()
{
    switch (m_macroExecutorMonitor->getMode()) {
        case WuQMacroExecutorMonitor::Mode::PAUSE:
            m_macroExecutorMonitor->setMode(WuQMacroExecutorMonitor::Mode::RUN);
            break;
        case WuQMacroExecutorMonitor::Mode::RUN:
            m_macroExecutorMonitor->setMode(WuQMacroExecutorMonitor::Mode::PAUSE);
            break;
        case WuQMacroExecutorMonitor::Mode::STOP:
            break;
    }
}

/**
 * Called by macro executor when a macro command has completed
 *
 * @param window
 *     Window in which command is running
 * @param command
 *     Command that just finished execution
 * @param allowDelayFlagOut
 *     Allow delay for after command has completed
 */
void
WuQMacroManager::macroCommandCompletedExecution(QWidget* window,
                                                const WuQMacroCommand* command,
                                                const WuQMacroExecutorOptions* executorOptions,
                                                bool& allowDelayFlagOut)
{
    CaretAssert(command);
    if (m_macroHelper != NULL) {
        m_macroHelper->macroCommandHasCompleted(window,
                                                command,
                                                executorOptions,
                                                allowDelayFlagOut);
    }
}

/**
 * Called by macro executor when a macro command is about to start
 *
 * @param window
 *     Window in which command is running
 * @param command
 *     Command that about to start execution
 * @param allowDelayFlagOut
 *     Allow delay for after command has completed
 */
void
WuQMacroManager::macroCommandStartingExecution(QWidget* window,
                                               const WuQMacroCommand* command,
                                               const WuQMacroExecutorOptions* executorOptions,
                                               bool& allowDelayFlagOut)
{
    CaretAssert(command);
    if (m_macroHelper != NULL) {
        m_macroHelper->macroCommandAboutToStart(window,
                                                command,
                                                executorOptions,
                                                allowDelayFlagOut);
    }
}

/**
 * Reset the given macro
 *
 * @param parentg
 *     Parent for any dialogs
 * @param macro
 *     Macro that is reset to beginning state
 */
WuQMacro*
WuQMacroManager::resetMacro(QWidget* parent,
                            WuQMacro* macro)
{
    WuQMacro* newMacro(macro);
    if (m_macroHelper != NULL) {
        newMacro = m_macroHelper->resetMacroStateToBeginning(macro,
                                                             parent);
    }
    return newMacro;
}

/**
 * Add a parent object that will be searched during macro
 * execution to find objects by name that are contained
 * in a macro command
 *
 * @param parentObject
 *     Object used to find objects
 */
void
WuQMacroManager::addParentObject(QObject* parentObject)
{
    CaretAssert(parentObject);
    m_parentObjects.push_back(parentObject);
}

/**
 * Can be called to indicate that a macro was modified
 *
 * @param macro
 *     Macro that was modified
 */
void
WuQMacroManager::macroWasModified(WuQMacro* macro)
{
    if (m_macroHelper) {
        m_macroHelper->macroWasModified(macro);
    }
}


/**
 * Delete a macro
 *
 * @param parent
 *     Parent widget for dialog
 * @param macroGroup
 *     Group containing macro for deletion
 * @param macro
 *     Macro to delete
 * @return
 *     True if macro was deleted.
 */
bool
WuQMacroManager::deleteMacro(QWidget* parent,
                             WuQMacroGroup* macroGroup,
                             WuQMacro* macro)
{
    CaretAssert(macroGroup);
    CaretAssert(macro);
    if (QMessageBox::warning(parent,
                             "Warning",
                             ("Delete the macro: " + macro->getName()),
                             QMessageBox::Ok | QMessageBox::Cancel,
                             QMessageBox::Ok) == QMessageBox::Ok) {
        macroGroup->deleteMacro(macro);
        if (m_macroHelper) {
            m_macroHelper->macroGroupWasModified(macroGroup);
        }
        return true;
    }
    
    return false;
}

/**
 * Delete a macro command
 *
 * @param parent
 *     Parent widget for dialog
 * @param macroGroup
 *     Group containing macro
 * @param macro
 *     Macro containing command to be deleted
 * @param macroCommand
 *     Macro command for deletion
 * @return
 *     True if macro was deleted.
 */
bool
WuQMacroManager::deleteMacroCommand(QWidget* parent,
                                    WuQMacroGroup* macroGroup,
                                    WuQMacro* macro,
                                    WuQMacroCommand* macroCommand)
{
    CaretAssert(macroGroup);
    CaretAssert(macro);
    CaretAssert(macroCommand);
    bool deleteFlag(true);
    const bool confirmDeleteFlag(false);
    if (confirmDeleteFlag) {
        deleteFlag = (QMessageBox::warning(parent,
                                           "Warning",
                                           ("Delete the macro command: " + macroCommand->getDescriptiveName()),
                                           QMessageBox::Ok | QMessageBox::Cancel,
                                           QMessageBox::Ok) == QMessageBox::Ok);
    }
    if  (deleteFlag) {
        macro->deleteMacroCommand(macroCommand);
        if (m_macroHelper) {
            m_macroHelper->macroGroupWasModified(macroGroup);
        }
        return true;
    }
    
    return false;
}

/**
 * Import macros from a file
 *
 * @param parent
 *     Parent widget for dialog
 * @param macroGroup
 *     Group to which macros are appended
 * @return
 *     True if macro(s) were successfully imported
 */
bool
WuQMacroManager::importMacros(QWidget* parent,
                              WuQMacroGroup* appendToMacroGroup)
{
    QString fileFilterString(WuQMacroFile::getFileDialogFilter());
    const QString filename = QFileDialog::getOpenFileName(parent,
                                                          "Import Macros",
                                                          s_importExportMacroFileDirectory,
                                                          fileFilterString,
                                                          &fileFilterString,
                                                          QFileDialog::DontUseNativeDialog);
    if ( ! filename.isEmpty()) {
        WuQMacroFile macroFile;
        try {
            macroFile.readFile(filename);
            
            QFileInfo fileInfo(filename);
            s_importExportMacroFileDirectory = fileInfo.absolutePath();
            
            const WuQMacroGroup* fileMacroGroup = macroFile.getMacroGroup();
            if (fileMacroGroup->getNumberOfMacros() > 0) {
                appendToMacroGroup->appendMacroGroup(fileMacroGroup);
                if (m_macroHelper) {
                    m_macroHelper->macroGroupWasModified(appendToMacroGroup);
                }
                return true;
            }
            else {
                throw DataFileException("File is empty, no macros to import");
            }
        }
        catch (const DataFileException& dfe) {
            QMessageBox::critical(parent,
                                  "File Error",
                                  dfe.whatString(),
                                  QMessageBox::Ok,
                                  QMessageBox::Ok);
        }
    }
    
    return false;
}

/**
 * Export macro(s)
 *
 * @param parent
 *     Parent widget for dialog
 * @param macroGroup
 *     Group for export (if non-NULL)
 * @param macro
 *     Macro for export (if non-NULL)
 * @return
 *     True if macro was successfully exported
 */
bool
WuQMacroManager::exportMacros(QWidget* parent,
                              WuQMacroGroup* macroGroup,
                              WuQMacro* macro)
{
    QString fileFilterString(WuQMacroFile::getFileDialogFilter());
    const QString filename = QFileDialog::getSaveFileName(parent,
                                                          "Export Macros",
                                                          s_importExportMacroFileDirectory,
                                                          fileFilterString,
                                                          &fileFilterString,
                                                          (QFileDialog::DontUseNativeDialog
                                                           | QFileDialog::DontConfirmOverwrite));
    if ( ! filename.isEmpty()) {
        try {
            WuQMacroFile macroFile;
            
            if (macroGroup != NULL) {
                macroFile.appendMacroGroup(macroGroup);
            }
            else if (macro != NULL) {
                macroFile.addMacro(new WuQMacro(*macro));
            }
            else {
                throw DataFileException("No macro group or macro for export");
            }
            
            QString filenameToWrite(filename);
            if ( ! filenameToWrite.endsWith(WuQMacroFile::getFileExtension())) {
                filenameToWrite.append(WuQMacroFile::getFileExtension());
            }
            
            macroFile.writeFile(filenameToWrite);
            
            QFileInfo fileInfo(filenameToWrite);
            s_importExportMacroFileDirectory = fileInfo.absolutePath();

            return true;
        }
        catch (const DataFileException& dfe) {
            QMessageBox::critical(parent,
                                  "File Error",
                                  dfe.whatString(),
                                  QMessageBox::Ok,
                                  QMessageBox::Ok);
        }
    }
    
    return false;
}

/**
 * Get all signal watchers
 *
 * @param enabledItemsOnly
 *    Only valid signal watchers whose object currently exists are included.
 *    An object may cease to exists with something is closed (such as a window)
 *
 * @return
 *    All signal watchers
 */
std::vector<WuQMacroSignalWatcher*>
WuQMacroManager::getAllWidgetSignalWatchers(const bool enabledItemsOnly) const
{
    std::vector<WuQMacroSignalWatcher*> allWatchers;
    allWatchers.reserve(m_signalWatchers.size());
    
    for (auto iter : m_signalWatchers) {
        WuQMacroSignalWatcher* watcher = iter.second;
        bool validFlag(true);
        if (enabledItemsOnly) {
            validFlag = false;
            for (auto po : m_parentObjects) {
                QObject* object = po->findChild<QObject*>(watcher->getObjectName());
                if (object != NULL) {
                    validFlag = true;
                    break;
                }
            }
        }
        
        if (validFlag) {
            allWatchers.push_back(watcher);
        }
    }
    return allWatchers;
}

/**
 * Get names of all signal watchers
 *
 * @param enabledItemsOnly
 *    Only valid signal watches are returned
 * @return
 *    All signal watchers
 */
std::vector<QString>
WuQMacroManager::getAllWidgetSignalWatcherNames(const bool enabledItemsOnly) const
{
    std::vector<WuQMacroSignalWatcher*> allWatchers = getAllWidgetSignalWatchers(enabledItemsOnly);
    
    std::vector<QString> names;
    names.reserve(m_signalWatchers.size());
    
    for (auto iter : allWatchers) {
        names.push_back(iter->getObjectName());
    }
    
    return names;
}


/**
 * Get the widget signal watcher with the given name
 *
 * @param name
 *     Name of widget signal watcher
 * @retrurn
 *     Pointer to watcher or NULL if not valid.
 */
WuQMacroSignalWatcher*
WuQMacroManager::getWidgetSignalWatcherWithName(const QString& name)
{
    auto watcher = m_signalWatchers.find(name);
    if (watcher != m_signalWatchers.end()) {
        return watcher->second;
    }
    return NULL;
}

/**
 * Print supported widgets to the terminal window.
 */
void
WuQMacroManager::printSupportedWidgetsToTerminal()
{
    std::set<QString> allList;
    std::set<QString> duplicateList;
    for (auto iter : m_signalWatchers) {
        const QString s(iter.second->toString());
     
        const auto existIter = allList.find(s);
        if (existIter != allList.end()) {
            duplicateList.insert(s);
        }
        else {
            allList.insert(s);
        }
    }
    
    for (auto& iter : allList) {
        std::cout << iter << std::endl;
    }
    
    for (auto& iter : duplicateList) {
        std::cout << "DUPLICTE: " << iter << std::endl;
    }
}

/**
 * Print top level widgets
 */
void
WuQMacroManager::printToLevelWidgetsToTerminal()
{
    std::cout << "Top Level Widgets: " << std::endl;
    QWidgetList widgetList = qApp->topLevelWidgets();
    foreach (QWidget* widget, widgetList) {
        std::cout << "   " << widget->objectName()
        << ",  " << widget->metaObject()->className() << std::endl;
    }
    std::cout << std::endl;
}
/**
 * Get the tooltip for the object with the given name
 *
 * @param objectName
 *     Name of object
 * @return 
 *     Tooltip for object or empty if not found.
 */
QString
WuQMacroManager::getToolTipForObjectName(const QString& objectName) const
{
    QString tooltip;

    const auto existingWatcher = m_signalWatchers.find(objectName);
    if (existingWatcher != m_signalWatchers.end()) {
        tooltip = existingWatcher->second->getToolTip();
    }
    
    return tooltip;
}

/**
 * @return Pointer to the executor's run options (const method)
 */
const WuQMacroExecutorOptions*
WuQMacroManager::getExecutorOptions() const
{
    return m_executorOptions.get();
}

/**
 * @return Pointer to the executor's run options
 */
WuQMacroExecutorOptions*
WuQMacroManager::getExecutorOptions()
{
    return m_executorOptions.get();
}

/**
 * @return Pointer to the executor monitor.
 * Always returns valid pointer, even if macro is not running.
 */
const WuQMacroExecutorMonitor*
WuQMacroManager::getMacroExecutorMonitor() const
{
    return m_macroExecutorMonitor;
}

/**
 * Process a key press event for macro shortcut
 *
 * @param keyEvent
 *     Key event information.
 * @return
 *     True if the input process recognized the key event
 *     and the key event SHOULD NOT be propagated to parent
 *     widgets
 */
bool
WuQMacroManager::runMacroWithShortCutKeyEvent(QWidget* window,
                                              const QKeyEvent* keyEvent)
{
    CaretAssert(keyEvent);
    
    /*
     * On Mac, SHIFT, CONTROL, COMMANDS
     * On Linux/Windows: SHIFT CTRL META
     */
    Qt::KeyboardModifiers mods;
#ifdef CARET_OS_MACOSX
    mods.setFlag(Qt::ShiftModifier);
    mods.setFlag(Qt::MetaModifier);
    mods.setFlag(Qt::ControlModifier);
#else
    mods.setFlag(Qt::ShiftModifier);
    mods.setFlag(Qt::ControlModifier);
    mods.setFlag(Qt::AltModifier);
#endif
    if (keyEvent->modifiers() == mods) {
        const int qtKeyCode = keyEvent->key();
        const WuQMacroShortCutKeyEnum::Enum shortCutKey = WuQMacroShortCutKeyEnum::fromQtKeyEnum(qtKeyCode);
        if (shortCutKey != WuQMacroShortCutKeyEnum::Key_None) {
            const WuQMacro* macro = getMacroWithShortCutKey(shortCutKey);
            if (macro != NULL) {
                runMacro(window,
                         macro,
                         NULL,
                         NULL);
                return true;
            }
        }
    }
    
    return false;
}

/**
 * @return the macro with the given short cut key or NULL if not found
 *
 * @param shortCutKey
 *     The short cut key
 */
WuQMacro*
WuQMacroManager::getMacroWithShortCutKey(const WuQMacroShortCutKeyEnum::Enum shortCutKey) const
{
    WuQMacro* macro(NULL);
    
    const auto macroGroups = getActiveMacroGroups();
    for (const auto mg : macroGroups) {
        macro = mg->getMacroWithShortCutKey(shortCutKey);
        if (macro != NULL) {
            break;
        }
    }
    
    return macro;
}

QString
WuQMacroManager::getShortCutKeysMask()
{
    QString mask;
#ifdef CARET_OS_MACOSX
    mask = "shift/control/command";
#else
    mask = "shift/ctrl/alt";
#endif
    
    return mask;
}

/**
 * Get information about a custom parameter that may be a range or
 * a list of valid values
 *
 * @param browserWindowIndex
 *     Index of window
 * @param macroCommand
 *     Macro command that contains the parameter for editing
 * @param parameter
 *     Parameter for editing
 * @param dataInfoOut
 *     Information about the data
 * @return
 *     True if the parameter was modified
 */
bool
WuQMacroManager::getCustomParameterDataInfo(const int32_t browserWindowIndex,
                                            WuQMacroCommand* macroCommand,
                                            WuQMacroCommandParameter* parameter,
                                            WbMacroCustomDataInfo& dataInfoOut)
{
    bool validFlag(false);
    if (m_customCommandManager != NULL) {
        validFlag = m_customCommandManager->getCustomParameterDataInfo(browserWindowIndex,
                                                                       macroCommand,
                                                                       parameter,
                                                                       dataInfoOut);
    }
    else {
        CaretLogSevere("No Macro Helper available for editing custom values in a macro parameter");
    }
    return validFlag;
}

/**
 * Run a custom-defined macro command
 *
 * @param parent
 *     Parent widget for any dialogs
 * @param executorMonitor
 *     The executor monitor
 * @param executorOptions
 *     Options for the executor
 * @param macroCommand
 *     Custom macro command to run
 * @param errorMessageOut
 *     Contains any error information or empty if no error
 */
bool
WuQMacroManager::executeCustomOperationMacroCommand(QWidget* parent,
                                                    const WuQMacroExecutorMonitor* executorMonitor,
                                                    const WuQMacroExecutorOptions* executorOptions,
                                                    const WuQMacroCommand* macroCommand,
                                                    QString& errorMessageOut)
{
    CaretAssert(parent);
    CaretAssert(macroCommand);

    errorMessageOut.clear();
    
    bool successFlag(false);
    if (m_customCommandManager != NULL) {
        successFlag = m_customCommandManager->executeCustomOperationMacroCommand(parent,
                                                                                 executorMonitor,
                                                                                 executorOptions,
                                                                                 macroCommand,
                                                                                 errorMessageOut);
    }
    else {
        CaretLogSevere("No Macro Helper available for running custom macro commands");
    }
    
    return successFlag;
}

/**
 * @return All custom operation commands.  Caller is responsible for deleting
 * all content of the returned vector.
 */
std::vector<WuQMacroCommand*>
WuQMacroManager::getAllCustomOperationMacroCommands()
{
    std::vector<WuQMacroCommand*> customCommands;
    
    if (m_customCommandManager != NULL) {
        customCommands = m_customCommandManager->getAllCustomOperationMacroCommands();
    }
    return customCommands;
}

/**
 * @return Names of custom operation defined macro commands
 */
std::vector<QString>
WuQMacroManager::getNamesOfCustomOperationMacroCommands()
{
    std::vector<QString> names;
    
    if (m_customCommandManager != NULL) {
        names = m_customCommandManager->getNamesOfCustomOperationMacroCommands();
    }

    return names;
}

/**
 * Get a new instance of a custom operation for the given macro command name
 *
 * @param customMacroCommandName
 *     Name of custom macro command
 * @param errorMessageOut
 *     Contains any error information or empty if no error
 * @return
 *     Pointer to command or NULL if not valid
 */
WuQMacroCommand*
WuQMacroManager::newInstanceOfCustomOperationMacroCommand(const QString& macroCommandName,
                                                          QString& errorMessageOut)
{
    errorMessageOut.clear();
    WuQMacroCommand* command(NULL);
    
    if (m_customCommandManager != NULL) {
        command = m_customCommandManager->newInstanceOfCustomOperationMacroCommand(macroCommandName,
                                                                                   errorMessageOut);
    }
    else {
        errorMessageOut = "No Custom Operation Manager is available for creating custom commands";
    }
    
    return command;
}

/**
 * @return Identifiers of all available windows in which macros may be run
 */
std::vector<QString>
WuQMacroManager::getMainWindowIdentifiers()
{
    std::vector<QString> identifiers;
    if (m_macroHelper != NULL) {
        identifiers = m_macroHelper->getMainWindowIdentifiers();
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
WuQMacroManager::getMainWindowWithIdentifier(const QString& identifier)
{
    if (m_macroHelper != NULL) {
        return m_macroHelper->getMainWindowWithIdentifier(identifier);
    }
    return NULL;
}

/**
 * @return A default name for a macro
 */
QString
WuQMacroManager::getNewMacroDefaultName() const
{
    QString name("");
    bool foundFlag(false);
    static int32_t minimumIndex = 1;
    
    const std::vector<const WuQMacroGroup*> macroGroups = getAllMacroGroups();
    for (int32_t i = minimumIndex; i < 10000; i++) {
        name = ("Macro "
                + QString::number(i));
        foundFlag = false;
        for (auto mg : macroGroups) {
            if (mg->getMacroByName(name) != NULL) {
                foundFlag = true;
                break;
            }
        }
        
        if ( ! foundFlag) {
            /*
             * If a default name was created, do not allow a "lower-numbered"
             * default name.  Start with same index since user may not use it.
             */
            minimumIndex = i;
            break;
        }
    }
    
    return name;
}

/**
 * @return The macro widget action with the given name or
 * NULL if not found
 *
 * @param name
 *     Name of macro widget action
 */
WuQMacroWidgetAction*
WuQMacroManager::getMacroWidgetActionByName(const QString& name)
{
    for (auto mwa : m_macroWidgetActions) {
        if (mwa->getName() == name) {
            return mwa;
        }
    }
    return NULL;
}

/**
 * Get a widget for the macro widget action with the given name
 *
 * @param name
 *     Name of the macro widget action
 * @param parentWidget
 *     Optional parent widget for the returned widget
 * @return
 *     Widget associated with the widget macro action or NULL if failure.
 */
QWidget*
WuQMacroManager::getWidgetForMacroWidgetActionByName(const QString& name,
                                                     QWidget* parentWidget)
{
    QWidget* widget(NULL);
    WuQMacroWidgetAction* mwa = getMacroWidgetActionByName(name);
    if (mwa != NULL) {
        widget = mwa->requestWidget(parentWidget);
        if (widget == NULL) {
            const QString msg("Failed to create widget for macro widget action with name \""
                              + name
                              + ".");
            CaretAssertMessage(0, msg);
            CaretLogSevere(msg);
        }
    }
    else {
        const QString msg("No macro widget action with name \""
                          + name
                          + "\" was found.");
        CaretAssertMessage(0, msg);
        CaretLogSevere(msg);
    }
    
    return widget;
}

/**
 * Release the widget from its associated macro widget action.
 * Note: The widget is NOT destroyed as that is the responsibility
 * of the widget owner (typically a dialog).
 */
void
WuQMacroManager::releaseWidgetFromMacroWidgetAction(QWidget* widget,
                                                    QWidget* widget2,
                                                    QWidget* widget3,
                                                    QWidget* widget4,
                                                    QWidget* widget5,
                                                    QWidget* widget6)
{
    std::vector<QWidget*> allWidgets { widget, widget2, widget3, widget4, widget5, widget6 };
    for (auto widget :  allWidgets) {
        if (widget != NULL) {
            for (auto mwa : m_macroWidgetActions) {
                mwa->releaseWidget(widget);
            }
        }
    }
}

/**
 * Update the value of a widget from its macro widget action.
 *
 * @param widget
 *     Widget that gets its value update
 */
void
WuQMacroManager::updateValueInWidgetFromMacroWidgetAction(QWidget* widget,
                                                          QWidget* widget2,
                                                          QWidget* widget3,
                                                          QWidget* widget4,
                                                          QWidget* widget5,
                                                          QWidget* widget6)
{
    std::vector<QWidget*> allWidgets { widget, widget2, widget3, widget4, widget5, widget6 };
    for (auto widget :  allWidgets) {
        if (widget != NULL) {
            for (auto mwa : m_macroWidgetActions) {
                if (mwa->updateWidgetWithModelValue(widget)) {
                    return;
                }
            }
        }
    }
}
