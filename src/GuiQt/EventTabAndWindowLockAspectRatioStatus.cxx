
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

#define __EVENT_TAB_AND_WINDOW_LOCK_ASPECT_RATIO_STATUS_DECLARE__
#include "EventTabAndWindowLockAspectRatioStatus.h"
#undef __EVENT_TAB_AND_WINDOW_LOCK_ASPECT_RATIO_STATUS_DECLARE__

#include "Annotation.h"
#include "BrainBrowserWindow.h"
#include "BrainConstants.h"
#include "BrowserTabContent.h"
#include "CaretAssert.h"
#include "EventTypeEnum.h"

using namespace caret;


    
/**
 * \class caret::EventTabAndWindowLockAspectRatioStatus 
 * \brief Event that gets lock aspect and related information for tabs and windows
 * \ingroup GuiQt
 */

/**
 * Constructor.
 */
EventTabAndWindowLockAspectRatioStatus::EventTabAndWindowLockAspectRatioStatus()
: Event(EventTypeEnum::EVENT_TAB_AND_WINDOW_LOCK_ASPECT_RATIO_STATUS)
{
    m_tabStatus.resize(BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS);
    m_windowStatus.resize(BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_WINDOWS);
}

/**
 * Destructor.
 */
EventTabAndWindowLockAspectRatioStatus::~EventTabAndWindowLockAspectRatioStatus()
{
}

/**
 * @return Tab status for the given tab index, even if the tab is invalid.
 *
 * @param tabIndex
 *     Index of the tab.
 *     Must be in the range [0, BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS - 1]
 */
EventTabAndWindowLockAspectRatioStatus::TabStatus*
EventTabAndWindowLockAspectRatioStatus::getTabStatus(const int32_t tabIndex)
{
    CaretAssertVectorIndex(m_tabStatus, tabIndex);
    return &m_tabStatus[tabIndex];
}

/**
 * @return Window status for the given window index, even if the window is invalid.
 *
 * @param windowIndex
 *     Index of the window.
 *     Must be in the range [0, BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_WINDOWS - 1]
 */
EventTabAndWindowLockAspectRatioStatus::WindowStatus*
EventTabAndWindowLockAspectRatioStatus::getWindowStatus(const int32_t windowIndex)
{
    CaretAssertVectorIndex(m_windowStatus, windowIndex);
    return &m_windowStatus[windowIndex];
}


/**
 * Setup this instance with the given windows, the tabs in the windows, and
 * the given annotations.
 *
 * @param windows
 *     Open browser windows.
 * @param annotations
 *     All annotations.
 */
void
EventTabAndWindowLockAspectRatioStatus::setup(const std::vector<BrainBrowserWindow*>& windows,
                                              const std::vector<Annotation*>& annotations)
{
    /*
     * Find tab and window annotations
     */
    std::vector<bool> tabAnnotationFlag(BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,       false);
    std::vector<bool> windowAnnotationFlag(BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_WINDOWS, false);
    
    for (const auto ann : annotations) {
        switch (ann->getCoordinateSpace()) {
            case AnnotationCoordinateSpaceEnum::CHART:
                break;
            case AnnotationCoordinateSpaceEnum::STEREOTAXIC:
                break;
            case AnnotationCoordinateSpaceEnum::SURFACE:
                break;
            case AnnotationCoordinateSpaceEnum::TAB:
                CaretAssertVectorIndex(tabAnnotationFlag, ann->getTabIndex());
                tabAnnotationFlag[ann->getTabIndex()] = true;
                break;
            case AnnotationCoordinateSpaceEnum::VIEWPORT:
                break;
            case AnnotationCoordinateSpaceEnum::WINDOW:
                CaretAssertVectorIndex(windowAnnotationFlag, ann->getWindowIndex());
                windowAnnotationFlag[ann->getWindowIndex()] = true;
                break;
        }
    }
    
    /*
     * Set tab/window status.
     */
    for (const auto bbw : windows) {
        if (bbw != NULL) {
            const int32_t windowIndex = bbw->getBrowserWindowIndex();
            const bool windowAspectLocked = bbw->isAspectRatioLocked();
            const bool tileTabsEnabled = bbw->isTileTabsSelected();
            CaretAssertVectorIndex(windowAnnotationFlag, windowIndex);
            WindowStatus* windowStatus = getWindowStatus(windowIndex);
            windowStatus->setStatus(windowIndex,
                                    windowAspectLocked,
                                    tileTabsEnabled,
                                    windowAnnotationFlag[windowIndex]);
            
            std::vector<BrowserTabContent*> allTabs;
            bbw->getAllTabContent(allTabs);
            
            for (const auto tab : allTabs) {
                const int32_t tabIndex = tab->getTabNumber();
                const bool tabAspectLocked = tab->isAspectRatioLocked();
                
                CaretAssertVectorIndex(tabAnnotationFlag, tabIndex);
                TabStatus* tabStatus = getTabStatus(tabIndex);
                tabStatus->setStatus(tabIndex,
                                     windowIndex,
                                     tabAspectLocked,
                                     tabAnnotationFlag[tabIndex]);
            }
        }
    }
}

