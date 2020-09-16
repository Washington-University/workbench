
/*LICENSE_START*/
/*
 *  Copyright (C) 2020 Washington University School of Medicine
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

#define __EVENT_RECENT_FILES_SYSTEM_ACCESS_MODE_DECLARE__
#include "EventRecentFilesSystemAccessMode.h"
#undef __EVENT_RECENT_FILES_SYSTEM_ACCESS_MODE_DECLARE__

#include "CaretAssert.h"
#include "EventTypeEnum.h"

using namespace caret;


    
/**
 * \class caret::EventRecentFilesSystemAccessMode 
 * \brief Event to get the recent files system access mode
 * \ingroup Common
 */

/**
 * Constructor.
 */
EventRecentFilesSystemAccessMode::EventRecentFilesSystemAccessMode()
: Event(EventTypeEnum::EVENT_RECENT_FILES_SYSTEM_ACCESS_MODE)
{
    
}

/**
 * Destructor.
 */
EventRecentFilesSystemAccessMode::~EventRecentFilesSystemAccessMode()
{
}

/**
 * @return Mode for accessing the file system for recent files
 */
RecentFilesSystemAccessModeEnum::Enum
EventRecentFilesSystemAccessMode::getMode() const
{
    return m_mode;
}

/**
 * Set the mode for accessing the file system for recent files
 * @param mode
 *    New value for mode
 */
void
EventRecentFilesSystemAccessMode::setMode(const RecentFilesSystemAccessModeEnum::Enum mode)
{
    m_mode = mode;
}

