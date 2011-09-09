
/*LICENSE_START*/
/* 
 *  Copyright 1995-2011 Washington University School of Medicine 
 * 
 *  http://brainmap.wustl.edu 
 * 
 *  This file is part of CARET. 
 * 
 *  CARET is free software; you can redistribute it and/or modify 
 *  it under the terms of the GNU General Public License as published by 
 *  the Free Software Foundation; either version 2 of the License, or 
 *  (at your option) any later version. 
 * 
 *  CARET is distributed in the hope that it will be useful, 
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of 
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
 *  GNU General Public License for more details. 
 * 
 *  You should have received a copy of the GNU General Public License 
 *  along with CARET; if not, write to the Free Software 
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA 
 * 
 */ 

#define __BROWSER_TAB_CONTENT_DECLARE__
#include "BrowserTabContent.h"
#undef __BROWSER_TAB_CONTENT_DECLARE__

#include "EventGetModelDisplayControllers.h"
#include "EventManager.h"

using namespace caret;


/**
 * Constructor.
 * @param tabNumber
 *    Number for this tab.
 */
BrowserTabContent::BrowserTabContent(const int32_t tabNumber)
: CaretObject()
{
    this->tabNumber = tabNumber;
    this->displayedModel = NULL;
}

/**
 * Destructor.
 */
BrowserTabContent::~BrowserTabContent()
{
    
}

/**
 * Get the name of this browser tab.
 * Name priority is (1) name set by user, (2) name set by
 * user-interface, and (3) the default name.
 *
 * @return  Name of this tab.
 */
AString 
BrowserTabContent::getName() const
{
    AString s = "(" + AString::number(this->tabNumber + 1) + ") ";
    
    if (this->userName.isEmpty() == false) {
        s += userName;
    }
    else if (this->guiName.isEmpty() == false) {
        s += this->guiName;   
    }
    
    return s;
}

/**
 * Set the name of this tab.  This is typically
 * called by the user-interface with a name that
 * indicates the content of the tab.
 *
 * @param guiName
 *    New name for tab.
 */
void 
BrowserTabContent::setGuiName(const AString& guiName)
{
    this->guiName = guiName;
}

/**
 * Set the user name of this tab.  The user name
 * overrides the default naming.
 *
 * @param userName
 *    User name for tab.
 */
void 
BrowserTabContent::setUserName(const AString& userName)
{
    this->userName = userName;
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
BrowserTabContent::toString() const
{
    return "WindowTabContent";
}

ModelDisplayController* 
BrowserTabContent::getDisplayedModel()
{
    EventGetModelDisplayControllers modelsEvent;
    EventManager::get()->sendEvent(modelsEvent.getPointer());
    
    if (modelsEvent.isModelDisplayControllerValid(this->displayedModel) == false) {
        this->displayedModel = modelsEvent.getFirstModelDisplayController();
    }
    return this->displayedModel;
}

void 
BrowserTabContent::setDisplayedModel(ModelDisplayController* model)
{
    this->displayedModel = model;
}




