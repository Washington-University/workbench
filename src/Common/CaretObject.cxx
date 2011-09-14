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

#include <iostream>
#include <typeinfo>

#define __CARET_OBJECT_DECLARE_H__
#include "CaretObject.h"
#undef __CARET_OBJECT_DECLARE_H__

#include "SystemUtilities.h"

using namespace caret;

/**
 * Constructor.
 *
 */
CaretObject::CaretObject()
{
    this->initializeMembersCaretObject();
}

/**
 * Copy constructor.
 *
 */
CaretObject::CaretObject(const CaretObject& co)
{
    this->initializeMembersCaretObject();
    this->copyHelper(co);
}

/**
 * Destructor
 */
CaretObject::~CaretObject()
{
    /*
     * Erase returns the number of objects deleted.
     * If zero, then the object has already been deleted.
     */
    uint64_t numDeleted = CaretObject::allocatedObjects.erase(this);
    if (numDeleted <= 0) {
        std::cerr << "Destructor for a CaretObject called but the object is not allocated "
                  << "and this implies that the object has already been deleted.";
    }
}

CaretObject& 
CaretObject::operator=(const CaretObject& co)
{
    if (this != &co) {
        this->copyHelper(co);
    }
    return *this;
}

void
CaretObject::initializeMembersCaretObject()
{
    CaretObject::allocatedObjects.insert(
           std::make_pair(this, 
                          SystemUtilities::getBackTrace()));
}

void 
CaretObject::copyHelper(const CaretObject&)
{
    
}

/**
 * Get String representation of caret object.
 * @return String containing caret object.
 *
 */
AString
CaretObject::toString() const
{
    AString s = "CaretObjectType=" + this->className();
    return s;
}

/**
 * Get the class name of this object.
 * @return 
 *    Class name of the object.
 */
AString 
CaretObject::className() const
{
    AString name(typeid(*this).name());
    return name;    
}

/**
 * Print a list of CaretObjects that were not deleted.
 */
void 
CaretObject::printListOfObjectsNotDeleted(const bool showCallStack)
{
    int count = 0;
    
    if (CaretObject::allocatedObjects.empty() == false) {
        std::cout << "These Caret Objects were not deleted:" << std::endl;
        for (CARET_OBJECT_TRACKER_MAP_ITERATOR iter = CaretObject::allocatedObjects.begin();
             iter != allocatedObjects.end();
             iter++) {
            const CaretObject* caretObject = iter->first;
            const CaretObjectInfo& caretObjectInfo = iter->second;
            std::cout << caretObject->toString().toStdString() << std::endl;
            if (showCallStack) {
                std::cout << caretObjectInfo.callStack.toStdString() << std::endl;
            }
            std::cout << std::endl;
            
            count++;
        }
    }
    
    if (count > 0) {
        std::cout << count << " objects were not deleted." << std::endl;
    }
}

/**
 * Constructor.
 * @param caretObject
 *     A caret object.
 * @param callStack
 *     A callstack showing where the object was created.
 */
CaretObject::CaretObjectInfo::CaretObjectInfo(const AString& callStack)
{
    this->callStack   = callStack;
}

/**
 * Destructor.
 */
CaretObject::CaretObjectInfo::~CaretObjectInfo()
{
    
}

