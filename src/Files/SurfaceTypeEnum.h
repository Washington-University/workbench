#ifndef __SURFACE_TYPE_ENUM__H_
#define __SURFACE_TYPE_ENUM__H_

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


#include <stdint.h>
#include <vector>
#include "AString.h"

namespace caret {

/**
 * Enumerated type for 
 */
class SurfaceTypeEnum {

public:
    /**
     * Enumerated values.
     */
    enum Enum {
        /** UNKNOWN */
        SURFACE_TYPE_UNKNOWN,
        /** Reconstruction (raw) */
        SURFACE_TYPE_RECONSTRUCTION,
        /** Anatomical */
        SURFACE_TYPE_ANATOMICAL,
        /** Inflated */
        SURFACE_TYPE_INFLATED,
        /** Very Inflated */
        SURFACE_TYPE_VERY_INFLATED,
        /** Spherical */
        SURFACE_TYPE_SPHERICAL,
        /** Semi-Spherical (CMW) */
        SURFACE_TYPE_SEMI_SPHERICAL,
        /** Ellipsoid */
        SURFACE_TYPE_ELLIPSOID,
        /** Flat */
        SURFACE_TYPE_FLAT,
        /** Hull */
        SURFACE_TYPE_HULL
    };


    ~SurfaceTypeEnum();

    static AString toName(Enum e);
    
    static Enum fromName(const AString& s, bool* isValidOut);
    
    static AString toGuiName(Enum e);
    
    static Enum fromGuiName(const AString& s, bool* isValidOut);
    
    static AString toGiftiName(Enum e);
    
    static Enum fromGiftiName(const AString& s, bool* isValidOut);
    
    static int32_t toIntegerCode(Enum e);
    
    static Enum fromIntegerCode(const int32_t integerCode, bool* isValidOut);

    static void getAllEnums(std::vector<Enum>& allEnums);
    
private:
    SurfaceTypeEnum(const Enum e, 
                    const AString& name,
                    const AString& guiName,
                    const AString& giftiName);

    static const SurfaceTypeEnum* findData(const Enum e);

    static std::vector<SurfaceTypeEnum> enumData;

    static void initialize();

    static bool initializedFlag;

    static int32_t integerCodeGenerator;
    
    Enum e;

    int32_t integerCode;

    AString name;
    
    AString guiName;
    
    AString giftiName;
};

#ifdef __SURFACE_TYPE_ENUM_DECLARE__
    std::vector<SurfaceTypeEnum> SurfaceTypeEnum::enumData;
    bool SurfaceTypeEnum::initializedFlag = false;
    int32_t SurfaceTypeEnum::integerCodeGenerator = 0;
#endif // __SURFACE_TYPE_ENUM_DECLARE__

} // namespace
#endif  //__SURFACE_TYPE_ENUM__H_
