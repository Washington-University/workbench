
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

#include <QGridLayout>
#include <QLabel>
#include <QSpinBox>
#include <QVBoxLayout>

#define __VOLUME_SURFACE_OUTLINE_SET_VIEW_CONTROLLER_DECLARE__
#include "VolumeSurfaceOutlineSetViewController.h"
#undef __VOLUME_SURFACE_OUTLINE_SET_VIEW_CONTROLLER_DECLARE__

#include "BrowserTabContent.h"
#include "CaretAssert.h"
#include "EventManager.h"
#include "EventToolBoxUpdate.h"
#include "EventUserInterfaceUpdate.h"
#include "GuiManager.h"
#include "VolumeSurfaceOutlineSetModel.h"
#include "VolumeSurfaceOutlineViewController.h"
#include "WuQtUtilities.h"
using namespace caret;


    
/**
 * \class caret::VolumeSurfaceOutlineSetViewController 
 * \brief View Controller for VolumeSurfaceOutlineSetModel
 */

/**
 * Constructor.
 *
 * @param orientation
 *    Orientation for layout
 * @param browserWindowIndex
 *    Index of browser window that contains this view controller.
 * @param parent
 *    Parent widget.
 */
VolumeSurfaceOutlineSetViewController::VolumeSurfaceOutlineSetViewController(const Qt::Orientation orientation,
                                                                             const int32_t browserWindowIndex,
                                                                             QWidget* parent)
: QWidget(parent)
{    
    this->browserWindowIndex = browserWindowIndex;
    
    QWidget* gridWidget = new QWidget();
    QGridLayout* gridLayout = new QGridLayout(gridWidget);
    WuQtUtilities::setLayoutMargins(gridLayout, 4, 2);
    if (orientation == Qt::Horizontal) {
        gridLayout->setColumnStretch(0, 0);
        gridLayout->setColumnStretch(1, 0);
        gridLayout->setColumnStretch(2, 0);
        gridLayout->setColumnStretch(3, 100);
    }
    else {
        gridLayout->setColumnStretch(0, 0);
        gridLayout->setColumnStretch(1, 0);
        gridLayout->setColumnStretch(2, 100);
    }
    
    for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_VOLUME_SURFACE_OUTLINES; i++) {
        VolumeSurfaceOutlineViewController* ovc = new VolumeSurfaceOutlineViewController(orientation,
                                                               gridLayout);
        this->outlineViewControllers.push_back(ovc);
    }
    
    QLabel* outlineCountLabel = new QLabel("Number of Outlines: ");
    this->outlineCountSpinBox = new QSpinBox();
    this->outlineCountSpinBox->setRange(BrainConstants::MINIMUM_NUMBER_OF_OVERLAYS,
                                        BrainConstants::MAXIMUM_NUMBER_OF_OVERLAYS);
    this->outlineCountSpinBox->setSingleStep(1);
    QObject::connect(this->outlineCountSpinBox, SIGNAL(valueChanged(int)),
                     this, SLOT(outlineCountSpinBoxValueChanged(int)));
    
    QHBoxLayout* overlayCountLayout = new QHBoxLayout();
    overlayCountLayout->addWidget(outlineCountLabel);
    overlayCountLayout->addWidget(this->outlineCountSpinBox);
    overlayCountLayout->addStretch();
    
    QVBoxLayout* layout = new QVBoxLayout(this);
    WuQtUtilities::setLayoutMargins(layout, 2, 2);
    layout->addWidget(gridWidget);
    layout->addLayout(overlayCountLayout);
    layout->addStretch();
    
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_USER_INTERFACE_UPDATE);
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_TOOLBOX_UPDATE);
}

/**
 * Destructor.
 */
VolumeSurfaceOutlineSetViewController::~VolumeSurfaceOutlineSetViewController()
{
    EventManager::get()->removeAllEventsFromListener(this);
}

/**
 * Called when outline count spin box value changed.
 * @param value
 *    New value.
 */
void 
VolumeSurfaceOutlineSetViewController::outlineCountSpinBoxValueChanged(int value)
{
    VolumeSurfaceOutlineSetModel* outlineSet = this->getOutlineSet();
    if (outlineSet != NULL) {
        outlineSet->setNumberOfDisplayedVolumeSurfaceOutlines(value);
        this->updateViewController();
    }
}

/**
 * @return The outline set in this view controller.
 */
VolumeSurfaceOutlineSetModel* 
VolumeSurfaceOutlineSetViewController::getOutlineSet()
{
    VolumeSurfaceOutlineSetModel* outlineSet = NULL;
    BrowserTabContent* browserTabContent = 
    GuiManager::get()->getBrowserTabContentForBrowserWindow(this->browserWindowIndex, true);
    if (browserTabContent != NULL) {
        outlineSet = browserTabContent->getVolumeSurfaceOutlineSet();
    }
    
    return outlineSet;
}

/**
 * Update this overlay set view controller using the given overlay set.
 */
void 
VolumeSurfaceOutlineSetViewController::updateViewController()
{
    VolumeSurfaceOutlineSetModel* outlineSet = this->getOutlineSet();
    if (outlineSet == NULL) {
        return;
    }
    
    const int32_t numberOfOutlines = static_cast<int32_t>(this->outlineViewControllers.size());
    const int32_t numberOfDisplayedOutline = outlineSet->getNumberOfDislayedVolumeSurfaceOutlines();
    
    this->outlineCountSpinBox->blockSignals(true);
    this->outlineCountSpinBox->setValue(numberOfDisplayedOutline);
    this->outlineCountSpinBox->blockSignals(false);
    
    for (int32_t i = 0; i < numberOfOutlines; i++) {
        VolumeSurfaceOutlineModel* outlineModel = NULL;
        if (outlineSet != NULL) {
            outlineModel = outlineSet->getVolumeSurfaceOutlineModel(i);
        }
        
        this->outlineViewControllers[i]->updateViewController(outlineModel);
        
        bool displayOutline = (outlineModel != NULL);
        if (i >= numberOfDisplayedOutline) {
            displayOutline = false;
        }
        this->outlineViewControllers[i]->setVisible(displayOutline);
    }
}

/**
 * Receive events from the event manager.
 * 
 * @param event
 *   Event sent by event manager.
 */
void 
VolumeSurfaceOutlineSetViewController::receiveEvent(Event* event)
{
    if (event->getEventType() == EventTypeEnum::EVENT_USER_INTERFACE_UPDATE) {
        EventUserInterfaceUpdate* uiEvent =
        dynamic_cast<EventUserInterfaceUpdate*>(event);
        CaretAssert(uiEvent);
        
        this->updateViewController();
        
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
            this->updateViewController();
        }
        
        tbEvent->setEventProcessed();
    }
    else {
    }
}
