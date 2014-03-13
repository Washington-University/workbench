#ifndef __EVENT_USER_INTERFACE_UPDATE_H__
#define __EVENT_USER_INTERFACE_UPDATE_H__

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

    /// Event for updating the user-interface
    class EventUserInterfaceUpdate : public Event {
        
    public:
        EventUserInterfaceUpdate();
        
        virtual ~EventUserInterfaceUpdate();
        
        EventUserInterfaceUpdate& addBorder();
        
        EventUserInterfaceUpdate& addConnectivity();
        
        EventUserInterfaceUpdate& addFoci();
        
        EventUserInterfaceUpdate& addSurface();
        
        EventUserInterfaceUpdate& addTab();
        
        EventUserInterfaceUpdate& addToolBar();

        EventUserInterfaceUpdate& addToolBox();
        
        bool isBorderUpdate() const;
        
        bool isConnectivityUpdate() const;
        
        bool isFociUpdate() const;
        
        bool isSurfaceUpdate() const;
        
        bool isTabUpdate() const;
        
        bool isToolBarUpdate() const;
        
        bool isToolBoxUpdate() const;
        
        bool isUpdateForWindow(const int32_t windowIndex) const;
        
        EventUserInterfaceUpdate& setWindowIndex(const int32_t windowIndex);
        
    private:
        EventUserInterfaceUpdate(const EventUserInterfaceUpdate&);
        
        EventUserInterfaceUpdate& operator=(const EventUserInterfaceUpdate&);
        
        void setAll(bool selected);
        
        void addInitialization();
        
        bool m_borderUpdate;
        
        bool m_connectivityUpdate;
        
        bool m_fociUpdate;
        
        bool m_surfaceUpdate;
        
        bool m_tabUpdate;
        
        bool m_toolBarUpdate;
        
        bool m_toolBoxUpdate;
        
        bool m_isFirstUpdateType;
        
        int32_t m_windowIndex;
    };

} // namespace

#endif // __EVENT_USER_INTERFACE_UPDATE_H__
