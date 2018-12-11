#ifndef __BRAIN_BROWSER_WINDOW_TOOL_BAR_CHART_AXES_H__
#define __BRAIN_BROWSER_WINDOW_TOOL_BAR_CHART_AXES_H__

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


class QCheckBox;
class QDoubleSpinBox;
class QGridLayout;
class QLabel;

#include "BrainBrowserWindowToolBarComponent.h"



namespace caret {

    class ChartAxis;
    class ChartModelCartesian;
    class BrainBrowserWindowToolBar;
    class WuQWidgetObjectGroup;
    
    class BrainBrowserWindowToolBarChartAxes : public BrainBrowserWindowToolBarComponent {
        Q_OBJECT
        
    public:
        BrainBrowserWindowToolBarChartAxes(BrainBrowserWindowToolBar* parentToolBar,
                                           const QString& parentObjectName);
        
        virtual ~BrainBrowserWindowToolBarChartAxes();
        
        virtual void updateContent(BrowserTabContent* browserTabContent);
        
    private slots:
        void leftAxisAutoRangeScaleCheckBoxClicked(bool checked);
        
        void leftAxisValueChanged(double value);
        
        void bottomAxisAutoRangeScaleCheckBoxClicked(bool checked);
        
        void bottomAxisValueChanged(double value);
        
    private:
        BrainBrowserWindowToolBarChartAxes(const BrainBrowserWindowToolBarChartAxes&);

        BrainBrowserWindowToolBarChartAxes& operator=(const BrainBrowserWindowToolBarChartAxes&);
        
        void createAxisWidgets(QGridLayout* gridLayout,
                               QLabel*& nameLabel,
                               QCheckBox*& autoRangeScaleCheckBox,
                               QDoubleSpinBox*& minimumValueSpinBox,
                               QDoubleSpinBox*& maximumValueSpinBox,
                               WuQWidgetObjectGroup*& widgetGroup);
        
        void updateAxisWidgets(const ChartAxis* chartAxis,
                               QLabel* nameLabel,
                               QCheckBox* autoRangeScaleCheckBox,
                               QDoubleSpinBox* minimumValueSpinBox,
                               QDoubleSpinBox* maximumValueSpinBox,
                               WuQWidgetObjectGroup* widgetGroup);
        
        ChartModelCartesian* getCartesianChart();

    public:

        // ADD_NEW_METHODS_HERE

    private:
        
        BrainBrowserWindowToolBar* m_parentToolBar;
        
        QLabel* m_bottomAxisLabel;
        
        QCheckBox* m_bottomAxisAutoRangeScaleCheckBox;
        
        QDoubleSpinBox* m_bottomAxisMinimumValueSpinBox;
        
        QDoubleSpinBox* m_bottomAxisMaximumValueSpinBox;
        
        WuQWidgetObjectGroup* m_bottomAxisWidgetGroup;
        
        QLabel* m_leftAxisLabel;
        
        QCheckBox* m_leftAxisAutoRangeScaleCheckBox;
        
        QDoubleSpinBox* m_leftAxisMinimumValueSpinBox;
        
        QDoubleSpinBox* m_leftAxisMaximumValueSpinBox;
        
        WuQWidgetObjectGroup* m_leftAxisWidgetGroup;
        
//        QLabel* m_topAxisLabel;
//        
//        QCheckBox* m_topAxisAutoRangeScaleCheckBox;
//        
//        QDoubleSpinBox* m_topAxisMinimumValueSpinBox;
//        
//        QDoubleSpinBox* m_topAxisMaximumValueSpinBox;
//        
//        WuQWidgetObjectGroup* m_topAxisWidgetGroup;
//        
//        QLabel* m_rightAxisLabel;
//        
//        QCheckBox* m_rightAxisAutoRangeScaleCheckBox;
//        
//        QDoubleSpinBox* m_rightAxisMinimumValueSpinBox;
//        
//        QDoubleSpinBox* m_rightAxisMaximumValueSpinBox;
//        
//        WuQWidgetObjectGroup* m_rightAxisWidgetGroup;
        
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __BRAIN_BROWSER_WINDOW_TOOL_BAR_CHART_AXES_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __BRAIN_BROWSER_WINDOW_TOOL_BAR_CHART_AXES_DECLARE__

} // namespace
#endif  //__BRAIN_BROWSER_WINDOW_TOOL_BAR_CHART_AXES_H__
