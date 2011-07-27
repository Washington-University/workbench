#ifndef __NIFTITIMEUNITS_H__
#define __NIFTITIMEUNITS_H__

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



#include <stdint.h>

#include <vector>
#include <string>

namespace caret {

/**
 * NIFTI Time Units
 */
class NiftiTimeUnits {

public:
    /**  NIFTI Time Units
 */
    enum Enum {
        /**  */
        NIFTI_UNITS_UNKNOWN,
        /**  */
        NIFTI_UNITS_SEC,
        /**  */
        NIFTI_UNITS_MSEC,
        /**  */
        NIFTI_UNITS_USEC,
        /**  */
        NIFTI_UNITS_HZ,
        /**  */
        NIFTI_UNITS_PPM
    };


    ~NiftiTimeUnits();

    static std::string toName(Enum e);
    
    static Enum fromName(const std::string& s, bool* isValidOut);
    
    static int32_t toIntegerCode(Enum e);

    static Enum fromIntegerCode(const int32_t integerCode, bool* isValidOut);

private:
    NiftiTimeUnits(const Enum e, const int32_t integerCode, const std::string& name);

    static const NiftiTimeUnits* findData(const Enum e);
    
    static std::vector<NiftiTimeUnits> enumData;

    static void initializeTimeUnits();

    static bool initializedFlag;

    Enum e;

    int32_t integerCode;
    
    std::string name;


};

#ifdef __NIFTITIMEUNITS_DECLARE__
    std::vector<NiftiTimeUnits> NiftiTimeUnits::enumData;
    bool NiftiTimeUnits::initializedFlag = false;
#endif // __NIFTITIMEUNITS_DECLARE__

} // namespace

#endif // __NIFTITIMEUNITS_H__
