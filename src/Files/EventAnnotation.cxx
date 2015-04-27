
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
 */

/**
 * Constructor.
 */
EventAnnotation::EventAnnotation()
: Event(EventTypeEnum::EVENT_ANNOTATION)
{
    m_mode = MODE_INVALID;
    m_annotation = NULL;
    m_browserWindowIndex = -1;
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
 * Set the mode data for annotation editing in the annotation dialog.
 *
 * @param browserWindowIndex
 *     Index of the browser window.
 * @param annotation
 *     Annotation being edited.
 */
void
EventAnnotation::setModeEditAnnotation(const int32_t browserWindowIndex,
                                                 Annotation* annotation)
{
    m_mode               = MODE_ANNOTATION_EDIT;
    m_browserWindowIndex = browserWindowIndex;
    m_annotation         = annotation;
}

/**
 * Get the data for annotation editing in the annotation dialog.
 *
 * @param browserWindowIndexOut
 *     Index of the browser window.
 * @param annotationOut
 *     Annotation being edited.
 */
void
EventAnnotation::getModeEditAnnotation(int32_t& browserWindowIndexOut,
                                         Annotation* & annotationOut) const
{
    CaretAssert(m_mode == MODE_ANNOTATION_EDIT);
    
    browserWindowIndexOut = m_browserWindowIndex;
    annotationOut         = m_annotation;
}

/**
 * Set the mode for deselecting all annotations.
 */
void
EventAnnotation::setModeDeselectAllAnnotations()
{
    m_mode = MODE_DESELECT_ALL_ANNOTATIONS;
}


