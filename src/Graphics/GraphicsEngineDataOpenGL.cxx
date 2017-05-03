
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
#include "CaretLogger.h"
#include "EventGraphicsOpenGLCreateBufferObject.h"
#include "EventManager.h"
#include "GraphicsOpenGLBufferObject.h"
#include "GraphicsPrimitive.h"
#include "GraphicsPrimitiveSelectionHelper.h"

using namespace caret;


    
/**
 * \class caret::GraphicsEngineDataOpenGL 
 * \brief Contains data the OpenGL Graphics Engine may associate with primtive data.
 * \ingroup Graphics
 */

/**
 * Constructor.
 *
 * @param openglContextPointer
 *     Context to which the OpenGL buffers apply.  The current usage of QGLWidget
 *     shares the OpenGL context among all QGLWidgets.  However, in the event
 *     OpenGL contexts are not shared, this implementation will need to be 
 *     updated.
 */
GraphicsEngineDataOpenGL::GraphicsEngineDataOpenGL(const void* openglContextPointer)
: GraphicsEngineData(),
m_openglContextPointer(openglContextPointer)
{
    
}

/**
 * Destructor.
 */
GraphicsEngineDataOpenGL::~GraphicsEngineDataOpenGL()
{
    deleteBuffers();
}

/**
 * Delete an OpenGL buffer object.
 *
 * @param bufferObject
 *     Reference to pointer of buffer object for deletion.
 *     If NULL, no action is taken.
 *     Will be NULL upon exit.
 */
void
GraphicsEngineDataOpenGL::deleteBufferObjectHelper(GraphicsOpenGLBufferObject* &bufferObject)
{
    if (bufferObject != NULL) {
        delete bufferObject;
        bufferObject = NULL;
    }
}

/**
 * Delete the OpenGL buffer objects.
 */
void
GraphicsEngineDataOpenGL::deleteBuffers()
{
    deleteBufferObjectHelper(m_coordinateBufferObject);
    
    deleteBufferObjectHelper(m_normalVectorBufferObject);
    
    deleteBufferObjectHelper(m_colorBufferObject);
    
    for (auto mapBuff: m_alternativeColorBufferObjectMap) {
        deleteBufferObjectHelper(mapBuff.second);
    }
    m_alternativeColorBufferObjectMap.clear();
}

/**
 * Get the OpenGL Buffer Usage Hint from the primitive.
 *
 * @param primitive
 *     The graphics primitive
 * @return
 *     The OpenGL Buffer Usage Hint.
 */
GLenum
GraphicsEngineDataOpenGL::getOpeGLBufferUsageHint(const GraphicsPrimitive* primitive) const
{
    GLenum usageHint = GL_STREAM_DRAW;
    switch (primitive->getUsageType()) {
        case GraphicsPrimitive::UsageType::MODIFIED_ONCE_DRAWN_FEW_TIMES:
            usageHint = GL_STREAM_DRAW;
            break;
        case GraphicsPrimitive::UsageType::MODIFIED_ONCE_DRAWN_MANY_TIMES:
            usageHint = GL_STATIC_DRAW;
            break;
        case GraphicsPrimitive::UsageType::MODIFIED_MANY_DRAWN_MANY_TIMES:
            usageHint = GL_DYNAMIC_DRAW;
            break;
    }
    
    return usageHint;
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
    
    GLenum usageHint = getOpeGLBufferUsageHint(primitive);
    
    GLsizei coordinateCount = 0;
    switch (primitive->m_vertexType) {
        case GraphicsPrimitive::VertexType::FLOAT_XYZ:
            m_coordinateDataType = GL_FLOAT;
            m_coordinatesPerVertex = 3; // X, Y, Z
            
            coordinateCount = primitive->m_xyz.size();
            const GLuint xyzSizeBytes = coordinateCount * sizeof(float);
            const GLvoid* xyzDataPointer = (const GLvoid*)&primitive->m_xyz[0];
            
            EventGraphicsOpenGLCreateBufferObject createEvent;
            EventManager::get()->sendEvent(createEvent.getPointer());
            m_coordinateBufferObject = createEvent.getOpenGLBufferObject();
            CaretAssert(m_coordinateBufferObject->getBufferObjectName());
            
            glBindBuffer(GL_ARRAY_BUFFER,
                         m_coordinateBufferObject->getBufferObjectName());
            glBufferData(GL_ARRAY_BUFFER,
                         xyzSizeBytes,
                         xyzDataPointer,
                         usageHint);
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
            
            EventGraphicsOpenGLCreateBufferObject createEvent;
            EventManager::get()->sendEvent(createEvent.getPointer());
            m_normalVectorBufferObject = createEvent.getOpenGLBufferObject();
            CaretAssert(m_normalVectorBufferObject->getBufferObjectName());
            
            glBindBuffer(GL_ARRAY_BUFFER,
                         m_normalVectorBufferObject->getBufferObjectName());
            glBufferData(GL_ARRAY_BUFFER,
                         normalSizeBytes,
                         normalDataPointer,
                         usageHint);
        }
            break;
    }
    
    switch (primitive->m_colorType) {
        case GraphicsPrimitive::ColorType::FLOAT_RGBA:
        {
            EventGraphicsOpenGLCreateBufferObject createEvent;
            EventManager::get()->sendEvent(createEvent.getPointer());
            m_colorBufferObject = createEvent.getOpenGLBufferObject();
            CaretAssert(m_colorBufferObject->getBufferObjectName());
            
            m_componentsPerColor = 4;
            m_colorDataType = GL_FLOAT;
            
            const GLuint colorSizeBytes = primitive->m_floatRGBA.size() * sizeof(float);
            const GLvoid* colorDataPointer = (const GLvoid*)&primitive->m_floatRGBA[0];
            
            glBindBuffer(GL_ARRAY_BUFFER,
                         m_colorBufferObject->getBufferObjectName());
            glBufferData(GL_ARRAY_BUFFER,
                         colorSizeBytes,
                         colorDataPointer,
                         usageHint);
        }
            break;
        case GraphicsPrimitive::ColorType::UNSIGNED_BYTE_RGBA:
        {
            EventGraphicsOpenGLCreateBufferObject createEvent;
            EventManager::get()->sendEvent(createEvent.getPointer());
            m_colorBufferObject = createEvent.getOpenGLBufferObject();
            CaretAssert(m_colorBufferObject->getBufferObjectName());
            
            m_componentsPerColor = 4;
            m_colorDataType = GL_UNSIGNED_BYTE;
            
            const GLuint colorSizeBytes = primitive->m_unsignedByteRGBA.size() * sizeof(uint8_t);
            const GLvoid* colorDataPointer = (const GLvoid*)&primitive->m_unsignedByteRGBA[0];
            
            glBindBuffer(GL_ARRAY_BUFFER,
                         m_colorBufferObject->getBufferObjectName());
            glBufferData(GL_ARRAY_BUFFER,
                         colorSizeBytes,
                         colorDataPointer,
                         usageHint);
        }
            break;
    }
}

/**
 * Load the buffer for the given alternative color identifier.  If the buffer
 * was previously loaded, no action is taken.
 *
 * @param primitive
 *     The graphics primitive that will be drawn.
 * @param alternativeColorIdentifier
 *     Identifier for the alternative color.
 * @return
 *     True if the alternative color buffer is valid for the given alternative 
 *     color identifier so drawing can be performed.  Otherwise, false is returned.
 */
GraphicsOpenGLBufferObject*
GraphicsEngineDataOpenGL::loadAlternativeColorBuffer(GraphicsPrimitive* primitive,
                                                     const int32_t alternativeColorIdentifier)
{
    std::map<int32_t, GraphicsOpenGLBufferObject*>::iterator iter = m_alternativeColorBufferObjectMap.find(alternativeColorIdentifier);
    if (iter != m_alternativeColorBufferObjectMap.end()) {
        return iter->second;
    }
    
    GraphicsOpenGLBufferObject* outputOpenGLBufferObject = NULL;
    
    GLenum usageHint = getOpeGLBufferUsageHint(primitive);
    
    switch (primitive->m_colorType) {
        case GraphicsPrimitive::ColorType::FLOAT_RGBA:
        {
            const std::vector<float>& alternativeFloatRGBA = primitive->getAlternativeFloatRGBAProtected(alternativeColorIdentifier);
            if ((alternativeFloatRGBA.size() / 4) != (primitive->m_xyz.size() / 3)) {
                const AString msg("Number of alternative RGBA values is for "
                                  + AString::number((alternativeFloatRGBA.size() / 4))
                                  + " vertices but should be for "
                                  + AString::number((primitive->m_xyz.size() / 3))
                                  + " vertices.");
                CaretAssertMessage(0, msg);
                CaretLogSevere(msg);
                return NULL;
            }
            
            EventGraphicsOpenGLCreateBufferObject createEvent;
            EventManager::get()->sendEvent(createEvent.getPointer());
            GraphicsOpenGLBufferObject* bufferObject = createEvent.getOpenGLBufferObject();
            CaretAssert(bufferObject->getBufferObjectName());
            
            m_componentsPerColor = 4;
            m_colorDataType = GL_FLOAT;
            
            const GLuint colorSizeBytes = alternativeFloatRGBA.size() * sizeof(float);
            const GLvoid* colorDataPointer = (const GLvoid*)&alternativeFloatRGBA[0];
            
            glBindBuffer(GL_ARRAY_BUFFER,
                         bufferObject->getBufferObjectName());
            glBufferData(GL_ARRAY_BUFFER,
                         colorSizeBytes,
                         colorDataPointer,
                         usageHint);
            
            m_alternativeColorBufferObjectMap.insert(std::make_pair(alternativeColorIdentifier,
                                                                    bufferObject));
            outputOpenGLBufferObject = bufferObject;
        }
            break;
        case GraphicsPrimitive::ColorType::UNSIGNED_BYTE_RGBA:
        {
            const std::vector<uint8_t>& alternativeUnsignedByteRGBA = primitive->getAlternativeUnsignedByteRGBAProtected(alternativeColorIdentifier);
            if ((alternativeUnsignedByteRGBA.size() / 4) != (primitive->m_xyz.size() / 3)) {
                const AString msg("Number of alternative RGBA values is for "
                                  + AString::number((alternativeUnsignedByteRGBA.size() / 4))
                                  + " vertices but should be for "
                                  + AString::number((primitive->m_xyz.size() / 3))
                                  + " vertices.");
                CaretAssertMessage(0, msg);
                CaretLogSevere(msg);
                return NULL;
            }
            
            EventGraphicsOpenGLCreateBufferObject createEvent;
            EventManager::get()->sendEvent(createEvent.getPointer());
            GraphicsOpenGLBufferObject* bufferObject = createEvent.getOpenGLBufferObject();
            CaretAssert(bufferObject->getBufferObjectName());
            
            m_componentsPerColor = 4;
            m_colorDataType = GL_UNSIGNED_BYTE;
            
            const GLuint colorSizeBytes = alternativeUnsignedByteRGBA.size() * sizeof(uint8_t);
            const GLvoid* colorDataPointer = (const GLvoid*)&alternativeUnsignedByteRGBA[0];
            
            glBindBuffer(GL_ARRAY_BUFFER,
                         bufferObject->getBufferObjectName());
            glBufferData(GL_ARRAY_BUFFER,
                         colorSizeBytes,
                         colorDataPointer,
                         usageHint);
            
            m_alternativeColorBufferObjectMap.insert(std::make_pair(alternativeColorIdentifier,
                                                                    bufferObject));
            outputOpenGLBufferObject = bufferObject;
        }
            break;
    }
    
    return outputOpenGLBufferObject;
}


/**
 * Draw the given graphics primitive.
 *
 * @param openglContextPointer
 *     Pointer to the active OpenGL context.
 * @param primitive
 *     Primitive that is drawn.
 */
void
GraphicsEngineDataOpenGL::draw(void* openglContextPointer,
                               GraphicsPrimitive* primitive)
{
    float dummyRGBA[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
    drawPrivate(PrivateDrawMode::DRAW_NORMAL,
                openglContextPointer,
                primitive,
                NULL,
                -1,
                dummyRGBA);
}

/**
 * Draw to determine the index of primitive at the given (X, Y) location
 *
 * @param openglContextPointer
 *     Pointer to the active OpenGL context.
 * @param primitive
 *     Primitive that is drawn.
 * @param pixelX
 *     X location for selection.
 * @param pixelY
 *     Y location for selection.
 * @param selectedPrimitiveIndexOut
 *     Output with the index of the primitive (invalid no primitve at location
 * @param selectedPrimitiveDepthOut
 *     Output with depth at the location.
 */
void
GraphicsEngineDataOpenGL::drawWithSelection(void* openglContextPointer,
                                            GraphicsPrimitive* primitive,
                                            const int32_t pixelX,
                                            const int32_t pixelY,
                                            int32_t& selectedPrimitiveIndexOut,
                                            float&   selectedPrimitiveDepthOut)
{
    CaretAssert(primitive);
    
    selectedPrimitiveIndexOut = -1;
    selectedPrimitiveDepthOut = 0.0;

    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT,
                  viewport);
    if ((pixelX    >= viewport[0])
        && (pixelX <  viewport[2])
        && (pixelY >= viewport[1])
        && (pixelY <  viewport[3])) {
        /*
         * Saves glPixelStore parameters
         */
        glPushClientAttrib(GL_CLIENT_PIXEL_STORE_BIT);
        glPushAttrib(GL_LIGHTING_BIT);
        
        glShadeModel(GL_FLAT);
        glDisable(GL_LIGHTING);
        
        float dummyRGBA[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
        GraphicsPrimitiveSelectionHelper selectionHelper(primitive);
        selectionHelper.setupSelectionBeforeDrawing();
        drawPrivate(PrivateDrawMode::DRAW_SELECTION,
                    openglContextPointer,
                    primitive,
                    &selectionHelper,
                    -1,
                    dummyRGBA);
        
        /*
         * QOpenGLWidget Note: The QOpenGLWidget always renders in a
         * frame buffer object (see its documentation).  This is
         * probably why calls to glReadBuffer() always cause an
         * OpenGL error.
         */
#ifdef WORKBENCH_USE_QT5_QOPENGL_WIDGET
        /* do not call glReadBuffer() */
#else
        glReadBuffer(GL_BACK);
#endif
        uint8_t pixelRGBA[4];

        glPixelStorei(GL_PACK_SKIP_ROWS, 0);
        glPixelStorei(GL_PACK_SKIP_PIXELS, 0);
        glPixelStorei(GL_PACK_ALIGNMENT, 1); // bytes
        glReadPixels(pixelX,
                     pixelY,
                     1,
                     1,
                     GL_RGBA,
                     GL_UNSIGNED_BYTE,
                     pixelRGBA);
        
        /*
         * Get depth from depth buffer
         */
        glPixelStorei(GL_PACK_ALIGNMENT, 4); // float
        glReadPixels(pixelX,
                     pixelY,
                     1,
                     1,
                     GL_DEPTH_COMPONENT,
                     GL_FLOAT,
                     &selectedPrimitiveDepthOut);
        
        glPopAttrib();
        glPopClientAttrib();
        
        selectedPrimitiveIndexOut = selectionHelper.getPrimitiveIndexFromEncodedRGBA(pixelRGBA);
    }
}


/**
 * Draw the given graphics primitive using a solid color.
 *
 * @param openglContextPointer
 *     Pointer to the active OpenGL context.
 * @param primitive
 *     Primitive that is drawn.
 * @param solidColorOverrideRGBA
 *     Color that is applied to all vertices.
 */
void
GraphicsEngineDataOpenGL::drawWithOverrideColor(void* openglContextPointer,
                                                GraphicsPrimitive* primitive,
                                                const float solidColorOverrideRGBA[4])
{
    drawPrivate(PrivateDrawMode::DRAW_COLOR_SOLID,
                openglContextPointer,
                primitive,
                NULL,
                -1,
                solidColorOverrideRGBA);
}

/**
 * Draw the given graphics primitive using an alternative color.
 *
 * @param openglContextPointer
 *     Pointer to the active OpenGL context.
 * @param primitive
 *     Primitive that is drawn.
 * @param alternativeColorIdentifier
 *     Identifier for the alternative color.
 */
void
GraphicsEngineDataOpenGL::drawWithAlternativeColor(void* openglContextPointer,
                                                   GraphicsPrimitive* primitive,
                                                   const int32_t alternativeColorIdentifier)
{
    float dummyRGBA[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
    drawPrivate(PrivateDrawMode::DRAW_COLOR_ALTERNATIVE,
                openglContextPointer,
                primitive, NULL,
                alternativeColorIdentifier,
                dummyRGBA);
}


/**
 * Draw the graphics primitive.
 *
 * @param drawMode
 *     Mode for drawing.
 * @param openglContextPointer
 *     Pointer to the active OpenGL context.
 * @param primitive
 *     Primitive that is drawn.
 * @param primitiveSelectionHelper
 *     Selection helper when draw mode is selection.
 * @param alternativeColorIdentifier
 *     Identifier for the alternative color.
 * @param solidColorRGBA
 *     Color for solid color mode.
 */
void
GraphicsEngineDataOpenGL::drawPrivate(const PrivateDrawMode drawMode,
                                      void* openglContextPointer,
                                      GraphicsPrimitive* primitive,
                                      GraphicsPrimitiveSelectionHelper* primitiveSelectionHelper,
                                      const int32_t alternativeColorIdentifier,
                                      const float solidColorRGBA[4])
{
    CaretAssert(openglContextPointer);
    CaretAssert(primitive);
    if ( ! primitive->isValid()) {
        return;
    }
    
    switch (drawMode) {
        case PrivateDrawMode::DRAW_COLOR_ALTERNATIVE:
            if ( ! primitive->isAlternativeColoringValid(alternativeColorIdentifier)) {
                CaretLogSevere("Alternative coloring is invalid for index="
                               + AString::number(alternativeColorIdentifier));
                return;
            }
            break;
        case PrivateDrawMode::DRAW_COLOR_SOLID:
            break;
        case PrivateDrawMode::DRAW_NORMAL:
            break;
        case PrivateDrawMode::DRAW_SELECTION:
            break;
    }
    
    GraphicsEngineDataOpenGL* openglData = primitive->getGraphicsEngineDataForOpenGL();
    if (openglData == NULL) {
        openglData = new GraphicsEngineDataOpenGL(openglContextPointer);
        primitive->setGraphicsEngineDataForOpenGL(openglData);
        
        openglData->loadBuffers(primitive);
    }
    
    if (openglContextPointer != openglData->getOpenGLContextPointer()) {
        const AString msg("Only one OpenGL graphics context is allowed.  Have there been "
                          "changes made to QGLWidget creation?");
        CaretAssertMessage(0, msg);
        CaretLogSevere(msg);
        return;
    }
    
    const GLuint coordBufferID = openglData->m_coordinateBufferObject->getBufferObjectName();
    CaretAssert(coordBufferID);
    if ( ! glIsBuffer(coordBufferID)) {
        CaretAssertMessage(0, "Coordinate buffer is INVALID");
        CaretLogSevere("Coordinate buffer is INVALID");
        return;
    }
    
    GraphicsOpenGLBufferObject* alternativeColorBufferObject = NULL;
    switch (drawMode) {
        case PrivateDrawMode::DRAW_COLOR_ALTERNATIVE:
            alternativeColorBufferObject = openglData->loadAlternativeColorBuffer(primitive,
                                                                                  alternativeColorIdentifier);
            if (alternativeColorBufferObject == NULL) {
                CaretAssertMessage(0, "Coordinate buffer is INVALID");
                CaretLogSevere("Coordinate buffer is INVALID");
                return;
            }
            break;
        case PrivateDrawMode::DRAW_COLOR_SOLID:
            break;
        case PrivateDrawMode::DRAW_NORMAL:
            break;
        case PrivateDrawMode::DRAW_SELECTION:
            break;
    }
    /*
     * Setup vertices for drawing
     */
    glEnableClientState(GL_VERTEX_ARRAY);
    CaretAssert(glIsBuffer(coordBufferID));
    glBindBuffer(GL_ARRAY_BUFFER,
                 coordBufferID);
    glVertexPointer(openglData->m_coordinatesPerVertex,
                    openglData->m_coordinateDataType,
                    0,
                    (GLvoid*)0);
    
    /*
     * Setup normals for drawing
     */
    if (openglData->m_normalVectorBufferObject != NULL) {
        glEnableClientState(GL_NORMAL_ARRAY);
        CaretAssert(glIsBuffer(openglData->m_normalVectorBufferObject->getBufferObjectName()));
        glBindBuffer(GL_ARRAY_BUFFER, openglData->m_normalVectorBufferObject->getBufferObjectName());
        glNormalPointer(openglData->m_normalVectorDataType, 0, (GLvoid*)0);
    }
    else {
        glDisableClientState(GL_NORMAL_ARRAY);
    }
    
    /*
     * Setup colors for drawing
     */
    GLuint localColorBufferName = 0;
    switch (drawMode) {
        case PrivateDrawMode::DRAW_COLOR_ALTERNATIVE:
        {
            glEnableClientState(GL_COLOR_ARRAY);
            CaretAssert(glIsBuffer(alternativeColorBufferObject->getBufferObjectName()));
            glBindBuffer(GL_ARRAY_BUFFER, alternativeColorBufferObject->getBufferObjectName());
            glColorPointer(openglData->m_componentsPerColor, openglData->m_colorDataType, 0, (GLvoid*)0);
        }
            break;
        case PrivateDrawMode::DRAW_COLOR_SOLID:
        {
            const int32_t numberOfVertices = primitive->m_xyz.size() / 3;
            if (numberOfVertices > 0) {
                glGenBuffers(1, &localColorBufferName);
                CaretAssert(localColorBufferName > 0);
                
                const GLint  componentsPerColor = 4;
                const GLenum colorDataType = GL_FLOAT;
                
                std::vector<float> colorRGBA(numberOfVertices * componentsPerColor);
                for (int32_t i = 0; i < numberOfVertices; i++) {
                    const int32_t i4 = i * 4;
                    CaretAssertVectorIndex(colorRGBA, i4 + 3);
                    colorRGBA[i4]   = solidColorRGBA[0];
                    colorRGBA[i4+1] = solidColorRGBA[1];
                    colorRGBA[i4+2] = solidColorRGBA[2];
                    colorRGBA[i4+3] = solidColorRGBA[3];
                }
                
                const GLuint colorSizeBytes = colorRGBA.size() * sizeof(float);
                const GLvoid* colorDataPointer = (const GLvoid*)&colorRGBA[0];
                glBindBuffer(GL_ARRAY_BUFFER,
                             localColorBufferName);
                glBufferData(GL_ARRAY_BUFFER,
                             colorSizeBytes,
                             colorDataPointer,
                             GL_STREAM_DRAW);
                glEnableClientState(GL_COLOR_ARRAY);
                glColorPointer(componentsPerColor, colorDataType, 0, (GLvoid*)0);
            }
        }
            break;
        case PrivateDrawMode::DRAW_NORMAL:
        {
            glEnableClientState(GL_COLOR_ARRAY);
            CaretAssert(glIsBuffer(openglData->m_colorBufferObject->getBufferObjectName()));
            glBindBuffer(GL_ARRAY_BUFFER, openglData->m_colorBufferObject->getBufferObjectName());
            glColorPointer(openglData->m_componentsPerColor, openglData->m_colorDataType, 0, (GLvoid*)0);
        }
            break;
        case PrivateDrawMode::DRAW_SELECTION:
        {
            const int32_t numberOfVertices = primitive->m_xyz.size() / 3;
            if (numberOfVertices > 0) {
                glGenBuffers(1, &localColorBufferName);
                CaretAssert(localColorBufferName > 0);
                
                const GLint  componentsPerColor = 4;
                const GLenum colorDataType = GL_UNSIGNED_BYTE;
                
                const std::vector<uint8_t> selectionRGBA = primitiveSelectionHelper->getSelectionEncodedRGBA();
                CaretAssert((selectionRGBA.size() / 4) == numberOfVertices);
                
                const GLuint colorSizeBytes = selectionRGBA.size() * sizeof(GLubyte);
                const GLvoid* colorDataPointer = (const GLvoid*)&selectionRGBA[0];
                glBindBuffer(GL_ARRAY_BUFFER,
                             localColorBufferName);
                glBufferData(GL_ARRAY_BUFFER,
                             colorSizeBytes,
                             colorDataPointer,
                             GL_STREAM_DRAW);
                glEnableClientState(GL_COLOR_ARRAY);
                glColorPointer(componentsPerColor, colorDataType, 0, (GLvoid*)0);
            }
        }
            break;
    }
    
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
    
    if (localColorBufferName > 0) {
        glDeleteBuffers(1, &localColorBufferName);
    }
}

/**
 * @return Pointer to the OpenGL graphics context to which the buffers belong.
 */
const void*
GraphicsEngineDataOpenGL::getOpenGLContextPointer() const
{
    return m_openglContextPointer;
}



