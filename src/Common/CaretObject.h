#ifndef __CARETOBJECT_H__
#define __CARETOBJECT_H__

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

#include <map>
#include <AString.h>
#include "SystemBacktrace.h"

namespace caret {
    
/**
 * A base class for all objects that are not derived
 * from third party libraries.
 */
class CaretObject {
protected:
    CaretObject();

    CaretObject(const CaretObject& o);
    
    CaretObject& operator=(const CaretObject& co);

public:    
    
    virtual ~CaretObject();


    virtual AString toString() const;
    
    AString className() const;
    
    static void printListOfObjectsNotDeleted(const bool showCallStack);

private:
    
    void copyHelper(const CaretObject& co);
    
    void initializeMembersCaretObject();

    typedef std::map<CaretObject*, SystemBacktrace> CARET_OBJECT_TRACKER_MAP;
    typedef CARET_OBJECT_TRACKER_MAP::iterator CARET_OBJECT_TRACKER_MAP_ITERATOR;
    
    static CARET_OBJECT_TRACKER_MAP allocatedObjects;
};

#ifdef __CARET_OBJECT_DECLARE_H__
    CaretObject::CARET_OBJECT_TRACKER_MAP CaretObject::allocatedObjects;
#endif //__CARET_OBJECT_DECLARE_H__
    
} // namespace

#endif // __CARETOBJECT_H__
