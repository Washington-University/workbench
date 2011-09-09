
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

#define __SURFACE_OVERLAY_DATA_TYPE_ENUM_DECLARE__
#include "SurfaceOverlayDataTypeEnum.h"
#undef __SURFACE_OVERLAY_DATA_TYPE_ENUM_DECLARE__

using namespace caret;

/**
 * Constructor.
 *
 * @param e
 *    An enumerated value.
 * @param name
 *    Name of enumberated value.
 */
SurfaceOverlayDataTypeEnum::SurfaceOverlayDataTypeEnum(const Enum e,
                           const int32_t integerCode,
                           const AString& name,
                           const AString& guiName)
{
    this->e = e;
    this->integerCode = integerCode;
    this->name = name;
    this->guiName = guiName;
}

/**
 * Destructor.
 */
SurfaceOverlayDataTypeEnum::~SurfaceOverlayDataTypeEnum()
{
}

/**
 * Initialize the enumerated metadata.
 */
void
SurfaceOverlayDataTypeEnum::initialize()
{
    if (initializedFlag) {
        return;
    }
    initializedFlag = true;

    enumData.push_back(SurfaceOverlayDataTypeEnum(NONE, 
                                                  0, 
                                                  "NONE", 
                                                  "None"));
    
    enumData.push_back(SurfaceOverlayDataTypeEnum(CONNECTIVITY, 
                                                  1, 
                                                  "CONNECTIVITY", 
                                                  "Connectivity"));
    
    enumData.push_back(SurfaceOverlayDataTypeEnum(LABEL, 
                                                  2, 
                                                  "LABEL", 
                                                  "Label"));
    
    enumData.push_back(SurfaceOverlayDataTypeEnum(METRIC, 
                                                  3, 
                                                  "METRIC", 
                                                  "Metric"));
    
    enumData.push_back(SurfaceOverlayDataTypeEnum(RGBA, 
                                                  4, 
                                                  "RGBA", 
                                                  "RGBA"));
    
}

/**
 * Find the data for and enumerated value.
 * @param e
 *     The enumerated value.
 * @return Pointer to data for this enumerated type
 * or NULL if no data for type or if type is invalid.
 */
const SurfaceOverlayDataTypeEnum*
SurfaceOverlayDataTypeEnum::findData(const Enum e)
{
    initialize();

    size_t num = enumData.size();
    for (size_t i = 0; i < num; i++) {
        const SurfaceOverlayDataTypeEnum* d = &enumData[i];
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
SurfaceOverlayDataTypeEnum::toName(Enum e) {
    initialize();
    
    const SurfaceOverlayDataTypeEnum* enumValue = findData(e);
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
SurfaceOverlayDataTypeEnum::Enum 
SurfaceOverlayDataTypeEnum::fromName(const AString& s, bool* isValidOut)
{
    initialize();
    
    bool validFlag = false;
    Enum e;
    
    for (std::vector<SurfaceOverlayDataTypeEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const SurfaceOverlayDataTypeEnum& d = *iter;
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
SurfaceOverlayDataTypeEnum::toGuiName(Enum e) {
    initialize();
    
    const SurfaceOverlayDataTypeEnum* enumValue = findData(e);
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
SurfaceOverlayDataTypeEnum::Enum 
SurfaceOverlayDataTypeEnum::fromGuiName(const AString& s, bool* isValidOut)
{
    initialize();
    
    bool validFlag = false;
    Enum e;
    
    for (std::vector<SurfaceOverlayDataTypeEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const SurfaceOverlayDataTypeEnum& d = *iter;
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
SurfaceOverlayDataTypeEnum::toIntegerCode(Enum e)
{
    initialize();
    const SurfaceOverlayDataTypeEnum* enumValue = findData(e);
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
SurfaceOverlayDataTypeEnum::Enum
SurfaceOverlayDataTypeEnum::fromIntegerCode(const int32_t integerCode, bool* isValidOut)
{
    initialize();
    
    bool validFlag = false;
    Enum e = SurfaceOverlayDataTypeEnum::NONE;
    
    for (std::vector<SurfaceOverlayDataTypeEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const SurfaceOverlayDataTypeEnum& enumValue = *iter;
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

