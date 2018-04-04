
/*LICENSE_START*/
/*
 *  Copyright (C) 2014 Washington University School of Medicine
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

#define __BORDER_FILE_SPLIT_DIALOG_DECLARE__
#include "BorderFileSplitDialog.h"
#undef __BORDER_FILE_SPLIT_DIALOG_DECLARE__

#include <QAction>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QSignalMapper>

#include "BorderFile.h"
#include "Brain.h"
#include "BrainStructure.h"
#include "CaretAssert.h"
#include "CaretDataFileSelectionComboBox.h"
#include "CaretDataFileSelectionModel.h"
#include "CaretFileDialog.h"
#include "EventDataFileAdd.h"
#include "EventManager.h"
#include "EventUserInterfaceUpdate.h"
#include "FileInformation.h"
#include "GuiManager.h"
#include "WuQMessageBox.h"
#include "WuQtUtilities.h"

using namespace caret;


    
/**
 * \class caret::BorderFileSplitDialog 
 * \brief Dialog for splitting multi-structure border file.
 * \ingroup GuiQt
 *
 * Some older border files contained borders for multiple structures
 * which will not work with wb_command operations.  This dialog is 
 * used to split up a multi-structure border file into multiple border
 * files each containing a single structure.
 */

/**
 * Constructor.
 */
BorderFileSplitDialog::BorderFileSplitDialog(QWidget* parent)
: WuQDialogModal("Split Multi-Structure Border File",
                 parent)
{
    m_dialogIsBeingCreatedFlag = true;
    
    m_fileNameToolButtonSignalMapper = new QSignalMapper(this);
    QObject::connect(m_fileNameToolButtonSignalMapper, SIGNAL(mapped(int)),
                     this, SLOT(fileNameToolButtonClicked(int)));
    
    m_fileSelectionModel.grabNew(CaretDataFileSelectionModel::newInstanceForMultiStructureBorderFiles());
    
    QLabel* fileSelectionLabel = new QLabel("Multi Structure Border File: ");
    m_fileSelectionComboBox = new CaretDataFileSelectionComboBox(this);
    m_fileSelectionComboBox->updateComboBox(m_fileSelectionModel);
    QObject::connect(m_fileSelectionComboBox, SIGNAL(fileSelected(CaretDataFile*)),
                     this, SLOT(borderMultiStructureFileSelected(CaretDataFile*)));
    
    QHBoxLayout* multiLayout = new QHBoxLayout();
    multiLayout->addWidget(fileSelectionLabel);
    multiLayout->addWidget(m_fileSelectionComboBox->getWidget());
    multiLayout->addStretch();
    
    m_gridLayout = new QGridLayout();
    WuQtUtilities::setLayoutSpacingAndMargins(m_gridLayout, 3, 3);
    m_gridLayout->setColumnStretch(0,   0);
    m_gridLayout->setColumnStretch(1, 100);
    int rowIndex = 0;
    m_gridLayout->addWidget(new QLabel("Structures"),
                            rowIndex, 0,
                            Qt::AlignHCenter);
    m_gridLayout->addWidget(new QLabel("New Single-Structure Border Files"),
                            rowIndex, 1,
                            Qt::AlignHCenter);
    
    QWidget* widget = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(widget);
    layout->addLayout(multiLayout);
    layout->addWidget(WuQtUtilities::createHorizontalLineWidget());
    layout->addLayout(m_gridLayout);
    layout->addStretch();
    setCentralWidget(widget,
                     WuQDialog::SCROLL_AREA_AS_NEEDED);
    
    m_dialogIsBeingCreatedFlag = false;
    
    CaretDataFile* firstFile = m_fileSelectionModel->getSelectedFile();
    if (firstFile != NULL) {
        borderMultiStructureFileSelected(firstFile);
    }
}

/**
 * Destructor.
 */
BorderFileSplitDialog::~BorderFileSplitDialog()
{
}

/**
 * Called when a multi-structure border file is selected.
 */
void
BorderFileSplitDialog::borderMultiStructureFileSelected(CaretDataFile* caretDataFile)
{
    if (m_dialogIsBeingCreatedFlag) {
        return;
    }
    
    AString pathName;
    AString fileNameNoExt;
    AString fileExtension;
    std::vector<StructureEnum::Enum> structures;
    if (caretDataFile != NULL) {
        BorderFile* borderFile = dynamic_cast<BorderFile*>(caretDataFile);
        CaretAssert(borderFile);
        structures = borderFile->getAllBorderStructures();

        FileInformation fileInfo(caretDataFile->getFileName());
        fileInfo.getFileComponents(pathName,
                                   fileNameNoExt,
                                   fileExtension);
    }
    
    const int32_t numStructures = static_cast<int32_t>(structures.size());
    for (int32_t i = 0; i < numStructures; i++) {
        QPushButton* pushButton = NULL;
        QLineEdit*   lineEdit   = NULL;
        if (i < static_cast<int32_t>(m_structureRows.size())) {
            CaretAssertVectorIndex(m_structureRows, i);
            pushButton = m_structureRows[i].m_fileNamePushButton;
            lineEdit   = m_structureRows[i].m_fileNameLineEdit;
            
            if (m_structureRows[i].m_fileNamePushButton->isHidden()) {
                pushButton->setVisible(true);
                lineEdit->setVisible(true);
            }
        }
        else {
            const int MINIMUM_LINE_EDIT_WIDTH = 400;
            pushButton = new QPushButton();
            QObject::connect(pushButton, SIGNAL(clicked()),
                             m_fileNameToolButtonSignalMapper, SLOT(map()));
            m_fileNameToolButtonSignalMapper->setMapping(pushButton, i);
            
            lineEdit   = new QLineEdit();
            lineEdit->setMinimumWidth(MINIMUM_LINE_EDIT_WIDTH);
            
            const int rowIndex = m_gridLayout->rowCount();
            m_gridLayout->addWidget(pushButton,
                                    rowIndex, 0);
            m_gridLayout->addWidget(lineEdit,
                                    rowIndex, 1);
            StructureRow structureRow;
            structureRow.m_fileNamePushButton = pushButton;
            structureRow.m_fileNameLineEdit = lineEdit;
            m_structureRows.push_back(structureRow);
        }
        
        CaretAssertVectorIndex(m_structureRows, i);
        CaretAssert(pushButton);
        CaretAssert(lineEdit);
        CaretAssertVectorIndex(structures, i);
        m_structureRows[i].m_structure = structures[i];
        pushButton->setText(StructureEnum::toGuiName(structures[i]) + "...");
        
        AString singleStructureFileName;
        if ( ! pathName.isEmpty()) {
            singleStructureFileName += (pathName + "/");
        }
        singleStructureFileName += fileNameNoExt;
        singleStructureFileName += ("_" + StructureEnum::toGuiName(structures[i]));
        singleStructureFileName += ("." + fileExtension);
        lineEdit->setText(singleStructureFileName);
        lineEdit->end(false);
    }
    
    const int32_t numRows = static_cast<int32_t>(m_structureRows.size());
    for (int32_t i = numStructures; i < numRows; i++) {
        m_structureRows[i].m_fileNamePushButton->hide();
        m_structureRows[i].m_fileNameLineEdit->hide();
    }
}


/**
 * Called when a file name tool button is clicked.  Allows user to set
 * name of new border file with file selection dialog.
 *
 * @param buttonIndex
 *    Index of button that was clicked.
 */
void
BorderFileSplitDialog::fileNameToolButtonClicked(int buttonIndex)
{
    CaretAssertVectorIndex(m_structureRows, buttonIndex);
    QLineEdit* lineEdit = m_structureRows[buttonIndex].m_fileNameLineEdit;
    
    AString newBorderFileName = CaretFileDialog::getSaveFileNameDialog(DataFileTypeEnum::BORDER,
                                                                       this,
                                                                       "Choose Border File Name",
                                                                       lineEdit->text().trimmed());
    if ( ! newBorderFileName.isEmpty()) {
        lineEdit->setText(newBorderFileName);
        lineEdit->end(false);
    }
}

/**
 * Called when the OK button is clicked.
 */
void
BorderFileSplitDialog::okButtonClicked()
{
    const BorderFile* borderFile = m_fileSelectionComboBox->getSelectionModel()->getSelectedFileOfType<BorderFile>();
    if (borderFile == NULL) {
        WuQMessageBox::errorOk(this,
                               "No border file is selected.");
        return;
    }
    
    std::map<StructureEnum::Enum, AString> singleStructureFileNames;
    const int32_t numSingleStructureFiles = static_cast<int32_t>(m_structureRows.size());
    for (int32_t i = 0; i < numSingleStructureFiles; i++) {
        if (m_structureRows[i].m_fileNameLineEdit->isVisible()) {
            singleStructureFileNames.insert(std::make_pair(m_structureRows[i].m_structure,
                                                           m_structureRows[i].m_fileNameLineEdit->text().trimmed()));
        }
    }
    
    if (singleStructureFileNames.empty()) {
        WuQMessageBox::errorOk(this,
                               "Border file contains no structures.");
        return;
    }
    
    std::map<StructureEnum::Enum, int32_t> structureNumberOfNodes;
    Brain* brain = GuiManager::get()->getBrain();
    const int32_t numStructures = brain->getNumberOfBrainStructures();
    for (int32_t iStruct = 0; iStruct < numStructures; iStruct++) {
        const BrainStructure* bs = brain->getBrainStructure(iStruct);
        structureNumberOfNodes.insert(std::make_pair(bs->getStructure(),
                                                     bs->getNumberOfNodes()));
    }
    
    std::vector<BorderFile*> singleStructureBorderFiles;
    AString errorMessage;
    if (borderFile->splitIntoSingleStructureFiles(singleStructureFileNames,
                                                     structureNumberOfNodes,
                                                     singleStructureBorderFiles,
                                                     errorMessage)) {
        for (std::vector<BorderFile*>::iterator iter = singleStructureBorderFiles.begin();
             iter != singleStructureBorderFiles.end();
             iter++) {
            EventManager::get()->sendEvent(EventDataFileAdd(*iter).getPointer());
        }
        
        EventManager::get()->sendEvent(EventUserInterfaceUpdate().getPointer());
        
        WuQMessageBox::informationOk(this, ("New border file(s) were created but not saved.  "
                                            "Use File Menu->Save/Manage Files to save these new files."));
    }
    else {
        WuQMessageBox::errorOk(this, errorMessage);
        return;
    }
    
    WuQDialog::okButtonClicked();
}


