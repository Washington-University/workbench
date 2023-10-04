#ifndef __DRAWING_VIEWPORT_CONTENT_H__
#define __DRAWING_VIEWPORT_CONTENT_H__

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

#include "CaretObject.h"
#include "DrawingViewportContentTypeEnum.h"
#include "DrawingViewportContentSurface.h"
#include "DrawingViewportContentVolumeSlice.h"
#include "GraphicsViewport.h"
#include "ModelTypeEnum.h"

namespace caret {

    class DrawingViewportContent : public CaretObject {
    public:
        DrawingViewportContent(const DrawingViewportContentTypeEnum::Enum viewportContentType,
                               const int32_t windowIndex,
                               const int32_t tabIndex,
                               const GraphicsViewport& graphicsViewport);
        
        DrawingViewportContent(const DrawingViewportContentTypeEnum::Enum viewportContentType,
                               const int32_t windowIndex,
                               const int32_t tabIndex,
                               const GraphicsViewport& graphicsViewport,
                               const ModelTypeEnum::Enum modelType,
                               const DrawingViewportContentVolumeSlice& volumeSlice,
                               const DrawingViewportContentSurface& surface);
        
        DrawingViewportContent();

        virtual ~DrawingViewportContent();
        
        DrawingViewportContent(const DrawingViewportContent&);
        
        DrawingViewportContent& operator=(const DrawingViewportContent&);
        
        DrawingViewportContentTypeEnum::Enum getViewportContentType() const;
        
        const DrawingViewportContentVolumeSlice& getVolumeSlice() const;
        
        int32_t getWindowIndex() const;
        
        int32_t getTabIndex() const;
        
        GraphicsViewport getGraphicsViewport() const;;
        
        bool containsWindowXY(const Vector3D& windowXY) const;

        virtual AString toString() const;

        // ADD_NEW_METHODS_HERE
        
    private:
        void copyHelperDrawingViewportContent(const DrawingViewportContent& obj);
        
        AString toStringWithIndentation(const AString& indentation) const;
        
        DrawingViewportContentTypeEnum::Enum m_viewportContentType = DrawingViewportContentTypeEnum::INVALID;
        
        int32_t m_windowIndex = -1;
        
        int32_t m_tabIndex = -1;
        
        GraphicsViewport m_graphicsViewport;
        
        ModelTypeEnum::Enum m_modelType = ModelTypeEnum::MODEL_TYPE_INVALID;
        
        DrawingViewportContentVolumeSlice m_volumeSlice;
        
        DrawingViewportContentSurface m_surface;
        
        // ADD_NEW_MEMBERS_HERE
        
        friend class DrawingViewportContentManager;
    };
    
#ifdef __DRAWING_VIEWPORT_CONTENT_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __DRAWING_VIEWPORT_CONTENT_DECLARE__

} // namespace
#endif  //__DRAWING_VIEWPORT_CONTENT_H__
