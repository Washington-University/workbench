
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

#define __YOKING_TYPE_ENUM_DECLARE__
#include "YokingTypeEnum.h"
#undef __YOKING_TYPE_ENUM_DECLARE__

using namespace caret;

/**
 * Constructor.
 *
 * @param e
 *    An enumerated value.
 * @param integerCode
 *    The integer code.
 * @param name
 *    Name of enumberated value.
 * @param name
 *    GUI Name of enumberated value.
 */
YokingTypeEnum::YokingTypeEnum(const Enum e,
                           const int32_t integerCode,
                           const AString& name,
                           const AString& guiName)
{
    this->e = e;
    this->integerCode = integerCode;
    this->name = name;
    this->guiName = guiName;
}

/**
 * Destructor.
 */
YokingTypeEnum::~YokingTypeEnum()
{
}

/**
 * Initialize the enumerated metadata.
 */
void
YokingTypeEnum::initialize()
{
    if (initializedFlag) {
        return;
    }
    initializedFlag = true;

    enumData.push_back(YokingTypeEnum(OFF, 
                                    0, 
                                    "OFF", 
                                    "Off"));
    
    enumData.push_back(YokingTypeEnum(ON, 
                                    1, 
                                    "ON", 
                                    "On"));
    
    enumData.push_back(YokingTypeEnum(ON_LATERAL_MEDIAL, 
                                    2, 
                                    "ON_LATERAL_MEDIAL", 
                                    "On L/M"));
    
}

/**
 * Find the data for and enumerated value.
 * @param e
 *     The enumerated value.
 * @return Pointer to data for this enumerated type
 * or NULL if no data for type or if type is invalid.
 */
const YokingTypeEnum*
YokingTypeEnum::findData(const Enum e)
{
    initialize();

    size_t num = enumData.size();
    for (size_t i = 0; i < num; i++) {
        const YokingTypeEnum* d = &enumData[i];
        if (d->e == e) {
            return d;
        }
    }

    return NULL;
}

/**
 * Get a string representation of the enumerated type.
 * @param e 
 *     Enumerated value.
 * @return 
 *     String representing enumerated value.
 */
AString 
YokingTypeEnum::toName(Enum e) {
    initialize();
    
    const YokingTypeEnum* enumValue = findData(e);
    return enumValue->name;
}

/**
 * Get an enumerated value corresponding to its name.
 * @param s 
 *     Name of enumerated value.
 * @param isValidOut 
 *     If not NULL, it is set indicating that a
 *     enum value exists for the input name.
 * @return 
 *     Enumerated value.
 */
YokingTypeEnum::Enum 
YokingTypeEnum::fromName(const AString& s, bool* isValidOut)
{
    initialize();
    
    bool validFlag = false;
    Enum e = OFF;
    
    for (std::vector<YokingTypeEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const YokingTypeEnum& d = *iter;
        if (d.name == s) {
            e = d.e;
            validFlag = true;
            break;
        }
    }
    
    if (isValidOut != 0) {
        *isValidOut = validFlag;
    }
    return e;
}

/**
 * Get a GUI string representation of the enumerated type.
 * @param e 
 *     Enumerated value.
 * @return 
 *     String representing enumerated value.
 */
AString 
YokingTypeEnum::toGuiName(Enum e) {
    initialize();
    
    const YokingTypeEnum* enumValue = findData(e);
    return enumValue->guiName;
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
YokingTypeEnum::Enum 
YokingTypeEnum::fromGuiName(const AString& s, bool* isValidOut)
{
    initialize();
    
    bool validFlag = false;
    Enum e = OFF;
    
    for (std::vector<YokingTypeEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const YokingTypeEnum& d = *iter;
        if (d.guiName == s) {
            e = d.e;
            validFlag = true;
            break;
        }
    }
    
    if (isValidOut != 0) {
        *isValidOut = validFlag;
    }
    return e;
}

/**
 * Get the integer code for a data type.
 *
 * @return
 *    Integer code for data type.
 */
int32_t
YokingTypeEnum::toIntegerCode(Enum e)
{
    initialize();
    const YokingTypeEnum* enumValue = findData(e);
    return enumValue->integerCode;
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
YokingTypeEnum::Enum
YokingTypeEnum::fromIntegerCode(const int32_t integerCode, bool* isValidOut)
{
    initialize();
    
    bool validFlag = false;
    Enum e = YokingTypeEnum::OFF;
    
    for (std::vector<YokingTypeEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const YokingTypeEnum& enumValue = *iter;
        if (enumValue.integerCode == integerCode) {
            e = enumValue.e;
            validFlag = true;
            break;
        }
    }
    
    if (isValidOut != 0) {
        *isValidOut = validFlag;
    }
    return e;
}

/**
 * Get all of the enumerated type values.  The values can be used
 * as parameters to toXXX() methods to get associated metadata.
 *
 * @param allEnums
 *     A vector that is OUTPUT containing all of the enumerated values.
 */
void
YokingTypeEnum::getAllEnums(std::vector<YokingTypeEnum::Enum>& allEnums)
{
    if (initializedFlag == false) initialize();
    
    allEnums.clear();
    
    for (std::vector<YokingTypeEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        allEnums.push_back(iter->e);
    }
}


