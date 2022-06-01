
/*LICENSE_START*/
/*
 *  Copyright (C) 2017 Washington University School of Medicine
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
#define __VOLUME_SLICE_INTERPOLATION_EDGE_EFFECTS_MASKING_ENUM_DECLARE__
#include "VolumeSliceInterpolationEdgeEffectsMaskingEnum.h"
#undef __VOLUME_SLICE_INTERPOLATION_EDGE_EFFECTS_MASKING_ENUM_DECLARE__

#include "CaretAssert.h"

using namespace caret;

    
/**
 * \class caret::VolumeSliceInterpolationEdgeEffectsMaskingEnum 
 * \brief Enumerated type for edge effects masking.
 *
 * Using this enumerated type in the GUI with an EnumComboBoxTemplate
 * 
 * Header File (.h)
 *     Forward declare the data type:
 *         class EnumComboBoxTemplate;
 * 
 *     Declare the member:
 *         EnumComboBoxTemplate* m_volumeSliceInterpolationEdgeEffectsMaskingEnumComboBox;
 * 
 *     Declare a slot that is called when user changes selection
 *         private slots:
 *             void volumeSliceInterpolationEdgeEffectsMaskingEnumComboBoxItemActivated();
 * 
 * Implementation File (.cxx)
 *     Include the header files
 *         #include "EnumComboBoxTemplate.h"
 *         #include "VolumeSliceInterpolationEdgeEffectsMaskingEnum.h"
 * 
 *     Instatiate:
 *         m_volumeSliceInterpolationEdgeEffectsMaskingEnumComboBox = new EnumComboBoxTemplate(this);
 *         m_volumeSliceInterpolationEdgeEffectsMaskingEnumComboBox->setup<VolumeSliceInterpolationEdgeEffectsMaskingEnum,VolumeSliceInterpolationEdgeEffectsMaskingEnum::Enum>();
 * 
 *     Get notified when the user changes the selection: 
 *         QObject::connect(m_volumeSliceInterpolationEdgeEffectsMaskingEnumComboBox, SIGNAL(itemActivated()),
 *                          this, SLOT(volumeSliceInterpolationEdgeEffectsMaskingEnumComboBoxItemActivated()));
 * 
 *     Update the selection:
 *         m_volumeSliceInterpolationEdgeEffectsMaskingEnumComboBox->setSelectedItem<VolumeSliceInterpolationEdgeEffectsMaskingEnum,VolumeSliceInterpolationEdgeEffectsMaskingEnum::Enum>(NEW_VALUE);
 * 
 *     Read the selection:
 *         const VolumeSliceInterpolationEdgeEffectsMaskingEnum::Enum VARIABLE = m_volumeSliceInterpolationEdgeEffectsMaskingEnumComboBox->getSelectedItem<VolumeSliceInterpolationEdgeEffectsMaskingEnum,VolumeSliceInterpolationEdgeEffectsMaskingEnum::Enum>();
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
 * @param shortGuiName
 *    Short name for use in GUI
 * @param toolTip
 *    Tooltip for enum
 */
VolumeSliceInterpolationEdgeEffectsMaskingEnum::VolumeSliceInterpolationEdgeEffectsMaskingEnum(const Enum enumValue,
                                                                                               const AString& name,
                                                                                               const AString& guiName,
                                                                                               const AString& shortGuiName,
                                                                                               const AString& toolTip)
{
    this->enumValue = enumValue;
    this->integerCode = integerCodeCounter++;
    this->name = name;
    this->guiName = guiName;
    this->shortGuiName = shortGuiName;
    this->toolTip = toolTip;
}

/**
 * Destructor.
 */
VolumeSliceInterpolationEdgeEffectsMaskingEnum::~VolumeSliceInterpolationEdgeEffectsMaskingEnum()
{
}

AString
VolumeSliceInterpolationEdgeEffectsMaskingEnum::getToolTip()
{
    AString toolTip("<html>"
                    "Masking is used to remove edge effects from cubic<br>"
                    "interpolation.  In extreme instances, the artifacts<br>"
                    "result in blocky and/or striped patterns.<br>"
                    "<br>");
    
    std::vector<Enum> allEnums;
    getAllEnums(allEnums);
    
    for (const auto enumValue : allEnums) {
        AString description(toGuiName(enumValue)
                            + " - "
                            + toToolTip(enumValue));
//        switch (enumValue) {
//            case OFF:
//                description.append("No masking");
//                break;
//            case LOOSE:
//                description.append("Mask with Trilinear Interpolation");
//                break;
//            case TIGHT:
//                description.append("Mask with Enclosing Voxel");
//                break;
//        }
        toolTip.append(description + "<br>");
    }
    
    toolTip.append("</html>");
    
    return toolTip;
}

/**
 * Initialize the enumerated metadata.
 */
void
VolumeSliceInterpolationEdgeEffectsMaskingEnum::initialize()
{
    if (initializedFlag) {
        return;
    }
    initializedFlag = true;

    enumData.push_back(VolumeSliceInterpolationEdgeEffectsMaskingEnum(OFF,
                                                                      "OFF",
                                                                      "Masking Off",
                                                                      "Off",
                                                                      "No masking"));
    
    enumData.push_back(VolumeSliceInterpolationEdgeEffectsMaskingEnum(LOOSE,
                                                                      "LOOSE",
                                                                      "Masking Loose",
                                                                      "Loose",
                                                                      "Mask with Trilinear Interpolation"));
    
    enumData.push_back(VolumeSliceInterpolationEdgeEffectsMaskingEnum(TIGHT,
                                                                      "TIGHT",
                                                                      "Masking Tight",
                                                                      "Tight",
                                                                      "Mask with Enclosing Voxel"));
}

/**
 * Find the data for and enumerated value.
 * @param enumValue
 *     The enumerated value.
 * @return Pointer to data for this enumerated type
 * or NULL if no data for type or if type is invalid.
 */
const VolumeSliceInterpolationEdgeEffectsMaskingEnum*
VolumeSliceInterpolationEdgeEffectsMaskingEnum::findData(const Enum enumValue)
{
    if (initializedFlag == false) initialize();

    size_t num = enumData.size();
    for (size_t i = 0; i < num; i++) {
        const VolumeSliceInterpolationEdgeEffectsMaskingEnum* d = &enumData[i];
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
VolumeSliceInterpolationEdgeEffectsMaskingEnum::toName(Enum enumValue) {
    if (initializedFlag == false) initialize();
    
    const VolumeSliceInterpolationEdgeEffectsMaskingEnum* enumInstance = findData(enumValue);
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
VolumeSliceInterpolationEdgeEffectsMaskingEnum::Enum 
VolumeSliceInterpolationEdgeEffectsMaskingEnum::fromName(const AString& name, bool* isValidOut)
{
    if (initializedFlag == false) initialize();
    
    bool validFlag = false;
    Enum enumValue = VolumeSliceInterpolationEdgeEffectsMaskingEnum::enumData[0].enumValue;
    
    for (std::vector<VolumeSliceInterpolationEdgeEffectsMaskingEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const VolumeSliceInterpolationEdgeEffectsMaskingEnum& d = *iter;
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
        CaretAssertMessage(0, AString("Name " + name + "failed to match enumerated value for type VolumeSliceInterpolationEdgeEffectsMaskingEnum"));
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
VolumeSliceInterpolationEdgeEffectsMaskingEnum::toGuiName(Enum enumValue)
{
    if (initializedFlag == false) initialize();
    
    const VolumeSliceInterpolationEdgeEffectsMaskingEnum* enumInstance = findData(enumValue);
    return enumInstance->guiName;
}

/**
 * Get a SHORT GUI string representation of the enumerated type.
 * @param enumValue
 *     Enumerated value.
 * @return
 *     short String representing enumerated value.
 */
AString
VolumeSliceInterpolationEdgeEffectsMaskingEnum::toShortGuiName(Enum enumValue)
{
    if (initializedFlag == false) initialize();
    
    const VolumeSliceInterpolationEdgeEffectsMaskingEnum* enumInstance = findData(enumValue);
    return enumInstance->shortGuiName;
}

/**
 * ToolTip of the enumerated type.
 * @param enumValue
 *     Enumerated value.
 * @return
 *     Tool tip representing enumerated value.
 */
AString
VolumeSliceInterpolationEdgeEffectsMaskingEnum::toToolTip(Enum enumValue)
{
    if (initializedFlag == false) initialize();
    
    const VolumeSliceInterpolationEdgeEffectsMaskingEnum* enumInstance = findData(enumValue);
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
VolumeSliceInterpolationEdgeEffectsMaskingEnum::Enum 
VolumeSliceInterpolationEdgeEffectsMaskingEnum::fromGuiName(const AString& guiName, bool* isValidOut)
{
    if (initializedFlag == false) initialize();
    
    bool validFlag = false;
    Enum enumValue = VolumeSliceInterpolationEdgeEffectsMaskingEnum::enumData[0].enumValue;
    
    for (std::vector<VolumeSliceInterpolationEdgeEffectsMaskingEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const VolumeSliceInterpolationEdgeEffectsMaskingEnum& d = *iter;
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
        CaretAssertMessage(0, AString("guiName " + guiName + "failed to match enumerated value for type VolumeSliceInterpolationEdgeEffectsMaskingEnum"));
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
VolumeSliceInterpolationEdgeEffectsMaskingEnum::toIntegerCode(Enum enumValue)
{
    if (initializedFlag == false) initialize();
    const VolumeSliceInterpolationEdgeEffectsMaskingEnum* enumInstance = findData(enumValue);
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
VolumeSliceInterpolationEdgeEffectsMaskingEnum::Enum
VolumeSliceInterpolationEdgeEffectsMaskingEnum::fromIntegerCode(const int32_t integerCode, bool* isValidOut)
{
    if (initializedFlag == false) initialize();
    
    bool validFlag = false;
    Enum enumValue = VolumeSliceInterpolationEdgeEffectsMaskingEnum::enumData[0].enumValue;
    
    for (std::vector<VolumeSliceInterpolationEdgeEffectsMaskingEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const VolumeSliceInterpolationEdgeEffectsMaskingEnum& enumInstance = *iter;
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
        CaretAssertMessage(0, AString("Integer code " + AString::number(integerCode) + "failed to match enumerated value for type VolumeSliceInterpolationEdgeEffectsMaskingEnum"));
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
VolumeSliceInterpolationEdgeEffectsMaskingEnum::getAllEnums(std::vector<VolumeSliceInterpolationEdgeEffectsMaskingEnum::Enum>& allEnums)
{
    if (initializedFlag == false) initialize();
    
    allEnums.clear();
    
    for (std::vector<VolumeSliceInterpolationEdgeEffectsMaskingEnum>::iterator iter = enumData.begin();
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
VolumeSliceInterpolationEdgeEffectsMaskingEnum::getAllNames(std::vector<AString>& allNames, const bool isSorted)
{
    if (initializedFlag == false) initialize();
    
    allNames.clear();
    
    for (std::vector<VolumeSliceInterpolationEdgeEffectsMaskingEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        allNames.push_back(VolumeSliceInterpolationEdgeEffectsMaskingEnum::toName(iter->enumValue));
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
VolumeSliceInterpolationEdgeEffectsMaskingEnum::getAllGuiNames(std::vector<AString>& allGuiNames, const bool isSorted)
{
    if (initializedFlag == false) initialize();
    
    allGuiNames.clear();
    
    for (std::vector<VolumeSliceInterpolationEdgeEffectsMaskingEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        allGuiNames.push_back(VolumeSliceInterpolationEdgeEffectsMaskingEnum::toGuiName(iter->enumValue));
    }
    
    if (isSorted) {
        std::sort(allGuiNames.begin(), allGuiNames.end());
    }
}

/**
 * @return The next enum value after the given enum value
 * @param enumValue
 *    Enum value for which next enum value is requested
 */
VolumeSliceInterpolationEdgeEffectsMaskingEnum::Enum
VolumeSliceInterpolationEdgeEffectsMaskingEnum::nextEnum(const Enum enumValue)
{
    int32_t enumIndex(-1);
    const int32_t numEnums = static_cast<int32_t>(enumData.size());
    for (int32_t i = 0; i < numEnums; i++) {
        CaretAssertVectorIndex(enumData, i);
        if (enumData[i].enumValue == enumValue) {
            enumIndex = i + 1;
            break;
        }
    }
    CaretAssertMessage((enumIndex >= 0),
                       ("Invalid input enumValue="
                        + AString::number(static_cast<int32_t>(enumValue))));
    if (enumIndex < 0) {
        return enumValue;
    }
    
    if (enumIndex >= numEnums) {
        enumIndex = 0;
    }
    CaretAssertVectorIndex(enumData, enumIndex);
    
    const Enum nextEnum(enumData[enumIndex].enumValue);
    return nextEnum;
}

