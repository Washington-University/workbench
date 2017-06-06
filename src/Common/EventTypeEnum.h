#ifndef __EVENT_TYPE_ENUM__H_
#define __EVENT_TYPE_ENUM__H_

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
        /** Alert user about something */
        EVENT_ALERT_USER,
        /** Add annotation to or remove from a file */
        EVENT_ANNOTATION_ADD_TO_REMOVE_FROM_FILE,
        /** Get annnotation chart labels */
        EVENT_ANNOTATION_CHART_LABEL_GET,
        /** Get color bars from tab(s) */
        EVENT_ANNOTATION_COLOR_BAR_GET,
        /** Annotation create new of a particular type */
        EVENT_ANNOTATION_CREATE_NEW_TYPE,
        /** Get the annotations drawn in a window */
        EVENT_ANNOTATION_GET_DRAWN_IN_WINDOW,
        /** Get an annotation group */
        EVENT_ANNOTATION_GROUP_GET_WITH_KEY,
        /** Annotation grouping (group, regroup, ungroup) operation */
        EVENT_ANNOTATION_GROUPING,
        /** Annotation toolbar update */
        EVENT_ANNOTATION_TOOLBAR_UPDATE,
        /** Inform that Brain has been reset (new spec or scene loaded) */
        EVENT_BRAIN_RESET,
        /** Get all brain structures */
        EVENT_BRAIN_STRUCTURE_GET_ALL,
        /** Delete a browser tab. */
        EVENT_BROWSER_TAB_DELETE,
        /** Get a browser tab by tab number */
        EVENT_BROWSER_TAB_GET,
        /** Get indices of all valid browser tabs */
        EVENT_BROWSER_TAB_INDICES_GET_ALL,
        /** Get ALL (both viewed and not viewed) browser tabs */
        EVENT_BROWSER_TAB_GET_ALL,
        /** Get ALL VIEWED browser tabs (tabs that are viewed in windows) */
        EVENT_BROWSER_TAB_GET_ALL_VIEWED,
        /** Create a new browser tab */
        EVENT_BROWSER_TAB_NEW,
        /** Get the content of a browser window */
        EVENT_BROWSER_WINDOW_CONTENT_GET,
        /** Create tabs after loading a file */
        EVENT_BROWSER_WINDOW_CREATE_TABS,
        /** Issued after a browser window's graphicshave been redrawn */
        EVENT_BROWSER_WINDOW_GRAPHICS_HAVE_BEEN_REDRAWN,
        /** Issued when displayed browser window menu's may change */
        EVENT_BROWSER_WINDOW_MENUS_UPDATE,
        /** Create a new browser window */
        EVENT_BROWSER_WINDOW_NEW,
        /** Get CaretMappable data files */
        EVENT_CARET_MAPPABLE_DATA_FILES_GET,
        /** Get CaretMappableDataFiles and their maps viewed as overlays */
        EVENT_CARET_MAPPABLE_DATA_FILE_MAPS_VIEWED_IN_OVERLAYS,
        /** Event to get the Caret Preferences */
        EVENT_CARET_PREFERENCES_GET,
        /** Event for yoking the loading of matrix chart rows/columns */
        EVENT_CHART_MATRIX_YOKING_VALIDATION,
        /** Validate that chart overlay is valid (it exists). */
        EVENT_CHART_OVERLAY_VALIDATE,
        /** Add a data file into the Brain*/
        EVENT_DATA_FILE_ADD,
        /** Delete a data file from the brain */
        EVENT_DATA_FILE_DELETE,
        /** Read a data file into the Brain */
        EVENT_DATA_FILE_READ,
        /** Reload (replace) a data file with its saved version in the brain*/
        EVENT_DATA_FILE_RELOAD,
        /** Get data files that are display in windows/tabs */
        EVENT_GET_DISPLAYED_DATA_FILES,
        /** Get node data files */
        EVENT_GET_NODE_DATA_FILES,
        /** get or set the user input mode */
        EVENT_GET_OR_SET_USER_INPUT_MODE,
        /** Get the text renderer for a window */
        EVENT_GET_TEXT_RENDERER_FOR_WINDOW,
        /** Get the viewport size for model, tab, window */
        EVENT_GET_VIEWPORT_SIZE,
        /** Create a buffer object for an OpenGL context */
        EVENT_GRAPHICS_OPENGL_CREATE_BUFFER_OBJECT,
        /** Create a texture name for an OpenGL context */
        EVENT_GRAPHICS_OPENGL_CREATE_TEXTURE_NAME,
        /** Delete a buffer object for an OpenGL context */
        EVENT_GRAPHICS_OPENGL_DELETE_BUFFER_OBJECT,
        /** Delete a texture name for an OpenGL context */
        EVENT_GRAPHICS_OPENGL_DELETE_TEXTURE_NAME,
        /** Update all graphics windows */
        EVENT_GRAPHICS_UPDATE_ALL_WINDOWS,
        /** Update graphics in a window */
        EVENT_GRAPHICS_UPDATE_ONE_WINDOW,
        /** Display the help viewer */
        EVENT_HELP_VIEWER_DISPLAY,
        /** Highlight location when an identification occurs */
        EVENT_IDENTIFICATION_HIGHLIGHT_LOCATION,
        /** Perform an identification operation */
        EVENT_IDENTIFICATION_REQUEST,
        /** Remove all identification symbols */
        EVENT_IDENTIFICATION_SYMBOL_REMOVAL,
        /** Browser window image capture */
        EVENT_IMAGE_CAPTURE,
        /** Update the Mac Dock Menu */
        EVENT_MAC_DOCK_MENU_UPDATE,
        /** Validate when adding a mapped file to mapped yoking */
        EVENT_MAP_YOKING_SELECT_MAP,
        /** Select a map for mapped yoked files */
        EVENT_MAP_YOKING_VALIDATION,
        /** model - ADD */
        EVENT_MODEL_ADD,
        /** model  - DELETE */
        EVENT_MODEL_DELETE,
        /** model - get all*/
        EVENT_MODEL_GET_ALL,
        /** model surface - get */
        EVENT_MODEL_SURFACE_GET,
        /** Get the color for a node's identification symbol from a chart that contains the node */
        EVENT_NODE_IDENTIFICATION_COLORS_GET_FROM_CHARTS,
        /** open file request from the operating system (Mac only) for now */
        EVENT_OPERATING_SYSTEM_REQUEST_OPEN_DATA_FILE,
        /** request display of overlay settings editor */
        EVENT_OVERLAY_SETTINGS_EDITOR_SHOW,
        /** Validate that overlay is valid (it exists). */
        EVENT_OVERLAY_VALIDATE,
        /** request display of palette color mapping editor */
        EVENT_PALETTE_COLOR_MAPPING_EDITOR_SHOW,
        /** Get a palette by name from a palette file */
        EVENT_PALETTE_GET_BY_NAME,
        /** Read the selected files in a spec file */
        EVENT_SPEC_FILE_READ_DATA_FILES,
        /** Invalidate surface coloring */
        EVENT_SURFACE_COLORING_INVALIDATE,
        /** Get surfaces */
        EVENT_SURFACES_GET,
        /** Get valid surface strucutures and their number of nodes */
        EVENT_SURFACE_STRUCTURES_VALID_GET,
        /** Display/Hide the selection toolbox */
        EVENT_TOOLBOX_SELECTION_DISPLAY,
        /** Update the User-Interface */
        EVENT_USER_INTERFACE_UPDATE,
        /** Update the progress amount, text, or finished status */
        EVENT_PROGRESS_UPDATE,
        /** Update the information windows */
        EVENT_UPDATE_INFORMATION_WINDOWS,
        /** Event to update yoked windows (graphics and toolbar) */
        EVENT_UPDATE_YOKED_WINDOWS,
        /** Update the volume editing toolbar */
        EVENT_UPDATE_VOLUME_EDITING_TOOLBAR,
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
