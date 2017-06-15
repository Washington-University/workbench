
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

#define __EVENT_CHART_TWO_SHOW_LINE_SERIES_HISTORY_DIALOG_DECLARE__
#include "EventChartTwoShowLineSeriesHistoryDialog.h"
#undef __EVENT_CHART_TWO_SHOW_LINE_SERIES_HISTORY_DIALOG_DECLARE__

#include "CaretAssert.h"
#include "EventTypeEnum.h"

using namespace caret;


    
/**
 * \class caret::EventChartTwoShowLineSeriesHistoryDialog 
 * \brief Event to show the chart two line series history dialog
 * \ingroup GuiQt
 */

/**
 * Constructor.
 * 
 * @param mode
 *     Mode for the dialog.
 * @param browserWindowIndex
 *     Index of the browser window.
 * @param chartOverlay
 *     The chart overlay.
 */
EventChartTwoShowLineSeriesHistoryDialog::EventChartTwoShowLineSeriesHistoryDialog(const Mode mode,
                                                                                   const int32_t browserWindowIndex,
                                                                                   ChartTwoOverlay* chartOverlay)
: Event(EventTypeEnum::EVENT_CHART_TWO_SHOW_LINE_SERIES_HISTORY_DIALOG),
m_mode(mode),
m_browserWindowIndex(browserWindowIndex),
m_chartOverlay(chartOverlay)
{
    
}

/**
 * Destructor.
 */
EventChartTwoShowLineSeriesHistoryDialog::~EventChartTwoShowLineSeriesHistoryDialog()
{
}

/**
 * @return The mode.
 */
EventChartTwoShowLineSeriesHistoryDialog::Mode
EventChartTwoShowLineSeriesHistoryDialog::getMode() const
{
    return m_mode;
}

/**
 * Index of the browser window.
 */
int32_t
EventChartTwoShowLineSeriesHistoryDialog::getBrowserWindowIndex() const
{
    return m_browserWindowIndex;
}

/**
 * @return The chart overlay.
 */
ChartTwoOverlay*
EventChartTwoShowLineSeriesHistoryDialog::getChartOverlay() const
{
    return m_chartOverlay;
}
