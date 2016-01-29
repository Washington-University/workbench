
/*LICENSE_START*/
/*
 *  Copyright (C) 2014  Washington University School of Medicine
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

#define __CLASS_AND_NAME_HIERARCHY_NAME_DECLARE__
#include "GroupAndNameHierarchyName.h"
#undef __CLASS_AND_NAME_HIERARCHY_NAME_DECLARE__

#include "CaretAssert.h"

using namespace caret;

/**
 * \class caret::GroupAndNameHierarchyName
 * \brief Maintains selection of a name in each 'DisplayGroupEnum'.
 *
 * Methods that operate on a boolean value are used to query and set
 * the selected status.  A separate method is provided to query
 * the 'check state'.  The 'check state' may be unchecked, checked, or
 * partially checked (some children checked but not all).
 */

/**
 * Constructor.
 *
 * @param name
 *    The name.
 * @param idNumber
 *    ID number assigned to the name.
 */
GroupAndNameHierarchyName::GroupAndNameHierarchyName(const AString& name,
                                                     const int32_t idNumber)
: GroupAndNameHierarchyItem(GroupAndNameHierarchyItem::ITEM_TYPE_NAME,
                           name,
                           idNumber)
{
}

/**
 * Destructor.
 */
GroupAndNameHierarchyName::~GroupAndNameHierarchyName()
{
}

