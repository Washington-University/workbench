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

#include "EventGetModelToDrawForWindow.h"

using namespace caret;

/**
 * Constructor.
 */
EventGetModelToDrawForWindow::EventGetModelToDrawForWindow(const int32_t windowIndex)
: Event(EventTypeEnum::EVENT_GET_MODEL_TO_DRAW_FOR_WINDOW)
{
    this->windowIndex = windowIndex;
    this->windowTabIndex = -1;
    this->modelDisplayController = NULL;
}

/*
 * Destructor.
 */
EventGetModelToDrawForWindow::~EventGetModelToDrawForWindow()
{
    
}

/**
 * Get the index of the window that is to be updated.
 */
int32_t 
EventGetModelToDrawForWindow::getWindowIndex() const 
{ 
    return this->windowIndex; 
}

/**
 * Get window tab index that is being drawn
 */
int32_t 
EventGetModelToDrawForWindow::getWindowTabIndex() const 
{ 
    return this->windowTabIndex; 
}

/**
 * Get the model controller being drawn
 */
ModelDisplayController* 
EventGetModelToDrawForWindow::getModelDisplayController() 
{ 
    return this->modelDisplayController; 
}

/**
 * Set the window tab index.
 *
 * @param windowTabIndex
 *     Index of tab window.
 */
void 
EventGetModelToDrawForWindow::setWindowTabIndex(const int32_t windowTabIndex)
{
    this->windowTabIndex = windowTabIndex;
}

/**
 * Set the model display controller.
 *
 * @param modelDisplayController
 *    The model display controller.
 */
void 
EventGetModelToDrawForWindow::setModelDisplayController(ModelDisplayController* modelDisplayController)
{
    this->modelDisplayController = modelDisplayController;   
}
