
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

#define __GRAPHICS_ENGINE_OPEN_G_L_DECLARE__
#include "GraphicsEngineOpenGL.h"
#undef __GRAPHICS_ENGINE_OPEN_G_L_DECLARE__

#include "CaretAssert.h"
#include "CaretOpenGLInclude.h"
#include "GraphicsPrimitive.h"

using namespace caret;


    
/**
 * \class caret::GraphicsEngineOpenGL 
 * \brief Render graphics objects using OpenGL.
 * \ingroup Graphics
 */

/**
 * Constructor.
 */
GraphicsEngineOpenGL::GraphicsEngineOpenGL()
: GraphicsEngine()
{
    
}

/**
 * Destructor.
 */
GraphicsEngineOpenGL::~GraphicsEngineOpenGL()
{
}

/**
 * Draw the given graphics primitive.
 *
 * @param primitive
 *     Primitive that is drawn.
 */
void
GraphicsEngineOpenGL::draw(GraphicsPrimitive* primitive)
{
    CaretAssert(primitive);
    if ( ! primitive->isValid()) {
        return;
    }
    
    GLenum coordinateDataType = GL_FLOAT;
    GLint coordinatesPerVertex = 0;
    GLuint vertexBufferID = 0;
    GLsizei indicesCount = 0;
    switch (primitive->m_vertexType) {
        case GraphicsPrimitive::VertexType::FLOAT_XYZ:
            coordinateDataType = GL_FLOAT;
            coordinatesPerVertex = 3; // X, Y, Z

            indicesCount = primitive->m_xyz.size();
            const GLuint xyzSizeBytes = indicesCount * sizeof(float);
            const GLvoid* xyzDataPointer = (const GLvoid*)&primitive->m_xyz[0];
            
            glGenBuffers(1, &vertexBufferID);
            glBindBuffer(GL_ARRAY_BUFFER,
                         vertexBufferID);
            glBufferData(GL_ARRAY_BUFFER,
                         xyzSizeBytes,
                         xyzDataPointer,
                         GL_STREAM_DRAW);
            break;
    }
    
    GLenum normalVectorDataType = GL_FLOAT;
    GLuint normalBufferID = 0;
    switch (primitive->m_normalVectorType) {
        case GraphicsPrimitive::NormalVectorType::NONE:
            break;
        case GraphicsPrimitive::NormalVectorType::FLOAT_XYZ:
        {
            normalVectorDataType = GL_FLOAT;
            const GLuint normalSizeBytes = primitive->m_floatNormalVectorXYZ.size() * sizeof(float);
            const GLvoid* normalDataPointer = (const GLvoid*)&primitive->m_floatNormalVectorXYZ[0];

            glGenBuffers(1, &normalBufferID);
            glBindBuffer(GL_ARRAY_BUFFER,
                         normalBufferID);
            glBufferData(GL_ARRAY_BUFFER,
                         normalSizeBytes,
                         normalDataPointer,
                         GL_STREAM_DRAW);
        }
            break;
    }
    
    GLenum colorDataType = GL_FLOAT;
    GLint componentsPerColor = 0;
    GLuint colorBufferID = 0;
    glGenBuffers(1, &colorBufferID);
    switch (primitive->m_colorType) {
        case GraphicsPrimitive::ColorType::FLOAT_RGBA:
        {
            componentsPerColor = 4;
            colorDataType = GL_FLOAT;
            
            const GLuint colorSizeBytes = primitive->m_floatRGBA.size() * sizeof(float);
            const GLvoid* colorDataPointer = (const GLvoid*)&primitive->m_floatRGBA[0];
            
            glBindBuffer(GL_ARRAY_BUFFER,
                         colorBufferID);
            glBufferData(GL_ARRAY_BUFFER,
                         colorSizeBytes,
                         colorDataPointer,
                         GL_STREAM_DRAW);
        }
            break;
        case GraphicsPrimitive::ColorType::UNSIGNED_BYTE_RGBA:
        {
            componentsPerColor = 4;
            colorDataType = GL_UNSIGNED_BYTE;
            
            const GLuint colorSizeBytes = primitive->m_unsignedByteRGBA.size() * sizeof(uint8_t);
            const GLvoid* colorDataPointer = (const GLvoid*)&primitive->m_unsignedByteRGBA[0];
            
            glBindBuffer(GL_ARRAY_BUFFER,
                         colorBufferID);
            glBufferData(GL_ARRAY_BUFFER,
                         colorSizeBytes,
                         colorDataPointer,
                         GL_STREAM_DRAW);
        }
            break;
    }

    /*
     * Setup vertices for drawing
     */
    glEnableClientState(GL_VERTEX_ARRAY);
    glBindBuffer(GL_ARRAY_BUFFER,
                 vertexBufferID);
    glVertexPointer(coordinatesPerVertex,
                    coordinateDataType,
                    0,
                    (GLvoid*)0);
    
    /*
     * Setup normals for drawing
     */
    if (normalBufferID > 0) {
        glEnableClientState(GL_NORMAL_ARRAY);
        glBindBuffer(GL_ARRAY_BUFFER, normalBufferID);
        glNormalPointer(normalVectorDataType, 0, (GLvoid*)0);
    }
    else {
        glDisableClientState(GL_NORMAL_ARRAY);
    }
    
    /*
     * Setup colors for drawing
     */
    glEnableClientState(GL_COLOR_ARRAY);
    glBindBuffer(GL_ARRAY_BUFFER, colorBufferID);
    glColorPointer(componentsPerColor, colorDataType, 0, (GLvoid*)0);
    
    /*
     * Unbind buffers
     */
    glBindBuffer(GL_ARRAY_BUFFER,
                 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,
                 0);
    
    /*
     * Draw the primitive
     */
    switch (primitive->m_primitiveType) {
        case GraphicsPrimitive::PrimitiveType::QUADS:
            glDrawArrays(GL_QUADS,
                         0,
                         indicesCount);
            break;
        case GraphicsPrimitive::PrimitiveType::TRIANGLES:
            glDrawArrays(GL_TRIANGLES,
                         0,
                         indicesCount);
            break;
    }
    
    /*
     * Disable drawing
     */
    glDisableClientState(GL_VERTEX_ARRAY);
    if (normalBufferID > 0) {
        glDisableClientState(GL_NORMAL_ARRAY);
    }
    glDisableClientState(GL_COLOR_ARRAY);
    
    /*
     * Delete buffers
     */
    glDeleteBuffers(1,
                    &vertexBufferID);
    if (normalBufferID > 0) {
        glDeleteBuffers(1,
                        &normalBufferID);
    }
    glDeleteBuffers(1,
                    &colorBufferID);
}

