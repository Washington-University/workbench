
/*LICENSE_START*/
/*
 *  Copyright (C) 2014  Washington University School of Medicine
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

#define __EVENT_BROWSER_WINDOW_GRAPHICS_REDRAWN_DECLARE__
#include "EventBrowserWindowGraphicsRedrawn.h"
#undef __EVENT_BROWSER_WINDOW_GRAPHICS_REDRAWN_DECLARE__

#include "BrainBrowserWindow.h"
#include "EventTypeEnum.h"

using namespace caret;


    
/**
 * \class caret::EventBrowserWindowGraphicsRedrawn 
 * \brief Event issued when a browser windows graphics are redrawn.
 * \ingroup GuiQt
 */

/**
 * Constructor.
 */
EventBrowserWindowGraphicsRedrawn::EventBrowserWindowGraphicsRedrawn(BrainBrowserWindow* brainBrowserWindow)
: Event(EventTypeEnum::EVENT_BROWSER_WINDOW_GRAPHICS_HAVE_BEEN_REDRAWN)
{
    m_brainBrowserWindow = brainBrowserWindow;
    m_brainBrowserWindowIndex = m_brainBrowserWindow->getBrowserWindowIndex();
}

/**
 * Destructor.
 */
EventBrowserWindowGraphicsRedrawn::~EventBrowserWindowGraphicsRedrawn()
{
    
}

/**
 * @return Browser window that was redrawn.
 */
BrainBrowserWindow*
EventBrowserWindowGraphicsRedrawn::getBrowserWindow() const
{
    return m_brainBrowserWindow;
}

/**
 * @return Index of browser window that was redrawn.
 */
int32_t
EventBrowserWindowGraphicsRedrawn::getBrowserWindowIndex() const
{
    return m_brainBrowserWindowIndex;
}

