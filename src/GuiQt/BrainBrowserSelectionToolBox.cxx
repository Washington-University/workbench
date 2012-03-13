
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

#include <iostream>

#include <QAction>
#include <QCheckBox>
#include <QComboBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QStackedWidget>
#include <QVBoxLayout>

#define __BRAIN_BROWSER_SELECTION_TOOL_BOX_DECLARE__
#include "BrainBrowserSelectionToolBox.h"
#undef __BRAIN_BROWSER_SELECTION_TOOL_BOX_DECLARE__

#include "BorderFile.h"
#include "Brain.h"
#include "BrowserTabContent.h"
#include "CaretAssert.h"
#include "ClassAndNameHierarchyModel.h"
#include "ClassAndNameHierarchySelectedItem.h"
#include "ClassAndNameHierarchyViewController.h"
#include "DisplayGroupEnumComboBox.h"
#include "DisplayPropertiesBorders.h"
#include "EventGraphicsUpdateAllWindows.h"
#include "EventManager.h"
#include "EventToolBoxSelectionDisplay.h"
#include "EventUserInterfaceUpdate.h"
#include "GuiManager.h"

using namespace caret;


    
/**
 * \class caret::BrainBrowserSelectionToolBox 
 * \brief Toolbox for data selections.
 *
 */
/**
 * Constructor.
 */
BrainBrowserSelectionToolBox::BrainBrowserSelectionToolBox(const int32_t browserWindowIndex,
                                                           QWidget* parent)
: QDockWidget(parent)
{
    this->browserWindowIndex = browserWindowIndex;
    
    /*
     * Right side only and do not float
     */
    this->setAllowedAreas(Qt::RightDockWidgetArea);
    this->setFeatures(QDockWidget::NoDockWidgetFeatures);

    /*
     * Create the selection widgets
     */
    this->borderSelectionWidget = this->createBorderSelectionWidget();
    
    /*
     * Create stacked widget for selection tools
     */
    this->stackedWidget = new QStackedWidget();
    this->stackedWidget->addWidget(this->borderSelectionWidget);
    this->setWidget(this->stackedWidget);

    /*
     * No title bar widget
     */
    //this->setTitleBarWidget(NULL);
    
    /*
     * Track each toolbox created
     */
    BrainBrowserSelectionToolBox::allSelectionToolBoxes.insert(this);    

    /*
     * Listen for events sent to this selection toolbox
     */
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_TOOLBOX_SELECTION_DISPLAY);    
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_USER_INTERFACE_UPDATE);
}

/**
 * Destructor.
 */
BrainBrowserSelectionToolBox::~BrainBrowserSelectionToolBox()
{
    EventManager::get()->removeAllEventsFromListener(this);
    
    BrainBrowserSelectionToolBox::allSelectionToolBoxes.erase(this);
}

/**
 * Create the border selection widget.
 * @return The border selection widget.
 */
QWidget* 
BrainBrowserSelectionToolBox::createBorderSelectionWidget()
{
    QLabel* groupLabel = new QLabel("Group");
    this->bordersDisplayGroupComboBox = new DisplayGroupEnumComboBox();
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

    
    QWidget* w = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(w);
    layout->addWidget(this->bordersDisplayCheckBox);  
    layout->addWidget(this->bordersContralateralCheckBox);  
    layout->addLayout(groupLayout);  
    layout->addWidget(this->borderClassNameHierarchyViewController);  
    
    return w;
}

/**
 * Called when the border display group combo box is changed.
 */
void 
BrainBrowserSelectionToolBox::borderDisplayGroupSelected(const DisplayGroupEnum::Enum displayGroup)
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
    this->updateSelectionToolBox();
    
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
BrainBrowserSelectionToolBox::bordersSelectionsChanged(ClassAndNameHierarchySelectedItem* /*selectedItem*/)
{
    this->processBorderSelectionChanges();
}

/**
 * Update the border selection widget.
 */
void 
BrainBrowserSelectionToolBox::updateBorderSelectionWidget()
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

/*
 * Update this selection toolbox.
 */
void 
BrainBrowserSelectionToolBox::updateSelectionToolBox()
{
    QWidget* w = this->stackedWidget->currentWidget();
    if (w == this->borderSelectionWidget) {
        this->updateBorderSelectionWidget();
    }
    else {
        CaretAssertMessage(0, "Invalid widget displayed in Selection Tool Box");
    }
}

/**
 * Update other selection toolbox since they should all be the same.
 */
void 
BrainBrowserSelectionToolBox::updateOtherSelectionToolBoxes()
{
    for (std::set<BrainBrowserSelectionToolBox*>::iterator iter = BrainBrowserSelectionToolBox::allSelectionToolBoxes.begin();
         iter != BrainBrowserSelectionToolBox::allSelectionToolBoxes.end();
         iter++) {
        BrainBrowserSelectionToolBox* stb = *iter;
        if (stb != this) {
            stb->updateSelectionToolBox();
        }
    }
}

/**
 * Gets called when border selections are changed.
 */
void 
BrainBrowserSelectionToolBox::processBorderSelectionChanges()
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
BrainBrowserSelectionToolBox::processSelectionChanges()
{
    this->updateOtherSelectionToolBoxes();
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
}

/**
 * Receive events from the event manager.
 * 
 * @param event
 *   Event sent by event manager.
 */
void 
BrainBrowserSelectionToolBox::receiveEvent(Event* event)
{
   if (event->getEventType() == EventTypeEnum::EVENT_TOOLBOX_SELECTION_DISPLAY) {
        EventToolBoxSelectionDisplay* tbEvent =
        dynamic_cast<EventToolBoxSelectionDisplay*>(event);
        CaretAssert(tbEvent);
        
       QAction* viewAction = this->toggleViewAction();
        const int32_t browserWindowIndex = tbEvent->getBrowserWindowIndex();
        if (browserWindowIndex == this->browserWindowIndex) {
            const bool isVisible = viewAction->isChecked();
            switch (tbEvent->getDisplayMode()) {
                case EventToolBoxSelectionDisplay::DISPLAY_MODE_DISPLAY_BORDERS:
                    this->updateSelectionToolBox();
                    if (isVisible == false) {
                        viewAction->trigger();
                    }
                    break;
                case EventToolBoxSelectionDisplay::DISPLAY_MODE_HIDE:
                    if (isVisible) {
                        viewAction->trigger();
                    }
                    break;
            }
            
            tbEvent->setEventProcessed();
        }
    }
   else if (event->getEventType() == EventTypeEnum::EVENT_USER_INTERFACE_UPDATE) {
       EventUserInterfaceUpdate* uiEvent = dynamic_cast<EventUserInterfaceUpdate*>(event);
       CaretAssert(uiEvent);
              
       this->updateSelectionToolBox();
       event->setEventProcessed();
   }
}
