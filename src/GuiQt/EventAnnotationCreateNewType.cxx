
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

#define __EVENT_ANNOTATION_CREATE_NEW_TYPE_DECLARE__
#include "EventAnnotationCreateNewType.h"
#undef __EVENT_ANNOTATION_CREATE_NEW_TYPE_DECLARE__

#include "CaretAssert.h"
#include "EventTypeEnum.h"

using namespace caret;


    
/**
 * \class caret::EventAnnotationCreateNewType 
 * \brief Event that starts creationg of a new annotation with the annotation type.
 * \ingroup GuiQt
 */

/**
 * Constructor.
 */
EventAnnotationCreateNewType::EventAnnotationCreateNewType(const AnnotationTypeEnum::Enum annotationType)
: Event(EventTypeEnum::EVENT_ANNOTATION_CREATE_NEW_TYPE),
m_annotationType(annotationType)
{
    
}

/**
 * Destructor.
 */
EventAnnotationCreateNewType::~EventAnnotationCreateNewType()
{
}

/**
 * @return Type of annotation for creation.
 */
AnnotationTypeEnum::Enum
EventAnnotationCreateNewType::getAnnotationType() const
{
    return m_annotationType;
}


