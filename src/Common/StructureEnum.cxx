
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

#define __STRUCTURE_ENUM_DECLARE__
#include "StructureEnum.h"
#undef __STRUCTURE_ENUM_DECLARE__

using namespace caret;

/**
 * Constructor.
 *
 * @param e
 *    An enumerated value.
 * @param name
 *    Name of enumberated value.
 */
StructureEnum::StructureEnum(const Enum e,
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
StructureEnum::~StructureEnum()
{
}

/**
 * Initialize the enumerated metadata.
 */
void
StructureEnum::initialize()
{
    if (initializedFlag) {
        return;
    }
    initializedFlag = true;

    enumData.push_back(StructureEnum(INVALID, 
                                    0, 
                                    "INVALID", 
                                    ""));
    
    enumData.push_back(StructureEnum(CORTEX_LEFT, 
                                    1, 
                                    "CORTEX_LEFT", 
                                    ""));
    
    enumData.push_back(StructureEnum(CORTEX_RIGHT, 
                                    2, 
                                    "CORTEX_RIGHT", 
                                    ""));
    
    enumData.push_back(StructureEnum(ACCUMBENS_LEFT, 
                                    3, 
                                    "ACCUMBENS_LEFT", 
                                    ""));
    
    enumData.push_back(StructureEnum(ACCUMBENS_RIGHT, 
                                    4, 
                                    "ACCUMBENS_RIGHT", 
                                    ""));
    
    enumData.push_back(StructureEnum(AMYGDALA_LEFT, 
                                    5, 
                                    "AMYGDALA_LEFT", 
                                    ""));
    
    enumData.push_back(StructureEnum(AMYGDALA_RIGHT, 
                                    6, 
                                    "AMYGDALA_RIGHT", 
                                    ""));
    
    enumData.push_back(StructureEnum(BRAIN_STEM, 
                                    7, 
                                    "BRAIN_STEM", 
                                    ""));
    
    enumData.push_back(StructureEnum(CAUDATE_LEFT, 
                                    8, 
                                    "CAUDATE_LEFT", 
                                    ""));
    
    enumData.push_back(StructureEnum(CAUDATE_RIGHT, 
                                    9, 
                                    "CAUDATE_RIGHT", 
                                    ""));
    
    enumData.push_back(StructureEnum(CEREBELLUM, 
                                    10, 
                                    "CEREBELLUM", 
                                    ""));
    
    enumData.push_back(StructureEnum(CEREBELLUM_LEFT, 
                                    11, 
                                    "CEREBELLUM_LEFT", 
                                    ""));
    
    enumData.push_back(StructureEnum(CEREBELLUM_RIGHT, 
                                    12, 
                                    "CEREBELLUM_RIGHT", 
                                    ""));
    
    enumData.push_back(StructureEnum(DIENCEPHALON_VENTRAL_LEFT, 
                                    13, 
                                    "DIENCEPHALON_VENTRAL_LEFT", 
                                    ""));
    
    enumData.push_back(StructureEnum(DIENCEPHALON_VENTRAL_RIGHT, 
                                    14, 
                                    "DIENCEPHALON_VENTRAL_RIGHT", 
                                    ""));
    
    enumData.push_back(StructureEnum(HIPPOCAMPUS_LEFT, 
                                    15, 
                                    "HIPPOCAMPUS_LEFT", 
                                    ""));
    
    enumData.push_back(StructureEnum(HIPPOCAMPUS_RIGHT, 
                                    16, 
                                    "HIPPOCAMPUS_RIGHT", 
                                    ""));
    
    enumData.push_back(StructureEnum(PALLIDUM_LEFT, 
                                    17, 
                                    "PALLIDUM_LEFT", 
                                    ""));
    
    enumData.push_back(StructureEnum(PALLIDUM_RIGHT, 
                                    18, 
                                    "PALLIDUM_RIGHT", 
                                    ""));
    
    enumData.push_back(StructureEnum(OTHER, 
                                    19, 
                                    "OTHER", 
                                    ""));
    
    enumData.push_back(StructureEnum(PUTAMEN_LEFT, 
                                    20, 
                                    "PUTAMEN_LEFT", 
                                    ""));
    
    enumData.push_back(StructureEnum(PUTAMEN_RIGHT, 
                                    21, 
                                    "PUTAMEN_RIGHT", 
                                    ""));
    
    enumData.push_back(StructureEnum(SUBCORTICAL_WHITE_MATTER_LEFT, 
                                    22, 
                                    "SUBCORTICAL_WHITE_MATTER_LEFT", 
                                    ""));
    
    enumData.push_back(StructureEnum(SUBCORTICAL_WHITE_MATTER_RIGHT, 
                                    23, 
                                    "SUBCORTICAL_WHITE_MATTER_RIGHT", 
                                    ""));
    
    enumData.push_back(StructureEnum(THALAMUS_LEFT, 
                                    24, 
                                    "THALAMUS_LEFT", 
                                    ""));
    
    enumData.push_back(StructureEnum(THALAMUS_RIGHT, 
                                    25, 
                                    "THALAMUS_RIGHT", 
                                    ""));
}

/**
 * Find the data for and enumerated value.
 * @param e
 *     The enumerated value.
 * @return Pointer to data for this enumerated type
 * or NULL if no data for type or if type is invalid.
 */
const StructureEnum*
StructureEnum::findData(const Enum e)
{
    initialize();

    size_t num = enumData.size();
    for (size_t i = 0; i < num; i++) {
        const StructureEnum* d = &enumData[i];
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
StructureEnum::toName(Enum e) {
    initialize();
    
    const StructureEnum* enumValue = findData(e);
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
StructureEnum::Enum 
StructureEnum::fromName(const AString& s, bool* isValidOut)
{
    initialize();
    
    bool validFlag = false;
    Enum e;
    
    for (std::vector<StructureEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const StructureEnum& d = *iter;
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
StructureEnum::toGuiName(Enum e) {
    initialize();
    
    const StructureEnum* enumValue = findData(e);
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
StructureEnum::Enum 
StructureEnum::fromGuiName(const AString& s, bool* isValidOut)
{
    initialize();
    
    bool validFlag = false;
    Enum e;
    
    for (std::vector<StructureEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const StructureEnum& d = *iter;
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
StructureEnum::toIntegerCode(Enum e)
{
    initialize();
    const StructureEnum* enumValue = findData(e);
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
StructureEnum::Enum
StructureEnum::fromIntegerCode(const int32_t integerCode, bool* isValidOut)
{
    initialize();
    
    bool validFlag = false;
    Enum e = StructureEnum::INVALID;
    
    for (std::vector<StructureEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const StructureEnum& enumValue = *iter;
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

