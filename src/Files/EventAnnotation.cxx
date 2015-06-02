
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

#define __EVENT_ANNOTATION_DECLARE__
#include "EventAnnotation.h"
#undef __EVENT_ANNOTATION_DECLARE__

#include "CaretAssert.h"
#include "EventTypeEnum.h"

using namespace caret;


    
/**
 * \class caret::EventAnnotation 
 * \brief Event for annotations.
 * \ingroup Files
 *
 * This event for annotation contains multiple modes.
 * Both "get" and "set" method are available for each mode.
 */

/**
 * Constructor.
 */
EventAnnotation::EventAnnotation()
: Event(EventTypeEnum::EVENT_ANNOTATION)
{
    m_mode = MODE_INVALID;
    m_annotationType = AnnotationTypeEnum::LINE;
}

/**
 * Destructor.
 */
EventAnnotation::~EventAnnotation()
{
}

/**
 * @return the mode.
 */
EventAnnotation::Mode
EventAnnotation::getMode() const
{
    return m_mode;
}

/**
 * @return Type of annotation for create new annotation mode.
 */
AnnotationTypeEnum::Enum
EventAnnotation::getModeCreateNewAnnotationType() const
{
    CaretAssert(m_mode == MODE_CREATE_NEW_ANNOTATION_TYPE);
    if (m_mode != MODE_CREATE_NEW_ANNOTATION_TYPE) {
        return AnnotationTypeEnum::LINE;
    }
    
    return m_annotationType;
}

/**
 * Set the mode for creating an new annotation of the given type.
 *
 * @param annotationType
 *     Type of annotation that will be created.
 * @return
 *     Reference to "self".
 */
EventAnnotation&
EventAnnotation::setModeCreateNewAnnotationType(const AnnotationTypeEnum::Enum annotationType)
{
    m_mode = MODE_CREATE_NEW_ANNOTATION_TYPE;
    m_annotationType = annotationType;
    
    return *this;
}
