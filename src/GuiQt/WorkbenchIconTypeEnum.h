#ifndef __WORKBENCH_ICON_TYPE_ENUM_H__
#define __WORKBENCH_ICON_TYPE_ENUM_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2026 Washington University School of Medicine
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

class WorkbenchIconTypeEnum {

public:
    /**
     * Enumerated values.
     */
    enum Enum {
        /** No Icon */
        NO_ICON,
        /** Annotation Delete */
        ANNOTATION_DELETE,
        /** Annotation Line Arrow Down */
        ANNOTATION_LINE_ARROW_DOWN,
        /** Annotation Line Arrow Up */
        ANNOTATION_LINE_ARROW_UP,
        /** Annotation New Shape Arrow */
        ANNOTATION_NEW_SHAPE_ARROW,
        /** Annotation New Shape Box */
        ANNOTATION_NEW_SHAPE_BOX,
        /** Annotation New Shape Image */
        ANNOTATION_NEW_SHAPE_IMAGE,
        /** Annotation New Shape Line */
        ANNOTATION_NEW_SHAPE_LINE,
        /** Annotation New Shape Marker */
        ANNOTATION_NEW_SHAPE_MARKER,
        /** Annotation New Shape Oval */
        ANNOTATION_NEW_SHAPE_OVAL,
        /** Annotation New Shape Polyhedron */
        ANNOTATION_NEW_SHAPE_POLYHEDRON,
        /** Annotation New Shape Polygon */
        ANNOTATION_NEW_SHAPE_POLYGON,
        /** Annotation New Shape Polyline */
        ANNOTATION_NEW_SHAPE_POLYLINE,
        /** Annotation New Shape Text */
        ANNOTATION_NEW_SHAPE_TEXT,
        /** Annotation New Space Chart */
        ANNOTATION_NEW_SPACE_CHART,
        /** Annotation New Space Histology */
        ANNOTATION_NEW_SPACE_HISTOLOGY,
        /** Annotation New Space Media File Name And Pixel */
        ANNOTATION_NEW_SPACE_MEDIA_FILE_NAME_AND_PIXEL,
        /** Annotation New Space Stereotaxic */
        ANNOTATION_NEW_SPACE_STEREOTAXIC,
        /** Annotation New Space Surface */
        ANNOTATION_NEW_SPACE_SURFACE,
        /** Annotation New Space Tab */
        ANNOTATION_NEW_SPACE_TAB,
        /** Annotation New Space Window */
        ANNOTATION_NEW_SPACE_WINDOW,
        /** Annotation Text Orientation Horizontal */
        ANNOTATION_TEXT_ORIENTATION_HORIZONTAL,
        /** Annotation Text Orientation Vertical */
        ANNOTATION_TEXT_ORIENTATION_VERTICAL,
        /** Annotation Text Align Horiz Center */
        ANNOTATION_TEXT_ALIGN_HORIZ_CENTER,
        /** Annotation Text Align Horiz Left */
        ANNOTATION_TEXT_ALIGN_HORIZ_LEFT,
        /** Annotation Text Align Horiz Right */
        ANNOTATION_TEXT_ALIGN_HORIZ_RIGHT,
        /** Annotation Text Align Vert Bottom */
        ANNOTATION_TEXT_ALIGN_VERT_BOTTOM,
        /** Annotation Text Align Vert Middle */
        ANNOTATION_TEXT_ALIGN_VERT_MIDDLE,
        /** Annotation Text Align Vert Top */
        ANNOTATION_TEXT_ALIGN_VERT_TOP,
        /** Orientation Anterior */
        ORIENTATION_ANTERIOR,
        /** Orientation Dorsal */
        ORIENTATION_DORSAL,
        /** Orientation Left */
        ORIENTATION_LEFT,
        /** Orientation Left Lateral*/
        ORIENTATION_LEFT_LATERAL,
        /** Orientation Left Medial*/
        ORIENTATION_LEFT_MEDIAL,
        /** Orientation Posterior */
        ORIENTATION_POSTERIOR,
        /** Orientation Redo */
        ORIENTATION_REDO,
        /** Orientation Region */
        ORIENTATION_REGION,
        /** Orientation Right */
        ORIENTATION_RIGHT,
        /** Orientation Right Lateral*/
        ORIENTATION_RIGHT_LATERAL,
        /** Orientation Right Medial*/
        ORIENTATION_RIGHT_MEDIAL,
        /** Orientation Undo */
        ORIENTATION_UNDO,
        /** Orientation Ventral */
        ORIENTATION_VENTRAL,
        /** Overlay Toolbox Color Bar */
        OVERLAY_TOOLBOX_COLOR_BAR,
        /** Overlay Toolbox Construct */
        OVERLAY_TOOLBOX_CONSTRUCT,
        /** Overlay Toolbox Wrench */
        OVERLAY_TOOLBOX_WRENCH,
        /** Recent Files Dialog  Favorite Filled */
        RECENT_FILES_DIALOG_FAVORITE_FILLED,
        /** Recent Files Dialog Favorite Outline */
        RECENT_FILES_DIALOG_FAVORITE_OUTLINE,
        /** Recent Files Dialog Forget Black*/
        RECENT_FILES_DIALOG_FORGET_BLACK,
        /** Recent Files Dialog Forget Red */
        RECENT_FILES_DIALOG_FORGET_RED,
        /** Recent Files Dialog Share */
        RECENT_FILES_DIALOG_SHARE,
        /** Recent Files HCP Image */
        RECENT_FILES_HCP_IMAGE,
        /** Recent Files X Image */
        RECENT_FILES_X_IMAGE,
        /** Scene dialog caution */
        SCENE_DIALOG_CAUTION,
        /** Spec File Dialog Delete */
        SPEC_FILE_DIALOG_DELETE,
        /** Spec File Dialog Load */
        SPEC_FILE_DIALOG_LOAD,
        /** Spec File Dialog Options */
        SPEC_FILE_DIALOG_OPTIONS,
        /** Spec File Dialog Reload */
        SPEC_FILE_DIALOG_RELOAD,
        /** Tabbar Data Tooltips */
        TABBAR_DATA_TOOLTIPS,
        /** Tabbar Features */
        TABBAR_FEATURES,
        /** Tabbar Help */
        TABBAR_HELP,
        /** Tabbar Identify Brainordinate */
        TABBAR_IDENTIFY_BRAINORDINATE,
        /** Tabbar Information */
        TABBAR_INFORMATION,
        /** Tabbar Macros Scroll */
        TABBAR_MACROS_SCROLL,
        /** Tabbar Movie */
        TABBAR_MOVIE,
        /** Tabbar Overlays */
        TABBAR_OVERLAYS,
        /** Tabbar Scenes Clap Board */
        TABBAR_SCENES_CLAP_BOARD,
        /** Tabbar Toolbar */
        TABBAR_TOOLBAR,
        /** Toolbar Misc Light Bulb */
        TOOLBAR_MISC_LIGHT_BULB,
        /** Toolbar Relock Arrow */
        TOOLBAR_RELOCK_ARROW,
        /** Toolbar Misc Ruler */
        TOOLBAR_MISC_RULER,
        /** Toolbar Misc Scissors */
        TOOLBAR_MISC_SCISSORS,
        /** Toolbar Slice Indices Move Crosshairs */
        TOOLBAR_SLICE_INDICES_MOVE_CROSSHAIRS,
        /** Toolbar Volume Slice Plane Axial */
        TOOLBAR_VOLUME_SLICE_PLANE_AXIAL,
        /** Toolbar Volume Slice Crosshairs */
        TOOLBAR_VOLUME_SLICE_CROSSHAIRS,
        /** Toolbar Volume Slice Crosshair Labels */
        TOOLBAR_VOLUME_SLICE_CROSSHAIR_LABELS,
        /** Toolbar Volume Slice Plane All */
        TOOLBAR_VOLUME_SLICE_PLANE_ALL,
        /** Toolbar Volume Slice Plane Coronal */
        TOOLBAR_VOLUME_SLICE_PLANE_CORONAL,
        /** Toolbar Volume Slice Plane Parasagittal */
        TOOLBAR_VOLUME_SLICE_PLANE_PARASAGITTAL
    };


    ~WorkbenchIconTypeEnum();

    static AString toName(Enum enumValue);
    
    static Enum fromName(const AString& name, bool* isValidOut);
    
    static AString toGuiName(Enum enumValue);
    
    static Enum fromGuiName(const AString& guiName, bool* isValidOut);
    
    static int32_t toIntegerCode(Enum enumValue);
    
    static Enum fromIntegerCode(const int32_t integerCode, bool* isValidOut);

    static void getAllEnums(std::vector<Enum>& allEnums);

    static void getAllNames(std::vector<AString>& allNames, const bool isSorted);

    static void getAllGuiNames(std::vector<AString>& allGuiNames, const bool isSorted);

private:
    WorkbenchIconTypeEnum(const Enum enumValue, 
                 const AString& name,
                 const AString& guiName);

    static const WorkbenchIconTypeEnum* findData(const Enum enumValue);

    /** Holds all instance of enum values and associated metadata */
    static std::vector<WorkbenchIconTypeEnum> enumData;

    /** Initialize instances that contain the enum values and metadata */
    static void initialize();

    /** Indicates instance of enum values and metadata have been initialized */
    static bool initializedFlag;
    
    /** Auto generated integer codes */
    static int32_t integerCodeCounter;
    
    /** The enumerated type value for an instance */
    Enum enumValue;

    /** The integer code associated with an enumerated value */
    int32_t integerCode;

    /** The name, a text string that is identical to the enumerated value */
    AString name;
    
    /** A user-friendly name that is displayed in the GUI */
    AString guiName;
};

#ifdef __WORKBENCH_ICON_TYPE_ENUM_DECLARE__
std::vector<WorkbenchIconTypeEnum> WorkbenchIconTypeEnum::enumData;
bool WorkbenchIconTypeEnum::initializedFlag = false;
int32_t WorkbenchIconTypeEnum::integerCodeCounter = 0; 
#endif // __WORKBENCH_ICON_TYPE_ENUM_DECLARE__

} // namespace
#endif  //__WORKBENCH_ICON_TYPE_ENUM_H__
