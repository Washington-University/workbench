#ifndef __EVENT_FOCUS_FILE_GET_COLOR_H__
#define __EVENT_FOCUS_FILE_GET_COLOR_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2025 Washington University School of Medicine
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
#include <cstdint>
#include <memory>

#include "Event.h"
#include "SamplesColorModeEnum.h"


namespace caret {

    class EventFocusFileGetColor : public Event {
        
    public:
        EventFocusFileGetColor(const AString& fociFileName,
                               const SamplesColorModeEnum::Enum samplesColorMode,
                               const AString& focusOrClassName);
        
        virtual ~EventFocusFileGetColor();
        
        EventFocusFileGetColor(const EventFocusFileGetColor&) = delete;

        EventFocusFileGetColor& operator=(const EventFocusFileGetColor&) = delete;
        
        AString getFociFileName() const;
        
        SamplesColorModeEnum::Enum getSamplesColorMode() const;
        
        AString getFocusOrClassName() const;
        
        void setColorRGBA(const std::array<uint8_t, 4>& rgba);
        
        std::array<uint8_t, 4> getColorRGBA() const;
        
        // ADD_NEW_METHODS_HERE

    private:
        const AString m_fociFileName;
        
        const SamplesColorModeEnum::Enum m_samplesColorMode;
        
        const AString m_focusOrClassName;
        
        std::array<uint8_t, 4> m_rgba;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __EVENT_FOCUS_FILE_GET_COLOR_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __EVENT_FOCUS_FILE_GET_COLOR_DECLARE__

} // namespace
#endif  //__EVENT_FOCUS_FILE_GET_COLOR_H__
