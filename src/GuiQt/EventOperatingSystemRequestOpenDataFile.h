#ifndef __EVENT_OPERATING_SYSTEM_REQUEST_OPEN_DATA_FILE_H__
#define __EVENT_OPERATING_SYSTEM_REQUEST_OPEN_DATA_FILE_H__

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

    class EventOperatingSystemRequestOpenDataFile : public Event {
        
    public:
        EventOperatingSystemRequestOpenDataFile(const AString& dataFileName);
        
        virtual ~EventOperatingSystemRequestOpenDataFile();
        
        AString getDataFileName() const { return m_dataFileName; }
        
    private:
        EventOperatingSystemRequestOpenDataFile(const EventOperatingSystemRequestOpenDataFile&);
        
        EventOperatingSystemRequestOpenDataFile& operator=(const EventOperatingSystemRequestOpenDataFile&);

        const AString m_dataFileName;
    };

} // namespace

#endif // __EVENT_OPERATING_SYSTEM_REQUEST_OPEN_DATA_FILE_H__
