#ifndef __GRAPHICS_ENGINE_OPEN_G_L_H__
#define __GRAPHICS_ENGINE_OPEN_G_L_H__

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
#include "GraphicsEngine.h"



namespace caret {

    class GraphicsOpenGLBufferObject;
    
    class GraphicsEngineOpenGL : public GraphicsEngine {
        
    public:
        GraphicsEngineOpenGL();
        
        virtual ~GraphicsEngineOpenGL();
        
        virtual void receiveEvent(Event* event);
        
        virtual void draw(void* openglContextPointer,
                          GraphicsPrimitive* primitive);

        // ADD_NEW_METHODS_HERE

    private:
        
        GraphicsEngineOpenGL(const GraphicsEngineOpenGL&);

        GraphicsEngineOpenGL& operator=(const GraphicsEngineOpenGL&);
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __GRAPHICS_ENGINE_OPEN_G_L_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __GRAPHICS_ENGINE_OPEN_G_L_DECLARE__

} // namespace
#endif  //__GRAPHICS_ENGINE_OPEN_G_L_H__
