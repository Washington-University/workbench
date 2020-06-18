
/*LICENSE_START*/
/*
 *  Copyright (C) 2020 Washington University School of Medicine
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

#define __EVENT_GRAPHICS_WINDOW_SHOW_TOOL_TIP_DECLARE__
#include "EventGraphicsWindowShowToolTip.h"
#undef __EVENT_GRAPHICS_WINDOW_SHOW_TOOL_TIP_DECLARE__

#include "CaretAssert.h"
#include "EventTypeEnum.h"

using namespace caret;


    
/**
 * \class caret::EventGraphicsWindowShowToolTip 
 * \brief Show a toolip in the graphics region of a window
 * \ingroup GuiQt
 */

/**
 * Constructor.
 * @param windowIndex
 *    Index of the window
 * @param windowOrigin
 *    Location of origin in window for coordinate
 * @param windowXYZ
 *    The coordinate in window for display of tooltip
 * @param text
 *    Text to display in tooltip
 */
EventGraphicsWindowShowToolTip::EventGraphicsWindowShowToolTip(const int32_t windowIndex,
                                                               const WindowOrigin windowOrigin,
                                                               const std::array<float,3>& windowXYZ,
                                                               const QString& text)
: Event(EventTypeEnum::EVENT_GRAPHICS_WINDOW_SHOW_TOOL_TIP),
m_windowIndex(windowIndex),
m_windowOrigin(windowOrigin),
m_windowXYZ(windowXYZ),
m_text(text)
{
    
}

/**
 * Destructor.
 */
EventGraphicsWindowShowToolTip::~EventGraphicsWindowShowToolTip()
{
}

/**
 * @return Origin of window coordinate
 */
EventGraphicsWindowShowToolTip::WindowOrigin
EventGraphicsWindowShowToolTip::getWindowOrigin() const
{
    return m_windowOrigin;
}

/**
 * @return Index of window in which to show tooltip
 */
int32_t
EventGraphicsWindowShowToolTip::getWindowIndex() const
{
    return m_windowIndex;
}

/**
 * @return Coordinate for placement of tooltip
 */
std::array<float,3>
EventGraphicsWindowShowToolTip::getWindowXYZ() const
{
    return m_windowXYZ;
}

/**
 * @return Text displayed in tooltip
 */
QString
EventGraphicsWindowShowToolTip::getText() const
{
    return m_text;
}


