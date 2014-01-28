#ifndef __BRAIN_BROWSER_WINDOW_TOOL_BAR_CHART_AXES_H__
#define __BRAIN_BROWSER_WINDOW_TOOL_BAR_CHART_AXES_H__

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


class QCheckBox;
class QDoubleSpinBox;
class QGridLayout;
class QLabel;
class WuQWidgetObjectGroup;

#include "BrainBrowserWindowToolBarComponent.h"



namespace caret {

    class ChartAxis;
    class ChartModelCartesian;
    class BrainBrowserWindowToolBar;
    
    class BrainBrowserWindowToolBarChartAxes : public BrainBrowserWindowToolBarComponent {
        Q_OBJECT
        
    public:
        BrainBrowserWindowToolBarChartAxes(BrainBrowserWindowToolBar* parentToolBar);
        
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
        
        void updateAxisWidgets(const ChartModelCartesian* chart,
                               const ChartAxis* chartAxis,
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
