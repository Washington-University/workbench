#ifndef __EVENT_TYPE_ENUM__H_
#define __EVENT_TYPE_ENUM__H_

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


#include <stdint.h>
#include <vector>
#include "AString.h"

namespace caret {

/// Enumerated type for events.
class EventTypeEnum {

public:
    /**
     * Enumerated values.
     */
    enum Enum {
        /** Find a brain structure */
        EVENT_BRAIN_STRUCTURE_GET,
        /** Delete a browser tab. */
        EVENT_BROWSER_TAB_DELETE,
        /** Get a new browser tab by tab number */
        EVENT_BROWSER_TAB_GET,
        /** Create a new browser tab */
        EVENT_BROWSER_TAB_NEW,
        /** Read a data file */
        EVENT_DATA_FILE_READ,
        /** Get the content of a browser window */
        EVENT_GET_BROWSER_WINDOW_CONTENT,
        /** Get node data files */
        EVENT_GET_NODE_DATA_FILES,
        /** Update all graphics windows */
        EVENT_GRAPHICS_UPDATE_ALL_WINDOWS,
        /** Update graphics in a window */
        EVENT_GRAPHICS_UPDATE_ONE_WINDOW,
        /** model display controllers - ADD */
        EVENT_MODEL_DISPLAY_CONTROLLER_ADD,
        /** model display controllers - DELETE */
        EVENT_MODEL_DISPLAY_CONTROLLER_DELETE,
        /** model display controllers - get all*/
        EVENT_MODEL_DISPLAY_CONTROLLER_GET_ALL,
        /** Invalidate surface coloring */
        EVENT_SURFACE_COLORING_INVALIDATE,
        /** Update the User-Interface */
        EVENT_USER_INTERFACE_UPDATE,
        /** Update the progress amount, text, or finished status */
        EVENT_PROGRESS_UPDATE,
        /* THIS MUST ALWAYS BE LAST - NOT an event type but is number of event types */
        EVENT_COUNT 
    };


    ~EventTypeEnum();

    static AString toName(Enum enumValue);
    
    static Enum fromName(const AString& name, bool* isValidOut);
    
    static AString toGuiName(Enum enumValue);
    
    static Enum fromGuiName(const AString& guiName, bool* isValidOut);
    
    static void getAllEnums(std::vector<Enum>& allEnums);

private:
    EventTypeEnum(const Enum enumValue, 
                  const AString& name,
                  const AString& guiName);

    static const EventTypeEnum* findData(const Enum enumValue);

    /** Holds all instance of enum values and associated metadata */
    static std::vector<EventTypeEnum> enumData;

    /** Initialize instances that contain the enum values and metadata */
    static void initialize();

    /** Indicates instance of enum values and metadata have been initialized */
    static bool initializedFlag;
    
    /** The enumerated type value for an instance */
    Enum enumValue;

    /** The name, a text string that is identical to the enumerated value */
    AString name;
    
    /** A user-friendly name that is displayed in the GUI */
    AString guiName;
};

#ifdef __EVENT_TYPE_ENUM_DECLARE__
std::vector<EventTypeEnum> EventTypeEnum::enumData;
bool EventTypeEnum::initializedFlag = false;
#endif // __EVENT_TYPE_ENUM_DECLARE__

} // namespace
#endif  //__EVENT_TYPE_ENUM__H_
