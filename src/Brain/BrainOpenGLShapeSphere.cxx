
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

#define __BRAIN_OPEN_G_L_SHAPE_SPHERE_DECLARE__
#include "BrainOpenGLShapeSphere.h"
#undef __BRAIN_OPEN_G_L_SHAPE_SPHERE_DECLARE__

#include "CaretAssert.h"

using namespace caret;


    
/**
 * \class caret::BrainOpenGLShapeSphere 
 * \brief Create a spherical shape for use with OpenGL.
 */

/**
 * Constructor.
 */
BrainOpenGLShapeSphere::BrainOpenGLShapeSphere(const int32_t numberOfLatitudeAndLongitude)
: BrainOpenGLShape(),
  m_numberOfLatitudeAndLongitude(numberOfLatitudeAndLongitude)
{
    m_displayList    = 0;
    m_vertexBufferID = 0;
}

/**
 * Destructor.
 */
BrainOpenGLShapeSphere::~BrainOpenGLShapeSphere()
{
    
}

void
BrainOpenGLShapeSphere::setupShape(const BrainOpenGL::DrawMode drawMode)
{
    
    const float radius = 1.0;
    const int numLat = m_numberOfLatitudeAndLongitude;
    const int numLon = m_numberOfLatitudeAndLongitude;
    
    const float degToRad = M_PI / 180.0;
    
    const float dLat = 180.0 / numLat;
    for (int iLat = 0; iLat <= numLat; iLat++) {
        const float latDeg = -90.0 + (iLat * dLat);
        const float latRad = latDeg * degToRad;
        
        const float z1 = radius * std::sin(latRad);
        
        const float latDeg2 = -90.0 + ((iLat + 1) * dLat);
        const float latRad2 = latDeg2 * degToRad;
        
        const float z2 = radius * std::sin(latRad2);
        
        m_quadStripVerticesStartIndex.push_back(static_cast<int>(m_vertices.size()));
        
        const float dLon = 360.0 / numLon;
        for (int iLon = 0; iLon <= numLon; iLon++) {
            const float lonDeg = iLon * dLon;
            const float lonRad = lonDeg * degToRad;
            
            const float x1 = radius * std::cos(latRad) * std::cos(lonRad);
            const float y1 = radius * std::cos(latRad) * std::sin(lonRad);
            
            const float x2 = radius * std::cos(latRad2) * std::cos(lonRad);
            const float y2 = radius * std::cos(latRad2) * std::sin(lonRad);
            
            m_vertices.push_back(static_cast<int>(m_coordinates.size() / 3));
            m_coordinates.push_back(x2);
            m_coordinates.push_back(y2);
            m_coordinates.push_back(z2);
            const float length2 = std::sqrt(x2*x2 + y2*y2 + z2*z2);
            m_normals.push_back(x2 / length2);
            m_normals.push_back(y2 / length2);
            m_normals.push_back(z2 / length2);
            
            m_vertices.push_back(static_cast<int>(m_coordinates.size() / 3));
            m_coordinates.push_back(x1);
            m_coordinates.push_back(y1);
            m_coordinates.push_back(z1);
            const float length1 = std::sqrt(x1*x1 + y1*y1 + z1*z1);
            m_normals.push_back(x1 / length1);
            m_normals.push_back(y1 / length1);
            m_normals.push_back(z1 / length1);
        }
        
        m_quadStripVerticesEndIndex.push_back(static_cast<int>(m_vertices.size()));
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
        {
            CaretAssert(0);
        }
            break;
    }
}

/**
 * Draw the sphere.
 * @param drawMode
 *   How to draw the shape.
 */
void
BrainOpenGLShapeSphere::drawShape(const BrainOpenGL::DrawMode drawMode)
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
            const int32_t numQuadStrips = m_quadStripVerticesEndIndex.size();
            for (int32_t i = 0; i < numQuadStrips; i++) {
                const int32_t vertexStart = m_quadStripVerticesStartIndex[i];
                const int32_t vertexEnd = m_quadStripVerticesEndIndex[i];
                glBegin(GL_QUAD_STRIP);
                for (int32_t iVertex = vertexStart; iVertex < vertexEnd; iVertex++) {
                    CaretAssertVectorIndex(m_vertices, iVertex);
                    const int32_t vertexIndex = m_vertices[iVertex];
                    const int32_t v3 = vertexIndex * 3;
                    
                    CaretAssertVectorIndex(m_normals, v3);
                    CaretAssertVectorIndex(m_coordinates, v3);
                    
                    glNormal3fv(&m_normals[v3]);
                    glVertex3fv(&m_coordinates[v3]);
                }
                glEnd();
            }
        }
            break;
        case BrainOpenGL::DRAW_MODE_INVALID:
        {
            CaretAssert(0);
        }
            break;
        case BrainOpenGL::DRAW_MODE_VERTEX_BUFFERS:
        {
            CaretAssert(0);            
        }
            break;
    }
}

