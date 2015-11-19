
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

#define __ANNOTATION_SPATIAL_MODIFICATION_DECLARE__
#include "AnnotationSpatialModification.h"
#undef __ANNOTATION_SPATIAL_MODIFICATION_DECLARE__

#include "CaretAssert.h"
using namespace caret;


    
/**
 * \class caret::AnnotationSpatialModification 
 * \brief Contains information for spatially modifying an annotation.
 * \ingroup Annotations
 */

/**
 * Apply a move or resize operation received from the GUI.
 *
 * @param handleSelected
 *     Annotatoion handle that is being processed by the user.
 * @param viewportWidth
 *     Width of viewport
 * @param viewportHeight
 *     Height of viewport
 * @param mousePressX
 *     Mouse pressed X-coordinate.
 * @param mousePressY
 *     Mouse pressed Y-coordinate.
 * @param mouseX
 *     Mouse X-coordinate.
 * @param mouseY
 *     Mouse Y-coordinate.
 * @param mouseDX
 *     Change in mouse X-coordinate.
 * @param mouseDY
 *     Change in mouse Y-coordinate.
 */
AnnotationSpatialModification::AnnotationSpatialModification(const AnnotationSizingHandleTypeEnum::Enum sizingHandleType,
                                                             const float viewportWidth,
                                                             const float viewportHeight,
                                                             const float mousePressX,
                                                             const float mousePressY,
                                                             const float mouseX,
                                                             const float mouseY,
                                                             const float mouseDX,
                                                             const float mouseDY)
: CaretObject(),
m_sizingHandleType(sizingHandleType),
m_viewportWidth(viewportWidth),
m_viewportHeight(viewportHeight),
m_mousePressX(mousePressX),
m_mousePressY(mousePressY),
m_mouseX(mouseX),
m_mouseY(mouseY),
m_mouseDX(mouseDX),
m_mouseDY(mouseDY)
{
    
}

/**
 * Destructor.
 */
AnnotationSpatialModification::~AnnotationSpatialModification()
{
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
AnnotationSpatialModification::toString() const
{
    return "AnnotationSpatialModification";
}

