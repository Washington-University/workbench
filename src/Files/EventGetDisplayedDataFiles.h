#ifndef __EVENT_GET_DISPLAYED_DATA_FILES_H__
#define __EVENT_GET_DISPLAYED_DATA_FILES_H__

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

#include <set>

#include "Event.h"



namespace caret {

    class CaretDataFile;
    
    class EventGetDisplayedDataFiles : public Event {
        
    public:
        EventGetDisplayedDataFiles(const std::vector<int32_t>& windowIndices,
                                   const std::vector<int32_t>& tabIndices);
        
        virtual ~EventGetDisplayedDataFiles();
        
        bool isTestForDisplayedDataFileInTabIndex(const int32_t tabIndex) const;
        
        bool isTestForDisplayedDataFileInWindowIndex(const int32_t windowIndex) const;
        
        void addDisplayedDataFile(const CaretDataFile* caretDataFile);
        
        bool isDataFileDisplayed(const CaretDataFile* caretDataFile) const;

        std::set<const CaretDataFile*> getDisplayedDataFiles() const;
        
    private:
        EventGetDisplayedDataFiles(const EventGetDisplayedDataFiles&);

        EventGetDisplayedDataFiles& operator=(const EventGetDisplayedDataFiles&);
        
    public:

        // ADD_NEW_METHODS_HERE

    private:
        // ADD_NEW_MEMBERS_HERE

        std::set<int32_t> m_windowIndices;
        
        std::set<int32_t> m_tabIndices;
        
        std::set<const CaretDataFile*> m_displayedDataFiles;
    };
    
#ifdef __EVENT_GET_DISPLAYED_DATA_FILES_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __EVENT_GET_DISPLAYED_DATA_FILES_DECLARE__

} // namespace
#endif  //__EVENT_GET_DISPLAYED_DATA_FILES_H__
