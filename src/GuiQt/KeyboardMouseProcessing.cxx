/*LICENSE_START*/ 
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


#include "GuiOpenGLKeyboardMouseProcessing.h"

#include "BrainOpenGLDataSelectionManager.h"
#include "GuiOpenGL.h"
#include "GuiOpenGLMouseEvent.h"
#include "KeyEvent.h"
#include "Viewer.h"

using namespace caret;

/**
 * Destructor
 */
GuiOpenGLKeyboardMouseProcessing::~GuiOpenGLKeyboardMouseProcessing()
{
}

/**
 * Copy Constructor
 * @param Object that is copied.
 */
GuiOpenGLKeyboardMouseProcessing::GuiOpenGLKeyboardMouseProcessing(const GuiOpenGLKeyboardMouseProcessing& o)
    : CaretObject(o)
{
    this->initializeMembersGuiOpenGLKeyboardMouseProcessing();
    this->copyHelper(o);
}

/**
 * Assignment operator.
 */
GuiOpenGLKeyboardMouseProcessing&
GuiOpenGLKeyboardMouseProcessing::operator=(const GuiOpenGLKeyboardMouseProcessing& o)
{
    if (this != &o) {
        CaretObject::operator=(o);
        this->copyHelper(o);
    };
    return *this;
}

/**
 * Helps with copy constructor and assignment operator.
 */
void
GuiOpenGLKeyboardMouseProcessing::copyHelper(const GuiOpenGLKeyboardMouseProcessing& o)
{
}

void
GuiOpenGLKeyboardMouseProcessing::initializeMembersGuiOpenGLKeyboardMouseProcessing()
{
}
/**
 * Receives mouse events from OpenGL brain renderer.
 * @param guiOpenGL - OpenGL window in which event occurred.
 * @param mouseEvent - The mouse event.
 * @param dataSelectionManager - Used for data selection.
 * @param viewer - The viewer on which event occurred.
 *
 */
void
GuiOpenGLKeyboardMouseProcessing::processMouseEvent(
                   const GuiOpenGL& guiOpenGL,
                   const GuiOpenGLMouseEvent& mouseEvent,
                   const BrainOpenGLDataSelectionManager& dataSelectionManager,
                   const Viewer& viewer)
{
}

/**
 * Receives keyboard events from OpenGL brain renderer.
 * @param guiOpenGL - OpenGL window in which event occurred.
 * @param ke - The Java Key Event.
 * @param viewer - The viewer on which event occurred.
 *
 */
void
GuiOpenGLKeyboardMouseProcessing::processKeyboardEvent(
                   const GuiOpenGL& guiOpenGL,
                   const KeyEvent& ke,
                   const Viewer& viewer)
{
}

/**
 * Get the labels for the status bar.
 *
 * @return  The labels for the status bar.
 *
 */
GuiStatusBarLabels
GuiOpenGLKeyboardMouseProcessing::getStatusBarLabels() const
{
}

/**
 * Called when the mouse and keyboard processing is switched
 * to this keyboard and mouse processing.
 *
 */
void
GuiOpenGLKeyboardMouseProcessing::initializeKeyboardMouseProcessing()
{
}

/**
 * Called when the mouse and keyboard is changing from this
 * keyboard and mouse processing to another.
 *
 */
void
GuiOpenGLKeyboardMouseProcessing::finishKeyboardMouseProcessing()
{
}

