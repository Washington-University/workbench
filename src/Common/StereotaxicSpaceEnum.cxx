
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

#include <algorithm>
#define __STEREOTAXIC_SPACE_ENUM_DECLARE__
#include "StereotaxicSpaceEnum.h"
#undef __STEREOTAXIC_SPACE_ENUM_DECLARE__

#include "CaretAssert.h"
#include "CaretLogger.h"

using namespace caret;

/**
 * Constructor.
 *
 * @param enumValue
 *    An enumerated value.
 * @param name
 *    Name of enumerated value.
 *
 * @param guiName
 *    User-friendly name for use in user-interface.
 */
StereotaxicSpaceEnum::StereotaxicSpaceEnum(const Enum enumValue, 
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
                                           const float originZ)
{
    this->enumValue = enumValue;
    this->name = name;
    this->guiName = guiName;
    
    this->volumeDimensions[0] = dimI;
    this->volumeDimensions[1] = dimJ;
    this->volumeDimensions[2] = dimK;
    
    this->volumeVoxelSizes[0] = voxelSizeX;
    this->volumeVoxelSizes[1] = voxelSizeY;
    this->volumeVoxelSizes[2] = voxelSizeZ;
    
    this->volumeOrigin[0] = originX;
    this->volumeOrigin[1] = originY;
    this->volumeOrigin[2] = originZ;
}

/**
 * Destructor.
 */
StereotaxicSpaceEnum::~StereotaxicSpaceEnum()
{
}

/**
 * Get the dimensions for a stereotaxic space.
 *
 * @param enumValue
 *   Input - Enumerated type for stereotaxic space.
 * @param dimensionsOut
 *   Output - Dimensions for the stereotaxic space.
 */
void 
StereotaxicSpaceEnum::toDimensions(const Enum enumValue, 
                                   int32_t dimensionsOut[3])
{
    const StereotaxicSpaceEnum* enumInstance = findData(enumValue);
    dimensionsOut[0] = enumInstance->volumeDimensions[0];
    dimensionsOut[1] = enumInstance->volumeDimensions[1];
    dimensionsOut[2] = enumInstance->volumeDimensions[2];
}

/**
 * Get the voxel sizes for a stereotaxic space.
 *
 * @param enumValue
 *   Input - Enumerated type for stereotaxic space.
 * @param voxelSizesOut
 *   Output - Voxel sizes for the stereotaxic space.
 */
void 
StereotaxicSpaceEnum::toVoxelSizes(const Enum enumValue, 
                                   float voxelSizesOut[3])
{
    const StereotaxicSpaceEnum* enumInstance = findData(enumValue);
    voxelSizesOut[0] = enumInstance->volumeVoxelSizes[0];
    voxelSizesOut[1] = enumInstance->volumeVoxelSizes[1];
    voxelSizesOut[2] = enumInstance->volumeVoxelSizes[2];
}

/**
 * Get the origin for a stereotaxic space.
 *
 * @param enumValue
 *   Input - Enumerated type for stereotaxic space.
 * @param originOut
 *   Output - Origin for the stereotaxic space.
 */
void 
StereotaxicSpaceEnum::toOrigin(const Enum enumValue, 
                               float originOut[3])
{
    const StereotaxicSpaceEnum* enumInstance = findData(enumValue);
    originOut[0] = enumInstance->volumeOrigin[0];
    originOut[1] = enumInstance->volumeOrigin[1];
    originOut[2] = enumInstance->volumeOrigin[2];
}

/**
 * Initialize the enumerated metadata.
 */
void
StereotaxicSpaceEnum::initialize()
{
    if (initializedFlag) {
        return;
    }
    initializedFlag = true;

    enumData.push_back(StereotaxicSpaceEnum(SPACE_UNKNOWN, 
                                            "SPACE_UNKNOWN", 
                                            "Unknown",
                                            0,
                                            0,
                                            0,
                                            0.0,
                                            0.0,
                                            0.0,
                                            0.0,
                                            0.0,
                                            0.0));
    
    enumData.push_back(StereotaxicSpaceEnum(SPACE_OTHER, 
                                            "SPACE_OTHER", 
                                            "Other not specified",
                                            0,
                                            0,
                                            0,
                                            0.0,
                                            0.0,
                                            0.0,
                                            0.0,
                                            0.0,
                                            0.0));
    
    enumData.push_back(StereotaxicSpaceEnum(SPACE_AFNI_TALAIRACH, 
                                            "SPACE_AFNI_TALAIRACH", 
                                            "AFNI",
                                            161, 191, 151,
                                            1.0, 1.0, 1.0,
                                            -80.0, -110.0, -65.0));
    
    enumData.push_back(StereotaxicSpaceEnum(SPACE_FLIRT, 
                                            "SPACE_FLIRT", 
                                            "FLIRT",
                                            182, 217, 182,
                                            1.0, 1.0, 1.0,
                                            -90.0, -126.0, -72.0));
    
    enumData.push_back(StereotaxicSpaceEnum(SPACE_FLIRT_222, 
                                            "SPACE_FLIRT_222", 
                                            "FLIRT-222",
                                            91, 109, 91,
                                            2.0, 2.0, 2.0,
                                            -90.0, -126.0, -72.0));
    
    enumData.push_back(StereotaxicSpaceEnum(SPACE_MACAQUE_F6, 
                                            "SPACE_MACAQUE_F6", 
                                            "MACAQUE-F6",
                                            143, 187, 118,
                                            0.5, 0.5, 0.5,
                                            -35.75, -54.75, -30.25));
    
    enumData.push_back(StereotaxicSpaceEnum(SPACE_MACAQUE_F99, 
                                            "SPACE_MACAQUE_F99", 
                                            "MACAQUE-F99",
                                            143, 187, 118,
                                            0.5, 0.5, 0.5,
                                            -35.75, -54.75, -30.25));
    
    enumData.push_back(StereotaxicSpaceEnum(SPACE_MRITOTAL, 
                                            "SPACE_MRITOTAL", 
                                            "MRITOTAL",
                                            182, 217, 182,
                                            1.0, 1.0, 1.0,
                                            -90.0, -126.0, -72.0));
    
    enumData.push_back(StereotaxicSpaceEnum(SPACE_SPM, 
                                            "SPACE_SPM", 
                                            "SPM",
                                            182, 217, 182,
                                            1.0, 1.0, 1.0,
                                            -90.0, -126.0, -72.0));
    
    enumData.push_back(StereotaxicSpaceEnum(SPACE_SPM_95, 
                                            "SPACE_SPM_95", 
                                            "SPM95",
                                            182, 217, 182,
                                            1.0, 1.0, 1.0,
                                            -90.0, -126.0, -72.0));
    
    enumData.push_back(StereotaxicSpaceEnum(SPACE_SPM_96, 
                                            "SPACE_SPM_96", 
                                            "SPM96",
                                            182, 217, 182,
                                            1.0, 1.0, 1.0,
                                            -90.0, -126.0, -72.0));
    
    enumData.push_back(StereotaxicSpaceEnum(SPACE_SPM_99, 
                                            "SPACE_SPM_99", 
                                            "SPM99",
                                            182, 217, 182,
                                            1.0, 1.0, 1.0,
                                            -90.0, -126.0, -72.0));
    
    enumData.push_back(StereotaxicSpaceEnum(SPACE_SPM_2, 
                                            "SPACE_SPM_2", 
                                            "SPM2",
                                            182, 217, 182,
                                            1.0, 1.0, 1.0,
                                            -90.0, -126.0, -72.0));
    
    enumData.push_back(StereotaxicSpaceEnum(SPACE_SPM_5, 
                                            "SPACE_SPM_5", 
                                            "SPM5",
                                            182, 217, 182,
                                            1.0, 1.0, 1.0,
                                            -90.0, -126.0, -72.0));
    
    enumData.push_back(StereotaxicSpaceEnum(SPACE_T88, 
                                            "SPACE_T88", 
                                            "T88",
                                            161, 191, 151,
                                            1.0, 1.0, 1.0,
                                            -80.0, -110.0, -65.0));
    
    enumData.push_back(StereotaxicSpaceEnum(SPACE_WU_7112B, 
                                            "SPACE_WU_7112B", 
                                            "711-2B",
                                            176, 208, 176,
                                            1.0, 1.0, 1.0,
                                            -88.5, -123.5, -75.5));
    
    enumData.push_back(StereotaxicSpaceEnum(SPACE_WU_7112B_111, 
                                            "SPACE_WU_7112B_111", 
                                            "711-2B-111",
                                            176, 208, 176,
                                            1.0, 1.0, 1.0,
                                            -88.5, -123.5, -75.5));
    
    enumData.push_back(StereotaxicSpaceEnum(SPACE_WU_7112B_222, 
                                            "SPACE_WU_7112B_222", 
                                            "711-2B-222",
                                            128, 128, 75,
                                            2.0, 2.0, 2.0,
                                            -128, -128, -69));
    
    enumData.push_back(StereotaxicSpaceEnum(SPACE_WU_7112B_333, 
                                            "SPACE_WU_7112B_333", 
                                            "711-2B-333",
                                            48, 64, 48,
                                            3.0, 3.0, 3.0,
                                            -72.0, -106.5, -61.5));
    
    enumData.push_back(StereotaxicSpaceEnum(SPACE_WU_7112C, 
                                            "SPACE_WU_7112C", 
                                            "711-2C",
                                            176, 208, 176,
                                            1.0, 1.0, 1.0,
                                            -88.5, -123.5, -75.5));
    
    enumData.push_back(StereotaxicSpaceEnum(SPACE_WU_7112C_111, 
                                            "SPACE_WU_7112C_111", 
                                            "711-2C-111",
                                            176, 208, 176,
                                            1.0, 1.0, 1.0,
                                            -88.5, -123.5, -75.5));
    
    enumData.push_back(StereotaxicSpaceEnum(SPACE_WU_7112C_222, 
                                            "SPACE_WU_7112C_222", 
                                            "711-2C-222",
                                            128, 128, 75,
                                            2.0, 2.0, 2.0,
                                            -128, -128, -69));
    
    enumData.push_back(StereotaxicSpaceEnum(SPACE_WU_7112C_333, 
                                            "SPACE_WU_7112C_333", 
                                            "711-2C-333",
                                            48, 64, 48,
                                            3.0, 3.0, 3.0,
                                            -72.0, -106.5, -61.5));
    
    enumData.push_back(StereotaxicSpaceEnum(SPACE_WU_7112O, 
                                            "SPACE_WU_7112O", 
                                            "711-2O",
                                            176, 208, 176,
                                            1.0, 1.0, 1.0,
                                            -88.5, -123.5, -75.5));
    
    enumData.push_back(StereotaxicSpaceEnum(SPACE_WU_7112O_111, 
                                            "SPACE_WU_7112O_111", 
                                            "711-2O-111",
                                            176, 208, 176,
                                            1.0, 1.0, 1.0,
                                            -88.5, -123.5, -75.5));
    
    enumData.push_back(StereotaxicSpaceEnum(SPACE_WU_7112O_222, 
                                            "SPACE_WU_7112O_222", 
                                            "711-2O-222",
                                            128, 128, 75,
                                            2.0, 2.0, 2.0,
                                            -128, -128, -69));
    
    enumData.push_back(StereotaxicSpaceEnum(SPACE_WU_7112O_333, 
                                            "SPACE_WU_7112O_333", 
                                            "711-2O-333",
                                            48, 64, 48,
                                            3.0, 3.0, 3.0,
                                            -72.0, -106.5, -61.5));
    
    enumData.push_back(StereotaxicSpaceEnum(SPACE_WU_7112Y, 
                                            "SPACE_WU_7112Y", 
                                            "711-2Y",
                                            176, 208, 176,
                                            1.0, 1.0, 1.0,
                                            -88.5, -123.5, -75.5));
    
    enumData.push_back(StereotaxicSpaceEnum(SPACE_WU_7112Y_111, 
                                            "SPACE_WU_7112Y_111", 
                                            "711-2Y-111",
                                            176, 208, 176,
                                            1.0, 1.0, 1.0,
                                            -88.5, -123.5, -75.5));
    
    enumData.push_back(StereotaxicSpaceEnum(SPACE_WU_7112Y_222, 
                                            "SPACE_WU_7112Y_222", 
                                            "711-2Y-222",
                                            128, 128, 75,
                                            2.0, 2.0, 2.0,
                                            -128, -128, -69));
    
    enumData.push_back(StereotaxicSpaceEnum(SPACE_WU_7112Y_333, 
                                            "SPACE_WU_7112Y_333", 
                                            "711-2Y-333",
                                            48, 64, 48,
                                            3.0, 3.0, 3.0,
                                            -72.0, -106.5, -61.5));
}

/**
 * Find the data for and enumerated value.
 * @param enumValue
 *     The enumerated value.
 * @return Pointer to data for this enumerated type
 * or NULL if no data for type or if type is invalid.
 */
const StereotaxicSpaceEnum*
StereotaxicSpaceEnum::findData(const Enum enumValue)
{
    if (initializedFlag == false) initialize();

    size_t num = enumData.size();
    for (size_t i = 0; i < num; i++) {
        const StereotaxicSpaceEnum* d = &enumData[i];
        if (d->enumValue == enumValue) {
            return d;
        }
    }

    return NULL;
}

/**
 * Get a string representation of the enumerated type.
 * @param enumValue 
 *     Enumerated value.
 * @return 
 *     String representing enumerated value.
 */
AString 
StereotaxicSpaceEnum::toName(Enum enumValue) {
    if (initializedFlag == false) initialize();
    
    const StereotaxicSpaceEnum* enumInstance = findData(enumValue);
    return enumInstance->name;
}

/**
 * Get an enumerated value corresponding to its name.
 * @param name 
 *     Name of enumerated value.
 * @param isValidOut 
 *     If not NULL, it is set indicating that a
 *     enum value exists for the input name.
 * @return 
 *     Enumerated value.
 */
StereotaxicSpaceEnum::Enum 
StereotaxicSpaceEnum::fromName(const AString& name, bool* isValidOut)
{
    if (initializedFlag == false) initialize();
    
    bool validFlag = false;
    Enum enumValue = SPACE_UNKNOWN;
    
    for (std::vector<StereotaxicSpaceEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const StereotaxicSpaceEnum& d = *iter;
        if (d.name == name) {
            enumValue = d.enumValue;
            validFlag = true;
            break;
        }
    }
    
    if (isValidOut != 0) {
        *isValidOut = validFlag;
    }
    else if (validFlag == false) {
        CaretAssertMessage(0, AString("Name " + name + "failed to match enumerated value for type StereotaxicSpaceEnum"));
    }
    return enumValue;
}

/**
 * Get a GUI string representation of the enumerated type.
 * @param enumValue 
 *     Enumerated value.
 * @return 
 *     String representing enumerated value.
 */
AString 
StereotaxicSpaceEnum::toGuiName(Enum enumValue) {
    if (initializedFlag == false) initialize();
    
    const StereotaxicSpaceEnum* enumInstance = findData(enumValue);
    return enumInstance->guiName;
}

/**
 * Get an enumerated value corresponding to its GUI name.
 * @param s 
 *     Name of enumerated value.
 * @param isValidOut 
 *     If not NULL, it is set indicating that a
 *     enum value exists for the input name.
 * @return 
 *     Enumerated value.
 */
StereotaxicSpaceEnum::Enum 
StereotaxicSpaceEnum::fromGuiName(const AString& guiName, bool* isValidOut)
{
    if (initializedFlag == false) initialize();
    
    bool validFlag = false;
    Enum enumValue = SPACE_UNKNOWN;
    
    for (std::vector<StereotaxicSpaceEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const StereotaxicSpaceEnum& d = *iter;
        if (d.guiName == guiName) {
            enumValue = d.enumValue;
            validFlag = true;
            break;
        }
    }
    
    if (isValidOut != 0) {
        *isValidOut = validFlag;
    }
    else if (validFlag == false) {
        CaretAssertMessage(0, AString("guiName " + guiName + "failed to match enumerated value for type StereotaxicSpaceEnum"));
    }
    return enumValue;
}

/**
 * Get the integer code for a data type.
 *
 * @return
 *    Integer code for data type.
 */
int32_t
StereotaxicSpaceEnum::toIntegerCode(Enum enumValue)
{
    if (initializedFlag == false) initialize();
    const StereotaxicSpaceEnum* enumInstance = findData(enumValue);
    return enumInstance->integerCode;
}

/**
 * Find the data type corresponding to an integer code.
 *
 * @param integerCode
 *     Integer code for enum.
 * @param isValidOut
 *     If not NULL, on exit isValidOut will indicate if
 *     integer code is valid.
 * @return
 *     Enum for integer code.
 */
StereotaxicSpaceEnum::Enum
StereotaxicSpaceEnum::fromIntegerCode(const int32_t integerCode, bool* isValidOut)
{
    if (initializedFlag == false) initialize();
    
    bool validFlag = false;
    Enum enumValue = SPACE_UNKNOWN;
    
    for (std::vector<StereotaxicSpaceEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const StereotaxicSpaceEnum& enumInstance = *iter;
        if (enumInstance.integerCode == integerCode) {
            enumValue = enumInstance.enumValue;
            validFlag = true;
            break;
        }
    }
    
    if (isValidOut != 0) {
        *isValidOut = validFlag;
    }
    else if (validFlag == false) {
        CaretAssertMessage(0, AString("Integer code " + AString::number(integerCode) + "failed to match enumerated value for type StereotaxicSpaceEnum"));
    }
    return enumValue;
}

/**
 * Get all of the enumerated type values.  The values can be used
 * as parameters to toXXX() methods to get associated metadata.
 *
 * @param allEnums
 *     A vector that is OUTPUT containing all of the enumerated values.
 */
void
StereotaxicSpaceEnum::getAllEnums(std::vector<StereotaxicSpaceEnum::Enum>& allEnums)
{
    if (initializedFlag == false) initialize();
    
    allEnums.clear();
    
    for (std::vector<StereotaxicSpaceEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        allEnums.push_back(iter->enumValue);
    }
}

/**
 * Get all of the names of the enumerated type values.
 *
 * @param allNames
 *     A vector that is OUTPUT containing all of the names of the enumerated values.
 * @param isSorted
 *     If true, the names are sorted in alphabetical order.
 */
void
StereotaxicSpaceEnum::getAllNames(std::vector<AString>& allNames, const bool isSorted)
{
    if (initializedFlag == false) initialize();
    
    allNames.clear();
    
    for (std::vector<StereotaxicSpaceEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        allNames.push_back(StereotaxicSpaceEnum::toName(iter->enumValue));
    }
    
    if (isSorted) {
        std::sort(allNames.begin(), allNames.end());
    }
}

/**
 * Get all of the GUI names of the enumerated type values.
 *
 * @param allNames
 *     A vector that is OUTPUT containing all of the GUI names of the enumerated values.
 * @param isSorted
 *     If true, the names are sorted in alphabetical order.
 */
void
StereotaxicSpaceEnum::getAllGuiNames(std::vector<AString>& allGuiNames, const bool isSorted)
{
    if (initializedFlag == false) initialize();
    
    allGuiNames.clear();
    
    for (std::vector<StereotaxicSpaceEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        allGuiNames.push_back(StereotaxicSpaceEnum::toGuiName(iter->enumValue));
    }
    
    if (isSorted) {
        std::sort(allGuiNames.begin(), allGuiNames.end());
    }
}

