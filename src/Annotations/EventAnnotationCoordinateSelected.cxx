
/*LICENSE_START*/
/*
 *  Copyright (C) 2026 Washington University School of Medicine
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

#define __EVENT_ANNOTATION_COORDINATE_SELECTED_DECLARE__
#include "EventAnnotationCoordinateSelected.h"
#undef __EVENT_ANNOTATION_COORDINATE_SELECTED_DECLARE__

#include "CaretAssert.h"
#include "EventTypeEnum.h"

using namespace caret;


    
/**
 * \class caret::EventAnnotationCoordinateSelected 
 * \brief Event issued when an annotation coordinate is selected, usually by mouse and dragging
 * \ingroup Annotations
 */

/**
 * Constructor.
 * @param annotation
 *   Annotation selected
 * @param coordinateIndex
 *   Index of coordinate selected
 */
EventAnnotationCoordinateSelected::EventAnnotationCoordinateSelected(const Annotation* annotation,
                                                                     const int32_t coordinateIndex)
: Event(EventTypeEnum::EVENT_ANNOTATION_COORDINATE_SELECTED),
m_annotation(annotation),
m_coordinateIndex(coordinateIndex)
{
    
}

/**
 * Destructor.
 */
EventAnnotationCoordinateSelected::~EventAnnotationCoordinateSelected()
{
}

/**
 * @return Annotation selected
 */
const Annotation*
EventAnnotationCoordinateSelected::getAnnotation() const
{
    return m_annotation;
}

/**
 * @return Index of the coordinate selected
 */
int32_t
EventAnnotationCoordinateSelected::getCoordinateIndex() const
{
    return m_coordinateIndex;
}

