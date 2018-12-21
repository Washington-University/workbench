
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
#define __WU_Q_MACRO_SHORT_CUT_KEY_ENUM_DECLARE__
#include "WuQMacroShortCutKeyEnum.h"
#undef __WU_Q_MACRO_SHORT_CUT_KEY_ENUM_DECLARE__

#include "CaretAssert.h"

using namespace caret;

    
/**
 * \class caret::WuQMacroShortCutKeyEnum 
 * \brief Shortcut keys for running macros
 *
 * Using this enumerated type in the GUI with an EnumComboBoxTemplate
 * 
 * Header File (.h)
 *     Forward declare the data type:
 *         class EnumComboBoxTemplate;
 * 
 *     Declare the member:
 *         EnumComboBoxTemplate* m_wuQMacroShortCutKeyEnumComboBox;
 * 
 *     Declare a slot that is called when user changes selection
 *         private slots:
 *             void wuQMacroShortCutKeyEnumComboBoxItemActivated();
 * 
 * Implementation File (.cxx)
 *     Include the header files
 *         #include "EnumComboBoxTemplate.h"
 *         #include "WuQMacroShortCutKeyEnum.h"
 * 
 *     Instatiate:
 *         m_wuQMacroShortCutKeyEnumComboBox = new EnumComboBoxTemplate(this);
 *         m_wuQMacroShortCutKeyEnumComboBox->setup<WuQMacroShortCutKeyEnum,WuQMacroShortCutKeyEnum::Enum>();
 * 
 *     Get notified when the user changes the selection: 
 *         QObject::connect(m_wuQMacroShortCutKeyEnumComboBox, SIGNAL(itemActivated()),
 *                          this, SLOT(wuQMacroShortCutKeyEnumComboBoxItemActivated()));
 * 
 *     Update the selection:
 *         m_wuQMacroShortCutKeyEnumComboBox->setSelectedItem<WuQMacroShortCutKeyEnum,WuQMacroShortCutKeyEnum::Enum>(NEW_VALUE);
 * 
 *     Read the selection:
 *         const WuQMacroShortCutKeyEnum::Enum VARIABLE = m_wuQMacroShortCutKeyEnumComboBox->getSelectedItem<WuQMacroShortCutKeyEnum,WuQMacroShortCutKeyEnum::Enum>();
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
WuQMacroShortCutKeyEnum::WuQMacroShortCutKeyEnum(const Enum enumValue,
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
WuQMacroShortCutKeyEnum::~WuQMacroShortCutKeyEnum()
{
}

/**
 * Initialize the enumerated metadata.
 */
void
WuQMacroShortCutKeyEnum::initialize()
{
    if (initializedFlag) {
        return;
    }
    initializedFlag = true;

    enumData.push_back(WuQMacroShortCutKeyEnum(Key_None, 
                                    "Key_None", 
                                    "None"));
    
    enumData.push_back(WuQMacroShortCutKeyEnum(Key_A, 
                                    "Key_A", 
                                    "A"));
    
    enumData.push_back(WuQMacroShortCutKeyEnum(Key_B, 
                                    "Key_B", 
                                    "B"));
    
    enumData.push_back(WuQMacroShortCutKeyEnum(Key_C, 
                                    "Key_C", 
                                    "C"));
    
    enumData.push_back(WuQMacroShortCutKeyEnum(Key_D, 
                                    "Key_D", 
                                    "D"));
    
    enumData.push_back(WuQMacroShortCutKeyEnum(Key_E, 
                                    "Key_E", 
                                    "E"));
    
    enumData.push_back(WuQMacroShortCutKeyEnum(Key_F, 
                                    "Key_F", 
                                    "F"));
    
    enumData.push_back(WuQMacroShortCutKeyEnum(Key_G, 
                                    "Key_G", 
                                    "G"));
    
    enumData.push_back(WuQMacroShortCutKeyEnum(Key_H, 
                                    "Key_H", 
                                    "H"));
    
    enumData.push_back(WuQMacroShortCutKeyEnum(Key_I, 
                                    "Key_I", 
                                    "I"));
    
    enumData.push_back(WuQMacroShortCutKeyEnum(Key_J, 
                                    "Key_J", 
                                    "J"));
    
    enumData.push_back(WuQMacroShortCutKeyEnum(Key_K, 
                                    "Key_K", 
                                    "K"));
    
    enumData.push_back(WuQMacroShortCutKeyEnum(Key_L, 
                                    "Key_L", 
                                    "L"));
    
    enumData.push_back(WuQMacroShortCutKeyEnum(Key_M, 
                                    "Key_M", 
                                    "M"));
    
    enumData.push_back(WuQMacroShortCutKeyEnum(Key_N, 
                                    "Key_N", 
                                    "N"));
    
    enumData.push_back(WuQMacroShortCutKeyEnum(Key_O, 
                                    "Key_O", 
                                    "O"));
    
    enumData.push_back(WuQMacroShortCutKeyEnum(Key_P, 
                                    "Key_P", 
                                    "P"));
    
    enumData.push_back(WuQMacroShortCutKeyEnum(Key_Q, 
                                    "Key_Q", 
                                    "Q"));
    
    enumData.push_back(WuQMacroShortCutKeyEnum(Key_R, 
                                    "Key_R", 
                                    "R"));
    
    enumData.push_back(WuQMacroShortCutKeyEnum(Key_S, 
                                    "Key_S", 
                                    "S"));
    
    enumData.push_back(WuQMacroShortCutKeyEnum(Key_T, 
                                    "Key_T", 
                                    "T"));
    
    enumData.push_back(WuQMacroShortCutKeyEnum(Key_U, 
                                    "Key_U", 
                                    "U"));
    
    enumData.push_back(WuQMacroShortCutKeyEnum(Key_V, 
                                    "Key_V", 
                                    "V"));
    
    enumData.push_back(WuQMacroShortCutKeyEnum(Key_W, 
                                    "Key_W", 
                                    "W"));
    
    enumData.push_back(WuQMacroShortCutKeyEnum(Key_X, 
                                    "Key_X", 
                                    "X"));
    
    enumData.push_back(WuQMacroShortCutKeyEnum(Key_Y, 
                                    "Key_Y", 
                                    "Y"));
    
    enumData.push_back(WuQMacroShortCutKeyEnum(Key_Z, 
                                    "Key_Z", 
                                    "Z"));
    
    enumData.push_back(WuQMacroShortCutKeyEnum(Key_0, 
                                    "Key_0", 
                                    "0"));
    
    enumData.push_back(WuQMacroShortCutKeyEnum(Key_1, 
                                    "Key_1", 
                                    "1"));
    
    enumData.push_back(WuQMacroShortCutKeyEnum(Key_2, 
                                    "Key_2", 
                                    "2"));
    
    enumData.push_back(WuQMacroShortCutKeyEnum(Key_3, 
                                    "Key_3", 
                                    "3"));
    
    enumData.push_back(WuQMacroShortCutKeyEnum(Key_4, 
                                    "Key_4", 
                                    "4"));
    
    enumData.push_back(WuQMacroShortCutKeyEnum(Key_5, 
                                    "Key_5", 
                                    "5"));
    
    enumData.push_back(WuQMacroShortCutKeyEnum(Key_6, 
                                    "Key_6", 
                                    "6"));
    
    enumData.push_back(WuQMacroShortCutKeyEnum(Key_7, 
                                    "Key_7", 
                                    "7"));
    
    enumData.push_back(WuQMacroShortCutKeyEnum(Key_8, 
                                    "Key_8", 
                                    "8"));
    
    enumData.push_back(WuQMacroShortCutKeyEnum(Key_9, 
                                    "Key_9", 
                                    "9"));
    
}

/**
 * Find the data for and enumerated value.
 * @param enumValue
 *     The enumerated value.
 * @return Pointer to data for this enumerated type
 * or NULL if no data for type or if type is invalid.
 */
const WuQMacroShortCutKeyEnum*
WuQMacroShortCutKeyEnum::findData(const Enum enumValue)
{
    if (initializedFlag == false) initialize();

    size_t num = enumData.size();
    for (size_t i = 0; i < num; i++) {
        const WuQMacroShortCutKeyEnum* d = &enumData[i];
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
WuQMacroShortCutKeyEnum::toName(Enum enumValue) {
    if (initializedFlag == false) initialize();
    
    const WuQMacroShortCutKeyEnum* enumInstance = findData(enumValue);
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
WuQMacroShortCutKeyEnum::Enum 
WuQMacroShortCutKeyEnum::fromName(const AString& name, bool* isValidOut)
{
    if (initializedFlag == false) initialize();
    
    bool validFlag = false;
    Enum enumValue = WuQMacroShortCutKeyEnum::enumData[0].enumValue;
    
    for (std::vector<WuQMacroShortCutKeyEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const WuQMacroShortCutKeyEnum& d = *iter;
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
        CaretAssertMessage(0, AString("Name " + name + "failed to match enumerated value for type WuQMacroShortCutKeyEnum"));
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
WuQMacroShortCutKeyEnum::toGuiName(Enum enumValue) {
    if (initializedFlag == false) initialize();
    
    const WuQMacroShortCutKeyEnum* enumInstance = findData(enumValue);
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
WuQMacroShortCutKeyEnum::Enum 
WuQMacroShortCutKeyEnum::fromGuiName(const AString& guiName, bool* isValidOut)
{
    if (initializedFlag == false) initialize();
    
    bool validFlag = false;
    Enum enumValue = WuQMacroShortCutKeyEnum::enumData[0].enumValue;
    
    for (std::vector<WuQMacroShortCutKeyEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const WuQMacroShortCutKeyEnum& d = *iter;
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
        CaretAssertMessage(0, AString("guiName " + guiName + "failed to match enumerated value for type WuQMacroShortCutKeyEnum"));
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
WuQMacroShortCutKeyEnum::toIntegerCode(Enum enumValue)
{
    if (initializedFlag == false) initialize();
    const WuQMacroShortCutKeyEnum* enumInstance = findData(enumValue);
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
WuQMacroShortCutKeyEnum::Enum
WuQMacroShortCutKeyEnum::fromIntegerCode(const int32_t integerCode, bool* isValidOut)
{
    if (initializedFlag == false) initialize();
    
    bool validFlag = false;
    Enum enumValue = WuQMacroShortCutKeyEnum::enumData[0].enumValue;
    
    for (std::vector<WuQMacroShortCutKeyEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const WuQMacroShortCutKeyEnum& enumInstance = *iter;
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
        CaretAssertMessage(0, AString("Integer code " + AString::number(integerCode) + "failed to match enumerated value for type WuQMacroShortCutKeyEnum"));
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
WuQMacroShortCutKeyEnum::getAllEnums(std::vector<WuQMacroShortCutKeyEnum::Enum>& allEnums)
{
    if (initializedFlag == false) initialize();
    
    allEnums.clear();
    
    for (std::vector<WuQMacroShortCutKeyEnum>::iterator iter = enumData.begin();
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
WuQMacroShortCutKeyEnum::getAllNames(std::vector<AString>& allNames, const bool isSorted)
{
    if (initializedFlag == false) initialize();
    
    allNames.clear();
    
    for (std::vector<WuQMacroShortCutKeyEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        allNames.push_back(WuQMacroShortCutKeyEnum::toName(iter->enumValue));
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
WuQMacroShortCutKeyEnum::getAllGuiNames(std::vector<AString>& allGuiNames, const bool isSorted)
{
    if (initializedFlag == false) initialize();
    
    allGuiNames.clear();
    
    for (std::vector<WuQMacroShortCutKeyEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        allGuiNames.push_back(WuQMacroShortCutKeyEnum::toGuiName(iter->enumValue));
    }
    
    if (isSorted) {
        std::sort(allGuiNames.begin(), allGuiNames.end());
    }
}

/**
 * Find the data type corresponding to a Qt::Key enum value.
 * If not valid, the none key (Key_None) is returned.
 *
 * @param qtKeyEnum
 *     The Qt::Key enum value
 * @return
 *     Enum for integer code.
 */
WuQMacroShortCutKeyEnum::Enum
WuQMacroShortCutKeyEnum::fromQtKeyEnum(const int32_t qtKeyEnum)
{
    if (initializedFlag == false) initialize();
    
    int32_t shortCutKeyInteger = -1;
    if ((qtKeyEnum >= Qt::Key_A)
        && (qtKeyEnum <= Qt::Key_Z)) {
        shortCutKeyInteger = (WuQMacroShortCutKeyEnum::toIntegerCode(Key_A)
                              + (qtKeyEnum - (int)Qt::Key_A));
    }
    else if ((qtKeyEnum >= Qt::Key_0)
             && (qtKeyEnum <= Qt::Key_9)) {
        shortCutKeyInteger = (WuQMacroShortCutKeyEnum::toIntegerCode(Key_0)
                              + (qtKeyEnum - (int)Qt::Key_0));
    }
    else {
        shortCutKeyInteger = WuQMacroShortCutKeyEnum::toIntegerCode(Key_None);
    }
    
    bool validFlag = false;
    const Enum enumValue = WuQMacroShortCutKeyEnum::fromIntegerCode(shortCutKeyInteger,
                                                                    &validFlag);
    CaretAssert(validFlag);
    
    return enumValue;
}
