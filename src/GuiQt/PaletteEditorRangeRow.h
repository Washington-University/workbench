#ifndef __PALETTE_EDITOR_RANGE_ROW_H__
#define __PALETTE_EDITOR_RANGE_ROW_H__

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


#include <memory>

#include <QObject>

#include "CaretRgb.h"
#include "PaletteEditorRangeWidget.h"

class QButtonGroup;
class QGridLayout;
class QLabel;
class QStackedWidget;

namespace caret {

    class WuQGridLayoutGroup;
    
    class PaletteEditorRangeRow : public QObject {
        
        Q_OBJECT

    public:
        PaletteEditorRangeRow(const int32_t rowIndex,
                              const PaletteEditorRangeWidget::DataRangeMode rangeMode,
                              QButtonGroup* colorEditButtonGroup,
                              QGridLayout* parentGridLayout,
                              QObject* parent);
        
        virtual ~PaletteEditorRangeRow();
        
        PaletteEditorRangeRow(const PaletteEditorRangeRow&) = delete;

        PaletteEditorRangeRow& operator=(const PaletteEditorRangeRow&) = delete;

        float getScalar() const;
        
        CaretRgb getRgb() const;
        
        void setWidgetsVisible(const bool visibleFlag);

        void setScalar(const float scalar);
        
        void setRgb(const CaretRgb& rgb);
        
        void updateRgbIfRadioButtonChecked(const CaretRgb& rgb);
        
        void updateScalarValueRange(const float minValue,
                                    const float maxValue);
        
        void updateContent(const int32_t numberOfControlPoints);
        
        /**
         * Step value (amount spin box value changes when zero clicks
         * one of the arrow buttons)
         */
        static float getScalarValueSpinBoxSingleStep() { return 0.001f; }
        
        // ADD_NEW_METHODS_HERE

    signals:
        /**
         * Emitted when this control point has changed (scalar or color)
         */
        void signalDataChanged();
        
        /**
         * Emitted when the user clicks the radio button to
         * select this control point for color editing
         */
        void signalColorEditingRequested(const CaretRgb& rgb);
        
        /**
         * Emitted when an items is selected from the construction menu
         */
        void signalConstructionOperationRequested(const int32_t rowIndex,
                                                  const PaletteEditorRangeWidget::ConstructionOperation operation);
        
    private slots:
        void constructionMenuTriggered(QAction* action);
        
        void scalarValueChangedByUser(double value);
        
    private:
        // ADD_NEW_MEMBERS_HERE

        QToolButton* createConstructionToolButton();
        
        QWidget* createScalarEditingWidget();
        
        const int32_t m_rowIndex;
        
        const PaletteEditorRangeWidget::DataRangeMode m_rangeMode;

        WuQGridLayoutGroup* m_gridLayoutGroup = NULL;
        
        QStackedWidget* m_valueStackedWidget = NULL;
        
        QLabel* m_valueLabel = NULL;
        
        QDoubleSpinBox* m_valueSpinBox = NULL;
        
        QWidget* m_colorSwatchWidget = NULL;
        
        QRadioButton* m_colorEditRadioButton = NULL;
        
        QAction* m_constructionInsertAboveAction = NULL;
        
        QAction* m_constructionInsertBelowAction = NULL;
        
        QAction* m_constructionRemoveAction = NULL;
        
        CaretRgb m_rgb;
        
        static constexpr int32_t COLUMN_CONSTRUCTION = 0;
        
        static constexpr int32_t COLUMN_SCALAR       = 1;
        
        static constexpr int32_t COLUMN_COLOR_SWATCH = 2;
        
        static constexpr int32_t COLUMN_RADIO_BUTTON = 3;
        
    };
    
#ifdef __PALETTE_EDITOR_RANGE_ROW_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __PALETTE_EDITOR_RANGE_ROW_DECLARE__

} // namespace
#endif  //__PALETTE_EDITOR_RANGE_ROW_H__
