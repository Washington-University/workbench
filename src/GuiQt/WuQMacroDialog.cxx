
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

#include <QAction>
#include <QCheckBox>
#include <QComboBox>
#include <QDialogButtonBox>
#include <QDoubleSpinBox>
#include <QGridLayout>
#include <QInputDialog>
#include <QLabel>
#include <QLineEdit>
#include <QMenu>
#include <QMessageBox>
#include <QPainter>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QStackedWidget>
#include <QTreeView>
#include <QToolButton>
#include <QVBoxLayout>

#include "CaretAssert.h"
#include "CaretLogger.h"
#include "WuQMacro.h"
#include "WuQMacroCommand.h"
#include "WuQMacroGroup.h"
#include "WuQMacroExecutor.h"
#include "WuQMacroManager.h"
#include "WuQMacroShortCutKeyComboBox.h"
#include "WuQMacroStandardItemTypeEnum.h"
#include "WuQtUtilities.h"

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
    
//    QLabel* macrosLabel = new QLabel("Macro:");
    QLabel* macroGroupLabel = new QLabel("Macros in:");
    
    m_macroGroupComboBox = new QComboBox();
    QObject::connect(m_macroGroupComboBox, static_cast<void(QComboBox::*)(int)>(&QComboBox::activated),
                     this, &WuQMacroDialog::macroGroupComboBoxActivated);
    
    m_macroGroupToolButton = new QToolButton();
    m_macroGroupToolButton->setText("...");
    m_macroGroupToolButton->setToolTip("Export and import macro groups");
    QObject::connect(m_macroGroupToolButton, &QToolButton::clicked,
                     this, &WuQMacroDialog::macroGroupToolButtonClicked);
    
    QGridLayout* gridLayout = new QGridLayout();
    gridLayout->setContentsMargins(0, 0, 0, 0);
    gridLayout->setRowStretch(0, 0);
    gridLayout->setRowStretch(1, 0);
    gridLayout->setRowStretch(2, 100);
    int row = 0;
    gridLayout->addWidget(macroGroupLabel, row, 0);
    gridLayout->addWidget(m_macroGroupComboBox, row, 1);
    gridLayout->addWidget(m_macroGroupToolButton, row, 2);
    row++;
    gridLayout->addWidget(createHorizontalLine(), row, 0, 1, 3);
    row++;
    gridLayout->addWidget(createMacroRunAndEditingToolButtons(), row, 0, 1, 3);
//    gridLayout->addWidget(macrosLabel, row, 0, (Qt::AlignTop| Qt::AlignLeft));
    row++;
    gridLayout->addWidget(createMacroAndCommandSelectionWidget(), row, 0, 1, 3);
    row++;
    
    for (int32_t iRow = 0; iRow < gridLayout->rowCount(); iRow++) {
        gridLayout->setRowStretch(iRow, 0);
    }
    
    m_macroWidget = createMacroDisplayWidget();
    m_commandWidget = createCommandDisplayWidget();
    m_stackedWidget = new QStackedWidget();
    m_stackedWidget->addWidget(m_macroWidget);
    m_stackedWidget->addWidget(m_commandWidget);
    m_stackedWidget->setCurrentWidget(m_macroWidget);
    
    m_dialogButtonBox = new QDialogButtonBox(QDialogButtonBox::Close);
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

QWidget*
WuQMacroDialog::createMacroRunAndEditingToolButtons()
{
    m_runMacroToolButton = new QToolButton();
    m_runMacroToolButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    m_runMacroToolButton->setText("Run");
    QPixmap runPixmap = createEditingToolButtonPixmap(m_runMacroToolButton,
                                                         EditButton::RUN);
    m_runMacroToolButton->setIcon(runPixmap);
    m_runMacroToolButton->setToolTip("Run the selected macro");
    QObject::connect(m_runMacroToolButton, &QToolButton::clicked,
                     this, &WuQMacroDialog::runMacroToolButtonClicked);
    
    m_editingMoveUpToolButton = new QToolButton();
    QPixmap moveUpPixmap = createEditingToolButtonPixmap(m_editingMoveUpToolButton,
                                                         EditButton::MOVE_UP);
    m_editingMoveUpToolButton->setIcon(moveUpPixmap);
    m_editingMoveUpToolButton->setToolTip("Move selected macro/command up");
    QObject::connect(m_editingMoveUpToolButton, &QToolButton::clicked,
                     this, &WuQMacroDialog::editingMoveUpToolButtonClicked);
    
    m_editingMoveDownToolButton = new QToolButton();
    QPixmap moveDownPixmap = createEditingToolButtonPixmap(m_editingMoveDownToolButton,
                                                         EditButton::MOVE_DOWN);
    m_editingMoveDownToolButton->setIcon(moveDownPixmap);
    m_editingMoveDownToolButton->setToolTip("Move selected macro/command down");
    QObject::connect(m_editingMoveDownToolButton, &QToolButton::clicked,
                     this, &WuQMacroDialog::editingMoveDownToolButtonClicked);
    
    m_editingDeleteToolButton = new QToolButton();
    QPixmap deletePixmap = createEditingToolButtonPixmap(m_editingDeleteToolButton,
                                                         EditButton::DELETER);
    m_editingDeleteToolButton->setIcon(deletePixmap);
    m_editingDeleteToolButton->setToolTip("Delete the selected macro/command");
    QObject::connect(m_editingDeleteToolButton, &QToolButton::clicked,
                     this, &WuQMacroDialog::editingDeleteToolButtonClicked);
    
    m_editingInsertToolButton = new QToolButton();
    QPixmap insertPixmap = createEditingToolButtonPixmap(m_editingInsertToolButton,
                                                         EditButton::INSERTER);
    m_editingInsertToolButton->setIcon(insertPixmap);
    m_editingInsertToolButton->setToolTip("Insert a macro command");
    QObject::connect(m_editingInsertToolButton, &QToolButton::clicked,
                     this, &WuQMacroDialog::editingInsertToolButtonClicked);
    
    QWidget* widget = new QWidget();
    QHBoxLayout* toolButtonLayout = new QHBoxLayout(widget);
    toolButtonLayout->setContentsMargins(0, 0, 0, 0);
    toolButtonLayout->addWidget(m_runMacroToolButton);
    toolButtonLayout->addStretch();
    toolButtonLayout->addWidget(m_editingMoveUpToolButton);
    toolButtonLayout->addWidget(m_editingMoveDownToolButton);
    toolButtonLayout->addWidget(m_editingInsertToolButton);
    toolButtonLayout->addSpacing(15);
    toolButtonLayout->addWidget(m_editingDeleteToolButton);
    
    return widget;
}

/**
 * @return the macro and command selection widget
 */
QWidget*
WuQMacroDialog::createMacroAndCommandSelectionWidget()
{
    m_treeView = new QTreeView();
    m_treeView->setHeaderHidden(true);
    QObject::connect(m_treeView, &QTreeView::clicked,
                     this, &WuQMacroDialog::treeViewItemClicked);

    return m_treeView;
}

/**
 * @return The widget displayed when a macro is selected
 */
QWidget*
WuQMacroDialog::createMacroDisplayWidget()
{
    QLabel* nameLabel = new QLabel("Name:");
    m_macroNameLabel = new QLabel();
    QPushButton* nameEditPushButton = new QPushButton("Edit...");
    nameEditPushButton->setSizePolicy(QSizePolicy::Fixed,
                                      nameEditPushButton->sizePolicy().verticalPolicy());
    QObject::connect(nameEditPushButton, &QPushButton::clicked,
                     this, &WuQMacroDialog::macroNameEditButtonClicked);
    
    QLabel* shortCutKeyLabel = new QLabel("Short Cut Key:");
    QLabel* shortCutKeyMaskLabel = new QLabel(WuQMacroManager::getShortCutKeysMask());
    m_macroShortCutKeyComboBox = new WuQMacroShortCutKeyComboBox(this);
    QObject::connect(m_macroShortCutKeyComboBox, &WuQMacroShortCutKeyComboBox::shortCutKeySelected,
                     this, &WuQMacroDialog::macroShortCutKeySelected);

    QLabel* descriptionLabel = new QLabel("Description:");
    m_macroDescriptionLabel = new QLabel("");
    m_macroDescriptionLabel->setWordWrap(true);
    m_macroDescriptionLabel->setMinimumHeight(40);
    m_macroDescriptionLabel->setAlignment(Qt::AlignTop
                                          | Qt::AlignLeft);
    
    QPushButton* descriptionEditPushButton = new QPushButton("Edit...");
    descriptionEditPushButton->setSizePolicy(QSizePolicy::Fixed,
                                             descriptionEditPushButton->sizePolicy().verticalPolicy());
    QObject::connect(descriptionEditPushButton, &QPushButton::clicked,
                     this, &WuQMacroDialog::macroDescriptionEditButtonClicked);

    QGridLayout* gridLayout = new QGridLayout();
    gridLayout->setContentsMargins(0, 0, 0, 0);
    gridLayout->setColumnStretch(0, 0);
    gridLayout->setColumnStretch(1, 0);
    gridLayout->setColumnStretch(2, 0);
    gridLayout->setColumnStretch(3, 100);
    int row = 0;
    gridLayout->addWidget(nameLabel, row, 0);
    gridLayout->addWidget(nameEditPushButton, row, 1);
    gridLayout->addWidget(m_macroNameLabel, row, 2, 1, 2);
    row++;
    gridLayout->addWidget(shortCutKeyLabel, row, 0, 1, 2);
    gridLayout->addWidget(shortCutKeyMaskLabel, row, 2);
    gridLayout->addWidget(m_macroShortCutKeyComboBox->getWidget(), row, 3, Qt::AlignLeft);
    row++;
    gridLayout->addWidget(descriptionLabel, row, 0, 1, 2);
    gridLayout->addWidget(m_macroDescriptionLabel, row, 2, 2, 2);
    row++;
    gridLayout->addWidget(descriptionEditPushButton, row, 0, 1, 2);
    row++;

    QHBoxLayout* titleLayout = new QHBoxLayout();
    titleLayout->setContentsMargins(0, 0, 0, 0);
    titleLayout->addWidget(new QLabel("Macro "));
    titleLayout->addWidget(createHorizontalLine(), 100);
    
    QHBoxLayout* runOptionsTitleLayout = new QHBoxLayout();
    runOptionsTitleLayout->setContentsMargins(0, 0, 0, 0);
    runOptionsTitleLayout->setContentsMargins(0, 0, 0, 0);
    runOptionsTitleLayout->addWidget(new QLabel("Run Macro Options "));
    runOptionsTitleLayout->addWidget(createHorizontalLine(), 100);
    
    QWidget* widget = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(widget);
    layout->addLayout(titleLayout);
    layout->addLayout(gridLayout);
    layout->addLayout(runOptionsTitleLayout);
    layout->addWidget(createRunOptionsWidget());
    layout->addStretch();

    return widget;
}

/**
 * Called to edit macro name
 */
void
WuQMacroDialog::macroNameEditButtonClicked()
{
    WuQMacro* macro = getSelectedMacro();
    if (macro != NULL) {
        bool ok(false);
        const QString text = QInputDialog::getText(this,
                                                   "New Macro Name",
                                                   "Name",
                                                   QLineEdit::Normal,
                                                   macro->getName(),
                                                   &ok);
        if (ok) {
            if ( ! text.isEmpty()) {
                macro->setName(text);
                WuQMacroManager::instance()->macroWasModified(macro);
                updateMacroWidget(macro);
            }
        }
    }
}

/**
 * Called when macro short cut key is selected
 *
 * @param shortCutKey
 *     Shortcut key that was selected
 */
void
WuQMacroDialog::macroShortCutKeySelected(const WuQMacroShortCutKeyEnum::Enum shortCutKey)
{
    WuQMacro* macro = getSelectedMacro();
    if (macro != NULL) {
        macro->setShortCutKey(shortCutKey);
        WuQMacroManager::instance()->macroWasModified(macro);
        updateMacroWidget(macro);
    }
}

/**
 * Called to edit macro description
 */
void
WuQMacroDialog::macroDescriptionEditButtonClicked()
{
    WuQMacro* macro = getSelectedMacro();
    if (macro != NULL) {
        bool ok(false);
        const QString text = QInputDialog::getMultiLineText(this,
                                                            "New Macro Description",
                                                            "Description",
                                                            macro->getDescription(),
                                                            &ok);
        if (ok) {
            if ( ! text.isEmpty()) {
                macro->setDescription(text);
                WuQMacroManager::instance()->macroWasModified(macro);
                updateMacroWidget(macro);
            }
        }
    }
}

/**
 * @return New instance of widget containing macro run options
 */
QWidget*
WuQMacroDialog::createRunOptionsWidget()
{
    QWidget* widget = new QWidget();
    
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
WuQMacroDialog::buttonBoxButtonClicked(QAbstractButton* /*button*/)
{
}

/**
 * @return The widget displayed when a commnand is selected
 */
QWidget*
WuQMacroDialog::createCommandDisplayWidget()
{
    QLabel* titleLabel  = new QLabel("Title:");
    m_commandTitleLabel = new QLabel();
    
    QLabel* nameLabel  = new QLabel("Name:");
    m_commandNameLabel = new QLabel();
    
    QLabel* typeLabel = new QLabel("Type:");
    m_commandTypeLabel = new QLabel();
    
    QLabel* valueOneLabel = new QLabel("Value:");
    m_commandValueOnePushButton = new QPushButton("Set...");
    m_commandValueOnePushButton->setSizePolicy(QSizePolicy::Fixed,
                                               m_commandValueOnePushButton->sizePolicy().verticalPolicy());
    QObject::connect(m_commandValueOnePushButton, &QPushButton::clicked,
                     [=] { setMacroCommandValue(ValueIndex::ONE); });
    m_commandValueOneLabel  = new QLabel();
    
    QLabel* valueTwoLabel = new QLabel("Alt Value:");
    m_commandValueTwoPushButton = new QPushButton("Set...");
    m_commandValueTwoPushButton->setSizePolicy(QSizePolicy::Fixed,
                                               m_commandValueTwoPushButton->sizePolicy().verticalPolicy());
    QObject::connect(m_commandValueTwoPushButton, &QPushButton::clicked,
                     [=] { setMacroCommandValue(ValueIndex::TWO); });
    m_commandValueTwoLabel  = new QLabel();
    
    QLabel* toolTipLabel = new QLabel("ToolTip:");
    m_commandToolTip     = new QLabel();
    m_commandToolTip->setWordWrap(true);
    
    QGridLayout* layout = new QGridLayout();
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setColumnStretch(0, 0);
    layout->setColumnStretch(1, 0);
    layout->setColumnStretch(2, 100);
    int row = 0;
    layout->addWidget(titleLabel, row, 0);
    layout->addWidget(m_commandTitleLabel, row, 1, 1, 2);
    row++;
    layout->addWidget(valueOneLabel, row, 0);
    layout->addWidget(m_commandValueOnePushButton, row, 1);
    layout->addWidget(m_commandValueOneLabel, row, 2);
    row++;
    layout->addWidget(valueTwoLabel, row, 0);
    layout->addWidget(m_commandValueTwoPushButton, row, 1);
    layout->addWidget(m_commandValueTwoLabel, row, 2);
    row++;
    layout->addWidget(nameLabel, row, 0);
    layout->addWidget(m_commandNameLabel, row, 1, 1, 2);
    row++;
    layout->addWidget(typeLabel, row, 0);
    layout->addWidget(m_commandTypeLabel, row, 1, 1, 2);
    row++;
    layout->addWidget(toolTipLabel, row, 0);
    layout->addWidget(m_commandToolTip, row, 1, 1, 2);
    row++;
    
    QHBoxLayout* titleLayout = new QHBoxLayout();
    titleLayout->setContentsMargins(0, 0, 0, 0);
    titleLayout->addWidget(new QLabel("Command"));
    titleLayout->addWidget(createHorizontalLine(), 100);
    
    QWidget* widget = new QWidget();
    QVBoxLayout* widgetLayout = new QVBoxLayout(widget);
    widgetLayout->addLayout(titleLayout);
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
    updateEditingToolButtons();
}

/**
 * Called when macro group combo box selection is made
 */
void
WuQMacroDialog::macroGroupComboBoxActivated(int)
{
    WuQMacroGroup* selectedGroup = getSelectedMacroGroup();

    if (selectedGroup != NULL) {
        QModelIndex selectedIndex;
        if (m_treeView->model() != selectedGroup) {
            /*
             * Model has changed, select first macro
             */
            if (selectedGroup->getNumberOfMacros() > 0) {
                selectedIndex = selectedGroup->indexFromItem(selectedGroup->getMacroAtIndex(0));
            }
        }
        else {
            selectedIndex = m_treeView->currentIndex();
        }
        
        m_treeView->setModel(selectedGroup);
    
        if (selectedIndex.isValid()) {
            m_treeView->setCurrentIndex(selectedIndex);
        }
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
    QString name;
    WuQMacroShortCutKeyEnum::Enum shortCutKey = WuQMacroShortCutKeyEnum::Key_None;
    QString text;
    if (macro != NULL) {
        name = macro->getName();
        text = macro->getDescription();
        shortCutKey = macro->getShortCutKey();
    }

    m_macroNameLabel->setText(name);
    m_macroShortCutKeyComboBox->setSelectedShortCutKey(shortCutKey);
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
            case WuQMacroDataValueTypeEnum::NONE:
                newDataValueOne = "";
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
            case WuQMacroDataValueTypeEnum::NONE:
                newDataValueTwo = "";
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
 * Called when run button is clicked
 */
void
WuQMacroDialog::runMacroToolButtonClicked()
{
    if (WuQMacroManager::instance()->isModeRecording()) {
        QMessageBox::critical(m_runMacroToolButton,
                              "Error",
                              "A macro is being recorded.  Finish recording of macro.",
                              QMessageBox::Ok,
                              QMessageBox::NoButton);
        return;
    }
    
    WuQMacro* macro = getSelectedMacro();
    if (macro != NULL) {
        if (macro->getNumberOfMacroCommands() <= 0) {
            QMessageBox::critical(m_runMacroToolButton,
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
 * Called when import item is selected
 */
void
WuQMacroDialog::importMacroGroupActionTriggered()
{
    WuQMacroGroup* macroGroup = getSelectedMacroGroup();
    if (macroGroup != NULL) {
        if (WuQMacroManager::instance()->importMacros(m_macroGroupToolButton,
                                                      macroGroup)) {
            updateDialogContents();
        }
    }
}

/**
 * Called when export item is selected
 */
void
WuQMacroDialog::exportMacroGroupActionTriggered()
{
    WuQMacroGroup* macroGroup = getSelectedMacroGroup();
    WuQMacro* macro = getSelectedMacro();

    if (WuQMacroManager::instance()->exportMacros(m_macroGroupToolButton,
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
    QString dataValueUpdateText;
    switch (valueIndex) {
        case ValueIndex::ONE:
            dataType = macroCommand->getDataType();
            dataValue = macroCommand->getDataValue();
            dataValueUpdateText = macroCommand->getDataValueUpdateLabelText();
            parentWidget = m_commandValueOnePushButton;
            break;
        case ValueIndex::TWO:
            dataType = macroCommand->getDataTypeTwo();
            dataValue = macroCommand->getDataValueTwo();
            dataValueUpdateText = macroCommand->getDataValueTwoUpdateLabelText();
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
                                                       dataValueUpdateText,
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
                                                    dataValueUpdateText,
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
                                               dataValueUpdateText,
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
        case WuQMacroDataValueTypeEnum::NONE:
            break;
        case WuQMacroDataValueTypeEnum::STRING:
        {
            bool ok(false);
            const QString text = QInputDialog::getText(parentWidget,
                                                       "New Text",
                                                       dataValueUpdateText,
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
    
    updateDialogContents();
}

/**
 * @return a horizontal line
 */
QWidget*
WuQMacroDialog::createHorizontalLine() const
{
    QFrame* horizontalLine = new QFrame();
    horizontalLine->setMidLineWidth(1);
    horizontalLine->setLineWidth(1);
    horizontalLine->setFrameStyle(QFrame::HLine | QFrame::Sunken);
    return horizontalLine;
}

/**
 * Called when macro group tool button is clicked
 */
void
WuQMacroDialog::macroGroupToolButtonClicked()
{
    QMenu* menu = new QMenu(this);
    
    QAction* importAction = menu->addAction("Import...");
    QObject::connect(importAction, &QAction::triggered,
                     this, &WuQMacroDialog::importMacroGroupActionTriggered);
    
    QAction* exportAction = menu->addAction("Export...");
    QObject::connect(exportAction, &QAction::triggered,
                     this, &WuQMacroDialog::exportMacroGroupActionTriggered);
    
    menu->exec(mapToGlobal(m_macroGroupToolButton->pos()));
    
    delete menu;
}

/**
 * @return Instance of macro group menu
 */
QMenu*
WuQMacroDialog::createMacroGroupMenu()
{
    QMenu* menu = new QMenu(this);
    
    QAction* importAction = menu->addAction("Import...");
    QObject::connect(importAction, &QAction::triggered,
                     this, &WuQMacroDialog::importMacroGroupActionTriggered);
    
    QAction* exportAction = menu->addAction("Export...");
    QObject::connect(exportAction, &QAction::triggered,
                     this, &WuQMacroDialog::exportMacroGroupActionTriggered);
    
    return menu;
}

/**
 * Called when editing move up button clicked
 */
void
WuQMacroDialog::editingMoveUpToolButtonClicked()
{
    WuQMacroGroup* macroGroup = getSelectedMacroGroup();
    WuQMacro* macro = getSelectedMacro();
    WuQMacroCommand* command = getSelectedMacroCommand();
    
    switch (getSelectedItemType()) {
        case WuQMacroStandardItemTypeEnum::INVALID:
            break;
        case WuQMacroStandardItemTypeEnum::MACRO:
            if (macroGroup != NULL) {
                if (macro != NULL) {
                    macroGroup->moveMacroUp(macro);
//                    updateMacroWidget(macro);
                    m_treeView->setCurrentIndex(macro->index());
                    treeViewItemClicked(macro->index());
                }
            }
            break;
        case WuQMacroStandardItemTypeEnum::MACRO_COMMAND:
            if ((macro != NULL)
                && (command != NULL)) {
                macro->moveMacroCommandUp(command);
//                updateCommandWidget(command);
                m_treeView->setCurrentIndex(command->index());
                treeViewItemClicked(command->index());
            }
            break;
    }
}

/**
 * Called when editing move down button clicked
 */
void
WuQMacroDialog::editingMoveDownToolButtonClicked()
{
    WuQMacroGroup* macroGroup = getSelectedMacroGroup();
    WuQMacro* macro = getSelectedMacro();
    WuQMacroCommand* command = getSelectedMacroCommand();
    
    switch (getSelectedItemType()) {
        case WuQMacroStandardItemTypeEnum::INVALID:
            break;
        case WuQMacroStandardItemTypeEnum::MACRO:
            if (macroGroup != NULL) {
               if (macro != NULL) {
                   macroGroup->moveMacroDown(macro);
//                   updateMacroWidget(macro);
                   m_treeView->setCurrentIndex(macro->index());
                   treeViewItemClicked(macro->index());
               }
            }
            break;
        case WuQMacroStandardItemTypeEnum::MACRO_COMMAND:
            if ((macro != NULL)
                && (command != NULL)) {
                macro->moveMacroCommandDown(command);
//                updateCommandWidget(command);
                m_treeView->setCurrentIndex(command->index());
                treeViewItemClicked(command->index());
            }
            break;
    }
}

/**
 * Called when editing delete button clicked
 */
void
WuQMacroDialog::editingDeleteToolButtonClicked()
{
    WuQMacroGroup* macroGroup = getSelectedMacroGroup();
    WuQMacro* macro = getSelectedMacro();
    WuQMacroCommand* command = getSelectedMacroCommand();
    
    switch (getSelectedItemType()) {
        case WuQMacroStandardItemTypeEnum::INVALID:
            break;
        case WuQMacroStandardItemTypeEnum::MACRO:
            if ((macroGroup != NULL)
                && (macro != NULL)) {
                int32_t macroIndex = macroGroup->getIndexOfMacro(macro);
                if (WuQMacroManager::instance()->deleteMacro(m_editingDeleteToolButton,
                                                             macroGroup,
                                                             macro)) {
                    
                    updateDialogContents();
                    
                    CaretAssert(macroIndex >= 0);
                    if (macroIndex >= macroGroup->getNumberOfMacros()) {
                        macroIndex = macroGroup->getNumberOfMacros() - 1;
                    }
                    if ((macroIndex >= 0)
                        && (macroIndex < macroGroup->getNumberOfMacros())) {
                        QModelIndex modelIndex = macroGroup->getMacroAtIndex(macroIndex)->index();
                        m_treeView->setCurrentIndex(modelIndex);
                        treeViewItemClicked(modelIndex);
                    }
                }
            }
            break;
        case WuQMacroStandardItemTypeEnum::MACRO_COMMAND:
            if ((macro != NULL)
                && (command != NULL)) {
                if (WuQMacroManager::instance()->deleteMacroCommand(m_editingDeleteToolButton,
                                                                    macroGroup,
                                                                    macro,
                                                                    command)) {
                    updateDialogContents();
                }
            }
            break;
    }
}

/**
 * Called when editing insert button clicked
 */
void
WuQMacroDialog::editingInsertToolButtonClicked()
{
    std::cout << "Inserrt button clicked" << std::endl;
}

/**
 * Update editing buttons after item selected
 * in macro/command tree view
 */
void
WuQMacroDialog::updateEditingToolButtons()
{
    bool runValid(false);
    bool insertValid(false);
    bool deleteValid(false);
    bool moveUpValid(false);
    bool moveDownValid(false);

    WuQMacroGroup* macroGroup = getSelectedMacroGroup();
    WuQMacro* macro = getSelectedMacro();
    WuQMacroCommand* command = getSelectedMacroCommand();
    
    switch (getSelectedItemType()) {
        case WuQMacroStandardItemTypeEnum::INVALID:
            break;
        case WuQMacroStandardItemTypeEnum::MACRO:
            if (macroGroup != NULL) {
                if (macro != NULL) {
                    const int32_t macroIndex = macroGroup->getIndexOfMacro(macro);
                    deleteValid = true;
                    moveUpValid = (macroIndex > 0);
                    moveDownValid = (macroIndex < (macroGroup->getNumberOfMacros() - 1));
                    runValid = true;
                }
            }
            break;
        case WuQMacroStandardItemTypeEnum::MACRO_COMMAND:
        {
            if ((macro != NULL)
                && (command != NULL)) {
                const int32_t commandIndex = macro->getIndexOfMacroCommand(command);
                deleteValid   = true;
                moveUpValid   = (commandIndex > 0);
                moveDownValid = (commandIndex < (macro->getNumberOfMacroCommands() - 1));
            }
        }
            break;
    }
    
    m_runMacroToolButton->setEnabled(runValid);
    m_editingDeleteToolButton->setEnabled(deleteValid);
    m_editingInsertToolButton->setEnabled(insertValid);
    m_editingMoveDownToolButton->setEnabled(moveDownValid);
    m_editingMoveUpToolButton->setEnabled(moveUpValid);
}

/**
 * Create a pixmap for the given editing tool button
 *
 * @param editButton
 *     The edit button identifier
 * @return
 *     Pixmap for the given button
 */
QPixmap
WuQMacroDialog::createEditingToolButtonPixmap(const QWidget* widget,
                                              const EditButton editButton)
{
    CaretAssert(widget);
    const qreal pixmapSize = 22.0;
    const qreal maxValue = pixmapSize / 2.0 - 1.0;
    const qreal arrowTip = maxValue * (2.0 / 3.0);

    uint32_t pixmapOptions(static_cast<uint32_t>(WuQtUtilities::PixMapCreationOptions::TransparentBackground));
    switch (editButton) {
        case EditButton::DELETER:
            break;
        case EditButton::INSERTER:
            break;
        case EditButton::MOVE_DOWN:
            break;
        case EditButton::MOVE_UP:
            break;
        case EditButton::RUN:
            /* allow background */
            pixmapOptions = 0;
            break;
    }

    QPixmap pixmap(static_cast<int>(pixmapSize),
                   static_cast<int>(pixmapSize));
    QSharedPointer<QPainter> painter = WuQtUtilities::createPixmapWidgetPainterOriginCenter(widget,
                                                                                            pixmap,
                                                                                            pixmapOptions);
    QPen pen(painter->pen());
    pen.setWidth(3);
    painter->setPen(pen);
    
    switch (editButton) {
        case EditButton::DELETER:
            pen.setColor(Qt::red);
            painter->setPen(pen);
            painter->drawLine(QPointF(-maxValue,  maxValue),  QPointF(maxValue, -maxValue));
            painter->drawLine(QPointF(-maxValue,  -maxValue), QPointF(maxValue, maxValue));
            break;
        case EditButton::INSERTER:
            painter->drawLine(QPointF(0,  maxValue), QPointF(0, -maxValue));
            painter->drawLine(QPointF(-maxValue, 0), QPointF(maxValue, 0));
            break;
        case EditButton::MOVE_DOWN:
            painter->drawLine(QPointF(0, maxValue), QPointF(0, -maxValue));
            painter->drawLine(QPointF(0, -maxValue), QPointF(arrowTip,  -maxValue + arrowTip));
            painter->drawLine(QPointF(0, -maxValue), QPointF(-arrowTip, -maxValue + arrowTip));
            break;
        case EditButton::MOVE_UP:
            painter->drawLine(QPointF(0, maxValue), QPointF(0, -maxValue));
            painter->drawLine(QPointF(0, maxValue), QPointF(arrowTip,  maxValue - arrowTip));
            painter->drawLine(QPointF(0, maxValue), QPointF(-arrowTip, maxValue - arrowTip));
            break;
        case EditButton::RUN:
        {
            painter->setPen(Qt::green);
            painter->setBrush(Qt::green);
            const qreal p = pixmapSize / 3;
            const QPointF points[3] = {
                QPointF(-p, -maxValue),
                QPointF( p, 0.0),
                QPointF(-p,  maxValue)
            };
            painter->drawConvexPolygon(points, 3);
        }
            break;
    }
    
    return pixmap;
}




