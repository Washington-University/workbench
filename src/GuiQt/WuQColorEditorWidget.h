#ifndef __WU_Q_COLOR_EDITOR_WIDGET_H__
#define __WU_Q_COLOR_EDITOR_WIDGET_H__

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

#include <QColor>
#include <QWidget>

class QAction;
class QLabel;
class QSlider;
class QSpinBox;
class QTransform;

namespace caret {

    class LinearEquationTransform;
    class WuQImageLabel;
    
    class WuQColorEditorWidget : public QWidget {
        
        Q_OBJECT

    public:
        WuQColorEditorWidget(QWidget* parent = 0);
        
        virtual ~WuQColorEditorWidget();
        
        WuQColorEditorWidget(const WuQColorEditorWidget&) = delete;

        WuQColorEditorWidget& operator=(const WuQColorEditorWidget&) = delete;
        
    public slots:
        void setCurrentColor(const QColor& color);
        
        void setCurrentColor(const uint8_t red,
                             const uint8_t green,
                             const uint8_t blue);
        
        // ADD_NEW_METHODS_HERE

    private slots:
        void hueChanged(int hue);
    
        void saturationChanged(int saturation);
        
        void valueChanged(int value);
        
        void redChanged(int red);
        
        void greenChanged(int green);
        
        void blueChanged(int blue);
        
        void hueSaturationLabelClicked(int x, int y);
        
        void valueLabelClicked(int x, int y);
        
        void caretColorActionClicked(QAction* action);
        
        void revertToOriginalColorToolButtonClicked();
        
    private:
        void updateControls();
        
        void updateSliderAndSpinBox(QSlider* slider,
                                    QSpinBox* spinBox,
                                    const int value);
        
        QWidget* createControlsWidget();
        
        QWidget* createHueSaturationColorLabel();
        
        void updateHueSaturationToLabelTransforms();
        
        QWidget* createValueColorLabel();
        
        void updateValueToLabelXyTransform();
        
        void updateValueColorLabel();
        
        void updateHueSaturationLabel();
        
        QWidget* createCaretColorNoNamesSelectionButtonsWidget();
        
        QSpinBox* m_hueSpinBox;
        
        QSlider* m_hueSlider;
        
        QSpinBox* m_saturationSpinBox;
        
        QSlider* m_saturationSlider;
        
        QSpinBox* m_valueSpinBox;
        
        QSlider* m_valueSlider;
        
        QSpinBox* m_redSpinBox;
        
        QSlider* m_redSlider;
        
        QSpinBox* m_greenSpinBox;
        
        QSlider* m_greenSlider;
        
        QSpinBox* m_blueSpinBox;
        
        QSlider* m_blueSlider;
        
        QWidget* m_currentColorSwatchWidget;
        
        QWidget* m_originalColorSwatchWidget;

        WuQImageLabel* m_hueSaturationColorLabel;
        
        QPixmap m_hueSaturationPixmap;
        
        WuQImageLabel* m_valueColorLabel;
        
        QColor m_currentColor;
        
        QColor m_originalColor;
        
        std::unique_ptr<LinearEquationTransform> m_valueToLabelLinearTransform;
        
        QSize m_valueLabelSize;
        
        QSize m_hueSaturationLabelSize;
        
        std::unique_ptr<LinearEquationTransform> m_hueToLabelLinearTransform;
        
        std::unique_ptr<LinearEquationTransform> m_saturationToLabelLinearTransform;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
    class LinearEquationTransform {
    public:
        LinearEquationTransform(const float xMin,
                                const float xMax,
                                const float yMin,
                                const float yMax,
                                const float x0,
                                const float y0);

        ~LinearEquationTransform();
        
        float transformValue(const float value) const;
        
        float inverseTransformValue(const float value) const;
        
    private:
        QTransform m_transform;
        
        QTransform m_inverseTransform;
    };
    
#ifdef __WU_Q_COLOR_EDITOR_WIDGET_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __WU_Q_COLOR_EDITOR_WIDGET_DECLARE__

} // namespace
#endif  //__WU_Q_COLOR_EDITOR_WIDGET_H__
