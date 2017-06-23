#ifndef __EVENT_CHART_TWO_ATTRIBUTES_CHANGED_H__
#define __EVENT_CHART_TWO_ATTRIBUTES_CHANGED_H__

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

#include "ChartTwoDataTypeEnum.h"
#include "Event.h"
#include "YokingGroupEnum.h"


namespace caret {

    class ChartTwoCartesianAxis;
    class EventChartTwoAttributesChanged : public Event {
        
    public:
        enum class Mode {
            INVALID,
            CARTESIAN_AXIS
        };
        
        EventChartTwoAttributesChanged();
        
        ~EventChartTwoAttributesChanged();
        
        Mode getMode() const;
        
        void setCartesianAxisChanged(const YokingGroupEnum::Enum yokingGroup,
                                      const ChartTwoDataTypeEnum::Enum chartTwoDataType,
                                      const ChartTwoCartesianAxis* cartesianAxis);
        
        void getCartesianAxisChanged(YokingGroupEnum::Enum &yokingGroupOut,
                                      ChartTwoDataTypeEnum::Enum &chartTwoDataTypeOut,
                                      ChartTwoCartesianAxis* &cartesianAxisOut);
        
        

        // ADD_NEW_METHODS_HERE

    private:
        EventChartTwoAttributesChanged(const EventChartTwoAttributesChanged&);

        EventChartTwoAttributesChanged& operator=(const EventChartTwoAttributesChanged&);
        
        Mode m_mode = Mode::INVALID;
        
        YokingGroupEnum::Enum m_yokingGroup = YokingGroupEnum::YOKING_GROUP_OFF;
        
        ChartTwoDataTypeEnum::Enum m_chartTwoDataType = ChartTwoDataTypeEnum::CHART_DATA_TYPE_INVALID;
        
        ChartTwoCartesianAxis* m_cartesianAxis = NULL;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __EVENT_CHART_TWO_ATTRIBUTES_CHANGED_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __EVENT_CHART_TWO_ATTRIBUTES_CHANGED_DECLARE__

} // namespace
#endif  //__EVENT_CHART_TWO_ATTRIBUTES_CHANGED_H__
