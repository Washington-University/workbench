
/*LICENSE_START*/
/*
 *  Copyright (C) 2024 Washington University School of Medicine
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
#define __VOLUME_SURFACE_OUTLINE_DRAWING_MODE_ENUM_DECLARE__
#include "VolumeSurfaceOutlineDrawingModeEnum.h"
#undef __VOLUME_SURFACE_OUTLINE_DRAWING_MODE_ENUM_DECLARE__

#include "CaretAssert.h"

using namespace caret;

    
/**
 * \class caret::VolumeSurfaceOutlineDrawingModeEnum 
 * \brief <REPLACE-WITH-ONE-LINE-DESCRIPTION>
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
 *         EnumComboBoxTemplate* m_volumeSurfaceOutlineDrawingModeEnumComboBox;
 * 
 *     Declare a slot that is called when user changes selection
 *         private slots:
 *             void volumeSurfaceOutlineDrawingModeEnumComboBoxItemActivated();
 * 
 * Implementation File (.cxx)
 *     Include the header files
 *         #include "EnumComboBoxTemplate.h"
 *         #include "VolumeSurfaceOutlineDrawingModeEnum.h"
 * 
 *     Instatiate:
 *         m_volumeSurfaceOutlineDrawingModeEnumComboBox = new EnumComboBoxTemplate(this);
 *         m_volumeSurfaceOutlineDrawingModeEnumComboBox->setup<VolumeSurfaceOutlineDrawingModeEnum,VolumeSurfaceOutlineDrawingModeEnum::Enum>();
 * 
 *     Get notified when the user changes the selection: 
 *         QObject::connect(m_volumeSurfaceOutlineDrawingModeEnumComboBox, SIGNAL(itemActivated()),
 *                          this, SLOT(volumeSurfaceOutlineDrawingModeEnumComboBoxItemActivated()));
 * 
 *     Update the selection:
 *         m_volumeSurfaceOutlineDrawingModeEnumComboBox->setSelectedItem<VolumeSurfaceOutlineDrawingModeEnum,VolumeSurfaceOutlineDrawingModeEnum::Enum>(NEW_VALUE);
 * 
 *     Read the selection:
 *         const VolumeSurfaceOutlineDrawingModeEnum::Enum VARIABLE = m_volumeSurfaceOutlineDrawingModeEnumComboBox->getSelectedItem<VolumeSurfaceOutlineDrawingModeEnum,VolumeSurfaceOutlineDrawingModeEnum::Enum>();
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
VolumeSurfaceOutlineDrawingModeEnum::VolumeSurfaceOutlineDrawingModeEnum(const Enum enumValue,
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
VolumeSurfaceOutlineDrawingModeEnum::~VolumeSurfaceOutlineDrawingModeEnum()
{
}

/**
 * Initialize the enumerated metadata.
 */
void
VolumeSurfaceOutlineDrawingModeEnum::initialize()
{
    if (initializedFlag) {
        return;
    }
    initializedFlag = true;

    enumData.push_back(VolumeSurfaceOutlineDrawingModeEnum(LINES, 
                                    "LINES", 
                                    "Lines"));
    
    enumData.push_back(VolumeSurfaceOutlineDrawingModeEnum(SURFACE, 
                                    "SURFACE", 
                                    "Surface"));
    
    enumData.push_back(VolumeSurfaceOutlineDrawingModeEnum(BOTH, 
                                    "BOTH", 
                                    "Both"));
    
}

/**
 * Find the data for and enumerated value.
 * @param enumValue
 *     The enumerated value.
 * @return Pointer to data for this enumerated type
 * or NULL if no data for type or if type is invalid.
 */
const VolumeSurfaceOutlineDrawingModeEnum*
VolumeSurfaceOutlineDrawingModeEnum::findData(const Enum enumValue)
{
    if (initializedFlag == false) initialize();

    size_t num = enumData.size();
    for (size_t i = 0; i < num; i++) {
        const VolumeSurfaceOutlineDrawingModeEnum* d = &enumData[i];
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
VolumeSurfaceOutlineDrawingModeEnum::toName(Enum enumValue) {
    if (initializedFlag == false) initialize();
    
    const VolumeSurfaceOutlineDrawingModeEnum* enumInstance = findData(enumValue);
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
VolumeSurfaceOutlineDrawingModeEnum::Enum 
VolumeSurfaceOutlineDrawingModeEnum::fromName(const AString& name, bool* isValidOut)
{
    if (initializedFlag == false) initialize();
    
    bool validFlag = false;
    Enum enumValue = VolumeSurfaceOutlineDrawingModeEnum::enumData[0].enumValue;
    
    for (std::vector<VolumeSurfaceOutlineDrawingModeEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const VolumeSurfaceOutlineDrawingModeEnum& d = *iter;
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
        CaretAssertMessage(0, AString("Name " + name + " failed to match enumerated value for type VolumeSurfaceOutlineDrawingModeEnum"));
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
VolumeSurfaceOutlineDrawingModeEnum::toGuiName(Enum enumValue) {
    if (initializedFlag == false) initialize();
    
    const VolumeSurfaceOutlineDrawingModeEnum* enumInstance = findData(enumValue);
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
VolumeSurfaceOutlineDrawingModeEnum::Enum 
VolumeSurfaceOutlineDrawingModeEnum::fromGuiName(const AString& guiName, bool* isValidOut)
{
    if (initializedFlag == false) initialize();
    
    bool validFlag = false;
    Enum enumValue = VolumeSurfaceOutlineDrawingModeEnum::enumData[0].enumValue;
    
    for (std::vector<VolumeSurfaceOutlineDrawingModeEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const VolumeSurfaceOutlineDrawingModeEnum& d = *iter;
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
        CaretAssertMessage(0, AString("guiName " + guiName + " failed to match enumerated value for type VolumeSurfaceOutlineDrawingModeEnum"));
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
VolumeSurfaceOutlineDrawingModeEnum::toIntegerCode(Enum enumValue)
{
    if (initializedFlag == false) initialize();
    const VolumeSurfaceOutlineDrawingModeEnum* enumInstance = findData(enumValue);
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
VolumeSurfaceOutlineDrawingModeEnum::Enum
VolumeSurfaceOutlineDrawingModeEnum::fromIntegerCode(const int32_t integerCode, bool* isValidOut)
{
    if (initializedFlag == false) initialize();
    
    bool validFlag = false;
    Enum enumValue = VolumeSurfaceOutlineDrawingModeEnum::enumData[0].enumValue;
    
    for (std::vector<VolumeSurfaceOutlineDrawingModeEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const VolumeSurfaceOutlineDrawingModeEnum& enumInstance = *iter;
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
        CaretAssertMessage(0, AString("Integer code " + AString::number(integerCode) + " failed to match enumerated value for type VolumeSurfaceOutlineDrawingModeEnum"));
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
VolumeSurfaceOutlineDrawingModeEnum::getAllEnums(std::vector<VolumeSurfaceOutlineDrawingModeEnum::Enum>& allEnums)
{
    if (initializedFlag == false) initialize();
    
    allEnums.clear();
    
    for (std::vector<VolumeSurfaceOutlineDrawingModeEnum>::iterator iter = enumData.begin();
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
VolumeSurfaceOutlineDrawingModeEnum::getAllNames(std::vector<AString>& allNames, const bool isSorted)
{
    if (initializedFlag == false) initialize();
    
    allNames.clear();
    
    for (std::vector<VolumeSurfaceOutlineDrawingModeEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        allNames.push_back(VolumeSurfaceOutlineDrawingModeEnum::toName(iter->enumValue));
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
VolumeSurfaceOutlineDrawingModeEnum::getAllGuiNames(std::vector<AString>& allGuiNames, const bool isSorted)
{
    if (initializedFlag == false) initialize();
    
    allGuiNames.clear();
    
    for (std::vector<VolumeSurfaceOutlineDrawingModeEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        allGuiNames.push_back(VolumeSurfaceOutlineDrawingModeEnum::toGuiName(iter->enumValue));
    }
    
    if (isSorted) {
        std::sort(allGuiNames.begin(), allGuiNames.end());
    }
}

/**
 * @return tooltip for this type
 */
AString
VolumeSurfaceOutlineDrawingModeEnum::getToolTip()
{
    if (initializedFlag == false) initialize();
    
    std::vector<Enum> allEnums;
    getAllEnums(allEnums);
    
    AString text("<html>Outline drawing mode:");
    text.append("<ul>");
    for (auto& e : allEnums) {
        AString s("<li> " + toGuiName(e) + ": ");
        switch (e) {
            case BOTH:
                s.append("Draw as both "
                         + toGuiName(LINES)
                         + " and "
                         + toGuiName(SURFACE)
                         + ".");
                break;
            case LINES:
                s.append("Draw as lines, may get gaps.");
                break;
            case SURFACE:
                s.append("Draw part of surface within 'depth' distance from volume slice (eliminates gaps).  Some parts may be very thin.  DEPTH MUST NOT BE ZERO.");
                break;
        }
        text.append(s);
    }
    text.append("</ul></html>");
    
    return text;
}

