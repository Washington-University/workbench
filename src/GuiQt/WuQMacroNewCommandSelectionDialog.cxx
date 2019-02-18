
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

#define __WU_Q_MACRO_NEW_COMMAND_SELECTION_DIALOG_DECLARE__
#include "WuQMacroNewCommandSelectionDialog.h"
#undef __WU_Q_MACRO_NEW_COMMAND_SELECTION_DIALOG_DECLARE__

#include <QComboBox>
#include <QDialogButtonBox>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QListWidgetItem>
#include <QMessageBox>
#include <QPlainTextEdit>
#include <QStackedWidget>
#include <QVBoxLayout>

#include "CaretAssert.h"
#include "CaretLogger.h"
#include "EnumComboBoxTemplate.h"
#include "WuQMacro.h"
#include "WuQMacroCommand.h"
#include "WuQMacroGroup.h"
#include "WuQMacroManager.h"
#include "WuQMacroShortCutKeyComboBox.h"
#include "WuQMacroSignalWatcher.h"

using namespace caret;

/**
 * \class caret::WuQMacroNewCommandSelectionDialog
 * \brief Dialog for creating a new macro
 * \ingroup WuQMacro
 */

/**
 * Constructor.
 *
 * @param parent
 *     The parent widget
 */
WuQMacroNewCommandSelectionDialog::WuQMacroNewCommandSelectionDialog(QWidget* parent)
: QDialog(parent)
{
    setWindowTitle("Add Macro Command");
    
    QLabel* commandTypeLabel = new QLabel("Type:");
    QLabel* commandsLabel = new QLabel("Commands:");
    QLabel* descriptionLabel = new QLabel("Description:");
    
    std::vector<WuQMacroCommandTypeEnum::Enum> commandTypes;
    commandTypes.push_back(WuQMacroCommandTypeEnum::CUSTOM_OPERATION);
    commandTypes.push_back(WuQMacroCommandTypeEnum::WIDGET);
    m_commandTypeComboBox = new EnumComboBoxTemplate(this);
    m_commandTypeComboBox->setupWithItems<WuQMacroCommandTypeEnum,  WuQMacroCommandTypeEnum::Enum>(commandTypes);
    m_commandTypeComboBox->setSelectedItem<WuQMacroCommandTypeEnum,  WuQMacroCommandTypeEnum::Enum>(s_lastCommandTypeSelected);
    QObject::connect(m_commandTypeComboBox, &EnumComboBoxTemplate::itemActivated,
                     this, &WuQMacroNewCommandSelectionDialog::commandTypeComboBoxActivated);
    
    m_customCommandListWidget = new QListWidget();
    QObject::connect(m_customCommandListWidget, &QListWidget::itemClicked,
                     this, &WuQMacroNewCommandSelectionDialog::customCommandListWidgetItemClicked);
    
    m_widgetCommandListWidget = new QListWidget();
    QObject::connect(m_widgetCommandListWidget, &QListWidget::itemClicked,
                     this, &WuQMacroNewCommandSelectionDialog::widgetCommandListWidgetItemClicked);
    
    m_stackedWidget = new QStackedWidget();
    m_stackedWidget->addWidget(m_customCommandListWidget);
    m_stackedWidget->addWidget(m_widgetCommandListWidget);

    m_macroDescriptionTextEdit = new QPlainTextEdit();
    m_macroDescriptionTextEdit->setFixedHeight(100);
    
    QGridLayout* gridLayout = new QGridLayout();
    gridLayout->setColumnStretch(0, 0);
    gridLayout->setColumnStretch(1, 100);
    int row = 0;
    gridLayout->addWidget(commandTypeLabel, row, 0);
    gridLayout->addWidget(m_commandTypeComboBox->getWidget(), row, 1, 1, 2);
    row++;
    gridLayout->addWidget(commandsLabel, row, 0);
    gridLayout->addWidget(m_stackedWidget, row, 1, 1, 2);
    row++;
    gridLayout->addWidget(descriptionLabel, row, 0);
    gridLayout->addWidget(m_macroDescriptionTextEdit, row, 1, 1, 2);
    row++;
    
    m_dialogButtonBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                             | QDialogButtonBox::Cancel);
    QObject::connect(m_dialogButtonBox, &QDialogButtonBox::accepted,
                     this, &WuQMacroNewCommandSelectionDialog::accept);
    QObject::connect(m_dialogButtonBox, &QDialogButtonBox::rejected,
                     this, &WuQMacroNewCommandSelectionDialog::reject);
    
    QVBoxLayout* dialogLayout = new QVBoxLayout(this);
    dialogLayout->addLayout(gridLayout);
    dialogLayout->addWidget(m_dialogButtonBox);
    
    commandTypeComboBoxActivated();
}

/**
 * Destructor.
 */
WuQMacroNewCommandSelectionDialog::~WuQMacroNewCommandSelectionDialog()
{
    for (auto cc : m_customCommands) {
        delete cc;
    }
    m_customCommands.clear();
}

/**
 * Called when a selection is made from command type combo box
 */
void
WuQMacroNewCommandSelectionDialog::commandTypeComboBoxActivated()
{
    s_lastCommandTypeSelected = m_commandTypeComboBox->getSelectedItem<WuQMacroCommandTypeEnum,  WuQMacroCommandTypeEnum::Enum>();
    
    m_macroDescriptionTextEdit->clear();
    
    switch (s_lastCommandTypeSelected) {
        case WuQMacroCommandTypeEnum::CUSTOM_OPERATION:
            m_stackedWidget->setCurrentWidget(m_customCommandListWidget);
            if (m_customCommandListWidget->count() <= 0) {
                loadCustomCommandListWidget();
                if (m_customCommandListWidget->count() > 0) {
                    QListWidgetItem* firstItem = m_customCommandListWidget->item(0);
                    if (firstItem != NULL) {
                        m_customCommandListWidget->setCurrentItem(firstItem);
                        customCommandListWidgetItemClicked(firstItem);
                    }
                }
            }
            break;
        case WuQMacroCommandTypeEnum::MOUSE:
            CaretAssert(0);
            break;
        case WuQMacroCommandTypeEnum::WIDGET:
            m_stackedWidget->setCurrentWidget(m_widgetCommandListWidget);
            if (m_widgetCommandListWidget->count() <= 0) {
                loadWidgetCommandListWidget();
            }
            break;
    }
}

/**
 * Load the custom commands into the list widget
 */
void
WuQMacroNewCommandSelectionDialog::loadCustomCommandListWidget()
{
    m_customCommands = WuQMacroManager::instance()->getAllCustomOperationMacroCommands();
    for (auto cc : m_customCommands) {
        const QString name = cc->getDescriptiveName();
        const QString customTypeName = cc->getCustomOperationTypeName();
        
        QListWidgetItem* item = new QListWidgetItem(name);
        item->setData(Qt::UserRole, customTypeName);
        m_customCommandListWidget->addItem(item);
    }
}

/**
 * Load the widget commands into the list widget
 */
void
WuQMacroNewCommandSelectionDialog::loadWidgetCommandListWidget()
{
    m_widgetCommands = WuQMacroManager::instance()->getAllWidgetSignalWatchers(false);
    for (auto wc : m_widgetCommands) {
        const QString name = wc->getObjectName();        
        QListWidgetItem *item = new QListWidgetItem(name);
        m_widgetCommandListWidget->addItem(item);
    }
}

/**
 * Called when an item in the custom list widget is clicked
 *
 * @param item
 *    Item that was clicked
 */
void
WuQMacroNewCommandSelectionDialog::customCommandListWidgetItemClicked(QListWidgetItem* item)
{
    QString description;
    if (item != NULL) {
        const QString customTypeName = item->data(Qt::UserRole).toString();
        for (auto cc : m_customCommands) {
            if (customTypeName == cc->getCustomOperationTypeName()) {
                description = cc->getObjectToolTip();
            }
        }
    }
    
    m_macroDescriptionTextEdit->setPlainText(description);
}

/**
 * Called when an item in the custom list widget is clicked
 *
 * @param item
 *    Item that was clicked
 */
void
WuQMacroNewCommandSelectionDialog::widgetCommandListWidgetItemClicked(QListWidgetItem* item)
{
    QString description;
    const int index = m_widgetCommandListWidget->currentRow();
    if ((index >= 0)
        && (index < static_cast<int32_t>(m_widgetCommands.size()))) {
        description = m_widgetCommands[index]->getToolTip();
    }

    m_macroDescriptionTextEdit->setPlainText(description);
}

/**
 * Called when user clicks OK or Cancel
 *
 * @param r
 *     The dialog code (Accepted or Rejected)
 */
void
WuQMacroNewCommandSelectionDialog::done(int r)
{
    if (r == QDialog::Accepted) {
        bool validFlag(false);
        switch (s_lastCommandTypeSelected) {
            case WuQMacroCommandTypeEnum::CUSTOM_OPERATION:
                validFlag = (m_customCommandListWidget->currentItem() != NULL);
                break;
            case WuQMacroCommandTypeEnum::MOUSE:
                CaretAssert(0);
                validFlag = false;
                break;
            case WuQMacroCommandTypeEnum::WIDGET:
                validFlag = (m_widgetCommandListWidget->currentItem() != NULL);
                break;
        }

        if ( ! validFlag) {
            QMessageBox::critical(this,
                                  "Error",
                                  "No command is selected",
                                  QMessageBox::Ok,
                                  QMessageBox::Ok);
        }
    }
    
    QDialog::done(r);
}

/**
 * Get new instance of the command selected
 *
 * @param errorMessageOut
 *     Output with error information.
 * @return
       Pointer to new command or NULL if it failed.
 */
WuQMacroCommand*
WuQMacroNewCommandSelectionDialog::getNewInstanceOfSelectedCommand(QString& errorMessageOut)
{
    errorMessageOut.clear();
    WuQMacroCommand* commandOut(NULL);
    
    switch (s_lastCommandTypeSelected) {
        case WuQMacroCommandTypeEnum::CUSTOM_OPERATION:
        {
            QListWidgetItem* item = m_customCommandListWidget->currentItem();
            if (item != NULL) {
                const QString customTypeName = item->data(Qt::UserRole).toString();
                QString errorMessage;
                commandOut = WuQMacroManager::instance()->newInstanceOfCustomOperationMacroCommand(customTypeName,
                                                                                                   errorMessage);
                if (commandOut == NULL) {
                    errorMessageOut = ("New instance of custom command \""
                                       + customTypeName
                                       + "\" failed");
                }
            }
        }
            break;
        case WuQMacroCommandTypeEnum::MOUSE:
            errorMessageOut = "Mouse commands not supported for new instances";
            CaretAssert(0);
            break;
        case WuQMacroCommandTypeEnum::WIDGET:
        {
            QListWidgetItem* item = m_widgetCommandListWidget->currentItem();
            if (item != NULL) {
                const QString widgetName = item->text();
                WuQMacroSignalWatcher* watcher = WuQMacroManager::instance()->getWidgetSignalWatcherWithName(widgetName);
                if (watcher != NULL) {
                    commandOut = watcher->createMacroCommandWithDefaultParameters(errorMessageOut);
                }
                else {
                    errorMessageOut = ("Unable to find widget watcher with name \""
                                       + widgetName
                                       + "\"");
                }
            }
        }
            break;
    }

    return commandOut;
}


