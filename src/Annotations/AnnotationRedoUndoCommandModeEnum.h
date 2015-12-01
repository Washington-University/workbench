#ifndef __ANNOTATION_UNDO_COMMAND_MODE_ENUM_H__
#define __ANNOTATION_UNDO_COMMAND_MODE_ENUM_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2015 Washington University School of Medicine
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

class AnnotationRedoUndoCommandModeEnum {

public:
    /**
     * Enumerated values.
     */
    enum Enum {
        /** Invalid mode */
        INVALID,
        /** Color - Background */
        COLOR_BACKGROUND,
        /** Color - Foreground */
        COLOR_FOREGROUND,
        /** Coordinate One */
        COORDINATE_ONE,
        /** Coordinate One And Two */
        COORDINATE_ONE_AND_TWO,
        /** Coordinate Two */
        COORDINATE_TWO,
        /** Create an annotation */
        CREATE_ANNOTATION,
        /** Cut Annotation */
        CUT_ANNOTATION,
        /** Delete Annotations */
        DELETE_ANNOTATIONS,
        /** Line Arrow Start */
        LINE_ARROW_START,
        /** Line Arrow End */
        LINE_ARROW_END,
        /** Line Width of Foreground */
        LINE_WIDTH_FOREGROUND,
        /** Location and size of annotations (coords, size, space, window, tab) */
        LOCATION_AND_SIZE,
        /** Paste Annotation */
        PASTE_ANNOTATION,
        /** Rotation Angle */
        ROTATION_ANGLE,
        /** Text Alignment Horizontal */
        TEXT_ALIGNMENT_HORIZONTAL,
        /** Text Alignment Vertical */
        TEXT_ALIGNMENT_VERTICAL,
        /** Text Characters */
        TEXT_CHARACTERS,
        /** Text Connect to Brainordinate */
        TEXT_CONNECT_TO_BRAINORDINATE,
        /** Text Font Bold */
        TEXT_FONT_BOLD,
        /** Text Font Italic */
        TEXT_FONT_ITALIC,
        /** Text Font Name */
        TEXT_FONT_NAME,
        /** Text Font Percent Size */
        TEXT_FONT_PERCENT_SIZE,
        /** Text Font Point Size */
        TEXT_FONT_POINT_SIZE,
        /** Text Font Undeline */
        TEXT_FONT_UNDERLINE,
        /** Text Orientation */
        TEXT_ORIENTATION,
        /** Two dimensional (box, oval) height */
        TWO_DIM_HEIGHT,
        /** Two dimensional (box, oval) width */
        TWO_DIM_WIDTH
    };


    ~AnnotationRedoUndoCommandModeEnum();

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
    AnnotationRedoUndoCommandModeEnum(const Enum enumValue, 
                 const AString& name,
                 const AString& guiName);

    static const AnnotationRedoUndoCommandModeEnum* findData(const Enum enumValue);

    /** Holds all instance of enum values and associated metadata */
    static std::vector<AnnotationRedoUndoCommandModeEnum> enumData;

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

#ifdef __ANNOTATION_UNDO_COMMAND_MODE_ENUM_DECLARE__
std::vector<AnnotationRedoUndoCommandModeEnum> AnnotationRedoUndoCommandModeEnum::enumData;
bool AnnotationRedoUndoCommandModeEnum::initializedFlag = false;
int32_t AnnotationRedoUndoCommandModeEnum::integerCodeCounter = 0; 
#endif // __ANNOTATION_UNDO_COMMAND_MODE_ENUM_DECLARE__

} // namespace
#endif  //__ANNOTATION_UNDO_COMMAND_MODE_ENUM_H__
