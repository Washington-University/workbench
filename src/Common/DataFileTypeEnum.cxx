
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

#define __DATA_FILE_TYPE_ENUM_DECLARE__
#include "DataFileTypeEnum.h"
#undef __DATA_FILE_TYPE_ENUM_DECLARE__

#include "CaretAssert.h"
#include "CaretLogger.h"

using namespace caret;

/**
 * \class caret::DataFileTypeEnum 
 * \brief An enumerated type for data files.
 */


/**
 * Constructor.
 *
 * @param enumValue
 *    An enumerated value.
 * @param name
 *    Name of enumberated value.
 * @param guiName
 *    Name displayed in the user-interface
 * @param overlayTypeName
 *    Name displayed in overlay type combo box
 * @param fileIsUsedWithOneStructure
 *    True if file is used with ONE structure (eg node file (surface, metric, etc).
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
                                   const AString& overlayTypeName,
                                   const bool fileIsUsedWithOneStructure,
                                   const AString& fileExtensionOne,
                                   const AString& fileExtensionTwo,
                                   const AString& fileExtensionThree)
{
    this->enumValue = enumValue;
    this->integerCode = DataFileTypeEnum::integerCodeGenerator++;
    this->name = name;
    this->guiName = guiName;
    this->overlayTypeName = overlayTypeName;
    this->oneStructureFlag = fileIsUsedWithOneStructure;
    
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

    enumData.push_back(DataFileTypeEnum(ANNOTATION,
                                        "ANNOTATION",
                                        "Annotation",
                                        "ANNOTATION",
                                        false,
                                        "annot",
                                        "wb_annot"));
    
    enumData.push_back(DataFileTypeEnum(BORDER,
                                        "BORDER", 
                                        "Border",
                                        "BORDER",
                                        true,
                                        "border",
                                        "wb_border"));
    
    enumData.push_back(DataFileTypeEnum(CONNECTIVITY_DENSE, 
                                        "CONNECTIVITY_DENSE", 
                                        "Connectivity - Dense",
                                        "CONNECTIVITY",
                                        false,
                                        "dconn.nii"));
    
    enumData.push_back(DataFileTypeEnum(CONNECTIVITY_DENSE_LABEL,
                                        "CONNECTIVITY_DENSE_LABEL",
                                        "Connectivity - Dense Label",
                                        "CIFTI LABELS",
                                        false,
                                        "dlabel.nii"));
    
    enumData.push_back(DataFileTypeEnum(CONNECTIVITY_DENSE_PARCEL,
                                        "CONNECTIVITY_DENSE_PARCEL",
                                        "Connectivity - Dense Parcel",
                                        "CIFTI DENSE PARCEL",
                                        false,
                                        "dpconn.nii"));
    
    enumData.push_back(DataFileTypeEnum(CONNECTIVITY_DENSE_SCALAR,
                                        "CONNECTIVITY_DENSE_SCALAR",
                                        "Connectivity - Dense Scalar",
                                        "CIFTI SCALARS",
                                        false,
                                        "dscalar.nii"));
    
    enumData.push_back(DataFileTypeEnum(CONNECTIVITY_DENSE_TIME_SERIES,
                                        "CONNECTIVITY_DENSE_TIME_SERIES", 
                                        "Connectivity - Dense Data Series",
                                        "DATA SERIES",
                                        false,
                                        "dtseries.nii"));
    
    enumData.push_back(DataFileTypeEnum(CONNECTIVITY_FIBER_ORIENTATIONS_TEMPORARY,
                                        "CONNECTIVITY_FIBER_ORIENTATIONS_TEMPORARY",
                                        "Connectivity - Fiber Orientations TEMPORARY",
                                        "FIBER ORIENTATION TEMPORARY",
                                        false,
                                        "fiberTEMP.nii"));
    
    enumData.push_back(DataFileTypeEnum(CONNECTIVITY_FIBER_TRAJECTORY_TEMPORARY,
                                        "CONNECTIVITY_FIBER_TRAJECTORY_TEMPORARY",
                                        "Connectivity - Fiber Trajectory TEMPORARY",
                                        "FIBER TRAJECTORY TEMPORARY",
                                        false,
                                        "trajTEMP.wbsparse"));
    
    enumData.push_back(DataFileTypeEnum(CONNECTIVITY_PARCEL,
                                        "CONNECTIVITY_PARCEL",
                                        "Connectivity - Parcel",
                                        "CIFTI PARCEL",
                                        false,
                                        "pconn.nii"));
    
    enumData.push_back(DataFileTypeEnum(CONNECTIVITY_PARCEL_DENSE,
                                        "CONNECTIVITY_PARCEL_DENSE",
                                        "Connectivity - Parcel Dense",
                                        "CIFTI PARCEL DENSE",
                                        false,
                                        "pdconn.nii"));
    
    enumData.push_back(DataFileTypeEnum(CONNECTIVITY_PARCEL_LABEL,
                                        "CONNECTIVITY_PARCEL_LABEL",
                                        "Connectivity - Parcel Label",
                                        "CIFTI PARCEL LABEL",
                                        false,
                                        "plabel.nii"));
    
    enumData.push_back(DataFileTypeEnum(CONNECTIVITY_PARCEL_SCALAR,
                                        "CONNECTIVITY_PARCEL_SCALAR",
                                        "Connectivity - Parcel Scalar",
                                        "CIFTI PARCEL SCALAR",
                                        false,
                                        "pscalar.nii"));
    
    enumData.push_back(DataFileTypeEnum(CONNECTIVITY_PARCEL_SERIES,
                                        "CONNECTIVITY_PARCEL_SERIES",
                                        "Connectivity - Parcel Series",
                                        "CIFTI PARCEL SERIES",
                                        false,
                                        "ptseries.nii"));
    
    enumData.push_back(DataFileTypeEnum(CONNECTIVITY_SCALAR_DATA_SERIES,
                                        "CONNECTIVITY_SCALAR_DATA_SERIES",
                                        "Connectivity - Scalar Data Series",
                                        "CIFTI SCALAR DATA SERIES",
                                        false,
                                        "sdseries.nii"));
    
    enumData.push_back(DataFileTypeEnum(FOCI,
                                        "FOCI", 
                                        "Foci",
                                        "FOCI",
                                        false,
                                        "foci",
                                        "wb_foci"));
    
    enumData.push_back(DataFileTypeEnum(IMAGE,
                                        "IMAGE",
                                        "Image",
                                        "IMAGE",
                                        false,
                                        "png",
                                        "ppm"));
    
    enumData.push_back(DataFileTypeEnum(LABEL,
                                        "LABEL", 
                                        "Label",
                                        "LABEL",
                                        true,
                                        "label.gii"));
    
    enumData.push_back(DataFileTypeEnum(METRIC, 
                                        "METRIC", 
                                        "Metric",
                                        "METRIC",
                                        true,
                                        "func.gii",
                                        "shape.gii"));
    
    enumData.push_back(DataFileTypeEnum(PALETTE, 
                                        "PALETTE", 
                                        "Palette",
                                        "PALETTE",
                                        false,
                                        "palette",
                                        "wb_palette"));
    
    enumData.push_back(DataFileTypeEnum(RGBA, 
                                        "RGBA", 
                                        "RGBA",
                                        "RGBA",
                                        true,
                                        "rgba.gii"));
    
    enumData.push_back(DataFileTypeEnum(SCENE, 
                                        "SCENE", 
                                        "Scene",
                                        "SCENE",
                                        false,
                                        "scene",
                                        "wb_scene"));
    
    enumData.push_back(DataFileTypeEnum(SPECIFICATION, 
                                        "SPECIFICATION", 
                                        "Specification",
                                        "SPECIFICATION",
                                        false,
                                        "spec",
                                        "wb_spec"));
    
    enumData.push_back(DataFileTypeEnum(SURFACE, 
                                        "SURFACE", 
                                        "Surface",
                                        "SURFACE",
                                        true,
                                        "surf.gii"));
    
    enumData.push_back(DataFileTypeEnum(UNKNOWN, 
                                        "UNKNOWN", 
                                        "Unknown",
                                        "UNKNOWN",
                                        false,
                                        "unknown"));
    
    enumData.push_back(DataFileTypeEnum(VOLUME, 
                                        "VOLUME", 
                                        "Volume",
                                        "VOLUME",
                                        false,
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
    else if (validFlag == false) {
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
    else if (validFlag == false) {
        CaretAssertMessage(0, AString("guiName \"" + guiName + "\" failed to match enumerated value for type DataFileTypeEnum"));
    }
    return enumValue;
}

/**
 * Get a Overlay Type Name representation of the enumerated type.
 * @param enumValue
 *     Enumerated value.
 * @return
 *     String representing enumerated value.
 */
AString
DataFileTypeEnum::toOverlayTypeName(Enum enumValue) {
    if (initializedFlag == false) initialize();
    
    const DataFileTypeEnum* enumInstance = findData(enumValue);
    return enumInstance->overlayTypeName;
}

/**
 * Get an enumerated value corresponding to its overlay type name.
 * @param s
 *     Overlay Name of enumerated value.
 * @param isValidOut
 *     If not NULL, it is set indicating that a
 *     enum value exists for the input name.
 * @return
 *     Enumerated value.
 */
DataFileTypeEnum::Enum
DataFileTypeEnum::fromOverlayTypeName(const AString& overlayTypeName, bool* isValidOut)
{
    if (initializedFlag == false) initialize();
    
    bool validFlag = false;
    Enum enumValue = UNKNOWN;
    
    for (std::vector<DataFileTypeEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const DataFileTypeEnum& d = *iter;
        if (d.overlayTypeName == overlayTypeName) {
            enumValue = d.enumValue;
            validFlag = true;
            break;
        }
    }
    
    if (isValidOut != 0) {
        *isValidOut = validFlag;
    }
    else if (validFlag == false) {
        CaretAssertMessage(0, AString("guiName \"" + overlayTypeName + "\" failed to match enumerated value for type DataFileTypeEnum"));
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
    else if (validFlag == false) {
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
 * Is the file type used with a one structure (node based file, surface, label, etc.)?
 * @param enumValue
 *     Enumerated type for file filter.
 * @return  true if used with one structure, else false.
 */
bool 
DataFileTypeEnum::isFileUsedWithOneStructure(const Enum enumValue)
{
    if (initializedFlag == false) initialize();
    const DataFileTypeEnum* enumInstance = findData(enumValue);
    return enumInstance->oneStructureFlag;
}

/**
 * @return All valid file extensions for the given enum value.
 * @param enumValue
 *     Enumerated type for file extensions.
 */
std::vector<AString> 
DataFileTypeEnum::getAllFileExtensions(const Enum enumValue)
{
    if (initializedFlag == false) initialize();
    const DataFileTypeEnum* enumInstance = findData(enumValue);
    return enumInstance->fileExtensions;
}

/**
 * @return All valid file extensions for all file types except UNKNOWN.
 */
std::vector<AString>
DataFileTypeEnum::getFilesExtensionsForEveryFile()
{
    std::vector<AString> allExtensions;
    
    for (std::vector<DataFileTypeEnum>::iterator enumIter = enumData.begin();
         enumIter != enumData.end();
         enumIter++) {
        if (enumIter->enumValue != DataFileTypeEnum::UNKNOWN) {
            allExtensions.insert(allExtensions.end(),
                                 enumIter->fileExtensions.begin(),
                                 enumIter->fileExtensions.end());
        }
    }
    
    return allExtensions;
}

/**
 * If the given filename does not contain a file extension that is valid
 * for the given data file type, add the first valid file extension from
 * the given data file type.
 *
 * @param filename
 *    Name of file that may not have the correct file extension.
 * @param enumValue
 *    The data file type.
 * @return
 *    Input file name to which a file extension may have been added.
 */
AString 
DataFileTypeEnum::addFileExtensionIfMissing(const AString& filenameIn,
                                            const Enum enumValue)
{   
    AString filename = filenameIn;

    if (initializedFlag == false) initialize();
    const DataFileTypeEnum* enumInstance = findData(enumValue);
    
    /*
     * See if filename ends with any of the available extensions for the
     * given data file type.
     */
    for (std::vector<AString>::const_iterator iter = enumInstance->fileExtensions.begin();
         iter != enumInstance->fileExtensions.end();
         iter++) {
        const AString ext = ("." + *iter);
        if (filename.endsWith(ext)) {
            return filename;
        }
    }
    
    /*
     * Add default extension.
     */
    const AString defaultExtension = DataFileTypeEnum::toFileExtension(enumValue);
    filename += ("." + defaultExtension);
    
    return filename;
}

/**
 * Get the primary file extension for the file type.
 * @param enumValue
 *    The data file type.
 * @return 
 *    Extension for file type.
 */
AString 
DataFileTypeEnum::toFileExtension(const Enum enumValue)
{
    if (initializedFlag == false) initialize();
    const DataFileTypeEnum* enumInstance = findData(enumValue);
    
    AString ext = "file";
    if (enumInstance->fileExtensions.empty() == false) {
        ext = enumInstance->fileExtensions[0];
    }
    return ext;
}

/***
 * Does the filename have a valid extension for the given file type?
 *
 * @param filename
 *    Name of file that may not have the correct file extension.
 * @param enumValue
 *    The data file type.
 * @return
 *    True if the filename has a valid extension, else false.
 */
bool
DataFileTypeEnum::isValidFileExtension(const AString& filename,
                                       const Enum enumValue)
{
    if (initializedFlag == false) initialize();
    const DataFileTypeEnum* enumInstance = findData(enumValue);
    
    /*
     * See if filename ends with any of the available extensions for the
     * given data file type.
     */
    for (std::vector<AString>::const_iterator iter = enumInstance->fileExtensions.begin();
         iter != enumInstance->fileExtensions.end();
         iter++) {
        const AString ext = ("." + *iter);
        if (filename.endsWith(ext)) {
            return true;
        }
    }
    
    return false;
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
            /*
             * Need to add "." to avoid ambiguous matching ("dconn.nii, pdconn.nii)
             */
            const AString extensionWithDot = ("." + *extIter);
            if (filename.endsWith(extensionWithDot)) {
                enumValue = d.enumValue;
                validFlag = true;
                break;
            }
        }
        if (validFlag) {
            break;
        }
    }
    
    if (isValidOut != 0) {
        *isValidOut = validFlag;
    }
    else if (validFlag == false) {
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
    else if (validFlag == false) {
        CaretAssertMessage(0, AString("Integer code \"" + AString::number(integerCode) + " \"failed to match enumerated value for type DataFileTypeEnum"));
    }
    return enumValue;
}

/**
 * Get all of the enumerated type values.  The values can be used
 * as parameters to toXXX() methods to get associated metadata.
 *
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
            if ( ! includeUnknown) {
                continue;
            }
        }
        
        allEnums.push_back(iter->enumValue);
    }
}

/**
 * Is the enumerated type a connectivity enumerated type?
 * @param enumValue
 *   The enumerated type value.
 * @return
 *   true if so, else false.
 */
bool 
DataFileTypeEnum::isConnectivityDataType(const Enum enumValue)
{
    const AString name = DataFileTypeEnum::toName(enumValue);
    if (name.startsWith("CONNECTIVITY")) {
        return true;
    }
    return false;
}

/**
 * Get all connectivity enumerated type values.
 * @param connectivityEnumsOut
 *    Will be loaded with all connectivity enumerated types.
 */
void 
DataFileTypeEnum::getAllConnectivityEnums(std::vector<Enum>& connectivityEnumsOut)
{
    if (initializedFlag == false) initialize();
    
    connectivityEnumsOut.clear();
    
    for (std::vector<DataFileTypeEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        if (DataFileTypeEnum::isConnectivityDataType(iter->enumValue)) {
            connectivityEnumsOut.push_back(iter->enumValue);
        }
    }
    
}


