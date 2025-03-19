
/*LICENSE_START*/
/*
 *  Copyright (C) 2024 Washington University School of Medicine
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

#define __CHOOSE_BORDER_FOCUS_FROM_FILE_DIALOG_DECLARE__
#include "ChooseBorderFocusFromFileDialog.h"
#undef __CHOOSE_BORDER_FOCUS_FROM_FILE_DIALOG_DECLARE__

#include <QAction>
#include <QButtonGroup>
#include <QComboBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QDialogButtonBox>
#include <QHeaderView>
#include <QItemSelectionModel>
#include <QLabel>
#include <QLineEdit>
#include <QRadioButton>
#include <QToolButton>
#include <QTreeView>
#include <QStackedWidget>

#include "Border.h"
#include "BorderFile.h"
#include "Brain.h"
#include "CaretAssert.h"
#include "CaretDataFileSelectionComboBox.h"
#include "CaretDataFileSelectionModel.h"
#include "CaretFileDialog.h"
#include "DataFileEditorItem.h"
#include "DataFileEditorModel.h"
#include "DataFileException.h"
#include "FileInformation.h"
#include "FociFile.h"
#include "Focus.h"
#include "GuiManager.h"
#include "WuQMessageBox.h"
#include "WuQtUtilities.h"

using namespace caret;


    
/**
 * \class caret::ChooseBorderFocusFromFileDialog 
 * \brief Dialog for selecting a border or focus from a file
 * \ingroup GuiQt
 */

/**
 * Constructor.
 * @param fileMode
 *    The file mode - border or focus
 * @param parent
 *    The parent widget
 */
ChooseBorderFocusFromFileDialog::ChooseBorderFocusFromFileDialog(const FileMode fileMode,
                                                                 QWidget* parent)
: QDialog(parent),
m_fileMode(fileMode)
{
    m_dataFileType = DataFileTypeEnum::UNKNOWN;
    switch (m_fileMode) {
        case FileMode::BORDER:
            setWindowTitle("Choose Border");
            m_dataFileType = DataFileTypeEnum::BORDER;
            break;
        case FileMode::FOCUS:
            setWindowTitle("Choose Focus");
            m_dataFileType = DataFileTypeEnum::FOCI;
            break;
    }
    CaretAssert(m_dataFileType != DataFileTypeEnum::UNKNOWN);

    QLabel* fileLabel(new QLabel("File: "));
    
    auto modelAndComboBox = CaretDataFileSelectionComboBox::newInstanceForFileType(m_dataFileType,
                                                                                   this);
    m_fileSelectionModel    = modelAndComboBox.first;
    CaretAssert(m_fileSelectionModel);
    m_fileSelectionComboBox = modelAndComboBox.second;
    CaretAssert(m_fileSelectionComboBox);
    QObject::connect(m_fileSelectionComboBox, &CaretDataFileSelectionComboBox::fileSelected,
                     this, &ChooseBorderFocusFromFileDialog::dataFileSelected);
    m_fileSelectionComboBox->updateComboBox(m_fileSelectionModel);
    
    /*
     * Tree view displaying model
     */
    m_treeView = new QTreeView();
    m_treeView->setSortingEnabled(true);
    m_treeView->setSelectionMode(QTreeView::SingleSelection);
    QHeaderView* headerView(m_treeView->header());
    headerView->setSectionResizeMode(QHeaderView::ResizeToContents);

    QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                                       | QDialogButtonBox::Cancel);
    QObject::connect(buttonBox, SIGNAL(accepted()),
                     this, SLOT(accept()));
    QObject::connect(buttonBox, SIGNAL(rejected()),
                     this, SLOT(reject()));

    QWidget* widget(this);
    QGridLayout* layout(new QGridLayout(widget));
    layout->setColumnStretch(1, 100);
    int row(layout->rowCount());
    layout->addWidget(fileLabel, row, 0);
    layout->addWidget(m_fileSelectionComboBox->getWidget(), row, 1);
    row = layout->rowCount();
    layout->addWidget(m_treeView, row, 0, 1, 2);
    row = layout->rowCount();
    layout->addWidget(buttonBox, row, 0, 1, 2, Qt::AlignHCenter);
    
    std::vector<CaretDataFile*> allFiles(m_fileSelectionModel->getAvailableFiles());
    if ( ! allFiles.empty()) {
        CaretAssertVectorIndex(allFiles, 0);
        CaretDataFile* firstDataFile(allFiles[0]);
        m_fileSelectionModel->setSelectedFile(firstDataFile);
        dataFileSelected(firstDataFile);
    }
    
    /*
     * Needed for sizeHint() to work.
     */
    setSizePolicy(QSizePolicy::Preferred,
                  QSizePolicy::Preferred);
}

/**
 * Destructor.
 */
ChooseBorderFocusFromFileDialog::~ChooseBorderFocusFromFileDialog()
{
}

/**
 * @return A size hint for this dialog
 */
QSize
ChooseBorderFocusFromFileDialog::sizeHint() const
{
    /*
     * Make dialog a little larger than the header of the tree view
     */
    const QHeaderView* header(m_treeView->header());
    CaretAssert(header);
    
    int32_t dialogWidth(header->width() + 20);
    if (dialogWidth < 400) {
        dialogWidth = 400;
    }
    const QSize dialogSize(dialogWidth, 300);
    return dialogSize;
}


void
ChooseBorderFocusFromFileDialog::done(int result)
{
    if (result == ChooseBorderFocusFromFileDialog::Accepted) {
        if (getFileSelections().getName().isEmpty()) {
            WuQMessageBox::errorOk(this, "A row must be selected");
            return;
        }
    }
    
    QDialog::done(result);
}

ChooseBorderFocusFromFileDialog::FilesSelections
ChooseBorderFocusFromFileDialog::getFileSelections() const
{
    AString filename;
    AString name;
    AString className;
    AString identifier;
    
    const DataFileEditorItem* item(getSelectedItem());
    if (item != NULL) {
        switch (m_fileMode) {
            case FileMode::BORDER:
            {
                const Border* border(item->getBorder());
                CaretAssert(border);
                CaretDataFile* file(m_fileSelectionModel->getSelectedFile());
                if (file != NULL) {
                    filename = file->getFileName();
                }
                name       = border->getName();
                className  = border->getClassName();
                identifier = "";
            }
                break;
            case FileMode::FOCUS:
            {
                const Focus* focus(item->getFocus());
                CaretAssert(focus);
                CaretDataFile* file(m_fileSelectionModel->getSelectedFile());
                if (file != NULL) {
                    filename = file->getFileName();
                }
                name       = focus->getName();
                className  = focus->getClassName();
                identifier = focus->getFocusID();
            }
                break;
        }
    }
    return FilesSelections(filename,
                           name,
                           className,
                           identifier);
}

/**
 * @return The selected item or NULL if no item selected
 */
const DataFileEditorItem*
ChooseBorderFocusFromFileDialog::getSelectedItem() const
{
    std::vector<DataFileEditorItem*> items;
    
    QItemSelectionModel* selectionModel(m_treeView->selectionModel());
    if (selectionModel != NULL) { /* selection model may be NULL */
        QModelIndexList modelndexes = selectionModel->selectedRows();
        QAbstractItemModel* model(m_treeView->model());
        if (model != NULL) {
            DataFileEditorModel* editorModel(dynamic_cast<DataFileEditorModel*>(model));
            CaretAssert(editorModel);
            items = editorModel->getItemsFromIndices(modelndexes);
        }
    }
    
    if (items.size() == 1) {
        CaretAssertVectorIndex(items, 0);
        return items[0];
    }
    
    return NULL;
}

/**
 * Called with selected border or focus file
 * @param caretDataFile
 *    File that was selected
 */
void
ChooseBorderFocusFromFileDialog::dataFileSelected(CaretDataFile* caretDataFile)
{
    m_borderFile = NULL;
    m_fociFile   = NULL;
    
    if (caretDataFile != NULL) {
        switch (m_fileMode) {
            case FileMode::BORDER:
            {
                m_borderFile = dynamic_cast<BorderFile*>(caretDataFile);
                CaretAssert(m_borderFile);
                DataFileEditorColumnContent modelContent;
                modelContent.addColumn(DataFileEditorItemTypeEnum::NAME, "Name");
                modelContent.addColumn(DataFileEditorItemTypeEnum::CLASS_NAME, "Class");
                modelContent.addColumn(DataFileEditorItemTypeEnum::COORDINATES, "XYZ");
                modelContent.setDefaultSortingColumnDataType(DataFileEditorItemTypeEnum::CLASS_NAME);
                FunctionResultValue<DataFileEditorModel*> modelResult(m_borderFile->exportToDataFileEditorModel(modelContent));
                if (modelResult.isOk()) {
                    m_treeView->setModel(modelResult.getValue());
                    m_treeView->sortByColumn(modelResult.getValue()->getDefaultSortingColumnIndex(),
                                             Qt::AscendingOrder);
                }
            }
                break;
            case FileMode::FOCUS:
            {
                m_fociFile = dynamic_cast<FociFile*>(caretDataFile);
                CaretAssert(m_fociFile);
                DataFileEditorColumnContent modelContent;
                modelContent.addColumn(DataFileEditorItemTypeEnum::NAME, "Name");
                modelContent.addColumn(DataFileEditorItemTypeEnum::CLASS_NAME, "Class");
                modelContent.addColumn(DataFileEditorItemTypeEnum::IDENTIFIER, "ID");
                modelContent.setDefaultSortingColumnDataType(DataFileEditorItemTypeEnum::CLASS_NAME);
                FunctionResultValue<DataFileEditorModel*> modelResult(m_fociFile->exportToDataFileEditorModel(modelContent));
                if (modelResult.isOk()) {
                    m_treeView->setModel(modelResult.getValue());
                    m_treeView->sortByColumn(modelResult.getValue()->getDefaultSortingColumnIndex(),
                                             Qt::AscendingOrder);
                }
            }
                break;
        }
    }
}

/**
 * Set the selections to the given filename, class, and name
 * @param filename
 *    name of file
 * @param className
 *    name of class
 * @param name
 *    name of border or focus
 */
void
ChooseBorderFocusFromFileDialog::setSelections(const AString& filename,
                                               const AString& className,
                                               const AString& name)
{
    const CaretDataFile* caretDataFile(NULL);
    Brain* brain(GuiManager::get()->getBrain());
    CaretAssert(brain);
    switch (m_fileMode) {
        case FileMode::BORDER:
            caretDataFile = brain->getBorderFileMatchingToName(filename);
            if (caretDataFile == NULL) {
                if (brain->getNumberOfBorderFiles() > 0) {
                    caretDataFile = brain->getBorderFile(0);
                }
            }
            break;
        case FileMode::FOCUS:
            caretDataFile = brain->getFociFileMatchingToName(filename);
            if (caretDataFile == NULL) {
                if (brain->getNumberOfFociFiles() > 0) {
                    caretDataFile = brain->getFociFile(0);
                }
            }
            break;
    }
    
    if (caretDataFile != NULL) {
        m_fileSelectionModel->setSelectedFile(const_cast<CaretDataFile*>(caretDataFile));
        m_fileSelectionComboBox->updateComboBox(m_fileSelectionModel);
        dataFileSelected(m_fileSelectionModel->getSelectedFile());
    }
    
    QAbstractItemModel* model(m_treeView->model());
    if (model != NULL) {
        DataFileEditorModel* dataFileEditorModel(dynamic_cast<DataFileEditorModel*>(model));
        if (dataFileEditorModel != NULL) {
            QModelIndex modelIndex(dataFileEditorModel->findItemsWithText(name,
                                                                          className));
            if (modelIndex.isValid()) {
                QItemSelectionModel* sm(m_treeView->selectionModel());
                sm->select(modelIndex,
                           QItemSelectionModel::Select);
                m_treeView->setCurrentIndex(modelIndex);
                m_treeView->scrollTo(modelIndex);
            }
        }
    }
}
