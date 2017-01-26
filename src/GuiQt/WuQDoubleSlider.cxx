
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

#define __WU_Q_DOUBLE_SLIDER_DECLARE__
#include "WuQDoubleSlider.h"
#undef __WU_Q_DOUBLE_SLIDER_DECLARE__

#include <QSlider>

using namespace caret;


    
/**
 * \class caret::WuQDoubleSlider 
 * \brief A slider for real values.
 *
 * Creates a slider for real values by encapsulating
 * a QSlider that operates only on integer values.
 */

/**
 * Constructor.
 *
 * @param orientation
 *   horizontal or vertical
 * @param parent
 *   optional parent
 */
WuQDoubleSlider::WuQDoubleSlider(Qt::Orientation orientation,
                                 QObject* parent)
: WuQWidget(parent)
{
    this->slider = new QSlider(orientation);
    this->slider->setRange(0, 1000);
    QObject::connect(this->slider, SIGNAL(valueChanged(int)),
                     this, SLOT(qSliderValueChanged(int)));
    
    this->setRange(-100.0, 100.0);    
    this->setValue(0);
}

/**
 * Destructor.
 */
WuQDoubleSlider::~WuQDoubleSlider()
{
    
}

/**
 * Called when the encapsulated QSlider
 * value is changed.
 * @param value
 *    New value.
 */
void 
WuQDoubleSlider::qSliderValueChanged(int value)
{
    const double dSlider = this->slider->maximum() - this->slider->minimum();
    const double parametricValue = (static_cast<double>(value) - this->slider->minimum()) / dSlider;

    double dRange = this->maximum - this->minimum;
    if (dRange == 0.0) {
        dRange = 1.0;
    }
    this->sliderValue = dRange * parametricValue + this->minimum;
    
    emit valueChanged(this->sliderValue);
}

/**
 * @return  The widget that is enapsulated.
 */
QWidget* 
WuQDoubleSlider::getWidget()
{
    return this->slider;
}

/**
 * Set range of values.
 * @param minValue
 *    New value for minimum.
 * @param maxValue
 *    New value for maximum.
 */
void 
WuQDoubleSlider::setRange(double minValue, double maxValue)
{
    this->minimum = minValue;
    this->maximum = maxValue;
    if (this->minimum > this->maximum) {
        this->maximum = this->minimum;
    }
    this->updateSlider();
}

/**
 * @return  The minimum value of the slider.
 */
double
WuQDoubleSlider::minimumValue() const
{
    return this->minimum;;
}

/**
 * @return  The maximum value of the slider.
 */
double
WuQDoubleSlider::maximumValue() const
{
    return this->maximum;;
}


/**
 * @return  The value of the slider.
 */
double 
WuQDoubleSlider::value() const
{
    return this->sliderValue;
}

/**
 * Set the value for the slider.
 * @param d
 *    New value.
 */
void 
WuQDoubleSlider::setValue(double valueIn)
{
    this->sliderValue = valueIn;
    this->updateSlider();
    
    emit valueChanged(this->sliderValue);
}

/**
 * Update the encapsulated slider.
 */
void 
WuQDoubleSlider::updateSlider()
{
    double dRange = this->maximum - this->minimum;
    if (dRange == 0.0) {
        dRange = 1.0;
    }
    
    if (this->sliderValue > this->maximum) {
        this->sliderValue = this->maximum;
    }
    if (this->sliderValue < this->minimum) {
        this->sliderValue = this->minimum;
    }
    
    const double parametricValue = (this->sliderValue - this->minimum) / dRange;
    
    const int dSlider = this->slider->maximum() - this->slider->minimum();
    const int qSliderValue = static_cast<int>(dSlider * parametricValue);;// + this->minimum;
    this->slider->blockSignals(true);
    this->slider->setValue(qSliderValue);
    this->slider->blockSignals(false);    
}


