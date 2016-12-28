
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

#define __CHART_MODEL_TIME_SERIES_DECLARE__
#include "ChartModelTimeSeries.h"
#undef __CHART_MODEL_TIME_SERIES_DECLARE__

#include "CaretAssert.h"
using namespace caret;


    
/**
 * \class caret::ChartModelTimeSeries 
 * \brief Chart model for line series charts.
 * \ingroup Charting
 */

/**
 * Constructor.
 */
ChartModelTimeSeries::ChartModelTimeSeries()
: ChartModelCartesian(ChartVersionOneDataTypeEnum::CHART_DATA_TYPE_LINE_TIME_SERIES,
                      ChartAxisUnitsEnum::CHART_AXIS_UNITS_TIME_SECONDS,
                      ChartAxisUnitsEnum::CHART_AXIS_UNITS_NONE)
{
    
}

/**
 * Destructor.
 */
ChartModelTimeSeries::~ChartModelTimeSeries()
{
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
ChartModelTimeSeries::ChartModelTimeSeries(const ChartModelTimeSeries& obj)
: ChartModelCartesian(obj)
{
    this->copyHelperChartModelTimeSeries(obj);
}

/**
 * Assignment operator.
 * @param obj
 *    Data copied from obj to this.
 * @return 
 *    Reference to this object.
 */
ChartModelTimeSeries&
ChartModelTimeSeries::operator=(const ChartModelTimeSeries& obj)
{
    if (this != &obj) {
        ChartModelCartesian::operator=(obj);
        this->copyHelperChartModelTimeSeries(obj);
    }
    return *this;    
}

/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void 
ChartModelTimeSeries::copyHelperChartModelTimeSeries(const ChartModelTimeSeries& /*obj*/)
{
    
}

