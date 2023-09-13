#ifndef __DRAWING_VIEWPORT_CONTENT_TAB_H__
#define __DRAWING_VIEWPORT_CONTENT_TAB_H__

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
#include <vector>

#include "DrawingViewportContentBase.h"
#include "DrawingViewportContentModel.h"

namespace caret {
    class DrawingViewportContentTab : public DrawingViewportContentBase {
        
    public:
        DrawingViewportContentTab(const int32_t windowIndex,
                                  const int32_t tabIndex,
                                  const GraphicsViewport& beforeAspectLockedGraphicsViewport,
                                  const GraphicsViewport& afterAspectLockedGraphicsViewport);

        virtual ~DrawingViewportContentTab();
        
        DrawingViewportContentTab(const DrawingViewportContentTab&) = delete;

        DrawingViewportContentTab& operator=(const DrawingViewportContentTab&) = delete;
        
        virtual void addChildViewport(DrawingViewportContentBase* drawingViewportContentBase) override;
        
        int32_t getTabIndex() const;
        
        GraphicsViewport getBeforeAspectLockedGraphicsViewport() const;;
        
        GraphicsViewport getAfterAspectLockedGraphicsViewport() const;

        /** @return Cast to type tab (NULL if not tab) */
        virtual DrawingViewportContentTab* castToTab() override { return this; }
        /** @return Cast to type tab (NULL if not tab) */
        virtual const DrawingViewportContentTab* castToTab() const override { return this; }

        DrawingViewportContentBase* getViewportTypeAtMouse(const DrawingViewportContentTypeEnum::Enum contentType,
                                                           const Vector3D& mouseXY) override;
        
        // ADD_NEW_METHODS_HERE

    private:
        std::vector<std::unique_ptr<DrawingViewportContentModel>> m_modelViewports;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __DRAWING_VIEWPORT_CONTENT_TAB_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __DRAWING_VIEWPORT_CONTENT_TAB_DECLARE__

} // namespace
#endif  //__DRAWING_VIEWPORT_CONTENT_TAB_H__
