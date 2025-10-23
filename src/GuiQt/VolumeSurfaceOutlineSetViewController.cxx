
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

#include <map>

#include <QAction>
#include <QCheckBox>
#include <QGridLayout>
#include <QLabel>
#include <QSpinBox>
#include <QToolButton>
#include <QVBoxLayout>

#define __VOLUME_SURFACE_OUTLINE_SET_VIEW_CONTROLLER_DECLARE__
#include "VolumeSurfaceOutlineSetViewController.h"
#undef __VOLUME_SURFACE_OUTLINE_SET_VIEW_CONTROLLER_DECLARE__

#include "BrowserTabContent.h"
#include "CaretAssert.h"
#include "EventManager.h"
#include "EventBrowserTabGetAll.h"
#include "EventGraphicsPaintSoonAllWindows.h"
#include "EventUserInterfaceUpdate.h"
#include "GuiManager.h"
#include "VolumeSurfaceOutlineSetModel.h"
#include "VolumeSurfaceOutlineViewController.h"
#include "WuQDataEntryDialog.h"
#include "WuQFactory.h"
#include "WuQMacroManager.h"
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
 * @param descriptivePrefix
 *    Descriptive prefix for macros
 * @param parent
 *    Parent widget.
 */
VolumeSurfaceOutlineSetViewController::VolumeSurfaceOutlineSetViewController(const Qt::Orientation orientation,
                                                                             const int32_t browserWindowIndex,
                                                                             const QString& parentObjectNamePrefix,
                                                                             const QString& descriptivePrefix,
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
        gridLayout->setColumnStretch(3, 0);
        gridLayout->setColumnStretch(4, 0);
        gridLayout->setColumnStretch(5, 0);
        gridLayout->setColumnStretch(6, 100);

        QLabel* onLabel = new QLabel("On");
        QLabel* colorLabel = new QLabel("Color Source");
        QLabel* thicknessLabel = new QLabel("Thickness");
        QLabel* depthLabel = new QLabel("Depth");
        QLabel* opacityLabel = new QLabel("Opacity");
        QLabel* drawingLabel = new QLabel("Drawing");
        QLabel* fileLabel = new QLabel("File");
        
        const int row = gridLayout->rowCount();
        gridLayout->addWidget(onLabel, row, 0, Qt::AlignHCenter);
        gridLayout->addWidget(colorLabel, row, 1, Qt::AlignHCenter);
        gridLayout->addWidget(thicknessLabel, row, 2, Qt::AlignHCenter);
        gridLayout->addWidget(depthLabel, row, 3, Qt::AlignHCenter);
        gridLayout->addWidget(opacityLabel, row, 4, Qt::AlignHCenter);
        gridLayout->addWidget(drawingLabel, row, 5, Qt::AlignHCenter);
        gridLayout->addWidget(fileLabel, row, 6, Qt::AlignHCenter);
        
    }
    else {
        gridLayout->setColumnStretch(0, 0);
        gridLayout->setColumnStretch(1, 0);
        gridLayout->setColumnStretch(2, 0);
        gridLayout->setColumnStretch(3, 0);
        gridLayout->setColumnStretch(4, 0);
        gridLayout->setColumnStretch(5, 100);
    }
    
    for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_VOLUME_SURFACE_OUTLINES; i++) {
        const QString name = QString(parentObjectNamePrefix
                                     + ":VolumeSurfaceOutline%1").arg((int)(i + 1), 2, 10, QLatin1Char('0'));
        VolumeSurfaceOutlineViewController* ovc = new VolumeSurfaceOutlineViewController(orientation,
                                                                                         gridLayout,
                                                                                         name,
                                                                                         descriptivePrefix + " " + QString::number(i + 1));
        this->outlineViewControllers.push_back(ovc);
    }
    
    QLabel* outlineCountLabel = new QLabel("Number of Outlines: ");
    this->outlineCountSpinBox = WuQFactory::newSpinBox();
    this->outlineCountSpinBox->setRange(BrainConstants::MINIMUM_NUMBER_OF_VOLUME_SURFACE_OUTLINES,
                                        BrainConstants::MAXIMUM_NUMBER_OF_VOLUME_SURFACE_OUTLINES);
    this->outlineCountSpinBox->setSingleStep(1);
    QObject::connect(this->outlineCountSpinBox, SIGNAL(valueChanged(int)),
                     this, SLOT(outlineCountSpinBoxValueChanged(int)));
    this->outlineCountSpinBox->setObjectName(parentObjectNamePrefix
                                             + ":VolumeSurfaceOutlineNumberOfOutlines");
    this->outlineCountSpinBox->setToolTip("Number of volume surface outlines");
    WuQMacroManager::instance()->addMacroSupportToObject(this->outlineCountSpinBox,
                                                         "Set number of displayed volume/surface outlines for " + descriptivePrefix);
    
    QAction* copyOutlinesAction(new QAction());
    copyOutlinesAction->setIconText("Copy outlines to other tabs...");
    QObject::connect(copyOutlinesAction, &QAction::triggered,
                     this, &VolumeSurfaceOutlineSetViewController::copyOutlinesActionTriggered);
    QToolButton* copyOutlinesToolButton(new QToolButton());
    copyOutlinesToolButton->setDefaultAction(copyOutlinesAction);
    
    QHBoxLayout* overlayCountLayout = new QHBoxLayout();
    overlayCountLayout->addWidget(outlineCountLabel);
    overlayCountLayout->addWidget(this->outlineCountSpinBox);
    overlayCountLayout->addSpacing(25);
    overlayCountLayout->addWidget(copyOutlinesToolButton);
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
 * @return Browser tab content in the window containing this widget
 */
BrowserTabContent*
VolumeSurfaceOutlineSetViewController::getBrowserTabContent()
{
    const bool allowInvalidBrowserWindowIndex(true);
    return GuiManager::get()->getBrowserTabContentForBrowserWindow(this->browserWindowIndex,
                                                                   allowInvalidBrowserWindowIndex);

}

/**
 * @return The outline set in this view controller.
 */
VolumeSurfaceOutlineSetModel* 
VolumeSurfaceOutlineSetViewController::getOutlineSet()
{
    VolumeSurfaceOutlineSetModel* outlineSet = NULL;
    BrowserTabContent* browserTabContent = getBrowserTabContent();
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

/**
 * Called to display dialog to copy outlines to other tabs
 */
void
VolumeSurfaceOutlineSetViewController::copyOutlinesActionTriggered()
{
    BrowserTabContent* thisTabContent = getBrowserTabContent();
    if (thisTabContent == NULL) {
        return;
    }
    
    EventBrowserTabGetAll allTabsEvent;
    EventManager::get()->sendEvent(allTabsEvent.getPointer());
    
    std::vector<BrowserTabContent*> allTabs(allTabsEvent.getAllBrowserTabs());
    const int32_t numTabs(allTabs.size());
    if (numTabs <= 1) {
        return;
    }
    
    std::map<int32_t, QCheckBox*> checkboxes;
    WuQDataEntryDialog ded("Copy Volume/Surface Outlines",
                           this);
    const bool wrapTheTextFlag(true);
    ded.setTextAtTop(("Copy the Volume/Surface outline settings "
                      "from this tab to the selected tabs"),
                     wrapTheTextFlag);
    for (const BrowserTabContent* tab : allTabs) {
        CaretAssert(tab);
        const int32_t tabIndex(tab->getTabNumber());
        checkboxes[tabIndex] = ded.addCheckBox(tab->getTabName());
        checkboxes[tabIndex]->setEnabled(tab != thisTabContent);
    }
    if (ded.exec() == WuQDataEntryDialog::Accepted) {
        for (BrowserTabContent* tab : allTabs) {
            CaretAssert(tab);
            const int32_t tabIndex(tab->getTabNumber());
            const QCheckBox* checkbox(checkboxes[tabIndex]);
            CaretAssert(checkbox);
            if (checkbox->isChecked()) {
                tab->getVolumeSurfaceOutlineSet()->copyVolumeSurfaceOutlineSetModel(thisTabContent->getVolumeSurfaceOutlineSet());
            }
        }
        
        EventManager::get()->sendEvent(EventGraphicsPaintSoonAllWindows().getPointer());
        EventManager::get()->sendEvent(EventUserInterfaceUpdate().getPointer());
    }
}

