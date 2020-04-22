
/*LICENSE_START*/
/*
 *  Copyright (C) 2014  Washington University School of Medicine
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

#include <algorithm>

#include <QButtonGroup>
#include <QCheckBox>
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
#include "CaretPreferences.h"
#include "DataFileTypeEnum.h"
#include "EnumComboBoxTemplate.h"
#include "EventDataFileRead.h"
#include "EventGraphicsUpdateAllWindows.h"
#include "EventManager.h"
#include "EventUserInterfaceUpdate.h"
#include "GuiManager.h"
#include "ProgressReportingDialog.h"
#include "SessionManager.h"
#include "SpecFile.h"
#include "UsernamePasswordWidget.h"
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
: WuQDialogModal("Open Location",
                 parent)
{
    QWidget* locationWidget = createLocationWidget();
    m_usernamePasswordWidget = new UsernamePasswordWidget();
    
    createAndLoadStandardData();
    
    QWidget* controlsWidget = new QWidget();
    QVBoxLayout* controlsLayout = new QVBoxLayout(controlsWidget);
    controlsLayout->addWidget(locationWidget);
    controlsLayout->addWidget(m_usernamePasswordWidget, 0, Qt::AlignCenter);
    WuQtUtilities::setLayoutSpacingAndMargins(controlsLayout, 4, 2);

    setCentralWidget(controlsWidget,
                     WuQDialog::SCROLL_AREA_NEVER);
   
    /*
     * Restore previous selections
     */
    QRadioButton* defaultRadioButton = NULL;
    
    if (s_previousSelections.m_firstTime) {
        s_previousSelections.m_firstTime = false;
    }
    
    m_customUrlLineEdit->setText(s_previousSelections.m_customURL);
    m_customUrlFileTypeComboBox->setSelectedItem<DataFileTypeEnum, DataFileTypeEnum::Enum>(s_previousSelections.m_customDataFileType);
    m_standardFileComboBox->setCurrentIndex(s_previousSelections.m_standardFileComboBoxIndex);
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
    DataFileTypeEnum::getAllEnums(dataFileTypes,
                                  DataFileTypeEnum::OPTIONS_NONE);
    
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
    m_customUrlFileTypeComboBox->setupWithItems<DataFileTypeEnum, DataFileTypeEnum::Enum>(dataFileTypes);
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
    bool okButtonEnabled = true;
    
    if (button == m_locationCustomRadioButton) {
        /* nothing */
    }
    else if (button == m_locationStandardRadioButton) {
        /* nothing */
    }
    else {
        okButtonEnabled = false;
    }
    
    getDialogButtonBox()->button(QDialogButtonBox::Ok)->setEnabled(okButtonEnabled);
}

/**
 * Create and load the standard data.
 */
void
CaretFileRemoteDialog::createAndLoadStandardData()
{
    m_standardFileComboBox->blockSignals(true);
    
    m_standardData.push_back(StandardData("HCP-Q1 Correlation with Mean Gray Regression (Avg 20)",
                                          "https://db.humanconnectome.org/spring/cifti-average?resource=HCP_Q1:Q1:Demo_HCP_unrelated20_FunctionalConnectivity_mgt-regression",
                                          DataFileTypeEnum::CONNECTIVITY_DENSE));
    m_standardData.push_back(StandardData("HCP-Q1 Full Correlation (Avg 20)",
                                          "https://db.humanconnectome.org/spring/cifti-average?resource=HCP_Q1:Q1:Demo_HCP_unrelated20_FunctionalConnectivity_FullCorrel",
                                          DataFileTypeEnum::CONNECTIVITY_DENSE));
    m_standardData.push_back(StandardData("Pilot1 Average Dense Connectome",
                                          "https://db.humanconnectome.org/data/services/cifti-average?searchID=PILOT1_AVG_xnat:subjectData",
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
    AString username;
    AString password;
    m_usernamePasswordWidget->getUsernameAndPassword(username,
                                                     password);
    
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

