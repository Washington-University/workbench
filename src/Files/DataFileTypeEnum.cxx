
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

using namespace caret;

/**
 * Constructor.
 *
 * @param e
 *    An enumerated value.
 * @param name
 *    Name of enumberated value.
 */
DataFileTypeEnum::DataFileTypeEnum(const Enum e,
                                   const int32_t integerCode,
                                   const AString& name,
                                   const AString& guiName,
                                   const AString& fileExtension1,
                                   const AString& fileExtension2)
{
    this->e = e;
    this->integerCode = integerCode;
    this->name = name;
    this->guiName = guiName;
    
    if (fileExtension1.isEmpty() == false) {
        this->fileExtensions.push_back(fileExtension1);
    }
    if (fileExtension2.isEmpty() == false) {
        this->fileExtensions.push_back(fileExtension2);
    }
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
                                    0, 
                                    "BORDER_PROJECTION", 
                                    "Border Projection",
                                        "borderproj"));
    
    enumData.push_back(DataFileTypeEnum(CIFTI, 
                                    1, 
                                    "CIFTI", 
                                    "Connectivity",
                                        "cii"));
    
    enumData.push_back(DataFileTypeEnum(FOCI_PROJECTION, 
                                    2, 
                                    "FOCI_PROJECTION", 
                                    "Foci Projection",
                                        "fociproj"));
    
    enumData.push_back(DataFileTypeEnum(LABEL, 
                                    3, 
                                    "LABEL", 
                                    "Label",
                                        "label.gii"));
    
    enumData.push_back(DataFileTypeEnum(METRIC, 
                                    4, 
                                    "METRIC", 
                                    "Metric",
                                        "func.gii"));
    
    enumData.push_back(DataFileTypeEnum(PALETTE, 
                                    5, 
                                    "PALETTE", 
                                    "Palette",
                                        "palette"));
    
    enumData.push_back(DataFileTypeEnum(RGBA, 
                                    6, 
                                    "RGBA", 
                                    "RGBA",
                                        "rgba.gii"));
    
    enumData.push_back(DataFileTypeEnum(SCENE, 
                                    7, 
                                    "SCENE", 
                                    "Scene",
                                        "scene"));
    
    enumData.push_back(DataFileTypeEnum(SPECIFICATION, 
                                    8, 
                                    "SPECIFICATION", 
                                    "Specification",
                                        "spec"));
    
    enumData.push_back(DataFileTypeEnum(SURFACE_ANATOMICAL, 
                                    9, 
                                    "SURFACE_ANATOMICAL", 
                                    "Surface - Anatomical",
                                        "surf.gii"));
    
    enumData.push_back(DataFileTypeEnum(SURFACE_INFLATED, 
                                    10, 
                                    "SURFACE_INFLATED", 
                                        "Surface - Inflated",
                                        "surf.gii"));
    
    enumData.push_back(DataFileTypeEnum(SURFACE_VERY_INFLATED, 
                                    11, 
                                    "SURFACE_VERY_INFLATED", 
                                        "Surface - Very Inflated",
                                        "surf.gii"));
    
    enumData.push_back(DataFileTypeEnum(SURFACE_FLAT, 
                                    12, 
                                    "SURFACE_FLAT", 
                                        "Surface - Flat",
                                        "surf.gii"));
    
    enumData.push_back(DataFileTypeEnum(UNKNOWN, 
                                    13, 
                                    "UNKNOWN", 
                                    "Unknown",
                                        "unknown"));
    
    enumData.push_back(DataFileTypeEnum(VOLUME_ANATOMY, 
                                        14, 
                                        "VOLUME_ANATOMY", 
                                        "Volume - Anatomy",
                                        "nii",
                                        "nii.gz"));
    
    enumData.push_back(DataFileTypeEnum(VOLUME_FUNCTIONAL, 
                                    15, 
                                    "VOLUME_FUNCTIONAL", 
                                        "Volume - Functional",
                                        "nii",
                                        "nii.gz"));
    
    enumData.push_back(DataFileTypeEnum(VOLUME_LABEL, 
                                    16, 
                                    "VOLUME_LABEL", 
                                        "Volume - Label",
                                        "nii",
                                        "nii.gz"));
    
}

/**
 * Find the data for and enumerated value.
 * @param e
 *     The enumerated value.
 * @return Pointer to data for this enumerated type
 * or NULL if no data for type or if type is invalid.
 */
const DataFileTypeEnum*
DataFileTypeEnum::findData(const Enum e)
{
    initialize();

    size_t num = enumData.size();
    for (size_t i = 0; i < num; i++) {
        const DataFileTypeEnum* d = &enumData[i];
        if (d->e == e) {
            return d;
        }
    }

    return NULL;
}

/**
 * Get a string representation of the enumerated type.
 * @param e 
 *     Enumerated value.
 * @return 
 *     String representing enumerated value.
 */
AString 
DataFileTypeEnum::toName(Enum e) {
    initialize();
    
    const DataFileTypeEnum* enumValue = findData(e);
    return enumValue->name;
}

/**
 * Get an enumerated value corresponding to its name.
 * @param s 
 *     Name of enumerated value.
 * @param isValidOut 
 *     If not NULL, it is set indicating that a
 *     enum value exists for the input name.
 * @return 
 *     Enumerated value.
 */
DataFileTypeEnum::Enum 
DataFileTypeEnum::fromName(const AString& s, bool* isValidOut)
{
    initialize();
    
    bool validFlag = false;
    Enum e;
    
    for (std::vector<DataFileTypeEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const DataFileTypeEnum& d = *iter;
        if (d.name == s) {
            e = d.e;
            validFlag = true;
            break;
        }
    }
    
    if (isValidOut != 0) {
        *isValidOut = validFlag;
    }
    return e;
}

/**
 * Get a GUI string representation of the enumerated type.
 * @param e 
 *     Enumerated value.
 * @return 
 *     String representing enumerated value.
 */
AString 
DataFileTypeEnum::toGuiName(Enum e) {
    initialize();
    
    const DataFileTypeEnum* enumValue = findData(e);
    return enumValue->guiName;
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
DataFileTypeEnum::fromGuiName(const AString& s, bool* isValidOut)
{
    initialize();
    
    bool validFlag = false;
    Enum e;
    
    for (std::vector<DataFileTypeEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const DataFileTypeEnum& d = *iter;
        if (d.guiName == s) {
            e = d.e;
            validFlag = true;
            break;
        }
    }
    
    if (isValidOut != 0) {
        *isValidOut = validFlag;
    }
    return e;
}

/**
 * Get the integer code for a data type.
 *
 * @return
 *    Integer code for data type.
 */
int32_t
DataFileTypeEnum::toIntegerCode(Enum e)
{
    initialize();
    const DataFileTypeEnum* enumValue = findData(e);
    return enumValue->integerCode;
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
    initialize();
    
    bool validFlag = false;
    Enum e = DataFileTypeEnum::UNKNOWN;
    
    for (std::vector<DataFileTypeEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const DataFileTypeEnum& enumValue = *iter;
        if (enumValue.integerCode == integerCode) {
            e = enumValue.e;
            validFlag = true;
            break;
        }
    }
    
    if (isValidOut != 0) {
        *isValidOut = validFlag;
    }
    return e;
}

