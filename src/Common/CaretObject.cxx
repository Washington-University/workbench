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

#include <iostream>
#include <typeinfo>
#include <utility>

#define __CARET_OBJECT_DECLARE_H__
#include "CaretObject.h"
#undef __CARET_OBJECT_DECLARE_H__

#include "CaretOMP.h"
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
#ifdef CARET_OBJECT_MEMORY_LEAK_TRACKER
    /*
     * Erase returns the number of objects deleted.
     * If zero, then the object has already been deleted.
     */
    uint64_t numDeleted;
#pragma omp critical
    {
        numDeleted = CaretObject::allocatedObjects.erase(this);
    }
    if (numDeleted <= 0) {
        std::cerr << "Destructor for a CaretObject called but the object is not allocated "
                  << "and this implies that the object has already been deleted.";
    }
#endif
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
#ifdef CARET_OBJECT_MEMORY_LEAK_TRACKER
    SystemBacktrace myBacktrace;
    SystemUtilities::getBackTrace(myBacktrace);
#pragma omp critical
    {
        CaretObject::allocatedObjects.insert(
                std::make_pair(this,
                                myBacktrace));
    }
#endif
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

#ifndef CARET_OBJECT_MEMORY_LEAK_TRACKER

    /** 
     * Do not print objects not deleted if NOT debug
     */
    void CaretObject::printListOfObjectsNotDeleted(const bool) { }
#else
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
            const unsigned long objectAddress = (long long)iter->first;
            const SystemBacktrace& backtrace = iter->second;
            // below will crash if item has been deleted
            //std::cout << caretObject->toString().toStdString() << std::endl;
            std::cout << "Address (hex)=" << std::hex << objectAddress << std::endl;
            if (showCallStack) {
                std::cout << backtrace.toSymbolString() << std::endl;
            }
            std::cout << std::endl;
            
            count++;
        }
    }
    
    if (count > 0) {
        std::cout << std::dec << count << " objects were not deleted." << std::endl;
    }
}
#endif


