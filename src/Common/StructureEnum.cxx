
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
    
    //TSC: WARNING: the order of these determines the standard order of the structures in some -cifti-create-* commands, DO NOT reorder any *existing* entries

    enumData.push_back(StructureEnum(CORTEX_LEFT,
                                     "CORTEX_LEFT",
                                     "CortexLeft"));
    
    enumData.push_back(StructureEnum(CORTEX_RIGHT,
                                     "CORTEX_RIGHT",
                                     "CortexRight"));
    
    enumData.push_back(StructureEnum(CEREBELLUM,
                                     "CEREBELLUM",
                                     "Cerebellum"));
    
    enumData.push_back(StructureEnum(ACCUMBENS_LEFT,
                                     "ACCUMBENS_LEFT",
                                     "AccumbensLeft"));
    
    enumData.push_back(StructureEnum(ACCUMBENS_RIGHT,
                                     "ACCUMBENS_RIGHT",
                                     "AccumbensRight"));
    
    enumData.push_back(StructureEnum(ALL,
                                     "ALL",
                                     "All"));
    
    enumData.push_back(StructureEnum(ALL_GREY_MATTER,
                                     "ALL_GREY_MATTER",
                                     "AllGreyMatter"));
    
    enumData.push_back(StructureEnum(ALL_WHITE_MATTER,
                                     "ALL_WHITE_MATTER",
                                     "AllWhiteMatter"));
    
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
    
    enumData.push_back(StructureEnum(CEREBELLAR_WHITE_MATTER_LEFT,
                                     "CEREBELLAR_WHITE_MATTER_LEFT",
                                     "CerebellarWhiteMatterLeft"));
    
    enumData.push_back(StructureEnum(CEREBELLAR_WHITE_MATTER_RIGHT,
                                     "CEREBELLAR_WHITE_MATTER_RIGHT",
                                     "CerebellarWhiteMatterRight"));
    
    enumData.push_back(StructureEnum(CEREBELLUM_LEFT,
                                     "CEREBELLUM_LEFT", 
                                     "CerebellumLeft"));
    
    enumData.push_back(StructureEnum(CEREBELLUM_RIGHT, 
                                     "CEREBELLUM_RIGHT", 
                                     "CerebellumRight"));
    
    enumData.push_back(StructureEnum(CEREBRAL_WHITE_MATTER_LEFT,
                                     "CEREBRAL_WHITE_MATTER_LEFT",
                                     "CerebralWhiteMatterLeft"));
    
    enumData.push_back(StructureEnum(CEREBRAL_WHITE_MATTER_RIGHT,
                                     "CEREBRAL_WHITE_MATTER_RIGHT",
                                     "CerebralWhiteMatterRight"));
    
    enumData.push_back(StructureEnum(CORTEX,
                                     "CORTEX",
                                     "Cortex"));
    
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
    
    enumData.push_back(StructureEnum(INVALID,
                                     "INVALID",
                                     "Invalid"));
    
    enumData.push_back(StructureEnum(OTHER,
                                     "OTHER",
                                     "Other"));
    
    enumData.push_back(StructureEnum(OTHER_GREY_MATTER,
                                     "OTHER_GREY_MATTER",
                                     "OtherGreyMatter"));
    
    enumData.push_back(StructureEnum(OTHER_WHITE_MATTER,
                                     "OTHER_WHITE_MATTER",
                                     "OtherWhiteMatter"));
    
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
    
//    enumData.push_back(StructureEnum(SUBCORTICAL_WHITE_MATTER_LEFT, 
//                                     "SUBCORTICAL_WHITE_MATTER_LEFT", 
//                                     "SubcorticalWhiteMatterLeft"));
//    
//    enumData.push_back(StructureEnum(SUBCORTICAL_WHITE_MATTER_RIGHT, 
//                                     "SUBCORTICAL_WHITE_MATTER_RIGHT", 
//                                     "SubcorticalWhiteMatterRight"));
    
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
 * Get a GUI string representation of the enumerated type.
 * @param enumValue
 *     Enumerated value.
 * @return
 *     String representing enumerated value.
 */
AString
StructureEnum::toCiftiName(Enum enumValue) {
    if (initializedFlag == false) initialize();

    const StructureEnum* enumInstance = findData(enumValue);
    return "CIFTI_STRUCTURE_" + enumInstance->name;
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
StructureEnum::fromCiftiName(const AString& ciftiName, bool* isValidOut)
{
    if (initializedFlag == false) initialize();

    bool validFlag = false;
    Enum enumValue = INVALID;
    if (ciftiName.startsWith("CIFTI_STRUCTURE_"))
    {
        QString toMatch = ciftiName.mid(16);
        for (std::vector<StructureEnum>::iterator iter = enumData.begin();
            iter != enumData.end();
            iter++) {
            const StructureEnum& d = *iter;
            if (toMatch == d.name) {
                enumValue = d.enumValue;
                validFlag = true;
                break;
            }
        }
    }

    if (isValidOut != 0) {
        *isValidOut = validFlag;
    }
    else if (validFlag == false) {
        CaretAssertMessage(0, AString("guiName " + ciftiName + "failed to match enumerated value for type StructureEnum"));
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

/**
 * Is this 'right' structure?
 * @param enumValue
 *   The enumerated type.
 * @return 
 *   true if the enumerated value represents a 'right' structure, else false.
 */
bool 
StructureEnum::isRight(const Enum enumValue)
{
    switch (enumValue)
    {
        case ACCUMBENS_RIGHT:
        case AMYGDALA_RIGHT:
        case CAUDATE_RIGHT:
        case CEREBELLAR_WHITE_MATTER_RIGHT:
        case CEREBELLUM_RIGHT:
        case CEREBRAL_WHITE_MATTER_RIGHT:
        case CORTEX_RIGHT:
        case DIENCEPHALON_VENTRAL_RIGHT:
        case HIPPOCAMPUS_RIGHT:
        case PALLIDUM_RIGHT:
        case PUTAMEN_RIGHT:
        case THALAMUS_RIGHT:
            return true;
        case ALL://avoid default so smart compilers can warn when a new structure isn't added here
        case ALL_GREY_MATTER:
        case ALL_WHITE_MATTER:
        case BRAIN_STEM:
        case CEREBELLUM:
        case CORTEX:
        case INVALID:
        case OTHER:
        case OTHER_GREY_MATTER:
        case OTHER_WHITE_MATTER:
            return false;//visually separate none/both from opposite cases
        case ACCUMBENS_LEFT:
        case AMYGDALA_LEFT:
        case CAUDATE_LEFT:
        case CEREBELLAR_WHITE_MATTER_LEFT:
        case CEREBELLUM_LEFT:
        case CEREBRAL_WHITE_MATTER_LEFT:
        case CORTEX_LEFT:
        case DIENCEPHALON_VENTRAL_LEFT:
        case HIPPOCAMPUS_LEFT:
        case PALLIDUM_LEFT:
        case PUTAMEN_LEFT:
        case THALAMUS_LEFT:
            return false;
    }
    CaretAssert(false);
    return false;
}

/**
 * Is this 'left' structure?
 * @param enumValue
 *   The enumerated type.
 * @return 
 *   true if the enumerated value represents a 'left' structure, else false.
 */
bool 
StructureEnum::isLeft(const Enum enumValue)
{
    switch (enumValue)
    {
        case ACCUMBENS_LEFT:
        case AMYGDALA_LEFT:
        case CAUDATE_LEFT:
        case CEREBELLAR_WHITE_MATTER_LEFT:
        case CEREBELLUM_LEFT:
        case CEREBRAL_WHITE_MATTER_LEFT:
        case CORTEX_LEFT:
        case DIENCEPHALON_VENTRAL_LEFT:
        case HIPPOCAMPUS_LEFT:
        case PALLIDUM_LEFT:
        case PUTAMEN_LEFT:
        case THALAMUS_LEFT:
            return true;
        case ALL://avoid default so smart compilers can warn when a new structure isn't added here
        case ALL_GREY_MATTER:
        case ALL_WHITE_MATTER:
        case BRAIN_STEM:
        case CEREBELLUM:
        case CORTEX:
        case INVALID:
        case OTHER:
        case OTHER_GREY_MATTER:
        case OTHER_WHITE_MATTER:
            return false;//visually separate none/both from opposite cases
        case ACCUMBENS_RIGHT:
        case AMYGDALA_RIGHT:
        case CAUDATE_RIGHT:
        case CEREBELLAR_WHITE_MATTER_RIGHT:
        case CEREBELLUM_RIGHT:
        case CEREBRAL_WHITE_MATTER_RIGHT:
        case CORTEX_RIGHT:
        case DIENCEPHALON_VENTRAL_RIGHT:
        case HIPPOCAMPUS_RIGHT:
        case PALLIDUM_RIGHT:
        case PUTAMEN_RIGHT:
        case THALAMUS_RIGHT:
            return false;
    }
    CaretAssert(false);
    return false;
}

/**
 * Is this a 'single' structure?  The structure must be a valid type.
 *
 * @param enumValue
 *   The enumerated type.
 * @return
 *   true if the enumerated value represents a 'single' structure, else false.
 */
bool
StructureEnum::isSingleStructure(const Enum enumValue)
{
    bool singleStructureFlag = true;
    
    switch (enumValue) {
        case ACCUMBENS_LEFT:
            break;
        case ACCUMBENS_RIGHT:
            break;
        case ALL:
            singleStructureFlag = false;
            break;
        case ALL_GREY_MATTER:
            singleStructureFlag = false;
            break;
        case ALL_WHITE_MATTER:
            singleStructureFlag = false;
            break;
        case AMYGDALA_LEFT:
            break;
        case AMYGDALA_RIGHT:
            break;
        case BRAIN_STEM:
            break;
        case CAUDATE_LEFT:
            break;
        case CAUDATE_RIGHT:
            break;
        case CEREBELLAR_WHITE_MATTER_LEFT:
            break;
        case CEREBELLAR_WHITE_MATTER_RIGHT:
            break;
        case CEREBELLUM:
            break;
        case CEREBELLUM_LEFT:
            break;
        case CEREBELLUM_RIGHT:
            break;
        case CEREBRAL_WHITE_MATTER_LEFT:
            break;
        case CEREBRAL_WHITE_MATTER_RIGHT:
            break;
        case CORTEX:
            break;
        case CORTEX_LEFT:
            break;
        case CORTEX_RIGHT:
            break;
        case DIENCEPHALON_VENTRAL_LEFT:
            break;
        case DIENCEPHALON_VENTRAL_RIGHT:
            break;
        case HIPPOCAMPUS_LEFT:
            break;
        case HIPPOCAMPUS_RIGHT:
            break;
        case INVALID:
            singleStructureFlag = false;
            break;
        case PALLIDUM_LEFT:
            break;
        case PALLIDUM_RIGHT:
            break;
        case OTHER:
            singleStructureFlag = false;
            break;
        case OTHER_GREY_MATTER:
            singleStructureFlag = false;
            break;
        case OTHER_WHITE_MATTER:
            singleStructureFlag = false;
            break;
        case PUTAMEN_LEFT:
            break;
        case PUTAMEN_RIGHT:
            break;
//        case SUBCORTICAL_WHITE_MATTER_LEFT:
//            contralateralStructure = SUBCORTICAL_WHITE_MATTER_RIGHT;
//            break;
//        case SUBCORTICAL_WHITE_MATTER_RIGHT:
//            contralateralStructure = SUBCORTICAL_WHITE_MATTER_LEFT;
//            break;
        case THALAMUS_LEFT:
            break;
        case THALAMUS_RIGHT:
            break;
    }
    
    return singleStructureFlag;
}


/**
 * Are the two structure's cortices and contralateral (is one CortexLeft
 * and one CortexRight)?
 *
 * @param enumValueA
 *    First structure enumerated type.
 * @param enumValueB
 *    Second structure enumerated type.
 * @return
 *    True if one is CORTEX_LEFT and one is CORTEX_LEFT.
 */
bool 
StructureEnum::isCortexContralateral(const Enum enumValueA,
                               const Enum enumValueB)
{
    if ((enumValueA == CORTEX_LEFT)
        && (enumValueB == CORTEX_RIGHT)) {
        return true;
    }
    if ((enumValueA == CORTEX_RIGHT)
        && (enumValueB == CORTEX_LEFT)) {
        return true;
    }
    
    return false;
}

/**
 * For the given structure return its contralateral structure.
 * Thats is, if this is a left/right structure return its
 * corresponding structure from the other side.
 * 
 * @param enumValue
 *    Structure for which contralateral structure is desired.
 * @return The contralateral structure or NULL if it does
 *    not have a contralateral structure.
 */
StructureEnum::Enum 
StructureEnum::getContralateralStructure(const Enum enumValue)
{
    StructureEnum::Enum contralateralStructure = INVALID;

    switch (enumValue) {
        case ACCUMBENS_LEFT:
            contralateralStructure = ACCUMBENS_RIGHT;
            break;
        case ACCUMBENS_RIGHT:
            contralateralStructure = ACCUMBENS_LEFT;
            break;
        case ALL:
            contralateralStructure = INVALID;
            break;
        case ALL_GREY_MATTER:
            break;
        case ALL_WHITE_MATTER:
            break;
        case AMYGDALA_LEFT:
            contralateralStructure = AMYGDALA_RIGHT;
            break;
        case AMYGDALA_RIGHT:
            contralateralStructure = AMYGDALA_LEFT;
            break;
        case BRAIN_STEM:
            contralateralStructure = INVALID;
            break;
        case CAUDATE_LEFT:
            contralateralStructure = CAUDATE_RIGHT;
            break;
        case CAUDATE_RIGHT:
            contralateralStructure = CAUDATE_LEFT;
            break;
        case CEREBELLAR_WHITE_MATTER_LEFT:
            contralateralStructure= CEREBELLAR_WHITE_MATTER_RIGHT;
            break;
        case CEREBELLAR_WHITE_MATTER_RIGHT:
            contralateralStructure = CEREBELLAR_WHITE_MATTER_LEFT;
            break;
        case CEREBELLUM:
            contralateralStructure = INVALID;
            break;
        case CEREBELLUM_LEFT:
            contralateralStructure = CEREBELLUM_RIGHT;
            break;
        case CEREBELLUM_RIGHT:
            contralateralStructure = CEREBELLUM_LEFT;
            break;
        case CEREBRAL_WHITE_MATTER_LEFT:
            contralateralStructure = CEREBELLAR_WHITE_MATTER_RIGHT;
            break;
        case CEREBRAL_WHITE_MATTER_RIGHT:
            contralateralStructure = CEREBELLAR_WHITE_MATTER_LEFT;
            break;
        case CORTEX:
            break;
        case CORTEX_LEFT:
            contralateralStructure = CORTEX_RIGHT;
            break;
        case CORTEX_RIGHT:
            contralateralStructure = CORTEX_LEFT;
            break;
        case DIENCEPHALON_VENTRAL_LEFT:
            contralateralStructure = DIENCEPHALON_VENTRAL_RIGHT;
            break;
        case DIENCEPHALON_VENTRAL_RIGHT:
            contralateralStructure = DIENCEPHALON_VENTRAL_LEFT;
            break;
        case HIPPOCAMPUS_LEFT:
            contralateralStructure = HIPPOCAMPUS_RIGHT;
            break;
        case HIPPOCAMPUS_RIGHT:
            contralateralStructure = HIPPOCAMPUS_LEFT;
            break;
        case INVALID:
            contralateralStructure = INVALID;
            break;
        case PALLIDUM_LEFT:
            contralateralStructure = PALLIDUM_RIGHT;
            break;
        case PALLIDUM_RIGHT:
            contralateralStructure = PALLIDUM_LEFT;
            break;
        case OTHER:
            contralateralStructure = INVALID;
            break;
        case OTHER_GREY_MATTER:
            break;
        case OTHER_WHITE_MATTER:
            break;
        case PUTAMEN_LEFT:
            contralateralStructure = PUTAMEN_RIGHT;
            break;
        case PUTAMEN_RIGHT:
            contralateralStructure = PUTAMEN_LEFT;
            break;
//        case SUBCORTICAL_WHITE_MATTER_LEFT:
//            contralateralStructure = SUBCORTICAL_WHITE_MATTER_RIGHT;
//            break;
//        case SUBCORTICAL_WHITE_MATTER_RIGHT:
//            contralateralStructure = SUBCORTICAL_WHITE_MATTER_LEFT;
//            break;
        case THALAMUS_LEFT:
            contralateralStructure = THALAMUS_RIGHT;
            break;
        case THALAMUS_RIGHT:
            contralateralStructure = THALAMUS_LEFT;
            break;
    }
    
    return contralateralStructure;
}


