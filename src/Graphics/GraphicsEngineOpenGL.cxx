
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

#include <QMutexLocker>

#include "CaretAssert.h"
#include "CaretOpenGLInclude.h"
#include "GraphicsEngineDataOpenGL.h"
#include "EventGraphicsEngineOpenGLDeleteBuffers.h"
#include "EventManager.h"
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
    EventManager::get()->addEventListener(this,
                                          EventTypeEnum::EVENT_GRAPHICS_ENGINE_OPENGL_DELETE_BUFFERS);
}

/**
 * Destructor.
 */
GraphicsEngineOpenGL::~GraphicsEngineOpenGL()
{
    EventManager::get()->removeAllEventsFromListener(this);
}

/**
 * Receive an event.
 *
 * @param event
 *    An event for which this instance is listening.
 */
void
GraphicsEngineOpenGL::receiveEvent(Event* event)
{
    if (event->getEventType() == EventTypeEnum::EVENT_GRAPHICS_ENGINE_OPENGL_DELETE_BUFFERS) {
        EventGraphicsEngineOpenGLDeleteBuffers* bufferEvent
        = dynamic_cast<EventGraphicsEngineOpenGLDeleteBuffers*>(event);
        CaretAssert(bufferEvent);
        event->setEventProcessed();
        
        std::vector<GLuint> bufferIDs = bufferEvent->getBufferIdentifiers();
        
        QMutexLocker locker(&m_unusedBufferIdentifiersMutex);
        
        m_unusedBufferIdentifiers.insert(m_unusedBufferIdentifiers.end(),
                                         bufferIDs.begin(), bufferIDs.end());
    }
    else {
        GraphicsEngine::receiveEvent(event);
    }
}

/**
 * This method must be called only when the OpenGL context
 * is current.
 */
void
GraphicsEngineOpenGL::deleteUnusedBuffers()
{
    const GLsizei numberOfIDs = m_unusedBufferIdentifiers.size();
    if (numberOfIDs > 0) {
        QMutexLocker locker(&m_unusedBufferIdentifiersMutex);
        
        for (GLuint buffID : m_unusedBufferIdentifiers) {
            if ( ! glIsBuffer(buffID)) {
                std::cout << "Attempting to delete invalid buffer ID: " << buffID << std::endl;
            }
        }
        
        const GLuint* idsPointer = &m_unusedBufferIdentifiers[0];
        glDeleteBuffers(numberOfIDs,
                        idsPointer);
        m_unusedBufferIdentifiers.clear();
        
        std::cout << "Deleted " << numberOfIDs << " buffers." << std::endl;
    }
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
    
    GraphicsEngineDataOpenGL* openglData = primitive->getGraphicsEngineDataForOpenGL();
    if (openglData == NULL) {
        openglData = new GraphicsEngineDataOpenGL();
        primitive->setGraphicsEngineDataForOpenGL(openglData);
        
        openglData->loadBuffers(primitive);
    }
    
//    GLenum coordinateDataType = GL_FLOAT;
//    GLint coordinatesPerVertex = 0;
//    GLuint coordinateBufferID = 0;
//    GLsizei coordinateCount = 0;
//    switch (primitive->m_vertexType) {
//        case GraphicsPrimitive::VertexType::FLOAT_XYZ:
//            coordinateDataType = GL_FLOAT;
//            coordinatesPerVertex = 3; // X, Y, Z
//
//            coordinateCount = primitive->m_xyz.size();
//            const GLuint xyzSizeBytes = coordinateCount * sizeof(float);
//            const GLvoid* xyzDataPointer = (const GLvoid*)&primitive->m_xyz[0];
//            
//            glGenBuffers(1, &coordinateBufferID);
//            glBindBuffer(GL_ARRAY_BUFFER,
//                         coordinateBufferID);
//            glBufferData(GL_ARRAY_BUFFER,
//                         xyzSizeBytes,
//                         xyzDataPointer,
//                         GL_STREAM_DRAW);
//            break;
//    }
//    
//    GLenum normalVectorDataType = GL_FLOAT;
//    GLuint normalBufferID = 0;
//    switch (primitive->m_normalVectorType) {
//        case GraphicsPrimitive::NormalVectorType::NONE:
//            break;
//        case GraphicsPrimitive::NormalVectorType::FLOAT_XYZ:
//        {
//            normalVectorDataType = GL_FLOAT;
//            const GLuint normalSizeBytes = primitive->m_floatNormalVectorXYZ.size() * sizeof(float);
//            const GLvoid* normalDataPointer = (const GLvoid*)&primitive->m_floatNormalVectorXYZ[0];
//
//            glGenBuffers(1, &normalBufferID);
//            glBindBuffer(GL_ARRAY_BUFFER,
//                         normalBufferID);
//            glBufferData(GL_ARRAY_BUFFER,
//                         normalSizeBytes,
//                         normalDataPointer,
//                         GL_STREAM_DRAW);
//        }
//            break;
//    }
//    
//    GLenum colorDataType = GL_FLOAT;
//    GLint componentsPerColor = 0;
//    GLuint colorBufferID = 0;
//    glGenBuffers(1, &colorBufferID);
//    switch (primitive->m_colorType) {
//        case GraphicsPrimitive::ColorType::FLOAT_RGBA:
//        {
//            componentsPerColor = 4;
//            colorDataType = GL_FLOAT;
//            
//            const GLuint colorSizeBytes = primitive->m_floatRGBA.size() * sizeof(float);
//            const GLvoid* colorDataPointer = (const GLvoid*)&primitive->m_floatRGBA[0];
//            
//            glBindBuffer(GL_ARRAY_BUFFER,
//                         colorBufferID);
//            glBufferData(GL_ARRAY_BUFFER,
//                         colorSizeBytes,
//                         colorDataPointer,
//                         GL_STREAM_DRAW);
//        }
//            break;
//        case GraphicsPrimitive::ColorType::UNSIGNED_BYTE_RGBA:
//        {
//            componentsPerColor = 4;
//            colorDataType = GL_UNSIGNED_BYTE;
//            
//            const GLuint colorSizeBytes = primitive->m_unsignedByteRGBA.size() * sizeof(uint8_t);
//            const GLvoid* colorDataPointer = (const GLvoid*)&primitive->m_unsignedByteRGBA[0];
//            
//            glBindBuffer(GL_ARRAY_BUFFER,
//                         colorBufferID);
//            glBufferData(GL_ARRAY_BUFFER,
//                         colorSizeBytes,
//                         colorDataPointer,
//                         GL_STREAM_DRAW);
//        }
//            break;
//    }

    /*
     * Setup vertices for drawing
     */
    glEnableClientState(GL_VERTEX_ARRAY);
    glBindBuffer(GL_ARRAY_BUFFER,
                 openglData->m_coordinateBufferID);
    glVertexPointer(openglData->m_coordinatesPerVertex,
                    openglData->m_coordinateDataType,
                    0,
                    (GLvoid*)0);
    
    /*
     * Setup normals for drawing
     */
    if (openglData->m_normalVectorBufferID > 0) {
        glEnableClientState(GL_NORMAL_ARRAY);
        glBindBuffer(GL_ARRAY_BUFFER, openglData->m_normalVectorBufferID);
        glNormalPointer(openglData->m_normalVectorDataType, 0, (GLvoid*)0);
    }
    else {
        glDisableClientState(GL_NORMAL_ARRAY);
    }
    
    /*
     * Setup colors for drawing
     */
    glEnableClientState(GL_COLOR_ARRAY);
    glBindBuffer(GL_ARRAY_BUFFER, openglData->m_colorBufferID);
    glColorPointer(openglData->m_componentsPerColor, openglData->m_colorDataType, 0, (GLvoid*)0);
    
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
//    const GLsizei indicesCount = openglDatacoordinateCount / coordinatesPerVertex;
    GLenum openGLPrimitiveType = GL_INVALID_ENUM;
    switch (primitive->m_primitiveType) {
        case GraphicsPrimitive::PrimitiveType::LINE_LOOP:
            openGLPrimitiveType = GL_LINE_LOOP;
            break;
        case GraphicsPrimitive::PrimitiveType::LINE_STRIP:
            openGLPrimitiveType = GL_LINE_STRIP;
            break;
        case GraphicsPrimitive::PrimitiveType::LINES:
            openGLPrimitiveType = GL_LINES;
            break;
        case GraphicsPrimitive::PrimitiveType::POINTS:
            openGLPrimitiveType = GL_POINTS;
            break;
        case GraphicsPrimitive::PrimitiveType::POLYGON:
            openGLPrimitiveType = GL_POLYGON;
            break;
        case GraphicsPrimitive::PrimitiveType::QUAD_STRIP:
            openGLPrimitiveType = GL_QUAD_STRIP;
            break;
        case GraphicsPrimitive::PrimitiveType::QUADS:
            openGLPrimitiveType = GL_QUADS;
            break;
        case GraphicsPrimitive::PrimitiveType::TRIANGLE_FAN:
            openGLPrimitiveType = GL_TRIANGLE_FAN;
            break;
        case GraphicsPrimitive::PrimitiveType::TRIANGLE_STRIP:
            openGLPrimitiveType = GL_TRIANGLE_STRIP;
            break;
        case GraphicsPrimitive::PrimitiveType::TRIANGLES:
            openGLPrimitiveType = GL_TRIANGLES;
            break;
    }
    
    CaretAssert(openGLPrimitiveType != GL_INVALID_ENUM);
    glDrawArrays(openGLPrimitiveType, 0, openglData->m_arrayIndicesCount);
    
    /*
     * Disable drawing
     */
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);
    
    
//    /*
//     * Delete buffers
//     */
//    glDeleteBuffers(1,
//                    &coordinateBufferID);
//    if (normalBufferID > 0) {
//        glDeleteBuffers(1,
//                        &normalBufferID);
//    }
//    glDeleteBuffers(1,
//                    &colorBufferID);
}

