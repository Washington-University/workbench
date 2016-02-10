
/*LICENSE_START*/
/*
 *  Copyright (C) 2015 Washington University School of Medicine
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
#define __ANNOTATION_COORDINATE_SPACE_ENUM_DECLARE__
#include "AnnotationCoordinateSpaceEnum.h"
#undef __ANNOTATION_COORDINATE_SPACE_ENUM_DECLARE__

#include "CaretAssert.h"

using namespace caret;

    
/**
 * \class caret::AnnotationCoordinateSpaceEnum 
 * \brief Coordinate space of annotation.
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
 *         EnumComboBoxTemplate* m_annotationCoordinateSpaceEnumComboBox;
 * 
 *     Declare a slot that is called when user changes selection
 *         private slots:
 *             void annotationCoordinateSpaceEnumComboBoxItemActivated();
 * 
 * Implementation File (.cxx)
 *     Include the header files
 *         #include "EnumComboBoxTemplate.h"
 *         #include "AnnotationCoordinateSpaceEnum.h"
 * 
 *     Instatiate:
 *         m_annotationCoordinateSpaceEnumComboBox = new EnumComboBoxTemplate(this);
 *         m_annotationCoordinateSpaceEnumComboBox->setup<AnnotationCoordinateSpaceEnum,AnnotationCoordinateSpaceEnum::Enum>();
 * 
 *     Get notified when the user changes the selection: 
 *         QObject::connect(m_annotationCoordinateSpaceEnumComboBox, SIGNAL(itemActivated()),
 *                          this, SLOT(annotationCoordinateSpaceEnumComboBoxItemActivated()));
 * 
 *     Update the selection:
 *         m_annotationCoordinateSpaceEnumComboBox->setSelectedItem<AnnotationCoordinateSpaceEnum,AnnotationCoordinateSpaceEnum::Enum>(NEW_VALUE);
 * 
 *     Read the selection:
 *         const AnnotationCoordinateSpaceEnum::Enum VARIABLE = m_annotationCoordinateSpaceEnumComboBox->getSelectedItem<AnnotationCoordinateSpaceEnum,AnnotationCoordinateSpaceEnum::Enum>();
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
 * @param guiAbbreviatedName
 *    Abbreviated User-friendly name for use in user-interface.
 */
AnnotationCoordinateSpaceEnum::AnnotationCoordinateSpaceEnum(const Enum enumValue,
                                                             const AString& name,
                                                             const AString& guiName,
                                                             const AString& guiAbbreviatedName)
{
    this->enumValue = enumValue;
    this->integerCode = integerCodeCounter++;
    this->name = name;
    this->guiName = guiName;
    this->guiAbbreviatedName = guiAbbreviatedName;
}

/**
 * Destructor.
 */
AnnotationCoordinateSpaceEnum::~AnnotationCoordinateSpaceEnum()
{
}

/**
 * Initialize the enumerated metadata.
 */
void
AnnotationCoordinateSpaceEnum::initialize()
{
    if (initializedFlag) {
        return;
    }
    initializedFlag = true;

    enumData.push_back(AnnotationCoordinateSpaceEnum(PIXELS,
                                                     "PIXELS",
                                                     "Pixels",
                                                     "P"));
    
    enumData.push_back(AnnotationCoordinateSpaceEnum(STEREOTAXIC,
                                                     "STEREOTAXIC",
                                                     "Stereotaxic",
                                                     "St"));
    
    enumData.push_back(AnnotationCoordinateSpaceEnum(SURFACE,
                                                     "SURFACE",
                                                     "Surface",
                                                     "Sf"));
    
    enumData.push_back(AnnotationCoordinateSpaceEnum(TAB,
                                                     "TAB",
                                                     "Tab",
                                                     "T"));
    
    enumData.push_back(AnnotationCoordinateSpaceEnum(WINDOW,
                                                     "WINDOW",
                                                     "Window",
                                                     "W"));
}

/**
 * Find the data for and enumerated value.
 * @param enumValue
 *     The enumerated value.
 * @return Pointer to data for this enumerated type
 * or NULL if no data for type or if type is invalid.
 */
const AnnotationCoordinateSpaceEnum*
AnnotationCoordinateSpaceEnum::findData(const Enum enumValue)
{
    if (initializedFlag == false) initialize();

    size_t num = enumData.size();
    for (size_t i = 0; i < num; i++) {
        const AnnotationCoordinateSpaceEnum* d = &enumData[i];
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
AnnotationCoordinateSpaceEnum::toName(Enum enumValue) {
    if (initializedFlag == false) initialize();
    
    const AnnotationCoordinateSpaceEnum* enumInstance = findData(enumValue);
    return enumInstance->name;
}

/**
 * Get an enumerated value corresponding to its name.
 * @param name In
 *     Name of enumerated value.
 * @param isValidOut 
 *     If not NULL, it is set indicating that a
 *     enum value exists for the input name.
 * @return 
 *     Enumerated value.
 */
AnnotationCoordinateSpaceEnum::Enum 
AnnotationCoordinateSpaceEnum::fromName(const AString& nameIn, bool* isValidOut)
{
    if (initializedFlag == false) initialize();
    
    AString name = nameIn;
    if (name == "MODEL") {
        name = "STEREOTAXIC";
    }
    
    bool validFlag = false;
    Enum enumValue = AnnotationCoordinateSpaceEnum::enumData[0].enumValue;
    
    for (std::vector<AnnotationCoordinateSpaceEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const AnnotationCoordinateSpaceEnum& d = *iter;
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
        CaretAssertMessage(0, AString("Name " + name + "failed to match enumerated value for type AnnotationCoordinateSpaceEnum"));
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
AnnotationCoordinateSpaceEnum::toGuiName(Enum enumValue) {
    if (initializedFlag == false) initialize();
    
    const AnnotationCoordinateSpaceEnum* enumInstance = findData(enumValue);
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
AnnotationCoordinateSpaceEnum::Enum 
AnnotationCoordinateSpaceEnum::fromGuiName(const AString& guiNameIn, bool* isValidOut)
{
    if (initializedFlag == false) initialize();
 
    AString guiName = guiNameIn;
    if (guiName == "Model") {
        guiName = "Stereotaxic";
    }
    
    bool validFlag = false;
    Enum enumValue = AnnotationCoordinateSpaceEnum::enumData[0].enumValue;
    
    for (std::vector<AnnotationCoordinateSpaceEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const AnnotationCoordinateSpaceEnum& d = *iter;
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
        CaretAssertMessage(0, AString("guiName " + guiName + "failed to match enumerated value for type AnnotationCoordinateSpaceEnum"));
    }
    return enumValue;
}

/**
 * Get a GUI abbreviated string representation of the enumerated type.
 * @param enumValue
 *     Enumerated value.
 * @return
 *     String representing enumerated value.
 */
AString
AnnotationCoordinateSpaceEnum::toGuiAbbreviatedName(Enum enumValue) {
    if (initializedFlag == false) initialize();
    
    const AnnotationCoordinateSpaceEnum* enumInstance = findData(enumValue);
    return enumInstance->guiAbbreviatedName;
}

/**
 * Get the integer code for a data type.
 *
 * @return
 *    Integer code for data type.
 */
int32_t
AnnotationCoordinateSpaceEnum::toIntegerCode(Enum enumValue)
{
    if (initializedFlag == false) initialize();
    const AnnotationCoordinateSpaceEnum* enumInstance = findData(enumValue);
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
AnnotationCoordinateSpaceEnum::Enum
AnnotationCoordinateSpaceEnum::fromIntegerCode(const int32_t integerCode, bool* isValidOut)
{
    if (initializedFlag == false) initialize();
    
    bool validFlag = false;
    Enum enumValue = AnnotationCoordinateSpaceEnum::enumData[0].enumValue;
    
    for (std::vector<AnnotationCoordinateSpaceEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const AnnotationCoordinateSpaceEnum& enumInstance = *iter;
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
        CaretAssertMessage(0, AString("Integer code " + AString::number(integerCode) + "failed to match enumerated value for type AnnotationCoordinateSpaceEnum"));
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
AnnotationCoordinateSpaceEnum::getAllEnums(std::vector<AnnotationCoordinateSpaceEnum::Enum>& allEnums)
{
    if (initializedFlag == false) initialize();
    
    allEnums.clear();
    
    for (std::vector<AnnotationCoordinateSpaceEnum>::iterator iter = enumData.begin();
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
AnnotationCoordinateSpaceEnum::getAllNames(std::vector<AString>& allNames, const bool isSorted)
{
    if (initializedFlag == false) initialize();
    
    allNames.clear();
    
    for (std::vector<AnnotationCoordinateSpaceEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        allNames.push_back(AnnotationCoordinateSpaceEnum::toName(iter->enumValue));
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
AnnotationCoordinateSpaceEnum::getAllGuiNames(std::vector<AString>& allGuiNames, const bool isSorted)
{
    if (initializedFlag == false) initialize();
    
    allGuiNames.clear();
    
    for (std::vector<AnnotationCoordinateSpaceEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        allGuiNames.push_back(AnnotationCoordinateSpaceEnum::toGuiName(iter->enumValue));
    }
    
    if (isSorted) {
        std::sort(allGuiNames.begin(), allGuiNames.end());
    }
}

