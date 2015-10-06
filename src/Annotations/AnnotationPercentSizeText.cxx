
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

#define __ANNOTATION_PERCENT_SIZE_TEXT_DECLARE__
#include "AnnotationPercentSizeText.h"
#undef __ANNOTATION_PERCENT_SIZE_TEXT_DECLARE__

#include "CaretAssert.h"
using namespace caret;


    
/**
 * \class caret::AnnotationPercentSizeText 
 * \brief Text annotation with font height a percentage of viewport height.
 * \ingroup Annotations
 */

/**
 * Constructor.
 *
 * @param attributeDefaultType
 *    Type for attribute defaults
 */
AnnotationPercentSizeText::AnnotationPercentSizeText(const AnnotationAttributesDefaultTypeEnum::Enum attributeDefaultType)
: AnnotationText(attributeDefaultType,
                 AnnotationTextFontSizeTypeEnum::PERCENTAGE_OF_VIEWPORT_HEIGHT)
{
    
}

/**
 * Destructor.
 */
AnnotationPercentSizeText::~AnnotationPercentSizeText()
{
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
AnnotationPercentSizeText::AnnotationPercentSizeText(const AnnotationPercentSizeText& obj)
: AnnotationText(obj)
{
    this->copyHelperAnnotationPercentSizeText(obj);
}

/**
 * Assignment operator.
 * @param obj
 *    Data copied from obj to this.
 * @return 
 *    Reference to this object.
 */
AnnotationPercentSizeText&
AnnotationPercentSizeText::operator=(const AnnotationPercentSizeText& obj)
{
    if (this != &obj) {
        AnnotationText::operator=(obj);
        this->copyHelperAnnotationPercentSizeText(obj);
    }
    return *this;    
}

/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void 
AnnotationPercentSizeText::copyHelperAnnotationPercentSizeText(const AnnotationPercentSizeText& /* obj */)
{
    
}

/**
 * @return Size of font as a percentage of the viewport height.
 *
 * Range is zero to one hundred.
 */
float
AnnotationPercentSizeText::getFontPercentViewportSize() const
{
    return getFontPercentViewportSizeProtected();
}

/**
 * Set the size of the font as a percentage of the viewport height.
 *
 * @param fontPercentViewportHeight
 *    New value for percentage of viewport height.
 *    Range is zero to one hundred.
 */
void
AnnotationPercentSizeText::setFontPercentViewportSize(const float fontPercentViewportHeight)
{
    setFontPercentViewportSizeProtected(fontPercentViewportHeight);
}


