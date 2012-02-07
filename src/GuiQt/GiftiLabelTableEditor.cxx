
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
#include <QInputDialog>
#include <QListWidget>
#include <QListWidgetItem>
#include <QVBoxLayout>
#include <QToolButton>

#include "CaretAssert.h"
#include "ColorEditorWidget.h"
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
    QObject::connect(this->labelSelectionListWidget, SIGNAL(currentRowChanged(int)),
                     this, SLOT(listWidgetLabelSelected(int)));
    
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
     * Color editor widget
     */
    this->colorEditorWidget = new ColorEditorWidget();
    QObject::connect(this->colorEditorWidget, SIGNAL(colorChanged(const float*)),
                     this, SLOT(colorEditorColorChanged(const float*)));
    
    /*
     * Layout for buttons
     */
    QHBoxLayout* buttonsLayout = new QHBoxLayout();
    buttonsLayout->addWidget(newToolButton);
    buttonsLayout->addStretch();
    buttonsLayout->addWidget(editNameToolButton);
    buttonsLayout->addStretch();
    buttonsLayout->addWidget(deleteToolButton);
    
    /*
     * Layout items in dialog
     */
    QWidget* widget = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(widget);
    layout->addWidget(this->labelSelectionListWidget);
    layout->addLayout(buttonsLayout);
    layout->addWidget(this->colorEditorWidget);
    
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
 * @return
 *   The last name that was selected.
 */
AString 
GiftiLabelTableEditor::getLastSelectedLabelName() const
{
    return this->lastSelectedLabelName;
}

/**
 * Select the label with the given name.
 * @param labelName
 *   Name of label that is to be selected.
 */
void 
GiftiLabelTableEditor::selectLabelWithName(const AString& labelName)
{
    QList<QListWidgetItem*> itemsWithLabelName = this->labelSelectionListWidget->findItems(labelName,
                                                                                           Qt::MatchExactly);
    if (itemsWithLabelName.empty() == false) {
        this->labelSelectionListWidget->setCurrentItem(itemsWithLabelName.at(0));
    }
}

/**
 * Called when a label in the list widget is selected.
 * @param row
 *    Row of label selected.
 */
void 
GiftiLabelTableEditor::listWidgetLabelSelected(int /*row*/)
{
    GiftiLabel* gl = this->getSelectedLabel();
    if (gl != NULL) {
        float rgba[4];
        gl->getColor(rgba);
        this->colorEditorWidget->setColor(rgba);
        
        this->lastSelectedLabelName = gl->getName();
    }
    else {
        this->lastSelectedLabelName = "";
    }
}

/**
 * Called when a change is made in the color editor.
 * @param rgba
 *    New RGBA values.
 */
void 
GiftiLabelTableEditor::colorEditorColorChanged(const float* rgba)
{
    QListWidgetItem* selectedItem = this->labelSelectionListWidget->currentItem();
    if (selectedItem != NULL) {
        this->setWidgetItemIconColor(selectedItem, rgba);
    }
    GiftiLabel* gl = this->getSelectedLabel();
    if (gl != NULL) {
        gl->setColor(rgba);
    }
}

/**
 * Load labels into the list widget.
 * 
 * @param selectedName
 *    If not empty, select the label with this name
 * @param usePreviouslySelectedIndex
 *    If true, use selected index prior to reloading list widget.
 */
void 
GiftiLabelTableEditor::loadLabels(const AString& selectedName,
                                  const bool usePreviouslySelectedIndex)
{
    this->labelSelectionListWidget->blockSignals(true);
    
    int32_t previousSelectedIndex = -1;
    if (usePreviouslySelectedIndex) {
        previousSelectedIndex = this->labelSelectionListWidget->currentRow();
    }
    int32_t selectedKey = -1;
    GiftiLabel* selectedLabel = this->getSelectedLabel();
    if (selectedLabel != NULL) {
        selectedKey = selectedLabel->getKey();
    }
    if (selectedName.isEmpty() == false) {
        selectedKey = this->giftiLableTable->getLabelKeyFromName(selectedName);
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
        
        QListWidgetItem* colorItem = new QListWidgetItem(gl->getName());
        this->setWidgetItemIconColor(colorItem, rgba);
        
        colorItem->setData(Qt::UserRole, 
                           qVariantFromValue((void*)gl));
        this->labelSelectionListWidget->addItem(colorItem);
        
        if (selectedKey == key) {
            defaultIndex = this->labelSelectionListWidget->count() - 1;
        }
    }
    
    if (usePreviouslySelectedIndex) {
        defaultIndex = previousSelectedIndex;
        if (defaultIndex >= this->labelSelectionListWidget->count()) {
            defaultIndex--;
        }
    }
    
    if (defaultIndex < 0) {
        if (this->labelSelectionListWidget->count() > 0) {
            defaultIndex = 0;
        }
    }
    
    this->labelSelectionListWidget->blockSignals(false);
    
    if (defaultIndex >= 0) {
        this->labelSelectionListWidget->setCurrentRow(defaultIndex);
    }
}

/**
 * Set the Icon color for the item.
 * @param item
 *    The list widget item.
 * @param rgba
 *    RGBA values.
 */
void 
GiftiLabelTableEditor::setWidgetItemIconColor(QListWidgetItem* item,
                                              const float rgba[4])
{
    QColor color;
    color.setRedF(rgba[0]);
    color.setGreenF(rgba[1]);
    color.setBlueF(rgba[2]);
    color.setAlphaF(1.0);
    QPixmap pixmap(12, 12);
    pixmap.fill(color);
    QIcon colorIcon(pixmap);
    
    item->setIcon(colorIcon);
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
    const AString nameEntered = QInputDialog::getText(this, 
                                               "New Name", 
                                               "New Name",
                                               QLineEdit::Normal,
                                               "",
                                               &ok);
    if (ok && 
        (nameEntered.isEmpty() == false)) {
        float red   = 0.0;
        float green = 0.0;
        float blue  = 0.0;
        float alpha = 1.0;
        const AString& name = nameEntered.trimmed();
        this->giftiLableTable->addLabel(name,
                                        red,
                                        green,
                                        blue,
                                        alpha);
        
        this->loadLabels(name);
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
            this->loadLabels("", true);
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
        const AString nameEntered = QInputDialog::getText(this, 
                                                   "New Name", 
                                                   "New Name",
                                                   QLineEdit::Normal,
                                                   gl->getName(),
                                                   &ok);
        if (ok && 
            (nameEntered.isEmpty() == false)) {
            const AString name = nameEntered.trimmed();
            gl->setName(name);
            this->loadLabels(name);
        }
    }
}

