
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

#define __CHART_MODEL_DATA_SERIES_DECLARE__
#include "ChartModelDataSeries.h"
#undef __CHART_MODEL_DATA_SERIES_DECLARE__

#include "CaretAssert.h"
using namespace caret;


    
/**
 * \class caret::ChartModelDataSeries 
 * \brief Chart model for line series charts.
 * \ingroup Charting
 */

/**
 * Constructor.
 */
ChartModelDataSeries::ChartModelDataSeries()
: ChartModelCartesian(ChartOneDataTypeEnum::CHART_DATA_TYPE_LINE_DATA_SERIES,
                      ChartAxisUnitsEnum::CHART_AXIS_UNITS_NONE,
                      ChartAxisUnitsEnum::CHART_AXIS_UNITS_NONE)
{
    
}

/**
 * Destructor.
 */
ChartModelDataSeries::~ChartModelDataSeries()
{
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
ChartModelDataSeries::ChartModelDataSeries(const ChartModelDataSeries& obj)
: ChartModelCartesian(obj)
{
    this->copyHelperChartModelDataSeries(obj);
}

/**
 * Assignment operator.
 * @param obj
 *    Data copied from obj to this.
 * @return 
 *    Reference to this object.
 */
ChartModelDataSeries&
ChartModelDataSeries::operator=(const ChartModelDataSeries& obj)
{
    if (this != &obj) {
        ChartModelCartesian::operator=(obj);
        this->copyHelperChartModelDataSeries(obj);
    }
    return *this;    
}

/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void 
ChartModelDataSeries::copyHelperChartModelDataSeries(const ChartModelDataSeries& /*obj*/)
{
    
}

