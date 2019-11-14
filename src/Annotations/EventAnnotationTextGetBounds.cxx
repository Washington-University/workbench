
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

#define __EVENT_ANNOTATION_TEXT_GET_BOUNDS_DECLARE__
#include "EventAnnotationTextGetBounds.h"
#undef __EVENT_ANNOTATION_TEXT_GET_BOUNDS_DECLARE__

#include "AnnotationText.h"
#include "CaretAssert.h"
#include "EventTypeEnum.h"

using namespace caret;
    
/**
 * \class caret::EventAnnotationTextGetBounds 
 * \brief Event to get bounds for drawing texty
 * \ingroup Annotations
 */

/**
 * Constructor.
 * @param text
 *    The text
 * @param viewportWidthForPercentageHeightText
 *    The text
 * @param viewportHeightForPercentageHeightText
 *    The text
 */
EventAnnotationTextGetBounds::EventAnnotationTextGetBounds(const AnnotationText& annotationText,
                                                           const int32_t viewportWidthForPercentageHeightText,
                                                           const int32_t viewportHeightForPercentageHeightText)
: Event(EventTypeEnum::EVENT_ANNOTATION_TEXT_GET_BOUNDS),
m_annotationText(annotationText),
m_viewportWidth(viewportWidthForPercentageHeightText),
m_viewportHeight(viewportHeightForPercentageHeightText)
{
    
}

/**
 * Destructor.
 */
EventAnnotationTextGetBounds::~EventAnnotationTextGetBounds()
{
}

/**
 * @return The text
 */
const AnnotationText&
EventAnnotationTextGetBounds::getAnnotationText() const
{
    return m_annotationText;
}

/**
 * @return Width of viewport
 */
int32_t
EventAnnotationTextGetBounds::getViewportWidth() const
{
    return m_viewportWidth;
}

/**
 * @return Height of viewport
 */
int32_t
EventAnnotationTextGetBounds::getViewportHeight() const
{
    return m_viewportHeight;
}

/**
 * @return Width of text
 */
float
EventAnnotationTextGetBounds::getTextWidth() const
{
    return m_textWidth;
}

/**
 * @return Height of text
 */
float
EventAnnotationTextGetBounds::getTextHeight() const
{
    return m_textHeight;
}

/**
 * Set the width and height of text and sets validity to true.
 *
 * @param textWidth
 *    Width of text
 * @param textHeight
 *    Height of text
 */
void
EventAnnotationTextGetBounds::setTextWidthHeight(const float textWidth,
                                                 const float textHeight)
{
    m_textWidth  = textWidth;
    m_textHeight = textHeight;
    m_validFlag  = true;
}

/**
 * @return True if text width/height has been set
 */
bool
EventAnnotationTextGetBounds::isValid() const
{
    return m_validFlag;
}
