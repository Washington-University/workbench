/*LICENSE_START*/ 
/* 
 *  Copyright 1995-2002 Washington University School of Medicine 
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


#define __PALETTE_THRESHOLDTYPE_DECLARE__
#include "PaletteThresholdTypeEnum.h"
#undef __PALETTE_THRESHOLDTYPE_DECLARE__


using namespace caret;

/**
 * Constructor.
 *
 * @param e
 *    An enumerated value.
 * @param name
 *    Name of enumberated value.
 */
PaletteThresholdTypeEnum::PaletteThresholdTypeEnum(
                   const Enum e,
                   const int32_t integerCode,
                   const std::string& name,
                   const std::string& guiName)
{
    this->e = e;
    this->integerCode = integerCode;
    this->name = name;
    this->guiName = guiName;
}

/**
 * Destructor.
 */
PaletteThresholdTypeEnum::~PaletteThresholdTypeEnum()
{
}

void
PaletteThresholdTypeEnum::initialize()
{
    if (initializedFlag) {
        return;
    }
    initializedFlag = true;

    enumData.push_back(PaletteThresholdTypeEnum(THRESHOLD_TYPE_OFF, 0, "THRESHOLD_TYPE_OFF", "Off"));
    enumData.push_back(PaletteThresholdTypeEnum(THRESHOLD_TYPE_NORMAL, 1, "THRESHOLD_TYPE_NORMAL", "Normal"));
    enumData.push_back(PaletteThresholdTypeEnum(THRESHOLD_TYPE_MAPPED, 2, "THRESHOLD_TYPE_MAPPED", "Mapped"));
    enumData.push_back(PaletteThresholdTypeEnum(THRESHOLD_TYPE_MAPPED_AVERAGE_AREA, 3, "THRESHOLD_TYPE_MAPPED_AVERAGE_AREA", "Mapped Average Area"));
}

/**
 * Find the data for and enumerated value.
 * @param e
 *     The enumerated value.
 * @return Pointer to data for this enumerated type
 * or NULL if no data for type or if type is invalid.
 */
const PaletteThresholdTypeEnum*
PaletteThresholdTypeEnum::findData(const Enum e)
{
    initialize();

    int64_t num = enumData.size();
    for (int64_t i = 0; i < num; i++) {
        const PaletteThresholdTypeEnum* d = &enumData[i];
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
std::string 
PaletteThresholdTypeEnum::toName(Enum e) {
    initialize();
    
    const PaletteThresholdTypeEnum* ptt = findData(e);
    return ptt->name;
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
PaletteThresholdTypeEnum::Enum 
PaletteThresholdTypeEnum::fromName(const std::string& s, bool* isValidOut)
{
    initialize();
    
    bool validFlag = false;
    Enum e;
    
    for (std::vector<PaletteThresholdTypeEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const PaletteThresholdTypeEnum& d = *iter;
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
 * Get a gui name representation of the enumerated type.
 * @param e 
 *     Enumerated value.
 * @return 
 *     String representing enumerated value.
 */
std::string 
PaletteThresholdTypeEnum::toGuiName(Enum e) {
    initialize();
    
    const PaletteThresholdTypeEnum* psm = findData(e);
    return psm->guiName;
}

/**
 * Get an enumerated value corresponding to its gui name.
 * @param s 
 *     Name of enumerated value.
 * @param isValidOut 
 *     If not NULL, it is set indicating that a
 *     enum value exists for the input name.
 * @return 
 *     Enumerated value.
 */
PaletteThresholdTypeEnum::Enum 
PaletteThresholdTypeEnum::fromGuiName(const std::string& s, bool* isValidOut)
{
    initialize();
    
    bool validFlag = false;
    Enum e;
    
    for (std::vector<PaletteThresholdTypeEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const PaletteThresholdTypeEnum& d = *iter;
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
 * Get the integer code associated with a threshold type.
 * @param e
 *   The enum.
 * @return 
 *   Integer code associated with a threshold type.
 */
int32_t 
PaletteThresholdTypeEnum::toIntegerCode(Enum e)
{
    initialize();
    const PaletteThresholdTypeEnum* nsu = findData(e);
    return nsu->integerCode;
}

/**
 * Find enum corresponding to integer code.
 * @param integerCode
 *    The integer code.
 * @param isValidOut
 *    If not NULL, on exit it indicates valid integer code.
 * @return
 *    Enum corresponding to integer code.
 */
PaletteThresholdTypeEnum::Enum 
PaletteThresholdTypeEnum::fromIntegerCode(const int32_t integerCode, bool* isValidOut)
{
    initialize();
    
    bool validFlag = false;
    Enum e;
    
    for (std::vector<PaletteThresholdTypeEnum>::const_iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const PaletteThresholdTypeEnum& nsu = *iter;
        if (nsu.integerCode == integerCode) {
            e = nsu.e;
            validFlag = true;
            break;
        }
    }
    
    if (isValidOut != 0) {
        *isValidOut = validFlag;
    }
    return e;
}
