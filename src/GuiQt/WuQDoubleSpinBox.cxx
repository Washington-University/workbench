
/*LICENSE_START*/
/*
 *  Copyright (C) 2017 Washington University School of Medicine
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

#define __WU_Q_DOUBLE_SPIN_BOX_DECLARE__
#include "WuQDoubleSpinBox.h"
#undef __WU_Q_DOUBLE_SPIN_BOX_DECLARE__

#include <cmath>
#include <iostream>

#include <QDoubleSpinBox>

#include "AString.h"
#include "CaretAssert.h"

using namespace caret;

/**
 * \class caret::WuQDoubleSpinBox
 * \brief Customizable double spin box
 * \ingroup GuiQt
 *
 * An 'extension' of QDoubleSpinBox.  There are methods in 
 * QDoubleSpinBox that we would like to override but the methods
 * are not virtual.  So, extend WuQWidget (which extends QObject),
 * encapsulate QDoubleSpinbox, and implement needed methods.
 *
 * Added features are:
 * (1) Ability to automatically compute digits right of decimal
 * based upon the range of the data.
 *
 * (2) Ablity to set increment/decrement values as a percentage
 * of the range of data.
 *
 * (3) Ability to set an "exceedable" range.  A QDoubleSpinBox
 * will NOT emit a 'valueChanged' signal if the user tries to 
 * exceed the minimum or maximum value.  'setRangeExceedable()'
 * will allow the minimum and maximum values to be exceeded
 * but any auto formatting (decimals, step size) is based
 * only on the minimum and maximum values specified.
 *
 * (4) if 'QDoubleSpinBox::setValue()' is called, it will
 * emit the 'valueChanged()' signal which can be problematic
 * when updating the value in the spin box.  This implementation
 * DOES NOT emit the 'valueChanged()' signal when 'setValue()' 
 * is called.
 */

/**
 * Constructs a spin box with 0.0 as minimum value and 99.99 as maximum value, 
 * a step value of 1.0 and a precision of 2 decimal places. 
 * The value is initially set to 0.00. The spin box has the given parent.
 *
 * @param parent
 *     The parent widget.
 */
WuQDoubleSpinBox::WuQDoubleSpinBox(QWidget* parent)
: WuQWidget(parent)
{
    /*
     * Create the spin box and initialize it
     * Note that most members are initialized in the header file
     */
    m_spinBox = new QDoubleSpinBox();
    m_spinBox->setRange(m_minimumValue, m_maximumValue);
    m_spinBox->setSingleStep(1.0);
    m_spinBox->setDecimals(2);
    m_spinBox->setValue(0.0);
    
    QObject::connect(m_spinBox,  static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
                     this, &WuQDoubleSpinBox::valueChangedPrivate);
}

/**
 * Destructor.
 */
WuQDoubleSpinBox::~WuQDoubleSpinBox()
{
}

/**
 * @return The encapsulated widget.  Usually used to
 * add the widget to a layout.
 */
QWidget*
WuQDoubleSpinBox::getWidget()
{
    return m_spinBox;
}

///**
// * Copy all setting from the given spin box
// *
// * @param copyFromSpinBox
// *     Spinbox from which settings are copied to 'this' instance
// */
//void
//WuQDoubleSpinBox::copySettings(const WuQDoubleSpinBox* copyFromSpinBox)
//{
//    CaretAssert(copyFromSpinBox);
//    
//    switch (m_rangeMode) {
//        case RangeMode::EXCEEDABLE:
//            setRangeExceedable(m_minimumValue, m_maximumValue, m_exceedRangeMultiplier);
//            break;
//        case RangeMode::INCLUSIVE:
//            setRange(m_minimumValue, m_maximumValue);
//    }
//    m_decimalsMode = copyFromSpinBox->m_decimalsMode;
//    switch (copyFromSpinBox->m_decimalsMode) {
//        case DecimalsMode::AUTO:
//            setDecimalsModeAuto();
//            break;
//        case DecimalsMode::FIXED:
//            setDecimals(copyFromSpinBox->decimals());
//            break;
//    }
//    
//    setPrefix(copyFromSpinBox->prefix());
//    setSuffix(copyFromSpinBox->suffix());
//    
//    setSingleStep(copyFromSpinBox->singleStep());
//    m_singleStepPercentage = copyFromSpinBox->m_singleStepPercentage;
//    m_singleStepMode = copyFromSpinBox->m_singleStepMode;
//    switch (m_singleStepMode) {
//        case SingleStepMode::FIXED:
//            break;
//        case SingleStepMode::PERCENTAGE:
//            setSingleStepPercentage(copyFromSpinBox->singleStepPercentage());
//            break;
//    }
//}


///**
// * Explicit conversion to QWidget
// */
//WuQDoubleSpinBox::operator
//QWidget*() const
//{
//    return m_spinBox;
//}

/**
 * @return The decimals mode.
 */
WuQDoubleSpinBox::DecimalsMode
WuQDoubleSpinBox::decimalsMode() const
{
    return m_decimalsMode;
}

/**
 * Set the decimals mode to auto.  To exit auto use setDecimals(int).
 */
void
WuQDoubleSpinBox::setDecimalsModeAuto()
{
    m_decimalsMode = DecimalsMode::AUTO;
    updateDecimalsForAutoMode();
}

/**
 * @return Digits right of decimal point
 */
int
WuQDoubleSpinBox::decimals() const
{
    return m_spinBox->decimals();
}

/**
 * Sets fixed number of decimals the spinbox will use for displaying and interpreting doubles.
 *
 * Also sets the DecimalMode to a fixed number of digits right of the decimal.
 *
 * @param prec
 *     Number of digits right of decimal
 */
void
WuQDoubleSpinBox::setDecimals(int prec)
{
    m_decimalsMode = DecimalsMode::FIXED;
    m_spinBox->setDecimals(prec);
}

/**
 * Compute the number of digits right of the decimal point for 
 * the given range of data.
 *
 * @param range
 *     Range of the data.
 * @return 
 *     Number of digits right of decimal point.
 */
int32_t
WuQDoubleSpinBox::computeDigitsRightOfDecimal(const double dataRange)
{
    const double range = ((dataRange >=0) ? dataRange : -dataRange);
    
    int32_t numDecimals = 3;
    if (range > 0.0) {
        int32_t logValue = static_cast<int32_t>(std::log10(range));
        
        if (logValue == 0) {
            numDecimals = 3;
        }
        else if (logValue > 0) { // range > 1
            numDecimals = 4 - logValue;
        }
        else if (logValue < 0) { // 0 < range < 1
            numDecimals = 3 - logValue;
        }
        
        if (numDecimals < 1) {
            numDecimals = 1;
        }
        else if (numDecimals > 15) {
            numDecimals = 15;
        }
    }
    
    return numDecimals;
}

/**
 * Used for testing computation of digits right of decimal point.
 */
void
WuQDoubleSpinBox::testDigitsRightOfDecimal()
{
    {
        const double value = 0.0;
        const int32_t decimals = computeDigitsRightOfDecimal(value);
        AString s = QString("Range: %1  decimals: %2").arg(value, 10).arg(decimals, 5);
        std::cout << s << std::endl;
    }
    
    for (int32_t exponent = -10; exponent <= 10; exponent++) {
        const double value = std::pow(10.0, exponent);
        const int32_t decimals = computeDigitsRightOfDecimal(value);
        AString s = QString("Range: %1  decimals: %2  lo10: %3").arg(value, 10).arg(decimals, 5).arg((int)std::log10(value));
        std::cout << s << std::endl;
    }
    std::cout << std::endl << std::endl;
}

/**
 * Update the decimals for AUTO mode
 */
void
WuQDoubleSpinBox::updateDecimalsForAutoMode()
{
    static bool decimalsTestFlag = false;
    if (decimalsTestFlag) {
        testDigitsRightOfDecimal();
        decimalsTestFlag = false;
    }
    
    switch (m_decimalsMode) {
        case DecimalsMode::AUTO:
            m_spinBox->setDecimals(computeDigitsRightOfDecimal(m_maximumValue
                                                               - m_minimumValue));
            break;
        case DecimalsMode::FIXED:
            break;
    }
}

/**
 * @return Minimum displayed value.
 */
double
WuQDoubleSpinBox::minimum() const
{
    return m_minimumValue;
}

/**
 * @return Maximum displayed value.
 */
double
WuQDoubleSpinBox::maximum() const
{
    return m_maximumValue;
}

/**
 * Sets the maximum value displayed in the spin box.
 *
 * @param max
 *     New maximum value.
 */
void
WuQDoubleSpinBox::setMaximum(double max)
{
    m_maximumValue = max;
    switch (m_rangeMode) {
        case RangeMode::EXCEEDABLE:
            setRangeExceedable(m_minimumValue, m_maximumValue, m_exceedRangeMultiplier);
            break;
        case RangeMode::INCLUSIVE:
            setRange(m_minimumValue, m_maximumValue);
            break;
    }
}

/**
 * Sets the minimum value displayed in the spin box.
 *
 * @param min
 *     New minimum value.
 */
void
WuQDoubleSpinBox::setMinimum(double min)
{
    m_minimumValue = min;
    switch (m_rangeMode) {
        case RangeMode::EXCEEDABLE:
            setRangeExceedable(m_minimumValue, m_maximumValue, m_exceedRangeMultiplier);
            break;
        case RangeMode::INCLUSIVE:
            setRange(m_minimumValue, m_maximumValue);
            break;
    }
}

/**
 * @return Prefix prepended to start of displayed value.
 */
QString
WuQDoubleSpinBox::prefix() const
{
    return m_spinBox->prefix();
}

/**
 * Set the prefix is prepended to the start of the displayed value.
 * Typical use is to display a unit of measurement or a currency symbol.
 *
 * @param prefix 
 *     New prefix
 */
void
WuQDoubleSpinBox::setPrefix(const QString &prefix)
{
    m_spinBox->setPrefix(prefix);
}

/**
 * Convenience function to set the minimum and maximum values with a single function call.
 * Selected value is limited to minimum and maximum.
 *
 * @param minimum 
 *    New minimum value.
 * @param maximum
 *    New maximum value.
 */
void
WuQDoubleSpinBox::setRange(double minimum,
                           double maximum)
{
    m_minimumValue = minimum;
    m_maximumValue = maximum;
    m_rangeMode = RangeMode::INCLUSIVE;
    m_spinBox->setRange(m_minimumValue, m_maximumValue);
    updateDecimalsForAutoMode();
    updateSingleStepPercentage();
}

/**
 * Convenience function to set the minimum and maximum values with a single function call.
 * Selected value is allowed to exceed minimum and maximum by (maximum - minimum) * rangeMultiplier.
 * Any automatic formatting or decimals excludes the "exceed amount".
 *
 * @param minimum
 *    New minimum value.
 * @param maximum
 *    New maximum value.
 * @param rangeMultiplier
 *    The range multiplier.
 */
void
WuQDoubleSpinBox::setRangeExceedable(double minimum,
                                     double maximum,
                                     double rangeMultiplier)
{
    m_minimumValue = minimum;
    m_maximumValue = maximum;
    m_rangeMode = RangeMode::EXCEEDABLE;

    m_exceedRangeMultiplier = rangeMultiplier;
    const double exceedAmount = (maximum - minimum) * m_exceedRangeMultiplier;
    m_spinBox->setRange(m_minimumValue - exceedAmount, m_maximumValue + exceedAmount);
    
    updateDecimalsForAutoMode();
    updateSingleStepPercentage();
}

/**
 * Set range for percentage values.  Percentage range
 * [0.0, 100], with 2 digits right of decimal,
 * a single step of 0.1, and a '%' suffix.
 *
 * @param minimumPercentage
 *     Minimum percentage (must be in range [0.0, 100.0]
 * @param maximumPercentage
 *     Maximum percentage (must be in range [0.0, 100.0]
 */
void
WuQDoubleSpinBox::setupRangePercentage(const double minimumPercentage,
                                       const double maximumPercentage)
{
    setRange(minimumPercentage,
             maximumPercentage);
    setDecimals(2);
    setSingleStepPercentage(0.1);
    setSuffix("%");
}

/**
 * Set the step value.  When the user uses the arrows to change the
 * spin box's value the value will be incremented/decremented by the
 * amount of the given value.
 *
 * NOTE: When this method is called, the SingleStepMode is set
 * to FIXED
 *
 * @param value
 *     New step value.
 */
void
WuQDoubleSpinBox::setSingleStep(double value)
{
    CaretAssert(value >= 0.0);
    
    m_spinBox->setSingleStep(value);
    m_singleStepMode = SingleStepMode::FIXED;
}

/**
 * Set the step value.  When the user uses the arrows to change the
 * spin box's value the value will be incremented/decremented by a
 * percentage of the minimum and maximum values.
 *
 * NOTE: When this method is called, the SingleStepMode is set
 * to PERCENTAGE.
 *
 * @param percentage
 *     The step percentage that ranges (0.0, 100.0].
 */
void
WuQDoubleSpinBox::setSingleStepPercentage(double percentage)
{
    CaretAssert(percentage > 0.0);
    CaretAssert(percentage <= 100.0);
    
    m_singleStepPercentage = percentage;
    m_singleStepMode       = SingleStepMode::PERCENTAGE;
    
    updateSingleStepPercentage();
}

/**
 * Update the single step percentage when the minimum value,
 * maximum value, or step percentage is changed.
 */
void
WuQDoubleSpinBox::updateSingleStepPercentage()
{
    switch (m_singleStepMode) {
        case SingleStepMode::FIXED:
            break;
        case SingleStepMode::PERCENTAGE:
        {
            const double minValue = m_minimumValue;
            const double maxValue = m_maximumValue;
            const double dataRange = maxValue - minValue;
            
            double stepValue = 0.0;
            if (dataRange > 0.0) {
                stepValue = dataRange * (m_singleStepPercentage / 100.0);
            }
            
            m_spinBox->setSingleStep(stepValue);
        }
            break;
    }
}

/**
 * Set the suffix is append to the start of the displayed value.
 * Typical use is to display a unit of measurement or a currency symbol.
 *
 * @param suffix
 *     New suffix
 */
void
WuQDoubleSpinBox::setSuffix(const QString &suffix)
{
    m_spinBox->setSuffix(suffix);
}

/**
 * @return The single step value.
 */
double
WuQDoubleSpinBox::singleStep() const
{
    return m_spinBox->singleStep();
}

/**
 * @return The single step mode.
 */
WuQDoubleSpinBox::SingleStepMode
WuQDoubleSpinBox::singleStepMode() const
{
    return m_singleStepMode;
}

/**
 * @return The single step percentage value.
 */
double
WuQDoubleSpinBox::singleStepPercentage() const
{
    return m_singleStepPercentage;
}

/**
 * @return The suffix.
 */
QString
WuQDoubleSpinBox::suffix() const
{
    return m_spinBox->suffix();
}

/**
 * This virtual function is used by the spin box whenever it needs to 
 * display the given value. The default implementation returns a
 * string containing value printed using QWidget::locale().toString(value, QLatin1Char('f'),
 * decimals()) and will remove the thousand separator unless setGroupSeparatorShown() is set.
 * Reimplementations may return anything.
 *
 * @return Text representation of current value.
 */
QString
WuQDoubleSpinBox::textFromValue(double value) const
{
    return m_spinBox->textFromValue(value);
}

/**
 * @return Current value as double.
 */
double
WuQDoubleSpinBox::value() const
{
    return m_spinBox->value();
}

/**
 * This virtual function is used by the spin box whenever it needs to 
 * interpret text entered by the user as a value.
 * Subclasses that need to display spin box values in a non-numeric way 
 * need to reimplement this function.
 *
 * @return double value from the given text value.
 */
double
WuQDoubleSpinBox::valueFromText(const QString &text) const
{
    return m_spinBox->valueFromText(text);
}


/**
 * Slot to set the value.  Unlike QDoubleSpinBox::setValue()
 * this method DOES NOT cause the valueChanged() signal to
 * be emitted.
 *
 * @param val
 *     New value for spin box.
 */
void WuQDoubleSpinBox::setValue(double val)
{
    if (m_blockValueUpdateFlag) {
        return;
    }
    
    QSignalBlocker blocker(m_spinBox);
    m_spinBox->setValue(val);
}

/**
 * Called when the enapsulated spin box's value changed signal is emitted.
 * Use 'this' instance of signal blocking to prevent a signal from
 * being emitted.  This allows user to use WuQDoubleSpinBox::blockSignals().
 *
 * @param d
 *     Value contained in the spin box.
 */
void
WuQDoubleSpinBox::valueChangedPrivate(double d)
{
    if (m_spinBox->signalsBlocked()) {
        return;
    }
    if (signalsBlocked()) {
        return;
    }
    
    /*
     * Emitting the signal may cause a call to the 'setValue()' method.
     * We need to prevent this.  Otherwise, the spin box will get updated
     * with only the first character that was typed by user.
     * For example, without this, if the user tried to enter 9.3,
     * setValue gets called with only 9 and the user cannot enter the .3.
     */
    m_blockValueUpdateFlag = true;
    emit valueChanged(d);
    m_blockValueUpdateFlag = false;
}

// ADD_NEW_METHODS_HERE

