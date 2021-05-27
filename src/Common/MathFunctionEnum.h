#ifndef __MATH_FUNCTION_ENUM__H_
#define __MATH_FUNCTION_ENUM__H_

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
 * \brief Enumerated type for math functions.
 *
 * Enumerated types for math functions.
 */
class MathFunctionEnum {

public:
    /**
     * Enumerated values.
     */
    enum Enum {
            INVALID,
            SIN,
            COS,
            TAN,
            ASIN,
            ACOS,
            ATAN,
            ATAN2,
            SINH,
            COSH,
            TANH,
            ASINH,
            ACOSH,
            ATANH,
            SINC,
            LN,
            EXP,
            LOG,
            LOG2,
            SQRT,
            ABS,
            FLOOR,
            ROUND,
            CEIL,
            MIN,
            MAX,
            MOD,
            CLAMP
    };

    ~MathFunctionEnum();

    static AString toName(Enum enumValue);
    
    static AString toExplanation(Enum enumValue);
    
    static Enum fromName(const AString& name, bool* isValidOut);
    
    static void getAllEnums(std::vector<Enum>& allEnums);

private:
    MathFunctionEnum(const Enum enumValue, 
                 const AString& name,
                 const AString& explanation);

    static const MathFunctionEnum* findData(const Enum enumValue);

    /** Holds all instance of enum values and associated metadata */
    static std::vector<MathFunctionEnum> enumData;

    /** Initialize instances that contain the enum values and metadata */
    static void initialize();

    /** Indicates instance of enum values and metadata have been initialized */
    static bool initializedFlag;
    
    /** The enumerated type value for an instance */
    Enum enumValue;

    /** The name, a text string that is identical to the enumerated value */
    AString name;
    
    /** An explanation of the function */
    AString explanation;
};

#ifdef __MATH_FUNCTION_ENUM_DECLARE__
std::vector<MathFunctionEnum> MathFunctionEnum::enumData;
bool MathFunctionEnum::initializedFlag = false;
#endif // __MATH_FUNCTION_ENUM_DECLARE__

} // namespace
#endif  //__MATH_FUNCTION_ENUM__H_
