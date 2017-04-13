
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

#define __GRAPHICS_ENGINE_DATA_OPEN_G_L_DECLARE__
#include "GraphicsEngineDataOpenGL.h"
#undef __GRAPHICS_ENGINE_DATA_OPEN_G_L_DECLARE__

#include "CaretAssert.h"
#include "EventGraphicsEngineOpenGLDeleteBuffers.h"
#include "EventManager.h"
#include "GraphicsPrimitive.h"

using namespace caret;


    
/**
 * \class caret::GraphicsEngineDataOpenGL 
 * \brief Contains data the OpenGL Graphics Engine may associate with primtive data.
 * \ingroup Graphics
 */

/**
 * Constructor.
 */
GraphicsEngineDataOpenGL::GraphicsEngineDataOpenGL()
: GraphicsEngineData()
{
    
}

/**
 * Destructor.
 */
GraphicsEngineDataOpenGL::~GraphicsEngineDataOpenGL()
{
    deleteBuffers();
}

void
GraphicsEngineDataOpenGL::deleteBuffers()
{
    std::vector<GLuint> bufferIDs;
    
    if (m_coordinateBufferID > 0) {
        bufferIDs.push_back(m_coordinateBufferID);
    }
    m_coordinateBufferID = 0;
    
    if (m_normalVectorBufferID > 0) {
        bufferIDs.push_back(m_normalVectorBufferID);
    }
    m_normalVectorBufferID = 0;
    
    if (m_colorBufferID > 0) {
        bufferIDs.push_back(m_colorBufferID);
    }
    m_colorBufferID = 0;
    
    if ( ! bufferIDs.empty()) {
        EventGraphicsEngineOpenGLDeleteBuffers buffersEvent(bufferIDs);
        EventManager::get()->sendEvent(buffersEvent.getPointer());
    }
}


/**
 * Load the buffers with data from the grpahics primitive.
 *
 * @param primitive
 *     The graphics primitive that will be drawn.
 */
void
GraphicsEngineDataOpenGL::loadBuffers(GraphicsPrimitive* primitive)
{
    CaretAssert(primitive);
    
    GLsizei coordinateCount = 0;
    switch (primitive->m_vertexType) {
        case GraphicsPrimitive::VertexType::FLOAT_XYZ:
            m_coordinateDataType = GL_FLOAT;
            m_coordinatesPerVertex = 3; // X, Y, Z
            
            coordinateCount = primitive->m_xyz.size();
            const GLuint xyzSizeBytes = coordinateCount * sizeof(float);
            const GLvoid* xyzDataPointer = (const GLvoid*)&primitive->m_xyz[0];
            
            glGenBuffers(1, &m_coordinateBufferID);
            glBindBuffer(GL_ARRAY_BUFFER,
                         m_coordinateBufferID);
            glBufferData(GL_ARRAY_BUFFER,
                         xyzSizeBytes,
                         xyzDataPointer,
                         GL_STREAM_DRAW);
            break;
    }
    
    if (m_coordinatesPerVertex > 0) {
        m_arrayIndicesCount = coordinateCount / m_coordinatesPerVertex;
    }
    else {
        m_arrayIndicesCount = 0;
    }
    
    switch (primitive->m_normalVectorType) {
        case GraphicsPrimitive::NormalVectorType::NONE:
            break;
        case GraphicsPrimitive::NormalVectorType::FLOAT_XYZ:
        {
            m_normalVectorDataType = GL_FLOAT;
            const GLuint normalSizeBytes = primitive->m_floatNormalVectorXYZ.size() * sizeof(float);
            const GLvoid* normalDataPointer = (const GLvoid*)&primitive->m_floatNormalVectorXYZ[0];
            
            glGenBuffers(1, &m_normalVectorBufferID);
            glBindBuffer(GL_ARRAY_BUFFER,
                         m_normalVectorBufferID);
            glBufferData(GL_ARRAY_BUFFER,
                         normalSizeBytes,
                         normalDataPointer,
                         GL_STREAM_DRAW);
        }
            break;
    }
    
    glGenBuffers(1, &m_colorBufferID);
    switch (primitive->m_colorType) {
        case GraphicsPrimitive::ColorType::FLOAT_RGBA:
        {
            m_componentsPerColor = 4;
            m_colorDataType = GL_FLOAT;
            
            const GLuint colorSizeBytes = primitive->m_floatRGBA.size() * sizeof(float);
            const GLvoid* colorDataPointer = (const GLvoid*)&primitive->m_floatRGBA[0];
            
            glBindBuffer(GL_ARRAY_BUFFER,
                         m_colorBufferID);
            glBufferData(GL_ARRAY_BUFFER,
                         colorSizeBytes,
                         colorDataPointer,
                         GL_STREAM_DRAW);
        }
            break;
        case GraphicsPrimitive::ColorType::UNSIGNED_BYTE_RGBA:
        {
            m_componentsPerColor = 4;
            m_colorDataType = GL_UNSIGNED_BYTE;
            
            const GLuint colorSizeBytes = primitive->m_unsignedByteRGBA.size() * sizeof(uint8_t);
            const GLvoid* colorDataPointer = (const GLvoid*)&primitive->m_unsignedByteRGBA[0];
            
            glBindBuffer(GL_ARRAY_BUFFER,
                         m_colorBufferID);
            glBufferData(GL_ARRAY_BUFFER,
                         colorSizeBytes,
                         colorDataPointer,
                         GL_STREAM_DRAW);
        }
            break;
    }
}

