
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

#include <algorithm>

#define __EVENT_GET_DISPLAYED_DATA_FILES_DECLARE__
#include "EventGetDisplayedDataFiles.h"
#undef __EVENT_GET_DISPLAYED_DATA_FILES_DECLARE__

#include "CaretAssert.h"
#include "EventBrowserTabIndicesGetAllViewed.h"
#include "EventManager.h"
#include "EventTypeEnum.h"

using namespace caret;


    
/**
 * \class caret::EventGetDisplayedDataFiles 
 * \brief Find data files displayed in some or all tabs.
 * \ingroup Files
 */

/**
 * Constructor for finding data files displayed in all windows/tabs.
 * @param mode
 *  Mode for getting tab
 */
EventGetDisplayedDataFiles::EventGetDisplayedDataFiles(const Mode mode)
: Event(EventTypeEnum::EVENT_GET_DISPLAYED_DATA_FILES),
m_mode(mode)
{
    switch (m_mode) {
        case Mode::FILES_IN_ALL_TABS:
            break;
        case Mode::FILES_IN_VIEWED_TABS:
        {
            EventBrowserTabIndicesGetAllViewed viewedTabs;
            EventManager::get()->sendEvent(viewedTabs.getPointer());
            std::vector<int32_t> indices = viewedTabs.getAllBrowserTabIndices();
            m_tabIndices.insert(indices.begin(),
                                indices.end());
        }
            break;
        case Mode::FILES_IN_WINDOWS_TABS:
            CaretAssertMessage(0, "Use constructor that accepts window/tab indiceds for this mode");
            break;
    }
}
/**
 * Constructor for finding data files displayed in the given window and tab indices.
 *
 * param windowIndices
 *     Indices of windows for displayed data files.
 * param tabIndices
 *     Indices of tabs for displayed data files.
 */
EventGetDisplayedDataFiles::EventGetDisplayedDataFiles(const std::vector<int32_t>& windowIndices,
                                                       const std::vector<int32_t>& tabIndices)
: Event(EventTypeEnum::EVENT_GET_DISPLAYED_DATA_FILES),
m_mode(Mode::FILES_IN_WINDOWS_TABS)
{
    m_tabIndices.insert(tabIndices.begin(),
                        tabIndices.end());
    m_windowIndices.insert(windowIndices.begin(),
                           windowIndices.end());
}

/**
 * Destructor.
 */
EventGetDisplayedDataFiles::~EventGetDisplayedDataFiles()
{
}

/**
 * @return the mode
 */
EventGetDisplayedDataFiles::Mode
EventGetDisplayedDataFiles::getMode() const
{
    return m_mode;
}

/**
 * Is the tab index one for determining displayed data files.
 *
 * @param tabIndex
 *    Index for displayed data files.
 * @return
 *    True if the tab is one determining displayed data files.
 */
bool
EventGetDisplayedDataFiles::isTestForDisplayedDataFileInTabIndex(const int32_t tabIndex) const
{
    switch (m_mode) {
        case Mode::FILES_IN_ALL_TABS:
            return true;
            break;
        case Mode::FILES_IN_VIEWED_TABS:
            break;
        case Mode::FILES_IN_WINDOWS_TABS:
            break;
    }

    if (m_tabIndices.find(tabIndex) != m_tabIndices.end()) {
        return true;
    }
    
    return false;
}

/**
 * Is the window index one for determining displayed data files.
 *
 * @param windowIndex
 *    Index for displayed data files.
 * @return
 *    True if the window is one determining displayed data files.
 */
bool
EventGetDisplayedDataFiles::isTestForDisplayedDataFileInWindowIndex(const int32_t windowIndex) const
{
    switch (m_mode) {
        case Mode::FILES_IN_ALL_TABS:
            return true;
            break;
        case Mode::FILES_IN_VIEWED_TABS:
            return true;
            break;
        case Mode::FILES_IN_WINDOWS_TABS:
            break;
    }
    
    if (m_windowIndices.find(windowIndex) != m_windowIndices.end()) {
        return true;
    }
    
    return false;
}

/**
 * Add the given file as a displayed data file.
 *
 * @param caretDataFile
 *    Data file that is displayed.
 */
void
EventGetDisplayedDataFiles::addDisplayedDataFile(const CaretDataFile* caretDataFile)
{
    m_displayedDataFiles.insert(caretDataFile);
}

/*
 * Is the given data file displayed?
 *
 * @param caretDataFile
 *    Caret data file for testing displayed in a tab.
 * @return
 *    True if the file is displayed, else false.
 */
bool
EventGetDisplayedDataFiles::isDataFileDisplayed(const CaretDataFile* caretDataFile) const
{
    if (m_displayedDataFiles.find(caretDataFile) != m_displayedDataFiles.end()) {
        return true;
    }
    
    return false;
}

/**
 * @return The displayed data files in a set.
 */
std::set<const CaretDataFile*>
EventGetDisplayedDataFiles::getDisplayedDataFiles() const
{
    return m_displayedDataFiles;
}

/**
 * @return The tab indices.
 */
std::vector<int32_t>
EventGetDisplayedDataFiles::getTabIndices() const
{
    std::vector<int32_t> tabVector(m_tabIndices.begin(),
                                   m_tabIndices.end());
    return tabVector;
}

/**
 * @return The window indices.
 */
std::vector<int32_t>
EventGetDisplayedDataFiles::getWindowIndices() const
{
    std::vector<int32_t> windowVector(m_windowIndices.begin(),
                                      m_windowIndices.end());
    return windowVector;
}


