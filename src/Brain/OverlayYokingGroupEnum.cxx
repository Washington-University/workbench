
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
#define __OVERLAY_YOKING_GROUP_ENUM_DECLARE__
#include "OverlayYokingGroupEnum.h"
#undef __OVERLAY_YOKING_GROUP_ENUM_DECLARE__

#include "CaretAssert.h"
#include "EventManager.h"
#include "EventOverlayYokingGroupGet.h"

using namespace caret;

    
/**
 * \class caret::OverlayYokingGroupEnum 
 * \brief Enumerated types for overlay yoking selection.
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
OverlayYokingGroupEnum::OverlayYokingGroupEnum(const Enum enumValue,
                           const AString& name,
                           const AString& guiName)
{
    this->enumValue = enumValue;
    this->integerCode = integerCodeCounter++;
    this->name = name;
    this->guiName = guiName;
    
    this->mapIndex = 0;
}

/**
 * Destructor.
 */
OverlayYokingGroupEnum::~OverlayYokingGroupEnum()
{
}

/**
 * Initialize the enumerated metadata.
 */
void
OverlayYokingGroupEnum::initialize()
{
    if (initializedFlag) {
        return;
    }
    initializedFlag = true;

    enumData.push_back(OverlayYokingGroupEnum(OVERLAY_YOKING_GROUP_OFF, 
                                    "OVERLAY_YOKING_GROUP_OFF", 
                                    "Off"));
    
    enumData.push_back(OverlayYokingGroupEnum(OVERLAY_YOKING_GROUP_1,
                                    "OVERLAY_YOKING_GROUP_1", 
                                    "1"));
    
    enumData.push_back(OverlayYokingGroupEnum(OVERLAY_YOKING_GROUP_2,
                                    "OVERLAY_YOKING_GROUP_1", 
                                    "2"));
    
    enumData.push_back(OverlayYokingGroupEnum(OVERLAY_YOKING_GROUP_3,
                                    "OVERLAY_YOKING_GROUP_3", 
                                    "3"));
    
    enumData.push_back(OverlayYokingGroupEnum(OVERLAY_YOKING_GROUP_4,
                                    "OVERLAY_YOKING_GROUP_4", 
                                    "4"));
    
}

/**
 * Find the data for and enumerated value.
 * @param enumValue
 *     The enumerated value.
 * @return Pointer to data for this enumerated type
 * or NULL if no data for type or if type is invalid.
 */
OverlayYokingGroupEnum*
OverlayYokingGroupEnum::findData(const Enum enumValue)
{
    if (initializedFlag == false) initialize();

    size_t num = enumData.size();
    for (size_t i = 0; i < num; i++) {
        OverlayYokingGroupEnum* d = &enumData[i];
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
OverlayYokingGroupEnum::toName(Enum enumValue) {
    if (initializedFlag == false) initialize();
    
    const OverlayYokingGroupEnum* enumInstance = findData(enumValue);
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
OverlayYokingGroupEnum::Enum 
OverlayYokingGroupEnum::fromName(const AString& name, bool* isValidOut)
{
    if (initializedFlag == false) initialize();
    
    bool validFlag = false;
    Enum enumValue = OVERLAY_YOKING_GROUP_OFF;
    
    for (std::vector<OverlayYokingGroupEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const OverlayYokingGroupEnum& d = *iter;
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
        CaretAssertMessage(0, AString("Name " + name + "failed to match enumerated value for type OverlayYokingGroupEnum"));
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
OverlayYokingGroupEnum::toGuiName(Enum enumValue) {
    if (initializedFlag == false) initialize();
    
    const OverlayYokingGroupEnum* enumInstance = findData(enumValue);
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
OverlayYokingGroupEnum::Enum 
OverlayYokingGroupEnum::fromGuiName(const AString& guiName, bool* isValidOut)
{
    if (initializedFlag == false) initialize();
    
    bool validFlag = false;
    Enum enumValue = OVERLAY_YOKING_GROUP_OFF;
    
    for (std::vector<OverlayYokingGroupEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const OverlayYokingGroupEnum& d = *iter;
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
        CaretAssertMessage(0, AString("guiName " + guiName + "failed to match enumerated value for type OverlayYokingGroupEnum"));
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
OverlayYokingGroupEnum::toIntegerCode(Enum enumValue)
{
    if (initializedFlag == false) initialize();
    const OverlayYokingGroupEnum* enumInstance = findData(enumValue);
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
OverlayYokingGroupEnum::Enum
OverlayYokingGroupEnum::fromIntegerCode(const int32_t integerCode, bool* isValidOut)
{
    if (initializedFlag == false) initialize();
    
    bool validFlag = false;
    Enum enumValue = OVERLAY_YOKING_GROUP_OFF;
    
    for (std::vector<OverlayYokingGroupEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const OverlayYokingGroupEnum& enumInstance = *iter;
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
        CaretAssertMessage(0, AString("Integer code " + AString::number(integerCode) + "failed to match enumerated value for type OverlayYokingGroupEnum"));
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
OverlayYokingGroupEnum::getAllEnums(std::vector<OverlayYokingGroupEnum::Enum>& allEnums)
{
    if (initializedFlag == false) initialize();
    
    allEnums.clear();
    
    for (std::vector<OverlayYokingGroupEnum>::iterator iter = enumData.begin();
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
OverlayYokingGroupEnum::getAllNames(std::vector<AString>& allNames, const bool isSorted)
{
    if (initializedFlag == false) initialize();
    
    allNames.clear();
    
    for (std::vector<OverlayYokingGroupEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        allNames.push_back(OverlayYokingGroupEnum::toName(iter->enumValue));
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
OverlayYokingGroupEnum::getAllGuiNames(std::vector<AString>& allGuiNames, const bool isSorted)
{
    if (initializedFlag == false) initialize();
    
    allGuiNames.clear();
    
    for (std::vector<OverlayYokingGroupEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        allGuiNames.push_back(OverlayYokingGroupEnum::toGuiName(iter->enumValue));
    }
    
    if (isSorted) {
        std::sort(allGuiNames.begin(), allGuiNames.end());
    }
}

/**
 * @return The selected map index associated with the given value.
 *
 * @param enumValue
 *     Value for which map index is requested.
 */
int32_t
OverlayYokingGroupEnum::getSelectedMapIndex(const Enum enumValue)
{
    CaretAssertMessage(enumValue != OVERLAY_YOKING_GROUP_OFF,
                       "Never should be called with OVERLAY_YOKING_GROUP_OFF");
    
    if (initializedFlag == false) initialize();

    OverlayYokingGroupEnum* enumInstance = findData(enumValue);
    return enumInstance->mapIndex;
}

/**
 * Set the map index for the given enum value.
 *
 * @param enumValue
 *     Value for which map index is requested.
 * @param mapIndex
 *     New value for map index.
 */
void
OverlayYokingGroupEnum::setSelectedMapIndex(const Enum enumValue,
                                            const int32_t mapIndex)
{
    CaretAssertMessage(enumValue != OVERLAY_YOKING_GROUP_OFF,
                       "Never should be called with OVERLAY_YOKING_GROUP_OFF");
    
    if (initializedFlag == false) initialize();
    
    OverlayYokingGroupEnum* enumInstance = findData(enumValue);
    enumInstance->mapIndex = mapIndex;
}

