
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

#define __WU_Q_MACRO_COPY_DIALOG_DECLARE__
#include "WuQMacroCopyDialog.h"
#undef __WU_Q_MACRO_COPY_DIALOG_DECLARE__

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

using namespace caret;
    
/**
 * \class caret::WuQMacroCopyDialog
 * \brief Dialog for creating a new macro
 * \ingroup WuQMacro
 */

/**
 * Constructor.
 *
 * @param parent
 *     The parent widget
 */
WuQMacroCopyDialog::WuQMacroCopyDialog(QWidget* parent)
: QDialog(parent)
{
    setWindowTitle("Copy Macro");
    
    m_macroGroups = WuQMacroManager::instance()->getAllMacroGroups();
    
    QLabel* nameLabel = new QLabel("Macro:");
    QLabel* descriptionLabel = new QLabel("Description:");
    QLabel* macroGroupLabel = new QLabel("Macro From:");
    
    m_macroDescriptionTextEdit = new QPlainTextEdit();
    m_macroDescriptionTextEdit->setFixedHeight(100);
    m_macroDescriptionTextEdit->setReadOnly(true);
    
    int32_t selectedMacroGroupIndex(-1);
    m_macroGroupComboBox = new QComboBox();
    QObject::connect(m_macroGroupComboBox, QOverload<int>::of(&QComboBox::activated),
                     this, &WuQMacroCopyDialog::macroGroupComboBoxItemActivated);
    for (auto mg : m_macroGroups) {
        if ( ! mg->isEmpty()) {
            if (mg->getUniqueIdentifier() == s_lastSelectedMacroGroupIdentifier) {
                selectedMacroGroupIndex = m_macroGroupComboBox->count();
            }
            m_macroGroupComboBox->addItem(mg->getName());
        }
    }
    if (selectedMacroGroupIndex < 0) {
        selectedMacroGroupIndex = m_macroGroupComboBox->count() - 1;
    }
    if ((selectedMacroGroupIndex >= 0)
        && (selectedMacroGroupIndex < m_macroGroupComboBox->count())) {
        m_macroGroupComboBox->setCurrentIndex(selectedMacroGroupIndex);
    }    
    
    m_macroNameComboBox = new QComboBox();
    QObject::connect(m_macroNameComboBox, QOverload<int>::of(&QComboBox::activated),
                     this, &WuQMacroCopyDialog::macroComboBoxItemActivated);

    

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
    gridLayout->addWidget(m_macroNameComboBox, row, 1, 1, 3);
    row++;
    gridLayout->addWidget(descriptionLabel, row, 0);
    gridLayout->addWidget(m_macroDescriptionTextEdit, row, 1, 1, 3);
    row++;
    
    m_dialogButtonBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                             | QDialogButtonBox::Cancel);
    QObject::connect(m_dialogButtonBox, &QDialogButtonBox::accepted,
                     this, &WuQMacroCopyDialog::accept);
    QObject::connect(m_dialogButtonBox, &QDialogButtonBox::rejected,
                     this, &WuQMacroCopyDialog::reject);
    
    QVBoxLayout* dialogLayout = new QVBoxLayout(this);
    dialogLayout->addLayout(gridLayout, 100);
    dialogLayout->addWidget(m_dialogButtonBox);
    
    setFixedHeight(sizeHint().height());
    
    macroGroupComboBoxItemActivated(m_macroGroupComboBox->currentIndex());
}

/**
 * Destructor.
 */
WuQMacroCopyDialog::~WuQMacroCopyDialog()
{
}

/**
 * Called when macro group is selected from combo box
 *
 * @param index
 *     Index of item selected
 */
void
WuQMacroCopyDialog::macroGroupComboBoxItemActivated(int /*index*/)
{
    m_macroNameComboBox->clear();
    
    const WuQMacroGroup* mg = getMacroGroup();
    if (mg != NULL) {
        const int32_t numMacros = mg->getNumberOfMacros();
        for (int32_t i = 0; i < numMacros; i++) {
            m_macroNameComboBox->addItem(mg->getMacroAtIndex(i)->getName());
        }
    }
    macroComboBoxItemActivated(m_macroNameComboBox->currentIndex());
}

/**
 * Called when macro group is selected from combo box
 *
 * @param index
 *     Index of item selected
 */
void
WuQMacroCopyDialog::macroComboBoxItemActivated(int /*index*/)
{
    QString text;
    const WuQMacro* macro = getMacroToCopy();
    if (macro != NULL) {
        text = macro->getDescription();
    }
    m_macroDescriptionTextEdit->setPlainText(text);
}

/**
 * Called when user clicks OK or Cancel
 *
 * @param r
 *     The dialog code (Accepted or Rejected)
 */
void
WuQMacroCopyDialog::done(int r)
{
    if (r == QDialog::Accepted) {
//        const QString name(m_macroNameLineEdit->text().trimmed());
//        if (name.isEmpty()) {
//            QMessageBox::critical(this,
//                                  "Error",
//                                  "Name is missing",
//                                  QMessageBox::Ok,
//                                  QMessageBox::Ok);
//            return;
//        }
//
//        m_macro = new WuQMacro();
//        m_macro->setName(name);
//        m_macro->setDescription(m_macroDescriptionTextEdit->toPlainText());
//
//        const int32_t groupIndex = m_macroGroupComboBox->currentIndex();
//        if (groupIndex >= 0) {
//            WuQMacroGroup* macroGroup = m_macroGroups[groupIndex];
//            macroGroup->addMacro(m_macro);
//            s_lastSelectedMacroGroupIdentifier = macroGroup->getUniqueIdentifier();
//        }
    }
    
    QDialog::done(r);
}

/**
 * @return Pointer to selected macro group or NULL if none available
 */
const WuQMacroGroup*
WuQMacroCopyDialog::getMacroGroup() const
{
    const WuQMacroGroup* macroGroup(NULL);
    const int32_t groupIndex = m_macroGroupComboBox->currentIndex();
    if ((groupIndex >= 0)
        && (groupIndex < static_cast<int32_t>(m_macroGroups.size()))) {
        CaretAssertVectorIndex(m_macroGroups, groupIndex);
        macroGroup = m_macroGroups[groupIndex];
        CaretAssert(macroGroup);
    }
    
    return macroGroup;
}

/**
 * @return Pointer to macro that was selected and should be copied.
 * NULL if no macro to copy.
 */
const WuQMacro*
WuQMacroCopyDialog::getMacroToCopy() const
{
    const WuQMacro* macro(NULL);
    
    const WuQMacroGroup* mg = getMacroGroup();
    if (mg != NULL) {
        const int32_t macroIndex = m_macroNameComboBox->currentIndex();
        if ((macroIndex >= 0)
            && (macroIndex < mg->getNumberOfMacros())) {
            macro = mg->getMacroAtIndex(macroIndex);
            CaretAssert(macro);
        }
    }
    
    return macro;
}

