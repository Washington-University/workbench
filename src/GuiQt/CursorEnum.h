#ifndef __CURSOR_ENUM__H_
#define __CURSOR_ENUM__H_

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

class CursorEnum {

public:
    /**
     * Enumerated values.
     */
    enum Enum {
        /** The default cursor which is usually the parent widget's cursor */
        CURSOR_DEFAULT,
        /** Arrow (typically same as CURSOR_DEFAULT but overrides cursor provided by parent widget, Qt::ArrowCursor) */
        CURSOR_ARROW,
        /** Closed Hand */
        CURSOR_CLOSED_HAND,
        /** Cross (plus symbol) */
        CURSOR_CROSS,
        /** Delete (X) cursor */
        CURSOR_DELETE,
        /** Drawing Pen */
        CURSOR_DRAWING_PEN,
        /** Size All "four arrows", Qt::SizeAllCursor */
        CURSOR_FOUR_ARROWS,
        /** Pointing hand, Qt::PointingHandCursor*/
        CURSOR_POINTING_HAND,
        /** Resize arrows pointing bottom left and top right */
        CURSOR_RESIZE_BOTTOM_LEFT_TOP_RIGHT,
        /** Resize arrows pointing top left and bottom right */
        CURSOR_RESIZE_BOTTOM_RIGHT_TOP_LEFT,
        /** Resize Horizontal */
        CURSOR_RESIZE_HORIZONTAL,
        /** Resize Vertical */
        CURSOR_RESIZE_VERTICAL,
        /** Half Rotation Cursor */
        CURSOR_HALF_ROTATION,
        /** Rotation Cursor */
        CURSOR_ROTATION,
        /** Wait, Qt::WaitCursor*/
        CURSOR_WAIT,
        /** What's this? Arrow with question mark */
        CURSOR_WHATS_THIS
    };


    ~CursorEnum();

    static Qt::CursorShape toQtCursorShape(Enum enumValue);
    
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
    CursorEnum(const Enum enumValue, 
                 const AString& name,
                 const AString& guiName,
               const Qt::CursorShape qtCursorShape);

    static const CursorEnum* findData(const Enum enumValue);

    /** Holds all instance of enum values and associated metadata */
    static std::vector<CursorEnum> enumData;

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
    
    /** The corresponding Qt Cursor Shape */
    Qt::CursorShape qtCursorShape;
};

#ifdef __CURSOR_ENUM_DECLARE__
std::vector<CursorEnum> CursorEnum::enumData;
bool CursorEnum::initializedFlag = false;
int32_t CursorEnum::integerCodeCounter = 0; 
#endif // __CURSOR_ENUM_DECLARE__

} // namespace
#endif  //__CURSOR_ENUM__H_
