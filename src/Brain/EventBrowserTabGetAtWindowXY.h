#ifndef __EVENT_BROWSER_TAB_GET_AT_WINDOW_X_Y_H__
#define __EVENT_BROWSER_TAB_GET_AT_WINDOW_X_Y_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2023 Washington University School of Medicine
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

#include "DrawingViewportContent.h"
#include "Event.h"
#include "Vector3D.h"

namespace caret {

    class BrainOpenGLViewportContent;
    class BrowserTabContent;
    class BrowserWindowContent;
    class DrawingViewportContent;
    
    class EventBrowserTabGetAtWindowXY : public Event {
        
    public:
        EventBrowserTabGetAtWindowXY(const int32_t windowIndex,
                                     const Vector3D& mouseXY);
        
        virtual ~EventBrowserTabGetAtWindowXY();
        
        EventBrowserTabGetAtWindowXY(const EventBrowserTabGetAtWindowXY&) = delete;

        EventBrowserTabGetAtWindowXY& operator=(const EventBrowserTabGetAtWindowXY&) = delete;

        int32_t getWindowIndex() const;
        
        Vector3D getMouseXY() const;
        
        const BrainOpenGLViewportContent* getBrainOpenGLViewportContent() const;
        
        BrowserTabContent* getBrowserTabContent() const;
        
        std::vector<std::shared_ptr<DrawingViewportContent>> getVolumeMontageViewportContent() const;
        
        std::vector<std::shared_ptr<DrawingViewportContent>> getSamplesDrawingVolumeMontageViewportContents() const;
        
        void setBrowserTabContent(const BrainOpenGLViewportContent* brainOpenGLViewportContent,
                                  BrowserWindowContent* browserWindowContent,
                                  BrowserTabContent* browserTabContent);

        bool isWindowXyInSamplesDrawingVolumeSlice(const Vector3D& windowXY) const;
        
        std::vector<std::shared_ptr<DrawingViewportContent>> getSamplesDrawingViewportContents(const Vector3D& windowXY) const;
        
        std::vector<std::shared_ptr<DrawingViewportContent>> getSamplesResetExtentViewportContents() const;
        
        // ADD_NEW_METHODS_HERE

    private:
        const int32_t m_windowIndex;
        
        const Vector3D m_mouseXY;
        
        const BrainOpenGLViewportContent* m_brainOpenGLViewportContent = NULL;
        
        BrowserWindowContent* m_browserWindowContent = NULL;
        
        BrowserTabContent* m_browserTabContent = NULL;
        
        std::vector<std::shared_ptr<DrawingViewportContent>> m_volumeMontageViewportContent;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __EVENT_BROWSER_TAB_GET_AT_WINDOW_X_Y_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __EVENT_BROWSER_TAB_GET_AT_WINDOW_X_Y_DECLARE__

} // namespace
#endif  //__EVENT_BROWSER_TAB_GET_AT_WINDOW_X_Y_H__
