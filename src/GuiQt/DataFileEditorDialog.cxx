
/*LICENSE_START*/
/*
 *  Copyright (C) 2025 Washington University School of Medicine
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

#define __DATA_FILE_EDITOR_DIALOG_DECLARE__
#include "DataFileEditorDialog.h"
#undef __DATA_FILE_EDITOR_DIALOG_DECLARE__

#include <algorithm>
#include <cstdint>
#include <set>

#include <QAbstractButton>
#include <QDialogButtonBox>
#include <QGridLayout>
#include <QHeaderView>
#include <QLabel>
#include <QToolButton>
#include <QTreeView>
#include <QVBoxLayout>

#include "Border.h"
#include "BorderFile.h"
#include "CaretAssert.h"
#include "CaretDataFileSelectionComboBox.h"
#include "CaretDataFileSelectionModel.h"
#include "DataFileEditorItem.h"
#include "DataFileEditorModel.h"
#include "EventGraphicsPaintSoonAllWindows.h"
#include "EventManager.h"
#include "EventUserInterfaceUpdate.h"
#include "FociFile.h"
#include "Focus.h"
#include "WuQMessageBoxTwo.h"

using namespace caret;


    
/**
 * \class caret::DataFileEditorDialog 
 * \brief Dialog for editing data file items
 * \ingroup GuiQt
 */

/**
 * Constructor.
 * @param parent
 *    Parent widget
 */
DataFileEditorDialog::DataFileEditorDialog(const DataType dataType,
                                           QWidget* parent)
: QDialog(parent),
m_dataType(dataType)
{
    m_dataFileType = DataFileTypeEnum::UNKNOWN;
    switch (m_dataType) {
        case DataType::BORDERS:
            m_dataFileType = DataFileTypeEnum::BORDER;
            setWindowTitle("Edit Borders");
            break;
        case DataType::FOCI:
            m_dataFileType = DataFileTypeEnum::FOCI;
            setWindowTitle("Edit Foci");
            break;
    }
    CaretAssert(m_dataFileType != DataFileTypeEnum::UNKNOWN);

    setModal(true);
    
    createDialog();
    
    dataFileSelected(m_viewFileSelectionModel->getSelectedFile());
    
    /*
     * View file will default to first file.
     * Try to load second file in copy/move to file.
     */
    std::vector<CaretDataFile*> copyMoveToFiles(m_copyMoveToFileSelectionModel->getAvailableFiles());
    if (copyMoveToFiles.size() >= 2) {
        CaretAssertVectorIndex(copyMoveToFiles, 1);
        m_copyMoveToFileSelectionModel->setSelectedFile(copyMoveToFiles[1]);
        m_copyMoveToFileSelectionComboBox->updateComboBox(m_copyMoveToFileSelectionModel.get());
    }
    updateCopyMoveToActions();
}

/**
 * Destructor.
 */
DataFileEditorDialog::~DataFileEditorDialog()
{
}

/**
 * Create the dialog
 */
void
DataFileEditorDialog::createDialog()
{
    /*
     * File selection
     */
    QLabel* viewFileLabel(new QLabel("View File: "));
    m_viewFileSelectionModel.reset(CaretDataFileSelectionModel::newInstanceForCaretDataFileType(m_dataFileType));
    
    m_viewFileSelectionComboBox = new CaretDataFileSelectionComboBox(this);
    m_viewFileSelectionComboBox->updateComboBox(m_viewFileSelectionModel.get());
    QObject::connect(m_viewFileSelectionComboBox, &CaretDataFileSelectionComboBox::fileSelected,
                     this, &DataFileEditorDialog::dataFileSelected);
    
    /*
     * Action buttons
     */
    m_copyAction = new QAction("Copy to");
    QObject::connect(m_copyAction, &QAction::triggered,
                     this, &DataFileEditorDialog::copyActionTriggered);
    QToolButton* copyButton(new QToolButton());
    copyButton->setDefaultAction(m_copyAction);
    
    m_moveAction = new QAction("Move to");
    QObject::connect(m_moveAction, &QAction::triggered,
                     this, &DataFileEditorDialog::moveActionTriggered);
    QToolButton* moveButton(new QToolButton());
    moveButton->setDefaultAction(m_moveAction);
    
    m_deleteAction = new QAction("Delete...");
    QObject::connect(m_deleteAction, &QAction::triggered,
                     this, &DataFileEditorDialog::deleteActionTriggered);
    QToolButton* deleteButton(new QToolButton());
    deleteButton->setDefaultAction(m_deleteAction);

    /*
     * Copy/Move to File selection
     */
    m_copyMoveToFileSelectionModel.reset(CaretDataFileSelectionModel::newInstanceForCaretDataFileType(m_dataFileType));
    m_copyMoveToFileSelectionComboBox = new CaretDataFileSelectionComboBox(this);
    m_copyMoveToFileSelectionComboBox->updateComboBox(m_copyMoveToFileSelectionModel.get());
    QObject::connect(m_copyMoveToFileSelectionComboBox, &CaretDataFileSelectionComboBox::fileSelected,
                     this, &DataFileEditorDialog::copyMoveToFileSelected);

    /*
     * Tree view displaying model
     */
    m_treeView = new QTreeView();
    m_treeView->setSortingEnabled(true);
    m_treeView->setSelectionMode(QTreeView::ExtendedSelection);
    QHeaderView* headerView(m_treeView->header());
    headerView->setSectionResizeMode(QHeaderView::ResizeToContents);
    
    /*
     * Dialog buttons
     */
    QDialogButtonBox::StandardButtons buttons(QDialogButtonBox::StandardButton::Close);
    m_buttonBox = new QDialogButtonBox(buttons,
                                       Qt::Horizontal);
    QObject::connect(m_buttonBox, &QDialogButtonBox::clicked,
                     this, &DataFileEditorDialog::dialogButtonClicked);
    
    /*
     * Dialog's layouts
     */
    QMargins zeroMargin(0, 0, 0, 0);
    QHBoxLayout* fileSelectionLayout(new QHBoxLayout());
    fileSelectionLayout->setContentsMargins(zeroMargin);
    fileSelectionLayout->addWidget(viewFileLabel);
    fileSelectionLayout->addWidget(m_viewFileSelectionComboBox->getWidget(), 100);
    
    QHBoxLayout* copyMoveLayout(new QHBoxLayout());
    copyMoveLayout->setContentsMargins(zeroMargin);
    copyMoveLayout->addWidget(copyButton);
    copyMoveLayout->addWidget(moveButton);
    copyMoveLayout->addWidget(m_copyMoveToFileSelectionComboBox->getWidget());
        
    QVBoxLayout* layout(new QVBoxLayout(this));
    layout->addLayout(fileSelectionLayout);
    layout->addLayout(copyMoveLayout);
    layout->addWidget(deleteButton);
    layout->addWidget(m_treeView, 100);
    layout->addWidget(m_buttonBox);
}

/**
 * Called when a dialog button is clicked
 * @param buttonClicked
 *    Button that was clicked
 */
void
DataFileEditorDialog::dialogButtonClicked(QAbstractButton* buttonClicked)
{
    const QDialogButtonBox::StandardButton button(m_buttonBox->standardButton(buttonClicked));
    if (button == QDialogButtonBox::StandardButton::Ok) {
        accept();
    }
    else if (button == QDialogButtonBox::StandardButton::Cancel) {
        reject();
    }
    else if (button == QDialogButtonBox::StandardButton::Close) {
        reject();
    }
    else if (buttonClicked != NULL) {
        CaretAssertMessage(0, ("Button not tested: "
                               + buttonClicked->text()));
    }
    else {
        CaretAssertMessage(0, "Button is NULL");
    }
}

/**
 * Called when view file is selected
 * @param caretDataFile
 *    File that is selected
 */
void
DataFileEditorDialog::dataFileSelected(CaretDataFile* caretDataFile)
{
    m_model    = NULL;
    
    if (caretDataFile != NULL) {
        switch (m_dataType) {
            case DataType::BORDERS:
            {
                BorderFile* borderFile = dynamic_cast<BorderFile*>(caretDataFile);
                if (borderFile != NULL) {
                    FunctionResultValue<DataFileEditorModel*> result = borderFile->exportToDataFileEditorModel();
                    if (result.isOk()) {
                        m_model.reset(result.getValue());
                    }
                    else {
                        WuQMessageBoxTwo::criticalOk(this,
                                                     "Error",
                                                     result.getErrorMessage());
                    }
                }
                
            }
                break;
            case DataType::FOCI:
            {
                FociFile* fociFile = dynamic_cast<FociFile*>(caretDataFile);
                if (fociFile != NULL) {
                    FunctionResultValue<DataFileEditorModel*> result = fociFile->exportToDataFileEditorModel();
                    if (result.isOk()) {
                        m_model.reset(result.getValue());
                    }
                    else {
                        WuQMessageBoxTwo::criticalOk(this,
                                                     "Error",
                                                     result.getErrorMessage());
                    }
                }
                
            }
                break;
        }
    }
    
    if (m_model) {
        m_treeView->setModel(m_model.get());
        m_treeView->sortByColumn(0, Qt::AscendingOrder);
    }
    else {
        m_treeView->reset();
    }
    
    updateCopyMoveToActions();
}

/**
 * Called when copy/move to file is selected
 * @param caretDataFile
 *    File that is selected
 */
void
DataFileEditorDialog::copyMoveToFileSelected(CaretDataFile* /*caretDataFile*/)
{
    updateCopyMoveToActions();
}

/**
 * Update the copy/move to actions
 */
void
DataFileEditorDialog::updateCopyMoveToActions()
{
    bool enabledFlag(m_viewFileSelectionModel->getSelectedFile()
                     != m_copyMoveToFileSelectionModel->getSelectedFile());
    m_copyAction->setEnabled(enabledFlag);
    m_moveAction->setEnabled(enabledFlag);
}

/**
 * Closes dialog and sets result
 */
void
DataFileEditorDialog::done(int result)
{
    if (result == DataFileEditorDialog::Accepted) {
        /* Save data */
    }
    QDialog::done(result);
}

/**
 * @return Indicies of selected rows
 */
std::vector<int32_t>
DataFileEditorDialog::getSelectedRowIndicesSorted() const
{
    /*
     * Put in set to remove duplicates and sort
     */
    std::set<int32_t> rowIndicesSet;
    const QModelIndexList modelndexes(m_treeView->selectionModel()->selectedRows());
    for (const QModelIndex& mi : modelndexes) {
        rowIndicesSet.insert(mi.row());
    }
    
    std::vector<int32_t> rowIndicesOut(rowIndicesSet.begin(),
                                       rowIndicesSet.end());
    return rowIndicesOut;
}

/**
 * @return All items that are selected
 */
std::vector<DataFileEditorItem*>
DataFileEditorDialog::getSelectedItems() const
{
    QModelIndexList modelndexes(m_treeView->selectionModel()->selectedRows());
    return m_model->getItemsFromIndices(modelndexes);
}

/**
 * @return The selected borders
 */
std::vector<const Border*>
DataFileEditorDialog::getSelectedBorders() const
{
    CaretAssert(m_dataType == DataType::BORDERS);
    
    std::vector<const Border*> bordersOut;
    
    std::vector<DataFileEditorItem*> selectedItems(getSelectedItems());
    for (DataFileEditorItem* item : selectedItems) {
        const Border* border(item->getBorder());
        if (border != NULL) {
            bordersOut.push_back(border);
        }
    }
    return bordersOut;
}

/**
 * @return The selected foci
 */
std::vector<const Focus*>
DataFileEditorDialog::getSelectedFoci() const
{
    CaretAssert(m_dataType == DataType::FOCI);
    
    std::vector<const Focus*> fociOut;
    
    std::vector<DataFileEditorItem*> selectedItems(getSelectedItems());
    for (DataFileEditorItem* item : selectedItems) {
        const Focus* focus(item->getFocus());
        if (focus != NULL) {
            fociOut.push_back(focus);
        }
    }
    return fociOut;
}

/**
 * Called when copy action is triggered
 */
void
DataFileEditorDialog::copyActionTriggered()
{
    switch (m_dataType) {
        case DataType::BORDERS:
            copyBorders();
            break;
        case DataType::FOCI:
            copyFoci();
            break;
    }
}

/**
 * Copy borders.
 * @return True if successful, else false.
 */
bool
DataFileEditorDialog::copyBorders()
{
    std::vector<const Border*> borders(getSelectedBorders());
    if (borders.empty()) {
        WuQMessageBoxTwo::critical(this, "Error", "No borders are selected");
        return false;
    }
    
    BorderFile* sourceBorderFile(m_viewFileSelectionModel->getSelectedFileOfType<BorderFile>());
    BorderFile* destinationBorderFile(m_copyMoveToFileSelectionModel->getSelectedFileOfType<BorderFile>());
    if ((sourceBorderFile != NULL)
        && (destinationBorderFile != NULL)) {
        if (sourceBorderFile != destinationBorderFile) {
            for (const Border* border : borders) {
                Border* borderCopy(new Border(*border));
                if (border->isNameRgbaValid()) {
                    float rgba[4];
                    border->getNameRgba(rgba);
                    borderCopy->setNameRgba(rgba);
                }
                if (border->isClassRgbaValid()) {
                    float rgba[4];
                    border->getClassRgba(rgba);
                    borderCopy->setClassRgba(rgba);
                }
                destinationBorderFile->addBorderUseColorsFromBorder(borderCopy);
            }
            updateGraphicsAndUserInterface();
            return true;
        }
        else {
            WuQMessageBoxTwo::criticalOk(this, "Error", "Viewed and copy/move to files are the same");
        }
    }
    else {
        WuQMessageBoxTwo::criticalOk(this, "Error", "Selected files are invalid");
    }
    return false;
}

/**
 * Copy foci.
 * @return True if successful, else false.
 */
bool
DataFileEditorDialog::copyFoci()
{
    std::vector<const Focus*> foci(getSelectedFoci());
    if (foci.empty()) {
        WuQMessageBoxTwo::critical(this, "Error", "No foci are selected");
        return false;
    }
    
    FociFile* sourceFociFile(m_viewFileSelectionModel->getSelectedFileOfType<FociFile>());
    FociFile* destinationFociFile(m_copyMoveToFileSelectionModel->getSelectedFileOfType<FociFile>());
    if ((sourceFociFile != NULL)
        && (destinationFociFile != NULL)) {
        if (sourceFociFile != destinationFociFile) {
            for (const Focus* focus : foci) {
                destinationFociFile->addFocusUseColorsFromFocus(new Focus(*focus));
            }
            updateGraphicsAndUserInterface();
            return true;
        }
        else {
            WuQMessageBoxTwo::criticalOk(this, "Error", "Viewed and copy/move to files are the same");
        }
    }
    else {
        WuQMessageBoxTwo::criticalOk(this, "Error", "Selected files are invalid");
    }
    return false;
}

/**
 * Called when delete action is triggered
 */
void
DataFileEditorDialog::deleteActionTriggered()
{
    std::vector<int32_t> rowIndices(getSelectedRowIndicesSorted());
    if (rowIndices.empty()) {
        WuQMessageBoxTwo::critical(this, "Error", "No items are selected");
        return;
    }
    
    /*
     * Reverse indices since need to remove higher
     * numbered rows first
     */
    std::reverse(rowIndices.begin(),
                 rowIndices.end());
    for (const int32_t rowIndex : rowIndices) {
        m_model->removeRow(rowIndex);
    }
    
    switch (m_dataType) {
        case DataType::BORDERS:
        {
            BorderFile* selectedBorderFile(m_viewFileSelectionModel->getSelectedFileOfType<BorderFile>());
            if (selectedBorderFile != NULL) {
                selectedBorderFile->importFromDataFileEditorModel(*m_model);
            }
            else {
                WuQMessageBoxTwo::critical(this, "Error", "PROGRAM ERROR: Selected file is invalid");
            }
        }
            break;
        case DataType::FOCI:
        {
            FociFile* selectedFociFile(m_viewFileSelectionModel->getSelectedFileOfType<FociFile>());
            if (selectedFociFile != NULL) {
                selectedFociFile->importFromDataFileEditorModel(*m_model);
            }
            else {
                WuQMessageBoxTwo::critical(this, "Error", "PROGRAM ERROR: Selected file is invalid");
            }
        }
            break;
    }
    
    updateGraphicsAndUserInterface();
}

/**
 * Called when move action is triggered
 */
void
DataFileEditorDialog::moveActionTriggered()
{
    switch (m_dataType) {
        case DataType::BORDERS:
            /*
             * First copy to destination file
             */
            if (copyBorders()) {
                /*
                 * If copy successful, then delete from source file
                 */
                deleteActionTriggered();
            }
            break;
        case DataType::FOCI:
            /*
             * First copy to destination file
             */
            if (copyFoci()) {
                /*
                 * If copy successful, then delete from source file
                 */
                deleteActionTriggered();
            }
            break;
    }
}

/**
 * Update the graphics and the user interface
 */
void
DataFileEditorDialog::updateGraphicsAndUserInterface()
{
    EventManager::get()->sendEvent(EventGraphicsPaintSoonAllWindows().getPointer());
    EventManager::get()->sendEvent(EventUserInterfaceUpdate().getPointer());
}


