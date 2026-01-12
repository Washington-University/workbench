
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

#include "AnnotationFile.h"
#include "CaretAssert.h"
#include "CaretDataFile.h"
#include "CaretFileDialog.h"
#include "CziImageFile.h"
#include "DataFile.h"
#include "DataFileContentCopyMoveInterface.h"
#include "DataFileContentCopyMoveParameters.h"
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
 * @param windowIndex
 *    Index of window.
 * @param sourceDataFileInterface
 *    The source data file (copy from file)
 * @param destinationDataFileInterfaces
 *    Files to which data may be moved.
 * @param options
 *    Options for the dialog
 * @param parent
 *    Optional parent for this dialog.
 */
DataFileContentCopyMoveDialog::DataFileContentCopyMoveDialog(const int32_t windowIndex,
                                                             DataFileContentCopyMoveInterface* sourceDataFileInterface,
                                                             std::vector<DataFileContentCopyMoveInterface*>& destinationDataFileInterfaces,
                                                             const Options& options,
                                                             QWidget* parent)
: WuQDialogModal("Copy/Move Data File Content",
                 parent),
m_windowIndex(windowIndex),
m_sourceDataFileInterface(sourceDataFileInterface),
m_options(options),
m_newDestinatonFileButtonGroupIndex(-1)
{
    CaretAssert(m_sourceDataFileInterface);
    
    /*
     * Annotations may come from a CZI image file
     */
    const DataFile* df(m_sourceDataFileInterface->getAsDataFile());
    CaretAssert(df);
    AString filename(df->getFileNameNoPath());
    if (m_options.isCziAnnotationFile()) {
        const AnnotationFile* annFile(dynamic_cast<const AnnotationFile*>(df));
        if (annFile != NULL) {
            const CziImageFile* cziFile(annFile->getParentCziImageFile());
            if (cziFile != NULL) {
                m_cziImageFileName = cziFile->getFileNameNoPath();
                if (m_cziImageFileName.isNotEmpty()) {
                    m_cziAnnotationFileName = FileInformation::replaceExtension(m_cziImageFileName,
                                                                                DataFileTypeEnum::toFileExtension(DataFileTypeEnum::ANNOTATION));
                }
            }
        }
    }
    
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
    
    QWidget* optionsWidget(createOptionsWidget());
    optionsWidget->setVisible( ! options.isCziAnnotationFile());
    
    QWidget* dialogWidget = new QWidget;
    QVBoxLayout* dialogLayout = new QVBoxLayout(dialogWidget);
    dialogLayout->addWidget(createSourceWidget());
    dialogLayout->addWidget(createDestinationWidget());
    dialogLayout->addWidget(optionsWidget);
    
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
    const DataFile* df(m_sourceDataFileInterface->getAsDataFile());
    CaretAssert(df);
    AString filename(df->getFileNameNoPath());
    if (m_cziImageFileName.isNotEmpty()) {
        filename = m_cziImageFileName;
    }

    QLabel* sourceFileLabel = new QLabel(filename);
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
    
    m_copySelectedAnnotationsOnlyCheckBox = new QCheckBox("Copy Only Annotations SELECTED FOR EDITING");
    m_copySelectedAnnotationsOnlyCheckBox->setChecked(false);
    
    QGroupBox* groupBox = new QGroupBox("Options");
    QVBoxLayout* layout = new QVBoxLayout(groupBox);
    layout->addWidget(m_closeSourceFileCheckBox);
    layout->addWidget(m_copySelectedAnnotationsOnlyCheckBox);
    
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
    m_newDestinationFileRadioButton = new QRadioButton("");
    m_destinationButtonGroup->addButton(m_newDestinationFileRadioButton,
                                        m_newDestinatonFileButtonGroupIndex);

    
    QGroupBox* groupBox = new QGroupBox("Destination File");
    QVBoxLayout* layout = new QVBoxLayout(groupBox);
    
    QListIterator<QAbstractButton*> buttonIter(m_destinationButtonGroup->buttons());
    while (buttonIter.hasNext()) {
        QAbstractButton* button = buttonIter.next();
        if (button == m_newDestinationFileRadioButton) {
            QHBoxLayout* newFileLayout(new QHBoxLayout());
            newFileLayout->setContentsMargins(0, 0, 0, 0);
            newFileLayout->addWidget(button);
            newFileLayout->addWidget(newDestinationFileToolButton);
            newFileLayout->addWidget(m_newDestinationFileNameLabel);
            newFileLayout->addStretch();
            layout->addLayout(newFileLayout);
        }
        else {
            layout->addWidget(button, 0, Qt::AlignLeft);
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
    AString filename;
    if (m_cziAnnotationFileName.isNotEmpty()) {
        filename = m_cziAnnotationFileName;
    }
    
    CaretFileDialog fd(CaretFileDialog::Mode::MODE_SAVE,
                       this,
                       "Choose New File",
                       QString(),
                       DataFileTypeEnum::toQFileDialogFilterForWriting(DataFileTypeEnum::ANNOTATION));
    if (filename.isNotEmpty()) {
        fd.selectFile(filename);
    }
    if (fd.exec() == CaretFileDialog::Accepted) {
        QStringList selectedFileList(fd.selectedFiles());
        if ( ! selectedFileList.isEmpty()) {
            m_newDestinationFileName = selectedFileList.at(0);
            FileInformation fileInfo(m_newDestinationFileName);
            m_newDestinationFileNameLabel->setText(fileInfo.getFileName());
            m_newDestinationFileRadioButton->setChecked(true);
        }
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
                CaretAssert(dynamic_cast<const CaretDataFile*>(m_sourceDataFileInterface));
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
        DataFileContentCopyMoveParameters copyMoveParams(m_sourceDataFileInterface,
                                                         m_windowIndex);
        copyMoveParams.setOptionSelectedItems(m_copySelectedAnnotationsOnlyCheckBox->isChecked());
        
        destinationFile->appendContentFromDataFile(copyMoveParams);
        
        if (newFileFlag) {
            CaretDataFile* destinationCaretFile = dynamic_cast<CaretDataFile*>(destinationFile);
            CaretAssert(destinationCaretFile);
            if (destinationCaretFile->isEmpty()) {
                throw DataFileException("There was no data to copy.  New file was not created.");
            }
            EventDataFileAdd addFileEvent(destinationCaretFile);
            EventManager::get()->sendEvent(addFileEvent.getPointer());
            if (addFileEvent.isError()) {
                throw DataFileException(addFileEvent.getErrorMessage());
            }
        }
        
        if ( ! m_options.isCziAnnotationFile()) {
            if (m_closeSourceFileCheckBox->isChecked()) {
                CaretDataFile* sourceCaretFile = dynamic_cast<CaretDataFile*>(m_sourceDataFileInterface);
                CaretAssert(sourceCaretFile);
                EventManager::get()->sendEvent(EventDataFileDelete(sourceCaretFile).getPointer());
            }
        }
    }
    catch (const DataFileException& dfe) {
        WuQMessageBox::errorOk(this,
                               dfe.whatString());
        return;
    }
    
    WuQDialogModal::okButtonClicked();
}


