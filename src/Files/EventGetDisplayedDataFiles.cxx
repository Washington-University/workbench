
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
#include "EventTypeEnum.h"

using namespace caret;


    
/**
 * \class caret::EventGetDisplayedDataFiles 
 * \brief Find data files displayed in some or all tabs.
 * \ingroup Files
 */

/**
 * Constructor for finding data files displayed in the given tab indices.
 *
 * param tabIndices
 *     Indices of tabs for displayed data files. 
 */
EventGetDisplayedDataFiles::EventGetDisplayedDataFiles(const std::vector<int32_t>& windowIndices,
                                                       const std::vector<int32_t>& tabIndices)
: Event(EventTypeEnum::EVENT_GET_DISPLAYED_DATA_FILES)
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


