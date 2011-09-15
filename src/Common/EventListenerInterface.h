#ifndef __EVENT_LISTENER_INTERFACE_H__
#define __EVENT_LISTENER_INTERFACE_H__

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

namespace caret {

    class Event;
    
    /**
     * \brief  Interface for objects receiving events.
     *
     * This interface must be implemented by any object that
     * wants to receive events.
     */
    class EventListenerInterface {
        
    protected:
        /**
         * Constructor.
         */
        EventListenerInterface() { }
        
        /**
         * Destructor.
         */
        virtual ~EventListenerInterface() { }
        
    private:
        EventListenerInterface(const EventListenerInterface&) { }
        
        EventListenerInterface& operator=(const EventListenerInterface& ei) {
            return *this;
        }
        
    public:
        /**
         * Receive an event.
         * 
         * @param event
         *     The event that the receive can respond to.
         */
        virtual void receiveEvent(Event* event) = 0;
        
    };

} // namespace

#endif // __EVENT_LISTENER_INTERFACE_H__
