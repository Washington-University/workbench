
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

#define __GIFTI_LABEL_TABLE_SELECTION_COMBO_BOX_DECLARE__
#include "GiftiLabelTableSelectionComboBox.h"
#undef __GIFTI_LABEL_TABLE_SELECTION_COMBO_BOX_DECLARE__

#include <QComboBox>
#include <QStyle>
#include <QStyleFactory>

#include "CaretAssert.h"
#include "CaretLogger.h"
#include "GiftiLabel.h"
#include "GiftiLabelTable.h"

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
    m_giftiLabelTable = NULL;
    m_unassignedLabelTextOverride = "";
    
    m_comboBox = new QComboBox();
    m_comboBox->setMaxVisibleItems(20);
    
    QObject::connect(m_comboBox, SIGNAL(activated(int)),
                     this, SLOT(itemActivated(int)));
    QObject::connect(m_comboBox, SIGNAL(currentIndexChanged(int)),
                     this, SLOT(currentIndexChanged(int)));
    
#ifdef CARET_OS_MACOSX
    /*
     * On Mac, use a windows combo box so that 
     * the pop-up is not a list the covers the
     * full vertical dimension of the monitor.
     */
    QStyle* style = QStyleFactory::create("Windows");
    if (style != NULL) {
        m_comboBox->setStyle(style);
        //QPalette macPalette(QPalette(m_comboBox->palette()));
        //m_comboBox->setPalette(macPalette);
    }
//    QStringList styleNames;
//    //styleNames << "Windows" << "Plastique";
//    QStringListIterator styleNameIterator(styleNames);
//    while (styleNameIterator.hasNext()) {
//        style = QStyleFactory::create(styleNameIterator.next());
//        if (style != NULL) {
//            break;
//        }
//    }
#endif // CARET_OS_MACOSX  QStyle::SH_ComboBox_Popup
    
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
 * Override the text of the unassigned label.
 *
 * @param text
 *    Text that overrides the text of the unassigned label.
 */
void
GiftiLabelTableSelectionComboBox::setUnassignedLabelTextOverride(const AString& text)
{
    m_unassignedLabelTextOverride = text;
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
GiftiLabelTableSelectionComboBox::rowsWereInserted(const QModelIndex& /*parent*/,
                  int start,
                  int end)
{
    if (m_ignoreInsertedRowsFlag) {
        return;
    }
    
    for (int i = start; i <= end; i++) {
        const QString text = m_comboBox->itemText(i);
        
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
            
            QVariant userData = QVariant::fromValue((void*)label);
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
GiftiLabelTableSelectionComboBox::currentIndexChanged(int /*indx*/)
{
    GiftiLabel* gl = getSelectedLabel();
    
    emit labelChanged(gl);
    
    if (gl != NULL) {
        emit labelKeyChanged(gl->getKey());
    }
    else {
        emit labelKeyChanged(GiftiLabel::getInvalidLabelKey());
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
GiftiLabelTableSelectionComboBox::itemActivated(int /*indx*/)
{
    GiftiLabel* gl = getSelectedLabel();
    
    emit labelActivated(gl);
    
    if (gl != NULL) {
        emit labelKeyActivated(gl->getKey());
    }
    else {
        emit labelKeyActivated(GiftiLabel::getInvalidLabelKey());
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
    
    m_giftiLabelTable = giftiLabelTable;

    const AString selectedLabelName = m_comboBox->currentText();
    m_comboBox->clear();
    
    int32_t defaultIndex = -1;
    
    if (m_giftiLabelTable != NULL) {
        const int32_t unassignedKey = m_giftiLabelTable->getUnassignedLabelKey();
        
        const std::vector<int32_t> keySet = m_giftiLabelTable->getLabelKeysSortedByName();
        for (std::vector<int32_t>::const_iterator iter = keySet.begin();
             iter != keySet.end();
             iter++) {
            const int32_t key = *iter;
            
            GiftiLabel* label = giftiLabelTable->getLabel(key);
            
            AString labelName = label->getName();
            if (key == unassignedKey) {
                if (m_unassignedLabelTextOverride.isEmpty() == false) {
                    labelName = m_unassignedLabelTextOverride;
                }
            }
            
            if (labelName == selectedLabelName) {
                defaultIndex = m_comboBox->count();
            }
            
            QPixmap pm(10, 10);
            pm.fill(QColor::fromRgbF(label->getRed(),
                                     label->getGreen(),
                                     label->getBlue()));
            QIcon icon(pm);
            
            QVariant userData = QVariant::fromValue((void*)label);
            
            m_comboBox->addItem(icon,
                                labelName,
                                userData);
        }
        m_comboBox->setEnabled(true);
        
        if (defaultIndex >= 0) {
            m_comboBox->blockSignals(true);
            m_comboBox->setCurrentIndex(defaultIndex);
            m_comboBox->blockSignals(false);
        }
    }
    else {
        m_comboBox->setEnabled(false);
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
    
    if (indx >= 0) {
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
    
    if (indx >= 0) {
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
        QVariant userData = QVariant::fromValue((void*)label);
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
 * @return Key of the selected label or GiftiLabel::getInvalidLabelKey()
 * if no label selected.
 */
int32_t
GiftiLabelTableSelectionComboBox::getSelectedLabelKey() const
{
    const GiftiLabel* gl = getSelectedLabel();
    if (gl != NULL) {
        return gl->getKey();
    }
    
    return GiftiLabel::getInvalidLabelKey();
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
//        CaretLogSevere("No label with name found: " +
//                       labelName);
    }
}


