
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

#define __EVENT_MOVIE_MANUAL_MODE_RECORDING_DECLARE__
#include "EventMovieManualModeRecording.h"
#undef __EVENT_MOVIE_MANUAL_MODE_RECORDING_DECLARE__

#include "CaretAssert.h"
#include "EventTypeEnum.h"

using namespace caret;


    
/**
 * \class caret::EventMovieManualModeRecording 
 * \brief Manual mode recording image capture
 * \ingroup GuiQt
 */

/**
 * Constructor.
 *
 * @param browserWindowIndex
 *     Index of the browser window
 * @param durationSeconds
 *     Time in seconds for manual recording (used with movie recording frame
 *     rate to determine number of images captured)
 */
EventMovieManualModeRecording::EventMovieManualModeRecording(const int32_t browserWindowIndex,
                                                             const float durationSeconds)
: Event(EventTypeEnum::EVENT_MOVIE_RECORDING_MANUAL_MODE_CAPTURE),
m_browserWindowIndex(browserWindowIndex),
m_durationSeconds(durationSeconds)
{
    
}

/**
 * Destructor.
 */
EventMovieManualModeRecording::~EventMovieManualModeRecording()
{
}

/**
 * @return Index of browser window or negative for all windows
 */
int32_t
EventMovieManualModeRecording::getBrowserWindowIndex() const
{
    return m_browserWindowIndex;
}

/**
 * @return Duration in seconds
 */
float
EventMovieManualModeRecording::getDurationSeconds() const
{
    return m_durationSeconds;
}
