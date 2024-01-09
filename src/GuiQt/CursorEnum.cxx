
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

#include <algorithm>
#define __CURSOR_ENUM_DECLARE__
#include "CursorEnum.h"
#undef __CURSOR_ENUM_DECLARE__

#include "CaretAssert.h"

using namespace caret;

    
/**
 * \class caret::CursorEnum 
 * \brief Enumerated type for Cursors (both Qt and custom)
 * \ingroup GuiQt
 *
 * An enumerated type for cursors.  Some cursors are 
 * from Qt and others are created using bitmaps.
 */
/**
 * Constructor.
 *
 * @param enumValue
 *    An enumerated value.
 * @param name
 *    Name of enumerated value.
 *
 * @param guiName
 *    User-friendly name for use in user-interface.
 */
CursorEnum::CursorEnum(const Enum enumValue,
                           const AString& name,
                       const AString& guiName,
                       const Qt::CursorShape qtCursorShape)
{
    this->enumValue     = enumValue;
    this->integerCode   = integerCodeCounter++;
    this->name          = name;
    this->guiName       = guiName;
    this->qtCursorShape = qtCursorShape;
}

/**
 * Destructor.
 */
CursorEnum::~CursorEnum()
{
}

/**
 * Initialize the enumerated metadata.
 */
void
CursorEnum::initialize()
{
    if (initializedFlag) {
        return;
    }
    initializedFlag = true;

    enumData.push_back(CursorEnum(CURSOR_DEFAULT, 
                                    "CURSOR_DEFAULT", 
                                    "Default Cursor",
                                  Qt::ArrowCursor));
    
    enumData.push_back(CursorEnum(CURSOR_ARROW, 
                                    "CURSOR_ARROW", 
                                    "Arrow Cursor",
                                  Qt::ArrowCursor));
    
    enumData.push_back(CursorEnum(CURSOR_CLOSED_HAND,
                                  "CURSOR_CLOSED_HAND",
                                  "Closed Hand Cursor",
                                  Qt::ClosedHandCursor));
    
    enumData.push_back(CursorEnum(CURSOR_CROSS,
                                  "CURSOR_CROSS",
                                  "Cross Cursor",
                                  Qt::CrossCursor));
    
    enumData.push_back(CursorEnum(CURSOR_DELETE,
                                    "CURSOR_DELETE",
                                    "Delete Cursor",
                                  Qt::ArrowCursor)); /* we draw cusor so this is not used */
    
    enumData.push_back(CursorEnum(CURSOR_DRAWING_PEN,
                                  "CURSOR_DRAWING_PEN",
                                  "Drawing Pen Cursor",
                                  Qt::ArrowCursor));
    
    enumData.push_back(CursorEnum(CURSOR_FOUR_ARROWS,
                                  "CURSOR_FOUR_ARROWS",
                                  "Four Arrows Cursor",
                                  Qt::SizeAllCursor));
    
    enumData.push_back(CursorEnum(CURSOR_HALF_ROTATION,
                                  "CURSOR_HALF_ROTATION",
                                  "Half Rotation Cursor",
                                  Qt::ArrowCursor));
    
    enumData.push_back(CursorEnum(CURSOR_POINTING_HAND,
                                    "CURSOR_POINTING_HAND", 
                                    "Pointing Hand Cursor",
                                  Qt::PointingHandCursor));
    
    enumData.push_back(CursorEnum(CURSOR_RESIZE_BOTTOM_LEFT_TOP_RIGHT,
                                  "CURSOR_RESIZE_BOTTOM_LEFT_TOP_RIGHT",
                                  "Resize arrows pointing bottom left and top right Cursor",
                                  Qt::SizeBDiagCursor));
    
    enumData.push_back(CursorEnum(CURSOR_RESIZE_BOTTOM_RIGHT_TOP_LEFT,
                                  "CURSOR_RESIZE_BOTTOM_RIGHT_TOP_LEFT",
                                  "Resize arrows pointing top left and bottom right Cursor",
                                  Qt::SizeFDiagCursor));
    
    enumData.push_back(CursorEnum(CURSOR_RESIZE_HORIZONTAL,
                                  "CURSOR_RESIZE_HORIZONTAL",
                                  "Resize Horizontal Cursor",
                                  Qt::SizeHorCursor));
    
    enumData.push_back(CursorEnum(CURSOR_RESIZE_VERTICAL,
                                  "CURSOR_RESIZE_VERTICAL",
                                  "Resize Vertical Cursor",
                                  Qt::SizeVerCursor));
    
    enumData.push_back(CursorEnum(CURSOR_ROTATION,
                                  "CURSOR_ROTATION",
                                  "Rotation Cursor",
                                  Qt::ArrowCursor));
    
    enumData.push_back(CursorEnum(CURSOR_WAIT,
                                    "CURSOR_WAIT", 
                                    "Wait Cursor",
                                  Qt::WaitCursor));
    
    enumData.push_back(CursorEnum(CURSOR_WHATS_THIS,
                                  "CURSOR_WHATS_THIS",
                                  "What's this Cursor",
                                  Qt::WhatsThisCursor));
}

/**
 * Find the data for and enumerated value.
 * @param enumValue
 *     The enumerated value.
 * @return Pointer to data for this enumerated type
 * or NULL if no data for type or if type is invalid.
 */
const CursorEnum*
CursorEnum::findData(const Enum enumValue)
{
    if (initializedFlag == false) initialize();

    size_t num = enumData.size();
    for (size_t i = 0; i < num; i++) {
        const CursorEnum* d = &enumData[i];
        if (d->enumValue == enumValue) {
            return d;
        }
    }

    return NULL;
}

/**
 * Get the Qt Cursor corresponding to the enumerated type.
 * @param enumValue
 *     Enumerated value.
 * @return
 *     String representing enumerated value.
 */
Qt::CursorShape
CursorEnum::toQtCursorShape(Enum enumValue)
{
    if (initializedFlag == false) initialize();
    
    const CursorEnum* enumInstance = findData(enumValue);
    return enumInstance->qtCursorShape;
}

/**
 * Get a string representation of the enumerated type.
 * @param enumValue 
 *     Enumerated value.
 * @return 
 *     String representing enumerated value.
 */
AString 
CursorEnum::toName(Enum enumValue) {
    if (initializedFlag == false) initialize();
    
    const CursorEnum* enumInstance = findData(enumValue);
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
CursorEnum::Enum 
CursorEnum::fromName(const AString& name, bool* isValidOut)
{
    if (initializedFlag == false) initialize();
    
    bool validFlag = false;
    Enum enumValue = CURSOR_DEFAULT;
    
    for (std::vector<CursorEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const CursorEnum& d = *iter;
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
        CaretAssertMessage(0, AString("Name " + name + "failed to match enumerated value for type CursorEnum"));
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
CursorEnum::toGuiName(Enum enumValue) {
    if (initializedFlag == false) initialize();
    
    const CursorEnum* enumInstance = findData(enumValue);
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
CursorEnum::Enum 
CursorEnum::fromGuiName(const AString& guiName, bool* isValidOut)
{
    if (initializedFlag == false) initialize();
    
    bool validFlag = false;
    Enum enumValue = CURSOR_DEFAULT;
    
    for (std::vector<CursorEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const CursorEnum& d = *iter;
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
        CaretAssertMessage(0, AString("guiName " + guiName + "failed to match enumerated value for type CursorEnum"));
    }
    return enumValue;
}

/**
 * Get the integer code for a data type.
 *
 * @return
 *    Integer code for data type.
 */
int32_t
CursorEnum::toIntegerCode(Enum enumValue)
{
    if (initializedFlag == false) initialize();
    const CursorEnum* enumInstance = findData(enumValue);
    return enumInstance->integerCode;
}

/**
 * Find the data type corresponding to an integer code.
 *
 * @param integerCode
 *     Integer code for enum.
 * @param isValidOut
 *     If not NULL, on exit isValidOut will indicate if
 *     integer code is valid.
 * @return
 *     Enum for integer code.
 */
CursorEnum::Enum
CursorEnum::fromIntegerCode(const int32_t integerCode, bool* isValidOut)
{
    if (initializedFlag == false) initialize();
    
    bool validFlag = false;
    Enum enumValue = CURSOR_DEFAULT;
    
    for (std::vector<CursorEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const CursorEnum& enumInstance = *iter;
        if (enumInstance.integerCode == integerCode) {
            enumValue = enumInstance.enumValue;
            validFlag = true;
            break;
        }
    }
    
    if (isValidOut != 0) {
        *isValidOut = validFlag;
    }
    else if (validFlag == false) {
        CaretAssertMessage(0, AString("Integer code " + AString::number(integerCode) + "failed to match enumerated value for type CursorEnum"));
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
CursorEnum::getAllEnums(std::vector<CursorEnum::Enum>& allEnums)
{
    if (initializedFlag == false) initialize();
    
    allEnums.clear();
    
    for (std::vector<CursorEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        allEnums.push_back(iter->enumValue);
    }
}

/**
 * Get all of the names of the enumerated type values.
 *
 * @param allNames
 *     A vector that is OUTPUT containing all of the names of the enumerated values.
 * @param isSorted
 *     If true, the names are sorted in alphabetical order.
 */
void
CursorEnum::getAllNames(std::vector<AString>& allNames, const bool isSorted)
{
    if (initializedFlag == false) initialize();
    
    allNames.clear();
    
    for (std::vector<CursorEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        allNames.push_back(CursorEnum::toName(iter->enumValue));
    }
    
    if (isSorted) {
        std::sort(allNames.begin(), allNames.end());
    }
}

/**
 * Get all of the GUI names of the enumerated type values.
 *
 * @param allNames
 *     A vector that is OUTPUT containing all of the GUI names of the enumerated values.
 * @param isSorted
 *     If true, the names are sorted in alphabetical order.
 */
void
CursorEnum::getAllGuiNames(std::vector<AString>& allGuiNames, const bool isSorted)
{
    if (initializedFlag == false) initialize();
    
    allGuiNames.clear();
    
    for (std::vector<CursorEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        allGuiNames.push_back(CursorEnum::toGuiName(iter->enumValue));
    }
    
    if (isSorted) {
        std::sort(allGuiNames.begin(), allGuiNames.end());
    }
}

