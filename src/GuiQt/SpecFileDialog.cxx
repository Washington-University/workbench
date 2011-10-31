
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

#include <QCheckBox>
#include <QGridLayout>
#include <QLabel>
#include <QVBoxLayout>

#define __SPEC_FILE_DIALOG_DECLARE__
#include "SpecFileDialog.h"
#undef __SPEC_FILE_DIALOG_DECLARE__

#include "SpecFile.h"
#include "SpecFileDataFile.h"
#include "SpecFileDataFileTypeGroup.h"
#include "StructureEnum.h"

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
: WuQDialogModal("Spec File Dialog",
                 parent)
{
    int ctr = 0;
    const int COLUMN_CHECKBOX  = ctr++;
    const int COLUMN_TYPE      = ctr++;
    const int COLUMN_STRUCTURE = ctr++;
    const int COLUMN_NAME      = ctr++;
    
    QWidget* w = new QWidget();
    QGridLayout* gridLayout = new QGridLayout(w);
    
    const int32_t numGroups = specFile->getNumberOfDataFileTypeGroups();
    for (int32_t ig = 0 ; ig < numGroups; ig++) {
        SpecFileDataFileTypeGroup* group = specFile->getDataFileTypeGroup(ig);
        const DataFileTypeEnum::Enum dataFileType = group->getDataFileType();
        const int32_t numFiles = group->getNumberOfFiles();
        for (int idf = 0; idf < numFiles; idf++) {
            SpecFileDataFile* dataFile = group->getFileInformation(idf);
            
            QCheckBox* cb = new QCheckBox(" ");
            cb->setChecked(dataFile->isSelected());
            QLabel* dataTypeLabel = new QLabel(DataFileTypeEnum::toGuiName(dataFileType));
            QLabel* structureLabel = new QLabel("");
            if (DataFileTypeEnum::isFileUsedWithOneStructure(dataFileType)) {
                structureLabel->setText(StructureEnum::toGuiName(dataFile->getStructure()));
            }
            QLabel* nameLabel = new QLabel(dataFile->getFileName());
            
            this->checkBoxes.push_back(cb);
            this->dataFiles.push_back(dataFile);
            
            const int iRow = gridLayout->rowCount();
            gridLayout->addWidget(cb, iRow, COLUMN_CHECKBOX);
            gridLayout->addWidget(dataTypeLabel, iRow, COLUMN_TYPE);
            gridLayout->addWidget(structureLabel, iRow, COLUMN_STRUCTURE);
            gridLayout->addWidget(nameLabel, iRow, COLUMN_NAME);
        }
    }
    
    this->setCentralWidget(w);
}

/**
 * Destructor.
 */
SpecFileDialog::~SpecFileDialog()
{
    
}

/**
 * Called when user presses the OK button.
 */
void 
SpecFileDialog::okButtonPressed()
{
    const int numFiles = static_cast<int32_t>(this->checkBoxes.size());
    for (int i = 0; i < numFiles; i++) {
        const bool selected = this->checkBoxes[i]->isChecked();
        SpecFileDataFile* dataFile = this->dataFiles[i];
        dataFile->setSelected(selected);
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
