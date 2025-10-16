#ifndef __EVENT_DISPLAY_PROPERTIES_LABELS_H__
#define __EVENT_DISPLAY_PROPERTIES_LABELS_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2025 Washington University School of Medicine
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

    class DisplayPropertiesLabels;
    
    class EventDisplayPropertiesLabels : public Event {
        
    public:
        /**
         * Mode of event
         */
        enum class Mode {
            /**
             * Get DisplayPropertiesLabels
             */
            GET,
            /**
             * DisplayPropertiesLabels' show unused labels flag has been changed and are being sent to listeners
             */
            SEND_SHOW_UNUSED_LABELS_CHANGED
        };
        
        EventDisplayPropertiesLabels(const Mode mode);
        
        virtual ~EventDisplayPropertiesLabels();
        
        EventDisplayPropertiesLabels(const EventDisplayPropertiesLabels&) = delete;

        EventDisplayPropertiesLabels& operator=(const EventDisplayPropertiesLabels&) = delete;
        
        Mode getMode() const;

        const DisplayPropertiesLabels* getDisplayPropertiesLabels() const;
        
        void setDisplayPropertiesLabels(const DisplayPropertiesLabels* dpl);
        
        // ADD_NEW_METHODS_HERE

    private:
        const Mode m_mode;

        const DisplayPropertiesLabels* m_displayPropertiesLabels = NULL;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __EVENT_DISPLAY_PROPERTIES_LABELS_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __EVENT_DISPLAY_PROPERTIES_LABELS_DECLARE__

} // namespace
#endif  //__EVENT_DISPLAY_PROPERTIES_LABELS_H__
