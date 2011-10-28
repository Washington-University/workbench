
/*LICENSE_START*/
/* 
 *  Copyright 1995-2011 Washington University School of Medicine 
 * 
 *  http://brainmap.wustl.edu 
 * 
 *  This file is part of CARET. 
 * 
 *  CARET is free software; you can redistribute it and/or modify 
 *  it under the terms of the GNU General Public License as published by 
 *  the Free Software Foundation; either version 2 of the License, or 
 *  (at your option) any later version. 
 * 
 *  CARET is distributed in the hope that it will be useful, 
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of 
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
 *  GNU General Public License for more details. 
 * 
 *  You should have received a copy of the GNU General Public License 
 *  along with CARET; if not, write to the Free Software 
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA 
 * 
 */ 

#define __DATA_FILE_TYPE_ENUM_DECLARE__
#include "DataFileTypeEnum.h"
#undef __DATA_FILE_TYPE_ENUM_DECLARE__

#include "CaretAssert.h"
#include "CaretLogger.h"

using namespace caret;

/**
 * Constructor.
 *
 * @param enumValue
 *    An enumerated value.
 * @param name
 *    Name of enumberated value.
 * @param guiName
 *    Name displayed in the user-interface
 * @param fileExtensionOne
 *    File extension
 * @param fileExtensionTwo
 *    Additional File extension
 * @param fileExtensionThree
 *    Additional File extension
 */
DataFileTypeEnum::DataFileTypeEnum(const Enum enumValue,
                                   const AString& name,
                                   const AString& guiName,
                                   const AString& fileExtensionOne,
                                   const AString& fileExtensionTwo,
                                   const AString& fileExtensionThree)
{
    this->enumValue = enumValue;
    this->integerCode = DataFileTypeEnum::integerCodeGenerator++;
    this->name = name;
    this->guiName = guiName;
    
    if (fileExtensionOne.isEmpty() == false) {
        this->fileExtensions.push_back(fileExtensionOne);
    }
    if (fileExtensionTwo.isEmpty() == false) {
        this->fileExtensions.push_back(fileExtensionTwo);
    }
    if (fileExtensionThree.isEmpty() == false) {
        this->fileExtensions.push_back(fileExtensionThree);
    }

    AString filterText = this->guiName + " Files (";
    
    
    for (std::vector<AString>::const_iterator iter = this->fileExtensions.begin();
         iter != this->fileExtensions.end();
         iter++) {
        if (iter != fileExtensions.begin()) {
            filterText += " ";
        }
        filterText += ("*." + *iter);
    }
    filterText += ")";
    
    this->qFileDialogNameFilter = filterText;
}

/**
 * Destructor.
 */
DataFileTypeEnum::~DataFileTypeEnum()
{
}

/**
 * Initialize the enumerated metadata.
 */
void
DataFileTypeEnum::initialize()
{
    if (initializedFlag) {
        return;
    }
    initializedFlag = true;

    enumData.push_back(DataFileTypeEnum(BORDER_PROJECTION, 
                                        "BORDER_PROJECTION", 
                                        "Border Projection",
                                        "borderproj"));
    
    enumData.push_back(DataFileTypeEnum(CIFTI, 
                                        "CIFTI", 
                                        "Connectivity",
                                        "cii"));
    
    enumData.push_back(DataFileTypeEnum(FOCI_PROJECTION, 
                                        "FOCI_PROJECTION", 
                                        "Foci Projection",
                                        "fociproj"));
    
    enumData.push_back(DataFileTypeEnum(LABEL, 
                                        "LABEL", 
                                        "Label",
                                        "label.gii"));
    
    enumData.push_back(DataFileTypeEnum(METRIC, 
                                        "METRIC", 
                                        "Metric",
                                        "func.gii"));
    
    enumData.push_back(DataFileTypeEnum(PALETTE, 
                                        "PALETTE", 
                                        "Palette",
                                        "palette"));
    
    enumData.push_back(DataFileTypeEnum(RGBA, 
                                        "RGBA", 
                                        "RGBA",
                                        "rgba.gii"));
    
    enumData.push_back(DataFileTypeEnum(SCENE, 
                                        "SCENE", 
                                        "Scene",
                                        "scene"));
    
    enumData.push_back(DataFileTypeEnum(SPECIFICATION, 
                                        "SPECIFICATION", 
                                        "Specification",
                                        "spec"));
    
    enumData.push_back(DataFileTypeEnum(SURFACE, 
                                        "SURFACE", 
                                        "Surface",
                                        "surf.gii"));    
    
    enumData.push_back(DataFileTypeEnum(UNKNOWN, 
                                        "UNKNOWN", 
                                        "Unknown",
                                        "unknown"));
    
    enumData.push_back(DataFileTypeEnum(VOLUME, 
                                        "VOLUME", 
                                        "Volume",
                                        "nii",
                                        "nii.gz"));
    
}

/**
 * Find the data for and enumerated value.
 * @param enumValue
 *     The enumerated value.
 * @return Pointer to data for this enumerated type
 * or NULL if no data for type or if type is invalid.
 */
const DataFileTypeEnum*
DataFileTypeEnum::findData(const Enum enumValue)
{
    if (initializedFlag == false) initialize();

    size_t num = enumData.size();
    for (size_t i = 0; i < num; i++) {
        const DataFileTypeEnum* d = &enumData[i];
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
DataFileTypeEnum::toName(Enum enumValue) {
    if (initializedFlag == false) initialize();
    
    const DataFileTypeEnum* enumInstance = findData(enumValue);
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
DataFileTypeEnum::Enum 
DataFileTypeEnum::fromName(const AString& nameIn, bool* isValidOut)
{
    AString name = nameIn;
    if (initializedFlag == false) initialize();
    
    bool validFlag = false;
    Enum enumValue = UNKNOWN;
    
    /*
     * Maintain compatibility with early spec files
     */
    if (name.startsWith("SURFACE_")) {
        CaretLogWarning("Obsolete spec file tag \"" + name + "\", replace with SURFACE");
        name = "SURFACE";
    }
    else if (name.startsWith("VOLUME_")) {
        CaretLogWarning("Obsolete spec file tag \"" + name + "\", replace with VOLUME");
        name = "VOLUME";
    }
    
    for (std::vector<DataFileTypeEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const DataFileTypeEnum& d = *iter;
        if (d.name == name) {
            enumValue = d.enumValue;
            validFlag = true;
            break;
        }
    }
    
    if (isValidOut != 0) {
        *isValidOut = validFlag;
    }
    else {
        CaretAssertMessage(0, AString("Name \"" + name + "\" failed to match enumerated value for type DataFileTypeEnum"));
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
DataFileTypeEnum::toGuiName(Enum enumValue) {
    if (initializedFlag == false) initialize();
    
    const DataFileTypeEnum* enumInstance = findData(enumValue);
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
DataFileTypeEnum::Enum 
DataFileTypeEnum::fromGuiName(const AString& guiName, bool* isValidOut)
{
    if (initializedFlag == false) initialize();
    
    bool validFlag = false;
    Enum enumValue = UNKNOWN;
    
    for (std::vector<DataFileTypeEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const DataFileTypeEnum& d = *iter;
        if (d.guiName == guiName) {
            enumValue = d.enumValue;
            validFlag = true;
            break;
        }
    }
    
    if (isValidOut != 0) {
        *isValidOut = validFlag;
    }
    else {
        CaretAssertMessage(0, AString("guiName \"" + guiName + "\" failed to match enumerated value for type DataFileTypeEnum"));
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
DataFileTypeEnum::toIntegerCode(Enum enumValue)
{
    if (initializedFlag == false) initialize();
    const DataFileTypeEnum* enumInstance = findData(enumValue);
    return enumInstance->integerCode;
}

/**
 * Get an enumerated value corresponding to its QFileDialog filter name.
 * @param qFileDialogNameFilter 
 *     Name of enumerated value.
 * @param isValidOut 
 *     If not NULL, it is set indicating that a
 *     enum value exists for the input name.
 * @return 
 *     Enumerated value.
 */
DataFileTypeEnum::Enum 
DataFileTypeEnum::fromQFileDialogFilter(const AString& qFileDialogNameFilter, 
                                        bool* isValidOut)
{
    if (initializedFlag == false) initialize();
    
    bool validFlag = false;
    Enum enumValue = UNKNOWN;
    
    for (std::vector<DataFileTypeEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const DataFileTypeEnum& d = *iter;
        if (d.qFileDialogNameFilter == qFileDialogNameFilter) {
            enumValue = d.enumValue;
            validFlag = true;
            break;
        }
    }
    
    if (isValidOut != 0) {
        *isValidOut = validFlag;
    }
    else {
        CaretAssertMessage(0, AString("qFileDialogNameFilter \"" + qFileDialogNameFilter + " \"failed to match enumerated value for type DataFileTypeEnum"));
    }
    return enumValue;
}

/**
 * Get the file filter text for use in a QFileDialog.
 *
 * @param enumValue
 *     Enumerated type for file filter.
 * @return
 *     Text containing file filter.
 */
AString 
DataFileTypeEnum::toQFileDialogFilter(const Enum enumValue)
{
    if (initializedFlag == false) initialize();
    const DataFileTypeEnum* enumInstance = findData(enumValue);
    return enumInstance->qFileDialogNameFilter;
}

/**
 * For the filename, match its extension to a DataFileType enumerated type.
 * @param filename 
 *     Name of file.
 * @param isValidOut 
 *     If not NULL, it is set indicating that a
 *     enum value exists for the input name.
 * @return 
 *     Enumerated value.
 */
DataFileTypeEnum::Enum 
DataFileTypeEnum::fromFileExtension(const AString& filename, bool* isValidOut)
{
    if (initializedFlag == false) initialize();
    
    bool validFlag = false;
    Enum enumValue = UNKNOWN;
    
    for (std::vector<DataFileTypeEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const DataFileTypeEnum& d = *iter;
        const std::vector<AString> extensions = iter->fileExtensions;
        for (std::vector<AString>::const_iterator extIter = extensions.begin();
             extIter != extensions.end();
             extIter++) {
            if (filename.endsWith(*extIter)) {
                enumValue = d.enumValue;
                validFlag = true;
                break;
            }
        }
    }
    
    if (isValidOut != 0) {
        *isValidOut = validFlag;
    }
    else {
        CaretAssertMessage(0, AString("filename \"" + filename + " \"has no matching extensions in DataFileTypeEnum"));
    }
    return enumValue;
    
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
DataFileTypeEnum::Enum
DataFileTypeEnum::fromIntegerCode(const int32_t integerCode, bool* isValidOut)
{
    if (initializedFlag == false) initialize();
    
    bool validFlag = false;
    Enum enumValue = UNKNOWN;
    
    for (std::vector<DataFileTypeEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const DataFileTypeEnum& enumInstance = *iter;
        if (enumInstance.integerCode == integerCode) {
            enumValue = enumInstance.enumValue;
            validFlag = true;
            break;
        }
    }
    
    if (isValidOut != 0) {
        *isValidOut = validFlag;
    }
    else {
        CaretAssertMessage(0, AString("Integer code \"" + AString::number(integerCode) + " \"failed to match enumerated value for type DataFileTypeEnum"));
    }
    return enumValue;
}

/**
 * Get all of the enumerated type values.  The values can be used
 * as parameters to toXXX() methods to get associated metadata.

 * @param allEnums
 *     A vector that is OUTPUT containing all of the enumerated values.
 * @param includeUNKNOWN
 *     If true, the UNKNOWN enum is included.
 */
void
DataFileTypeEnum::getAllEnums(std::vector<DataFileTypeEnum::Enum>& allEnums,
                              const bool includeUnknown)
{
    if (initializedFlag == false) initialize();
    
    allEnums.clear();
    
    for (std::vector<DataFileTypeEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        if (iter->enumValue == UNKNOWN) {
            if (includeUnknown) {
                allEnums.push_back(iter->enumValue);
            }
        }
        else {
            allEnums.push_back(iter->enumValue);
        }
    }
}

