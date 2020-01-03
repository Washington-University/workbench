
/*LICENSE_START*/
/*
 *  Copyright (C) 2020 Washington University School of Medicine
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

#define __RECENT_FILE_ITEMS_FILTER_DECLARE__
#include "RecentFileItemsFilter.h"
#undef __RECENT_FILE_ITEMS_FILTER_DECLARE__

#include "CaretAssert.h"
using namespace caret;


    
/**
 * \class caret::RecentFileItemsFilter 
 * \brief Filters recent file items
 * \ingroup Common
 */

/**
 * Constructor.
 */
RecentFileItemsFilter::RecentFileItemsFilter()
: CaretObject()
{
    
}

/**
 * Destructor.
 */
RecentFileItemsFilter::~RecentFileItemsFilter()
{
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
RecentFileItemsFilter::RecentFileItemsFilter(const RecentFileItemsFilter& obj)
: CaretObject(obj)
{
    this->copyHelperRecentFileItemsFilter(obj);
}

/**
 * Assignment operator.
 * @param obj
 *    Data copied from obj to this.
 * @return 
 *    Reference to this object.
 */
RecentFileItemsFilter&
RecentFileItemsFilter::operator=(const RecentFileItemsFilter& obj)
{
    if (this != &obj) {
        CaretObject::operator=(obj);
        this->copyHelperRecentFileItemsFilter(obj);
    }
    return *this;    
}

/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void 
RecentFileItemsFilter::copyHelperRecentFileItemsFilter(const RecentFileItemsFilter& obj)
{
    
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
RecentFileItemsFilter::toString() const
{
    return "RecentFileItemsFilter";
}

