
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

#define __EVENT_DRAWING_VIEWPORT_CONTENT_ADD_DECLARE__
#include "EventDrawingViewportContentAdd.h"
#undef __EVENT_DRAWING_VIEWPORT_CONTENT_ADD_DECLARE__

#include "CaretAssert.h"
#include "EventTypeEnum.h"

using namespace caret;


    
/**
 * \class caret::EventDrawingViewportContentAdd 
 * \brief Event to associate a drawing viewport with a window
 * \ingroup Brain
 */

/**
 * Constructor.
 * @param drawingViewportContent
 *    Drawing viewport content for association with a window
 */
EventDrawingViewportContentAdd::EventDrawingViewportContentAdd(DrawingViewportContentBase* drawingViewportContent)
: Event(EventTypeEnum::EVENT_DRAWING_VIEWPORT_CONTENT_ADD),
m_drawingViewportContent(drawingViewportContent)
{
    
}

/**
 * Destructor.
 */
EventDrawingViewportContentAdd::~EventDrawingViewportContentAdd()
{
}

/**
 * @return Drawing viewport content for association with a window
 */
DrawingViewportContentBase*
EventDrawingViewportContentAdd::getDrawingViewportContent() const
{
    return m_drawingViewportContent;
}

