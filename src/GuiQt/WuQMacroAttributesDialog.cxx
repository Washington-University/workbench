
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

#define __WU_Q_MACRO_ATTRIBUTES_DIALOG_DECLARE__
#include "WuQMacroAttributesDialog.h"
#undef __WU_Q_MACRO_ATTRIBUTES_DIALOG_DECLARE__

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
 * \class caret::WuQMacroAttributesDialog 
 * \brief Dialog for creating a new macro
 * \ingroup WuQMacro
 */

/**
 * Constructor.
 *
 * @param macro
 *     The macro being edited
 * @param parent
 *     The parent widget
 */
WuQMacroAttributesDialog::WuQMacroAttributesDialog(WuQMacro* macro,
                                             QWidget* parent)
: QDialog(parent),
m_macro(macro),
m_macroWasModifiedFlag(false)
{
    CaretAssert(m_macro);
    
    setWindowTitle("Macro Attributes");
    
    QLabel* nameLabel = new QLabel("Macro name:");
    QLabel* shortCutKeyLabel = new QLabel("Short Cut Key:");
    QLabel* shortCutKeyMaskLabel = new QLabel(WuQMacroManager::getShortCutKeysMask());
    QLabel* descriptionLabel = new QLabel("Description:");
    
    m_macroNameLineEdit = new QLineEdit();
    m_macroNameLineEdit->setText(m_macro->getName());
    m_macroShortCutKeyComboBox = new WuQMacroShortCutKeyComboBox(this);
    m_macroShortCutKeyComboBox->setSelectedShortCutKey(m_macro->getShortCutKey());
    m_macroDescriptionTextEdit = new QPlainTextEdit();
    m_macroDescriptionTextEdit->setFixedHeight(100);
    m_macroDescriptionTextEdit->setPlainText(m_macro->getDescription());
    
    QGridLayout* gridLayout = new QGridLayout();
    int row = 0;
    gridLayout->addWidget(nameLabel, row, 0);
    gridLayout->addWidget(m_macroNameLineEdit, row, 1, 1, 2);
    row++;
    gridLayout->addWidget(shortCutKeyLabel, row, 0);
    gridLayout->addWidget(shortCutKeyMaskLabel, row, 1);
    gridLayout->addWidget(m_macroShortCutKeyComboBox->getWidget(), row, 2, Qt::AlignLeft);
    row++;
    gridLayout->addWidget(descriptionLabel, row, 0);
    gridLayout->addWidget(m_macroDescriptionTextEdit, row, 1, 1, 2);
    row++;
    
    m_dialogButtonBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                             | QDialogButtonBox::Cancel);
    QObject::connect(m_dialogButtonBox, &QDialogButtonBox::accepted,
                     this, &WuQMacroAttributesDialog::accept);
    QObject::connect(m_dialogButtonBox, &QDialogButtonBox::rejected,
                     this, &WuQMacroAttributesDialog::reject);
    
    QVBoxLayout* dialogLayout = new QVBoxLayout(this);
    dialogLayout->addLayout(gridLayout);
    dialogLayout->addWidget(m_dialogButtonBox);
}

/**
 * Destructor.
 */
WuQMacroAttributesDialog::~WuQMacroAttributesDialog()
{
}

/**
 * Called when user clicks OK or Cancel
 *
 * @param r
 *     The dialog code (Accepted or Rejected)
 */
void
WuQMacroAttributesDialog::done(int r)
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
        
        const QString newName = m_macroNameLineEdit->text().trimmed();
        if (m_macro->getName() != newName) {
            m_macro->setName(newName);
            m_macroWasModifiedFlag = true;
        }
        
        const WuQMacroShortCutKeyEnum::Enum newShortCutKey = m_macroShortCutKeyComboBox->getSelectedShortCutKey();
        if (m_macro->getShortCutKey() != newShortCutKey) {
            m_macro->setShortCutKey(newShortCutKey);
            m_macroWasModifiedFlag = true;
        }
        
        const QString newDescription = m_macroDescriptionTextEdit->toPlainText().trimmed();
        if (m_macro->getDescription() != newDescription) {
            m_macro->setDescription(newDescription);
            m_macroWasModifiedFlag = true;
        }
    }
    
    QDialog::done(r);
}

/**
 * @return True if the macro was modified and may need to be saved,
 * else false
 */
bool
WuQMacroAttributesDialog::isMacroModified() const
{
    return m_macroWasModifiedFlag;
}

