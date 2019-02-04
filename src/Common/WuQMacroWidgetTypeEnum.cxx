
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
#define __WU_Q_MACRO_WIDGET_TYPE_ENUM_DECLARE__
#include "WuQMacroWidgetTypeEnum.h"
#undef __WU_Q_MACRO_WIDGET_TYPE_ENUM_DECLARE__

#include "CaretAssert.h"

using namespace caret;

    
/**
 * \class caret::WuQMacroWidgetTypeEnum
 * \brief Enumerated type for widgets in a macro command
 *
 * Using this enumerated type in the GUI with an EnumComboBoxTemplate
 * 
 * Header File (.h)
 *     Forward declare the data type:
 *         class EnumComboBoxTemplate;
 * 
 *     Declare the member:
 *         EnumComboBoxTemplate* m_WuQMacroWidgetTypeEnumComboBox;
 * 
 *     Declare a slot that is called when user changes selection
 *         private slots:
 *             void WuQMacroWidgetTypeEnumComboBoxItemActivated();
 * 
 * Implementation File (.cxx)
 *     Include the header files
 *         #include "EnumComboBoxTemplate.h"
 *         #include "WuQMacroWidgetTypeEnum.h"
 * 
 *     Instatiate:
 *         m_WuQMacroWidgetTypeEnumComboBox = new EnumComboBoxTemplate(this);
 *         m_WuQMacroWidgetTypeEnumComboBox->setup<WuQMacroWidgetTypeEnum,WuQMacroWidgetTypeEnum::Enum>();
 * 
 *     Get notified when the user changes the selection: 
 *         QObject::connect(m_WuQMacroWidgetTypeEnumComboBox, SIGNAL(itemActivated()),
 *                          this, SLOT(WuQMacroWidgetTypeEnumComboBoxItemActivated()));
 * 
 *     Update the selection:
 *         m_WuQMacroWidgetTypeEnumComboBox->setSelectedItem<WuQMacroWidgetTypeEnum,WuQMacroWidgetTypeEnum::Enum>(NEW_VALUE);
 * 
 *     Read the selection:
 *         const WuQMacroWidgetTypeEnum::Enum VARIABLE = m_WuQMacroWidgetTypeEnumComboBox->getSelectedItem<WuQMacroWidgetTypeEnum,WuQMacroWidgetTypeEnum::Enum>();
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
WuQMacroWidgetTypeEnum::WuQMacroWidgetTypeEnum(const Enum enumValue,
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
WuQMacroWidgetTypeEnum::~WuQMacroWidgetTypeEnum()
{
}

/**
 * Initialize the enumerated metadata.
 */
void
WuQMacroWidgetTypeEnum::initialize()
{
    if (initializedFlag) {
        return;
    }
    initializedFlag = true;

    enumData.push_back(WuQMacroWidgetTypeEnum(INVALID,
                                    "INVALID", 
                                    "Invalid"));
    
    enumData.push_back(WuQMacroWidgetTypeEnum(ACTION,
                                    "ACTION", 
                                    "QAction"));
    
    enumData.push_back(WuQMacroWidgetTypeEnum(ACTION_CHECKABLE,
                                             "ACTION_CHECKABLE",
                                             "QActionCheckable"));
    
    enumData.push_back(WuQMacroWidgetTypeEnum(ACTION_GROUP,
                                              "ACTION_GROUP",
                                              "QActionGroup"));
    
    enumData.push_back(WuQMacroWidgetTypeEnum(BUTTON_GROUP,
                                              "BUTTON_GROUP",
                                              "QButtonGroup"));
    
    enumData.push_back(WuQMacroWidgetTypeEnum(CHECK_BOX,
                                              "CHECK_BOX",
                                              "QCheckBox"));
    
    enumData.push_back(WuQMacroWidgetTypeEnum(COMBO_BOX,
                                    "COMBO_BOX", 
                                    "QComboBox"));
    
    enumData.push_back(WuQMacroWidgetTypeEnum(DOUBLE_SPIN_BOX,
                                    "DOUBLE_SPIN_BOX", 
                                    "QDoubleSpinBox"));
    
    enumData.push_back(WuQMacroWidgetTypeEnum(LINE_EDIT,
                                    "LINE_EDIT", 
                                    "QLineEdit"));
    
    enumData.push_back(WuQMacroWidgetTypeEnum(LIST_WIDGET,
                                    "LIST_WIDGET", 
                                    "QListWidget"));
    
    enumData.push_back(WuQMacroWidgetTypeEnum(MENU,
                                    "MENU", 
                                    "QMenu"));
    
    enumData.push_back(WuQMacroWidgetTypeEnum(PUSH_BUTTON,
                                    "PUSH_BUTTON", 
                                    "QPushButton"));
    
    enumData.push_back(WuQMacroWidgetTypeEnum(PUSH_BUTTON_CHECKABLE,
                                             "PUSH_BUTTON_CHECKABLE",
                                             "QPushButtonCheckable"));
    
    enumData.push_back(WuQMacroWidgetTypeEnum(RADIO_BUTTON,
                                    "RADIO_BUTTON", 
                                    "QRadioButton"));
    
    enumData.push_back(WuQMacroWidgetTypeEnum(SLIDER,
                                    "SLIDER", 
                                    "QSlider"));
    
    enumData.push_back(WuQMacroWidgetTypeEnum(SPIN_BOX,
                                    "SPIN_BOX", 
                                    "QSpinBox"));
    
    enumData.push_back(WuQMacroWidgetTypeEnum(TAB_BAR,
                                    "TAB_BAR", 
                                    "QTabBar"));
    
    enumData.push_back(WuQMacroWidgetTypeEnum(TAB_WIDGET,
                                    "TAB_WIDGET", 
                                    "QTabWidget"));
    
    enumData.push_back(WuQMacroWidgetTypeEnum(TOOL_BUTTON,
                                    "TOOL_BUTTON", 
                                    "QToolButton"));
    
    enumData.push_back(WuQMacroWidgetTypeEnum(TOOL_BUTTON_CHECKABLE,
                                             "TOOL_BUTTON_CHECKABLE",
                                             "QToolButtonCheckable"));
}

/**
 * Find the data for and enumerated value.
 * @param enumValue
 *     The enumerated value.
 * @return Pointer to data for this enumerated type
 * or NULL if no data for type or if type is invalid.
 */
const WuQMacroWidgetTypeEnum*
WuQMacroWidgetTypeEnum::findData(const Enum enumValue)
{
    if (initializedFlag == false) initialize();

    size_t num = enumData.size();
    for (size_t i = 0; i < num; i++) {
        const WuQMacroWidgetTypeEnum* d = &enumData[i];
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
WuQMacroWidgetTypeEnum::toName(Enum enumValue) {
    if (initializedFlag == false) initialize();
    
    const WuQMacroWidgetTypeEnum* enumInstance = findData(enumValue);
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
WuQMacroWidgetTypeEnum::Enum
WuQMacroWidgetTypeEnum::fromName(const AString& name, bool* isValidOut)
{
    if (initializedFlag == false) initialize();
    
    bool validFlag = false;
    Enum enumValue = WuQMacroWidgetTypeEnum::enumData[0].enumValue;
    
    for (std::vector<WuQMacroWidgetTypeEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const WuQMacroWidgetTypeEnum& d = *iter;
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
        CaretAssertMessage(0, AString("Name " + name + "failed to match enumerated value for type WuQMacroWidgetTypeEnum"));
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
WuQMacroWidgetTypeEnum::toGuiName(Enum enumValue) {
    if (initializedFlag == false) initialize();
    
    const WuQMacroWidgetTypeEnum* enumInstance = findData(enumValue);
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
WuQMacroWidgetTypeEnum::Enum
WuQMacroWidgetTypeEnum::fromGuiName(const AString& guiNameIn, bool* isValidOut)
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
    Enum enumValue = WuQMacroWidgetTypeEnum::enumData[0].enumValue;
    
    for (std::vector<WuQMacroWidgetTypeEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const WuQMacroWidgetTypeEnum& d = *iter;
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
        CaretAssertMessage(0, AString("guiName " + guiName + "failed to match enumerated value for type WuQMacroWidgetTypeEnum"));
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
WuQMacroWidgetTypeEnum::toIntegerCode(Enum enumValue)
{
    if (initializedFlag == false) initialize();
    const WuQMacroWidgetTypeEnum* enumInstance = findData(enumValue);
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
WuQMacroWidgetTypeEnum::Enum
WuQMacroWidgetTypeEnum::fromIntegerCode(const int32_t integerCode, bool* isValidOut)
{
    if (initializedFlag == false) initialize();
    
    bool validFlag = false;
    Enum enumValue = WuQMacroWidgetTypeEnum::enumData[0].enumValue;
    
    for (std::vector<WuQMacroWidgetTypeEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const WuQMacroWidgetTypeEnum& enumInstance = *iter;
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
        CaretAssertMessage(0, AString("Integer code " + AString::number(integerCode) + "failed to match enumerated value for type WuQMacroWidgetTypeEnum"));
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
WuQMacroWidgetTypeEnum::getAllEnums(std::vector<WuQMacroWidgetTypeEnum::Enum>& allEnums)
{
    if (initializedFlag == false) initialize();
    
    allEnums.clear();
    
    for (std::vector<WuQMacroWidgetTypeEnum>::iterator iter = enumData.begin();
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
WuQMacroWidgetTypeEnum::getAllNames(std::vector<AString>& allNames, const bool isSorted)
{
    if (initializedFlag == false) initialize();
    
    allNames.clear();
    
    for (std::vector<WuQMacroWidgetTypeEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        allNames.push_back(WuQMacroWidgetTypeEnum::toName(iter->enumValue));
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
WuQMacroWidgetTypeEnum::getAllGuiNames(std::vector<AString>& allGuiNames, const bool isSorted)
{
    if (initializedFlag == false) initialize();
    
    allGuiNames.clear();
    
    for (std::vector<WuQMacroWidgetTypeEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        allGuiNames.push_back(WuQMacroWidgetTypeEnum::toGuiName(iter->enumValue));
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
WuQMacroWidgetTypeEnum::addWidgetClassNameAlias(const QString& widgetName,
                                                const QString& aliasWidgetName)
{
    s_widgetClassNameAliases.push_back(std::make_pair(widgetName,
                                                      aliasWidgetName));
}


