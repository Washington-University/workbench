
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

#define __WU_Q_MACRO_MENU_DECLARE__
#include "WuQMacroMenu.h"
#undef __WU_Q_MACRO_MENU_DECLARE__

#include "CaretAssert.h"
#include "WuQMacroManager.h"

using namespace caret;


    
/**
 * \class caret::WuQMacroMenu 
 * \brief Menu for accessing and interacting with the Macro System.
 * \ingroup WuQMacro
 */

/**
 * Constructor.
 *
 * @param windowParent
 *    Window used as parent for dialogs
 * @param parent
 *    Parent of the menu
 */
WuQMacroMenu::WuQMacroMenu(QWidget* windowParent,
                           QWidget* parent)
: QMenu(parent),
m_windowParent(windowParent)
{
    CaretAssert(windowParent);
    
    setTitle("Macro");
    
    m_macroDialogAction = addAction("Macros...");
    QObject::connect(m_macroDialogAction, &QAction::triggered,
                     this, &WuQMacroMenu::macroDialogSelected);
    
    m_recordMacroAction = addAction("Start Recording New Macro...");
    QObject::connect(m_recordMacroAction, &QAction::triggered,
                     this, &WuQMacroMenu::macroRecordSelected);
    
    m_stopMacroAction = addAction("Stop Recording New Macro");
    QObject::connect(m_stopMacroAction, &QAction::triggered,
                     this, &WuQMacroMenu::macroStopSelected);

    {
        QMenu* developmentMenu = new QMenu("Development");
        QAction* printAction = developmentMenu->addAction("Print Supported Widgets");
        QObject::connect(printAction, &QAction::triggered,
                         this, &WuQMacroMenu::macroPrintAllSelected);
        QAction* printTopLevelWidgetsAction = developmentMenu->addAction("Print Top-Level Widgets");
        QObject::connect(printTopLevelWidgetsAction, &QAction::triggered,
                         this, &WuQMacroMenu::macroPrintTopLevelWidgets);

        addSeparator();
        addMenu(developmentMenu);
    }
    
    QObject::connect(this, &QMenu::aboutToShow,
                     this, &WuQMacroMenu::macroMenuAboutToShow);
}

/**
 * Destructor.
 */
WuQMacroMenu::~WuQMacroMenu()
{
}

/**
 * Called when the macro menu is about to show.
 */
void
WuQMacroMenu::macroMenuAboutToShow()
{
    WuQMacroManager* macroManager = WuQMacroManager::instance();
    CaretAssert(macroManager);
    
    const bool hasMacroGroupFlag = ( ! macroManager->getActiveMacroGroups().empty());
    bool editValidFlag(false);
    bool recordValidFlag(false);
    bool stopValidFlag(false);
    
    switch (macroManager->getMode()) {
        case WuQMacroModeEnum::OFF:
            editValidFlag = true;
            recordValidFlag = hasMacroGroupFlag;
            break;
        case WuQMacroModeEnum::RECORDING_INSERT_COMMANDS:
        case WuQMacroModeEnum::RECORDING_NEW_MACRO:
            stopValidFlag = true;
            break;
        case WuQMacroModeEnum::RUNNING:
            break;
    }
    
    m_macroDialogAction->setEnabled(editValidFlag);
    m_recordMacroAction->setEnabled(recordValidFlag);
    m_stopMacroAction->setEnabled(stopValidFlag);
}

/**
 * Called to start macro recording.
 */
void
WuQMacroMenu::macroRecordSelected()
{
    WuQMacroManager* macroManager = WuQMacroManager::instance();
    CaretAssert(macroManager);
    
    switch (macroManager->getMode()) {
        case WuQMacroModeEnum::OFF:
            macroManager->startRecordingNewMacro(m_windowParent);
            break;
        case WuQMacroModeEnum::RECORDING_INSERT_COMMANDS:
            CaretAssert(0);
            break;
        case WuQMacroModeEnum::RECORDING_NEW_MACRO:
            CaretAssert(0);
            break;
        case WuQMacroModeEnum::RUNNING:
            CaretAssert(0);
            break;
    }
}

/**
 * Called to stop macro recording.
 */
void
WuQMacroMenu::macroStopSelected()
{
    WuQMacroManager* macroManager = WuQMacroManager::instance();
    CaretAssert(macroManager);
    
    switch (macroManager->getMode()) {
        case WuQMacroModeEnum::OFF:
            CaretAssert(0);
            break;
        case WuQMacroModeEnum::RECORDING_INSERT_COMMANDS:
        case WuQMacroModeEnum::RECORDING_NEW_MACRO:
            macroManager->stopRecordingNewMacro();
            break;
        case WuQMacroModeEnum::RUNNING:
            CaretAssert(0);
            break;
    }
}

/**
 * Called to display the macro editor dialog.
 */
void
WuQMacroMenu::macroDialogSelected()
{
    WuQMacroManager::instance()->showMacrosDialog(m_windowParent);
}

/**
 * Print all watched widgets to terminal
 */
void
WuQMacroMenu::macroPrintAllSelected()
{
    WuQMacroManager::instance()->printSupportedWidgetsToTerminal();
}

/**
 * Print all top level widgets name and class name
 */
void
WuQMacroMenu::macroPrintTopLevelWidgets()
{
    WuQMacroManager::instance()->printToLevelWidgetsToTerminal();
}
