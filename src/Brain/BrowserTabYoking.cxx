
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

#define __BROWSER_TAB_YOKING_DECLARE__
#include "BrowserTabYoking.h"
#undef __BROWSER_TAB_YOKING_DECLARE__

#include "BrainConstants.h"
#include "BrowserTabContent.h"

using namespace caret;


    
/**
 * \class BrowserTabYoking 
 * \brief Controls the yoking status for a browser tab.
 *
 * Controls the yoking status for a browser tab.
 */

/**
 * Constructor.
 */
BrowserTabYoking::BrowserTabYoking(BrowserTabContent* parentBrowserTabContent)
: CaretObject()
{
    if (BrowserTabYoking::allBrowserTabYoking.empty()) {
        BrowserTabYoking::allBrowserTabYoking.resize(BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS, 
                                                     NULL);
    }
    BrowserTabYoking::allBrowserTabYoking[parentBrowserTabContent->getTabNumber()] = this;
    
    this->parentBrowserTabContent = parentBrowserTabContent;
    this->yokedToBrowserTabContent = NULL;
    this->yokingType = YokingTypeEnum::OFF;
}

/**
 * Destructor.
 */
BrowserTabYoking::~BrowserTabYoking()
{
    BrowserTabYoking::allBrowserTabYoking[parentBrowserTabContent->getTabNumber()] = NULL;
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
BrowserTabYoking::toString() const
{
    return "BrowserTabYoking";
}

/**
 * @return Get the browser tab that is controlled by this yoking.
 */
BrowserTabContent* 
BrowserTabYoking::getParentBrowserTabContent()
{
    return this->parentBrowserTabContent;
}

/**
 * @return Browser tab to which this is yoked to.
 * May be NULL
 */
BrowserTabContent* 
BrowserTabYoking::getYokedToBrowserTabContent()
{
    std::vector<BrowserTabContent*> allYokableBrowserTabConent;
    this->getYokableBrowserTabContent(allYokableBrowserTabConent);
    
    if (this->yokedToBrowserTabContent != NULL) {
        if (std::find(allYokableBrowserTabConent.begin(),
                      allYokableBrowserTabConent.end(),
                      this->yokedToBrowserTabContent) == allYokableBrowserTabConent.end()) {
            this->yokedToBrowserTabContent = NULL;
        }
    }
    
    if (this->yokedToBrowserTabContent == NULL) {
        if (allYokableBrowserTabConent.empty() == false) {
            this->yokedToBrowserTabContent = allYokableBrowserTabConent[0];
        }
    }
                  
    return this->yokedToBrowserTabContent;
}

/**
 * Set the browser tab to which this is yoked.
 * @param yokedToBrowserTabConent
 *    Tab to which this is yoked.
 */
void 
BrowserTabYoking::setYokedToBrowserTabContent(BrowserTabContent* yokedToBrowserTabContent)
{
    this->yokedToBrowserTabContent = yokedToBrowserTabContent;
}

/**
 * Get all browser tabs to which this can be yoked.  Essentially,
 * this is all browser tabs that are active with the exception of
 * the browser tab that is the parent of this yoking.
 */
void 
BrowserTabYoking::getYokableBrowserTabContent(std::vector<BrowserTabContent*>& yokableBrowserTabContentOut) const
{
    yokableBrowserTabContentOut.clear();
    
    for (std::vector<BrowserTabYoking*>::iterator iter = BrowserTabYoking::allBrowserTabYoking.begin();
         iter != BrowserTabYoking::allBrowserTabYoking.end();
         iter++) {
        BrowserTabYoking* bty = *iter;
        if (bty != NULL) {
            if (bty != this) {
                yokableBrowserTabContentOut.push_back(bty->getParentBrowserTabContent());
            }
        }
    }
}

/**
 * @return The type of yoking.
 */
YokingTypeEnum::Enum 
BrowserTabYoking::getYokingType() const
{
    return this->yokingType;
}

/**
 * Set the type of yoking.
 * @param yokingType
 *    New value for yoking type.
 */
void 
BrowserTabYoking::setYokingType(YokingTypeEnum::Enum yokingType)
{
    this->yokingType = yokingType;
}

