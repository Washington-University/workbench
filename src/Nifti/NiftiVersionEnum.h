#ifndef __NIFTIVERSION_H__
#define __NIFTIVERSION_H__

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
#include <AString.h>

namespace caret {

/**
 * The NIFTI version
 */
class NiftiVersionEnum {

public:
    /**  The NIFTI version
 */
    enum Enum {
        /** NIFTI-1  */
        NIFTI_VERSION_1,
        /** NIFTI-2  */
        NIFTI_VERSION_2
    };


    ~NiftiVersionEnum();

    static AString toName(Enum e);
    
    static Enum fromName(const AString& s, bool* isValidOut);
    
    static int32_t toIntegerCode(Enum e);
    
    static Enum fromIntegerCode(const int32_t integerCode, bool* isValidOut);
    
private:
    NiftiVersionEnum(const Enum e, const int32_t integerCode, const AString& name);

    static std::vector<NiftiVersionEnum> enumData;

    static void initialize();

    static bool initializedFlag;

    Enum e;

    int32_t integerCode;

    AString name;

    static const NiftiVersionEnum* findData(const Enum e);

};

#ifdef __NIFTIVERSION_DECLARE__
    std::vector<NiftiVersionEnum> NiftiVersionEnum::enumData;
    bool NiftiVersionEnum::initializedFlag = false;
#endif // __NIFTIVERSION_DECLARE__

} // namespace

#endif // __NIFTIVERSION_H__
