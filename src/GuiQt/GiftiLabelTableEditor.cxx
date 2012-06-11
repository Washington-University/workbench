
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

#include <iostream>

#define __GIFTI_LABEL_TABLE_EDITOR_DECLARE__
#include "GiftiLabelTableEditor.h"
#undef __GIFTI_LABEL_TABLE_EDITOR_DECLARE__

#include <QAction>
#include <QInputDialog>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QListWidgetItem>
#include <QPushButton>
#include <QVBoxLayout>
#include <QToolButton>

#include "CaretAssert.h"
#include "ColorEditorWidget.h"
#include "GiftiLabel.h"
#include "GiftiLabelTable.h"
#include "WuQMessageBox.h"
#include "WuQtUtilities.h"
#include "WuQWidgetObjectGroup.h"

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
    this->undoGiftiLabel = NULL;
    
    /*
     * List widget for editing labels.
     */
    this->labelSelectionListWidget = new QListWidget();
    this->labelSelectionListWidget->setSelectionMode(QListWidget::SingleSelection);
    QObject::connect(this->labelSelectionListWidget, SIGNAL(currentRowChanged(int)),
                     this, SLOT(listWidgetLabelSelected(int)));
    
    /*
     * New color button.
     */
    QPushButton* newPushButton = WuQtUtilities::createPushButton("New",
                                                       "Create a new entry",
                                                       this,
                                                       SLOT(newButtonClicked()));
    
    /*
     * Undo Edit button.
     */
    QPushButton* undoPushButton = WuQtUtilities::createPushButton("Undo Edit",
                                                  "Create a new entry",
                                                  this,
                                                  SLOT(undoButtonClicked()));
    
    /*
     * Delete button.
     */
    QPushButton* deletePushButton = WuQtUtilities::createPushButton("Delete",
                                                       "Delete the selected entry",
                                                       this,
                                                       SLOT(deleteButtonClicked()));
    
    /*
     * Color editor widget
     */
    this->colorEditorWidget = new ColorEditorWidget();
    QObject::connect(this->colorEditorWidget, SIGNAL(colorChanged(const float*)),
                     this, SLOT(colorEditorColorChanged(const float*)));
    
    /*
     * Label name line edit
     */
    QLabel* nameLabel = new QLabel("Name: ");
    this->labelNameLineEdit = new QLineEdit();
    QObject::connect(this->labelNameLineEdit, SIGNAL(textEdited(const QString&)),
                     this, SLOT(labelNameLineEditTextEdited(const QString&)));
    WuQtUtilities::setToolTipAndStatusTip(this->labelNameLineEdit, 
                                          "Edit the name");
    QHBoxLayout* nameLayout = new QHBoxLayout();
    WuQtUtilities::setLayoutMargins(nameLayout, 2, 2);
    nameLayout->addWidget(nameLabel, 0);
    nameLayout->addWidget(this->labelNameLineEdit, 100);
    
    /*
     * Layout for buttons
     */
    QHBoxLayout* buttonsLayout = new QHBoxLayout();
    buttonsLayout->addWidget(newPushButton);
    buttonsLayout->addStretch();
    buttonsLayout->addWidget(undoPushButton);
    buttonsLayout->addStretch();
    buttonsLayout->addWidget(deletePushButton);
    
    /*
     * Layout items in dialog
     */
    QWidget* widget = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(widget);
    WuQtUtilities::setLayoutMargins(layout, 4, 2);
    layout->addWidget(this->labelSelectionListWidget);
    layout->addLayout(buttonsLayout);
    layout->addWidget(WuQtUtilities::createHorizontalLineWidget());
    layout->addLayout(nameLayout);
    layout->addWidget(this->colorEditorWidget);
    
    this->setCentralWidget(widget);
    
    this->editingGroup = new WuQWidgetObjectGroup(this);
    this->editingGroup->add(undoPushButton);
    this->editingGroup->add(deletePushButton);
    this->editingGroup->add(nameLabel);
    this->editingGroup->add(this->labelNameLineEdit);
    this->editingGroup->add(this->colorEditorWidget);
    
    this->loadLabels("", false);
    
    this->setOkButtonText("Close");
    this->setCancelButtonText("");
    
    /*
     * No auto default button processing (Qt highlights button)
     */
    disableAutoDefaultForAllPushButtons();
}

/**
 * Destructor.
 */
GiftiLabelTableEditor::~GiftiLabelTableEditor()
{
    if (this->undoGiftiLabel != NULL) {
        delete this->undoGiftiLabel;
        this->undoGiftiLabel = NULL;
    }
}

/**
 * Called when name line edit text is changed.
 * @param text
 *    Text currently in the line edit.
 */
void 
GiftiLabelTableEditor::labelNameLineEditTextEdited(const QString& text)
{
    QListWidgetItem* selectedItem = this->labelSelectionListWidget->currentItem();
    if (selectedItem != NULL) {
        selectedItem->setText(text);
    }
    GiftiLabel* gl = this->getSelectedLabel();
    if (gl != NULL) {
        gl->setName(text);
    }
    this->lastSelectedLabelName = text;
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
    if (this->undoGiftiLabel != NULL) {
        delete this->undoGiftiLabel;
        this->undoGiftiLabel = NULL;
    }
    
    bool isEditingAllowed = false;
    GiftiLabel* gl = this->getSelectedLabel();
    if (gl != NULL) {
        const bool isUnassignedLabel = (gl->getKey() == this->giftiLableTable->getUnassignedLabelKey());
        float rgba[4];
        gl->getColor(rgba);
        this->colorEditorWidget->setColor(rgba);
        this->labelNameLineEdit->setText(gl->getName());
        
        this->lastSelectedLabelName = gl->getName();
        
        if (isUnassignedLabel) {
            this->undoGiftiLabel = NULL;
        }
        else {
            this->undoGiftiLabel = new GiftiLabel(*gl);
            isEditingAllowed = true;
        }
    }
    else {
        this->lastSelectedLabelName = "";
    }
    
    this->editingGroup->setEnabled(isEditingAllowed);
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
    
    std::vector<int32_t> keys = this->giftiLableTable->getLabelKeysSortedByName();
    for (std::vector<int32_t>::iterator keyIterator = keys.begin();
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
    QPixmap pixmap(14, 14);
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
    /*
     * Make sure default name does not already exist
     */
    AString name = "NewName_";
    for (int i = 1; i < 10000; i++) {
        const AString testName = name + QString::number(i);
        if (this->giftiLableTable->getLabel(testName) == NULL) {
            name = testName;
            break;
        }
    }
    
    float red   = 0.0;
    float green = 0.0;
    float blue  = 0.0;
    float alpha = 1.0;
    this->giftiLableTable->addLabel(name,
                                    red,
                                    green,
                                    blue,
                                    alpha);
    
    this->loadLabels(name, false);
    
    this->labelNameLineEdit->selectAll();
//    this->labelNameLineEdit->grabKeyboard();
//    this->labelNameLineEdit->grabMouse();
    this->labelNameLineEdit->setFocus();
}

/**
 * Called to undo changes to selected label.
 */
void 
GiftiLabelTableEditor::undoButtonClicked()
{
    if (this->undoGiftiLabel != NULL) {
        labelNameLineEditTextEdited(this->undoGiftiLabel->getName());
        float rgba[4];
        this->undoGiftiLabel->getColor(rgba);
        this->colorEditorColorChanged(rgba);
        this->listWidgetLabelSelected(-1);
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
