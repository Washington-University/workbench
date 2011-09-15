
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
                                    700, 
                                    "SEVERE", 
                                    "Severe"));
    
    enumData.push_back(LogLevelEnum(WARNING, 
                                    600, 
                                    "WARNING", 
                                    "Warning"));
    
    enumData.push_back(LogLevelEnum(INFO, 
                                    500, 
                                    "INFO", 
                                    "Information"));
    
    enumData.push_back(LogLevelEnum(FINE, 
                                    400, 
                                    "FINE", 
                                    "Fine (Tracing)"));
    
    enumData.push_back(LogLevelEnum(FINER, 
                                    300, 
                                    "FINER", 
                                    "Fine (Detailed Tracing)"));
    
    enumData.push_back(LogLevelEnum(FINEST, 
                                    200, 
                                    "FINEST", 
                                    "Fine (Very Detailed Tracing)"));
    
    enumData.push_back(LogLevelEnum(ALL, 
                                    100, 
                                    "ALL", 
                                    "All"));
    
    enumData.push_back(LogLevelEnum(OFF, 
                                    0, 
                                    "OFF", 
                                    "Off"));
    
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
    else {
        CaretAssertMessage(0, AString("Name " + name + "failed to match enumerated value for type LogLevelEnum"));
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
    else {
        CaretAssertMessage(0, AString("guiName " + guiName + "failed to match enumerated value for type LogLevelEnum"));
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
    else {
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

