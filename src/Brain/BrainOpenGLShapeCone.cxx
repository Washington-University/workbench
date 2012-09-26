
/*LICENSE_START*/
/*
 * Copyright 2012 Washington University,
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

#define __BRAIN_OPEN_GL_SHAPE_CONE_DECLARE__
#include "BrainOpenGLShapeCone.h"
#undef __BRAIN_OPEN_GL_SHAPE_CONE_DECLARE__

#include "CaretAssert.h"
#include "MathFunctions.h"

using namespace caret;


    
/**
 * \class caret::BrainOpenGLShapeCone 
 * \brief Create a conical shape for use with OpenGL.
 */

/**
 * Constructor.
 * @param numberOfSides
 *    Number of sides in the cone.
 */
BrainOpenGLShapeCone::BrainOpenGLShapeCone(const int32_t numberOfSides)
: BrainOpenGLShape(),
  m_numberOfSides(numberOfSides)
{
    m_displayList    = 0;
    
    m_coordinatesBufferID = 0;
    m_sidesNormalBufferID = 0;
    m_sidesTriangleFanBufferID = 0;
    m_capNormalBufferID = 0;
    m_capTriangleFanBufferID = 0;
}

/**
 * Destructor.
 */
BrainOpenGLShapeCone::~BrainOpenGLShapeCone()
{
    
}

void
BrainOpenGLShapeCone::setupShape(const BrainOpenGL::DrawMode drawMode)
{
    bool debugFlag = false;
    
    
    /*
     * Add origin (apex) to points
     */
    const int32_t apexIndex = static_cast<int32_t>(m_coordinates.size() / 3);
    const float origin[3] = { 0.0, 0.0, 0.0 };
    m_coordinates.push_back(origin[0]);
    m_coordinates.push_back(origin[1]);
    m_coordinates.push_back(origin[2]);
    
    m_sideNormals.push_back(0.0);
    m_sideNormals.push_back(0.0);
    m_sideNormals.push_back(-1.0);
    
    m_capNormals.push_back(0.0);
    m_capNormals.push_back(0.0);
    m_capNormals.push_back(1.0);
    
    
    /*
     * Setup step size based upon number of points around circle
     */
//    const int32_t numberOfPoints = 8;
    const float step = (2.0 * M_PI) / m_numberOfSides;
    
    /*
     * Generate points around circle
     */
    const float radius = 0.5;
    const float z = 1.0;
    for (int32_t i = 0; i < m_numberOfSides; i++) {
        const float t = step * i;
        
        //        const float x = majorRadius * std::cos(t);
        //        const float y = minorRadius * std::sin(t);
        const float x = radius * std::cos(t);
        const float y = radius * std::sin(t);
        
        m_coordinates.push_back(x);
        m_coordinates.push_back(y);
        m_coordinates.push_back(z);
        
        m_sideNormals.push_back(0.0);
        m_sideNormals.push_back(0.0);
        m_sideNormals.push_back(0.0);
        
        m_capNormals.push_back(0.0);
        m_capNormals.push_back(0.0);
        m_capNormals.push_back(1.0);
    }
    
    /*
     * Add cap center
     */
    const int32_t capCenterIndex = static_cast<int32_t>(m_coordinates.size() / 3);
    m_coordinates.push_back(origin[0]);
    m_coordinates.push_back(origin[1]);
    m_coordinates.push_back(origin[2]);
    
    m_sideNormals.push_back(0.0);
    m_sideNormals.push_back(0.0);
    m_sideNormals.push_back(1.0);

    m_capNormals.push_back(0.0);
    m_capNormals.push_back(0.0);
    m_capNormals.push_back(1.0);
    
    /*
     * Add normal vectors of triangles into normal vector summations
     * NOTE: Vertex at index zero is the apex
     */
    for (int32_t i = 1; i <= m_numberOfSides; i++) {
        int32_t nextIndex = i + 1;
        if (nextIndex > m_numberOfSides) {
            nextIndex = 1;
        }
        
        /*
         * Normal of triangle
         */
        const int32_t i3 = i * 3;
        const int32_t next3 = nextIndex * 3;
        float triangleNormal[3];
        MathFunctions::normalVector(origin,
                                    &m_coordinates[i3],
                                    &m_coordinates[next3],
                                    triangleNormal);
        m_sideNormals[i3]   += triangleNormal[0];
        m_sideNormals[i3+1] += triangleNormal[1];
        m_sideNormals[i3+2] += triangleNormal[2];
        
        m_sideNormals[next3]   += triangleNormal[0];
        m_sideNormals[next3+1] += triangleNormal[1];
        m_sideNormals[next3+2] += triangleNormal[2];
    }
    
    /*
     * Finish creation of the normal vectors
     */
    for (int32_t i = 1; i <= m_numberOfSides; i++) {
        const int32_t i3 = i * 3;
        m_sideNormals[i3]   /= 2.0; // vertices are shared by two triangles
        m_sideNormals[i3+1] /= 2.0;
        m_sideNormals[i3+2] /= 2.0;
        MathFunctions::normalizeVector(&m_sideNormals[i3]);
    }
    
    /*
     * Generate vector list for triangles
     */
    m_sidesTriangleFan.push_back(apexIndex);
    for (int32_t i = m_numberOfSides; i > 0; i--) {
        m_sidesTriangleFan.push_back(i);
    }
    m_sidesTriangleFan.push_back(m_numberOfSides);
    
    m_capTriangleFan.push_back(capCenterIndex);
    for (int32_t i = 1; i <= m_numberOfSides; i++) {
        m_capTriangleFan.push_back(i);
    }
    m_capTriangleFan.push_back(1);
    

    if (debugFlag) {
        CaretAssert(m_coordinates.size() == m_sideNormals.size());
        CaretAssert(m_coordinates.size() == m_capNormals.size());
        const int32_t numPoints = static_cast<int32_t>(m_coordinates.size() / 3);
        for (int32_t i = 0; i < numPoints; i++) {
            const int32_t i3 = i * 3;
            std::cout << "points[" << i << "]=(" << AString::fromNumbers(&m_coordinates[i3], 3, ",")
            << ")" << std::endl;
            std::cout << "side normal[" << i << "]=(" << AString::fromNumbers(&m_sideNormals[i3], 3, ",")
            << ")" << std::endl;
            std::cout << "cap normal[" << i << "]=(" << AString::fromNumbers(&m_capNormals[i3], 3, ",")
            << ")" << std::endl;
        }
        
        std::cout << "Sides: ";
        printTriangleFan(m_sidesTriangleFan);
        std::cout << std::endl;
        std::cout << "Cap: ";
        printTriangleFan(m_capTriangleFan);
        std::cout << std::endl;
    }
    
//    /*
//     * End Cap
//     */
//    glBegin(GL_POLYGON);
//    glNormal3f(0.0, 0.0, 1.0);
//    for (int32_t i = 0; i < m_numberOfSides; i++) {
//        const int32_t i3 = i * 3;
//        glVertex3fv(&m_coordinates[i3]);
//    }
//    glEnd();
    

    
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
                 * Put side normals into its buffer.
                 */
                m_sidesNormalBufferID = createBufferID();
                glBindBuffer(GL_ARRAY_BUFFER,
                             m_sidesNormalBufferID);
                glBufferData(GL_ARRAY_BUFFER,
                             m_sideNormals.size() * sizeof(GLfloat),
                             &m_sideNormals[0],
                             GL_STATIC_DRAW);
                
                /*
                 * Put sides triangle fan for sides into its buffer.
                 */
                m_sidesTriangleFanBufferID = createBufferID();
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,
                             m_sidesTriangleFanBufferID);
                glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                             m_sidesTriangleFan.size() * sizeof(GLuint),
                             &m_sidesTriangleFan[0],
                             GL_STATIC_DRAW);
                
                /*
                 * Put cap normals into its buffer.
                 */
                m_capNormalBufferID = createBufferID();
                glBindBuffer(GL_ARRAY_BUFFER,
                             m_capNormalBufferID);
                glBufferData(GL_ARRAY_BUFFER,
                             m_capNormals.size() * sizeof(GLfloat),
                             &m_capNormals[0],
                             GL_STATIC_DRAW);
                
                /*
                 * Put cap triangle fan for cap into its buffer.
                 */
                m_capTriangleFanBufferID = createBufferID();
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,
                             m_capTriangleFanBufferID);
                glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                             m_capTriangleFan.size() * sizeof(GLuint),
                             &m_capTriangleFan[0],
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
BrainOpenGLShapeCone::drawShape(const BrainOpenGL::DrawMode drawMode)
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
            const int32_t numSideVertices = static_cast<int32_t>(m_sidesTriangleFan.size());
            glBegin(GL_TRIANGLE_FAN);
            for (int32_t j = 0; j < numSideVertices; j++) {
                const int32_t vertexIndex = m_sidesTriangleFan[j] * 3;
                
                CaretAssertVectorIndex(m_sideNormals, vertexIndex);
                CaretAssertVectorIndex(m_coordinates, vertexIndex);
                
                glNormal3fv(&m_sideNormals[vertexIndex]);
                glVertex3fv(&m_coordinates[vertexIndex]);
            }
            glEnd();
            
            const int32_t numCapVertices = static_cast<int32_t>(m_capTriangleFan.size());
            glBegin(GL_TRIANGLE_FAN);
            for (int32_t j = 0; j < numCapVertices; j++) {
                const int32_t vertexIndex = m_capTriangleFan[j] * 3;
                
                CaretAssertVectorIndex(m_capNormals, vertexIndex);
                CaretAssertVectorIndex(m_coordinates, vertexIndex);
                
                glNormal3fv(&m_capNormals[vertexIndex]);
                glVertex3fv(&m_coordinates[vertexIndex]);
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
                 * Set the side normal vectors for drawing.
                 */
                glBindBuffer(GL_ARRAY_BUFFER,
                             m_sidesNormalBufferID);
                glNormalPointer(GL_FLOAT,
                                0,
                                (GLvoid*)0);
                
                /*
                 * Draw the side triangle fans.
                 */
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,
                             m_sidesTriangleFanBufferID);
                glDrawElements(GL_TRIANGLE_FAN,
                               m_sidesTriangleFan.size(),
                               GL_UNSIGNED_INT,
                               (GLvoid*)0);
                /*
                 * Set the cap normal vectors for drawing.
                 */
                glBindBuffer(GL_ARRAY_BUFFER,
                             m_capNormalBufferID);
                glNormalPointer(GL_FLOAT,
                                0,
                                (GLvoid*)0);
                
                /*
                 * Draw the cap triangle fans.
                 */
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,
                             m_capTriangleFanBufferID);
                glDrawElements(GL_TRIANGLE_FAN,
                               m_capTriangleFan.size(),
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
            }
#endif // BRAIN_OPENGL_INFO_SUPPORTS_VERTEX_BUFFERS
            break;
    }
}

