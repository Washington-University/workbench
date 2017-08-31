#ifndef __NUMERIC_TEXT_FORMATTING_H__
#define __NUMERIC_TEXT_FORMATTING_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2014 Washington University School of Medicine
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


#include "CaretObject.h"
#include "NumericFormatModeEnum.h"


namespace caret {
    
    class NumericTextFormatting : public CaretObject {
        
    public:
        static void formatValueRangeNegativeAndPositive(const float negMaxNegMinPosMinPosMaxValuesIn[4],
                                                        AString formattedValuesOut[4]);
        
        static void formatValueRange(const float valuesIn[],
                                     AString formattedValuesOut[],
                                     const int32_t numberOfValues);
        
        static void formatValueRange(const NumericFormatModeEnum::Enum numericFormat,
                                     const int32_t numericFormatPrecision,
                                     const float valuesIn[],
                                     AString formattedValuesOut[],
                                     const int32_t numberOfValues);
        
        static AString formatValue(const float valueIn);
        
        static void formatValuesIndividually(const float valuesIn[],
                                             AString formattedValuesOut[],
                                             const int32_t numberOfValues);
        
        static AString cleanZerosInValueText(const AString& textValueIn);
        
        // ADD_NEW_METHODS_HERE

    private:
        NumericTextFormatting();
        
        virtual ~NumericTextFormatting();
        
        NumericTextFormatting(const NumericTextFormatting&);
        
        NumericTextFormatting& operator=(const NumericTextFormatting&);
        
        static AString removeLeadingZeroFromExponent(const NumericFormatModeEnum::Enum numericFormat,
                                                     const AString& textValueIn);
        
        static AString formatNumberForDisplay(const NumericFormatModeEnum::Enum numericFormat,
                                              const double value,
                                              const char format,
                                              const int fieldWidth,
                                              const int precision);
        
        static void getFormatAndPrecision(const float valueIn,
                                          char& formatOut,
                                          int& precisionOut);
        // ADD_NEW_MEMBERS_HERE
        
    };
    
#ifdef __NUMERIC_TEXT_FORMATTING_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __NUMERIC_TEXT_FORMATTING_DECLARE__
    
} // namespace
#endif  //__NUMERIC_TEXT_FORMATTING_H__
