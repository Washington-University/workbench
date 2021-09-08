
/*LICENSE_START*/
/*
 *  Copyright (C) 2014  Washington University School of Medicine
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
#define __SELECTION_ITEM_DATA_TYPE_ENUM_DECLARE__
#include "SelectionItemDataTypeEnum.h"
#undef __SELECTION_ITEM_DATA_TYPE_ENUM_DECLARE__

#include "CaretAssert.h"

using namespace caret;

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
SelectionItemDataTypeEnum::SelectionItemDataTypeEnum(const Enum enumValue,
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
SelectionItemDataTypeEnum::~SelectionItemDataTypeEnum()
{
}

/**
 * Initialize the enumerated metadata.
 */
void
SelectionItemDataTypeEnum::initialize()
{
    if (initializedFlag) {
        return;
    }
    initializedFlag = true;

    enumData.push_back(SelectionItemDataTypeEnum(INVALID, 
                                    "INVALID", 
                                    "Invalid"));
    
    enumData.push_back(SelectionItemDataTypeEnum(ANNOTATION, 
                                    "ANNOTATION", 
                                    "Annotation"));
    
    enumData.push_back(SelectionItemDataTypeEnum(BORDER_SURFACE, 
                                    "BORDER_SURFACE", 
                                    "Surface Border"));
    
    enumData.push_back(SelectionItemDataTypeEnum(BORDER_VOLUME, 
                                    "BORDER_VOLUME", 
                                    "Volume Border"));
    
    enumData.push_back(SelectionItemDataTypeEnum(CHART_DATA_SERIES,
                                                 "CHART_DATA_SERIES",
                                                 "Data-Series Chart"));
    
    enumData.push_back(SelectionItemDataTypeEnum(CHART_FREQUENCY_SERIES,
                                                 "CHART_FREQUENCY_SERIES",
                                                 "Frequency-Series Chart"));
    
    enumData.push_back(SelectionItemDataTypeEnum(CHART_MATRIX,
                                                 "CHART_MATRIX",
                                                 "Matrix Chart"));
    
    enumData.push_back(SelectionItemDataTypeEnum(CHART_TIME_SERIES,
                                                 "CHART_TIME_SERIES",
                                                 "Time-Series Chart"));
    
    enumData.push_back(SelectionItemDataTypeEnum(CHART_TWO_HISTOGRAM,
                                                 "CHART_TWO_HISTOGRAM",
                                                 "Histogram Chart Two"));
    
    enumData.push_back(SelectionItemDataTypeEnum(CHART_TWO_LINE_LAYER,
                                                 "CHART_TWO_LINE_LAYER",
                                                 "Line-Layer Chart Two"));
    
    enumData.push_back(SelectionItemDataTypeEnum(CHART_TWO_LINE_LAYER_VERTICAL_NEAREST,
                                                 "CHART_TWO_LINE_LAYER_VERTICAL_NEAREST",
                                                 "Line-Layer Vertical Nearest Chart Two"));
    
    enumData.push_back(SelectionItemDataTypeEnum(CHART_TWO_LINE_SERIES,
                                                 "CHART_TWO_LINE_SERIES",
                                                 "Line-Series Chart Two"));
    
    enumData.push_back(SelectionItemDataTypeEnum(CHART_TWO_LABEL,
                                                 "CHART_TWO_LABEL",
                                                 "Label Chart Two"));
    
    enumData.push_back(SelectionItemDataTypeEnum(CHART_TWO_MATRIX,
                                                 "CHART_TWO_MATRIX",
                                                 "Matrix Chart Two"));

    enumData.push_back(SelectionItemDataTypeEnum(CIFTI_CONNECTIVITY_MATRIX_ROW_COLUMN,
                                                 "CIFTI_CONNECTIVITY_MATRIX_ROW_COLUMN",
                                                 "CIFTI Connectivity Row or Column"));
    
    enumData.push_back(SelectionItemDataTypeEnum(CZI_IMAGE,
                                                 "CZI_IMAGE",
                                                 "CZI Image"));
    
    enumData.push_back(SelectionItemDataTypeEnum(FOCUS_SURFACE,
                                    "FOCUS_SURFACE", 
                                    "Surface Focus"));
    
    enumData.push_back(SelectionItemDataTypeEnum(FOCUS_VOLUME, 
                                    "FOCUS_VOLUME", 
                                    "Volume Focus"));
    
    enumData.push_back(SelectionItemDataTypeEnum(IMAGE,
                                                 "IMAGE",
                                                 "Image"));
    
    enumData.push_back(SelectionItemDataTypeEnum(IMAGE_CONTROL_POINT,
                                                 "IMAGE_CONTROL_POINT",
                                                 "Image Control Point"));
    
    enumData.push_back(SelectionItemDataTypeEnum(MEDIA,
                                                 "MEDIA",
                                                 "Media"));
    
    enumData.push_back(SelectionItemDataTypeEnum(SURFACE_NODE,
                                    "SURFACE_NODE", 
                                    "Surface Vertex"));
    
    enumData.push_back(SelectionItemDataTypeEnum(SURFACE_TRIANGLE,
                                    "SURFACE_TRIANGLE", 
                                    "Surface Triangle"));
    
    enumData.push_back(SelectionItemDataTypeEnum(UNIVERSAL_IDENTIFICATION_SYMBOL,
                                                 "UNIVERSAL_IDENTIFICATION_SYMBOL",
                                                 "Universal Identification Symbol"));
    
    enumData.push_back(SelectionItemDataTypeEnum(VOXEL,
                                    "VOXEL", 
                                    "Voxel"));
    
    enumData.push_back(SelectionItemDataTypeEnum(VOXEL_EDITING,
                                                 "VOXEL_EDITING",
                                                 "Voxel Editing"));    
}

/**
 * Find the data for and enumerated value.
 * @param enumValue
 *     The enumerated value.
 * @return Pointer to data for this enumerated type
 * or NULL if no data for type or if type is invalid.
 */
const SelectionItemDataTypeEnum*
SelectionItemDataTypeEnum::findData(const Enum enumValue)
{
    if (initializedFlag == false) initialize();

    size_t num = enumData.size();
    for (size_t i = 0; i < num; i++) {
        const SelectionItemDataTypeEnum* d = &enumData[i];
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
SelectionItemDataTypeEnum::toName(Enum enumValue) {
    if (initializedFlag == false) initialize();
    
    const SelectionItemDataTypeEnum* enumInstance = findData(enumValue);
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
SelectionItemDataTypeEnum::Enum 
SelectionItemDataTypeEnum::fromName(const AString& name, bool* isValidOut)
{
    if (initializedFlag == false) initialize();
    
    bool validFlag = false;
    Enum enumValue = INVALID;
    
    for (std::vector<SelectionItemDataTypeEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const SelectionItemDataTypeEnum& d = *iter;
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
        CaretAssertMessage(0, AString("Name " + name + "failed to match enumerated value for type SelectionItemDataTypeEnum"));
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
SelectionItemDataTypeEnum::toGuiName(Enum enumValue) {
    if (initializedFlag == false) initialize();
    
    const SelectionItemDataTypeEnum* enumInstance = findData(enumValue);
    return enumInstance->guiName;
}

/**
 * Get an enumerated value corresponding to its GUI name.
 * @param guiName
 *     Name of enumerated value.
 * @param isValidOut 
 *     If not NULL, it is set indicating that a
 *     enum value exists for the input name.
 * @return 
 *     Enumerated value.
 */
SelectionItemDataTypeEnum::Enum 
SelectionItemDataTypeEnum::fromGuiName(const AString& guiName, bool* isValidOut)
{
    if (initializedFlag == false) initialize();
    
    bool validFlag = false;
    Enum enumValue = INVALID;
    
    for (std::vector<SelectionItemDataTypeEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const SelectionItemDataTypeEnum& d = *iter;
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
        CaretAssertMessage(0, AString("guiName " + guiName + "failed to match enumerated value for type SelectionItemDataTypeEnum"));
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
SelectionItemDataTypeEnum::toIntegerCode(Enum enumValue)
{
    if (initializedFlag == false) initialize();
    const SelectionItemDataTypeEnum* enumInstance = findData(enumValue);
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
SelectionItemDataTypeEnum::Enum
SelectionItemDataTypeEnum::fromIntegerCode(const int32_t integerCode, bool* isValidOut)
{
    if (initializedFlag == false) initialize();
    
    bool validFlag = false;
    Enum enumValue = INVALID;
    
    for (std::vector<SelectionItemDataTypeEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const SelectionItemDataTypeEnum& enumInstance = *iter;
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
        CaretAssertMessage(0, AString("Integer code " + AString::number(integerCode) + "failed to match enumerated value for type SelectionItemDataTypeEnum"));
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
SelectionItemDataTypeEnum::getAllEnums(std::vector<SelectionItemDataTypeEnum::Enum>& allEnums)
{
    if (initializedFlag == false) initialize();
    
    allEnums.clear();
    
    for (std::vector<SelectionItemDataTypeEnum>::iterator iter = enumData.begin();
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
SelectionItemDataTypeEnum::getAllNames(std::vector<AString>& allNames, const bool isSorted)
{
    if (initializedFlag == false) initialize();
    
    allNames.clear();
    
    for (std::vector<SelectionItemDataTypeEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        allNames.push_back(SelectionItemDataTypeEnum::toName(iter->enumValue));
    }
    
    if (isSorted) {
        std::sort(allNames.begin(), allNames.end());
    }
}

/**
 * Get all of the GUI names of the enumerated type values.
 *
 * @param allGuiNames
 *     A vector that is OUTPUT containing all of the GUI names of the enumerated values.
 * @param isSorted
 *     If true, the names are sorted in alphabetical order.
 */
void
SelectionItemDataTypeEnum::getAllGuiNames(std::vector<AString>& allGuiNames, const bool isSorted)
{
    if (initializedFlag == false) initialize();
    
    allGuiNames.clear();
    
    for (std::vector<SelectionItemDataTypeEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        allGuiNames.push_back(SelectionItemDataTypeEnum::toGuiName(iter->enumValue));
    }
    
    if (isSorted) {
        std::sort(allGuiNames.begin(), allGuiNames.end());
    }
}

