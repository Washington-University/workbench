#ifndef __EVENT_H__
#define __EVENT_H__

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


#include <CaretObject.h>

#include <stdint.h>

#include "EventTypeEnum.h"

namespace caret {

    /// Base class for an event.
    class Event : public CaretObject {
        
    public:
        EventTypeEnum::Enum getEventType() const;
        
        virtual ~Event();
        
        Event* getPointer();
        
        bool isError() const;
        
        AString getErrorMessage() const;
        
        void setErrorMessage(const AString& errorMessage);
        
        virtual AString toString() const;
        
        void setEventProcessed();
        
        int32_t getEventProcessCount() const;
        
    protected:
        Event(const EventTypeEnum::Enum eventType);
        
    private:
        Event(const Event&);
        
        Event& operator=(const Event&);
        
        /** The type of event */
        EventTypeEnum::Enum eventType;
        
        /** The error message */
        AString errorMessage;
        
        /** Tracks error status */
        bool errorStatus;
        
        /** Number of times event was processed. */
        int32_t eventProcessedCount;
        
        friend class EventManager;
    };

} // namespace

#endif // __EVENT_H__
