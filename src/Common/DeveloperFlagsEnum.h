#ifndef __DEVELOPER_FLAGS_ENUM_H__
#define __DEVELOPER_FLAGS_ENUM_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2014 Washington University School of Medicine
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

class DeveloperFlagsEnum {

public:
    /**
     * Enumerated values.
     */
    enum Enum {
        DEVELOPER_FLAG_UNUSED,
        DEVELOPER_FLAG_FLIP_PALETTE_NOT_DATA,
        DEVELOPER_FLAG_TEXTURE_VOLUME,
        DELELOPER_FLAG_TEXTURE_ANATOMY_VOLUME_SMOOTH,
        DELELOPER_FLAG_TEXTURE_FUNCTIONAL_VOLUME_SMOOTH,
        DEVELOPER_FLAG_SURFACE_BUFFER,
        DEVELOPER_FLAG_BALSA,
        DEVELOPER_FLAG_VOXEL_CUBES_TEST,
        DEVELOPER_FLAG_CHART_OPENGL_LINES,
        DEVELOPER_FLAG_BLENDING,
        DEVELOPER_FLAG_MPR_INDEPENDENT_ROTATION,
        DEVELOPER_FLAG_MPR_ROTATE_ABOUT_NORMAL_VECTOR
    };

    ~DeveloperFlagsEnum();

    static AString toName(Enum enumValue);
    
    static Enum fromName(const AString& name, bool* isValidOut);
    
    static AString toToolTip(Enum enumValue);
    
    static AString toGuiName(Enum enumValue);
    
    static Enum fromGuiName(const AString& guiName, bool* isValidOut);
    
    static int32_t toIntegerCode(Enum enumValue);
    
    static Enum fromIntegerCode(const int32_t integerCode, bool* isValidOut);

    static void getAllEnums(std::vector<Enum>& allEnums);

    static void getAllNames(std::vector<AString>& allNames, const bool isSorted);

    static void getAllGuiNames(std::vector<AString>& allGuiNames, const bool isSorted);

    static bool isFlag(const Enum enumValue);
    
    static void setFlag(const Enum enumValue,
                        const bool flagStatus);
    
    static bool isCheckable(const Enum enumValue);
    
private:
    enum class CheckableEnum {
        NO,
        YES
    };
    
    DeveloperFlagsEnum(const Enum enumValue,
                       const AString& name,
                       const AString& guiName,
                       const CheckableEnum checkable,
                       const bool defaultValue);

    static DeveloperFlagsEnum* findData(const Enum enumValue);

    /** Holds all instance of enum values and associated metadata */
    static std::vector<DeveloperFlagsEnum> enumData;

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
    
    /** Flag status */
    bool flagStatus;
    
    /** Checkable status */
    CheckableEnum checkable;
};

#ifdef __DEVELOPER_FLAGS_ENUM_DECLARE__
std::vector<DeveloperFlagsEnum> DeveloperFlagsEnum::enumData;
bool DeveloperFlagsEnum::initializedFlag = false;
int32_t DeveloperFlagsEnum::integerCodeCounter = 0; 
#endif // __DEVELOPER_FLAGS_ENUM_DECLARE__

} // namespace
#endif  //__DEVELOPER_FLAGS_ENUM_H__
