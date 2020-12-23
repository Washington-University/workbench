
/*LICENSE_START*/
/*
 *  Copyright (C) 2020 Washington University School of Medicine
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
#define __IDENTIFICATION_SYMBOL_SIZE_TYPE_ENUM_DECLARE__
#include "IdentificationSymbolSizeTypeEnum.h"
#undef __IDENTIFICATION_SYMBOL_SIZE_TYPE_ENUM_DECLARE__

#include "CaretAssert.h"

using namespace caret;

    
/**
 * \class caret::IdentificationSymbolSizeTypeEnum 
 * \brief Enumerated type for sizing of identification symbol
 *
 * Using this enumerated type in the GUI with an EnumComboBoxTemplate
 * 
 * Header File (.h)
 *     Forward declare the data type:
 *         class EnumComboBoxTemplate;
 * 
 *     Declare the member:
 *         EnumComboBoxTemplate* m_identificationSymbolSizeTypeEnumComboBox;
 * 
 *     Declare a slot that is called when user changes selection
 *         private slots:
 *             void identificationSymbolSizeTypeEnumComboBoxItemActivated();
 * 
 * Implementation File (.cxx)
 *     Include the header files
 *         #include "EnumComboBoxTemplate.h"
 *         #include "IdentificationSymbolSizeTypeEnum.h"
 * 
 *     Instatiate:
 *         m_identificationSymbolSizeTypeEnumComboBox = new EnumComboBoxTemplate(this);
 *         m_identificationSymbolSizeTypeEnumComboBox->setup<IdentificationSymbolSizeTypeEnum,IdentificationSymbolSizeTypeEnum::Enum>();
 * 
 *     Get notified when the user changes the selection: 
 *         QObject::connect(m_identificationSymbolSizeTypeEnumComboBox, SIGNAL(itemActivated()),
 *                          this, SLOT(identificationSymbolSizeTypeEnumComboBoxItemActivated()));
 * 
 *     Update the selection:
 *         m_identificationSymbolSizeTypeEnumComboBox->setSelectedItem<IdentificationSymbolSizeTypeEnum,IdentificationSymbolSizeTypeEnum::Enum>(NEW_VALUE);
 * 
 *     Read the selection:
 *         const IdentificationSymbolSizeTypeEnum::Enum VARIABLE = m_identificationSymbolSizeTypeEnumComboBox->getSelectedItem<IdentificationSymbolSizeTypeEnum,IdentificationSymbolSizeTypeEnum::Enum>();
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
IdentificationSymbolSizeTypeEnum::IdentificationSymbolSizeTypeEnum(const Enum enumValue,
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
IdentificationSymbolSizeTypeEnum::~IdentificationSymbolSizeTypeEnum()
{
}

/**
 * Initialize the enumerated metadata.
 */
void
IdentificationSymbolSizeTypeEnum::initialize()
{
    if (initializedFlag) {
        return;
    }
    initializedFlag = true;

    enumData.push_back(IdentificationSymbolSizeTypeEnum(MILLIMETERS, 
                                    "MILLIMETERS", 
                                    "Millimeters"));
    
    enumData.push_back(IdentificationSymbolSizeTypeEnum(PERCENTAGE, 
                                    "PERCENTAGE", 
                                    "Percentage"));
    
}

/**
 * Find the data for and enumerated value.
 * @param enumValue
 *     The enumerated value.
 * @return Pointer to data for this enumerated type
 * or NULL if no data for type or if type is invalid.
 */
const IdentificationSymbolSizeTypeEnum*
IdentificationSymbolSizeTypeEnum::findData(const Enum enumValue)
{
    if (initializedFlag == false) initialize();

    size_t num = enumData.size();
    for (size_t i = 0; i < num; i++) {
        const IdentificationSymbolSizeTypeEnum* d = &enumData[i];
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
IdentificationSymbolSizeTypeEnum::toName(Enum enumValue) {
    if (initializedFlag == false) initialize();
    
    const IdentificationSymbolSizeTypeEnum* enumInstance = findData(enumValue);
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
IdentificationSymbolSizeTypeEnum::Enum 
IdentificationSymbolSizeTypeEnum::fromName(const AString& name, bool* isValidOut)
{
    if (initializedFlag == false) initialize();
    
    bool validFlag = false;
    Enum enumValue = IdentificationSymbolSizeTypeEnum::enumData[0].enumValue;
    
    for (std::vector<IdentificationSymbolSizeTypeEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const IdentificationSymbolSizeTypeEnum& d = *iter;
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
        CaretAssertMessage(0, AString("Name " + name + "failed to match enumerated value for type IdentificationSymbolSizeTypeEnum"));
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
IdentificationSymbolSizeTypeEnum::toGuiName(Enum enumValue) {
    if (initializedFlag == false) initialize();
    
    const IdentificationSymbolSizeTypeEnum* enumInstance = findData(enumValue);
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
IdentificationSymbolSizeTypeEnum::Enum 
IdentificationSymbolSizeTypeEnum::fromGuiName(const AString& guiName, bool* isValidOut)
{
    if (initializedFlag == false) initialize();
    
    bool validFlag = false;
    Enum enumValue = IdentificationSymbolSizeTypeEnum::enumData[0].enumValue;
    
    for (std::vector<IdentificationSymbolSizeTypeEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const IdentificationSymbolSizeTypeEnum& d = *iter;
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
        CaretAssertMessage(0, AString("guiName " + guiName + "failed to match enumerated value for type IdentificationSymbolSizeTypeEnum"));
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
IdentificationSymbolSizeTypeEnum::toIntegerCode(Enum enumValue)
{
    if (initializedFlag == false) initialize();
    const IdentificationSymbolSizeTypeEnum* enumInstance = findData(enumValue);
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
IdentificationSymbolSizeTypeEnum::Enum
IdentificationSymbolSizeTypeEnum::fromIntegerCode(const int32_t integerCode, bool* isValidOut)
{
    if (initializedFlag == false) initialize();
    
    bool validFlag = false;
    Enum enumValue = IdentificationSymbolSizeTypeEnum::enumData[0].enumValue;
    
    for (std::vector<IdentificationSymbolSizeTypeEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const IdentificationSymbolSizeTypeEnum& enumInstance = *iter;
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
        CaretAssertMessage(0, AString("Integer code " + AString::number(integerCode) + "failed to match enumerated value for type IdentificationSymbolSizeTypeEnum"));
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
IdentificationSymbolSizeTypeEnum::getAllEnums(std::vector<IdentificationSymbolSizeTypeEnum::Enum>& allEnums)
{
    if (initializedFlag == false) initialize();
    
    allEnums.clear();
    
    for (std::vector<IdentificationSymbolSizeTypeEnum>::iterator iter = enumData.begin();
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
IdentificationSymbolSizeTypeEnum::getAllNames(std::vector<AString>& allNames, const bool isSorted)
{
    if (initializedFlag == false) initialize();
    
    allNames.clear();
    
    for (std::vector<IdentificationSymbolSizeTypeEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        allNames.push_back(IdentificationSymbolSizeTypeEnum::toName(iter->enumValue));
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
IdentificationSymbolSizeTypeEnum::getAllGuiNames(std::vector<AString>& allGuiNames, const bool isSorted)
{
    if (initializedFlag == false) initialize();
    
    allGuiNames.clear();
    
    for (std::vector<IdentificationSymbolSizeTypeEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        allGuiNames.push_back(IdentificationSymbolSizeTypeEnum::toGuiName(iter->enumValue));
    }
    
    if (isSorted) {
        std::sort(allGuiNames.begin(), allGuiNames.end());
    }
}

/**
 * Get tooltip for use in GUI.
 */
AString
IdentificationSymbolSizeTypeEnum::getToolTip(const AString& symbolTypeName)
{
    AString tt("<html>"
               "Select method for sizing YYYY symbols."
               "<ul>");
    
    std::vector<IdentificationSymbolSizeTypeEnum::Enum> allEnums;
    getAllEnums(allEnums);
    
    for (auto e : allEnums) {
        tt.append("<li>" + toGuiName(e) + " - ");
        switch (e) {
            case MILLIMETERS:
                tt.append("XXXX symbols are sized in millimeters.  If viewing brains of different sizes "
                          "(different species; child vs adult), the YYYY symbols appear larger "
                          "on smaller brains (monkey, child) and smaller on larger brains (human, adult).");
                break;
            case PERCENTAGE:
                tt.append("XXXX symbols are sized as a percentage of the maximum brain dimension (typically "
                          "the Anterior-Posterior axis).  "
                          "Thus, the YYYY symbols are drawn in a proportional size on all brains.");
                break;
        }
    }
    tt.append("</ul>");
    tt.append("Zooming a surface or volume always affects the drawn size of the YYYY symbols.");
    tt.append("</html>");
    
    if ( ! symbolTypeName.isEmpty()) {
        AString upperCase(symbolTypeName);
        upperCase[0] = upperCase[0].toUpper();
        AString lowerCase(symbolTypeName);
        lowerCase[0] = lowerCase[0].toLower();
        tt = tt.replace("XXXX", upperCase);
        tt = tt.replace("YYYY", lowerCase);
    }
    
    return tt;
}

