
/*LICENSE_START*/
/*
 * Copyright 2013 Washington University,
 * All rights reserved.
 *
 * Connectome DB and Connectome Workbench are part of the integrated Connectome 
 * Informatics Platform.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    * Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *    * Neither the names of Washington University nor the
 *      names of its contributors may be used to endorse or promote products
 *      derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR  
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
 * SUBSTITUTE GOODS OR  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
 * POSSIBILITY OF SUCH DAMAGE.
 */
/*LICENSE_END*/

#include <cmath>

#define __BRAIN_OPEN_G_L_SHAPE_CYLINDER_DECLARE__
#include "BrainOpenGLShapeCylinder.h"
#undef __BRAIN_OPEN_G_L_SHAPE_CYLINDER_DECLARE__

#include "CaretAssert.h"
#include "MathFunctions.h"

using namespace caret;
    
/**
 * \class caret::BrainOpenGLShapeCylinder 
 * \brief Creates a cylinder shape for use with OpenGL.
 * \ingroup Brain
 */

/**
 * Constructor.
 */
BrainOpenGLShapeCylinder::BrainOpenGLShapeCylinder(const int32_t numberOfSides)
: BrainOpenGLShape(),
m_numberOfSides(numberOfSides)
{
    m_displayList    = 0;
    
    m_coordinatesBufferID = 0;
    m_coordinatesRGBABufferID =0;
    m_sidesNormalBufferID = 0;
    m_sidesTriangleStripBufferID = 0;
    m_topCapNormalBufferID = 0;
    m_topCapTriangleFanBufferID = 0;
    m_bottomCapNormalBufferID = 0;
    m_bottomCapTriangleFanBufferID = 0;
}

/**
 * Destructor.
 */
BrainOpenGLShapeCylinder::~BrainOpenGLShapeCylinder()
{
}

void
BrainOpenGLShapeCylinder::setupShape(const BrainOpenGL::DrawMode drawMode)
{
    bool debugFlag = false;
    
    
    /*
     * Setup step size based upon number of points around circle
     */
    const float step = (2.0 * M_PI) / m_numberOfSides;
    
    
    /*
     * Z-coordinates at bottom and top of cylinder
     */
    const float zTop =     1.0;
    const float zBottom =  0.0;
    
    /*
     * Counts vertices
     */
    int32_t vertexCounter = 0;
    
    const float topNormalX = 0.0;
    const float topNormalY = 0.0;
    const float topNormalZ = 1.0;
    
    const float bottomNormalX =  0.0;
    const float bottomNormalY =  0.0;
    const float bottomNormalZ = -1.0;
    
//    /*
//     * Center of top
//     */
//    m_coordinates.push_back(0.0);
//    m_coordinates.push_back(0.0);
//    m_coordinates.push_back(1.0);
//    m_topNormals.push_back(topNormalX);
//    m_topNormals.push_back(topNormalY);
//    m_topNormals.push_back(topNormalZ);
//    m_topTriangleFan.push_back(vertexCounter);
//    vertexCounter++;
//    
//    /*
//     * Center of bottom
//     */
//    m_coordinates.push_back(0.0);
//    m_coordinates.push_back(0.0);
//    m_coordinates.push_back(-1.0);
//    m_bottomNormals.push_back(bottomNormalX);
//    m_bottomNormals.push_back(bottomNormalY);
//    m_bottomNormals.push_back(bottomNormalZ);
//    m_bottomTriangleFan.push_back(vertexCounter);
//    vertexCounter++;
    
    const int32_t firstSideVertex = vertexCounter;
    
    /*
     * Generate points around cylinder
     */
    const float radius = 0.5;
    for (int32_t i = 0; i < m_numberOfSides; i++) {
        const float t = step * i;
        
        const float x = radius * std::cos(t);
        const float y = radius * std::sin(t);

        m_coordinates.push_back(x);
        m_coordinates.push_back(y);
        m_coordinates.push_back(zTop);
        
        m_sidesNormals.push_back(x);
        m_sidesNormals.push_back(y);
        m_sidesNormals.push_back(0.0);
        m_sidesTriangleStrip.push_back(vertexCounter);

        m_topNormals.push_back(topNormalX);
        m_topNormals.push_back(topNormalY);
        m_topNormals.push_back(topNormalZ);
        m_topTriangleFan.push_back(vertexCounter);

        vertexCounter++;
        
        m_coordinates.push_back(x);
        m_coordinates.push_back(y);
        m_coordinates.push_back(zBottom);
        
        m_sidesNormals.push_back(x);
        m_sidesNormals.push_back(y);
        m_sidesNormals.push_back(0.0);
        m_sidesTriangleStrip.push_back(vertexCounter);
        
        m_bottomNormals.push_back(bottomNormalX);
        m_bottomNormals.push_back(bottomNormalY);
        m_bottomNormals.push_back(bottomNormalZ);
        m_bottomTriangleFan.push_back(vertexCounter);
        
        vertexCounter++;
    }

    CaretAssert((vertexCounter * 3) == static_cast<int32_t>(m_coordinates.size()));
    /*
     * Finish cylinder by specifying first two coordinates again
     */
    m_sidesTriangleStrip.push_back(firstSideVertex);
    m_sidesTriangleStrip.push_back(firstSideVertex + 1);
    
    /*
     * Finish top and bottom
     * Note bottom vertices need to be reverse so that 
     * that the vertices are counter clockwise when pointing down.
     */
    m_topNormals.push_back(m_topNormals[0]);
    m_topNormals.push_back(m_topNormals[1]);
    m_topNormals.push_back(m_topNormals[2]);
    m_topTriangleFan.push_back(firstSideVertex);

    m_bottomNormals.push_back(m_bottomNormals[0]);
    m_bottomNormals.push_back(m_bottomNormals[1]);
    m_bottomNormals.push_back(m_bottomNormals[2]);
    m_bottomTriangleFan.push_back(firstSideVertex + 1);
    std::reverse(m_bottomTriangleFan.begin(),
                 m_bottomTriangleFan.end());

    CaretAssert(m_topNormals.size() == (m_topTriangleFan.size() * 3));
    CaretAssert(m_sidesNormals.size() == ((m_sidesTriangleStrip.size() - 2) * 3));
    CaretAssert(m_topNormals.size() == (m_topTriangleFan.size() * 3));

    /*
     * Create storage for colors
     */
    m_coordinatesRGBA.resize(vertexCounter * 4, 0.0);
    for (int32_t i = 0; i < vertexCounter; i++) {
        const int32_t i4 = i * 4;
        m_coordinatesRGBA[i4] = 0.0;
        m_coordinatesRGBA[i4+1] = 0.0;
        m_coordinatesRGBA[i4+2] = 1.0;
        m_coordinatesRGBA[i4+3] = 1.0;
    }
        
    switch (drawMode) {
        case BrainOpenGL::DRAW_MODE_DISPLAY_LISTS:
        {
            m_displayList = createDisplayList();
            
            if (m_displayList > 0) {
                glNewList(m_displayList,
                          GL_COMPILE);
                drawShape(BrainOpenGL::DRAW_MODE_IMMEDIATE);
                glEndList();
            }
        }
            break;
        case BrainOpenGL::DRAW_MODE_IMMEDIATE:
        {
            /* nothing to do for this case */
        }
            break;
        case BrainOpenGL::DRAW_MODE_INVALID:
        {
            CaretAssert(0);
        }
            break;
        case BrainOpenGL::DRAW_MODE_VERTEX_BUFFERS:
#ifdef BRAIN_OPENGL_INFO_SUPPORTS_VERTEX_BUFFERS
            if (BrainOpenGL::isVertexBuffersSupported()) {
                /*
                 * Put coordinates into its buffer.
                 */
                m_coordinatesBufferID = createBufferID();
                glBindBuffer(GL_ARRAY_BUFFER,
                             m_coordinatesBufferID);
                glBufferData(GL_ARRAY_BUFFER,
                             m_coordinates.size() * sizeof(GLfloat),
                             &m_coordinates[0],
                             GL_STATIC_DRAW);
                
                /*
                 * Put colors into its buffer
                 */
                m_coordinatesRGBABufferID = createBufferID();
                glBindBuffer(GL_ARRAY_BUFFER,
                             m_coordinatesRGBABufferID);
                glBufferData(GL_ARRAY_BUFFER,
                             m_coordinatesRGBA.size() * sizeof(GLfloat),
                             &m_coordinatesRGBA[0],
                             GL_DYNAMIC_DRAW);
                
                
                /*
                 * Put side normals into its buffer.
                 */
                m_sidesNormalBufferID = createBufferID();
                glBindBuffer(GL_ARRAY_BUFFER,
                             m_sidesNormalBufferID);
                glBufferData(GL_ARRAY_BUFFER,
                             m_sidesNormals.size() * sizeof(GLfloat),
                             &m_sidesNormals[0],
                             GL_STATIC_DRAW);
                
                /*
                 * Put sides triangle strip for sides into its buffer.
                 */
                m_sidesTriangleStripBufferID = createBufferID();
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,
                             m_sidesTriangleStripBufferID);
                glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                             m_sidesTriangleStrip.size() * sizeof(GLuint),
                             &m_sidesTriangleStrip[0],
                             GL_STATIC_DRAW);
                
                /*
                 * Put top cap normals into its buffer.
                 */
                m_topCapNormalBufferID = createBufferID();
                glBindBuffer(GL_ARRAY_BUFFER,
                             m_topCapNormalBufferID);
                glBufferData(GL_ARRAY_BUFFER,
                             m_topNormals.size() * sizeof(GLfloat),
                             &m_topNormals[0],
                             GL_STATIC_DRAW);
                
                /*
                 * Put top cap triangle fan for cap into its buffer.
                 */
                m_topCapTriangleFanBufferID = createBufferID();
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,
                             m_topCapTriangleFanBufferID);
                glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                             m_topTriangleFan.size() * sizeof(GLuint),
                             &m_topTriangleFan[0],
                             GL_STATIC_DRAW);
                
                /*
                 * Put bottom cap normals into its buffer.
                 */
                m_bottomCapNormalBufferID = createBufferID();
                glBindBuffer(GL_ARRAY_BUFFER,
                             m_bottomCapNormalBufferID);
                glBufferData(GL_ARRAY_BUFFER,
                             m_bottomNormals.size() * sizeof(GLfloat),
                             &m_bottomNormals[0],
                             GL_STATIC_DRAW);
                
                /*
                 * Put bottom cap triangle fan for cap into its buffer.
                 */
                m_bottomCapTriangleFanBufferID = createBufferID();
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,
                             m_bottomCapTriangleFanBufferID);
                glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                             m_bottomTriangleFan.size() * sizeof(GLuint),
                             &m_bottomTriangleFan[0],
                             GL_STATIC_DRAW);
                
                /*
                 * Deselect active buffer.
                 */
                glBindBuffer(GL_ARRAY_BUFFER,
                             0);
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,
                             0);
            }
#endif // BRAIN_OPENGL_INFO_SUPPORTS_VERTEX_BUFFERS
            break;
    }
}

/**
 * Draw the cone.
 * @param drawMode
 *   How to draw the shape.
 */
void
BrainOpenGLShapeCylinder::drawShape(const BrainOpenGL::DrawMode drawMode)
{
    switch (drawMode) {
        case BrainOpenGL::DRAW_MODE_DISPLAY_LISTS:
        {
            if (m_displayList > 0) {
                glCallList(m_displayList);
            }
        }
            break;
        case BrainOpenGL::DRAW_MODE_IMMEDIATE:
        {
//            /*
//             * Draw the top
//             */
//            const int32_t numTopVertices = static_cast<int32_t> (m_topTriangleFan.size());
//            glBegin(GL_TRIANGLE_FAN);
//            for (int32_t i = 0; i < numTopVertices; i++) {
//                CaretAssertVectorIndex(m_topTriangleFan, i);
//                const int32_t fanIndex = m_topTriangleFan[i];
//                const int32_t coordinateOffset = fanIndex * 3;
//                const int32_t colorOffset = fanIndex * 4;
//                const int32_t normalOffset = i * 3;
//                
//                CaretAssertVectorIndex(m_coordinates, coordinateOffset + 2);
//                CaretAssertVectorIndex(m_topNormals, normalOffset + 2);
//                CaretAssertVectorIndex(m_coordinatesRGBA, colorOffset + 3);
//                
//                glColor4fv(&m_coordinatesRGBA[colorOffset]);
//                glNormal3fv(&m_topNormals[normalOffset]);
//                glVertex3fv(&m_coordinates[coordinateOffset]);
//            }
//            glEnd();
//            
//            /*
//             * Draw the bottom
//             */
//            const int32_t numBottomVertices = static_cast<int32_t> (m_bottomTriangleFan.size());
//            glBegin(GL_TRIANGLE_FAN);
//            for (int32_t i = 0; i < numBottomVertices; i++) {
//                CaretAssertVectorIndex(m_bottomTriangleFan, i);
//                const int32_t fanIndex = m_bottomTriangleFan[i];
//                const int32_t coordinateOffset = fanIndex * 3;
//                const int32_t colorOffset = fanIndex * 4;
//                const int32_t normalOffset = i * 3;
//                
//                CaretAssertVectorIndex(m_coordinates, coordinateOffset + 2);
//                CaretAssertVectorIndex(m_bottomNormals, normalOffset + 2);
//                CaretAssertVectorIndex(m_coordinatesRGBA, colorOffset + 3);
//                
//                glColor4fv(&m_coordinatesRGBA[colorOffset]);
//                glNormal3fv(&m_bottomNormals[normalOffset]);
//                glVertex3fv(&m_coordinates[coordinateOffset]);
//            }
//            glEnd();
            
            /*
             * Draw the sides
             */
            const int32_t numSideVertices = static_cast<int32_t>(m_sidesTriangleStrip.size());
            glBegin(GL_TRIANGLE_STRIP);
            for (int32_t i = 0; i < numSideVertices; i++) {
                CaretAssertVectorIndex(m_sidesTriangleStrip, i);
                const int32_t stripIndex = m_sidesTriangleStrip[i];
                const int32_t coordinateOffset = stripIndex * 3;
                const int32_t colorOffset = stripIndex * 4;
                const int32_t normalOffset = stripIndex * 3;
                
                CaretAssertVectorIndex(m_coordinates, coordinateOffset + 2);
                CaretAssertVectorIndex(m_sidesNormals, normalOffset + 2);
                CaretAssertVectorIndex(m_coordinatesRGBA, colorOffset + 3);
                
                glColor4fv(&m_coordinatesRGBA[colorOffset]);
                glNormal3fv(&m_sidesNormals[normalOffset]);
                glVertex3fv(&m_coordinates[coordinateOffset]);
            }
            glEnd();
        }
            break;
        case BrainOpenGL::DRAW_MODE_INVALID:
        {
            CaretAssert(0);
        }
            break;
        case BrainOpenGL::DRAW_MODE_VERTEX_BUFFERS:
#ifdef BRAIN_OPENGL_INFO_SUPPORTS_VERTEX_BUFFERS
            if (BrainOpenGL::isVertexBuffersSupported()) {
                /*
                 * Enable vertices and normals for buffers
                 */
                glEnableClientState(GL_VERTEX_ARRAY);
                glEnableClientState(GL_NORMAL_ARRAY);
                glEnableClientState(GL_COLOR_ARRAY);
                
                /*
                 * Set the vertices for drawing.
                 */
                glBindBuffer(GL_ARRAY_BUFFER,
                             m_coordinatesBufferID);
                glVertexPointer(3,
                                GL_FLOAT,
                                0,
                                (GLvoid*)0);
                
                /*
                 * Set color components for drawing
                 */
                glBindBuffer(GL_ARRAY_BUFFER,
                             m_coordinatesRGBABufferID);
                glColorPointer(4,
                                GL_FLOAT,
                                0,
                                (GLvoid*)0);
                
                /*
                 * Set the side normal vectors for drawing.
                 */
                glBindBuffer(GL_ARRAY_BUFFER,
                             m_sidesNormalBufferID);
                glNormalPointer(GL_FLOAT,
                                0,
                                (GLvoid*)0);
                
                /*
                 * Draw the side triangle strip.
                 */
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,
                             m_sidesTriangleStripBufferID);
                glDrawElements(GL_TRIANGLE_STRIP,
                               m_sidesTriangleStrip.size(),
                               GL_UNSIGNED_INT,
                               (GLvoid*)0);
                /*
                 * Set the top cap normal vectors for drawing.
                 */
                glBindBuffer(GL_ARRAY_BUFFER,
                             m_topCapNormalBufferID);
                glNormalPointer(GL_FLOAT,
                                0,
                                (GLvoid*)0);
                
                /*
                 * Draw the top cap triangle fans.
                 */
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,
                             m_topCapTriangleFanBufferID);
                glDrawElements(GL_TRIANGLE_FAN,
                               m_topTriangleFan.size(),
                               GL_UNSIGNED_INT,
                               (GLvoid*)0);
                
                /*
                 * Set the bottom cap normal vectors for drawing.
                 */
                glBindBuffer(GL_ARRAY_BUFFER,
                             m_bottomCapNormalBufferID);
                glNormalPointer(GL_FLOAT,
                                0,
                                (GLvoid*)0);
                
                /*
                 * Draw the bottom cap triangle fans.
                 */
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,
                             m_bottomCapTriangleFanBufferID);
                glDrawElements(GL_TRIANGLE_FAN,
                               m_bottomTriangleFan.size(),
                               GL_UNSIGNED_INT,
                               (GLvoid*)0);
                
                /*
                 * Deselect active buffer.
                 */
                glBindBuffer(GL_ARRAY_BUFFER,
                             0);
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,
                             0);
                
                /*
                 * Disable vertices and normals for buffers.
                 * Otherwise, bad thing will happen.
                 */
                glDisableClientState(GL_VERTEX_ARRAY);
                glDisableClientState(GL_NORMAL_ARRAY);
                glDisableClientState(GL_COLOR_ARRAY);
            }
#endif // BRAIN_OPENGL_INFO_SUPPORTS_VERTEX_BUFFERS
            break;
    }
}
