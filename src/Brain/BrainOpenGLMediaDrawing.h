#ifndef __BRAIN_OPEN_G_L_MEDIA_DRAWING_H__
#define __BRAIN_OPEN_G_L_MEDIA_DRAWING_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2020 Washington University School of Medicine
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



namespace caret {

    class BrainOpenGLFixedPipeline;
    class BrainOpenGLViewportContent;
    class BrowserTabContent;
    class GraphicsPrimitiveV3fT3f;
    class ImageFile;
    class ModelMedia;
    
    class BrainOpenGLMediaDrawing : public CaretObject {
        
    public:
        BrainOpenGLMediaDrawing();
        
        virtual ~BrainOpenGLMediaDrawing();
        
        void draw(BrainOpenGLFixedPipeline* fixedPipelineDrawing,
                  const BrainOpenGLViewportContent* viewportContent,
                  BrowserTabContent* browserTabContent,
                  ModelMedia* mediaModel,
                  const std::array<int32_t, 4>& viewport);
        
        BrainOpenGLMediaDrawing(const BrainOpenGLMediaDrawing&) = delete;

        BrainOpenGLMediaDrawing& operator=(const BrainOpenGLMediaDrawing&) = delete;
        

        // ADD_NEW_METHODS_HERE

        virtual AString toString() const;
        
    private:
        void drawModelLayers();
        
        void processImageFileSelection(ImageFile* imageFile,
                                       GraphicsPrimitiveV3fT3f* primitive);
        
        BrainOpenGLFixedPipeline* m_fixedPipelineDrawing = NULL;
        
        BrowserTabContent* m_browserTabContent = NULL;
        
        ModelMedia* m_mediaModel;
        
        std::array<int32_t, 4> m_viewport;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __BRAIN_OPEN_G_L_MEDIA_DRAWING_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __BRAIN_OPEN_G_L_MEDIA_DRAWING_DECLARE__

} // namespace
#endif  //__BRAIN_OPEN_G_L_MEDIA_DRAWING_H__
