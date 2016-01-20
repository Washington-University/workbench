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
#include <set>

#include "CaretObject.h"
#include "CaretOpenGLInclude.h"
#include "EventListenerInterface.h"


namespace caret {

    class DrawnWithOpenGLTextureInfo;
    
    class BrainOpenGLTextureManager : public CaretObject, public EventListenerInterface {
        
    public:
        BrainOpenGLTextureManager(const int32_t windowIndex);
        
        virtual ~BrainOpenGLTextureManager();
        
        virtual void receiveEvent(Event* event);
        
        void getTextureName(DrawnWithOpenGLTextureInfo* textureInfo,
                            GLuint& textureNameOut,
                            bool& newTextureNameFlagOut);
        
        void deleteAllTexturesForWindow(const int32_t windowIndex);
        
        // ADD_NEW_METHODS_HERE

        virtual AString toString() const;
        
    private:
        BrainOpenGLTextureManager(const BrainOpenGLTextureManager&);

        BrainOpenGLTextureManager& operator=(const BrainOpenGLTextureManager&);
        
        GLuint createNewTextureName();
        
        void deleteTextureName(GLuint textureName);
        
        const int32_t m_windowIndex;
        
        /**
         * Generates the texture names
         */
        static int32_t s_textureNameGenerator;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __BRAIN_OPEN_G_L_TEXTURE_MANAGER_DECLARE__
    int32_t BrainOpenGLTextureManager::s_textureNameGenerator = 0;
#endif // __BRAIN_OPEN_G_L_TEXTURE_MANAGER_DECLARE__

} // namespace
#endif  //__BRAIN_OPEN_G_L_TEXTURE_MANAGER_H__
