
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

#define __ANNOTATION_POINT_SIZE_TEXT_DECLARE__
#include "AnnotationPointSizeText.h"
#undef __ANNOTATION_POINT_SIZE_TEXT_DECLARE__

#include "CaretAssert.h"
using namespace caret;


    
/**
 * \class caret::AnnotationPointSizeText 
 * \brief Text annotation with font height in points (one point is 1/72 of inch).
 * \ingroup Annotations
 */

/**
 * Constructor.
 *
 * @param attributeDefaultType
 *    Type for attribute defaults
 */
AnnotationPointSizeText::AnnotationPointSizeText(const AnnotationAttributesDefaultTypeEnum::Enum attributeDefaultType)
: AnnotationText(attributeDefaultType,
                 AnnotationTextFontSizeTypeEnum::POINTS)
{
    
}

/**
 * Destructor.
 */
AnnotationPointSizeText::~AnnotationPointSizeText()
{
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
AnnotationPointSizeText::AnnotationPointSizeText(const AnnotationPointSizeText& obj)
: AnnotationText(obj)
{
    this->copyHelperAnnotationPointSizeText(obj);
}

/**
 * Assignment operator.
 * @param obj
 *    Data copied from obj to this.
 * @return 
 *    Reference to this object.
 */
AnnotationPointSizeText&
AnnotationPointSizeText::operator=(const AnnotationPointSizeText& obj)
{
    if (this != &obj) {
        AnnotationText::operator=(obj);
        this->copyHelperAnnotationPointSizeText(obj);
    }
    return *this;    
}

/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void 
AnnotationPointSizeText::copyHelperAnnotationPointSizeText(const AnnotationPointSizeText& /*obj*/)
{
    
}

/**
 * @return The font point size.
 */
AnnotationTextFontPointSizeEnum::Enum
AnnotationPointSizeText::getFontPointSize() const
{
    return getFontPointSizeProtected();
}

/**
 * Set the font point size.
 *
 * @param fontPointSize
 *     New font point size.
 */
void
AnnotationPointSizeText::setFontPointSize(const AnnotationTextFontPointSizeEnum::Enum fontPointSize)
{
    setFontPointSizeProtected(fontPointSize);
}

/**
 * @return Size of font as a percentage of the viewport height.
 *
 * Range is zero to one hundred.
 */
float
AnnotationPointSizeText::getFontPercentViewportSize() const
{
    CaretAssertMessage(0, "This method should never be called for Point Size Text");
    return 10.0;
}

/**
 * Set the size of the font as a percentage of the viewport height.
 *
 * @param fontPercentViewportHeight
 *    New value for percentage of viewport height.
 *    Range is zero to one hundred.
 */
void
AnnotationPointSizeText::setFontPercentViewportSize(const float /*fontPercentViewportHeight*/)
{
    CaretAssertMessage(0, "This method should never be called for Point Size Text");
}



