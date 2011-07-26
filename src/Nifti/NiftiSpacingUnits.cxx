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
NiftiSpacingUnits::initialize()
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
 * @param isValidOut 
 *     If not NULL, it is set indicating that a
 *     label exists for the input enum value.
 * @return 
 *     String representing enumerated value.
 */
std::string 
NiftiSpacingUnits::toString(Enum e, bool* isValidOut) {
    initialize();
    
    std::string s;
    
    for (std::vector<NiftiSpacingUnits>::iterator iter = spacingUnits.begin();
         iter != spacingUnits.end();
         iter++) {
        const NiftiSpacingUnits& ndt = *iter;
        if (ndt.e == e) {
            s = ndt.name;
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
NiftiSpacingUnits::Enum 
NiftiSpacingUnits::fromString(const std::string& s, bool* isValidOut)
{
    initialize();
    
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
