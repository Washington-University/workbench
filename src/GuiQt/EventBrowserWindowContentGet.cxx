/*LICENSE_START*/ 
/* 
 *  Copyright 1995-2002 Washington University School of Medicine 
 * 
 *  http://brainmap.wustl.edu 
 * 
 *  This file is part of CARET. 
 * 
 *  CARET is free software; you can redistribute it and/or modify 
 *  it under the terms of the GNU General Public License as published by 
 *  the Free Software Foundation; either version 2 of the License, or 
 *  (at your option) any later version. 
 * 
 *  CARET is distributed in the hope that it will be useful, 
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of 
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
 *  GNU General Public License for more details. 
 * 
 *  You should have received a copy of the GNU General Public License 
 *  along with CARET; if not, write to the Free Software 
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA 
 * 
 */ 

#include "EventBrowserWindowContentGet.h"

using namespace caret;

#include "CaretAssert.h"
/** 
 * \class caret::DisplayGroupEnumComboBox 
 * \brief Combo box for selection of a display group.
 * \ingroup GuiQt
 */

/**
 * Constructor.
 * @param browserWindowIndex
 *    Index of browser window.
 */
EventBrowserWindowContentGet::EventBrowserWindowContentGet(const int32_t browserWindowIndex)
: Event(EventTypeEnum::EVENT_BROWSER_WINDOW_CONTENT_GET)
{
    m_browserWindowIndex = browserWindowIndex;
    m_tabIndexForTileTabsHighlighting = -1;
    m_tileTabsConfiguration = NULL;
    m_selectedBrowserTabContent = NULL;
}

/*
 * Destructor.
 */
EventBrowserWindowContentGet::~EventBrowserWindowContentGet()
{
    
}

/**
 * @return Get the browser window index.
 */
int32_t 
EventBrowserWindowContentGet::getBrowserWindowIndex() const 
{ 
    return m_browserWindowIndex; 
}

/**
 * @return The number of items to draw.
 */
int32_t 
EventBrowserWindowContentGet::getNumberOfItemsToDraw() const
{
    return this->browserTabContents.size();
}

/**
 * Add tab content for drawing in a window.
 */
void 
EventBrowserWindowContentGet::addTabContentToDraw(BrowserTabContent* browserTabContent)
{
    this->browserTabContents.push_back(browserTabContent);
}

/**
 * Get the tab content for drawing in a window.
 * 
 * @param itemIndex
 *    Index of the item to draw.
 * @return
 *    Pointer to tab contents for the item index.
 */
BrowserTabContent*
EventBrowserWindowContentGet::getTabContentToDraw(const int32_t itemIndex)
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
EventBrowserWindowContentGet::setTabIndexForTileTabsHighlighting(const int32_t tabIndex)
{
    m_tabIndexForTileTabsHighlighting = tabIndex;
}

/**
 * @return Index of tab for highlighting in Tile Tabs mode.
 */
int32_t
EventBrowserWindowContentGet::getTabIndexForTileTabsHighlighting() const
{
    return m_tabIndexForTileTabsHighlighting;
}

/**
 * @return The tile tabs configuration when more than one tab to draw.
 *         May be NULL.
 */
TileTabsConfiguration*
EventBrowserWindowContentGet::getTileTabsConfiguration() const
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
EventBrowserWindowContentGet::setTileTabsConfiguration(TileTabsConfiguration* tileTabsConfiguration)
{
    m_tileTabsConfiguration = tileTabsConfiguration;
}

/**
 * @return The selected browser tab content.  May be NULL.
 */
BrowserTabContent*
EventBrowserWindowContentGet::getSelectedBrowserTabContent()
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
EventBrowserWindowContentGet::setSelectedBrowserTabContent(BrowserTabContent* browserTabContent)
{
    m_selectedBrowserTabContent = browserTabContent;
}



