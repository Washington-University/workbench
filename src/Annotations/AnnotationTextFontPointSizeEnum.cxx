
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
#define __ANNOTATION_TEXT_FONT_POINT_SIZE_ENUM_DECLARE__
#include "AnnotationTextFontPointSizeEnum.h"
#undef __ANNOTATION_TEXT_FONT_POINT_SIZE_ENUM_DECLARE__

#include "CaretAssert.h"

using namespace caret;

    
/**
 * \class caret::AnnotationTextFontPointSizeEnum 
 * \brief Enumerated type for font size.
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
 *         EnumComboBoxTemplate* m_annotationFontSizeEnumComboBox;
 * 
 *     Declare a slot that is called when user changes selection
 *         private slots:
 *             void annotationFontSizeEnumComboBoxItemActivated();
 * 
 * Implementation File (.cxx)
 *     Include the header files
 *         #include "EnumComboBoxTemplate.h"
 *         #include "AnnotationTextFontPointSizeEnum.h"
 * 
 *     Instatiate:
 *         m_annotationFontSizeEnumComboBox = new EnumComboBoxTemplate(this);
 *         m_annotationFontSizeEnumComboBox->setup<AnnotationTextFontPointSizeEnum,AnnotationTextFontPointSizeEnum::Enum>();
 * 
 *     Get notified when the user changes the selection: 
 *         QObject::connect(m_annotationFontSizeEnumComboBox, SIGNAL(itemActivated()),
 *                          this, SLOT(annotationFontSizeEnumComboBoxItemActivated()));
 * 
 *     Update the selection:
 *         m_annotationFontSizeEnumComboBox->setSelectedItem<AnnotationTextFontPointSizeEnum,AnnotationTextFontPointSizeEnum::Enum>(NEW_VALUE);
 * 
 *     Read the selection:
 *         const AnnotationTextFontPointSizeEnum::Enum VARIABLE = m_annotationFontSizeEnumComboBox->getSelectedItem<AnnotationTextFontPointSizeEnum,AnnotationTextFontPointSizeEnum::Enum>();
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
 * @param sizeNumeric
 *    Numerical size of font.
 */
AnnotationTextFontPointSizeEnum::AnnotationTextFontPointSizeEnum(const Enum enumValue,
                           const AString& name,
                                               const AString& guiName,
                                               const int32_t sizeNumeric)
{
    this->enumValue   = enumValue;
    this->integerCode = integerCodeCounter++;
    this->name        = name;
    this->guiName     = guiName;
    this->sizeNumeric = sizeNumeric;
}

/**
 * Destructor.
 */
AnnotationTextFontPointSizeEnum::~AnnotationTextFontPointSizeEnum()
{
}

/**
 * Initialize the enumerated metadata.
 */
void
AnnotationTextFontPointSizeEnum::initialize()
{
    if (initializedFlag) {
        return;
    }
    initializedFlag = true;

    enumData.push_back(AnnotationTextFontPointSizeEnum(SIZE10,
                                              "SIZE10",
                                              "10",
                                              10));
    
    enumData.push_back(AnnotationTextFontPointSizeEnum(SIZE12,
                                              "SIZE12",
                                              "12",
                                              12));
    
    enumData.push_back(AnnotationTextFontPointSizeEnum(SIZE14,
                                              "SIZE14",
                                              "14",
                                              14));
    
    enumData.push_back(AnnotationTextFontPointSizeEnum(SIZE16,
                                              "SIZE16",
                                              "16",
                                              16));
    
    enumData.push_back(AnnotationTextFontPointSizeEnum(SIZE18,
                                              "SIZE18",
                                              "18",
                                              18));
    
    enumData.push_back(AnnotationTextFontPointSizeEnum(SIZE20,
                                              "SIZE20",
                                              "20",
                                              20));
    
    enumData.push_back(AnnotationTextFontPointSizeEnum(SIZE24,
                                              "SIZE24",
                                              "24",
                                              24));
    
    enumData.push_back(AnnotationTextFontPointSizeEnum(SIZE28,
                                              "SIZE28",
                                              "28",
                                              28));
    
    enumData.push_back(AnnotationTextFontPointSizeEnum(SIZE32,
                                              "SIZE32",
                                              "32",
                                              32));
    
    enumData.push_back(AnnotationTextFontPointSizeEnum(SIZE36,
                                              "SIZE36",
                                              "36",
                                              36));
    
    enumData.push_back(AnnotationTextFontPointSizeEnum(SIZE40,
                                              "SIZE40",
                                              "40",
                                              40));
    
    enumData.push_back(AnnotationTextFontPointSizeEnum(SIZE44,
                                              "SIZE44",
                                              "44",
                                              44));
    
    enumData.push_back(AnnotationTextFontPointSizeEnum(SIZE48,
                                              "SIZE48",
                                              "48",
                                              48));
    
    enumData.push_back(AnnotationTextFontPointSizeEnum(SIZE54,
                                              "SIZE54",
                                              "54",
                                              54));
    
    enumData.push_back(AnnotationTextFontPointSizeEnum(SIZE60,
                                              "SIZE60",
                                              "60",
                                              60));
    
    enumData.push_back(AnnotationTextFontPointSizeEnum(SIZE66,
                                              "SIZE66",
                                              "66",
                                              66));
    
    enumData.push_back(AnnotationTextFontPointSizeEnum(SIZE72, 
                                              "SIZE72", 
                                              "72",
                                              72));
    
    for (std::vector<AnnotationTextFontPointSizeEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        if (minimumNumericSize < 0) {
            minimumNumericSize = iter->sizeNumeric;
        }
        else if (iter->sizeNumeric < minimumNumericSize) {
            minimumNumericSize = iter->sizeNumeric;
        }
    }
}

/**
 * Find the data for and enumerated value.
 * @param enumValue
 *     The enumerated value.
 * @return Pointer to data for this enumerated type
 * or NULL if no data for type or if type is invalid.
 */
const AnnotationTextFontPointSizeEnum*
AnnotationTextFontPointSizeEnum::findData(const Enum enumValue)
{
    if (initializedFlag == false) initialize();

    size_t num = enumData.size();
    for (size_t i = 0; i < num; i++) {
        const AnnotationTextFontPointSizeEnum* d = &enumData[i];
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
AnnotationTextFontPointSizeEnum::toName(Enum enumValue) {
    if (initializedFlag == false) initialize();
    
    const AnnotationTextFontPointSizeEnum* enumInstance = findData(enumValue);
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
AnnotationTextFontPointSizeEnum::Enum 
AnnotationTextFontPointSizeEnum::fromName(const AString& name, bool* isValidOut)
{
    if (initializedFlag == false) initialize();
    
    bool validFlag = false;
    Enum enumValue = AnnotationTextFontPointSizeEnum::enumData[0].enumValue;
    
    for (std::vector<AnnotationTextFontPointSizeEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const AnnotationTextFontPointSizeEnum& d = *iter;
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
        CaretAssertMessage(0, AString("Name " + name + "failed to match enumerated value for type AnnotationTextFontPointSizeEnum"));
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
AnnotationTextFontPointSizeEnum::toGuiName(Enum enumValue) {
    if (initializedFlag == false) initialize();
    
    const AnnotationTextFontPointSizeEnum* enumInstance = findData(enumValue);
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
AnnotationTextFontPointSizeEnum::Enum 
AnnotationTextFontPointSizeEnum::fromGuiName(const AString& guiName, bool* isValidOut)
{
    if (initializedFlag == false) initialize();
    
    bool validFlag = false;
    Enum enumValue = AnnotationTextFontPointSizeEnum::enumData[0].enumValue;
    
    for (std::vector<AnnotationTextFontPointSizeEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const AnnotationTextFontPointSizeEnum& d = *iter;
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
        CaretAssertMessage(0, AString("guiName " + guiName + "failed to match enumerated value for type AnnotationTextFontPointSizeEnum"));
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
AnnotationTextFontPointSizeEnum::toIntegerCode(Enum enumValue)
{
    if (initializedFlag == false) initialize();
    const AnnotationTextFontPointSizeEnum* enumInstance = findData(enumValue);
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
AnnotationTextFontPointSizeEnum::Enum
AnnotationTextFontPointSizeEnum::fromIntegerCode(const int32_t integerCode, bool* isValidOut)
{
    if (initializedFlag == false) initialize();
    
    bool validFlag = false;
    Enum enumValue = AnnotationTextFontPointSizeEnum::enumData[0].enumValue;
    
    for (std::vector<AnnotationTextFontPointSizeEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const AnnotationTextFontPointSizeEnum& enumInstance = *iter;
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
        CaretAssertMessage(0, AString("Integer code " + AString::number(integerCode) + "failed to match enumerated value for type AnnotationTextFontPointSizeEnum"));
    }
    return enumValue;
}

/**
 * Get the integer size for a data type.
 *
 * @return
 *    Integer size for data type.
 */
int32_t
AnnotationTextFontPointSizeEnum::toSizeNumeric(Enum enumValue)
{
    if (initializedFlag == false) initialize();
    const AnnotationTextFontPointSizeEnum* enumInstance = findData(enumValue);
    return enumInstance->sizeNumeric;
}

/**
 * @return The minimum font size numeric value.
 */
int32_t
AnnotationTextFontPointSizeEnum::getMinimumSizeNumeric()
{
    if (initializedFlag == false) initialize();
    
    return minimumNumericSize;
}


/**
 * Get all of the enumerated type values.  The values can be used
 * as parameters to toXXX() methods to get associated metadata.
 *
 * @param allEnums
 *     A vector that is OUTPUT containing all of the enumerated values.
 */
void
AnnotationTextFontPointSizeEnum::getAllEnums(std::vector<AnnotationTextFontPointSizeEnum::Enum>& allEnums)
{
    if (initializedFlag == false) initialize();
    
    allEnums.clear();
    
    for (std::vector<AnnotationTextFontPointSizeEnum>::iterator iter = enumData.begin();
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
AnnotationTextFontPointSizeEnum::getAllNames(std::vector<AString>& allNames, const bool isSorted)
{
    if (initializedFlag == false) initialize();
    
    allNames.clear();
    
    for (std::vector<AnnotationTextFontPointSizeEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        allNames.push_back(AnnotationTextFontPointSizeEnum::toName(iter->enumValue));
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
AnnotationTextFontPointSizeEnum::getAllGuiNames(std::vector<AString>& allGuiNames, const bool isSorted)
{
    if (initializedFlag == false) initialize();
    
    allGuiNames.clear();
    
    for (std::vector<AnnotationTextFontPointSizeEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        allGuiNames.push_back(AnnotationTextFontPointSizeEnum::toGuiName(iter->enumValue));
    }
    
    if (isSorted) {
        std::sort(allGuiNames.begin(), allGuiNames.end());
    }
}

