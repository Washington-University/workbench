
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
#include "FociFile.h"
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
 *
 * @param giftiLabelTable
 *    Label table being edited.
 * @param dialogTitle
 *    Title for the dialog.
 * @param parent
 *    Parent on which this dialog is displayed.
 */
GiftiLabelTableEditor::GiftiLabelTableEditor(GiftiLabelTable* giftiLabelTable,
                                             const AString& dialogTitle,
                                             QWidget* parent)
: WuQDialogModal(dialogTitle,
                 parent)
{
    initializeDialog(giftiLabelTable);
}

/**
 * \class caret::GiftiLabelTableEditor
 * \brief Dialog for editing a GIFTI lable table.
 *
 */

/**
 * Constructor.
 *
 * @param fociFile
 *    Foci file whose color table being edited.  As colors are edited,
 *    the assigned foci will have their color validity invalidated.
 * @param dialogTitle
 *    Title for the dialog.
 * @param parent
 *    Parent on which this dialog is displayed.
 */
GiftiLabelTableEditor::GiftiLabelTableEditor(FociFile* fociFile,
                                             const AString& dialogTitle,
                                             QWidget* parent)
: WuQDialogModal(dialogTitle,
                 parent)
{
    CaretAssert(fociFile);
    initializeDialog(fociFile->getColorTable());
    m_fociFile = fociFile;
}

/**
 * Destructor.
 */
GiftiLabelTableEditor::~GiftiLabelTableEditor()
{
    if (m_undoGiftiLabel != NULL) {
        delete m_undoGiftiLabel;
        m_undoGiftiLabel = NULL;
    }
}

void
GiftiLabelTableEditor::initializeDialog(GiftiLabelTable* giftiLabelTable)
{
    m_fociFile = NULL;
    
    CaretAssert(giftiLabelTable);
    m_giftiLableTable = giftiLabelTable;
    m_undoGiftiLabel = NULL;
    
    /*
     * List widget for editing labels.
     */
    m_labelSelectionListWidget = new QListWidget();
    m_labelSelectionListWidget->setSelectionMode(QListWidget::SingleSelection);
    QObject::connect(m_labelSelectionListWidget, SIGNAL(currentRowChanged(int)),
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
    m_colorEditorWidget = new ColorEditorWidget();
    QObject::connect(m_colorEditorWidget, SIGNAL(colorChanged(const float*)),
                     this, SLOT(colorEditorColorChanged(const float*)));
    
    /*
     * Label name line edit
     */
    QLabel* nameLabel = new QLabel("Name: ");
    m_labelNameLineEdit = new QLineEdit();
    QObject::connect(m_labelNameLineEdit, SIGNAL(textEdited(const QString&)),
                     this, SLOT(labelNameLineEditTextEdited(const QString&)));
    WuQtUtilities::setToolTipAndStatusTip(m_labelNameLineEdit,
                                          "Edit the name");
    QHBoxLayout* nameLayout = new QHBoxLayout();
    WuQtUtilities::setLayoutMargins(nameLayout, 2, 2);
    nameLayout->addWidget(nameLabel, 0);
    nameLayout->addWidget(m_labelNameLineEdit, 100);
    
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
    layout->addWidget(m_labelSelectionListWidget);
    layout->addLayout(buttonsLayout);
    layout->addWidget(WuQtUtilities::createHorizontalLineWidget());
    layout->addLayout(nameLayout);
    layout->addWidget(m_colorEditorWidget);
    
    setCentralWidget(widget);
    
    m_editingGroup = new WuQWidgetObjectGroup(this);
    m_editingGroup->add(undoPushButton);
    m_editingGroup->add(deletePushButton);
    m_editingGroup->add(nameLabel);
    m_editingGroup->add(m_labelNameLineEdit);
    m_editingGroup->add(m_colorEditorWidget);
    
    loadLabels("", false);
    
    setOkButtonText("Close");
    setCancelButtonText("");
    
    /*
     * No auto default button processing (Qt highlights button)
     */
    disableAutoDefaultForAllPushButtons();    
}

/**
 * Called when name line edit text is changed.
 * @param text
 *    Text currently in the line edit.
 */
void 
GiftiLabelTableEditor::labelNameLineEditTextEdited(const QString& text)
{
    QListWidgetItem* selectedItem = m_labelSelectionListWidget->currentItem();
    if (selectedItem != NULL) {
        selectedItem->setText(text);
    }
    GiftiLabel* gl = getSelectedLabel();
    if (gl != NULL) {
        gl->setName(text);
    }
    m_lastSelectedLabelName = text;
}

/**
 * @return
 *   The last name that was selected.
 */
AString 
GiftiLabelTableEditor::getLastSelectedLabelName() const
{
    return m_lastSelectedLabelName;
}

/**
 * Select the label with the given name.
 * @param labelName
 *   Name of label that is to be selected.
 */
void 
GiftiLabelTableEditor::selectLabelWithName(const AString& labelName)
{
    QList<QListWidgetItem*> itemsWithLabelName = m_labelSelectionListWidget->findItems(labelName,
                                                                                           Qt::MatchExactly);
    if (itemsWithLabelName.empty() == false) {
        m_labelSelectionListWidget->setCurrentItem(itemsWithLabelName.at(0));
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
    if (m_undoGiftiLabel != NULL) {
        delete m_undoGiftiLabel;
        m_undoGiftiLabel = NULL;
    }
    
    bool isEditingAllowed = false;
    GiftiLabel* gl = getSelectedLabel();
    if (gl != NULL) {
        const bool isUnassignedLabel = (gl->getKey() == m_giftiLableTable->getUnassignedLabelKey());
        float rgba[4];
        gl->getColor(rgba);
        m_colorEditorWidget->setColor(rgba);
        m_labelNameLineEdit->setText(gl->getName());
        
        m_lastSelectedLabelName = gl->getName();
        
        if (isUnassignedLabel) {
            m_undoGiftiLabel = NULL;
        }
        else {
            m_undoGiftiLabel = new GiftiLabel(*gl);
            isEditingAllowed = true;
        }
    }
    else {
        m_lastSelectedLabelName = "";
    }
    
    m_editingGroup->setEnabled(isEditingAllowed);
}

/**
 * Called when a change is made in the color editor.
 * @param rgba
 *    New RGBA values.
 */
void 
GiftiLabelTableEditor::colorEditorColorChanged(const float* rgba)
{
    QListWidgetItem* selectedItem = m_labelSelectionListWidget->currentItem();
    if (selectedItem != NULL) {
        setWidgetItemIconColor(selectedItem, rgba);
    }
    GiftiLabel* gl = getSelectedLabel();
    if (gl != NULL) {
        gl->setColor(rgba);
        
        if (m_fociFile != NULL) {
            m_fociFile->invalidateAllAssigndColors();
        }
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
    m_labelSelectionListWidget->blockSignals(true);
    
    int32_t previousSelectedIndex = -1;
    if (usePreviouslySelectedIndex) {
        previousSelectedIndex = m_labelSelectionListWidget->currentRow();
    }
    int32_t selectedKey = -1;
    GiftiLabel* selectedLabel = getSelectedLabel();
    if (selectedLabel != NULL) {
        selectedKey = selectedLabel->getKey();
    }
    if (selectedName.isEmpty() == false) {
        selectedKey = m_giftiLableTable->getLabelKeyFromName(selectedName);
    }
    
    m_labelSelectionListWidget->clear();
    int defaultIndex = -1;
    
    std::vector<int32_t> keys = m_giftiLableTable->getLabelKeysSortedByName();
    for (std::vector<int32_t>::iterator keyIterator = keys.begin();
         keyIterator != keys.end();
         keyIterator++) {
        const int32_t key = *keyIterator;
        const GiftiLabel* gl = m_giftiLableTable->getLabel(key);
        float rgba[4];
        gl->getColor(rgba);
        
        QListWidgetItem* colorItem = new QListWidgetItem(gl->getName());
        setWidgetItemIconColor(colorItem, rgba);
        
        colorItem->setData(Qt::UserRole, 
                           qVariantFromValue((void*)gl));
        m_labelSelectionListWidget->addItem(colorItem);
        
        if (selectedKey == key) {
            defaultIndex = m_labelSelectionListWidget->count() - 1;
        }
    }
    
    if (usePreviouslySelectedIndex) {
        defaultIndex = previousSelectedIndex;
        if (defaultIndex >= m_labelSelectionListWidget->count()) {
            defaultIndex--;
        }
    }
    
    if (defaultIndex < 0) {
        if (m_labelSelectionListWidget->count() > 0) {
            defaultIndex = 0;
        }
    }
    
    m_labelSelectionListWidget->blockSignals(false);
    
    if (defaultIndex >= 0) {        
        m_labelSelectionListWidget->setCurrentRow(defaultIndex);
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
    
    QListWidgetItem* selectedItem = m_labelSelectionListWidget->currentItem();
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
        if (m_giftiLableTable->getLabel(testName) == NULL) {
            name = testName;
            break;
        }
    }
    
    float red   = 0.0;
    float green = 0.0;
    float blue  = 0.0;
    float alpha = 1.0;
    m_giftiLableTable->addLabel(name,
                                    red,
                                    green,
                                    blue,
                                    alpha);
    
    loadLabels(name, false);
    
    m_labelNameLineEdit->selectAll();
//    m_labelNameLineEdit->grabKeyboard();
//    m_labelNameLineEdit->grabMouse();
    m_labelNameLineEdit->setFocus();
}

/**
 * Called to undo changes to selected label.
 */
void 
GiftiLabelTableEditor::undoButtonClicked()
{
    if (m_undoGiftiLabel != NULL) {
        labelNameLineEditTextEdited(m_undoGiftiLabel->getName());
        float rgba[4];
        m_undoGiftiLabel->getColor(rgba);
        colorEditorColorChanged(rgba);
        listWidgetLabelSelected(-1);
    }
}

/**
 * Called to delete the label.
 */
void 
GiftiLabelTableEditor::deleteButtonClicked()
{
    GiftiLabel* gl = getSelectedLabel();
    if (gl != NULL) {
        if (WuQMessageBox::warningOkCancel(this,
                                           "Delete " + gl->getName())) {
            m_giftiLableTable->deleteLabel(gl);
            loadLabels("", true);
        }
    }
    
}
