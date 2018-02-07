
/*LICENSE_START*/
/*
 *  Copyright (C) 2018 Washington University School of Medicine
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

#define __EVENT_BROWSER_WINDOW_CONTENT_DECLARE__
#include "EventBrowserWindowContent.h"
#undef __EVENT_BROWSER_WINDOW_CONTENT_DECLARE__

#include "CaretAssert.h"
#include "EventTypeEnum.h"

using namespace caret;


    
/**
 * \class caret::EventBrowserWindowContent 
 * \brief Evnets involving browser window content
 * \ingroup Brain
 */

/**
 * Constructor.
 *
 * @param mode
 *     The mode.
 * @param browserWindowIndex
 *     Index of the browser window.
 */
EventBrowserWindowContent::EventBrowserWindowContent(const Mode mode,
                                                     const int32_t browserWindowIndex)
: Event(EventTypeEnum::EVENT_BROWSER_WINDOW_CONTENT),
m_mode(mode),
m_browserWindowIndex(browserWindowIndex)
{
    
}

/**
 * Destructor.
 */
EventBrowserWindowContent::~EventBrowserWindowContent()
{
}

/**
 * @return Event that is sent to create new BrowserWindowContent
 * for the given window index.
 *
 * @param windowIndex
 *     Index of the new window.
 */
std::unique_ptr<EventBrowserWindowContent>
EventBrowserWindowContent::newWindowContent(const int32_t windowIndex)
{
    EventBrowserWindowContent* event = new EventBrowserWindowContent(Mode::NEW,
                                                                     windowIndex);
    std::unique_ptr<EventBrowserWindowContent> pointer(event);
    return pointer;
}

/**
 * @return Event that is sent to create delete BrowserWindowContent
 * for the given window index.
 *
 * @param windowIndex
 *     Index of the deleted window.
 */
std::unique_ptr<EventBrowserWindowContent>
EventBrowserWindowContent::deleteWindowContent(const int32_t windowIndex)
{
    EventBrowserWindowContent* event = new EventBrowserWindowContent(Mode::DELETE,
                                                                     windowIndex);
    std::unique_ptr<EventBrowserWindowContent> pointer(event);
    return pointer;
}

/**
 * @return Event that is sent to create get BrowserWindowContent
 * for the given window index.
 *
 * @param windowIndex
 *     Index of the window.
 */
std::unique_ptr<EventBrowserWindowContent>
EventBrowserWindowContent::getWindowContent(const int32_t windowIndex)
{
    EventBrowserWindowContent* event = new EventBrowserWindowContent(Mode::GET,
                                                                     windowIndex);
    std::unique_ptr<EventBrowserWindowContent> pointer(event);
    return pointer;
}


/**
 * @return The mode.
 */
EventBrowserWindowContent::Mode
EventBrowserWindowContent::getMode() const
{
    return m_mode;
}

/**
 * @return Pointer to the browser window content.
 */
BrowserWindowContent*
EventBrowserWindowContent::getBrowserWindowContent()
{
    return m_browserWindowContent;
}

/**
 * Set the browser window content.
 *
 * @param browserWindowContent
 *     Pointer to the browser window content.
 */
void
EventBrowserWindowContent::setBrowserWindowContent(BrowserWindowContent* browserWindowContent)
{
    m_browserWindowContent = browserWindowContent;
}

/**
 * @return Index of the browser window.
 */
int32_t
EventBrowserWindowContent::getBrowserWindowIndex() const
{
    return m_browserWindowIndex;
}
