
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

#include <QComboBox>
#include <QDialogButtonBox>
#include <QGridLayout>
#include <QLabel>
#include <QListWidget>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QVBoxLayout>

#include "CaretAssert.h"
#include "CaretLogger.h"
#include "WuQMacro.h"
#include "WuQMacroGroup.h"
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
    
    QLabel* nameLabel = new QLabel("Macros:");
    QLabel* descriptionLabel = new QLabel("Description:");
    QLabel* macroGroupLabel = new QLabel("Macros in:");
    
    m_macrosListWidget = new QListWidget();
    QObject::connect(m_macrosListWidget, &QListWidget::currentRowChanged,
                     this, &WuQMacroDialog::macrosListWidgetCurrentRowChanged);
    
    m_macroDescriptionTextEdit = new QPlainTextEdit();
    
    m_macroGroupComboBox = new QComboBox();
    QObject::connect(m_macroGroupComboBox, static_cast<void(QComboBox::*)(int)>(&QComboBox::activated),
                     this, &WuQMacroDialog::macroGroupBoxActivated);
    
    QGridLayout* gridLayout = new QGridLayout();
    int row = 0;
    gridLayout->addWidget(macroGroupLabel, row, 0);
    gridLayout->addWidget(m_macroGroupComboBox, row, 1);
    row++;
    gridLayout->addWidget(nameLabel, row, 0);
    gridLayout->addWidget(m_macrosListWidget, row, 1);
    row++;
    gridLayout->addWidget(descriptionLabel, row, 0);
    gridLayout->addWidget(m_macroDescriptionTextEdit, row, 1);
    row++;
    
    m_dialogButtonBox = new QDialogButtonBox(QDialogButtonBox::Close);
    m_editButton = m_dialogButtonBox->addButton("Edit...", QDialogButtonBox::ApplyRole);
    m_runButton = m_dialogButtonBox->addButton("Run...", QDialogButtonBox::ApplyRole);
    QObject::connect(m_dialogButtonBox, &QDialogButtonBox::rejected,
                     this, &WuQMacroDialog::close);
    QObject::connect(m_dialogButtonBox, &QDialogButtonBox::clicked,
                     this, &WuQMacroDialog::buttonClicked);
    
    QVBoxLayout* dialogLayout = new QVBoxLayout(this);
    dialogLayout->addLayout(gridLayout);
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
 * Called when a button in dialog is clicked
 *
 * @param button
 *     Button that was clicked.
 */
void
WuQMacroDialog::buttonClicked(QAbstractButton* button)
{
    if (button == m_editButton) {
        editSelectedMacro();
    }
    else if (button == m_runButton) {
        runSelectedMacro();
    }
}

/**
 * Update content of the dialog
 */
void
WuQMacroDialog::updateDialogContents()
{
    QString selectedGroupName = m_macroGroupComboBox->currentText();
    
    m_macroGroups = WuQMacroManager::instance()->getMacroGroups();
    
    int32_t selectedIndex = 0;
    int32_t groupCounter(0);
    m_macroGroupComboBox->clear();
    for (auto mg : m_macroGroups) {
        m_macroGroupComboBox->addItem(mg->getGroupName());
        if (selectedGroupName == mg->getGroupName()) {
            selectedIndex = groupCounter;
        }
        groupCounter++;
    }
    
    if ((selectedIndex >= 0)
        && (selectedIndex < m_macroGroupComboBox->count())) {
        m_macroGroupComboBox->setCurrentIndex(selectedIndex);
    }
    
    macroGroupBoxActivated(selectedIndex);
}

/**
 * Called when macro group combo box selection is made
 */
void
WuQMacroDialog::macroGroupBoxActivated(int)
{
    QListWidgetItem* selectedItem = m_macrosListWidget->currentItem();
    QString selectedName = ((selectedItem != NULL) ? selectedItem->text() : "");
    
    m_macrosListWidget->clear();

    int32_t selectedIndex = 0;
    WuQMacroGroup* selectedGroup = getSelectedMacroGroup();
    const int32_t numMacros = selectedGroup->getNumberOfMacros();
    for (int32_t i = 0; i < numMacros; i++) {
        const QString macroName = selectedGroup->getMacroAtIndex(i)->getName();
        m_macrosListWidget->addItem(macroName);
        if (selectedName == macroName) {
            selectedIndex = i;
        }
    }
    
    if ((selectedIndex >= 0)
        && (selectedIndex < numMacros)) {
        m_macrosListWidget->setCurrentRow(selectedIndex);
    }
    macrosListWidgetCurrentRowChanged(selectedIndex);
}

/**
 * Called when item is selected in the macros list widget
 */
void
WuQMacroDialog::macrosListWidgetCurrentRowChanged(int rowIndex)
{
    m_macroDescriptionTextEdit->clear();
    
    WuQMacroGroup* selectedGroup = getSelectedMacroGroup();
    if ((rowIndex >= 0)
        && (rowIndex < selectedGroup->getNumberOfMacros())) {
        WuQMacro* macro = selectedGroup->getMacroAtIndex(rowIndex);
        CaretAssert(macro);
        m_macroDescriptionTextEdit->setPlainText(macro->getDescription());
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
 * Edit the selected macro
 */
void
WuQMacroDialog::editSelectedMacro()
{
    WuQMacro* macro = getSelectedMacro();
    if (macro != NULL) {
        std::cout << "Edit macro: " << macro->getName() << std::endl;
    }
}

/**
 * Run the selected macro
 */
void
WuQMacroDialog::runSelectedMacro()
{
    WuQMacro* macro = getSelectedMacro();
    if (macro != NULL) {
        std::cout << "Run macro: " << macro->getName() << std::endl;
        std::cout << macro->toString() << std::endl;
        
        WuQMacroManager::instance()->runMacro(parentWidget(),
                                              macro);
    }
}


