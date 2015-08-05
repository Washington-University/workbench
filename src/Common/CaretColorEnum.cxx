
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
#define __CARET_COLOR_ENUM_DECLARE__
#include "CaretColorEnum.h"
#undef __CARET_COLOR_ENUM_DECLARE__

#include "CaretAssert.h"

using namespace caret;

    
/**
 * \class caret::CaretColorEnum 
 * \brief Enumerate types for standard colors (HTML 4.01)
 *
 * Enumerated types for standard colors
 */

/**
 * Constructor.
 *
 * @param enumValue
 *    An enumerated value.
 * @param name
 *    Name of enumerated value.
 * @param guiName
 *    User-friendly name for use in user-interface.
 * @param red
 *    Red color component [0 - 1]
 * @param green
 *    Green color component [0 - 1]
 * @param blue
 *    Blue color component [0 - 1]
 */
CaretColorEnum::CaretColorEnum(const Enum enumValue,
                               const AString& name,
                               const AString& guiName,
                               const float red,
                               const float green,
                               const float blue)
{
    this->enumValue = enumValue;
    this->integerCode = integerCodeCounter++;
    this->name = name;
    this->guiName = guiName;
    
    this->rgb[0]= red;
    this->rgb[1]= green;
    this->rgb[2]= blue;
}

/**
 * Destructor.
 */
CaretColorEnum::~CaretColorEnum()
{
}

/**
 * Initialize the enumerated metadata.
 */
void
CaretColorEnum::initialize()
{
    if (initializedFlag) {
        return;
    }
    initializedFlag = true;

    enumData.push_back(CaretColorEnum(NONE,
                                      "NONE",
                                      "None",
                                      0,
                                      0,
                                      0));
    
    enumData.push_back(CaretColorEnum(CUSTOM,
                                      "CUSTOM",
                                      "Custom",
                                      1,
                                      1,
                                      1));
    
    enumData.push_back(CaretColorEnum(AQUA,
                                      "AQUA", 
                                      "Aqua",
                                      0,
                                      1,
                                      1));
    
    enumData.push_back(CaretColorEnum(BLACK, 
                                      "BLACK", 
                                      "Black",
                                      0,
                                      0,
                                      0));
    
    enumData.push_back(CaretColorEnum(BLUE, 
                                      "BLUE", 
                                      "Blue",
                                      0,
                                      0,
                                      1));
    
    enumData.push_back(CaretColorEnum(FUCHSIA, 
                                      "FUCHSIA", 
                                      "Fuchsia",
                                      1,
                                      0,
                                      1));
    
    enumData.push_back(CaretColorEnum(GRAY, 
                                      "GRAY", 
                                      "Gray",
                                      0.50,
                                      0.50,
                                      0.50));
    
    enumData.push_back(CaretColorEnum(GREEN, 
                                      "GREEN", 
                                      "Green",
                                      0,
                                      0.5,
                                      0));
    
    enumData.push_back(CaretColorEnum(LIME, 
                                      "LIME", 
                                      "Lime",
                                      0,
                                      1,
                                      0));
    
    enumData.push_back(CaretColorEnum(MAROON, 
                                      "MAROON", 
                                      "Maroon",
                                      0.5,
                                      0,
                                      0));
    
    enumData.push_back(CaretColorEnum(NAVY, 
                                      "NAVY", 
                                      "Navy",
                                      0,
                                      0,
                                      0.5));
    
    enumData.push_back(CaretColorEnum(OLIVE, 
                                      "OLIVE", 
                                      "Olive",
                                      0.5,
                                      0.5,
                                      0));
    
    enumData.push_back(CaretColorEnum(PURPLE, 
                                      "PURPLE", 
                                      "Purple",
                                      0.5,
                                      0,
                                      0.5));
    
    enumData.push_back(CaretColorEnum(RED, 
                                      "RED", 
                                      "Red",
                                      1,
                                      0,
                                      0));
    
    enumData.push_back(CaretColorEnum(SILVER, 
                                      "SILVER", 
                                      "Silver",
                                      0.75,
                                      0.75,
                                      0.75));
    
    enumData.push_back(CaretColorEnum(TEAL, 
                                      "TEAL", 
                                      "Teal",
                                      0,
                                      0.5,
                                      0.5));
    
    enumData.push_back(CaretColorEnum(WHITE, 
                                      "WHITE", 
                                      "White",
                                      1,
                                      1,
                                      1));
    
    enumData.push_back(CaretColorEnum(YELLOW, 
                                      "YELLOW", 
                                      "Yellow",
                                      1,
                                      1,
                                      0));
}

/**
 * Find the data for and enumerated value.
 * @param enumValue
 *     The enumerated value.
 * @return Pointer to data for this enumerated type
 * or NULL if no data for type or if type is invalid.
 */
const CaretColorEnum*
CaretColorEnum::findData(const Enum enumValue)
{
    if (initializedFlag == false) initialize();

    size_t num = enumData.size();
    for (size_t i = 0; i < num; i++) {
        const CaretColorEnum* d = &enumData[i];
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
CaretColorEnum::toName(Enum enumValue) {
    if (initializedFlag == false) initialize();
    
    const CaretColorEnum* enumInstance = findData(enumValue);
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
CaretColorEnum::Enum 
CaretColorEnum::fromName(const AString& name, bool* isValidOut)
{
    if (initializedFlag == false) initialize();
    
    bool validFlag = false;
    Enum enumValue = BLACK;
    
    for (std::vector<CaretColorEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const CaretColorEnum& d = *iter;
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
        CaretAssertMessage(0, AString("Name " + name + "failed to match enumerated value for type CaretColorEnum"));
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
CaretColorEnum::toGuiName(Enum enumValue) {
    if (initializedFlag == false) initialize();
    
    const CaretColorEnum* enumInstance = findData(enumValue);
    return enumInstance->guiName;
}

/**
 * Get the RGB components (ranging zero to one) of the enumerated type.
 * @param enumValue 
 *     Enumerated value.
 * @return  Pointer to RGB components.
 */
const float* 
CaretColorEnum::toRGB(Enum enumValue)
{
    if (initializedFlag == false) initialize();
    
    const CaretColorEnum* enumInstance = findData(enumValue);
    return enumInstance->rgb;
}

/**
 * Get the RGB components (ranging zero to one) of the enumerated type.
 * @param enumValue
 *     Enumerated value.
 * @param rgbOut
 *     Output with RGB components [0.0, 1.0]
 */
void
CaretColorEnum::toRGBFloat(Enum enumValue,
                           float rgbOut[3])
{
    if (initializedFlag == false) initialize();
    
    const CaretColorEnum* enumInstance = findData(enumValue);
    rgbOut[0] = enumInstance->rgb[0];
    rgbOut[1] = enumInstance->rgb[1];
    rgbOut[2] = enumInstance->rgb[2];
}

/**
 * Get the RGB components (ranging zero to one) of the enumerated type.
 * @param enumValue
 *     Enumerated value.
 * @param rgbOut
 *     Output with RGB components [0.0, 1.0]
 */
void
CaretColorEnum::toRGBByte(Enum enumValue,
                          uint8_t rgbOut[3])
{
    float rgbFloat[3];
    toRGBFloat(enumValue, rgbFloat);

    rgbOut[0] = static_cast<uint8_t>(rgbFloat[0] * 255.0);
    rgbOut[1] = static_cast<uint8_t>(rgbFloat[1] * 255.0);
    rgbOut[2] = static_cast<uint8_t>(rgbFloat[2] * 255.0);
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
CaretColorEnum::Enum 
CaretColorEnum::fromGuiName(const AString& guiName, bool* isValidOut)
{
    if (initializedFlag == false) initialize();
    
    bool validFlag = false;
    Enum enumValue = BLACK;
    
    for (std::vector<CaretColorEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const CaretColorEnum& d = *iter;
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
        CaretAssertMessage(0, AString("guiName " + guiName + "failed to match enumerated value for type CaretColorEnum"));
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
CaretColorEnum::toIntegerCode(Enum enumValue)
{
    if (initializedFlag == false) initialize();
    const CaretColorEnum* enumInstance = findData(enumValue);
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
CaretColorEnum::Enum
CaretColorEnum::fromIntegerCode(const int32_t integerCode, bool* isValidOut)
{
    if (initializedFlag == false) initialize();
    
    bool validFlag = false;
    Enum enumValue = BLACK;
    
    for (std::vector<CaretColorEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const CaretColorEnum& enumInstance = *iter;
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
        CaretAssertMessage(0, AString("Integer code " + AString::number(integerCode) + "failed to match enumerated value for type CaretColorEnum"));
    }
    return enumValue;
}

/**
 * Get all of the enumerated type values THAT REPRESENT A COLOR.  The values can be used
 * as parameters to toXXX() methods to get associated metadata.
 *
 * @param allColorEnums
 *     A vector that is OUTPUT containing all of the VALID COLOR enumerated values.
 *     No optional enum values are included.
 */
void
CaretColorEnum::getAllEnums(std::vector<CaretColorEnum::Enum>& allColorEnums)
{
    if (initializedFlag == false) initialize();
    
    getColorAndOptionalEnums(allColorEnums,
                             OPTION_NO_OPTIONS);
}

/**
 * Get all of the enumerated type values THAT REPRESENT A COLOR.  The values can be used
 * as parameters to toXXX() methods to get associated metadata.
 *
 * @param allColorEnums
 *     A vector that is OUTPUT containing all of the VALID COLOR enumerated values.
 *     No optional enum values are included.
 */
void
CaretColorEnum::getColorEnums(std::vector<CaretColorEnum::Enum>& allColorEnums)
{
    if (initializedFlag == false) initialize();
    
    getColorAndOptionalEnums(allColorEnums,
                             OPTION_NO_OPTIONS);
}

/**
 * Get all of the enumerated type values THAT REPRESENT A COLOR and the specified
 * optional enums.  The values can be used
 * as parameters to toXXX() methods to get associated metadata.
 *
 * @param allEnums
 *     A vector that is OUTPUT containing all of the VALID COLOR enumerated values
 *     and also the optional enums.
 * @param colorOptions
 *     Bitwise OR of the color options.
 */
void
CaretColorEnum::getColorAndOptionalEnums(std::vector<Enum>& allEnums,
                                         const int64_t colorOptions)
{
    if (initializedFlag == false) initialize();
    
    allEnums.clear();
    
    const bool includeCustomColorFlag = (colorOptions & OPTION_INCLUDE_CUSTOM_COLOR);
    const bool includeNoneColorFlag   = (colorOptions & OPTION_INCLUDE_NONE_COLOR);
    
    for (std::vector<CaretColorEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const CaretColorEnum::Enum colorEnum = iter->enumValue;
        
        bool addColorEnumFlag = true;
        if (colorEnum == CUSTOM) {
            if ( ! includeCustomColorFlag) {
                addColorEnumFlag = false;
            }
        }
        else if (colorEnum == NONE) {
            if ( ! includeNoneColorFlag) {
                addColorEnumFlag = false;
            }
        }
        
        if (addColorEnumFlag) {
            allEnums.push_back(iter->enumValue);
        }
    }
}


///**
// * Get all of the names of the enumerated type values.
// *
// * @param allNames
// *     A vector that is OUTPUT containing all of the names of the enumerated values.
// * @param isSorted
// *     If true, the names are sorted in alphabetical order.
// */
//void
//CaretColorEnum::getAllNames(std::vector<AString>& allNames, 
//                            const bool isSorted)
//{
//    if (initializedFlag == false) initialize();
//    
//    allNames.clear();
//    
//    std::vector<CaretColorEnum::Enum> allEnums;
//    CaretColorEnum::getColorEnums(allEnums);
//    for (std::vector<CaretColorEnum::Enum>::iterator iter = allEnums.begin();
//         iter != allEnums.end();
//         iter++) {
//        allNames.push_back(CaretColorEnum::toName(*iter));
//    }
//    
//    if (isSorted) {
//        std::sort(allNames.begin(), allNames.end());
//    }
//}
//
///**
// * Get all of the GUI names of the enumerated type values.
// *
// * @param allNames
// *     A vector that is OUTPUT containing all of the GUI names of the enumerated values.
// * @param isSorted
// *     If true, the names are sorted in alphabetical order.
// */
//void
//CaretColorEnum::getAllGuiNames(std::vector<AString>& allGuiNames, 
//                               const bool isSorted)
//{
//    if (initializedFlag == false) initialize();
//    
//    allGuiNames.clear();
//    
//    std::vector<CaretColorEnum::Enum> allEnums;
//    CaretColorEnum::getColorEnums(allEnums);
//    for (std::vector<CaretColorEnum::Enum>::iterator iter = allEnums.begin();
//         iter != allEnums.end();
//         iter++) {
//        allGuiNames.push_back(CaretColorEnum::toGuiName(*iter));
//    }
//    
//    if (isSorted) {
//        std::sort(allGuiNames.begin(), allGuiNames.end());
//    }
//}

