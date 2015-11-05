
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

#define __ANNOTATION_COLOR_BAR_SECTION_DECLARE__
#include "AnnotationColorBarSection.h"
#undef __ANNOTATION_COLOR_BAR_SECTION_DECLARE__

#include "CaretAssert.h"
using namespace caret;


    
/**
 * \class caret::AnnotationColorBarSection 
 * \brief Contains scalars and coloring for one section of the colorbar.
 * \ingroup Annotations
 */

/**
 * Constructor.
 * 
 * @param startScalar
 *     Value of the starting scalar.
 * @param endScalar
 *     Value of the ending scalar.
 * @param startRGBA
 *     RGBA coloring at the starting scalar.
 * @param endRGBA
 *     RGBA coloring at the ending scalar.
 */
AnnotationColorBarSection::AnnotationColorBarSection(const float startScalar,
                                                     const float endScalar,
                                                     const float startRGBA[4],
                                                     const float endRGBA[4])
: CaretObject(),
m_startScalar(startScalar),
m_endScalar(endScalar)
{
    for (int32_t i = 0; i < 4; i++) {
        m_startRGBA[i] = startRGBA[i];
        m_endRGBA[i]   = endRGBA[i];
    }
}

/**
 * Destructor.
 */
AnnotationColorBarSection::~AnnotationColorBarSection()
{
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
AnnotationColorBarSection::AnnotationColorBarSection(const AnnotationColorBarSection& obj)
: CaretObject(obj)
{
    this->copyHelperAnnotationColorBarSection(obj);
}

/**
 * Assignment operator.
 * @param obj
 *    Data copied from obj to this.
 * @return 
 *    Reference to this object.
 */
AnnotationColorBarSection&
AnnotationColorBarSection::operator=(const AnnotationColorBarSection& obj)
{
    if (this != &obj) {
        CaretObject::operator=(obj);
        this->copyHelperAnnotationColorBarSection(obj);
    }
    return *this;    
}

/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void 
AnnotationColorBarSection::copyHelperAnnotationColorBarSection(const AnnotationColorBarSection& obj)
{
    m_startScalar = obj.m_startScalar;
    m_endScalar   = obj.m_endScalar;
    
    for (int32_t i = 0; i < 4; i++) {
        m_startRGBA[i] = obj.m_startRGBA[i];
        m_endRGBA[i]   = obj.m_endRGBA[i];
    }
}

