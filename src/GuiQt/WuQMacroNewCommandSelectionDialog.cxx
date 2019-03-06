
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

#include <QAbstractButton>
#include <QComboBox>
#include <QDialogButtonBox>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QListWidgetItem>
#include <QMessageBox>
#include <QPlainTextEdit>
#include <QSplitter>
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
    
    QLabel* commandTypeLabel = new QLabel("Command Type:");
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
    
    QWidget* typeWidget = new QWidget();
    QHBoxLayout* typeLayout = new QHBoxLayout(typeWidget);
    typeLayout->setContentsMargins(0, 0, 0, 0);
    typeLayout->addWidget(commandTypeLabel, 0);
    typeLayout->addWidget(m_commandTypeComboBox->getWidget(), 100);
    
    m_customCommandListWidget = new QListWidget();
    QObject::connect(m_customCommandListWidget, &QListWidget::itemClicked,
                     this, &WuQMacroNewCommandSelectionDialog::customCommandListWidgetItemClicked);
    
    m_widgetCommandListWidget = new QListWidget();
    QObject::connect(m_widgetCommandListWidget, &QListWidget::itemClicked,
                     this, &WuQMacroNewCommandSelectionDialog::widgetCommandListWidgetItemClicked);
    
    m_stackedWidget = new QStackedWidget();
    m_stackedWidget->addWidget(m_customCommandListWidget);
    m_stackedWidget->addWidget(m_widgetCommandListWidget);

    QWidget* commandsWidget = new QWidget();
    QHBoxLayout* commandsLayout = new QHBoxLayout(commandsWidget);
    commandsLayout->setContentsMargins(0, 0, 0, 0);
    commandsLayout->addWidget(commandsLabel, 0);
    commandsLayout->addWidget(m_stackedWidget, 100);
    
    m_macroDescriptionTextEdit = new QPlainTextEdit();
    
    QWidget* descriptionWidget = new QWidget();
    QHBoxLayout* descriptionLayout = new QHBoxLayout(descriptionWidget);
    descriptionLayout->setContentsMargins(0, 0, 0, 0);
    descriptionLayout->addWidget(descriptionLabel, 0);
    descriptionLayout->addWidget(m_macroDescriptionTextEdit, 100);
    
    m_splitter = new QSplitter();
    m_splitter->setOrientation(Qt::Vertical);
    m_splitter->addWidget(commandsWidget);
    m_splitter->addWidget(descriptionWidget);
    m_splitter->setStretchFactor(0, 75);
    m_splitter->setStretchFactor(1, 25);
    m_splitter->setChildrenCollapsible(false);
    
    m_dialogButtonBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                             | QDialogButtonBox::Cancel);
    const bool allowApplyButtonFlag(false);
    if (allowApplyButtonFlag) {
        m_dialogButtonBox->addButton(QDialogButtonBox::Apply);
    }
    QObject::connect(m_dialogButtonBox, &QDialogButtonBox::clicked,
                     this, &WuQMacroNewCommandSelectionDialog::otherButtonClicked);

    QVBoxLayout* dialogLayout = new QVBoxLayout(this);
    dialogLayout->addWidget(typeWidget);
    dialogLayout->addWidget(m_splitter);
    dialogLayout->addWidget(m_dialogButtonBox);
    
    commandTypeComboBoxActivated();
    
    if ( ! s_previousDialogGeometry.isEmpty()) {
        restoreGeometry(s_previousDialogGeometry);
    }
    if ( ! s_previousSplitterState.isEmpty()) {
        m_splitter->restoreState(s_previousSplitterState);
    }
}

/**
 * Destructor.
 */
WuQMacroNewCommandSelectionDialog::~WuQMacroNewCommandSelectionDialog()
{
    s_previousDialogGeometry = saveGeometry();
    s_previousSplitterState  = m_splitter->saveState();
    
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
    
    m_commandSelectionChangedSinceApplyClickedFlag = true;
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
    m_commandSelectionChangedSinceApplyClickedFlag = true;
}

/**
 * Called when an item in the custom list widget is clicked
 *
 * @param item
 *    Item that was clicked
 */
void
WuQMacroNewCommandSelectionDialog::widgetCommandListWidgetItemClicked(QListWidgetItem* /*item*/)
{
    QString description;
    const int index = m_widgetCommandListWidget->currentRow();
    if ((index >= 0)
        && (index < static_cast<int32_t>(m_widgetCommands.size()))) {
        description = m_widgetCommands[index]->getToolTip();
    }

    m_macroDescriptionTextEdit->setPlainText(description);
    m_commandSelectionChangedSinceApplyClickedFlag = true;
}

/**
 * Called when a dialog button is clicked
 */
void
WuQMacroNewCommandSelectionDialog::otherButtonClicked(QAbstractButton* button)
{
    switch (m_dialogButtonBox->standardButton(button)) {
        case QDialogButtonBox::Apply:
            processApplyButtonClicked(false);
            break;
        case QDialogButtonBox::Ok:
            accept();
            break;
        case QDialogButtonBox::Cancel:
            reject();
            break;
        default:
            CaretAssertMessage(0, ("Unknown button clicked with text \""
                                   + button->text()
                                   + "\""));
            break;
    }
}

/**
 * Called to process the apply button.
 *
 * @return True if apply button process was successful, else false.
 */
bool
WuQMacroNewCommandSelectionDialog::processApplyButtonClicked(const bool okButtonClicked)
{
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
    
    if (validFlag) {
        bool addCommandFlag = true;
        if (okButtonClicked) {
            /*
             * Both Apply and Ok add a command to the macro.  If the user adds a command
             * using apply and then clicks Ok without changing the selected command,
             * warn the user to avoid unintentionally adding the command twice.
             */
            if ( ! m_commandSelectionChangedSinceApplyClickedFlag) {
                const QString msg("Selected command has not changed since Apply button was clicked.  "
                                  "Add command to macro again?");
                switch (QMessageBox::warning(this,
                                             "Warning",
                                             msg,
                                             (QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel),
                                             QMessageBox::No)) {
                    case QMessageBox::Yes:
                        addCommandFlag = true;
                        break;
                    case QMessageBox::No:
                        addCommandFlag = false;
                        break;
                    case QMessageBox::Cancel:
                        addCommandFlag = false;
                        validFlag = false;
                        break;
                    default:
                        break;
                }
            }
        }
        QString errorMessage;
        if (addCommandFlag) {
            WuQMacroCommand* command = NULL;
            command = getNewInstanceOfSelectedCommand(errorMessage);
            if (command != NULL) {
                emit signalNewMacroCommandCreated(command);
                if ( ! okButtonClicked) {
                    m_commandSelectionChangedSinceApplyClickedFlag = false;
                }
            }
            else {
                validFlag = false;
                QMessageBox::critical(this,
                                      "Error",
                                      errorMessage,
                                      QMessageBox::Ok,
                                      QMessageBox::Ok);
            }
        }
    }
    else {
        QMessageBox::critical(this,
                              "Error",
                              "No command is selected",
                              QMessageBox::Ok,
                              QMessageBox::Ok);
    }
    return validFlag;
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
        const bool validFlag = processApplyButtonClicked(true);
        if ( ! validFlag) {
            return;
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


