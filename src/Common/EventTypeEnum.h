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
        /** Get color and scale bars from tab(s) */
        EVENT_ANNOTATION_BARS_GET,
        /** Annotation create new of a particular type */
        EVENT_ANNOTATION_CREATE_NEW_TYPE,
        /** Annotation finish cancel annotation being drawn */
        EVENT_ANNOTATION_DRAWING_FINISH_CANCEL,
        /** Get the annotation that is being drawn in the given window */
        EVENT_ANNOTATION_GET_BEING_DRAWN_IN_WINDOW,
        /** Get the annotations drawn in a window */
        EVENT_ANNOTATION_GET_DRAWN_IN_WINDOW,
        /** Get the file selected in the "insert new" section of toolbar */
        EVENT_ANNOTATION_GET_SELECTED_INSERT_NEW_FILE,
        /** Get an annotation group */
        EVENT_ANNOTATION_GROUP_GET_WITH_KEY,
        /** Annotation grouping (group, regroup, ungroup) operation */
        EVENT_ANNOTATION_GROUPING,
        /** Annotation get slice depth while drawing a new polyhedron */
        EVENT_ANNOTATION_NEW_DRAWING_POLYHEDRON_SLICE_DEPTH,
        /** Get an annotation polyhedron using its linked identifier */
        EVENT_ANNOTATION_POLYHEDRON_GET_BY_LINKED_IDENTIFIER,
        /** Annotation polyhedron name  component settings - which components to display */
        EVENT_ANNOTATION_POLYHEDRON_NAME_COMPONENT_SETTINGS,
        /** Get the bounds of annotation text */
        EVENT_ANNOTATION_TEXT_GET_BOUNDS,
        /** Get annotation text substitutions */
        EVENT_ANNOTATION_TEXT_SUBSTITUTION_GET,
        /** Get all text substitution group IDs from all text annotations */
        EVENT_ANNOTATION_TEXT_SUBSTITUTION_GET_ALL_GROUP_IDS,
        /** Invalid annotation text substitutions */
        EVENT_ANNOTATION_TEXT_SUBSTITUTION_INVALIDATE,
        /** Annotation toolbar update */
        EVENT_ANNOTATION_TOOLBAR_UPDATE,
        /** Test a pointer to an annotation to verify that it is still valid (exists) */
        EVENT_ANNOTATION_VALIDATE,
        /** Inform that Brain has been reset (new spec or scene loaded) */
        EVENT_BRAIN_RESET,
        /** Get all brain structures */
        EVENT_BRAIN_STRUCTURE_GET_ALL,
        /** Close a browser tab but may reopen later. */
        EVENT_BROWSER_TAB_CLOSE,
        /** Close (for possible later reopening) a browser tab from within the toolbar */
        EVENT_BROWSER_TAB_CLOSE_IN_TOOL_BAR,
        /** Delete a browser tab. */
        EVENT_BROWSER_TAB_DELETE,
        /** Delete a browser tab from within the toolbar */
        EVENT_BROWSER_TAB_DELETE_IN_TOOL_BAR,
        /** Get a browser tab by tab number */
        EVENT_BROWSER_TAB_GET,
        /** Get indices of all valid browser tabs */
        EVENT_BROWSER_TAB_INDICES_GET_ALL,
        /** Get indices of all viewed browser tabs */
        EVENT_BROWSER_TAB_INDICES_GET_ALL_VIEWED,
        /** Get ALL (both viewed and not viewed) browser tabs */
        EVENT_BROWSER_TAB_GET_ALL,
        /** Get ALL VIEWED browser tabs (tabs that are viewed in windows) */
        EVENT_BROWSER_TAB_GET_ALL_VIEWED,
        /** Get the browser tab at the window's XY */
        EVENT_BROWSER_TAB_GET_AT_WINDOW_XY,
        /** Get the window index for the given tab index*/
        EVENT_BROWSER_TAB_INDEX_GET_WINDOW_INDEX,
        /** Create a new browser tab */
        EVENT_BROWSER_TAB_NEW,
        /** Create a new browser tab from code within the GUI so toolbar is updated */
        EVENT_BROWSER_TAB_NEW_IN_GUI,
        /** Create a new browser tab by cloning an existing browser tab */
        EVENT_BROWSER_TAB_NEW_CLONE,
        /** Reopen a closed tab */
        EVENT_BROWSER_TAB_REOPEN_AVAILBLE,
        /** Reopen a closed tab */
        EVENT_BROWSER_TAB_REOPEN_CLOSED,
        /** Select  a  tab in a window */
        EVENT_BROWSER_TAB_SELECT_IN_WINDOW,
        /** Event to test validity of a browser tab */
        EVENT_BROWSER_TAB_VALIDATE,
        /** Event for browser window content */
        EVENT_BROWSER_WINDOW_CONTENT,
        /** Get the content of a browser window */
        EVENT_BROWSER_WINDOW_DRAWING_CONTENT_GET,
        /** Create tabs after loading a file */
        EVENT_BROWSER_WINDOW_CREATE_TABS,
        /** Get tabs in a browser window */
        EVENT_BROWSER_WINDOW_GET_TABS,
        /** Issued after a browser window's graphicshave been redrawn */
        EVENT_BROWSER_WINDOW_GRAPHICS_HAVE_BEEN_REDRAWN,
        /** Issued when displayed browser window menu's may change */
        EVENT_BROWSER_WINDOW_MENUS_UPDATE,
        /** Create a new browser window */
        EVENT_BROWSER_WINDOW_NEW,
        /** Pixel info (sizes) in a browser window */
        EVENT_BROWSER_WINDOW_PIXEL_SIZE_INFO,
        /** Browser tile tab operations */
        EVENT_BROWSER_WINDOW_TILE_TAB_OPERATION,
        /** Get caret data files */
        EVENT_CARET_DATA_FILES_GET,
        /** Get CaretMappable data files */
        EVENT_CARET_MAPPABLE_DATA_FILES_GET,
        /** Get CaretMappableDataFiles and their maps viewed as overlays */
        EVENT_CARET_MAPPABLE_DATA_FILE_MAPS_VIEWED_IN_OVERLAYS,
        /** Get all mappable files and selected maps in all displayed overlays */
        EVENT_CARET_MAPPABLE_DATA_FILES_AND_MAPS_IN_DISPLAYED_OVERLAYS,
        /** Event to get the Caret Preferences */
        EVENT_CARET_PREFERENCES_GET,
        /** Event for yoking the loading of matrix chart rows/columns */
        EVENT_CHART_MATRIX_YOKING_VALIDATION,
        /** Get a chart cartesian axis for a display group */
        EVENT_CHART_TWO_CARTEISAN_AXIS_DISPLAY_GROUP,
        /** Event for yoking of chart two cartesian oriented axes */
        EVENT_CHART_TWO_CARTESIAN_ORIENTED_AXES_YOKING,
        /** Load chart two line series data */
        EVENT_CHART_TWO_LOAD_LINE_SERIES_DATA,
        /** Validate that chart two overlay is valid (it exists). */
        EVENT_CHART_TWO_OVERLAY_VALIDATE,
        /** Add a data file into the Brain*/
        EVENT_DATA_FILE_ADD,
        /** Delete a data file from the brain */
        EVENT_DATA_FILE_DELETE,
        /** Read a data file into the Brain */
        EVENT_DATA_FILE_READ,
        /** Reload (replace) a data file with its saved version in the brain*/
        EVENT_DATA_FILE_RELOAD,
        /** Reload (replace) a data file with its saved version in the brain*/
        EVENT_DATA_FILE_RELOAD_ALL,
        /** Event used by/with DisplayPropertiesLabels */
        EVENT_DISPLAY_PROPERTIES_LABELS,
        /** Drawing viewport content event, add new while drawing */
        EVENT_DRAWING_VIEWPORT_CONTENT_ADD,
        /** Drawing viewport content event, clear before drawing*/
        EVENT_DRAWING_VIEWPORT_CONTENT_CLEAR,
        /** Drawing viewport content event, get after drawing */
        EVENT_DRAWING_VIEWPORT_CONTENT_GET,
        /** Get a focus class or name color from a focus file */
        EVENT_FOCUS_FILE_GET_COLOR,
        /** Get data files that are display in windows/tabs */
        EVENT_GET_DISPLAYED_DATA_FILES,
        /** Get node data files */
        EVENT_GET_NODE_DATA_FILES,
        /** get or set the user input mode */
        EVENT_GET_OR_SET_USER_INPUT_MODE,
        /** Get the text renderer for a window */
        EVENT_GET_TEXT_RENDERER_FOR_WINDOW,
        /** Get the user input mode for a window */
        EVENT_GET_USER_INPUT_MODE,
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
        /** Paint (draw immediately) graphics in a window */
        EVENT_GRAPHICS_PAINT_NOW_ALL_WINDOWS,
        /** Paint  (draw immediately) graphics in a window */
        EVENT_GRAPHICS_PAINT_NOW_ONE_WINDOW,
        /** Paint (draw soon but not immediately) all graphics windows */
        EVENT_GRAPHICS_PAINT_SOON_ALL_WINDOWS,
        /** Paint (draw soon but not immediately) all graphics windows */
        EVENT_GRAPHICS_PAINT_SOON_ONE_WINDOW,
        /** Time the OpenGL graphics in a window */
        EVENT_GRAPHICS_TIMING_ONE_WINDOW,
        /** Show tooltip in graphics window */
        EVENT_GRAPHICS_WINDOW_SHOW_TOOL_TIP,
        /** Display the help viewer */
        EVENT_HELP_VIEWER_DISPLAY,
        /** Get histology slices files */
        EVENT_HISTOLOGY_SLICES_FILES_GET,
        /** Highlight location when an identification occurs */
        EVENT_IDENTIFICATION_HIGHLIGHT_LOCATION,
        /** Perform an identification operation */
        EVENT_IDENTIFICATION_REQUEST,
        /** Remove all identification symbols */
        EVENT_IDENTIFICATION_SYMBOL_REMOVAL,
        /** Browser window image capture */
        EVENT_IMAGE_CAPTURE,
        /** Validate when adding a mapped file to mapped yoking */
        EVENT_MAP_YOKING_SELECT_MAP,
        /** Select a map for mapped yoked files */
        EVENT_MAP_YOKING_VALIDATION,
        /** Get media files */
        EVENT_MEDIA_FILES_GET,
        /** model - ADD */
        EVENT_MODEL_ADD,
        /** model  - DELETE */
        EVENT_MODEL_DELETE,
        /** model - get all*/
        EVENT_MODEL_GET_ALL,
        /** model - get all displayed */
        EVENT_MODEL_GET_ALL_DISPLAYED,
        /** model surface - get */
        EVENT_MODEL_SURFACE_GET,
        /** Update the movie dialog */
        EVENT_MOVIE_RECORDING_DIALOG_UPDATE,
        /** Movie manual mode image recording */
        EVENT_MOVIE_RECORDING_MANUAL_MODE_CAPTURE,
        /** Get the color for a node's identification symbol from a chart that contains the node */
        EVENT_NODE_IDENTIFICATION_COLORS_GET_FROM_CHARTS,
        /** Get the transformation for converting object coordinates to window coordinates */
        EVENT_OPENGL_OBJECT_TO_WINDOW_TRANSFORM,
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
        /** Get palette groups */
        EVENT_PALETTE_GROUPS_GET,
        /** Get the file system access mode for recent files */
        EVENT_RECENT_FILES_SYSTEM_ACCESS_MODE,
        /** Issued when reset view is requested */
        EVENT_RESET_VIEW,
        /** Get the active scene */
        EVENT_SCENE_ACTIVE,
        /** Show a dialog containing warnings encountered when reading data files */
        EVENT_SHOW_FILE_DATA_READ_WARNING_DIALOG,
        /** Get a spacer tab by tab number */
        EVENT_SPACER_TAB_GET,
        /** Read the selected files in a spec file */
        EVENT_SPEC_FILE_READ_DATA_FILES,
        /** Invalidate surface coloring */
        EVENT_SURFACE_COLORING_INVALIDATE,
        /** Invalidate volume coloring */
        EVENT_VOLUME_COLORING_INVALIDATE,
        /** Get surfaces (Surface instance) */
        EVENT_SURFACES_GET,
        /** Get surface files with structure and number of nodes */
        EVENT_SURFACE_FILE_GET,
        /** Get surface nodes within some distance of XYZ */
        EVENT_SURFACE_NODES_GET_NEAR_XYZ,
        /** Get valid surface strucutures and their number of nodes */
        EVENT_SURFACE_STRUCTURES_VALID_GET,
        /** Tile tabs modification */
        EVENT_TILE_TABS_MODIFICATION,
        /** Update the toolbar's chart two oriented axes controls */
        EVENT_TOOLBAR_CHART_ORIENTED_AXES_UPDATE,
        /** Display/Hide the selection toolbox */
        EVENT_TOOLBOX_SELECTION_DISPLAY,
        /** Update the User-Interface */
        EVENT_USER_INTERFACE_UPDATE,
        /** Update the progress amount, text, or finished status */
        EVENT_PROGRESS_UPDATE,
        /** Update the information windows */
        EVENT_UPDATE_INFORMATION_WINDOWS,
        /** Update the volume editing toolbar */
        EVENT_UPDATE_VOLUME_EDITING_TOOLBAR,
        /** Update the slice indices and coordinates in the toolbar */
        EVENT_UPDATE_VOLUME_SLICE_INDICES_COORDS_TOOLBAR,
        /** Update yoked windows */
        EVENT_UPDATE_YOKED_WINDOWS,
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
