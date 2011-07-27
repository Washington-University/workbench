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

#define __NIFTI_SPACING_UNITS_DECLARE__
#include "NiftiSpacingUnits.h"
#undef __NIFTI_SPACING_UNITS_DECLARE__

#include <cassert>

using namespace caret;

NiftiSpacingUnits::NiftiSpacingUnits(Enum e, 
                                     const int32_t integerCode,
                                     const std::string& name)
{
    this->e = e;
    this->integerCode = integerCode;
    this->name = name;
}

NiftiSpacingUnits::~NiftiSpacingUnits()
{
    
}

void 
NiftiSpacingUnits::initializeSpacingUnits()
{
    if (initializedFlag) {
        return;
    }
    initializedFlag = true;
    
    spacingUnits.push_back(NiftiSpacingUnits(NIFTI_UNITS_UNKNOWN,
                                             0,
                                     "NIFTI_UNITS_UNKNOWN"));
    spacingUnits.push_back(NiftiSpacingUnits(NIFTI_UNITS_METER,
                                             1,
                                            "NIFTI_UNITS_METER"));
    spacingUnits.push_back(NiftiSpacingUnits(NIFTI_UNITS_MM,
                                             2,
                                            "NIFTI_UNITS_MM"));
    spacingUnits.push_back(NiftiSpacingUnits(NIFTI_UNITS_MICRON,
                                             3,
                                            "NIFTI_UNITS_MICRON"));
}

/**
 * Get a string representition of the enumerated type.
 * @param e 
 *     Enumerated value.
 * @return 
 *     String representing enumerated value.
 */
std::string 
NiftiSpacingUnits::toName(Enum e) {
    initializeSpacingUnits();
    
    const NiftiSpacingUnits* nsu = findData(e);
    return nsu->name;
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
NiftiSpacingUnits::Enum 
NiftiSpacingUnits::fromName(const std::string& s, bool* isValidOut)
{
    initializeSpacingUnits();
    
    bool validFlag = false;
    Enum e;
    
    for (std::vector<NiftiSpacingUnits>::iterator iter = spacingUnits.begin();
         iter != spacingUnits.end();
         iter++) {
        const NiftiSpacingUnits& ndt = *iter;
        if (ndt.name == s) {
            e = ndt.e;
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
 * Find the Intent object corresponding to the enum.
 * @param e
 *    The enum
 * @return 
 *    The Intent or NULL if enum does not match an intent.
 */
const 
NiftiSpacingUnits* 
NiftiSpacingUnits::findData(Enum e)
{
    initializeSpacingUnits();
    
    for (std::vector<NiftiSpacingUnits>::const_iterator iter = spacingUnits.begin();
         iter != spacingUnits.end();
         iter++) {
        const NiftiSpacingUnits& nsu = *iter;
        return &nsu;
    }
    
    assert(0);
    
    return NULL;
}

/**
 * Get the integer code associated with an spacing units.
 * @param e
 *   The enum.
 * @return 
 *   Integer code associated with spacing units.
 */
int32_t 
NiftiSpacingUnits::toIntegerCode(Enum e)
{
    initializeSpacingUnits();
    const NiftiSpacingUnits* nsu = findData(e);
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
NiftiSpacingUnits::Enum 
NiftiSpacingUnits::fromIntegerCode(const int32_t integerCode, bool* isValidOut)
{
    initializeSpacingUnits();
    
    bool validFlag = false;
    Enum e;
    
    for (std::vector<NiftiSpacingUnits>::const_iterator iter = spacingUnits.begin();
         iter != spacingUnits.end();
         iter++) {
        const NiftiSpacingUnits& nsu = *iter;
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


