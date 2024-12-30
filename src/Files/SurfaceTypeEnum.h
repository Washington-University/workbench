#ifndef __SURFACE_TYPE_ENUM__H_
#define __SURFACE_TYPE_ENUM__H_

/*LICENSE_START*/
/*
 *  Copyright (C) 2014  Washington University School of Medicine
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */
/*LICENSE_END*/


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
        UNKNOWN,
        /** Reconstruction (raw) */
        RECONSTRUCTION,
        /** Anatomical */
        ANATOMICAL,
        /** Inflated */
        INFLATED,
        /** Very Inflated */
        VERY_INFLATED,
        /** Spherical */
        SPHERICAL,
        /** Semi-Spherical (CMW) */
        SEMI_SPHERICAL,
        /** Ellipsoid */
        ELLIPSOID,
        /** Flat */
        FLAT,
        /** Hull */
        HULL
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
    
    static void getAllEnumsExceptFlat(std::vector<Enum>& enumsOut);
    
    static void getAllAnatomicallyShapedEnums(std::vector<Enum>& threeDimEnums);
    
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

class SecondarySurfaceTypeEnum
{
public:
    enum Enum {
        INVALID,
        GRAY_WHITE,
        MIDTHICKNESS,
        PIAL,
        INNER,
        OUTER
    };

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
    SecondarySurfaceTypeEnum(const Enum e, 
                    const AString& name,
                    const AString& guiName,
                    const AString& giftiName);

    static const SecondarySurfaceTypeEnum* findData(const Enum e);

    static std::vector<SecondarySurfaceTypeEnum> enumData;

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

    std::vector<SecondarySurfaceTypeEnum> SecondarySurfaceTypeEnum::enumData;
    bool SecondarySurfaceTypeEnum::initializedFlag = false;
    int32_t SecondarySurfaceTypeEnum::integerCodeGenerator = 0;
#endif // __SURFACE_TYPE_ENUM_DECLARE__

} // namespace
#endif  //__SURFACE_TYPE_ENUM__H_
