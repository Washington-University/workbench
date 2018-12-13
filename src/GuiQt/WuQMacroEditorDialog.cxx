
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

#define __WU_Q_MACRO_EDITOR_DIALOG_DECLARE__
#include "WuQMacroEditorDialog.h"
#undef __WU_Q_MACRO_EDITOR_DIALOG_DECLARE__

#include <QComboBox>
#include <QDialogButtonBox>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPlainTextEdit>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QVBoxLayout>

#include "CaretAssert.h"
#include "CaretLogger.h"
#include "WuQMacro.h"
#include "WuQMacroCommand.h"
#include "WuQMacroGroup.h"
#include "WuQMacroManager.h"

using namespace caret;


    
/**
 * \class caret::WuQMacroEditorDialog 
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
WuQMacroEditorDialog::WuQMacroEditorDialog(WuQMacro* macro,
                                             QWidget* parent)
: QDialog(parent),
m_macro(macro),
m_macroWasModifiedFlag(false)
{
    CaretAssert(m_macro);
    
    setWindowTitle("Macro Editor");
    
    QLabel* nameLabel = new QLabel("Macro name:");
    QLabel* functionKeyLabel = new QLabel("Function Key:");
    QLabel* descriptionLabel = new QLabel("Description:");
    QLabel* commandsLabel = new QLabel("Commands:");
    
    m_macroNameLineEdit = new QLineEdit();
    m_macroNameLineEdit->setText(m_macro->getName());
    m_macroFunctionKeyLineEdit = new QLineEdit();
    m_macroFunctionKeyLineEdit->setFixedWidth(40);
    m_macroFunctionKeyLineEdit->setText(m_macro->getFunctionKey());
    m_macroDescriptionTextEdit = new QPlainTextEdit();
    m_macroDescriptionTextEdit->setFixedHeight(60);
    m_macroDescriptionTextEdit->setPlainText(m_macro->getDescription());
    
    m_tableWidget = createTableWidget();
    
    QGridLayout* gridLayout = new QGridLayout();
    int row = 0;
    gridLayout->addWidget(nameLabel, row, 0);
    gridLayout->addWidget(m_macroNameLineEdit, row, 1);
    row++;
    gridLayout->addWidget(functionKeyLabel, row, 0);
    gridLayout->addWidget(m_macroFunctionKeyLineEdit, row, 1);
    row++;
    gridLayout->addWidget(descriptionLabel, row, 0);
    gridLayout->addWidget(m_macroDescriptionTextEdit, row, 1);
    row++;
    gridLayout->addWidget(commandsLabel, row, 0);
    gridLayout->addWidget(m_tableWidget, row, 1);
    row++;
    
    
    m_dialogButtonBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                             | QDialogButtonBox::Cancel);
    QObject::connect(m_dialogButtonBox, &QDialogButtonBox::accepted,
                     this, &WuQMacroEditorDialog::accept);
    QObject::connect(m_dialogButtonBox, &QDialogButtonBox::rejected,
                     this, &WuQMacroEditorDialog::reject);
    
    QVBoxLayout* dialogLayout = new QVBoxLayout(this);
    dialogLayout->addLayout(gridLayout);
    dialogLayout->addWidget(m_dialogButtonBox);
    
    loadTableWidget();
}

/**
 * Destructor.
 */
WuQMacroEditorDialog::~WuQMacroEditorDialog()
{
}

/**
 * @return New instance of the table widget
 */
QTableWidget*
WuQMacroEditorDialog::createTableWidget()
{
    QTableWidget* tableWidget = new QTableWidget();
    
    QStringList columnNames;
    CaretAssert(COLUMN_TYPE       == 0);
    columnNames << "Class";
    CaretAssert(COLUMN_VALUE_TYPE == 1);
    columnNames << "Value-Type";
    CaretAssert(COLUMN_NAME       == 2);
    columnNames << "Name";
    CaretAssert(COLUMN_VALUE      == 3);
    columnNames << "Value";
    CaretAssert(COLUMN_TOOLTIP    == 4);
    columnNames << "ToolTip";
    CaretAssert(columnNames.size() == NUMBER_OF_COLUMNS);
    
    tableWidget->setHorizontalHeaderLabels(columnNames);
    
    return tableWidget;
}

/**
 * Load the table widget
 */
void
WuQMacroEditorDialog::loadTableWidget()
{
    CaretAssert(m_tableWidget);
    
    const int32_t numberOfCommands = m_macro->getNumberOfMacroCommands();
    m_tableWidget->setRowCount(numberOfCommands);
    m_tableWidget->setColumnCount(NUMBER_OF_COLUMNS);
    
    for (int32_t i = 0; i < numberOfCommands; i++) {
        const WuQMacroCommand* command = m_macro->getMacroCommandAtIndex(i);
        QTableWidgetItem* classItem = new QTableWidgetItem(WuQMacroClassTypeEnum::toGuiName(command->getClassType()));
        QTableWidgetItem* valueTypeItem = new QTableWidgetItem(WuQMacroDataValueTypeEnum::toGuiName(command->getDataType()));
        QTableWidgetItem* nameItem = new QTableWidgetItem(command->getObjectName());
        QTableWidgetItem* valueItem = new QTableWidgetItem(command->getDataValue().toString());
        
        const QString toolTip = WuQMacroManager::instance()->getToolTipForObjectName(command->getObjectName());
        QTableWidgetItem* toolTipItem = new QTableWidgetItem(toolTip);
        
        switch (command->getDataType()) {
            case WuQMacroDataValueTypeEnum::BOOLEAN:
                break;
            case WuQMacroDataValueTypeEnum::FLOAT:
                break;
            case WuQMacroDataValueTypeEnum::INTEGER:
                break;
            case WuQMacroDataValueTypeEnum::MOUSE:
                valueItem->setText("Mouse");
                break;
            case WuQMacroDataValueTypeEnum::STRING:
                break;
        }
        
        m_tableWidget->setItem(i, COLUMN_TYPE, classItem);
        m_tableWidget->setItem(i, COLUMN_VALUE_TYPE, valueTypeItem);
        m_tableWidget->setItem(i, COLUMN_NAME, nameItem);
        m_tableWidget->setItem(i, COLUMN_VALUE, valueItem);
        m_tableWidget->setItem(i, COLUMN_TOOLTIP, toolTipItem);
    }

    QStringList columnNames;
    CaretAssert(COLUMN_TYPE       == 0);
    columnNames << "Class";
    CaretAssert(COLUMN_VALUE_TYPE == 1);
    columnNames << "Value-Type";
    CaretAssert(COLUMN_NAME       == 2);
    columnNames << "Name";
    CaretAssert(COLUMN_VALUE      == 3);
    columnNames << "Value";
    CaretAssert(COLUMN_TOOLTIP    == 4);
    columnNames << "ToolTip";
    CaretAssert(columnNames.size() == NUMBER_OF_COLUMNS);
    
    m_tableWidget->setHorizontalHeaderLabels(columnNames);
}


/**
 * Called when user clicks OK or Cancel
 *
 * @param r
 *     The dialog code (Accepted or Rejected)
 */
void
WuQMacroEditorDialog::done(int r)
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
        
        const QString newFunctionKey = m_macroFunctionKeyLineEdit->text().trimmed();
        if (m_macro->getFunctionKey() != newFunctionKey) {
            m_macro->setFunctionKey(newFunctionKey);
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
WuQMacroEditorDialog::isMacroModified() const
{
    return m_macroWasModifiedFlag;
}

