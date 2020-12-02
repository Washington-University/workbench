
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

#define __MODEL_DISPLAY_CONTROLLER_TYPE_ENUM_DECLARE__
#include "ModelTypeEnum.h"
#undef __MODEL_DISPLAY_CONTROLLER_TYPE_ENUM_DECLARE__

#include "CaretAssert.h"

using namespace caret;

/**
 * Constructor.
 *
 * @param enumValue
 *    An enumerated value.
 * @param integerCode
 *    Integer code for this enumerated value.
 *
 * @param name
 *    Name of enumerated value.
 *
 * @param guiName
 *    User-friendly name for use in user-interface.
 */
ModelTypeEnum::ModelTypeEnum(const Enum enumValue,
                           const int32_t integerCode,
                           const AString& name,
                           const AString& guiName)
{
    this->enumValue = enumValue;
    this->integerCode = integerCode;
    this->name = name;
    this->guiName = guiName;
}

/**
 * Destructor.
 */
ModelTypeEnum::~ModelTypeEnum()
{
}

/**
 * Initialize the enumerated metadata.
 */
void
ModelTypeEnum::initialize()
{
    if (initializedFlag) {
        return;
    }
    initializedFlag = true;

    enumData.push_back(ModelTypeEnum(MODEL_TYPE_INVALID,
                                     0,
                                     "MODEL_TYPE_INVALID",
                                     "Invalid"));
    
    enumData.push_back(ModelTypeEnum(MODEL_TYPE_CHART,
                                     1,
                                     "MODEL_TYPE_CHART",
                                     "Chart"));
    
    enumData.push_back(ModelTypeEnum(MODEL_TYPE_CHART_TWO,
                                     2,
                                     "MODEL_TYPE_CHART_TWO",
                                     "Chart"));
    
    enumData.push_back(ModelTypeEnum(MODEL_TYPE_SURFACE,
                                     3,
                                     "MODEL_TYPE_SURFACE",
                                     "Surface"));
    
    enumData.push_back(ModelTypeEnum(MODEL_TYPE_SURFACE_MONTAGE,
                                     4,
                                     "MODEL_TYPE_SURFACE_MONTAGE",
                                     "Surface Montage"));
    
    enumData.push_back(ModelTypeEnum(MODEL_TYPE_VOLUME_SLICES,
                                     5,
                                     "MODEL_TYPE_VOLUME_SLICES",
                                     "Volume"));
    
    enumData.push_back(ModelTypeEnum(MODEL_TYPE_WHOLE_BRAIN,
                                     6,
                                     "MODEL_TYPE_WHOLE_BRAIN",
                                     "Whole Brain"));
    
    enumData.push_back(ModelTypeEnum(MODEL_TYPE_MULTI_MEDIA,
                                     7,
                                     "MODEL_TYPE_MULTI_MEDIA",
                                     "Media"));
    
}

/**
 * Find the data for and enumerated value.
 * @param enumValue
 *     The enumerated value.
 * @return Pointer to data for this enumerated type
 * or NULL if no data for type or if type is invalid.
 */
const ModelTypeEnum*
ModelTypeEnum::findData(const Enum enumValue)
{
    if (initializedFlag == false) initialize();

    size_t num = enumData.size();
    for (size_t i = 0; i < num; i++) {
        const ModelTypeEnum* d = &enumData[i];
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
ModelTypeEnum::toName(Enum enumValue) {
    if (initializedFlag == false) initialize();
    
    const ModelTypeEnum* enumInstance = findData(enumValue);
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
ModelTypeEnum::Enum 
ModelTypeEnum::fromName(const AString& name, bool* isValidOut)
{
    if (initializedFlag == false) initialize();
    
    bool validFlag = false;
    Enum enumValue = MODEL_TYPE_INVALID;
    
    for (std::vector<ModelTypeEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const ModelTypeEnum& d = *iter;
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
        CaretAssertMessage(0, AString("Name " + name + "failed to match enumerated value for type ModelTypeEnum"));
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
ModelTypeEnum::toGuiName(Enum enumValue) {
    if (initializedFlag == false) initialize();
    
    const ModelTypeEnum* enumInstance = findData(enumValue);
    return enumInstance->guiName;
}

/**
 * Get an enumerated value corresponding to its GUI name.
 * @param guiName
 *     Name of enumerated value.
 * @param isValidOut 
 *     If not NULL, it is set indicating that a
 *     enum value exists for the input name.
 * @return 
 *     Enumerated value.
 */
ModelTypeEnum::Enum 
ModelTypeEnum::fromGuiName(const AString& guiName, bool* isValidOut)
{
    if (initializedFlag == false) initialize();
    
    bool validFlag = false;
    Enum enumValue = MODEL_TYPE_INVALID;
    
    for (std::vector<ModelTypeEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const ModelTypeEnum& d = *iter;
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
        CaretAssertMessage(0, AString("guiName " + guiName + "failed to match enumerated value for type ModelTypeEnum"));
    }
    return enumValue;
}

/**
 * Get the integer code for a data type.
 * @param enumValue
 *    Enumerated value.
 * @return
 *    Integer code for data type.
 */
int32_t
ModelTypeEnum::toIntegerCode(Enum enumValue)
{
    if (initializedFlag == false) initialize();
    const ModelTypeEnum* enumInstance = findData(enumValue);
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
ModelTypeEnum::Enum
ModelTypeEnum::fromIntegerCode(const int32_t integerCode, bool* isValidOut)
{
    if (initializedFlag == false) initialize();
    
    bool validFlag = false;
    Enum enumValue = MODEL_TYPE_INVALID;
    
    for (std::vector<ModelTypeEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const ModelTypeEnum& enumInstance = *iter;
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
        CaretAssertMessage(0, AString("Integer code " + AString::number(integerCode) + "failed to match enumerated value for type ModelTypeEnum"));
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
ModelTypeEnum::getAllEnums(std::vector<ModelTypeEnum::Enum>& allEnums)
{
    if (initializedFlag == false) initialize();
    
    allEnums.clear();
    
    for (std::vector<ModelTypeEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        allEnums.push_back(iter->enumValue);
    }
}

