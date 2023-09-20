
/*LICENSE_START*/
/*
 *  Copyright (C) 2023 Washington University School of Medicine
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

#define __EVENT_ANNOTATION_GET_DRAWING_POLYHEDRON_SLICE_DEPTH_DECLARE__
#include "EventAnnotationGetDrawingPolyhedronSliceDepth.h"
#undef __EVENT_ANNOTATION_GET_DRAWING_POLYHEDRON_SLICE_DEPTH_DECLARE__

#include "CaretAssert.h"
#include "EventTypeEnum.h"

using namespace caret;


    
/**
 * \class caret::EventAnnotationGetDrawingPolyhedronSliceDepth 
 * \brief Get the depth used when drawing a new polyhedron
 * \ingroup GuiQt
 */

/**
 * Constructor.
 * @param userInputMode
 *    The user input mode
 * @param windowIndex
 *    The window index
 */
EventAnnotationGetDrawingPolyhedronSliceDepth::EventAnnotationGetDrawingPolyhedronSliceDepth(const UserInputModeEnum::Enum userInputMode,
                                                                                             const int32_t windowIndex)
: Event(EventTypeEnum::EVENT_ANNOTATION_NEW_DRAWING_POLYHEDRON_SLICE_DEPTH),
m_userInputMode(userInputMode),
m_windowIndex(windowIndex)
{
    
}

/**
 * Destructor.
 */
EventAnnotationGetDrawingPolyhedronSliceDepth::~EventAnnotationGetDrawingPolyhedronSliceDepth()
{
}

/**
 * @return The user input mode
 */
UserInputModeEnum::Enum
EventAnnotationGetDrawingPolyhedronSliceDepth::getUserInputMode() const
{
    return m_userInputMode;
}

/**
 * @return The window index
 */
int32_t
EventAnnotationGetDrawingPolyhedronSliceDepth::getWindowIndex() const
{
    return m_windowIndex;
}

/**
 * @return True if the number of slices depth is valid
 */
bool
EventAnnotationGetDrawingPolyhedronSliceDepth::isNumberOfSlicesDepthValid() const
{
    return m_numberOfSlicesDepthValidFlag;
}

/**
 * @return The depth in total number of slices
 */
int32_t
EventAnnotationGetDrawingPolyhedronSliceDepth::getNumberOfSlicesDepth() const
{
    return m_numberOfSlicesDepth;
}

/**
 * Set the slice depth
 * @param numberOfSlicesDepth
 *    New depth in total number of slices
 */
void
EventAnnotationGetDrawingPolyhedronSliceDepth::setNumberOfSlicesDepth(const float numberOfSlicesDepth)
{
    m_numberOfSlicesDepth = numberOfSlicesDepth;
    m_numberOfSlicesDepthValidFlag = true;
}

/**
 * @return True if the millimeters depth is valid
 */
bool
EventAnnotationGetDrawingPolyhedronSliceDepth::isMillimetersDepthValid() const
{
    return m_millimetersDepthValidFlag;
}

/**
 * @return The depth in millimeters
 */
float
EventAnnotationGetDrawingPolyhedronSliceDepth::getMillimetersDepth() const
{
    return m_millimetersDepth;
}

/**
 * Set the millimeters depth
 * @param millimetersDepth
 *    New depth in millimeters
 */
void
EventAnnotationGetDrawingPolyhedronSliceDepth::setMillimetersDepth(const float millimetersDepth)
{
    m_millimetersDepth = millimetersDepth;
    m_millimetersDepthValidFlag = true;
}
