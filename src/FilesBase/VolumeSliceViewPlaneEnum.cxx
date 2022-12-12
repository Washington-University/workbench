
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

#include <algorithm>
#define __VOLUME_SLICE_VIEW_AXIS_ENUM_DECLARE__
#include "VolumeSliceViewPlaneEnum.h"
#undef __VOLUME_SLICE_VIEW_AXIS_ENUM_DECLARE__

#include "CaretAssert.h"
#include "MathFunctions.h"
#include "Plane.h"
#include "Vector3D.h"

using namespace caret;

    
/**
 * \class VolumeSliceViewPlaneEnum 
 * \brief enumerated type for volume slice plane
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
 * @param guiNameAbbreviation
 *    Abbreviated name for user-interface.
 */
VolumeSliceViewPlaneEnum::VolumeSliceViewPlaneEnum(const Enum enumValue,
                                                 const AString& name,
                                                 const AString& guiName,
                                                 const AString& guiNameAbbreviation)
{
    this->enumValue = enumValue;
    this->integerCode = integerCodeCounter++;
    this->name = name;
    this->guiName = guiName;
    this->guiNameAbbreviation = guiNameAbbreviation;
}

/**
 * Destructor.
 */
VolumeSliceViewPlaneEnum::~VolumeSliceViewPlaneEnum()
{
}

/**
 * Initialize the enumerated metadata.
 */
void
VolumeSliceViewPlaneEnum::initialize()
{
    if (initializedFlag) {
        return;
    }
    initializedFlag = true;

    enumData.push_back(VolumeSliceViewPlaneEnum(ALL, 
                                               "ALL", 
                                               "All",
                                               "All"));
    
    enumData.push_back(VolumeSliceViewPlaneEnum(AXIAL, 
                                               "AXIAL", 
                                               "Axial",
                                               "A"));
    
    enumData.push_back(VolumeSliceViewPlaneEnum(CORONAL, 
                                               "CORONAL", 
                                               "Coronal",
                                               "C"));
    
    enumData.push_back(VolumeSliceViewPlaneEnum(PARASAGITTAL, 
                                               "PARASAGITTAL", 
                                               "Parasagittal",
                                               "P"));    
}

/**
 * Find the data for and enumerated value.
 * @param enumValue
 *     The enumerated value.
 * @return Pointer to data for this enumerated type
 * or NULL if no data for type or if type is invalid.
 */
const VolumeSliceViewPlaneEnum*
VolumeSliceViewPlaneEnum::findData(const Enum enumValue)
{
    if (initializedFlag == false) initialize();

    size_t num = enumData.size();
    for (size_t i = 0; i < num; i++) {
        const VolumeSliceViewPlaneEnum* d = &enumData[i];
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
VolumeSliceViewPlaneEnum::toName(Enum enumValue) {
    if (initializedFlag == false) initialize();
    
    const VolumeSliceViewPlaneEnum* enumInstance = findData(enumValue);
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
VolumeSliceViewPlaneEnum::Enum 
VolumeSliceViewPlaneEnum::fromName(const AString& name, bool* isValidOut)
{
    if (initializedFlag == false) initialize();
    
    bool validFlag = false;
    Enum enumValue = AXIAL;
    
    for (std::vector<VolumeSliceViewPlaneEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const VolumeSliceViewPlaneEnum& d = *iter;
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
        CaretAssertMessage(0, AString("Name " + name + "failed to match enumerated value for type VolumeSliceViewPlaneEnum"));
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
VolumeSliceViewPlaneEnum::toGuiName(Enum enumValue) {
    if (initializedFlag == false) initialize();
    
    const VolumeSliceViewPlaneEnum* enumInstance = findData(enumValue);
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
VolumeSliceViewPlaneEnum::Enum 
VolumeSliceViewPlaneEnum::fromGuiName(const AString& guiName, bool* isValidOut)
{
    if (initializedFlag == false) initialize();
    
    bool validFlag = false;
    Enum enumValue = AXIAL;
    
    for (std::vector<VolumeSliceViewPlaneEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const VolumeSliceViewPlaneEnum& d = *iter;
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
        CaretAssertMessage(0, AString("guiName " + guiName + "failed to match enumerated value for type VolumeSliceViewPlaneEnum"));
    }
    return enumValue;
}

/**
 * Get a GUI abbreviated string representation  of the enumerated type.
 * @param enumValue 
 *     Enumerated value.
 * @return 
 *     Short name representing enumerated value.
 */
AString 
VolumeSliceViewPlaneEnum::toGuiNameAbbreviation(Enum enumValue) {
    if (initializedFlag == false) initialize();
    
    const VolumeSliceViewPlaneEnum* enumInstance = findData(enumValue);
    return enumInstance->guiNameAbbreviation;
}

/**
 * Get an enumerated value corresponding to its abbreviated GUI name.
 * @param guiNameAbbreviation 
 *     Name of enumerated value.
 * @param isValidOut 
 *     If not NULL, it is set indicating that a
 *     enum value exists for the input name.
 * @return 
 *     Enumerated value.
 */
VolumeSliceViewPlaneEnum::Enum 
VolumeSliceViewPlaneEnum::fromGuiNameAbbreviation(const AString& guiNameAbbreviation, bool* isValidOut)
{
    if (initializedFlag == false) initialize();
    
    bool validFlag = false;
    Enum enumValue = AXIAL;
    
    for (std::vector<VolumeSliceViewPlaneEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const VolumeSliceViewPlaneEnum& d = *iter;
        if (d.guiNameAbbreviation == guiNameAbbreviation) {
            enumValue = d.enumValue;
            validFlag = true;
            break;
        }
    }
    
    if (isValidOut != 0) {
        *isValidOut = validFlag;
    }
    else if (validFlag == false) {
        CaretAssertMessage(0, AString("guiNameAbbreviation " + guiNameAbbreviation + "failed to match enumerated value for type VolumeSliceViewPlaneEnum"));
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
VolumeSliceViewPlaneEnum::toIntegerCode(Enum enumValue)
{
    if (initializedFlag == false) initialize();
    const VolumeSliceViewPlaneEnum* enumInstance = findData(enumValue);
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
VolumeSliceViewPlaneEnum::Enum
VolumeSliceViewPlaneEnum::fromIntegerCode(const int32_t integerCode, bool* isValidOut)
{
    if (initializedFlag == false) initialize();
    
    bool validFlag = false;
    Enum enumValue = AXIAL;
    
    for (std::vector<VolumeSliceViewPlaneEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const VolumeSliceViewPlaneEnum& enumInstance = *iter;
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
        CaretAssertMessage(0, AString("Integer code " + AString::number(integerCode) + "failed to match enumerated value for type VolumeSliceViewPlaneEnum"));
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
VolumeSliceViewPlaneEnum::getAllEnums(std::vector<VolumeSliceViewPlaneEnum::Enum>& allEnums)
{
    if (initializedFlag == false) initialize();
    
    allEnums.clear();
    
    for (std::vector<VolumeSliceViewPlaneEnum>::iterator iter = enumData.begin();
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
VolumeSliceViewPlaneEnum::getAllNames(std::vector<AString>& allNames, const bool isSorted)
{
    if (initializedFlag == false) initialize();
    
    allNames.clear();
    
    for (std::vector<VolumeSliceViewPlaneEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        allNames.push_back(VolumeSliceViewPlaneEnum::toName(iter->enumValue));
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
VolumeSliceViewPlaneEnum::getAllGuiNames(std::vector<AString>& allGuiNames, const bool isSorted)
{
    if (initializedFlag == false) initialize();
    
    allGuiNames.clear();
    
    for (std::vector<VolumeSliceViewPlaneEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        allGuiNames.push_back(VolumeSliceViewPlaneEnum::toGuiName(iter->enumValue));
    }
    
    if (isSorted) {
        std::sort(allGuiNames.begin(), allGuiNames.end());
    }
}

/**
 * @return The view plane that whose normal vector best matches
 * the plane's normal vector.  If ALL is returned, it should be interpreted
 * as finding view plane failed
 * @param plane
 *     The plane.
 */
VolumeSliceViewPlaneEnum::Enum
VolumeSliceViewPlaneEnum::fromPlane(const Plane& plane)
{
    VolumeSliceViewPlaneEnum::Enum sliceViewPlaneOut(VolumeSliceViewPlaneEnum::ALL);
    
    if (plane.isValidPlane()) {
        Vector3D planeNormal;
        plane.getNormalVector(planeNormal);
        
        const Vector3D axialSuperiorNormal(0.0, 0.0, 1.0);
        const Vector3D axialInferiorNormal(0.0, 0.0, -1.0);
        const Vector3D coronalAnteriorNormal(0.0, 1.0, 0.0);
        const Vector3D coronalPosteriorNormal(0.0, -1.0, 0.0);
        const Vector3D parasagittalRightNormal(1.0, 0.0, 0.0);
        const Vector3D parasagittalLeftNormal(-1.0, 0.0, 0.0);
        
        float minimumAngle(100000.0);
        
        const float axialSuperiorAngle(MathFunctions::angleInDegreesBetweenVectors(planeNormal,
                                                                                   axialSuperiorNormal));
        if (axialSuperiorAngle < minimumAngle) {
            minimumAngle      = axialSuperiorAngle;
            sliceViewPlaneOut = VolumeSliceViewPlaneEnum::AXIAL;
        }
        
        const float axialInferiorAngle(MathFunctions::angleInDegreesBetweenVectors(planeNormal,
                                                                                   axialInferiorNormal));
        if (axialInferiorAngle < minimumAngle) {
            minimumAngle      = axialInferiorAngle;
            sliceViewPlaneOut = VolumeSliceViewPlaneEnum::AXIAL;
        }
        
        
        const float coronalAnteriorAngle(MathFunctions::angleInDegreesBetweenVectors(planeNormal,
                                                                                     coronalAnteriorNormal));
        if (coronalAnteriorAngle < minimumAngle) {
            minimumAngle      = coronalAnteriorAngle;
            sliceViewPlaneOut = VolumeSliceViewPlaneEnum::CORONAL;
        }
        
        const float coronalPosteriorAngle(MathFunctions::angleInDegreesBetweenVectors(planeNormal,
                                                                                      coronalPosteriorNormal));
        if (coronalPosteriorAngle < minimumAngle) {
            minimumAngle      = coronalPosteriorAngle;
            sliceViewPlaneOut = VolumeSliceViewPlaneEnum::CORONAL;
        }
        
        const float parasagittalRightAngle(MathFunctions::angleInDegreesBetweenVectors(planeNormal,
                                                                                       parasagittalRightNormal));
        if (parasagittalRightAngle < minimumAngle) {
            minimumAngle      = parasagittalRightAngle;
            sliceViewPlaneOut = VolumeSliceViewPlaneEnum::PARASAGITTAL;
        }
        
        const float parasagittalLeftAngle(MathFunctions::angleInDegreesBetweenVectors(planeNormal,
                                                                                      parasagittalLeftNormal));
        if (parasagittalLeftAngle < minimumAngle) {
            minimumAngle      = parasagittalLeftAngle;
            sliceViewPlaneOut = VolumeSliceViewPlaneEnum::PARASAGITTAL;
        }
    }
    
    return sliceViewPlaneOut;
}


