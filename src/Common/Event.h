#ifndef __EVENT_H__
#define __EVENT_H__

/*LICENSE_START*/ 
/* 
 *  Copyright 1995-2002 Washington University School of Medicine 
 * 
 *  http://brainmap.wustl.edu 
 * 
 *  This file is part of CARET. 
 * 
 *  CARET is free software; you can redistribute it and/or modify 
 *  it under the terms of the GNU General Public License as published by 
 *  the Free Software Foundation; either version 2 of the License, or 
 *  (at your option) any later version. 
 * 
 *  CARET is distributed in the hope that it will be useful, 
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of 
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
 *  GNU General Public License for more details. 
 * 
 *  You should have received a copy of the GNU General Public License 
 *  along with CARET; if not, write to the Free Software 
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA 
 * 
 */ 


#include <CaretObject.h>

#include <stdint.h>

namespace caret {

    /// Base class for an event.
    class Event : public CaretObject {
        
    public:
        /** Type of event */
        enum EventType {
            /** Delete a browser tab */
            EVENT_BROWSER_TAB_DELETE,
            /** Create a new browser tab */
            EVENT_BROWSER_TAB_NEW,
            /** Get all model display controllers */
            EVENT_GET_MODEL_DISPLAY_CONTROLLERS,
            /** Load a surface file */
            EVENT_LOAD_SURFACE_FILE,
            /** Update all graphics windows */
            EVENT_UPDATE_ALL_GRAPHICS,
            /* NOT an event type but is number of event types THIS MUST ALWAYS BE LAST */
            EVENT_COUNT 
        };
        
        EventType getEventType() const;
        
        virtual ~Event();
        
        Event* getPointer();
        
        bool isError() const;
        
        AString getErrorMessage() const;
        
        void setErrorMessage(const AString& errorMessage);
        
    protected:
        Event(const EventType eventType);
        
    private:
        Event(const Event&);
        
        Event& operator=(const Event&);
        
        /** The type of event */
        EventType eventType;
        
        /** The error message */
        AString errorMessage;
        
        /** Tracks error status */
        bool errorStatus;
    };

} // namespace

#endif // __EVENT_H__
