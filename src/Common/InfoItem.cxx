
/*LICENSE_START*/
/*
 *  Copyright (C) 2022 Washington University School of Medicine
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

#define __INFO_ITEM_DECLARE__
#include "InfoItem.h"
#undef __INFO_ITEM_DECLARE__

#include "CaretAssert.h"

using namespace caret;


    
/**
 * \class caret::InfoItem 
 * \brief Contains an info item with name, value, and tooltip
 * \ingroup Common
 */

/**
 * Constructor.
 */
InfoItem::InfoItem(const AString& name,
                   const AString& value,
                   const AString& tooltip)
: CaretObject(),
m_name(name),
m_value(value),
m_tooltip(tooltip)
{
    
}

/**
 * Destructor.
 */
InfoItem::~InfoItem()
{
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
InfoItem::InfoItem(const InfoItem& obj)
: CaretObject(obj)
{
    this->copyHelperInfoItem(obj);
}

/**
 * Assignment operator.
 * @param obj
 *    Data copied from obj to this.
 * @return 
 *    Reference to this object.
 */
InfoItem&
InfoItem::operator=(const InfoItem& obj)
{
    if (this != &obj) {
        CaretObject::operator=(obj);
        this->copyHelperInfoItem(obj);
    }
    return *this;    
}

/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void 
InfoItem::copyHelperInfoItem(const InfoItem& obj)
{
    m_name    = obj.m_name;
    m_value   = obj.m_value;
    m_tooltip = obj.m_tooltip;
}

/**
 * @return Name of info item.
 */
AString
InfoItem::getName() const
{
    return m_name;
}

/**
 * @return Value of info item.
 */
AString
InfoItem::getValue() const
{
    return m_value;
}

/**
 * @return Tooltip of info item.
 */
AString
InfoItem::getToolTip() const
{
    return m_tooltip;
}


