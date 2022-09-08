
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


#include <QGridLayout>
#include <QBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QSpinBox>
#include <QVBoxLayout>

#define __HISTOLOGY_OVERLAY_SET_VIEW_CONTROLLER_DECLARE__
#include "HistologyOverlaySetViewController.h"
#undef __HISTOLOGY_OVERLAY_SET_VIEW_CONTROLLER_DECLARE__

#include "BrainConstants.h"
#include "BrowserTabContent.h"
#include "CaretAssert.h"
#include "EventGraphicsUpdateOneWindow.h"
#include "EventManager.h"
#include "EventSurfaceColoringInvalidate.h"
#include "EventUserInterfaceUpdate.h"
#include "GuiManager.h"
#include "HistologyOverlaySet.h"
#include "HistologyOverlayViewController.h"
#include "WuQtUtilities.h"

using namespace caret;


    
/**
 * \class caret::HistologyOverlaySetViewController
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
 * @param parentObjectName
 *    Name of parent
 * @param parent
 *    Parent widget.
 */
HistologyOverlaySetViewController::HistologyOverlaySetViewController(const Qt::Orientation orientation,
                                                   const int32_t browserWindowIndex,
                                                   const QString& parentObjectName,
                                                   QWidget* parent)
: QWidget(parent)
{
    m_browserWindowIndex = browserWindowIndex;
    
    QWidget* gridWidget = new QWidget();
    QGridLayout* gridLayout = new QGridLayout(gridWidget);
    WuQtUtilities::setLayoutSpacingAndMargins(gridLayout, 4, 2);
    if (orientation == Qt::Horizontal) {
        gridLayout->setColumnStretch(0, 0);
        gridLayout->setColumnStretch(1, 0);
        gridLayout->setColumnStretch(2, 0);
        gridLayout->setColumnStretch(3, 0);
        gridLayout->setColumnStretch(4, 0);
        gridLayout->setColumnStretch(5, 100);
        gridLayout->setColumnStretch(6, 0);
        gridLayout->setColumnStretch(7, 0);
        gridLayout->setColumnStretch(8, 100);
        
        QLabel* onLabel       = new QLabel("On");
        QLabel* settingsLabel = new QLabel("Settings");
        QLabel* opacityLabel  = new QLabel("Opacity");
        QLabel* fileLabel     = new QLabel("File");
        QLabel* yokeLabel     = new QLabel("Yoke");
        QLabel* mapLabel      = new QLabel("Frame");

        const int row = gridLayout->rowCount();
        gridLayout->addWidget(onLabel, row, 0, Qt::AlignHCenter);
        gridLayout->addWidget(settingsLabel, row, 1, 1, 3, Qt::AlignHCenter);
        gridLayout->addWidget(opacityLabel, row, 4, Qt::AlignHCenter);
        gridLayout->addWidget(fileLabel, row, 5, Qt::AlignHCenter);
        gridLayout->addWidget(yokeLabel, row, 6, Qt::AlignHCenter);
        gridLayout->addWidget(mapLabel, row, 7, 1, 2, Qt::AlignHCenter);
    }
    else {
        gridLayout->setColumnStretch(0, 0);
        gridLayout->setColumnStretch(1, 0);
        gridLayout->setColumnStretch(2, 0);
        gridLayout->setColumnStretch(3, 0);
        gridLayout->setColumnStretch(4, 0);
        gridLayout->setColumnStretch(5, 0);
        gridLayout->setColumnStretch(6, 100);
    }
    
    for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_OVERLAYS; i++) {
        HistologyOverlayViewController* ovc = new HistologyOverlayViewController(orientation,
                                                               gridLayout,
                                                               browserWindowIndex,
                                                               i,
                                                               parentObjectName,
                                                               this);
        m_overlayViewControllers.push_back(ovc);
        
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
        WuQtUtilities::setLayoutSpacingAndMargins(verticalLayout, 2, 2);
        verticalLayout->addWidget(gridWidget);
        verticalLayout->addStretch();
    }
    else {
        /*
         * Resolve WB-649
         */
        QVBoxLayout* verticalLayout = new QVBoxLayout(this);
        WuQtUtilities::setLayoutSpacingAndMargins(verticalLayout, 1, 1);
        verticalLayout->addWidget(gridWidget);
        verticalLayout->addStretch();
    }
    
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_USER_INTERFACE_UPDATE);
}

/**
 * Destructor.
 */
HistologyOverlaySetViewController::~HistologyOverlaySetViewController()
{
    EventManager::get()->removeAllEventsFromListener(this);
}

/**
 * @return The overlay set in this view controller.
 */
HistologyOverlaySet*
HistologyOverlaySetViewController::getOverlaySet()
{
    HistologyOverlaySet* overlaySet = NULL;
    BrowserTabContent* browserTabContent = 
    GuiManager::get()->getBrowserTabContentForBrowserWindow(m_browserWindowIndex, true);
    if (browserTabContent != NULL) {
        overlaySet = browserTabContent->getHistologyOverlaySet();
    }
    
    return overlaySet;
}

/**
 * Update this overlay set view controller using the given overlay set.
 */
void 
HistologyOverlaySetViewController::updateViewController()
{
    HistologyOverlaySet* overlaySet = this->getOverlaySet();
    if (overlaySet == NULL) {
        return;
    }
    
    const int32_t numberOfOverlays = static_cast<int32_t>(m_overlayViewControllers.size());
    const int32_t numberOfDisplayedOverlays = overlaySet->getNumberOfDisplayedOverlays();

    for (int32_t i = 0; i < numberOfOverlays; i++) {
        HistologyOverlay* overlay = NULL;
        if (overlaySet != NULL) {
            overlay = overlaySet->getOverlay(i);
        }
        m_overlayViewControllers[i]->updateViewController(overlay);
        
        bool displayOverlay = (overlay != NULL);
        if (i >= numberOfDisplayedOverlays) {
            displayOverlay = false;
        }
        m_overlayViewControllers[i]->setVisible(displayOverlay);
    }
}

/**
 * Receive events from the event manager.
 * 
 * @param event
 *   Event sent by event manager.
 */
void 
HistologyOverlaySetViewController::receiveEvent(Event* event)
{
    if (event->getEventType() == EventTypeEnum::EVENT_USER_INTERFACE_UPDATE) {
        EventUserInterfaceUpdate* uiEvent =
            dynamic_cast<EventUserInterfaceUpdate*>(event);
        CaretAssert(uiEvent);
        
        if (uiEvent->isUpdateForWindow(m_browserWindowIndex)) {
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
HistologyOverlaySetViewController::processAddOverlayAbove(const int32_t overlayIndex)
{
    HistologyOverlaySet* overlaySet = getOverlaySet();
    if (overlaySet != NULL) {
        overlaySet->insertOverlayAbove(overlayIndex);
        this->updateColoringAndGraphics();
    }
}

/**
 * Add an overlay below the overlay with the given index.
 * @param overlayIndex
 *    Index of overlay that will have an overlay added below it.
 */
void 
HistologyOverlaySetViewController::processAddOverlayBelow(const int32_t overlayIndex)
{
    HistologyOverlaySet* overlaySet = getOverlaySet();
    if (overlaySet != NULL) {
        overlaySet->insertOverlayBelow(overlayIndex);
        this->updateColoringAndGraphics();
    }
}

/**
 * Remove an overlay above the overlay with the given index.
 * @param overlayIndex
 *    Index of overlay that will be removed
 */
void 
HistologyOverlaySetViewController::processRemoveOverlay(const int32_t overlayIndex)
{
    HistologyOverlaySet* overlaySet = getOverlaySet();
    if (overlaySet != NULL) {
        overlaySet->removeDisplayedOverlay(overlayIndex);
        this->updateColoringAndGraphics();
    }
}

/**
 * Remove an overlay above the overlay with the given index.
 * @param overlayIndex
 *    Index of overlay that will be removed
 */
void 
HistologyOverlaySetViewController::processMoveOverlayDown(const int32_t overlayIndex)
{
    HistologyOverlaySet* overlaySet = getOverlaySet();
    if (overlaySet != NULL) {
        overlaySet->moveDisplayedOverlayDown(overlayIndex);
        this->updateColoringAndGraphics();
    }
}

/**
 * Remove an overlay above the overlay with the given index.
 * @param overlayIndex
 *    Index of overlay that will be removed
 */
void 
HistologyOverlaySetViewController::processMoveOverlayUp(const int32_t overlayIndex)
{
    HistologyOverlaySet* overlaySet = getOverlaySet();
    if (overlaySet != NULL) {
        overlaySet->moveDisplayedOverlayUp(overlayIndex);
        this->updateColoringAndGraphics();
    }
}

/**
 * Update surface coloring and graphics after overlay changes.
 */
void
HistologyOverlaySetViewController::updateColoringAndGraphics()
{
    this->updateViewController();
    
    EventManager::get()->sendEvent(EventSurfaceColoringInvalidate().getPointer());
    EventGraphicsUpdateOneWindow graphicsUpdate(m_browserWindowIndex);
    EventManager::get()->sendEvent(graphicsUpdate.getPointer());
}


