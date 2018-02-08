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

#include "EventBrowserWindowDrawingContent.h"

using namespace caret;

#include "CaretAssert.h"
/** 
 * \class caret::EventBrowserWindowDrawingContent
 * \brief Event to get content for drawing browser window.
 * \ingroup GuiQt
 */

/**
 * Constructor.
 * @param browserWindowIndex
 *    Index of browser window.
 */
EventBrowserWindowDrawingContent::EventBrowserWindowDrawingContent(const int32_t browserWindowIndex)
: Event(EventTypeEnum::EVENT_BROWSER_WINDOW_DRAWING_CONTENT_GET)
{
    m_browserWindowIndex = browserWindowIndex;
    m_tabIndexForTileTabsHighlighting = -1;
    m_tileTabsConfiguration = NULL;
    m_selectedBrowserTabContent = NULL;
    m_tileTabsSelectedFlag = false;
}

/*
 * Destructor.
 */
EventBrowserWindowDrawingContent::~EventBrowserWindowDrawingContent()
{
    
}

/**
 * @return Get the browser window index.
 */
int32_t 
EventBrowserWindowDrawingContent::getBrowserWindowIndex() const 
{ 
    return m_browserWindowIndex; 
}

/**
 * @return The number of browser tabs in window
 */
int32_t 
EventBrowserWindowDrawingContent::getNumberOfBrowserTabs() const
{
    return this->browserTabContents.size();
}

/**
 * Add browser tab
 */
void 
EventBrowserWindowDrawingContent::addBrowserTab(BrowserTabContent* browserTabContent)
{
    this->browserTabContents.push_back(browserTabContent);
}

/**
 * Get the browser tab at the given index
 * 
 * @param itemIndex
 *    Index of the item to browser tab.
 * @return
 *    Pointer to tab contents for the item index.
 */
BrowserTabContent*
EventBrowserWindowDrawingContent::getBrowserTab(const int32_t itemIndex)
{
    CaretAssertVectorIndex(this->browserTabContents, itemIndex);
    return this->browserTabContents[itemIndex];
}

/**
 * Set index of tab for highlighting in tile tabs mode.
 * 
 * @param tabIndex
 *    Index of tab for highlighting.
 */
void
EventBrowserWindowDrawingContent::setTabIndexForTileTabsHighlighting(const int32_t tabIndex)
{
    m_tabIndexForTileTabsHighlighting = tabIndex;
}

/**
 * @return Index of tab for highlighting in Tile Tabs mode.
 */
int32_t
EventBrowserWindowDrawingContent::getTabIndexForTileTabsHighlighting() const
{
    return m_tabIndexForTileTabsHighlighting;
}

/**
 * @return The tile tabs configuration when more than one tab to draw.
 *         May be NULL.
 */
TileTabsConfiguration*
EventBrowserWindowDrawingContent::getTileTabsConfiguration() const
{
    return m_tileTabsConfiguration;
}

/**
 * Set the tile tabs configuration.
 *
 * @param tileTabsConfiguration
 *    New selected tile tabs configuration.
 */
void
EventBrowserWindowDrawingContent::setTileTabsConfiguration(TileTabsConfiguration* tileTabsConfiguration)
{
    m_tileTabsConfiguration = tileTabsConfiguration;
}

/**
 * @return The selected browser tab content.  May be NULL.
 * In single tab mode, this is the tab to draw.
 */
BrowserTabContent*
EventBrowserWindowDrawingContent::getSelectedBrowserTabContent()
{
    return m_selectedBrowserTabContent;
}

/**
 * Set the selected browser tab content.
 *
 * @param browserTabContent
 *     The selected browser tab content.
 */
void
EventBrowserWindowDrawingContent::setSelectedBrowserTabContent(BrowserTabContent* browserTabContent)
{
    m_selectedBrowserTabContent = browserTabContent;
}

/**
 * @return Is tile tabs selected.
 */
bool
EventBrowserWindowDrawingContent::isTileTabsSelected() const
{
    return m_tileTabsSelectedFlag;
}

/**
 * Set tile tabs selected.
 *
 * @param tileTabsSelected
 *    New status of tile tabs.
 */
void
EventBrowserWindowDrawingContent::setTileTabsSelected(const bool tileTabsSelected)
{
    m_tileTabsSelectedFlag = tileTabsSelected;
}



