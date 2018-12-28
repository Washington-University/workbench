
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

#include "ReductionEnum.h"

#include "CaretAssert.h"

using namespace caret;
using namespace std;

vector<ReductionEnum> ReductionEnum::enumData;
bool ReductionEnum::initializedFlag = false;

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
ReductionEnum::ReductionEnum(const Enum enumValue,
                           const AString& name, const AString& explanation)
{
    this->enumValue = enumValue;
    this->name = name;
    this->explanation = explanation;
}

/**
 * Destructor.
 */
ReductionEnum::~ReductionEnum()
{
}

/**
 * Initialize the enumerated metadata.
 */
void
ReductionEnum::initialize()
{
    if (initializedFlag) {
        return;
    }
    initializedFlag = true;

    enumData.push_back(ReductionEnum(MAX, "MAX", "the maximum value"));
    enumData.push_back(ReductionEnum(MIN, "MIN", "the minimum value"));
    enumData.push_back(ReductionEnum(INDEXMAX, "INDEXMAX", "the 1-based index of the maximum value"));
    enumData.push_back(ReductionEnum(INDEXMIN, "INDEXMIN", "the 1-based index of the minimum value"));
    enumData.push_back(ReductionEnum(SUM, "SUM", "add all values"));
    enumData.push_back(ReductionEnum(PRODUCT, "PRODUCT", "multiply all values"));
    enumData.push_back(ReductionEnum(MEAN, "MEAN", "the mean of the data"));
    enumData.push_back(ReductionEnum(STDEV, "STDEV", "the standard deviation (N denominator)"));
    enumData.push_back(ReductionEnum(SAMPSTDEV, "SAMPSTDEV", "the sample standard deviation (N-1 denominator)"));
    enumData.push_back(ReductionEnum(VARIANCE, "VARIANCE", "the variance of the data"));
    enumData.push_back(ReductionEnum(TSNR, "TSNR", "mean divided by sample standard deviation (N-1 denominator)"));
    enumData.push_back(ReductionEnum(COV, "COV", "sample standard deviation (N-1 denominator) divided by mean"));
    enumData.push_back(ReductionEnum(L2NORM, "L2NORM", "square root of sum of squares"));
    enumData.push_back(ReductionEnum(MEDIAN, "MEDIAN", "the median of the data"));
    enumData.push_back(ReductionEnum(MODE, "MODE", "the mode of the data"));
    enumData.push_back(ReductionEnum(COUNT_NONZERO, "COUNT_NONZERO", "the number of nonzero elements in the data"));
}

/**
 * Find the data for and enumerated value.
 * @param enumValue
 *     The enumerated value.
 * @return Pointer to data for this enumerated type
 * or NULL if no data for type or if type is invalid.
 */
const ReductionEnum*
ReductionEnum::findData(const Enum enumValue)
{
    if (initializedFlag == false) initialize();

    size_t num = enumData.size();
    for (size_t i = 0; i < num; i++) {
        const ReductionEnum* d = &enumData[i];
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
ReductionEnum::toName(Enum enumValue) {
    if (initializedFlag == false) initialize();
    
    const ReductionEnum* enumInstance = findData(enumValue);
    if (enumInstance == NULL) return "";
    return enumInstance->name;
}

/**
 * Get a string explaining the enumerated type.
 * @param enumValue 
 *     Enumerated value.
 * @return 
 *     String representing enumerated value.
 */
AString 
ReductionEnum::toExplanation(Enum enumValue) {
    if (initializedFlag == false) initialize();
    
    const ReductionEnum* enumInstance = findData(enumValue);
    if (enumInstance == NULL) return "";
    return enumInstance->explanation;
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
ReductionEnum::Enum 
ReductionEnum::fromName(const AString& name, bool* isValidOut)
{
    if (initializedFlag == false) initialize();
    
    bool validFlag = false;
    Enum enumValue = INVALID;
    
    for (std::vector<ReductionEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const ReductionEnum& d = *iter;
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
        CaretAssertMessage(0, AString("Name " + name + "failed to match enumerated value for type ReductionEnum"));
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
ReductionEnum::getAllEnums(std::vector<ReductionEnum::Enum>& allEnums)
{
    if (initializedFlag == false) initialize();
    
    allEnums.clear();
    
    for (std::vector<ReductionEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        allEnums.push_back(iter->enumValue);
    }
}
