
/*LICENSE_START*/
/*
 *  Copyright (C) 2018 Washington University School of Medicine
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
#define __PALETTE_INVERT_MODE_ENUM_DECLARE__
#include "PaletteInvertModeEnum.h"
#undef __PALETTE_INVERT_MODE_ENUM_DECLARE__

#include "CaretAssert.h"

using namespace caret;

    
/**
 * \class caret::PaletteInvertModeEnum 
 * \brief Inversion mode for palettes.
 *
 * Using this enumerated type in the GUI with an EnumComboBoxTemplate
 * 
 * Header File (.h)
 *     Forward declare the data type:
 *         class EnumComboBoxTemplate;
 * 
 *     Declare the member:
 *         EnumComboBoxTemplate* m_paletteInvertModeEnumComboBox;
 * 
 *     Declare a slot that is called when user changes selection
 *         private slots:
 *             void paletteInvertModeEnumComboBoxItemActivated();
 * 
 * Implementation File (.cxx)
 *     Include the header files
 *         #include "EnumComboBoxTemplate.h"
 *         #include "PaletteInvertModeEnum.h"
 * 
 *     Instatiate:
 *         m_paletteInvertModeEnumComboBox = new EnumComboBoxTemplate(this);
 *         m_paletteInvertModeEnumComboBox->setup<PaletteInvertModeEnum,PaletteInvertModeEnum::Enum>();
 * 
 *     Get notified when the user changes the selection: 
 *         QObject::connect(m_paletteInvertModeEnumComboBox, SIGNAL(itemActivated()),
 *                          this, SLOT(paletteInvertModeEnumComboBoxItemActivated()));
 * 
 *     Update the selection:
 *         m_paletteInvertModeEnumComboBox->setSelectedItem<PaletteInvertModeEnum,PaletteInvertModeEnum::Enum>(NEW_VALUE);
 * 
 *     Read the selection:
 *         const PaletteInvertModeEnum::Enum VARIABLE = m_paletteInvertModeEnumComboBox->getSelectedItem<PaletteInvertModeEnum,PaletteInvertModeEnum::Enum>();
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
PaletteInvertModeEnum::PaletteInvertModeEnum(const Enum enumValue,
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
PaletteInvertModeEnum::~PaletteInvertModeEnum()
{
}

/**
 * Initialize the enumerated metadata.
 */
void
PaletteInvertModeEnum::initialize()
{
    if (initializedFlag) {
        return;
    }
    initializedFlag = true;

    enumData.push_back(PaletteInvertModeEnum(OFF, 
                                    "OFF", 
                                    "Off"));
    
    enumData.push_back(PaletteInvertModeEnum(POSITIVE_WITH_NEGATIVE, 
                                    "POSITIVE_WITH_NEGATIVE",
                                    "Swap Pos with Neg"));
    
    enumData.push_back(PaletteInvertModeEnum(POSITIVE_NEGATIVE_SEPARATE, 
                                    "POSITIVE_NEGATIVE_SEPARATE",
                                    "Swap Pos/Neg Separate Zero"));
    
    enumData.push_back(PaletteInvertModeEnum(POSITIVE_NEGATIVE_SEPARATE_NONE,
                                             "POSITIVE_NEGATIVE_SEPARATE_NONE",
                                             "Swap Pos/Neg Separate NONE"));
    
}

/**
 * Find the data for and enumerated value.
 * @param enumValue
 *     The enumerated value.
 * @return Pointer to data for this enumerated type
 * or NULL if no data for type or if type is invalid.
 */
const PaletteInvertModeEnum*
PaletteInvertModeEnum::findData(const Enum enumValue)
{
    if (initializedFlag == false) initialize();

    size_t num = enumData.size();
    for (size_t i = 0; i < num; i++) {
        const PaletteInvertModeEnum* d = &enumData[i];
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
PaletteInvertModeEnum::toName(Enum enumValue) {
    if (initializedFlag == false) initialize();
    
    const PaletteInvertModeEnum* enumInstance = findData(enumValue);
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
PaletteInvertModeEnum::Enum 
PaletteInvertModeEnum::fromName(const AString& nameIn, bool* isValidOut)
{
    if (initializedFlag == false) initialize();
    
    /*
     * Fix misspelled name that may be in some files
     */
    AString name = nameIn;
    if (name == "POSITIVE_NEGATIVE_SEPARATE NONE") {
        name = "POSITIVE_NEGATIVE_SEPARATE_NONE";
    }
    
    bool validFlag = false;
    Enum enumValue = PaletteInvertModeEnum::enumData[0].enumValue;
    
    for (std::vector<PaletteInvertModeEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const PaletteInvertModeEnum& d = *iter;
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
        CaretAssertMessage(0, AString("Name " + name + "failed to match enumerated value for type PaletteInvertModeEnum"));
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
PaletteInvertModeEnum::toGuiName(Enum enumValue) {
    if (initializedFlag == false) initialize();
    
    const PaletteInvertModeEnum* enumInstance = findData(enumValue);
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
PaletteInvertModeEnum::Enum 
PaletteInvertModeEnum::fromGuiName(const AString& guiName, bool* isValidOut)
{
    if (initializedFlag == false) initialize();
    
    bool validFlag = false;
    Enum enumValue = PaletteInvertModeEnum::enumData[0].enumValue;
    
    for (std::vector<PaletteInvertModeEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const PaletteInvertModeEnum& d = *iter;
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
        CaretAssertMessage(0, AString("guiName " + guiName + "failed to match enumerated value for type PaletteInvertModeEnum"));
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
PaletteInvertModeEnum::toIntegerCode(Enum enumValue)
{
    if (initializedFlag == false) initialize();
    const PaletteInvertModeEnum* enumInstance = findData(enumValue);
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
PaletteInvertModeEnum::Enum
PaletteInvertModeEnum::fromIntegerCode(const int32_t integerCode, bool* isValidOut)
{
    if (initializedFlag == false) initialize();
    
    bool validFlag = false;
    Enum enumValue = PaletteInvertModeEnum::enumData[0].enumValue;
    
    for (std::vector<PaletteInvertModeEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const PaletteInvertModeEnum& enumInstance = *iter;
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
        CaretAssertMessage(0, AString("Integer code " + AString::number(integerCode) + "failed to match enumerated value for type PaletteInvertModeEnum"));
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
PaletteInvertModeEnum::getAllEnums(std::vector<PaletteInvertModeEnum::Enum>& allEnums)
{
    if (initializedFlag == false) initialize();
    
    allEnums.clear();
    
    for (std::vector<PaletteInvertModeEnum>::iterator iter = enumData.begin();
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
PaletteInvertModeEnum::getAllNames(std::vector<AString>& allNames, const bool isSorted)
{
    if (initializedFlag == false) initialize();
    
    allNames.clear();
    
    for (std::vector<PaletteInvertModeEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        allNames.push_back(PaletteInvertModeEnum::toName(iter->enumValue));
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
PaletteInvertModeEnum::getAllGuiNames(std::vector<AString>& allGuiNames, const bool isSorted)
{
    if (initializedFlag == false) initialize();
    
    allGuiNames.clear();
    
    for (std::vector<PaletteInvertModeEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        allGuiNames.push_back(PaletteInvertModeEnum::toGuiName(iter->enumValue));
    }
    
    if (isSorted) {
        std::sort(allGuiNames.begin(), allGuiNames.end());
    }
}

