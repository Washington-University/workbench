
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
#define __SPECIES_ENUM_DECLARE__
#include "SpeciesEnum.h"
#undef __SPECIES_ENUM_DECLARE__

#include "CaretAssert.h"

using namespace caret;

/**
 * Constructor.
 *
 * @param enumValue
 *    An enumerated value.
 * @param integerCode
 *    Integer code for this enumerated value.
 *
 * @param name
 *    Name of enumerated value.
 *
 * @param guiName
 *    User-friendly name for use in user-interface.
 */
SpeciesEnum::SpeciesEnum(const Enum enumValue,
                           const int32_t integerCode,
                           const AString& name,
                           const AString& guiName)
{
    this->enumValue = enumValue;
    this->integerCode = integerCode;
    this->name = name;
    this->guiName = guiName;
}

/**
 * Destructor.
 */
SpeciesEnum::~SpeciesEnum()
{
}

/**
 * Initialize the enumerated metadata.
 */
void
SpeciesEnum::initialize()
{
    if (initializedFlag) {
        return;
    }
    initializedFlag = true;

    enumData.push_back(SpeciesEnum(TYPE_UNKNOWN, 
                                    0, 
                                    "TYPE_UNKNOWN", 
                                    "Unknown"));
    
    enumData.push_back(SpeciesEnum(TYPE_BABOON, 
                                    1, 
                                    "TYPE_BABOON", 
                                    "Baboon"));
    
    enumData.push_back(SpeciesEnum(TYPE_CHIMPANZEE, 
                                    2, 
                                    "TYPE_CHIMPANZEE", 
                                    "Chimpanzee"));
    
    enumData.push_back(SpeciesEnum(TYPE_FERRET, 
                                    3, 
                                    "TYPE_FERRET", 
                                    "Ferret"));
    
    enumData.push_back(SpeciesEnum(TYPE_GALAGO, 
                                    4, 
                                    "TYPE_GALAGO", 
                                    "Galago"));
    
    enumData.push_back(SpeciesEnum(TYPE_GIBBON, 
                                    5, 
                                    "TYPE_GIBBON", 
                                    "Gibbon"));
    
    enumData.push_back(SpeciesEnum(TYPE_GORILLA, 
                                    6, 
                                    "TYPE_GORILLA", 
                                    "Gorilla"));
    
    enumData.push_back(SpeciesEnum(TYPE_HUMAN, 
                                    7, 
                                    "TYPE_HUMAN", 
                                    "Human"));
    
    enumData.push_back(SpeciesEnum(TYPE_MACAQUE, 
                                    8, 
                                    "TYPE_MACAQUE", 
                                    "Macaque"));
    
    enumData.push_back(SpeciesEnum(TYPE_MOUSE, 
                                    9, 
                                    "TYPE_MOUSE", 
                                    "Mouse"));
    
    enumData.push_back(SpeciesEnum(TYPE_ORANGUTAN, 
                                    10, 
                                    "TYPE_ORANGUTAN", 
                                    "Orangutan"));
    
    enumData.push_back(SpeciesEnum(TYPE_RAT, 
                                    11, 
                                    "TYPE_RAT", 
                                    "Rat"));
    
    enumData.push_back(SpeciesEnum(TYPE_OTHER, 
                                    12, 
                                    "TYPE_OTHER", 
                                    "Other not specified"));
    
}

/**
 * Find the data for and enumerated value.
 * @param enumValue
 *     The enumerated value.
 * @return Pointer to data for this enumerated type
 * or NULL if no data for type or if type is invalid.
 */
const SpeciesEnum*
SpeciesEnum::findData(const Enum enumValue)
{
    if (initializedFlag == false) initialize();

    size_t num = enumData.size();
    for (size_t i = 0; i < num; i++) {
        const SpeciesEnum* d = &enumData[i];
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
SpeciesEnum::toName(Enum enumValue) {
    if (initializedFlag == false) initialize();
    
    const SpeciesEnum* enumInstance = findData(enumValue);
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
SpeciesEnum::Enum 
SpeciesEnum::fromName(const AString& name, bool* isValidOut)
{
    if (initializedFlag == false) initialize();
    
    bool validFlag = false;
    Enum enumValue = TYPE_UNKNOWN;
    
    for (std::vector<SpeciesEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const SpeciesEnum& d = *iter;
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
        CaretAssertMessage(0, AString("Name " + name + "failed to match enumerated value for type SpeciesEnum"));
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
SpeciesEnum::toGuiName(Enum enumValue) {
    if (initializedFlag == false) initialize();
    
    const SpeciesEnum* enumInstance = findData(enumValue);
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
SpeciesEnum::Enum 
SpeciesEnum::fromGuiName(const AString& guiName, bool* isValidOut)
{
    if (initializedFlag == false) initialize();
    
    bool validFlag = false;
    Enum enumValue = TYPE_UNKNOWN;
    
    for (std::vector<SpeciesEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const SpeciesEnum& d = *iter;
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
        CaretAssertMessage(0, AString("guiName " + guiName + "failed to match enumerated value for type SpeciesEnum"));
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
SpeciesEnum::toIntegerCode(Enum enumValue)
{
    if (initializedFlag == false) initialize();
    const SpeciesEnum* enumInstance = findData(enumValue);
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
SpeciesEnum::Enum
SpeciesEnum::fromIntegerCode(const int32_t integerCode, bool* isValidOut)
{
    if (initializedFlag == false) initialize();
    
    bool validFlag = false;
    Enum enumValue = TYPE_UNKNOWN;
    
    for (std::vector<SpeciesEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const SpeciesEnum& enumInstance = *iter;
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
        CaretAssertMessage(0, AString("Integer code " + AString::number(integerCode) + "failed to match enumerated value for type SpeciesEnum"));
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
SpeciesEnum::getAllEnums(std::vector<SpeciesEnum::Enum>& allEnums)
{
    if (initializedFlag == false) initialize();
    
    allEnums.clear();
    
    for (std::vector<SpeciesEnum>::iterator iter = enumData.begin();
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
SpeciesEnum::getAllNames(std::vector<AString>& allNames, const bool isSorted)
{
    if (initializedFlag == false) initialize();
    
    allNames.clear();
    
    for (std::vector<SpeciesEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        allNames.push_back(SpeciesEnum::toName(iter->enumValue));
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
SpeciesEnum::getAllGuiNames(std::vector<AString>& allGuiNames, const bool isSorted)
{
    if (initializedFlag == false) initialize();
    
    allGuiNames.clear();
    
    for (std::vector<SpeciesEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        allGuiNames.push_back(SpeciesEnum::toGuiName(iter->enumValue));
    }
    
    if (isSorted) {
        std::sort(allGuiNames.begin(), allGuiNames.end());
    }
}

