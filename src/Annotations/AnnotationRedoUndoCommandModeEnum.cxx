
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
#define __ANNOTATION_UNDO_COMMAND_MODE_ENUM_DECLARE__
#include "AnnotationRedoUndoCommandModeEnum.h"
#undef __ANNOTATION_UNDO_COMMAND_MODE_ENUM_DECLARE__

#include "CaretAssert.h"

using namespace caret;


/**
 * \class caret::AnnotationRedoUndoCommandModeEnum
 * \brief Mode (type of modification) for the annotation undo command
 *
 * Using this enumerated type in the GUI with an EnumComboBoxTemplate
 *
 * Header File (.h)
 *     Forward declare the data type:
 *         class EnumComboBoxTemplate;
 *
 *     Declare the member:
 *         EnumComboBoxTemplate* m_AnnotationRedoUndoCommandModeEnumComboBox;
 *
 *     Declare a slot that is called when user changes selection
 *         private slots:
 *             void AnnotationRedoUndoCommandModeEnumComboBoxItemActivated();
 *
 * Implementation File (.cxx)
 *     Include the header files
 *         #include "EnumComboBoxTemplate.h"
 *         #include "AnnotationRedoUndoCommandModeEnum.h"
 *
 *     Instatiate:
 *         m_AnnotationRedoUndoCommandModeEnumComboBox = new EnumComboBoxTemplate(this);
 *         m_AnnotationRedoUndoCommandModeEnumComboBox->setup<AnnotationRedoUndoCommandModeEnum,AnnotationRedoUndoCommandModeEnum::Enum>();
 *
 *     Get notified when the user changes the selection:
 *         QObject::connect(m_AnnotationRedoUndoCommandModeEnumComboBox, SIGNAL(itemActivated()),
 *                          this, SLOT(AnnotationRedoUndoCommandModeEnumComboBoxItemActivated()));
 *
 *     Update the selection:
 *         m_AnnotationRedoUndoCommandModeEnumComboBox->setSelectedItem<AnnotationRedoUndoCommandModeEnum,AnnotationRedoUndoCommandModeEnum::Enum>(NEW_VALUE);
 *
 *     Read the selection:
 *         const AnnotationRedoUndoCommandModeEnum::Enum VARIABLE = m_AnnotationRedoUndoCommandModeEnumComboBox->getSelectedItem<AnnotationRedoUndoCommandModeEnum,AnnotationRedoUndoCommandModeEnum::Enum>();
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
AnnotationRedoUndoCommandModeEnum::AnnotationRedoUndoCommandModeEnum(const Enum enumValue,
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
AnnotationRedoUndoCommandModeEnum::~AnnotationRedoUndoCommandModeEnum()
{
}

/**
 * Initialize the enumerated metadata.
 */
void
AnnotationRedoUndoCommandModeEnum::initialize()
{
    if (initializedFlag) {
        return;
    }
    initializedFlag = true;
    
    enumData.push_back(AnnotationRedoUndoCommandModeEnum(INVALID,
                                                     "INVALID",
                                                     "Invalid"));
    
    enumData.push_back(AnnotationRedoUndoCommandModeEnum(BROWSER_TAB_BACKGROUND,
                                                         "BROWSER_TAB_BACKGROUND",
                                                         "Browser Tab Background"));
    
    enumData.push_back(AnnotationRedoUndoCommandModeEnum(BOUNDS_2D_ALL,
                                                         "BOUNDS_2D_ALL",
                                                         "Bounds 2D All"));
    
    enumData.push_back(AnnotationRedoUndoCommandModeEnum(BOUNDS_2D_SINGLE,
                                                         "BOUNDS_2D_SINGLE",
                                                         "Bounds 2D Single"));
    
    enumData.push_back(AnnotationRedoUndoCommandModeEnum(COLOR_BACKGROUND,
                                                     "COLOR_BACKGROUND",
                                                     "Color - Background"));
    
    enumData.push_back(AnnotationRedoUndoCommandModeEnum(COLOR_FOREGROUND,
                                                     "COLOR_FOREGROUND",
                                                     "Color - Foreground"));
    
    enumData.push_back(AnnotationRedoUndoCommandModeEnum(COORDINATE_ONE,
                                                     "COORDINATE_ONE",
                                                     "Coordinate One"));
    
    enumData.push_back(AnnotationRedoUndoCommandModeEnum(COORDINATE_TWO,
                                                     "COORDINATE_TWO",
                                                     "Coordinate Two"));
    
    enumData.push_back(AnnotationRedoUndoCommandModeEnum(CREATE_ANNOTATION,
                                                         "CREATE_ANNOTATION",
                                                         "Create Annotation"));
    
    enumData.push_back(AnnotationRedoUndoCommandModeEnum(CUT_ANNOTATION,
                                                         "CUT_ANNOTATION",
                                                         "Cut Annotations"));
    
    enumData.push_back(AnnotationRedoUndoCommandModeEnum(DELETE_ANNOTATIONS,
                                                         "DELETE_ANNOTATIONS",
                                                         "Delete Annotations"));
    
    enumData.push_back(AnnotationRedoUndoCommandModeEnum(DUPLICATE_ANNOTATION,
                                                         "DUPLICATE_ANNOTATION",
                                                         "Duplicate Annotation"));
    
    enumData.push_back(AnnotationRedoUndoCommandModeEnum(GROUPING_GROUP,
                                                         "GROUPING_GROUP",
                                                         "Group Annotations"));
    
    enumData.push_back(AnnotationRedoUndoCommandModeEnum(GROUPING_REGROUP,
                                                         "GROUPING_REGROUP",
                                                         "Regroup Annotations"));
    
    enumData.push_back(AnnotationRedoUndoCommandModeEnum(GROUPING_UNGROUP,
                                                         "GROUPING_UNGROUP",
                                                         "Ungroup Annotations"));
    
    enumData.push_back(AnnotationRedoUndoCommandModeEnum(LINE_ARROW_START,
                                                     "LINE_ARROW_START",
                                                     "Line Arrow Start"));
    
    enumData.push_back(AnnotationRedoUndoCommandModeEnum(LINE_ARROW_END,
                                                     "LINE_ARROW_END",
                                                     "Line Arrow End"));
    
    enumData.push_back(AnnotationRedoUndoCommandModeEnum(LINE_WIDTH_FOREGROUND,
                                                     "LINE_WIDTH_FOREGROUND",
                                                     "Line Width - Foreground"));

    enumData.push_back(AnnotationRedoUndoCommandModeEnum(LOCATION_AND_SIZE,
                                                         "LOCATION_AND_SIZE",
                                                         "location and size of annotations (coords, size, space, window, tab)"));
    
    enumData.push_back(AnnotationRedoUndoCommandModeEnum(PASTE_ANNOTATION,
                                                         "PASTE_ANNOTATION",
                                                         "Paste Annotation"));
    
    enumData.push_back(AnnotationRedoUndoCommandModeEnum(ROTATION_ANGLE,
                                                         "ROTATION_ANGLE",
                                                         "Rotation Angle"));
    
    enumData.push_back(AnnotationRedoUndoCommandModeEnum(TEXT_ALIGNMENT_HORIZONTAL,
                                                     "TEXT_ALIGNMENT_HORIZONTAL",
                                                     "Text Alignment Horizontal"));
    
    enumData.push_back(AnnotationRedoUndoCommandModeEnum(TEXT_ALIGNMENT_VERTICAL,
                                                     "TEXT_ALIGNMENT_VERTICAL",
                                                     "Text Alignment Vertical"));
    
    enumData.push_back(AnnotationRedoUndoCommandModeEnum(TEXT_CHARACTERS,
                                                     "TEXT_CHARACTERS",
                                                     "Text Characters"));
    
    enumData.push_back(AnnotationRedoUndoCommandModeEnum(TEXT_COLOR,
                                                         "TEXT_COLOR",
                                                         "Text Color"));

    enumData.push_back(AnnotationRedoUndoCommandModeEnum(TEXT_CONNECT_TO_BRAINORDINATE,
                                                     "TEXT_CONNECT_TO_BRAINORDINATE",
                                                     "Text Connect to Brainordinate"));
    
    enumData.push_back(AnnotationRedoUndoCommandModeEnum(TEXT_FONT_BOLD,
                                                     "TEXT_FONT_BOLD",
                                                     "Text Font Bold"));
    
    enumData.push_back(AnnotationRedoUndoCommandModeEnum(TEXT_FONT_ITALIC,
                                                     "TEXT_FONT_ITALIC",
                                                     "Text Font Italic"));
    
    enumData.push_back(AnnotationRedoUndoCommandModeEnum(TEXT_FONT_NAME,
                                                     "TEXT_FONT_NAME",
                                                     "Text Font Name"));
    
    enumData.push_back(AnnotationRedoUndoCommandModeEnum(TEXT_FONT_PERCENT_SIZE,
                                                     "TEXT_FONT_PERCENT_SIZE",
                                                     "Text Font Percent Size"));
    
    enumData.push_back(AnnotationRedoUndoCommandModeEnum(TEXT_FONT_POINT_SIZE,
                                                         "TEXT_FONT_POINT_SIZE",
                                                         "Text Font Point Size"));
    
    enumData.push_back(AnnotationRedoUndoCommandModeEnum(TEXT_FONT_UNDERLINE,
                                                     "TEXT_FONT_UNDERLINE",
                                                     "Text Font Undeline"));
    
    enumData.push_back(AnnotationRedoUndoCommandModeEnum(TEXT_ORIENTATION,
                                                     "TEXT_ORIENTATION",
                                                     "Text Orientation"));
    
}

/**
 * Find the data for and enumerated value.
 * @param enumValue
 *     The enumerated value.
 * @return Pointer to data for this enumerated type
 * or NULL if no data for type or if type is invalid.
 */
const AnnotationRedoUndoCommandModeEnum*
AnnotationRedoUndoCommandModeEnum::findData(const Enum enumValue)
{
    if (initializedFlag == false) initialize();
    
    size_t num = enumData.size();
    for (size_t i = 0; i < num; i++) {
        const AnnotationRedoUndoCommandModeEnum* d = &enumData[i];
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
AnnotationRedoUndoCommandModeEnum::toName(Enum enumValue) {
    if (initializedFlag == false) initialize();
    
    const AnnotationRedoUndoCommandModeEnum* enumInstance = findData(enumValue);
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
AnnotationRedoUndoCommandModeEnum::Enum
AnnotationRedoUndoCommandModeEnum::fromName(const AString& name, bool* isValidOut)
{
    if (initializedFlag == false) initialize();
    
    bool validFlag = false;
    Enum enumValue = AnnotationRedoUndoCommandModeEnum::enumData[0].enumValue;
    
    for (std::vector<AnnotationRedoUndoCommandModeEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const AnnotationRedoUndoCommandModeEnum& d = *iter;
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
        CaretAssertMessage(0, AString("Name " + name + "failed to match enumerated value for type AnnotationRedoUndoCommandModeEnum"));
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
AnnotationRedoUndoCommandModeEnum::toGuiName(Enum enumValue) {
    if (initializedFlag == false) initialize();
    
    const AnnotationRedoUndoCommandModeEnum* enumInstance = findData(enumValue);
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
AnnotationRedoUndoCommandModeEnum::Enum
AnnotationRedoUndoCommandModeEnum::fromGuiName(const AString& guiName, bool* isValidOut)
{
    if (initializedFlag == false) initialize();
    
    bool validFlag = false;
    Enum enumValue = AnnotationRedoUndoCommandModeEnum::enumData[0].enumValue;
    
    for (std::vector<AnnotationRedoUndoCommandModeEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const AnnotationRedoUndoCommandModeEnum& d = *iter;
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
        CaretAssertMessage(0, AString("guiName " + guiName + "failed to match enumerated value for type AnnotationRedoUndoCommandModeEnum"));
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
AnnotationRedoUndoCommandModeEnum::toIntegerCode(Enum enumValue)
{
    if (initializedFlag == false) initialize();
    const AnnotationRedoUndoCommandModeEnum* enumInstance = findData(enumValue);
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
AnnotationRedoUndoCommandModeEnum::Enum
AnnotationRedoUndoCommandModeEnum::fromIntegerCode(const int32_t integerCode, bool* isValidOut)
{
    if (initializedFlag == false) initialize();
    
    bool validFlag = false;
    Enum enumValue = AnnotationRedoUndoCommandModeEnum::enumData[0].enumValue;
    
    for (std::vector<AnnotationRedoUndoCommandModeEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const AnnotationRedoUndoCommandModeEnum& enumInstance = *iter;
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
        CaretAssertMessage(0, AString("Integer code " + AString::number(integerCode) + "failed to match enumerated value for type AnnotationRedoUndoCommandModeEnum"));
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
AnnotationRedoUndoCommandModeEnum::getAllEnums(std::vector<AnnotationRedoUndoCommandModeEnum::Enum>& allEnums)
{
    if (initializedFlag == false) initialize();
    
    allEnums.clear();
    
    for (std::vector<AnnotationRedoUndoCommandModeEnum>::iterator iter = enumData.begin();
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
AnnotationRedoUndoCommandModeEnum::getAllNames(std::vector<AString>& allNames, const bool isSorted)
{
    if (initializedFlag == false) initialize();
    
    allNames.clear();
    
    for (std::vector<AnnotationRedoUndoCommandModeEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        allNames.push_back(AnnotationRedoUndoCommandModeEnum::toName(iter->enumValue));
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
AnnotationRedoUndoCommandModeEnum::getAllGuiNames(std::vector<AString>& allGuiNames, const bool isSorted)
{
    if (initializedFlag == false) initialize();
    
    allGuiNames.clear();
    
    for (std::vector<AnnotationRedoUndoCommandModeEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        allGuiNames.push_back(AnnotationRedoUndoCommandModeEnum::toGuiName(iter->enumValue));
    }
    
    if (isSorted) {
        std::sort(allGuiNames.begin(), allGuiNames.end());
    }
}

