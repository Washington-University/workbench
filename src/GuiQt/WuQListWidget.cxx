
/*LICENSE_START*/
/*
 * Copyright 2012 Washington University,
 * All rights reserved.
 *
 * Connectome DB and Connectome Workbench are part of the integrated Connectome 
 * Informatics Platform.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    * Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *    * Neither the names of Washington University nor the
 *      names of its contributors may be used to endorse or promote products
 *      derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR  
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
 * SUBSTITUTE GOODS OR  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
 * POSSIBILITY OF SUCH DAMAGE.
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
