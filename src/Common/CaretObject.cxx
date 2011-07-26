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

#include <typeinfo>

#include "CaretObject.h"

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
std::string
CaretObject::toString() const
{
    std::string typeName(typeid(*this).name());
    std::string s = "Type=" + typeName;
    return s;
}

