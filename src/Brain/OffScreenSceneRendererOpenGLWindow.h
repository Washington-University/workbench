#ifndef __OFF_SCREEN_SCENE_RENDERER_OPENGL_WINDOW_H__
#define __OFF_SCREEN_SCENE_RENDERER_OPENGL_WINDOW_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2021 Washington University School of Medicine
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

#include "CaretOpenGLInclude.h"


#include "OffScreenSceneRendererBase.h"

namespace caret {

    class OffScreenSceneRendererOpenGLWindow : public OffScreenSceneRendererBase {
        
    public:
        OffScreenSceneRendererOpenGLWindow();
        
        virtual ~OffScreenSceneRendererOpenGLWindow();
        
        OffScreenSceneRendererOpenGLWindow(const OffScreenSceneRendererOpenGLWindow&) = delete;

        OffScreenSceneRendererOpenGLWindow& operator=(const OffScreenSceneRendererOpenGLWindow&) = delete;
        
        virtual void* getDrawingContext() override;
        
        virtual bool isAvailable() const override;
        
        virtual const ImageFile* getImageFile() const override;
        
        virtual bool initialize(const int32_t imageWidth,
                                const int32_t imageHeight) override;

        // ADD_NEW_METHODS_HERE

    private:
        // ADD_NEW_MEMBERS_HERE
        
        void* m_openGLContext = 0;
        
        std::vector<unsigned char> m_imageBuffer;

        mutable std::unique_ptr<ImageFile> m_imageFile;
        
        int32_t m_imageWidth = 0;
        
        int32_t m_imageHeight = 0;
    };
    
#ifdef __OFF_SCREEN_SCENE_RENDERER_OPENGL_WINDOW_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __OFF_SCREEN_SCENE_RENDERER_OPENGL_WINDOW_DECLARE__

} // namespace
#endif  //__OFF_SCREEN_SCENE_RENDERER_OPENGL_WINDOW_H__
