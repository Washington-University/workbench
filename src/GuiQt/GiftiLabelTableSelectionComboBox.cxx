
/*LICENSE_START*/
/*
 * Copyright 2013 Washington University,
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

#define __GIFTI_LABEL_TABLE_SELECTION_COMBO_BOX_DECLARE__
#include "GiftiLabelTableSelectionComboBox.h"
#undef __GIFTI_LABEL_TABLE_SELECTION_COMBO_BOX_DECLARE__

#include <QComboBox>

#include "CaretAssert.h"
#include "CaretLogger.h"
#include "GiftiLabel.h"
#include "GiftiLabelTable.h"
#include "GiftiLabelTableEditor.h"

using namespace caret;


    
/**
 * \class caret::GiftiLabelTableSelectionComboBox 
 * \brief Combo box for selection of a gifti label.
 * \ingroup GuiQt
 */

/**
 * Constructor.
 *
 * @param parent
 *    Parent of this object.
 */
GiftiLabelTableSelectionComboBox::GiftiLabelTableSelectionComboBox(QObject* parent)
: WuQWidget(parent)
{
    m_ignoreInsertedRowsFlag = true;
    
    m_comboBox = new QComboBox();
    
    
    QObject::connect(m_comboBox, SIGNAL(activated(int)),
                     this, SLOT(itemActivated(int)));
    QObject::connect(m_comboBox, SIGNAL(currentIndexChanged(int)),
                     this, SLOT(currentIndexChanged(int)));
    
    const bool allowEditingFlag = false;
    if (allowEditingFlag) {
        m_comboBox->setEditable(true);
        m_comboBox->setDuplicatesEnabled(false);
        m_comboBox->setInsertPolicy(QComboBox::InsertAlphabetically);
        QAbstractItemModel* model = m_comboBox->model();
        QObject::connect(model, SIGNAL(rowsInserted(const QModelIndex&,
                                                    int,
                                                    int)),
                         this, SLOT(rowsWereInserted(const QModelIndex,
                                                     int,
                                                     int)));
    }
}

/**
 * Destructor.
 */
GiftiLabelTableSelectionComboBox::~GiftiLabelTableSelectionComboBox()
{
    
}

/**
 * If the combo box is editable, this can be used to create the new
 * label.
 *
 * @param parent
 *    The parent model index.
 * @param start
 *    First new row index.
 * @param end
 *    Last new row index.
 */
void
GiftiLabelTableSelectionComboBox::rowsWereInserted(const QModelIndex& parent,
                  int start,
                  int end)
{
    if (m_ignoreInsertedRowsFlag) {
        return;
    }
    
    std::cout << "Rows were inserted " << start << " " << end << std::endl;
    for (int i = start; i <= end; i++) {
        const QString text = m_comboBox->itemText(i);
        std::cout << "New item is " << qPrintable(text) << std::endl;
        
        QVariant userData = m_comboBox->itemData(i);
        void* pointer = userData.value<void*>();
        if (pointer == NULL) {
            const int32_t key = m_giftiLabelTable->addLabel(text,
                                                            0.0f,
                                                            0.0f,
                                                            0.0f,
                                                            1.0f);
            GiftiLabel* label = m_giftiLabelTable->getLabel(key);
            
            QPixmap pm(10, 10);
            pm.fill(QColor::fromRgbF(label->getRed(),
                                     label->getGreen(),
                                     label->getBlue()));
            QIcon icon(pm);
            
            QVariant userData = qVariantFromValue((void*)label);
            m_comboBox->setItemData(i, userData);
            m_comboBox->setItemIcon(i, icon);
        }
    }
}

/**
 * Called when the add/edit button is clicked.
 */
void
GiftiLabelTableSelectionComboBox::addEditButtonClicked()
{
    
}

/**
 * Gets called when the index is changed by user or program code.
 *
 * @parma index
 *     Index of item.
 */
void
GiftiLabelTableSelectionComboBox::currentIndexChanged(int indx)
{
    std::cout << "Item changed: " << indx << std::endl;
    
    GiftiLabel* gl = getSelectedLabel();
    
    emit labelChanged(gl);
    
    if (gl != NULL) {
        emit labelKeyChanged(gl->getKey());
    }
    else {
        emit labelKeyChanged(-1);
    }
}

/**
 * Gets called when the user selects an item even if the selection
 * does not change.
 *
 * @parma index
 *     Index of item.
 */
void
GiftiLabelTableSelectionComboBox::itemActivated(int indx)
{
    std::cout << "Item activated: " << indx << std::endl;
    
    GiftiLabel* gl = getSelectedLabel();
    
    emit labelActivated(gl);
    
    if (gl != NULL) {
        emit labelKeyActivated(gl->getKey());
    }
    else {
        emit labelKeyActivated(-1);
    }
}

/**
 * Update the content of this control with the given lable table.
 *
 * @param giftiLabelTable
 *    The label table.
 */
void
GiftiLabelTableSelectionComboBox::updateContent(GiftiLabelTable* giftiLabelTable)
{
    m_ignoreInsertedRowsFlag = true;
    
    CaretAssert(giftiLabelTable);
    m_giftiLabelTable = giftiLabelTable;

    const AString selectedLabelName = m_comboBox->currentText();
    m_comboBox->clear();
    
    const std::set<int32_t> keySet = giftiLabelTable->getKeys();
    for (std::set<int32_t>::const_iterator iter = keySet.begin();
         iter != keySet.end();
         iter++) {
        const int32_t key = *iter;
        
        GiftiLabel* label = giftiLabelTable->getLabel(key);
        const AString labelName = label->getName();
        
        QPixmap pm(10, 10);
        pm.fill(QColor::fromRgbF(label->getRed(),
                                 label->getGreen(),
                                 label->getBlue()));
        QIcon icon(pm);

        QVariant userData = qVariantFromValue((void*)label);
        
        m_comboBox->addItem(icon,
                            labelName,
                            userData);
    }
    
    m_ignoreInsertedRowsFlag = false;
}

/**
 * Return the widget in this object.
 */
QWidget*
GiftiLabelTableSelectionComboBox::getWidget()
{
    return m_comboBox;
}

/**
 * @return The selected label (NULL if no selection).
 */
const GiftiLabel*
GiftiLabelTableSelectionComboBox::getSelectedLabel() const
{
    const int indx = m_comboBox->currentIndex();
    
    if (indx > 0) {
        QVariant userData = m_comboBox->itemData(indx);
        void* pointer = userData.value<void*>();
        GiftiLabel* giftiLabel = (GiftiLabel*)pointer;
        return giftiLabel;
    }
    
    return NULL;
}

/**
 * @return The selected label (NULL if no selection).
 */
GiftiLabel*
GiftiLabelTableSelectionComboBox::getSelectedLabel()
{
    const int indx = m_comboBox->currentIndex();
    
    if (indx > 0) {
        QVariant userData = m_comboBox->itemData(indx);
        void* pointer = userData.value<void*>();
        GiftiLabel* giftiLabel = (GiftiLabel*)pointer;
        return giftiLabel;
    }
    
    return NULL;
}

/**
 * Set the selected label to the given label.
 *
 * @param label
 *    The label that is to be selected.
 */
void
GiftiLabelTableSelectionComboBox::setSelectedLabel(const GiftiLabel* label)
{
    if (label != NULL) {
        QVariant userData = qVariantFromValue((void*)label);
        int indx = m_comboBox->findData(userData);
        if (indx >= 0) {
            m_comboBox->setCurrentIndex(indx);
        }
        else {
            CaretLogSevere("Label not found: " +
                           label->getName());
        }
    }
}

/**
 * @return Key of the selected label or -1 if no label selected.
 */
int32_t
GiftiLabelTableSelectionComboBox::getSelectedLabelKey() const
{
    const GiftiLabel* gl = getSelectedLabel();
    if (gl != NULL) {
        return gl->getKey();
    }
    
    return -1;
}

/**
 * Set the selected label to the label with the given key.
 *
 * @param key
 *    Key of label that is to be selected.
 */
void
GiftiLabelTableSelectionComboBox::setSelectedLabelKey(const int32_t key)
{
    const GiftiLabel* label = m_giftiLabelTable->getLabel(key);
    if (label != NULL) {
        setSelectedLabel(label);
    }
    else {
        CaretLogSevere("No label with key found: " +
                       QString::number(key));
    }
}

/**
 * @return Name of selected label or empty string if no label selected.
 */
QString
GiftiLabelTableSelectionComboBox::getSelectedLabelName() const
{
    const GiftiLabel* gl = getSelectedLabel();
    if (gl != NULL) {
        return gl->getName();
    }
    
    return "";
}

/**
 * Set the selected label to the label with the given name.
 *
 * @param labelName
 *    Name of label that is to be selected.
 */
void
GiftiLabelTableSelectionComboBox::setSelectedLabelName(const QString& labelName)
{
    const GiftiLabel* label = m_giftiLabelTable->getLabel(labelName);
    if (label != NULL) {
        setSelectedLabel(label);
    }
    else {
        CaretLogSevere("No label with name found: " +
                       labelName);
    }
}


