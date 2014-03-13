#ifndef __CHART_SCALE_AUTO_RANGING_H__
#define __CHART_SCALE_AUTO_RANGING_H__

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
