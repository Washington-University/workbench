#ifndef __WU_Q_DOUBLE_SLIDER__H_
#define __WU_Q_DOUBLE_SLIDER__H_

/****************************************************************************
**
** Copyright (C) 1992-2007 Trolltech ASA. All rights reserved.
**
** This file is part of the Qt3Support module of the Qt Toolkit.
**
** This file may be used under the terms of the GNU General Public
** License version 2.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of
** this file.  Please review the following information to ensure GNU
** General Public Licensing requirements will be met:
** http://www.trolltech.com/products/qt/opensource.html
**
** If you are unsure which license is appropriate for your use, please
** review the following information:
** http://www.trolltech.com/products/qt/licensing.html or contact the
** sales department at sales@trolltech.com.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/


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
