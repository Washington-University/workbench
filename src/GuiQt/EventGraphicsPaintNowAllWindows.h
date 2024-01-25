#ifndef __EVENT_GRAPHICS_PAINT_NOW_ALL_WINDOWS_H__
#define __EVENT_GRAPHICS_PAINT_NOW_ALL_WINDOWS_H__

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


#include "Event.h"

namespace caret {

    /// Event for painting all graphics windows immediately
    class EventGraphicsPaintNowAllWindows : public Event {
        
    public:
        EventGraphicsPaintNowAllWindows();
        
        virtual ~EventGraphicsPaintNowAllWindows();
        
    private:
        EventGraphicsPaintNowAllWindows(const EventGraphicsPaintNowAllWindows&);
        
        EventGraphicsPaintNowAllWindows& operator=(const EventGraphicsPaintNowAllWindows&);
    };

} // namespace

#endif // __EVENT_GRAPHICS_PAINT_NOW_ALL_WINDOWS_H__
