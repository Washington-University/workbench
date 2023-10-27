#ifndef __DATA_FILE_TYPE_ENUM__H_
#define __DATA_FILE_TYPE_ENUM__H_

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


#include <stdint.h>
#include <vector>
#include "AString.h"

namespace caret {

class DataFileTypeEnum {

public:
    /**
     * Enumerated values.
     */
    enum Enum {
        /** Annotation */
        ANNOTATION,
        /** Annotation Text Substitution */
        ANNOTATION_TEXT_SUBSTITUTION,
        /** Border */
        BORDER,
        /** Connectivity - Dense */
        CONNECTIVITY_DENSE,
        /** Connectivity - Dense Dynamic (correlate from time-series)*/
        CONNECTIVITY_DENSE_DYNAMIC,
        /** Connectivity - Dense Label */
        CONNECTIVITY_DENSE_LABEL,
        /** Connectivity - Dense Parcel */
        CONNECTIVITY_DENSE_PARCEL,
        /** Connectivity - Dense Scalar */
        CONNECTIVITY_DENSE_SCALAR,
        /** Connectivity - Dense Time Series */
        CONNECTIVITY_DENSE_TIME_SERIES,
        /** Connectivity - Fiber Orientations TEMPORARY */
        CONNECTIVITY_FIBER_ORIENTATIONS_TEMPORARY,
        /** Connectivity - Fiber Trajectory TEMPORARY */
        CONNECTIVITY_FIBER_TRAJECTORY_TEMPORARY,
        /** Connectivity - Parcel */
        CONNECTIVITY_PARCEL,
        /** Connectivity - Parcel Dense*/
        CONNECTIVITY_PARCEL_DENSE,
        /** Connectivity - Parcel Dynamic (correlate from parcel-series)*/
        CONNECTIVITY_PARCEL_DYNAMIC,
        /** Connectivity - Parcel Label*/
        CONNECTIVITY_PARCEL_LABEL,
        /** Connectivity - Parcel Scalar */
        CONNECTIVITY_PARCEL_SCALAR,
        /** Connectivity - Parcel Series */
        CONNECTIVITY_PARCEL_SERIES,
        /** Connectivity - Scalar Data Series */
        CONNECTIVITY_SCALAR_DATA_SERIES,
        /** Zeiss CZI Image File */
        CZI_IMAGE_FILE,
        /** Foci */
        FOCI,
        /** Histology Slices File */
        HISTOLOGY_SLICES,
        /** Image */
        IMAGE,
        /** Labels */
        LABEL,
        /** Metric */
        METRIC,
        /** Metric Dynamic Connectivity */
        METRIC_DYNAMIC,
        /** Palette */
        PALETTE,
        /** RGBA */
        RGBA,
        /** Samples */
        SAMPLES,
        /** Scene */
        SCENE,
        /** Specification */
        SPECIFICATION,
        /**  Surface */
        SURFACE,
        /** Unknown */
        UNKNOWN,
        /** Volume */
        VOLUME,
        /** Volume Dynamic Connectivity*/
        VOLUME_DYNAMIC
    };
    
    /**
     * Options for getting all enumerated values.
     * Bitwise 'OR' for multiple options
     */
    enum Options {
        /** No options */
        OPTIONS_NONE = 0,
        /** Include the dense dynamic data file type */
        OPTIONS_INCLUDE_CONNECTIVITY_DENSE_DYNAMIC = 1,
        /** Include the parcel dynamic data file type */
        OPTIONS_INCLUDE_CONNECTIVITY_PARCEL_DYNAMIC = 2,
        /** Include the metric dynamic data file type */
        OPTIONS_INCLUDE_METRIC_DENSE_DYNAMIC = 4,
        /** Include the volume dynamic data file type */
        OPTIONS_INCLUDE_VOLUME_DENSE_DYNAMIC = 8,
        /** Include the unknown data file type */
        OPTIONS_INCLUDE_UNKNOWN = 16
    };
    
    ~DataFileTypeEnum();

    static void reinitializeDataFileTypeEnums();
    
    static AString toName(Enum enumValue);
    
    static Enum fromName(const AString& name, bool* isValidOut);
    
    static AString toGuiName(Enum enumValue);
    
    static AString toShortGuiName(Enum enumValue);
    
    static AString toOverlayTypeName(Enum enumValue);
    
    static Enum fromOverlayTypeName(const AString& overlayTypeName, bool* isValidOut);
    
    static int32_t toIntegerCode(Enum enumValue);
    
    static Enum fromIntegerCode(const int32_t integerCode, bool* isValidOut);

    static void getAllEnums(std::vector<Enum>& allEnums,
                            const uint32_t options);

    static Enum fromQFileDialogFilter(const AString& qFileDialogNameFilter, bool* isValidOut);
    
    static AString toQFileDialogFilterForReading(const Enum enumValue);
    
    static AString toQFileDialogFilterForWriting(const Enum enumValue);
    
    static Enum fromFileExtension(const AString& filename, bool* isValidOut);
    
    static AString toFileExtension(const Enum enumValue);
    
    static bool isValidFileExtension(const AString& filename,
                                     const Enum enumValue);
    
    static bool isValidWriteFileExtension(const AString& filename,
                                          const Enum enumValue);
    
    static std::vector<AString> getAllFileExtensions(const Enum enumValue);
    
    static std::vector<AString> getAllFileExtensionsForReading(const Enum enumValue);
    
    static std::vector<AString> getAllFileExtensionsForWriting(const Enum enumValue);
    
    static std::vector<AString> getFilesExtensionsForEveryFile(const bool includeNonWritableFileTypesFlag = false);
    
    static std::vector<AString> getWildCardMatching(const Enum enumValue);
    
    static std::vector<AString> getFilesInDirectory(const Enum enumValue,
                                                    const AString& directoryPath);
    
    static bool isFileUsedWithOneStructure(const Enum enumValue);
    
    static bool isConnectivityDataType(const Enum enumValue);
    
    static void getAllConnectivityEnums(std::vector<Enum>& connectivityEnumsOut);
    
    static AString addFileExtensionIfMissing(const AString& filename,
                                             const Enum enumValue);
    
    static void getSaveQFileDialogImageFilters(std::vector<AString>& imageFiltersOut,
                                               AString& defaultImageFilterOut);
    
    static void getQtSupportedImageFileExtensions(std::vector<AString>& readableExtensionsOut,
                                                  std::vector<AString>& writableExtensionsOut);
    
    static void getWorkbenchSupportedImageFileExtensions(std::vector<AString>& readableExtensionsOut,
                                                         std::vector<AString>& writableExtensionsOut,
                                                         AString& defaultWritableExtension);

    static void getQtSupportedMovieFileExtensions(std::vector<AString>& readableExtensionsOut);
    
    static AString toCziImageFileExtension() { return ".czi"; }
    
private:
    DataFileTypeEnum(const Enum enumValue, 
                     const AString& name,
                     const AString& guiName,
                     const AString& overlayTypeName,
                     const bool fileIsUsedWithOneStructure,
                     const AString& fileExtensionOne,
                     const AString& fileExtensionTwo = "",
                     const AString& fileExtensionThree = "",
                     const AString& fileExtensionFour = "");

    static const DataFileTypeEnum* findData(const Enum enumValue);

    static AString createQFileDialogNameFilter(const AString& fileTypeName,
                                               const std::vector<AString>& fileExtensions);
    
    /** Holds all instance of enum values and associated metadata */
    static std::vector<DataFileTypeEnum> enumData;

    /** Initialize instances that contain the enum values and metadata */
    static void initialize();

    /** Indicates instance of enum values and metadata have been initialized */
    static bool initializedFlag;
    
    /** Automatically generates the integer code */
    static int32_t integerCodeGenerator;
    
    /** The enumerated type value for an instance */
    Enum enumValue;

    /** The integer code associated with an enumerated value */
    int32_t integerCode;

    /** The name, a text string that is identical to the enumerated value */
    AString name;
    
    /** A user-friendly name that is displayed in the GUI */
    AString guiName;
    
    /** Name for use in overlay selection */
    AString overlayTypeName;
    
    /** All Extension(s) for the file */
    std::vector<AString> readAndWriteFileExtensions;
    
    /** Extension(s) for reading the file */
    std::vector<AString> readFileExtensions;
    
    /** Extension(s) for writing the file */
    std::vector<AString> writeFileExtensions;
    
    /** Name filter for reading files in a QFileDialog */
    AString qReadFileDialogNameFilter;
    
    /** Name filter for writing files in a QFileDialog */
    AString qWriteFileDialogNameFilter;
    
    /** Is file for use with one structure */
    bool oneStructureFlag;
};

#ifdef __DATA_FILE_TYPE_ENUM_DECLARE__
std::vector<DataFileTypeEnum> DataFileTypeEnum::enumData;
bool DataFileTypeEnum::initializedFlag = false;
    int32_t DataFileTypeEnum::integerCodeGenerator = 0;
#endif // __DATA_FILE_TYPE_ENUM_DECLARE__

} // namespace
#endif  //__DATA_FILE_TYPE_ENUM__H_
