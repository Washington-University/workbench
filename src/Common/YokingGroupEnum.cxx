
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
#define __YOKING_GROUP_ENUM_DECLARE__
#include "YokingGroupEnum.h"
#undef __YOKING_GROUP_ENUM_DECLARE__

#include "CaretAssert.h"

using namespace caret;

    
/**
 * \class caret::YokingGroupEnum 
 * \brief Enumerated type for yoking views of model.
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
YokingGroupEnum::YokingGroupEnum(const Enum enumValue,
                           const AString& name,
                           const AString& guiName)
{
    this->enumValue = enumValue;
    this->integerCode = integerCodeCounter++;
    this->name = name;
    this->guiName = guiName;
}

/**
 * Destructor.
 */
YokingGroupEnum::~YokingGroupEnum()
{
}

/**
 * Initialize the enumerated metadata.
 */
void
YokingGroupEnum::initialize()
{
    if (initializedFlag) {
        return;
    }
    initializedFlag = true;

    enumData.push_back(YokingGroupEnum(YOKING_GROUP_OFF, 
                                    "YOKING_GROUP_OFF", 
                                    "Off"));
    
    enumData.push_back(YokingGroupEnum(YOKING_GROUP_A, 
                                    "YOKING_GROUP_A", 
                                    "Group A"));
    
    enumData.push_back(YokingGroupEnum(YOKING_GROUP_B,
                                       "YOKING_GROUP_B",
                                       "Group B"));
    
    enumData.push_back(YokingGroupEnum(YOKING_GROUP_C,
                                       "YOKING_GROUP_C",
                                       "Group C"));
    
    enumData.push_back(YokingGroupEnum(YOKING_GROUP_D,
                                       "YOKING_GROUP_D",
                                       "Group D"));
    
    enumData.push_back(YokingGroupEnum(YOKING_GROUP_E,
                                       "YOKING_GROUP_E",
                                       "Group E"));
    
    enumData.push_back(YokingGroupEnum(YOKING_GROUP_F,
                                       "YOKING_GROUP_F",
                                       "Group F"));
    
    enumData.push_back(YokingGroupEnum(YOKING_GROUP_G,
                                       "YOKING_GROUP_G",
                                       "Group G"));
    
    enumData.push_back(YokingGroupEnum(YOKING_GROUP_H,
                                       "YOKING_GROUP_H",
                                       "Group H"));
    
    enumData.push_back(YokingGroupEnum(YOKING_GROUP_I,
                                       "YOKING_GROUP_I",
                                       "Group I"));
    
    enumData.push_back(YokingGroupEnum(YOKING_GROUP_J,
                                       "YOKING_GROUP_J",
                                       "Group J"));
    
    enumData.push_back(YokingGroupEnum(YOKING_GROUP_K,
                                       "YOKING_GROUP_K",
                                       "Group K"));
    
    enumData.push_back(YokingGroupEnum(YOKING_GROUP_L,
                                       "YOKING_GROUP_L",
                                       "Group L"));
    
    enumData.push_back(YokingGroupEnum(YOKING_GROUP_M,
                                       "YOKING_GROUP_M",
                                       "Group M"));
    
    enumData.push_back(YokingGroupEnum(YOKING_GROUP_N,
                                       "YOKING_GROUP_N",
                                       "Group N"));
    
    enumData.push_back(YokingGroupEnum(YOKING_GROUP_O,
                                       "YOKING_GROUP_O",
                                       "Group O"));
    
    enumData.push_back(YokingGroupEnum(YOKING_GROUP_P,
                                       "YOKING_GROUP_P",
                                       "Group P"));
    
    enumData.push_back(YokingGroupEnum(YOKING_GROUP_Q,
                                       "YOKING_GROUP_Q",
                                       "Group Q"));
    
    enumData.push_back(YokingGroupEnum(YOKING_GROUP_R,
                                       "YOKING_GROUP_R",
                                       "Group R"));
    
    enumData.push_back(YokingGroupEnum(YOKING_GROUP_S,
                                       "YOKING_GROUP_S",
                                       "Group S"));
    
    enumData.push_back(YokingGroupEnum(YOKING_GROUP_T,
                                       "YOKING_GROUP_T",
                                       "Group T"));
    
    enumData.push_back(YokingGroupEnum(YOKING_GROUP_U,
                                       "YOKING_GROUP_U",
                                       "Group U"));
    
    enumData.push_back(YokingGroupEnum(YOKING_GROUP_V,
                                       "YOKING_GROUP_V",
                                       "Group V"));
    
    enumData.push_back(YokingGroupEnum(YOKING_GROUP_W,
                                       "YOKING_GROUP_W",
                                       "Group W"));
    
    enumData.push_back(YokingGroupEnum(YOKING_GROUP_X,
                                       "YOKING_GROUP_X",
                                       "Group X"));
    
    enumData.push_back(YokingGroupEnum(YOKING_GROUP_Y,
                                       "YOKING_GROUP_Y",
                                       "Group Y"));
    
    enumData.push_back(YokingGroupEnum(YOKING_GROUP_Z,
                                       "YOKING_GROUP_Z",
                                       "Group Z"));
    
    CaretAssert(enumData.size() == 27);  /* OFF + A to Z */
}

/**
 * Find the data for and enumerated value.
 * @param enumValue
 *     The enumerated value.
 * @return Pointer to data for this enumerated type
 * or NULL if no data for type or if type is invalid.
 */
const YokingGroupEnum*
YokingGroupEnum::findData(const Enum enumValue)
{
    if (initializedFlag == false) initialize();

    size_t num = enumData.size();
    for (size_t i = 0; i < num; i++) {
        const YokingGroupEnum* d = &enumData[i];
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
YokingGroupEnum::toName(Enum enumValue) {
    if (initializedFlag == false) initialize();
    
    const YokingGroupEnum* enumInstance = findData(enumValue);
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
YokingGroupEnum::Enum 
YokingGroupEnum::fromName(const AString& name, bool* isValidOut)
{
    if (initializedFlag == false) initialize();
    
    bool validFlag = false;
    Enum enumValue = YOKING_GROUP_OFF;
    
    for (std::vector<YokingGroupEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const YokingGroupEnum& d = *iter;
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
        CaretAssertMessage(0, AString("Name " + name + "failed to match enumerated value for type YokingGroupEnum"));
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
YokingGroupEnum::toGuiName(Enum enumValue) {
    if (initializedFlag == false) initialize();
    
    const YokingGroupEnum* enumInstance = findData(enumValue);
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
YokingGroupEnum::Enum 
YokingGroupEnum::fromGuiName(const AString& guiName, bool* isValidOut)
{
    if (initializedFlag == false) initialize();
    
    bool validFlag = false;
    Enum enumValue = YOKING_GROUP_OFF;
    
    for (std::vector<YokingGroupEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const YokingGroupEnum& d = *iter;
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
        CaretAssertMessage(0, AString("guiName " + guiName + "failed to match enumerated value for type YokingGroupEnum"));
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
YokingGroupEnum::toIntegerCode(Enum enumValue)
{
    if (initializedFlag == false) initialize();
    const YokingGroupEnum* enumInstance = findData(enumValue);
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
YokingGroupEnum::Enum
YokingGroupEnum::fromIntegerCode(const int32_t integerCode, bool* isValidOut)
{
    if (initializedFlag == false) initialize();
    
    bool validFlag = false;
    Enum enumValue = YOKING_GROUP_OFF;
    
    for (std::vector<YokingGroupEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const YokingGroupEnum& enumInstance = *iter;
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
        CaretAssertMessage(0, AString("Integer code " + AString::number(integerCode) + "failed to match enumerated value for type YokingGroupEnum"));
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
YokingGroupEnum::getAllEnums(std::vector<YokingGroupEnum::Enum>& allEnums)
{
    if (initializedFlag == false) initialize();
    
    allEnums.clear();
    
    for (std::vector<YokingGroupEnum>::iterator iter = enumData.begin();
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
YokingGroupEnum::getAllNames(std::vector<AString>& allNames, const bool isSorted)
{
    if (initializedFlag == false) initialize();
    
    allNames.clear();
    
    for (std::vector<YokingGroupEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        allNames.push_back(YokingGroupEnum::toName(iter->enumValue));
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
YokingGroupEnum::getAllGuiNames(std::vector<AString>& allGuiNames, const bool isSorted)
{
    if (initializedFlag == false) initialize();
    
    allGuiNames.clear();
    
    for (std::vector<YokingGroupEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        allGuiNames.push_back(YokingGroupEnum::toGuiName(iter->enumValue));
    }
    
    if (isSorted) {
        std::sort(allGuiNames.begin(), allGuiNames.end());
    }
}

