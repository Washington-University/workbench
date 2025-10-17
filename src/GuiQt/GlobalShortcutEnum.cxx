
/*LICENSE_START*/
/*
 *  Copyright (C) 2025 Washington University School of Medicine
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
#define __GLOBAL_SHORTCUT_ENUM_DECLARE__
#include "GlobalShortcutEnum.h"
#undef __GLOBAL_SHORTCUT_ENUM_DECLARE__

#include "CaretAssert.h"

using namespace caret;

    
/**
 * \class caret::GlobalShortcutEnum 
 * \brief 
 *
 *
 * Using this enumerated type in the GUI with an EnumComboBoxTemplate
 * 
 * Header File (.h)
 *     Forward declare the data type:
 *         class EnumComboBoxTemplate;
 * 
 *     Declare the member:
 *         EnumComboBoxTemplate* m_globalShortcutEnumComboBox;
 * 
 *     Declare a slot that is called when user changes selection
 *         private slots:
 *             void globalShortcutEnumComboBoxItemActivated();
 * 
 * Implementation File (.cxx)
 *     Include the header files
 *         #include "EnumComboBoxTemplate.h"
 *         #include "GlobalShortcutEnum.h"
 * 
 *     Instatiate:
 *         m_globalShortcutEnumComboBox = new EnumComboBoxTemplate(this);
 *         m_globalShortcutEnumComboBox->setup<GlobalShortcutEnum,GlobalShortcutEnum::Enum>();
 * 
 *     Get notified when the user changes the selection: 
 *         QObject::connect(m_globalShortcutEnumComboBox, SIGNAL(itemActivated()),
 *                          this, SLOT(globalShortcutEnumComboBoxItemActivated()));
 * 
 *     Update the selection:
 *         m_globalShortcutEnumComboBox->setSelectedItem<GlobalShortcutEnum,GlobalShortcutEnum::Enum>(NEW_VALUE);
 * 
 *     Read the selection:
 *         const GlobalShortcutEnum::Enum VARIABLE = m_globalShortcutEnumComboBox->getSelectedItem<GlobalShortcutEnum,GlobalShortcutEnum::Enum>();
 * 
 */

/*
switch (value) {
    case GlobalShortcutEnum::OPEN_FILE:
        break;
    case GlobalShortcutEnum::OPEN_LOCATION:
        break;
    case GlobalShortcutEnum::OPEN_QUICKLY:
        break;
    case GlobalShortcutEnum::OPEN_RECENT:
        break;
    case GlobalShortcutEnum::QUIT:
        break;
    case GlobalShortcutEnum::SAVE:
        break;
}
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
GlobalShortcutEnum::GlobalShortcutEnum(const Enum enumValue,
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
GlobalShortcutEnum::~GlobalShortcutEnum()
{
}

/**
 * Initialize the enumerated metadata.
 */
void
GlobalShortcutEnum::initialize()
{
    if (initializedFlag) {
        return;
    }
    initializedFlag = true;

    enumData.push_back(GlobalShortcutEnum(NEW_WINDOW,
                                    "NEW_WINDOW",
                                    "New Window"));
    
    enumData.push_back(GlobalShortcutEnum(OPEN_FILE,
                                          "OPEN_FILE",
                                          "Open File"));
    
    enumData.push_back(GlobalShortcutEnum(OPEN_LOCATION,
                                    "OPEN_LOCATION", 
                                    "Open Location"));
    
    enumData.push_back(GlobalShortcutEnum(OPEN_QUICKLY, 
                                    "OPEN_QUICKLY", 
                                    "Open Quickly"));
    
    enumData.push_back(GlobalShortcutEnum(OPEN_RECENT, 
                                    "OPEN_RECENT", 
                                    "Open Recent"));
    
    enumData.push_back(GlobalShortcutEnum(QUIT, 
                                    "QUIT", 
                                    "Quit"));
    
    enumData.push_back(GlobalShortcutEnum(SAVE, 
                                    "SAVE", 
                                    "Save"));
    
}

/**
 * Find the data for and enumerated value.
 * @param enumValue
 *     The enumerated value.
 * @return Pointer to data for this enumerated type
 * or NULL if no data for type or if type is invalid.
 */
const GlobalShortcutEnum*
GlobalShortcutEnum::findData(const Enum enumValue)
{
    if (initializedFlag == false) initialize();

    size_t num = enumData.size();
    for (size_t i = 0; i < num; i++) {
        const GlobalShortcutEnum* d = &enumData[i];
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
GlobalShortcutEnum::toName(Enum enumValue) {
    if (initializedFlag == false) initialize();
    
    const GlobalShortcutEnum* enumInstance = findData(enumValue);
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
GlobalShortcutEnum::Enum 
GlobalShortcutEnum::fromName(const AString& name, bool* isValidOut)
{
    if (initializedFlag == false) initialize();
    
    bool validFlag = false;
    Enum enumValue = GlobalShortcutEnum::enumData[0].enumValue;
    
    for (std::vector<GlobalShortcutEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const GlobalShortcutEnum& d = *iter;
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
        CaretAssertMessage(0, AString("Name " + name + " failed to match enumerated value for type GlobalShortcutEnum"));
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
GlobalShortcutEnum::toGuiName(Enum enumValue) {
    if (initializedFlag == false) initialize();
    
    const GlobalShortcutEnum* enumInstance = findData(enumValue);
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
GlobalShortcutEnum::Enum 
GlobalShortcutEnum::fromGuiName(const AString& guiName, bool* isValidOut)
{
    if (initializedFlag == false) initialize();
    
    bool validFlag = false;
    Enum enumValue = GlobalShortcutEnum::enumData[0].enumValue;
    
    for (std::vector<GlobalShortcutEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const GlobalShortcutEnum& d = *iter;
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
        CaretAssertMessage(0, AString("guiName " + guiName + " failed to match enumerated value for type GlobalShortcutEnum"));
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
GlobalShortcutEnum::toIntegerCode(Enum enumValue)
{
    if (initializedFlag == false) initialize();
    const GlobalShortcutEnum* enumInstance = findData(enumValue);
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
GlobalShortcutEnum::Enum
GlobalShortcutEnum::fromIntegerCode(const int32_t integerCode, bool* isValidOut)
{
    if (initializedFlag == false) initialize();
    
    bool validFlag = false;
    Enum enumValue = GlobalShortcutEnum::enumData[0].enumValue;
    
    for (std::vector<GlobalShortcutEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const GlobalShortcutEnum& enumInstance = *iter;
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
        CaretAssertMessage(0, AString("Integer code " + AString::number(integerCode) + " failed to match enumerated value for type GlobalShortcutEnum"));
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
GlobalShortcutEnum::getAllEnums(std::vector<GlobalShortcutEnum::Enum>& allEnums)
{
    if (initializedFlag == false) initialize();
    
    allEnums.clear();
    
    for (std::vector<GlobalShortcutEnum>::iterator iter = enumData.begin();
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
GlobalShortcutEnum::getAllNames(std::vector<AString>& allNames, const bool isSorted)
{
    if (initializedFlag == false) initialize();
    
    allNames.clear();
    
    for (std::vector<GlobalShortcutEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        allNames.push_back(GlobalShortcutEnum::toName(iter->enumValue));
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
GlobalShortcutEnum::getAllGuiNames(std::vector<AString>& allGuiNames, const bool isSorted)
{
    if (initializedFlag == false) initialize();
    
    allGuiNames.clear();
    
    for (std::vector<GlobalShortcutEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        allGuiNames.push_back(GlobalShortcutEnum::toGuiName(iter->enumValue));
    }
    
    if (isSorted) {
        std::sort(allGuiNames.begin(), allGuiNames.end());
    }
}

