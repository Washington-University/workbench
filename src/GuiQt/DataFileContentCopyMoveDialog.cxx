
/*LICENSE_START*/
/*
 *  Copyright (C) 2016 Washington University School of Medicine
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

#define __DATA_FILE_CONTENT_COPY_MOVE_DIALOG_DECLARE__
#include "DataFileContentCopyMoveDialog.h"
#undef __DATA_FILE_CONTENT_COPY_MOVE_DIALOG_DECLARE__

#include <QButtonGroup>
#include <QCheckBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QRadioButton>
#include <QToolButton>
#include <QVBoxLayout>

#include "CaretAssert.h"
#include "CaretDataFile.h"
#include "CaretFileDialog.h"
#include "DataFile.h"
#include "DataFileContentCopyMoveInterface.h"
#include "DataFileException.h"
#include "EventDataFileAdd.h"
#include "EventDataFileDelete.h"
#include "EventManager.h"
#include "FileInformation.h"
#include "WuQMessageBox.h"

using namespace caret;


    
/**
 * \class caret::DataFileContentCopyMoveDialog 
 * \brief Dialog for copying/moving content between data files
 * \ingroup GuiQt
 */

/**
 * Constructor.
 *
 * @param sourceDataFileInterface
 *    The source data file (copy from file)
 * @param destinationDataFileInterfaces
 *    Files to which data may be moved.
 * @param parent
 *    Optional parent for this dialog.
 */
DataFileContentCopyMoveDialog::DataFileContentCopyMoveDialog(DataFileContentCopyMoveInterface* sourceDataFileInterface,
                                                             std::vector<DataFileContentCopyMoveInterface*>& destinationDataFileInterfaces,
                                                             QWidget* parent)
: WuQDialogModal("Copy/Move Data File Content",
                 parent),
m_sourceDataFileInterface(sourceDataFileInterface),
m_newDestinatonFileButtonGroupIndex(-1)
{
    CaretAssert(m_sourceDataFileInterface);
    
    /*
     * Copy pointers to destination files but ignore the source file
     */
    for (std::vector<DataFileContentCopyMoveInterface*>::iterator fileIter = destinationDataFileInterfaces.begin();
         fileIter != destinationDataFileInterfaces.end();
         fileIter++) {
        DataFileContentCopyMoveInterface* df = *fileIter;
        if (df != sourceDataFileInterface) {
            m_destinationDataFileInterfaces.push_back(df);
        }
    }
    
    QWidget* dialogWidget = new QWidget;
    QVBoxLayout* dialogLayout = new QVBoxLayout(dialogWidget);
    dialogLayout->addWidget(createSourceWidget());
    dialogLayout->addWidget(createDestinationWidget());
    dialogLayout->addWidget(createOptionsWidget());
    
    setCentralWidget(dialogWidget,
                     WuQDialog::SCROLL_AREA_AS_NEEDED);
}

/**
 * Destructor.
 */
DataFileContentCopyMoveDialog::~DataFileContentCopyMoveDialog()
{
}

/**
 * @return The source widget.
 */
QWidget*
DataFileContentCopyMoveDialog::createSourceWidget()
{
    QLabel* sourceFileLabel = new QLabel(m_sourceDataFileInterface->getAsDataFile()->getFileNameNoPath());
    
    QGroupBox* groupBox = new QGroupBox("Source File");
    QVBoxLayout* layout = new QVBoxLayout(groupBox);
    layout->addWidget(sourceFileLabel);
    
    return groupBox;
}

/**
 * @return The options widget.
 */
QWidget*
DataFileContentCopyMoveDialog::createOptionsWidget()
{
    m_closeSourceFileCheckBox = new QCheckBox("Close Source File After Copying Data");
    m_closeSourceFileCheckBox->setChecked(true);
    QObject::connect(m_closeSourceFileCheckBox, SIGNAL(toggled(bool)),
                     this, SLOT(closeSourceFileCheckBoxToggled(bool)));
    
    QGroupBox* groupBox = new QGroupBox("Options");
    QVBoxLayout* layout = new QVBoxLayout(groupBox);
    layout->addWidget(m_closeSourceFileCheckBox);
    
    return groupBox;
}

/**
 * Called when the close source file checkbox is toggled.
 *
 * @param checked
 *     New checked status.
 */
void
DataFileContentCopyMoveDialog::closeSourceFileCheckBoxToggled(bool checked)
{
    
    if ( ! checked) {
        /*
         * Display warning only once.
         */
        static bool firstTimeFlag = true;
        
        if (firstTimeFlag) {
            const QString msg("If the source file is not closed, identical data items will appear "
                              "in the graphics region.");
            WuQMessageBox::warningOk(m_closeSourceFileCheckBox,
                                     msg);
            firstTimeFlag = false;
        }
    }
}


/**
 * @return The destination widget.
 */
QWidget*
DataFileContentCopyMoveDialog::createDestinationWidget()
{
    m_destinationButtonGroup = new QButtonGroup;
    
    const int32_t numberOfFiles = static_cast<int32_t>(m_destinationDataFileInterfaces.size());
    for (int32_t iFile = 0; iFile < numberOfFiles; iFile++) {
        DataFile* dataFile = m_destinationDataFileInterfaces[iFile]->getAsDataFile();
        QRadioButton* rb = new QRadioButton(dataFile->getFileNameNoPath());
        m_destinationButtonGroup->addButton(rb, iFile);
    }
    
    QToolButton* newDestinationFileToolButton = new QToolButton();
    QObject::connect(newDestinationFileToolButton, SIGNAL(clicked(bool)),
                     this, SLOT(newDestinationFileToolButtonClicked()));
    newDestinationFileToolButton->setText("New File Name...");
    m_newDestinationFileNameLabel = new QLabel("                          ");
    m_newDestinatonFileButtonGroupIndex = m_destinationButtonGroup->buttons().size();
    QRadioButton* newFileRadioButton = new QRadioButton("");
    m_destinationButtonGroup->addButton(newFileRadioButton,
                                        m_newDestinatonFileButtonGroupIndex);

    
    QGroupBox* groupBox = new QGroupBox("Destination File");
    QGridLayout* layout = new QGridLayout(groupBox);
    layout->setColumnStretch(0, 0);
    layout->setColumnStretch(1, 0);
    layout->setColumnStretch(2, 0);
    layout->setColumnStretch(3, 100);
    
    QListIterator<QAbstractButton*> buttonIter(m_destinationButtonGroup->buttons());
    while (buttonIter.hasNext()) {
        const int row = layout->rowCount();
        QAbstractButton* button = buttonIter.next();
        if (button == newFileRadioButton) {
            layout->addWidget(button,
                              row, 0);
            layout->addWidget(newDestinationFileToolButton,
                              row, 1);
            layout->addWidget(m_newDestinationFileNameLabel,
                              row, 2, Qt::AlignLeft);
        }
        else {
            layout->addWidget(button,
                              row, 0, 1, 3, Qt::AlignLeft);
        }
    }
    
    return groupBox;
}

/**
 * Gets called when "New File..." button is clicked.
 */
void
DataFileContentCopyMoveDialog::newDestinationFileToolButtonClicked()
{
    const CaretDataFile* caretDataFile = dynamic_cast<const CaretDataFile*>(m_sourceDataFileInterface);
    CaretAssert(caretDataFile);
    const QString fileName = CaretFileDialog::getSaveFileNameDialog(caretDataFile->getDataFileType(),
                                                                    this,
                                                                    "Choose New File");
    if ( ! fileName.isEmpty()) {
        m_newDestinationFileName = fileName;
        FileInformation fileInfo(fileName);
        m_newDestinationFileNameLabel->setText(fileInfo.getFileName());
    }
}


/**
 * Called when OK button clicked.
 */
void
DataFileContentCopyMoveDialog::okButtonClicked()
{
    try {
        DataFileContentCopyMoveInterface* destinationFile = NULL;
        bool newFileFlag = false;
        const int32_t destinationFileIndex = m_destinationButtonGroup->checkedId();
        if (destinationFileIndex >= 0) {
            if (destinationFileIndex == m_newDestinatonFileButtonGroupIndex) {
                if (m_newDestinationFileName.isEmpty()) {
                    throw DataFileException("New file name is empty.");
                }
                const CaretDataFile* caretDataFile = dynamic_cast<const CaretDataFile*>(m_sourceDataFileInterface);
                CaretAssert(caretDataFile);
                destinationFile = m_sourceDataFileInterface->newInstanceOfDataFile();
                if (destinationFile == NULL) {
                    throw DataFileException("Failed to created new file.");
                }
                destinationFile->getAsDataFile()->setFileName(m_newDestinationFileName);
                newFileFlag = true;
            }
            else {
                CaretAssertVectorIndex(m_destinationDataFileInterfaces, destinationFileIndex);
                destinationFile = m_destinationDataFileInterfaces[destinationFileIndex];
            }
        }
        
        if (destinationFile == NULL) {
            throw DataFileException("No destination file is selected");
        }
        
        CaretAssert(destinationFile);
        destinationFile->appendContentFromDataFile(m_sourceDataFileInterface);
        
        if (newFileFlag) {
            CaretDataFile* destinationCaretFile = dynamic_cast<CaretDataFile*>(destinationFile);
            CaretAssert(destinationCaretFile);
            EventDataFileAdd addFileEvent(destinationCaretFile);
            EventManager::get()->sendEvent(addFileEvent.getPointer());
            if (addFileEvent.isError()) {
                throw DataFileException(addFileEvent.getErrorMessage());
            }
        }
        
        if (m_closeSourceFileCheckBox->isChecked()) {
            CaretDataFile* sourceCaretFile = dynamic_cast<CaretDataFile*>(m_sourceDataFileInterface);
            CaretAssert(sourceCaretFile);
            EventManager::get()->sendEvent(EventDataFileDelete(sourceCaretFile).getPointer());
        }
    }
    catch (const DataFileException& dfe) {
        WuQMessageBox::errorOk(this,
                               dfe.whatString());
        return;
    }
    
    WuQDialogModal::okButtonClicked();
}


