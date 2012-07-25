
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
 * @param brain
 *    Brain that contains or will contain a spec file.
 * @param mode
 *    Indicates opening or saving a file
 * @param parent
 *    Parent widget on which this dialog is displayed.
 */
SpecFileCreateAddToDialog::SpecFileCreateAddToDialog(Brain* brain,
                                                     const FileOpenSaveMode mode,
                                                     QWidget* parent)
: WuQDialogModal("Choose a Spec File",
                 parent)
{
    m_addFilesToSpecFileFlag = false;
    
    m_specFileName = brain->getSpecFileName();
    
    FileInformation fileInfo(m_specFileName);
    m_isSpecFileValid = fileInfo.exists();
    
    m_specFileNameLineEdit = new QLineEdit();
    m_specFileNameLineEdit->setMinimumWidth(500);
    m_specFileNameLineEdit->setReadOnly(true);

    QAction* fileAction = WuQtUtilities::createAction("Spec File...", 
                                                      "Choose Spec File", 
                                                      this, 
                                                      this, 
                                                      SLOT(fileButtonClicked()));
    QToolButton* fileToolButton = new QToolButton();
    fileToolButton->setDefaultAction(fileAction);
    
    QString instructionsText;
    switch (mode) {
        case MODE_OPEN:
            instructionsText = ("<html>"
                                "The checkbox labeled <B>Add Opened Data File to Spec File</B> at the "
                                "bottom of the file selection dialog "
                                "from which you selected files is checked but there is no Spec File.  You can:"
                                "<UL>"
                                "<LI> Press the <B>Spec File</B> button to use a file selection dialog to "
                                "choose an existing spec file or enter the name of a new spec file."
                                "<LI> Press the <B>Skip</B> button to load the files that you selected "
                                "without creating a Spec File."
                                "<LI> Press the <B>Cancel</B> button to cancel file loading."
                                "</UL>"
                                "</html>");
            break;
        case MODE_SAVE:
            instructionsText = ("<html>"
                                "The checkbox labeled <B>Add Saved Files to Spec File</B>box at the "
                                "bottom of the Manage and Save Loaded Files dialog is checked but "
                                "there is no Spec File.  You can:"
                                "<UL>"
                                "<LI> Press the <B>Spec File</B> button to use a file selection dialog to "
                                "choose an existing spec file or enter the name of a new spec file."
                                "<LI> Press the <B>Skip</B> button to save the files "
                                "without creating a Spec File."
                                "<LI> Press the <B>Cancel</B> button to cancel file saving."
                                "</UL>"
                                "</html>");
            break;
    }
    QLabel* instructionsLabel = new QLabel(instructionsText);
    instructionsLabel->setWordWrap(true);
    
    QWidget* widget = new QWidget();
    QGridLayout* gridLayout = new QGridLayout(widget);
    gridLayout->addWidget(instructionsLabel, 0, 0, 1, 2);
    gridLayout->addWidget(fileToolButton, 2, 0);
    gridLayout->addWidget(m_specFileNameLineEdit, 2, 1);
    setCentralWidget(widget);
    
    m_skipPushButton = addUserPushButton("Skip",
                                         QDialogButtonBox::AcceptRole);
    
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
//    CaretFileDialog fd(this);
//    fd.setAcceptMode(CaretFileDialog::AcceptSave);
//    fd.setLabelText(QFileDialog::Accept, "Choose");
//    fd.setNameFilter(DataFileTypeEnum::toQFileDialogFilter(DataFileTypeEnum::SPECIFICATION));
//    fd.setFileMode(CaretFileDialog::AnyFile);
//    fd.setViewMode(CaretFileDialog::List);
//    fd.selectNameFilter(DataFileTypeEnum::toQFileDialogFilter(DataFileTypeEnum::SPECIFICATION));
//    fd.setConfirmOverwrite(false);
//    
//    if (fd.exec() == CaretFileDialog::Accepted) {
//        if (fd.selectedFiles().empty() == false) {
//            QString filename = fd.selectedFiles().at(0);
//
//            if (filename.isEmpty() == false) {
//                const QString fileExt = ("." + DataFileTypeEnum::toFileExtension(DataFileTypeEnum::SPECIFICATION));
//                if (filename.endsWith(fileExt) == false) {
//                    filename += fileExt;
//                }
//                m_specFileNameLineEdit->setText(filename);
//            }
//        }
//    }
    
    QString filename = CaretFileDialog::getSaveFileNameDialog(DataFileTypeEnum::SPECIFICATION,
                                                              this,
                                                              "Choose Spec File",
                                                              "");
    if (filename.isEmpty() == false) {
        m_specFileNameLineEdit->setText(filename);
    }
}

/**
 * Called when an extra button is pressed.
 */
WuQDialogModal::ModalDialogUserButtonResult 
SpecFileCreateAddToDialog::userButtonPressed(QPushButton* userPushButton)
{
    if (userPushButton == m_skipPushButton) {
        return WuQDialogModal::RESULT_ACCEPT;
    }
    return WuQDialogModal::RESULT_NONE;
}


/**
 * Called the OK button is pressed.
 */
void 
SpecFileCreateAddToDialog::okButtonClicked()
{
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
                 * If spec file exists, try to read it.
                 */
                try {
                    SpecFile sf;
                    sf.readFile(specFileName);
                }
                catch (const DataFileException& dfe) {
                    WuQMessageBox::errorOk(this, 
                                           dfe.whatString());
                    return;
                }
            }
            else {
                /*
                 * Write spec file to create it
                 */
                try {
                    SpecFile sf;
                    sf.writeFile(specFileName);
                }
                catch (const DataFileException& dfe) {
                    WuQMessageBox::errorOk(this, 
                                           dfe.whatString());
                    return;
                }
            }
            
            m_addFilesToSpecFileFlag = true;
        }

    WuQDialogModal::okButtonClicked();
}

/**
 * @return Did the user choose to add files to the spec file?
 */
bool 
SpecFileCreateAddToDialog::isAddToSpecFileSelected() const
{
    return m_addFilesToSpecFileFlag;
}


