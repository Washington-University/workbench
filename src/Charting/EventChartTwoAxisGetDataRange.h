#ifndef __EVENT_CHART_TWO_AXIS_GET_DATA_RANGE_H__
#define __EVENT_CHART_TWO_AXIS_GET_DATA_RANGE_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2017 Washington University School of Medicine
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

#include "ChartAxisLocationEnum.h"
#include "ChartTwoAxisOrientationTypeEnum.h"
#include "Event.h"

namespace caret {

    class ChartTwoOverlaySet;
    
    class EventChartTwoAxisGetDataRange : public Event {
        
    public:
        enum AxisMode {
            AXIS_LOCATION,
            AXIS_ORIENTATION
        };
        
        EventChartTwoAxisGetDataRange(const ChartTwoOverlaySet* chartOverlaySet,
                                      const ChartAxisLocationEnum::Enum chartAxisLocation);
        
        EventChartTwoAxisGetDataRange(const ChartTwoOverlaySet* chartOverlaySet,
                                      const ChartTwoAxisOrientationTypeEnum::Enum axisOrienationType);
        
        virtual ~EventChartTwoAxisGetDataRange();

        AxisMode getAxisMode() const;
        
        const ChartTwoOverlaySet* getChartOverlaySet() const;
        
        ChartAxisLocationEnum::Enum getChartAxisLocation() const;
        
        ChartTwoAxisOrientationTypeEnum::Enum getChartAxisOrientation() const;
        
        bool getMinimumAndMaximumValues(float& minimumValue,
                                        float& maximumValue) const;
        
        void setMinimumAndMaximumValues(const float minimumValue,
                                        const float maximumValue);

        // ADD_NEW_METHODS_HERE

    private:
        EventChartTwoAxisGetDataRange(const EventChartTwoAxisGetDataRange&);

        EventChartTwoAxisGetDataRange& operator=(const EventChartTwoAxisGetDataRange&);
        
        const AxisMode m_axisMode;
        
        const ChartTwoOverlaySet* m_chartOverlaySet;
        
        const ChartAxisLocationEnum::Enum m_chartAxisLocation;
        
        const ChartTwoAxisOrientationTypeEnum::Enum m_axisOrienationType;
        
        float m_minimumValue = 0.0f;
        
        float m_maximumValue = 0.0f;
        
        bool m_valuesValidFlag = false;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __EVENT_CHART_TWO_AXIS_GET_DATA_RANGE_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __EVENT_CHART_TWO_AXIS_GET_DATA_RANGE_DECLARE__

} // namespace
#endif  //__EVENT_CHART_TWO_AXIS_GET_DATA_RANGE_H__
