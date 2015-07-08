
/*LICENSE_START*/
/*
 *  Copyright (C) 2015 Washington University School of Medicine
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

#define __EVENT_IDENTIFICATION_REQUEST_DECLARE__
#include "EventIdentificationRequest.h"
#undef __EVENT_IDENTIFICATION_REQUEST_DECLARE__

#include "CaretAssert.h"
#include "EventTypeEnum.h"

using namespace caret;


    
/**
 * \class caret::EventIdentificationRequest 
 * \brief Perform an identification operation in a window
 * \ingroup GuiQt
 */

/**
 * Constructor.
 */
EventIdentificationRequest::EventIdentificationRequest(const int32_t windowIndex,
                                                       const int32_t windowX,
                                                       const int32_t windowY)
: Event(EventTypeEnum::EVENT_IDENTIFICATION_REQUEST),
m_windowIndex(windowIndex),
m_windowX(windowX),
m_windowY(windowY)
{
    m_selectionManager = NULL;
}

/**
 * Destructor.
 */
EventIdentificationRequest::~EventIdentificationRequest()
{
}

/**
 * @return Selection manager (NULL if event failed).
 */
SelectionManager*
EventIdentificationRequest::getSelectionManager() const
{
    return m_selectionManager;
}

/**
 * Set the selection manager.
 *
 * @param selectionManager
 *     The selection manager.
 */
void
EventIdentificationRequest::setSelectionManager(SelectionManager* selectionManager)
{
    m_selectionManager = selectionManager;
}

/**
 * @return Index of window in which identification is performed.
 */
int32_t
EventIdentificationRequest::getWindowIndex() const
{
    return m_windowIndex;
}

/**
 * @return X window coordinate for identification.
 */
int32_t
EventIdentificationRequest::getWindowX() const
{
    return m_windowX;
}

/**
 * @return Y window coordinate for identification.
 */
int32_t
EventIdentificationRequest::getWindowY() const
{
    return m_windowY;
}


