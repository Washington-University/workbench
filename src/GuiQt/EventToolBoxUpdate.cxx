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

#include "CaretAssert.h"
#include "EventToolBoxUpdate.h"

using namespace caret;

/**
 * Constructor for updating the toolbox.
 */
EventToolBoxUpdate::EventToolBoxUpdate()
: Event(EventTypeEnum::EVENT_TOOLBOX_UPDATE)
{
    m_windowIndex = -1;
}

/*
 * Destructor.
 */
EventToolBoxUpdate::~EventToolBoxUpdate()
{
    
}

/**
 * @return Is the update for the given window?
 *
 * @param windowIndex
 *     Index of window.
 */
bool 
EventToolBoxUpdate::isUpdateForWindow(const int32_t windowIndex) const
{
    if (m_windowIndex < 0) {
        return true;
    }
    else if (m_windowIndex == windowIndex) {
        return true;
    }
    return false;
}

/**
 * Set the update so that it only updates a specific window.
 * 
 * @return A reference to the instance so that the
 * request update calls can be chained.
 */
EventToolBoxUpdate& 
EventToolBoxUpdate::setWindowIndex(const int32_t windowIndex)
{
    m_windowIndex = windowIndex;
    return *this;
}

