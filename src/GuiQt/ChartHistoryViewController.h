#ifndef __CHART_HISTORY_VIEW_CONTROLLER_H__
#define __CHART_HISTORY_VIEW_CONTROLLER_H__

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

#include <stdint.h>

#include <QWidget>

#include "EventListenerInterface.h"

class QCheckBox;
class QSpinBox;
class QTableWidget;


namespace caret {

    class ChartModel;
    
    class ChartHistoryViewController : public QWidget, public EventListenerInterface {
        
        Q_OBJECT

    public:
        ChartHistoryViewController(const Qt::Orientation orientation,
                                   const int32_t browserWindowIndex,
                                   QWidget* parent);
        
        virtual ~ChartHistoryViewController();
        
    private:
        ChartHistoryViewController(const ChartHistoryViewController&);

        ChartHistoryViewController& operator=(const ChartHistoryViewController&);
        
    public:

        // ADD_NEW_METHODS_HERE

        virtual void receiveEvent(Event* event);

    private slots:
        void averageCheckBoxClicked(bool);
        
        void clearPushButtonClicked();
        
        void maximumDisplayedSpinBoxValueChanged(int);
        
        void chartDataTableCellChanged(int, int);
        
    private:
        // ADD_NEW_MEMBERS_HERE

        void updateAfterSelectionsChanged();
        
        void updateHistoryViewController();
        
        void getSelectedChartModelAndTab(ChartModel* &chartModelOut,
                                         int32_t& tabIndexOut);
        
        const int32_t m_browserWindowIndex;
        
        QCheckBox* m_averageCheckBox;
        
        QTableWidget* m_chartDataTableWidget;
        
        QSpinBox* m_maximumDisplayedSpinBox;
        
        static const int32_t COLUMN_CHART_DATA_CHECKBOX;
        static const int32_t COLUMN_CHART_DATA_NAME;
        static const int32_t COLUMN_CHART_DATA_COLOR;
        static const int32_t COLUMN_COUNT;
        
//        QGridLayout* m_gridLayout;
//        
//        QSignalMapper* m_signalMapperFileEnableCheckBox;
//        
//        static int COLUMN_ENABLE_CHECKBOX;
//        static int COLUMN_NAME_LINE_EDIT;
    };
    
#ifdef __CHART_HISTORY_VIEW_CONTROLLER_DECLARE__
    const int32_t ChartHistoryViewController::COLUMN_CHART_DATA_CHECKBOX = 0;
    const int32_t ChartHistoryViewController::COLUMN_CHART_DATA_COLOR    = 1;
    const int32_t ChartHistoryViewController::COLUMN_CHART_DATA_NAME     = 2;
    const int32_t ChartHistoryViewController::COLUMN_COUNT               = 3;
#endif // __CHART_HISTORY_VIEW_CONTROLLER_DECLARE__

} // namespace
#endif  //__CHART_HISTORY_VIEW_CONTROLLER_H__
