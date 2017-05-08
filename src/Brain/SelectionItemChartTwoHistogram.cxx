
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

#define __SELECTION_ITEM_CHART_TWO_HISTOGRAM_DECLARE__
#include "SelectionItemChartTwoHistogram.h"
#undef __SELECTION_ITEM_CHART_TWO_HISTOGRAM_DECLARE__

#include "CaretAssert.h"
using namespace caret;


    
/**
 * \class caret::SelectionItemChartTwoHistogram 
 * \brief Selection of charting two histogram
 * \ingroup Brain
 */

/**
 * Constructor.
 */
SelectionItemChartTwoHistogram::SelectionItemChartTwoHistogram()
: SelectionItem(SelectionItemDataTypeEnum::CHART_TWO_HISTOGRAM)
{
    resetPrivate();
}

/**
 * Destructor.
 */
SelectionItemChartTwoHistogram::~SelectionItemChartTwoHistogram()
{
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
SelectionItemChartTwoHistogram::SelectionItemChartTwoHistogram(const SelectionItemChartTwoHistogram& obj)
: SelectionItem(obj)
{
    this->copyHelperSelectionItemChartTwoHistogram(obj);
}

/**
 * Assignment operator.
 * @param obj
 *    Data copied from obj to this.
 * @return 
 *    Reference to this object.
 */
SelectionItemChartTwoHistogram&
SelectionItemChartTwoHistogram::operator=(const SelectionItemChartTwoHistogram& obj)
{
    if (this != &obj) {
        SelectionItem::operator=(obj);
        this->copyHelperSelectionItemChartTwoHistogram(obj);
    }
    return *this;    
}

/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void 
SelectionItemChartTwoHistogram::copyHelperSelectionItemChartTwoHistogram(const SelectionItemChartTwoHistogram& obj)
{
    m_fileHistogramChart  = obj.m_fileHistogramChart;
    m_mapIndex            = obj.m_mapIndex;
    m_bucketIndex         = obj.m_bucketIndex;
    m_allMapsSelectedFlag = obj.m_allMapsSelectedFlag;
}


ChartableTwoFileHistogramChart*
SelectionItemChartTwoHistogram::getFileHistogramChart() const
{
    return m_fileHistogramChart;
}

/**
 * @return Index of map index selected.  Also
 * should test isAllMapsSelected()
 */
int32_t
SelectionItemChartTwoHistogram::getMapIndex() const
{
    return m_mapIndex;
}

/**
 * @return Index of histogram bucket selected.
 */
int32_t
SelectionItemChartTwoHistogram::getBucketIndex() const
{
    return m_bucketIndex;
}

/**
 * @return Is all maps selected?
 */
bool
SelectionItemChartTwoHistogram::isAllMapsSelected() const
{
    return m_allMapsSelectedFlag;
}


/*
 * Set histogram identification.
 *
 * @param fileHistogramChart
 *     The histogram chart.
 * @param mapIndex
 *     Index of the map whose histogram is displayed (negative indicates all maps).
 * @param bucketIndex
 *     Index of the histogram bucket.
 * @param allMapsSelected
 *     True if all maps selected, else false.
 */
void
SelectionItemChartTwoHistogram::setHistogramChart(ChartableTwoFileHistogramChart* fileHistogramChart,
                                                  const int32_t mapIndex,
                                                  const int32_t bucketIndex,
                                                  const bool allMapsSelected)
{
    m_fileHistogramChart  = fileHistogramChart;
    m_mapIndex            = mapIndex;
    m_bucketIndex         = bucketIndex;
    m_allMapsSelectedFlag = allMapsSelected;
}

/**
 * @return True if the selected chart is valid, else false.
 */
bool
SelectionItemChartTwoHistogram::isValid() const
{
    /*
     * Map index is negative if all maps selected.
     */
    if ((m_fileHistogramChart != NULL)
        && (m_bucketIndex >= 0)
        && ((m_mapIndex >= 0)
            || m_allMapsSelectedFlag)) {
        return true;
    }
    
    return false;
}

/**
 * Reset the selections PRIVATE.
 * Note that reset() is virtual and cannot
 * be called from constructor.
 */
void
SelectionItemChartTwoHistogram::resetPrivate()
{
    m_fileHistogramChart = NULL;
    m_mapIndex = -1;
    m_bucketIndex = -1;
    m_allMapsSelectedFlag = false;
}

/**
 * Reset the selections.
 */
void
SelectionItemChartTwoHistogram::reset()
{
    resetPrivate();
}


