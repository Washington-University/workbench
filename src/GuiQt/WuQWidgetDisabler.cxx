
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

#define __WU_Q_WIDGET_DISABLER_DECLARE__
#include "WuQWidgetDisabler.h"
#undef __WU_Q_WIDGET_DISABLER_DECLARE__

#include <QWidget>

#include "CaretAssert.h"

using namespace caret;


    
/**
 * \class caret::WuQWidgetDisabler 
 * \brief Simplifies disabling of QWidget instance within a scope
 * \ingroup GuiQt
 *
 * WuQWidgetDisabler can be used whereever you would otherwise
 * use a pair of calls to setEnabled().  It disables the 
 * QWidget in its constructor and in the destructor it resets
 * the enabled state to what it was before the constructor
 * ran.
 *
 * An example for usage of this class is in the method
 * that receives a QPushButton:clicked signal.  User cannot
 * push the button until the method completes.
 *
 * The design of this class is modeled after QSignalBlocker
 * so Qt license is below.
 *
 */

/****************************************************************************
 **
 ** Copyright (C) 2016 The Qt Company Ltd.
 ** Contact: https://www.qt.io/licensing/
 **
 ** This file is part of the QtWidgets module of the Qt Toolkit.
 **
 ** $QT_BEGIN_LICENSE:LGPL$
 ** Commercial License Usage
 ** Licensees holding valid commercial Qt licenses may use this file in
 ** accordance with the commercial license agreement provided with the
 ** Software or, alternatively, in accordance with the terms contained in
 ** a written agreement between you and The Qt Company. For licensing terms
 ** and conditions see https://www.qt.io/terms-conditions. For further
 ** information use the contact form at https://www.qt.io/contact-us.
 **
 ** GNU Lesser General Public License Usage
 ** Alternatively, this file may be used under the terms of the GNU Lesser
 ** General Public License version 3 as published by the Free Software
 ** Foundation and appearing in the file LICENSE.LGPL3 included in the
 ** packaging of this file. Please review the following information to
 ** ensure the GNU Lesser General Public License version 3 requirements
 ** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
 **
 ** GNU General Public License Usage
 ** Alternatively, this file may be used under the terms of the GNU
 ** General Public License version 2.0 or (at your option) the GNU General
 ** Public license version 3 or any later version approved by the KDE Free
 ** Qt Foundation. The licenses are as published by the Free Software
 ** Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
 ** included in the packaging of this file. Please review the following
 ** information to ensure the GNU General Public License requirements will
 ** be met: https://www.gnu.org/licenses/gpl-2.0.html and
 ** https://www.gnu.org/licenses/gpl-3.0.html.
 **
 ** $QT_END_LICENSE$
 **
 ****************************************************************************/


/**
 * Constructor.  
 * Calls widget->setEnabled(false)
 *
 * @param widget
 *     The QWidget
 */
WuQWidgetDisabler::WuQWidgetDisabler(QWidget* widget)
: CaretObject(),
m_widget(widget)
{
    CaretAssert(m_widget);
    
    m_savedEnabledState = m_widget->isEnabled();
    m_widget->setEnabled(false);
}

/**
 * Destructor.  Restores enabled status of the QObject
 * to what it was before the constructor rang.
 */
WuQWidgetDisabler::~WuQWidgetDisabler()
{
    m_widget->setEnabled(m_savedEnabledState);
}

