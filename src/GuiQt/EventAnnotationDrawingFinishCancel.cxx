
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

#define __EVENT_ANNOTATION_DRAWING_FINISH_CANCEL_DECLARE__
#include "EventAnnotationDrawingFinishCancel.h"
#undef __EVENT_ANNOTATION_DRAWING_FINISH_CANCEL_DECLARE__

#include "CaretAssert.h"
#include "EventTypeEnum.h"

using namespace caret;


    
/**
 * \class caret::EventAnnotationDrawingFinishCancel 
 * \brief Event to finish or cancel an annotation being drawn
 * \ingroup GuiQt
 */

/**
 * Constructor.
 * @param mode
 *    The mode
 * @param browserWindowIndex
 *    Index of the browser window
 */
EventAnnotationDrawingFinishCancel::EventAnnotationDrawingFinishCancel(const Mode mode,
                                                                       const int32_t browserWindowIndex,
                                                                       const UserInputModeEnum::Enum userInputMode)
: Event(EventTypeEnum::EVENT_ANNOTATION_DRAWING_FINISH_CANCEL),
m_mode(mode),
m_browserWindowIndex(browserWindowIndex),
m_userInputMode(userInputMode)
{
    
}

/**
 * Destructor.
 */
EventAnnotationDrawingFinishCancel::~EventAnnotationDrawingFinishCancel()
{
}

/**
 * @return The mode
 */
EventAnnotationDrawingFinishCancel::Mode
EventAnnotationDrawingFinishCancel::getMode() const
{
    return m_mode;
}

/**
 * @return Index of the browser window
 */
int32_t
EventAnnotationDrawingFinishCancel::getBrowserWindowIndex() const
{
    return m_browserWindowIndex;
}

/**
 * @return The user input mode
 */
UserInputModeEnum::Enum
EventAnnotationDrawingFinishCancel::getUserInputMode() const
{
    return m_userInputMode;
}


