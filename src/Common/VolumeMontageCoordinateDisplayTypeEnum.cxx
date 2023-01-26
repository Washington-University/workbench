
/*LICENSE_START*/
/*
 *  Copyright (C) 2023 Washington University School of Medicine
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
#define __VOLUME_MONTAGE_COORDINATE_DISPLAY_TYPE_ENUM_DECLARE__
#include "VolumeMontageCoordinateDisplayTypeEnum.h"
#undef __VOLUME_MONTAGE_COORDINATE_DISPLAY_TYPE_ENUM_DECLARE__

#include "CaretAssert.h"

using namespace caret;

    
/**
 * \class caret::VolumeMontageCoordinateDisplayTypeEnum 
 * \brief Mode for display of coordinates on slices in a volume slice montage
 *
 * Using this enumerated type in the GUI with an EnumComboBoxTemplate
 * 
 * Header File (.h)
 *     Forward declare the data type:
 *         class EnumComboBoxTemplate;
 * 
 *     Declare the member:
 *         EnumComboBoxTemplate* m_volumeMontageCoordinateDisplayTypeEnumComboBox;
 * 
 *     Declare a slot that is called when user changes selection
 *         private slots:
 *             void volumeMontageCoordinateDisplayTypeEnumComboBoxItemActivated();
 * 
 * Implementation File (.cxx)
 *     Include the header files
 *         #include "EnumComboBoxTemplate.h"
 *         #include "VolumeMontageCoordinateDisplayTypeEnum.h"
 * 
 *     Instatiate:
 *         m_volumeMontageCoordinateDisplayTypeEnumComboBox = new EnumComboBoxTemplate(this);
 *         m_volumeMontageCoordinateDisplayTypeEnumComboBox->setup<VolumeMontageCoordinateDisplayTypeEnum,VolumeMontageCoordinateDisplayTypeEnum::Enum>();
 * 
 *     Get notified when the user changes the selection: 
 *         QObject::connect(m_volumeMontageCoordinateDisplayTypeEnumComboBox, SIGNAL(itemActivated()),
 *                          this, SLOT(volumeMontageCoordinateDisplayTypeEnumComboBoxItemActivated()));
 * 
 *     Update the selection:
 *         m_volumeMontageCoordinateDisplayTypeEnumComboBox->setSelectedItem<VolumeMontageCoordinateDisplayTypeEnum,VolumeMontageCoordinateDisplayTypeEnum::Enum>(NEW_VALUE);
 * 
 *     Read the selection:
 *         const VolumeMontageCoordinateDisplayTypeEnum::Enum VARIABLE = m_volumeMontageCoordinateDisplayTypeEnumComboBox->getSelectedItem<VolumeMontageCoordinateDisplayTypeEnum,VolumeMontageCoordinateDisplayTypeEnum::Enum>();
 * 
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
VolumeMontageCoordinateDisplayTypeEnum::VolumeMontageCoordinateDisplayTypeEnum(const Enum enumValue,
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
VolumeMontageCoordinateDisplayTypeEnum::~VolumeMontageCoordinateDisplayTypeEnum()
{
}

/**
 * Initialize the enumerated metadata.
 */
void
VolumeMontageCoordinateDisplayTypeEnum::initialize()
{
    if (initializedFlag) {
        return;
    }
    initializedFlag = true;

    enumData.push_back(VolumeMontageCoordinateDisplayTypeEnum(OFFSET,
                                                              "OFFSET",
                                                              "Offset"));
    
    enumData.push_back(VolumeMontageCoordinateDisplayTypeEnum(XYZ,
                                                              "XYZ",
                                                              "XYZ"));
}

/**
 * Find the data for and enumerated value.
 * @param enumValue
 *     The enumerated value.
 * @return Pointer to data for this enumerated type
 * or NULL if no data for type or if type is invalid.
 */
const VolumeMontageCoordinateDisplayTypeEnum*
VolumeMontageCoordinateDisplayTypeEnum::findData(const Enum enumValue)
{
    if (initializedFlag == false) initialize();

    size_t num = enumData.size();
    for (size_t i = 0; i < num; i++) {
        const VolumeMontageCoordinateDisplayTypeEnum* d = &enumData[i];
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
VolumeMontageCoordinateDisplayTypeEnum::toName(Enum enumValue) {
    if (initializedFlag == false) initialize();
    
    const VolumeMontageCoordinateDisplayTypeEnum* enumInstance = findData(enumValue);
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
VolumeMontageCoordinateDisplayTypeEnum::Enum 
VolumeMontageCoordinateDisplayTypeEnum::fromName(const AString& name, bool* isValidOut)
{
    if (initializedFlag == false) initialize();
    
    bool validFlag = false;
    Enum enumValue = VolumeMontageCoordinateDisplayTypeEnum::enumData[0].enumValue;
    
    for (std::vector<VolumeMontageCoordinateDisplayTypeEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const VolumeMontageCoordinateDisplayTypeEnum& d = *iter;
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
        CaretAssertMessage(0, AString("Name " + name + " failed to match enumerated value for type VolumeMontageCoordinateDisplayTypeEnum"));
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
VolumeMontageCoordinateDisplayTypeEnum::toGuiName(Enum enumValue) {
    if (initializedFlag == false) initialize();
    
    const VolumeMontageCoordinateDisplayTypeEnum* enumInstance = findData(enumValue);
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
VolumeMontageCoordinateDisplayTypeEnum::Enum 
VolumeMontageCoordinateDisplayTypeEnum::fromGuiName(const AString& guiName, bool* isValidOut)
{
    if (initializedFlag == false) initialize();
    
    bool validFlag = false;
    Enum enumValue = VolumeMontageCoordinateDisplayTypeEnum::enumData[0].enumValue;
    
    for (std::vector<VolumeMontageCoordinateDisplayTypeEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const VolumeMontageCoordinateDisplayTypeEnum& d = *iter;
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
        CaretAssertMessage(0, AString("guiName " + guiName + " failed to match enumerated value for type VolumeMontageCoordinateDisplayTypeEnum"));
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
VolumeMontageCoordinateDisplayTypeEnum::toIntegerCode(Enum enumValue)
{
    if (initializedFlag == false) initialize();
    const VolumeMontageCoordinateDisplayTypeEnum* enumInstance = findData(enumValue);
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
VolumeMontageCoordinateDisplayTypeEnum::Enum
VolumeMontageCoordinateDisplayTypeEnum::fromIntegerCode(const int32_t integerCode, bool* isValidOut)
{
    if (initializedFlag == false) initialize();
    
    bool validFlag = false;
    Enum enumValue = VolumeMontageCoordinateDisplayTypeEnum::enumData[0].enumValue;
    
    for (std::vector<VolumeMontageCoordinateDisplayTypeEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const VolumeMontageCoordinateDisplayTypeEnum& enumInstance = *iter;
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
        CaretAssertMessage(0, AString("Integer code " + AString::number(integerCode) + " failed to match enumerated value for type VolumeMontageCoordinateDisplayTypeEnum"));
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
VolumeMontageCoordinateDisplayTypeEnum::getAllEnums(std::vector<VolumeMontageCoordinateDisplayTypeEnum::Enum>& allEnums)
{
    if (initializedFlag == false) initialize();
    
    allEnums.clear();
    
    for (std::vector<VolumeMontageCoordinateDisplayTypeEnum>::iterator iter = enumData.begin();
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
VolumeMontageCoordinateDisplayTypeEnum::getAllNames(std::vector<AString>& allNames, const bool isSorted)
{
    if (initializedFlag == false) initialize();
    
    allNames.clear();
    
    for (std::vector<VolumeMontageCoordinateDisplayTypeEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        allNames.push_back(VolumeMontageCoordinateDisplayTypeEnum::toName(iter->enumValue));
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
VolumeMontageCoordinateDisplayTypeEnum::getAllGuiNames(std::vector<AString>& allGuiNames, const bool isSorted)
{
    if (initializedFlag == false) initialize();
    
    allGuiNames.clear();
    
    for (std::vector<VolumeMontageCoordinateDisplayTypeEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        allGuiNames.push_back(VolumeMontageCoordinateDisplayTypeEnum::toGuiName(iter->enumValue));
    }
    
    if (isSorted) {
        std::sort(allGuiNames.begin(), allGuiNames.end());
    }
}

