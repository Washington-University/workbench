
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

#define __EVENT_MEDIA_FILES_GET_DECLARE__
#include "EventMediaFilesGet.h"
#undef __EVENT_MEDIA_FILES_GET_DECLARE__

#include "CaretAssert.h"
#include "EventTypeEnum.h"

using namespace caret;


    
/**
 * \class caret::EventMediaFilesGet 
 * \brief Get media type files
 * \ingroup Files
 */

/**
 * Constructor.
 */
EventMediaFilesGet::EventMediaFilesGet()
: Event(EventTypeEnum::EVENT_MEDIA_FILES_GET)
{
    
}

/**
 * Destructor.
 */
EventMediaFilesGet::~EventMediaFilesGet()
{
}

/**
 * Add a media file
 * @param dataFile
 *    Media file for adding
 */
void
EventMediaFilesGet::addMediaFile(MediaFile* dataFile)
{
    CaretAssert(dataFile);
    m_mediaFiles.push_back(dataFile);
}

/**
 * @return the media files
 */
std::vector<MediaFile*>
EventMediaFilesGet::getMediaFiles() const
{
    return m_mediaFiles;
}

