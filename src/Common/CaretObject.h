#ifndef __CARETOBJECT_H__
#define __CARETOBJECT_H__

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

#include <map>
#include <string>

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


    virtual std::string toString() const;
    
    static void printListOfObjectsNotDeleted(const bool showCallStack);

private:
    /**
     * Info about an allocated object.
     */
    class CaretObjectInfo {
    public:
        CaretObjectInfo(const std::string& callStack);
        ~CaretObjectInfo();
        
        std::string callStack;
    };
    
    void copyHelper(const CaretObject& co);
    
    void initializeMembersCaretObject();

    typedef std::map<CaretObject*, CaretObjectInfo> CARET_OBJECT_TRACKER_MAP;
    typedef CARET_OBJECT_TRACKER_MAP::iterator CARET_OBJECT_TRACKER_MAP_ITERATOR;
    
    static CARET_OBJECT_TRACKER_MAP allocatedObjects;
};

#ifdef __CARET_OBJECT_DECLARE_H__
    CaretObject::CARET_OBJECT_TRACKER_MAP CaretObject::allocatedObjects;
#endif __CARET_OBJECT_DECLARE_H__
    
} // namespace

#endif // __CARETOBJECT_H__
