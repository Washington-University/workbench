
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

#define __EVENT_ANNOTATION_GET_BEING_DRAWN_IN_WINDOW_DECLARE__
#include "EventAnnotationGetBeingDrawnInWindow.h"
#undef __EVENT_ANNOTATION_GET_BEING_DRAWN_IN_WINDOW_DECLARE__

#include "CaretAssert.h"
#include "EventTypeEnum.h"
using namespace caret;


    
/**
 * \class caret::EventAnnotationGetBeingDrawnInWindow 
 * \brief Event that gets the annotation being drawn in a browser window
 * \ingroup Brain
 */

/**
 * Constructor.
 * @param userInputMode
 *    The user input mode
 * @param browserWindowIndex
 *   The window index
 */
EventAnnotationGetBeingDrawnInWindow::EventAnnotationGetBeingDrawnInWindow(const UserInputModeEnum::Enum userInputMode,
                                                                           const int32_t browserWindowIndex)
: Event(EventTypeEnum::EVENT_ANNOTATION_GET_BEING_DRAWN_IN_WINDOW),
m_userInputMode(userInputMode),
m_browserWindowIndex(browserWindowIndex)
{
    
}

/**
 * Destructor.
 */
EventAnnotationGetBeingDrawnInWindow::~EventAnnotationGetBeingDrawnInWindow()
{
}

/**
 * @return The user input mode
 */
UserInputModeEnum::Enum
EventAnnotationGetBeingDrawnInWindow::getUserInputMode() const
{
    return m_userInputMode;
}

/**
 * @return The index of the browser window
 */
int32_t
EventAnnotationGetBeingDrawnInWindow::getBrowserWindowIndex() const
{
    return m_browserWindowIndex;
}

/**
 * @return The annotation being drawn in window
 */
Annotation*
EventAnnotationGetBeingDrawnInWindow::getAnnotation() const
{
    return m_annotation;
}

int32_t
EventAnnotationGetBeingDrawnInWindow::getDrawingViewportHeight() const
{
    return m_drawingViewportHeight;
}

/**
 * Set the annotation being drawn in window
 * @param annotation
 *   The annotation
 * @param drawingViewportHeight
 *    Height of viewport in which annotation is drawn.  Valid if greater than zero
 *    and used for volume montage height
 */
void
EventAnnotationGetBeingDrawnInWindow::setAnnotation(Annotation* annotation,
                                                    const int32_t drawingViewportHeight)
{
    m_annotation            = annotation;
    m_drawingViewportHeight = drawingViewportHeight;
}

/**
 * @return True if annotation drawing is in progress
 */
bool
EventAnnotationGetBeingDrawnInWindow::isAnnotationDrawingInProgress() const
{
    return m_annotationDrawingInProgressFlag;
}

/**
 * Set annotation drawing is in progress
 * @param statusFlag
 *    True if drawing annotation
 */
void
EventAnnotationGetBeingDrawnInWindow::setAnnotationDrawingInProgress(const bool statusFlag)
{
    m_annotationDrawingInProgressFlag = statusFlag;
}

/**
 * @return Is the annotation being drawn selectable?
 */
bool
EventAnnotationGetBeingDrawnInWindow::isAnnontationBeingDrawnSelectable() const
{
    return m_annotationBeingDrawnSelectableFlag;
}


