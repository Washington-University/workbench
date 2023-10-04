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

    class DrawingViewportContent;

    class EventDrawingViewportContentGet : public Event {
        
    public:
        enum class Mode {
            /** Match the content type exactly */
            MATCH_CONTENT_TYPE,
            /** Get the top-most of all model viewports */
            MODEL_TOP_VIEWPORT,
            /** Testing */
            TESTING,
            /** Get volume montage slices */
            VOLUME_MONTAGE_SLICES
        };
        
        static std::unique_ptr<EventDrawingViewportContentGet> newInstanceGetTopModelViewport(const int32_t windowIndex,
                                                                                              const Vector3D& windowXY);
        
        static std::unique_ptr<EventDrawingViewportContentGet> newInstanceGetContentType(const int32_t windowIndex,
                                                                                         const Vector3D& windowXY,
                                                                                         const DrawingViewportContentTypeEnum::Enum contentType);
        
        static std::unique_ptr<EventDrawingViewportContentGet> newInstancePrintAllAtWindowXY(const int32_t windowIndex,
                                                                                             const Vector3D& windowXY);
        
        static std::vector<std::shared_ptr<DrawingViewportContent>> getVolumeMontageSlicesInTab(const int32_t tabIndex);
        
        virtual ~EventDrawingViewportContentGet();
        
        EventDrawingViewportContentGet(const EventDrawingViewportContentGet&) = delete;

        EventDrawingViewportContentGet& operator=(const EventDrawingViewportContentGet&) = delete;
        
        Mode getMode() const;
        
        std::shared_ptr<DrawingViewportContent> getDrawingViewportContent() const;
        
        std::vector<std::shared_ptr<DrawingViewportContent>> getAllDrawingViewportContent() const;
        
        void addDrawingViewportContent(std::shared_ptr<DrawingViewportContent>& drawingViewportContent);
        
        DrawingViewportContentTypeEnum::Enum getContentType() const;
        
        int32_t getWindowIndex() const;
        
        const Vector3D getWindowXY() const;
        
        int32_t getTabIndex() const;
        
        // ADD_NEW_METHODS_HERE

    private:
        EventDrawingViewportContentGet(const int32_t windowIndex,
                                       const Vector3D& windowXY);
        

        EventDrawingViewportContentGet(const DrawingViewportContentTypeEnum::Enum contentType,
                                       const int32_t windowIndex,
                                       const Vector3D& windowXY);
        
        EventDrawingViewportContentGet(const Mode mode,
                                       const DrawingViewportContentTypeEnum::Enum contentType,
                                       const int32_t windowIndex,
                                       const int32_t tabIndex,
                                       const Vector3D& windowXY);

        const Mode m_mode;
        
        const DrawingViewportContentTypeEnum::Enum m_contentType;
        
        const int32_t m_windowIndex;
        
        const int32_t m_tabIndex;
        
        const Vector3D m_windowXY;

        std::vector<std::shared_ptr<DrawingViewportContent>> m_drawingViewportContent;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __EVENT_DRAWING_VIEWPORT_CONTENT_GET_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __EVENT_DRAWING_VIEWPORT_CONTENT_GET_DECLARE__

} // namespace
#endif  //__EVENT_DRAWING_VIEWPORT_CONTENT_GET_H__
