#ifndef __VOLUME_OVERLAY_DATA_TYPE_ENUM__H_
#define __VOLUME_OVERLAY_DATA_TYPE_ENUM__H_

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

class VolumeOverlayDataTypeEnum {

public:
    /**
     * Enumerated values.
     */
    enum Enum {
        /** Anatomy */
        ANATOMY,
        /** Functional */
        FUNCTIONAL,
        /** Label */
        LABEL
    };


    ~VolumeOverlayDataTypeEnum();

    static AString toName(Enum enumValue);
    
    static Enum fromName(const AString& name, bool* isValidOut);
    
    static AString toGuiName(Enum enumValue);
    
    static Enum fromGuiName(const AString& guiName, bool* isValidOut);
    
    static int32_t toIntegerCode(Enum enumValue);
    
    static Enum fromIntegerCode(const int32_t integerCode, bool* isValidOut);

    static void getAllEnums(std::vector<Enum>& allEnums);

    static void getAllNames(std::vector<AString>& allNames, const bool isSorted);

    static void getAllGuiNames(std::vector<AString>& allGuiNames, const bool isSorted);

private:
    VolumeOverlayDataTypeEnum(const Enum enumValue, 
                 const AString& name,
                 const AString& guiName);

    static const VolumeOverlayDataTypeEnum* findData(const Enum enumValue);

    /** Holds all instance of enum values and associated metadata */
    static std::vector<VolumeOverlayDataTypeEnum> enumData;

    /** Initialize instances that contain the enum values and metadata */
    static void initialize();

    /** Indicates instance of enum values and metadata have been initialized */
    static bool initializedFlag;
    
    /** Auto generated integer codes */
    static int32_t integerCodeCounter;
    
    /** The enumerated type value for an instance */
    Enum enumValue;

    /** The integer code associated with an enumerated value */
    int32_t integerCode;

    /** The name, a text string that is identical to the enumerated value */
    AString name;
    
    /** A user-friendly name that is displayed in the GUI */
    AString guiName;
};

#ifdef __VOLUME_OVERLAY_DATA_TYPE_ENUM_DECLARE__
std::vector<VolumeOverlayDataTypeEnum> VolumeOverlayDataTypeEnum::enumData;
bool VolumeOverlayDataTypeEnum::initializedFlag = false;
int32_t VolumeOverlayDataTypeEnum::integerCodeCounter = 0; 
#endif // __VOLUME_OVERLAY_DATA_TYPE_ENUM_DECLARE__

} // namespace
#endif  //__VOLUME_OVERLAY_DATA_TYPE_ENUM__H_
