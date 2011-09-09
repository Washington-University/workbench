#ifndef __SESSION_MANAGER__H_
#define __SESSION_MANAGER__H_

/*LICENSE_START*/
/* 
 *  Copyright 1995-2011 Washington University School of Medicine 
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

#include "BrainConstants.h"
#include "CaretObject.h"
#include "EventListenerInterface.h"

namespace caret {
    
    class BrowserTabContent;
    
    /// <REPLACE WITH DESCRIPTION OF CLASS>
    class SessionManager : public CaretObject, public EventListenerInterface {
        
    public:
        SessionManager* get();
        
        virtual ~SessionManager();
        
        virtual void receiveEvent(Event* event);
        
    private:
        SessionManager();
        
        SessionManager(const SessionManager&);

        SessionManager& operator=(const SessionManager&);
        
    public:
        virtual AString toString() const;
        
    private:
        /** The session manager */
        static SessionManager* singletonSessionManager;
        
        /** The browser tabs */
        BrowserTabContent* browserTabs[BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS];
    };
    
#ifdef __SESSION_MANAGER_DECLARE__
    SessionManager* SessionManager::singletonSessionManager = NULL;
    
#endif // __SESSION_MANAGER_DECLARE__

} // namespace
#endif  //__SESSION_MANAGER__H_
