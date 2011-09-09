#ifndef __SURFACE_OVERLAY_DATA_TYPE_ENUM__H_
#define __SURFACE_OVERLAY_DATA_TYPE_ENUM__H_

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

/// Enumerated type for surface overlay 
class SurfaceOverlayDataTypeEnum {

public:
    /**
     * Enumerated values.
     */
    enum Enum {
        /** None */
        NONE,
        /** Connectivity */
        CONNECTIVITY,
        /** Label */
        LABEL,
        /** Metric */
        METRIC,
        /** RGBA */
        RGBA
    };


    ~SurfaceOverlayDataTypeEnum();

    static AString toName(Enum e);
    
    static Enum fromName(const AString& s, bool* isValidOut);
    
    static AString toGuiName(Enum e);
    
    static Enum fromGuiName(const AString& s, bool* isValidOut);
    
    static int32_t toIntegerCode(Enum e);
    
    static Enum fromIntegerCode(const int32_t integerCode, bool* isValidOut);

private:
    SurfaceOverlayDataTypeEnum(const Enum e, 
                 const int32_t integerCode, 
                 const AString& name,
                 const AString& guiName);

    static const SurfaceOverlayDataTypeEnum* findData(const Enum e);

    static std::vector<SurfaceOverlayDataTypeEnum> enumData;

    static void initialize();

    static bool initializedFlag;

    Enum e;

    int32_t integerCode;

    AString name;
    
    AString guiName;
};

#ifdef __SURFACE_OVERLAY_DATA_TYPE_ENUM_DECLARE__
std::vector<SurfaceOverlayDataTypeEnum> SurfaceOverlayDataTypeEnum::enumData;
bool SurfaceOverlayDataTypeEnum::initializedFlag = false;
#endif // __SURFACE_OVERLAY_DATA_TYPE_ENUM_DECLARE__

} // namespace
#endif  //__SURFACE_OVERLAY_DATA_TYPE_ENUM__H_
