
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

#define __EVENT_CHART_TWO_LOAD_LINE_SERIES_DATA_DECLARE__
#include "EventChartTwoLoadLineSeriesData.h"
#undef __EVENT_CHART_TWO_LOAD_LINE_SERIES_DATA_DECLARE__

#include "CaretAssert.h"
#include "EventTypeEnum.h"

using namespace caret;



/**
 * \class caret::EventChartTwoLoadLineSeriesData
 * \brief Event for loading chart two line series data
 * \ingroup Charting
 */

/**
 * Constructor.
 *
 * @param validTabIndices
 *     Indices of valid tabs.
 * @param mapFileDataSelector
 *     Map file data selector that provides data selection criteria.
 */
EventChartTwoLoadLineSeriesData::EventChartTwoLoadLineSeriesData(const std::vector<int32_t>& validTabIndices,
                                                                 const MapFileDataSelector& mapFileDataSelector)
: Event(EventTypeEnum::EVENT_CHART_TWO_LOAD_LINE_SERIES_DATA),
m_validTabIndices(validTabIndices),
m_mapFileDataSelector(mapFileDataSelector)
{
    
}

/**
 * Destructor.
 */
EventChartTwoLoadLineSeriesData::~EventChartTwoLoadLineSeriesData()
{
}

/**
 * @return The valid tab indices.
 */
std::vector<int32_t>
EventChartTwoLoadLineSeriesData::getValidTabIndices() const
{
    return m_validTabIndices;
}

/**
 * @return Map file data selector that provides data selection criteria.
 */
MapFileDataSelector
EventChartTwoLoadLineSeriesData::getMapFileDataSelector() const
{
    return m_mapFileDataSelector;
}

