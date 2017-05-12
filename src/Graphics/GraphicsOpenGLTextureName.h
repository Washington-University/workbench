#ifndef __GRAPHICS_OPEN_G_L_TEXTURE_NAME_H__
#define __GRAPHICS_OPEN_G_L_TEXTURE_NAME_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2017 Washington University School of Medicine
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

#include "CaretOpenGLInclude.h"
#include "CaretObject.h"



namespace caret {

    class GraphicsOpenGLTextureName : public CaretObject {
        
    private:
        GraphicsOpenGLTextureName(void* openglContextPointer,
                                   const GLuint textureName);
        
    public:
        virtual ~GraphicsOpenGLTextureName();
        
        /**
         * @return Pointer to OpenGL context in which texture name was created.
         */
        inline void* getOpenGLContextPointer() const { return m_openglContextPointer; }
        
        /**
         * @return The OpenGL Texture Name Name.
         * While it is an unsigned interger, OpenGL refers to it as a "name".
         */
        inline GLuint getTextureName() const { return m_textureName; }
        
        // ADD_NEW_METHODS_HERE

        virtual AString toString() const;
        
    private:
        GraphicsOpenGLTextureName(const GraphicsOpenGLTextureName&);

        GraphicsOpenGLTextureName& operator=(const GraphicsOpenGLTextureName&);
        
        void* m_openglContextPointer = NULL;
        
        const GLuint m_textureName = 0;
        
        friend class BrainOpenGL;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __GRAPHICS_OPEN_G_L_TEXTURE_NAME_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __GRAPHICS_OPEN_G_L_TEXTURE_NAME_DECLARE__

} // namespace
#endif  //__GRAPHICS_OPEN_G_L_TEXTURE_NAME_H__
