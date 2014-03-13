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

#include "EventGraphicsUpdateAllWindows.h"

using namespace caret;

/**
 * \class caret::EventGraphicsUpdateAllWindows 
 * \brief Event for updating all Window gui elements.
 * \ingroup GuiQt
 */

/**
 * Constructor.
 * @param doRepaint
 *    If true, a repaint is performed and this event does not
 *    return until painting is complete.  If false, an update
 *    is performed which schedules a repaint but does not dictate
 *    when the repaint is performed.
 */
EventGraphicsUpdateAllWindows::EventGraphicsUpdateAllWindows(const bool doRepaint)
: Event(EventTypeEnum::EVENT_GRAPHICS_UPDATE_ALL_WINDOWS)
{
    this->doRepaint = doRepaint;
}

/*
 * Destructor.
 */
EventGraphicsUpdateAllWindows::~EventGraphicsUpdateAllWindows()
{
    
}

/**
 * @return Indicates a repaint (instead of updates) is 
 * to be performed.
 */
bool 
EventGraphicsUpdateAllWindows::isRepaint() const 
{ 
    return this->doRepaint; 
}

