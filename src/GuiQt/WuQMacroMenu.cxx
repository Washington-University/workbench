
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
    
    setTitle("Macro New");
    
    m_macroDialogAction = addAction("Macros...");
    QObject::connect(m_macroDialogAction, &QAction::triggered,
                     this, &WuQMacroMenu::macroDialogSelected);
    
    m_recordMacroAction = addAction("Record Macro...");
    QObject::connect(m_recordMacroAction, &QAction::triggered,
                     this, &WuQMacroMenu::macroRecordSelected);
    
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
    
    bool editValidFlag(false);
    bool recordValidFlag(false);
    QString recordText;
    
    switch (macroManager->getMode()) {
        case WuQMacroModeEnum::OFF:
            editValidFlag = true;
            recordText = "Record Macro...";
            recordValidFlag = true;
            break;
        case WuQMacroModeEnum::RECORDING:
            recordText = "Stop Recording";
            recordValidFlag = true;
            break;
        case WuQMacroModeEnum::RUNNING:
            recordText = "Record Macro...";
            break;
    }
    
    m_macroDialogAction->setEnabled(editValidFlag);
    m_recordMacroAction->setEnabled(recordValidFlag);
    m_recordMacroAction->setText(recordText);
}

/**
 * Called to start/stop macro recording.
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
        case WuQMacroModeEnum::RECORDING:
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
