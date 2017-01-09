
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

#define __CHART_MODEL_FREQUENCY_SERIES_DECLARE__
#include "ChartModelFrequencySeries.h"
#undef __CHART_MODEL_FREQUENCY_SERIES_DECLARE__

#include "CaretAssert.h"
using namespace caret;


    
/**
 * \class caret::ChartModelFrequencySeries 
 * \brief Chart model for frequency series charts.
 * \ingroup Charting
 */

/**
 * Constructor.
 */
ChartModelFrequencySeries::ChartModelFrequencySeries()
: ChartModelCartesian(ChartOneDataTypeEnum::CHART_DATA_TYPE_LINE_FREQUENCY_SERIES,
                      ChartAxisUnitsEnum::CHART_AXIS_UNITS_FREQUENCY_HERTZ,
                      ChartAxisUnitsEnum::CHART_AXIS_UNITS_NONE)
{
    
}

/**
 * Destructor.
 */
ChartModelFrequencySeries::~ChartModelFrequencySeries()
{
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
ChartModelFrequencySeries::ChartModelFrequencySeries(const ChartModelFrequencySeries& obj)
: ChartModelCartesian(obj)
{
    this->copyHelperChartModelFrequencySeries(obj);
}

/**
 * Assignment operator.
 * @param obj
 *    Data copied from obj to this.
 * @return 
 *    Reference to this object.
 */
ChartModelFrequencySeries&
ChartModelFrequencySeries::operator=(const ChartModelFrequencySeries& obj)
{
    if (this != &obj) {
        ChartModelCartesian::operator=(obj);
        this->copyHelperChartModelFrequencySeries(obj);
    }
    return *this;    
}

/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void 
ChartModelFrequencySeries::copyHelperChartModelFrequencySeries(const ChartModelFrequencySeries& /*obj*/)
{
    
}

