
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

#include <algorithm>

#include <QButtonGroup>
#include <QComboBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QRadioButton>
#include <QVBoxLayout>

#define __CARET_FILE_REMOTE_DIALOG_DECLARE__
#include "CaretFileRemoteDialog.h"
#undef __CARET_FILE_REMOTE_DIALOG_DECLARE__

#include "Brain.h"
#include "BrainBrowserWindow.h"
#include "CaretAssert.h"
#include "DataFileTypeEnum.h"
#include "EnumComboBoxTemplate.h"
#include "EventDataFileRead.h"
#include "EventGraphicsUpdateAllWindows.h"
#include "EventManager.h"
#include "EventSpecFileReadDataFiles.h"
#include "EventSurfaceColoringInvalidate.h"
#include "EventUserInterfaceUpdate.h"
#include "GuiManager.h"
#include "ProgressReportingDialog.h"
#include "SpecFile.h"
#include "WuQMessageBox.h"
#include "WuQWidgetObjectGroup.h"
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
    QWidget* locationWidget = createLocationWidget();
    QWidget* loginWidget    = createLoginWidget();
    
    createAndLoadStandardData();
    
    QWidget* controlsWidget = new QWidget();
    QVBoxLayout* controlsLayout = new QVBoxLayout(controlsWidget);
    controlsLayout->addWidget(locationWidget);
    controlsLayout->addWidget(loginWidget, 0, Qt::AlignCenter);
    WuQtUtilities::setLayoutMargins(controlsLayout, 4, 2);

    setCentralWidget(controlsWidget);
   
    /*
     * Restore previous selections
     */
    QRadioButton* defaultRadioButton = NULL;
    
    m_customUrlLineEdit->setText(s_previousSelections.m_customURL);
    m_customUrlFileTypeComboBox->setSelectedItem<DataFileTypeEnum, DataFileTypeEnum::Enum>(s_previousSelections.m_customDataFileType);
    m_standardFileComboBox->setCurrentIndex(s_previousSelections.m_standardFileComboBoxIndex);
    m_usernameLineEdit->setText(s_previousSelections.m_username);
    m_passwordLineEdit->setText(s_previousSelections.m_password);
    if (m_locationCustomRadioButton->text() == s_previousSelections.m_radioButtonText) {
        defaultRadioButton = m_locationCustomRadioButton;
    }
    else if (m_locationStandardRadioButton->text() == s_previousSelections.m_radioButtonText) {
        defaultRadioButton = m_locationStandardRadioButton;
    }

    if (defaultRadioButton != NULL) {
        defaultRadioButton->setChecked(true);
    }
    locationSourceRadioButtonClicked(defaultRadioButton);
}

/**
 * Destructor.
 */
CaretFileRemoteDialog::~CaretFileRemoteDialog()
{
    
}

/**
 * @return The location widget.
 */
QWidget*
CaretFileRemoteDialog::createLocationWidget()
{
    QStringList filenameFilterList;
    std::vector<DataFileTypeEnum::Enum> dataFileTypes;
    DataFileTypeEnum::getAllEnums(dataFileTypes);
    
    m_locationCustomRadioButton = new QRadioButton("Custom");
    m_locationStandardRadioButton = new QRadioButton("Standard");
    
    QButtonGroup* locationButtonGroup = new QButtonGroup(this);
    locationButtonGroup->addButton(m_locationCustomRadioButton);
    locationButtonGroup->addButton(m_locationStandardRadioButton);
    QObject::connect(locationButtonGroup, SIGNAL(buttonClicked(QAbstractButton*)),
                     this, SLOT(locationSourceRadioButtonClicked(QAbstractButton*)));
    
    QLabel* customUrlLabel = new QLabel("URL: ");
    QLabel* customUrlTypeLabel = new QLabel("Type: ");
    m_customUrlLineEdit = new QLineEdit();
    m_customUrlLineEdit->setMinimumWidth(400);
    
    m_customUrlFileTypeComboBox = new EnumComboBoxTemplate(this);
    m_customUrlFileTypeComboBox->setup<DataFileTypeEnum, DataFileTypeEnum::Enum>();
    m_customUrlFileTypeComboBox->setSelectedItem<DataFileTypeEnum, DataFileTypeEnum::Enum>(DataFileTypeEnum::CONNECTIVITY_DENSE_SCALAR);
    
    m_standardFileComboBox = new QComboBox();
    
    QGroupBox* locationGroupBox = new QGroupBox("Location");
    QGridLayout* locationGridLayout = new QGridLayout(locationGroupBox);
    locationGridLayout->setColumnStretch(0, 0);
    locationGridLayout->setColumnStretch(1, 0);
    locationGridLayout->setColumnStretch(2, 100);
    int row = locationGridLayout->rowCount();
    locationGridLayout->addWidget(m_locationCustomRadioButton,
                                  row, 0, 2, 1);
    locationGridLayout->addWidget(customUrlLabel,
                                  row, 1);
    locationGridLayout->addWidget(m_customUrlLineEdit,
                                  row, 2);
    row++;
    
    locationGridLayout->addWidget(customUrlTypeLabel,
                                  row, 1);
    locationGridLayout->addWidget(m_customUrlFileTypeComboBox->getWidget(),
                                  row, 2);
    row++;
    
    locationGridLayout->addWidget(WuQtUtilities::createHorizontalLineWidget(),
                                  row, 0,
                                  1, 3);
    
    row++;
    locationGridLayout->addWidget(m_locationStandardRadioButton,
                                  row, 0, 1, 2);
    locationGridLayout->addWidget(m_standardFileComboBox,
                                  row, 2);
    
    m_customWidgetGroup = new WuQWidgetObjectGroup(this);
    m_customWidgetGroup->add(customUrlLabel);
    m_customWidgetGroup->add(customUrlTypeLabel);
    m_customWidgetGroup->add(m_customUrlLineEdit);
    m_customWidgetGroup->add(m_customUrlFileTypeComboBox);

    m_standardWidgetGroup = new WuQWidgetObjectGroup(this);
    m_standardWidgetGroup->add(m_standardFileComboBox);
    
    QObject::connect(m_customUrlLineEdit, SIGNAL(textEdited(const QString&)),
                     this, SLOT(selectCustomRadioButton()));
    QObject::connect(m_customUrlFileTypeComboBox, SIGNAL(itemActivated()),
                     this, SLOT(selectCustomRadioButton()));
    
    QObject::connect(m_standardFileComboBox, SIGNAL(activated(int)),
                     this, SLOT(selectStandardRadioButton()));
    
    return locationGroupBox;
}

/**
 * To select standard radio button.
 */
void
CaretFileRemoteDialog::selectStandardRadioButton()
{
    m_locationStandardRadioButton->setChecked(true);
    locationSourceRadioButtonClicked(m_locationStandardRadioButton);
}

/**
 * To select custom radio button.
 */
void
CaretFileRemoteDialog::selectCustomRadioButton()
{
    m_locationCustomRadioButton->setChecked(true);
    locationSourceRadioButtonClicked(m_locationCustomRadioButton);
}

/**
 * Called when a location source radio button is clicked.
 */
void
CaretFileRemoteDialog::locationSourceRadioButtonClicked(QAbstractButton* button)
{
    bool customEnabled   = false;
    bool standardEnabled = false;
    bool okButtonEnabled = true;
    
    if (button == m_locationCustomRadioButton) {
        customEnabled = true;
    }
    else if (button == m_locationStandardRadioButton) {
        standardEnabled = true;
    }
    else {
        okButtonEnabled = false;
    }
    
//    m_customWidgetGroup->setEnabled(customEnabled);
//    m_standardWidgetGroup->setEnabled(standardEnabled);
    
    getDialogButtonBox()->button(QDialogButtonBox::Ok)->setEnabled(okButtonEnabled);
}

/**
 * @return The login widget.
 */
QWidget*
CaretFileRemoteDialog::createLoginWidget()
{
    QLabel* usernameLabel = new QLabel("Username: ");
    m_usernameLineEdit = new QLineEdit();
    m_usernameLineEdit->setFixedWidth(200);
    
    QLabel* passwordLabel = new QLabel("Password: ");
    m_passwordLineEdit = new QLineEdit();
    m_passwordLineEdit->setFixedWidth(200);
    m_passwordLineEdit->setEchoMode(QLineEdit::Password);
    
    QGroupBox* loginGroupBox = new QGroupBox("Login");
    QGridLayout* loginGridLayout = new QGridLayout(loginGroupBox);
    loginGridLayout->setColumnStretch(0, 0);
    loginGridLayout->setColumnStretch(1, 100);
    loginGridLayout->addWidget(usernameLabel, 0, 0);
    loginGridLayout->addWidget(m_usernameLineEdit, 0, 1);
    loginGridLayout->addWidget(passwordLabel, 1, 0);
    loginGridLayout->addWidget(m_passwordLineEdit, 1, 1);

    return loginGroupBox;
}

/**
 * Create and load the standard data.
 */
void
CaretFileRemoteDialog::createAndLoadStandardData()
{
    m_standardFileComboBox->blockSignals(true);
    
    m_standardData.push_back(StandardData("Pilot1 Average Dense Connectome",
                                          "https://db.humanconnectome.org/data/services/cifti-average?searchID=PILOT1_AVG_xnat:subjectData",
                                          DataFileTypeEnum::CONNECTIVITY_DENSE));
    m_standardData.push_back(StandardData("New URL from Kevin",
                                          "http://hcpx-demo.humanconnectome.org/spring/cifti-average?resource=HCP_Q1:Q1:Demo_HCP_unrelated20_FunctionalConnectivity_mgt-regression",
                                          DataFileTypeEnum::CONNECTIVITY_DENSE));
    
    const int numStandardData = static_cast<int>(m_standardData.size());
    for (int i = 0; i < numStandardData; i++) {
        m_standardFileComboBox->addItem(m_standardData[i].m_userFriendlyName,
                                        qVariantFromValue(i));
    }

    m_standardFileComboBox->blockSignals(false);
}


/**
 * Called when OK button is pressed.
 */
void
CaretFileRemoteDialog::okButtonClicked()
{
    bool customSelected = false;
    if (m_locationCustomRadioButton->isChecked()) {
        customSelected = true;
    }
    else if (m_locationStandardRadioButton->isChecked()) {
        customSelected = false;
    }
    else {
        CaretAssert(0);  // OK button only enabled if radio button selected
    }
    
    AString dataFileURL;
    DataFileTypeEnum::Enum dataFileType = DataFileTypeEnum::UNKNOWN;
    
    if (customSelected) {
        dataFileURL = m_customUrlLineEdit->text().trimmed();
        dataFileType = m_customUrlFileTypeComboBox->getSelectedItem<DataFileTypeEnum, DataFileTypeEnum::Enum>();

        s_previousSelections.m_customURL = dataFileURL;
        s_previousSelections.m_customDataFileType = dataFileType;
        s_previousSelections.m_radioButtonText = m_locationCustomRadioButton->text();
    }
    else {
        const int indx = m_standardFileComboBox->currentIndex();
        if (indx >= 0) {
            dataFileURL = m_standardData[indx].m_locationUrl;
            dataFileType = m_standardData[indx].m_dataFileType;
        }
        
        s_previousSelections.m_standardFileComboBoxIndex = indx;
        s_previousSelections.m_radioButtonText = m_locationStandardRadioButton->text();
    }

    const AString username = m_usernameLineEdit->text().trimmed();
    const AString password = m_passwordLineEdit->text().trimmed();
    
    s_previousSelections.m_username = username;
    s_previousSelections.m_password = password;
    
    BrainBrowserWindow* browserWindow = GuiManager::get()->getActiveBrowserWindow();
    std::vector<AString> files;
    files.push_back(dataFileURL);
    std::vector<DataFileTypeEnum::Enum> dataFileTypes;
    dataFileTypes.push_back(dataFileType);
    
    const bool successFlag = browserWindow->loadFilesFromNetwork(this,
                                                                 files,
                                                                 dataFileTypes,
                                                                 username,
                                                                 password);
    
    if (successFlag) {
        WuQDialogModal::okButtonClicked();
    }
}

