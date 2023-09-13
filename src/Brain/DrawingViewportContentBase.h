#ifndef __DRAWING_VIEWPORT_CONTENT_BASE_H__
#define __DRAWING_VIEWPORT_CONTENT_BASE_H__

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
#include "GraphicsViewport.h"

namespace caret {
    class DrawingViewportContentModel;
    class DrawingViewportContentTab;
    class DrawingViewportContentWindow;
    
    class DrawingViewportContentBase : public CaretObject {
    public:
        
        DrawingViewportContentTypeEnum::Enum getViewportContentType() const;
        
        int32_t getWindowIndex() const;
        
    protected:
        DrawingViewportContentBase(const DrawingViewportContentTypeEnum::Enum viewportContentType,
                                   const int32_t windowIndex,
                                   const int32_t tabIndex,
                                   const GraphicsViewport& beforeAspectLockedGraphicsViewport,
                                   const GraphicsViewport& afterAspectLockedGraphicsViewport);
        
        int32_t getTabIndexFromBase() const;
        
        GraphicsViewport getBeforeAspectLockedGraphicsViewportFromBase() const;;
        
        GraphicsViewport getAfterAspectLockedGraphicsViewportFromBase() const;
        
    public:
        /** @return cast to type model (NULL if not model) */
        virtual DrawingViewportContentModel* castToModel() { return NULL; }
        /** @return cast to type model (NULL if not model) const */
        virtual const DrawingViewportContentModel* castToModel() const { return NULL; }
        
        /** @return Cast to type tab (NULL if not tab) */
        virtual DrawingViewportContentTab* castToTab() { return NULL; }
        /** @return Cast to type tab (NULL if not tab) */
        virtual const DrawingViewportContentTab* castToTab() const { return NULL; }
        
        /** @return Cast to type window (NULL if not window) */
        virtual DrawingViewportContentWindow* castToWindow() { return NULL; }
        /** @return Cast to type window (NULL if not window)  const*/
        virtual const DrawingViewportContentWindow* castToWindow() const { return NULL; }

        virtual ~DrawingViewportContentBase();
        
        DrawingViewportContentBase(const DrawingViewportContentBase&) = delete;

        DrawingViewportContentBase& operator=(const DrawingViewportContentBase&) = delete;
        
        virtual void addChildViewport(DrawingViewportContentBase* drawingViewportContentBase);
        
        virtual DrawingViewportContentBase* getViewportTypeAtMouse(const DrawingViewportContentTypeEnum::Enum contentType,
                                                                   const Vector3D& mouseXY) = 0;
        

        // ADD_NEW_METHODS_HERE

        virtual AString toString() const;
        
    private:
        // ADD_NEW_MEMBERS_HERE

        DrawingViewportContentTypeEnum::Enum m_viewportContentType = DrawingViewportContentTypeEnum::INVALID;
        
        int32_t m_windowIndex;
        
        int32_t m_tabIndex;
        
        const GraphicsViewport m_beforeAspectLockedGraphicsViewport;
        
        const GraphicsViewport m_afterAspectLockedGraphicsViewport;

        friend class DrawingViewportContentManager;
    };
    
#ifdef __DRAWING_VIEWPORT_CONTENT_BASE_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __DRAWING_VIEWPORT_CONTENT_BASE_DECLARE__

} // namespace
#endif  //__DRAWING_VIEWPORT_CONTENT_BASE_H__
