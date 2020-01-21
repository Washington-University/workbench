
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

/* Qt License is included since the API is copied from QSpinBox */

/****************************************************************************
 **
 ** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
 ** Contact: http://www.qt-project.org/legal
 **
 ** This file is part of the QtGui module of the Qt Toolkit.
 **
 ** $QT_BEGIN_LICENSE:LGPL$
 ** Commercial License Usage
 ** Licensees holding valid commercial Qt licenses may use this file in
 ** accordance with the commercial license agreement provided with the
 ** Software or, alternatively, in accordance with the terms contained in
 ** a written agreement between you and Digia.  For licensing terms and
 ** conditions see http://qt.digia.com/licensing.  For further information
 ** use the contact form at http://qt.digia.com/contact-us.
 **
 ** GNU Lesser General Public License Usage
 ** Alternatively, this file may be used under the terms of the GNU Lesser
 ** General Public License version 2.1 as published by the Free Software
 ** Foundation and appearing in the file LICENSE.LGPL included in the
 ** packaging of this file.  Please review the following information to
 ** ensure the GNU Lesser General Public License version 2.1 requirements
 ** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
 **
 ** In addition, as a special exception, Digia gives you certain additional
 ** rights.  These rights are described in the Digia Qt LGPL Exception
 ** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
 **
 ** GNU General Public License Usage
 ** Alternatively, this file may be used under the terms of the GNU
 ** General Public License version 3.0 as published by the Free Software
 ** Foundation and appearing in the file LICENSE.GPL included in the
 ** packaging of this file.  Please review the following information to
 ** ensure the GNU General Public License version 3.0 requirements will be
 ** met: http://www.gnu.org/copyleft/gpl.html.
 **
 **
 ** $QT_END_LICENSE$
 **
 ****************************************************************************/

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
    /* Line edit is not disabled, otherwise it appears disabled but arrow buttons still function */
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

