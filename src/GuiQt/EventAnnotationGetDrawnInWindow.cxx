
/*LICENSE_START*/
/*
 *  Copyright (C) 2016 Washington University School of Medicine
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

#define __EVENT_ANNOTATION_GET_DRAWN_IN_WINDOW_DECLARE__
#include "EventAnnotationGetDrawnInWindow.h"
#undef __EVENT_ANNOTATION_GET_DRAWN_IN_WINDOW_DECLARE__

#include "CaretAssert.h"
#include "EventTypeEnum.h"

using namespace caret;


    
/**
 * \class caret::EventAnnotationGetDrawnInWindow 
 * \brief Event to get annotations that are drawn in a window.
 * \ingroup GuiQt
 */

/**
 * Constructor.
 *
 * @param dataTypeMode
 *     Data type mode annotations or samples
 * @param windowIndex
 *     Index of window.
 */
EventAnnotationGetDrawnInWindow::EventAnnotationGetDrawnInWindow(const DataTypeMode dataTypeMode,
                                                                 const int32_t windowIndex)
: Event(EventTypeEnum::EVENT_ANNOTATION_GET_DRAWN_IN_WINDOW),
m_dataTypeMode(dataTypeMode),
m_windowIndex(windowIndex)
{
    
}

/**
 * Destructor.
 */
EventAnnotationGetDrawnInWindow::~EventAnnotationGetDrawnInWindow()
{
}

/**
 * @return The data type mode
 */
EventAnnotationGetDrawnInWindow::DataTypeMode
EventAnnotationGetDrawnInWindow::getDataTypeMode() const
{
    return m_dataTypeMode;
}

/**
 * @return Index of the window.
 */
int32_t
EventAnnotationGetDrawnInWindow::getWindowIndex() const
{
    return m_windowIndex;
}

/**
 * Add annotations that were drawn in the requested window.
 *
 * @param annotations
 *     Annotations added.
 */
void
EventAnnotationGetDrawnInWindow::addAnnotations(const std::vector<Annotation*>& annotations)
{
    m_annotations.insert(m_annotations.end(),
                         annotations.begin(),
                         annotations.end());
}

/**
 * Get annotations that were drawn in the requested window.
 *
 * @param annotationsOut
 *     Annotations output.
 */
void
EventAnnotationGetDrawnInWindow::getAnnotations(std::vector<Annotation*>& annotationsOut) const
{
    annotationsOut = m_annotations;
}

