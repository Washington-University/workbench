#ifndef __DATA_FILE_TYPE_ENUM__H_
#define __DATA_FILE_TYPE_ENUM__H_

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


#include <stdint.h>
#include <vector>
#include "AString.h"

namespace caret {

/// Enumerated type for data files
class DataFileTypeEnum {

public:
    /**
     * Enumerated values.
     */
    enum Enum {
        /** Border Projection */
        BORDER_PROJECTION,
        /** Connectivity - Dense */
        CONNECTIVITY_DENSE,
        /** Connectivity - Dense Time Series */
        CONNECTIVITY_DENSE_TIME_SERIES,
        /** Foci Projection */
        FOCI_PROJECTION,
        /** Labels */
        LABEL,
        /** Metric */
        METRIC,
        /** Palette */
        PALETTE,
        /** RGBA */
        RGBA,
        /** Scene */
        SCENE,
        /** Specification */
        SPECIFICATION,
        /**  Surface */
        SURFACE,
        /** Unknown */
        UNKNOWN,
        /** Volume */
        VOLUME
    };
    
    ~DataFileTypeEnum();

    static AString toName(Enum enumValue);
    
    static Enum fromName(const AString& name, bool* isValidOut);
    
    static AString toGuiName(Enum enumValue);
    
    static Enum fromGuiName(const AString& guiName, bool* isValidOut);
    
    static int32_t toIntegerCode(Enum enumValue);
    
    static Enum fromIntegerCode(const int32_t integerCode, bool* isValidOut);

    static void getAllEnums(std::vector<Enum>& allEnums,
                            const bool includeUnknown = false,
                            const bool isForDataFileDialog = false);

    static Enum fromQFileDialogFilter(const AString& qFileDialogNameFilter, bool* isValidOut);
    
    static AString toQFileDialogFilter(const Enum enumValue);
    
    static Enum fromFileExtension(const AString& filename, bool* isValidOut);
    
    static AString toFileExtension(const Enum enumValue);
    
    static bool isFileUsedWithOneStructure(const Enum enumValue);
    
    static bool isConnectivityDataType(const Enum enumValue);
    
    static void getAllConnectivityEnums(std::vector<Enum>& connectivityEnumsOut);
    
private:
    DataFileTypeEnum(const Enum enumValue, 
                     const AString& name,
                     const AString& guiName,
                     const bool fileIsUsedWithOneStructure,
                     const bool fileIsOpenedWithDataFileDialog,
                     const AString& fileExtensionOne,
                     const AString& fileExtensionTwo = "",
                     const AString& fileExtensionThree = "");

    static const DataFileTypeEnum* findData(const Enum enumValue);

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
    
    /** Extension(s) for the file */
    std::vector<AString> fileExtensions;
    
    /** Name filter for use in a QFileDialog */
    AString qFileDialogNameFilter;
    
    /** Is file for use with one structure */
    bool oneStructureFlag;
    
    /** File is opened with the data file dialog */
    bool fileIsOpenedWithDataFileDialog;
};

#ifdef __DATA_FILE_TYPE_ENUM_DECLARE__
std::vector<DataFileTypeEnum> DataFileTypeEnum::enumData;
bool DataFileTypeEnum::initializedFlag = false;
    int32_t DataFileTypeEnum::integerCodeGenerator = 0;
#endif // __DATA_FILE_TYPE_ENUM_DECLARE__

} // namespace
#endif  //__DATA_FILE_TYPE_ENUM__H_
