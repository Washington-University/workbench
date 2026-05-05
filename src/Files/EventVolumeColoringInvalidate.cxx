
/*LICENSE_START*/
/*
 *  Copyright (C) 2024 Washington University School of Medicine
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

#define __EVENT_VOLUME_COLORING_INVALIDATE_DECLARE__
#include "EventVolumeColoringInvalidate.h"
#undef __EVENT_VOLUME_COLORING_INVALIDATE_DECLARE__

#include "CaretAssert.h"
#include "EventTypeEnum.h"

using namespace caret;


    
/**
 * \class caret::EventVolumeColoringInvalidate 
 * \brief Event to update coloring for one or all volume files
 * \ingroup Files
 */

/**
 * Constructor for coloring all volume files
 */
EventVolumeColoringInvalidate::EventVolumeColoringInvalidate()
: Event(EventTypeEnum::EVENT_VOLUME_COLORING_INVALIDATE),
m_volumeMappableInterface(NULL)
{
    
}

/**
 * Constructor for coloring one volume file
 * @param volumeMappableInterface
 *    Volume that is to have coloring updated
 */
EventVolumeColoringInvalidate::EventVolumeColoringInvalidate(const VolumeMappableInterface* volumeMappableInterface)
: Event(EventTypeEnum::EVENT_VOLUME_COLORING_INVALIDATE),
m_volumeMappableInterface(volumeMappableInterface)
{
    
}

/**
 * Destructor.
 */
EventVolumeColoringInvalidate::~EventVolumeColoringInvalidate()
{
}

/**
 * @return The volume to have coloring updated or NULL if all volumes to have coloring updated
 */
const VolumeMappableInterface*
EventVolumeColoringInvalidate::getVolumeMappableIterface() const
{
    return m_volumeMappableInterface;
}

