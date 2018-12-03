#ifndef __WU_Q_MACRO_MOUSE_EVENT_WIDGET_INTERFACE_H__
#define __WU_Q_MACRO_MOUSE_EVENT_WIDGET_INTERFACE_H__

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

class QMouseEvent;

namespace caret {

    class WuQMacroMouseEventWidgetInterface {
        
    public:
        WuQMacroMouseEventWidgetInterface() { }
        
        virtual ~WuQMacroMouseEventWidgetInterface() { }
        
        WuQMacroMouseEventWidgetInterface(const WuQMacroMouseEventWidgetInterface&) = delete;

        WuQMacroMouseEventWidgetInterface& operator=(const WuQMacroMouseEventWidgetInterface&) = delete;
        
        virtual void processMouseEventFromMacro(QMouseEvent* me) = 0;
        
    private:
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __WU_Q_MACRO_MOUSE_EVENT_WIDGET_INTERFACE_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __WU_Q_MACRO_MOUSE_EVENT_WIDGET_INTERFACE_DECLARE__

} // namespace
#endif  //__WU_Q_MACRO_MOUSE_EVENT_WIDGET_INTERFACE_H__
