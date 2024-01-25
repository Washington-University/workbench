/*LICENSE_START*/
/*
 *  Copyright (C) 2014  Washington University School of Medicine
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

#include "EventGraphicsPaintSoonAllWindows.h"

using namespace caret;

/**
 * \class caret::EventGraphicsPaintSoonAllWindows 
 * \brief Event for updating all Window gui elements.
 * \ingroup GuiQt
 */

/**
 * Constructor.
 */
EventGraphicsPaintSoonAllWindows::EventGraphicsPaintSoonAllWindows()
: Event(EventTypeEnum::EVENT_GRAPHICS_PAINT_SOON_ALL_WINDOWS)
{
}

/*
 * Destructor.
 */
EventGraphicsPaintSoonAllWindows::~EventGraphicsPaintSoonAllWindows()
{
    
}
