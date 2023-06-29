
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
#define __VOLUME_SLICE_PROJECTION_TYPE_ENUM_DECLARE__
#include "VolumeSliceProjectionTypeEnum.h"
#undef __VOLUME_SLICE_PROJECTION_TYPE_ENUM_DECLARE__

#include "CaretAssert.h"

using namespace caret;

    
/**
 * \class caret::VolumeSliceProjectionTypeEnum 
 * \brief Type of projection for drawing a volume slice
 *
 * Draw volume slice with an oblique or orthogonal projection
 *
 * Using this enumerated type in the GUI with an EnumComboBoxTemplate
 * 
 * Header File (.h)
 *     Forward declare the data type:
 *         class EnumComboBoxTemplate;
 * 
 *     Declare the member:
 *         EnumComboBoxTemplate* m_volumeSliceProjectionTypeEnumComboBox;
 * 
 *     Declare a slot that is called when user changes selection
 *         private slots:
 *             void volumeSliceProjectionTypeEnumComboBoxItemActivated();
 * 
 * Implementation File (.cxx)
 *     Include the header files
 *         #include "EnumComboBoxTemplate.h"
 *         #include "VolumeSliceProjectionTypeEnum.h"
 * 
 *     Instatiate:
 *         m_volumeSliceProjectionTypeEnumComboBox = new EnumComboBoxTemplate(this);
 *         m_volumeSliceProjectionTypeEnumComboBox->setup<VolumeSliceProjectionTypeEnum,VolumeSliceProjectionTypeEnum::Enum>();
 * 
 *     Get notified when the user changes the selection: 
 *         QObject::connect(m_volumeSliceProjectionTypeEnumComboBox, SIGNAL(itemActivated()),
 *                          this, SLOT(volumeSliceProjectionTypeEnumComboBoxItemActivated()));
 * 
 *     Update the selection:
 *         m_volumeSliceProjectionTypeEnumComboBox->setSelectedItem<VolumeSliceProjectionTypeEnum,VolumeSliceProjectionTypeEnum::Enum>(NEW_VALUE);
 * 
 *     Read the selection:
 *         const VolumeSliceProjectionTypeEnum::Enum VARIABLE = m_volumeSliceProjectionTypeEnumComboBox->getSelectedItem<VolumeSliceProjectionTypeEnum,VolumeSliceProjectionTypeEnum::Enum>();
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
 * @param toolTip
 *    Tooltip for GUI
 */
VolumeSliceProjectionTypeEnum::VolumeSliceProjectionTypeEnum(const Enum enumValue,
                                                             const AString& name,
                                                             const AString& guiName,
                                                             const AString& toolTip)
{
    this->enumValue = enumValue;
    this->integerCode = integerCodeCounter++;
    this->name = name;
    this->guiName = guiName;
    this->toolTip = toolTip;
}

/**
 * Destructor.
 */
VolumeSliceProjectionTypeEnum::~VolumeSliceProjectionTypeEnum()
{
}

/**
 * Initialize the enumerated metadata.
 */
void
VolumeSliceProjectionTypeEnum::initialize()
{
    if (initializedFlag) {
        return;
    }
    initializedFlag = true;

    enumData.push_back(VolumeSliceProjectionTypeEnum(VOLUME_SLICE_PROJECTION_ORTHOGONAL,
                                                     "VOLUME_SLICE_PROJECTION_ORTHOGONAL",
                                                     "Ortho",
                                                     "View slices perpendicular to X, Y, an Z axes"));
    
    enumData.push_back(VolumeSliceProjectionTypeEnum(VOLUME_SLICE_PROJECTION_OBLIQUE,
                                                     "VOLUME_SLICE_PROJECTION_OBLIQUE",
                                                     "Oblique",
                                                     "Rotate to view slices along arbitrary axes"));
    
    enumData.push_back(VolumeSliceProjectionTypeEnum(VOLUME_SLICE_PROJECTION_MPR,
                                                     "VOLUME_SLICE_PROJECTION_MPR",
                                                     "MPR",
                                                     "Multi-Planar Reconstruction"));
    
    enumData.push_back(VolumeSliceProjectionTypeEnum(VOLUME_SLICE_PROJECTION_MPR_THREE,
                                                     "VOLUME_SLICE_PROJECTION_MPR_THREE",
                                                     "MPR New",
                                                     "Multi-Planar Reconstruction New"));
}

/**
 * Find the data for and enumerated value.
 * @param enumValue
 *     The enumerated value.
 * @return Pointer to data for this enumerated type
 * or NULL if no data for type or if type is invalid.
 */
const VolumeSliceProjectionTypeEnum*
VolumeSliceProjectionTypeEnum::findData(const Enum enumValue)
{
    if (initializedFlag == false) initialize();

    size_t num = enumData.size();
    for (size_t i = 0; i < num; i++) {
        const VolumeSliceProjectionTypeEnum* d = &enumData[i];
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
VolumeSliceProjectionTypeEnum::toName(Enum enumValue) {
    if (initializedFlag == false) initialize();
    
    const VolumeSliceProjectionTypeEnum* enumInstance = findData(enumValue);
    return enumInstance->name;
}

/**
 * Get an enumerated value corresponding to its name.
 * @param nameIn
 *     Name of enumerated value.
 * @param isValidOut 
 *     If not NULL, it is set indicating that a
 *     enum value exists for the input name.
 * @return 
 *     Enumerated value.
 */
VolumeSliceProjectionTypeEnum::Enum 
VolumeSliceProjectionTypeEnum::fromName(const AString& nameIn, bool* isValidOut)
{
    if (initializedFlag == false) initialize();
    
    AString name(nameIn);
    if (name == "VOLUME_SLICE_PROJECTION_MPR") {
        name = "VOLUME_SLICE_PROJECTION_MPR";
    }
    else if (name == "VOLUME_SLICE_PROJECTION_MPR_NEUROLOGICAL") {
        name = "VOLUME_SLICE_PROJECTION_MPR";
    }
    else if (name == "VOLUME_SLICE_PROJECTION_MPR_RADIOLOGICAL") {
        name = "VOLUME_SLICE_PROJECTION_MPR";
    }
    
    bool validFlag = false;
    Enum enumValue = VolumeSliceProjectionTypeEnum::enumData[0].enumValue;
    
    for (std::vector<VolumeSliceProjectionTypeEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const VolumeSliceProjectionTypeEnum& d = *iter;
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
        CaretAssertMessage(0, AString("Name " + name + "failed to match enumerated value for type VolumeSliceProjectionTypeEnum"));
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
VolumeSliceProjectionTypeEnum::toGuiName(Enum enumValue) {
    if (initializedFlag == false) initialize();
    
    const VolumeSliceProjectionTypeEnum* enumInstance = findData(enumValue);
    return enumInstance->guiName;
}

/**
 * Get a tooltip for use in GUI
 * @param enumValue
 *     Enumerated value.
 * @return
 *     String representing enumerated value.
 */
AString
VolumeSliceProjectionTypeEnum::toToolTip(Enum enumValue)
{
    if (initializedFlag == false) initialize();
    
    const VolumeSliceProjectionTypeEnum* enumInstance = findData(enumValue);
    return enumInstance->toolTip;
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
VolumeSliceProjectionTypeEnum::Enum 
VolumeSliceProjectionTypeEnum::fromGuiName(const AString& guiName, bool* isValidOut)
{
    if (initializedFlag == false) initialize();
    
    bool validFlag = false;
    Enum enumValue = VolumeSliceProjectionTypeEnum::enumData[0].enumValue;
    
    for (std::vector<VolumeSliceProjectionTypeEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const VolumeSliceProjectionTypeEnum& d = *iter;
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
        CaretAssertMessage(0, AString("guiName " + guiName + "failed to match enumerated value for type VolumeSliceProjectionTypeEnum"));
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
VolumeSliceProjectionTypeEnum::toIntegerCode(Enum enumValue)
{
    if (initializedFlag == false) initialize();
    const VolumeSliceProjectionTypeEnum* enumInstance = findData(enumValue);
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
VolumeSliceProjectionTypeEnum::Enum
VolumeSliceProjectionTypeEnum::fromIntegerCode(const int32_t integerCode, bool* isValidOut)
{
    if (initializedFlag == false) initialize();
    
    bool validFlag = false;
    Enum enumValue = VolumeSliceProjectionTypeEnum::enumData[0].enumValue;
    
    for (std::vector<VolumeSliceProjectionTypeEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const VolumeSliceProjectionTypeEnum& enumInstance = *iter;
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
        CaretAssertMessage(0, AString("Integer code " + AString::number(integerCode) + "failed to match enumerated value for type VolumeSliceProjectionTypeEnum"));
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
VolumeSliceProjectionTypeEnum::getAllEnums(std::vector<VolumeSliceProjectionTypeEnum::Enum>& allEnums)
{
    if (initializedFlag == false) initialize();
    
    allEnums.clear();
    
    for (std::vector<VolumeSliceProjectionTypeEnum>::iterator iter = enumData.begin();
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
VolumeSliceProjectionTypeEnum::getAllNames(std::vector<AString>& allNames, const bool isSorted)
{
    if (initializedFlag == false) initialize();
    
    allNames.clear();
    
    for (std::vector<VolumeSliceProjectionTypeEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        allNames.push_back(VolumeSliceProjectionTypeEnum::toName(iter->enumValue));
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
VolumeSliceProjectionTypeEnum::getAllGuiNames(std::vector<AString>& allGuiNames, const bool isSorted)
{
    if (initializedFlag == false) initialize();
    
    allGuiNames.clear();
    
    for (std::vector<VolumeSliceProjectionTypeEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        allGuiNames.push_back(VolumeSliceProjectionTypeEnum::toGuiName(iter->enumValue));
    }
    
    if (isSorted) {
        std::sort(allGuiNames.begin(), allGuiNames.end());
    }
}

/**
 * @return Tooltip for use in the GUI control for selecting a projection type
 */
AString
VolumeSliceProjectionTypeEnum::getToolTipForGuiInHtml()
{
    std::vector<VolumeSliceProjectionTypeEnum::Enum> allEnums;
    getAllEnums(allEnums);
    
    AString txt;
    txt.append("<html>Select mode for viewing volume slices:");
    txt.append("<ul>");
    for (auto e : allEnums) {
        txt.append("<li> "
                   + toGuiName(e)
                   + " - "
                   + toToolTip(e));
    }
    txt.append("</ul>");
    txt.append("</html>");
    
    return txt;
}

