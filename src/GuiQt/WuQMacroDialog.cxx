
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

#define __WU_Q_MACRO_DIALOG_DECLARE__
#include "WuQMacroDialog.h"
#undef __WU_Q_MACRO_DIALOG_DECLARE__

#include <QCheckBox>
#include <QComboBox>
#include <QDialogButtonBox>
#include <QDoubleSpinBox>
#include <QGridLayout>
#include <QInputDialog>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QStackedWidget>
#include <QTreeView>
#include <QVBoxLayout>

#include "CaretAssert.h"
#include "CaretLogger.h"
#include "WuQMacro.h"
#include "WuQMacroCommand.h"
#include "WuQMacroGroup.h"
#include "WuQMacroExecutor.h"
#include "WuQMacroManager.h"
#include "WuQMacroStandardItemTypeEnum.h"

using namespace caret;

/**
 * \class caret::WuQMacroDialog 
 * \brief Dialog for managing macros
 * \ingroup WuQMacro
 */

/**
 * Constructor.
 *
 * @param defaultMacroName
 *    Default name for new macro
 * @param parent
 *    The dialog's parent widget.
 */
WuQMacroDialog::WuQMacroDialog(QWidget* parent)
: QDialog(parent)
{
    setWindowTitle("Macros");
    this->setAttribute(Qt::WA_DeleteOnClose, false);
    
    m_macroGroups = WuQMacroManager::instance()->getMacroGroups();
    
    QLabel* macrosLabel = new QLabel("Macro:");
    QLabel* macroGroupLabel = new QLabel("Macros in:");
    
    m_macroGroupComboBox = new QComboBox();
    QObject::connect(m_macroGroupComboBox, static_cast<void(QComboBox::*)(int)>(&QComboBox::activated),
                     this, &WuQMacroDialog::macroGroupComboBoxActivated);
    
    m_treeView = new QTreeView();
    m_treeView->setHeaderHidden(true);
    QObject::connect(m_treeView, &QTreeView::clicked,
                     this, &WuQMacroDialog::treeViewItemClicked);
    
    QGridLayout* gridLayout = new QGridLayout();
    int row = 0;
    gridLayout->addWidget(macroGroupLabel, row, 0);
    gridLayout->addWidget(m_macroGroupComboBox, row, 1);
    row++;
    gridLayout->addWidget(macrosLabel, row, 0, (Qt::AlignTop
                                                | Qt::AlignLeft));
    gridLayout->addWidget(m_treeView, row, 1, 2, 1);
    row++;
    gridLayout->addWidget(createMacroButtonsWidget(), row, 0);
    row++;
    
    for (int32_t iRow = 0; iRow < gridLayout->rowCount(); iRow++) {
        gridLayout->setRowStretch(iRow, 0);
    }
    
    m_macroWidget = createMacroWidget();
    m_commandWidget = createCommandWidget();
    m_stackedWidget = new QStackedWidget();
    m_stackedWidget->addWidget(m_macroWidget);
    m_stackedWidget->addWidget(m_commandWidget);
    m_stackedWidget->setCurrentWidget(m_macroWidget);
    
    m_dialogButtonBox = new QDialogButtonBox(QDialogButtonBox::Close);
    m_runButton = m_dialogButtonBox->addButton("Run", QDialogButtonBox::ApplyRole);
    QObject::connect(m_dialogButtonBox, &QDialogButtonBox::rejected,
                     this, &WuQMacroDialog::close);
    QObject::connect(m_dialogButtonBox, &QDialogButtonBox::clicked,
                     this, &WuQMacroDialog::buttonBoxButtonClicked);
    
    QVBoxLayout* dialogLayout = new QVBoxLayout(this);
    dialogLayout->addLayout(gridLayout);
    dialogLayout->addWidget(m_stackedWidget);
    dialogLayout->addWidget(m_dialogButtonBox);
    
    updateDialogContents();
    
    QPushButton* closeButton = m_dialogButtonBox->button(QDialogButtonBox::Close);
    CaretAssert(closeButton);
    closeButton->setDefault(true);
}

/**
 * Destructor.
 */
WuQMacroDialog::~WuQMacroDialog()
{
}

/**
 * @return New instance of widget containing macro buttons
 */
QWidget*
WuQMacroDialog::createMacroButtonsWidget()
{
    QWidget* widget = new QWidget();
    
    m_attributesPushButton = new QPushButton("Attributes...");
    m_attributesPushButton->setToolTip("Edit the selected macro's name, description, etc");
    QObject::connect(m_attributesPushButton, &QPushButton::clicked,
                     this, &WuQMacroDialog::attributesButtonClicked);
    
    m_deletePushButton = new QPushButton("Delete...");
    m_deletePushButton->setToolTip("Delete the selected macro");
    QObject::connect(m_deletePushButton, &QPushButton::clicked,
                     this, &WuQMacroDialog::deleteButtonClicked);
    
    m_importPushButton = new QPushButton("Import...");
    m_importPushButton->setToolTip("Import a macro");
    QObject::connect(m_importPushButton, &QPushButton::clicked,
                     this, &WuQMacroDialog::importButtonClicked);
    
    m_exportPushButton = new QPushButton("Export...");
    m_exportPushButton->setToolTip("Import a macro");
    QObject::connect(m_exportPushButton, &QPushButton::clicked,
                     this, &WuQMacroDialog::exportButtonClicked);
    
    QVBoxLayout* macroButtonsLayout = new QVBoxLayout(widget);
    macroButtonsLayout->setSpacing(4);
    macroButtonsLayout->addWidget(m_attributesPushButton);
    macroButtonsLayout->addSpacing(15);
    macroButtonsLayout->addWidget(m_deletePushButton);
    macroButtonsLayout->addSpacing(15);
    macroButtonsLayout->addWidget(m_importPushButton);
    macroButtonsLayout->addWidget(m_exportPushButton);
    macroButtonsLayout->addStretch();
    
    return widget;
}

/**
 * Update buttons based upon selected item(s)
 */
void
WuQMacroDialog::updateButtons()
{
    bool macroSelected(false);
    WuQMacroStandardItemTypeEnum::Enum itemType = getSelectedItemType();
    switch (itemType) {
        case WuQMacroStandardItemTypeEnum::INVALID:
            break;
        case WuQMacroStandardItemTypeEnum::MACRO:
            macroSelected = true;
            break;
        case WuQMacroStandardItemTypeEnum::MACRO_COMMAND:
            break;
    }
    
    m_attributesPushButton->setEnabled(macroSelected);
    m_deletePushButton->setEnabled(macroSelected);
}

/**
 * @return The widget displayed when a macro is selected
 */
QWidget*
WuQMacroDialog::createMacroWidget()
{
    QLabel* descriptionLabel = new QLabel("Macro Description:");
    m_macroDescriptionLabel = new QLabel("");
    m_macroDescriptionLabel->setWordWrap(true);
    m_macroDescriptionLabel->setMinimumHeight(40);
    m_macroDescriptionLabel->setAlignment(Qt::AlignTop
                                          | Qt::AlignLeft);
    
    QFrame* horizontalLine = new QFrame();
    horizontalLine->setMidLineWidth(1);
    horizontalLine->setLineWidth(1);
    horizontalLine->setFrameStyle(QFrame::HLine | QFrame::Sunken);

    QWidget* widget = new QWidget();
    QVBoxLayout* dialogLayout = new QVBoxLayout(widget);
    dialogLayout->addWidget(descriptionLabel);
    dialogLayout->addWidget(m_macroDescriptionLabel);
    dialogLayout->addWidget(horizontalLine);
    dialogLayout->addWidget(createRunOptionsWidget());

    return widget;
}


/**
 * @return New instance of widget containing macro run options
 */
QWidget*
WuQMacroDialog::createRunOptionsWidget()
{
    QWidget* widget = new QWidget();
    
    QLabel* runOptionsLabel = new QLabel("Run Macro Options: ");
    m_runOptionLoopCheckBox = new QCheckBox("Loop");
    m_runOptionLoopCheckBox->setChecked(false);
    m_runOptionLoopCheckBox->setEnabled(false);
    QObject::connect(m_runOptionLoopCheckBox, &QCheckBox::clicked,
                     this, &WuQMacroDialog::runOptionLoopCheckBoxClicked);

    m_runOptionMoveMouseCheckBox = new QCheckBox("Move mouse to to highlight controls");
    m_runOptionMoveMouseCheckBox->setChecked(true);
    m_runOptionMoveMouseCheckBox->setToolTip("As macro runs, the mouse is moved to\n"
                                             "highlight user-interface controls");
    QObject::connect(m_runOptionMoveMouseCheckBox, &QCheckBox::clicked,
                     this, &WuQMacroDialog::runOptionMoveMouseCheckBoxClicked);

    QLabel* runOptionsDelayLabel = new QLabel("Delay (seconds) between commands");
    m_runOptionDelayBetweenCommandsSpinBox = new QDoubleSpinBox();
    m_runOptionDelayBetweenCommandsSpinBox->setMinimum(0.0);
    m_runOptionDelayBetweenCommandsSpinBox->setMaximum(1000.0);
    m_runOptionDelayBetweenCommandsSpinBox->setSingleStep(0.1);
    m_runOptionDelayBetweenCommandsSpinBox->setDecimals(1);
    m_runOptionDelayBetweenCommandsSpinBox->setValue(1.0);
    m_runOptionDelayBetweenCommandsSpinBox->setToolTip("Pause for this amount of time");
    m_runOptionDelayBetweenCommandsSpinBox->setFixedWidth(80);
    QObject::connect(m_runOptionDelayBetweenCommandsSpinBox, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
                     this, &WuQMacroDialog::runOptionDelaySpinBoxValueChanged);
    
    QGridLayout* runOptionsLayout = new QGridLayout(widget);
    runOptionsLayout->setColumnMinimumWidth(0, 10);
    runOptionsLayout->addWidget(runOptionsLabel, 0, 0, 1, 3);
    runOptionsLayout->addWidget(m_runOptionLoopCheckBox, 1, 1, 1, 2);
    runOptionsLayout->addWidget(m_runOptionMoveMouseCheckBox, 2, 1, 1, 2);
    runOptionsLayout->addWidget(m_runOptionDelayBetweenCommandsSpinBox, 3, 1);
    runOptionsLayout->addWidget(runOptionsDelayLabel, 3, 2);
    
    return widget;
}

/**
 * Called when run options delay between commands spin box value changed
 *
 * @param value
 *     New value.
 */
void
WuQMacroDialog::runOptionDelaySpinBoxValueChanged(float value)
{
    WuQMacroExecutorOptions* options = WuQMacroManager::instance()->getExecutorOptions();
    CaretAssert(options);
    options->setSecondsDelayBetweenCommands(value);
}

/**
 * Called when run options move mouse checkbox is changed
 *
 * @param value
 *     New value.
 */
void
WuQMacroDialog::runOptionMoveMouseCheckBoxClicked(bool checked)
{
    WuQMacroExecutorOptions* options = WuQMacroManager::instance()->getExecutorOptions();
    CaretAssert(options);
    options->setShowMouseMovement(checked);
}

/**
 * Called when run options loop checkbox is changed
 *
 * @param value
 *     New value.
 */
void
WuQMacroDialog::runOptionLoopCheckBoxClicked(bool checked)
{
    WuQMacroExecutorOptions* options = WuQMacroManager::instance()->getExecutorOptions();
    CaretAssert(options);
    options->setLooping(checked);
}

/**
 * Called when a button in dialog is clicked
 *
 * @param button
 *     Button that was clicked.
 */
void
WuQMacroDialog::buttonBoxButtonClicked(QAbstractButton* button)
{
    if (button == m_runButton) {
        runSelectedMacro();
    }
}

/**
 * @return The widget displayed when a commnand is selected
 */
QWidget*
WuQMacroDialog::createCommandWidget()
{
    QLabel* titleLabel  = new QLabel("Title");
    m_commandTitleLabel = new QLabel();
    
    QLabel* nameLabel  = new QLabel("Name");
    m_commandNameLabel = new QLabel();
    
    QLabel* typeLabel = new QLabel("Type");
    m_commandTypeLabel = new QLabel();
    
    QLabel* valueOneLabel = new QLabel("Value");
    m_commandValueOnePushButton = new QPushButton("Set...");
    m_commandValueOnePushButton->setSizePolicy(QSizePolicy::Fixed,
                                               m_commandValueOnePushButton->sizePolicy().verticalPolicy());
    QObject::connect(m_commandValueOnePushButton, &QPushButton::clicked,
                     [=] { setMacroCommandValue(ValueIndex::ONE); });
    m_commandValueOneLabel  = new QLabel();
    
    QLabel* valueTwoLabel = new QLabel("Alt Value");
    m_commandValueTwoPushButton = new QPushButton("Set...");
    m_commandValueTwoPushButton->setSizePolicy(QSizePolicy::Fixed,
                                               m_commandValueTwoPushButton->sizePolicy().verticalPolicy());
    QObject::connect(m_commandValueTwoPushButton, &QPushButton::clicked,
                     [=] { setMacroCommandValue(ValueIndex::TWO); });
    m_commandValueTwoLabel  = new QLabel();
    
    QLabel* toolTipLabel = new QLabel("ToolTip");
    m_commandToolTip     = new QLabel();
    m_commandToolTip->setWordWrap(true);
    
    QGridLayout* layout = new QGridLayout();
    layout->setColumnStretch(0, 0);
    layout->setColumnStretch(1, 0);
    layout->setColumnStretch(2, 100);
    int row = 0;
    layout->addWidget(titleLabel, row, 0);
    layout->addWidget(m_commandTitleLabel, row, 1, 1, 2);
    row++;
    layout->addWidget(nameLabel, row, 0);
    layout->addWidget(m_commandNameLabel, row, 1, 1, 2);
    row++;
    layout->addWidget(typeLabel, row, 0);
    layout->addWidget(m_commandTypeLabel, row, 1, 1, 2);
    row++;
    layout->addWidget(valueOneLabel, row, 0);
    layout->addWidget(m_commandValueOnePushButton, row, 1);
    layout->addWidget(m_commandValueOneLabel, row, 2);
    row++;
    layout->addWidget(valueTwoLabel, row, 0);
    layout->addWidget(m_commandValueTwoPushButton, row, 1);
    layout->addWidget(m_commandValueTwoLabel, row, 2);
    row++;
    layout->addWidget(toolTipLabel, row, 0);
    layout->addWidget(m_commandToolTip, row, 1, 1, 2);
    
    QWidget* widget = new QWidget();
    QVBoxLayout* widgetLayout = new QVBoxLayout(widget);
    widgetLayout->addLayout(layout);
    widgetLayout->addStretch();
    
    return widget;
}

/**
 * Update content of the dialog
 */
void
WuQMacroDialog::updateDialogContents()
{
    QString selectedUniqueIdentifer;
    const QVariant dataSelected = m_macroGroupComboBox->currentData();
    if (dataSelected.isValid()) {
        if (dataSelected.type() == QVariant::String) {
            selectedUniqueIdentifer = dataSelected.toString();
        }
    }

    m_macroGroups = WuQMacroManager::instance()->getMacroGroups();
    
    m_macroGroupComboBox->clear();
    for (auto mg : m_macroGroups) {
        m_macroGroupComboBox->addItem(mg->getName(),
                                      mg->getUniqueIdentifier());
    }
    
    int32_t selectedIndex = m_macroGroupComboBox->findData(selectedUniqueIdentifer);
    if (selectedIndex < 0) {
        selectedIndex = m_macroGroupComboBox->count() - 1;
    }
    if ((selectedIndex >= 0)
        && (selectedIndex < m_macroGroupComboBox->count())) {
        m_macroGroupComboBox->setCurrentIndex(selectedIndex);
    }
    
    const WuQMacroExecutorOptions* runOptions = WuQMacroManager::instance()->getExecutorOptions();
    CaretAssert(runOptions);
    QSignalBlocker delaySpinBoxBlocker(m_runOptionDelayBetweenCommandsSpinBox);
    m_runOptionDelayBetweenCommandsSpinBox->setValue(runOptions->getSecondsDelayBetweenCommands());
    m_runOptionMoveMouseCheckBox->setChecked(runOptions->isShowMouseMovement());
    m_runOptionLoopCheckBox->setChecked(runOptions->isLooping());
    
    macroGroupComboBoxActivated(selectedIndex);
    updateButtons();
}

/**
 * Called when an item in the tree view is clicked
 */
void
WuQMacroDialog::treeViewItemClicked(const QModelIndex& modelIndex)
{
    QStandardItemModel* selectedModel = NULL;
    if (modelIndex.isValid()) {
        const QAbstractItemModel* abstractModel = modelIndex.model();
        if (abstractModel != NULL) {
            const QStandardItemModel* constModel = qobject_cast<const QStandardItemModel*>(abstractModel);
            if (constModel != NULL) {
                selectedModel = const_cast<QStandardItemModel*>(constModel);
            }
        }
    }
    
    WuQMacro* macro(NULL);
    WuQMacroCommand* macroCommand(NULL);
    
    if (selectedModel != NULL) {
        QStandardItem* selectedItem = selectedModel->itemFromIndex(modelIndex);
        bool validFlag(false);
        const WuQMacroStandardItemTypeEnum::Enum itemType = WuQMacroStandardItemTypeEnum::fromIntegerCode(selectedItem->type(),
                                                                                                          &validFlag);
        if (validFlag) {
            switch (itemType) {
                case WuQMacroStandardItemTypeEnum::INVALID:
                    CaretAssertMessage(0, "Type should never be invalid");
                    break;
                case WuQMacroStandardItemTypeEnum::MACRO:
                    macro = dynamic_cast<WuQMacro*>(selectedItem);
                    CaretAssert(macro);
                    m_stackedWidget->setCurrentWidget(m_macroWidget);
                    break;
                case WuQMacroStandardItemTypeEnum::MACRO_COMMAND:
                    macroCommand = dynamic_cast<WuQMacroCommand*>(selectedItem);
                    CaretAssert(macroCommand);
                    m_stackedWidget->setCurrentWidget(m_commandWidget);
                    break;
            }
        }
        else {
            CaretAssertMessage(0,
                               ("Invalid StandardItemModel type=" + AString::number(selectedItem->type())));
        }
    }
    
    updateMacroWidget(macro);
    updateCommandWidget(macroCommand);
    updateButtons();
}

/**
 * Called when macro group combo box selection is made
 */
void
WuQMacroDialog::macroGroupComboBoxActivated(int)
{
    WuQMacroGroup* selectedGroup = getSelectedMacroGroup();

    if (selectedGroup != NULL) {
        m_treeView->setModel(selectedGroup);
        treeViewItemClicked(m_treeView->currentIndex());
    }
    else {
        m_treeView->setModel(new QStandardItemModel());
    }
}

/**
 * Update the macro widget with the given macro
 *
 * @param macro
 *     The macro (may be NULL)
 */
void
WuQMacroDialog::updateMacroWidget(WuQMacro* macro)
{
    QString text;
    if (macro != NULL) {
        text = macro->getDescription();
    }

    m_macroDescriptionLabel->setText(text);
}

/**
 * Update the command widget with the given macommandcro
 *
 * @param command
 *     The command (may be NULL)
 */
void
WuQMacroDialog::updateCommandWidget(WuQMacroCommand* command)
{
    bool validOneFlag = false;
    QString newDataValueOne;
    if (command != NULL) {
        newDataValueOne = command->getDataValue().toString();
        
        switch (command->getDataType()) {
            case WuQMacroDataValueTypeEnum::INVALID:
                break;
            case WuQMacroDataValueTypeEnum::BOOLEAN:
                if (command->getDataValue().toBool()) {
                    newDataValueOne = "On";
                }
                else {
                    newDataValueOne = "Off";
                }
                validOneFlag = true;
                break;
            case WuQMacroDataValueTypeEnum::FLOAT:
                validOneFlag = true;
                break;
            case WuQMacroDataValueTypeEnum::INTEGER:
                validOneFlag = true;
                break;
            case WuQMacroDataValueTypeEnum::MOUSE:
                break;
            case WuQMacroDataValueTypeEnum::STRING:
                validOneFlag = true;
                break;
        }
    }
    
    bool validTwoFlag = false;
    QString newDataValueTwo;
    if (command != NULL) {
        newDataValueTwo = command->getDataValueTwo().toString();
        
        switch (command->getDataTypeTwo()) {
            case WuQMacroDataValueTypeEnum::INVALID:
                break;
            case WuQMacroDataValueTypeEnum::BOOLEAN:
                if (command->getDataValueTwo().toBool()) {
                    newDataValueTwo = "On";
                }
                else {
                    newDataValueTwo = "Off";
                }
                validTwoFlag = true;
                break;
            case WuQMacroDataValueTypeEnum::FLOAT:
                validTwoFlag = true;
                break;
            case WuQMacroDataValueTypeEnum::INTEGER:
                validTwoFlag = true;
                break;
            case WuQMacroDataValueTypeEnum::MOUSE:
                break;
            case WuQMacroDataValueTypeEnum::STRING:
                validTwoFlag = true;
                break;
        }
    }
    
    QString title;
    QString name;
    QString type;
    QString newDataTypeTwo;
    QString toolTip;
    if (command != NULL) {
        title           = command->text();
        name            = command->getObjectName();
        type            = WuQMacroClassTypeEnum::toGuiName(command->getClassType());
        newDataTypeTwo  = WuQMacroDataValueTypeEnum::toGuiName(command->getDataTypeTwo());
        toolTip         = command->getObjectToolTip();
    }
    m_commandTitleLabel->setText(title);
    m_commandNameLabel->setText(name);
    m_commandTypeLabel->setText(type);
    m_commandValueOneLabel->setText(newDataValueOne);
    m_commandValueOnePushButton->setEnabled(validOneFlag);
    m_commandValueTwoLabel->setText("");
    m_commandValueTwoPushButton->setEnabled(false);
    if (validTwoFlag) {
        m_commandValueTwoLabel->setText(newDataValueTwo);
        m_commandValueTwoPushButton->setEnabled(true);
    }
    m_commandToolTip->setText(toolTip);
}

/**
 * @return Pointer to selected macro group or NULL if none available.
 */
WuQMacroGroup*
WuQMacroDialog::getSelectedMacroGroup()
{
    const int32_t selectedGroupIndex = m_macroGroupComboBox->currentIndex();
    if ((selectedGroupIndex >= 0)
        && (selectedGroupIndex < m_macroGroupComboBox->count())) {
        CaretAssertVectorIndex(m_macroGroups, selectedGroupIndex);
        WuQMacroGroup* group = m_macroGroups[selectedGroupIndex];
        return group;
    }
    return NULL;
}

/**
 * @return Pointer to selected macro.  If a macro command is selected, its parent
 * macro is returned (NULL if no macro is selected)
 */
WuQMacro*
WuQMacroDialog::getSelectedMacro()
{
    WuQMacro* macro(NULL);
    
    QStandardItem* selectedItem = getSelectedItem();
    if (selectedItem != NULL) {
        bool validFlag(false);
        const WuQMacroStandardItemTypeEnum::Enum itemType = WuQMacroStandardItemTypeEnum::fromIntegerCode(selectedItem->type(),
                                                                                                          &validFlag);
        if (validFlag) {
            switch (itemType) {
                case WuQMacroStandardItemTypeEnum::INVALID:
                    CaretAssertMessage(0, "Type should never be invalid");
                    break;
                case WuQMacroStandardItemTypeEnum::MACRO_COMMAND:
                {
                    /*
                     * Parent should be WuQMacro
                     */
                    QStandardItem* selectedItemParent = selectedItem->parent();
                    CaretAssert(selectedItemParent);
                    macro = dynamic_cast<WuQMacro*>(selectedItemParent);
                    CaretAssert(macro);
                }
                    break;
                case WuQMacroStandardItemTypeEnum::MACRO:
                    macro = dynamic_cast<WuQMacro*>(selectedItem);
                    CaretAssert(macro);
                    break;
            }
        }
        else {
            CaretAssertMessage(0,
                               ("Invalid StandardItemModel type=" + AString::number(selectedItem->type())));
        }
    }
    
    return macro;
}

/**
 * @return Pointer to selected macro command (NULL if no macro command is selected)
 */
WuQMacroCommand*
WuQMacroDialog::getSelectedMacroCommand()
{
    WuQMacroCommand* macroCommand(NULL);
    
    QStandardItem* selectedItem = getSelectedItem();
    if (selectedItem != NULL) {
        bool validFlag(false);
        const WuQMacroStandardItemTypeEnum::Enum itemType = WuQMacroStandardItemTypeEnum::fromIntegerCode(selectedItem->type(),
                                                                                                          &validFlag);
        if (validFlag) {
            switch (itemType) {
                case WuQMacroStandardItemTypeEnum::INVALID:
                    CaretAssertMessage(0, "Type should never be invalid");
                    break;
                case WuQMacroStandardItemTypeEnum::MACRO_COMMAND:
                    macroCommand = dynamic_cast<WuQMacroCommand*>(selectedItem);
                    CaretAssert(macroCommand);
                    break;
                case WuQMacroStandardItemTypeEnum::MACRO:
                    break;
            }
        }
        else {
            CaretAssertMessage(0,
                               ("Invalid StandardItemModel type=" + AString::number(selectedItem->type())));
        }
    }
    
    return macroCommand;
}

/**
 * @return The selected item (NULL if invalid)
 */
QStandardItem*
WuQMacroDialog::getSelectedItem() const
{
    QModelIndex modelIndex = m_treeView->currentIndex();
    if (modelIndex.isValid()) {
        QAbstractItemModel* abstractModel = m_treeView->model();
        if (abstractModel != NULL) {
            QStandardItemModel* model = qobject_cast<QStandardItemModel*>(abstractModel);
            if (model != NULL) {
                QStandardItem* item = model->itemFromIndex(modelIndex);
                return item;
            }
        }
    }
    
    return NULL;
}

/**
 * @return The selected item type
 */
WuQMacroStandardItemTypeEnum::Enum
WuQMacroDialog::getSelectedItemType() const
{
    WuQMacroStandardItemTypeEnum::Enum itemType = WuQMacroStandardItemTypeEnum::INVALID;
    QStandardItem* item = getSelectedItem();
    if (item != NULL) {
        bool validFlag(false);
        itemType = WuQMacroStandardItemTypeEnum::fromIntegerCode(item->type(),
                                                                 &validFlag);
    }
    
    return itemType;
}

/**
 * Run the selected macro
 */
void
WuQMacroDialog::runSelectedMacro()
{
    if (WuQMacroManager::instance()->isModeRecording()) {
        QMessageBox::critical(m_runButton,
                              "Error",
                              "A macro is being recorded.  Finish recording of macro.",
                              QMessageBox::Ok,
                              QMessageBox::NoButton);
        return;
    }
    
    WuQMacro* macro = getSelectedMacro();
    if (macro != NULL) {
        if (macro->getNumberOfMacroCommands() <= 0) {
            QMessageBox::critical(m_runButton,
                                  "Error",
                                  "Macro does not contain any commands",
                                  QMessageBox::Ok,
                                  QMessageBox::NoButton);
            return;
        }
        
        WuQMacroManager::instance()->runMacro(parentWidget(),
                                              macro);
    }
}

/**
 * Called when the attributes button is clicked
 */
void
WuQMacroDialog::attributesButtonClicked()
{
    WuQMacro* macro = getSelectedMacro();
    if (macro != NULL) {
        if (WuQMacroManager::instance()->editMacroAttributes(this,
                                                             macro)) {
            updateDialogContents();
        }
    }
}

/**
 * Called when the delete button is clicked
 */
void
WuQMacroDialog::deleteButtonClicked()
{
    WuQMacroGroup* macroGroup = getSelectedMacroGroup();
    WuQMacro* macro = getSelectedMacro();
    if ((macroGroup != NULL)
        && (macro != NULL)) {
        if (WuQMacroManager::instance()->deleteMacro(m_deletePushButton, macroGroup, macro)) {
            updateDialogContents();
        }
    }
}

/**
 * Called when import button is clicked
 */
void
WuQMacroDialog::importButtonClicked()
{
    WuQMacroGroup* macroGroup = getSelectedMacroGroup();
    if (macroGroup != NULL) {
        if (WuQMacroManager::instance()->importMacros(m_importPushButton,
                                                      macroGroup)) {
            updateDialogContents();
        }
    }
}

/**
 * Called when export button is clicked
 */
void
WuQMacroDialog::exportButtonClicked()
{
    WuQMacroGroup* macroGroup = getSelectedMacroGroup();
    WuQMacro* macro = getSelectedMacro();

    if (WuQMacroManager::instance()->exportMacros(m_exportPushButton,
                                                  macroGroup,
                                                  macro)) {
        updateDialogContents();
    }
}

/**
 * Set the command value for the given value index
 *
 * @param valueIndex
 *    Value one or two
 */
void
WuQMacroDialog::setMacroCommandValue(const ValueIndex valueIndex)
{
    WuQMacroCommand* macroCommand = getSelectedMacroCommand();
    if (macroCommand == NULL) {
        return;
    }
    
    WuQMacroDataValueTypeEnum::Enum dataType = WuQMacroDataValueTypeEnum::INVALID;
    QVariant dataValue;
    QWidget* parentWidget(NULL);
    switch (valueIndex) {
        case ValueIndex::ONE:
            dataType = macroCommand->getDataType();
            dataValue = macroCommand->getDataValue();
            parentWidget = m_commandValueOnePushButton;
            break;
        case ValueIndex::TWO:
            dataType = macroCommand->getDataTypeTwo();
            dataValue = macroCommand->getDataValueTwo();
            parentWidget = m_commandValueTwoPushButton;
            break;
    }
    
    bool validFlag(false);
    switch (dataType) {
        case WuQMacroDataValueTypeEnum::BOOLEAN:
        {
            QStringList items;
            items.push_back("On");
            items.push_back("Off");
            
            bool ok(false);
            int defaultIndex = (dataValue.toBool() ? 0 : 1);
            const QString text = QInputDialog::getItem(parentWidget,
                                                       "New Status",
                                                       "New Status",
                                                       items,
                                                       defaultIndex,
                                                       false,
                                                       &ok);
            if (ok
                && ( ! text.isEmpty())) {
                bool result(text == items.at(0));
                dataValue.setValue(result);
                validFlag = true;
            }
        }
            break;
        case WuQMacroDataValueTypeEnum::FLOAT:
        {
            bool ok(false);
            const float f = QInputDialog::getDouble(parentWidget,
                                                    "New Value",
                                                    "New Value",
                                                    dataValue.toDouble(),
                                                    -2147483647,
                                                    2147483647,
                                                    3,
                                                    &ok);
            if (ok) {
                dataValue.setValue(f);
                validFlag = true;
            }
        }
            break;
        case WuQMacroDataValueTypeEnum::INTEGER:
        {
            bool ok(false);
            const int i = QInputDialog::getInt(parentWidget,
                                               "New Value",
                                               "New Value",
                                               dataValue.toInt(),
                                               -2147483647,
                                               2147483647,
                                               1,
                                               &ok);
            if (ok) {
                dataValue.setValue(i);
                validFlag = true;
            }
        }
            break;
        case WuQMacroDataValueTypeEnum::INVALID:
            break;
        case WuQMacroDataValueTypeEnum::MOUSE:
            break;
        case WuQMacroDataValueTypeEnum::STRING:
        {
            bool ok(false);
            const QString text = QInputDialog::getText(parentWidget,
                                                       "New Text",
                                                       "New Text",
                                                       QLineEdit::Normal,
                                                       dataValue.toString(),
                                                       &ok);
            if (ok) {
                dataValue.setValue(text);
                validFlag = true;
            }
        }
            break;
    }

    if (validFlag) {
        switch (valueIndex) {
            case ValueIndex::ONE:
                macroCommand->setDataValue(dataValue);
                break;
            case ValueIndex::TWO:
                macroCommand->setDataValueTwo(dataValue);
                break;
        }
        
        WuQMacro* macro = getSelectedMacro();
        if (macro != NULL) {
            WuQMacroManager::instance()->macroWasModified(macro);
        }
    }
    
    updateCommandWidget(macroCommand);
}


