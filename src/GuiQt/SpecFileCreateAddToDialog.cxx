
/*LICENSE_START*/
/*
 * Copyright 2012 Washington University,
 * All rights reserved.
 *
 * Connectome DB and Connectome Workbench are part of the integrated Connectome 
 * Informatics Platform.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    * Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *    * Neither the names of Washington University nor the
 *      names of its contributors may be used to endorse or promote products
 *      derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR  
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
 * SUBSTITUTE GOODS OR  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
 * POSSIBILITY OF SUCH DAMAGE.
 */
/*LICENSE_END*/

#include <QAction>
#include <QCheckBox>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QToolButton>

#define __SPEC_FILE_CREATE_ADD_TO_DIALOG_DECLARE__
#include "SpecFileCreateAddToDialog.h"
#undef __SPEC_FILE_CREATE_ADD_TO_DIALOG_DECLARE__

#include "Brain.h"
#include "CaretFileDialog.h"
#include "FileInformation.h"
#include "SpecFile.h"
#include "WuQMessageBox.h"
#include "WuQtUtilities.h"

using namespace caret;


    
/**
 * \class caret::SpecFileCreateAddToDialog 
 * \brief Add file to spec file (and possibly create spec file)
 */

/**
 * Constructor.
 */
SpecFileCreateAddToDialog::SpecFileCreateAddToDialog(Brain* brain,
                                                     QWidget* parent)
: WuQDialogModal("Choose a Spec File",
                 parent)
{
    const bool showAddCheckBox = false;
    
    m_specFile = brain->getSpecFile();
    const QString specFileName = m_specFile->getFileName();
    
    FileInformation fileInfo(specFileName);
    m_isSpecFileValid = fileInfo.exists();
    
    QString checkBoxText = "Add data file(s) to the Spec File";
    if (m_isSpecFileValid == false) {
        checkBoxText = "Create Spec File and add data file(s) to the Spec File";
    }
    
    m_createAddSpecFileCheckBox = new QCheckBox(checkBoxText);
    m_specFileNameLineEdit      = new QLineEdit();
    m_specFileNameLineEdit->setMinimumWidth(500);
    m_specFileNameLineEdit->setReadOnly(true);

    QWidget* fileLabelOrToolButtonWidget = NULL;
    if (m_isSpecFileValid) {
        QLabel* fileLabel = new QLabel("Choose Spec File: ");
        fileLabelOrToolButtonWidget = fileLabel;
        m_specFileNameLineEdit->setText(specFileName);
        m_specFileNameLineEdit->end(false);
    }
    else {
        QAction* fileAction = WuQtUtilities::createAction("Spec File...", 
                                                          "Choose Spec File", 
                                                          this, 
                                                          this, 
                                                          SLOT(fileButtonClicked()));
        QToolButton* fileToolButton = new QToolButton();
        fileToolButton->setDefaultAction(fileAction);
        fileLabelOrToolButtonWidget = fileToolButton;
    }
    
    QLabel* instructionsLabel = new QLabel("There is no spec file.  Press the <B>Choose Spec File</B> button to navigate the "
                                           "the file system, choose an existing Spec File or enter the name of a new "
                                           "Spec File in the desired directory, and press the <B>Continue</B> button.  Otherwise,"
                                           "press the <B>Cancel</B> button and uncheck the Add To Spec File option on "
                                           "the previous Open or Save window.");
    instructionsLabel->setWordWrap(true);
    if (showAddCheckBox) {
        instructionsLabel->setVisible(false);
    }
    
    QWidget* widget = new QWidget();
    QGridLayout* gridLayout = new QGridLayout(widget);
    gridLayout->addWidget(instructionsLabel, 0, 0, 1, 2);
    gridLayout->addWidget(m_createAddSpecFileCheckBox, 1, 0, 1, 2);
    gridLayout->addWidget(fileLabelOrToolButtonWidget, 2, 0);
    gridLayout->addWidget(m_specFileNameLineEdit, 2, 1);
    setCentralWidget(widget);

    if (dynamic_cast<QToolButton*>(fileLabelOrToolButtonWidget) != NULL) {
        QObject::connect(m_createAddSpecFileCheckBox, SIGNAL(toggled(bool)),
                         fileLabelOrToolButtonWidget, SLOT(setEnabled(bool)));
    }
    QObject::connect(m_createAddSpecFileCheckBox, SIGNAL(toggled(bool)),
                     m_specFileNameLineEdit, SLOT(setEnabled(bool)));
    m_createAddSpecFileCheckBox->setChecked(true);
    
    /*
     * May not need checkbox any longer
     */
    if (showAddCheckBox == false) {
        m_createAddSpecFileCheckBox->setVisible(false);
    }
    
    setOkButtonText("Continue");
}

/**
 * Destructor.
 */
SpecFileCreateAddToDialog::~SpecFileCreateAddToDialog()
{
    
}

/**
 * Called when file button clicked to selecte a
 * spec file name.
 */
void 
SpecFileCreateAddToDialog::fileButtonClicked()
{
    CaretFileDialog fd(this);
    fd.setAcceptMode(CaretFileDialog::AcceptSave);
    fd.setLabelText(QFileDialog::Accept, "Choose");
    fd.setNameFilter(DataFileTypeEnum::toQFileDialogFilter(DataFileTypeEnum::SPECIFICATION));
    fd.setFileMode(CaretFileDialog::AnyFile);
    fd.setViewMode(CaretFileDialog::List);
    fd.selectNameFilter(DataFileTypeEnum::toQFileDialogFilter(DataFileTypeEnum::SPECIFICATION));
    fd.setConfirmOverwrite(false);
    
    if (fd.exec() == CaretFileDialog::Accepted) {
        if (fd.selectedFiles().empty() == false) {
            QString filename = fd.selectedFiles().at(0);

            if (filename.isEmpty() == false) {
                const QString fileExt = ("." + DataFileTypeEnum::toFileExtension(DataFileTypeEnum::SPECIFICATION));
                if (filename.endsWith(fileExt) == false) {
                    filename += fileExt;
                }
                m_specFileNameLineEdit->setText(filename);
            }
        }
    }
    
//    QString filename = CaretFileDialog::getSaveFileNameDialog(this,
//                                                              "Choose Spec File",
//                                                              "",
//                                                              DataFileTypeEnum::toQFileDialogFilter(DataFileTypeEnum::SPECIFICATION));
//    if (filename.isEmpty() == false) {
//        const QString fileExt = ("." + DataFileTypeEnum::toFileExtension(DataFileTypeEnum::SPECIFICATION));
//        if (filename.endsWith(fileExt) == false) {
//            filename += fileExt;
//        }
//        m_specFileNameLineEdit->setText(filename);
//    }
}

/**
 * Called the OK button is pressed.
 */
void 
SpecFileCreateAddToDialog::okButtonPressed()
{
    if (m_createAddSpecFileCheckBox->isChecked()) {
        if (m_isSpecFileValid == false) {
            QString specFileName = m_specFileNameLineEdit->text().trimmed();
            if (specFileName.isEmpty()) {
                WuQMessageBox::errorOk(this,
                                       "Spec File name is invalid");
                return;
            }
            
            const QString fileExt = ("." + DataFileTypeEnum::toFileExtension(DataFileTypeEnum::SPECIFICATION));
            if (specFileName.endsWith(fileExt) == false) {
                specFileName += fileExt;
            }
            
            FileInformation fileInfo(specFileName);
            if (fileInfo.exists()) {
                /**
                 * If spec file exists, need to read it.
                 */
                try {
                    m_specFile->readFile(specFileName);
                }
                catch (const DataFileException& dfe) {
                    WuQMessageBox::errorOk(this, 
                                           dfe.whatString());
                    return;
                }
            }
            else {
                try {
                    m_specFile->writeFile(specFileName);
                }
                catch (const DataFileException& dfe) {
                    WuQMessageBox::errorOk(this, 
                                           dfe.whatString());
                    return;
                }
            }
        }
    }
    
    WuQDialogModal::okButtonPressed();
}

/**
 * @return Did the user choose to add files to the spec file?
 */
bool 
SpecFileCreateAddToDialog::isAddToSpecFileSelected() const
{
    return m_createAddSpecFileCheckBox->isChecked();
}


