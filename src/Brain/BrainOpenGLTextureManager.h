#ifndef __BRAIN_OPEN_G_L_TEXTURE_MANAGER_H__
#define __BRAIN_OPEN_G_L_TEXTURE_MANAGER_H__

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

#include <map>

#include "CaretObject.h"
#include "CaretOpenGLInclude.h"



namespace caret {

    class BrainOpenGLTextureManager : public CaretObject {
        
    public:
        BrainOpenGLTextureManager();
        
        virtual ~BrainOpenGLTextureManager();
        
        GLuint createTextureNameForData(const void* dataPointer);

        GLuint getTextureNameForData(const void* dataPointer);
        
        void deleteAllTextureNames();
        
        // ADD_NEW_METHODS_HERE

        virtual AString toString() const;
        
    private:
        BrainOpenGLTextureManager(const BrainOpenGLTextureManager&);

        BrainOpenGLTextureManager& operator=(const BrainOpenGLTextureManager&);
        
        typedef std::map<const void*, GLuint> TextureNameContainer;

        /** Tracks textures */
        TextureNameContainer m_dataPointerToTextureNameMap;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __BRAIN_OPEN_G_L_TEXTURE_MANAGER_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __BRAIN_OPEN_G_L_TEXTURE_MANAGER_DECLARE__

} // namespace
#endif  //__BRAIN_OPEN_G_L_TEXTURE_MANAGER_H__
