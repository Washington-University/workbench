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

    class GraphicsPrimitive;
    
    class GraphicsEngineDataOpenGL : public GraphicsEngineData {
        
    public:
        GraphicsEngineDataOpenGL();
        
        virtual ~GraphicsEngineDataOpenGL();

        void deleteBuffers();
        
        void loadBuffers(GraphicsPrimitive* primitive);
        
        // ADD_NEW_METHODS_HERE

    private:
        GraphicsEngineDataOpenGL(const GraphicsEngineDataOpenGL&);

        GraphicsEngineDataOpenGL& operator=(const GraphicsEngineDataOpenGL&);
        
        GLsizei m_arrayIndicesCount = 0;
        
        GLuint m_coordinateBufferID = 0;
        
        GLenum m_coordinateDataType = GL_FLOAT;

        GLint m_coordinatesPerVertex = 0;

        GLuint m_normalVectorBufferID = 0;
        
        GLenum m_normalVectorDataType = GL_FLOAT;
        
        GLuint m_colorBufferID = 0;
        
        GLenum m_colorDataType = GL_FLOAT;
        
        GLint m_componentsPerColor = 0;
        
        friend class GraphicsEngineOpenGL;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __GRAPHICS_ENGINE_DATA_OPEN_G_L_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __GRAPHICS_ENGINE_DATA_OPEN_G_L_DECLARE__

} // namespace
#endif  //__GRAPHICS_ENGINE_DATA_OPEN_G_L_H__
