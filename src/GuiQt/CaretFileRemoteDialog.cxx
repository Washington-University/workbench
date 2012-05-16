
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

#include <QComboBox>
#include <QLabel>
#include <QLineEdit>
#include <QGridLayout>

#define __CARET_FILE_REMOTE_DIALOG_DECLARE__
#include "CaretFileRemoteDialog.h"
#undef __CARET_FILE_REMOTE_DIALOG_DECLARE__

#include "Brain.h"
#include "DataFileTypeEnum.h"
#include "EventDataFileRead.h"
#include "EventGraphicsUpdateAllWindows.h"
#include "EventManager.h"
#include "EventSurfaceColoringInvalidate.h"
#include "EventUserInterfaceUpdate.h"
#include "GuiManager.h"
#include "WuQMessageBox.h"
#include "WuQtUtilities.h"

using namespace caret;
    
/**
 * \class caret::CaretFileRemoteDialog 
 * \brief Open a remote data file
 *
 */

/**
 * Constructor.
 */
CaretFileRemoteDialog::CaretFileRemoteDialog(QWidget* parent)
: WuQDialogModal("Open Remote File",
                 parent)
{
    QStringList filenameFilterList;
    std::vector<DataFileTypeEnum::Enum> dataFileTypes;
    DataFileTypeEnum::getAllConnectivityEnums(dataFileTypes);
    
    QLabel* urlLabel = new QLabel("URL: ");
    m_urlLineEdit = new QLineEdit();
    m_urlLineEdit->setText(CaretFileRemoteDialog::previousNetworkFileName);
    m_urlLineEdit->setMinimumWidth(500);
    
    int defaultFileTypeComboBoxIndex = -1;
    QLabel* fileTypeLabel = new QLabel("File Type: ");
    m_fileTypeComboBox = new QComboBox();
    const int numDataFileTypes = static_cast<int>(dataFileTypes.size());
    for (int idft = 0; idft < numDataFileTypes; idft++) {
        const DataFileTypeEnum::Enum dft = dataFileTypes[idft];
        if (dft == CaretFileRemoteDialog::previousNetworkDataFileType) {
            defaultFileTypeComboBoxIndex = idft;
        }
        m_fileTypeComboBox->addItem(DataFileTypeEnum::toGuiName(dft),
                                  (int)DataFileTypeEnum::toIntegerCode(dft));
    }
    
    QLabel* usernameLabel = new QLabel("Username: ");
    m_usernameLineEdit = new QLineEdit();
    m_usernameLineEdit->setFixedWidth(200);
    m_usernameLineEdit->setText(CaretFileRemoteDialog::previousNetworkUsername);
    
    QLabel* passwordLabel = new QLabel("Password: ");
    m_passwordLineEdit = new QLineEdit();
    m_passwordLineEdit->setFixedWidth(200);
    m_passwordLineEdit->setEchoMode(QLineEdit::Password);
    m_passwordLineEdit->setText(CaretFileRemoteDialog::previousNetworkPassword);
    
    QWidget* controlsWidget = new QWidget();
    QGridLayout* controlsLayout = new QGridLayout(controlsWidget);
    WuQtUtilities::setLayoutMargins(controlsLayout, 4, 2);
    controlsLayout->addWidget(urlLabel, 0, 0);
    controlsLayout->addWidget(m_urlLineEdit, 0, 1);
    controlsLayout->addWidget(fileTypeLabel, 1, 0);
    controlsLayout->addWidget(m_fileTypeComboBox, 1, 1, Qt::AlignLeft);
    controlsLayout->addWidget(usernameLabel, 2, 0);
    controlsLayout->addWidget(m_usernameLineEdit, 2, 1, Qt::AlignLeft);
    controlsLayout->addWidget(passwordLabel, 3, 0);
    controlsLayout->addWidget(m_passwordLineEdit, 3, 1, Qt::AlignLeft);

    setCentralWidget(controlsWidget);
    
    m_usernameLineEdit->setText("wbuser");
    m_passwordLineEdit->setText("hcpWb0512");
    m_urlLineEdit->setText("https://db.humanconnectome.org/data/services/cifti-average?searchID=PILOT1_AVG_xnat:subjectData");
}

/**
 * Destructor.
 */
CaretFileRemoteDialog::~CaretFileRemoteDialog()
{
    
}

/**
 * Called when OK button is pressed.
 */
void 
CaretFileRemoteDialog::okButtonPressed()
{
    const AString filename = m_urlLineEdit->text().trimmed();
    
    DataFileTypeEnum::Enum dataFileType = DataFileTypeEnum::UNKNOWN;
    const int fileTypeComboBoxIndex = m_fileTypeComboBox->currentIndex();
    if (fileTypeComboBoxIndex >= 0) {
        const int dataTypeIntegerCode = m_fileTypeComboBox->itemData(fileTypeComboBoxIndex).toInt();
        dataFileType = DataFileTypeEnum::fromIntegerCode(dataTypeIntegerCode, 
                                                         NULL);
    }
    
    CaretFileRemoteDialog::previousNetworkDataFileType = dataFileType;
    CaretFileRemoteDialog::previousNetworkFileName = filename;
    CaretFileRemoteDialog::previousNetworkUsername = m_usernameLineEdit->text().trimmed();
    CaretFileRemoteDialog::previousNetworkPassword = m_passwordLineEdit->text().trimmed();
    
    Brain* brain = GuiManager::get()->getBrain();
    
    EventDataFileRead readFileEvent(brain,
                                    CaretFileRemoteDialog::previousNetworkDataFileType,
                                    CaretFileRemoteDialog::previousNetworkFileName,
                                    false);
    readFileEvent.setUsernameAndPassword(CaretFileRemoteDialog::previousNetworkUsername,
                                         CaretFileRemoteDialog::previousNetworkPassword);

    bool isError = false;
    EventManager::get()->sendEvent(readFileEvent.getPointer());
    if (readFileEvent.isError()) {
        WuQMessageBox::errorOk(this, 
                               readFileEvent.getErrorMessage());  
        isError = true;
    }    

    EventManager::get()->sendEvent(EventSurfaceColoringInvalidate().getPointer());
    EventManager::get()->sendEvent(EventUserInterfaceUpdate().getPointer());
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
    
    if (isError) {
        return;
    }
    
    WuQDialogModal::okButtonPressed();
}

