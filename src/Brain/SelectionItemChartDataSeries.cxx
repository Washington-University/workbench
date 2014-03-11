
/*LICENSE_START*/
/*
 * Copyright 2014 Washington University,
 * All rights reserved.
 *
 * Connectome DB and Connectome Workbench are part of the integrated Connectome 
 * Informatics Platform.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    * Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *    * Neither the names of Washington University nor the
 *      names of its contributors may be used to endorse or promote products
 *      derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR  
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
 * SUBSTITUTE GOODS OR  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
 * POSSIBILITY OF SUCH DAMAGE.
 */
/*LICENSE_END*/

#define __SELECTION_ITEM_CHART_DATA_SERIES_DECLARE__
#include "SelectionItemChartDataSeries.h"
#undef __SELECTION_ITEM_CHART_DATA_SERIES_DECLARE__

#include "CaretAssert.h"
using namespace caret;


    
/**
 * \class caret::SelectionItemChartDataSeries 
 * \brief Contains selection of a data-series chart.
 * \ingroup Brain
 */

/**
 * Constructor.
 */
SelectionItemChartDataSeries::SelectionItemChartDataSeries()
: SelectionItem(SelectionItemDataTypeEnum::CHART_DATA_SERIES)
{
    m_chartModelDataSeries = NULL;
    m_chartDataCartesian  = NULL;
    m_chartDataPointIndex = -1;
}

/**
 * Destructor.
 */
SelectionItemChartDataSeries::~SelectionItemChartDataSeries()
{
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
SelectionItemChartDataSeries::SelectionItemChartDataSeries(const SelectionItemChartDataSeries& obj)
: SelectionItem(obj)
{
    this->copyHelperSelectionItemChartDataSeries(obj);
}

/**
 * Assignment operator.
 * @param obj
 *    Data copied from obj to this.
 * @return 
 *    Reference to this object.
 */
SelectionItemChartDataSeries&
SelectionItemChartDataSeries::operator=(const SelectionItemChartDataSeries& obj)
{
    if (this != &obj) {
        SelectionItem::operator=(obj);
        this->copyHelperSelectionItemChartDataSeries(obj);
    }
    return *this;    
}

/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void 
SelectionItemChartDataSeries::copyHelperSelectionItemChartDataSeries(const SelectionItemChartDataSeries& obj)
{
    m_chartModelDataSeries = obj.m_chartModelDataSeries;
    m_chartDataCartesian  = obj.m_chartDataCartesian;
    m_chartDataPointIndex = obj.m_chartDataPointIndex;
}

/**
 * @return True if the selected chart is valid, else false.
 */
bool
SelectionItemChartDataSeries::isValid() const
{
    if ((m_chartModelDataSeries != NULL)
        && (m_chartDataCartesian != NULL)
        && (m_chartDataPointIndex >= 0)) {
        return true;
    }
    
    return false;
}

/**
 * Reset the selections.
 */
void
SelectionItemChartDataSeries::reset()
{
    m_chartModelDataSeries = NULL;
    m_chartDataCartesian  = NULL;
    m_chartDataPointIndex = -1;
}

/**
 * @return
 */
ChartModelDataSeries*
SelectionItemChartDataSeries::getChartModelDataSeries() const
{
    return m_chartModelDataSeries;
}

/**
 * @return
 */
ChartDataCartesian*
SelectionItemChartDataSeries::getChartDataCartesian() const
{
    return m_chartDataCartesian;
}

/**
 * @return
 */
int32_t
SelectionItemChartDataSeries::getChartDataPointIndex() const
{
    return m_chartDataPointIndex;
}

/**
 * Set the selected chart information.
 *
 * @param chartModelDataSeries
 *     Data series chart model that was selected.
 * @param chartDataCartesian
 *     Cartesian chart data that was selected.
 * @param chartDataPointIndex
 *     Point index of selected chart data.
 */
void
SelectionItemChartDataSeries::setChart(ChartModelDataSeries* chartModelDataSeries,
                                       ChartDataCartesian* chartDataCartesian,
                                       const int32_t chartDataPointIndex)
{
    CaretAssert(chartModelDataSeries);
    CaretAssert(chartDataCartesian);
    CaretAssert(chartDataPointIndex >= 0);

    m_chartModelDataSeries = chartModelDataSeries;
    m_chartDataCartesian   = chartDataCartesian;
    m_chartDataPointIndex  = chartDataPointIndex;
}



