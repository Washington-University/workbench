
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
#include <QColorDialog>
#include <QInputDialog>
#include <QListWidget>
#include <QListWidgetItem>
#include <QVBoxLayout>
#include <QToolButton>

#include "CaretAssert.h"
#include "GiftiLabel.h"
#include "GiftiLabelTable.h"
#include "WuQMessageBox.h"
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
    CaretAssert(giftiLableTable);
    this->giftiLableTable = giftiLableTable;
    
    /*
     * List widget for editing labels.
     */
    this->labelSelectionListWidget = new QListWidget();
    
    /*
     * New color button.
     */
    this->newAction = WuQtUtilities::createAction("New...",
                                                       "Create a new entry",
                                                       this,
                                                       this,
                                                       SLOT(newButtonClicked()));
    QToolButton* newToolButton = new QToolButton();
    newToolButton->setDefaultAction(this->newAction);
    
    /*
     * Edit name button.
     */
    this->editNameAction = WuQtUtilities::createAction("Edit Name...",
                                                       "Edit the selected name",
                                                       this,
                                                       this,
                                                       SLOT(editNameButtonClicked()));
    QToolButton* editNameToolButton = new QToolButton();
    editNameToolButton->setDefaultAction(this->editNameAction);
    
    /*
     * Edit color button.
     */
    this->editColorAction = WuQtUtilities::createAction("Edit Color...",
                                                       "Edit the selected color",
                                                       this,
                                                       this,
                                                       SLOT(editColorButtonClicked()));
    QToolButton* editColorToolButton = new QToolButton();
    editColorToolButton->setDefaultAction(this->editColorAction);
    
    /*
     * Delete color button.
     */
    this->deleteAction = WuQtUtilities::createAction("Delete...",
                                                       "Delete the selected entry",
                                                       this,
                                                       this,
                                                       SLOT(deleteButtonClicked()));
    QToolButton* deleteToolButton = new QToolButton();
    deleteToolButton->setDefaultAction(this->deleteAction);
    
    /*
     * Layout for buttons
     */
    QHBoxLayout* buttonsLayout = new QHBoxLayout();
    buttonsLayout->addWidget(newToolButton);
    buttonsLayout->addStretch();
    buttonsLayout->addWidget(editNameToolButton);
    buttonsLayout->addStretch();
    buttonsLayout->addWidget(editColorToolButton);
    buttonsLayout->addStretch();
    buttonsLayout->addWidget(deleteToolButton);
    
    /*
     * Layout items in dialog
     */
    QWidget* widget = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(widget);
    layout->addWidget(this->labelSelectionListWidget);
    layout->addLayout(buttonsLayout);
    
    this->setCentralWidget(widget);
    
    this->loadLabels();
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
    int32_t selectedKey = -1;
    GiftiLabel* selectedLabel = this->getSelectedLabel();
    if (selectedLabel != NULL) {
        selectedKey = selectedLabel->getKey();
    }
    
    this->labelSelectionListWidget->clear();
    int defaultIndex = -1;
    
    std::set<int32_t> keys = this->giftiLableTable->getKeys();
    for (std::set<int32_t>::iterator keyIterator = keys.begin();
         keyIterator != keys.end();
         keyIterator++) {
        const int32_t key = *keyIterator;
        const GiftiLabel* gl = this->giftiLableTable->getLabel(key);
        float rgba[4];
        gl->getColor(rgba);
        
        QColor color;
        color.setRedF(rgba[0]);
        color.setGreenF(rgba[1]);
        color.setBlueF(rgba[2]);
        color.setAlphaF(1.0);
        QPixmap pixmap(12, 12);
        pixmap.fill(color);
        QIcon colorIcon(pixmap);
        QListWidgetItem* colorItem = new QListWidgetItem(colorIcon,
                                                         gl->getName());
        colorItem->setData(Qt::UserRole, 
                           qVariantFromValue((void*)gl));
        this->labelSelectionListWidget->addItem(colorItem);
        
        if (selectedKey == key) {
            defaultIndex = this->labelSelectionListWidget->count() - 1;
        }
    }
    
    if (defaultIndex >= 0) {
        this->labelSelectionListWidget->setCurrentRow(defaultIndex);
    }
}

/**
 * @return The selected label or NULL if
 * no label is selected.
 */
GiftiLabel* 
GiftiLabelTableEditor::getSelectedLabel()
{
    GiftiLabel* gl = NULL;
    
    QListWidgetItem* selectedItem = this->labelSelectionListWidget->currentItem();
    if (selectedItem != NULL) {
        void* pointer = selectedItem->data(Qt::UserRole).value<void*>();
        gl = (GiftiLabel*)pointer;
    }
    
    return gl;
}


/**
 * Called to create a new label.
 */
void 
GiftiLabelTableEditor::newButtonClicked()
{
    bool ok = false;
    const AString name = QInputDialog::getText(this, 
                                               "New Name", 
                                               "New Name",
                                               QLineEdit::Normal,
                                               "",
                                               &ok);
    if (ok && 
        (name.isEmpty() == false)) {
        float red   = 0.0;
        float green = 0.0;
        float blue  = 0.0;
        float alpha = 1.0;
        this->giftiLableTable->addLabel(name.trimmed(),
                                        red,
                                        green,
                                        blue,
                                        alpha);
        
        this->loadLabels();
    }
}

/**
 * Called to delete the label.
 */
void 
GiftiLabelTableEditor::deleteButtonClicked()
{
    GiftiLabel* gl = this->getSelectedLabel();
    if (gl != NULL) {
        if (WuQMessageBox::warningOkCancel(this,
                                           "Delete " + gl->getName())) {
            this->giftiLableTable->deleteLabel(gl);
            this->loadLabels();
        }
    }
    
}

/** 
 * Called to edit the lablel name.
 */
void 
GiftiLabelTableEditor::editNameButtonClicked()
{
    GiftiLabel* gl = this->getSelectedLabel();
    if (gl != NULL) {
        bool ok = false;
        const AString name = QInputDialog::getText(this, 
                                                   "New Name", 
                                                   "New Name",
                                                   QLineEdit::Normal,
                                                   gl->getName(),
                                                   &ok);
        if (ok && 
            (name.isEmpty() == false)) {
            gl->setName(name);
            this->loadLabels();
        }
    }
}

/** 
 * Called to edit the lablel color.
 */
void 
GiftiLabelTableEditor::editColorButtonClicked()
{
    GiftiLabel* gl = this->getSelectedLabel();
    if (gl != NULL) {
        QColor inputColor;
        inputColor.setRedF(gl->getRed());
        inputColor.setGreenF(gl->getGreen());
        inputColor.setBlueF(gl->getBlue());
        inputColor.setAlphaF(gl->getAlpha());
        
        QColor color = QColorDialog::getColor(inputColor, 
                                              this, 
                                              gl->getName(),
                                              (QColorDialog::DontUseNativeDialog
                                               | QColorDialog::ShowAlphaChannel));
        if (color.isValid()) {
            float rgba[4] = {
                color.redF(),
                color.greenF(),
                color.blueF(),
                color.alphaF()
            };
            gl->setColor(rgba);
            
            this->loadLabels();
        }
    }
}

