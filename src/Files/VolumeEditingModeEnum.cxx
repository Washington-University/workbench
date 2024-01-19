
/*LICENSE_START*/
/*
 *  Copyright (C) 2014 Washington University School of Medicine
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
#define __VOLUME_EDITING_MODE_DECLARE__
#include "VolumeEditingModeEnum.h"
#undef __VOLUME_EDITING_MODE_DECLARE__

#include "CaretAssert.h"

using namespace caret;

    
/**
 * \class caret::VolumeEditingModeEnum 
 * \brief Enumerated type for volume editing.
 *
 * <REPLACE-WITH-THOROUGH DESCRIPTION>
 *
 * Using this enumerated type in the GUI with an EnumComboBoxTemplate
 * 
 * Header File (.h)
 *     Forward declare the data type:
 *         class EnumComboBoxTemplate;
 * 
 *     Declare the member:
 *         EnumComboBoxTemplate* m_volumeEditingModeEnumComboBox;
 * 
 *     Declare a slot that is called when user changes selection
 *         private slots:
 *             void volumeEditingModeEnumComboBoxItemActivated();
 * 
 * Implementation File (.cxx)
 *     Include the header files
 *         #include "EnumComboBoxTemplate.h"
 *         #include "VolumeEditingModeEnum.h"
 * 
 *     Instatiate:
 *         m_volumeEditingModeEnumComboBox = new EnumComboBoxTemplate(this);
 *         m_volumeEditingModeEnumComboBox->setup<VolumeEditingModeEnum,VolumeEditingModeEnum::Enum>();
 * 
 *     Get notified when the user changes the selection: 
 *         QObject::connect(m_volumeEditingModeEnumComboBox, SIGNAL(itemActivated()),
 *                          this, SLOT(volumeEditingModeEnumComboBoxItemActivated()));
 * 
 *     Update the selection:
 *         m_volumeEditingModeEnumComboBox->setSelectedItem<VolumeEditingModeEnum,VolumeEditingModeEnum::Enum>(NEW_VALUE);
 * 
 *     Read the selection:
 *         const VolumeEditingModeEnum::Enum VARIABLE = m_volumeEditingModeEnumComboBox->getSelectedItem<VolumeEditingModeEnum,VolumeEditingModeEnum::Enum>();
 * 
 */

/**
 * Constructor.
 *
 * @param enumValue
 *    An enumerated value.
 * @param name
 *    Name of enumerated value.
 * @param guiName
 *    User-friendly name for use in user-interface.
 * @param toolTipText
 *    Text for the tooltip.
 */
VolumeEditingModeEnum::VolumeEditingModeEnum(const Enum enumValue,
                           const AString& name,
                                             const AString& guiName,
                                             const AString& toolTipText)
{
    this->enumValue   = enumValue;
    this->integerCode = integerCodeCounter++;
    this->name        = name;
    this->guiName     = guiName;
    this->toolTipText = toolTipText;
}

/**
 * Destructor.
 */
VolumeEditingModeEnum::~VolumeEditingModeEnum()
{
}

/**
 * Initialize the enumerated metadata.
 */
void
VolumeEditingModeEnum::initialize()
{
    if (initializedFlag) {
        return;
    }
    initializedFlag = true;

    enumData.push_back(VolumeEditingModeEnum(VOLUME_EDITING_MODE_ON,
                                             "VOLUME_EDITING_MODE_ON",
                                             "On",
                                             "Turn voxels on at mouse click.  Or move the mouse with the left "
                                             "mouse button down while holding down the CTRL (Apple key on Mac) "
                                             "and SHIFT keys."));
    
    enumData.push_back(VolumeEditingModeEnum(VOLUME_EDITING_MODE_OFF,
                                             "VOLUME_EDITING_MODE_OFF",
                                             "Off",
                                             "Turn voxels off at mouse click.   Or move the mouse with the left "
                                             "mouse button down while holding down the CTRL (Apple key on Mac) "
                                             "and SHIFT keys."));
    
    enumData.push_back(VolumeEditingModeEnum(VOLUME_EDITING_MODE_DILATE,
                                             "VOLUME_EDITING_MODE_DILATE",
                                             "Dilate",
                                             "Dilate voxels at mouse click"));
    
    enumData.push_back(VolumeEditingModeEnum(VOLUME_EDITING_MODE_ERODE,
                                             "VOLUME_EDITING_MODE_ERODE",
                                             "Erode",
                                             "Erode voxels at mouse click"));
    
    enumData.push_back(VolumeEditingModeEnum(VOLUME_EDITING_MODE_FLOOD_FILL_2D,
                                             "VOLUME_EDITING_MODE_FLOOD_FILL_2D",
                                             "Fill 2D",
                                             "Fill closed region (2D) at mouse click"));
    
    enumData.push_back(VolumeEditingModeEnum(VOLUME_EDITING_MODE_FLOOD_FILL_3D,
                                             "VOLUME_EDITING_MODE_FLOOD_FILL_3D",
                                             "Fill 3D",
                                             "Fill closed region (3D) at mouse click"));
    
    enumData.push_back(VolumeEditingModeEnum(VOLUME_EDITING_MODE_REMOVE_CONNECTED_2D,
                                             "VOLUME_EDITING_MODE_REMOVE_CONNECTED_2D",
                                             "Remove 2D",
                                             "Remove connected region (2D) at mouse click"));
    
    enumData.push_back(VolumeEditingModeEnum(VOLUME_EDITING_MODE_REMOVE_CONNECTED_3D,
                                             "VOLUME_EDITING_MODE_REMOVE_CONNECTED_3D",
                                             "Remove 3D",
                                             "Remove connected region (3D) at mouse click"));
    
    enumData.push_back(VolumeEditingModeEnum(VOLUME_EDITING_MODE_RETAIN_CONNECTED_3D,
                                             "VOLUME_EDITING_MODE_RETAIN_CONNECTED_3D",
                                             "Retain 3D",
                                             "Remove voxel not connected to region (3D) at mouse click"));
    
}

/**
 * Find the data for and enumerated value.
 * @param enumValue
 *     The enumerated value.
 * @return Pointer to data for this enumerated type
 * or NULL if no data for type or if type is invalid.
 */
const VolumeEditingModeEnum*
VolumeEditingModeEnum::findData(const Enum enumValue)
{
    if (initializedFlag == false) initialize();

    size_t num = enumData.size();
    for (size_t i = 0; i < num; i++) {
        const VolumeEditingModeEnum* d = &enumData[i];
        if (d->enumValue == enumValue) {
            return d;
        }
    }

    return NULL;
}

/**
 * Get a tool tip string of the enumerated type.
 * @param enumValue
 *     Enumerated value.
 * @return
 *     String representing tooltip.
 */
AString
VolumeEditingModeEnum::toToolTip(Enum enumValue)
{
    if (initializedFlag == false) initialize();
    
    const VolumeEditingModeEnum* enumInstance = findData(enumValue);
    return enumInstance->toolTipText;
}


/**
 * Get a string representation of the enumerated type.
 * @param enumValue 
 *     Enumerated value.
 * @return 
 *     String representing enumerated value.
 */
AString 
VolumeEditingModeEnum::toName(Enum enumValue) {
    if (initializedFlag == false) initialize();
    
    const VolumeEditingModeEnum* enumInstance = findData(enumValue);
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
VolumeEditingModeEnum::Enum 
VolumeEditingModeEnum::fromName(const AString& name, bool* isValidOut)
{
    if (initializedFlag == false) initialize();
    
    bool validFlag = false;
    Enum enumValue = VolumeEditingModeEnum::enumData[0].enumValue;
    
    for (std::vector<VolumeEditingModeEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const VolumeEditingModeEnum& d = *iter;
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
        CaretAssertMessage(0, AString("Name " + name + "failed to match enumerated value for type VolumeEditingModeEnum"));
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
VolumeEditingModeEnum::toGuiName(Enum enumValue) {
    if (initializedFlag == false) initialize();
    
    const VolumeEditingModeEnum* enumInstance = findData(enumValue);
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
VolumeEditingModeEnum::Enum 
VolumeEditingModeEnum::fromGuiName(const AString& guiName, bool* isValidOut)
{
    if (initializedFlag == false) initialize();
    
    bool validFlag = false;
    Enum enumValue = VolumeEditingModeEnum::enumData[0].enumValue;
    
    for (std::vector<VolumeEditingModeEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const VolumeEditingModeEnum& d = *iter;
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
        CaretAssertMessage(0, AString("guiName " + guiName + "failed to match enumerated value for type VolumeEditingModeEnum"));
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
VolumeEditingModeEnum::toIntegerCode(Enum enumValue)
{
    if (initializedFlag == false) initialize();
    const VolumeEditingModeEnum* enumInstance = findData(enumValue);
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
VolumeEditingModeEnum::Enum
VolumeEditingModeEnum::fromIntegerCode(const int32_t integerCode, bool* isValidOut)
{
    if (initializedFlag == false) initialize();
    
    bool validFlag = false;
    Enum enumValue = VolumeEditingModeEnum::enumData[0].enumValue;
    
    for (std::vector<VolumeEditingModeEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const VolumeEditingModeEnum& enumInstance = *iter;
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
        CaretAssertMessage(0, AString("Integer code " + AString::number(integerCode) + "failed to match enumerated value for type VolumeEditingModeEnum"));
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
VolumeEditingModeEnum::getAllEnums(std::vector<VolumeEditingModeEnum::Enum>& allEnums)
{
    if (initializedFlag == false) initialize();
    
    allEnums.clear();
    
    for (std::vector<VolumeEditingModeEnum>::iterator iter = enumData.begin();
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
VolumeEditingModeEnum::getAllNames(std::vector<AString>& allNames, const bool isSorted)
{
    if (initializedFlag == false) initialize();
    
    allNames.clear();
    
    for (std::vector<VolumeEditingModeEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        allNames.push_back(VolumeEditingModeEnum::toName(iter->enumValue));
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
VolumeEditingModeEnum::getAllGuiNames(std::vector<AString>& allGuiNames, const bool isSorted)
{
    if (initializedFlag == false) initialize();
    
    allGuiNames.clear();
    
    for (std::vector<VolumeEditingModeEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        allGuiNames.push_back(VolumeEditingModeEnum::toGuiName(iter->enumValue));
    }
    
    if (isSorted) {
        std::sort(allGuiNames.begin(), allGuiNames.end());
    }
}

/**
 * Does the enum value allow oblique editing?
 *
 * @param enumValue
 *     The editing mode.
 * @return
 *     True if the enum value allows oblique slice editing, else false.
 */
bool
VolumeEditingModeEnum::isObliqueEditingAllowed(const Enum enumValue)
{
    bool obliqueEditingSupportedFlag = false;
    
    switch (enumValue) {
        case VolumeEditingModeEnum::VOLUME_EDITING_MODE_ON:
        case VolumeEditingModeEnum::VOLUME_EDITING_MODE_OFF:
        case VolumeEditingModeEnum::VOLUME_EDITING_MODE_DILATE:
        case VolumeEditingModeEnum::VOLUME_EDITING_MODE_ERODE:
        case VolumeEditingModeEnum::VOLUME_EDITING_MODE_REMOVE_CONNECTED_3D:
        case VolumeEditingModeEnum::VOLUME_EDITING_MODE_RETAIN_CONNECTED_3D:
        case VolumeEditingModeEnum::VOLUME_EDITING_MODE_FLOOD_FILL_3D:
            obliqueEditingSupportedFlag = true;
            break;
        case VolumeEditingModeEnum::VOLUME_EDITING_MODE_REMOVE_CONNECTED_2D:
        case VolumeEditingModeEnum::VOLUME_EDITING_MODE_FLOOD_FILL_2D:
            break;
    }
    
    return obliqueEditingSupportedFlag;
}


