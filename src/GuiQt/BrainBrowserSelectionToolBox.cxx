
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

#define __BRAIN_BROWSER_SELECTION_TOOL_BOX_DECLARE__
#include "BrainBrowserSelectionToolBox.h"
#undef __BRAIN_BROWSER_SELECTION_TOOL_BOX_DECLARE__

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
    
    BrainBrowserSelectionToolBox::allSelectionToolBoxes.insert(this);
}

/**
 * Destructor.
 */
BrainBrowserSelectionToolBox::~BrainBrowserSelectionToolBox()
{
    BrainBrowserSelectionToolBox::allSelectionToolBoxes.erase(this);
}

/*
 * Update this selection toolbox.
 */
void 
BrainBrowserSelectionToolBox::updateSelectionToolBox()
{
    
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
/*
    if (event->getEventType() == EventTypeEnum::EVENT_USER_INTERFACE_UPDATE) {
        EventUserInterfaceUpdate* uiEvent =
        dynamic_cast<EventUserInterfaceUpdate*>(event);
        CaretAssert(uiEvent);
        
        uiEvent->setEventProcessed();
        
        this->updateDisplayedPanel();
    }
    else if (event->getEventType() == EventTypeEnum::EVENT_INFORMATION_TEXT_DISPLAY) {
        EventInformationTextDisplay* textEvent =
        dynamic_cast<EventInformationTextDisplay*>(event);
        CaretAssert(textEvent);
        
        textEvent->setEventProcessed();
        
        const AString text = textEvent->getText();
        if (text.isEmpty() == false) {
            if (textEvent->isImportant()) {
                this->tabWidget->setCurrentWidget(this->informationWidget);
            }
            this->informationWidget->processTextEvent(textEvent);
            textEvent->setEventProcessed();
        }
    }
    else if ((event->getEventType() == EventTypeEnum::EVENT_SPEC_FILE_READ_DATA_FILES)
             || (event->getEventType() == EventTypeEnum::EVENT_SPEC_FILE_READ_DATA_FILES)) {
        this->tabWidget->setCurrentWidget(this->overlayWidget);
    }
    else {
    }
 */
}
