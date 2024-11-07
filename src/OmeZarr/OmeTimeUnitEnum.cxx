
/*LICENSE_START*/
/*
 *  Copyright (C) 2024 Washington University School of Medicine
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
#define __OME_TIME_UNIT_ENUM_DECLARE__
#include "OmeTimeUnitEnum.h"
#undef __OME_TIME_UNIT_ENUM_DECLARE__

#include "CaretAssert.h"

using namespace caret;

    
/**
 * \class caret::OmeTimeUnitEnum 
 * \brief Units when type is time
 *
 * https://ngff.openmicroscopy.org/0.4/index.html#multiscale-md
 *
 * Using this enumerated type in the GUI with an EnumComboBoxTemplate
 * 
 * Header File (.h)
 *     Forward declare the data type:
 *         class EnumComboBoxTemplate;
 * 
 *     Declare the member:
 *         EnumComboBoxTemplate* m_ngffTimeUnitEnumComboBox;
 * 
 *     Declare a slot that is called when user changes selection
 *         private slots:
 *             void ngffTimeUnitEnumComboBoxItemActivated();
 * 
 * Implementation File (.cxx)
 *     Include the header files
 *         #include "EnumComboBoxTemplate.h"
 *         #include "OmeTimeUnitEnum.h"
 * 
 *     Instatiate:
 *         m_ngffTimeUnitEnumComboBox = new EnumComboBoxTemplate(this);
 *         m_ngffTimeUnitEnumComboBox->setup<OmeTimeUnitEnum,OmeTimeUnitEnum::Enum>();
 * 
 *     Get notified when the user changes the selection: 
 *         QObject::connect(m_ngffTimeUnitEnumComboBox, SIGNAL(itemActivated()),
 *                          this, SLOT(ngffTimeUnitEnumComboBoxItemActivated()));
 * 
 *     Update the selection:
 *         m_ngffTimeUnitEnumComboBox->setSelectedItem<OmeTimeUnitEnum,OmeTimeUnitEnum::Enum>(NEW_VALUE);
 * 
 *     Read the selection:
 *         const OmeTimeUnitEnum::Enum VARIABLE = m_ngffTimeUnitEnumComboBox->getSelectedItem<OmeTimeUnitEnum,OmeTimeUnitEnum::Enum>();
 * 
 */

/*
switch (value) {
    case OmeTimeUnitEnum::UNKNOWN:
        break;
    case OmeTimeUnitEnum::ATTOSECOND:
        break;
    case OmeTimeUnitEnum::CENTISECOND:
        break;
    case OmeTimeUnitEnum::DAY:
        break;
    case OmeTimeUnitEnum::DECISECOND:
        break;
    case OmeTimeUnitEnum::EXASECOND:
        break;
    case OmeTimeUnitEnum::FEMTOSECOND:
        break;
    case OmeTimeUnitEnum::GIGASECOND:
        break;
    case OmeTimeUnitEnum::HECTOSECOND:
        break;
    case OmeTimeUnitEnum::HOUR:
        break;
    case OmeTimeUnitEnum::KILOSECOND:
        break;
    case OmeTimeUnitEnum::MEGASECOND:
        break;
    case OmeTimeUnitEnum::MICROSECOND:
        break;
    case OmeTimeUnitEnum::MILLISECOND:
        break;
    case OmeTimeUnitEnum::MINUTE:
        break;
    case OmeTimeUnitEnum::NANOSECOND:
        break;
    case OmeTimeUnitEnum::PETASECOND:
        break;
    case OmeTimeUnitEnum::PICOSECOND:
        break;
    case OmeTimeUnitEnum::SECOND:
        break;
    case OmeTimeUnitEnum::TERASECOND:
        break;
    case OmeTimeUnitEnum::YOCTOSECOND:
        break;
    case OmeTimeUnitEnum::YOTTASECOND:
        break;
    case OmeTimeUnitEnum::ZEPTOSECOND:
        break;
    case OmeTimeUnitEnum::ZETTASECOND:
        break;
}
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
OmeTimeUnitEnum::OmeTimeUnitEnum(const Enum enumValue,
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
OmeTimeUnitEnum::~OmeTimeUnitEnum()
{
}

/**
 * Initialize the enumerated metadata.
 */
void
OmeTimeUnitEnum::initialize()
{
    if (initializedFlag) {
        return;
    }
    initializedFlag = true;

    enumData.push_back(OmeTimeUnitEnum(UNKNOWN, 
                                    "UNKNOWN", 
                                    "Unknown"));
    
    enumData.push_back(OmeTimeUnitEnum(ATTOSECOND, 
                                    "ATTOSECOND", 
                                    "Attosecond"));
    
    enumData.push_back(OmeTimeUnitEnum(CENTISECOND, 
                                    "CENTISECOND", 
                                    "Centisecond"));
    
    enumData.push_back(OmeTimeUnitEnum(DAY, 
                                    "DAY", 
                                    "Day"));
    
    enumData.push_back(OmeTimeUnitEnum(DECISECOND, 
                                    "DECISECOND", 
                                    "Decisecond"));
    
    enumData.push_back(OmeTimeUnitEnum(EXASECOND, 
                                    "EXASECOND", 
                                    "Exasecond"));
    
    enumData.push_back(OmeTimeUnitEnum(FEMTOSECOND, 
                                    "FEMTOSECOND", 
                                    "Femtosecond"));
    
    enumData.push_back(OmeTimeUnitEnum(GIGASECOND, 
                                    "GIGASECOND", 
                                    "Gigasecond"));
    
    enumData.push_back(OmeTimeUnitEnum(HECTOSECOND, 
                                    "HECTOSECOND", 
                                    "Hectosecond"));
    
    enumData.push_back(OmeTimeUnitEnum(HOUR, 
                                    "HOUR", 
                                    "Hour"));
    
    enumData.push_back(OmeTimeUnitEnum(KILOSECOND, 
                                    "KILOSECOND", 
                                    "Kilosecond"));
    
    enumData.push_back(OmeTimeUnitEnum(MEGASECOND, 
                                    "MEGASECOND", 
                                    "Megasecond"));
    
    enumData.push_back(OmeTimeUnitEnum(MICROSECOND, 
                                    "MICROSECOND", 
                                    "Microsecond"));
    
    enumData.push_back(OmeTimeUnitEnum(MILLISECOND, 
                                    "MILLISECOND", 
                                    "Millisecond"));
    
    enumData.push_back(OmeTimeUnitEnum(MINUTE, 
                                    "MINUTE", 
                                    "Minute"));
    
    enumData.push_back(OmeTimeUnitEnum(NANOSECOND, 
                                    "NANOSECOND", 
                                    "Nanosecond"));
    
    enumData.push_back(OmeTimeUnitEnum(PETASECOND, 
                                    "PETASECOND", 
                                    "Petasecond"));
    
    enumData.push_back(OmeTimeUnitEnum(PICOSECOND, 
                                    "PICOSECOND", 
                                    "Picosecond"));
    
    enumData.push_back(OmeTimeUnitEnum(SECOND, 
                                    "SECOND", 
                                    "Second"));
    
    enumData.push_back(OmeTimeUnitEnum(TERASECOND, 
                                    "TERASECOND", 
                                    "Terasecond"));
    
    enumData.push_back(OmeTimeUnitEnum(YOCTOSECOND, 
                                    "YOCTOSECOND", 
                                    "Yoctosecond"));
    
    enumData.push_back(OmeTimeUnitEnum(YOTTASECOND, 
                                    "YOTTASECOND", 
                                    "Yottasecond"));
    
    enumData.push_back(OmeTimeUnitEnum(ZEPTOSECOND, 
                                    "ZEPTOSECOND", 
                                    "Zeptosecond"));
    
    enumData.push_back(OmeTimeUnitEnum(ZETTASECOND, 
                                    "ZETTASECOND", 
                                    "Zettasecond"));
    
}

/**
 * Find the data for and enumerated value.
 * @param enumValue
 *     The enumerated value.
 * @return Pointer to data for this enumerated type
 * or NULL if no data for type or if type is invalid.
 */
const OmeTimeUnitEnum*
OmeTimeUnitEnum::findData(const Enum enumValue)
{
    if (initializedFlag == false) initialize();

    size_t num = enumData.size();
    for (size_t i = 0; i < num; i++) {
        const OmeTimeUnitEnum* d = &enumData[i];
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
OmeTimeUnitEnum::toName(Enum enumValue) {
    if (initializedFlag == false) initialize();
    
    const OmeTimeUnitEnum* enumInstance = findData(enumValue);
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
OmeTimeUnitEnum::Enum 
OmeTimeUnitEnum::fromName(const AString& name, bool* isValidOut)
{
    if (initializedFlag == false) initialize();
    
    bool validFlag = false;
    Enum enumValue = OmeTimeUnitEnum::enumData[0].enumValue;
    
    for (std::vector<OmeTimeUnitEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const OmeTimeUnitEnum& d = *iter;
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
        CaretAssertMessage(0, AString("Name " + name + " failed to match enumerated value for type OmeTimeUnitEnum"));
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
OmeTimeUnitEnum::toGuiName(Enum enumValue) {
    if (initializedFlag == false) initialize();
    
    const OmeTimeUnitEnum* enumInstance = findData(enumValue);
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
OmeTimeUnitEnum::Enum 
OmeTimeUnitEnum::fromGuiName(const AString& guiName, bool* isValidOut)
{
    if (initializedFlag == false) initialize();
    
    bool validFlag = false;
    Enum enumValue = OmeTimeUnitEnum::enumData[0].enumValue;
    
    for (std::vector<OmeTimeUnitEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const OmeTimeUnitEnum& d = *iter;
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
        CaretAssertMessage(0, AString("guiName " + guiName + " failed to match enumerated value for type OmeTimeUnitEnum"));
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
OmeTimeUnitEnum::toLowerCaseName(Enum enumValue) {
    if (initializedFlag == false) initialize();
    
    const OmeTimeUnitEnum* enumInstance = findData(enumValue);
    return enumInstance->guiName.toLower();
}

/**
 * Get an enumerated value corresponding to its GUI name.
 * @param lowerCaseName
 *     Name of enumerated value.
 * @param isValidOut
 *     If not NULL, it is set indicating that a
 *     enum value exists for the input name.
 * @return
 *     Enumerated value.
 */
OmeTimeUnitEnum::Enum
OmeTimeUnitEnum::fromLowerCaseName(const AString& lowerCaseName, bool* isValidOut)
{
    if (initializedFlag == false) initialize();
    
    bool validFlag = false;
    Enum enumValue = OmeTimeUnitEnum::enumData[0].enumValue;
    
    for (std::vector<OmeTimeUnitEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const OmeTimeUnitEnum& d = *iter;
        if (d.guiName.toLower() == lowerCaseName) {
            enumValue = d.enumValue;
            validFlag = true;
            break;
        }
    }
    
    if (isValidOut != 0) {
        *isValidOut = validFlag;
    }
    else if (validFlag == false) {
        CaretAssertMessage(0, AString("guiName " + lowerCaseName + " failed to match enumerated value for type OmeTimeUnitEnum"));
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
OmeTimeUnitEnum::toIntegerCode(Enum enumValue)
{
    if (initializedFlag == false) initialize();
    const OmeTimeUnitEnum* enumInstance = findData(enumValue);
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
OmeTimeUnitEnum::Enum
OmeTimeUnitEnum::fromIntegerCode(const int32_t integerCode, bool* isValidOut)
{
    if (initializedFlag == false) initialize();
    
    bool validFlag = false;
    Enum enumValue = OmeTimeUnitEnum::enumData[0].enumValue;
    
    for (std::vector<OmeTimeUnitEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const OmeTimeUnitEnum& enumInstance = *iter;
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
        CaretAssertMessage(0, AString("Integer code " + AString::number(integerCode) + " failed to match enumerated value for type OmeTimeUnitEnum"));
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
OmeTimeUnitEnum::getAllEnums(std::vector<OmeTimeUnitEnum::Enum>& allEnums)
{
    if (initializedFlag == false) initialize();
    
    allEnums.clear();
    
    for (std::vector<OmeTimeUnitEnum>::iterator iter = enumData.begin();
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
OmeTimeUnitEnum::getAllNames(std::vector<AString>& allNames, const bool isSorted)
{
    if (initializedFlag == false) initialize();
    
    allNames.clear();
    
    for (std::vector<OmeTimeUnitEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        allNames.push_back(OmeTimeUnitEnum::toName(iter->enumValue));
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
OmeTimeUnitEnum::getAllGuiNames(std::vector<AString>& allGuiNames, const bool isSorted)
{
    if (initializedFlag == false) initialize();
    
    allGuiNames.clear();
    
    for (std::vector<OmeTimeUnitEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        allGuiNames.push_back(OmeTimeUnitEnum::toGuiName(iter->enumValue));
    }
    
    if (isSorted) {
        std::sort(allGuiNames.begin(), allGuiNames.end());
    }
}

