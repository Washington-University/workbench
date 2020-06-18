#ifndef __EVENT_GRAPHICS_WINDOW_SHOW_TOOL_TIP_H__
#define __EVENT_GRAPHICS_WINDOW_SHOW_TOOL_TIP_H__

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


#include <array>
#include <memory>

#include "Event.h"



namespace caret {

    class EventGraphicsWindowShowToolTip : public Event {
        
    public:
        enum class WindowOrigin {
            BOTTOM_LEFT,
            TOP_LEFT
        };
        
        EventGraphicsWindowShowToolTip(const int32_t windowIndex,
                                       const WindowOrigin windowOrigin,
                                       const std::array<float,3>& windowXYZ,
                                       const QString& text);
        
        virtual ~EventGraphicsWindowShowToolTip();
        
        EventGraphicsWindowShowToolTip(const EventGraphicsWindowShowToolTip&) = delete;

        EventGraphicsWindowShowToolTip& operator=(const EventGraphicsWindowShowToolTip&) = delete;
        
        WindowOrigin getWindowOrigin() const;
        
        int32_t getWindowIndex() const;
        
        std::array<float,3> getWindowXYZ() const;
        
        QString getText() const;

        // ADD_NEW_METHODS_HERE

    private:
        const int32_t m_windowIndex;
        
        const WindowOrigin m_windowOrigin;
        
        const std::array<float,3>& m_windowXYZ;
        
        const QString m_text;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __EVENT_GRAPHICS_WINDOW_SHOW_TOOL_TIP_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __EVENT_GRAPHICS_WINDOW_SHOW_TOOL_TIP_DECLARE__

} // namespace
#endif  //__EVENT_GRAPHICS_WINDOW_SHOW_TOOL_TIP_H__
