
/*LICENSE_START*/
/*
 *  Copyright (C) 2014 Washington University School of Medicine
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

#define __USER_INPUT_MODE_ABSTRACT_DECLARE__
#include "UserInputModeAbstract.h"
#undef __USER_INPUT_MODE_ABSTRACT_DECLARE__

#include <iostream>

#include <QWidget>

#include "CaretAssert.h"
using namespace caret;


    
/**
 * \class caret::UserInputModeAbstract 
 * \brief Abstract class for processing user input events
 * \ingroup GuiQt
 *
 * Classes implementing this interface receive
 * user input events from the OpenGL graphics
 * region of a BrowserWindow containing brain
 * models.
 */

/**
 * Constructor.
 */
UserInputModeAbstract::UserInputModeAbstract(const UserInputMode inputMode)
: CaretObject(),
m_userInputMode(inputMode),
m_widgetForToolBar(NULL)
{
    
}

/**
 * Destructor.
 */
UserInputModeAbstract::~UserInputModeAbstract()
{
    /*
     * If the widget does not have a parent, then it is not 
     * displayed (owned by another QWidget class) and must
     * be destroyed to avoid a memory leak.
     */
    if (m_widgetForToolBar != NULL) {
        if (m_widgetForToolBar->parent() == 0) {
            delete m_widgetForToolBar;
        }
        m_widgetForToolBar = NULL;
    }
}

/**
 * @return The input mode enumerated type.
 */
UserInputModeAbstract::UserInputMode
UserInputModeAbstract::getUserInputMode() const
{
    return m_userInputMode;
}

/**
 * @return A widget for display at the bottom of the
 * Browser Window Toolbar when this mode is active.
 * If no user-interface controls are needed, this
 * method will return NULL.
 */
QWidget*
UserInputModeAbstract::getWidgetForToolBar()
{
    return m_widgetForToolBar;
}

/**
 * Set the widget that is displayed in the toolbar when 
 * the user input mode is active.
 *
 * @param widgetForToolBar
 *   Widget that is displayed in toolbar, may be NULL indicating
 *   no widget.
 */
void
UserInputModeAbstract::setWidgetForToolBar(QWidget* widgetForToolBar)
{
    m_widgetForToolBar = widgetForToolBar;
}

/**
 * Process a selection that was made from the browser window's edit menu.
 * Intended for override by sub-classes.
 *
 * @param editMenuItem
 *     Item that was selected from the edit menu.
 */
void
UserInputModeAbstract::processEditMenuItemSelection(const BrainBrowserWindowEditMenuItemEnum::Enum /*editMenuItem*/)
{
    
}

/**
 * Get the menu items that should be enabled for the current user input processor.
 * Intended for override by sub-classes.
 * Unless this method is overridden, all items on Edit menu are disabled.
 *
 * @param enabledEditMenuItemsOut
 *     Upon exit contains edit menu items that should be enabled.
 * @param redoMenuItemSuffixTextOut
 *     If the redo menu is enabled, the contents of string becomes
 *     the suffix for the 'Redo' menu item.
 * @param undoMenuItemSuffixTextOut
 *     If the undo menu is enabled, the contents of string becomes
 *     the suffix for the 'Undo' menu item.
 */
void
UserInputModeAbstract::getEnabledEditMenuItems(std::vector<BrainBrowserWindowEditMenuItemEnum::Enum>& enabledEditMenuItemsOut,
                                               AString& redoMenuItemSuffixTextOut,
                                               AString& undoMenuItemSuffixTextOut)
{
    enabledEditMenuItemsOut.clear();
    redoMenuItemSuffixTextOut = "";
    undoMenuItemSuffixTextOut = "";
}


