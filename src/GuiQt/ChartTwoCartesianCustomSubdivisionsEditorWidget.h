#ifndef __CHART_TWO_CARTESIAN_CUSTOM_SUBDIVISIONS_EDITOR_WIDGET_H__
#define __CHART_TWO_CARTESIAN_CUSTOM_SUBDIVISIONS_EDITOR_WIDGET_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2020 Washington University School of Medicine
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
#include <memory>

class QGridLayout;
class QLineEdit;
class QMenu;
class QToolButton;

namespace caret {
    class EnumComboBoxTemplate;
    class WuQDoubleSpinBox;

    class ChartTwoCartesianCustomSubdivisions;
    
    class ChartTwoCartesianCustomSubdivisionsEditorWidget : public QWidget {
        
        Q_OBJECT

    public:
        ChartTwoCartesianCustomSubdivisionsEditorWidget(QWidget* parent = 0);
        
        virtual ~ChartTwoCartesianCustomSubdivisionsEditorWidget();
        
        ChartTwoCartesianCustomSubdivisionsEditorWidget(const ChartTwoCartesianCustomSubdivisionsEditorWidget&) = delete;

        ChartTwoCartesianCustomSubdivisionsEditorWidget& operator=(const ChartTwoCartesianCustomSubdivisionsEditorWidget&) = delete;

        void updateContent(ChartTwoCartesianCustomSubdivisions* subdivisions);
        
        // ADD_NEW_METHODS_HERE

    signals:
        void widgetSizeChanged();
        
    protected:

    private:
        class Row {
        public:
            Row(const int32_t rowIndex,
                QToolButton* constructionToolButton,
                WuQDoubleSpinBox* valueSpinBox,
                QLineEdit* labelLineEdit)
            : m_rowIndex(rowIndex),
            m_constructionToolButton(constructionToolButton),
            m_valueSpinBox(valueSpinBox),
            m_labelLineEdit(labelLineEdit) { }
            
            const int32_t m_rowIndex;
            QToolButton* m_constructionToolButton;
            WuQDoubleSpinBox* m_valueSpinBox;
            QLineEdit* m_labelLineEdit;            
        };
        
        enum class ConstructionType {
            INSERT_ABOVE,
            INSERT_BELOW,
            REMOVE
        };
        
        void loadAxisIntoWidgets();
        
        void valueSpinBoxValueChanged(const int32_t rowIndex,
                                      const float value);
        
        void labelLineEditTextChanged(const int32_t rowIndex,
                                      const QString& text);
        
        void constructionMenuItemSelected(const int32_t rowIndex,
                                          const ConstructionType constructionType);
        
        void updateRangesOfValueSpinBoxes();
        
        void updateGraphics();
        
        QMenu* createConstructionMenu(const int32_t rowIndex);
        
        ChartTwoCartesianCustomSubdivisions* m_subdivisions = NULL;
        
        QGridLayout* m_rowsGridLayout;
        
        std::vector<std::unique_ptr<Row>> m_rows;
        
        static constexpr int32_t GRID_COLUMN_CONSTRUCTION = 0;
        
        static constexpr int32_t GRID_COLUMN_VALUE = 1;
        
        static constexpr int32_t GRID_COLUMN_TEXT = 2;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __CHART_TWO_CARTESIAN_CUSTOM_SUBDIVISIONS_EDITOR_WIDGET_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __CHART_TWO_CARTESIAN_CUSTOM_SUBDIVISIONS_EDITOR_WIDGET_DECLARE__

} // namespace
#endif  //__CHART_TWO_CARTESIAN_CUSTOM_SUBDIVISIONS_EDITOR_WIDGET_H__
