#ifndef __BRAIN_BROWSER_WINDOW_TOOL_BAR_CHART_TWO_ORIENTED_AXES_H__
#define __BRAIN_BROWSER_WINDOW_TOOL_BAR_CHART_TWO_ORIENTED_AXES_H__

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

#include <tuple>

#include "BrainBrowserWindowToolBarComponent.h"
#include "ChartAxisLocationEnum.h"
#include "ChartTwoAxisOrientationTypeEnum.h"

class QCheckBox;
class QDoubleSpinBox;
class QMenu;
class QToolButton;

namespace caret {

    class ChartTwoAxisPropertiesEditorWidget;
    class ChartTwoCartesianOrientedAxes;
    class ChartTwoOverlaySet;
    class ChartTwoTitleEditorWidget;
    class EnumComboBoxTemplate;
    class WuQDoubleSpinBox;
    class WuQTrueFalseComboBox;
    class WuQWidgetObjectGroup;
    
    class BrainBrowserWindowToolBarChartTwoOrientedAxes : public BrainBrowserWindowToolBarComponent {
        Q_OBJECT
        
    public:
        BrainBrowserWindowToolBarChartTwoOrientedAxes(BrainBrowserWindowToolBar* parentToolBar,
                                                      const QString& parentObjectName);
        
        virtual ~BrainBrowserWindowToolBarChartTwoOrientedAxes();
        
        virtual void updateContent(BrowserTabContent* browserTabContent);
        
        virtual void receiveEvent(Event* event);
        
        // ADD_NEW_METHODS_HERE

    private slots:
        void valueChanged();
        
        void valueChangedBool(bool);
        
        void valueChangedDouble(double);
        
        void valueChangedInt(int);
        
        void horizontalRangeModeChanged();
        
        void verticalRangeModeChanged();
        
        void horizontalAxisMinimumValueChanged(double);
        
        void horizontalAxisMaximumValueChanged(double);
        
        void horizontalTransformEnabledChecked(bool);
        
        void horizontalRangeResetToolButton();
        
        void verticalAxisMinimumValueChanged(double);
        
        void verticalAxisMaximumValueChanged(double);
        
        void verticalTransformEnabledChecked(bool);
        
        void verticalRangeResetToolButton();
        
        void axisCheckBoxOnOffClicked(const ChartAxisLocationEnum::Enum axis,
                                      const bool checkedStatus);
        
        void axisToolButtonEditClicked(const ChartAxisLocationEnum::Enum axis);
        
        void axisEditorMenuAboutToShow(const ChartAxisLocationEnum::Enum axis);
        
        void titleCheckBoxClicked(bool checked);
        
        void titleEditToolButtonClicked();
        
        void titleEditorMenuAboutToShow();
        
    private:
        BrainBrowserWindowToolBarChartTwoOrientedAxes(const BrainBrowserWindowToolBarChartTwoOrientedAxes&);

        BrainBrowserWindowToolBarChartTwoOrientedAxes& operator=(const BrainBrowserWindowToolBarChartTwoOrientedAxes&);
        
        void getSelectionData(ChartTwoOverlaySet* &chartOverlaySetOut,
                              ChartTwoCartesianOrientedAxes* &horizontalAxesOut,
                              ChartTwoCartesianOrientedAxes* &verticalAxesOut) const;
        
        void updateControls();
        
        void updateGraphics();
        
        std::tuple<EnumComboBoxTemplate*,
        WuQDoubleSpinBox*,
        WuQDoubleSpinBox*,
        WuQTrueFalseComboBox*,
        QToolButton*> createAxesWidgets(const ChartTwoAxisOrientationTypeEnum::Enum orientation,
                                        const QString& objectNamePrefix);
        
        std::tuple<QCheckBox*, QToolButton*> createAxisEditing(const ChartAxisLocationEnum::Enum axis,
                                                               const QString& objectNamePrefix);
        
        // ADD_NEW_MEMBERS_HERE
        
        int32_t m_browserTabIndex = -1;
        
        EnumComboBoxTemplate* m_horizontalRangeModeComboBox;
        
        WuQDoubleSpinBox* m_horizontalUserMinimumValueSpinBox;
        
        WuQDoubleSpinBox* m_horizontalUserMaximumValueSpinBox;
                
        WuQTrueFalseComboBox* m_horizontalTransformEnabledComboBox;
        
        QToolButton* m_horizontalRangeResetToolButton;
        
        EnumComboBoxTemplate* m_verticalRangeModeComboBox;
        
        WuQDoubleSpinBox* m_verticalUserMinimumValueSpinBox;
        
        WuQDoubleSpinBox* m_verticalUserMaximumValueSpinBox;
        
        WuQTrueFalseComboBox* m_verticalTransformEnabledComboBox;
        
        QToolButton* m_verticalRangeResetToolButton;
        
        QCheckBox* m_leftAxisCheckBox;
        
        QToolButton* m_leftAxisEditToolButton;
        
        ChartTwoAxisPropertiesEditorWidget* m_leftAxisEditorWidget;
        
        QMenu* m_leftAxisMenu;
        
        QCheckBox* m_rightAxisCheckBox;
        
        QToolButton* m_rightAxisEditToolButton;
        
        ChartTwoAxisPropertiesEditorWidget* m_rightAxisEditorWidget;
        
        QMenu* m_rightAxisMenu;
        
        QCheckBox* m_bottomAxisCheckBox;
        
        QToolButton* m_bottomAxisEditToolButton;
        
        ChartTwoAxisPropertiesEditorWidget* m_bottomAxisEditorWidget;
        
        QMenu* m_bottomAxisMenu;
        
        QCheckBox* m_topAxisCheckBox;
        
        QToolButton* m_topAxisEditToolButton;
        
        ChartTwoAxisPropertiesEditorWidget* m_topAxisEditorWidget;
        
        QMenu* m_topAxisMenu;
        
        QCheckBox* m_titleCheckBox;
        
        QToolButton* m_titleEditToolButton;
        
        ChartTwoTitleEditorWidget* m_titleEditorWidget;
        
        QMenu* m_titleEditMenu;
        
        WuQWidgetObjectGroup* m_widgetGroup;
        
    };
    
#ifdef __BRAIN_BROWSER_WINDOW_TOOL_BAR_CHART_TWO_ORIENTED_AXES_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __BRAIN_BROWSER_WINDOW_TOOL_BAR_CHART_TWO_ORIENTED_AXES_DECLARE__

} // namespace
#endif  //__BRAIN_BROWSER_WINDOW_TOOL_BAR_CHART_TWO_ORIENTED_AXES_H__
