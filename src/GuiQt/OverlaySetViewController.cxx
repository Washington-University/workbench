
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
#include <QBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QSpinBox>
#include <QVBoxLayout>

#define __OVERLAY_SET_VIEW_CONTROLLER_DECLARE__
#include "OverlaySetViewController.h"
#undef __OVERLAY_SET_VIEW_CONTROLLER_DECLARE__

#include "BrainConstants.h"
#include "BrowserTabContent.h"
#include "CaretAssert.h"
#include "EventGraphicsUpdateOneWindow.h"
#include "EventManager.h"
#include "EventUserInterfaceUpdate.h"
#include "GuiManager.h"
#include "OverlaySet.h"
#include "OverlayViewController.h"
#include "WuQtUtilities.h"

using namespace caret;


    
/**
 * \class caret::OverlaySetViewController 
 * \brief View Controller for an overlay set.
 * \ingroup GuiQt
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
OverlaySetViewController::OverlaySetViewController(const Qt::Orientation orientation,
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
        gridLayout->setColumnStretch(3, 0);
        gridLayout->setColumnStretch(4, 0);
        gridLayout->setColumnStretch(5, 100);
        gridLayout->setColumnStretch(6, 100);
        
        QLabel* onLabel       = new QLabel("On");
        QLabel* settingsLabel = new QLabel("Settings");
        QLabel* opacityLabel  = new QLabel("Opacity");
        QLabel* fileLabel     = new QLabel("File");
        QLabel* mapLabel      = new QLabel("Map");

        const int row = gridLayout->rowCount();
        gridLayout->addWidget(onLabel, row, 0, Qt::AlignHCenter);
        gridLayout->addWidget(settingsLabel, row, 1, 1, 3, Qt::AlignHCenter);
        gridLayout->addWidget(opacityLabel, row, 4, Qt::AlignHCenter);
        gridLayout->addWidget(fileLabel, row, 5, Qt::AlignHCenter);
        gridLayout->addWidget(mapLabel, row, 6, Qt::AlignHCenter);
    }
    else {
        gridLayout->setColumnStretch(0, 0);
        gridLayout->setColumnStretch(1, 0);
        gridLayout->setColumnStretch(2, 0);
        gridLayout->setColumnStretch(3, 0);
        gridLayout->setColumnStretch(4, 0);
        gridLayout->setColumnStretch(5, 100);
    }
    
    for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_OVERLAYS; i++) {
        OverlayViewController* ovc = new OverlayViewController(orientation,
                                                               gridLayout,
                                                               browserWindowIndex,
                                                               i,
                                                               this);
        this->overlayViewControllers.push_back(ovc);
        
        QObject::connect(ovc, SIGNAL(requestAddOverlayAbove(const int32_t)),
                         this, SLOT(processAddOverlayAbove(const int32_t)));
        QObject::connect(ovc, SIGNAL(requestAddOverlayBelow(const int32_t)),
                         this, SLOT(processAddOverlayBelow(const int32_t)));
        QObject::connect(ovc, SIGNAL(requestRemoveOverlay(const int32_t)),
                         this, SLOT(processRemoveOverlay(const int32_t)));
        QObject::connect(ovc, SIGNAL(requestMoveOverlayUp(const int32_t)),
                         this, SLOT(processMoveOverlayUp(const int32_t)));
        QObject::connect(ovc, SIGNAL(requestMoveOverlayDown(const int32_t)),
                         this, SLOT(processMoveOverlayDown(const int32_t)));
    }

    if (orientation == Qt::Horizontal) {
        QVBoxLayout* verticalLayout = new QVBoxLayout(this);
        WuQtUtilities::setLayoutMargins(verticalLayout, 2, 2);
        verticalLayout->addWidget(gridWidget);
        verticalLayout->addStretch();
    }
    else {
        QVBoxLayout* verticalLayout = new QVBoxLayout();
        WuQtUtilities::setLayoutMargins(verticalLayout, 1, 1);
        verticalLayout->addWidget(gridWidget);
        verticalLayout->addStretch();
        
        QHBoxLayout* horizontalLayout = new QHBoxLayout(this);
        WuQtUtilities::setLayoutMargins(horizontalLayout, 1, 1);
        horizontalLayout->addLayout(verticalLayout);
        horizontalLayout->addStretch();
    }
    
//    QWidget* widget = new QWidget();
//    QVBoxLayout* widgetLayout = new QVBoxLayout(widget);
//    WuQtUtilities::setLayoutMargins(widgetLayout, 2, 2);
//    widgetLayout->addWidget(gridWidget);
//    widgetLayout->addStretch();
//    
//    QVBoxLayout* layout = new QVBoxLayout(this);
//    WuQtUtilities::setLayoutMargins(layout, 0, 0);
//    layout->addWidget(widget);
    
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_USER_INTERFACE_UPDATE);
}

/**
 * Destructor.
 */
OverlaySetViewController::~OverlaySetViewController()
{
    EventManager::get()->removeAllEventsFromListener(this);
}

/**
 * @return The overlay set in this view controller.
 */
OverlaySet* 
OverlaySetViewController::getOverlaySet()
{
    OverlaySet* overlaySet = NULL;
    BrowserTabContent* browserTabContent = 
    GuiManager::get()->getBrowserTabContentForBrowserWindow(this->browserWindowIndex, true);
    if (browserTabContent != NULL) {
        overlaySet = browserTabContent->getOverlaySet();
    }
    
    return overlaySet;
}

/**
 * Update this overlay set view controller using the given overlay set.
 */
void 
OverlaySetViewController::updateViewController()
{
    OverlaySet* overlaySet = this->getOverlaySet();
    if (overlaySet == NULL) {
        return;
    }
    
    const int32_t numberOfOverlays = static_cast<int32_t>(this->overlayViewControllers.size());
    const int32_t numberOfDisplayedOverlays = overlaySet->getNumberOfDisplayedOverlays();

    for (int32_t i = 0; i < numberOfOverlays; i++) {
        Overlay* overlay = NULL;
        if (overlaySet != NULL) {
            overlay = overlaySet->getOverlay(i);
        }
        this->overlayViewControllers[i]->updateViewController(overlay);
        
        bool displayOverlay = (overlay != NULL);
        if (i >= numberOfDisplayedOverlays) {
            displayOverlay = false;
        }
        this->overlayViewControllers[i]->setVisible(displayOverlay);
    }
}

/**
 * Receive events from the event manager.
 * 
 * @param event
 *   Event sent by event manager.
 */
void 
OverlaySetViewController::receiveEvent(Event* event)
{
    if (event->getEventType() == EventTypeEnum::EVENT_USER_INTERFACE_UPDATE) {
        EventUserInterfaceUpdate* uiEvent =
            dynamic_cast<EventUserInterfaceUpdate*>(event);
        CaretAssert(uiEvent);
        
        if (uiEvent->isUpdateForWindow(this->browserWindowIndex)) {
            if (uiEvent->isToolBoxUpdate()) {
                this->updateViewController();
                uiEvent->setEventProcessed();
            }
        }
    }
}

/**
 * Add an overlay above the overlay with the given index.
 * @param overlayIndex
 *    Index of overlay that will have an overlay added above it.
 */
void 
OverlaySetViewController::processAddOverlayAbove(const int32_t overlayIndex)
{
    OverlaySet* overlaySet = getOverlaySet();
    if (overlaySet != NULL) {
        overlaySet->insertOverlayAbove(overlayIndex);
        this->updateViewController();
        EventGraphicsUpdateOneWindow graphicsUpdate(this->browserWindowIndex);
        EventManager::get()->sendEvent(graphicsUpdate.getPointer());
    }
}

/**
 * Add an overlay below the overlay with the given index.
 * @param overlayIndex
 *    Index of overlay that will have an overlay added below it.
 */
void 
OverlaySetViewController::processAddOverlayBelow(const int32_t overlayIndex)
{
    OverlaySet* overlaySet = getOverlaySet();
    if (overlaySet != NULL) {
        overlaySet->insertOverlayBelow(overlayIndex);
        this->updateViewController();
        EventGraphicsUpdateOneWindow graphicsUpdate(this->browserWindowIndex);
        EventManager::get()->sendEvent(graphicsUpdate.getPointer());
    }
}

/**
 * Remove an overlay above the overlay with the given index.
 * @param overlayIndex
 *    Index of overlay that will be removed
 */
void 
OverlaySetViewController::processRemoveOverlay(const int32_t overlayIndex)
{
    OverlaySet* overlaySet = getOverlaySet();
    if (overlaySet != NULL) {
        overlaySet->removeDisplayedOverlay(overlayIndex);
        this->updateViewController();
        EventGraphicsUpdateOneWindow graphicsUpdate(this->browserWindowIndex);
        EventManager::get()->sendEvent(graphicsUpdate.getPointer());
    }
}

/**
 * Remove an overlay above the overlay with the given index.
 * @param overlayIndex
 *    Index of overlay that will be removed
 */
void 
OverlaySetViewController::processMoveOverlayDown(const int32_t overlayIndex)
{
    OverlaySet* overlaySet = getOverlaySet();
    if (overlaySet != NULL) {
        overlaySet->moveDisplayedOverlayDown(overlayIndex);
        this->updateViewController();
        EventGraphicsUpdateOneWindow graphicsUpdate(this->browserWindowIndex);
        EventManager::get()->sendEvent(graphicsUpdate.getPointer());
    }
}

/**
 * Remove an overlay above the overlay with the given index.
 * @param overlayIndex
 *    Index of overlay that will be removed
 */
void 
OverlaySetViewController::processMoveOverlayUp(const int32_t overlayIndex)
{
    OverlaySet* overlaySet = getOverlaySet();
    if (overlaySet != NULL) {
        overlaySet->moveDisplayedOverlayUp(overlayIndex);
        this->updateViewController();
        EventGraphicsUpdateOneWindow graphicsUpdate(this->browserWindowIndex);
        EventManager::get()->sendEvent(graphicsUpdate.getPointer());
    }
}


