
/*LICENSE_START*/
/* 
 *  Copyright 1995-2011 Washington University School of Medicine 
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

#define __IDENTIFICATION_ITEM_DECLARE__
#include "IdentificationItem.h"
#undef __IDENTIFICATION_ITEM_DECLARE__

using namespace caret;

#include "IdentificationItemDataTypeEnum.h"

/**
 * \class IdentificationItem
 * \brief Abstract class for identified items.
 *
 * Abstract class for identified items.
 */

/**
 * Constructor.
 */
IdentificationItem::IdentificationItem(const IdentificationItemDataTypeEnum::Enum itemDataType)
: CaretObject()
{
    this->itemDataType = itemDataType;
    this->enabledForSelection = true;
    this->brain = NULL;
    this->screenDepth = 0.0;
}

/**
 * Destructor.
 */
IdentificationItem::~IdentificationItem()
{
    
}

/**
 * Reset this selection item.  Deriving
 * classes should override this method to
 * reset its selection data and also call
 * the method in this class.
 */
void 
IdentificationItem::reset()
{
    this->brain = NULL;
    this->screenDepth = 0.0;
}


/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
IdentificationItem::toString() const
{
    AString text = "";
    text += "Depth: " + AString::number(screenDepth); 
    return text;
}

/**
 * @return The type of identified item.
 */
IdentificationItemDataTypeEnum::Enum 
IdentificationItem::getItemDataType() const
{
    return this->itemDataType;
}

/**
 * @return Data type enabled for selection.
 */
bool 
IdentificationItem::isEnabledForSelection() const
{
    return this->enabledForSelection;
}

/**
 * Set the data type enabled for selection.
 * @param enabled
 *    New value for selection enabled status.
 */
void 
IdentificationItem::setEnabledForSelection(const bool enabled)
{
    this->enabledForSelection = enabled;
}

/**
 * @return Brain in which identification item resides.
 */
Brain* 
IdentificationItem::getBrain()
{
    return this->brain;
}

/**
 * Set the brain.
 *
 * param brain
 *    Brain in which identification item resides.
 */
void 
IdentificationItem::setBrain(Brain* brain)
{
    this->brain = brain;
}

/**
 * @return Screen depth of item.
 */
float 
IdentificationItem::getScreenDepth() const
{
    return this->screenDepth;
}

/**
 * Set the screen depth of the item.
 * @param screenDepth
 *    New value for screen depth.
 */
void 
IdentificationItem::setScreenDepth(const float screenDepth)
{
    this->screenDepth = screenDepth;
}




