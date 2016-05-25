#ifndef __PROGRESS_REPORTING_FROM_EVENT_H__
#define __PROGRESS_REPORTING_FROM_EVENT_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2016 Washington University School of Medicine
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


#include "ProgressReportingWithSlots.h"

#include "EventListenerInterface.h"


namespace caret {

    class ProgressReportingFromEvent : public ProgressReportingWithSlots, public EventListenerInterface {
        Q_OBJECT
        
    public:
        ProgressReportingFromEvent(QObject* parent);
        
        virtual ~ProgressReportingFromEvent();
        
        void setEventReceivingEnabled(bool enabled);
        
        bool isEventReceivingEnabled() const;

        // ADD_NEW_METHODS_HERE

        virtual void receiveEvent(Event* event);

    private:
        ProgressReportingFromEvent(const ProgressReportingFromEvent&);

        ProgressReportingFromEvent& operator=(const ProgressReportingFromEvent&);

        bool m_eventReceivingEnabled;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __PROGRESS_REPORTING_FROM_EVENT_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __PROGRESS_REPORTING_FROM_EVENT_DECLARE__

} // namespace
#endif  //__PROGRESS_REPORTING_FROM_EVENT_H__
