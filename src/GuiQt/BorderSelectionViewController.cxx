
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

#include <QCheckBox>
#include <QLabel>
#include <QLayout>

#define __BORDER_SELECTION_VIEW_CONTROLLER_DECLARE__
#include "BorderSelectionViewController.h"
#undef __BORDER_SELECTION_VIEW_CONTROLLER_DECLARE__

#include "Brain.h"
#include "BrowserTabContent.h"
#include "CaretAssert.h"
#include "ClassAndNameHierarchyViewController.h"
#include "DisplayGroupEnumComboBox.h"
#include "DisplayPropertiesBorders.h"
#include "EventGraphicsUpdateAllWindows.h"
#include "EventManager.h"
#include "EventToolBoxUpdate.h"
#include "EventUserInterfaceUpdate.h"
#include "GuiManager.h"

using namespace caret;


    
/**
 * \class caret::BorderSelectionViewController 
 * \brief Widget for controlling display of borders
 *
 * Widget for controlling the display of borders including
 * different display groups.
 */

/**
 * Constructor.
 */
BorderSelectionViewController::BorderSelectionViewController(const int32_t browserWindowIndex,
                                             QWidget* parent)
: QWidget(parent)
{
    this->browserWindowIndex = browserWindowIndex;
    QLabel* groupLabel = new QLabel("Group");
    this->bordersDisplayGroupComboBox = new DisplayGroupEnumComboBox(this);
    QObject::connect(this->bordersDisplayGroupComboBox, SIGNAL(displayGroupSelected(const DisplayGroupEnum::Enum)),
                     this, SLOT(borderDisplayGroupSelected(const DisplayGroupEnum::Enum)));
    
    QHBoxLayout* groupLayout = new QHBoxLayout();
    groupLayout->addWidget(groupLabel);
    groupLayout->addWidget(this->bordersDisplayGroupComboBox->getWidget());
    groupLayout->addStretch(); 
    
    this->bordersContralateralCheckBox = new QCheckBox("Contralateral");
    QObject::connect(this->bordersContralateralCheckBox, SIGNAL(clicked(bool)),
                     this, SLOT(processBorderSelectionChanges()));
    
    this->bordersDisplayCheckBox = new QCheckBox("Display Borders");
    QObject::connect(this->bordersDisplayCheckBox, SIGNAL(clicked(bool)),
                     this, SLOT(processBorderSelectionChanges()));
    
    this->borderClassNameHierarchyViewController = new ClassAndNameHierarchyViewController(this->browserWindowIndex);
    QObject::connect(this->borderClassNameHierarchyViewController, SIGNAL(itemSelected(ClassAndNameHierarchySelectedItem*)),
                     this, SLOT(bordersSelectionsChanged(ClassAndNameHierarchySelectedItem*)));
    
    
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(this->bordersDisplayCheckBox, 0);  
    layout->addWidget(this->bordersContralateralCheckBox, 0);  
    layout->addLayout(groupLayout, 0);  
    layout->addWidget(this->borderClassNameHierarchyViewController, 100);  
    
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_USER_INTERFACE_UPDATE);
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_TOOLBOX_UPDATE);
    
    BorderSelectionViewController::allBorderSelectionViewControllers.insert(this);
}

/**
 * Destructor.
 */
BorderSelectionViewController::~BorderSelectionViewController()
{
    EventManager::get()->removeAllEventsFromListener(this);
    
    BorderSelectionViewController::allBorderSelectionViewControllers.erase(this);
}

/**
 * Called when the border display group combo box is changed.
 */
void 
BorderSelectionViewController::borderDisplayGroupSelected(const DisplayGroupEnum::Enum displayGroup)
{
    /*
     * Update selected display group in model.
     */
    BrowserTabContent* browserTabContent = 
    GuiManager::get()->getBrowserTabContentForBrowserWindow(this->browserWindowIndex, false);
    const int32_t browserTabIndex = browserTabContent->getTabNumber();
    Brain* brain = GuiManager::get()->getBrain();
    DisplayPropertiesBorders* dsb = brain->getDisplayPropertiesBorders();
    dsb->setDisplayGroup(browserTabIndex,
                         displayGroup);
    
    /*
     * Since display group has changed, need to update controls
     */
    this->updateBorderSelectionViewController();
    
    /*
     * Apply the changes.
     */
    this->processBorderSelectionChanges();
}

/**
 * Called when the border selections are changed.
 * Updates border display information and redraws
 * graphics.
 */
void 
BorderSelectionViewController::bordersSelectionsChanged(ClassAndNameHierarchySelectedItem* /*selectedItem*/)
{
    this->processBorderSelectionChanges();
}

/**
 * Update the border selection widget.
 */
void 
BorderSelectionViewController::updateBorderSelectionViewController()
{
    BrowserTabContent* browserTabContent = 
    GuiManager::get()->getBrowserTabContentForBrowserWindow(this->browserWindowIndex, true);
    if (browserTabContent == NULL) {
        return;
    }
    
    const int32_t browserTabIndex = browserTabContent->getTabNumber();
    
    this->setWindowTitle("Borders");
    
    Brain* brain = GuiManager::get()->getBrain();
    DisplayPropertiesBorders* dsb = brain->getDisplayPropertiesBorders();
    
    this->bordersDisplayCheckBox->setChecked(dsb->isDisplayed(browserTabIndex));
    this->bordersContralateralCheckBox->setChecked(dsb->isContralateralDisplayed(browserTabIndex));
    this->bordersDisplayGroupComboBox->setSelectedDisplayGroup(dsb->getDisplayGroup(browserTabIndex));
    
    /*;
     * Get all of border files.
     */
    std::vector<BorderFile*> allBorderFiles;
    const int32_t numberOfBorderFiles = brain->getNumberOfBorderFiles();
    for (int32_t ibf= 0; ibf < numberOfBorderFiles; ibf++) {
        allBorderFiles.push_back(brain->getBorderFile(ibf));
    }
    
    /*
     * Update the class/name hierarchy
     */
    this->borderClassNameHierarchyViewController->updateContents(allBorderFiles);
}

/**
 * Update other selection toolbox since they should all be the same.
 */
void 
BorderSelectionViewController::updateOtherBorderSelectionViewControllers()
{
    for (std::set<BorderSelectionViewController*>::iterator iter = BorderSelectionViewController::allBorderSelectionViewControllers.begin();
         iter != BorderSelectionViewController::allBorderSelectionViewControllers.end();
         iter++) {
        BorderSelectionViewController* bsw = *iter;
        if (bsw != this) {
            bsw->updateBorderSelectionViewController();
        }
    }
}

/**
 * Gets called when border selections are changed.
 */
void 
BorderSelectionViewController::processBorderSelectionChanges()
{
    BrowserTabContent* browserTabContent = 
    GuiManager::get()->getBrowserTabContentForBrowserWindow(this->browserWindowIndex, false);
    const int32_t browserTabIndex = browserTabContent->getTabNumber();
    Brain* brain = GuiManager::get()->getBrain();
    DisplayPropertiesBorders* dsb = brain->getDisplayPropertiesBorders();
    dsb->setDisplayed(browserTabIndex,
                      this->bordersDisplayCheckBox->isChecked());
    dsb->setContralateralDisplayed(browserTabIndex,
                                   this->bordersContralateralCheckBox->isChecked());
    dsb->setDisplayGroup(browserTabIndex, 
                         this->bordersDisplayGroupComboBox->getSelectedDisplayGroup());
    
    
    this->processSelectionChanges();
}

/**
 * Issue update events after selections are changed.
 */
void 
BorderSelectionViewController::processSelectionChanges()
{
    this->updateOtherBorderSelectionViewControllers();
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
}

/**
 * Receive events from the event manager.
 * 
 * @param event
 *   Event sent by event manager.
 */
void 
BorderSelectionViewController::receiveEvent(Event* event)
{
    if (event->getEventType() == EventTypeEnum::EVENT_USER_INTERFACE_UPDATE) {
        EventUserInterfaceUpdate* uiEvent = dynamic_cast<EventUserInterfaceUpdate*>(event);
        CaretAssert(uiEvent);
        
        this->updateBorderSelectionViewController();
        event->setEventProcessed();
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
            this->updateBorderSelectionViewController();
        }
        
        tbEvent->setEventProcessed();
    }
}


