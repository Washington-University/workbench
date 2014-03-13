#ifndef __STEREOTAXIC_SPACE_ENUM__H_
#define __STEREOTAXIC_SPACE_ENUM__H_

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
 * \brief <REPLACE-WITH-ONE-LINE-DESCRIPTION>
 *
 * <REPLACE-WITH-THOROUGH DESCRIPTION>
 */
class StereotaxicSpaceEnum {

public:
    /**
     * Enumerated values.
     */
    enum Enum {
        /// unknown space
        SPACE_UNKNOWN,
        /// other space
        SPACE_OTHER,
        /// AFNI Talairach space
        SPACE_AFNI_TALAIRACH,
        /// FLIRT space
        SPACE_FLIRT,
        /// FLIRT 222 space
        SPACE_FLIRT_222,
        /// macaque atlas
        SPACE_MACAQUE_F6,
        /// Macaque F99
        SPACE_MACAQUE_F99,
        /// MRITOTAL space
        SPACE_MRITOTAL,
        /// SPM space
        SPACE_SPM,
        /// SPM 95 space
        SPACE_SPM_95,
        /// SPM 95 space
        SPACE_SPM_96,
        /// SPM 99 Template space
        SPACE_SPM_99,
        /// SPM 2 Template space
        SPACE_SPM_2,
        /// SPM 5 space
        SPACE_SPM_5,
        /// Talairach 88 space (same as AFNI)
        SPACE_T88,
        /// Washington University 711-2B space
        SPACE_WU_7112B,
        /// Washington University 711-2B 1mm voxelspace
        SPACE_WU_7112B_111,
        /// Washington University 711-2B 2mm voxelspace
        SPACE_WU_7112B_222,
        /// Washington University 711-2B 3mm voxelspace
        SPACE_WU_7112B_333,
        /// Washington University 711-2C space
        SPACE_WU_7112C,
        /// Washington University 711-2C 1mm voxelspace
        SPACE_WU_7112C_111,
        /// Washington University 711-2C 2mm voxelspace
        SPACE_WU_7112C_222,
        /// Washington University 711-2C 3mm voxelspace
        SPACE_WU_7112C_333,
        /// Washington University 711-2O space
        SPACE_WU_7112O,
        /// Washington University 711-2O 1mm voxelspace
        SPACE_WU_7112O_111,
        /// Washington University 711-2O 2mm voxelspace
        SPACE_WU_7112O_222,
        /// Washington University 711-2O 3mm voxelspace
        SPACE_WU_7112O_333,
        /// Washington University 711-2Y space
        SPACE_WU_7112Y,
        /// Washington University 711-2Y 1mm voxelspace
        SPACE_WU_7112Y_111,
        /// Washington University 711-2Y 2mm voxelspace
        SPACE_WU_7112Y_222,
        /// Washington University 711-2Y 3mm voxelspace
        SPACE_WU_7112Y_333
    };


    ~StereotaxicSpaceEnum();

    static AString toName(Enum enumValue);
    
    static Enum fromName(const AString& name, bool* isValidOut);
    
    static AString toGuiName(Enum enumValue);
    
    static Enum fromGuiName(const AString& guiName, bool* isValidOut);
    
    static int32_t toIntegerCode(Enum enumValue);
    
    static Enum fromIntegerCode(const int32_t integerCode, bool* isValidOut);

    static void getAllEnums(std::vector<Enum>& allEnums);

    static void getAllNames(std::vector<AString>& allNames, const bool isSorted);

    static void getAllGuiNames(std::vector<AString>& allGuiNames, const bool isSorted);

    static void toDimensions(const Enum enumValue, int32_t dimensionsOut[3]);
    
    static void toVoxelSizes(const Enum enumValue, float voxelSizesOut[3]);
    
    static void toOrigin(const Enum enumValue, float originOut[3]);
    
private:
    StereotaxicSpaceEnum(const Enum enumValue, 
                         const AString& name,
                         const AString& guiName,
                         const int32_t dimI,
                         const int32_t dimJ,
                         const int32_t dimK,
                         const float voxelSizeX,
                         const float voxelSizeY,
                         const float voxelSizeZ,
                         const float originX,
                         const float originY,
                         const float originZ);

    static const StereotaxicSpaceEnum* findData(const Enum enumValue);

    /** Holds all instance of enum values and associated metadata */
    static std::vector<StereotaxicSpaceEnum> enumData;

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
    
    /** Volume Dimensions */
    int32_t volumeDimensions[3];
    
    /** Volume Origin */
    float volumeOrigin[3];
    
    /** Volume Voxel Sizes */
    float volumeVoxelSizes[3];
};

#ifdef __STEREOTAXIC_SPACE_ENUM_DECLARE__
std::vector<StereotaxicSpaceEnum> StereotaxicSpaceEnum::enumData;
bool StereotaxicSpaceEnum::initializedFlag = false;
int32_t StereotaxicSpaceEnum::integerCodeCounter = 0; 
#endif // __STEREOTAXIC_SPACE_ENUM_DECLARE__

} // namespace
#endif  //__STEREOTAXIC_SPACE_ENUM__H_
