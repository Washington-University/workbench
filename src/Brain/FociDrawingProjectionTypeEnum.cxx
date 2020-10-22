
/*LICENSE_START*/
/*
 *  Copyright (C) 2020 Washington University School of Medicine
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
#define __FOCI_DRAWING_PROJECTION_TYPE_ENUM_DECLARE__
#include "FociDrawingProjectionTypeEnum.h"
#undef __FOCI_DRAWING_PROJECTION_TYPE_ENUM_DECLARE__

#include "CaretAssert.h"

using namespace caret;

    
/**
 * \class caret::FociDrawingProjectionTypeEnum 
 * \brief Type for selection of how foci are drawn using their projectiojs
 *
 * Using this enumerated type in the GUI with an EnumComboBoxTemplate
 * 
 * Header File (.h)
 *     Forward declare the data type:
 *         class EnumComboBoxTemplate;
 * 
 *     Declare the member:
 *         EnumComboBoxTemplate* m_fociDrawingProjectionTypeEnumComboBox;
 * 
 *     Declare a slot that is called when user changes selection
 *         private slots:
 *             void fociDrawingProjectionTypeEnumComboBoxItemActivated();
 * 
 * Implementation File (.cxx)
 *     Include the header files
 *         #include "EnumComboBoxTemplate.h"
 *         #include "FociDrawingProjectionTypeEnum.h"
 * 
 *     Instatiate:
 *         m_fociDrawingProjectionTypeEnumComboBox = new EnumComboBoxTemplate(this);
 *         m_fociDrawingProjectionTypeEnumComboBox->setup<FociDrawingProjectionTypeEnum,FociDrawingProjectionTypeEnum::Enum>();
 * 
 *     Get notified when the user changes the selection: 
 *         QObject::connect(m_fociDrawingProjectionTypeEnumComboBox, SIGNAL(itemActivated()),
 *                          this, SLOT(fociDrawingProjectionTypeEnumComboBoxItemActivated()));
 * 
 *     Update the selection:
 *         m_fociDrawingProjectionTypeEnumComboBox->setSelectedItem<FociDrawingProjectionTypeEnum,FociDrawingProjectionTypeEnum::Enum>(NEW_VALUE);
 * 
 *     Read the selection:
 *         const FociDrawingProjectionTypeEnum::Enum VARIABLE = m_fociDrawingProjectionTypeEnumComboBox->getSelectedItem<FociDrawingProjectionTypeEnum,FociDrawingProjectionTypeEnum::Enum>();
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
FociDrawingProjectionTypeEnum::FociDrawingProjectionTypeEnum(const Enum enumValue,
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
FociDrawingProjectionTypeEnum::~FociDrawingProjectionTypeEnum()
{
}

/**
 * Initialize the enumerated metadata.
 */
void
FociDrawingProjectionTypeEnum::initialize()
{
    if (initializedFlag) {
        return;
    }
    initializedFlag = true;

    enumData.push_back(FociDrawingProjectionTypeEnum(PROJECTED, 
                                    "PROJECTED", 
                                    "Projected"));
    
    enumData.push_back(FociDrawingProjectionTypeEnum(STEREOTAXIC, 
                                    "STEREOTAXIC", 
                                    "Stereotaxic"));
    
}

/**
 * Find the data for and enumerated value.
 * @param enumValue
 *     The enumerated value.
 * @return Pointer to data for this enumerated type
 * or NULL if no data for type or if type is invalid.
 */
const FociDrawingProjectionTypeEnum*
FociDrawingProjectionTypeEnum::findData(const Enum enumValue)
{
    if (initializedFlag == false) initialize();

    size_t num = enumData.size();
    for (size_t i = 0; i < num; i++) {
        const FociDrawingProjectionTypeEnum* d = &enumData[i];
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
FociDrawingProjectionTypeEnum::toName(Enum enumValue) {
    if (initializedFlag == false) initialize();
    
    const FociDrawingProjectionTypeEnum* enumInstance = findData(enumValue);
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
FociDrawingProjectionTypeEnum::Enum 
FociDrawingProjectionTypeEnum::fromName(const AString& name, bool* isValidOut)
{
    if (initializedFlag == false) initialize();
    
    bool validFlag = false;
    Enum enumValue = FociDrawingProjectionTypeEnum::enumData[0].enumValue;
    
    for (std::vector<FociDrawingProjectionTypeEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const FociDrawingProjectionTypeEnum& d = *iter;
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
        CaretAssertMessage(0, AString("Name " + name + "failed to match enumerated value for type FociDrawingProjectionTypeEnum"));
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
FociDrawingProjectionTypeEnum::toGuiName(Enum enumValue) {
    if (initializedFlag == false) initialize();
    
    const FociDrawingProjectionTypeEnum* enumInstance = findData(enumValue);
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
FociDrawingProjectionTypeEnum::Enum 
FociDrawingProjectionTypeEnum::fromGuiName(const AString& guiName, bool* isValidOut)
{
    if (initializedFlag == false) initialize();
    
    bool validFlag = false;
    Enum enumValue = FociDrawingProjectionTypeEnum::enumData[0].enumValue;
    
    for (std::vector<FociDrawingProjectionTypeEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const FociDrawingProjectionTypeEnum& d = *iter;
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
        CaretAssertMessage(0, AString("guiName " + guiName + "failed to match enumerated value for type FociDrawingProjectionTypeEnum"));
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
FociDrawingProjectionTypeEnum::toIntegerCode(Enum enumValue)
{
    if (initializedFlag == false) initialize();
    const FociDrawingProjectionTypeEnum* enumInstance = findData(enumValue);
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
FociDrawingProjectionTypeEnum::Enum
FociDrawingProjectionTypeEnum::fromIntegerCode(const int32_t integerCode, bool* isValidOut)
{
    if (initializedFlag == false) initialize();
    
    bool validFlag = false;
    Enum enumValue = FociDrawingProjectionTypeEnum::enumData[0].enumValue;
    
    for (std::vector<FociDrawingProjectionTypeEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const FociDrawingProjectionTypeEnum& enumInstance = *iter;
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
        CaretAssertMessage(0, AString("Integer code " + AString::number(integerCode) + "failed to match enumerated value for type FociDrawingProjectionTypeEnum"));
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
FociDrawingProjectionTypeEnum::getAllEnums(std::vector<FociDrawingProjectionTypeEnum::Enum>& allEnums)
{
    if (initializedFlag == false) initialize();
    
    allEnums.clear();
    
    for (std::vector<FociDrawingProjectionTypeEnum>::iterator iter = enumData.begin();
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
FociDrawingProjectionTypeEnum::getAllNames(std::vector<AString>& allNames, const bool isSorted)
{
    if (initializedFlag == false) initialize();
    
    allNames.clear();
    
    for (std::vector<FociDrawingProjectionTypeEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        allNames.push_back(FociDrawingProjectionTypeEnum::toName(iter->enumValue));
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
FociDrawingProjectionTypeEnum::getAllGuiNames(std::vector<AString>& allGuiNames, const bool isSorted)
{
    if (initializedFlag == false) initialize();
    
    allGuiNames.clear();
    
    for (std::vector<FociDrawingProjectionTypeEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        allGuiNames.push_back(FociDrawingProjectionTypeEnum::toGuiName(iter->enumValue));
    }
    
    if (isSorted) {
        std::sort(allGuiNames.begin(), allGuiNames.end());
    }
}

/**
 * @return Tooltip for display in the GUI
 */
AString
FociDrawingProjectionTypeEnum::getToolTip()
{
    AString projectedName;
    AString stereotaxicName;
    std::vector<FociDrawingProjectionTypeEnum::Enum> allEnums;
    getAllEnums(allEnums);
    for (auto e : allEnums) {
        switch (e) {
            case PROJECTED:
                projectedName = FociDrawingProjectionTypeEnum::toGuiName(e);
                break;
            case STEREOTAXIC:
                stereotaxicName = FociDrawingProjectionTypeEnum::toGuiName(e);
                break;
        }
    }
    
    AString s("<html>");
    
    s.appendWithNewLine("Display and Compatible Coordinate Type(s): ");
    s.append("<ul>");
    s.appendWithNewLine("<li>All     - " + projectedName + " or " + stereotaxicName);
    s.appendWithNewLine("<li>Montage - " + projectedName);
    s.appendWithNewLine("<li>Surface - " + projectedName);
    s.appendWithNewLine("<li>Volume  - " + stereotaxicName);
    s.appendWithNewLine("</ul>");
    
    if (initializedFlag == false) initialize();
    
    s.appendWithNewLine("Coordinate Types:");
    s.appendWithNewLine("<ul>");
    for (auto e : allEnums) {
        s.appendWithNewLine("<li>"
                            + FociDrawingProjectionTypeEnum::toGuiName(e)
                            + " - ");
        switch (e) {
            case PROJECTED:
                s.append("Show foci that are projected to their displayed surface (surface must be displayed).  "
                         "Projected foci are shown on all surface types (anatomical, inflated, etc.).");
                break;
            case STEREOTAXIC:
                s.append("Show all foci at stereotaxic coordinates.  This mode should be used only "
                         "with no surfaces, anatomical surfaces, or volume slices.");
                break;
        }
    }
    
    s.appendWithNewLine("</ul>");
    s.appendWithNewLine("</html>");
    
    return s;
}

