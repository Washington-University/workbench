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
#include <cmath>

#include "BrowserTabContent.h"
#include "BoundingBox.h"
#include "CaretAssert.h"
#include "ModelDisplayControllerYokingGroup.h"

#include "Brain.h"
#include "BrainOpenGL.h"
#include "Surface.h"

using namespace caret;

/**
 * Constructor.
 * @param surface - surface for this controller.
 *
 */
ModelDisplayControllerYokingGroup::ModelDisplayControllerYokingGroup(const int32_t yokingGroupIndex)
: ModelDisplayController(ModelDisplayControllerTypeEnum::MODEL_TYPE_YOKING,
                         YOKING_ALLOWED_YES,
                         ROTATION_ALLOWED_YES)
{
    this->initializeMembersModelDisplayControllerYokingGroup();
    this->yokingGroupIndex = yokingGroupIndex;
    this->yokingGroupName = ("Group "
                             + AString::number(this->yokingGroupIndex + 1));
    
    for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; i++) {
        this->dorsalView(i);
    }
}

/**
 * Destructor
 */
ModelDisplayControllerYokingGroup::~ModelDisplayControllerYokingGroup()
{
}

void
ModelDisplayControllerYokingGroup::initializeMembersModelDisplayControllerYokingGroup()
{
    this->yokingGroupIndex = -1;
}

/**
 * Get the name for use in a GUI.
 *
 * @param includeStructureFlag - Prefix label with structure to which
 *      this structure model belongs.
 * @return   Name for use in a GUI.
 *
 */
AString
ModelDisplayControllerYokingGroup::getNameForGUI(const bool /*includeStructureFlag*/) const
{
    return this->yokingGroupName;
}

/**
 * @return The name that should be displayed in the tab
 * displaying this model controller.
 */
AString 
ModelDisplayControllerYokingGroup::getNameForBrowserTab() const
{
    return this->yokingGroupName;
}

/**
 * @return The index of this yoking group.
 */
int32_t 
ModelDisplayControllerYokingGroup::getYokingGroupIndex() const
{
    return this->yokingGroupIndex;
}


