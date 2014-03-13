#ifndef __COLOR_EDITOR_WIDGET__H_
#define __COLOR_EDITOR_WIDGET__H_

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

#include <stdint.h>

#include <QWidget>

class QSlider;
class QSpinBox;

namespace caret {

    class WuQWidgetObjectGroup;
    
    class ColorEditorWidget : public QWidget {
        
        Q_OBJECT

    public:
        ColorEditorWidget(const bool alphaControlEnabled = false,
                          QWidget* parent = 0);
        
        virtual ~ColorEditorWidget();

        void setColor(const float rgba[4]);
        
        void getColor(float rgba[4]) const;

        void setColor(const int rgba[4]);
        
        void getColor(int rgba[4]) const;
        
    signals:
        void colorChanged(const float*);
        void colorChanged(const int*);
        
    public slots:
        void redValueChanged(int);
        void blueValueChanged(int);
        void greenValueChanged(int);
        void alphaValueChanged(int);
        
        void emitColorChangedSignal();
        
    private:
        ColorEditorWidget(const ColorEditorWidget&);

        ColorEditorWidget& operator=(const ColorEditorWidget&);
        
    private:
        void updateColorSwatch();
        
        WuQWidgetObjectGroup* controlsWidgetGroup;
        
        QWidget* colorSwatchWidget;
        
        QSpinBox* redSpinBox;
        QSpinBox* greenSpinBox;
        QSpinBox* blueSpinBox;
        QSpinBox* alphaSpinBox;
        
        QSlider* redSlider;
        QSlider* greenSlider;
        QSlider* blueSlider;
        QSlider* alphaSlider;
        
    };
    
#ifdef __COLOR_EDITOR_WIDGET_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __COLOR_EDITOR_WIDGET_DECLARE__

} // namespace
#endif  //__COLOR_EDITOR_WIDGET__H_
