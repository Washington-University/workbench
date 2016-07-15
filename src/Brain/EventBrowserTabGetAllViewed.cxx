
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

#define __EVENT_BROWSER_TAB_GET_ALL_VIEWED_DECLARE__
#include "EventBrowserTabGetAllViewed.h"
#undef __EVENT_BROWSER_TAB_GET_ALL_VIEWED_DECLARE__

#include "BrowserTabContent.h"
#include "CaretAssert.h"
#include "EventTypeEnum.h"

using namespace caret;


    
/**
 * \class caret::EventBrowserTabGetAllViewed 
 * \brief Event that gets all viewed browser tabs
 * \ingroup GuiQt
 *
 * For each window, it will add its selected browser tab.  If
 * the window is in 'Tile Tabs' mode, all of its browser tabs
 * are added.
 */

/**
 * Constructor.
 */
EventBrowserTabGetAllViewed::EventBrowserTabGetAllViewed()
: Event(EventTypeEnum::EVENT_BROWSER_TAB_GET_ALL_VIEWED)
{
    
}

/**
 * Destructor.
 */
EventBrowserTabGetAllViewed::~EventBrowserTabGetAllViewed()
{
}

/**
 * Add a viewed browser tab.
 *
 * @param browserTabContent
 *     Browser tab that is added.
 */
void
EventBrowserTabGetAllViewed::addViewedBrowserTab(BrowserTabContent* browserTabContent)
{
    m_viewedBrowserTabs.push_back(browserTabContent);
}

/**
 * @return A vector containing the viewed browser tabs
 */
std::vector<BrowserTabContent*>
EventBrowserTabGetAllViewed::getViewedBrowserTabs() const
{
    return m_viewedBrowserTabs;
}

/**
 * @return A vector containing the indices of the viewed browser tabs.
 */
std::vector<int32_t>
EventBrowserTabGetAllViewed::getViewdedBrowserTabIndices() const
{
    std::vector<int32_t> tabIndices;
    
    for (std::vector<BrowserTabContent*>::const_iterator iter = m_viewedBrowserTabs.begin();
         iter != m_viewedBrowserTabs.end();
         iter++) {
        const BrowserTabContent* btc = *iter;
        tabIndices.push_back(btc->getTabNumber());
    }
    
    return tabIndices;
}

/**
 * @return A vector containing surface structures in the viewed browser tabs.
 */
std::vector<StructureEnum::Enum>
EventBrowserTabGetAllViewed::getViewedSurfaceStructures() const
{
    std::set<StructureEnum::Enum> structureSet;
    
    for (std::vector<BrowserTabContent*>::const_iterator iter = m_viewedBrowserTabs.begin();
         iter != m_viewedBrowserTabs.end();
         iter++) {
        BrowserTabContent* btc = *iter;
        std::vector<StructureEnum::Enum> tabStructures = btc->getSurfaceStructuresDisplayed();
        structureSet.insert(tabStructures.begin(),
                            tabStructures.end());
    }
    
    std::vector<StructureEnum::Enum> structuresOut(structureSet.begin(),
                                                   structureSet.end());
    return structuresOut;
}




