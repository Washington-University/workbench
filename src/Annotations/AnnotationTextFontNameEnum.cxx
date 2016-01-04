
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
#define __ANNOTATION_TEXT_FONT_NAME_ENUM_DECLARE__
#include "AnnotationTextFontNameEnum.h"
#undef __ANNOTATION_TEXT_FONT_NAME_ENUM_DECLARE__

#include "CaretAssert.h"

using namespace caret;

    
/**
 * \class caret::AnnotationTextFontNameEnum 
 * \brief Names of annotation fonts
 *
 * Using this enumerated type in the GUI with an EnumComboBoxTemplate
 * 
 * Header File (.h)
 *     Forward declare the data type:
 *         class EnumComboBoxTemplate;
 * 
 *     Declare the member:
 *         EnumComboBoxTemplate* m_annotationFontNameEnumComboBox;
 * 
 *     Declare a slot that is called when user changes selection
 *         private slots:
 *             void annotationFontNameEnumComboBoxItemActivated();
 * 
 * Implementation File (.cxx)
 *     Include the header files
 *         #include "EnumComboBoxTemplate.h"
 *         #include "AnnotationTextFontNameEnum.h"
 * 
 *     Instatiate:
 *         m_annotationFontNameEnumComboBox = new EnumComboBoxTemplate(this);
 *         m_annotationFontNameEnumComboBox->setup<AnnotationTextFontNameEnum,AnnotationTextFontNameEnum::Enum>();
 * 
 *     Get notified when the user changes the selection: 
 *         QObject::connect(m_annotationFontNameEnumComboBox, SIGNAL(itemActivated()),
 *                          this, SLOT(annotationFontNameEnumComboBoxItemActivated()));
 * 
 *     Update the selection:
 *         m_annotationFontNameEnumComboBox->setSelectedItem<AnnotationTextFontNameEnum,AnnotationTextFontNameEnum::Enum>(NEW_VALUE);
 * 
 *     Read the selection:
 *         const AnnotationTextFontNameEnum::Enum VARIABLE = m_annotationFontNameEnumComboBox->getSelectedItem<AnnotationTextFontNameEnum,AnnotationTextFontNameEnum::Enum>();
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
AnnotationTextFontNameEnum::AnnotationTextFontNameEnum(const Enum enumValue,
                                               const AString& name,
                                               const AString& guiName,
                                               const AString& fontFileName,
                                               const AString& boldFontFileName,
                                               const AString& boldItalicFontFileName,
                                               const AString& italicFontFileName)
{
    this->enumValue = enumValue;
    this->integerCode = integerCodeCounter++;
    this->name = name;
    this->guiName = guiName;
    this->resourceFontFileName           = fontFileName;
    this->resourceBoldFontFileName       = boldFontFileName;
    this->resourceBoldItalicFontFileName = boldItalicFontFileName;
    this->resourceItalicFontFileName     = italicFontFileName;
}

/**
 * Destructor.
 */
AnnotationTextFontNameEnum::~AnnotationTextFontNameEnum()
{
}

/**
 * Initialize the enumerated metadata.
 */
void
AnnotationTextFontNameEnum::initialize()
{
    if (initializedFlag) {
        return;
    }
    initializedFlag = true;

    enumData.push_back(AnnotationTextFontNameEnum(OUTLINE,
                                                  "OUTLINE",
                                                  "Outline",
                                                  ":/Fonts/Outline/fenwick_ol_free.ttf",
                                                  ":/Fonts/Outline/fenwick_ol_free.ttf",
                                                  ":/Fonts/Outline/fenwick_ol_free.ttf",
                                                  ":/Fonts/Outline/fenwick_ol_free.ttf"));
    
   enumData.push_back(AnnotationTextFontNameEnum(VERA,
                                                  "VERA",
                                                  "Vera",
                                                  ":/Fonts/VeraFonts/Vera.ttf",
                                                  ":/Fonts/VeraFonts/VeraBd.ttf",
                                                  ":/Fonts/VeraFonts/VeraBI.ttf",
                                                  ":/Fonts/VeraFonts/VeraIt.ttf"));
    
    enumData.push_back(AnnotationTextFontNameEnum(VERA_MONOSPACE,
                                              "VERA_MONOSPACE",
                                              "Vera Mono",
                                              ":/Fonts/VeraFonts/VeraMono.ttf",
                                              ":/Fonts/VeraFonts/VeraMoBd.ttf",
                                              ":/Fonts/VeraFonts/VeraMoBI.ttf",
                                              ":/Fonts/VeraFonts/VeraMoIt.ttf"));
}

/**
 * @return The default font name.
 */
AnnotationTextFontNameEnum::Enum
AnnotationTextFontNameEnum::getDefaultFontName()
{
    if (initializedFlag == false) initialize();
    
    CaretAssert( ! enumData.empty());
    
    return AnnotationTextFontNameEnum::VERA;
//    return enumData[0].enumValue;
}

/**
 * Find the data for and enumerated value.
 * @param enumValue
 *     The enumerated value.
 * @return Pointer to data for this enumerated type
 * or NULL if no data for type or if type is invalid.
 */
const AnnotationTextFontNameEnum*
AnnotationTextFontNameEnum::findData(const Enum enumValue)
{
    if (initializedFlag == false) initialize();

    size_t num = enumData.size();
    for (size_t i = 0; i < num; i++) {
        const AnnotationTextFontNameEnum* d = &enumData[i];
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
AnnotationTextFontNameEnum::toName(Enum enumValue) {
    if (initializedFlag == false) initialize();
    
    const AnnotationTextFontNameEnum* enumInstance = findData(enumValue);
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
AnnotationTextFontNameEnum::Enum 
AnnotationTextFontNameEnum::fromName(const AString& name, bool* isValidOut)
{
    if (initializedFlag == false) initialize();
    
    bool validFlag = false;
    Enum enumValue = getDefaultFontName();
    
    for (std::vector<AnnotationTextFontNameEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const AnnotationTextFontNameEnum& d = *iter;
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
        CaretAssertMessage(0, AString("Name " + name + "failed to match enumerated value for type AnnotationTextFontNameEnum"));
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
AnnotationTextFontNameEnum::toGuiName(Enum enumValue) {
    if (initializedFlag == false) initialize();
    
    const AnnotationTextFontNameEnum* enumInstance = findData(enumValue);
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
AnnotationTextFontNameEnum::Enum 
AnnotationTextFontNameEnum::fromGuiName(const AString& guiName, bool* isValidOut)
{
    if (initializedFlag == false) initialize();
    
    bool validFlag = false;
    Enum enumValue = getDefaultFontName();
    
    for (std::vector<AnnotationTextFontNameEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const AnnotationTextFontNameEnum& d = *iter;
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
        CaretAssertMessage(0, AString("guiName " + guiName + "failed to match enumerated value for type AnnotationTextFontNameEnum"));
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
AnnotationTextFontNameEnum::toIntegerCode(Enum enumValue)
{
    if (initializedFlag == false) initialize();
    const AnnotationTextFontNameEnum* enumInstance = findData(enumValue);
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
AnnotationTextFontNameEnum::Enum
AnnotationTextFontNameEnum::fromIntegerCode(const int32_t integerCode, bool* isValidOut)
{
    if (initializedFlag == false) initialize();
    
    bool validFlag = false;
    Enum enumValue = getDefaultFontName();
    
    for (std::vector<AnnotationTextFontNameEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const AnnotationTextFontNameEnum& enumInstance = *iter;
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
        CaretAssertMessage(0, AString("Integer code " + AString::number(integerCode) + "failed to match enumerated value for type AnnotationTextFontNameEnum"));
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
AnnotationTextFontNameEnum::getAllEnums(std::vector<AnnotationTextFontNameEnum::Enum>& allEnums)
{
    if (initializedFlag == false) initialize();
    
    allEnums.clear();
    
    for (std::vector<AnnotationTextFontNameEnum>::iterator iter = enumData.begin();
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
AnnotationTextFontNameEnum::getAllNames(std::vector<AString>& allNames, const bool isSorted)
{
    if (initializedFlag == false) initialize();
    
    allNames.clear();
    
    for (std::vector<AnnotationTextFontNameEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        allNames.push_back(AnnotationTextFontNameEnum::toName(iter->enumValue));
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
AnnotationTextFontNameEnum::getAllGuiNames(std::vector<AString>& allGuiNames, const bool isSorted)
{
    if (initializedFlag == false) initialize();
    
    allGuiNames.clear();
    
    for (std::vector<AnnotationTextFontNameEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        allGuiNames.push_back(AnnotationTextFontNameEnum::toGuiName(iter->enumValue));
    }
    
    if (isSorted) {
        std::sort(allGuiNames.begin(), allGuiNames.end());
    }
}

/**
 * Get the resource file name for the regular font file.
 * 
 * @param enumValue
 *     Enumerated value.
 * @return
 *     String containing name of font file in resources.
 */
AString
AnnotationTextFontNameEnum::getResourceFontFileName(Enum enumValue)
{
    if (initializedFlag == false) initialize();
    const AnnotationTextFontNameEnum* enumInstance = findData(enumValue);
    return enumInstance->resourceFontFileName;
}

/**
 * Get the resource file name for the bold font file.
 *
 * @param enumValue
 *     Enumerated value.
 * @return
 *     String containing name of font file in resources.
 */
AString
AnnotationTextFontNameEnum::getResourceBoldFontFileName(Enum enumValue)
{
    if (initializedFlag == false) initialize();
    const AnnotationTextFontNameEnum* enumInstance = findData(enumValue);
    return enumInstance->resourceBoldFontFileName;
}

/**
 * Get the resource file name for the bold italic font file.
 *
 * @param enumValue
 *     Enumerated value.
 * @return
 *     String containing name of font file in resources.
 */
AString
AnnotationTextFontNameEnum::getResourceBoldItalicFontFileName(Enum enumValue)
{
    if (initializedFlag == false) initialize();
    const AnnotationTextFontNameEnum* enumInstance = findData(enumValue);
    return enumInstance->resourceBoldItalicFontFileName;
}

/**
 * Get the resource file name for the italic font file.
 *
 * @param enumValue
 *     Enumerated value.
 * @return
 *     String containing name of font file in resources.
 */
AString
AnnotationTextFontNameEnum::getResourceItalicFontFileName(Enum enumValue)
{
    if (initializedFlag == false) initialize();
    const AnnotationTextFontNameEnum* enumInstance = findData(enumValue);
    return enumInstance->resourceItalicFontFileName;
}


