
/*LICENSE_START*/
/*
 *  Copyright (C) 2015 Washington University School of Medicine
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
#define __PALETTE_NORMALIZATION_MODE_ENUM_DECLARE__
#include "PaletteNormalizationModeEnum.h"
#undef __PALETTE_NORMALIZATION_MODE_ENUM_DECLARE__

#include "CaretAssert.h"

using namespace caret;

    
/**
 * \class caret::PaletteNormalizationModeEnum 
 * \brief Enumerated type for normalization of file/map data.
 *
 * Using this enumerated type in the GUI with an EnumComboBoxTemplate
 * 
 * Header File (.h)
 *     Forward declare the data type:
 *         class EnumComboBoxTemplate;
 * 
 *     Declare the member:
 *         EnumComboBoxTemplate* m_paletteNormalizationModeEnumComboBox;
 * 
 *     Declare a slot that is called when user changes selection
 *         private slots:
 *             void paletteNormalizationModeEnumComboBoxItemActivated();
 * 
 * Implementation File (.cxx)
 *     Include the header files
 *         #include "EnumComboBoxTemplate.h"
 *         #include "PaletteNormalizationModeEnum.h"
 * 
 *     Instatiate:
 *         m_paletteNormalizationModeEnumComboBox = new EnumComboBoxTemplate(this);
 *         m_paletteNormalizationModeEnumComboBox->setup<PaletteNormalizationModeEnum,PaletteNormalizationModeEnum::Enum>();
 * 
 *     Get notified when the user changes the selection: 
 *         QObject::connect(m_paletteNormalizationModeEnumComboBox, SIGNAL(itemActivated()),
 *                          this, SLOT(paletteNormalizationModeEnumComboBoxItemActivated()));
 * 
 *     Update the selection:
 *         m_paletteNormalizationModeEnumComboBox->setSelectedItem<PaletteNormalizationModeEnum,PaletteNormalizationModeEnum::Enum>(NEW_VALUE);
 * 
 *     Read the selection:
 *         const PaletteNormalizationModeEnum::Enum VARIABLE = m_paletteNormalizationModeEnumComboBox->getSelectedItem<PaletteNormalizationModeEnum,PaletteNormalizationModeEnum::Enum>();
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
PaletteNormalizationModeEnum::PaletteNormalizationModeEnum(const Enum enumValue,
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
PaletteNormalizationModeEnum::~PaletteNormalizationModeEnum()
{
}

/**
 * Initialize the enumerated metadata.
 */
void
PaletteNormalizationModeEnum::initialize()
{
    if (initializedFlag) {
        return;
    }
    initializedFlag = true;

    enumData.push_back(PaletteNormalizationModeEnum(NORMALIZATION_ALL_MAP_DATA, 
                                    "NORMALIZATION_ALL_MAP_DATA", 
                                    "All Maps In File"));
    
    enumData.push_back(PaletteNormalizationModeEnum(NORMALIZATION_SELECTED_MAP_DATA, 
                                    "NORMALIZATION_SELECTED_MAP_DATA", 
                                    "Selected Map In File"));
    
}

/**
 * Find the data for and enumerated value.
 * @param enumValue
 *     The enumerated value.
 * @return Pointer to data for this enumerated type
 * or NULL if no data for type or if type is invalid.
 */
const PaletteNormalizationModeEnum*
PaletteNormalizationModeEnum::findData(const Enum enumValue)
{
    if (initializedFlag == false) initialize();

    size_t num = enumData.size();
    for (size_t i = 0; i < num; i++) {
        const PaletteNormalizationModeEnum* d = &enumData[i];
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
PaletteNormalizationModeEnum::toName(Enum enumValue) {
    if (initializedFlag == false) initialize();
    
    const PaletteNormalizationModeEnum* enumInstance = findData(enumValue);
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
PaletteNormalizationModeEnum::Enum 
PaletteNormalizationModeEnum::fromName(const AString& name, bool* isValidOut)
{
    if (initializedFlag == false) initialize();
    
    bool validFlag = false;
    Enum enumValue = PaletteNormalizationModeEnum::enumData[0].enumValue;
    
    for (std::vector<PaletteNormalizationModeEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const PaletteNormalizationModeEnum& d = *iter;
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
        CaretAssertMessage(0, AString("Name " + name + "failed to match enumerated value for type PaletteNormalizationModeEnum"));
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
PaletteNormalizationModeEnum::toGuiName(Enum enumValue) {
    if (initializedFlag == false) initialize();
    
    const PaletteNormalizationModeEnum* enumInstance = findData(enumValue);
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
PaletteNormalizationModeEnum::Enum 
PaletteNormalizationModeEnum::fromGuiName(const AString& guiName, bool* isValidOut)
{
    if (initializedFlag == false) initialize();
    
    bool validFlag = false;
    Enum enumValue = PaletteNormalizationModeEnum::enumData[0].enumValue;
    
    for (std::vector<PaletteNormalizationModeEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const PaletteNormalizationModeEnum& d = *iter;
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
        CaretAssertMessage(0, AString("guiName " + guiName + "failed to match enumerated value for type PaletteNormalizationModeEnum"));
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
PaletteNormalizationModeEnum::toIntegerCode(Enum enumValue)
{
    if (initializedFlag == false) initialize();
    const PaletteNormalizationModeEnum* enumInstance = findData(enumValue);
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
PaletteNormalizationModeEnum::Enum
PaletteNormalizationModeEnum::fromIntegerCode(const int32_t integerCode, bool* isValidOut)
{
    if (initializedFlag == false) initialize();
    
    bool validFlag = false;
    Enum enumValue = PaletteNormalizationModeEnum::enumData[0].enumValue;
    
    for (std::vector<PaletteNormalizationModeEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const PaletteNormalizationModeEnum& enumInstance = *iter;
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
        CaretAssertMessage(0, AString("Integer code " + AString::number(integerCode) + "failed to match enumerated value for type PaletteNormalizationModeEnum"));
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
PaletteNormalizationModeEnum::getAllEnums(std::vector<PaletteNormalizationModeEnum::Enum>& allEnums)
{
    if (initializedFlag == false) initialize();
    
    allEnums.clear();
    
    for (std::vector<PaletteNormalizationModeEnum>::iterator iter = enumData.begin();
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
PaletteNormalizationModeEnum::getAllNames(std::vector<AString>& allNames, const bool isSorted)
{
    if (initializedFlag == false) initialize();
    
    allNames.clear();
    
    for (std::vector<PaletteNormalizationModeEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        allNames.push_back(PaletteNormalizationModeEnum::toName(iter->enumValue));
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
PaletteNormalizationModeEnum::getAllGuiNames(std::vector<AString>& allGuiNames, const bool isSorted)
{
    if (initializedFlag == false) initialize();
    
    allGuiNames.clear();
    
    for (std::vector<PaletteNormalizationModeEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        allGuiNames.push_back(PaletteNormalizationModeEnum::toGuiName(iter->enumValue));
    }
    
    if (isSorted) {
        std::sort(allGuiNames.begin(), allGuiNames.end());
    }
}

AString
PaletteNormalizationModeEnum::getEnumToolTopInHTML()
{
    const AString msg("<HTML><BODY>"
                      "Normalization controls how data values are mapped to the color palette.  "
                      "The data's most negative, zero, and most positive values are mapped to "
                      "the palette's -1.0, 0.0, and 1.0 colors.<br><br>"
                      "<B>" + toGuiName(PaletteNormalizationModeEnum::NORMALIZATION_ALL_MAP_DATA) + "</B><br>"
                      + "   Uses data from all maps within the file and results in identical data "
                      "values from any map in the file receiving identical coloring.<br><br>"
                      "<B>" + toGuiName(PaletteNormalizationModeEnum::NORMALIZATION_SELECTED_MAP_DATA) + "</B><br>"
                      + "   Uses data from the selected map and so identical data values "
                      " in different maps may receive different coloring.<br>"
                      "</BODY></HTML>");
    
    return msg;
}


