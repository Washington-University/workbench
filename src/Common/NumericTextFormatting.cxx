
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

#include <QRegExp>

#define __NUMERIC_TEXT_FORMATTING_DECLARE__
#include "NumericTextFormatting.h"
#undef __NUMERIC_TEXT_FORMATTING_DECLARE__

#include "CaretAssert.h"
#include "CaretLogger.h"

using namespace caret;

#include "MathFunctions.h"
    
/**
 * \class caret::NumericTextFormatting 
 * \brief Formats numeric values for display as text.
 * \ingroup Common
 */

/**
 * Constructor.
 */
NumericTextFormatting::NumericTextFormatting()
: CaretObject()
{
    
}

/**
 * Destructor.
 */
NumericTextFormatting::~NumericTextFormatting()
{
}

/**
 * If there are trailing zeros after the decimal point, remove
 * any after the first zero.  Change a negative zero (-0) to zero (0);
 *
 * @param textValueIn
 *     Text representation of the value.
 * @reutrn
 *     Value after cleanup of zeros.
 */
AString
NumericTextFormatting::cleanZerosInValueText(const AString& textValueIn)
{
    AString textValue = textValueIn;
    
    const int dotIndex = textValue.indexOf('.');
    if (dotIndex > 0) {
        /*
         * Remove any trailing zeros
         */
        for (int32_t i = (textValue.length() - 1); i >= (dotIndex + 2); --i) {
            if (textValue[i] == '0') {
                textValue[i] = ' ';
            }
            else {
                break;
            }
        }
    }
    
    textValue = textValue.trimmed();
    
    if (textValue == "-0.0") {
        textValue = "0";
    }
    else if (textValue == "-0") {
        textValue = "0";
    }
    else if (textValue == "0.0") {
        textValue = "0";
    }
    
    textValue = textValue.trimmed();
    
    return textValue;
}

/**
 * If there is a leading zero in the exponent, remove it
 * to save space (-2.1e+03 => -2.1e+3)
 *
 * @param textValueIn
 *     Text representation of the value.
 * @reutrn
 *     Value after cleanup of zeros.
 */
AString
NumericTextFormatting::removeLeadingZeroFromExponent(const AString& textValueIn)
{
    AString textValue = textValueIn;

    AString ePlusMinus = "";
    int eIndex = textValue.indexOf("e+");
    if (eIndex < 0) {
        eIndex = textValue.indexOf("e-");
    }
    
    /*
     * Regular expression that matches something like 0.000e+00 (zero !!!)
     */
    static QRegExp zeroRegExp("^0\\.0+e[\\+-]0+$");
    
    if (eIndex > 0) {
        if (textValue.indexOf(zeroRegExp) >= 0) {
            textValue = "0";
        }
        else {
            AString eText = textValue.mid(eIndex, 2);
            AString mantissaText = textValue.left(eIndex);
            AString exponentText = textValue.mid(eIndex + 2);
            
            /*
             * Remove trailing zeros from mantissa
             */
            mantissaText = cleanZerosInValueText(mantissaText);
            
            /*
             * Remove leading zeros from exponent and
             * keep last zero
             */
            for (int32_t i = 0; i < (exponentText.length() - 1); i++) {
                if (exponentText[i] == '0') {
                    exponentText[i] = ' ';
                }
                else {
                    break;
                }
            }
            exponentText = exponentText.trimmed();
            
            textValue = (mantissaText
                         + eText
                         + exponentText);

//            std::cout << "   Sci Value "
//            << textValueIn
//            << " Mantissa: " << mantissaText
//            << " e: " << eText
//            << " Exponent: " << exponentText << std::endl;
        }
        
    }
    
    
    return textValue;
}

/**
 * Format a number for display.
 *
 * @param value
 *     The value for formatting.
 * @param format
 *     Formatting to use for number ('f' fixed, 'e' scientific).
 * @param fieldWidth
 *
 * @param precision
 *     Digits right of the decimal point
 * @return
 *     Text representation of number with formatting applied.
 */
AString
NumericTextFormatting::formatNumberForDisplay(const double value,
                                              const char format,
                                              const int fieldWidth,
                                              const int precision)
{
    if (MathFunctions::isNaN(value)) {
        return "NaN";
    }
    else if (MathFunctions::isInf(value)) {
        return "Inf";
    }
    else if (MathFunctions::isNegInf(value)) {
        return "-Inf";
    }
    else if (MathFunctions::isPosInf(value)) {
        return "Inf";
    }
    
    const double absValue = ((value < 0.0) ? -value : value);
    
    
    AString numberValue = QString("%1").arg(absValue,
                                            fieldWidth,
                                            format,
                                            precision);
    
    numberValue = removeLeadingZeroFromExponent(numberValue);
    
    AString textValue;
    if (value < 0.0) {
        textValue = "-";
    }
    textValue += numberValue;
    
    if ( ! textValue.contains('e')) {
        textValue = cleanZerosInValueText(textValue);
    }
    
    return textValue;
}

/**
 * Get format and precision based upon value of the number.
 *
 * @param valueIn
 *    The number.
 * @param formatOut
 *    Output format (fixed 'f' or scientific 'e')
 * @param precisionOut
 *    Number of digits right of decimal point.
 */
void
NumericTextFormatting::getFormatAndPrecision(const float valueIn,
                           char& formatOut,
                           int& precisionOut)
{
    const float value = ((valueIn < 0.0)
                         ? -valueIn
                         : valueIn);
    
    if (value >= 10000) {
        formatOut = 'e';
        precisionOut = 3;
    }
    else if (value >= 100) {
        formatOut = 'f';
        precisionOut = 0;
    }
    else if (value >= 10) {
        formatOut = 'f';
        precisionOut = 1;
    }
    else if (value >= 1) {
        formatOut = 'f';
        precisionOut = 2;
    }
    else if (value >= 0.01) {
        formatOut = 'f';
        precisionOut = 3;
    }
    else {
        formatOut = 'e';
        precisionOut = 3;
    }
}

/**
 * Format the values by using the value of the range (min to max)
 * to set format and precision for all values.
 *
 * @param numericFormat
 *    How to format the numbers when converted to text.
 * @param numericFormatPrecision
 *    Precision used the numeric format is not automatic.
 * @param valuesIn
 *    The input values array.  Must be at least two elements and
 *    the values must be sorted from smallest to largest.
 * @param formattedValuesOut
 *    Output containing values formatted as text.
 * @param numberOfValues
 *    Number of values in the arrays and both the input and output
 *    arrays must sized to this value.
 */
void
NumericTextFormatting::formatValueRange(const NumericFormatModeEnum::Enum numericFormat,
                                        const int32_t numericFormatPrecision,
                                        const float valuesIn[],
                                        AString formattedValuesOut[],
                                        const int32_t numberOfValues)
{
    for (int32_t i = 0; i < numberOfValues; i++) {
        formattedValuesOut[i] = "";
    }
    
    if (numberOfValues < 2) {
        CaretLogSevere("NumericTextFormatting::formatValueRange requires at least two values.");
        return;
    }
    
    const double range = valuesIn[numberOfValues - 1] - valuesIn[0];
    
    if (MathFunctions::isNaN(range)) {
        for (int32_t i = 0; i < numberOfValues; i++) {
            formattedValuesOut[i] = "0";
        }
        formattedValuesOut[0] = "NaN";
        formattedValuesOut[numberOfValues - 1] = "NaN";
        return;
    }
    
    char format    = 'f';
    int  precision = 0;
    switch (numericFormat) {
        case NumericFormatModeEnum::AUTO:
            getFormatAndPrecision(range,
                                  format,
                                  precision);
            break;
        case NumericFormatModeEnum::DECIMAL:
            format = 'f';
            precision = numericFormatPrecision;
            break;
        case NumericFormatModeEnum::SCIENTIFIC:
            format = 'e';
            precision = numericFormatPrecision;
            break;
    }
    
    
    const int FIELD_WIDTH = 0;
    
    for (int32_t i = 0; i < numberOfValues; i++) {
        const double value = valuesIn[i];
        
        AString textValue = formatNumberForDisplay(value,
                                                   format,
                                                   FIELD_WIDTH,
                                                   precision);
        formattedValuesOut[i] = textValue;
    }
}

/**
 * Format the values by using the value of the range (min to max)
 * to set format and precision for all values.
 *
 * @param valuesIn
 *    The input values array.  Must be at least two elements and 
 *    the values must be sorted from smallest to largest.
 * @param formattedValuesOut
 *    Output containing values formatted as text.
 * @param numberOfValues
 *    Number of values in the arrays and both the input and output
 *    arrays must sized to this value.
 */
void
NumericTextFormatting::formatValueRange(const float valuesIn[],
                                        AString formattedValuesOut[],
                                        const int32_t numberOfValues)
{
    NumericTextFormatting::formatValueRange(NumericFormatModeEnum::AUTO,
                                            0,
                                            valuesIn,
                                            formattedValuesOut,
                                            numberOfValues);
}

/**
 * Format the values by formatting the negative and positive
 * ranges separately.
 *
 * @param negMaxNegMinPosMinPosMaxValuesIn
 *    [0] => most negative value
 *    [1] => least negative value
 *    [2] => least positive value
 *    [3] => most positive value
 *    [0] <= [1] <= 0.0 <= [2] <= [3]
 * @param formattedValuesOut
 *    Output containing values formatted as text.
 */
void
NumericTextFormatting::formatValueRangeNegativeAndPositive(const float negMaxNegMinPosMinPosMaxValuesIn[4],
                                                           AString formattedValuesOut[4])
{
    formatValueRange(&negMaxNegMinPosMinPosMaxValuesIn[0],
                     &formattedValuesOut[0],
                     2);
    
    formatValueRange(&negMaxNegMinPosMinPosMaxValuesIn[2],
                     &formattedValuesOut[2],
                     2);
}

/**
 * Format the value.
 *
 * @param valuesn
 *    The input values.
 * @return 
 *    Value formatted as text.
 */
AString
NumericTextFormatting::formatValue(const float value)
{
    char format    = 'f';
    int  precision = 0;
    
    getFormatAndPrecision(value,
                          format,
                          precision);
    
    const int FIELD_WIDTH = 0;
    
    AString textValue = formatNumberForDisplay(value,
                                               format,
                                               FIELD_WIDTH,
                                               precision);
    return textValue;
}

/**
 * Format each of the values.
 *
 * @param valuesIn
 *    The input values array.  
 * @param formattedValuesOut
 *    Output containing values formatted as text.
 * @param numberOfValues
 *    Number of values in the arrays and both the input and output
 *    arrays must sized to this value.
 */
void
NumericTextFormatting::formatValuesIndividually(const float valuesIn[],
                                                AString formattedValuesOut[],
                                                const int32_t numberOfValues)
{
    for (int32_t i = 0; i < numberOfValues; i++) {
        formattedValuesOut[i] = formatValue(valuesIn[i]);
    }
}

