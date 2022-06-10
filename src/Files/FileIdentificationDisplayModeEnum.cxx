
/*LICENSE_START*/
/*
 *  Copyright (C) 2022 Washington University School of Medicine
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
#define __FILE_IDENTIFICATION_DISPLAY_MODE_ENUM_DECLARE__
#include "FileIdentificationDisplayModeEnum.h"
#undef __FILE_IDENTIFICATION_DISPLAY_MODE_ENUM_DECLARE__

#include "CaretAssert.h"

using namespace caret;

    
/**
 * \class caret::FileIdentificationDisplayModeEnum 
 * \brief Mode for displaying a file's identification
 *
 * Using this enumerated type in the GUI with an EnumComboBoxTemplate
 * 
 * Header File (.h)
 *     Forward declare the data type:
 *         class EnumComboBoxTemplate;
 * 
 *     Declare the member:
 *         EnumComboBoxTemplate* m_fileIdentificationDisplayModeEnumComboBox;
 * 
 *     Declare a slot that is called when user changes selection
 *         private slots:
 *             void fileIdentificationDisplayModeEnumComboBoxItemActivated();
 * 
 * Implementation File (.cxx)
 *     Include the header files
 *         #include "EnumComboBoxTemplate.h"
 *         #include "FileIdentificationDisplayModeEnum.h"
 * 
 *     Instatiate:
 *         m_fileIdentificationDisplayModeEnumComboBox = new EnumComboBoxTemplate(this);
 *         m_fileIdentificationDisplayModeEnumComboBox->setup<FileIdentificationDisplayModeEnum,FileIdentificationDisplayModeEnum::Enum>();
 * 
 *     Get notified when the user changes the selection: 
 *         QObject::connect(m_fileIdentificationDisplayModeEnumComboBox, SIGNAL(itemActivated()),
 *                          this, SLOT(fileIdentificationDisplayModeEnumComboBoxItemActivated()));
 * 
 *     Update the selection:
 *         m_fileIdentificationDisplayModeEnumComboBox->setSelectedItem<FileIdentificationDisplayModeEnum,FileIdentificationDisplayModeEnum::Enum>(NEW_VALUE);
 * 
 *     Read the selection:
 *         const FileIdentificationDisplayModeEnum::Enum VARIABLE = m_fileIdentificationDisplayModeEnumComboBox->getSelectedItem<FileIdentificationDisplayModeEnum,FileIdentificationDisplayModeEnum::Enum>();
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
 * @param toolTip
 *    Tooltip for use in GUI
 */
FileIdentificationDisplayModeEnum::FileIdentificationDisplayModeEnum(const Enum enumValue,
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
FileIdentificationDisplayModeEnum::~FileIdentificationDisplayModeEnum()
{
}

/**
 * Initialize the enumerated metadata.
 */
void
FileIdentificationDisplayModeEnum::initialize()
{
    if (initializedFlag) {
        return;
    }
    initializedFlag = true;

    enumData.push_back(FileIdentificationDisplayModeEnum(ALWAYS,
                                                         "ALWAYS",
                                                         "Always",
                                                         "Always display information from file"));
    
    enumData.push_back(FileIdentificationDisplayModeEnum(OVERLAY,
                                                         "OVERLAY",
                                                         "Overlay",
                                                         "Display information from file if file is in an enabled overlay"));
    
    enumData.push_back(FileIdentificationDisplayModeEnum(NEVER,
                                                         "NEVER",
                                                         "Never",
                                                         "Never display information from file"));
}

/**
 * Find the data for and enumerated value.
 * @param enumValue
 *     The enumerated value.
 * @return Pointer to data for this enumerated type
 * or NULL if no data for type or if type is invalid.
 */
const FileIdentificationDisplayModeEnum*
FileIdentificationDisplayModeEnum::findData(const Enum enumValue)
{
    if (initializedFlag == false) initialize();

    size_t num = enumData.size();
    for (size_t i = 0; i < num; i++) {
        const FileIdentificationDisplayModeEnum* d = &enumData[i];
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
FileIdentificationDisplayModeEnum::toName(Enum enumValue) {
    if (initializedFlag == false) initialize();
    
    const FileIdentificationDisplayModeEnum* enumInstance = findData(enumValue);
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
FileIdentificationDisplayModeEnum::Enum 
FileIdentificationDisplayModeEnum::fromName(const AString& name, bool* isValidOut)
{
    if (initializedFlag == false) initialize();
    
    bool validFlag = false;
    Enum enumValue = FileIdentificationDisplayModeEnum::enumData[0].enumValue;
    
    for (std::vector<FileIdentificationDisplayModeEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const FileIdentificationDisplayModeEnum& d = *iter;
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
        CaretAssertMessage(0, AString("Name " + name + " failed to match enumerated value for type FileIdentificationDisplayModeEnum"));
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
FileIdentificationDisplayModeEnum::toGuiName(Enum enumValue) {
    if (initializedFlag == false) initialize();
    
    const FileIdentificationDisplayModeEnum* enumInstance = findData(enumValue);
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
FileIdentificationDisplayModeEnum::Enum 
FileIdentificationDisplayModeEnum::fromGuiName(const AString& guiName, bool* isValidOut)
{
    if (initializedFlag == false) initialize();
    
    bool validFlag = false;
    Enum enumValue = FileIdentificationDisplayModeEnum::enumData[0].enumValue;
    
    for (std::vector<FileIdentificationDisplayModeEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const FileIdentificationDisplayModeEnum& d = *iter;
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
        CaretAssertMessage(0, AString("guiName " + guiName + " failed to match enumerated value for type FileIdentificationDisplayModeEnum"));
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
FileIdentificationDisplayModeEnum::toIntegerCode(Enum enumValue)
{
    if (initializedFlag == false) initialize();
    const FileIdentificationDisplayModeEnum* enumInstance = findData(enumValue);
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
FileIdentificationDisplayModeEnum::Enum
FileIdentificationDisplayModeEnum::fromIntegerCode(const int32_t integerCode, bool* isValidOut)
{
    if (initializedFlag == false) initialize();
    
    bool validFlag = false;
    Enum enumValue = FileIdentificationDisplayModeEnum::enumData[0].enumValue;
    
    for (std::vector<FileIdentificationDisplayModeEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const FileIdentificationDisplayModeEnum& enumInstance = *iter;
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
        CaretAssertMessage(0, AString("Integer code " + AString::number(integerCode) + " failed to match enumerated value for type FileIdentificationDisplayModeEnum"));
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
FileIdentificationDisplayModeEnum::getAllEnums(std::vector<FileIdentificationDisplayModeEnum::Enum>& allEnums)
{
    if (initializedFlag == false) initialize();
    
    allEnums.clear();
    
    for (std::vector<FileIdentificationDisplayModeEnum>::iterator iter = enumData.begin();
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
FileIdentificationDisplayModeEnum::getAllNames(std::vector<AString>& allNames, const bool isSorted)
{
    if (initializedFlag == false) initialize();
    
    allNames.clear();
    
    for (std::vector<FileIdentificationDisplayModeEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        allNames.push_back(FileIdentificationDisplayModeEnum::toName(iter->enumValue));
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
FileIdentificationDisplayModeEnum::getAllGuiNames(std::vector<AString>& allGuiNames, const bool isSorted)
{
    if (initializedFlag == false) initialize();
    
    allGuiNames.clear();
    
    for (std::vector<FileIdentificationDisplayModeEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        allGuiNames.push_back(FileIdentificationDisplayModeEnum::toGuiName(iter->enumValue));
    }
    
    if (isSorted) {
        std::sort(allGuiNames.begin(), allGuiNames.end());
    }
}

/**
 * Get a tooltip of the enumerated type.
 * @param enumValue
 *     Enumerated value.
 * @return
 *     String representing enumerated value.
 */
AString
FileIdentificationDisplayModeEnum::toToolTip(Enum enumValue)
{
    if (initializedFlag == false) initialize();
    
    const FileIdentificationDisplayModeEnum* enumInstance = findData(enumValue);
    return enumInstance->toolTip;
}

/**
 * @return A tooltip for all the enums in HTML format
 */
AString
FileIdentificationDisplayModeEnum::toToolTipForAllEnums()
{
    AString txt("<html>");
    
    txt.appendWithNewLine("<ul>");
    
    std::vector<Enum> allEnums;
    getAllEnums(allEnums);
    for (auto& e : allEnums) {
        txt.appendWithNewLine("<li> "
                              + toGuiName(e)
                              + " - "
                              + toToolTip(e));
    }
    
    txt.appendWithNewLine("</ul>");
    txt.appendWithNewLine("</html>");
    
    return txt;
}

