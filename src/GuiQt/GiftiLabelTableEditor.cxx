
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

#define __GIFTI_LABEL_TABLE_EDITOR_DECLARE__
#include "GiftiLabelTableEditor.h"
#undef __GIFTI_LABEL_TABLE_EDITOR_DECLARE__

#include <QAction>
#include <QListWidget>
#include <QVBoxLayout>
#include <QToolButton>

#include "CaretAssert.h"
#include "GiftiLabelTable.h"
#include "WuQtUtilities.h"

using namespace caret;


    
/**
 * \class caret::GiftiLabelTableEditor 
 * \brief Dialog for editing a GIFTI lable table.
 *
 */
/**
 * Constructor.
 */
GiftiLabelTableEditor::GiftiLabelTableEditor(GiftiLabelTable* giftiLableTable,
                                             const AString& dialogTitle,
                                             QWidget* parent)
: WuQDialogModal(dialogTitle,
                 parent)
{
    //CaretAssert(giftiLableTable);
    this->giftiLableTable = giftiLableTable;
    
    /*
     * List widget for editing labels.
     */
    this->labelSelectionListWidget = new QListWidget();
    
    /*
     * New color button.
     */
    this->newLabelAction = WuQtUtilities::createAction("New...",
                                                       "Create a new label",
                                                       this,
                                                       this,
                                                       SLOT(newLabelButtonClicked()));
    QToolButton* newLabelToolButton = new QToolButton();
    newLabelToolButton->setDefaultAction(this->newLabelAction);
    
    /*
     * Edit color button.
     */
    this->editLabelAction = WuQtUtilities::createAction("Edit...",
                                                       "Edit the selected label",
                                                       this,
                                                       this,
                                                       SLOT(editLabelButtonClicked()));
    QToolButton* editLabelToolButton = new QToolButton();
    editLabelToolButton->setDefaultAction(this->editLabelAction);
    
    /*
     * Delete color button.
     */
    this->deleteLabelAction = WuQtUtilities::createAction("Delete...",
                                                       "Delete the selected label",
                                                       this,
                                                       this,
                                                       SLOT(deleteLabelButtonClicked()));
    QToolButton* deleteLabelToolButton = new QToolButton();
    deleteLabelToolButton->setDefaultAction(this->deleteLabelAction);
    
    /*
     * Layout for buttons
     */
    QHBoxLayout* buttonsLayout = new QHBoxLayout();
    buttonsLayout->addWidget(newLabelToolButton);
    buttonsLayout->addStretch();
    buttonsLayout->addWidget(editLabelToolButton);
    buttonsLayout->addStretch();
    buttonsLayout->addWidget(deleteLabelToolButton);
    
    /*
     * Layout items in dialog
     */
    QWidget* widget = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(widget);
    layout->addWidget(this->labelSelectionListWidget);
    layout->addLayout(buttonsLayout);
    
    this->setCentralWidget(widget);
}

/**
 * Destructor.
 */
GiftiLabelTableEditor::~GiftiLabelTableEditor()
{
    
}

/**
 * Load labels into the list widget.
 */
void 
GiftiLabelTableEditor::loadLabels()
{
    
    QPixmap pixmap(12, 12);
    pixmap.fill(Qt::red);
    QIcon colorIcon(pixmap);
    QListWidgetItem* colorItem = new QListWidgetItem(colorIcon,
                                                     "red");
    this->labelSelectionListWidget->addItem(colorItem);
    
    pixmap.fill(Qt::blue);
    QIcon colorIcon2(pixmap);
    QListWidgetItem* colorItem2 = new QListWidgetItem(colorIcon2,
                                                      "blue");
    this->labelSelectionListWidget->addItem(colorItem2);
}


/**
 * Called to create a new label.
 */
void 
GiftiLabelTableEditor::newLabelButtonClicked()
{
    
}

/**
 * Called to delete the label.
 */
void 
GiftiLabelTableEditor::deleteLabelButtonClicked()
{
    
}

/** 
 * Called to edit the lablel.
 */
void 
GiftiLabelTableEditor::editLabelButtonClicked()
{
    
}

