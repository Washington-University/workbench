
/*LICENSE_START*/
/*
 *  Copyright (C) 2018 Washington University School of Medicine
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

#define __WU_Q_MACRO_CREATE_DIALOG_DECLARE__
#include "WuQMacroCreateDialog.h"
#undef __WU_Q_MACRO_CREATE_DIALOG_DECLARE__

#include <QComboBox>
#include <QDialogButtonBox>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPlainTextEdit>
#include <QVBoxLayout>

#include "CaretAssert.h"
#include "CaretLogger.h"
#include "WuQMacro.h"
#include "WuQMacroGroup.h"
#include "WuQMacroManager.h"
#include "WuQMacroShortCutKeyComboBox.h"

using namespace caret;


    
/**
 * \class caret::WuQMacroCreateDialog 
 * \brief Dialog for creating a new macro
 * \ingroup WuQMacro
 */

/**
 * Constructor.
 *
 * @param parent
 *     The parent widget
 */
WuQMacroCreateDialog::WuQMacroCreateDialog(QWidget* parent)
: QDialog(parent)
{
    setWindowTitle("Record Macro");
    
    m_macroGroups = WuQMacroManager::instance()->getMacroGroups();
    
    QLabel* nameLabel = new QLabel("Macro name:");
    QLabel* shortCutKeyLabel = new QLabel("Short Cut Key:");
    QLabel* shortCutKeyMaskLabel = new QLabel(WuQMacroManager::getShortCutKeysMask());
    QLabel* descriptionLabel = new QLabel("Description:");
    QLabel* macroGroupLabel = new QLabel("Store macro in:");
    
    m_macroNameLineEdit = new QLineEdit();
    m_macroNameLineEdit->setText(getDefaultMacroName());
    m_macroShortCutKeyComboBox = new WuQMacroShortCutKeyComboBox(this);
    m_macroDescriptionTextEdit = new QPlainTextEdit();
    m_macroDescriptionTextEdit->setFixedHeight(100);
    
    int32_t selectedMacroGroupIndex(-1);
    m_macroGroupComboBox = new QComboBox();
    for (auto mg : m_macroGroups) {
        if (mg->getUniqueIdentifier() == s_lastSelectedMacroGroupIdentifier) {
            selectedMacroGroupIndex = m_macroGroupComboBox->count();
        }
        m_macroGroupComboBox->addItem(mg->getName());
    }
    if (selectedMacroGroupIndex < 0) {
        selectedMacroGroupIndex = m_macroGroupComboBox->count() - 1;
    }
    if ((selectedMacroGroupIndex >= 0)
        && (selectedMacroGroupIndex < m_macroGroupComboBox->count())) {
        m_macroGroupComboBox->setCurrentIndex(selectedMacroGroupIndex);
    }
    
    QGridLayout* gridLayout = new QGridLayout();
    gridLayout->setColumnStretch(0, 0);
    gridLayout->setColumnStretch(1, 0);
    gridLayout->setColumnStretch(2, 0);
    gridLayout->setColumnStretch(3, 100);
    int row = 0;
    gridLayout->addWidget(macroGroupLabel, row, 0);
    gridLayout->addWidget(m_macroGroupComboBox, row, 1, 1, 3);
    row++;
    gridLayout->addWidget(nameLabel, row, 0);
    gridLayout->addWidget(m_macroNameLineEdit, row, 1, 1, 3);
    row++;
    gridLayout->addWidget(shortCutKeyLabel, row, 0);
    gridLayout->addWidget(shortCutKeyMaskLabel, row, 1);
    gridLayout->addWidget(m_macroShortCutKeyComboBox->getWidget(), row, 2); //, Qt::AlignLeft);
    row++;
    gridLayout->addWidget(descriptionLabel, row, 0);
    gridLayout->addWidget(m_macroDescriptionTextEdit, row, 1, 1, 3);
    row++;
    
    m_dialogButtonBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                             | QDialogButtonBox::Cancel);
    QObject::connect(m_dialogButtonBox, &QDialogButtonBox::accepted,
                     this, &WuQMacroCreateDialog::accept);
    QObject::connect(m_dialogButtonBox, &QDialogButtonBox::rejected,
                     this, &WuQMacroCreateDialog::reject);
    
    QVBoxLayout* dialogLayout = new QVBoxLayout(this);
    dialogLayout->addLayout(gridLayout, 100);
    dialogLayout->addWidget(m_dialogButtonBox);
    
    setFixedHeight(sizeHint().height());
}

/**
 * Destructor.
 */
WuQMacroCreateDialog::~WuQMacroCreateDialog()
{
}

/**
 * @return A default name for a macro
 */
QString
WuQMacroCreateDialog::getDefaultMacroName() const
{
    QString name("");
    bool foundFlag(false);
    static int32_t minimumIndex = 1;
    
    for (int32_t i = minimumIndex; i < 10000; i++) {
        name = ("Macro "
                + QString::number(i));
        foundFlag = false;
        for (auto mg : m_macroGroups) {
            if (mg->getMacroByName(name) != NULL) {
                foundFlag = true;
                break;
            }
        }
        
        if ( ! foundFlag) {
            /* 
             * If a default name was created, do not allow a "lower-numbered"
             * default name.  Start with same index since user may not use it.
             */
            minimumIndex = i;
            break;
        }
    }
    
    return name;
}

/**
 * Called when user clicks OK or Cancel
 *
 * @param r
 *     The dialog code (Accepted or Rejected)
 */
void
WuQMacroCreateDialog::done(int r)
{
    if (r == QDialog::Accepted) {
        const QString name(m_macroNameLineEdit->text().trimmed());
        if (name.isEmpty()) {
            QMessageBox::critical(this,
                                  "Error",
                                  "Name is missing",
                                  QMessageBox::Ok,
                                  QMessageBox::Ok);
            return;
        }
        
        m_macro = new WuQMacro();
        m_macro->setName(name);
        m_macro->setShortCutKey(m_macroShortCutKeyComboBox->getSelectedShortCutKey());
        m_macro->setDescription(m_macroDescriptionTextEdit->toPlainText());
        
        const int32_t groupIndex = m_macroGroupComboBox->currentIndex();
        if (groupIndex >= 0) {
            WuQMacroGroup* macroGroup = m_macroGroups[groupIndex];
            macroGroup->addMacro(m_macro);
            s_lastSelectedMacroGroupIdentifier = macroGroup->getUniqueIdentifier();
        }
    }
    
    QDialog::done(r);
}

/**
 * @return Pointer to new macro.  Macro has been added to a group.
 *  Do not delete the pointer.
 */
WuQMacro*
WuQMacroCreateDialog::getNewMacro() const
{
    return m_macro;
}

/**
 * @return Create a combo box with valid function keys.
 * Method is static so that other classes may use it.
 */
QComboBox*
WuQMacroCreateDialog::createFunctionKeyComboBox()
{
    QComboBox* comboBox = new QComboBox();
    comboBox->addItem(" ");
    
    const char firstChar = 'A';
    const char lastChar  = 'Z';
    for (char ch = firstChar; ch <= lastChar; ch++) {
        comboBox->addItem(QString(ch));
    }
    
    return comboBox;
}

