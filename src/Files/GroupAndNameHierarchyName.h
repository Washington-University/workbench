#ifndef __CLASS_AND_NAME_HIERARCHY_NAME__H_
#define __CLASS_AND_NAME_HIERARCHY_NAME__H_

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

#include "GroupAndNameHierarchyItem.h"

namespace caret {

    class GroupAndNameHierarchyName : public GroupAndNameHierarchyItem {
    public:
        GroupAndNameHierarchyName(GroupAndNameHierarchyUserInterface* groupAndNameHierarchyUserInterface,
                                  const AString& name,
                                  const int32_t idNumber);
        
        ~GroupAndNameHierarchyName();
        
        // ADD_NEW_METHODS_HERE
    
    private:
        GroupAndNameHierarchyName(const GroupAndNameHierarchyName&);
        
        GroupAndNameHierarchyName& operator=(const GroupAndNameHierarchyName&);
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __CLASS_AND_NAME_HIERARCHY_NAME_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __CLASS_AND_NAME_HIERARCHY_NAME_DECLARE__

} // namespace
#endif  //__CLASS_AND_NAME_HIERARCHY_NAME__H_
