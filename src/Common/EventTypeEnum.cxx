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

#define __EVENT_TYPE_ENUM_DECLARE__
#include "EventTypeEnum.h"
#undef __EVENT_TYPE_ENUM_DECLARE__

#include "CaretAssert.h"

using namespace caret;

/**
 * Constructor.
 *
 * @param enumValue
 *    An enumerated value.
 * @param name
 *    Name of enumberated value.
 */
EventTypeEnum::EventTypeEnum(const Enum enumValue,
                           const AString& name,
                           const AString& guiName)
{
    this->enumValue = enumValue;
    this->name = name;
    this->guiName = guiName;
}

/**
 * Destructor.
 */
EventTypeEnum::~EventTypeEnum()
{
}

/**
 * Initialize the enumerated metadata.
 */
void
EventTypeEnum::initialize()
{
    if (initializedFlag) {
        return;
    }
    initializedFlag = true;

    enumData.push_back(EventTypeEnum(EVENT_INVALID, 
                                     "EVENT_INVALID", 
                                     "Invalid Event"));
    
    enumData.push_back(EventTypeEnum(EVENT_ALERT_USER,
                                     "EVENT_ALERT_USER",
                                     "Alert user about something (if gui, a pop is displayed, otherwise logged at severe level"));
    
    enumData.push_back(EventTypeEnum(EVENT_ANNOTATION_ADD_TO_REMOVE_FROM_FILE,
                                     "EVENT_ANNOTATION_ADD_TO_REMOVE_FROM_FILE",
                                     "Event to add or remove an annotation from a file"));

    enumData.push_back(EventTypeEnum(EVENT_ANNOTATION_COLOR_BAR_GET,
                                     "EVENT_ANNOTATION_COLOR_BAR_GET",
                                     "Event to get annotation color bars from tab(s)"));
    
    enumData.push_back(EventTypeEnum(EVENT_ANNOTATION_CREATE_NEW_TYPE,
                                     "EVENT_ANNOTATION_CREATE_NEW_TYPE",
                                     "Event to create a new annotation of a particular type"));
    
    enumData.push_back(EventTypeEnum(EVENT_ANNOTATION_TOOLBAR_UPDATE,
                                     "EVENT_ANNOTATION_TOOLBAR_UPDATE",
                                     "Event to update annotation toolbar"));
    
    enumData.push_back(EventTypeEnum(EVENT_BRAIN_RESET,
                                     "EVENT_BRAIN_RESET",
                                     "Brain has been reset"));
                                     
    enumData.push_back(EventTypeEnum(EVENT_BRAIN_STRUCTURE_GET_ALL,
                                     "EVENT_BRAIN_STRUCTURE_GET_ALL",
                                     "Get all brain structures"));
    
    enumData.push_back(EventTypeEnum(EVENT_BROWSER_TAB_DELETE,
                                     "EVENT_BROWSER_TAB_DELETE", 
                                     "Delete a browser tab"));
    
    enumData.push_back(EventTypeEnum(EVENT_BROWSER_TAB_GET, 
                                    "EVENT_BROWSER_TAB_GET", 
                                    "Get a browser tab by number"));
    
    enumData.push_back(EventTypeEnum(EVENT_BROWSER_TAB_GET_ALL, 
                                     "EVENT_BROWSER_TAB_GET_ALL", 
                                     "Get ALL browser tabs"));

    enumData.push_back(EventTypeEnum(EVENT_BROWSER_TAB_GET_ALL_VIEWED,
                                     "EVENT_BROWSER_TAB_GET_ALL_VIEWED",
                                     "Get ALL Viewed browser tabs"));

    enumData.push_back(EventTypeEnum(EVENT_BROWSER_TAB_INDICES_GET_ALL,
                                     "EVENT_BROWSER_TAB_INDICES_GET_ALL",
                                     "Browser Tab Indices Get All"));
    
    enumData.push_back(EventTypeEnum(EVENT_BROWSER_TAB_NEW,
                                     "EVENT_BROWSER_TAB_NEW", 
                                     "Create a browser tab"));
    
    enumData.push_back(EventTypeEnum(EVENT_BROWSER_WINDOW_CONTENT_GET, 
                                     "EVENT_BROWSER_WINDOW_CONTENT_GET", 
                                     "Get the content in a browser window"));

    enumData.push_back(EventTypeEnum(EVENT_BROWSER_WINDOW_CREATE_TABS, 
                                     "EVENT_BROWSER_WINDOW_CREATE_TABS", 
                                     "Create tabs (if needed) after loading data files"));
    
    enumData.push_back(EventTypeEnum(EVENT_BROWSER_WINDOW_GRAPHICS_HAVE_BEEN_REDRAWN,
                                     "EVENT_BROWSER_WINDOW_GRAPHICS_HAVE_BEEN_REDRAWN",
                                     "A Browser Window's graphics have been redrawn"));
    
    enumData.push_back(EventTypeEnum(EVENT_BROWSER_WINDOW_MENUS_UPDATE,
                                     "EVENT_BROWSER_WINDOW_MENUS_UPDATE",
                                     "Update the browser windows menus"));
    
    enumData.push_back(EventTypeEnum(EVENT_BROWSER_WINDOW_NEW,
                                     "EVENT_BROWSER_WINDOW_NEW", 
                                     "Create a new browser window"));
    
    enumData.push_back(EventTypeEnum(EVENT_CARET_MAPPABLE_DATA_FILES_GET, 
                                     "EVENT_CARET_MAPPABLE_DATA_FILES_GET", 
                                     "Get all Caret Mappable data files"));

    enumData.push_back(EventTypeEnum(EVENT_CARET_MAPPABLE_DATA_FILE_MAPS_VIEWED_IN_OVERLAYS,
                                     "EVENT_CARET_MAPPABLE_DATA_FILE_MAPS_VIEWED_IN_OVERLAYS",
                                     "Get Caret Mappable data file maps viewed in overlays"));
    
    enumData.push_back(EventTypeEnum(EVENT_CHART_MATRIX_YOKING_VALIDATION,
                                     "EVENT_CHART_MATRIX_YOKING_VALIDATION",
                                     "Validate Yoking of matrix chart's rows/columns"));

    enumData.push_back(EventTypeEnum(EVENT_DATA_FILE_ADD,
                                     "EVENT_DATA_FILE_ADD",
                                     "Add a data file to the Brain"));
    
    enumData.push_back(EventTypeEnum(EVENT_DATA_FILE_DELETE,
                                     "EVENT_DATA_FILE_DELETE",
                                     "Delete a data file from the Brain"));
    
    enumData.push_back(EventTypeEnum(EVENT_DATA_FILE_READ,
                                    "EVENT_DATA_FILE_READ", 
                                    "Read a data file into the Brain"));
    
    enumData.push_back(EventTypeEnum(EVENT_DATA_FILE_RELOAD,
                                     "EVENT_DATA_FILE_RELOAD",
                                     "Reopen a data file (replace it with saved version) in the Brain"));
    
    enumData.push_back(EventTypeEnum(EVENT_GET_DISPLAYED_DATA_FILES,
                                     "EVENT_GET_DISPLAYED_DATA_FILES",
                                     "Get data files displayed in windows/tabs"));
    
    enumData.push_back(EventTypeEnum(EVENT_GET_NODE_DATA_FILES,
                                     "EVENT_GET_NODE_DATA_FILES",
                                     "Get node data files"));
    
    enumData.push_back(EventTypeEnum(EVENT_GET_OR_SET_USER_INPUT_MODE,
                                     "EVENT_GET_OR_SET_USER_INPUT_MODE",
                                     "Get or set the user input mode"));
    
    enumData.push_back(EventTypeEnum(EVENT_GET_VIEWPORT_SIZE,
                                     "EVENT_GET_VIEWPORT_SIZE",
                                     "Get the viewport size"));
    
    enumData.push_back(EventTypeEnum(EVENT_GRAPHICS_UPDATE_ALL_WINDOWS,
                                     "EVENT_GRAPHICS_UPDATE_ALL_WINDOWS", 
                                     "Update all graphics windows"));
    
    enumData.push_back(EventTypeEnum(EVENT_GRAPHICS_UPDATE_ONE_WINDOW, 
                                     "EVENT_GRAPHICS_UPDATE_ONE_WINDOW", 
                                     "Update graphics in one window"));
    
    enumData.push_back(EventTypeEnum(EVENT_HELP_VIEWER_DISPLAY,
                                     "EVENT_HELP_VIEWER_DISPLAY", 
                                     "Display the help viewer"));
    
    enumData.push_back(EventTypeEnum(EVENT_IDENTIFICATION_HIGHLIGHT_LOCATION,
                                     "EVENT_IDENTIFICATION_HIGHLIGHT_LOCATION",
                                     "Highlight the location when identification takes place"));
    
    enumData.push_back(EventTypeEnum(EVENT_IDENTIFICATION_REQUEST,
                                     "EVENT_IDENTIFICATION_REQUEST",
                                     "Request an identification operation"));
    
    enumData.push_back(EventTypeEnum(EVENT_IDENTIFICATION_SYMBOL_REMOVAL,
                                     "EVENT_IDENTIFICATION_SYMBOL_REMOVAL", 
                                     "Remove all identification symbols"));
    
    enumData.push_back(EventTypeEnum(EVENT_IMAGE_CAPTURE,
                                     "EVENT_IMAGE_CAPTURE",
                                     "Capture an Image of Browser Window Graphics Region"));

    enumData.push_back(EventTypeEnum(EVENT_MAC_DOCK_MENU_UPDATE,
                                     "EVENT_MAC_DOCK_MENU_UPDATE",
                                     "Update the Mac Dock Menu"));
    
    enumData.push_back(EventTypeEnum(EVENT_MAP_YOKING_SELECT_MAP,
                                     "EVENT_MAP_YOKING_SELECT_MAP", 
                                     "Map Yoking Select Map"));
    
    enumData.push_back(EventTypeEnum(EVENT_MAP_YOKING_VALIDATION,
                                     "EVENT_MAP_YOKING_VALIDATION",
                                     "Map Yoking Validation"));
    
    enumData.push_back(EventTypeEnum(EVENT_MODEL_ADD,
                                     "EVENT_MODEL_ADD",
                                     "Add a model"));
    
    enumData.push_back(EventTypeEnum(EVENT_MODEL_DELETE,
                                    "EVENT_MODEL_DELETE", 
                                    "Delete a model"));
    
    enumData.push_back(EventTypeEnum(EVENT_MODEL_GET_ALL, 
                                    "EVENT_MODEL_GET_ALL", 
                                    "Get all models"));

    enumData.push_back(EventTypeEnum(EVENT_MODEL_SURFACE_GET, 
                                     "EVENT_MODEL_SURFACE_GET", 
                                     "Get a specific model surface"));
    
    enumData.push_back(EventTypeEnum(EVENT_NODE_IDENTIFICATION_COLORS_GET_FROM_CHARTS,
                                     "EVENT_NODE_IDENTIFICATION_COLORS_GET_FROM_CHARTS",
                                     "Get the color for node identification symbols from all charts that contain nodes"));
    
    enumData.push_back(EventTypeEnum(EVENT_OPERATING_SYSTEM_REQUEST_OPEN_DATA_FILE,
                                     "EVENT_OPERATING_SYSTEM_REQUEST_OPEN_DATA_FILE",
                                     "Operating system requests open data file (Mac only)"));
    
    enumData.push_back(EventTypeEnum(EVENT_OVERLAY_SETTINGS_EDITOR_SHOW,
                                     "EVENT_OVERLAY_SETTINGS_EDITOR_SHOW",
                                     "Request display of overlay settings editor"));
    
    enumData.push_back(EventTypeEnum(EVENT_OVERLAY_VALIDATE,
                                     "EVENT_OVERLAY_VALIDATE",
                                     "Validate an overlay for validity (it exists)"));
    
    enumData.push_back(EventTypeEnum(EVENT_PALETTE_COLOR_MAPPING_EDITOR_SHOW,
                                     "EVENT_PALETTE_COLOR_MAPPING_EDITOR_SHOW",
                                     "Request display of palette color mapping editor"));
    
    enumData.push_back(EventTypeEnum(EVENT_PALETTE_GET_BY_NAME,
                                     "EVENT_PALETTE_GET_BY_NAME",
                                     "Read the selected files in a spec file"));
    
    enumData.push_back(EventTypeEnum(EVENT_SPEC_FILE_READ_DATA_FILES,
                                     "EVENT_SPEC_FILE_READ_DATA_FILES",
                                     "Read the selected data files in a spec file"));
    
    enumData.push_back(EventTypeEnum(EVENT_SURFACE_COLORING_INVALIDATE, 
                                     "EVENT_SURFACE_COLORING_INVALIDATE", 
                                     "Invalidate surface coloring"));
    
    enumData.push_back(EventTypeEnum(EVENT_SURFACES_GET, 
                                     "EVENT_SURFACES_GET", 
                                     "Get Surfaces"));
    
    enumData.push_back(EventTypeEnum(EVENT_SURFACE_STRUCTURES_VALID_GET,
                                     "EVENT_SURFACE_STRUCTURES_VALID_GET",
                                     "GGet valid surface strucutures and their number of node"));

    enumData.push_back(EventTypeEnum(EVENT_TOOLBOX_SELECTION_DISPLAY,
                                     "EVENT_TOOLBOX_SELECTION_DISPLAY", 
                                     "Display or hide the selection toolbox"));
    
    enumData.push_back(EventTypeEnum(EVENT_USER_INTERFACE_UPDATE, 
                                     "EVENT_USER_INTERFACE_UPDATE", 
                                     "Update the user-interface"));
    
    enumData.push_back(EventTypeEnum(EVENT_PROGRESS_UPDATE, 
                                     "EVENT_PROGRESS_UPDATE", 
                                     "Update the progress amount, text, or finished status"));

    enumData.push_back(EventTypeEnum(EVENT_UPDATE_INFORMATION_WINDOWS, 
                                     "EVENT_UPDATE_INFORMATION_WINDOWS", 
                                     "Update the information windows"));

    enumData.push_back(EventTypeEnum(EVENT_UPDATE_YOKED_WINDOWS,
                                     "EVENT_UPDATE_YOKED_WINDOWS",
                                     "Update yoked windows graphics and toolbar"));

    enumData.push_back(EventTypeEnum(EVENT_UPDATE_VOLUME_EDITING_TOOLBAR,
                                     "EVENT_UPDATE_VOLUME_EDITING_TOOLBAR",
                                     "Update the volume editing toolbar"));

    enumData.push_back(EventTypeEnum(EVENT_COUNT,
                                    "EVENT_COUNT", 
                                    "Count of events"));
    
    CaretAssertMessage((enumData.size() == static_cast<uint64_t>(EVENT_COUNT + 1)),
                       ("Number of EventTypeEnum::Enum values is incorrect.\n"
                        "Have enumerated type been added?\n"
                        "enumData.size()="
                        + AString::number(enumData.size())
                        + "   EVENT_COUNT+1="
                        + AString::number(EVENT_COUNT + 1)));
}

/**
 * Find the data for and enumerated value.
 * @param enumValue
 *     The enumerated value.
 * @return Pointer to data for this enumerated type
 * or NULL if no data for type or if type is invalid.
 */
const EventTypeEnum*
EventTypeEnum::findData(const Enum enumValue)
{
    if (initializedFlag == false) initialize();

    size_t num = enumData.size();
    for (size_t i = 0; i < num; i++) {
        const EventTypeEnum* d = &enumData[i];
        if (d->enumValue == enumValue) {
            return d;
        }
    }

    return NULL;
}

/**
 * Get a string representation of the enumerated type.
 * @param enumValue 
 *     Enumerated value.
 * @return 
 *     String representing enumerated value.
 */
AString 
EventTypeEnum::toName(Enum enumValue) {
    if (initializedFlag == false) initialize();
    
    const EventTypeEnum* enumInstance = findData(enumValue);
    return enumInstance->name;
}

/**
 * Get an enumerated value corresponding to its name.
 * @param name 
 *     Name of enumerated value.
 * @param isValidOut 
 *     If not NULL, it is set indicating that a
 *     enum value exists for the input name.
 * @return 
 *     Enumerated value.
 */
EventTypeEnum::Enum 
EventTypeEnum::fromName(const AString& name, bool* isValidOut)
{
    if (initializedFlag == false) initialize();
    
    bool validFlag = false;
    Enum enumValue = EVENT_INVALID;
    
    for (std::vector<EventTypeEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const EventTypeEnum& d = *iter;
        if (d.name == name) {
            enumValue = d.enumValue;
            validFlag = true;
            break;
        }
    }
    
    if (isValidOut != 0) {
        *isValidOut = validFlag;
    }
    else if (validFlag == false) {
        CaretAssertMessage(0, AString("Name " + name + "failed to match enumerated value for type EventTypeEnum"));
    }
    return enumValue;
}

/**
 * Get a GUI string representation of the enumerated type.
 * @param enumValue 
 *     Enumerated value.
 * @return 
 *     String representing enumerated value.
 */
AString 
EventTypeEnum::toGuiName(Enum enumValue) {
    if (initializedFlag == false) initialize();
    
    const EventTypeEnum* enumInstance = findData(enumValue);
    return enumInstance->guiName;
}

/**
 * Get an enumerated value corresponding to its GUI name.
 * @param s 
 *     Name of enumerated value.
 * @param isValidOut 
 *     If not NULL, it is set indicating that a
 *     enum value exists for the input name.
 * @return 
 *     Enumerated value.
 */
EventTypeEnum::Enum 
EventTypeEnum::fromGuiName(const AString& guiName, bool* isValidOut)
{
    if (initializedFlag == false) initialize();
    
    bool validFlag = false;
    Enum enumValue = EVENT_INVALID;
    
    for (std::vector<EventTypeEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const EventTypeEnum& d = *iter;
        if (d.guiName == guiName) {
            enumValue = d.enumValue;
            validFlag = true;
            break;
        }
    }
    
    if (isValidOut != 0) {
        *isValidOut = validFlag;
    }
    else if (validFlag == false) {
        CaretAssertMessage(0, AString("guiName " + guiName + "failed to match enumerated value for type EventTypeEnum"));
    }
    return enumValue;
}

/**
 * Get all of the enumerated type values.  The values can be used
 * as parameters to toXXX() methods to get associated metadata.
 *
 * @param allEnums
 *     A vector that is OUTPUT containing all of the enumerated values.
 */
void
EventTypeEnum::getAllEnums(std::vector<EventTypeEnum::Enum>& allEnums)
{
    if (initializedFlag == false) initialize();
    
    allEnums.clear();
    
    for (std::vector<EventTypeEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        allEnums.push_back(iter->enumValue);
    }
}

