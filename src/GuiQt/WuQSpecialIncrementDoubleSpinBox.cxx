
/*LICENSE_START*/
/*
 *  Copyright (C) 2015 Washington University School of Medicine
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

#include <algorithm>
#include <iostream>

#define __WU_Q_SPECIAL_INCREMENT_DOUBLE_SPIN_BOX_DECLARE__
#include "WuQSpecialIncrementDoubleSpinBox.h"
#undef __WU_Q_SPECIAL_INCREMENT_DOUBLE_SPIN_BOX_DECLARE__

#include "CaretAssert.h"
using namespace caret;


    
/**
 * \class caret::WuQSpecialIncrementDoubleSpinBox 
 * \brief A double spin box that allows customized processing of increment/decrement using a function object.
 * \ingroup GuiQt
 */

/**
 * Constructor.
 *
 * @param stepFunctionObject
 *    Function object that processes increment and decrement requests.
 */
WuQSpecialIncrementDoubleSpinBox::WuQSpecialIncrementDoubleSpinBox(StepFunctionObject* stepFunctionObject)
: QDoubleSpinBox(),
m_stepFunctionObject(stepFunctionObject)
{
    CaretAssert(stepFunctionObject);
}

/**
 * Destructor.
 */
WuQSpecialIncrementDoubleSpinBox::~WuQSpecialIncrementDoubleSpinBox()
{
}

/**
 * Virtual function that is called whenever the user triggers a step. 
 * The steps parameter indicates how many steps were taken, e.g. 
 * Pressing Qt::Key_Down will trigger a call to stepBy(-1), whereas 
 * pressing Qt::Key_Prior will trigger a call to stepBy(10).
 *
 * If you subclass QAbstractSpinBox you must reimplement this function. 
 * Note that this function is called even if the resulting value will
 * be outside the bounds of minimum and maximum. It's this function's
 * job to handle these situations.
 *
 * @param steps
 *    Number of steps
 */
void
WuQSpecialIncrementDoubleSpinBox::stepBy(int steps)
{
    const double previousValue = value();
    
//    QString msg("Step "
//                + QString::number(steps)
//                + " current value "
//                + QString::number(previousValue));
    
    double newValue = m_stepFunctionObject->getNewValue(previousValue,
                                                       steps);
    newValue = std::min(newValue,
                        maximum());
    newValue = std::max(newValue,
                        minimum());
    
    if (newValue != previousValue) {
        blockSignals(true);
        setValue(newValue);
        blockSignals(false);
        
//        msg += (" new value "
//                + QString::number(value()));
//        std::cout << qPrintable(msg) << std::endl;
        
        emit valueChanged(newValue);
#if QT_VERSION >= 0x060000
        emit textChanged(text());
#else
        emit valueChanged(text());
#endif
    }
}

