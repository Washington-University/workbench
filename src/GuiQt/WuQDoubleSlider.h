#ifndef __WU_Q_DOUBLE_SLIDER__H_
#define __WU_Q_DOUBLE_SLIDER__H_

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

#include "WuQWidget.h"

class QSlider;

namespace caret {

    class WuQDoubleSlider : public WuQWidget {
        
        Q_OBJECT

    public:
        WuQDoubleSlider(Qt::Orientation orientation,
                        QObject* parent);
        
        virtual ~WuQDoubleSlider();
        
        QWidget* getWidget();
        
        void setRange(double minValue, double maxValue);
        
        double minimumValue() const;
        
        double maximumValue() const;
        
        double value() const;
        
    signals:
        void valueChanged(double);
        
    public slots:
        void setValue(double);
        
    private slots:
        void qSliderValueChanged(int);
        
    private:
        WuQDoubleSlider(const WuQDoubleSlider&);

        WuQDoubleSlider& operator=(const WuQDoubleSlider&);

        void updateSlider();
        
        QSlider* slider;
        
        double minimum;
        
        double maximum;
        
        double sliderValue;
    };
    
#ifdef __WU_Q_DOUBLE_SLIDER_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __WU_Q_DOUBLE_SLIDER_DECLARE__

} // namespace
#endif  //__WU_Q_DOUBLE_SLIDER__H_
