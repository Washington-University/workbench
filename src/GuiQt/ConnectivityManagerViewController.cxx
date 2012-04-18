
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

#include <QVBoxLayout>

#include "Brain.h"
#include "CaretAssert.h"
#include "ConnectivityLoaderFile.h"
#include "ConnectivityLoaderManager.h"
#include "EventManager.h"
#include "EventToolBoxUpdate.h"
#include "EventUserInterfaceUpdate.h"
#include "GuiManager.h"
#include "WuQtUtilities.h"

#define __CONNECTIVITY_MANAGER_VIEW_CONTROLLER_DECLARE__
#include "ConnectivityManagerViewController.h"
#undef __CONNECTIVITY_MANAGER_VIEW_CONTROLLER_DECLARE__

#include "ConnectivityViewController.h"

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
                                                                                 QWidget* parent)
: QWidget(parent)
{
    this->orientation = orientation;
    this->browserWindowIndex = browserWindowIndex;
    
    this->viewControllerGridLayout = new QGridLayout();
    WuQtUtilities::setLayoutMargins(this->viewControllerGridLayout, 2, 2);
    this->viewControllerGridLayout->setColumnStretch(0, 0);
    this->viewControllerGridLayout->setColumnStretch(1, 100);
    
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addLayout(this->viewControllerGridLayout);
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
ConnectivityManagerViewController::updateManagerViewController()
{
    Brain* brain = GuiManager::get()->getBrain();
    ConnectivityLoaderManager* manager = brain->getConnectivityLoaderManager();
    const int numberOfFiles = manager->getNumberOfConnectivityLoaderFiles();
    
    /*
     * Find connectivity files
     */
    std::vector<ConnectivityLoaderFile*> connectivityFiles;
    for (int32_t i = 0; i < numberOfFiles; i++) {
        ConnectivityLoaderFile* clf = manager->getConnectivityLoaderFile(i);
        if (clf->isDense()) {
            connectivityFiles.push_back(clf);
        }
        else if (clf->isDenseTimeSeries()) {
            // ignore
        }
        else {
//            CaretAssertMessage(0, ("Unrecognized type of connectivity file: "
//                                   + clf->getFileNameNoPath()));
        }
    }
    
    /*
     * Update, show (and possibly add) connectivity view controllers
     */
    const int32_t numConnectivityFiles = static_cast<int32_t>(connectivityFiles.size());
    for (int32_t i = 0; i < numConnectivityFiles; i++) {
        if (i >= static_cast<int32_t>(this->viewControllers.size())) {
            this->viewControllers.push_back(new ConnectivityViewController(this->orientation,
                                                                           this->viewControllerGridLayout,
                                                                           this));
 
        }
        ConnectivityViewController* cvc = this->viewControllers[i];
        cvc->updateViewController(connectivityFiles[i]);
        cvc->setVisible(true);
    }
    
    /*
     * Hide view controllers not needed
     */
    const int32_t numViewControllers = static_cast<int32_t>(this->viewControllers.size());
    for (int32_t i = numConnectivityFiles; i < numViewControllers; i++) {
        this->viewControllers[i]->setVisible(false);
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

