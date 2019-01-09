
/*LICENSE_START*/
/*
 *  Copyright (C) 2019 Washington University School of Medicine
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
#define __WU_Q_MACRO_STANDARD_ITEM_TYPE_ENUM_DECLARE__
#include "WuQMacroStandardItemTypeEnum.h"
#undef __WU_Q_MACRO_STANDARD_ITEM_TYPE_ENUM_DECLARE__

#include <QStandardItem>

#include "CaretAssert.h"

using namespace caret;

    
/**
 * \class caret::WuQMacroStandardItemTypeEnum
 * \brief Enumerated type for macro items in a QStandardItemModel
 *
 * Macros and macro command are displayed in the GUI using a
 * QStandardItem.  The QStandardItem documentation recommends
 * overriding the type() method and returning a value greater
 * than or equal to UserType.  This enumerated type provides
 * these values.
 *
 * Using this enumerated type in the GUI with an EnumComboBoxTemplate
 * 
 * Header File (.h)
 *     Forward declare the data type:
 *         class EnumComboBoxTemplate;
 * 
 *     Declare the member:
 *         EnumComboBoxTemplate* m_wuQMacroStandardItemTypeEnumComboBox;
 * 
 *     Declare a slot that is called when user changes selection
 *         private slots:
 *             void wuQMacroStandardItemTypeEnumComboBoxItemActivated();
 * 
 * Implementation File (.cxx)
 *     Include the header files
 *         #include "EnumComboBoxTemplate.h"
 *         #include "WuQMacroStandardItemTypeEnum.h"
 * 
 *     Instatiate:
 *         m_wuQMacroStandardItemTypeEnumComboBox = new EnumComboBoxTemplate(this);
 *         m_wuQMacroStandardItemTypeEnumComboBox->setup<WuQMacroStandardItemTypeEnum,WuQMacroStandardItemTypeEnum::Enum>();
 * 
 *     Get notified when the user changes the selection: 
 *         QObject::connect(m_wuQMacroStandardItemTypeEnumComboBox, SIGNAL(itemActivated()),
 *                          this, SLOT(wuQMacroStandardItemTypeEnumComboBoxItemActivated()));
 * 
 *     Update the selection:
 *         m_wuQMacroStandardItemTypeEnumComboBox->setSelectedItem<WuQMacroStandardItemTypeEnum,WuQMacroStandardItemTypeEnum::Enum>(NEW_VALUE);
 * 
 *     Read the selection:
 *         const WuQMacroStandardItemTypeEnum::Enum VARIABLE = m_wuQMacroStandardItemTypeEnumComboBox->getSelectedItem<WuQMacroStandardItemTypeEnum,WuQMacroStandardItemTypeEnum::Enum>();
 * 
 */

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
WuQMacroStandardItemTypeEnum::WuQMacroStandardItemTypeEnum(const Enum enumValue,
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
WuQMacroStandardItemTypeEnum::~WuQMacroStandardItemTypeEnum()
{
}

/**
 * Initialize the enumerated metadata.
 */
void
WuQMacroStandardItemTypeEnum::initialize()
{
    if (initializedFlag) {
        return;
    }
    initializedFlag = true;

    int32_t typeOffset = QStandardItem::UserType + 1;
    const int32_t invalidType(typeOffset++);
    const int32_t macroType(typeOffset++);
    const int32_t macroCommandType(typeOffset++);
    
    enumData.push_back(WuQMacroStandardItemTypeEnum(INVALID,
                                                    invalidType,
                                                    "INVALID",
                                                    "Invalid"));
    
    enumData.push_back(WuQMacroStandardItemTypeEnum(MACRO,
                                                    macroType,
                                                    "MACRO",
                                                    "Macro"));
    
    enumData.push_back(WuQMacroStandardItemTypeEnum(MACRO_COMMAND,
                                                    macroCommandType,
                                                    "MACRO_COMMAND",
                                                    "Macro Command"));
}

/**
 * Find the data for and enumerated value.
 * @param enumValue
 *     The enumerated value.
 * @return Pointer to data for this enumerated type
 * or NULL if no data for type or if type is invalid.
 */
const WuQMacroStandardItemTypeEnum*
WuQMacroStandardItemTypeEnum::findData(const Enum enumValue)
{
    if (initializedFlag == false) initialize();

    size_t num = enumData.size();
    for (size_t i = 0; i < num; i++) {
        const WuQMacroStandardItemTypeEnum* d = &enumData[i];
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
WuQMacroStandardItemTypeEnum::toName(Enum enumValue) {
    if (initializedFlag == false) initialize();
    
    const WuQMacroStandardItemTypeEnum* enumInstance = findData(enumValue);
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
WuQMacroStandardItemTypeEnum::Enum 
WuQMacroStandardItemTypeEnum::fromName(const AString& name, bool* isValidOut)
{
    if (initializedFlag == false) initialize();
    
    bool validFlag = false;
    Enum enumValue = WuQMacroStandardItemTypeEnum::enumData[0].enumValue;
    
    for (std::vector<WuQMacroStandardItemTypeEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const WuQMacroStandardItemTypeEnum& d = *iter;
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
        CaretAssertMessage(0, AString("Name " + name + "failed to match enumerated value for type WuQMacroStandardItemTypeEnum"));
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
WuQMacroStandardItemTypeEnum::toGuiName(Enum enumValue) {
    if (initializedFlag == false) initialize();
    
    const WuQMacroStandardItemTypeEnum* enumInstance = findData(enumValue);
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
WuQMacroStandardItemTypeEnum::Enum 
WuQMacroStandardItemTypeEnum::fromGuiName(const AString& guiName, bool* isValidOut)
{
    if (initializedFlag == false) initialize();
    
    bool validFlag = false;
    Enum enumValue = WuQMacroStandardItemTypeEnum::enumData[0].enumValue;
    
    for (std::vector<WuQMacroStandardItemTypeEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const WuQMacroStandardItemTypeEnum& d = *iter;
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
        CaretAssertMessage(0, AString("guiName " + guiName + "failed to match enumerated value for type WuQMacroStandardItemTypeEnum"));
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
WuQMacroStandardItemTypeEnum::toIntegerCode(Enum enumValue)
{
    if (initializedFlag == false) initialize();
    const WuQMacroStandardItemTypeEnum* enumInstance = findData(enumValue);
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
WuQMacroStandardItemTypeEnum::Enum
WuQMacroStandardItemTypeEnum::fromIntegerCode(const int32_t integerCode, bool* isValidOut)
{
    if (initializedFlag == false) initialize();
    
    bool validFlag = false;
    Enum enumValue = WuQMacroStandardItemTypeEnum::enumData[0].enumValue;
    
    for (std::vector<WuQMacroStandardItemTypeEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const WuQMacroStandardItemTypeEnum& enumInstance = *iter;
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
        CaretAssertMessage(0, AString("Integer code " + AString::number(integerCode) + "failed to match enumerated value for type WuQMacroStandardItemTypeEnum"));
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
WuQMacroStandardItemTypeEnum::getAllEnums(std::vector<WuQMacroStandardItemTypeEnum::Enum>& allEnums)
{
    if (initializedFlag == false) initialize();
    
    allEnums.clear();
    
    for (std::vector<WuQMacroStandardItemTypeEnum>::iterator iter = enumData.begin();
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
WuQMacroStandardItemTypeEnum::getAllNames(std::vector<AString>& allNames, const bool isSorted)
{
    if (initializedFlag == false) initialize();
    
    allNames.clear();
    
    for (std::vector<WuQMacroStandardItemTypeEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        allNames.push_back(WuQMacroStandardItemTypeEnum::toName(iter->enumValue));
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
WuQMacroStandardItemTypeEnum::getAllGuiNames(std::vector<AString>& allGuiNames, const bool isSorted)
{
    if (initializedFlag == false) initialize();
    
    allGuiNames.clear();
    
    for (std::vector<WuQMacroStandardItemTypeEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        allGuiNames.push_back(WuQMacroStandardItemTypeEnum::toGuiName(iter->enumValue));
    }
    
    if (isSorted) {
        std::sort(allGuiNames.begin(), allGuiNames.end());
    }
}

