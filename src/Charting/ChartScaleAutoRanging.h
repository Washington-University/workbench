#ifndef __CHART_SCALE_AUTO_RANGING_H__
#define __CHART_SCALE_AUTO_RANGING_H__

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


#include "CaretObject.h"



namespace caret {

    class ChartScaleAutoRanging : public CaretObject {
        
    public:
        static void createAutoScale(const double minimumValueIn,
                                    const double maximumValueIn,
                                    double& scaleMinimumOut,
                                    double& scaleMaximumOut,
                                    double& scaleStepOut,
                                    int32_t& scaleDigitsRightOfDecimalOut);
        
//        static void adjustAxisDefaultRange(float& minValueInOut,
//                                           float& maxValueInOut);
        
        
    private:
        ChartScaleAutoRanging();
        
        virtual ~ChartScaleAutoRanging();
        
        ChartScaleAutoRanging(const ChartScaleAutoRanging&);
        
        ChartScaleAutoRanging& operator=(const ChartScaleAutoRanging&);
        
    public:
        
        // ADD_NEW_METHODS_HERE
        
        virtual AString toString() const;
        
    private:
        static void graphicsGemsHeckbertAutoScale(const double minimumValue,
                                                  const double maximumValue,
                                                  double& scaleMinimumOut,
                                                  double& scaleMaximumOut,
                                                  double& scaleStepOut,
                                                  int32_t& scaleDigitsRightOfDecimalOut);
        
        
        
//        static double adjustValueMagnitude(const double valueIn,
//                                           const double rangeIn,
//                                           const bool increaseMagnitudeFlag);
//        
        static bool isZero(const double value);
        
//        static double adjustValueUp(double valueIn,
//                                    const double range);
//        
//        static double adjustValueDown(double valueIn,
//                                      const double range);
        
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __CHART_SCALE_AUTO_RANGING_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __CHART_SCALE_AUTO_RANGING_DECLARE__

} // namespace
#endif  //__CHART_SCALE_AUTO_RANGING_H__
