
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
#define __SCENE_OBJECT_DATA_TYPE_ENUM_DECLARE__
#include "SceneObjectDataTypeEnum.h"
#undef __SCENE_OBJECT_DATA_TYPE_ENUM_DECLARE__

#include "CaretAssert.h"

using namespace caret;

    
/**
 * \class caret::SceneObjectDataTypeEnum 
 * \brief Types of data objects saved in scenes.
 * \ingroup Scene
 *
 * See the documentation in the class Scene for how to use the Scene system.
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
 * @param xmlName
 *    Name used by XML.
 */
SceneObjectDataTypeEnum::SceneObjectDataTypeEnum(const Enum enumValue,
                           const AString& name,
                           const AString& guiName,
                                                 const AString& xmlName)
{
    this->enumValue = enumValue;
    this->integerCode = integerCodeCounter++;
    this->name = name;
    this->guiName = guiName;
    this->xmlName = xmlName;
}

/**
 * Destructor.
 */
SceneObjectDataTypeEnum::~SceneObjectDataTypeEnum()
{
}

/**
 * Initialize the enumerated metadata.
 */
void
SceneObjectDataTypeEnum::initialize()
{
    if (initializedFlag) {
        return;
    }
    initializedFlag = true;

    enumData.push_back(SceneObjectDataTypeEnum(SCENE_INVALID, 
                                               "SCENE_INVALID", 
                                               "invalid",
                                               "invalid"));
    
    enumData.push_back(SceneObjectDataTypeEnum(SCENE_BOOLEAN, 
                                               "SCENE_BOOLEAN", 
                                               "boolean",
                                               "boolean"));
    
    enumData.push_back(SceneObjectDataTypeEnum(SCENE_CLASS, 
                                               "SCENE_CLASS", 
                                               "class",
                                               "class"));
    
    enumData.push_back(SceneObjectDataTypeEnum(SCENE_ENUMERATED_TYPE, 
                                               "SCENE_ENUMERATED_TYPE", 
                                               "enumeratedType",
                                               "enumeratedType"));
    
    enumData.push_back(SceneObjectDataTypeEnum(SCENE_FLOAT, 
                                               "SCENE_FLOAT", 
                                               "float",
                                               "float"));
    
    enumData.push_back(SceneObjectDataTypeEnum(SCENE_INTEGER, 
                                               "SCENE_INTEGER", 
                                               "integer",
                                               "integer"));
    
    enumData.push_back(SceneObjectDataTypeEnum(SCENE_PATH_NAME, 
                                               "SCENE_PATH_NAME", 
                                               "pathName",
                                               "pathName"));
    
    enumData.push_back(SceneObjectDataTypeEnum(SCENE_STRING, 
                                               "SCENE_STRING", 
                                               "string",
                                               "string"));
    
    enumData.push_back(SceneObjectDataTypeEnum(SCENE_UNSIGNED_BYTE,
                                               "SCENE_UNSIGNED_BYTE",
                                               "unsignedByte",
                                               "unsignedByte"));
    
}

/**
 * Find the data for and enumerated value.
 * @param enumValue
 *     The enumerated value.
 * @return Pointer to data for this enumerated type
 * or NULL if no data for type or if type is invalid.
 */
const SceneObjectDataTypeEnum*
SceneObjectDataTypeEnum::findData(const Enum enumValue)
{
    if (initializedFlag == false) initialize();

    size_t num = enumData.size();
    for (size_t i = 0; i < num; i++) {
        const SceneObjectDataTypeEnum* d = &enumData[i];
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
SceneObjectDataTypeEnum::toName(Enum enumValue) {
    if (initializedFlag == false) initialize();
    
    const SceneObjectDataTypeEnum* enumInstance = findData(enumValue);
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
SceneObjectDataTypeEnum::Enum 
SceneObjectDataTypeEnum::fromName(const AString& name, bool* isValidOut)
{
    if (initializedFlag == false) initialize();
    
    bool validFlag = false;
    Enum enumValue = SCENE_INVALID;
    
    for (std::vector<SceneObjectDataTypeEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const SceneObjectDataTypeEnum& d = *iter;
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
        CaretAssertMessage(0, AString("Name " + name + "failed to match enumerated value for type SceneObjectDataTypeEnum"));
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
SceneObjectDataTypeEnum::toGuiName(Enum enumValue) {
    if (initializedFlag == false) initialize();
    
    const SceneObjectDataTypeEnum* enumInstance = findData(enumValue);
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
SceneObjectDataTypeEnum::Enum 
SceneObjectDataTypeEnum::fromGuiName(const AString& guiName, bool* isValidOut)
{
    if (initializedFlag == false) initialize();
    
    bool validFlag = false;
    Enum enumValue = SCENE_INVALID;
    
    for (std::vector<SceneObjectDataTypeEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const SceneObjectDataTypeEnum& d = *iter;
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
        CaretAssertMessage(0, AString("guiName " + guiName + "failed to match enumerated value for type SceneObjectDataTypeEnum"));
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
SceneObjectDataTypeEnum::toIntegerCode(Enum enumValue)
{
    if (initializedFlag == false) initialize();
    const SceneObjectDataTypeEnum* enumInstance = findData(enumValue);
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
SceneObjectDataTypeEnum::Enum
SceneObjectDataTypeEnum::fromIntegerCode(const int32_t integerCode, bool* isValidOut)
{
    if (initializedFlag == false) initialize();
    
    bool validFlag = false;
    Enum enumValue = SCENE_INVALID;
    
    for (std::vector<SceneObjectDataTypeEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const SceneObjectDataTypeEnum& enumInstance = *iter;
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
        CaretAssertMessage(0, AString("Integer code " + AString::number(integerCode) + "failed to match enumerated value for type SceneObjectDataTypeEnum"));
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
SceneObjectDataTypeEnum::getAllEnums(std::vector<SceneObjectDataTypeEnum::Enum>& allEnums)
{
    if (initializedFlag == false) initialize();
    
    allEnums.clear();
    
    for (std::vector<SceneObjectDataTypeEnum>::iterator iter = enumData.begin();
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
SceneObjectDataTypeEnum::getAllNames(std::vector<AString>& allNames, const bool isSorted)
{
    if (initializedFlag == false) initialize();
    
    allNames.clear();
    
    for (std::vector<SceneObjectDataTypeEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        allNames.push_back(SceneObjectDataTypeEnum::toName(iter->enumValue));
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
SceneObjectDataTypeEnum::getAllGuiNames(std::vector<AString>& allGuiNames, const bool isSorted)
{
    if (initializedFlag == false) initialize();
    
    allGuiNames.clear();
    
    for (std::vector<SceneObjectDataTypeEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        allGuiNames.push_back(SceneObjectDataTypeEnum::toGuiName(iter->enumValue));
    }
    
    if (isSorted) {
        std::sort(allGuiNames.begin(), allGuiNames.end());
    }
}

/**
 * Get all of the XML names of the enumerated type values.
 *
 * @param allNames
 *     A vector that is OUTPUT containing all of the XML names of the enumerated values.
 * @param isSorted
 *     If true, the names are sorted in alphabetical order.
 */
void
SceneObjectDataTypeEnum::getAllXmlNames(std::vector<AString>& allXmlNames, const bool isSorted)
{
    if (initializedFlag == false) initialize();
    
    allXmlNames.clear();
    
    for (std::vector<SceneObjectDataTypeEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        allXmlNames.push_back(SceneObjectDataTypeEnum::toXmlName(iter->enumValue));
    }
    
    if (isSorted) {
        std::sort(allXmlNames.begin(), allXmlNames.end());
    }
}

/**
 * Get an XML string representation of the enumerated type.
 * @param enumValue 
 *     Enumerated value.
 * @return 
 *     String representing enumerated value.
 */
AString 
SceneObjectDataTypeEnum::toXmlName(Enum enumValue) {
    if (initializedFlag == false) initialize();
    
    const SceneObjectDataTypeEnum* enumInstance = findData(enumValue);
    return enumInstance->xmlName;
}

/**
 * Get an enumerated value corresponding to its XML name.
 * @param xmlName 
 *     XML name of enumerated value.
 * @param isValidOut 
 *     If not NULL, it is set indicating that a
 *     enum value exists for the input name.
 * @return 
 *     Enumerated value.
 */
SceneObjectDataTypeEnum::Enum 
SceneObjectDataTypeEnum::fromXmlName(const AString& xmlName, bool* isValidOut)
{
    if (initializedFlag == false) initialize();
    
    bool validFlag = false;
    Enum enumValue = SCENE_INVALID;
    
    for (std::vector<SceneObjectDataTypeEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const SceneObjectDataTypeEnum& d = *iter;
        if (d.xmlName == xmlName) {
            enumValue = d.enumValue;
            validFlag = true;
            break;
        }
    }
    
    if (isValidOut != 0) {
        *isValidOut = validFlag;
    }
    else if (validFlag == false) {
        CaretAssertMessage(0, AString("Name " + xmlName + "failed to match enumerated value for type SceneObjectDataTypeEnum"));
    }
    return enumValue;
}


