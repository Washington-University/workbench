
/*LICENSE_START*/
/*
 *  Copyright (C) 2015 Washington University School of Medicine
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

#define __SELECTION_ITEM_MEDIA_IDENTIFICATION_SYMBOL_DECLARE__
#include "SelectionItemMediaIdentificationSymbol.h"
#undef __SELECTION_ITEM_MEDIA_IDENTIFICATION_SYMBOL_DECLARE__

#include "CaretAssert.h"
using namespace caret;


    
/**
 * \class caret::SelectionItemMediaIdentificationSymbol
 * \brief Identification symbol for media
 * \ingroup Brain
 */

/**
 * Constructor.
 */
SelectionItemMediaIdentificationSymbol::SelectionItemMediaIdentificationSymbol()
: SelectionItem(SelectionItemDataTypeEnum::MEDIA_IDENTIFICATION_SYMBOL)
{
    /*
     * Note: reset() is virtual so cannot call from constructor.
     */
    resetPrivate();
}

/**
 * Destructor.
 */
SelectionItemMediaIdentificationSymbol::~SelectionItemMediaIdentificationSymbol()
{
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
SelectionItemMediaIdentificationSymbol::SelectionItemMediaIdentificationSymbol(const SelectionItemMediaIdentificationSymbol& obj)
: SelectionItem(obj)
{
    this->copyHelperSelectionItemMediaIdentificationSymbol(obj);
}

/**
 * Assignment operator.
 * @param obj
 *    Data copied from obj to this.
 * @return 
 *    Reference to this object.
 */
SelectionItemMediaIdentificationSymbol&
SelectionItemMediaIdentificationSymbol::operator=(const SelectionItemMediaIdentificationSymbol& obj)
{
    if (this != &obj) {
        SelectionItem::operator=(obj);
        this->copyHelperSelectionItemMediaIdentificationSymbol(obj);
    }
    return *this;    
}

/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void 
SelectionItemMediaIdentificationSymbol::copyHelperSelectionItemMediaIdentificationSymbol(const SelectionItemMediaIdentificationSymbol& obj)
{
    m_pixelXYZ[0] = obj.m_pixelXYZ[0];
    m_pixelXYZ[1] = obj.m_pixelXYZ[1];
    m_pixelXYZ[2] = obj.m_pixelXYZ[2];
    m_pixelValid  = obj.m_pixelValid;
}

/**
 * Get the coordinates of the pixel identification symbol.
 *
 * @param xyzOut
 *    Coordinates of the pixel identification symbol.
 */
void
SelectionItemMediaIdentificationSymbol::getPixelXYZ(float xyzOut[3]) const
{
    xyzOut[0] = m_pixelXYZ[0];
    xyzOut[1] = m_pixelXYZ[1];
    xyzOut[2] = m_pixelXYZ[2];
}

/**
 * Set the coordinates of the pixel identification symbol.
 *
 * @param xyzOut
 *    Coordinates of the pixel identification symbol.
 */
void
SelectionItemMediaIdentificationSymbol::setPixelXYZ(const float xyz[3])
{
    m_pixelXYZ[0] = xyz[0];
    m_pixelXYZ[1] = xyz[1];
    m_pixelXYZ[2] = xyz[2];
    m_pixelValid  = true;
}

/**
 * Reset this selection item.
 */
void
SelectionItemMediaIdentificationSymbol::reset()
{
    SelectionItem::reset();
    
    resetPrivate();
}

/**
 * Reset this items data.
 */
void
SelectionItemMediaIdentificationSymbol::resetPrivate()
{
    m_pixelValid  = false;
    m_pixelXYZ[0] = 0.0;
    m_pixelXYZ[1] = 0.0;
    m_pixelXYZ[2] = 0.0;
}

/**
 * @return Is this selected item valid?
 */
bool
SelectionItemMediaIdentificationSymbol::isValid() const
{
    return m_pixelValid;
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString
SelectionItemMediaIdentificationSymbol::toString() const
{
    AString text = SelectionItem::toString();
    text += ("Pixel XYZ: " + AString::fromNumbers(m_pixelXYZ, 3, ", ") + "\n"
             + "Valid: " + AString::fromBool(m_pixelValid) + "\n");
    
    
    return text;
}

