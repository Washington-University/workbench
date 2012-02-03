#ifndef __EVENT_TOOL_BOX_SELECTION_DISPLAY_H__
#define __EVENT_TOOL_BOX_SELECTION_DISPLAY_H__

/*LICENSE_START*/ 
/* 
 *  Copyright 1995-2002 Washington University School of Medicine 
 * 
 *  http://brainmap.wustl.edu 
 * 
 *  This file is part of CARET. 
 * 
 *  CARET is free software; you can redistribute it and/or modify 
 *  it under the terms of the GNU General Public License as published by 
 *  the Free Software Foundation; either version 2 of the License, or 
 *  (at your option) any later version. 
 * 
 *  CARET is distributed in the hope that it will be useful, 
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of 
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
 *  GNU General Public License for more details. 
 * 
 *  You should have received a copy of the GNU General Public License 
 *  along with CARET; if not, write to the Free Software 
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA 
 * 
 */ 


#include "Event.h"

namespace caret {

    /// Event for displaying/hiding the selection toolbox
    class EventToolBoxSelectionDisplay : public Event {
        
    public:
        /** Display or hide */
        enum DisplayMode {
            /** Display border selection in toolbox */
            DISPLAY_MODE_DISPLAY_BORDERS,
            /** Hide the selection toolbox */
            DISPLAY_MODE_HIDE
        };
        
        EventToolBoxSelectionDisplay(const int32_t browserWindowIndex,
                                     const DisplayMode displayMode);
        
        virtual ~EventToolBoxSelectionDisplay();
        
        int32_t getBrowserWindowIndex() const;

        DisplayMode getDisplayMode() const;
        
    private:
        EventToolBoxSelectionDisplay(const EventToolBoxSelectionDisplay&);
        
        EventToolBoxSelectionDisplay& operator=(const EventToolBoxSelectionDisplay&);
        
        const int32_t browserWindowIndex;
        
        const DisplayMode displayMode;
    };

} // namespace

#endif // __EVENT_TOOL_BOX_SELECTION_DISPLAY_H__
