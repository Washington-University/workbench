
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

#define __BRAIN_OPEN_GL_SHAPE_RING_DECLARE__
#include "BrainOpenGLShapeRing.h"
#undef __BRAIN_OPEN_GL_SHAPE_RING_DECLARE__

#include "CaretAssert.h"
#include "MathFunctions.h"

using namespace caret;


    
/**
 * \class caret::BrainOpenGLShapeRing 
 * \brief Create a conical shape for use with OpenGL.
 */

/**
 * Constructor.
 * @param numberOfSides
 *    Number of sides in the ring.
 */
BrainOpenGLShapeRing::BrainOpenGLShapeRing(const int32_t numberOfSides,
                                           const float innerRadius,
                                           const float outerRadius)
: BrainOpenGLShape(),
  m_numberOfSides(numberOfSides),
  m_innerRadius(innerRadius),
  m_outerRadius(outerRadius)
{
    m_displayList    = 0;
    
    m_coordinatesBufferID = 0;
    m_normalBufferID = 0;
    m_triangleStripBufferID = 0;
}

/**
 * Destructor.
 */
BrainOpenGLShapeRing::~BrainOpenGLShapeRing()
{
    
}

void
BrainOpenGLShapeRing::setupShape(const BrainOpenGL::DrawMode drawMode)
{
    bool debugFlag = false;
    
    /*
     * Setup step size based upon number of points around circle
     */
//    const int32_t numberOfPoints = 8;
    const float step = (2.0 * M_PI) / m_numberOfSides;
    
    /*
     * Generate points around ring
     */
    const float z = 0.0;
    for (int32_t i = 0; i < m_numberOfSides; i++) {
        const float t = step * i;
        
        //        const float x = majorRadius * std::cos(t);
        //        const float y = minorRadius * std::sin(t);
        const float xin = m_innerRadius * std::cos(t);
        const float yin = m_innerRadius * std::sin(t);
        const float xout = m_outerRadius * std::cos(t);
        const float yout = m_outerRadius * std::sin(t);
        
        m_triangleStrip.push_back(m_coordinates.size() / 3);
        
        m_coordinates.push_back(xin);
        m_coordinates.push_back(yin);
        m_coordinates.push_back(z);
        
        m_normals.push_back(0.0);
        m_normals.push_back(0.0);
        m_normals.push_back(1.0);
        
        m_triangleStrip.push_back(m_coordinates.size() / 3);
        
        m_coordinates.push_back(xout);
        m_coordinates.push_back(yout);
        m_coordinates.push_back(z);
        
        m_normals.push_back(0.0);
        m_normals.push_back(0.0);
        m_normals.push_back(1.0);
    }
    
    if (m_numberOfSides > 0) {
        m_triangleStrip.push_back(0);
        m_triangleStrip.push_back(1);
    }
    
    if (debugFlag) {
        CaretAssert(m_coordinates.size() == m_normals.size());
        const int32_t numPoints = static_cast<int32_t>(m_coordinates.size() / 3);
        for (int32_t i = 0; i < numPoints; i++) {
            const int32_t i3 = i * 3;
            std::cout << "points[" << i << "]=(" << AString::fromNumbers(&m_coordinates[i3], 3, ",")
            << ")" << std::endl;
            std::cout << "side normal[" << i << "]=(" << AString::fromNumbers(&m_normals[i3], 3, ",")
            << ")" << std::endl;
        }
        
        std::cout << "Sides: ";
        printTriangleStrip(m_triangleStrip);
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
                m_normalBufferID = createBufferID();
                glBindBuffer(GL_ARRAY_BUFFER,
                             m_normalBufferID);
                glBufferData(GL_ARRAY_BUFFER,
                             m_normals.size() * sizeof(GLfloat),
                             &m_normals[0],
                             GL_STATIC_DRAW);
                
                /*
                 * Put sides triangle strip into its buffer.
                 */
                m_triangleStripBufferID = createBufferID();
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,
                             m_triangleStripBufferID);
                glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                             m_triangleStrip.size() * sizeof(GLuint),
                             &m_triangleStrip[0],
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
 * Draw the ring.
 * @param drawMode
 *   How to draw the shape.
 */
void
BrainOpenGLShapeRing::drawShape(const BrainOpenGL::DrawMode drawMode)
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
            const int32_t numVertices = static_cast<int32_t>(m_triangleStrip.size());
            glBegin(GL_TRIANGLE_STRIP);
            for (int32_t j = 0; j < numVertices; j++) {
                const int32_t vertexIndex = m_triangleStrip[j] * 3;
                
                CaretAssertVectorIndex(m_normals, vertexIndex);
                CaretAssertVectorIndex(m_coordinates, vertexIndex);
                
                glNormal3fv(&m_normals[vertexIndex]);
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
                             m_normalBufferID);
                glNormalPointer(GL_FLOAT,
                                0,
                                (GLvoid*)0);
                
                /*
                 * Draw the side triangle fans.
                 */
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,
                             m_triangleStripBufferID);
                glDrawElements(GL_TRIANGLE_FAN,
                               m_triangleStrip.size(),
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

