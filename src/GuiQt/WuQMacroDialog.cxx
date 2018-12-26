
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
#include <QLabel>
#include <QListWidget>
#include <QMessageBox>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QVBoxLayout>

#include "CaretAssert.h"
#include "CaretLogger.h"
#include "WuQMacro.h"
#include "WuQMacroGroup.h"
#include "WuQMacroExecutor.h"
#include "WuQMacroManager.h"

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
    QLabel* descriptionLabel = new QLabel("Macro Description:");
    QLabel* macroGroupLabel = new QLabel("Macros in:");
    
    m_macroGroupComboBox = new QComboBox();
    QObject::connect(m_macroGroupComboBox, static_cast<void(QComboBox::*)(int)>(&QComboBox::activated),
                     this, &WuQMacroDialog::macroGroupBoxActivated);
    
    m_macrosListWidget = new QListWidget();
    QObject::connect(m_macrosListWidget, &QListWidget::currentRowChanged,
                     this, &WuQMacroDialog::macrosListWidgetCurrentRowChanged);
    
    m_macroDescriptionLabel = new QLabel("");
    m_macroDescriptionLabel->setWordWrap(true);
    m_macroDescriptionLabel->setMinimumHeight(40);
    m_macroDescriptionLabel->setAlignment(Qt::AlignTop
                                          | Qt::AlignLeft);
    
    QFrame* horizontalLine = new QFrame();
    horizontalLine->setMidLineWidth(1);
    horizontalLine->setLineWidth(1);
    horizontalLine->setFrameStyle(QFrame::HLine | QFrame::Sunken);

    QGridLayout* gridLayout = new QGridLayout();
    int row = 0;
    gridLayout->addWidget(macroGroupLabel, row, 0);
    gridLayout->addWidget(m_macroGroupComboBox, row, 1);
    row++;
    gridLayout->addWidget(macrosLabel, row, 0, (Qt::AlignTop
                                                | Qt::AlignLeft));
    gridLayout->addWidget(m_macrosListWidget, row, 1, 2, 1);
    row++;
    gridLayout->addWidget(createMacroButtonsWidget(), row, 0);
    row++;
    
    for (int32_t iRow = 0; iRow < gridLayout->rowCount(); iRow++) {
        gridLayout->setRowStretch(iRow, 0);
    }
    
    m_dialogButtonBox = new QDialogButtonBox(QDialogButtonBox::Close);
    m_runButton = m_dialogButtonBox->addButton("Run", QDialogButtonBox::ApplyRole);
    QObject::connect(m_dialogButtonBox, &QDialogButtonBox::rejected,
                     this, &WuQMacroDialog::close);
    QObject::connect(m_dialogButtonBox, &QDialogButtonBox::clicked,
                     this, &WuQMacroDialog::buttonBoxButtonClicked);
    
    QVBoxLayout* dialogLayout = new QVBoxLayout(this);
    dialogLayout->addLayout(gridLayout);
    dialogLayout->addWidget(descriptionLabel);
    dialogLayout->addWidget(m_macroDescriptionLabel);
    dialogLayout->addWidget(horizontalLine);
    dialogLayout->addWidget(createRunOptionsWidget());
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
    
    m_editPushButton = new QPushButton("Edit...");
    m_editPushButton->setToolTip("Edit the steps (commands) in the selected macro");
    QObject::connect(m_editPushButton, &QPushButton::clicked,
                     this, &WuQMacroDialog::editButtonClicked);
    
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
    macroButtonsLayout->addWidget(m_editPushButton);
    macroButtonsLayout->addSpacing(15);
    macroButtonsLayout->addWidget(m_deletePushButton);
    macroButtonsLayout->addSpacing(15);
    macroButtonsLayout->addWidget(m_importPushButton);
    macroButtonsLayout->addWidget(m_exportPushButton);
    macroButtonsLayout->addStretch();
    
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
        selectedIndex = 0;
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
    
    macroGroupBoxActivated(selectedIndex);
}

/**
 * Called when macro group combo box selection is made
 */
void
WuQMacroDialog::macroGroupBoxActivated(int)
{
    const QString emptySuffix(" (Empty)");
    QListWidgetItem* selectedItem = m_macrosListWidget->currentItem();
    QString selectedUniqueIdentifier;
    if (selectedItem != NULL) {
        selectedUniqueIdentifier = selectedItem->data(Qt::UserRole).toString();
    }
    
    m_macrosListWidget->clear();

    int32_t selectedIndex = 0;
    WuQMacroGroup* selectedGroup = getSelectedMacroGroup();
    if (selectedGroup != NULL) {
        const int32_t numMacros = selectedGroup->getNumberOfMacros();
        for (int32_t i = 0; i < numMacros; i++) {
            const WuQMacro* macro = selectedGroup->getMacroAtIndex(i);
            if (macro->getUniqueIdentifier() == selectedUniqueIdentifier) {
                selectedIndex = i;
            }
            
            QString macroName = macro->getName();
            if (selectedGroup->getMacroAtIndex(i)->getNumberOfMacroCommands() <= 0) {
                macroName.append(emptySuffix);
            }
            
            QListWidgetItem* item = new QListWidgetItem(macroName);
            item->setData(Qt::UserRole,
                          macro->getUniqueIdentifier());
            m_macrosListWidget->addItem(item);
        }
        
        if ((selectedIndex >= 0)
            && (selectedIndex < numMacros)) {
            m_macrosListWidget->setCurrentRow(selectedIndex);
        }
    }

    macrosListWidgetCurrentRowChanged(selectedIndex);
}

/**
 * Called when item is selected in the macros list widget
 */
void
WuQMacroDialog::macrosListWidgetCurrentRowChanged(int rowIndex)
{
    m_macroDescriptionLabel->clear();
    
    WuQMacroGroup* selectedGroup = getSelectedMacroGroup();
    if ((rowIndex >= 0)
        && (rowIndex < selectedGroup->getNumberOfMacros())) {
        WuQMacro* macro = selectedGroup->getMacroAtIndex(rowIndex);
        CaretAssert(macro);
        m_macroDescriptionLabel->setText(macro->getDescription());
    }
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
 * @return Pointer to selected macro
 */
WuQMacro*
WuQMacroDialog::getSelectedMacro()
{
    WuQMacroGroup* selectedGroup = getSelectedMacroGroup();
    if (selectedGroup != NULL) {
        const int32_t selectedMacroIndex = m_macrosListWidget->currentRow();
        if ((selectedMacroIndex >= 0)
            && (selectedMacroIndex < selectedGroup->getNumberOfMacros())) {
            WuQMacro* macro = selectedGroup->getMacroAtIndex(selectedMacroIndex);
            return macro;
        }
    }
    return NULL;
}

/**
 * Run the selected macro
 */
void
WuQMacroDialog::runSelectedMacro()
{
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
 * Called when the edit button is clicked
 */
void
WuQMacroDialog::editButtonClicked()
{
    WuQMacro* macro = getSelectedMacro();
    if (macro != NULL) {
        if (WuQMacroManager::instance()->editMacroCommands(m_editPushButton, macro)) {
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


