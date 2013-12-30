
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
 * Constructor for finding data files displayed in all tabs.
 */
EventGetDisplayedDataFiles::EventGetDisplayedDataFiles()
: Event(EventTypeEnum::EVENT_GET_DISPLAYED_DATA_FILES)
{
    m_allTabsMode = true;
}

/**
 * Constructor for finding data files displayed in the given tab indices.
 *
 * param tabIndices
 *     Indices of tabs for displayed data files. 
 */
EventGetDisplayedDataFiles::EventGetDisplayedDataFiles(const std::vector<int32_t>& tabIndices)
: Event(EventTypeEnum::EVENT_GET_DISPLAYED_DATA_FILES)
{
    m_allTabsMode = false;
    
    m_tabIndices.insert(tabIndices.begin(),
                        tabIndices.end());
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
    if (m_allTabsMode) {
        return true;
    }
    
    if (m_tabIndices.find(tabIndex) != m_tabIndices.end()) {
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


