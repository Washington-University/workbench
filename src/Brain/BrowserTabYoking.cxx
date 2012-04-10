
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
 * Constructor.
 */
BrowserTabYoking::BrowserTabYoking(BrowserTabContent* parentBrowserTabContent,
                                   ModelYokingGroup* selectedYokingGroup)
: CaretObject()
{
    this->parentBrowserTabContent = parentBrowserTabContent;
    this->selectedYokingGroup = selectedYokingGroup;
}

/**
 * Destructor.
 */
BrowserTabYoking::~BrowserTabYoking()
{
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

ModelYokingGroup* 
BrowserTabYoking::getSelectedYokingGroup()
{
    return this->selectedYokingGroup;
}

void 
BrowserTabYoking::setSelectedYokingGroup(ModelYokingGroup* selectedYokingGroup)
{
    this->selectedYokingGroup = selectedYokingGroup;
}

