#ifndef __OFF_SCREEN_SCENE_RENDERER_O_S_MESA_H__
#define __OFF_SCREEN_SCENE_RENDERER_O_S_MESA_H__

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

#ifdef HAVE_GLEW
#include <GL/glew.h>
#endif

#ifdef HAVE_OSMESA
#include <GL/osmesa.h>
#endif // HAVE_OSMESA

#include "OffScreenSceneRendererBase.h"

namespace caret {

    class OffScreenSceneRendererOSMesa : public OffScreenSceneRendererBase {
        
    public:
        OffScreenSceneRendererOSMesa();
        
        virtual ~OffScreenSceneRendererOSMesa();
        
        OffScreenSceneRendererOSMesa(const OffScreenSceneRendererOSMesa&) = delete;

        OffScreenSceneRendererOSMesa& operator=(const OffScreenSceneRendererOSMesa&) = delete;
        
        virtual void* getDrawingContext() override;
        
        virtual bool isAvailable() const override;
        
        virtual const ImageFile* getImageFile() const override;
        
        virtual bool initialize(const int32_t imageWidth,
                                const int32_t imageHeight) override;

        virtual AString toString() const;

        // ADD_NEW_METHODS_HERE

    private:
        // ADD_NEW_MEMBERS_HERE
        
#ifdef HAVE_OSMESA
        OSMesaContext m_mesaContext = 0;
#endif
        std::vector<unsigned char> m_imageBuffer;

        mutable std::unique_ptr<ImageFile> m_imageFile;
        
        int32_t m_imageWidth = 0;
        
        int32_t m_imageHeight = 0;
    };
    
#ifdef __OFF_SCREEN_SCENE_RENDERER_O_S_MESA_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __OFF_SCREEN_SCENE_RENDERER_O_S_MESA_DECLARE__

} // namespace
#endif  //__OFF_SCREEN_SCENE_RENDERER_O_S_MESA_H__
