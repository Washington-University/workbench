
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

#define __WU_Q_LIST_WIDGET_DECLARE__
#include "WuQListWidget.h"
#undef __WU_Q_LIST_WIDGET_DECLARE__

using namespace caret;


    
/**
 * \class caret::WuQListWidget 
 * \brief List widget that allows drag and drop to itself.
 *
 * QListWidget supports drag and drop but also requires
 * that one subclass QListWidget to receive the drop event.
 * This derivation of QListWidget receives the drop event
 * and emits a signal to indicate an item has been dropped.
 * 
 * This class can be used instead of QListWidget so that 
 * one can be notified, via a signal, that an item has been
 * dropped without having to subclass QListWidget.
 */

/**
 * Constructor.
 * @param parent
 *    The optional parent widget.
 */
WuQListWidget::WuQListWidget(QWidget* parent)
: QListWidget(parent)
{    
    /*
     * Enable dragging and dropping within this list widget.
     */
    setSelectionMode(QListWidget::SingleSelection);
    setDragEnabled(true);
    setDragDropMode(QListWidget::InternalMove);
}

/**
 * Destructor.
 */
WuQListWidget::~WuQListWidget()
{
    
}

/**
 * Receives drop events then emits the itemWasDropped signal.
 * @param e
 *    The drop event.
 */
void 
WuQListWidget::dropEvent(QDropEvent* e)
{
    QListWidget::dropEvent(e);
    emit itemWasDropped();
}
