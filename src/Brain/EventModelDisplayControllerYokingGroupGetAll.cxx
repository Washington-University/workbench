/*LICENSE_START*/ 
/* 
 *  Copyright 1995-2002 Washington University School of Medicine 
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

#include <algorithm>

#include "EventModelDisplayControllerYokingGroupGetAll.h"
#include "ModelDisplayControllerYokingGroup.h"

using namespace caret;

/**
 * Constructor.
 */
EventModelDisplayControllerYokingGroupGetAll::EventModelDisplayControllerYokingGroupGetAll()
: Event(EventTypeEnum::EVENT_MODEL_DISPLAY_CONTROLLER_YOKING_GROUP_GET_ALL)
{
}

/**
 * Destructor.
 */
EventModelDisplayControllerYokingGroupGetAll::~EventModelDisplayControllerYokingGroupGetAll()
{
    
}

/**
 * Add yoking groups.
 * @param yokingGroups
 *     Yoking groups that are added.
 */
void 
EventModelDisplayControllerYokingGroupGetAll::addYokingGroup(ModelDisplayControllerYokingGroup* yokingGroup)
{
    this->yokingGroups.push_back(yokingGroup);
}

/**
 * Get yoking groups.
 * @param yokingGroups
 *     Yoking groups in which groups are loaded..
 */
void 
EventModelDisplayControllerYokingGroupGetAll::getYokingGroups(std::vector<ModelDisplayControllerYokingGroup*>& yokingGroupsOut) const
{
    yokingGroupsOut = this->yokingGroups;
}

