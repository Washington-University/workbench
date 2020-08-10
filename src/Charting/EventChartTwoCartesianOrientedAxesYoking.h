#ifndef __EVENT_CHART_TWO_CARTESIAN_ORIENTED_AXES_YOKING_H__
#define __EVENT_CHART_TWO_CARTESIAN_ORIENTED_AXES_YOKING_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2020 Washington University School of Medicine
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



#include <memory>

#include "ChartTwoAxisOrientationTypeEnum.h"
#include "ChartTwoAxisScaleRangeModeEnum.h"
#include "Event.h"



namespace caret {

    class ChartTwoCartesianOrientedAxes;
    
    class EventChartTwoCartesianOrientedAxesYoking : public Event {

    public:
        enum class Mode {
            GET_YOKED_AXES,
            GET_MINIMUM_AND_MAXIMUM_VALUES,
            SET_MINIMUM_AND_MAXIMUM_VALUES,
            SET_MINIMUM_VALUE,
            SET_MAXIMUM_VALUE
        };

        static std::vector<ChartTwoCartesianOrientedAxes*> getYokedAxes(const ChartTwoAxisOrientationTypeEnum::Enum axisOrientation,
                                                                        const ChartTwoAxisScaleRangeModeEnum::Enum yokingRangeMode);
        
        static bool getDataRangeMinMaxValues(const ChartTwoAxisOrientationTypeEnum::Enum axisOrientation,
                                             const ChartTwoAxisScaleRangeModeEnum::Enum yokingRangeMode,
                                             float& dataMinimumValueOut,
                                             float& dataMaximumValueOut);

        static bool getMinMaxValues(const ChartTwoAxisOrientationTypeEnum::Enum axisOrientation,
                                    const ChartTwoAxisScaleRangeModeEnum::Enum yokingRangeMode,
                                    float& minimumValueOut,
                                    float& maximumValueOut);
        
        static void setMinMaxValues(const ChartTwoAxisOrientationTypeEnum::Enum axisOrientation,
                                    const ChartTwoAxisScaleRangeModeEnum::Enum yokingRangeMode,
                                    const float minimumValue,
                                    const float maximumValue);

        static void setMinimumValue(const ChartTwoAxisOrientationTypeEnum::Enum axisOrientation,
                                    const ChartTwoAxisScaleRangeModeEnum::Enum yokingRangeMode,
                                    const float minimumValue);
        
        static void setMaximumValue(const ChartTwoAxisOrientationTypeEnum::Enum axisOrientation,
                                    const ChartTwoAxisScaleRangeModeEnum::Enum yokingRangeMode,
                                    const float maximumValue);
        
        virtual ~EventChartTwoCartesianOrientedAxesYoking();
        
        EventChartTwoCartesianOrientedAxesYoking(const EventChartTwoCartesianOrientedAxesYoking&) = delete;

        EventChartTwoCartesianOrientedAxesYoking& operator=(const EventChartTwoCartesianOrientedAxesYoking&) = delete;
        
        Mode getMode() const;

        ChartTwoAxisOrientationTypeEnum::Enum getAxisOrientation() const;
        
        ChartTwoAxisScaleRangeModeEnum::Enum getYokingRangeMode() const;
        
        float getMinimumValue() const;
        
        float getMaximumValue() const;
        
        bool getMinimumAndMaximumValues(float& minimumValueOut,
                                        float& maximumValueOut) const;
        
        void setMinimumAndMaximumValues(const float minimumValue,
                                        const float maximumValue);

        void addYokedAxes(ChartTwoCartesianOrientedAxes* axes);
        
        std::vector<ChartTwoCartesianOrientedAxes*> getYokedAxes() const;

        // ADD_NEW_METHODS_HERE

    private:
        EventChartTwoCartesianOrientedAxesYoking(const Mode mode,
                                                 const ChartTwoAxisOrientationTypeEnum::Enum axisOrientation,
                                                 const ChartTwoAxisScaleRangeModeEnum::Enum yokingRangeMode);
        
        const Mode m_mode;
        
        const ChartTwoAxisOrientationTypeEnum::Enum m_axisOrientation;
        
        const ChartTwoAxisScaleRangeModeEnum::Enum m_yokingRangeMode;
        
        std::vector<ChartTwoCartesianOrientedAxes*> m_yokedAxes;
        
        float m_minimumValue = 0.0;
        
        float m_maximumValue = 0.0;
        
        bool m_valuesValid = false;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __EVENT_CHART_TWO_CARTESIAN_ORIENTED_AXES_YOKING_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __EVENT_CHART_TWO_CARTESIAN_ORIENTED_AXES_YOKING_DECLARE__

} // namespace
#endif  //__EVENT_CHART_TWO_CARTESIAN_ORIENTED_AXES_YOKING_H__
