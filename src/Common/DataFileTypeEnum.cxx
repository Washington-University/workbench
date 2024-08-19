
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

#include <set>

#include <QDir>
#include <QImageReader>
#include <QImageWriter>
#include <QMovie>

#include "CaretAssert.h"
#include "CaretLogger.h"
#include "FileInformation.h"

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
                                   const AString& fileExtensionThree,
                                   const AString& fileExtensionFour)
{
    this->enumValue = enumValue;
    this->integerCode = DataFileTypeEnum::integerCodeGenerator++;
    this->name = name;
    this->guiName = guiName;
    this->overlayTypeName = overlayTypeName;
    this->oneStructureFlag = fileIsUsedWithOneStructure;
    
    if (fileExtensionOne.isEmpty() == false) {
        this->readAndWriteFileExtensions.push_back(fileExtensionOne);
    }
    if (fileExtensionTwo.isEmpty() == false) {
        this->readAndWriteFileExtensions.push_back(fileExtensionTwo);
    }
    if (fileExtensionThree.isEmpty() == false) {
        this->readAndWriteFileExtensions.push_back(fileExtensionThree);
    }
    if (fileExtensionFour.isEmpty() == false) {
        this->readAndWriteFileExtensions.push_back(fileExtensionFour);
    }

    if (this->enumValue == DataFileTypeEnum::IMAGE) {
        /*
         * Images support different extensions for reading and writing
         */
        AString defaultWriteImageExtension;
        getWorkbenchSupportedImageFileExtensions(this->readFileExtensions,
                                                 this->writeFileExtensions,
                                                 defaultWriteImageExtension);
        
        std::set<AString> uniqueExtensions(this->readFileExtensions.begin(),
                                           this->readFileExtensions.end());
        uniqueExtensions.insert(this->writeFileExtensions.begin(),
                                this->writeFileExtensions.end());
        
        this->readAndWriteFileExtensions.clear();
        this->readAndWriteFileExtensions.insert(this->readAndWriteFileExtensions.end(),
                                                uniqueExtensions.begin(),
                                                uniqueExtensions.end());
    }
    else {
        /*
         * Most files use same extension for reading or writing
         */
        this->readFileExtensions  = this->readAndWriteFileExtensions;
        this->writeFileExtensions = this->readAndWriteFileExtensions;
    }
    
    this->qReadFileDialogNameFilter  = createQFileDialogNameFilter(this->guiName,
                                                                   this->readFileExtensions);
    this->qWriteFileDialogNameFilter = createQFileDialogNameFilter(this->guiName,
                                                                   this->writeFileExtensions);
}

/**
 * Destructor.
 */
DataFileTypeEnum::~DataFileTypeEnum()
{
}

/**
 * Reinitialize the data file types
 * These items are initialized when a palette file is creeated during session manager creation.
 * However, QCoreApplication has not been initialized so the plugin paths are not valid and this
 * prevents getting image file extensions for image files loaded using plugins.  Calling this method
 * after QCoreApplication results in the plugin image file extensions availability.
 */
void
DataFileTypeEnum::reinitializeDataFileTypeEnums()
{
    initializedFlag      = false;
    integerCodeGenerator = 0;
    enumData.clear();
    initialize();
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
                                        "wb_annot"));
    
    enumData.push_back(DataFileTypeEnum(ANNOTATION_TEXT_SUBSTITUTION,
                                        "ANNOTATION_TEXT_SUBSTITUTION",
                                        "Annotation Text Substitution",
                                        "ANNOTATION TEXT SUBSTITUTION",
                                        false,
                                        "wb_annsub.csv"));
    
    enumData.push_back(DataFileTypeEnum(BORDER,
                                        "BORDER", 
                                        "Border",
                                        "BORDER",
                                        true,
                                        "border",
                                        "wb_border"));
    
    enumData.push_back(DataFileTypeEnum(CONNECTIVITY_DENSE, 
                                        "CONNECTIVITY_DENSE", 
                                        "CIFTI - Dense",
                                        "CONNECTIVITY",
                                        false,
                                        "dconn.nii"));
    
    enumData.push_back(DataFileTypeEnum(CONNECTIVITY_DENSE_DYNAMIC,
                                        "CONNECTIVITY_DENSE_DYNAMIC",
                                        "CIFTI - Dense Dynamic",
                                        "CONNECTIVITY DYNAMIC",
                                        false,
                                        "dynconn.nii"));
    
    enumData.push_back(DataFileTypeEnum(CONNECTIVITY_DENSE_LABEL,
                                        "CONNECTIVITY_DENSE_LABEL",
                                        "CIFTI - Dense Label",
                                        "CIFTI LABELS",
                                        false,
                                        "dlabel.nii"));
    
    enumData.push_back(DataFileTypeEnum(CONNECTIVITY_DENSE_PARCEL,
                                        "CONNECTIVITY_DENSE_PARCEL",
                                        "CIFTI - Dense Parcel",
                                        "CIFTI DENSE PARCEL",
                                        false,
                                        "dpconn.nii"));
    
    enumData.push_back(DataFileTypeEnum(CONNECTIVITY_DENSE_SCALAR,
                                        "CONNECTIVITY_DENSE_SCALAR",
                                        "CIFTI - Dense Scalar",
                                        "CIFTI SCALARS",
                                        false,
                                        "dscalar.nii"));
    
    enumData.push_back(DataFileTypeEnum(CONNECTIVITY_DENSE_TIME_SERIES,
                                        "CONNECTIVITY_DENSE_TIME_SERIES", 
                                        "CIFTI - Dense Data Series",
                                        "DATA SERIES",
                                        false,
                                        "dtseries.nii"));
    
    enumData.push_back(DataFileTypeEnum(CONNECTIVITY_FIBER_ORIENTATIONS_TEMPORARY,
                                        "CONNECTIVITY_FIBER_ORIENTATIONS_TEMPORARY",
                                        "CIFTI - Fiber Orientations TEMPORARY",
                                        "FIBER ORIENTATION TEMPORARY",
                                        false,
                                        "fiberTEMP.nii"));
    
    enumData.push_back(DataFileTypeEnum(CONNECTIVITY_FIBER_TRAJECTORY_TEMPORARY,
                                        "CONNECTIVITY_FIBER_TRAJECTORY_TEMPORARY",
                                        "CIFTI - Fiber Trajectory TEMPORARY",
                                        "FIBER TRAJECTORY TEMPORARY",
                                        false,
                                        "trajTEMP.wbsparse"));
    
    enumData.push_back(DataFileTypeEnum(CONNECTIVITY_PARCEL,
                                        "CONNECTIVITY_PARCEL",
                                        "CIFTI - Parcel",
                                        "CIFTI PARCEL",
                                        false,
                                        "pconn.nii"));
    
    enumData.push_back(DataFileTypeEnum(CONNECTIVITY_PARCEL_DENSE,
                                        "CONNECTIVITY_PARCEL_DENSE",
                                        "CIFTI - Parcel Dense",
                                        "CIFTI PARCEL DENSE",
                                        false,
                                        "pdconn.nii"));
    
    enumData.push_back(DataFileTypeEnum(CONNECTIVITY_PARCEL_DYNAMIC,
                                        "CONNECTIVITY_PARCEL_DYNAMIC",
                                        "CIFTI - Parcel Dynamic",
                                        "CIFTI PARCEL DYNAMIC",
                                        false,
                                        "parcel_dynconn")); /* this file is never written */
    
    enumData.push_back(DataFileTypeEnum(CONNECTIVITY_PARCEL_LABEL,
                                        "CONNECTIVITY_PARCEL_LABEL",
                                        "CIFTI - Parcel Label",
                                        "CIFTI PARCEL LABEL",
                                        false,
                                        "plabel.nii"));
    
    enumData.push_back(DataFileTypeEnum(CONNECTIVITY_PARCEL_SCALAR,
                                        "CONNECTIVITY_PARCEL_SCALAR",
                                        "CIFTI - Parcel Scalar",
                                        "CIFTI PARCEL SCALAR",
                                        false,
                                        "pscalar.nii"));
    
    enumData.push_back(DataFileTypeEnum(CONNECTIVITY_PARCEL_SERIES,
                                        "CONNECTIVITY_PARCEL_SERIES",
                                        "CIFTI - Parcel Series",
                                        "CIFTI PARCEL SERIES",
                                        false,
                                        "ptseries.nii"));
    
    enumData.push_back(DataFileTypeEnum(CONNECTIVITY_SCALAR_DATA_SERIES,
                                        "CONNECTIVITY_SCALAR_DATA_SERIES",
                                        "CIFTI - Scalar Data Series",
                                        "CIFTI SCALAR DATA SERIES",
                                        false,
                                        "sdseries.nii"));
    
    enumData.push_back(DataFileTypeEnum(CZI_IMAGE_FILE,
                                        "CZI_IMAGE_FILE",
                                        "CZI Image",
                                        "CZI IMAGE",
                                        false,             /* ext below begins with dot */
                                        DataFileTypeEnum::toCziImageFileExtension().mid(1)));
    
    enumData.push_back(DataFileTypeEnum(FOCI,
                                        "FOCI",
                                        "Foci",
                                        "FOCI",
                                        false,
                                        "foci",
                                        "wb_foci"));
    
    enumData.push_back(DataFileTypeEnum(HISTOLOGY_SLICES,
                                        "HISTOLOGY_SLICES",
                                        "Histology Slices",
                                        "HISTOLOGY_SLICES",
                                        false,             /* ext below begins with dot */
                                        "metaczi",
                                        "meta-image"));
    
    enumData.push_back(DataFileTypeEnum(IMAGE,
                                        "IMAGE",
                                        "Image",
                                        "IMAGE",
                                        false,
                                        "XXX")); /* Extensions for IMAGE set in constructor */
    
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
    
    enumData.push_back(DataFileTypeEnum(METRIC_DYNAMIC,
                                        "METRIC_DYNAMIC",
                                        "Metric - Dynamic",
                                        "METRIC_DYNAMIC",
                                        true,
                                        "func_dynconn")); // this file is never written
    
    enumData.push_back(DataFileTypeEnum(OME_ZARR_IMAGE_FILE,
                                        "OME_ZARR_IMAGE_FILE",
                                        "OME-ZARR Image File",
                                        "OME-ZARR",
                                        false,
                                        "ome.zarr",
                                        "ome.zarr.zip"));
    
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
    
    enumData.push_back(DataFileTypeEnum(SAMPLES,
                                        "SAMPLES",
                                        "Samples",
                                        "SAMPLES",
                                        false,
                                        "wb_samples"));
    
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
    
    enumData.push_back(DataFileTypeEnum(VOLUME_DYNAMIC,
                                        "VOLUME_DYNAMIC",
                                        "Volume - Dynamic",
                                        "VOLUME DYNAMIC",
                                        false,
                                        "vol_dynconn")); // this file is never written
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
 * Get a short GUI string representation of the enumerated type.
 * Removes any "Connectivity - " or " TEMPORARY" from the toGuiName().
 * @param enumValue
 *     Enumerated value.
 * @return
 *     String representing enumerated value.
 */
AString
DataFileTypeEnum::toShortGuiName(Enum enumValue)
{
    const AString typeName = DataFileTypeEnum::toGuiName(enumValue);
    
    const AString connectivityPrefix("Connectivity - ");
    const int connectivityPrefixLength = connectivityPrefix.length();
    
    const AString temporarySuffix(" TEMPORARY");
    const int temporarySuffixLength = temporarySuffix.length();
    
    AString text = typeName;
    if (text.startsWith(connectivityPrefix)) {
        text = text.mid(connectivityPrefixLength);
    }
    
    if (text.endsWith(temporarySuffix)) {
        text = text.left(text.length() - temporarySuffixLength);
    }
    
    return text;
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
        if ((d.qReadFileDialogNameFilter == qFileDialogNameFilter)
            && (d.qWriteFileDialogNameFilter == qFileDialogNameFilter)) {
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
 * Get the file filter text for use in a QFileDialog for READING files
 *
 * @param enumValue
 *     Enumerated type for file filter.
 * @return
 *     Text containing file filter.
 */
AString 
DataFileTypeEnum::toQFileDialogFilterForReading(const Enum enumValue)
{
    if (initializedFlag == false) initialize();
    const DataFileTypeEnum* enumInstance = findData(enumValue);
    return enumInstance->qReadFileDialogNameFilter;
}

/**
 * Get the file filter text for use in a QFileDialog for WRITING files
 *
 * @param enumValue
 *     Enumerated type for file filter.
 * @return
 *     Text containing file filter.
 */
AString
DataFileTypeEnum::toQFileDialogFilterForWriting(const Enum enumValue)
{
    if (initializedFlag == false) initialize();
    const DataFileTypeEnum* enumInstance = findData(enumValue);
    return enumInstance->qWriteFileDialogNameFilter;
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
 * @return All wild card matching (*.ext) for the given enum value.
 * @param enumValue
 *     Enumerated type for file extensions.
 */
std::vector<AString>
DataFileTypeEnum::getWildCardMatching(const Enum enumValue)
{
    if (initializedFlag == false) initialize();
    
    const DataFileTypeEnum* enumInstance = findData(enumValue);
    std::vector<AString> wildcards;
    for (auto ext : enumInstance->readAndWriteFileExtensions) {
        wildcards.push_back("*." + ext);
    }
    
    return wildcards;
}

/**
 * Get files for the given enumerated type in the given directory
 * @param enumValue
 *    Enumerated type for file extensions.
 * @param directoryPath
 *    Name of directory
 * @return Files in the directory for the type
 */
std::vector<AString>
DataFileTypeEnum::getFilesInDirectory(const Enum enumValue,
                                      const AString& directoryPath)
{
    std::vector<AString> wildCards  = DataFileTypeEnum::getWildCardMatching(enumValue);
    
    QStringList fileNameFilters;
    for (auto w : wildCards) {
        fileNameFilters.append(w);
    }
    
    QDir::Filters typeFilter(QDir::Files);
    
    QDir dir(directoryPath);
    std::vector<AString> filenamesOut;

    QFileInfoList fileInfoList = dir.entryInfoList(fileNameFilters,
                                                   typeFilter);
    QListIterator<QFileInfo> iter(fileInfoList);
    while (iter.hasNext()) {
        filenamesOut.push_back(iter.next().absoluteFilePath());
    }

    return filenamesOut;
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
    return enumInstance->readAndWriteFileExtensions;
}

/**
 * @return All valid file extensions for reading the given enum value.
 * @param enumValue
 *     Enumerated type for file extensions.
 */
std::vector<AString>
DataFileTypeEnum::getAllFileExtensionsForReading(const Enum enumValue)
{
    if (initializedFlag == false) initialize();
    const DataFileTypeEnum* enumInstance = findData(enumValue);
    return enumInstance->readFileExtensions;
}

/**
 * @return All valid file extensions for writing the given enum value.
 * @param enumValue
 *     Enumerated type for file extensions.
 */
std::vector<AString>
DataFileTypeEnum::getAllFileExtensionsForWriting(const Enum enumValue)
{
    if (initializedFlag == false) initialize();
    const DataFileTypeEnum* enumInstance = findData(enumValue);
    return enumInstance->writeFileExtensions;
}

/**
 * @return All valid file extensions for all file types except UNKNOWN
 * and dynanmic connectivity files
 *
 * @param includeNonWritableFileTypesFlag
 *     If true, include non-writable files such as dynamic connectvity files
 */
std::vector<AString>
DataFileTypeEnum::getFilesExtensionsForEveryFile(const bool includeNonWritableFileTypesFlag)
{
    std::vector<AString> allExtensions;
    
    for (std::vector<DataFileTypeEnum>::iterator enumIter = enumData.begin();
         enumIter != enumData.end();
         enumIter++) {
        bool validFlag(true);
        
        switch (enumIter->enumValue) {
            case DataFileTypeEnum::ANNOTATION:
                break;
            case DataFileTypeEnum::ANNOTATION_TEXT_SUBSTITUTION:
                break;
            case DataFileTypeEnum::BORDER:
                break;
            case DataFileTypeEnum::CONNECTIVITY_DENSE:
                break;
            case DataFileTypeEnum::CONNECTIVITY_DENSE_DYNAMIC:
                validFlag = includeNonWritableFileTypesFlag;
                break;
            case DataFileTypeEnum::CONNECTIVITY_DENSE_LABEL:
                break;
            case DataFileTypeEnum::CONNECTIVITY_DENSE_PARCEL:
                break;
            case DataFileTypeEnum::CONNECTIVITY_PARCEL:
                break;
            case DataFileTypeEnum::CONNECTIVITY_PARCEL_DENSE:
                break;
            case DataFileTypeEnum::CONNECTIVITY_PARCEL_DYNAMIC:
                validFlag = includeNonWritableFileTypesFlag;
                break;
            case DataFileTypeEnum::CONNECTIVITY_PARCEL_LABEL:
                break;
            case DataFileTypeEnum::CONNECTIVITY_PARCEL_SCALAR:
                break;
            case DataFileTypeEnum::CONNECTIVITY_PARCEL_SERIES:
                break;
            case DataFileTypeEnum::CONNECTIVITY_DENSE_SCALAR:
                break;
            case DataFileTypeEnum::CONNECTIVITY_DENSE_TIME_SERIES:
                break;
            case DataFileTypeEnum::CONNECTIVITY_FIBER_ORIENTATIONS_TEMPORARY:
                break;
            case DataFileTypeEnum::CONNECTIVITY_FIBER_TRAJECTORY_TEMPORARY:
                break;
            case DataFileTypeEnum::CONNECTIVITY_SCALAR_DATA_SERIES:
                break;
            case DataFileTypeEnum::CZI_IMAGE_FILE:
                break;
            case DataFileTypeEnum::FOCI:
                break;
            case DataFileTypeEnum::HISTOLOGY_SLICES:
                break;
            case DataFileTypeEnum::IMAGE:
                break;
            case DataFileTypeEnum::LABEL:
                break;
            case DataFileTypeEnum::METRIC:
                break;
            case DataFileTypeEnum::METRIC_DYNAMIC:
                validFlag = includeNonWritableFileTypesFlag;
                break;
            case DataFileTypeEnum::OME_ZARR_IMAGE_FILE:
                break;
            case DataFileTypeEnum::PALETTE:
                break;
            case DataFileTypeEnum::RGBA:
                break;
            case DataFileTypeEnum::SAMPLES:
                break;
            case DataFileTypeEnum::SCENE:
                break;
            case DataFileTypeEnum::SPECIFICATION:
                break;
            case DataFileTypeEnum::SURFACE:
                break;
            case DataFileTypeEnum::UNKNOWN:
                validFlag = false;
                break;
            case DataFileTypeEnum::VOLUME:
                break;
            case DataFileTypeEnum::VOLUME_DYNAMIC:
                validFlag = includeNonWritableFileTypesFlag;
                break;
        }
        
        if (validFlag) {
            allExtensions.insert(allExtensions.end(),
                                 enumIter->readAndWriteFileExtensions.begin(),
                                 enumIter->readAndWriteFileExtensions.end());
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
    for (std::vector<AString>::const_iterator iter = enumInstance->writeFileExtensions.begin();
         iter != enumInstance->writeFileExtensions.end();
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
    
    if ( ! enumInstance->readAndWriteFileExtensions.empty()) {
        ext = enumInstance->readAndWriteFileExtensions[0];
    }
    
    if (enumValue == IMAGE) {
        /*
         * Image file's preferred extension
         */
        std::vector<AString> readFileExtensions;
        std::vector<AString> writeFileExtensions;
        getWorkbenchSupportedImageFileExtensions(readFileExtensions,
                                                 writeFileExtensions,
                                                 ext);
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
    for (std::vector<AString>::const_iterator iter = enumInstance->readAndWriteFileExtensions.begin();
         iter != enumInstance->readAndWriteFileExtensions.end();
         iter++) {
        const AString ext = ("." + *iter);
        if (filename.endsWith(ext)) {
            return true;
        }
    }
    
    return false;
}

/***
 * Does the filename have a valid extension for writing the given file type?
 *
 * @param filename
 *    Name of file that may not have the correct file extension for writing
 * @param enumValue
 *    The data file type.
 * @return
 *    True if the filename has a valid writing extension, else false.
 */
bool
DataFileTypeEnum::isValidWriteFileExtension(const AString& filename,
                                            const Enum enumValue)
{
    std::vector<AString> extensions = DataFileTypeEnum::getAllFileExtensionsForWriting(enumValue);
    
    const AString ext(FileInformation(filename).getFileExtension());
    if (std::find(extensions.begin(),
                  extensions.end(),
                  ext) != extensions.end()) {
        return true;
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
        const std::vector<AString> extensions = iter->readAndWriteFileExtensions;
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
 * @param options
 *     Bitwise mask for options
 */
void
DataFileTypeEnum::getAllEnums(std::vector<DataFileTypeEnum::Enum>& allEnums,
                              const uint32_t options)
{
    if (initializedFlag == false) initialize();
    
    allEnums.clear();
    
    const bool includeDenseDynamicFlag  = (options & OPTIONS_INCLUDE_CONNECTIVITY_DENSE_DYNAMIC);
    const bool includeParcelDynamicFlag = (options & OPTIONS_INCLUDE_CONNECTIVITY_PARCEL_DYNAMIC);
    const bool includeMetricDynamicFlag = (options & OPTIONS_INCLUDE_METRIC_DENSE_DYNAMIC);
    const bool includeVolumeDynamicFlag = (options & OPTIONS_INCLUDE_VOLUME_DENSE_DYNAMIC);
    const bool includeUnknownFlag       = (options & OPTIONS_INCLUDE_UNKNOWN);
    
    for (const auto& dataType : enumData) {
        bool addEnumFlag(true);
        
        switch (dataType.enumValue) {
            case DataFileTypeEnum::ANNOTATION:
                break;
            case DataFileTypeEnum::ANNOTATION_TEXT_SUBSTITUTION:
                break;
            case DataFileTypeEnum::BORDER:
                break;
            case DataFileTypeEnum::CONNECTIVITY_DENSE:
                break;
            case DataFileTypeEnum::CONNECTIVITY_DENSE_DYNAMIC:
                if ( ! includeDenseDynamicFlag) {
                    addEnumFlag = false;
                }
                break;
            case DataFileTypeEnum::CONNECTIVITY_DENSE_LABEL:
                break;
            case DataFileTypeEnum::CONNECTIVITY_DENSE_PARCEL:
                break;
            case DataFileTypeEnum::CONNECTIVITY_PARCEL:
                break;
            case DataFileTypeEnum::CONNECTIVITY_PARCEL_DENSE:
                break;
            case DataFileTypeEnum::CONNECTIVITY_PARCEL_DYNAMIC:
                if ( ! includeParcelDynamicFlag) {
                    addEnumFlag = false;
                }
                break;
            case DataFileTypeEnum::CONNECTIVITY_PARCEL_LABEL:
                break;
            case DataFileTypeEnum::CONNECTIVITY_PARCEL_SCALAR:
                break;
            case DataFileTypeEnum::CONNECTIVITY_PARCEL_SERIES:
                break;
            case DataFileTypeEnum::CONNECTIVITY_DENSE_SCALAR:
                break;
            case DataFileTypeEnum::CONNECTIVITY_DENSE_TIME_SERIES:
                break;
            case DataFileTypeEnum::CONNECTIVITY_FIBER_ORIENTATIONS_TEMPORARY:
                break;
            case DataFileTypeEnum::CONNECTIVITY_FIBER_TRAJECTORY_TEMPORARY:
                break;
            case DataFileTypeEnum::CONNECTIVITY_SCALAR_DATA_SERIES:
                break;
            case DataFileTypeEnum::CZI_IMAGE_FILE:
                break;
            case DataFileTypeEnum::FOCI:
                break;
            case DataFileTypeEnum::HISTOLOGY_SLICES:
                break;
            case DataFileTypeEnum::IMAGE:
                break;
            case DataFileTypeEnum::LABEL:
                break;
            case DataFileTypeEnum::METRIC:
                break;
            case DataFileTypeEnum::METRIC_DYNAMIC:
                if ( ! includeMetricDynamicFlag) {
                    addEnumFlag = false;
                }
                break;
            case DataFileTypeEnum::OME_ZARR_IMAGE_FILE:
                break;
            case DataFileTypeEnum::PALETTE:
                break;
            case DataFileTypeEnum::RGBA:
                break;
            case DataFileTypeEnum::SAMPLES:
                break;
            case DataFileTypeEnum::SCENE:
                break;
            case DataFileTypeEnum::SPECIFICATION:
                break;
            case DataFileTypeEnum::SURFACE:
                break;
            case DataFileTypeEnum::UNKNOWN:
                if ( ! includeUnknownFlag) {
                    addEnumFlag = false;
                }
                break;
            case DataFileTypeEnum::VOLUME:
                break;
            case DataFileTypeEnum::VOLUME_DYNAMIC:
                if ( ! includeVolumeDynamicFlag) {
                    addEnumFlag = false;
                }
                break;
        }
        
        if (addEnumFlag) {
            allEnums.push_back(dataType.enumValue);
        }
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

/**
 * Create a QDialog file filter using the given file type name and extensions
 * @param fileTypeName
 *    Description of file type name
 * @param fileExtensions
 *    Valid file extensions
 */
AString
DataFileTypeEnum::createQFileDialogNameFilter(const AString& fileTypeName,
                                              const std::vector<AString>& fileExtensions)
{
    AString filterText(fileTypeName + " Files (");
    
    bool firstTime(true);
    for (auto& ext : fileExtensions) {
        if ( ! firstTime) {
            filterText += " ";
        }
        filterText += ("*." + ext);
        
        firstTime = false;
    }
    filterText += ")";
    
    return filterText;
}

/**
 * Get all image file extensions supported by Qt for reading and writing image files.
 * @param readableExtensionsOut
 *    Output contains all readable image file extensions
 * @param writableExtensionsOut
 *    Output contains all writable image file extensions
 */
void
DataFileTypeEnum::getQtSupportedImageFileExtensions(std::vector<AString>& readableExtensionsOut,
                                                    std::vector<AString>& writableExtensionsOut)
{
    readableExtensionsOut.clear();
    writableExtensionsOut.clear();
    
    /*
     * Extensions Qt can read (use set so extensions sorted)
     */
    std::set<AString> qtReadExtensions;
    QList<QByteArray> readTypes(QImageReader::supportedImageFormats());
    QListIterator<QByteArray> readTypesIterator(readTypes);
    while (readTypesIterator.hasNext()) {
        QString fmt(readTypesIterator.next());
        qtReadExtensions.insert(fmt);
    }
    
    /*
     * Extensions Qt can write
     */
    std::set<AString> qtWriteExtensions;
    QList<QByteArray> writeTypes(QImageWriter::supportedImageFormats());
    QListIterator<QByteArray> writeTypesIterator(writeTypes);
    while (writeTypesIterator.hasNext()) {
        QString fmt(writeTypesIterator.next());
        qtWriteExtensions.insert(fmt);
    }
    
    readableExtensionsOut.insert(readableExtensionsOut.end(),
                                 qtReadExtensions.begin(),
                                 qtReadExtensions.end());
    
    writableExtensionsOut.insert(writableExtensionsOut.end(),
                                 qtWriteExtensions.begin(),
                                 qtWriteExtensions.end());
}

/**
 * Get all image file extensions supported by Workbench for reading and writing image files.
 * These are a subset of the extensions supported by Qt.
 * @param readableExtensionsOut
 *    Output contains all readable image file extensions
 * @param writableExtensionsOut
 *    Output contains all writable image file extensions
 * @param defaultWritableExtension
 *    Default extension for writing images
 */
void
DataFileTypeEnum::getWorkbenchSupportedImageFileExtensions(std::vector<AString>& readableExtensionsOut,
                                                           std::vector<AString>& writableExtensionsOut,
                                                           AString& defaultWritableExtension)
{
    readableExtensionsOut.clear();
    writableExtensionsOut.clear();
    defaultWritableExtension.clear();
    
    /*
     * Extensions that we want to support
     * There are some such as SVG that we don't want to support
     */
    std::set<AString> supportedExtensions;
    supportedExtensions.insert("bmp");
    supportedExtensions.insert("gif");
    supportedExtensions.insert("jpg");
    supportedExtensions.insert("jpeg");
    supportedExtensions.insert("jp2");
    supportedExtensions.insert("png");
    supportedExtensions.insert("ppm");
    supportedExtensions.insert("tiff");
    supportedExtensions.insert("tif");
    
    std::vector<AString> qtReadExtensions;
    std::vector<AString> qtWriteExtensions;
    getQtSupportedImageFileExtensions(qtReadExtensions,
                                      qtWriteExtensions);
    
    /*
     * Use only those extensions preferred for reading and writing
     */
    for (auto& ext : supportedExtensions) {
        if (std::find(qtReadExtensions.begin(),
                      qtReadExtensions.end(),
                      ext) != qtReadExtensions.end()) {
            readableExtensionsOut.push_back(ext);
        }
        if (std::find(qtWriteExtensions.begin(),
                      qtWriteExtensions.end(),
                      ext) != qtWriteExtensions.end()) {
            writableExtensionsOut.push_back(ext);
        }
    }
    
    AString pngExtension;
    AString jpegExtension;
    AString jpgExtension;
    AString tifExtension;
    AString tiffExtension;
    
    for (auto& extension : writableExtensionsOut) {
        if (extension == "png") {
            pngExtension = extension;
        }
        else if (extension == "jpg") {
            jpgExtension = extension;
        }
        else if (extension == "jpeg") {
            jpegExtension = extension;
        }
        else if (extension == "tif") {
            tifExtension = extension;
        }
        else if (extension == "tiff") {
            tiffExtension = extension;
        }
    }

    if ( ! writableExtensionsOut.empty()) {
        defaultWritableExtension = writableExtensionsOut[0];
        
        if ( ! pngExtension.isEmpty()) {
            defaultWritableExtension = pngExtension;
        }
        else if ( ! jpgExtension.isEmpty()) {
            defaultWritableExtension = jpgExtension;
        }
        else if ( ! jpegExtension.isEmpty()) {
            defaultWritableExtension = jpegExtension;
        }
        else if ( ! tiffExtension.isEmpty()) {
            defaultWritableExtension = tiffExtension;
        }
        else if ( ! tifExtension.isEmpty()) {
            defaultWritableExtension = tifExtension;
        }
    }
}

/**
 * Get filters for saving images with each image type in its own filter and the default filter
 * @param imageFiltersOut
 *    All image filters to saving images
 * @param defaultImageFilterOut
 *    Default image filter
 */
void
DataFileTypeEnum::getSaveQFileDialogImageFilters(std::vector<AString>& imageFiltersOut,
                                                 AString& defaultImageFilterOut)
{
    imageFiltersOut.clear();
    defaultImageFilterOut.clear();
    
    std::vector<AString> readableExtensions;
    std::vector<AString> writableExtensions;
    AString defaultWritableExtension;
    getWorkbenchSupportedImageFileExtensions(readableExtensions,
                                             writableExtensions,
                                             defaultWritableExtension);
    
    const DataFileTypeEnum* imageType = findData(DataFileTypeEnum::IMAGE);
    CaretAssert(imageType);
    
    for (const auto& extension: writableExtensions) {
        const AString name(extension.toUpper()
                           + " "
                           + imageType->guiName);
        const std::vector<AString> extVector { extension };
        
        const AString filter(createQFileDialogNameFilter(name,
                                                         extVector));
        imageFiltersOut.push_back(filter);

        if (extension == defaultWritableExtension) {
            defaultImageFilterOut = filter;
        }
    }
    
    if (defaultImageFilterOut.isEmpty()) {
        if ( ! imageFiltersOut.empty()) {
            defaultImageFilterOut = imageFiltersOut[0];
        }
    }
}

/**
 * Get files extensions supports by QMovie
 * @param readableExtensionsOut
 *   Output containing extensions of files that can be read by QMovie
 */
void
DataFileTypeEnum::getQtSupportedMovieFileExtensions(std::vector<AString>& readableExtensionsOut)
{
    /*
     * Extensions Qt can read (use set so extensions sorted)
     */
    std::set<AString> qtReadExtensions;
    QList<QByteArray> readTypes(QMovie::supportedFormats());
    QListIterator<QByteArray> readTypesIterator(readTypes);
    while (readTypesIterator.hasNext()) {
        QString fmt(readTypesIterator.next());
        qtReadExtensions.insert(fmt);
    }

    readableExtensionsOut.clear();
    readableExtensionsOut.insert(readableExtensionsOut.end(),
                                 qtReadExtensions.begin(),
                                 qtReadExtensions.end());
}

/**
 * @return The dialog filter type for the given file type (show/select files or directories)
 * @param enumValue
 *    The enumerated type
 */
DataFileTypeEnum::DialogFilterShowType
DataFileTypeEnum::getDialogFilterShowType(const Enum enumValue)
{
    DialogFilterShowType dialogFilterShowType(DialogFilterShowType::SHOW_FILES);
    
    switch (enumValue) {
        case DataFileTypeEnum::ANNOTATION:
            break;
        case DataFileTypeEnum::ANNOTATION_TEXT_SUBSTITUTION:
            break;
        case DataFileTypeEnum::BORDER:
            break;
        case DataFileTypeEnum::CONNECTIVITY_DENSE:
            break;
        case DataFileTypeEnum::CONNECTIVITY_DENSE_DYNAMIC:
            break;
        case DataFileTypeEnum::CONNECTIVITY_DENSE_LABEL:
            break;
        case DataFileTypeEnum::CONNECTIVITY_DENSE_PARCEL:
            break;
        case DataFileTypeEnum::CONNECTIVITY_PARCEL:
            break;
        case DataFileTypeEnum::CONNECTIVITY_PARCEL_DENSE:
            break;
        case DataFileTypeEnum::CONNECTIVITY_PARCEL_DYNAMIC:
            break;
        case DataFileTypeEnum::CONNECTIVITY_PARCEL_LABEL:
            break;
        case DataFileTypeEnum::CONNECTIVITY_PARCEL_SCALAR:
            break;
        case DataFileTypeEnum::CONNECTIVITY_PARCEL_SERIES:
            break;
        case DataFileTypeEnum::CONNECTIVITY_DENSE_SCALAR:
            break;
        case DataFileTypeEnum::CONNECTIVITY_DENSE_TIME_SERIES:
            break;
        case DataFileTypeEnum::CONNECTIVITY_FIBER_ORIENTATIONS_TEMPORARY:
            break;
        case DataFileTypeEnum::CONNECTIVITY_FIBER_TRAJECTORY_TEMPORARY:
            break;
        case DataFileTypeEnum::CONNECTIVITY_SCALAR_DATA_SERIES:
            break;
        case DataFileTypeEnum::CZI_IMAGE_FILE:
            break;
        case DataFileTypeEnum::FOCI:
            break;
        case DataFileTypeEnum::HISTOLOGY_SLICES:
            break;
        case DataFileTypeEnum::IMAGE:
            break;
        case DataFileTypeEnum::LABEL:
            break;
        case DataFileTypeEnum::METRIC:
            break;
        case DataFileTypeEnum::METRIC_DYNAMIC:
            break;
        case DataFileTypeEnum::OME_ZARR_IMAGE_FILE:
            dialogFilterShowType = DialogFilterShowType::SHOW_DIRECTORY;
            break;
        case DataFileTypeEnum::PALETTE:
            break;
        case DataFileTypeEnum::RGBA:
            break;
        case DataFileTypeEnum::SAMPLES:
            break;
        case DataFileTypeEnum::SCENE:
            break;
        case DataFileTypeEnum::SPECIFICATION:
            break;
        case DataFileTypeEnum::SURFACE:
            break;
        case DataFileTypeEnum::UNKNOWN:
            break;
        case DataFileTypeEnum::VOLUME:
            break;
        case DataFileTypeEnum::VOLUME_DYNAMIC:
            break;
    }

    return dialogFilterShowType;
}

