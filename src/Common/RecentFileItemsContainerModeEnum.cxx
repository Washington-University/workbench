
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
#define __RECENT_FILE_ITEMS_CONTAINER_MODE_ENUM_DECLARE__
#include "RecentFileItemsContainerModeEnum.h"
#undef __RECENT_FILE_ITEMS_CONTAINER_MODE_ENUM_DECLARE__

#include "CaretAssert.h"

using namespace caret;

    
/**
 * \class caret::RecentFileItemsContainerModeEnum
 * \brief Enumerated type for types of recently opened files
 *
 * Using this enumerated type in the GUI with an EnumComboBoxTemplate
 * 
 * Header File (.h)
 *     Forward declare the data type:
 *         class EnumComboBoxTemplate;
 * 
 *     Declare the member:
 *         EnumComboBoxTemplate* m_recentFilesModeEnumComboBox;
 * 
 *     Declare a slot that is called when user changes selection
 *         private slots:
 *             void recentFilesModeEnumComboBoxItemActivated();
 * 
 * Implementation File (.cxx)
 *     Include the header files
 *         #include "EnumComboBoxTemplate.h"
 *         #include "RecentFileItemsContainerModeEnum.h"
 * 
 *     Instatiate:
 *         m_recentFilesModeEnumComboBox = new EnumComboBoxTemplate(this);
 *         m_recentFilesModeEnumComboBox->setup<RecentFileItemsContainerModeEnum,RecentFileItemsContainerModeEnum::Enum>();
 * 
 *     Get notified when the user changes the selection: 
 *         QObject::connect(m_recentFilesModeEnumComboBox, SIGNAL(itemActivated()),
 *                          this, SLOT(recentFilesModeEnumComboBoxItemActivated()));
 * 
 *     Update the selection:
 *         m_recentFilesModeEnumComboBox->setSelectedItem<RecentFileItemsContainerModeEnum,RecentFileItemsContainerModeEnum::Enum>(NEW_VALUE);
 * 
 *     Read the selection:
 *         const RecentFileItemsContainerModeEnum::Enum VARIABLE = m_recentFilesModeEnumComboBox->getSelectedItem<RecentFileItemsContainerModeEnum,RecentFileItemsContainerModeEnum::Enum>();
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
RecentFileItemsContainerModeEnum::RecentFileItemsContainerModeEnum(const Enum enumValue,
                                         const AString& name,
                                         const AString& guiName,
                                         const AString& guiButtonName)
{
    this->enumValue = enumValue;
    this->integerCode = integerCodeCounter++;
    this->name = name;
    this->guiName = guiName;
    this->guiButtonName = guiButtonName;
}

/**
 * Destructor.
 */
RecentFileItemsContainerModeEnum::~RecentFileItemsContainerModeEnum()
{
}

/**
 * Initialize the enumerated metadata.
 */
void
RecentFileItemsContainerModeEnum::initialize()
{
    if (initializedFlag) {
        return;
    }
    initializedFlag = true;

    /*
     * Changing the order of these will affect the order in the recent files dialog buttons
     */
    
    enumData.push_back(RecentFileItemsContainerModeEnum(FAVORITES,
                                           "FAVORITES",
                                           "Favorites",
                                           "Favorites"));
    
    enumData.push_back(RecentFileItemsContainerModeEnum(EXAMPLE_DATA_SETS,
                                                        "EXAMPLE_DATA_SETS",
                                                        "Example Data",
                                                        "Example\nData"));
    

    enumData.push_back(RecentFileItemsContainerModeEnum(RECENT_SCENES,
                                                        "RECENT_SCENES",
                                                        "Recent Scenes",
                                                        "Recent\nScenes"));
    
    enumData.push_back(RecentFileItemsContainerModeEnum(RECENT_FILES,
                                           "RECENT_FILES",
                                           "Recent Files",
                                           "Recent\nFiles"));
    
    const QString noButtonName("");
    enumData.push_back(RecentFileItemsContainerModeEnum(OTHER,
                                                        "OTHER",
                                                        "Other",
                                                        noButtonName));
    
    enumData.push_back(RecentFileItemsContainerModeEnum(DIRECTORY_SCENE_AND_SPEC_FILES,
                                           "DIRECTORY_SCENE_AND_SPEC_FILES",
                                           "Directory Scene and Spec Files",
                                           "Current\nDirectory\nFiles"));
    
    enumData.push_back(RecentFileItemsContainerModeEnum(RECENT_DIRECTORIES,
                                           "RECENT_DIRECTORIES",
                                           "Recent Directories",
                                           "Recent\nDirectories"));
}

/**
 * Find the data for and enumerated value.
 * @param enumValue
 *     The enumerated value.
 * @return Pointer to data for this enumerated type
 * or NULL if no data for type or if type is invalid.
 */
const RecentFileItemsContainerModeEnum*
RecentFileItemsContainerModeEnum::findData(const Enum enumValue)
{
    if (initializedFlag == false) initialize();

    size_t num = enumData.size();
    for (size_t i = 0; i < num; i++) {
        const RecentFileItemsContainerModeEnum* d = &enumData[i];
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
RecentFileItemsContainerModeEnum::toName(Enum enumValue) {
    if (initializedFlag == false) initialize();
    
    const RecentFileItemsContainerModeEnum* enumInstance = findData(enumValue);
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
RecentFileItemsContainerModeEnum::Enum
RecentFileItemsContainerModeEnum::fromName(const AString& name, bool* isValidOut)
{
    if (initializedFlag == false) initialize();
    
    bool validFlag = false;
    Enum enumValue = getDefaultValue();
    
    for (std::vector<RecentFileItemsContainerModeEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const RecentFileItemsContainerModeEnum& d = *iter;
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
        CaretAssertMessage(0, AString("Name " + name + "failed to match enumerated value for type RecentFileItemsContainerModeEnum"));
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
RecentFileItemsContainerModeEnum::toGuiName(Enum enumValue) {
    if (initializedFlag == false) initialize();
    
    const RecentFileItemsContainerModeEnum* enumInstance = findData(enumValue);
    return enumInstance->guiName;
}

/**
 * Get a GUI string representation of the enumerated type for Recent Files Dialog buttons
 * @param enumValue
 *     Enumerated value.
 * @return
 *     String representing enumerated value.  If empty string, then do not use as a button
 */
AString
RecentFileItemsContainerModeEnum::toGuiButtonName(Enum enumValue) {
    if (initializedFlag == false) initialize();
    
    const RecentFileItemsContainerModeEnum* enumInstance = findData(enumValue);
    return enumInstance->guiButtonName;
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
RecentFileItemsContainerModeEnum::Enum
RecentFileItemsContainerModeEnum::fromGuiName(const AString& guiName, bool* isValidOut)
{
    if (initializedFlag == false) initialize();
    
    bool validFlag = false;
    Enum enumValue = getDefaultValue();
    
    for (std::vector<RecentFileItemsContainerModeEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const RecentFileItemsContainerModeEnum& d = *iter;
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
        CaretAssertMessage(0, AString("guiName " + guiName + "failed to match enumerated value for type RecentFileItemsContainerModeEnum"));
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
RecentFileItemsContainerModeEnum::toIntegerCode(Enum enumValue)
{
    if (initializedFlag == false) initialize();
    const RecentFileItemsContainerModeEnum* enumInstance = findData(enumValue);
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
RecentFileItemsContainerModeEnum::Enum
RecentFileItemsContainerModeEnum::fromIntegerCode(const int32_t integerCode, bool* isValidOut)
{
    if (initializedFlag == false) initialize();
    
    bool validFlag = false;
    Enum enumValue = getDefaultValue();
    
    for (std::vector<RecentFileItemsContainerModeEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const RecentFileItemsContainerModeEnum& enumInstance = *iter;
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
        CaretAssertMessage(0, AString("Integer code " + AString::number(integerCode) + "failed to match enumerated value for type RecentFileItemsContainerModeEnum"));
    }
    return enumValue;
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
 *     Default enum value.
 */
RecentFileItemsContainerModeEnum::Enum
RecentFileItemsContainerModeEnum::getDefaultValue()
{
    return RECENT_FILES;
}

/**
 * Get all of the enumerated type values.  The values can be used
 * as parameters to toXXX() methods to get associated metadata.
 *
 * @param allEnums
 *     A vector that is OUTPUT containing all of the enumerated values.
 */
void
RecentFileItemsContainerModeEnum::getAllEnums(std::vector<RecentFileItemsContainerModeEnum::Enum>& allEnums)
{
    if (initializedFlag == false) initialize();
    
    allEnums.clear();
    
    for (std::vector<RecentFileItemsContainerModeEnum>::iterator iter = enumData.begin();
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
RecentFileItemsContainerModeEnum::getAllNames(std::vector<AString>& allNames, const bool isSorted)
{
    if (initializedFlag == false) initialize();
    
    allNames.clear();
    
    for (std::vector<RecentFileItemsContainerModeEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        allNames.push_back(RecentFileItemsContainerModeEnum::toName(iter->enumValue));
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
RecentFileItemsContainerModeEnum::getAllGuiNames(std::vector<AString>& allGuiNames, const bool isSorted)
{
    if (initializedFlag == false) initialize();
    
    allGuiNames.clear();
    
    for (std::vector<RecentFileItemsContainerModeEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        allGuiNames.push_back(RecentFileItemsContainerModeEnum::toGuiName(iter->enumValue));
    }
    
    if (isSorted) {
        std::sort(allGuiNames.begin(), allGuiNames.end());
    }
}

