
/*LICENSE_START*/
/*
 * Copyright 2013 Washington University,
 * All rights reserved.
 *
 * Connectome DB and Connectome Workbench are part of the integrated Connectome 
 * Informatics Platform.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    * Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *    * Neither the names of Washington University nor the
 *      names of its contributors may be used to endorse or promote products
 *      derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR  
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
 * SUBSTITUTE GOODS OR  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
 * POSSIBILITY OF SUCH DAMAGE.
 */
/*LICENSE_END*/

#include <algorithm>
#define __WHOLE_BRAIN_VOXEL_DRAWING_MODE_ENUM_DECLARE__
#include "WholeBrainVoxelDrawingMode.h"
#undef __WHOLE_BRAIN_VOXEL_DRAWING_MODE_ENUM_DECLARE__

#include "CaretAssert.h"

using namespace caret;

    
/**
 * \class caret::WholeBrainVoxelDrawingMode 
 * \brief Enumerated type for drawing of voxels in whole brain view.
 */

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
WholeBrainVoxelDrawingMode::WholeBrainVoxelDrawingMode(const Enum enumValue,
                           const AString& name,
                           const AString& guiName)
{
    this->enumValue = enumValue;
    this->integerCode = integerCodeCounter++;
    this->name = name;
    this->guiName = guiName;
}

/**
 * Destructor.
 */
WholeBrainVoxelDrawingMode::~WholeBrainVoxelDrawingMode()
{
}

/**
 * Initialize the enumerated metadata.
 */
void
WholeBrainVoxelDrawingMode::initialize()
{
    if (initializedFlag) {
        return;
    }
    initializedFlag = true;

    enumData.push_back(WholeBrainVoxelDrawingMode(DRAW_VOXELS_AS_THREE_D_CUBES, 
                                    "DRAW_VOXELS_AS_THREE_D_CUBES", 
                                    "Draw Voxels as Cubes (3D)"));
    
    enumData.push_back(WholeBrainVoxelDrawingMode(DRAW_VOXELS_AS_ROUNDED_THREE_D_CUBES,
                                                  "DRAW_VOXELS_AS_ROUNDED_THREE_D_CUBES",
                                                  "Draw Voxels as Rounded Cubes (3D)"));
    
    enumData.push_back(WholeBrainVoxelDrawingMode(DRAW_VOXELS_ON_TWO_D_SLICES,
                                    "DRAW_VOXELS_ON_TWO_D_SLICES", 
                                    "Draw Voxels on Slices (2D)"));
    
}

/**
 * Find the data for and enumerated value.
 * @param enumValue
 *     The enumerated value.
 * @return Pointer to data for this enumerated type
 * or NULL if no data for type or if type is invalid.
 */
const WholeBrainVoxelDrawingMode*
WholeBrainVoxelDrawingMode::findData(const Enum enumValue)
{
    if (initializedFlag == false) initialize();

    size_t num = enumData.size();
    for (size_t i = 0; i < num; i++) {
        const WholeBrainVoxelDrawingMode* d = &enumData[i];
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
WholeBrainVoxelDrawingMode::toName(Enum enumValue) {
    if (initializedFlag == false) initialize();
    
    const WholeBrainVoxelDrawingMode* enumInstance = findData(enumValue);
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
WholeBrainVoxelDrawingMode::Enum 
WholeBrainVoxelDrawingMode::fromName(const AString& name, bool* isValidOut)
{
    if (initializedFlag == false) initialize();
    
    bool validFlag = false;
    Enum enumValue = DRAW_VOXELS_ON_TWO_D_SLICES;
    
    for (std::vector<WholeBrainVoxelDrawingMode>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const WholeBrainVoxelDrawingMode& d = *iter;
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
        CaretAssertMessage(0, AString("Name " + name + "failed to match enumerated value for type WholeBrainVoxelDrawingMode"));
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
WholeBrainVoxelDrawingMode::toGuiName(Enum enumValue) {
    if (initializedFlag == false) initialize();
    
    const WholeBrainVoxelDrawingMode* enumInstance = findData(enumValue);
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
WholeBrainVoxelDrawingMode::Enum 
WholeBrainVoxelDrawingMode::fromGuiName(const AString& guiName, bool* isValidOut)
{
    if (initializedFlag == false) initialize();
    
    bool validFlag = false;
    Enum enumValue = DRAW_VOXELS_ON_TWO_D_SLICES;
    
    for (std::vector<WholeBrainVoxelDrawingMode>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const WholeBrainVoxelDrawingMode& d = *iter;
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
        CaretAssertMessage(0, AString("guiName " + guiName + "failed to match enumerated value for type WholeBrainVoxelDrawingMode"));
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
WholeBrainVoxelDrawingMode::toIntegerCode(Enum enumValue)
{
    if (initializedFlag == false) initialize();
    const WholeBrainVoxelDrawingMode* enumInstance = findData(enumValue);
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
WholeBrainVoxelDrawingMode::Enum
WholeBrainVoxelDrawingMode::fromIntegerCode(const int32_t integerCode, bool* isValidOut)
{
    if (initializedFlag == false) initialize();
    
    bool validFlag = false;
    Enum enumValue = DRAW_VOXELS_ON_TWO_D_SLICES;
    
    for (std::vector<WholeBrainVoxelDrawingMode>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const WholeBrainVoxelDrawingMode& enumInstance = *iter;
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
        CaretAssertMessage(0, AString("Integer code " + AString::number(integerCode) + "failed to match enumerated value for type WholeBrainVoxelDrawingMode"));
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
WholeBrainVoxelDrawingMode::getAllEnums(std::vector<WholeBrainVoxelDrawingMode::Enum>& allEnums)
{
    if (initializedFlag == false) initialize();
    
    allEnums.clear();
    
    for (std::vector<WholeBrainVoxelDrawingMode>::iterator iter = enumData.begin();
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
WholeBrainVoxelDrawingMode::getAllNames(std::vector<AString>& allNames, const bool isSorted)
{
    if (initializedFlag == false) initialize();
    
    allNames.clear();
    
    for (std::vector<WholeBrainVoxelDrawingMode>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        allNames.push_back(WholeBrainVoxelDrawingMode::toName(iter->enumValue));
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
WholeBrainVoxelDrawingMode::getAllGuiNames(std::vector<AString>& allGuiNames, const bool isSorted)
{
    if (initializedFlag == false) initialize();
    
    allGuiNames.clear();
    
    for (std::vector<WholeBrainVoxelDrawingMode>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        allGuiNames.push_back(WholeBrainVoxelDrawingMode::toGuiName(iter->enumValue));
    }
    
    if (isSorted) {
        std::sort(allGuiNames.begin(), allGuiNames.end());
    }
}

