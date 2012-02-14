
/*LICENSE_START*/
/* 
 *  Copyright 1995-2011 Washington University School of Medicine 
 * 
 *  http://brainmap.wustl.edu 
 * 
 *  This file is part of CARET. 
 * 
 *  CARET is free software; you can redistribute it and/or modify 
 *  it under the terms of the GNU General Public License as published by 
 *  the Free Software Foundation; either version 2 of the License, or 
 *  (at your option) any later version. 
 * 
 *  CARET is distributed in the hope that it will be useful, 
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of 
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
 *  GNU General Public License for more details. 
 * 
 *  You should have received a copy of the GNU General Public License 
 *  along with CARET; if not, write to the Free Software 
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA 
 * 
 */ 

#define __MANAGE_LOADED_FILES_DIALOG_DECLARE__
#include "ManageLoadedFilesDialog.h"
#undef __MANAGE_LOADED_FILES_DIALOG_DECLARE__

#include "Brain.h"
#include "CaretAssert.h"
#include "CaretDataFile.h"
#include "CaretMappableDataFile.h"
#include "EventManager.h"
#include "EventGraphicsUpdateAllWindows.h"
#include "EventUserInterfaceUpdate.h"
#include "CaretFileDialog.h"
#include "WuQMessageBox.h"
#include "WuQtUtilities.h"
#include "WuQWidgetObjectGroup.h"

#include <QAction>
#include <QCheckBox>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QToolButton>


using namespace caret;


    
/**
 * \class ManageLoadedFilesDialog 
 *
 * 
 * \brief Dialog for managing loaded files.
 *
 * Dialog that allows the user to see and save loaded data files.
 */
/**
 * Constructor.
 */
ManageLoadedFilesDialog::ManageLoadedFilesDialog(QWidget* parent,
                                                 Brain* brain)
: WuQDialogModal("Manage and Save Loaded Files",
                 parent)
{
    this->brain = brain;
    
    this->setOkButtonText("");
    this->setCancelButtonText("Close");
    this->saveCheckedFilesPushButton = this->addUserPushButton("Save Checked Files");
    
    QWidget* filesWidget = new QWidget();
    QGridLayout* gridLayout = new QGridLayout(filesWidget);
    int gridRow = gridLayout->rowCount();
    
    gridLayout->addWidget(new QLabel("Save"),
                          gridRow,
                          COLUMN_SAVE_CHECKBOX);
    gridLayout->addWidget(new QLabel("Structure"),
                          gridRow,
                          COLUMN_STRUCTURE);
    gridLayout->addWidget(new QLabel("File Type"),
                          gridRow,
                          COLUMN_FILE_TYPE);
    gridLayout->addWidget(new QLabel("Mod"),
                          gridRow,
                          COLUMN_MODIFIED);
    gridLayout->addWidget(new QLabel("Metadata"),
                          gridRow,
                          COLUMN_METADATA);
    gridLayout->addWidget(new QLabel("Remove\nFile"),
                          gridRow,
                          COLUMN_REMOVE_BUTTON);
    gridLayout->addWidget(new QLabel("Remove\nMap"),
                          gridRow,
                          COLUMN_REMOVE_MAP_BUTTON);
    gridLayout->addWidget(new QLabel("Choose\nFile"),
                          gridRow,
                          COLUMN_FILE_NAME_BUTTON);
    gridLayout->addWidget(new QLabel("File Name"),
                          gridRow,
                          COLUMN_FILE_NAME);
    
    for (int i = 0; i < COLUMN_LAST; i++) {
        gridLayout->setColumnStretch(i, 0);
    }
    gridLayout->setColumnStretch(COLUMN_FILE_NAME, 100);
    
    std::vector<CaretDataFile*> caretDataFiles;
    brain->getAllDataFiles(caretDataFiles);
    
    const int32_t numFiles = static_cast<int32_t>(caretDataFiles.size());
    for (int32_t i = 0; i < numFiles; i++) {
        ManageFileRow* fileRow = new ManageFileRow(this,
                                                   this->brain,
                                                   caretDataFiles[i]);
        this->fileRows.push_back(fileRow);
        
        gridRow = gridLayout->rowCount();
        gridLayout->addWidget(fileRow->saveCheckBox,
                              gridRow,
                              COLUMN_SAVE_CHECKBOX);
        gridLayout->addWidget(fileRow->structureLabel,
                              gridRow,
                              COLUMN_STRUCTURE);
        gridLayout->addWidget(fileRow->fileTypeLabel,
                              gridRow,
                              COLUMN_FILE_TYPE);
        gridLayout->addWidget(fileRow->modifiedLabel,
                              gridRow,
                              COLUMN_MODIFIED);
        gridLayout->addWidget(fileRow->metaDataToolButton,
                              gridRow,
                              COLUMN_METADATA);
        gridLayout->addWidget(fileRow->removeFileToolButton,
                              gridRow,
                              COLUMN_REMOVE_BUTTON);
        gridLayout->addWidget(fileRow->removeMapToolButton,
                              gridRow,
                              COLUMN_REMOVE_MAP_BUTTON);
        gridLayout->addWidget(fileRow->fileNameToolButton,
                              gridRow,
                              COLUMN_FILE_NAME_BUTTON);
        gridLayout->addWidget(fileRow->fileNameLineEdit,
                              gridRow,
                              COLUMN_FILE_NAME);
    }
    
    QWidget* fw = WuQtUtilities::insertIntoScrollAreaIfNeeded(this,
                                                              filesWidget);
    
    QWidget* w = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(w);
    layout->addWidget(fw);

    this->setCentralWidget(w);
}

/**
 * Destructor.
 */
ManageLoadedFilesDialog::~ManageLoadedFilesDialog()
{
    
}

/**
 * Called when a push button was added using addUserPushButton().
 * Subclasses MUST override this if user push buttons were 
 * added using addUserPushButton().
 *
 * @param userPushButton
 *    User push button that was pressed.
 */
void 
ManageLoadedFilesDialog::userButtonPressed(QPushButton* userPushButton)
{
    if (this->saveCheckedFilesPushButton == userPushButton) {
        AString msg;
        try {
            const int32_t numFiles = static_cast<int32_t>(this->fileRows.size());
            for (int32_t i = 0; i < numFiles; i++) {
                this->fileRows[i]->saveFile();
            }
        }
        catch (const DataFileException& e) {
            if (msg.isEmpty() == false) {
                msg += "\n";
            }
            msg += e.whatString();
        }
        
        if (msg.isEmpty() == false) {
            WuQMessageBox::errorOk(this, msg);
        }
    }
    else {
        CaretAssert(0);
    }
    
    this->updateUserInterfaceAndGraphics();
}

/**
 * Update the user-interface.
 */
void 
ManageLoadedFilesDialog::updateUserInterfaceAndGraphics()
{
    EventManager::get()->sendEvent(EventUserInterfaceUpdate().getPointer());
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
}


/**
 * Constructor that creates a manage file dialog row.
 * @param caretDataFile
 *    Data file for the row.
 */
ManageFileRow::ManageFileRow(ManageLoadedFilesDialog* parentWidget,
                             Brain* brain,
                             CaretDataFile* caretDataFile)
{
    this->brain = brain;
    this->parentWidget  = parentWidget;
    this->caretDataFile = caretDataFile;
    this->caretMappableDataFile = dynamic_cast<CaretMappableDataFile*>(this->caretDataFile);
    
    this->saveCheckBox = new QCheckBox(" ");
    
    this->structureLabel = new QLabel("");
    const StructureEnum::Enum structure = caretDataFile->getStructure();
    if (structure != StructureEnum::INVALID) {
        this->structureLabel->setText(StructureEnum::toGuiName(structure));
    }
    
    this->fileTypeLabel = new QLabel(DataFileTypeEnum::toGuiName(caretDataFile->getDataFileType()));
    
    this->modifiedLabel = new QLabel("   ");
    if (this->caretDataFile->isModified()) {
        this->modifiedLabel->setText("***");
        this->modifiedLabel->setStyleSheet("QLabel { color: red; }");
        this->saveCheckBox->setChecked(true);
    }
    
    QAction* metaDataAction = WuQtUtilities::createAction("Edit...",
                                                          "Edit the file's metadata",
                                                          this,
                                                          this,
                                                          SLOT(metaDataToolButtonPressed()));
    this->metaDataToolButton = new QToolButton();
    this->metaDataToolButton->setDefaultAction(metaDataAction);
    
    QAction* removeFileAction = WuQtUtilities::createAction("XF",
                                                          "Remove the file from memory (does NOT delete the file from disk)",
                                                          this,
                                                          this,
                                                          SLOT(removeFileToolButtonPressed()));
    this->removeFileToolButton = new QToolButton();
    this->removeFileToolButton->setDefaultAction(removeFileAction);
    
    QAction* removeMapAction = WuQtUtilities::createAction("XM",
                                                          "Remove a map from the file",
                                                          this,
                                                          this,
                                                          SLOT(removeMapToolButtonPressed()));
    if (this->caretMappableDataFile == NULL) {
        removeMapAction->setEnabled(false);
    }
    this->removeMapToolButton = new QToolButton();
    this->removeMapToolButton->setDefaultAction(removeMapAction);
    
    QAction* fileNameAction = WuQtUtilities::createAction("Name...",
                                                          "Use a File Dialog to set the name of the file",
                                                          this,
                                                          this,
                                                          SLOT(fileNameToolButtonPressed()));
    this->fileNameToolButton = new QToolButton();
    this->fileNameToolButton->setDefaultAction(fileNameAction);
    
    const int lineEditWidth = 300;
    this->fileNameLineEdit = new QLineEdit(); 
    this->fileNameLineEdit->setMinimumWidth(lineEditWidth);
    this->fileNameLineEdit->setText(this->caretDataFile->getFileName());
    
    this->widgetGroup = new WuQWidgetObjectGroup(this);
    this->widgetGroup->add(this->saveCheckBox);
    this->widgetGroup->add(this->structureLabel);
    this->widgetGroup->add(this->fileTypeLabel);
    this->widgetGroup->add(this->modifiedLabel);
    this->widgetGroup->add(this->metaDataToolButton);
    this->widgetGroup->add(this->removeFileToolButton);
    this->widgetGroup->add(this->removeMapToolButton);
    this->widgetGroup->add(this->fileNameToolButton);
    this->widgetGroup->add(this->fileNameLineEdit);
}

/**
 * Destructor.
 */
ManageFileRow::~ManageFileRow()
{
}

/**
 * Called when metadata button is pressed.
 */
void 
ManageFileRow::metaDataToolButtonPressed()
{
    WuQMessageBox::informationOk(this->parentWidget, ("metadata not implemented " + this->caretDataFile->getFileNameNoPath()));
}

/**
 * Called when remove file button is pressed.
 */
void
ManageFileRow::removeFileToolButtonPressed()
{
    try {
        this->brain->removeDataFile(this->caretDataFile);
        this->widgetGroup->setEnabled(false);
        this->parentWidget->updateUserInterfaceAndGraphics();
    }
    catch (const DataFileException& e) {
        this->parentWidget->updateUserInterfaceAndGraphics();
        WuQMessageBox::errorOk(this->parentWidget, e.whatString());
    }
}

/**
 * Called when remove map button is pressed.
 */
void 
ManageFileRow::removeMapToolButtonPressed()
{
    WuQMessageBox::informationOk(this->parentWidget, ("remove map not implemented " + this->caretDataFile->getFileNameNoPath()));
    this->parentWidget->updateUserInterfaceAndGraphics();
}

/**
 * Called when file name button is pressed.
 */
void 
ManageFileRow::fileNameToolButtonPressed()
{
    AString filename = CaretFileDialog::getSaveFileNameDialog(this->parentWidget,
                                                      "Choose File",
                                                      this->brain->getCurrentDirectory(),
                                                      DataFileTypeEnum::toQFileDialogFilter(this->caretDataFile->getDataFileType()));
    if (filename.isEmpty() == false) {
        this->fileNameLineEdit->setText(filename);
    }
}

/**
 * Called to save the file
 */
void 
ManageFileRow::saveFile()  throw (DataFileException)
{
    if (this->saveCheckBox->isChecked()) {
        AString name = this->fileNameLineEdit->text().trimmed();
        this->caretDataFile->setFileName(name);
        this->brain->writeDataFile(this->caretDataFile);
        this->modifiedLabel->setText("   ");
        this->saveCheckBox->setChecked(false);
        this->parentWidget->updateUserInterfaceAndGraphics();
    }
}




