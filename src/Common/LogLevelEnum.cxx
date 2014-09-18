
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

#define __LOG_LEVEL_ENUM_DECLARE__
#include "LogLevelEnum.h"
#undef __LOG_LEVEL_ENUM_DECLARE__

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
LogLevelEnum::LogLevelEnum(const Enum enumValue,
                           const int32_t integerCode,
                           const AString& name,
                           const AString& guiName,
                           const AString& hintedName)
{
    this->enumValue = enumValue;
    this->integerCode = integerCode;
    this->name = name;
    this->guiName = guiName;
    this->hintedName = hintedName;
}

/**
 * Destructor.
 */
LogLevelEnum::~LogLevelEnum()
{
}

/**
 * Initialize the enumerated metadata.
 */
void
LogLevelEnum::initialize()
{
    if (initializedFlag) {
        return;
    }
    initializedFlag = true;

    enumData.push_back(LogLevelEnum(SEVERE, 
                                    800, 
                                    "SEVERE", 
                                    "Severe",
                                    "SEVERE"));
    
    enumData.push_back(LogLevelEnum(WARNING, 
                                    700, 
                                    "WARNING", 
                                    "Warning",
                                    "WARNING"));
    
    enumData.push_back(LogLevelEnum(INFO, 
                                    600, 
                                    "INFO", 
                                    "Information",
                                    "Info"));
    
    enumData.push_back(LogLevelEnum(CONFIG, 
                                    500, 
                                    "CONFIG", 
                                    "Configuration",
                                    "Config"));
    
    enumData.push_back(LogLevelEnum(FINE, 
                                    400, 
                                    "FINE", 
                                    "Fine (Tracing)",
                                    "Fine"));
    
    enumData.push_back(LogLevelEnum(FINER, 
                                    300, 
                                    "FINER", 
                                    "Finer (Detailed Tracing)",
                                    "Finer"));
    
    enumData.push_back(LogLevelEnum(FINEST, 
                                    200, 
                                    "FINEST", 
                                    "Finest (Very Detailed Tracing)",
                                    "Finest"));
    
    enumData.push_back(LogLevelEnum(ALL, 
                                    100, 
                                    "ALL", 
                                    "All",
                                    "ALL"));//shouldn't get used in messages - do we even need this?  FINEST should show everything
    
    enumData.push_back(LogLevelEnum(OFF, 
                                    0, 
                                    "OFF", 
                                    "Off",
                                    "Off"));//also shouldn't get used in messages
    
}

/**
 * Find the data for and enumerated value.
 * @param enumValue
 *     The enumerated value.
 * @return Pointer to data for this enumerated type
 * or NULL if no data for type or if type is invalid.
 */
const LogLevelEnum*
LogLevelEnum::findData(const Enum enumValue)
{
    if (initializedFlag == false) initialize();

    size_t num = enumData.size();
    for (size_t i = 0; i < num; i++) {
        const LogLevelEnum* d = &enumData[i];
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
LogLevelEnum::toName(Enum enumValue) {
    if (initializedFlag == false) initialize();
    
    const LogLevelEnum* enumInstance = findData(enumValue);
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
LogLevelEnum::Enum 
LogLevelEnum::fromName(const AString& name, bool* isValidOut)
{
    if (initializedFlag == false) initialize();
    
    bool validFlag = false;
    Enum enumValue = OFF;
    
    for (std::vector<LogLevelEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const LogLevelEnum& d = *iter;
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
        CaretAssertMessage(0, AString("Name " + name + " failed to match enumerated value for type LogLevelEnum"));
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
LogLevelEnum::toGuiName(Enum enumValue) {
    if (initializedFlag == false) initialize();
    
    const LogLevelEnum* enumInstance = findData(enumValue);
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
LogLevelEnum::Enum 
LogLevelEnum::fromGuiName(const AString& guiName, bool* isValidOut)
{
    if (initializedFlag == false) initialize();
    
    bool validFlag = false;
    Enum enumValue = OFF;
    
    for (std::vector<LogLevelEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const LogLevelEnum& d = *iter;
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
        CaretAssertMessage(0, AString("guiName " + guiName + " failed to match enumerated value for type LogLevelEnum"));
    }
    return enumValue;
}

AString LogLevelEnum::toHintedName(LogLevelEnum::Enum enumValue)
{
    if (initializedFlag == false) initialize();
    const LogLevelEnum* enumInstance = findData(enumValue);
    CaretAssert(enumInstance != NULL);
    return enumInstance->hintedName;
}

LogLevelEnum::Enum LogLevelEnum::fromHintedName(const AString& hintedName, bool* isValidOut)
{
    if (initializedFlag == false) initialize();
    bool validFlag = false;
    Enum enumValue = OFF;
    for (std::vector<LogLevelEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const LogLevelEnum& d = *iter;
        if (d.hintedName == hintedName) {
            enumValue = d.enumValue;
            validFlag = true;
            break;
        }
    }
    if (isValidOut != 0) {
        *isValidOut = validFlag;
    }
    else if (validFlag == false) {
        CaretAssertMessage(0, AString("hintedName " + hintedName + " failed to match enumerated value for type LogLevelEnum"));
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
LogLevelEnum::toIntegerCode(Enum enumValue)
{
    if (initializedFlag == false) initialize();
    const LogLevelEnum* enumInstance = findData(enumValue);
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
LogLevelEnum::Enum
LogLevelEnum::fromIntegerCode(const int32_t integerCode, bool* isValidOut)
{
    if (initializedFlag == false) initialize();
    
    bool validFlag = false;
    Enum enumValue = OFF;
    
    for (std::vector<LogLevelEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const LogLevelEnum& enumInstance = *iter;
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
        CaretAssertMessage(0, AString("Integer code " + AString::number(integerCode) + "failed to match enumerated value for type LogLevelEnum"));
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
LogLevelEnum::getAllEnums(std::vector<LogLevelEnum::Enum>& allEnums)
{
    if (initializedFlag == false) initialize();
    
    allEnums.clear();
    
    for (std::vector<LogLevelEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        allEnums.push_back(iter->enumValue);
    }
}

