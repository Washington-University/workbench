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
#include "PaletteThresholdType.h"
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
PaletteThresholdType::PaletteThresholdType(
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
PaletteThresholdType::~PaletteThresholdType()
{
}

void
PaletteThresholdType::initialize()
{
    if (initializedFlag) {
        return;
    }
    initializedFlag = true;

    enumData.push_back(PaletteThresholdType(THRESHOLD_TYPE_OFF, 0, "THRESHOLD_TYPE_OFF", "Off"));
    enumData.push_back(PaletteThresholdType(THRESHOLD_TYPE_NORMAL, 1, "THRESHOLD_TYPE_NORMAL", "Normal"));
    enumData.push_back(PaletteThresholdType(THRESHOLD_TYPE_MAPPED, 2, "THRESHOLD_TYPE_MAPPED", "Mapped"));
    enumData.push_back(PaletteThresholdType(THRESHOLD_TYPE_MAPPED_AVERAGE_AREA, 3, "THRESHOLD_TYPE_MAPPED_AVERAGE_AREA", "Mapped Average Area"));
}

/**
 * Get the enum value for this enumerated item.
 * @return the value for this enumerated item.
 */
PaletteThresholdType::Enum 
PaletteThresholdType::getEnum() const
{
    return this->e;
}

/**
 * Get the integer code for this enumerated item.
 * @return the integer code for this enumerated item.
 */
int32_t 
PaletteThresholdType::getIntegerCode() const
{
    return this->integerCode;
}

/**
 * Get the enum name for this enumerated item.
 * @return the name for this enumerated item.
 */
std::string
PaletteThresholdType::getName() const
{
    return this->name;
}

/**
 * Find the data for and enumerated value.
 * @param e
 *     The enumerated value.
 * @return Pointer to data for this enumerated type
 * or NULL if no data for type or if type is invalid.
 */
const PaletteThresholdType*
PaletteThresholdType::findData(const Enum e)
{
    initialize();

    int64_t num = enumData.size();
    for (int64_t i = 0; i < num; i++) {
        const PaletteThresholdType* d = &enumData[i];
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
 * @param isValidOut 
 *     If not NULL, it is set indicating that a
 *     label exists for the input enum value.
 * @return 
 *     String representing enumerated value.
 */
std::string 
PaletteThresholdType::toString(Enum e, bool* isValidOut) {
    initialize();
    
    std::string s;
    
    for (std::vector<PaletteThresholdType>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const PaletteThresholdType& d = *iter;
        if (d.e == e) {
            s = d.name;
            break;
        }
    }

    if (isValidOut != NULL) {
        *isValidOut = (s.size() > 0);
    }
    
    return s;
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
PaletteThresholdType::Enum 
PaletteThresholdType::fromString(const std::string& s, bool* isValidOut)
{
    initialize();
    
    bool validFlag = false;
    Enum e;
    
    for (std::vector<PaletteThresholdType>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const PaletteThresholdType& d = *iter;
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
