#ifndef __EVENT_DRAWING_VIEWPORT_CONTENT_ADD_H__
#define __EVENT_DRAWING_VIEWPORT_CONTENT_ADD_H__

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
#include "ModelTypeEnum.h"


namespace caret {

    class GraphicsViewport;
    
    class EventDrawingViewportContentAdd : public Event {
        
    public:
        EventDrawingViewportContentAdd();
        
        virtual ~EventDrawingViewportContentAdd();
        
        EventDrawingViewportContentAdd(const EventDrawingViewportContentAdd&) = delete;

        EventDrawingViewportContentAdd& operator=(const EventDrawingViewportContentAdd&) = delete;
        
        int32_t getNumberOfDrawingViewportContent() const;
        
        std::unique_ptr<DrawingViewportContent> takeDrawingViewportContent(const int32_t index);
        
        void addWindowBeforeLock(const int32_t windowIndex,
                                 const GraphicsViewport& viewport);
        
        void addWindowAfterLock(const int32_t windowIndex,
                                const GraphicsViewport& viewport);
        
        void addTabBeforeLock(const int32_t windowIndex,
                              const int32_t tabIndex,
                              const GraphicsViewport& viewport);
        
        void addTabAfterLock(const int32_t windowIndex,
                             const int32_t tabIndex,
                             const GraphicsViewport& viewport);
        
        void addModel(const int32_t windowIndex,
                      const int32_t tabIndex,
                      const GraphicsViewport& viewport,
                      const ModelTypeEnum::Enum modelType);
        
        void addModelSurfaceGrid(const int32_t windowIndex,
                                 const int32_t tabIndex,
                                 const GraphicsViewport& viewport,
                                 const int32_t numberOfGridRows,
                                 const int32_t numberOfGridColumns);

        void addModelSurfaceGridCell(const int32_t windowIndex,
                                     const int32_t tabIndex,
                                     const GraphicsViewport& viewport,
                                     const int32_t numberOfGridRows,
                                     const int32_t numberOfGridColumns,
                                     const int32_t gridRow,
                                     const int32_t gridColumn);
        
        void addModelVolumeGrid(const int32_t windowIndex,
                                const int32_t tabIndex,
                                const GraphicsViewport& viewport,
                                const int32_t numberOfGridRows,
                                const int32_t numberOfGridColumns);

        void addVolumeSlice(const int32_t windowIndex,
                            const int32_t tabIndex,
                            const GraphicsViewport& viewport,
                            const DrawingViewportContentVolumeSlice& volumeSliceInfo);

        // ADD_NEW_METHODS_HERE

    private:        
        std::vector<std::unique_ptr<DrawingViewportContent>> m_drawingViewportContents;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __EVENT_DRAWING_VIEWPORT_CONTENT_ADD_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __EVENT_DRAWING_VIEWPORT_CONTENT_ADD_DECLARE__

} // namespace
#endif  //__EVENT_DRAWING_VIEWPORT_CONTENT_ADD_H__
