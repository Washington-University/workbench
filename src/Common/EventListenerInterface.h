#ifndef __EVENT_LISTENER_INTERFACE_H__
#define __EVENT_LISTENER_INTERFACE_H__

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
        EventListenerInterface();
        
        /**
         * Destructor.
         */
        virtual ~EventListenerInterface();
        
    private:
        EventListenerInterface(const EventListenerInterface&) { }
        
        EventListenerInterface& operator=(const EventListenerInterface& ei); /*{
            return *this;
        }//*/  //TSC: removed implementation to prevent "parameter unused" warnings, since it is private in an interface class
        
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
