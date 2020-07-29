#ifndef __CHART_TWO_AXIS_PROPERTIES_EDITOR_WIDGET_H__
#define __CHART_TWO_AXIS_PROPERTIES_EDITOR_WIDGET_H__

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

#include <QWidget>
#include "ChartAxisLocationEnum.h"

class QCheckBox;
class QComboBox;
class QDoubleSpinBox;
class QLabel;
class QToolButton;

namespace caret {

    class AnnotationPercentSizeText;
    class ChartTwoCartesianAxis;
    class ChartTwoCartesianAxisWidget;
    class ChartTwoOverlaySet;
    class EnumComboBoxTemplate;
    class WuQDoubleSpinBox;
    class WuQSpinBox;
    class WuQWidgetObjectGroup;
    
    class ChartTwoAxisPropertiesEditorWidget : public QWidget {
        Q_OBJECT
        
    public:
        ChartTwoAxisPropertiesEditorWidget(QWidget* parent,
                                           const ChartAxisLocationEnum::Enum,
                                           const QString& parentObjectName);
        
        virtual ~ChartTwoAxisPropertiesEditorWidget();
        
        void updateControls(ChartTwoOverlaySet* chartOverlaySet,
                            ChartTwoCartesianAxis* chartAxis);
        
        // ADD_NEW_METHODS_HERE

    private slots:
        void valueChanged();
        
        void valueChangedBool(bool);
        
        void valueChangedDouble(double);
        
        void valueChangedInt(int);
        
        void axisLabelToolButtonClicked(bool);
        
        void axisLineThicknessChanged(double);
        
    private:
        ChartTwoAxisPropertiesEditorWidget(const ChartTwoAxisPropertiesEditorWidget&);

        ChartTwoAxisPropertiesEditorWidget& operator=(const ChartTwoAxisPropertiesEditorWidget&);
        
        void updateGraphics();
        
        // ADD_NEW_MEMBERS_HERE
        
        ChartTwoOverlaySet* m_chartOverlaySet;
        
        ChartTwoCartesianAxis* m_chartAxis;
        
        QToolButton* m_axisLabelToolButton;
        
        QComboBox* m_axisLabelFromOverlayComboBox;
        
        QCheckBox* m_showTickMarksCheckBox;
        
        QCheckBox* m_showLabelCheckBox;
        
        QCheckBox* m_showNumericsCheckBox;
        
        QCheckBox* m_rotateNumericsCheckBox;
        
        EnumComboBoxTemplate* m_userNumericFormatComboBox;
        
        WuQSpinBox* m_userDigitsRightOfDecimalSpinBox;
        
        EnumComboBoxTemplate* m_numericSubdivisionsModeComboBox;
        
        WuQSpinBox* m_userSubdivisionsSpinBox;
        
        WuQDoubleSpinBox* m_labelSizeSpinBox;
        
        WuQDoubleSpinBox* m_numericsSizeSpinBox;
        
        WuQDoubleSpinBox* m_linesTicksSizeSpinBox;
        
        WuQDoubleSpinBox* m_paddingSizeSpinBox;
        
        WuQWidgetObjectGroup* m_widgetGroup;
        
    };
    
#ifdef __CHART_TWO_AXIS_PROPERTIES_EDITOR_WIDGET_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __CHART_TWO_AXIS_PROPERTIES_EDITOR_WIDGET_DECLARE__

} // namespace
#endif  //__CHART_TWO_AXIS_PROPERTIES_EDITOR_WIDGET_H__
