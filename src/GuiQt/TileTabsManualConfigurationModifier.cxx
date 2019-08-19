
/*LICENSE_START*/
/*
 *  Copyright (C) 2019 Washington University School of Medicine
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

#define __TILE_TABS_MANUAL_CONFIGURATION_MODIFIER_DECLARE__
#include "TileTabsManualConfigurationModifier.h"
#undef __TILE_TABS_MANUAL_CONFIGURATION_MODIFIER_DECLARE__

#include <algorithm>

#include "AnnotationBrowserTab.h"
#include "BrainOpenGLViewportContent.h"
#include "BrowserTabContent.h"
#include "CaretAssert.h"
#include "TileTabsBrowserTabGeometry.h"
#include "TileTabsManualTabGeometryWidget.h"

using namespace caret;


    
/**
 * \class caret::TileTabsManualConfigurationModifier 
 * \brief Perform an operation on a manual configuration layout
 * \ingroup GuiQt
 */

/**
 * Constructor.
 *
 * @param browserTabContents
 *     The current tab content
 */
TileTabsManualConfigurationModifier::TileTabsManualConfigurationModifier(const std::vector<BrowserTabContent*>& browserTabContents)
: CaretObject(),
m_browserTabs(browserTabContents)
{
}

/**
 * Destructor.
 */
TileTabsManualConfigurationModifier::~TileTabsManualConfigurationModifier()
{
}

/**
 * Run a tab order operation
 *
 * @param operation
 *     The tab order operation
 * @param tabIndex
 *     Index of tab that has order changed
 * @param errorMessageOut
 *     Output with error information
 * @return
 *     True if successful, else false
 */
bool
TileTabsManualConfigurationModifier::runTabOrderOperation(const TabOrderOperation operation,
                                                          const int32_t browserTabIndex,
                                                          AString& errorMessageOut)
{
    errorMessageOut.clear();
    if (m_browserTabs.size() <= 1) {
        return true;
    }

    CaretAssert(browserTabIndex >= 0);
    
    BrowserTabContent* browserTabContent(NULL);
    for (auto tabContent : m_browserTabs) {
        CaretAssert(tabContent);
        if (tabContent->getTabNumber() == browserTabIndex) {
            browserTabContent = tabContent;
            break;
        }
    }
    if (browserTabContent == NULL) {
        errorMessageOut = ("Tab "
                           + AString::number(browserTabIndex + 1)
                           + " not found in browser tabs.");
        return false;
    }
    
    /*
     * Sort tabs by current stacking order
     */
    std::vector<TabInfo> tabOrderAndContent;
    for (const auto tabContent : m_browserTabs) {
        CaretAssert(tabContent);
        
        tabOrderAndContent.emplace_back(tabContent,
                                        tabContent->getManualLayoutBrowserTabAnnotation()->getStackingOrder());
    }
    std::sort(tabOrderAndContent.begin(),
              tabOrderAndContent.end());
    
    /*
     * Now that that tab are sorted by stacking order, order tabs with even
     * numbers which will make order changing easier.
     * Also find closest tab behind and in front of 'browserTabContent'
     */
    int32_t indexOfTab(-1);
    int32_t indexOfTabBehind(-1);
    int32_t indexOfTabInFront(-1);
    const int32_t numTabs = static_cast<int32_t>(tabOrderAndContent.size());
    CaretAssert(numTabs > 2);
    for (int32_t i = 0; i < numTabs; i++) {
        CaretAssertVectorIndex(tabOrderAndContent, i);
        tabOrderAndContent[i].m_stackOrder = i * 2;
        
        if (tabOrderAndContent[i].m_tabContent == browserTabContent) {
            indexOfTab = i;
        }
        else if (indexOfTab < 0) { /* have not yet found index of 'browserTabContent' */
            /*
             * Find tab that is closest to 'browserTabContent' but behind and intersects it
             */
            if (browserTabContent->getManualLayoutBrowserTabAnnotation()->intersectionTest(tabOrderAndContent[i].m_tabContent->getManualLayoutBrowserTabAnnotation())) {
                indexOfTabBehind = i;
            }
        }
        else if (indexOfTabInFront < 0) {
            /*
             * Find first (closest) tab that is in front of and intersects 'browserTabContent'
             */
            if (browserTabContent->getManualLayoutBrowserTabAnnotation()->intersectionTest(tabOrderAndContent[i].m_tabContent->getManualLayoutBrowserTabAnnotation())) {
                indexOfTabInFront = i;
            }
        }
    }
    
    CaretAssert(indexOfTab >= 0);
    
    /*
     * Now change the stack order of the tab.
     * Since earlier, the stacking orders were made (0, 2, 4, ...) we only need to
     * add or subtract 1 to the stack order using the reference tab that is in front or back
     */
    switch (operation) {
        case TabOrderOperation::BRING_FORWARD:
            if (indexOfTabInFront >= 0) {
                /*
                 * Move forward one position
                 */
                CaretAssertVectorIndex(tabOrderAndContent, indexOfTabInFront);
                CaretAssertVectorIndex(tabOrderAndContent, indexOfTab);
                tabOrderAndContent[indexOfTab].m_stackOrder = tabOrderAndContent[indexOfTabInFront].m_stackOrder + 1;
            }
            break;
        case TabOrderOperation::BRING_TO_FRONT:
        {
            /*
             * Move to in front of all tabs
             */
            const int32_t frontIndex = static_cast<int32_t>(tabOrderAndContent.size() - 1);
            CaretAssertVectorIndex(tabOrderAndContent, frontIndex);
            CaretAssertVectorIndex(tabOrderAndContent, indexOfTab);
            tabOrderAndContent[indexOfTab].m_stackOrder = tabOrderAndContent[frontIndex].m_stackOrder + 1;
        }
            break;
        case TabOrderOperation::SEND_BACKWARD:
            if (indexOfTabBehind >= 0) {
                /*
                 * Move back one position
                 */
                CaretAssertVectorIndex(tabOrderAndContent, indexOfTabBehind);
                CaretAssertVectorIndex(tabOrderAndContent, indexOfTab);
                tabOrderAndContent[indexOfTab].m_stackOrder = tabOrderAndContent[indexOfTabBehind].m_stackOrder - 1;
            }
            break;
        case TabOrderOperation::SEND_TO_BACK:
        {
            /*
             * Move to behind all tabs
             */
            const int32_t backIndex = 0;
            CaretAssertVectorIndex(tabOrderAndContent, backIndex);
            CaretAssertVectorIndex(tabOrderAndContent, indexOfTab);
            tabOrderAndContent[indexOfTab].m_stackOrder = tabOrderAndContent[backIndex].m_stackOrder - 1;
        }
            break;
    }
    
    /*
     * Now order tabs sequentially to clean up stacking order
     */
    std::sort(tabOrderAndContent.begin(),
              tabOrderAndContent.end());
    for (int32_t i = 0; i < numTabs; i++) {
        CaretAssertVectorIndex(tabOrderAndContent, i);
        tabOrderAndContent[i].m_tabContent->getManualLayoutBrowserTabAnnotation()->setStackingOrder(i);
    }
    
    return true;
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
TileTabsManualConfigurationModifier::toString() const
{
    return "TileTabsManualConfigurationModifier";
}

