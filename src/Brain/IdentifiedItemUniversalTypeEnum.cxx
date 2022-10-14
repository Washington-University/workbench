
/*LICENSE_START*/
/*
 *  Copyright (C) 2021 Washington University School of Medicine
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
#define __IDENTIFIED_ITEM_UNIVERSAL_TYPE_ENUM_DECLARE__
#include "IdentifiedItemUniversalTypeEnum.h"
#undef __IDENTIFIED_ITEM_UNIVERSAL_TYPE_ENUM_DECLARE__

#include "CaretAssert.h"

using namespace caret;

    
/**
 * \class caret::IdentifiedItemUniversalTypeEnum 
 * \brief Type of item in a IdentifiedItemUniversal instance
 *
 * Using this enumerated type in the GUI with an EnumComboBoxTemplate
 * 
 * Header File (.h)
 *     Forward declare the data type:
 *         class EnumComboBoxTemplate;
 * 
 *     Declare the member:
 *         EnumComboBoxTemplate* m_identifiedItemUniversalTypeEnumComboBox;
 * 
 *     Declare a slot that is called when user changes selection
 *         private slots:
 *             void identifiedItemUniversalTypeEnumComboBoxItemActivated();
 * 
 * Implementation File (.cxx)
 *     Include the header files
 *         #include "EnumComboBoxTemplate.h"
 *         #include "IdentifiedItemUniversalTypeEnum.h"
 * 
 *     Instatiate:
 *         m_identifiedItemUniversalTypeEnumComboBox = new EnumComboBoxTemplate(this);
 *         m_identifiedItemUniversalTypeEnumComboBox->setup<IdentifiedItemUniversalTypeEnum,IdentifiedItemUniversalTypeEnum::Enum>();
 * 
 *     Get notified when the user changes the selection: 
 *         QObject::connect(m_identifiedItemUniversalTypeEnumComboBox, SIGNAL(itemActivated()),
 *                          this, SLOT(identifiedItemUniversalTypeEnumComboBoxItemActivated()));
 * 
 *     Update the selection:
 *         m_identifiedItemUniversalTypeEnumComboBox->setSelectedItem<IdentifiedItemUniversalTypeEnum,IdentifiedItemUniversalTypeEnum::Enum>(NEW_VALUE);
 * 
 *     Read the selection:
 *         const IdentifiedItemUniversalTypeEnum::Enum VARIABLE = m_identifiedItemUniversalTypeEnumComboBox->getSelectedItem<IdentifiedItemUniversalTypeEnum,IdentifiedItemUniversalTypeEnum::Enum>();
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
IdentifiedItemUniversalTypeEnum::IdentifiedItemUniversalTypeEnum(const Enum enumValue,
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
IdentifiedItemUniversalTypeEnum::~IdentifiedItemUniversalTypeEnum()
{
}

/**
 * Initialize the enumerated metadata.
 */
void
IdentifiedItemUniversalTypeEnum::initialize()
{
    if (initializedFlag) {
        return;
    }
    initializedFlag = true;

    enumData.push_back(IdentifiedItemUniversalTypeEnum(INVALID,
                                                       "INVALID",
                                                       "Invalid"));
    
    enumData.push_back(IdentifiedItemUniversalTypeEnum(HISTOLOGY_PLANE_COORDINATE,
                                                       "HISTOLOGY_PLANE_COORDINATE",
                                                       "Histology Plane Coordinate"));
    
    enumData.push_back(IdentifiedItemUniversalTypeEnum(HISTOLOGY_STEREOTAXIC_COORDINATE,
                                                       "HISTOLOGY_STEREOTAXIC_COORDINATE",
                                                       "Histology Stereotaxic Coordinate"));
    
    enumData.push_back(IdentifiedItemUniversalTypeEnum(MEDIA_LOGICAL_COORDINATE,
                                                       "MEDIA_LOGICAL_COORDINATE",
                                                       "Media Logical Coordinate"));
    
    enumData.push_back(IdentifiedItemUniversalTypeEnum(MEDIA_PLANE_COORDINATE,
                                                       "MEDIA_PLANE_COORDINATE",
                                                       "Media Plane Coordinate"));
    
    enumData.push_back(IdentifiedItemUniversalTypeEnum(STEREOTAXIC_XYZ,
                                                       "STEREOTAXIC_XYZ",
                                                       "Stereotaxic XYZ"));
    
    enumData.push_back(IdentifiedItemUniversalTypeEnum(SURFACE,
                                                       "SURFACE",
                                                       "Surface"));
    
    enumData.push_back(IdentifiedItemUniversalTypeEnum(TEXT_NO_SYMBOL,
                                                       "TEXT_NO_SYMBOL",
                                                       "Text No Symbol"));
    
    enumData.push_back(IdentifiedItemUniversalTypeEnum(VOLUME_INTENSITY_2D,
                                                       "VOLUME_INTENSITY_3D",
                                                       "Volume Intensity 3D"));
    
    enumData.push_back(IdentifiedItemUniversalTypeEnum(VOLUME_INTENSITY_3D,
                                                       "VOLUME_INTENSITY_3D",
                                                       "Volume Intensity 3D"));
    
    enumData.push_back(IdentifiedItemUniversalTypeEnum(VOLUME_SLICES,
                                                       "VOLUME_SLICES",
                                                       "Volume Slices"));

}

/**
 * Find the data for and enumerated value.
 * @param enumValue
 *     The enumerated value.
 * @return Pointer to data for this enumerated type
 * or NULL if no data for type or if type is invalid.
 */
const IdentifiedItemUniversalTypeEnum*
IdentifiedItemUniversalTypeEnum::findData(const Enum enumValue)
{
    if (initializedFlag == false) initialize();

    size_t num = enumData.size();
    for (size_t i = 0; i < num; i++) {
        const IdentifiedItemUniversalTypeEnum* d = &enumData[i];
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
IdentifiedItemUniversalTypeEnum::toName(Enum enumValue) {
    if (initializedFlag == false) initialize();
    
    const IdentifiedItemUniversalTypeEnum* enumInstance = findData(enumValue);
    return enumInstance->name;
}

/**
 * Get an enumerated value corresponding to its name.
 * @param nameIn
 *     Name of enumerated value.
 * @param isValidOut 
 *     If not NULL, it is set indicating that a
 *     enum value exists for the input name.
 * @return 
 *     Enumerated value.
 */
IdentifiedItemUniversalTypeEnum::Enum 
IdentifiedItemUniversalTypeEnum::fromName(const AString& nameIn, bool* isValidOut)
{
    if (initializedFlag == false) initialize();
    
    AString name(nameIn);
    
    /* VOLUME was renamed to VOLUME_SLICES */
    if (name == "VOLUME") {
        name = IdentifiedItemUniversalTypeEnum::toName(VOLUME_SLICES);
    }
    if (name == "MEDIA") {
        name = IdentifiedItemUniversalTypeEnum::toName(MEDIA_LOGICAL_COORDINATE);
    }
    
    bool validFlag = false;
    Enum enumValue = IdentifiedItemUniversalTypeEnum::enumData[0].enumValue;
    
    for (std::vector<IdentifiedItemUniversalTypeEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const IdentifiedItemUniversalTypeEnum& d = *iter;
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
        CaretAssertMessage(0, AString("Name " + name + " failed to match enumerated value for type IdentifiedItemUniversalTypeEnum"));
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
IdentifiedItemUniversalTypeEnum::toGuiName(Enum enumValue) {
    if (initializedFlag == false) initialize();
    
    const IdentifiedItemUniversalTypeEnum* enumInstance = findData(enumValue);
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
IdentifiedItemUniversalTypeEnum::Enum 
IdentifiedItemUniversalTypeEnum::fromGuiName(const AString& guiName, bool* isValidOut)
{
    if (initializedFlag == false) initialize();
    
    bool validFlag = false;
    Enum enumValue = IdentifiedItemUniversalTypeEnum::enumData[0].enumValue;
    
    for (std::vector<IdentifiedItemUniversalTypeEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const IdentifiedItemUniversalTypeEnum& d = *iter;
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
        CaretAssertMessage(0, AString("guiName " + guiName + " failed to match enumerated value for type IdentifiedItemUniversalTypeEnum"));
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
IdentifiedItemUniversalTypeEnum::toIntegerCode(Enum enumValue)
{
    if (initializedFlag == false) initialize();
    const IdentifiedItemUniversalTypeEnum* enumInstance = findData(enumValue);
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
IdentifiedItemUniversalTypeEnum::Enum
IdentifiedItemUniversalTypeEnum::fromIntegerCode(const int32_t integerCode, bool* isValidOut)
{
    if (initializedFlag == false) initialize();
    
    bool validFlag = false;
    Enum enumValue = IdentifiedItemUniversalTypeEnum::enumData[0].enumValue;
    
    for (std::vector<IdentifiedItemUniversalTypeEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const IdentifiedItemUniversalTypeEnum& enumInstance = *iter;
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
        CaretAssertMessage(0, AString("Integer code " + AString::number(integerCode) + " failed to match enumerated value for type IdentifiedItemUniversalTypeEnum"));
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
IdentifiedItemUniversalTypeEnum::getAllEnums(std::vector<IdentifiedItemUniversalTypeEnum::Enum>& allEnums)
{
    if (initializedFlag == false) initialize();
    
    allEnums.clear();
    
    for (std::vector<IdentifiedItemUniversalTypeEnum>::iterator iter = enumData.begin();
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
IdentifiedItemUniversalTypeEnum::getAllNames(std::vector<AString>& allNames, const bool isSorted)
{
    if (initializedFlag == false) initialize();
    
    allNames.clear();
    
    for (std::vector<IdentifiedItemUniversalTypeEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        allNames.push_back(IdentifiedItemUniversalTypeEnum::toName(iter->enumValue));
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
IdentifiedItemUniversalTypeEnum::getAllGuiNames(std::vector<AString>& allGuiNames, const bool isSorted)
{
    if (initializedFlag == false) initialize();
    
    allGuiNames.clear();
    
    for (std::vector<IdentifiedItemUniversalTypeEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        allGuiNames.push_back(IdentifiedItemUniversalTypeEnum::toGuiName(iter->enumValue));
    }
    
    if (isSorted) {
        std::sort(allGuiNames.begin(), allGuiNames.end());
    }
}

