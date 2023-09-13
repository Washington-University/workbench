#ifndef __EVENT_DRAWING_VIEWPORT_CONTENT_GET_H__
#define __EVENT_DRAWING_VIEWPORT_CONTENT_GET_H__

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

#include "DrawingViewportContentTypeEnum.h"
#include "Event.h"
#include "Vector3D.h"

namespace caret {

    class DrawingViewportContentBase;
    class DrawingViewportContentModel;
    class DrawingViewportContentTab;
    class DrawingViewportContentWindow;

    class EventDrawingViewportContentGet : public Event {
        
    public:
        EventDrawingViewportContentGet(const DrawingViewportContentTypeEnum::Enum contentType,
                                       const int32_t windowIndex,
                                       const Vector3D& mouseXY);
        
        virtual ~EventDrawingViewportContentGet();
        
        EventDrawingViewportContentGet(const EventDrawingViewportContentGet&) = delete;

        EventDrawingViewportContentGet& operator=(const EventDrawingViewportContentGet&) = delete;
        
        const DrawingViewportContentBase* getDrawingViewportContent() const;

        const DrawingViewportContentModel* getDrawingViewportContentModel() const;
        
        const DrawingViewportContentTab* getDrawingViewportContentTab() const;
        
        const DrawingViewportContentWindow* getDrawingViewportContentWindow() const;
        
        void setDrawingViewportContent(const DrawingViewportContentBase* drawingViewportContent);
        
        DrawingViewportContentTypeEnum::Enum getContentType() const;
        
        int32_t getWindowIndex() const;
        
        const Vector3D getMouseXY() const;
        
        // ADD_NEW_METHODS_HERE

    private:
        const DrawingViewportContentTypeEnum::Enum m_contentType;
        
        const int32_t m_windowIndex;
        
        const Vector3D m_mouseXY;
        
        const DrawingViewportContentBase* m_drawingViewportContent = NULL;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __EVENT_DRAWING_VIEWPORT_CONTENT_GET_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __EVENT_DRAWING_VIEWPORT_CONTENT_GET_DECLARE__

} // namespace
#endif  //__EVENT_DRAWING_VIEWPORT_CONTENT_GET_H__
