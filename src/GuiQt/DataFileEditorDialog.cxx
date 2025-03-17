
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
#include <QAction>
#include <QDialogButtonBox>
#include <QGridLayout>
#include <QHeaderView>
#include <QLabel>
#include <QToolButton>
#include <QTreeView>
#include <QVBoxLayout>

#include "Annotation.h"
#include "AnnotationFile.h"
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
#include "SamplesFile.h"
#include "WuQMessageBoxTwo.h"

using namespace caret;


    
/**
 * \class caret::DataFileEditorDialog 
 * \brief Dialog for editing data file items
 * \ingroup GuiQt
 */

/**
 * Constructor.
 * @param dataType
 *    Type of data for editing
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
        case DataType::ANNOTATIONS:
            m_dataFileType = DataFileTypeEnum::ANNOTATION;
            setWindowTitle("Edit Annotations");
            break;
        case DataType::BORDERS:
            m_dataFileType = DataFileTypeEnum::BORDER;
            setWindowTitle("Edit Borders");
            break;
        case DataType::FOCI:
            m_dataFileType = DataFileTypeEnum::FOCI;
            setWindowTitle("Edit Foci");
            break;
        case DataType::SAMPLES:
            m_dataFileType = DataFileTypeEnum::SAMPLES;
            setWindowTitle("Edit Samples");
            break;
    }
    CaretAssert(m_dataFileType != DataFileTypeEnum::UNKNOWN);

    setModal(true);
    
    createDialog();
    
    CaretAssert(m_leftEditor);
    CaretAssert(m_rightEditor);
    
    fileSelected(EditorIndex::LEFT,
                 m_leftEditor->m_fileSelectionModel->getSelectedFile());
    
    /*
     * View file will default to first file.
     * Try to load second file in copy/move to file.
     */
    CaretDataFileSelectionModel* rightFileModel(m_rightEditor->m_fileSelectionModel.get());
    std::vector<CaretDataFile*> copyMoveToFiles(rightFileModel->getAvailableFiles());
    if ( ! copyMoveToFiles.empty()) {
        const int32_t fileIndex((copyMoveToFiles.size() >= 2)
                                ? 1
                                : 0);
        CaretAssertVectorIndex(copyMoveToFiles, fileIndex);
        rightFileModel->setSelectedFile(copyMoveToFiles[fileIndex]);
        m_rightEditor->m_fileSelectionComboBox->updateComboBox(rightFileModel);
        fileSelected(EditorIndex::RIGHT,
                     rightFileModel->getSelectedFile());
    }
    
    updateCopyMoveDeleteActions();
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
    std::pair<QWidget*,EditorWidgets*> leftSide = createEditor(EditorIndex::LEFT);
    m_leftEditor.reset(leftSide.second);
    std::pair<QWidget*,EditorWidgets*> rightSide(createEditor(EditorIndex::RIGHT));
    m_rightEditor.reset(rightSide.second);
    
    
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
    QGridLayout* layout(new QGridLayout(this));
    layout->setRowStretch(0, 100);
    layout->setColumnStretch(0, 50);
    layout->setColumnStretch(1, 50);
    layout->addWidget(leftSide.first, 0, 0);
    layout->addWidget(rightSide.first, 0, 1);
    layout->addWidget(m_buttonBox, 1, 0, 1, 2);
}

/**
 * Create an editor for data file items
 * @param editorIndex
 *    Left/right side index
 * @return
 *    Widget containing GUI components and widgets used for editing
 */
std::pair<QWidget*,DataFileEditorDialog::EditorWidgets*>
DataFileEditorDialog::createEditor(const EditorIndex editorIndex)
{
    /*
     * File selection
     */
    QLabel* fileLabel(new QLabel("File: "));
    CaretDataFileSelectionModel* fileSelectionModel(CaretDataFileSelectionModel::newInstanceForCaretDataFileType(m_dataFileType));
    
    CaretDataFileSelectionComboBox* fileSelectionComboBox = new CaretDataFileSelectionComboBox(this);
    fileSelectionComboBox->updateComboBox(fileSelectionModel);
    QObject::connect(fileSelectionComboBox, &CaretDataFileSelectionComboBox::fileSelected,
                     [=](CaretDataFile* caretDataFile) { fileSelected(editorIndex,
                                                                      caretDataFile); });
    fileSelectionComboBox->getWidget()->setToolTip("Select file displayed below");
    
    AString copyText;
    AString moveText;
    AString copyToolTip;
    AString deleteToolTip;
    AString moveToolTip;
    QBoxLayout::Direction buttonsLayoutDirection(QBoxLayout::LeftToRight);
    switch (editorIndex) {
        case LEFT:
            copyText = "Copy ->";
            moveText = "Move ->";
            copyToolTip = "Copy selected items to file on right";
            deleteToolTip = "Delete selected items";
            moveToolTip = "Move selected items to file on right";
            buttonsLayoutDirection = QBoxLayout::RightToLeft;
            break;
        case RIGHT:
            copyText = "<- Copy";
            moveText = "<- Move";
            copyToolTip = "Copy selected items to file on left";
            deleteToolTip = "Delete selected items";
            moveToolTip = "Move selected items to file on left";
            buttonsLayoutDirection = QBoxLayout::LeftToRight;
            break;
    }
    /*
     * Action buttons
     */
    QAction* copyAction = new QAction(copyText);
    QObject::connect(copyAction, &QAction::triggered,
                     [=]() { copyActionSelected(editorIndex); });
    copyAction->setToolTip(copyToolTip);
    QToolButton* copyButton(new QToolButton());
    copyButton->setDefaultAction(copyAction);
    
    QAction* moveAction = new QAction(moveText);
    QObject::connect(moveAction, &QAction::triggered,
                     [=]() { moveActionSelected(editorIndex); });
    moveAction->setToolTip(moveToolTip);
    QToolButton* moveButton(new QToolButton());
    moveButton->setDefaultAction(moveAction);
    
    QAction* deleteAction = new QAction("Delete...");
    QObject::connect(deleteAction, &QAction::triggered,
                     [=]() { deleteActionSelected(editorIndex); });
    deleteAction->setToolTip(deleteToolTip);
    QToolButton* deleteButton(new QToolButton());
    deleteButton->setDefaultAction(deleteAction);
        
    /*
     * Tree view displaying model
     */
    QTreeView* treeView = new QTreeView();
    treeView->setSortingEnabled(true);
    treeView->setSelectionMode(QTreeView::ExtendedSelection);
    QHeaderView* headerView(treeView->header());
    headerView->setSectionResizeMode(QHeaderView::ResizeToContents);
        
    EditorWidgets* editorWidget(new EditorWidgets(fileSelectionModel,
                                                  fileSelectionComboBox,
                                                  treeView,
                                                  copyAction,
                                                  moveAction,
                                                  deleteAction));

    /*
     * Widget's layouts
     */
    QMargins zeroMargin(0, 0, 0, 0);
    QHBoxLayout* fileSelectionLayout(new QHBoxLayout());
    fileSelectionLayout->setContentsMargins(zeroMargin);
    fileSelectionLayout->addWidget(fileLabel);
    fileSelectionLayout->addWidget(fileSelectionComboBox->getWidget(), 100);
    
    QHBoxLayout* buttonsLayout(new QHBoxLayout());
    buttonsLayout->setDirection(buttonsLayoutDirection);
    buttonsLayout->setContentsMargins(zeroMargin);
    buttonsLayout->addStretch();
    buttonsLayout->addWidget(copyButton);
    buttonsLayout->addWidget(moveButton);
    buttonsLayout->addWidget(deleteButton);
    buttonsLayout->addStretch();

    QWidget* widget(new QWidget());
    QVBoxLayout* layout(new QVBoxLayout(widget));
    layout->addLayout(fileSelectionLayout);
    layout->addLayout(buttonsLayout);
    layout->addWidget(treeView, 100);
    
    return std::make_pair(widget, editorWidget);
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
 * Update model in the other view if it contains the same file
 * @param editorIndex
 *    Side that is NOT updated
 * @param reloadIfThisFile
 *    Reload if this file is viewed
 */
void
DataFileEditorDialog::updateOtherModelView(const EditorIndex editorIndex,
                                           const CaretDataFile* caretDataFile)
{
    EditorIndex otherIndex = EditorIndex::LEFT;
    switch (editorIndex) {
        case LEFT:
            otherIndex = EditorIndex::RIGHT;
            break;
        case RIGHT:
            otherIndex = EditorIndex::LEFT;
            break;
    }
    
    if (getEditor(otherIndex)->getSelectedFile() == caretDataFile) {
        fileSelected(otherIndex,
                     getEditor(otherIndex)->getSelectedFile());
    }
}


/**
 * Called when file is selected
 * @param editorIndex
 *    Left/right side index
 * @param caretDataFile
 *    File that is selected
 */
void
DataFileEditorDialog::fileSelected(const EditorIndex editorIndex,
                                   CaretDataFile* caretDataFile)
{
    DataFileEditorModel* model(NULL);
    
    if (caretDataFile != NULL) {
        switch (m_dataType) {
            case DataType::ANNOTATIONS:
            {
                AnnotationFile* annotationFile = dynamic_cast<AnnotationFile*>(caretDataFile);
                if (annotationFile != NULL) {
                    DataFileEditorColumnContent modelContent;
                    modelContent.addColumn(DataFileEditorItemTypeEnum::NAME, "Name");
                    modelContent.addColumn(DataFileEditorItemTypeEnum::GROUP_NAME, "Group");
                    modelContent.addColumn(DataFileEditorItemTypeEnum::COORDINATES, "Coordinate");
                    modelContent.setDefaultSortingColumnDataType(DataFileEditorItemTypeEnum::GROUP_NAME);
                    FunctionResultValue<DataFileEditorModel*> result = annotationFile->exportToDataFileEditorModel(modelContent);
                    if (result.isOk()) {
                        model = result.getValue();
                    }
                    else {
                        WuQMessageBoxTwo::criticalOk(this,
                                                     "Error",
                                                     result.getErrorMessage());
                    }
                }
            }
                break;
            case DataType::BORDERS:
            {
                BorderFile* borderFile = dynamic_cast<BorderFile*>(caretDataFile);
                if (borderFile != NULL) {
                    DataFileEditorColumnContent modelContent;
                    modelContent.addColumn(DataFileEditorItemTypeEnum::NAME, "Name");
                    modelContent.addColumn(DataFileEditorItemTypeEnum::CLASS_NAME, "Class");
                    modelContent.addColumn(DataFileEditorItemTypeEnum::COORDINATES, "XYZ");
                    modelContent.setDefaultSortingColumnDataType(DataFileEditorItemTypeEnum::CLASS_NAME);
                    FunctionResultValue<DataFileEditorModel*> result = borderFile->exportToDataFileEditorModel(modelContent);
                    if (result.isOk()) {
                        model = result.getValue();
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
                    DataFileEditorColumnContent modelContent;
                    modelContent.addColumn(DataFileEditorItemTypeEnum::NAME, "Name");
                    modelContent.addColumn(DataFileEditorItemTypeEnum::CLASS_NAME, "Class");
                    modelContent.addColumn(DataFileEditorItemTypeEnum::COORDINATES, "XYZ");
                    modelContent.setDefaultSortingColumnDataType(DataFileEditorItemTypeEnum::CLASS_NAME);
                    FunctionResultValue<DataFileEditorModel*> result = fociFile->exportToDataFileEditorModel(modelContent);
                    if (result.isOk()) {
                        model = result.getValue();
                    }
                    else {
                        WuQMessageBoxTwo::criticalOk(this,
                                                     "Error",
                                                     result.getErrorMessage());
                    }
                }
            }
                break;
            case DataType::SAMPLES:
            {
                SamplesFile* samplesFile = dynamic_cast<SamplesFile*>(caretDataFile);
                if (samplesFile != NULL) {
                    DataFileEditorColumnContent modelContent;
                    modelContent.addColumn(DataFileEditorItemTypeEnum::NAME, "Name");
                    modelContent.addColumn(DataFileEditorItemTypeEnum::GROUP_NAME, "Group");
                    modelContent.addColumn(DataFileEditorItemTypeEnum::COORDINATES, "Coordinate");
                    modelContent.setDefaultSortingColumnDataType(DataFileEditorItemTypeEnum::GROUP_NAME);
                    FunctionResultValue<DataFileEditorModel*> result = samplesFile->exportToDataFileEditorModel(modelContent);
                    if (result.isOk()) {
                        model = result.getValue();
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
    
    QTreeView* treeView(NULL);
    switch (editorIndex) {
        case LEFT:
            m_leftEditor->setModel(model);
            treeView = m_leftEditor->m_treeView;
            break;
        case RIGHT:
            m_rightEditor->setModel(model);
            treeView = m_rightEditor->m_treeView;
            break;
    }

    if (model != NULL) {
        treeView->setModel(model);
        treeView->sortByColumn(model->getDefaultSortingColumnIndex(),
                               Qt::AscendingOrder);

        /*
         * Selection model is valid (not NULL) only when there is
         * a model in the tree view
         */
        QItemSelectionModel* selectionModel(treeView->selectionModel());
        CaretAssert(selectionModel);
        QObject::connect(selectionModel, &QItemSelectionModel::selectionChanged,
                         [=](const QItemSelection&, const QItemSelection&)
                         { updateCopyMoveDeleteActions(); });
    }
    else {
        treeView->reset();
    }
    
    updateCopyMoveDeleteActions();
}

/**
 * Called when
 * @param editorIndex
 *    Left/right side index
 */
void
DataFileEditorDialog::copyActionSelected(const EditorIndex editorIndex)
{
    EditorIndex destinationIndex = EditorIndex::LEFT;
    switch (editorIndex) {
        case LEFT:
            destinationIndex = EditorIndex::RIGHT;
            break;
        case RIGHT:
            destinationIndex = EditorIndex::LEFT;
            break;
    }
    CaretAssert(editorIndex != destinationIndex);
    
    switch (m_dataType) {
        case DataType::ANNOTATIONS:
            copyAnnotations(editorIndex, destinationIndex);
            break;
        case DataType::BORDERS:
            copyBorders(editorIndex, destinationIndex);
            break;
        case DataType::FOCI:
            copyFoci(editorIndex, destinationIndex);
            break;
        case DataType::SAMPLES:
            copySamples(editorIndex, destinationIndex);
            break;
    }
}

/**
 * Called when move button clicked
 * @param editorIndex
 *    Left/right side index
 */
void
DataFileEditorDialog::moveActionSelected(const EditorIndex editorIndex)
{
    EditorIndex destinationIndex = EditorIndex::LEFT;
    switch (editorIndex) {
        case LEFT:
            destinationIndex = EditorIndex::RIGHT;
            break;
        case RIGHT:
            destinationIndex = EditorIndex::LEFT;
            break;
    }
    CaretAssert(editorIndex != destinationIndex);
    
    switch (m_dataType) {
        case DataType::ANNOTATIONS:
            if (copyAnnotations(editorIndex, destinationIndex)) {
                deleteActionSelected(editorIndex);
            }
            break;
        case DataType::BORDERS:
            if (copyBorders(editorIndex, destinationIndex)) {
                deleteActionSelected(editorIndex);
            }
            break;
        case DataType::FOCI:
            if (copyFoci(editorIndex, destinationIndex)) {
                deleteActionSelected(editorIndex);
            }
            break;
        case DataType::SAMPLES:
            if (copySamples(editorIndex, destinationIndex)) {
                deleteActionSelected(editorIndex);
            }
            break;
    }
}

/**
 * Called when delete button clicked
 * @param editorIndex
 *    Left/right side index
 */
void
DataFileEditorDialog::deleteActionSelected(const EditorIndex editorIndex)
{
    std::vector<int32_t> rowIndices(getSelectedRowIndicesSorted(editorIndex));
    if (rowIndices.empty()) {
        WuQMessageBoxTwo::critical(this, "Error", "No items are selected");
        return;
    }
    
    CaretDataFileSelectionModel* fileSelectionModel(NULL);
    DataFileEditorModel* model(NULL);
    switch (editorIndex) {
        case LEFT:
            fileSelectionModel = m_leftEditor->m_fileSelectionModel.get();
            model = m_leftEditor->m_model.get();
            break;
        case RIGHT:
            fileSelectionModel = m_rightEditor->m_fileSelectionModel.get();
            model = m_rightEditor->m_model.get();
            break;
    }
    CaretAssert(fileSelectionModel);
    CaretAssert(model);
    
    /*
     * Reverse indices since need to remove higher
     * numbered rows first
     */
    std::reverse(rowIndices.begin(),
                 rowIndices.end());
    for (const int32_t rowIndex : rowIndices) {
        model->removeRow(rowIndex);
    }
    
    switch (m_dataType) {
        case DataType::ANNOTATIONS:
        {
            AnnotationFile* selectedAnnotationFile(fileSelectionModel->getSelectedFileOfType<AnnotationFile>());
            if (selectedAnnotationFile != NULL) {
                selectedAnnotationFile->importFromDataFileEditorModel(*model);
                
                /*
                 * Reloads file with deleted borders
                 */
                fileSelected(editorIndex,
                             selectedAnnotationFile);
                /*
                 * Reloads other view since it may have the same file
                 */
                updateOtherModelView(editorIndex,
                                     selectedAnnotationFile);
            }
            else {
                WuQMessageBoxTwo::critical(this, "Error", "PROGRAM ERROR: Selected file is invalid");
            }
        }
            break;
        case DataType::BORDERS:
        {
            BorderFile* selectedBorderFile(fileSelectionModel->getSelectedFileOfType<BorderFile>());
            if (selectedBorderFile != NULL) {
                selectedBorderFile->importFromDataFileEditorModel(*model);
                
                /*
                 * Reloads file with deleted borders
                 */
                fileSelected(editorIndex,
                             selectedBorderFile);
                /*
                 * Reloads other view since it may have the same file
                 */
                updateOtherModelView(editorIndex,
                                     selectedBorderFile);
            }
            else {
                WuQMessageBoxTwo::critical(this, "Error", "PROGRAM ERROR: Selected file is invalid");
            }
        }
            break;
        case DataType::FOCI:
        {
            FociFile* selectedFociFile(fileSelectionModel->getSelectedFileOfType<FociFile>());
            if (selectedFociFile != NULL) {
                selectedFociFile->importFromDataFileEditorModel(*model);
                
                /*
                 * Reloads file with deleted foci
                 */
                fileSelected(editorIndex,
                             selectedFociFile);
                /*
                 * Reloads other view since it may have the same file
                 */
                updateOtherModelView(editorIndex,
                                     selectedFociFile);
            }
            else {
                WuQMessageBoxTwo::critical(this, "Error", "PROGRAM ERROR: Selected file is invalid");
            }
        }
            break;
        case DataType::SAMPLES:
        {
            SamplesFile* selectedSamplesFile(fileSelectionModel->getSelectedFileOfType<SamplesFile>());
            if (selectedSamplesFile != NULL) {
                selectedSamplesFile->importFromDataFileEditorModel(*model);
                
                /*
                 * Reloads file with deleted borders
                 */
                fileSelected(editorIndex,
                             selectedSamplesFile);
                /*
                 * Reloads other view since it may have the same file
                 */
                updateOtherModelView(editorIndex,
                                     selectedSamplesFile);
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
 * Update the copy/move to actions
 */
void
DataFileEditorDialog::updateCopyMoveDeleteActions()
{
    const bool filesDifferentFlag(m_leftEditor->getSelectedFile()
                                  != m_rightEditor->getSelectedFile());
    
    const bool leftItemsSelectedFlag( ! getSelectedItems(EditorIndex::LEFT).empty());
    const bool rightItemsSelectedFlag( ! getSelectedItems(EditorIndex::RIGHT).empty());
    
    m_leftEditor->m_deleteAction->setEnabled(leftItemsSelectedFlag);
    m_leftEditor->m_copyAction->setEnabled(filesDifferentFlag
                                           && leftItemsSelectedFlag);
    m_leftEditor->m_moveAction->setEnabled(filesDifferentFlag
                                           && leftItemsSelectedFlag);

    m_rightEditor->m_deleteAction->setEnabled(rightItemsSelectedFlag);
    m_rightEditor->m_copyAction->setEnabled(filesDifferentFlag
                                            && rightItemsSelectedFlag);
    m_rightEditor->m_moveAction->setEnabled(filesDifferentFlag
                                            && rightItemsSelectedFlag);
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
 * @return Selected row indexes sorted for the given editor
 * @param editorIndex
 *    Left/right side index
 */
std::vector<int32_t>
DataFileEditorDialog::getSelectedRowIndicesSorted(const EditorIndex editorIndex) const
{
    QTreeView* treeView(NULL);
    switch (editorIndex) {
        case LEFT:
            treeView = m_leftEditor->m_treeView;
            break;
        case RIGHT:
            treeView = m_rightEditor->m_treeView;
            break;
    }
    CaretAssert(treeView);
    
    /*
     * Put in set to remove duplicates and sort
     */
    std::set<int32_t> rowIndicesSet;
    const QModelIndexList modelndexes(treeView->selectionModel()->selectedRows());
    for (const QModelIndex& mi : modelndexes) {
        rowIndicesSet.insert(mi.row());
    }
    
    std::vector<int32_t> rowIndicesOut(rowIndicesSet.begin(),
                                       rowIndicesSet.end());
    return rowIndicesOut;
}

/**
 * @return All items that are selected for the given editor
 * @param editorIndex
 *    Left/right side index
 */
std::vector<DataFileEditorItem*>
DataFileEditorDialog::getSelectedItems(const EditorIndex editorIndex) const
{
    std::vector<DataFileEditorItem*> items;
    
    EditorWidgets* editor(getEditor(editorIndex));
    CaretAssert(editor);
    QItemSelectionModel* selectionModel(editor->m_treeView->selectionModel());
    if (selectionModel != NULL) { /* selection model may be NULL */
        QModelIndexList modelndexes = selectionModel->selectedRows();
        items = editor->m_model->getItemsFromIndices(modelndexes);
    }
    return items;
}

/**
 * @return Editor for the left/right side
 * @param editorIndex
 *    Left/right side index
 */
DataFileEditorDialog::EditorWidgets*
DataFileEditorDialog::getEditor(const EditorIndex editorIndex) const
{
    EditorWidgets* editor(NULL);
    switch (editorIndex) {
        case LEFT:
            editor = m_leftEditor.get();
            break;
        case RIGHT:
            editor = m_rightEditor.get();
            break;
    }
    CaretAssert(editor);
    return editor;
}

/**
 * @return The selected annotations for the given editor
 * @param editorIndex
 *    Left/right side index
 */
std::vector<const Annotation*>
DataFileEditorDialog::getSelectedAnnotations(const EditorIndex editorIndex) const
{
    CaretAssert(m_dataType == DataType::ANNOTATIONS);
    
    std::vector<const Annotation*> annotationsOut;
    
    std::vector<DataFileEditorItem*> selectedItems(getSelectedItems(editorIndex));
    for (DataFileEditorItem* item : selectedItems) {
        const Annotation* annotation(item->getAnnotation());
        if (annotation != NULL) {
            annotationsOut.push_back(annotation);
        }
    }
    return annotationsOut;
}

/**
 * @return The selected borders for the given editor
 * @param editorIndex
 *    Left/right side index
 */
std::vector<const Border*>
DataFileEditorDialog::getSelectedBorders(const EditorIndex editorIndex) const
{
    CaretAssert(m_dataType == DataType::BORDERS);
    
    std::vector<const Border*> bordersOut;
    
    std::vector<DataFileEditorItem*> selectedItems(getSelectedItems(editorIndex));
    for (DataFileEditorItem* item : selectedItems) {
        const Border* border(item->getBorder());
        if (border != NULL) {
            bordersOut.push_back(border);
        }
    }
    return bordersOut;
}

/**
 * @return The selected foci for the given editor
 * @param editorIndex
 *    Left/right side index
 */
std::vector<const Focus*>
DataFileEditorDialog::getSelectedFoci(const EditorIndex editorIndex) const
{
    CaretAssert(m_dataType == DataType::FOCI);
    
    std::vector<const Focus*> fociOut;
    
    std::vector<DataFileEditorItem*> selectedItems(getSelectedItems(editorIndex));
    for (DataFileEditorItem* item : selectedItems) {
        const Focus* focus(item->getFocus());
        if (focus != NULL) {
            fociOut.push_back(focus);
        }
    }
    return fociOut;
}

/**
 * @return The selected samples for the given editor
 * @param editorIndex
 *    Left/right side index
 */
std::vector<const Annotation*>
DataFileEditorDialog::getSelectedSamples(const EditorIndex editorIndex) const
{
    CaretAssert(m_dataType == DataType::SAMPLES);
    
    std::vector<const Annotation*> annotationsOut;
    
    std::vector<DataFileEditorItem*> selectedItems(getSelectedItems(editorIndex));
    for (DataFileEditorItem* item : selectedItems) {
        const Annotation* annotation(item->getSample());
        if (annotation != NULL) {
            annotationsOut.push_back(annotation);
        }
    }
    return annotationsOut;
}

/**
 * Copy annotations from source to desintation
 * @return True if successful, else false.
 */
bool
DataFileEditorDialog::copyAnnotations(const EditorIndex sourceEditorIndex,
                                      const EditorIndex destinationEditorIndex)
{
    std::vector<const Annotation*> annotations(getSelectedAnnotations(sourceEditorIndex));
    if (annotations.empty()) {
        WuQMessageBoxTwo::critical(this, "Error", "No annotations are selected");
        return false;
    }
    
    AnnotationFile* sourceAnnotationFile(getEditor(sourceEditorIndex)->m_fileSelectionModel->getSelectedFileOfType<AnnotationFile>());
    AnnotationFile* destinationAnnotationFile(getEditor(destinationEditorIndex)->m_fileSelectionModel->getSelectedFileOfType<AnnotationFile>());
    if (sourceAnnotationFile == destinationAnnotationFile) {
        WuQMessageBoxTwo::critical(this, "Error", "Annotation files are the same");
        return false;
    }
    
    for (const Annotation* annotation : annotations) {
        Annotation* annotationCopy(annotation->clone());
        destinationAnnotationFile->addAnnotationCopiedFromAnotherFile(annotationCopy);
    }
    updateGraphicsAndUserInterface();
    
    /*
     * Reloads file with copied borders
     */
    fileSelected(destinationEditorIndex,
                 destinationAnnotationFile);
    
    return true;
}

/**
 * Copy borders from source to desintation
 * @return True if successful, else false.
 */
bool
DataFileEditorDialog::copyBorders(const EditorIndex sourceEditorIndex,
                                  const EditorIndex destinationEditorIndex)
{
    std::vector<const Border*> borders(getSelectedBorders(sourceEditorIndex));
    if (borders.empty()) {
        WuQMessageBoxTwo::critical(this, "Error", "No borders are selected");
        return false;
    }

    BorderFile* sourceBorderFile(getEditor(sourceEditorIndex)->m_fileSelectionModel->getSelectedFileOfType<BorderFile>());
    BorderFile* destinationBorderFile(getEditor(destinationEditorIndex)->m_fileSelectionModel->getSelectedFileOfType<BorderFile>());
    if (sourceBorderFile == destinationBorderFile) {
        WuQMessageBoxTwo::critical(this, "Error", "Border files are the same");
        return false;
    }
    
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
    
    /*
     * Reloads file with copied borders
     */
    fileSelected(destinationEditorIndex,
                 destinationBorderFile);
    
    return true;
}

/**
 * Copy foci from source to desintation
 * @return True if successful, else false.
 */
bool
DataFileEditorDialog::copyFoci(const EditorIndex sourceEditorIndex,
                               const EditorIndex destinationEditorIndex)
{
    std::vector<const Focus*> foci(getSelectedFoci(sourceEditorIndex));
    if (foci.empty()) {
        WuQMessageBoxTwo::critical(this, "Error", "No foci are selected");
        return false;
    }
    
    FociFile* sourceFociFile(getEditor(sourceEditorIndex)->m_fileSelectionModel->getSelectedFileOfType<FociFile>());
    FociFile* destinationFociFile(getEditor(destinationEditorIndex)->m_fileSelectionModel->getSelectedFileOfType<FociFile>());
    if (sourceFociFile == destinationFociFile) {
        WuQMessageBoxTwo::critical(this, "Error", "Foci files are the same");
        return false;
    }
    
    for (const Focus* focus : foci) {
        Focus* focusCopy(new Focus(*focus));
        if (focus->isNameRgbaValid()) {
            float rgba[4];
            focus->getNameRgba(rgba);
            focusCopy->setNameRgba(rgba);
        }
        if (focus->isClassRgbaValid()) {
            float rgba[4];
            focus->getClassRgba(rgba);
            focusCopy->setClassRgba(rgba);
        }
        destinationFociFile->addFocusUseColorsFromFocus(focusCopy);
    }
    updateGraphicsAndUserInterface();
    
    /*
     * Reloads file with copied foci
     */
    fileSelected(destinationEditorIndex,
                 destinationFociFile);
    
    return true;
}

/**
 * Copy samples from source to desintation
 * @return True if successful, else false.
 */
bool
DataFileEditorDialog::copySamples(const EditorIndex sourceEditorIndex,
                                  const EditorIndex destinationEditorIndex)
{
    std::vector<const Annotation*> samples(getSelectedSamples(sourceEditorIndex));
    if (samples.empty()) {
        WuQMessageBoxTwo::critical(this, "Error", "No samples are selected");
        return false;
    }
    
    SamplesFile* sourceSamplesFile(getEditor(sourceEditorIndex)->m_fileSelectionModel->getSelectedFileOfType<SamplesFile>());
    SamplesFile* destinationSamplesFile(getEditor(destinationEditorIndex)->m_fileSelectionModel->getSelectedFileOfType<SamplesFile>());
    if (sourceSamplesFile == destinationSamplesFile) {
        WuQMessageBoxTwo::critical(this, "Error", "Sample files are the same");
        return false;
    }
    
    for (const Annotation* sample : samples) {
        Annotation* sampleCopy(sample->clone());
        destinationSamplesFile->addAnnotationCopiedFromAnotherFile(sampleCopy);
    }
    updateGraphicsAndUserInterface();
    
    /*
     * Reloads file with copied borders
     */
    fileSelected(destinationEditorIndex,
                 destinationSamplesFile);
    
    return true;
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

/**
 * Constructor
 * @param fileSelectionModel
 *    The file selection model
 * @param fileSelectionComboBox
 *    The file selection combo box
 * @param treeView
 *    The tree view
 * @param copyAction
 *    The copy action
 * @param moveAction
 *    The move action
 * @param deleteAction
 *    The delete action
 */
DataFileEditorDialog::EditorWidgets::EditorWidgets(CaretDataFileSelectionModel* fileSelectionModel,
                                                   CaretDataFileSelectionComboBox* fileSelectionComboBox,
                                                   QTreeView* treeView,
                                                   QAction* copyAction,
                                                   QAction* moveAction,
                                                   QAction* deleteAction)
{
    m_fileSelectionModel.reset(fileSelectionModel);
    m_fileSelectionComboBox = fileSelectionComboBox;
    m_treeView              = treeView;
    m_copyAction            = copyAction;
    m_moveAction            = moveAction;
    m_deleteAction          = deleteAction;
}

DataFileEditorDialog::EditorWidgets::~EditorWidgets()
{
    
}

/**
 * Set the model (takes ownership and will delete model)
 * @param model
 *    The model
 */
void
DataFileEditorDialog::EditorWidgets::setModel(DataFileEditorModel* model)
{
    m_model.reset(model);
}

/**
 * @return File selected in this editor widget
 */
CaretDataFile*
DataFileEditorDialog::EditorWidgets::getSelectedFile()
{
    return m_fileSelectionModel->getSelectedFile();
}

