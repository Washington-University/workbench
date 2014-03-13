#ifndef __EVENT_DATA_FILE_RELOAD_H__
#define __EVENT_DATA_FILE_RELOAD_H__

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


#include "Event.h"

namespace caret {

    class Brain;
    class CaretDataFile;
    
    class EventDataFileReload : public Event {
        
    public:
        EventDataFileReload(Brain* brain,
                            CaretDataFile* caretDataFile);
        
        virtual ~EventDataFileReload();
        
        Brain* getBrain();
        
        CaretDataFile* getCaretDataFile();
        
        bool isError() const;
        
        AString getErrorMessage() const;
        
        void setErrorMessage(const AString& errorMessage);
        
        AString getUsername() const;
        
        AString getPassword() const;
        
        void setUsernameAndPassword(const AString& username,
                                    const AString& password);
        
    private:
        EventDataFileReload(const EventDataFileReload&);

        EventDataFileReload& operator=(const EventDataFileReload&);
        
    public:

        // ADD_NEW_METHODS_HERE

    private:

        Brain* m_brain;
        
        CaretDataFile* m_caretDataFile;
        
        AString m_username;
        
        AString m_password;
        
        AString m_errorMessage;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __EVENT_DATA_FILE_RELOAD_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __EVENT_DATA_FILE_RELOAD_DECLARE__

} // namespace
#endif  //__EVENT_DATA_FILE_RELOAD_H__
