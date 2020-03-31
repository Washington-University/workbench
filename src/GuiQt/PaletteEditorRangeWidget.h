#ifndef __PALETTE_EDITOR_RANGE_WIDGET_H__
#define __PALETTE_EDITOR_RANGE_WIDGET_H__

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


#include <array>
#include <memory>

#include <QWidget>

#include "CaretRgb.h"
#include "PaletteNew.h"

class QButtonGroup;
class QDoubleSpinBox;
class QGridLayout;
class QLabel;
class QRadioButton;
class QStackedWidget;
class QToolButton;

namespace caret {

    class PaletteEditorRangeRow;
    
    class PaletteEditorRangeWidget : public QWidget {
        
        Q_OBJECT

    public:
        enum class DataRangeMode {
            NEGATIVE,
            POSITIVE,
            ZERO
        };
        
        enum class ConstructionOperation {
            INSERT_CONTROL_POINT_ABOVE,
            INSERT_CONTROL_POINT_BELOW,
            REMOVE_CONTROL_POINT
        };
        
        enum class ColumnTitlesMode {
            SHOW_YES,
            SHOW_NO
        };
        
        PaletteEditorRangeWidget(const DataRangeMode dataRangeMode,
                                 QButtonGroup* colorEditButtonGroup,
                                 const ColumnTitlesMode columnTitlesMode,
                                 QWidget* parent);

        virtual ~PaletteEditorRangeWidget();
        
        PaletteEditorRangeWidget(const PaletteEditorRangeWidget&) = delete;

        PaletteEditorRangeWidget& operator=(const PaletteEditorRangeWidget&) = delete;
        
        std::vector<PaletteNew::ScalarColor> getScalarColors() const;
        
        void updateContent(const std::vector<PaletteNew::ScalarColor>& scalarColorsIn);
        
        void updateControlPointColor(const CaretRgb& rgb);

        void selectFirstControlPoint();
        
        // ADD_NEW_METHODS_HERE

    signals:
        void signalEditColorRequested(const CaretRgb& rgb);

        void signalDataChanged();
        
    private slots:
        void performConstruction(const int32_t controlPointIndex,
                                   const PaletteEditorRangeWidget::ConstructionOperation constructionOperation);
        
        void performRowDataChanged();
        
    private:
        PaletteNew::ScalarColor averageScalarColor(const PaletteNew::ScalarColor& sc1,
                                                   const PaletteNew::ScalarColor& sc2) const;

        void resetScalarSpinBoxRanges();
        
        void updateScalarSpinBoxRanges();
        
        const DataRangeMode m_dataRangeMode;
        
        QButtonGroup* m_colorEditButtonGroup;
        
        QGridLayout* m_controlPointGridLayout;
        
        std::vector<PaletteEditorRangeRow*> m_rowWidgets;
        
        int32_t m_numberOfValidControlPoints = 0;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __PALETTE_EDITOR_RANGE_WIDGET_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __PALETTE_EDITOR_RANGE_WIDGET_DECLARE__

} // namespace
#endif  //__PALETTE_EDITOR_RANGE_WIDGET_H__
