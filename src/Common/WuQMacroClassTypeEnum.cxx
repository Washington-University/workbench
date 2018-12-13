
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
#define __WU_Q_MACRO_CLASS_TYPE_ENUM_DECLARE__
#include "WuQMacroClassTypeEnum.h"
#undef __WU_Q_MACRO_CLASS_TYPE_ENUM_DECLARE__

#include "CaretAssert.h"

using namespace caret;

    
/**
 * \class caret::WuQMacroClassTypeEnum 
 * \brief Enumerated type for specific type of object in macro
 *
 * Using this enumerated type in the GUI with an EnumComboBoxTemplate
 * 
 * Header File (.h)
 *     Forward declare the data type:
 *         class EnumComboBoxTemplate;
 * 
 *     Declare the member:
 *         EnumComboBoxTemplate* m_WuQMacroClassTypeEnumComboBox;
 * 
 *     Declare a slot that is called when user changes selection
 *         private slots:
 *             void WuQMacroClassTypeEnumComboBoxItemActivated();
 * 
 * Implementation File (.cxx)
 *     Include the header files
 *         #include "EnumComboBoxTemplate.h"
 *         #include "WuQMacroClassTypeEnum.h"
 * 
 *     Instatiate:
 *         m_WuQMacroClassTypeEnumComboBox = new EnumComboBoxTemplate(this);
 *         m_WuQMacroClassTypeEnumComboBox->setup<WuQMacroClassTypeEnum,WuQMacroClassTypeEnum::Enum>();
 * 
 *     Get notified when the user changes the selection: 
 *         QObject::connect(m_WuQMacroClassTypeEnumComboBox, SIGNAL(itemActivated()),
 *                          this, SLOT(WuQMacroClassTypeEnumComboBoxItemActivated()));
 * 
 *     Update the selection:
 *         m_WuQMacroClassTypeEnumComboBox->setSelectedItem<WuQMacroClassTypeEnum,WuQMacroClassTypeEnum::Enum>(NEW_VALUE);
 * 
 *     Read the selection:
 *         const WuQMacroClassTypeEnum::Enum VARIABLE = m_WuQMacroClassTypeEnumComboBox->getSelectedItem<WuQMacroClassTypeEnum,WuQMacroClassTypeEnum::Enum>();
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
WuQMacroClassTypeEnum::WuQMacroClassTypeEnum(const Enum enumValue,
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
WuQMacroClassTypeEnum::~WuQMacroClassTypeEnum()
{
}

/**
 * Initialize the enumerated metadata.
 */
void
WuQMacroClassTypeEnum::initialize()
{
    if (initializedFlag) {
        return;
    }
    initializedFlag = true;

    enumData.push_back(WuQMacroClassTypeEnum(INVALID, 
                                    "INVALID", 
                                    "Invalid"));
    
    enumData.push_back(WuQMacroClassTypeEnum(ACTION, 
                                    "ACTION", 
                                    "QAction"));
    
    enumData.push_back(WuQMacroClassTypeEnum(ACTION_GROUP,
                                              "ACTION_GROUP",
                                              "QActionGroup"));
    
    enumData.push_back(WuQMacroClassTypeEnum(BUTTON_GROUP,
                                              "BUTTON_GROUP",
                                              "QButtonGroup"));
    
    enumData.push_back(WuQMacroClassTypeEnum(CHECK_BOX,
                                              "CHECK_BOX",
                                              "QCheckBox"));
    
    enumData.push_back(WuQMacroClassTypeEnum(COMBO_BOX,
                                    "COMBO_BOX", 
                                    "QComboBox"));
    
    enumData.push_back(WuQMacroClassTypeEnum(DOUBLE_SPIN_BOX, 
                                    "DOUBLE_SPIN_BOX", 
                                    "QDoubleSpinBox"));
    
    enumData.push_back(WuQMacroClassTypeEnum(LINE_EDIT, 
                                    "LINE_EDIT", 
                                    "QLineEdit"));
    
    enumData.push_back(WuQMacroClassTypeEnum(LIST_WIDGET, 
                                    "LIST_WIDGET", 
                                    "QListWidget"));
    
    enumData.push_back(WuQMacroClassTypeEnum(MENU, 
                                    "MENU", 
                                    "QMenu"));
    
    enumData.push_back(WuQMacroClassTypeEnum(MOUSE_USER_EVENT,
                                              "MOUSE_USER_EVENT",
                                              "QMouseEvent"));
    
    enumData.push_back(WuQMacroClassTypeEnum(PUSH_BUTTON,
                                    "PUSH_BUTTON", 
                                    "QPushButton"));
    
    enumData.push_back(WuQMacroClassTypeEnum(RADIO_BUTTON, 
                                    "RADIO_BUTTON", 
                                    "QRadioButton"));
    
    enumData.push_back(WuQMacroClassTypeEnum(SLIDER, 
                                    "SLIDER", 
                                    "QSlider"));
    
    enumData.push_back(WuQMacroClassTypeEnum(SPIN_BOX, 
                                    "SPIN_BOX", 
                                    "QSpinBox"));
    
    enumData.push_back(WuQMacroClassTypeEnum(TAB_BAR, 
                                    "TAB_BAR", 
                                    "QTabBar"));
    
    enumData.push_back(WuQMacroClassTypeEnum(TAB_WIDGET, 
                                    "TAB_WIDGET", 
                                    "QTabWidget"));
    
    enumData.push_back(WuQMacroClassTypeEnum(TOOL_BUTTON, 
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
const WuQMacroClassTypeEnum*
WuQMacroClassTypeEnum::findData(const Enum enumValue)
{
    if (initializedFlag == false) initialize();

    size_t num = enumData.size();
    for (size_t i = 0; i < num; i++) {
        const WuQMacroClassTypeEnum* d = &enumData[i];
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
WuQMacroClassTypeEnum::toName(Enum enumValue) {
    if (initializedFlag == false) initialize();
    
    const WuQMacroClassTypeEnum* enumInstance = findData(enumValue);
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
WuQMacroClassTypeEnum::Enum 
WuQMacroClassTypeEnum::fromName(const AString& name, bool* isValidOut)
{
    if (initializedFlag == false) initialize();
    
    bool validFlag = false;
    Enum enumValue = WuQMacroClassTypeEnum::enumData[0].enumValue;
    
    for (std::vector<WuQMacroClassTypeEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const WuQMacroClassTypeEnum& d = *iter;
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
        CaretAssertMessage(0, AString("Name " + name + "failed to match enumerated value for type WuQMacroClassTypeEnum"));
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
WuQMacroClassTypeEnum::toGuiName(Enum enumValue) {
    if (initializedFlag == false) initialize();
    
    const WuQMacroClassTypeEnum* enumInstance = findData(enumValue);
    return enumInstance->guiName;
}

/**
 * Get an enumerated value corresponding to its GUI name.
 * @param guiNameIn
 *     Name of enumerated value.
 * @param isValidOut 
 *     If not NULL, it is set indicating that a
 *     enum value exists for the input name.
 * @return 
 *     Enumerated value.
 */
WuQMacroClassTypeEnum::Enum 
WuQMacroClassTypeEnum::fromGuiName(const AString& guiNameIn, bool* isValidOut)
{
    if (initializedFlag == false) initialize();
    
    /*
     * Some widgets are extended from Qt Widgets and end with the Qt Name
     */
    AString guiName(guiNameIn);
    for (auto alias : s_widgetClassNameAliases) {
        if (alias.second == guiName) {
            guiName = alias.first;
            break;
        }
    }
    
    bool validFlag = false;
    Enum enumValue = WuQMacroClassTypeEnum::enumData[0].enumValue;
    
    for (std::vector<WuQMacroClassTypeEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const WuQMacroClassTypeEnum& d = *iter;
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
        CaretAssertMessage(0, AString("guiName " + guiName + "failed to match enumerated value for type WuQMacroClassTypeEnum"));
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
WuQMacroClassTypeEnum::toIntegerCode(Enum enumValue)
{
    if (initializedFlag == false) initialize();
    const WuQMacroClassTypeEnum* enumInstance = findData(enumValue);
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
WuQMacroClassTypeEnum::Enum
WuQMacroClassTypeEnum::fromIntegerCode(const int32_t integerCode, bool* isValidOut)
{
    if (initializedFlag == false) initialize();
    
    bool validFlag = false;
    Enum enumValue = WuQMacroClassTypeEnum::enumData[0].enumValue;
    
    for (std::vector<WuQMacroClassTypeEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const WuQMacroClassTypeEnum& enumInstance = *iter;
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
        CaretAssertMessage(0, AString("Integer code " + AString::number(integerCode) + "failed to match enumerated value for type WuQMacroClassTypeEnum"));
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
WuQMacroClassTypeEnum::getAllEnums(std::vector<WuQMacroClassTypeEnum::Enum>& allEnums)
{
    if (initializedFlag == false) initialize();
    
    allEnums.clear();
    
    for (std::vector<WuQMacroClassTypeEnum>::iterator iter = enumData.begin();
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
WuQMacroClassTypeEnum::getAllNames(std::vector<AString>& allNames, const bool isSorted)
{
    if (initializedFlag == false) initialize();
    
    allNames.clear();
    
    for (std::vector<WuQMacroClassTypeEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        allNames.push_back(WuQMacroClassTypeEnum::toName(iter->enumValue));
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
WuQMacroClassTypeEnum::getAllGuiNames(std::vector<AString>& allGuiNames, const bool isSorted)
{
    if (initializedFlag == false) initialize();
    
    allGuiNames.clear();
    
    for (std::vector<WuQMacroClassTypeEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        allGuiNames.push_back(WuQMacroClassTypeEnum::toGuiName(iter->enumValue));
    }
    
    if (isSorted) {
        std::sort(allGuiNames.begin(), allGuiNames.end());
    }
}

/**
 * Add a widget alias for class names
 *
 * @param widgetName
 *     The name of a Qt widget class
 * @param aliasWidgetName
 *     Name of class that is derived from Qt widget with name 'widgetName'
 */
void
WuQMacroClassTypeEnum::addWidgetClassNameAlias(const QString& widgetName,
                                                const QString& aliasWidgetName)
{
    s_widgetClassNameAliases.push_back(std::make_pair(widgetName,
                                                      aliasWidgetName));
}


