
/*LICENSE_START*/
/*
 *  Copyright (C) 2014 Washington University School of Medicine
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

#define __EVENT_UPDATE_VOLUME_EDITING_TOOL_BAR_DECLARE__
#include "EventUpdateVolumeEditingToolBar.h"
#undef __EVENT_UPDATE_VOLUME_EDITING_TOOL_BAR_DECLARE__

#include "CaretAssert.h"
#include "EventTypeEnum.h"

using namespace caret;


    
/**
 * \class caret::EventUpdateVolumeEditingToolBar 
 * \brief <REPLACE-WITH-ONE-LINE-DESCRIPTION>
 * \ingroup GuiQt
 *
 * <REPLACE-WITH-THOROUGH DESCRIPTION>
 */

/**
 * Constructor.
 */
EventUpdateVolumeEditingToolBar::EventUpdateVolumeEditingToolBar()
: Event(EventTypeEnum::EVENT_UPDATE_VOLUME_EDITING_TOOLBAR)
{
    
}

/**
 * Destructor.
 */
EventUpdateVolumeEditingToolBar::~EventUpdateVolumeEditingToolBar()
{
}

