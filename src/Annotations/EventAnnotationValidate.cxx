
/*LICENSE_START*/
/*
 *  Copyright (C) 2019 Washington University School of Medicine
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

#define __EVENT_ANNOTATION_VALIDATE_DECLARE__
#include "EventAnnotationValidate.h"
#undef __EVENT_ANNOTATION_VALIDATE_DECLARE__

#include "CaretAssert.h"
#include "EventTypeEnum.h"

using namespace caret;


    
/**
 * \class caret::EventAnnotationValidate 
 * \brief Verify that an annotation pointer is valid (annotation exists)
 * \ingroup Annotations
 */

/**
 * Constructor.
 *
 * @param annotation
 *     Annotation tested for validity
 */
EventAnnotationValidate::EventAnnotationValidate(const Annotation* annotation)
: Event(EventTypeEnum::EVENT_ANNOTATION_VALIDATE),
m_annotation(annotation)
{
    CaretAssert(m_annotation);
    m_validFlag = false;
}

/**
 * Destructor.
 */
EventAnnotationValidate::~EventAnnotationValidate()
{
}

/**
 * @return Pointer to annotation being tested for validity
 */
const Annotation*
EventAnnotationValidate::getAnnotation() const
{
    return m_annotation;
}

/**
 * @return Validity status of annotation
 */
bool
EventAnnotationValidate::isAnnotationValid() const
{
    return m_validFlag;
}

/**
 * Set the annotation as valid
 */
void
EventAnnotationValidate::setAnnotationValid()
{
    m_validFlag = true;
}

