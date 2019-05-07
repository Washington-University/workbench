
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
#include "OperationParametersEnum.h"

#include "CaretAssert.h"

using namespace caret;

std::vector<OperationParametersEnum> OperationParametersEnum::enumData;
bool OperationParametersEnum::initializedFlag = false;
    
/**
 * \class AlgorithmParametersEnum 
 * \brief enum for parameter types
 *
 * enum for parameter types
 */
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
OperationParametersEnum::OperationParametersEnum(const Enum enumValue,
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
OperationParametersEnum::~OperationParametersEnum()
{
}

/**
 * Initialize the enumerated metadata.
 */
void
OperationParametersEnum::initialize()
{
    if (initializedFlag) {
        return;
    }
    initializedFlag = true;

    enumData.push_back(OperationParametersEnum(SURFACE, 
                                    0, 
                                    "Surface File", 
                                    "Surface"));
    
    enumData.push_back(OperationParametersEnum(VOLUME, 
                                    1, 
                                    "Volume File", 
                                    "Volume"));
    
    enumData.push_back(OperationParametersEnum(METRIC, 
                                    2, 
                                    "Metric File", 
                                    "Metric"));
    
    enumData.push_back(OperationParametersEnum(LABEL, 
                                    3, 
                                    "Label File", 
                                    "Label"));
    
    enumData.push_back(OperationParametersEnum(CIFTI, 
                                    4, 
                                    "Cifti File", 
                                    "Cifti"));
    
    enumData.push_back(OperationParametersEnum(FOCI, 
                                    5, 
                                    "Foci File", 
                                    "Foci File"));
    
    enumData.push_back(OperationParametersEnum(BORDER, 
                                    6, 
                                    "Border File", 
                                    "Border File"));
    
    enumData.push_back(OperationParametersEnum(DOUBLE, 
                                    7, 
                                    "Floating Point", 
                                    "Floating Point"));
    
    enumData.push_back(OperationParametersEnum(INT, 
                                    8, 
                                    "Integer", 
                                    "Integer"));
    
    enumData.push_back(OperationParametersEnum(STRING, 
                                    9, 
                                    "String", 
                                    "String"));
    
    enumData.push_back(OperationParametersEnum(BOOL, 
                                    10, 
                                    "Boolean", 
                                    "Boolean"));
    
    enumData.push_back(OperationParametersEnum(ANNOTATION,
                                               11,
                                               "Annotation File",
                                               "Annotation"));
    
}

/**
 * Find the data for and enumerated value.
 * @param enumValue
 *     The enumerated value.
 * @return Pointer to data for this enumerated type
 * or NULL if no data for type or if type is invalid.
 */
const OperationParametersEnum*
OperationParametersEnum::findData(const Enum enumValue)
{
    if (initializedFlag == false) initialize();

    size_t num = enumData.size();
    for (size_t i = 0; i < num; i++) {
        const OperationParametersEnum* d = &enumData[i];
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
OperationParametersEnum::toName(Enum enumValue) {
    if (initializedFlag == false) initialize();
    
    const OperationParametersEnum* enumInstance = findData(enumValue);
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
OperationParametersEnum::Enum 
OperationParametersEnum::fromName(const AString& name, bool* isValidOut)
{
    if (initializedFlag == false) initialize();
    
    bool validFlag = false;
    Enum enumValue = SURFACE;
    
    for (std::vector<OperationParametersEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const OperationParametersEnum& d = *iter;
        if (d.name == name) {
            enumValue = d.enumValue;
            validFlag = true;
            break;
        }
    }
    
    if (isValidOut != 0) {
        *isValidOut = validFlag;
    }
    else {
        CaretAssertMessage(0, AString("Name " + name + "failed to match enumerated value for type AlgorithmParametersEnum"));
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
OperationParametersEnum::toGuiName(Enum enumValue) {
    if (initializedFlag == false) initialize();
    
    const OperationParametersEnum* enumInstance = findData(enumValue);
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
OperationParametersEnum::Enum 
OperationParametersEnum::fromGuiName(const AString& guiName, bool* isValidOut)
{
    if (initializedFlag == false) initialize();
    
    bool validFlag = false;
    Enum enumValue = SURFACE;
    
    for (std::vector<OperationParametersEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const OperationParametersEnum& d = *iter;
        if (d.guiName == guiName) {
            enumValue = d.enumValue;
            validFlag = true;
            break;
        }
    }
    
    if (isValidOut != 0) {
        *isValidOut = validFlag;
    }
    else {
        CaretAssertMessage(0, AString("guiName " + guiName + "failed to match enumerated value for type AlgorithmParametersEnum"));
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
OperationParametersEnum::toIntegerCode(Enum enumValue)
{
    if (initializedFlag == false) initialize();
    const OperationParametersEnum* enumInstance = findData(enumValue);
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
OperationParametersEnum::Enum
OperationParametersEnum::fromIntegerCode(const int32_t integerCode, bool* isValidOut)
{
    if (initializedFlag == false) initialize();
    
    bool validFlag = false;
    Enum enumValue = SURFACE;
    
    for (std::vector<OperationParametersEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const OperationParametersEnum& enumInstance = *iter;
        if (enumInstance.integerCode == integerCode) {
            enumValue = enumInstance.enumValue;
            validFlag = true;
            break;
        }
    }
    
    if (isValidOut != 0) {
        *isValidOut = validFlag;
    }
    else {
        CaretAssertMessage(0, AString("Integer code " + AString::number(integerCode) + "failed to match enumerated value for type AlgorithmParametersEnum"));
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
OperationParametersEnum::getAllEnums(std::vector<OperationParametersEnum::Enum>& allEnums)
{
    if (initializedFlag == false) initialize();
    
    allEnums.clear();
    
    for (std::vector<OperationParametersEnum>::iterator iter = enumData.begin();
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
OperationParametersEnum::getAllNames(std::vector<AString>& allNames, const bool isSorted)
{
    if (initializedFlag == false) initialize();
    
    allNames.clear();
    
    for (std::vector<OperationParametersEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        allNames.push_back(OperationParametersEnum::toName(iter->enumValue));
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
OperationParametersEnum::getAllGuiNames(std::vector<AString>& allGuiNames, const bool isSorted)
{
    if (initializedFlag == false) initialize();
    
    allGuiNames.clear();
    
    for (std::vector<OperationParametersEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        allGuiNames.push_back(OperationParametersEnum::toGuiName(iter->enumValue));
    }
    
    if (isSorted) {
        std::sort(allGuiNames.begin(), allGuiNames.end());
    }
}

