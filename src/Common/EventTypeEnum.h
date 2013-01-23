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
        /** Invalid event */
        EVENT_INVALID,
        /** Delete a browser tab. */
        EVENT_BROWSER_TAB_DELETE,
        /** Get a browser tab by tab number */
        EVENT_BROWSER_TAB_GET,
        /** Get a ALL browser tabs */
        EVENT_BROWSER_TAB_GET_ALL,
        /** Create a new browser tab */
        EVENT_BROWSER_TAB_NEW,
        /** Get the content of a browser window */
        EVENT_BROWSER_WINDOW_CONTENT_GET,
        /** Create tabs after loading a file */
        EVENT_BROWSER_WINDOW_CREATE_TABS,
        /** Create a new browser window */
        EVENT_BROWSER_WINDOW_NEW,
        /** Get CaretMappable data files */
        EVENT_CARET_MAPPABLE_DATA_FILES_GET,
        /** Read a data file */
        EVENT_DATA_FILE_READ,
        /** Reload (replace) a data file with its saved version */
        EVENT_DATA_FILE_RELOAD,
        /** Get node data files */
        EVENT_GET_NODE_DATA_FILES,
        /** get or set the user input mode */
        EVENT_GET_OR_SET_USER_INPUT_MODE,
        /** Update all graphics windows */
        EVENT_GRAPHICS_UPDATE_ALL_WINDOWS,
        /** Update graphics in a window */
        EVENT_GRAPHICS_UPDATE_ONE_WINDOW,
        /** Highlight location when an identification occurs */
        EVENT_IDENTIFICATION_HIGHLIGHT_LOCATION,
        /** Remove all identification symbols */
        EVENT_IDENTIFICATION_SYMBOL_REMOVAL,
        /** request display of map scalar color data mapping editor */
        EVENT_MAP_SCALAR_DATA_COLOR_MAPPING_EDITOR_SHOW,
        /** model display controllers - ADD */
        EVENT_MODEL_DISPLAY_CONTROLLER_ADD,
        /** model display controllers - DELETE */
        EVENT_MODEL_DISPLAY_CONTROLLER_DELETE,
        /** model display controllers - get all*/
        EVENT_MODEL_DISPLAY_CONTROLLER_GET_ALL,
        /** model display controller surface get */
        EVENT_MODEL_DISPLAY_CONTROLLER_SURFACE_GET,
        /** model display yoking group controllers - get all*/
        EVENT_MODEL_DISPLAY_CONTROLLER_YOKING_GROUP_GET_ALL,
        /** open file request from the operating system (Mac only) for now */
        EVENT_OPERATING_SYSTEM_REQUEST_OPEN_DATA_FILE,
        /** Read the selected files in a spec file */
        EVENT_SPEC_FILE_READ_DATA_FILES,
        /** Validate that overlay is valid (it exists). */
        EVENT_OVERLAY_VALIDATE,
        /** Invalidate surface coloring */
        EVENT_SURFACE_COLORING_INVALIDATE,
        /** Get surfaces */
        EVENT_SURFACES_GET,
        /** Display/Hide the selection toolbox */
        EVENT_TOOLBOX_SELECTION_DISPLAY,
        /** Update the User-Interface */
        EVENT_USER_INTERFACE_UPDATE,
        /** Update the progress amount, text, or finished status */
        EVENT_PROGRESS_UPDATE,
        /** Update the information windows */
        EVENT_UPDATE_INFORMATION_WINDOWS,
        /** Update and show the time course dialog */
        EVENT_UPDATE_TIME_COURSE_DIALOG,
        /** Update Animation start time */
        EVENT_UPDATE_ANIMATION_START_TIME,
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
