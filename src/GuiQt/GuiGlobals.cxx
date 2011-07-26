

/*
 *  Copyright 1995-2002 Washington University School of Medicine
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
/*LICENSE_END*/

#include "Actions.h"
#include "Brain.h"

#define __GUI_GLOBALS_DEFINE__
#include "GuiGlobals.h"
#undef __GUI_GLOBALS_DEFINE__

using namespace caret;

/**
 * Get the brain.
 *
 * @return 
 *    Point to the brain.
 */
Brain* 
GuiGlobals::getBrain()
{
    if (GuiGlobals::brain == NULL) {
        GuiGlobals::brain = new Brain();
    }
    return GuiGlobals::brain;
}

/**
 * Get the main window.
 * 
 * @return 
 *    Pointer to the main window.
 */
WindowMain* 
GuiGlobals::getMainWindow()
{
    return GuiGlobals::mainWindow;
}

/**
 * Set the main window.
 * 
 * @param mainWindow
 *    Pointer to the main window.
 */
void 
GuiGlobals::setMainWindow(WindowMain* mainWindow)
{
    GuiGlobals::mainWindow = mainWindow;
}

Actions*
GuiGlobals::getActions()
{
    if (GuiGlobals::actions == NULL) {
        GuiGlobals::actions = new Actions(GuiGlobals::getMainWindow());
    }
    return GuiGlobals::actions;
}