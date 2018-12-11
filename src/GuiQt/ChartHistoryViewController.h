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
class QGridLayout;
class QLabel;
class QMenu;
class QSignalMapper;
class QSpinBox;
class QToolButton;


namespace caret {

    class CaretColorEnumComboBox;
    class ChartModel;
    
    class ChartHistoryViewController : public QWidget, public EventListenerInterface {
        
        Q_OBJECT

    public:
        ChartHistoryViewController(const Qt::Orientation orientation,
                                   const int32_t browserWindowIndex,
                                   const QString& parentObjectName,
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
        
        void chartDataCheckBoxSignalMapped(int);
        
        void chartDataColorComboBoxSignalMapped(int);
        
        void chartDataConstructionToolButtonSignalMapped(int);
        
    private:
        // ADD_NEW_MEMBERS_HERE

        void updateAfterSelectionsChanged();
        
        void updateHistoryViewController();
        
        void getSelectedChartModelAndTab(ChartModel* &chartModelOut,
                                         int32_t& tabIndexOut);
        
        const Qt::Orientation m_orientation;
        
        const int32_t m_browserWindowIndex;
        
        const QString m_objectNamePrefix;
        
        QCheckBox* m_averageCheckBox;
        
        QSpinBox* m_maximumDisplayedSpinBox;
        
        QGridLayout* m_chartDataGridLayout;
        
        std::vector<QCheckBox*> m_chartDataCheckBoxes;
        std::vector<QToolButton*> m_chartDataContructionToolButtons;
        std::vector<CaretColorEnumComboBox*> m_chartDataColorComboBoxes;
        std::vector<QLabel*> m_chartDataNameLabels;
        
        QSignalMapper* m_chartDataCheckBoxesSignalMapper;
        QSignalMapper* m_chartDataColorComboBoxesSignalMapper;
        QSignalMapper* m_chartDataColorConstructionButtonSignalMapper;
        
        static const int32_t COLUMN_CHART_DATA_CHECKBOX;
        static const int32_t COLUMN_CHART_DATA_CONSTRUCTION;
        static const int32_t COLUMN_CHART_DATA_COLOR;
        static const int32_t COLUMN_CHART_DATA_NAME;
        static const int32_t COLUMN_COUNT;
    };
    
#ifdef __CHART_HISTORY_VIEW_CONTROLLER_DECLARE__
    const int32_t ChartHistoryViewController::COLUMN_CHART_DATA_CHECKBOX     = 0;
    const int32_t ChartHistoryViewController::COLUMN_CHART_DATA_CONSTRUCTION = 1;
    const int32_t ChartHistoryViewController::COLUMN_CHART_DATA_COLOR        = 2;
    const int32_t ChartHistoryViewController::COLUMN_CHART_DATA_NAME         = 3;
    const int32_t ChartHistoryViewController::COLUMN_COUNT                   = 4;
#endif // __CHART_HISTORY_VIEW_CONTROLLER_DECLARE__

} // namespace
#endif  //__CHART_HISTORY_VIEW_CONTROLLER_H__
