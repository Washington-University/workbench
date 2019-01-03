
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
#include <QKeyEvent>
#include <QMessageBox>
#include <QMouseEvent>
#include <QWidget>

#include "CaretAssert.h"
#include "CaretLogger.h"
#include "CaretFileDialog.h"
#include "DataFileException.h"
#include "WuQMacro.h"
#include "WuQMacroCommand.h"
#include "WuQMacroCreateDialog.h"
#include "WuQMacroDialog.h"
#include "WuQMacroExecutor.h"
#include "WuQMacroFile.h"
#include "WuQMacroGroup.h"
#include "WuQMacroMouseEventInfo.h"
#include "WuQMacroAttributesDialog.h"
#include "WuQMacroHelperInterface.h"
#include "WuQMacroEditorDialog.h"
#include "WuQMacroExecutorOptions.h"
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
    m_executorOptions.reset(new WuQMacroExecutorOptions());
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
    }
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
 * @param toolTipTextOverride
 *     Override of object's tooltip.  This is primarily used when
 *     an object of a particular class does not support a tooltip
 *     such as a QButtonGroup.  This can also be empty to avoid
 *     the "no tooltip" message.
 */
bool
WuQMacroManager::addMacroSupportToObjectWithToolTip(QObject* object,
                                        const QString& toolTipOverride)
{
    CaretAssert(object);
    
    const QString name = object->objectName();
    if (name.isEmpty()) {
        CaretLogSevere("Object name is empty, will be ignored for macros\n"
                       + SystemUtilities::getBackTrace());
        return false;
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
 * @param toolTipTextOverride
 *     Override of object's tooltip.  This is primarily used when
 *     an object of a particular class does not support a tooltip
 *     such as a QButtonGroup
 */
bool
WuQMacroManager::addMacroSupportToObject(QObject* object)
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
    
    const bool resultFlag = addMacroSupportToObjectWithToolTip(object,
                                                               toolTipText);
    if (resultFlag) {
        
        if (toolTipText.isEmpty()) {
            CaretLogWarning("Object named \""
                            + object->objectName()
                            + "\" is missing a tooltip");
        }
    }
    
    return resultFlag;
    
//    const QString name = object->objectName();
//    if (name.isEmpty()) {
//        CaretLogSevere("Object name is empty, will be ignored for macros\n"
//                       + SystemUtilities::getBackTrace());
//        return false;
//    }
//    
//    
//    {
//        QString toolTipText;
//        if (toolTipTextOverride.isEmpty()) {
//            QAction* action = qobject_cast<QAction*>(object);
//            if (action != NULL) {
//                toolTipText = action->toolTip();
//            }
//            QWidget* widget = qobject_cast<QWidget*>(object);
//            if (widget != NULL) {
//                toolTipText = widget->toolTip();
//            }
//            if (toolTipText.isEmpty()) {
//                CaretLogWarning("Object named \""
//                                + name
//                                + "\" is missing a tooltip");
//            }
//        }
//        else {
//            toolTipText = toolTipTextOverride;
//        }
//    }
//    auto existingWatcher = m_signalWatchers.find(name);
//    if (existingWatcher != m_signalWatchers.end()) {
//        CaretLogSevere("Object named \""
//                        + name
//                       + "\" has already been connected for macros\n"
//                       + SystemUtilities::getBackTrace()
//                       + "\n");
//    }
//    else {
//        AString errorMessage;
//        WuQMacroSignalWatcher* widgetWatcher = WuQMacroSignalWatcher::newInstance(this,
//                                                                                  object,
//                                                                                  errorMessage);
//        if (widgetWatcher != NULL) {
//            widgetWatcher->setParent(this);
//            m_signalWatchers.insert(std::make_pair(name,
//                                                   widgetWatcher));
//        }
//        else {
//            CaretLogWarning(errorMessage);
//        }
//    }
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
                                                                           me->localPos().x(),
                                                                           me->localPos().y(),
                                                                           static_cast<uint32_t>(me->button()),
                                                                           static_cast<uint32_t>(me->buttons()),
                                                                           static_cast<uint32_t>(me->modifiers()),
                                                                           widget->width(),
                                                                           widget->height());
        
            m_macroBeingRecorded->addMacroCommand(new WuQMacroCommand(name,
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
    if (m_macroHelper) {
        macroGroups = m_macroHelper->getMacroGroups();
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
    
    if (m_macroHelper) {
        m_macroHelper->macroWasModified(m_macroBeingRecorded);
    }
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
 * @param macro
 *     Macro that is run
 */
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
                             m_parentObjects,
                             m_executorOptions.get(),
                             errorMessage)) {
        QMessageBox::critical(widget,
                              "Run Macro Error",
                              errorMessage,
                              QMessageBox::Ok,
                              QMessageBox::NoButton);
    }
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
                if (m_macroHelper) {
                    m_macroHelper->macroWasModified(macro);
                }
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
                if (m_macroHelper) {
                    m_macroHelper->macroWasModified(macro);
                }
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
                         macro);
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
    
    const auto macroGroups = getMacroGroups();
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



