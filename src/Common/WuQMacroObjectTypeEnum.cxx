
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
#define __WU_Q_MACRO_OBJECT_TYPE_ENUM_DECLARE__
#include "WuQMacroObjectTypeEnum.h"
#undef __WU_Q_MACRO_OBJECT_TYPE_ENUM_DECLARE__

#include "CaretAssert.h"

using namespace caret;

    
/**
 * \class caret::WuQMacroObjectTypeEnum 
 * \brief Enumerated type for specific type of object in macro
 *
 * Using this enumerated type in the GUI with an EnumComboBoxTemplate
 * 
 * Header File (.h)
 *     Forward declare the data type:
 *         class EnumComboBoxTemplate;
 * 
 *     Declare the member:
 *         EnumComboBoxTemplate* m_wuQMacroObjectTypeEnumComboBox;
 * 
 *     Declare a slot that is called when user changes selection
 *         private slots:
 *             void wuQMacroObjectTypeEnumComboBoxItemActivated();
 * 
 * Implementation File (.cxx)
 *     Include the header files
 *         #include "EnumComboBoxTemplate.h"
 *         #include "WuQMacroObjectTypeEnum.h"
 * 
 *     Instatiate:
 *         m_wuQMacroObjectTypeEnumComboBox = new EnumComboBoxTemplate(this);
 *         m_wuQMacroObjectTypeEnumComboBox->setup<WuQMacroObjectTypeEnum,WuQMacroObjectTypeEnum::Enum>();
 * 
 *     Get notified when the user changes the selection: 
 *         QObject::connect(m_wuQMacroObjectTypeEnumComboBox, SIGNAL(itemActivated()),
 *                          this, SLOT(wuQMacroObjectTypeEnumComboBoxItemActivated()));
 * 
 *     Update the selection:
 *         m_wuQMacroObjectTypeEnumComboBox->setSelectedItem<WuQMacroObjectTypeEnum,WuQMacroObjectTypeEnum::Enum>(NEW_VALUE);
 * 
 *     Read the selection:
 *         const WuQMacroObjectTypeEnum::Enum VARIABLE = m_wuQMacroObjectTypeEnumComboBox->getSelectedItem<WuQMacroObjectTypeEnum,WuQMacroObjectTypeEnum::Enum>();
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
WuQMacroObjectTypeEnum::WuQMacroObjectTypeEnum(const Enum enumValue,
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
WuQMacroObjectTypeEnum::~WuQMacroObjectTypeEnum()
{
}

/**
 * Initialize the enumerated metadata.
 */
void
WuQMacroObjectTypeEnum::initialize()
{
    if (initializedFlag) {
        return;
    }
    initializedFlag = true;

    enumData.push_back(WuQMacroObjectTypeEnum(INVALID, 
                                    "INVALID", 
                                    "Invalid"));
    
    enumData.push_back(WuQMacroObjectTypeEnum(ACTION, 
                                    "ACTION", 
                                    "QAction"));
    
    enumData.push_back(WuQMacroObjectTypeEnum(CHECK_BOX, 
                                    "CHECK_BOX", 
                                    "QCheckBox"));
    
    enumData.push_back(WuQMacroObjectTypeEnum(COMBO_BOX, 
                                    "COMBO_BOX", 
                                    "QComboBox"));
    
    enumData.push_back(WuQMacroObjectTypeEnum(DOUBLE_SPIN_BOX, 
                                    "DOUBLE_SPIN_BOX", 
                                    "QDoubleSpinBox"));
    
    enumData.push_back(WuQMacroObjectTypeEnum(LINE_EDIT, 
                                    "LINE_EDIT", 
                                    "QLineEdit"));
    
    enumData.push_back(WuQMacroObjectTypeEnum(LIST_WIDGET, 
                                    "LIST_WIDGET", 
                                    "QListWidget"));
    
    enumData.push_back(WuQMacroObjectTypeEnum(MENU, 
                                    "MENU", 
                                    "QMenu"));
    
    enumData.push_back(WuQMacroObjectTypeEnum(MOUSE_USER_EVENT,
                                              "MOUSE_USER_EVENT",
                                              "QMouseEvent"));
    
    enumData.push_back(WuQMacroObjectTypeEnum(PUSH_BUTTON,
                                    "PUSH_BUTTON", 
                                    "QPushButton"));
    
    enumData.push_back(WuQMacroObjectTypeEnum(RADIO_BUTTON, 
                                    "RADIO_BUTTON", 
                                    "QRadioButton"));
    
    enumData.push_back(WuQMacroObjectTypeEnum(SLIDER, 
                                    "SLIDER", 
                                    "QSlider"));
    
    enumData.push_back(WuQMacroObjectTypeEnum(SPIN_BOX, 
                                    "SPIN_BOX", 
                                    "QSpinBox"));
    
    enumData.push_back(WuQMacroObjectTypeEnum(TAB_BAR, 
                                    "TAB_BAR", 
                                    "QTabBar"));
    
    enumData.push_back(WuQMacroObjectTypeEnum(TAB_WIDGET, 
                                    "TAB_WIDGET", 
                                    "QTabWidget"));
    
    enumData.push_back(WuQMacroObjectTypeEnum(TOOL_BUTTON, 
                                    "TOOL_BUTTON", 
                                    "QToolButton"));
}

/**
 * Find the data for and enumerated value.
 * @param enumValue
 *     The enumerated value.
 * @return Pointer to data for this enumerated type
 * or NULL if no data for type or if type is invalid.
 */
const WuQMacroObjectTypeEnum*
WuQMacroObjectTypeEnum::findData(const Enum enumValue)
{
    if (initializedFlag == false) initialize();

    size_t num = enumData.size();
    for (size_t i = 0; i < num; i++) {
        const WuQMacroObjectTypeEnum* d = &enumData[i];
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
WuQMacroObjectTypeEnum::toName(Enum enumValue) {
    if (initializedFlag == false) initialize();
    
    const WuQMacroObjectTypeEnum* enumInstance = findData(enumValue);
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
WuQMacroObjectTypeEnum::Enum 
WuQMacroObjectTypeEnum::fromName(const AString& name, bool* isValidOut)
{
    if (initializedFlag == false) initialize();
    
    bool validFlag = false;
    Enum enumValue = WuQMacroObjectTypeEnum::enumData[0].enumValue;
    
    for (std::vector<WuQMacroObjectTypeEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const WuQMacroObjectTypeEnum& d = *iter;
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
        CaretAssertMessage(0, AString("Name " + name + "failed to match enumerated value for type WuQMacroObjectTypeEnum"));
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
WuQMacroObjectTypeEnum::toGuiName(Enum enumValue) {
    if (initializedFlag == false) initialize();
    
    const WuQMacroObjectTypeEnum* enumInstance = findData(enumValue);
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
WuQMacroObjectTypeEnum::Enum 
WuQMacroObjectTypeEnum::fromGuiName(const AString& guiName, bool* isValidOut)
{
    if (initializedFlag == false) initialize();
    
    bool validFlag = false;
    Enum enumValue = WuQMacroObjectTypeEnum::enumData[0].enumValue;
    
    for (std::vector<WuQMacroObjectTypeEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const WuQMacroObjectTypeEnum& d = *iter;
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
        CaretAssertMessage(0, AString("guiName " + guiName + "failed to match enumerated value for type WuQMacroObjectTypeEnum"));
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
WuQMacroObjectTypeEnum::toIntegerCode(Enum enumValue)
{
    if (initializedFlag == false) initialize();
    const WuQMacroObjectTypeEnum* enumInstance = findData(enumValue);
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
WuQMacroObjectTypeEnum::Enum
WuQMacroObjectTypeEnum::fromIntegerCode(const int32_t integerCode, bool* isValidOut)
{
    if (initializedFlag == false) initialize();
    
    bool validFlag = false;
    Enum enumValue = WuQMacroObjectTypeEnum::enumData[0].enumValue;
    
    for (std::vector<WuQMacroObjectTypeEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const WuQMacroObjectTypeEnum& enumInstance = *iter;
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
        CaretAssertMessage(0, AString("Integer code " + AString::number(integerCode) + "failed to match enumerated value for type WuQMacroObjectTypeEnum"));
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
WuQMacroObjectTypeEnum::getAllEnums(std::vector<WuQMacroObjectTypeEnum::Enum>& allEnums)
{
    if (initializedFlag == false) initialize();
    
    allEnums.clear();
    
    for (std::vector<WuQMacroObjectTypeEnum>::iterator iter = enumData.begin();
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
WuQMacroObjectTypeEnum::getAllNames(std::vector<AString>& allNames, const bool isSorted)
{
    if (initializedFlag == false) initialize();
    
    allNames.clear();
    
    for (std::vector<WuQMacroObjectTypeEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        allNames.push_back(WuQMacroObjectTypeEnum::toName(iter->enumValue));
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
WuQMacroObjectTypeEnum::getAllGuiNames(std::vector<AString>& allGuiNames, const bool isSorted)
{
    if (initializedFlag == false) initialize();
    
    allGuiNames.clear();
    
    for (std::vector<WuQMacroObjectTypeEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        allGuiNames.push_back(WuQMacroObjectTypeEnum::toGuiName(iter->enumValue));
    }
    
    if (isSorted) {
        std::sort(allGuiNames.begin(), allGuiNames.end());
    }
}

