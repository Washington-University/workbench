#ifndef __BALSA_DATABASE_MANAGER_H__
#define __BALSA_DATABASE_MANAGER_H__

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


#include "CaretObject.h"

#include "EventListenerInterface.h"


namespace caret {

    class BalsaDatabaseManager : public CaretObject, public EventListenerInterface {
        
    public:
        BalsaDatabaseManager();
        
        virtual ~BalsaDatabaseManager();
        
        bool login(const AString& username,
                   const AString& password,
                   AString& errorMessageOut);

        AString getJSessionIdCookie() const;
        
        // ADD_NEW_METHODS_HERE

        virtual AString toString() const;
        
        virtual void receiveEvent(Event* event);

    private:
        BalsaDatabaseManager(const BalsaDatabaseManager&);

        BalsaDatabaseManager& operator=(const BalsaDatabaseManager&);
        
        AString m_username;
        
        AString m_password;
        
        AString m_jSessionIdCookie;
        
        bool m_debugFlag;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __BALSA_DATABASE_MANAGER_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __BALSA_DATABASE_MANAGER_DECLARE__

} // namespace
#endif  //__BALSA_DATABASE_MANAGER_H__
