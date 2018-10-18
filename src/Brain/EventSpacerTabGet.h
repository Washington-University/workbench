#ifndef __EVENT_SPACER_TAB_GET_H__
#define __EVENT_SPACER_TAB_GET_H__

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



#include <memory>

#include "Event.h"



namespace caret {

    class SpacerTabContent;
    
    class EventSpacerTabGet : public Event {
        
    public:
        EventSpacerTabGet(const int32_t windowIndex,
                          const int32_t rowIndex,
                          const int32_t columnIndex);
        
        virtual ~EventSpacerTabGet();
        
        EventSpacerTabGet(const EventSpacerTabGet&) = delete;

        EventSpacerTabGet& operator=(const EventSpacerTabGet&) = delete;

        int32_t getWindowIndex() const;
        
        int32_t getRowIndex() const;
        
        int32_t getColumnIndex() const;
        
        SpacerTabContent* getSpacerTabContent();
        
        void setSpacerTabContent(SpacerTabContent* spacerTabContent);

        // ADD_NEW_METHODS_HERE

    private:
        const int32_t m_windowIndex;
        const int32_t m_rowIndex;
        const int32_t m_columnIndex;
        
        SpacerTabContent* m_spacerTabContent = NULL;
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __EVENT_SPACER_TAB_GET_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __EVENT_SPACER_TAB_GET_DECLARE__

} // namespace
#endif  //__EVENT_SPACER_TAB_GET_H__
