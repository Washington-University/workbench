#ifndef __EVENT_CHART_TWO_SHOW_LINE_SERIES_HISTORY_DIALOG_H__
#define __EVENT_CHART_TWO_SHOW_LINE_SERIES_HISTORY_DIALOG_H__

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



namespace caret {

    class CaretMappableDataFile;
    class ChartTwoOverlay;
    
    class EventChartTwoShowLineSeriesHistoryDialog : public Event {
        
    public:
        enum class Mode {
            CHART_OVERLAY_CHANGED,
            SHOW_DIALOG
        };
        EventChartTwoShowLineSeriesHistoryDialog(const Mode mode,
                                                 const int32_t browserWindowIndex,
                                                 ChartTwoOverlay* chartOverlay);
        
        virtual ~EventChartTwoShowLineSeriesHistoryDialog();

        Mode getMode() const;
        
        int32_t getBrowserWindowIndex() const;
        
        ChartTwoOverlay* getChartOverlay() const;

        // ADD_NEW_METHODS_HERE

    private:
        EventChartTwoShowLineSeriesHistoryDialog(const EventChartTwoShowLineSeriesHistoryDialog&);

        EventChartTwoShowLineSeriesHistoryDialog& operator=(const EventChartTwoShowLineSeriesHistoryDialog&);
        
        const Mode m_mode;
        
        const int32_t m_browserWindowIndex;
        
        ChartTwoOverlay* m_chartOverlay;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __EVENT_CHART_TWO_SHOW_LINE_SERIES_HISTORY_DIALOG_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __EVENT_CHART_TWO_SHOW_LINE_SERIES_HISTORY_DIALOG_DECLARE__

} // namespace
#endif  //__EVENT_CHART_TWO_SHOW_LINE_SERIES_HISTORY_DIALOG_H__
