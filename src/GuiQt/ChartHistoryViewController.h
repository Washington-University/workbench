#ifndef __CHART_HISTORY_VIEW_CONTROLLER_H__
#define __CHART_HISTORY_VIEW_CONTROLLER_H__

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
        
        ChartModel* getSelectedChartModel();
        
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
