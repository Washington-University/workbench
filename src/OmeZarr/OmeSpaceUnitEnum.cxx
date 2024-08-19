
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
#define __OME_SPACE_UNIT_ENUM_DECLARE__
#include "OmeSpaceUnitEnum.h"
#undef __OME_SPACE_UNIT_ENUM_DECLARE__

#include "CaretAssert.h"

using namespace caret;

    
/**
 * \class caret::OmeSpaceUnitEnum 
 * \brief Units when type is space
 *
 *
 * Using this enumerated type in the GUI with an EnumComboBoxTemplate
 * 
 * Header File (.h)
 *     Forward declare the data type:
 *         class EnumComboBoxTemplate;
 * 
 *     Declare the member:
 *         EnumComboBoxTemplate* m_ngffSpaceUnitEnumComboBox;
 * 
 *     Declare a slot that is called when user changes selection
 *         private slots:
 *             void ngffSpaceUnitEnumComboBoxItemActivated();
 * 
 * Implementation File (.cxx)
 *     Include the header files
 *         #include "EnumComboBoxTemplate.h"
 *         #include "OmeSpaceUnitEnum.h"
 * 
 *     Instatiate:
 *         m_ngffSpaceUnitEnumComboBox = new EnumComboBoxTemplate(this);
 *         m_ngffSpaceUnitEnumComboBox->setup<OmeSpaceUnitEnum,OmeSpaceUnitEnum::Enum>();
 * 
 *     Get notified when the user changes the selection: 
 *         QObject::connect(m_ngffSpaceUnitEnumComboBox, SIGNAL(itemActivated()),
 *                          this, SLOT(ngffSpaceUnitEnumComboBoxItemActivated()));
 * 
 *     Update the selection:
 *         m_ngffSpaceUnitEnumComboBox->setSelectedItem<OmeSpaceUnitEnum,OmeSpaceUnitEnum::Enum>(NEW_VALUE);
 * 
 *     Read the selection:
 *         const OmeSpaceUnitEnum::Enum VARIABLE = m_ngffSpaceUnitEnumComboBox->getSelectedItem<OmeSpaceUnitEnum,OmeSpaceUnitEnum::Enum>();
 * 
 */

/*
switch (value) {
    case OmeSpaceUnitEnum::UNKNOWN:
        break;
    case OmeSpaceUnitEnum::ANGSTROM:
        break;
    case OmeSpaceUnitEnum::ATTOMETER:
        break;
    case OmeSpaceUnitEnum::CENTIMETER:
        break;
    case OmeSpaceUnitEnum::DECIMETER:
        break;
    case OmeSpaceUnitEnum::EXAMETER:
        break;
    case OmeSpaceUnitEnum::FEMTOMETER:
        break;
    case OmeSpaceUnitEnum::FOOT:
        break;
    case OmeSpaceUnitEnum::GIGAMETER:
        break;
    case OmeSpaceUnitEnum::HECTOMETER:
        break;
    case OmeSpaceUnitEnum::INCH:
        break;
    case OmeSpaceUnitEnum::KILOMETER:
        break;
    case OmeSpaceUnitEnum::MEGAMETER:
        break;
    case OmeSpaceUnitEnum::METER:
        break;
    case OmeSpaceUnitEnum::MICROMETER:
        break;
    case OmeSpaceUnitEnum::MILE:
        break;
    case OmeSpaceUnitEnum::MILLIMETER:
        break;
    case OmeSpaceUnitEnum::NANOMETER:
        break;
    case OmeSpaceUnitEnum::PARSEC:
        break;
    case OmeSpaceUnitEnum::PETAMETER:
        break;
    case OmeSpaceUnitEnum::PICOMETER:
        break;
    case OmeSpaceUnitEnum::TERAMETER:
        break;
    case OmeSpaceUnitEnum::YARD:
        break;
    case OmeSpaceUnitEnum::YOCTOMETER:
        break;
    case OmeSpaceUnitEnum::YOTTAMETER:
        break;
    case OmeSpaceUnitEnum::ZEPTOMETER:
        break;
    case OmeSpaceUnitEnum::ZETTAMETER:
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
OmeSpaceUnitEnum::OmeSpaceUnitEnum(const Enum enumValue,
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
OmeSpaceUnitEnum::~OmeSpaceUnitEnum()
{
}

/**
 * Initialize the enumerated metadata.
 */
void
OmeSpaceUnitEnum::initialize()
{
    if (initializedFlag) {
        return;
    }
    initializedFlag = true;

    enumData.push_back(OmeSpaceUnitEnum(UNKNOWN, 
                                    "UNKNOWN", 
                                    "Unknown"));
    
    enumData.push_back(OmeSpaceUnitEnum(ANGSTROM, 
                                    "ANGSTROM", 
                                    "Angstrom"));
    
    enumData.push_back(OmeSpaceUnitEnum(ATTOMETER, 
                                    "ATTOMETER", 
                                    "Attometer"));
    
    enumData.push_back(OmeSpaceUnitEnum(CENTIMETER, 
                                    "CENTIMETER", 
                                    "Centimeter"));
    
    enumData.push_back(OmeSpaceUnitEnum(DECIMETER, 
                                    "DECIMETER", 
                                    "Decimeter"));
    
    enumData.push_back(OmeSpaceUnitEnum(EXAMETER, 
                                    "EXAMETER", 
                                    "Exameter"));
    
    enumData.push_back(OmeSpaceUnitEnum(FEMTOMETER, 
                                    "FEMTOMETER", 
                                    "Femtometer"));
    
    enumData.push_back(OmeSpaceUnitEnum(FOOT, 
                                    "FOOT", 
                                    "Foot"));
    
    enumData.push_back(OmeSpaceUnitEnum(GIGAMETER, 
                                    "GIGAMETER", 
                                    "Gigameter"));
    
    enumData.push_back(OmeSpaceUnitEnum(HECTOMETER, 
                                    "HECTOMETER", 
                                    "Hectometer"));
    
    enumData.push_back(OmeSpaceUnitEnum(INCH, 
                                    "INCH", 
                                    "Inch"));
    
    enumData.push_back(OmeSpaceUnitEnum(KILOMETER, 
                                    "KILOMETER", 
                                    "Kilometer"));
    
    enumData.push_back(OmeSpaceUnitEnum(MEGAMETER, 
                                    "MEGAMETER", 
                                    "Megameter"));
    
    enumData.push_back(OmeSpaceUnitEnum(METER, 
                                    "METER", 
                                    "Meter"));
    
    enumData.push_back(OmeSpaceUnitEnum(MICROMETER, 
                                    "MICROMETER", 
                                    "Micrometer"));
    
    enumData.push_back(OmeSpaceUnitEnum(MILE, 
                                    "MILE", 
                                    "Mile"));
    
    enumData.push_back(OmeSpaceUnitEnum(MILLIMETER, 
                                    "MILLIMETER", 
                                    "Millimeter"));
    
    enumData.push_back(OmeSpaceUnitEnum(NANOMETER, 
                                    "NANOMETER", 
                                    "Nanometer"));
    
    enumData.push_back(OmeSpaceUnitEnum(PARSEC, 
                                    "PARSEC", 
                                    "Parsec"));
    
    enumData.push_back(OmeSpaceUnitEnum(PETAMETER, 
                                    "PETAMETER", 
                                    "Petameter"));
    
    enumData.push_back(OmeSpaceUnitEnum(PICOMETER, 
                                    "PICOMETER", 
                                    "Picometer"));
    
    enumData.push_back(OmeSpaceUnitEnum(TERAMETER, 
                                    "TERAMETER", 
                                    "Terameter"));
    
    enumData.push_back(OmeSpaceUnitEnum(YARD, 
                                    "YARD", 
                                    "Yard"));
    
    enumData.push_back(OmeSpaceUnitEnum(YOCTOMETER, 
                                    "YOCTOMETER", 
                                    "Yoctometer"));
    
    enumData.push_back(OmeSpaceUnitEnum(YOTTAMETER, 
                                    "YOTTAMETER", 
                                    "Yottameter"));
    
    enumData.push_back(OmeSpaceUnitEnum(ZEPTOMETER, 
                                    "ZEPTOMETER", 
                                    "Zeptometer"));
    
    enumData.push_back(OmeSpaceUnitEnum(ZETTAMETER, 
                                    "ZETTAMETER", 
                                    "Zettameter"));
    
}

/**
 * Find the data for and enumerated value.
 * @param enumValue
 *     The enumerated value.
 * @return Pointer to data for this enumerated type
 * or NULL if no data for type or if type is invalid.
 */
const OmeSpaceUnitEnum*
OmeSpaceUnitEnum::findData(const Enum enumValue)
{
    if (initializedFlag == false) initialize();

    size_t num = enumData.size();
    for (size_t i = 0; i < num; i++) {
        const OmeSpaceUnitEnum* d = &enumData[i];
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
OmeSpaceUnitEnum::toName(Enum enumValue) {
    if (initializedFlag == false) initialize();
    
    const OmeSpaceUnitEnum* enumInstance = findData(enumValue);
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
OmeSpaceUnitEnum::Enum 
OmeSpaceUnitEnum::fromName(const AString& name, bool* isValidOut)
{
    if (initializedFlag == false) initialize();
    
    bool validFlag = false;
    Enum enumValue = OmeSpaceUnitEnum::enumData[0].enumValue;
    
    for (std::vector<OmeSpaceUnitEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const OmeSpaceUnitEnum& d = *iter;
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
        CaretAssertMessage(0, AString("Name " + name + " failed to match enumerated value for type OmeSpaceUnitEnum"));
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
OmeSpaceUnitEnum::toGuiName(Enum enumValue) {
    if (initializedFlag == false) initialize();
    
    const OmeSpaceUnitEnum* enumInstance = findData(enumValue);
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
OmeSpaceUnitEnum::Enum 
OmeSpaceUnitEnum::fromGuiName(const AString& guiName, bool* isValidOut)
{
    if (initializedFlag == false) initialize();
    
    bool validFlag = false;
    Enum enumValue = OmeSpaceUnitEnum::enumData[0].enumValue;
    
    for (std::vector<OmeSpaceUnitEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const OmeSpaceUnitEnum& d = *iter;
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
        CaretAssertMessage(0, AString("guiName " + guiName + " failed to match enumerated value for type OmeSpaceUnitEnum"));
    }
    return enumValue;
}

/**
 * Get a lowercase string representation of the enumerated type.
 * @param enumValue
 *     Enumerated value.
 * @return
 *     String representing enumerated value.
 */
AString
OmeSpaceUnitEnum::toLowerCaseName(Enum enumValue) {
    if (initializedFlag == false) initialize();
    
    const OmeSpaceUnitEnum* enumInstance = findData(enumValue);
    return enumInstance->guiName.toLower();
}

/**
 * Get an enumerated value corresponding to its lowercase name.
 * @param lowerCaseName
 *     Name of enumerated value.
 * @param isValidOut
 *     If not NULL, it is set indicating that a
 *     enum value exists for the input name.
 * @return
 *     Enumerated value.
 */
OmeSpaceUnitEnum::Enum
OmeSpaceUnitEnum::fromLowerCaseName(const AString& lowerCaseName, bool* isValidOut)
{
    if (initializedFlag == false) initialize();
    
    bool validFlag = false;
    Enum enumValue = OmeSpaceUnitEnum::enumData[0].enumValue;
    
    for (std::vector<OmeSpaceUnitEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const OmeSpaceUnitEnum& d = *iter;
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
        CaretAssertMessage(0, AString("guiName " + lowerCaseName + " failed to match enumerated value for type OmeSpaceUnitEnum"));
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
OmeSpaceUnitEnum::toIntegerCode(Enum enumValue)
{
    if (initializedFlag == false) initialize();
    const OmeSpaceUnitEnum* enumInstance = findData(enumValue);
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
OmeSpaceUnitEnum::Enum
OmeSpaceUnitEnum::fromIntegerCode(const int32_t integerCode, bool* isValidOut)
{
    if (initializedFlag == false) initialize();
    
    bool validFlag = false;
    Enum enumValue = OmeSpaceUnitEnum::enumData[0].enumValue;
    
    for (std::vector<OmeSpaceUnitEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const OmeSpaceUnitEnum& enumInstance = *iter;
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
        CaretAssertMessage(0, AString("Integer code " + AString::number(integerCode) + " failed to match enumerated value for type OmeSpaceUnitEnum"));
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
OmeSpaceUnitEnum::getAllEnums(std::vector<OmeSpaceUnitEnum::Enum>& allEnums)
{
    if (initializedFlag == false) initialize();
    
    allEnums.clear();
    
    for (std::vector<OmeSpaceUnitEnum>::iterator iter = enumData.begin();
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
OmeSpaceUnitEnum::getAllNames(std::vector<AString>& allNames, const bool isSorted)
{
    if (initializedFlag == false) initialize();
    
    allNames.clear();
    
    for (std::vector<OmeSpaceUnitEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        allNames.push_back(OmeSpaceUnitEnum::toName(iter->enumValue));
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
OmeSpaceUnitEnum::getAllGuiNames(std::vector<AString>& allGuiNames, const bool isSorted)
{
    if (initializedFlag == false) initialize();
    
    allGuiNames.clear();
    
    for (std::vector<OmeSpaceUnitEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        allGuiNames.push_back(OmeSpaceUnitEnum::toGuiName(iter->enumValue));
    }
    
    if (isSorted) {
        std::sort(allGuiNames.begin(), allGuiNames.end());
    }
}

