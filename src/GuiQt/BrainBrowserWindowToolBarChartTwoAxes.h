#ifndef __BRAIN_BROWSER_WINDOW_TOOL_BAR_CHART_TWO_AXES_H__
#define __BRAIN_BROWSER_WINDOW_TOOL_BAR_CHART_TWO_AXES_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2014 Washington University School of Medicine
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


#include "BrainBrowserWindowToolBarComponent.h"
#include "ChartAxisLocationEnum.h"

class QCheckBox;
class QDoubleSpinBox;
class QLabel;
class QSpinBox;
class QToolButton;

namespace caret {

    class ChartTwoCartesianAxis;
    class ChartTwoCartesianAxisWidget;
    class EnumComboBoxTemplate;
    class WuQWidgetObjectGroup;
    
    class BrainBrowserWindowToolBarChartTwoAxes : public BrainBrowserWindowToolBarComponent {
        Q_OBJECT
        
    public:
        BrainBrowserWindowToolBarChartTwoAxes(BrainBrowserWindowToolBar* parentToolBar);
        
        virtual ~BrainBrowserWindowToolBarChartTwoAxes();
        
        virtual void updateContent(BrowserTabContent* browserTabContent);
        
        // ADD_NEW_METHODS_HERE

    private slots:
        void axisChanged();
        
        void valueChanged();
        
        void valueChangedBool(bool);
        
        void valueChangedDouble(double);
        
        void valueChangedInt(int);
        
        void axisNameToolButtonClicked(bool);
        
    private:
        BrainBrowserWindowToolBarChartTwoAxes(const BrainBrowserWindowToolBarChartTwoAxes&);

        BrainBrowserWindowToolBarChartTwoAxes& operator=(const BrainBrowserWindowToolBarChartTwoAxes&);
        
        ChartAxisLocationEnum::Enum getSelectedAxisLocation() const;
        
        void updateControls(ChartTwoCartesianAxis* chartAxis);
        
        void updateGraphics();
        
        // ADD_NEW_MEMBERS_HERE
        
        EnumComboBoxTemplate* m_axisComboBox;
        
        QToolButton* m_axisNameToolButton;
        
        EnumComboBoxTemplate* m_autoUserRangeComboBox;
        
        QDoubleSpinBox* m_userMinimumValueSpinBox;
        
        QDoubleSpinBox* m_userMaximumValueSpinBox;
        
        QCheckBox* m_showTickMarksCheckBox;
        
        EnumComboBoxTemplate* m_userNumericFormatComboBox;
        
        QSpinBox* m_userDigitsRightOfDecimalSpinBox;
        
        QCheckBox* m_autoSubdivisionsCheckBox;
        
        QSpinBox* m_userSubdivisionsSpinBox;
        
        ChartTwoCartesianAxis* m_chartAxis;
        
        WuQWidgetObjectGroup* m_widgetGroup;
        
    };
    
#ifdef __BRAIN_BROWSER_WINDOW_TOOL_BAR_CHART_TWO_AXES_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __BRAIN_BROWSER_WINDOW_TOOL_BAR_CHART_TWO_AXES_DECLARE__

} // namespace
#endif  //__BRAIN_BROWSER_WINDOW_TOOL_BAR_CHART_TWO_AXES_H__
