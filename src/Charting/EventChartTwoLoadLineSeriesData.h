#ifndef __EVENT_CHART_TWO_LOAD_LINE_SERIES_DATA_H__
#define __EVENT_CHART_TWO_LOAD_LINE_SERIES_DATA_H__

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

#include "Event.h"
#include "MapFileDataSelector.h"
#include "StructureEnum.h"


namespace caret {

    class EventChartTwoLoadLineSeriesData : public Event {
        
    public:
        EventChartTwoLoadLineSeriesData(const std::vector<int32_t>& validTabIndices,
                                        const MapFileDataSelector& mapFileDataSelector);
//        EventChartTwoLoadLineSeriesData(const std::vector<int32_t>& validTabIndices,
//                                        const MapFileDataSelector& mapFileDataSelector);
        
        virtual ~EventChartTwoLoadLineSeriesData();

        MapFileDataSelector getMapFileDataSelector() const;

        std::vector<int32_t> getValidTabIndices() const;
        
        // ADD_NEW_METHODS_HERE

    private:
        EventChartTwoLoadLineSeriesData(const EventChartTwoLoadLineSeriesData&);

        EventChartTwoLoadLineSeriesData& operator=(const EventChartTwoLoadLineSeriesData&);
        
        const std::vector<int32_t> m_validTabIndices;
        
        MapFileDataSelector m_mapFileDataSelector;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __EVENT_CHART_TWO_LOAD_LINE_SERIES_DATA_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __EVENT_CHART_TWO_LOAD_LINE_SERIES_DATA_DECLARE__

} // namespace
#endif  //__EVENT_CHART_TWO_LOAD_LINE_SERIES_DATA_H__
