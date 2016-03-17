
/*LICENSE_START*/
/*
 *  Copyright (C) 2016 Washington University School of Medicine
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

#define __DISPLAY_GROUP_AND_TAB_ITEM_INTERFACE_DECLARE__
#include "DisplayGroupAndTabItemInterface.h"
#undef __DISPLAY_GROUP_AND_TAB_ITEM_INTERFACE_DECLARE__

#include "CaretAssert.h"
using namespace caret;


    
/**
 * \class caret::DisplayGroupAndTabItemInterface 
 * \brief <REPLACE-WITH-ONE-LINE-DESCRIPTION>
 * \ingroup Common
 *
 * <REPLACE-WITH-THOROUGH DESCRIPTION>
 */

/**
 * Constructor.
 */
DisplayGroupAndTabItemInterface::DisplayGroupAndTabItemInterface()
: CaretObject()
{
    
}

/**
 * Destructor.
 */
DisplayGroupAndTabItemInterface::~DisplayGroupAndTabItemInterface()
{
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
DisplayGroupAndTabItemInterface::DisplayGroupAndTabItemInterface(const DisplayGroupAndTabItemInterface& obj)
: CaretObject(obj)
{
    this->copyHelperDisplayGroupAndTabItemInterface(obj);
}

/**
 * Assignment operator.
 * @param obj
 *    Data copied from obj to this.
 * @return 
 *    Reference to this object.
 */
DisplayGroupAndTabItemInterface&
DisplayGroupAndTabItemInterface::operator=(const DisplayGroupAndTabItemInterface& obj)
{
    if (this != &obj) {
        CaretObject::operator=(obj);
        this->copyHelperDisplayGroupAndTabItemInterface(obj);
    }
    return *this;    
}

/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void 
DisplayGroupAndTabItemInterface::copyHelperDisplayGroupAndTabItemInterface(const DisplayGroupAndTabItemInterface& obj)
{
    
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
DisplayGroupAndTabItemInterface::toString() const
{
    return "DisplayGroupAndTabItemInterface";
}

