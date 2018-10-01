
/*LICENSE_START*/
/*
 *  Copyright (C) 2018 Washington University School of Medicine
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

#define __WU_Q_TAB_BAR_DECLARE__
#include "WuQTabBar.h"
#undef __WU_Q_TAB_BAR_DECLARE__

#include "CaretAssert.h"
using namespace caret;


    
/**
 * \class caret::WuQTabBar 
 * \brief Overrides and adds capabilities to QTabBar
 * \ingroup GuiQt
 */

/**
 * Constructor.
 */
WuQTabBar::WuQTabBar(QWidget* parent)
: QTabBar(parent)
{
    
}

/**
 * Destructor.
 */
WuQTabBar::~WuQTabBar()
{
}

/**
 * Overrides parent widget's method to emit mousePressedSignal.
 *
 * @param event
 *     The mouse event.
 */
void
WuQTabBar::mousePressEvent(QMouseEvent* event)
{
    emit mousePressedSignal();
    
    QTabBar::mousePressEvent(event);
}

/**
 * Overrides parent widget's method to emit mouseReleaseSignal.
 *
 * @param event
 *     The mouse event.
 */
void
WuQTabBar::mouseReleaseEvent(QMouseEvent* event)
{
    emit mouseReleasedSignal();
    
    QTabBar::mouseReleaseEvent(event);
}

