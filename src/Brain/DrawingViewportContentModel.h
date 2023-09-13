#ifndef __DRAWING_VIEWPORT_CONTENT_MODEL_H__
#define __DRAWING_VIEWPORT_CONTENT_MODEL_H__

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

#include "DrawingViewportContentBase.h"
#include "ModelTypeEnum.h"

namespace caret {

    class DrawingViewportContentModel : public DrawingViewportContentBase {
        
    public:
        DrawingViewportContentModel(const int32_t windowIndex,
                                    const int32_t tabIndex,
                                    const GraphicsViewport& graphicsViewport,
                                    const ModelTypeEnum::Enum modelType);

        DrawingViewportContentModel(const int32_t windowIndex,
                                    const int32_t tabIndex,
                                    const GraphicsViewport& graphicsViewport,
                                    const ModelTypeEnum::Enum modelType,
                                    const int32_t montageRowIndex,
                                    const int32_t montageColumnIndex);
        
        virtual ~DrawingViewportContentModel();
        
        DrawingViewportContentModel(const DrawingViewportContentModel&) = delete;

        DrawingViewportContentModel& operator=(const DrawingViewportContentModel&) = delete;
        
        int32_t getTabIndex() const;
        
        GraphicsViewport getGraphicsViewport() const;
        
        ModelTypeEnum::Enum getModelType() const;
        
        int32_t getMontageRowIndex() const;
        
        int32_t getMontageColumnIndex() const;
        
        /** @return Cast to type model (NULL if not model) */
        virtual DrawingViewportContentModel* castToModel() override { return this; }
        
        /** @return Cast to type model (NULL if not model) const */
        virtual const DrawingViewportContentModel* castToModel() const override { return this; }
        
        DrawingViewportContentBase* getViewportTypeAtMouse(const DrawingViewportContentTypeEnum::Enum contentType,
                                                           const Vector3D& mouseXY) override;
        


        // ADD_NEW_METHODS_HERE

    private:
        
        ModelTypeEnum::Enum m_modelType = ModelTypeEnum::MODEL_TYPE_INVALID;
        
        int32_t m_montageRowIndex = -1;
        
        int32_t m_montageColumnIndex = -1;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __DRAWING_VIEWPORT_CONTENT_MODEL_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __DRAWING_VIEWPORT_CONTENT_MODEL_DECLARE__

} // namespace
#endif  //__DRAWING_VIEWPORT_CONTENT_MODEL_H__
