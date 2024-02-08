
/*LICENSE_START*/
/*
 *  Copyright (C) 2024 Washington University School of Medicine
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

#define __EVENT_BROWSER_WINDOW_PIXEL_SIZE_INFO_EVENT_DECLARE__
#include "EventBrowserWindowPixelSizeInfoEvent.h"
#undef __EVENT_BROWSER_WINDOW_PIXEL_SIZE_INFO_EVENT_DECLARE__

#include "CaretAssert.h"
#include "EventTypeEnum.h"

using namespace caret;


    
/**
 * \class caret::EventBrowserWindowPixelSizeInfoEvent 
 * \brief Get info about a window's pixel sizes
 * \ingroup Common
 */

/**
 * Constructor for using a widget to get the pixel size info from the widget's screen() method.
 * @param widget
 *    The Qt Widget
 */
EventBrowserWindowPixelSizeInfoEvent::EventBrowserWindowPixelSizeInfoEvent(QWidget* widget)
: Event(EventTypeEnum::EVENT_BROWSER_WINDOW_PIXEL_SIZE_INFO),
m_mode(Mode::WIDGET_POINTER),
m_widget(widget)
{
    
}

/**
 * Constructor for using a browser window's index to use the window's screen() method.
 * @param windowIndex
 *    Index of the window.
 */
EventBrowserWindowPixelSizeInfoEvent::EventBrowserWindowPixelSizeInfoEvent(const int32_t windowIndex)
: Event(EventTypeEnum::EVENT_BROWSER_WINDOW_PIXEL_SIZE_INFO),
m_mode(Mode::WINDOW_INDEX),
m_windowIndex(windowIndex)
{
    
}

/**
 * Destructor.
 */
EventBrowserWindowPixelSizeInfoEvent::~EventBrowserWindowPixelSizeInfoEvent()
{
}

EventBrowserWindowPixelSizeInfoEvent::Mode
EventBrowserWindowPixelSizeInfoEvent::getMode() const
{
    return m_mode;
}

/**
 * @return The Qt widget for when the mode is WIDGET_POINTER
 */
QWidget*
EventBrowserWindowPixelSizeInfoEvent::getWidget() const
{
    return m_widget;
}

/**
 * @return Index of the window for when the mode is WINDOW_INDEX
 */
int32_t
EventBrowserWindowPixelSizeInfoEvent::getWindowIndex() const
{
    return m_windowIndex;
}

/**
 * @return Logical dots per inch (valid if > 0)
 */
float
EventBrowserWindowPixelSizeInfoEvent::getLogicalDotsPerInch() const
{
    return m_logicalDotsPerInch;
}

/**
 * Set the logical dots per inch.
 * @param logicalDotsPerInch
 *    New value for dots per inch
 */
void
EventBrowserWindowPixelSizeInfoEvent::setLogicalDotsPerInch(const float logicalDotsPerInch)
{
    m_logicalDotsPerInch = logicalDotsPerInch;
}

/**
 * @return Physical dots per inch (valid if > 0)
 */
float
EventBrowserWindowPixelSizeInfoEvent::getPhysicalDotsPerInch() const
{
    return m_physicalDotsPerInch;
}

/**
 * Set the physical dots per inch.
 * @param physicalDotsPerInch
 *    New value for dots per inch
 */
void
EventBrowserWindowPixelSizeInfoEvent::setPhysicalDotsPerInch(const float physicalDotsPerInch)
{
    m_physicalDotsPerInch = physicalDotsPerInch;
}

