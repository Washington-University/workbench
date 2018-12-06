#ifndef __WU_Q_MACRO_MOUSE_EVENT_INFO_H__
#define __WU_Q_MACRO_MOUSE_EVENT_INFO_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2018 Washington University School of Medicine
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



#include <memory>

#include "CaretObject.h"
#include "WuQMacroMouseEventTypeEnum.h"


namespace caret {

    class WuQMacroMouseEventInfo : public CaretObject {
        
    public:
        WuQMacroMouseEventInfo(const WuQMacroMouseEventTypeEnum::Enum mouseEventType,
                               const int32_t localX,
                               const int32_t localY,
                               const uint32_t mouseButton,
                               const uint32_t mouseButtonsMask,
                               const uint32_t keyboardModifiersMask,
                               const int32_t widgetWidth,
                               const int32_t widgetHeight);
        
        virtual ~WuQMacroMouseEventInfo();
        
        WuQMacroMouseEventInfo(const WuQMacroMouseEventInfo&);

        WuQMacroMouseEventInfo& operator=(const WuQMacroMouseEventInfo&);
        
        void getLocalPositionRescaledToWidgetSize(const int32_t widgetWidth,
                                                  const int32_t widgetHeight,
                                                  int32_t& xOut,
                                                  int32_t& yOut) const;
        
        WuQMacroMouseEventTypeEnum::Enum getMouseEventType() const;
        
        int32_t getLocalX() const;
        
        int32_t getLocalY() const;
        
        uint32_t getMouseButton() const;
        
        uint32_t getMouseButtonsMask() const;
        
        uint32_t getKeyboardModifiersMask() const;
        
        int32_t getWidgetWidth() const;
        
        int32_t getWidgetHeight() const;

        // ADD_NEW_METHODS_HERE

        virtual AString toString() const;
        
    private:
        void copyHelperWuQMacroMouseEventInfo(const WuQMacroMouseEventInfo& obj);
        
        /** Type of mouse event */
        WuQMacroMouseEventTypeEnum::Enum m_mouseEventType;
        
        /** Position of mouse relative to widget */
        int32_t m_localX;
        
        /** Position of mouse relative to widget */
        int32_t m_localY;
        
        /** Button that caused the event */
        uint32_t m_mouseButton;
        
        /** Mask with buttons down during mouse event */
        uint32_t m_mouseButtonsMask;
        
        /** Mask with any keys down during mouse event */
        uint32_t m_keyboardModifiersMask;

        /** Width of widget where mouse event occurred */
        int32_t m_widgetWidth;
        
        /** Width of widget where mouse event occurred */
        int32_t m_widgetHeight;
        
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __WU_Q_MACRO_MOUSE_EVENT_INFO_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __WU_Q_MACRO_MOUSE_EVENT_INFO_DECLARE__

} // namespace
#endif  //__WU_Q_MACRO_MOUSE_EVENT_INFO_H__

