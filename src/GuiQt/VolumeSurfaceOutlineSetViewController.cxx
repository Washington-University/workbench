
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
#include <QLabel>
#include <QSpinBox>
#include <QVBoxLayout>

#define __VOLUME_SURFACE_OUTLINE_SET_VIEW_CONTROLLER_DECLARE__
#include "VolumeSurfaceOutlineSetViewController.h"
#undef __VOLUME_SURFACE_OUTLINE_SET_VIEW_CONTROLLER_DECLARE__

#include "BrowserTabContent.h"
#include "CaretAssert.h"
#include "EventManager.h"
#include "EventUserInterfaceUpdate.h"
#include "GuiManager.h"
#include "VolumeSurfaceOutlineSetModel.h"
#include "VolumeSurfaceOutlineViewController.h"
#include "WuQFactory.h"
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
 * @param parentObjectNamePrefix
 *    Name of parent object for macros
 * @param parent
 *    Parent widget.
 */
VolumeSurfaceOutlineSetViewController::VolumeSurfaceOutlineSetViewController(const Qt::Orientation orientation,
                                                                             const int32_t browserWindowIndex,
                                                                             const QString& parentObjectNamePrefix,
                                                                             QWidget* parent)
: QWidget(parent)
{    
    this->browserWindowIndex = browserWindowIndex;
    
    QWidget* gridWidget = new QWidget();
    QGridLayout* gridLayout = new QGridLayout(gridWidget);
    WuQtUtilities::setLayoutSpacingAndMargins(gridLayout, 4, 2);
    if (orientation == Qt::Horizontal) {
        gridLayout->setColumnStretch(0, 0);
        gridLayout->setColumnStretch(1, 0);
        gridLayout->setColumnStretch(2, 0);
        gridLayout->setColumnStretch(3, 100);
        
        QLabel* onLabel = new QLabel("On");
        QLabel* colorLabel = new QLabel("Color Source");
        QLabel* thicknessLabel = new QLabel("Thickness");
        QLabel* fileLabel = new QLabel("File");
        
        const int row = gridLayout->rowCount();
        gridLayout->addWidget(onLabel, row, 0, Qt::AlignHCenter);
        gridLayout->addWidget(colorLabel, row, 1, Qt::AlignHCenter);
        gridLayout->addWidget(thicknessLabel, row, 2, Qt::AlignHCenter);
        gridLayout->addWidget(fileLabel, row, 3, Qt::AlignHCenter);
        
    }
    else {
        gridLayout->setColumnStretch(0, 0);
        gridLayout->setColumnStretch(1, 0);
        gridLayout->setColumnStretch(2, 100);
    }
    
    for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_VOLUME_SURFACE_OUTLINES; i++) {
        const QString name = QString(parentObjectNamePrefix
                                     + ":VolumeSurfaceOutline%1").arg((int)(i + 1), 2, 10, QLatin1Char('0'));
        VolumeSurfaceOutlineViewController* ovc = new VolumeSurfaceOutlineViewController(orientation,
                                                                                         gridLayout,
                                                                                         name);
        this->outlineViewControllers.push_back(ovc);
    }
    
    QLabel* outlineCountLabel = new QLabel("Number of Outlines: ");
    this->outlineCountSpinBox = WuQFactory::newSpinBox();
    this->outlineCountSpinBox->setRange(BrainConstants::MINIMUM_NUMBER_OF_VOLUME_SURFACE_OUTLINES,
                                        BrainConstants::MAXIMUM_NUMBER_OF_VOLUME_SURFACE_OUTLINES);
    this->outlineCountSpinBox->setSingleStep(1);
    QObject::connect(this->outlineCountSpinBox, SIGNAL(valueChanged(int)),
                     this, SLOT(outlineCountSpinBoxValueChanged(int)));
    
    QHBoxLayout* overlayCountLayout = new QHBoxLayout();
    overlayCountLayout->addWidget(outlineCountLabel);
    overlayCountLayout->addWidget(this->outlineCountSpinBox);
    overlayCountLayout->addStretch();
    
    QVBoxLayout* layout = new QVBoxLayout(this);
    WuQtUtilities::setLayoutSpacingAndMargins(layout, 2, 2);
    layout->addWidget(gridWidget);
    layout->addLayout(overlayCountLayout);
    layout->addStretch();
    
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_USER_INTERFACE_UPDATE);
}

/**
 * Destructor.
 */
VolumeSurfaceOutlineSetViewController::~VolumeSurfaceOutlineSetViewController()
{
    EventManager::get()->removeAllEventsFromListener(this);
    for (size_t i = 0; i < outlineViewControllers.size(); ++i)
    {
        delete outlineViewControllers[i];
    }
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
        
        if (uiEvent->isUpdateForWindow(this->browserWindowIndex)) {
            if (uiEvent->isSurfaceUpdate()
                || uiEvent->isToolBoxUpdate()) {
                this->updateViewController();
                uiEvent->setEventProcessed();
            }
        }
    }
}
