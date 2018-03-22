
/*LICENSE_START*/
/*
 *  Copyright (C) 2014 Washington University School of Medicine
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

#define __EVENT_HELP_VIEWER_DISPLAY_DECLARE__
#include "EventHelpViewerDisplay.h"
#undef __EVENT_HELP_VIEWER_DISPLAY_DECLARE__

#include "CaretAssert.h"
#include "EventTypeEnum.h"

using namespace caret;


    
/**
 * \class caret::EventHelpViewerDisplay 
 * \brief Display the topic in the help viewer.
 * \ingroup GuiQt
 */

/**
 * Constructor displays help dialog with specific topic.
 *
 * @param brainBrowserWindow
 *     Parent window.
 * @param helpPageName
 *     Name of HTML file without any path nor ".html" extension.
 */
EventHelpViewerDisplay::EventHelpViewerDisplay(BrainBrowserWindow* brainBrowserWindow,
                                               const AString helpPageName)
: Event(EventTypeEnum::EVENT_HELP_VIEWER_DISPLAY),
m_brainBrowserWindow(brainBrowserWindow),
m_helpPageName(helpPageName)
{
}

/**
 * Destructor.
 */
EventHelpViewerDisplay::~EventHelpViewerDisplay()
{
}

/**
 * @return Brain browser window on which a new help viewer dialog is displayed
 * May be NULL in which case any browser window should be used as parent.
 */
const BrainBrowserWindow*
EventHelpViewerDisplay::getBrainBrowserWindow() const
{
    return m_brainBrowserWindow;
}

/**
 * @return Name of page for display (may be empty string).
 */
AString
EventHelpViewerDisplay::getHelpPageName() const
{
    return m_helpPageName;
}

