
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

#define ____ANNOTATION_ARRANGER_INPUTS__DECLARE__
#include "AnnotationArrangerInputs.h"
#undef ____ANNOTATION_ARRANGER_INPUTS__DECLARE__

#include "CaretAssert.h"
using namespace caret;


    
/**
 * \class caret::AnnotationArrangerInputs 
 * \brief  Contains information for aligning, distributing annotations
 * \ingroup Brain
 */

/**
 * Constructor.
 *
 * @param textRenderInterface
 *     Text render interface needed for bounds of text annotations.
 * @param alignment
 *     Alignment to direction.
 * @param windowIndex
 *     Index of window in which annotations are aligned.
 */
AnnotationArrangerInputs::AnnotationArrangerInputs(BrainOpenGLTextRenderInterface* textRenderInterface,
                                                                 const int32_t windowIndex)
: CaretObject(),
m_textRenderInterface(textRenderInterface),
m_windowIndex(windowIndex)
{
}

/**
 * Destructor.
 */
AnnotationArrangerInputs::~AnnotationArrangerInputs()
{
}

/**
 * @return Text renderer needed for text annotation sizing.
 */
BrainOpenGLTextRenderInterface*
AnnotationArrangerInputs::getTextRender() const
{
    return m_textRenderInterface;
}


/**
 * @return The window index.
 */
int32_t
AnnotationArrangerInputs::getWindowIndex() const
{
    return m_windowIndex;
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
AnnotationArrangerInputs::toString() const
{
    return "AnnotationArrangerInputs";
}

