#ifndef __GRAPHICS_ENGINE_DATA_OPEN_G_L_H__
#define __GRAPHICS_ENGINE_DATA_OPEN_G_L_H__

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



#include <memory>

#include "GraphicsEngineData.h"
#include "CaretOpenGLInclude.h"


namespace caret {

    class GraphicsOpenGLBufferObject;
    class GraphicsPrimitive;
    
    class GraphicsEngineDataOpenGL : public GraphicsEngineData {
        
    public:
        GraphicsEngineDataOpenGL(const void* openglContextPointer);
        
        virtual ~GraphicsEngineDataOpenGL();

        void deleteBuffers();
        
        void loadBuffers(GraphicsPrimitive* primitive);
        
        static void draw(void* openglContextPointer,
                         GraphicsPrimitive* primitive);
        
        static void drawWithOverrideColor(void* openglContextPointer,
                                          GraphicsPrimitive* primitive,
                                          const float solidColorOverrideRGBA[4]);
        
        const void* getOpenGLContextPointer() const;
        
        // ADD_NEW_METHODS_HERE

    private:
        GraphicsEngineDataOpenGL(const GraphicsEngineDataOpenGL&);

        GraphicsEngineDataOpenGL& operator=(const GraphicsEngineDataOpenGL&);
        
        void deleteBufferObjectHelper(GraphicsOpenGLBufferObject* &bufferObject);
        
        static void drawPrivate(void* openglContextPointer,
                                GraphicsPrimitive* primitive,
                                const float solidColorOverrideRGBA[4],
                                const bool solidColorOverrideValid);
        
        const void* m_openglContextPointer;
        
        GLsizei m_arrayIndicesCount = 0;
        
        GraphicsOpenGLBufferObject* m_coordinateBufferObject = NULL;
        
        GLenum m_coordinateDataType = GL_FLOAT;

        GLint m_coordinatesPerVertex = 0;

        GraphicsOpenGLBufferObject* m_normalVectorBufferObject = NULL;
        
        GLenum m_normalVectorDataType = GL_FLOAT;
        
        GraphicsOpenGLBufferObject* m_colorBufferObject = NULL;
        
        GLenum m_colorDataType = GL_FLOAT;
        
        GLint m_componentsPerColor = 0;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __GRAPHICS_ENGINE_DATA_OPEN_G_L_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __GRAPHICS_ENGINE_DATA_OPEN_G_L_DECLARE__

} // namespace
#endif  //__GRAPHICS_ENGINE_DATA_OPEN_G_L_H__
