
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

#define __VOLUME_TYPE_ENUM_DECLARE__
#include "VolumeTypeEnum.h"
#undef __VOLUME_TYPE_ENUM_DECLARE__

using namespace caret;

/**
 * Constructor.
 *
 * @param e
 *    An enumerated value.
 * @param name
 *    Name of enumberated value.
 */
VolumeTypeEnum::VolumeTypeEnum(const Enum e,
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
VolumeTypeEnum::~VolumeTypeEnum()
{
}

/**
 * Initialize the enumerated metadata.
 */
void
VolumeTypeEnum::initialize()
{
    if (initializedFlag) {
        return;
    }
    initializedFlag = true;

    enumData.push_back(VolumeTypeEnum(, 
                                    0, 
                                    "", 
                                    ""));
    
    enumData.push_back(VolumeTypeEnum(, 
                                    1, 
                                    "", 
                                    ""));
    
    enumData.push_back(VolumeTypeEnum(, 
                                    2, 
                                    "", 
                                    ""));
    
}

/**
 * Find the data for and enumerated value.
 * @param e
 *     The enumerated value.
 * @return Pointer to data for this enumerated type
 * or NULL if no data for type or if type is invalid.
 */
const VolumeTypeEnum*
VolumeTypeEnum::findData(const Enum e)
{
    initialize();

    size_t num = enumData.size();
    for (size_t i = 0; i < num; i++) {
        const VolumeTypeEnum* d = &enumData[i];
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
VolumeTypeEnum::toName(Enum e) {
    initialize();
    
    const VolumeTypeEnum* enumValue = findData(e);
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
VolumeTypeEnum::Enum 
VolumeTypeEnum::fromName(const AString& s, bool* isValidOut)
{
    initialize();
    
    bool validFlag = false;
    Enum e;
    
    for (std::vector<VolumeTypeEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const VolumeTypeEnum& d = *iter;
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
VolumeTypeEnum::toGuiName(Enum e) {
    initialize();
    
    const VolumeTypeEnum* enumValue = findData(e);
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
VolumeTypeEnum::Enum 
VolumeTypeEnum::fromGuiName(const AString& s, bool* isValidOut)
{
    initialize();
    
    bool validFlag = false;
    Enum e;
    
    for (std::vector<VolumeTypeEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const VolumeTypeEnum& d = *iter;
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
VolumeTypeEnum::toIntegerCode(Enum e)
{
    initialize();
    const VolumeTypeEnum* enumValue = findData(e);
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
VolumeTypeEnum::Enum
VolumeTypeEnum::fromIntegerCode(const int32_t integerCode, bool* isValidOut)
{
    initialize();
    
    bool validFlag = false;
    Enum e = <REPLACE_WITH_DEFAULT_ENUM_VALUE>;
    
    for (std::vector<VolumeTypeEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const VolumeTypeEnum& enumValue = *iter;
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

