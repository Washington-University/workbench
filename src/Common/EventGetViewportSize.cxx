
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

#define __EVENT_GET_VIEWPORT_SIZE_DECLARE__
#include "EventGetViewportSize.h"
#undef __EVENT_GET_VIEWPORT_SIZE_DECLARE__

#include "CaretAssert.h"
#include "EventTypeEnum.h"

using namespace caret;


    
/**
 * \class caret::EventGetViewportSize 
 * \brief Event to get the viewport size for a tab.
 * \ingroup GuiQt
 */

/**
 * Constructor for finding a specific tab index.
 *
 * @param tabIndex
 *     Index of tab for which viewport size is requested.
 */
//EventGetViewportSize::EventGetViewportSize(const int32_t tabIndex)
//: Event(EventTypeEnum::EVENT_BROWSER_TAB_GET_VIEWPORT_SIZE),
//m_mode(MODE_TAB_INDEX),
//m_tabIndex(tabIndex),
//m_viewportValid(false)
//{
//    m_viewport[0] = 0;
//    m_viewport[1] = 0;
//    m_viewport[2] = 0;
//    m_viewport[3] = 0;
//}

/**
 * Constructor for finding a surface or volume montage.
 *
 * @param mode
 *     The mode.
 */
EventGetViewportSize::EventGetViewportSize(const Mode mode,
                                                               const int32_t index)
: Event(EventTypeEnum::EVENT_GET_VIEWPORT_SIZE),
m_mode(mode),
m_index(index),
m_viewportValid(false)
{
    m_viewport[0] = 0;
    m_viewport[1] = 0;
    m_viewport[2] = 0;
    m_viewport[3] = 0;
}

/**
 * Destructor.
 */
EventGetViewportSize::~EventGetViewportSize()
{
}

/**
 * @return Is the viewport size valid (width and height greater than zero)?
 */
bool
EventGetViewportSize::isViewportSizeValid() const
{
    return m_viewportValid;
}

/**
 * @return The mode.
 */
EventGetViewportSize::Mode
EventGetViewportSize::getMode() const
{
    return m_mode;
}

/**
 * @return The tab/window index for which the viewport size is requested.
 */
int32_t
EventGetViewportSize::getIndex() const
{
    return m_index;
}

/**
 * Get the viewport size.
 *
 * @param viewportOut
 *     Output containing viewport x, y, width, height.
 */
void
EventGetViewportSize::getViewportSize(int32_t viewportOut[4]) const
{
    viewportOut[0] = m_viewport[0];
    viewportOut[1] = m_viewport[1];
    viewportOut[2] = m_viewport[2];
    viewportOut[3] = m_viewport[3];
}

/**
 * Set the viewport size.
 *
 * @param viewport
 *     Viewport x, y, width, height.
 */
void
EventGetViewportSize::setViewportSize(const int32_t viewport[4])
{
    m_viewportValid = false;
    
    m_viewport[0] = viewport[0];
    m_viewport[1] = viewport[1];
    m_viewport[2] = viewport[2];
    m_viewport[3] = viewport[3];
    
    if ((m_viewport[2] > 0)
        && (m_viewport[3] > 0)) {
        m_viewportValid = true;
    }
}

