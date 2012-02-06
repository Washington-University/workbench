
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
#include <QStackedWidget>
#include <QVBoxLayout>

#define __BRAIN_BROWSER_SELECTION_TOOL_BOX_DECLARE__
#include "BrainBrowserSelectionToolBox.h"
#undef __BRAIN_BROWSER_SELECTION_TOOL_BOX_DECLARE__

#include "CaretAssert.h"
#include "EventManager.h"
#include "EventToolBoxSelectionDisplay.h"

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
     * Listen for events sent to this selection toolbox
     */
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_TOOLBOX_SELECTION_DISPLAY);    

    /*
     * Track each toolbox created
     */
    BrainBrowserSelectionToolBox::allSelectionToolBoxes.insert(this);    
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
    QLabel* label = new QLabel("Borders");
    QWidget* w = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(w);
    layout->addWidget(label);  
    
    return w;
}

/**
 * Update the border selection widget.
 */
void 
BrainBrowserSelectionToolBox::updateBorderSelectionWidget()
{
    this->setWindowTitle("Borders");
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
                    this->updateSelectionToolBox();
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
}
