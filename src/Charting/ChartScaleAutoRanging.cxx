
/*LICENSE_START*/
/*
 * Copyright 2014 Washington University,
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
#include <cmath>

#define __CHART_SCALE_AUTO_RANGING_DECLARE__
#include "ChartScaleAutoRanging.h"
#undef __CHART_SCALE_AUTO_RANGING_DECLARE__

#include "CaretAssert.h"
#include "CaretLogger.h"

using namespace caret;


    
/**
 * \class caret::ChartScaleAutoRanging 
 * \brief Adjust minimum and maximum value for auto ranging.
 * \ingroup Charting
 */

/**
 * Constructor.
 */
ChartScaleAutoRanging::ChartScaleAutoRanging()
: CaretObject()
{
    
}

/**
 * Destructor.
 */
ChartScaleAutoRanging::~ChartScaleAutoRanging()
{
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
ChartScaleAutoRanging::toString() const
{
    return "ChartScaleAutoRanging";
}

static const double smallNegValue = -1.0e-6;
static const double smallPosValue =  1.0e-6;

/**
 * Is the value roughly zero?
 *
 * @param value
 *     The value
 * @return 
 *     True if value is roughly zero, else false.
 */
bool
ChartScaleAutoRanging::isZero(const double value)
{
    if ((value >= smallNegValue)
        && (value < smallPosValue)) {
        return true;
    }
    
    return false;
}

///**
// * Adjust the magnitude of a value so that it is an integral
// * value that is more/less positive (if positive) or more/less negative
// * (if negative).
// * 
// * @param valueIn
// *    The input value.
// * @param rangeIn
// *    Range between minimum and maximum values.
// * @param increaseMagnitudeFlag
// *    True if increasing value, else false.
// * @return
// *    The adjusted value.
// */
//double
//ChartScaleAutoRanging::adjustValueMagnitude(const double valueIn,
//                     const double rangeIn,
//                     const bool increaseMagnitudeFlag)
//{
//    double outputValue = valueIn;
//    double range = rangeIn;
//    
//    if (isZero(valueIn)) {
//        outputValue = 0.0;
//    }
//    else if (isZero(range)) {
//        outputValue = valueIn;
//    }
//    else {
//        double value = valueIn;
//        double scaleAmount = 1.0;
//        
//        /*
//         * If value is less than one scale it up to be
//         * greater than or equal to one
//         */
//        if (range < 1.0) {
//            double originalRange = range;
//            int32_t counter = 0;
//            while (range < 1.0) {
//                scaleAmount *= 10.0;
//                range = originalRange * scaleAmount;
//                counter++;
//                if (counter > 10) {
//                    /*
//                     * Should never happen but don't get stuck in loop
//                     */
//                    break;
//                }
//            }
//            
//            value *= scaleAmount;
//        }
//        
//        /*
//         * Only works for positive values
//         * Will be converted back to negative later
//         */
//        bool flipFlag = false;
//        if (value < 0.0) {
//            flipFlag = true;
//            value = -value;
//        }
//        
//        /*
//         * Determine size of value (10s, 100s, 1000s, etc)
//         */
//        const double rangeLog10 = std::log10(range);
//        double logLessOne = std::floor(rangeLog10);
//        if (logLessOne >= 1) {
//            --logLessOne;
//        }
//        else {
//            logLessOne = 0.0;
//        }
//        
//        const int64_t intLogValue = std::pow(10.0, logLessOne);
//        
//        /*
//         * Adust the value
//         */
//        double adjValue = (increaseMagnitudeFlag
//                           ? (value + intLogValue)
//                           : (value - intLogValue));
//        
//        bool addSmallPercentFlag = true;
//        if (addSmallPercentFlag) {
//            const double percentAmount = range * 0.05;
//            adjValue = (increaseMagnitudeFlag
//                        ? (adjValue + percentAmount)
//                        : (adjValue - percentAmount));
//        }
//        
//        double intValue = (int64_t)adjValue;
//        if (intLogValue >= 1) {
//            intValue = (int64_t)adjValue / intLogValue;
//        }
//        
//        double newValue = intValue * intLogValue;
//        
//        if (flipFlag) {
//            newValue = -newValue;
//        }
//        
//        outputValue = newValue / scaleAmount;
//    }
//    
//    return outputValue;
//}
//
///**
// * Increase the value.
// *
// * @param valueIn
// *    The input value.
// * @param range
// *    Range between minimum and maximum values.
// * @return
// *    The adjusted value.
// */
//double
//ChartScaleAutoRanging::adjustValueUp(double valueIn,
//              const double range)
//{
//    return adjustValueMagnitude(valueIn, range, true);
//}
//
///**
// * Decrease the value.
// *
// * @param valueIn
// *    The input value.
// * @param range
// *    Range between minimum and maximum values.
// * @return
// *    The adjusted value.
// */
//double
//ChartScaleAutoRanging::adjustValueDown(double valueIn,
//                const double range)
//{
//    return adjustValueMagnitude(valueIn, range, false);
//}
//
///**
// * Adjust the data min/max values to that they extend a little
// * beyond the rannge of the data but at an integral value.
// *
// * @param minValueInOut
// *    Minimum value that is adjusted.
// * @param maxValueInOut
// *    Maximum value that is adjusted.
// */
//void
//ChartScaleAutoRanging::adjustAxisDefaultRange(float& minValueInOut,
//                                              float& maxValueInOut)
//{
//    double maxValue = maxValueInOut;
//    double minValue = minValueInOut;
//    
//    /*
//     * Handle instance where max value is greater than
//     * min value.
//     */
//    bool invertedRangeFlag = false;
//    double dataRange = maxValue - minValue;
//    if (dataRange < 0.0) {
//        std::swap(minValue, maxValue);
//        dataRange = -dataRange;
//        invertedRangeFlag = true;
//    }
//    
//    if (maxValue > 0) {
//        maxValue = adjustValueUp(maxValue, dataRange);
//    }
//    else if (maxValue < 0.0) {
//        maxValue = adjustValueDown(maxValue, dataRange);
//    }
//    
//    if (minValue > 0) {
//        minValue = adjustValueDown(minValue, dataRange);
//    }
//    else if (minValue < 0.0) {
//        minValue = adjustValueUp(minValue, dataRange);
//    }
//    
//    CaretLogFine("Range was ("
//                 + AString::number(minValueInOut)
//                 + ", "
//                 + AString::number(maxValueInOut)
//                 + ") now ("
//                 + AString::number(minValue)
//                 + ", "
//                 + AString::number(maxValue)
//                 + ")");
//    
//    if (invertedRangeFlag) {
//        minValueInOut = maxValue;
//        maxValueInOut = minValue;
//    }
//    else {
//        minValueInOut = minValue;
//        maxValueInOut = maxValue;
//    }
//}


//#ifdef POW_NOT_TRUSTWORTHY
///* if roundoff errors in pow cause problems, use this: */
//
//double expt(a, n)
//double a;
//register int n;
//{
//    double x;
//    
//    x = 1.;
//    if (n>0) for (; n>0; n--) x *= a;
//        else for (; n<0; n++) x /= a;
//            return x;
//}
//
//#else
//#   define expt(a, n) pow(a, (double)(n))
//#endif

static double
expt(double a,
     int32_t n)
{
    return std::pow(a, static_cast<double>(n));
}

/*
 * nicenum: find a "nice" number approximately equal to x.
 * Round the number if round=1, take ceiling if round=0
 */
static double
nicenum(double x, int round)
{
    int expv;                           /* exponent of x */
    double f;                           /* fractional part of x */
    double nf;                          /* nice, rounded fraction */
    
    expv = std::floor(std::log10(x));
    f = x/expt(10., expv);              /* between 1 and 10 */
    if (round)
        if (f<1.5) nf = 1.;
        else if (f<3.) nf = 2.;
        else if (f<7.) nf = 5.;
        else nf = 10.;
        else
            if (f<=1.) nf = 1.;
            else if (f<=2.) nf = 2.;
            else if (f<=5.) nf = 5.;
            else nf = 10.;
    return nf*expt(10., expv);
}

//#define NTICK 5                 /* desired number of tick marks */

//void
//loose_label(double min, double max)
//{
//    char str[6], temp[20];
//    int nfrac;
//    double d;                           /* tick mark spacing */
//    double graphmin, graphmax;          /* graph range min and max */
//    double range, x;
//    
//    /* we expect min!=max */
//    range = nicenum(max-min, 0);
//    d = nicenum(range/(NTICK-1), 1);
//    graphmin = floor(min/d)*d;
//    graphmax = ceil(max/d)*d;
//    nfrac = std::max(-floor(log10(d)), (double)0);      /* # of fractional digits to show */
//    sprintf(str, "%%.%df", nfrac);      /* simplest axis labels */
//    
//    printf("graphmin=%g graphmax=%g increment=%g\n", graphmin, graphmax, d);
//    for (x=graphmin; x<graphmax+.5*d; x+=d) {
//        sprintf(temp, str, x);
//        printf("(%s)\n", temp);
//    }
//}


/**
 * Nice Numbers for Graph Labels
 * by Paul Heckbert
 * from "Graphics Gems", Academic Press, 1990
 *
 * label.c: demonstrate nice graph labeling
 *
 * Paul Heckbert        2 Dec 88
 *
 * @param minimumValue
 *     Minimum data value.
 * @param maximumValue
 *     Maximum data value.
 * @param scaleMinimumOut
 *     Minimum value for scale output
 * @param scaleMaximumOut
 *     Maximum value for scale output
 * @param scaleStepOut
 *     Step value for scale output
 * @param scaleDigitsRightOfDecimalOut
 *     Digits shown right of decimal point in scale display
 */
void
ChartScaleAutoRanging::graphicsGemsHeckbertAutoScale(const double minimumValue,
                                                     const double maximumValue,
                                                     double& scaleMinimumOut,
                                                     double& scaleMaximumOut,
                                                     double& scaleStepOut,
                                                     int32_t& scaleDigitsRightOfDecimalOut)
{
    const int32_t NUMBER_OF_TICKS = 5;
//    char str[6], temp[20];
    int nfrac;
    double d;                           /* tick mark spacing */
    double graphmin, graphmax;          /* graph range min and max */
    double range; //, x;
    
    /* we expect min!=max */
    range = nicenum(maximumValue - minimumValue, 0);
    d = nicenum(range/(NUMBER_OF_TICKS-1), 1);
    graphmin = std::floor(minimumValue/d)*d;
    graphmax = std::ceil(maximumValue/d)*d;
    nfrac = std::max(-std::floor(log10(d)), (double)0);      /* # of fractional digits to show */
//    sprintf(str, "%%.%df", nfrac);      /* simplest axis labels */
//    
//    printf("graphmin=%g graphmax=%g increment=%g\n", graphmin, graphmax, d);
//    for (x=graphmin; x<graphmax+.5*d; x+=d) {
//        sprintf(temp, str, x);
//        printf("(%s)\n", temp);
//    }
    
    scaleMinimumOut = graphmin;
    scaleMaximumOut = graphmax;
    scaleStepOut    = d;
    scaleDigitsRightOfDecimalOut = nfrac;

    CaretLogFine("MinIn=" + AString::number(minimumValue, 'f', scaleDigitsRightOfDecimalOut)
                   + "  MaxIn=" + AString::number(maximumValue, 'f', scaleDigitsRightOfDecimalOut)
                   + "  MinOut=" + AString::number(scaleMinimumOut, 'f', scaleDigitsRightOfDecimalOut)
                   + "  MaxOut=" + AString::number(scaleMaximumOut, 'f', scaleDigitsRightOfDecimalOut)
                   + "  StepOut=" + AString::number(scaleStepOut, 'f', scaleDigitsRightOfDecimalOut)
                   + "  DigitsRightOfDecimal=" + AString::number(scaleDigitsRightOfDecimalOut));
                   
}


/**
 * Given the data minimum and maximum values, generate reasonable values
 * for automatically scaling the minimum, maximum, and step (tickmark)
 * values for the data.
 *
 * @param minimumValueIn
 *     Minimum data value.
 * @param maximumValueIn
 *     Maximum data value.
 * @param scaleMinimumOut
 *     Minimum value for scale output
 * @param scaleMaximumOut
 *     Maximum value for scale output
 * @param scaleStepOut
 *     Step value for scale output
 * @param scaleDigitsRightOfDecimalOut
 *     Digits shown right of decimal point in scale display
 */
void
ChartScaleAutoRanging::createAutoScale(const double minimumValueIn,
                                       const double maximumValueIn,
                                       double& scaleMinimumOut,
                                       double& scaleMaximumOut,
                                       double& scaleStepOut,
                                       int32_t& scaleDigitsRightOfDecimalOut)
{
    if (isZero(maximumValueIn - minimumValueIn)) {
        scaleMaximumOut = maximumValueIn + 1.0;
        scaleMinimumOut = minimumValueIn - 1.0;
        scaleStepOut = 1.0;
        scaleDigitsRightOfDecimalOut = 1;
        return;
    }
    
    double minimumValue = minimumValueIn;
    double maximumValue = maximumValueIn;

    bool flipFlag = false;
    if (maximumValue < minimumValue) {
        std::swap(minimumValue,
                  maximumValue);
        flipFlag = true;
    }
    
    graphicsGemsHeckbertAutoScale(minimumValue,
                                  maximumValue,
                                  scaleMinimumOut,
                                  scaleMaximumOut,
                                  scaleStepOut,
                                  scaleDigitsRightOfDecimalOut);
    
    if (flipFlag) {
        std::swap(scaleMinimumOut,
                  scaleMaximumOut);
        scaleStepOut = -scaleStepOut;
    }
}







