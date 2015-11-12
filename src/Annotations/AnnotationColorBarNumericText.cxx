
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

#define __ANNOTATION_COLOR_BAR_NUMERIC_TEXT_DECLARE__
#include "AnnotationColorBarNumericText.h"
#undef __ANNOTATION_COLOR_BAR_NUMERIC_TEXT_DECLARE__

#include "CaretAssert.h"
using namespace caret;


    
/**
 * \class caret::AnnotationColorBarNumericText 
 * \brief Numeric text that is displayed above the color bar.
 * \ingroup Annotations
 */

/**
 * Constructor.
 *
 * @param scalar
 *     Scalar for position of text.
 * @param numericText
 *     Text that is displayed.
 * @param horizontalAlignment
 *     Horizontal alignment for the text.
 * @param drawTickMarkAtScalar
 *     If true, a tick mark is drawn at the scalar
 */
AnnotationColorBarNumericText::AnnotationColorBarNumericText(const float scalar,
                                                             const AString& numericText,
                                                             const AnnotationTextAlignHorizontalEnum::Enum horizontalAlignment,
                                                             const bool drawTickMarkAtScalar)
: CaretObject(),
m_scalar(scalar),
m_numericText(numericText),
m_horizontalAlignment(horizontalAlignment),
m_drawTickMarkAtScalar(drawTickMarkAtScalar)
{
    
}

/**
 * Destructor.
 */
AnnotationColorBarNumericText::~AnnotationColorBarNumericText()
{
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
AnnotationColorBarNumericText::AnnotationColorBarNumericText(const AnnotationColorBarNumericText& obj)
: CaretObject(obj)
{
    this->copyHelperAnnotationColorBarNumericText(obj);
}

/**
 * Assignment operator.
 * @param obj
 *    Data copied from obj to this.
 * @return 
 *    Reference to this object.
 */
AnnotationColorBarNumericText&
AnnotationColorBarNumericText::operator=(const AnnotationColorBarNumericText& obj)
{
    if (this != &obj) {
        CaretObject::operator=(obj);
        this->copyHelperAnnotationColorBarNumericText(obj);
    }
    return *this;    
}

/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void 
AnnotationColorBarNumericText::copyHelperAnnotationColorBarNumericText(const AnnotationColorBarNumericText& obj)
{
    m_scalar      = obj.m_scalar;
    m_numericText = obj.m_numericText;
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
AnnotationColorBarNumericText::toString() const
{
    return "AnnotationColorBarNumericText";
}

