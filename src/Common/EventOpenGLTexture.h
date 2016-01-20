#ifndef __EVENT_OPENGL_TEXTURE_H__
#define __EVENT_OPENGL_TEXTURE_H__

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


#include "Event.h"



namespace caret {

    class EventOpenGLTexture : public Event {
        
    public:
        enum Mode {
            MODE_NONE,
            MODE_DELETE_ALL_TEXTURES_IN_WINDOW,
            MODE_DELETE_TEXTURE
        };
        
        EventOpenGLTexture();
        
        virtual ~EventOpenGLTexture();

        Mode getMode() const;
        
        void setModeDeleteAllTexturesInWindow(const int32_t windowIndex);
        
        void getModeDeleteAllTexturesInWindow(int32_t windowIndexOut) const;
        
        void setModeDeleteTexture(const int32_t windowIndex,
                                  const int32_t textureName);

        void getModeDeleteTexture(int32_t& windowIndexOut,
                                  int32_t& textureNameOut) const;
        
        // ADD_NEW_METHODS_HERE

    private:
        EventOpenGLTexture(const EventOpenGLTexture&);

        EventOpenGLTexture& operator=(const EventOpenGLTexture&);
        
        Mode m_mode;
        
        int32_t m_windowIndex;
        
        int32_t m_textureName;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __EVENT_OPENGL_TEXTURE_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __EVENT_OPENGL_TEXTURE_DECLARE__

} // namespace
#endif  //__EVENT_OPENGL_TEXTURE_H__
