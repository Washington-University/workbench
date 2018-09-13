
/*LICENSE_START*/
/*
 *  Copyright (C) 2014 Washington University School of Medicine
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
#define __DEVELOPER_FLAGS_ENUM_DECLARE__
#include "DeveloperFlagsEnum.h"
#undef __DEVELOPER_FLAGS_ENUM_DECLARE__

#include "CaretAssert.h"

using namespace caret;

    
/**
 * \class caret::DeveloperFlagsEnum 
 * \brief Flags used during development.
 *
 * Using this enumerated type in the GUI with an EnumComboBoxTemplate
 * 
 * Header File (.h)
 *     Forward declare the data type:
 *         class EnumComboBoxTemplate;
 * 
 *     Declare the member:
 *         EnumComboBoxTemplate* m_developerFlagsEnumComboBox;
 * 
 *     Declare a slot that is called when user changes selection
 *         private slots:
 *             void developerFlagsEnumComboBoxItemActivated();
 * 
 * Implementation File (.cxx)
 *     Include the header files
 *         #include "EnumComboBoxTemplate.h"
 *         #include "DeveloperFlagsEnum.h"
 * 
 *     Instatiate:
 *         m_developerFlagsEnumComboBox = new EnumComboBoxTemplate(this);
 *         m_developerFlagsEnumComboBox->setup<DeveloperFlagsEnum,DeveloperFlagsEnum::Enum>();
 * 
 *     Get notified when the user changes the selection: 
 *         QObject::connect(m_developerFlagsEnumComboBox, SIGNAL(itemActivated()),
 *                          this, SLOT(developerFlagsEnumComboBoxItemActivated()));
 * 
 *     Update the selection:
 *         m_developerFlagsEnumComboBox->setSelectedItem<DeveloperFlagsEnum,DeveloperFlagsEnum::Enum>(NEW_VALUE);
 * 
 *     Read the selection:
 *         const DeveloperFlagsEnum::Enum VARIABLE = m_developerFlagsEnumComboBox->getSelectedItem<DeveloperFlagsEnum,DeveloperFlagsEnum::Enum>();
 * 
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
 * @param defaultValue
 *    Default value for flag
 */
DeveloperFlagsEnum::DeveloperFlagsEnum(const Enum enumValue,
                                       const AString& name,
                                       const AString& guiName,
                                       const bool defaultValue)
{
    this->enumValue = enumValue;
    this->integerCode = integerCodeCounter++;
    this->name = name;
    this->guiName = guiName;
    this->flagStatus = defaultValue;
}

/**
 * Destructor.
 */
DeveloperFlagsEnum::~DeveloperFlagsEnum()
{
}

/**
 * Initialize the enumerated metadata.
 */
void
DeveloperFlagsEnum::initialize()
{
    if (initializedFlag) {
        return;
    }
    initializedFlag = true;

    enumData.push_back(DeveloperFlagsEnum(DEVELOPER_FLAG_UNUSED,
                                          "DEVELOPER_FLAG_UNUSED",
                                          "Developer Flag Unused",
                                          false));
    enumData.push_back(DeveloperFlagsEnum(DEVELOPER_FLAG_FLIP_PALETTE_NOT_DATA,
                                          "DEVELOPER_FLAG_FLIP_PALETTE_NOT_DATA",
                                          "Flip Palette Not Data",
                                          false));
    enumData.push_back(DeveloperFlagsEnum(DEVELOPER_FLAG_BRAIN_TIPS,
                                          "DEVELOPER_FLAG_BRAIN_TIPS",
                                          "Enable Data Tool Tips",
                                          false));
}

/**
 * Find the data for and enumerated value.
 * @param enumValue
 *     The enumerated value.
 * @return Pointer to data for this enumerated type
 * or NULL if no data for type or if type is invalid.
 */
DeveloperFlagsEnum*
DeveloperFlagsEnum::findData(const Enum enumValue)
{
    if (initializedFlag == false) initialize();

    size_t num = enumData.size();
    for (size_t i = 0; i < num; i++) {
        DeveloperFlagsEnum* d = &enumData[i];
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
DeveloperFlagsEnum::toName(Enum enumValue) {
    if (initializedFlag == false) initialize();
    
    const DeveloperFlagsEnum* enumInstance = findData(enumValue);
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
DeveloperFlagsEnum::Enum 
DeveloperFlagsEnum::fromName(const AString& name, bool* isValidOut)
{
    if (initializedFlag == false) initialize();
    
    bool validFlag = false;
    Enum enumValue = DeveloperFlagsEnum::enumData[0].enumValue;
    
    for (std::vector<DeveloperFlagsEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const DeveloperFlagsEnum& d = *iter;
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
        CaretAssertMessage(0, AString("Name " + name + "failed to match enumerated value for type DeveloperFlagsEnum"));
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
DeveloperFlagsEnum::toGuiName(Enum enumValue) {
    if (initializedFlag == false) initialize();
    
    const DeveloperFlagsEnum* enumInstance = findData(enumValue);
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
DeveloperFlagsEnum::Enum 
DeveloperFlagsEnum::fromGuiName(const AString& guiName, bool* isValidOut)
{
    if (initializedFlag == false) initialize();
    
    bool validFlag = false;
    Enum enumValue = DeveloperFlagsEnum::enumData[0].enumValue;
    
    for (std::vector<DeveloperFlagsEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const DeveloperFlagsEnum& d = *iter;
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
        CaretAssertMessage(0, AString("guiName " + guiName + "failed to match enumerated value for type DeveloperFlagsEnum"));
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
DeveloperFlagsEnum::toIntegerCode(Enum enumValue)
{
    if (initializedFlag == false) initialize();
    const DeveloperFlagsEnum* enumInstance = findData(enumValue);
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
DeveloperFlagsEnum::Enum
DeveloperFlagsEnum::fromIntegerCode(const int32_t integerCode, bool* isValidOut)
{
    if (initializedFlag == false) initialize();
    
    bool validFlag = false;
    Enum enumValue = DeveloperFlagsEnum::enumData[0].enumValue;
    
    for (std::vector<DeveloperFlagsEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const DeveloperFlagsEnum& enumInstance = *iter;
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
        CaretAssertMessage(0, AString("Integer code " + AString::number(integerCode) + "failed to match enumerated value for type DeveloperFlagsEnum"));
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
DeveloperFlagsEnum::getAllEnums(std::vector<DeveloperFlagsEnum::Enum>& allEnums)
{
    if (initializedFlag == false) initialize();
    
    allEnums.clear();
    
    for (std::vector<DeveloperFlagsEnum>::iterator iter = enumData.begin();
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
DeveloperFlagsEnum::getAllNames(std::vector<AString>& allNames, const bool isSorted)
{
    if (initializedFlag == false) initialize();
    
    allNames.clear();
    
    for (std::vector<DeveloperFlagsEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        allNames.push_back(DeveloperFlagsEnum::toName(iter->enumValue));
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
DeveloperFlagsEnum::getAllGuiNames(std::vector<AString>& allGuiNames, const bool isSorted)
{
    if (initializedFlag == false) initialize();
    
    allGuiNames.clear();
    
    for (std::vector<DeveloperFlagsEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        allGuiNames.push_back(DeveloperFlagsEnum::toGuiName(iter->enumValue));
    }
    
    if (isSorted) {
        std::sort(allGuiNames.begin(), allGuiNames.end());
    }
}

/**
 * Is the developer flag set?
 *
 * @param enumValue
 *     Enum value for flag
 * @return
 *     True/False status for flag.
 */
bool
DeveloperFlagsEnum::isFlag(const Enum enumValue)
{
    if (initializedFlag == false) initialize();
    const DeveloperFlagsEnum* enumInstance = findData(enumValue);
    return enumInstance->flagStatus;
}

/**
 * Set the developer flag.
 *
 * @param enumValue
 *     Enum value for flag
 * @param flagStatus
 *     True/False status for flag.
 */
void
DeveloperFlagsEnum::setFlag(const Enum enumValue,
                            const bool flagStatus)
{
    if (initializedFlag == false) initialize();
    DeveloperFlagsEnum* enumInstance = findData(enumValue);
    enumInstance->flagStatus = flagStatus;
}


