#ifndef __DRAWN_WITH_OPENGL_TEXTURE_INTERFACE_H__
#define __DRAWN_WITH_OPENGL_TEXTURE_INTERFACE_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2016 Washington University School of Medicine
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

#include <stdint.h>

/**
 * \class caret::DrawnWithOpenGLTextureInterface
 * \brief Interface for items that are drawn with an OpenGL texture.
 * \ingroup Common
 *
 * OpenGL textures use resources in the graphics system.  Classes
 * implementing this interface must contain an instance of
 * DrawnWithOpenGLTextureInfo which contains information about 
 * the OpenGL texture.  When an instance of DrawnWithOpenGLTextureInfo
 * goes out of scope (deleted), it will send a message to
 * OpenGL rendering to free up any associated resources with the 
 * OpenGL texture.
 */



namespace caret {

    class DrawnWithOpenGLTextureInfo;
    
    class DrawnWithOpenGLTextureInterface {
        
    public:
        /**
         * Constructor.
         */
        DrawnWithOpenGLTextureInterface() { }
        
        /**
         * Destructor.
         */
        virtual ~DrawnWithOpenGLTextureInterface() { }
        
        /**
         * @return The OpenGL texture information used for managing
         * texture resources.
         */
        virtual DrawnWithOpenGLTextureInfo* getDrawWithOpenGLTextureInfo() = 0;
        
        /**
         * @return The OpenGL texture information used for managing
         * texture resources (const method)
         */
        virtual const DrawnWithOpenGLTextureInfo* getDrawWithOpenGLTextureInfo() const = 0;

        // ADD_NEW_METHODS_HERE

    private:
        DrawnWithOpenGLTextureInterface(const DrawnWithOpenGLTextureInterface& /*obj*/);
        
        DrawnWithOpenGLTextureInterface& operator=(const DrawnWithOpenGLTextureInterface& /*obj*/);

        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __DRAWN_WITH_OPENGL_TEXTURE_INTERFACE_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __DRAWN_WITH_OPENGL_TEXTURE_INTERFACE_DECLARE__

} // namespace
#endif  //__DRAWN_WITH_OPENGL_TEXTURE_INTERFACE_H__
