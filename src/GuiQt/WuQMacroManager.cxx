
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

#include <QAction>
#include <QApplication>
#include <QMessageBox>
#include <QMouseEvent>
#include <QWidget>

#include "CaretAssert.h"
#include "CaretLogger.h"
#include "CaretPreferences.h"
#include "SessionManager.h"
#include "WuQMacro.h"
#include "WuQMacroCommand.h"
#include "WuQMacroCreateDialog.h"
#include "WuQMacroDialog.h"
#include "WuQMacroExecutor.h"
#include "WuQMacroMouseEventInfo.h"
#include "WuQMacroSignalWatcher.h"

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
}

/**
 * Destructor.
 */
WuQMacroManager::~WuQMacroManager()
{
    /*
     * Do not delete the WuQMacroSignalWatcher instances in
     * m_signalWatchers.  This WuQMacroManager is set as the
     * parent object of the WuQMacroSignalWatcher's so Qt
     * will destroy them.
     */
    m_signalWatchers.clear();
    
    /*
     * If an instance is being deleted it MUST be the
     * singleton so make it NULL.
     */
    if (s_singletonMacroManager != NULL) {
        s_singletonMacroManager = NULL;
    }
}

///**
// * Create the singleton Macro Manager.  This method MUST be called before
// * calling instance() and this method should only be called once.  When
// * the given 'parent' is deleted, it will result in the singletone instance
// * being deleted.
// *
// * @param name
// *     Name of macro manager
// * @parent
// *     Parent of the macro manager.
// */
//WuQMacroManager*
//WuQMacroManager::createMacroManagerSingleton(const QString& name,
//                                                    QObject* parent)
//{
//    if (s_singletonMacroManager == NULL) {
//        s_singletonMacroManager = new WuQMacroManager(name,
//                                                      parent);
//    }
//    else {
//        CaretLogSevere("WuQMacroManager::createMacroManagerSingleton() has already been "
//                       "called, this should never be called more than once.");
//    }
//    
//    return s_singletonMacroManager;
//}

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
//        CaretLogSevere("A macro manager has not been created with createMacroManagerSingleton().  "
//                       "That method MUST be called before this method.");
//        std::abort();
    }
    
    return s_singletonMacroManager;
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
 * @return True if mode is RECORDING
 */
bool
WuQMacroManager::isModeRecording() const
{
    return (m_mode == WuQMacroModeEnum::RECORDING);
}

/**
 * Add macro support to the given object.  When recording,
 * The object's 'value changed' signal will be monitored so
 * that the new value can be part of a macro command.
 *
 * @param object
 *     Object that is monitored.
 */
void
WuQMacroManager::addMacroSupportToObject(QObject* object)
{
    CaretAssert(object);
    
    const QString name = object->objectName();
    if (name.isEmpty()) {
        CaretLogSevere("Object name is empty, will be ignored for macros\n"
                       + SystemUtilities::getBackTrace());
        return;
    }
    
    {
        QString toolTipText;
        QAction* action = qobject_cast<QAction*>(object);
        if (action != NULL) {
            toolTipText = action->toolTip();
        }
        QWidget* widget = qobject_cast<QWidget*>(object);
        if (widget != NULL) {
            toolTipText = widget->toolTip();
        }
        if (toolTipText.isEmpty()) {
            CaretLogWarning("Object named \""
                            + name
                            + "\" is missing a tooltip");
        }
    }
    auto existingWatcher = m_signalWatchers.find(name);
    if (existingWatcher != m_signalWatchers.end()) {
        CaretLogSevere("Object named \""
                        + name
                       + "\" has already been connected for macros\n"
                       + SystemUtilities::getBackTrace()
                       + "\n");
    }
    else {
        AString errorMessage;
        WuQMacroSignalWatcher* widgetWatcher = WuQMacroSignalWatcher::newInstance(this,
                                                                                  object,
                                                                                  errorMessage);
        if (widgetWatcher != NULL) {
            widgetWatcher->setParent(this);
            m_signalWatchers.insert(std::make_pair(name,
                                                   widgetWatcher));
        }
        else {
            CaretLogWarning(errorMessage);
        }
    }
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
    
    if (isModeRecording()) {
        CaretAssert(m_macroBeingRecorded);
        m_macroBeingRecorded->addMacroCommand(macroCommand);
        return true;
    }
    
    return false;
}

/**
 * Adds a mouse event to the macro that is currently being
 * recorded.  If no macro is being recorded, no action is taken.
 *
 * @param widget
 *     Widget where mouse event occurred
 * @param me
 *     The Qt Mouse Event
 * @return 
 *     True if the mouse event was recorded or false if there is an error.
 */
bool
WuQMacroManager::addMouseEventToRecording(QWidget* widget,
                                          const QMouseEvent* me)
{
    CaretAssert(widget);
    CaretAssert(me);
    
    if (isModeRecording()) {
        const QString name(widget->objectName());
        if (name.isEmpty()) {
            CaretLogSevere("Widget name is empty for recording of mouse event\n"
                           + SystemUtilities::getBackTrace());
            return false;
        }
        CaretAssert(m_macroBeingRecorded);

        WuQMacroMouseEventTypeEnum::Enum mouseEventType = WuQMacroMouseEventTypeEnum::MOVE;
        switch (me->type()) {
            case QEvent::MouseButtonPress:
                mouseEventType = WuQMacroMouseEventTypeEnum::BUTTON_PRESS;
                break;
            case QEvent::MouseButtonRelease:
                mouseEventType = WuQMacroMouseEventTypeEnum::BUTTON_RELEASE;
                break;
            case QEvent::MouseButtonDblClick:
                mouseEventType = WuQMacroMouseEventTypeEnum::DOUBLE_CLICK;
                break;
            case QEvent::MouseMove:
                mouseEventType = WuQMacroMouseEventTypeEnum::MOVE;
                break;
            default:
                CaretAssertMessage(0, ("Unknown mouse event type integer cast="
                                       + QString::number(static_cast<int>(me->type()))));
                break;
        }
        WuQMacroMouseEventInfo* mouseInfo = new WuQMacroMouseEventInfo(mouseEventType,
                                                                       me->localPos().x(),
                                                                       me->localPos().y(),
                                                                       me->windowPos().x(),
                                                                       me->windowPos().y(),
                                                                       me->screenPos().x(),
                                                                       me->screenPos().y(),
                                                                       static_cast<uint32_t>(me->button()),
                                                                       static_cast<uint32_t>(me->buttons()),
                                                                       static_cast<uint32_t>(me->modifiers()),
                                                                       widget->width(),
                                                                       widget->height());
        
        m_macroBeingRecorded->addMacroCommand(new WuQMacroCommand(name,
                                                                  widget->toolTip(),
                                                                  mouseInfo));
        return true;
    }
    
    return false;
}


/**
 * @return Vector containing all Macro Groups.
 */
std::vector<WuQMacroGroup*>
WuQMacroManager::getMacroGroups() const
{
    std::vector<WuQMacroGroup*> macroGroups;
    
    CaretPreferences* preferences = SessionManager::get()->getCaretPreferences();
    CaretAssert(preferences);
    macroGroups.push_back(preferences->getMacros());
    
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
    CaretAssert(m_mode == WuQMacroModeEnum::OFF);
    
    WuQMacroCreateDialog createMacroDialog(parent);
    if (createMacroDialog.exec() == WuQMacroCreateDialog::Accepted) {
        m_mode = WuQMacroModeEnum::RECORDING;
        m_macroBeingRecorded = createMacroDialog.getNewMacro();
        CaretAssert(m_macroBeingRecorded);
    }
}

/**
 * Stop recording the macro.
 */
void
WuQMacroManager::stopRecordingNewMacro()
{
    CaretAssert(m_mode == WuQMacroModeEnum::RECORDING);
    m_mode = WuQMacroModeEnum::OFF;
    m_macroBeingRecorded = NULL;
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
    
    //    switch (macroManager->getMode()) {
    //        case WuQMacroModeEnum::OFF:
    //            break;
    //        case WuQMacroModeEnum::RECORDING:
    //            CaretAssert(0);
    //            break;
    //        case WuQMacroModeEnum::RUNNING:
    //            CaretAssert(0);
    //            break;
    //    }
    
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

void
WuQMacroManager::runMacro(QWidget* widget,
                          const WuQMacro* macro)
{
    CaretAssert(widget);
    CaretAssert(macro);
    
    
    QString errorMessage;
    WuQMacroExecutor executor;
    if ( ! executor.runMacro(macro,
                             widget,
                             true,
                             errorMessage)) {
        QMessageBox::critical(widget,
                              "Run Macro Error",
                              errorMessage,
                              QMessageBox::Ok,
                              QMessageBox::NoButton);
    }
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




