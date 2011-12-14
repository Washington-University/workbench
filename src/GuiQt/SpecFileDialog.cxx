
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

#include <QAction>
#include <QActionGroup>
#include <QButtonGroup>
#include <QCheckBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QToolBar>
#include <QToolButton>
#include <QVBoxLayout>

#define __SPEC_FILE_DIALOG_DECLARE__
#include "SpecFileDialog.h"
#undef __SPEC_FILE_DIALOG_DECLARE__

#include "SpecFile.h"
#include "SpecFileDataFile.h"
#include "SpecFileDataFileTypeGroup.h"
#include "StructureEnum.h"
#include "StructureSelectionControl.h"
#include "WuQtUtilities.h"
#include "WuQWidgetObjectGroup.h"

using namespace caret;


    
/**
 * \class SpecFileDialog 
 * \brief Dialog for selection of files in a spec file.
 *
 * Presents a dialog that allows the user to select data
 * files and their attributes for loading into the 
 * application.
 */
/**
 * Constructor.
 */
SpecFileDialog::SpecFileDialog(SpecFile* specFile,
                               QWidget* parent)
: WuQDialogModal("Spec File Data File Selection",
                 parent)
{
    QWidget* w = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(w);
    
    /*
     * Toolbar
     */
    QToolBar* groupToolBar = new QToolBar();
    layout->addWidget(groupToolBar);
    
    /*
     * Action group for all buttons in tool bar
     */
    QActionGroup* toolBarActionGroup = new QActionGroup(this);
    toolBarActionGroup->setExclusive(true);
    QObject::connect(toolBarActionGroup, SIGNAL(triggered(QAction*)),
                     this, SLOT(toolBarButtonTriggered(QAction*)));
    
    /*
     * All action tool button
     */
    QAction* allToolButtonAction = WuQtUtilities::createAction("All", 
                                                               "Show files of all types", 
                                                               this);
    allToolButtonAction->setData(qVariantFromValue(0));
    allToolButtonAction->setCheckable(true);
    QToolButton* allToolButton = new QToolButton(this);
    allToolButton->setDefaultAction(allToolButtonAction);
    groupToolBar->addWidget(allToolButton);
    toolBarActionGroup->addAction(allToolButtonAction);

    
    /*
     * Display each type of data file
     */
    const int32_t numGroups = specFile->getNumberOfDataFileTypeGroups();
    for (int32_t ig = 0 ; ig < numGroups; ig++) {
        SpecFileDataFileTypeGroup* group = specFile->getDataFileTypeGroup(ig);
        GuiSpecGroup* guiSpecGroup = this->createDataTypeGroup(group);
        if (guiSpecGroup != NULL) {
            dataTypeGroups.push_back(guiSpecGroup);
            layout->addWidget(guiSpecGroup->widget);
            
            QAction* groupToolButtonAction = WuQtUtilities::createAction(DataFileTypeEnum::toGuiName(group->getDataFileType()), 
                                                                         "Show only files of this type", 
                                                                         this);
            groupToolButtonAction->setData(qVariantFromValue((void*)guiSpecGroup));
            groupToolButtonAction->setCheckable(true);
            
            QToolButton* groupToolButton = new QToolButton(this);
            groupToolButton->setDefaultAction(groupToolButtonAction);
            groupToolBar->addWidget(groupToolButton);
            
            toolBarActionGroup->addAction(groupToolButtonAction);
        }
    }
    
    this->setCentralWidget(w);
}

/**
 * Destructor.
 */
SpecFileDialog::~SpecFileDialog()
{    
    const int32_t numGroups = static_cast<int32_t>(this->dataTypeGroups.size());
    for (int32_t ig = 0; ig < numGroups; ig++) {
        delete this->dataTypeGroups[ig];
    }
}

/**
 * Called when a tool button is selected.
 * @param action
 *   Action for button that was pressed.
 */
void 
SpecFileDialog::toolBarButtonTriggered(QAction* action)
{
    void* p = action->data().value<void*>();
    
    if (p != NULL) {
        GuiSpecGroup* guiSpecGroup = (GuiSpecGroup*)p;
    }
}

/**
 * List the files in the data file type group in a widget.
 * 
 * @param group
 *   Group of files of one type.
 * @return 
 *   If there are files in the group, a widget listing the file
 *   or NULL if no files in the group.
 */
SpecFileDialog::GuiSpecGroup* 
SpecFileDialog::createDataTypeGroup(SpecFileDataFileTypeGroup* group)
{
    const int32_t numFiles = group->getNumberOfFiles();
    if (numFiles <= 0) {
        return NULL;
    }
    
    GuiSpecGroup* guiSpecGroup = new GuiSpecGroup();
    
    int ctr = 0;
    const int COLUMN_CHECKBOX  = ctr++;
    const int COLUMN_METADATA  = ctr++;
    const int COLUMN_REMOVE    = ctr++;
    const int COLUMN_STRUCTURE = ctr++;
    const int COLUMN_NAME      = ctr++;
    const int NUMBER_OF_COLUMNS = ctr;
    
    const DataFileTypeEnum::Enum dataFileType = group->getDataFileType();
    QGroupBox* groupBox = new QGroupBox(DataFileTypeEnum::toGuiName(dataFileType));
    QGridLayout* gridLayout = new QGridLayout(groupBox);
    for (int32_t i = 0; i < NUMBER_OF_COLUMNS; i++) {
        gridLayout->setColumnStretch(i, 0);
    }
    gridLayout->setColumnStretch(NUMBER_OF_COLUMNS, 100);
    

    const bool hasStructure = DataFileTypeEnum::isFileUsedWithOneStructure(dataFileType);
    
    for (int idf = 0; idf < numFiles; idf++) {
        SpecFileDataFile* dataFileInfo = group->getFileInformation(idf);
        
        GuiSpecDataFileInfo* dfi = new GuiSpecDataFileInfo(this,
                                                 dataFileInfo,
                                                 hasStructure);
                                                 
        const int iRow = gridLayout->rowCount();
        gridLayout->addWidget(dfi->selectionCheckBox, iRow, COLUMN_CHECKBOX);
        gridLayout->addWidget(dfi->metadataToolButton, iRow, COLUMN_METADATA);
        gridLayout->addWidget(dfi->removeToolButton, iRow, COLUMN_REMOVE);
        if (dfi->structureSelectionControl != NULL) {
            gridLayout->addWidget(dfi->structureSelectionControl->getWidget(), iRow, COLUMN_STRUCTURE);
        }
        gridLayout->addWidget(dfi->nameLabel, iRow, COLUMN_NAME);
        
        guiSpecGroup->dataFiles.push_back(dfi);
    }
    
    guiSpecGroup->widget = groupBox;
    
    return guiSpecGroup;
}

/**
 * Called when user presses the OK button.
 */
void 
SpecFileDialog::okButtonPressed()
{
    const int32_t numGroups = static_cast<int32_t>(this->dataTypeGroups.size());
    for (int32_t ig = 0; ig < numGroups; ig++) {
        GuiSpecGroup* guiSpecGroup = this->dataTypeGroups[ig];
        const int32_t numFiles = static_cast<int32_t>(guiSpecGroup->dataFiles.size());
        for (int j = 0; j < numFiles; j++) {
            GuiSpecDataFileInfo* fileInfo = guiSpecGroup->dataFiles[j];
            fileInfo->dataFileInfo->setSelected(fileInfo->selectionCheckBox->isChecked());
        }        
    }
    
    WuQDialogModal::okButtonPressed();
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
SpecFileDialog::toString() const
{
    return "SpecFileDialog";
}


//=====================================================================================
/**
 * Constructor.
 * @param dataFileInfo
 *    Info about the data file.
 * @param isStructureFile
      If true, file is structure related.
 */
GuiSpecDataFileInfo::GuiSpecDataFileInfo(QObject* parent,
                                   SpecFileDataFile* dataFileInfo,
                                   const bool isStructureFile)
: QObject(parent)
{
    this->dataFileInfo = dataFileInfo;
    
    this->selectionCheckBox = new QCheckBox("");
    this->selectionCheckBox->setChecked(dataFileInfo->isSelected());
    
    this->metadataAction = WuQtUtilities::createAction("M",
                                                     "View the file's metadata",
                                                     this,
                                                     this,
                                                     SLOT(metadataActionTriggered()));
    this->metadataToolButton = new QToolButton();
    this->metadataToolButton->setDefaultAction(this->metadataAction);
    
    
    this->removeAction = WuQtUtilities::createAction("X",
                                                     "Remove file from the spec file (does NOT delete file)",
                                                     this,
                                                     this,
                                                     SLOT(removeActionTriggered()));
    this->removeToolButton = new QToolButton();
    this->removeToolButton->setDefaultAction(this->removeAction);
    
    this->structureSelectionControl = NULL;
    if (isStructureFile) {
        this->structureSelectionControl = new StructureSelectionControl();
        this->structureSelectionControl->setSelectedStructure(dataFileInfo->getStructure());
    }
    
    this->nameLabel = new QLabel(dataFileInfo->getFileName());
    
    this->widgetGroup = new WuQWidgetObjectGroup(this);
}

/**
 * Destructor.
 */
GuiSpecDataFileInfo::~GuiSpecDataFileInfo()
{
    
}

/**
 * Called when metadata button is pressed.
 */
void 
GuiSpecDataFileInfo::metadataActionTriggered()
{
    
}

/**
 * Called when remove button is pressed.
 */
void GuiSpecDataFileInfo::removeActionTriggered()
{
    
}
