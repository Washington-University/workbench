#ifndef __DRAWN_WITH_OPENGL_TEXTURE_INFO_H__
#define __DRAWN_WITH_OPENGL_TEXTURE_INFO_H__

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

#include "BrainConstants.h"
#include "CaretObject.h"
#include "EventListenerInterface.h"

namespace caret {
    
    class DrawnWithOpenGLTextureInfo : public CaretObject, public EventListenerInterface {
        
    public:
        DrawnWithOpenGLTextureInfo();
        
        virtual ~DrawnWithOpenGLTextureInfo();
        
        virtual void receiveEvent(Event* event);
        
        DrawnWithOpenGLTextureInfo(const DrawnWithOpenGLTextureInfo& obj);
        
        DrawnWithOpenGLTextureInfo& operator=(const DrawnWithOpenGLTextureInfo& obj);
        
        int32_t getTextureNameForWindow(const int32_t windowIndex) const;
        
        void setTextureNameForWindow(const int32_t windowIndex,
                                           const int32_t textureName);
        
        // ADD_NEW_METHODS_HERE
        
        virtual AString toString() const;
        
    private:
        void copyHelperDrawnWithOpenGLTextureInfo(const DrawnWithOpenGLTextureInfo& obj);
        
        void resetTextureNames();
        
        int64_t m_textureNamesForWindow[BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_WINDOWS];
        
        // ADD_NEW_MEMBERS_HERE
        
    };
    
#ifdef __DRAWN_WITH_OPENGL_TEXTURE_INFO_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __DRAWN_WITH_OPENGL_TEXTURE_INFO_DECLARE__

} // namespace
#endif  //__DRAWN_WITH_OPENGL_TEXTURE_INFO_H__
