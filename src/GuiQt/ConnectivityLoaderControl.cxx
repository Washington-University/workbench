
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

#define __CONNECTIVITY_LOADER_CONTROL_DECLARE__
#include "ConnectivityLoaderControl.h"
#undef __CONNECTIVITY_LOADER_CONTROL_DECLARE__

#include <QBoxLayout>
#include <QButtonGroup>
#include <QCheckBox>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QToolButton>

#include "Brain.h"
#include "CaretAssert.h"
#include "ConnectivityLoaderFile.h"
#include "ConnectivityLoaderManager.h"
#include "EventGraphicsUpdateAllWindows.h"
#include "EventSurfaceColoringInvalidate.h"
#include "EventManager.h"
#include "GuiManager.h"
#include "WuQDialogModal.h"
#include "WuQFileDialog.h"
#include "WuQtUtilities.h"
#include "WuQWidgetObjectGroup.h"

using namespace caret;


static const int COLUMN_SELECTOR  = 0;
static const int COLUMN_FILE      = 1;
static const int COLUMN_FILE_TYPE = 2;
static const int COLUMN_TIME_CHECKBOX = 3;
static const int COLUMN_TIME_SPINBOX = 4;
static const int COLUMN_TIME_ANIMATE = 5;
static const int COLUMN_FILE_BUTTON = 6;
static const int COLUMN_NETWORK_BUTTON = 7;
static const int COLUMN_REMOVE    = 8;
    
/**
 * \class ConnectivityLoaderControl 
 * \brief User interface for connectivity selection.
 *
 * Creates a user-interface for connectivity selection 
 * including adding and removing connectivity loaders.
 */
/**
 * Constructor.
 */
ConnectivityLoaderControl::ConnectivityLoaderControl(QWidget* parent)
: QWidget(parent)
{
    this->animateButtonsGroup = new QButtonGroup();
    QObject::connect(this->animateButtonsGroup, SIGNAL(buttonClicked(QAbstractButton*)),
                     this, SLOT(animateButtonPressed(QAbstractButton*)));
    
    this->fileButtonsGroup = new QButtonGroup();
    QObject::connect(this->fileButtonsGroup, SIGNAL(buttonClicked(QAbstractButton*)),
                     this, SLOT(fileButtonPressed(QAbstractButton*)));
    
    this->networkButtonsGroup = new QButtonGroup();
    QObject::connect(this->networkButtonsGroup, SIGNAL(buttonClicked(QAbstractButton*)),
                     this, SLOT(networkButtonPressed(QAbstractButton*)));

    this->removeButtonsGroup = new QButtonGroup();
    QObject::connect(this->removeButtonsGroup, SIGNAL(buttonClicked(QAbstractButton*)),
                     this, SLOT(removeButtonPressed(QAbstractButton*)));
    
    QLabel* selectorLabel = new QLabel("Selector");
    QLabel* fileLabel = new QLabel("File");
    QLabel* animateLabel = new QLabel("Timepoint");
    QLabel* sourceLabel = new QLabel("Data Source");
    QLabel* fileTypeLabel = new QLabel("File Type");
    QLabel* removeLabel = new QLabel("Remove");
    
    this->loaderLayout = new QGridLayout();
    this->loaderLayout->addWidget(selectorLabel, 0, COLUMN_SELECTOR);
    this->loaderLayout->addWidget(fileLabel, 0, COLUMN_FILE);
    this->loaderLayout->addWidget(fileTypeLabel, 0, COLUMN_FILE_TYPE);
    this->loaderLayout->addWidget(animateLabel, 0, COLUMN_TIME_CHECKBOX, 1, 3);
    this->loaderLayout->addWidget(sourceLabel, 0, COLUMN_FILE_BUTTON, 1, 2);
    this->loaderLayout->addWidget(removeLabel, 0, COLUMN_REMOVE);

    this->loaderLayout->setColumnStretch(COLUMN_FILE, 100);
    
    QPushButton* addPushButton = new QPushButton("Add Connectivity Loader");
    addPushButton->setFixedSize(addPushButton->sizeHint());
    QObject::connect(addPushButton, SIGNAL(clicked()),
                     this, SLOT(addConnectivityLoader()));
    
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addLayout(this->loaderLayout);
    layout->addWidget(addPushButton);
    layout->addStretch();
}

/**
 * Destructor.
 */
ConnectivityLoaderControl::~ConnectivityLoaderControl()
{
    
}

/**
 * Update the control.
 */
void
ConnectivityLoaderControl::updateControl()
{
    Brain* brain = GuiManager::get()->getBrain();
    ConnectivityLoaderManager* manager = brain->getConnectivityLoaderManager();
    int32_t numLoaderWidgets = static_cast<int32_t>(this->loaderNumberLabels.size());
    
    /*
     * Create new rows, as needed
     */
    const int32_t numberOfConnectivityLoaders = manager->getNumberOfConnectivityLoaderFiles();
    for (int32_t i = 0; i < numberOfConnectivityLoaders; i++) {
        if (i >= numLoaderWidgets) {
            QLabel* numberLabel = new QLabel(AString::number(i + 1));
            
            QLineEdit* fileNameLineEdit = new QLineEdit();
            fileNameLineEdit->setReadOnly(true);
            
            QLabel* fileTypeLabel = new QLabel();
            
            QToolButton* animateButton = new QToolButton();
            animateButton->setText("Start");
            WuQtUtilities::setToolTipAndStatusTip(animateButton,
                                                  "Animate throught timepoints");
            
            QToolButton* fileButton = new QToolButton();
            fileButton->setText("File");
            WuQtUtilities::setToolTipAndStatusTip(fileButton,
                                                  "Open a connectivity file from disk");
            
            QToolButton* networkButton = new QToolButton();
            networkButton->setText("Web");
            WuQtUtilities::setToolTipAndStatusTip(networkButton,
                                                  "Open a connectivity file on the network");
            
            QToolButton* removeButton = new QToolButton();
            removeButton->setText("X");
            
            QCheckBox* timeCheckBox = new QCheckBox(" ");
            QObject::connect(timeCheckBox, SIGNAL(stateChanged(int)),
                             this, SLOT(showTimeGraphCheckBoxesStateChanged(int)));
            WuQtUtilities::setToolTipAndStatusTip(timeCheckBox,
                                                  "Display time-series chart for selected brainordinates");
            
            QDoubleSpinBox* timeSpinBox = new QDoubleSpinBox();
            timeSpinBox->setMinimum(0);
            timeSpinBox->setSingleStep(1.0);
            QObject::connect(timeSpinBox, SIGNAL(valueChanged(double)),
                             this, SLOT(timeSpinBoxesValueChanged(double)));
            WuQtUtilities::setToolTipAndStatusTip(timeSpinBox,
                                                  "Select timepoint for display on brainordinates");
            
            WuQWidgetObjectGroup* widgetGroup = new WuQWidgetObjectGroup(this);
            widgetGroup = new WuQWidgetObjectGroup(this);
            widgetGroup->add(numberLabel);
            widgetGroup->add(animateButton);
            widgetGroup->add(fileNameLineEdit);
            widgetGroup->add(fileTypeLabel);
            widgetGroup->add(fileButton);
            widgetGroup->add(networkButton);
            widgetGroup->add(removeButton);
            widgetGroup->add(timeCheckBox);
            widgetGroup->add(timeSpinBox);
            this->rowWidgetGroups.push_back(widgetGroup);
            
            const int row = this->loaderLayout->rowCount();
            this->loaderLayout->addWidget(numberLabel, row, COLUMN_SELECTOR);
            this->loaderLayout->addWidget(fileNameLineEdit, row, COLUMN_FILE);
            this->loaderLayout->addWidget(timeCheckBox, row, COLUMN_TIME_CHECKBOX);
            this->loaderLayout->addWidget(timeSpinBox, row, COLUMN_TIME_SPINBOX);
            this->loaderLayout->addWidget(animateButton, row, COLUMN_TIME_ANIMATE);
            this->loaderLayout->addWidget(fileTypeLabel, row, COLUMN_FILE_TYPE);
            this->loaderLayout->addWidget(fileButton, row, COLUMN_FILE_BUTTON);
            this->loaderLayout->addWidget(networkButton, row, COLUMN_NETWORK_BUTTON);
            this->loaderLayout->addWidget(removeButton, row, COLUMN_REMOVE);
            
            this->animateButtonsGroup->addButton(animateButton);
            this->fileButtonsGroup->addButton(fileButton);
            this->networkButtonsGroup->addButton(networkButton);
            this->removeButtonsGroup->addButton(removeButton);
            
            this->animateButtons.push_back(animateButton);
            this->loaderNumberLabels.push_back(numberLabel);
            this->fileNameLineEdits.push_back(fileNameLineEdit);
            this->fileTypeLabels.push_back(fileTypeLabel);
            this->fileButtons.push_back(fileButton);
            this->networkButtons.push_back(networkButton);

            this->removeButtons.push_back(removeButton);
            this->timeSpinBoxes.push_back(timeSpinBox);
            this->showTimeGraphCheckBoxes.push_back(timeCheckBox);

        }
    }
    
    /*
     * Update rows
     */
    numLoaderWidgets = static_cast<int32_t>(this->loaderNumberLabels.size());
    for (int32_t i = 0; i < numLoaderWidgets; i++) {
        ConnectivityLoaderFile* clf = NULL;
        if (i < numberOfConnectivityLoaders) {
            clf = manager->getConnectivityLoaderFile(i);
        }

        if (clf != NULL) {
            this->fileNameLineEdits[i]->setText(clf->getFileName());
            this->fileTypeLabels[i]->setText(clf->getCiftiTypeName());
            if (clf->isDenseTimeSeries()) {
                this->showTimeGraphCheckBoxes[i]->setChecked(clf->isTimeSeriesGraphEnabled());
                this->showTimeGraphCheckBoxes[i]->setEnabled(true);
                this->timeSpinBoxes[i]->setEnabled(true);
                this->timeSpinBoxes[i]->setSingleStep(clf->getTimeStep());
                this->animateButtons[i]->setEnabled(true);
            }
            else {
                this->showTimeGraphCheckBoxes[i]->setEnabled(false);
                this->timeSpinBoxes[i]->setEnabled(false);
                this->animateButtons[i]->setEnabled(false);
            }
            
            this->rowWidgetGroups[i]->setVisible(true);
        }
        else {
            this->rowWidgetGroups[i]->setVisible(false);
        }
    }
    
    //this->adjustSize();
}

/**
 * Called when an Animate button is clicked.
 * @param button
 *   Animate button that was clicked.
 */
void 
ConnectivityLoaderControl::animateButtonPressed(QAbstractButton* button)
{
    int32_t fileIndex = -1;
    for (int32_t i = 0; i < static_cast<int32_t>(this->animateButtons.size()); i++) {
        if (this->animateButtons[i] == button) {
            fileIndex = i;
        }
    }
    CaretAssert(fileIndex >= 0);
    
    animators[fileIndex]->toggleAnimation();
    std::cout << "Animate button " << fileIndex << " was pressed." << std::endl;
}

/**
 * Called when an File button is clicked.
 * @param button
 *   File button that was clicked.
 */
void 
ConnectivityLoaderControl::fileButtonPressed(QAbstractButton* button)
{
    int32_t fileIndex = -1;
    for (int32_t i = 0; i < static_cast<int32_t>(this->fileButtons.size()); i++) {
        if (this->fileButtons[i] == button) {
            fileIndex = i;
        }
    }
    CaretAssert(fileIndex >= 0);
    
    /*
     * CIFTI filters.
     */
    QStringList filenameFilterList;
    std::vector<DataFileTypeEnum::Enum> connectivityEnums;
    DataFileTypeEnum::getAllConnectivityEnums(connectivityEnums);
    for (int32_t i = 0; i < static_cast<int32_t>(connectivityEnums.size()); i++) {
        filenameFilterList.append(DataFileTypeEnum::toQFileDialogFilter(connectivityEnums[i]));
    }
    
    /*
     * Setup file selection dialog.
     */
    WuQFileDialog fd(this);
    fd.setAcceptMode(WuQFileDialog::AcceptOpen);
    fd.setNameFilters(filenameFilterList);
    fd.setFileMode(WuQFileDialog::ExistingFile);
    fd.setViewMode(WuQFileDialog::List);
    if (this->previousCiftiFileTypeFilter.isEmpty() == false) {
        fd.selectFilter(this->previousCiftiFileTypeFilter);
    }
    
    if (fd.exec()) {
        QStringList selectedFiles = fd.selectedFiles();
        this->previousCiftiFileTypeFilter = fd.selectedFilter();
        
        /*
         * Load each file.
         */
        QStringListIterator nameIter(selectedFiles);
        if (nameIter.hasNext()) {
            AString name = nameIter.next();
            ConnectivityLoaderManager* manager = GuiManager::get()->getBrain()->getConnectivityLoaderManager();
            ConnectivityLoaderFile* loaderFile = manager->getConnectivityLoaderFile(fileIndex);
            
            try {
                loaderFile->setupLocalFile(name, 
                                  DataFileTypeEnum::fromQFileDialogFilter(this->previousCiftiFileTypeFilter, NULL));
            }
            catch (DataFileException e) {
                QMessageBox::critical(this, 
                                      "ERROR", 
                                      e.whatString());                
            }
            if(loaderFile->isDenseTimeSeries())
            {
                if((this->animators.size())>fileIndex)
                {
                    if(this->animators[fileIndex])
                    {
                        animators[fileIndex]->stop();
                        delete animators[fileIndex];
                    }
                    TimeSeriesManager *tsManager = new TimeSeriesManager(fileIndex,manager);//TODO, time series needs a handle to ConnectivityLoaderFile
                    this->animators[fileIndex] = tsManager;
                }
                else
                {
                    TimeSeriesManager *tsManager = new TimeSeriesManager(fileIndex,manager);//TODO, time series needs a handle to ConnectivityLoaderFile
                    this->animators.push_back(tsManager);
                }
            }
            else
            {
                if(this->animators.size()>fileIndex)
                {
                    if(animators[fileIndex])
                    {
                        animators[fileIndex]->stop();
                        delete animators[fileIndex];
                        animators[fileIndex] = NULL;
                    }
                }
                else
                {
                    this->animators.push_back(NULL);
                }
            }
        }        
    }
    
    this->updateControl();
}

/**
 * Called when an Network button is clicked.
 * @param button
 *   Network button that was clicked.
 */
void 
ConnectivityLoaderControl::networkButtonPressed(QAbstractButton* button)
{
    int32_t fileIndex = -1;
    for (int32_t i = 0; i < static_cast<int32_t>(this->networkButtons.size()); i++) {
        if (this->networkButtons[i] == button) {
            fileIndex = i;
        }
    }
    CaretAssert(fileIndex >= 0);
    
    ConnectivityLoaderManager* manager = GuiManager::get()->getBrain()->getConnectivityLoaderManager();
    ConnectivityLoaderFile* loaderFile = manager->getConnectivityLoaderFile(fileIndex);

    QStringList filenameFilterList;
    std::vector<DataFileTypeEnum::Enum> connectivityEnums;
    DataFileTypeEnum::getAllConnectivityEnums(connectivityEnums);
    
    QLabel* urlLabel = new QLabel("URL: ");
    QLineEdit* urlLineEdit = new QLineEdit();
    urlLineEdit->setText("https://hcp-dev01.nrg.wustl.edu/data/services/cifti-average?searchID=xs1308076465528");
    
    QLabel* typeLabel = new QLabel("Type: ");
    QComboBox* typeComboBox = new QComboBox();
    for (int32_t i = 0; i < static_cast<int32_t>(connectivityEnums.size()); i++) {
        const AString name = DataFileTypeEnum::toGuiName(connectivityEnums[i]);
        typeComboBox->addItem(name);
        typeComboBox->setItemData(i, qVariantFromValue(DataFileTypeEnum::toIntegerCode(connectivityEnums[i])));
    }
    
    QLabel* usernameLabel = new QLabel("Username: ");
    QLineEdit* usernameLineEdit = new QLineEdit();
    
    QLabel* passwordLabel = new QLabel("Password: ");
    QLineEdit* passwordLineEdit = new QLineEdit();
    passwordLineEdit->setEchoMode(QLineEdit::Password);
    
    QWidget* controlsWidget = new QWidget();
    QGridLayout* controlsLayout = new QGridLayout(controlsWidget);
    controlsLayout->addWidget(urlLabel, 0, 0);
    controlsLayout->addWidget(urlLineEdit, 0, 1);
    controlsLayout->addWidget(typeLabel, 1, 0);
    controlsLayout->addWidget(typeComboBox, 1, 1);
    controlsLayout->addWidget(usernameLabel, 2, 0);
    controlsLayout->addWidget(usernameLineEdit, 2, 1);
    controlsLayout->addWidget(passwordLabel, 3, 0);
    controlsLayout->addWidget(passwordLineEdit, 3, 1);
    
    WuQDialogModal d("Connectivity File on Web",
                     controlsWidget,
                     this);
    if (d.exec() == QDialog::Accepted) {
        const AString name = urlLineEdit->text().trimmed();
        const int comboIndex = typeComboBox->currentIndex();
        const DataFileTypeEnum::Enum dataType = 
            DataFileTypeEnum::fromIntegerCode(typeComboBox->itemData(comboIndex).toInt(), NULL);
        const AString username = usernameLineEdit->text().trimmed();
        const AString password = passwordLineEdit->text().trimmed();
        
        try {
            loaderFile->setupNetworkFile(name,
                                         dataType,
                                         username,
                                         password);
        }
        catch (DataFileException e) {
            QMessageBox::critical(this, 
                                  "ERROR", 
                                  e.whatString());                
        }

        //TODO, move boiler plate below to separate method(s)
        if(loaderFile->isDenseTimeSeries())
        {
            if(this->animators.size()>fileIndex)
            {
                if(this->animators[fileIndex])
                {
                    animators[fileIndex]->stop();
                    delete animators[fileIndex];
                }
                TimeSeriesManager *tsManager = new TimeSeriesManager(fileIndex, manager);//TODO, time series needs a handle to ConnectivityLoaderFile
                this->animators[fileIndex] = tsManager;
            }
            else
            {
                TimeSeriesManager *tsManager = new TimeSeriesManager(fileIndex, manager);//TODO, time series needs a handle to ConnectivityLoaderFile
                this->animators.push_back(tsManager);
            }
        }
        else
        {
            if(this->animators.size()>fileIndex)
            {
                if(animators[fileIndex])
                {
                    animators[fileIndex]->stop();
                    delete animators[fileIndex];
                    animators[fileIndex] = NULL;
                }
            }
            else
            {
                this->animators.push_back(NULL);
            }
        }

        this->updateControl();
    }
}

/**
 * Called when an remove button is clicked.
 * @param button
 *   Remove button that was clicked.
 */
void 
ConnectivityLoaderControl::removeButtonPressed(QAbstractButton* button)
{
    int32_t fileIndex = -1;
    for (int32_t i = 0; i < static_cast<int32_t>(this->removeButtons.size()); i++) {
        if (this->removeButtons[i] == button) {
            fileIndex = i;
        }
    }
    CaretAssert(fileIndex >= 0);
    if(animators[fileIndex])
    {
        animators[fileIndex]->stop();//removes animation thread if it exists
    }

    Brain* brain = GuiManager::get()->getBrain();
    ConnectivityLoaderManager* manager = brain->getConnectivityLoaderManager();
    manager->removeConnectivityLoaderFile(fileIndex);

    this->updateControl();
}

/**
 * Called when a time spin box value is changed.
 * @param value
 *    New value.
 */
void 
ConnectivityLoaderControl::timeSpinBoxesValueChanged(double /*value*/)
{
    ConnectivityLoaderManager* manager = GuiManager::get()->getBrain()->getConnectivityLoaderManager();
    
    bool dataLoadedFlag = false;
    
    const int32_t numberOfConnectivityLoaders = manager->getNumberOfConnectivityLoaderFiles();
    for (int32_t i = 0; i < numberOfConnectivityLoaders; i++) {
        ConnectivityLoaderFile* clf = manager->getConnectivityLoaderFile(i);
        if (manager->loadTimePointAtTime(clf, this->timeSpinBoxes[i]->value())) {
            dataLoadedFlag = true;
        }
    }
    
    if (dataLoadedFlag) {
        EventManager::get()->sendEvent(EventSurfaceColoringInvalidate().getPointer());
        EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());        
    }
}

/**
 * Called when a show graph checkbox value is changed.
 * @param state
 *    New state.
 */
void 
ConnectivityLoaderControl::showTimeGraphCheckBoxesStateChanged(int /*state*/)
{
    ConnectivityLoaderManager* manager = GuiManager::get()->getBrain()->getConnectivityLoaderManager();
    
    const int32_t numberOfConnectivityLoaders = manager->getNumberOfConnectivityLoaderFiles();
    for (int32_t i = 0; i < numberOfConnectivityLoaders; i++) {
        ConnectivityLoaderFile* clf = manager->getConnectivityLoaderFile(i);
        if (clf->isDenseTimeSeries()) {
            clf->setTimeSeriesGraphEnabled(this->showTimeGraphCheckBoxes[i]->isChecked());
        }
    }
}

/**
 * Called to add a connectivity loader.
 */
void 
ConnectivityLoaderControl::addConnectivityLoader()
{
    Brain* brain = GuiManager::get()->getBrain();
    ConnectivityLoaderManager* manager = brain->getConnectivityLoaderManager();
    manager->addConnectivityLoaderFile();
    this->updateControl();
}
