#ifndef __OFF_SCREEN_SCENE_RENDERER_BASE_H__
#define __OFF_SCREEN_SCENE_RENDERER_BASE_H__

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

#include "CaretObject.h"

namespace caret {

    class ImageFile;
    
    class OffScreenSceneRendererBase : public CaretObject {
        
    public:
        OffScreenSceneRendererBase(const AString switchName,
                                   const AString descriptiveName);
        
        virtual ~OffScreenSceneRendererBase();
        
        OffScreenSceneRendererBase(const OffScreenSceneRendererBase&) = delete;

        OffScreenSceneRendererBase& operator=(const OffScreenSceneRendererBase&) = delete;

        AString getSwitchName() const;
        
        AString getDescriptiveName() const;
        
        bool isValid() const;
        
        AString getErrorMessage() const;
        
        /**
         * @return Drawing context for the renderer (may be NULL)
         */
        virtual void* getDrawingContext() = 0;

        /**
         * @return Is the renderer available ?
         * Some renderers may not be available on some systems.
         */
        virtual bool isAvailable() const = 0;
        
        /**
         * @return Image file produced by renderer.  May be NULL if creating image failed.
         */
        virtual const ImageFile* getImageFile() const = 0;
        
        /**
         * Initialize the renderer for creating an image of the given width and height.
         * @parm imageWidth
         *    Width of image.
         * @param imageHeight
         *    Height of image.
         * @return True if successful, else false.
         */
        virtual bool initialize(const int32_t imageWidth,
                                const int32_t imageHeight) = 0;

        // ADD_NEW_METHODS_HERE

        virtual AString toString() const;
    
    protected:
        void setValid(const bool valid);
        
        void setErrorMessage(const AString& errorMessage);
        
    private:
        const AString m_switchName;
        
        const AString m_descriptiveName;
        
        AString m_errorMessage;
        
        bool m_validFlag = false;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __OFF_SCREEN_SCENE_RENDERER_BASE_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __OFF_SCREEN_SCENE_RENDERER_BASE_DECLARE__

} // namespace
#endif  //__OFF_SCREEN_SCENE_RENDERER_BASE_H__
