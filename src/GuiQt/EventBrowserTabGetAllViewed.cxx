
/*LICENSE_START*/
/*
 * Copyright 2013 Washington University,
 * All rights reserved.
 *
 * Connectome DB and Connectome Workbench are part of the integrated Connectome 
 * Informatics Platform.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    * Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *    * Neither the names of Washington University nor the
 *      names of its contributors may be used to endorse or promote products
 *      derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR  
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
 * SUBSTITUTE GOODS OR  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
 * POSSIBILITY OF SUCH DAMAGE.
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




