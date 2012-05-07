
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

#include <QVBoxLayout>

#define __CARET_FILE_DIALOG_EXTENDABLE_DECLARE__
#include "CaretFileDialogExtendable.h"
#undef __CARET_FILE_DIALOG_EXTENDABLE_DECLARE__

#include "CaretAssert.h"
#include "CaretFileDialog.h"
using namespace caret;


    
/**
 * \class caret::CaretFileDialogExtendable 
 * \brief A File Dialog that can have a widget added to it.
 *
 * Embeds a QFileDialog inside a dialog so that a widget
 * can be inserted below the FileDialog.  All public methods
 * from QFileDialog are duplicated and go straight to 
 * the embedded QFileDialog.
 */

/**
 * Constructor.
 */
CaretFileDialogExtendable::CaretFileDialogExtendable(QWidget* parent,
                                                     Qt::WindowFlags f)
{
    m_caretFileDialog = new CaretFileDialog(parent,
                                            f);

    createDialog();
}
/**
 * Constructor.
 */
CaretFileDialogExtendable::CaretFileDialogExtendable(QWidget* parent,
                                                     const QString& caption,
                                                     const QString& directory,
                                                     const QString& filter)
{
    m_caretFileDialog = new CaretFileDialog(parent,
                                            caption,
                                            directory,
                                            filter);
    
    createDialog();
}


/**
 * Destructor.
 */
CaretFileDialogExtendable::~CaretFileDialogExtendable()
{
    
}

/**
 * Create the dialog.
 */
void 
CaretFileDialogExtendable::createDialog()
{
    QObject::connect(m_caretFileDialog, SIGNAL(currentChanged(const QString&)),
                     this, SIGNAL(currentChanged(const QString&)));
    
    QObject::connect(m_caretFileDialog, SIGNAL( directoryEntered(const QString&)),
                     this, SIGNAL( directoryEntered(const QString&)));
    
    QObject::connect(m_caretFileDialog, SIGNAL(fileSelected(const QString&)),
                     this, SIGNAL(fileSelected(const QString&)));
    
    QObject::connect(m_caretFileDialog, SIGNAL(filesSelected(const QStringList&)),
                     this, SIGNAL(filesSelected(const QStringList&)));
    
    QObject::connect(m_caretFileDialog, SIGNAL(filterSelected(const QString&)),
                     this, SIGNAL(filterSelected(const QString&)));
    
    QWidget* newWidget = new QWidget(this);
    QVBoxLayout* m_dialogLayout = new QVBoxLayout(newWidget);
    m_dialogLayout->addWidget(m_caretFileDialog);
}

/**
 * Add a widget at the bottom to this file dialog.
 * @param widget
 *   Widget that is added.
 */
void 
CaretFileDialogExtendable::addWidget(QWidget* widget)
{
    CaretAssert(widget);
    m_dialogLayout->addWidget(widget);
}


