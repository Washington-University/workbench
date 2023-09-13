#ifndef __DRAWING_VIEWPORT_CONTENT_MANAGER_H__
#define __DRAWING_VIEWPORT_CONTENT_MANAGER_H__

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


#include <array>
#include <memory>

#include "CaretObject.h"
#include "DrawingViewportContentBase.h"
#include "EventListenerInterface.h"


namespace caret {
    class DrawingViewportContentWindow;
    class EventDrawingViewportContentGet;

    class DrawingViewportContentManager : public CaretObject, public EventListenerInterface {
        
    public:
        DrawingViewportContentManager();
        
        virtual ~DrawingViewportContentManager();
        
        DrawingViewportContentManager(const DrawingViewportContentManager&) = delete;

        DrawingViewportContentManager& operator=(const DrawingViewportContentManager&) = delete;
        

        // ADD_NEW_METHODS_HERE

        virtual AString toString() const;
        
        virtual void receiveEvent(Event* event);

    private:
        void addViewport(DrawingViewportContentBase* viewportContent);
        
        void clearWindow(const int32_t windowIndex);
        
        void getViewportTypeAtMouse(EventDrawingViewportContentGet* edvc);
        
        std::vector<std::unique_ptr<DrawingViewportContentWindow>> m_windowViewports;
        
        bool m_debuFlag = false;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __DRAWING_VIEWPORT_CONTENT_MANAGER_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __DRAWING_VIEWPORT_CONTENT_MANAGER_DECLARE__

} // namespace
#endif  //__DRAWING_VIEWPORT_CONTENT_MANAGER_H__
