
/*LICENSE_START*/
/*
 *  Copyright (C) 2023 Washington University School of Medicine
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

#define __EVENT_DRAWING_VIEWPORT_CONTENT_CLEAR_DECLARE__
#include "EventDrawingViewportContentClear.h"
#undef __EVENT_DRAWING_VIEWPORT_CONTENT_CLEAR_DECLARE__

#include "CaretAssert.h"
#include "EventTypeEnum.h"

using namespace caret;


    
/**
 * \class caret::EventDrawingViewportContentClear 
 * \brief Event to clear all drawing viewports associated with a window
 * \ingroup Brain
 */

/**
 * Constructor.
 */
EventDrawingViewportContentClear::EventDrawingViewportContentClear(const int32_t windowIndex)
: Event(EventTypeEnum::EVENT_DRAWING_VIEWPORT_CONTENT_CLEAR),
m_windowIndex(windowIndex)
{
    
}

/**
 * Destructor.
 */
EventDrawingViewportContentClear::~EventDrawingViewportContentClear()
{
}

/**
 * @return Index of the window
 */
int32_t
EventDrawingViewportContentClear::getWindowIndex() const
{
    return m_windowIndex;
}

