
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
#include "CaretFileDialog.h"
#include "CaretPreferences.h"
#include "DataFileException.h"
#include "SessionManager.h"
#include "WuQMacro.h"
#include "WuQMacroCommand.h"
#include "WuQMacroCreateDialog.h"
#include "WuQMacroDialog.h"
#include "WuQMacroExecutor.h"
#include "WuQMacroFile.h"
#include "WuQMacroGroup.h"
#include "WuQMacroMouseEventInfo.h"
#include "WuQMacroAttributesDialog.h"
#include "WuQMacroEditorDialog.h"
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
                mouseEventType = WuQMacroMouseEventTypeEnum::DOUBLE_CLICK;
                break;
            case QEvent::MouseMove:
                mouseEventType = WuQMacroMouseEventTypeEnum::MOVE;
                
                /*
                 * Only track move events if a button is down
                 */
                if (me->button() == Qt::NoButton) {
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
                                                                           me->localPos().x(),
                                                                           me->localPos().y(),
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
    
    CaretPreferences* preferences = SessionManager::get()->getCaretPreferences();
    CaretAssert(preferences);
    preferences->writeMacros();
    
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
 * @param runOptions
 *     Options used when running the macro
 * @param macro
 *     Macro that is run
 */
void
WuQMacroManager::runMacro(QWidget* widget,
                          const WuQMacroExecutor::RunOptions& runOptions,
                          const WuQMacro* macro)
{
    CaretAssert(widget);
    CaretAssert(macro);
    
    
    QString errorMessage;
    WuQMacroExecutor executor;
    if ( ! executor.runMacro(macro,
                             widget,
                             runOptions,
                             errorMessage)) {
        QMessageBox::critical(widget,
                              "Run Macro Error",
                              errorMessage,
                              QMessageBox::Ok,
                              QMessageBox::NoButton);
    }
}

/**
 * Edit a macros attributes (name, description)
 *
 * @param parent
 *     Parent widget for dialog
 * @param macro
 *     Macro to edit
 * @return
 *     True if macro was modified.
 */
bool
WuQMacroManager::editMacroAttributes(QWidget* parent,
                                  WuQMacro* macro)
{
    if (macro != NULL) {
        WuQMacroAttributesDialog attributesDialog(macro,
                                               parent);
        if (attributesDialog.exec() == WuQMacroDialog::Accepted) {
            if (attributesDialog.isMacroModified()) {
                CaretPreferences* preferences = SessionManager::get()->getCaretPreferences();
                CaretAssert(preferences);
                preferences->writeMacros();
                return true;
            }
        }
    }
    
    return false;
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
    if (QMessageBox::warning(parent,
                             "Warning",
                             ("Delete the macro " + macro->getName()),
                             QMessageBox::Ok | QMessageBox::Cancel,
                             QMessageBox::Ok) == QMessageBox::Ok) {
        macroGroup->deleteMacro(macro);
        CaretPreferences* preferences = SessionManager::get()->getCaretPreferences();
        CaretAssert(preferences);
        preferences->writeMacros();
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
    const QString filename = CaretFileDialog::getOpenFileNameDialog(parent,
                                                                    "Import Macros",
                                                                    "",
                                                                    fileFilterString,
                                                                    &fileFilterString);
    if ( ! filename.isEmpty()) {
        WuQMacroFile macroFile;
        try {
            macroFile.readFile(filename);
            
            const WuQMacroGroup* fileMacroGroup = macroFile.getMacroGroup();
            if (fileMacroGroup->getNumberOfMacros() > 0) {
                appendToMacroGroup->appendMacroGroup(fileMacroGroup);
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
    const QString filename = CaretFileDialog::getSaveFileNameDialog(parent,
                                                                    "Export Macros",
                                                                    "",
                                                                    fileFilterString,
                                                                    &fileFilterString);
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
            
            macroFile.writeFile(filename);
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
 * Edit a macros commands
 *
 * @param parent
 *     Parent widget for dialog
 * @param macro
 *     Macro to edit
 * @return
 *     True if macro was modified.
 */
bool
WuQMacroManager::editMacroCommands(QWidget* parent,
                                   WuQMacro* macro)
{
    if (macro != NULL) {
        WuQMacroEditorDialog editorDialog(macro,
                                          parent);
        if (editorDialog.exec() == WuQMacroDialog::Accepted) {
            if (editorDialog.isMacroModified()) {
                CaretPreferences* preferences = SessionManager::get()->getCaretPreferences();
                CaretAssert(preferences);
                preferences->writeMacros();
                return true;
            }
        }
    }
    
    return false;
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




