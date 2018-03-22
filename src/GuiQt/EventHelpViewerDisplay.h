#ifndef __EVENT_HELP_VIEWER_DISPLAY_H__
#define __EVENT_HELP_VIEWER_DISPLAY_H__

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


#include "Event.h"

namespace caret {
    class BrainBrowserWindow;

    class EventHelpViewerDisplay : public Event {
        
    public:
        EventHelpViewerDisplay(BrainBrowserWindow* brainBrowserWindow,
                               const AString helpPageName);
        
        virtual ~EventHelpViewerDisplay();
        
        const BrainBrowserWindow* getBrainBrowserWindow() const;
        
        AString getHelpPageName() const;
        
        // ADD_NEW_METHODS_HERE

    private:
        EventHelpViewerDisplay(const EventHelpViewerDisplay&);

        EventHelpViewerDisplay& operator=(const EventHelpViewerDisplay&);
        
        const BrainBrowserWindow* m_brainBrowserWindow;
        
        const AString m_helpPageName;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __EVENT_HELP_VIEWER_DISPLAY_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __EVENT_HELP_VIEWER_DISPLAY_DECLARE__

} // namespace
#endif  //__EVENT_HELP_VIEWER_DISPLAY_H__
