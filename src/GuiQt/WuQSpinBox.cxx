
/*LICENSE_START*/
/*
 *  Copyright (C) 2019 Washington University School of Medicine
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

#define __WU_Q_SPIN_BOX_DECLARE__
#include "WuQSpinBox.h"
#undef __WU_Q_SPIN_BOX_DECLARE__

#include <QKeyEvent>

#include "CaretAssert.h"

using namespace caret;

#include <iostream>

    
/**
 * \class caret::WuQSpinBox 
 * \brief Emits a signal when the return key is pressed
 * \ingroup GuiQt
 */

/**
 * Constructor.
 */
WuQSpinBox::WuQSpinBox()
: QSpinBox()
{
}

/**
 * Destructor.
 */
WuQSpinBox::~WuQSpinBox()
{
}

void
WuQSpinBox::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Return) {
        emit signalReturnPressed();
        return;
    }
    
    QSpinBox::keyPressEvent(event);
}

