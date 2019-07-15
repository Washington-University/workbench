
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

#define __TILE_TABS_GRID_ROW_COLUMN_ELEMENT_DECLARE__
#include "TileTabsGridRowColumnElement.h"
#undef __TILE_TABS_GRID_ROW_COLUMN_ELEMENT_DECLARE__

#include "CaretAssert.h"
using namespace caret;


    
/**
 * \class caret::TileTabsGridRowColumnElement
 * \brief Contents on an element in a Tile Tabs Configuration grid row or column.
 * \ingroup Common
 */

/**
 * Constructor.
 */
TileTabsGridRowColumnElement::TileTabsGridRowColumnElement()
: CaretObject()
{
    clear();
}

/**
 * Destructor.
 */
TileTabsGridRowColumnElement::~TileTabsGridRowColumnElement()
{
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
TileTabsGridRowColumnElement::TileTabsGridRowColumnElement(const TileTabsGridRowColumnElement& obj)
: CaretObject(obj)
{
    this->copyHelperTileTabsGridRowColumnElement(obj);
}

/**
 * Assignment operator.
 * @param obj
 *    Data copied from obj to this.
 * @return 
 *    Reference to this object.
 */
TileTabsGridRowColumnElement&
TileTabsGridRowColumnElement::operator=(const TileTabsGridRowColumnElement& obj)
{
    if (this != &obj) {
        CaretObject::operator=(obj);
        this->copyHelperTileTabsGridRowColumnElement(obj);
    }
    return *this;    
}

/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void 
TileTabsGridRowColumnElement::copyHelperTileTabsGridRowColumnElement(const TileTabsGridRowColumnElement& obj)
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
TileTabsGridRowColumnElement::clear()
{
    m_contentType    = TileTabsGridRowColumnContentTypeEnum::TAB;
    m_stretchType    = TileTabsGridRowColumnStretchTypeEnum::WEIGHT;
    m_percentStretch = 20.0;
    m_weightStretch  = 1.0;
}

/**
 * @return Content type (spacer or tab)
 */
TileTabsGridRowColumnContentTypeEnum::Enum
TileTabsGridRowColumnElement::getContentType() const
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
TileTabsGridRowColumnElement::setContentType(const TileTabsGridRowColumnContentTypeEnum::Enum contentType)
{
    m_contentType = contentType;
}

/**
 * @return The stretch type (percent or weight)
 */
TileTabsGridRowColumnStretchTypeEnum::Enum
TileTabsGridRowColumnElement::getStretchType() const
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
TileTabsGridRowColumnElement::setStretchType(const TileTabsGridRowColumnStretchTypeEnum::Enum stretchType)
{
    m_stretchType = stretchType;
}

/**
 * @return The percent stretch value.
 */
float
TileTabsGridRowColumnElement::getPercentStretch() const
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
TileTabsGridRowColumnElement::setPercentStretch(const float percentStretch)
{
    m_percentStretch = percentStretch;
}

/**
 * @return The weight stretch value.
 */
float
TileTabsGridRowColumnElement::getWeightStretch() const
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
TileTabsGridRowColumnElement::setWeightStretch(const float weightStretch)
{
    m_weightStretch = weightStretch;
}


/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString
TileTabsGridRowColumnElement::toString() const
{
    AString s("TileTabsGridRowColumnElement: ");
    s.append("ContentType=" + TileTabsGridRowColumnContentTypeEnum::toGuiName(m_contentType));
    s.append(" StretchType=" + TileTabsGridRowColumnStretchTypeEnum::toGuiName(m_stretchType));
    s.append(" PercentStretch=" + AString::number(m_percentStretch));
    s.append(" WeightStretch=" + AString::number(m_weightStretch));
    return s;
}

