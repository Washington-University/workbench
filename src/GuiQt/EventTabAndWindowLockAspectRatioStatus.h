#ifndef __EVENT_TAB_AND_WINDOW_LOCK_ASPECT_RATIO_STATUS_H__
#define __EVENT_TAB_AND_WINDOW_LOCK_ASPECT_RATIO_STATUS_H__

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

#include "Event.h"



namespace caret {

    class Annotation;
    class BrainBrowserWindow;
    
    class EventTabAndWindowLockAspectRatioStatus : public Event {
        
    public:
        class TabStatus {
        public:
            TabStatus() { }
            
            void setStatus(const int32_t tabIndex,
                           const int32_t windowIndex,
                           const bool lockAspectRatioStatus,
                           const bool containsTabSpaceAnnotationsFlag) {
                m_tabIndex = tabIndex;
                m_windowIndex = windowIndex;
                m_lockAspectRatioStatus = lockAspectRatioStatus;
                m_containsTabSpaceAnnotationsFlag = containsTabSpaceAnnotationsFlag;
            }
            
            bool isValid() { return (m_tabIndex >= 0); }
            
            int32_t getTabIndex() const { return m_tabIndex; }
            
            int32_t getWindowIndex() const { return m_windowIndex; }
            
            bool isAspectRatioLocked() const { return m_lockAspectRatioStatus; }
            
            bool hasTabSpaceAnnotations() const { return m_containsTabSpaceAnnotationsFlag; }
            
        private:
            int32_t m_tabIndex = -1;
            int32_t m_windowIndex = -1;
            bool m_lockAspectRatioStatus = false;
            bool m_containsTabSpaceAnnotationsFlag = false;
        };
        
        class WindowStatus {
        public:
            WindowStatus() { }
            
            void setStatus(const int32_t windowIndex,
                           const bool lockAspectRatioStatus,
                           const bool tileTabsEnabled,
                           const bool containsWindowSpaceAnnotationsFlag) {
                m_windowIndex = windowIndex;
                m_lockAspectRatioStatus = lockAspectRatioStatus;
                m_tileTabsEnabled = tileTabsEnabled;
                m_containsWindowSpaceAnnotationsFlag = containsWindowSpaceAnnotationsFlag;
            }
            
            bool isValid() { return (m_windowIndex >= 0); }
            
            int32_t getWindowIndex() const { return m_windowIndex; }
            
            bool isAspectRatioLocked() const { return m_lockAspectRatioStatus; }
            
            bool isTileTabsEnabled() const { return m_tileTabsEnabled; }
            
            bool hasWindowSpaceAnnotations() const { return m_containsWindowSpaceAnnotationsFlag; }
            
            void addTabStatus(const TabStatus* tabStatus) { m_tabs.push_back(tabStatus); }
            
            int32_t getTabCount() const { return m_tabs.size(); }
            
            int32_t getTabLockedCount() const {
                const int32_t tabLockedCount = std::count_if(m_tabs.begin(),
                                                             m_tabs.end(),
                                                             [](const TabStatus* ts) { return ts->isAspectRatioLocked(); });
                return tabLockedCount;
            }
            
        private:
            int32_t m_windowIndex = -1;
            bool m_lockAspectRatioStatus = false;
            bool m_tileTabsEnabled = false;
            bool m_containsWindowSpaceAnnotationsFlag = false;
            std::vector<const TabStatus*> m_tabs;
        };
        
        EventTabAndWindowLockAspectRatioStatus();
        
        virtual ~EventTabAndWindowLockAspectRatioStatus();
        
        void setup(const std::vector<BrainBrowserWindow*>& windows,
                   const std::vector<Annotation*>& annotations);
        
        TabStatus* getTabStatus(const int32_t tabIndex);

        WindowStatus* getWindowStatus(const int32_t windowIndex);
        
        // ADD_NEW_METHODS_HERE

    private:
        EventTabAndWindowLockAspectRatioStatus(const EventTabAndWindowLockAspectRatioStatus&);

        EventTabAndWindowLockAspectRatioStatus& operator=(const EventTabAndWindowLockAspectRatioStatus&);
        
        std::vector<TabStatus> m_tabStatus;
        
        std::vector<WindowStatus> m_windowStatus;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __EVENT_TAB_AND_WINDOW_LOCK_ASPECT_RATIO_STATUS_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __EVENT_TAB_AND_WINDOW_LOCK_ASPECT_RATIO_STATUS_DECLARE__

} // namespace
#endif  //__EVENT_TAB_AND_WINDOW_LOCK_ASPECT_RATIO_STATUS_H__
