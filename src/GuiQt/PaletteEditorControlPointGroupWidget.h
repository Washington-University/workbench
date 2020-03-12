#ifndef __PALETTE_EDITOR_CONTROL_POINT_GROUP_WIDGET_H__
#define __PALETTE_EDITOR_CONTROL_POINT_GROUP_WIDGET_H__

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

#include <QWidget>

class QButtonGroup;
class QDoubleSpinBox;
class QGridLayout;
class QLabel;
class QRadioButton;
class QStackedWidget;
class QToolButton;

namespace caret {

    class PaletteControlPointRow;
    
    class PaletteEditorControlPointGroupWidget : public QWidget {
        
        Q_OBJECT

    public:
        enum class ModificationOperation {
            INSERT_CONTROL_POINT_ABOVE,
            INSERT_CONTROL_POINT_BELOW,
            REMOVE_CONTROL_POINT
        };
        
        PaletteEditorControlPointGroupWidget(QButtonGroup* colorEditButtonGroup,
                                             const bool showColumnTitles,
                                             QWidget* parent);
        
        virtual ~PaletteEditorControlPointGroupWidget();
        
        PaletteEditorControlPointGroupWidget(const PaletteEditorControlPointGroupWidget&) = delete;

        PaletteEditorControlPointGroupWidget& operator=(const PaletteEditorControlPointGroupWidget&) = delete;
        
        void updateContent(void* controlPointGroup,
                           const int32_t numberOfControlsPointsForLayoutTesting);

        void updateControlPointColor(const uint8_t red,
                                     const uint8_t green,
                                     const uint8_t blue);

        // ADD_NEW_METHODS_HERE

    signals:
        void editColorRequested(const int32_t controlPointIndex,
                                const uint8_t red, const uint8_t green, const uint8_t blue);

    private slots:
        void modificationRequest(const int32_t controlPointIndex,
                                   const PaletteEditorControlPointGroupWidget::ModificationOperation modificationOperation);
        
        void controlPointValueChangeRequested(const int32_t controlPointIndex,
                                      float value);
        
//        void editColorRequested(const int32_t controlPointIndex,
//                                const uint8_t red, const uint8_t green, const uint8_t blue);

    private:
        QButtonGroup* m_colorEditButtonGroup;
        
        QGridLayout* m_controlPointGridLayout;
        
        std::vector<PaletteControlPointRow*> m_paletteControlPointRows;
        
        const bool m_showColumnTitles;
        // ADD_NEW_MEMBERS_HERE

    };
    
    class PaletteControlPointRow : public QObject {
        Q_OBJECT
        
    public:
        PaletteControlPointRow(PaletteEditorControlPointGroupWidget* paletteEditorControlPointGroupWidget,
                               QButtonGroup* colorEditButtonGroup,
                               QGridLayout* gridLayout,
                               const int32_t controlPointIndex,
                               const bool showColumnTitles);

        ~PaletteControlPointRow();
        
        void updateContent(void* controlPoint,
                           const int32_t numberOfControlPoints);
        
        void updateColor(const uint8_t red,
                         const uint8_t green,
                         const uint8_t blue);
        
        const int32_t m_controlPointIndex;
        
        QToolButton* m_constructionToolButton;
        
        /** Used for adjustable values, may be able set min==max and use special value for text "+1", "+ Zero", "Zero", "- Zero", "-1" */
        QDoubleSpinBox* m_valueSpinBox;
        
        QLabel* m_valueLabel;
        
        /** Used if have label for some values and adjustable values for others */
        QStackedWidget* m_valueStackedWidget;
        
        QWidget* m_colorSwatchWidget;
        
        QRadioButton* m_editColorRadioButton;
        
        QAction* m_insertAboveAction;
        
        QAction* m_insertBelowAction;
        
        QAction* m_removeAction;

    signals:
        void modificationRequested(const int32_t controlPointIndex,
                                   const PaletteEditorControlPointGroupWidget::ModificationOperation modificationOperation);
        
        void controlPointValueChanged(const int32_t controlPointIndex,
                                      float value);
        
        void editColorRequested(const int32_t controlPointIndex,
                                const uint8_t red, const uint8_t green, const uint8_t blue);
        
    };

#ifdef __PALETTE_EDITOR_CONTROL_POINT_GROUP_WIDGET_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __PALETTE_EDITOR_CONTROL_POINT_GROUP_WIDGET_DECLARE__

} // namespace
#endif  //__PALETTE_EDITOR_CONTROL_POINT_GROUP_WIDGET_H__
