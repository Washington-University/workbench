
/*LICENSE_START*/
/*
 *  Copyright (C) 2018 Washington University School of Medicine
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

#define __TILE_TABS_ROW_COLUMN_ELEMENT_DECLARE__
#include "TileTabsRowColumnElement.h"
#undef __TILE_TABS_ROW_COLUMN_ELEMENT_DECLARE__

#include "CaretAssert.h"
using namespace caret;


    
/**
 * \class caret::TileTabsRowColumnElement 
 * \brief Contents on an element in a Tile Tabs Configuration row or column.
 * \ingroup Common
 */

/**
 * Constructor.
 */
TileTabsRowColumnElement::TileTabsRowColumnElement()
: CaretObject()
{
    clear();
}

/**
 * Destructor.
 */
TileTabsRowColumnElement::~TileTabsRowColumnElement()
{
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
TileTabsRowColumnElement::TileTabsRowColumnElement(const TileTabsRowColumnElement& obj)
: CaretObject(obj)
{
    this->copyHelperTileTabsRowColumnElement(obj);
}

/**
 * Assignment operator.
 * @param obj
 *    Data copied from obj to this.
 * @return 
 *    Reference to this object.
 */
TileTabsRowColumnElement&
TileTabsRowColumnElement::operator=(const TileTabsRowColumnElement& obj)
{
    if (this != &obj) {
        CaretObject::operator=(obj);
        this->copyHelperTileTabsRowColumnElement(obj);
    }
    return *this;    
}

/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void 
TileTabsRowColumnElement::copyHelperTileTabsRowColumnElement(const TileTabsRowColumnElement& obj)
{
    m_contentType    = obj.m_contentType;
    m_stretchType    = obj.m_stretchType;
    m_percentStretch = obj.m_percentStretch;
    m_weightStretch  = obj.m_weightStretch;
}

/**
 * Clear this instance.
 */
void
TileTabsRowColumnElement::clear()
{
    m_contentType    = TileTabsRowColumnContentTypeEnum::TAB;
    m_stretchType    = TileTabsRowColumnStretchTypeEnum::WEIGHT;
    m_percentStretch = 20.0;
    m_weightStretch  = 1.0;
}

/**
 * @return Content type (spacer or tab)
 */
TileTabsRowColumnContentTypeEnum::Enum
TileTabsRowColumnElement::getContentType() const
{
    return m_contentType;
}

/**
 * Set the content type (spacer or tab)
 *
 * @param contentType
 *     New value for content type.
 */
void
TileTabsRowColumnElement::setContentType(const TileTabsRowColumnContentTypeEnum::Enum contentType)
{
    m_contentType = contentType;
}

/**
 * @return The stretch type (percent or weight)
 */
TileTabsRowColumnStretchTypeEnum::Enum
TileTabsRowColumnElement::getStretchType() const
{
    return m_stretchType;
}

/**
 * Set the stretch type (percent or weight)
 *
 * @param stretchType
 *     New value for stretch type.
 */
void
TileTabsRowColumnElement::setStretchType(const TileTabsRowColumnStretchTypeEnum::Enum stretchType)
{
    m_stretchType = stretchType;
}

/**
 * @return The percent stretch value.
 */
float
TileTabsRowColumnElement::getPercentStretch() const
{
    return m_percentStretch;
}

/**
 * Set the percent stretch value.
 *
 * @param percentStretch
 *     New value for percent stretch.
 */
void
TileTabsRowColumnElement::setPercentStretch(const float percentStretch)
{
    m_percentStretch = percentStretch;
}

/**
 * @return The weight stretch value.
 */
float
TileTabsRowColumnElement::getWeightStretch() const
{
    return m_weightStretch;
}

/**
 * Set the weight stretch value.
 *
 * @param weightStretch
 *     New value for weight stretch.
 */
void
TileTabsRowColumnElement::setWeightStretch(const float weightStretch)
{
    m_weightStretch = weightStretch;
}


/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString
TileTabsRowColumnElement::toString() const
{
    AString s("TileTabsRowColumnElement: ");
    s.append("ContentType=" + TileTabsRowColumnContentTypeEnum::toGuiName(m_contentType));
    s.append(" StretchType=" + TileTabsRowColumnStretchTypeEnum::toGuiName(m_stretchType));
    s.append(" PercentStretch=" + AString::number(m_percentStretch));
    s.append(" WeightStretch=" + AString::number(m_weightStretch));
    return s;
}

