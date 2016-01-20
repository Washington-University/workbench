
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

#define __DRAWN_WITH_TEXTURE_INTERFACE_DECLARE__
#include "DrawnWithTextureInterface.h"
#undef __DRAWN_WITH_TEXTURE_INTERFACE_DECLARE__

#include "CaretAssert.h"
using namespace caret;


    
/**
 * \class caret::DrawnWithTextureInterface 
 * \brief <REPLACE-WITH-ONE-LINE-DESCRIPTION>
 * \ingroup Common
 *
 * <REPLACE-WITH-THOROUGH DESCRIPTION>
 */

/**
 * Constructor.
 */
DrawnWithTextureInterface::DrawnWithTextureInterface()
{
    
}

/**
 * Destructor.
 */
DrawnWithTextureInterface::~DrawnWithTextureInterface()
{
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
DrawnWithTextureInterface::DrawnWithTextureInterface(const DrawnWithTextureInterface& obj)
: (obj)
{
    this->copyHelperDrawnWithTextureInterface(obj);
}

/**
 * Assignment operator.
 * @param obj
 *    Data copied from obj to this.
 * @return 
 *    Reference to this object.
 */
DrawnWithTextureInterface&
DrawnWithTextureInterface::operator=(const DrawnWithTextureInterface& obj)
{
    if (this != &obj) {
        ::operator=(obj);
        this->copyHelperDrawnWithTextureInterface(obj);
    }
    return *this;    
}

/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void 
DrawnWithTextureInterface::copyHelperDrawnWithTextureInterface(const DrawnWithTextureInterface& obj)
{
    
}

