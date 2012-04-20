
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
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QToolButton>
#include <QVBoxLayout>

#define __CONNECTIVITY_MANAGER_VIEW_CONTROLLER_DECLARE__
#include "ConnectivityManagerViewController.h"
#undef __CONNECTIVITY_MANAGER_VIEW_CONTROLLER_DECLARE__

#include "Brain.h"
#include "CaretAssert.h"
#include "ConnectivityLoaderFile.h"
#include "ConnectivityTimeSeriesViewController.h"
#include "ConnectivityDenseViewController.h"
#include "EventDataFileRead.h"
#include "EventManager.h"
#include "EventToolBoxUpdate.h"
#include "EventUserInterfaceUpdate.h"
#include "GuiManager.h"
#include "WuQDialogModal.h"
#include "WuQtUtilities.h"

using namespace caret;


    
/**
 * \class caret::ConnectivityManagerViewController 
 * \brief View-Controller for the ConnectivityLoaderManager
 *
 */
/**
 * Constructor.
 */
ConnectivityManagerViewController::ConnectivityManagerViewController(const Qt::Orientation orientation,
                                                                     const int32_t browserWindowIndex,
                                                                     const DataFileTypeEnum::Enum connectivityFileType,                                                                     
                                                                     QWidget* parent)
: QWidget(parent)
{
    this->orientation = orientation;
    this->browserWindowIndex = browserWindowIndex;
    this->connectivityFileType = connectivityFileType;
    
    this->viewControllerGridLayout = NULL;
    
    switch (this->connectivityFileType) {
        case DataFileTypeEnum::CONNECTIVITY_DENSE:
            this->viewControllerGridLayout = ConnectivityDenseViewController::createGridLayout(orientation);
            break;
        case DataFileTypeEnum::CONNECTIVITY_DENSE_TIME_SERIES:
            this->viewControllerGridLayout = ConnectivityTimeSeriesViewController::createGridLayout(orientation);
            break;
        default:
            CaretAssertMessage(0, ("Unrecognized connectivity file type "
                                   + DataFileTypeEnum::toName(this->connectivityFileType)));
            return;
            break;
    }
    
    QAction* loadFileFromWebAction = 
    WuQtUtilities::createAction("Web",
                                "Load a file from the Web",
                                this,
                                this,
                                SLOT(processLoadFileFromWeb()));
    QToolButton* webToolButton = new QToolButton();
    webToolButton->setDefaultAction(loadFileFromWebAction);
    
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addLayout(this->viewControllerGridLayout);
    layout->addWidget(webToolButton, 0, Qt::AlignLeft);
    layout->addStretch();    

    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_USER_INTERFACE_UPDATE);
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_TOOLBOX_UPDATE);
}

/**
 * Destructor.
 */
ConnectivityManagerViewController::~ConnectivityManagerViewController()
{    
    EventManager::get()->removeAllEventsFromListener(this);
}

void 
ConnectivityManagerViewController::processLoadFileFromWeb()
{
    QStringList filenameFilterList;
    std::vector<DataFileTypeEnum::Enum> connectivityEnums;
    DataFileTypeEnum::getAllConnectivityEnums(connectivityEnums);
    
    QLabel* urlLabel = new QLabel("URL: ");
    QLineEdit* urlLineEdit = new QLineEdit();
    urlLineEdit->setText("");
    
    QLabel* usernameLabel = new QLabel("Username: ");
    QLineEdit* usernameLineEdit = new QLineEdit();
    usernameLineEdit->setText(ConnectivityManagerViewController::previousNetworkUsername);
    
    QLabel* passwordLabel = new QLabel("Password: ");
    QLineEdit* passwordLineEdit = new QLineEdit();
    passwordLineEdit->setEchoMode(QLineEdit::Password);
    passwordLineEdit->setText(ConnectivityManagerViewController::previousNetworkPassword);
    
    QWidget* controlsWidget = new QWidget();
    QGridLayout* controlsLayout = new QGridLayout(controlsWidget);
    WuQtUtilities::setLayoutMargins(controlsLayout, 4, 2);
    controlsLayout->addWidget(urlLabel, 0, 0);
    controlsLayout->addWidget(urlLineEdit, 0, 1);
    controlsLayout->addWidget(usernameLabel, 1, 0);
    controlsLayout->addWidget(usernameLineEdit, 1, 1);
    controlsLayout->addWidget(passwordLabel, 2, 0);
    controlsLayout->addWidget(passwordLineEdit, 2, 1);
    
    WuQDialogModal d("Connectivity File on Web",
                     controlsWidget,
                     this);
    if (d.exec() == QDialog::Accepted) {
        const AString filename = urlLineEdit->text().trimmed();
        ConnectivityManagerViewController::previousNetworkUsername = usernameLineEdit->text().trimmed();
        ConnectivityManagerViewController::previousNetworkPassword = passwordLineEdit->text().trimmed();
        
        Brain* brain = GuiManager::get()->getBrain();
        
        EventDataFileRead readFileEvent(brain,
                                        this->connectivityFileType,
                                        filename);
        readFileEvent.setUsernameAndPassword(ConnectivityManagerViewController::previousNetworkUsername,
                                             ConnectivityManagerViewController::previousNetworkPassword);
        
        EventManager::get()->sendEvent(readFileEvent.getPointer());
        if (readFileEvent.isError()) {
            QMessageBox::critical(this, 
                                  "ERROR", 
                                  readFileEvent.getErrorMessage());                
        }
    }
}

void 
ConnectivityManagerViewController::updateManagerViewController()
{
    Brain* brain = GuiManager::get()->getBrain();
    
    switch (this->connectivityFileType) {
        case DataFileTypeEnum::CONNECTIVITY_DENSE:
        {
            std::vector<ConnectivityLoaderFile*> files;
            brain->getConnectivityDenseFiles(files);
            this->updateForDenseFiles(files);
        }
            break;
        case DataFileTypeEnum::CONNECTIVITY_DENSE_TIME_SERIES:
        {
            std::vector<ConnectivityLoaderFile*> files;
            brain->getConnectivityTimeSeriesFiles(files);
            this->updateForTimeSeriesFiles(files);
        }
            break;
        default:
            break;
    }
}

/**
 * Update for dense connectivity files.
 * @param connectivityFiles
 *    The connectivity files.
 */
void 
ConnectivityManagerViewController::updateForDenseFiles(const std::vector<ConnectivityLoaderFile*>& denseFiles)
{
    /*
     * Update, show (and possibly add) connectivity view controllers
     */
    const int32_t numDenseFiles = static_cast<int32_t>(denseFiles.size());
    for (int32_t i = 0; i < numDenseFiles; i++) {
        if (i >= static_cast<int32_t>(this->denseViewControllers.size())) {
            this->denseViewControllers.push_back(new ConnectivityDenseViewController(this->orientation,
                                                                           this->viewControllerGridLayout,
                                                                           this));            
        }
        ConnectivityDenseViewController* cvc = this->denseViewControllers[i];
        cvc->updateViewController(denseFiles[i]);
        cvc->setVisible(true);
    }
    
    /*
     * Hide view controllers not needed
     */
    const int32_t numViewControllers = static_cast<int32_t>(this->denseViewControllers.size());
    for (int32_t i = numDenseFiles; i < numViewControllers; i++) {
        this->denseViewControllers[i]->setVisible(false);
    }    
}

/**
 * Update for time series files.
 * @param timeSeriesFiles
 *    The time series files.
 */
void 
ConnectivityManagerViewController::updateForTimeSeriesFiles(const std::vector<ConnectivityLoaderFile*>& timeSeriesFiles)
{
    /*
     * Update, show (and possibly add) time series view controllers
     */
    const int32_t numTimeSeriesFiles = static_cast<int32_t>(timeSeriesFiles.size());
    for (int32_t i = 0; i < numTimeSeriesFiles; i++) {
        if (i >= static_cast<int32_t>(this->timeSeriesViewControllers.size())) {
            this->timeSeriesViewControllers.push_back(new ConnectivityTimeSeriesViewController(this->orientation,
                                                                                       this->viewControllerGridLayout,
                                                                                       this));            
        }
        ConnectivityTimeSeriesViewController* tsvc = this->timeSeriesViewControllers[i];
        tsvc->updateViewController(timeSeriesFiles[i]);
        tsvc->setVisible(true);
    }
    
    /*
     * Hide view controllers not needed
     */
    const int32_t numTimeSeriesViewControllers = static_cast<int32_t>(this->timeSeriesViewControllers.size());
    for (int32_t i = numTimeSeriesFiles; i < numTimeSeriesViewControllers; i++) {
        this->timeSeriesViewControllers[i]->setVisible(false);
    }    
}

/**
 * Receive events from the event manager.
 * 
 * @param event
 *   Event sent by event manager.
 */
void 
ConnectivityManagerViewController::receiveEvent(Event* event)
{
    if (event->getEventType() == EventTypeEnum::EVENT_USER_INTERFACE_UPDATE) {
        EventUserInterfaceUpdate* uiEvent =
        dynamic_cast<EventUserInterfaceUpdate*>(event);
        CaretAssert(uiEvent);
        
        this->updateManagerViewController();
        
        uiEvent->setEventProcessed();
    }
    else if (event->getEventType() == EventTypeEnum::EVENT_TOOLBOX_UPDATE) {
        EventToolBoxUpdate* tbEvent =
        dynamic_cast<EventToolBoxUpdate*>(event);
        bool doUpdate = false;
        if (tbEvent->isUpdateAllWindows()) {
            doUpdate = true;
        }
        else if (tbEvent->getBrowserWindowIndex() == this->browserWindowIndex) {
            doUpdate = true;
        }
        
        if (doUpdate) {
            this->updateManagerViewController();
        }
        
        tbEvent->setEventProcessed();
    }
    else {
    }
}

