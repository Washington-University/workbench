
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

#define __STRUCTURE_ENUM_DECLARE__
#include "StructureEnum.h"
#undef __STRUCTURE_ENUM_DECLARE__

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
StructureEnum::StructureEnum(const Enum enumValue,
                           const AString& name,
                           const AString& guiName)
{
    this->enumValue = enumValue;
    this->integerCode = StructureEnum::integerCodeGenerator++;
    this->name = name;
    this->guiName = guiName;
}

/**
 * Destructor.
 */
StructureEnum::~StructureEnum()
{
}

/**
 * Initialize the enumerated metadata.
 */
void
StructureEnum::initialize()
{
    if (initializedFlag) {
        return;
    }
    initializedFlag = true;

    enumData.push_back(StructureEnum(ALL, 
                                     "ALL", 
                                     "All"));
    
    enumData.push_back(StructureEnum(CORTEX_LEFT, 
                                     "CORTEX_LEFT", 
                                     "CortexLeft"));
    
    enumData.push_back(StructureEnum(CORTEX_RIGHT, 
                                     "CORTEX_RIGHT", 
                                     "CortexRight"));
    
    enumData.push_back(StructureEnum(CEREBELLUM, 
                                     "CEREBELLUM", 
                                     "Cerebellum"));
    
    enumData.push_back(StructureEnum(CEREBELLUM_LEFT, 
                                     "CEREBELLUM_LEFT", 
                                     "CerebellumLeft"));
    
    enumData.push_back(StructureEnum(CEREBELLUM_RIGHT, 
                                     "CEREBELLUM_RIGHT", 
                                     "CerebellumRight"));
    
    enumData.push_back(StructureEnum(OTHER, 
                                     "OTHER", 
                                     "Other"));
    
    enumData.push_back(StructureEnum(INVALID, 
                                     "INVALID", 
                                     "Invalid"));
    
    enumData.push_back(StructureEnum(ACCUMBENS_LEFT, 
                                     "ACCUMBENS_LEFT", 
                                     "AccumbensLeft"));
    
    enumData.push_back(StructureEnum(ACCUMBENS_RIGHT, 
                                     "ACCUMBENS_RIGHT", 
                                     "AccumbensRight"));
    
    enumData.push_back(StructureEnum(AMYGDALA_LEFT, 
                                     "AMYGDALA_LEFT", 
                                     "AmygdalaLeft"));
    
    enumData.push_back(StructureEnum(AMYGDALA_RIGHT, 
                                     "AMYGDALA_RIGHT", 
                                     "AmygdalaRight"));
    
    enumData.push_back(StructureEnum(BRAIN_STEM, 
                                     "BRAIN_STEM", 
                                     "BrainStem"));
    
    enumData.push_back(StructureEnum(CAUDATE_LEFT, 
                                     "CAUDATE_LEFT", 
                                     "CaudateLeft"));
    
    enumData.push_back(StructureEnum(CAUDATE_RIGHT, 
                                     "CAUDATE_RIGHT", 
                                     "CaudateRight"));
    
    enumData.push_back(StructureEnum(DIENCEPHALON_VENTRAL_LEFT, 
                                     "DIENCEPHALON_VENTRAL_LEFT", 
                                     "DiencephalonVentralLeft"));
    
    enumData.push_back(StructureEnum(DIENCEPHALON_VENTRAL_RIGHT, 
                                     "DIENCEPHALON_VENTRAL_RIGHT", 
                                     "DiencephalonVentralRight"));
    
    enumData.push_back(StructureEnum(HIPPOCAMPUS_LEFT, 
                                     "HIPPOCAMPUS_LEFT", 
                                     "HippocampusLeft"));
    
    enumData.push_back(StructureEnum(HIPPOCAMPUS_RIGHT, 
                                     "HIPPOCAMPUS_RIGHT", 
                                     "HippocampusRight"));
    
    enumData.push_back(StructureEnum(PALLIDUM_LEFT, 
                                     "PALLIDUM_LEFT", 
                                     "PallidumLeft"));
    
    enumData.push_back(StructureEnum(PALLIDUM_RIGHT, 
                                     "PALLIDUM_RIGHT", 
                                     "PallidumRight"));
    
    enumData.push_back(StructureEnum(PUTAMEN_LEFT, 
                                     "PUTAMEN_LEFT", 
                                     "PutamenLeft"));
    
    enumData.push_back(StructureEnum(PUTAMEN_RIGHT, 
                                     "PUTAMEN_RIGHT", 
                                     "PutamenRight"));
    
    enumData.push_back(StructureEnum(SUBCORTICAL_WHITE_MATTER_LEFT, 
                                     "SUBCORTICAL_WHITE_MATTER_LEFT", 
                                     "SubcorticalWhiteMatterLeft"));
    
    enumData.push_back(StructureEnum(SUBCORTICAL_WHITE_MATTER_RIGHT, 
                                     "SUBCORTICAL_WHITE_MATTER_RIGHT", 
                                     "SubcorticalWhiteMatterRight"));
    
    enumData.push_back(StructureEnum(THALAMUS_LEFT, 
                                     "THALAMUS_LEFT", 
                                     "ThalamusLeft"));
    
    enumData.push_back(StructureEnum(THALAMUS_RIGHT, 
                                     "THALAMUS_RIGHT", 
                                     "ThalamusRight"));
}

/**
 * Find the data for and enumerated value.
 * @param enumValue
 *     The enumerated value.
 * @return Pointer to data for this enumerated type
 * or NULL if no data for type or if type is invalid.
 */
const StructureEnum*
StructureEnum::findData(const Enum enumValue)
{
    if (initializedFlag == false) initialize();

    size_t num = enumData.size();
    for (size_t i = 0; i < num; i++) {
        const StructureEnum* d = &enumData[i];
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
StructureEnum::toName(Enum enumValue) {
    if (initializedFlag == false) initialize();
    
    const StructureEnum* enumInstance = findData(enumValue);
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
StructureEnum::Enum 
StructureEnum::fromName(const AString& name, bool* isValidOut)
{
    if (initializedFlag == false) initialize();
    
    bool validFlag = false;
    Enum enumValue = INVALID;
    
    for (std::vector<StructureEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const StructureEnum& d = *iter;
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
        CaretAssertMessage(0, AString("Name " + name + "failed to match enumerated value for type StructureEnum"));
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
StructureEnum::toGuiName(Enum enumValue) {
    if (initializedFlag == false) initialize();
    
    const StructureEnum* enumInstance = findData(enumValue);
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
StructureEnum::Enum 
StructureEnum::fromGuiName(const AString& guiName, bool* isValidOut)
{
    if (initializedFlag == false) initialize();
    
    bool validFlag = false;
    Enum enumValue = INVALID;
    
    for (std::vector<StructureEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const StructureEnum& d = *iter;
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
        CaretAssertMessage(0, AString("guiName " + guiName + "failed to match enumerated value for type StructureEnum"));
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
StructureEnum::toIntegerCode(Enum enumValue)
{
    if (initializedFlag == false) initialize();
    const StructureEnum* enumInstance = findData(enumValue);
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
StructureEnum::Enum
StructureEnum::fromIntegerCode(const int32_t integerCode, bool* isValidOut)
{
    if (initializedFlag == false) initialize();
    
    bool validFlag = false;
    Enum enumValue = INVALID;
    
    for (std::vector<StructureEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const StructureEnum& enumInstance = *iter;
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
        CaretAssertMessage(0, AString("Integer code " + AString::number(integerCode) + "failed to match enumerated value for type StructureEnum"));
    }
    return enumValue;
}

/**
 * Get all of the enumerated type values.  The values can be used
 * as parameters to toXXX() methods to get associated metadata.
 *
 * @param allEnums
 *     A vector that is OUTPUT containing all of the enumerated values
 *     except ALL.
 */
void
StructureEnum::getAllEnums(std::vector<StructureEnum::Enum>& allEnums)
{
    if (initializedFlag == false) initialize();
    
    allEnums.clear();
    
    for (std::vector<StructureEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        StructureEnum::Enum value =iter->enumValue;
        if (value == ALL) {
            // nothing
        }
        else {
            allEnums.push_back(iter->enumValue);
        }
    }
}

