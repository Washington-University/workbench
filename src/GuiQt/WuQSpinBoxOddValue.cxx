
/*LICENSE_START*/
/*
 *  Copyright (C) 2014 Washington University School of Medicine
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

#define __WU_Q_SPIN_BOX_ODD_VALUE_DECLARE__
#include "WuQSpinBoxOddValue.h"
#undef __WU_Q_SPIN_BOX_ODD_VALUE_DECLARE__

#include <QLineEdit>
#include <QSpinBox>

#include "CaretAssert.h"
#include "CaretLogger.h"
#include "MathFunctions.h"

using namespace caret;


    
/**
 * \class caret::WuQSpinBoxOddValue 
 * \brief Spin box that allows only odd values
 * \ingroup GuiQt
 *
 * Creates a spin box that allows only odd values.
 * Any attempts to set the value to an event value
 * or any attempts to set other parameters that 
 * would cause an even value are ignored.  To 
 * ensure only odd values, enter of text is disabled.
 */

/**
 * Constructor.
 */
WuQSpinBoxOddValue::WuQSpinBoxOddValue(QObject* parent)
: WuQWidget(parent)
{
    m_spinBox = new WuQSpinBoxOddValueSpinBox();
    m_spinBox->setMinimum(-99999999);
    m_spinBox->setMaximum( 99999999);
    m_spinBox->setSingleStep(2);
}

/**
 * Destructor.
 */
WuQSpinBoxOddValue::~WuQSpinBoxOddValue()
{
}

/**
 * Return the enapsulated widget so that it can be added
 * to a layout
 */
QWidget*
WuQSpinBoxOddValue::getWidget()
{
    return m_spinBox;
}

/**
 * @return The minimum value allowed in the spin box.
 */
int
WuQSpinBoxOddValue::minimum() const
{
    return m_spinBox->minimum();
}

/**
 * @return The maximum value allowed in the spin box.
 */
int
WuQSpinBoxOddValue::maximum() const
{
    return m_spinBox->maximum();
}

/**
 * Set the minimum value.  If the value is even, no action is taken.
 *
 * @param min
 *    New minimum value.
 */
void
WuQSpinBoxOddValue::setMinimum(int min)
{
    if (MathFunctions::isEvenNumber(min)) {
        const AString msg = "Value passed to WuQSpinBoxOddValue::setMinimum MUST BE AN ODD NUMBER";
        CaretAssertMessage(0, msg);
        CaretLogSevere(msg);
        return;
    }
    
    m_spinBox->setMinimum(min);
}

/**
 * Set the maximum value.  If the value is even, no action is taken.
 *
 * @param min
 *    New maximum value.
 */
void
WuQSpinBoxOddValue::setMaximum(int max)
{
    if (MathFunctions::isEvenNumber(max)) {
        const AString msg = "Value passed to WuQSpinBoxOddValue::setMaximum MUST BE AN ODD NUMBER";
        CaretAssertMessage(0, msg);
        CaretLogSevere(msg);
        return;
    }
    
    m_spinBox->setMaximum(max);
}

/**
 * Set the range of values.
 *
 * @param minimum
 *    New minimum value.  If the value is even, no action is taken.
 * @param maximum
 *    New maximum value.  If the value is even, no action is taken.
 */
void
WuQSpinBoxOddValue::setRange(int minimum, int maximum)
{
    setMinimum(minimum);
    setMaximum(maximum);
}

/**
 * @return The step value when the user presses the up or down arrow.
 */
int
WuQSpinBoxOddValue::singleStep() const
{
    return m_spinBox->singleStep();
}

/**
 * Set the step value for when the up or down arrow is pressed.
 * If the value is odd, no action is taken since the spin box
 * must change value by 2 so that the value remains odd.
 *
 * @param val
 *    New step value.
 */
void
WuQSpinBoxOddValue::setSingleStep(int val)
{
    if (MathFunctions::isOddNumber(val)) {
        const AString msg = ("Value passed to WuQSpinBoxOddValue::setMaximum MUST BE AN EVEN NUMBER "
                             "so that incrementing and decrementing preserves an odd value in the "
                             "spin box.");
        CaretAssertMessage(0, msg);
        CaretLogSevere(msg);
        return;
    }
    
    m_spinBox->setSingleStep(val);
}

/**
 * @return The value in the spin box.
 */
int
WuQSpinBoxOddValue::value() const
{
    return m_spinBox->value();
}

/**
 * Set the value.  If the value is even, no action is taken.
 *
 * @param val
 *    New value.
 */
void
WuQSpinBoxOddValue::setValue(int val)
{
    if (MathFunctions::isEvenNumber(val)) {
        const AString msg = "Value passed to WuQSpinBoxOddValue::setValue MUST BE AN ODD NUMBER";
        CaretAssertMessage(0, msg);
        CaretLogSevere(msg);
        return;
    }
    
    m_spinBox->setValue(val);
}

/* -----------------------------------------------------------------------*/

/**
 * \class caret::WuQSpinBoxOddValueSpinBox
 * \brief Spin Box used by WuQSpinBoxOddValue
 * \ingroup GuiQt
 *
 *  Spin Box used by WuQSpinBoxOddValue.
 * This is intended only for use by WuQSpinBoxOddValue
 * and it is simply a QSpinBox with its line edit
 * disabled.  Disabling the line edit is a protected
 * functions in QSpinBox and that is the only reason
 * this class exists.
 */
/**
 * Constructor.
 *
 * @param parent
 *    Optional parent widget.
 */
WuQSpinBoxOddValueSpinBox::WuQSpinBoxOddValueSpinBox(QWidget* parent)
: QSpinBox(parent)
{
    lineEdit()->setEnabled(false);
}

/**
 * Destructor.
 */
WuQSpinBoxOddValueSpinBox::~WuQSpinBoxOddValueSpinBox()
{
    
}

int
WuQSpinBoxOddValueSpinBox::valueFromText(const QString& text) const
{
    bool validFlag = false;
    int value = text.toInt(&validFlag);
    
    if (MathFunctions::isEvenNumber(value)) {
        if (value > 0) {
            value = value - 1;
        }
        else if (value < 0) {
            value = value + 1;
        }
    }
    
    return value;
}

