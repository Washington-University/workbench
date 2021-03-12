
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

#include <QRegularExpression>

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
 * @param numericFormat
 *    How to format the numbers when converted to text.
 * @param textValueIn
 *     Text representation of the value.
 * @reutrn
 *     Value after cleanup of zeros.
 */
AString
NumericTextFormatting::removeLeadingZeroFromExponent(const NumericFormatModeEnum::Enum numericFormat,
                                                     const AString& textValueIn)
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
    static QRegularExpression zeroRegExp("^0\\.0+e[\\+-]0+$");
    
    if (eIndex > 0) {
        if (textValue.indexOf(zeroRegExp) >= 0) {
            textValue = "0";
        }
        else {
            AString eText = textValue.mid(eIndex, 2);
            AString mantissaText = textValue.left(eIndex);
            AString exponentText = textValue.mid(eIndex + 2);
            
            switch (numericFormat) {
                case NumericFormatModeEnum::AUTO:
                    /*
                     * Remove trailing zeros from mantissa
                     */
                    mantissaText = cleanZerosInValueText(mantissaText);
                    break;
                case NumericFormatModeEnum::DECIMAL:
                    break;
                case NumericFormatModeEnum::SCIENTIFIC:
                    break;
            }
            
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
 * @param numericFormat
 *    How to format the numbers when converted to text.
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
NumericTextFormatting::formatNumberForDisplay(const NumericFormatModeEnum::Enum numericFormat,
                                              const double value,
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
    
    numberValue = removeLeadingZeroFromExponent(numericFormat,
                                                numberValue);
    
    AString textValue;
    if (value < 0.0) {
        textValue = "-";
    }
    textValue += numberValue;
    
    if ( ! textValue.contains('e')) {
        switch (numericFormat) {
            case NumericFormatModeEnum::AUTO:
                textValue = cleanZerosInValueText(textValue);
                break;
            case NumericFormatModeEnum::DECIMAL:
                break;
            case NumericFormatModeEnum::SCIENTIFIC:
                break;
        }
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
 * Format the negative and positive values by using the value of the range (min to max)
 * to set format and precision for all values.
 *
 * @param numericFormatIn
 *    How to format the numbers when converted to text.
 * @param numericFormatPrecisionIn
 *    Precision used the numeric format is not automatic.
 * @param negativeValuesIn
 *    The negative input values array.  Must be at least two elements and
 *    the values must be sorted from smallest to largest.
 * @param negativeFormattedValuesOut
 *    Output containing negativevalues formatted as text.
 * @param negativeBumberOfValues
 *    Number of negativevalues in the arrays and both the input and output
 *    arrays must sized to this value.
 * @param positiveValuesIn
 *    The positive input values array.  Must be at least two elements and
 *    the values must be sorted from smallest to largest.
 * @param positiveFormattedValuesOut
 *    Output containing positive values formatted as text.
 * @param positiveNumberOfValues
 *    Number of positive values in the arrays and both the input and output
 *    arrays must sized to this value.
 */
void
NumericTextFormatting::formatValueRangeNegPos(const NumericFormatModeEnum::Enum numericFormatIn,
                                              const int32_t numericFormatPrecisionIn,
                                              const float negativeValuesIn[],
                                              AString negativeFormattedValuesOut[],
                                              const int32_t negativeNumberOfValues,
                                              const float positiveValuesIn[],
                                              AString positiveFormattedValuesOut[],
                                              const int32_t positiveNumberOfValues)
{
    NumericFormatModeEnum::Enum numericFormat = numericFormatIn;
    int32_t numericFormatPrecision = numericFormatPrecisionIn;
    
    
    if (negativeNumberOfValues > 0) {
        formatValueRange(numericFormat,
                         numericFormatPrecision,
                         negativeValuesIn,
                         negativeFormattedValuesOut,
                         negativeNumberOfValues);
    }

    if (positiveNumberOfValues > 0) {
        formatValueRange(numericFormat,
                         numericFormatPrecision,
                         positiveValuesIn,
                         positiveFormattedValuesOut,
                         positiveNumberOfValues);
    }
    
    switch (numericFormat) {
        case NumericFormatModeEnum::AUTO:
            removeDotZeroIfAllIntegers(negativeFormattedValuesOut,
                                       negativeNumberOfValues,
                                       positiveFormattedValuesOut,
                                       positiveNumberOfValues);
            break;
        case NumericFormatModeEnum::DECIMAL:
            break;
        case NumericFormatModeEnum::SCIENTIFIC:
            break;
    }
}

/**
 * If all of the positive and negative text values represent integers
 * (contain no decimal or end with decimal followed by zeros),
 * strip off decimal and trailing zeros.
 *
 * @param negativeFormattedValues
 *    Negative values formatted as text.
 * @param negativeBumberOfValues
 *    Number of negative values.
 * @param positiveFormattedValues
 *    Positive values formatted as text.
 * @param positiveNumberOfValues
 *    Number of positive values.
 */
void
NumericTextFormatting::removeDotZeroIfAllIntegers(AString negativeFormattedValues[],
                                                  const int32_t negativeNumberOfValues,
                                                  AString positiveFormattedValues[],
                                                  const int32_t positiveNumberOfValues)
{
    std::vector<AString> allValues(negativeFormattedValues,
                                   negativeFormattedValues + negativeNumberOfValues);
    allValues.insert(allValues.end(),
                     positiveFormattedValues,
                     positiveFormattedValues + positiveNumberOfValues);
    
    for (auto& text: allValues) {
        const int32_t len = text.length();
        
        /*
         * Does this text end with a decimal followed by all zeros
         */
        const int32_t decimalIndex = text.indexOf('.');
        if (decimalIndex >= 0) {
            for (int32_t j = decimalIndex + 1; j < len; j++) {
                if (text[j] != '0') {
                    /*
                     * Non-zero trailing value so not integer value
                     */
                    return;
                }
            }
            
            if (decimalIndex >= 0) {
                /*
                 * Chop off decimal and zeros 
                 * "text" is a reference so changes value in allValues vector
                 */
                text.resize(decimalIndex);
            }
            else {
                /* if no decimal found, then value is already integer */
            }
        }
    }
    
    for (int32_t i = 0; i < negativeNumberOfValues; i++) {
        CaretAssertArrayIndex(negativeFormattedValues, negativeNumberOfValues, i);
        CaretAssertVectorIndex(allValues, i);
        negativeFormattedValues[i] = allValues[i];
    }
    
    for (int32_t i = 0; i < positiveNumberOfValues; i++) {
        CaretAssertArrayIndex(positiveFormattedValues, positiveNumberOfValues, i);
        const int32_t j = i + negativeNumberOfValues;
        CaretAssertVectorIndex(allValues, j);
        positiveFormattedValues[i] = allValues[j];
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
        
        AString textValue = formatNumberForDisplay(numericFormat,
                                                   value,
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
    
    AString textValue = formatNumberForDisplay(NumericFormatModeEnum::AUTO,
                                               value,
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

