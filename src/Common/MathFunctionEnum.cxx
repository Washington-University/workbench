
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

#define __MATH_FUNCTION_ENUM_DECLARE__
#include "MathFunctionEnum.h"
#undef __MATH_FUNCTION_ENUM_DECLARE__

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
MathFunctionEnum::MathFunctionEnum(const Enum enumValue,
                           const AString& name)
{
    this->enumValue = enumValue;
    this->name = name;
}

/**
 * Destructor.
 */
MathFunctionEnum::~MathFunctionEnum()
{
}

/**
 * Initialize the enumerated metadata.
 */
void
MathFunctionEnum::initialize()
{
    if (initializedFlag) {
        return;
    }
    initializedFlag = true;

    //enumData.push_back(MathFunctionEnum(INVALID, "INVALID"));//should this be in the data? I don't think it should, it is a placeholder for "no matching enum value"
    enumData.push_back(MathFunctionEnum(SIN, "sin"));
    enumData.push_back(MathFunctionEnum(COS, "cos"));
    enumData.push_back(MathFunctionEnum(TAN, "tan"));
    enumData.push_back(MathFunctionEnum(ASIN, "asin"));
    enumData.push_back(MathFunctionEnum(ACOS, "acos"));
    enumData.push_back(MathFunctionEnum(ATAN, "atan"));
    enumData.push_back(MathFunctionEnum(SINH, "sinh"));
    enumData.push_back(MathFunctionEnum(COSH, "cosh"));
    enumData.push_back(MathFunctionEnum(TANH, "tanh"));
    enumData.push_back(MathFunctionEnum(LN, "ln"));
    enumData.push_back(MathFunctionEnum(EXP, "exp"));
    enumData.push_back(MathFunctionEnum(LOG, "log"));
    enumData.push_back(MathFunctionEnum(SQRT, "sqrt"));
    enumData.push_back(MathFunctionEnum(ABS, "abs"));
    enumData.push_back(MathFunctionEnum(FLOOR, "floor"));
    enumData.push_back(MathFunctionEnum(CEIL, "ceil"));
    enumData.push_back(MathFunctionEnum(CLAMP, "clamp"));
}

/**
 * Find the data for and enumerated value.
 * @param enumValue
 *     The enumerated value.
 * @return Pointer to data for this enumerated type
 * or NULL if no data for type or if type is invalid.
 */
const MathFunctionEnum*
MathFunctionEnum::findData(const Enum enumValue)
{
    if (initializedFlag == false) initialize();

    size_t num = enumData.size();
    for (size_t i = 0; i < num; i++) {
        const MathFunctionEnum* d = &enumData[i];
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
MathFunctionEnum::toName(Enum enumValue) {
    if (initializedFlag == false) initialize();
    
    const MathFunctionEnum* enumInstance = findData(enumValue);
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
MathFunctionEnum::Enum 
MathFunctionEnum::fromName(const AString& name, bool* isValidOut)
{
    if (initializedFlag == false) initialize();
    
    bool validFlag = false;
    Enum enumValue = INVALID;
    
    for (std::vector<MathFunctionEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const MathFunctionEnum& d = *iter;
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
        CaretAssertMessage(0, AString("Name " + name + "failed to match enumerated value for type MathFunctionEnum"));
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
MathFunctionEnum::getAllEnums(std::vector<MathFunctionEnum::Enum>& allEnums)
{
    if (initializedFlag == false) initialize();
    
    allEnums.clear();
    
    for (std::vector<MathFunctionEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        MathFunctionEnum::Enum value =iter->enumValue;
        allEnums.push_back(iter->enumValue);
    }
}
