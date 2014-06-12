
/*LICENSE_START*/
/*
 *  Copyright (C) 2014  Washington University School of Medicine
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
    m_isApplyColoring = true;
    
    
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
        m_sideNormals.push_back(1.0);
        
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
        CaretAssertVectorIndex(m_sideNormals, i3+2);
        m_sideNormals[i3]   += triangleNormal[0];
        m_sideNormals[i3+1] += triangleNormal[1];
        m_sideNormals[i3+2] += triangleNormal[2];
        
        CaretAssertVectorIndex(m_sideNormals, next3+2);
        m_sideNormals[next3]   += triangleNormal[0];
        m_sideNormals[next3+1] += triangleNormal[1];
        m_sideNormals[next3+2] += triangleNormal[2];
    }
    
    /*
     * Finish creation of the normal vectors
     */
    for (int32_t i = 1; i <= m_numberOfSides; i++) {
        const int32_t i3 = i * 3;
        CaretAssertVectorIndex(m_sideNormals, i3+2);
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
    m_sidesTriangleFan.push_back(m_numberOfSides); // closes
    
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
            CaretAssertVectorIndex(m_coordinates, i3+2);
            std::cout << "points[" << i << "]=(" << AString::fromNumbers(&m_coordinates[i3], 3, ",")
            << ")" << std::endl;
            CaretAssertVectorIndex(m_sideNormals, i3+2);
            std::cout << "side normal[" << i << "]=(" << AString::fromNumbers(&m_sideNormals[i3], 3, ",")
            << ")" << std::endl;
            CaretAssertVectorIndex(m_capNormals, i3+2);
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
    
    /*
     * Create storage for colors
     */
    const int64_t numCoords = static_cast<int64_t>(m_coordinates.size()) / 3;
    const int64_t numRGBA = numCoords * 4;
    m_rgbaByte.resize(numRGBA * 4, 0);
    for (GLuint i = 0; i < numCoords; i++) {
        const int32_t i4 = i * 4;
        
        CaretAssertVectorIndex(m_rgbaByte, i4+3);
        m_rgbaByte[i4]   = 0;
        m_rgbaByte[i4+1] = 0;
        m_rgbaByte[i4+2] = 0;
        m_rgbaByte[i4+3] = 255;
    }
}

/**
 * Destructor.
 */
BrainOpenGLShapeCone::~BrainOpenGLShapeCone()
{
    
}

void
BrainOpenGLShapeCone::setupOpenGLForShape(const BrainOpenGL::DrawMode drawMode)
{
    m_displayList    = 0;
    
    m_coordinatesBufferID = 0;
    m_coordinatesRgbaByteBufferID = 0;
    m_sidesNormalBufferID = 0;
    m_sidesTriangleFanBufferID = 0;
    m_capNormalBufferID = 0;
    m_capTriangleFanBufferID = 0;
    
    switch (drawMode) {
        case BrainOpenGL::DRAW_MODE_DISPLAY_LISTS:
        {
            m_displayList = createDisplayList();
            
            if (m_displayList > 0) {
                glNewList(m_displayList,
                          GL_COMPILE);
                uint8_t rgbaUnused[4] = { 0, 0, 0, 0 };
                m_isApplyColoring = false;
                drawShape(BrainOpenGL::DRAW_MODE_IMMEDIATE,
                          rgbaUnused);
                m_isApplyColoring = true;
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
             * For RGBA coloring
             */
            m_coordinatesRgbaByteBufferID = createBufferID();
            glBindBuffer(GL_ARRAY_BUFFER,
                         m_coordinatesRgbaByteBufferID);
            glBufferData(GL_ARRAY_BUFFER,
                         m_rgbaByte.size() * sizeof(GLubyte),
                         &m_rgbaByte[0],
                         GL_DYNAMIC_DRAW);
            
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
#endif // BRAIN_OPENGL_INFO_SUPPORTS_VERTEX_BUFFERS
            break;
    }
}

/**
 * Draw the shape.
 *
 * @param drawMode
 *   How to draw the shape.
 * @param rgba
 *   RGBA coloring ranging 0.0 to 1.0
 */
void
BrainOpenGLShapeCone::drawShape(const BrainOpenGL::DrawMode drawMode,
                                const float rgba[4])
{
    const uint8_t rgbaByte[4] = {
        static_cast<uint8_t>(rgba[0] * 255.0),
        static_cast<uint8_t>(rgba[1] * 255.0),
        static_cast<uint8_t>(rgba[2] * 255.0),
        static_cast<uint8_t>(rgba[3] * 255.0)
    };

    drawShape(drawMode,
              rgbaByte);
}

/**
 * Draw the shape.
 *
 * @param drawMode
 *   How to draw the shape.
 * @param rgba
 *   RGBA coloring ranging 0 to 255.
 */
void
BrainOpenGLShapeCone::drawShape(const BrainOpenGL::DrawMode drawMode,
                                const uint8_t rgba[4])
{
    switch (drawMode) {
        case BrainOpenGL::DRAW_MODE_DISPLAY_LISTS:
        {
            if (m_displayList > 0) {
                if (m_isApplyColoring) {
                    glColor4ubv(rgba);
                }
                
                glCallList(m_displayList);
            }
        }
            break;
        case BrainOpenGL::DRAW_MODE_IMMEDIATE:
        {
            if (m_isApplyColoring) {
                glColor4ubv(rgba);
            }
            
            const int32_t numSideVertices = static_cast<int32_t>(m_sidesTriangleFan.size());
            glBegin(GL_TRIANGLE_FAN);
            for (int32_t j = 0; j < numSideVertices; j++) {
                const int32_t vertexIndex = m_sidesTriangleFan[j] * 3;
                
                CaretAssertVectorIndex(m_sideNormals, vertexIndex+2);
                CaretAssertVectorIndex(m_coordinates, vertexIndex+2);
                
                glNormal3fv(&m_sideNormals[vertexIndex]);
                glVertex3fv(&m_coordinates[vertexIndex]);
            }
            glEnd();
            
            const int32_t numCapVertices = static_cast<int32_t>(m_capTriangleFan.size());
            glBegin(GL_TRIANGLE_FAN);
            for (int32_t j = 0; j < numCapVertices; j++) {
                const int32_t vertexIndex = m_capTriangleFan[j] * 3;
                
                CaretAssertVectorIndex(m_capNormals, vertexIndex+2);
                CaretAssertVectorIndex(m_coordinates, vertexIndex+2);
                
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
                /*
                 * Enable vertices and normals for buffers
                 */
                glEnableClientState(GL_VERTEX_ARRAY);
                glEnableClientState(GL_NORMAL_ARRAY);
            glEnableClientState(GL_COLOR_ARRAY);
            
                /*
                 * Set the vertices for drawing.
                 */
            CaretAssert(glIsBuffer(m_coordinatesBufferID) == GL_TRUE);
                glBindBuffer(GL_ARRAY_BUFFER,
                             m_coordinatesBufferID);
                glVertexPointer(3,
                                GL_FLOAT,
                                0,
                                (GLvoid*)0);
                
            /*
             * Put BYTE colors into its buffer
             */
            const int64_t numRGBA = static_cast<int64_t>(m_rgbaByte.size()) / 4;
            for (int64_t ir = 0; ir < numRGBA; ir++) {
                const int64_t ir4 = ir * 4;
                m_rgbaByte[ir4]   = rgba[0];
                m_rgbaByte[ir4+1] = rgba[1];
                m_rgbaByte[ir4+2] = rgba[2];
                m_rgbaByte[ir4+3] = rgba[3];
            }
            CaretAssert(glIsBuffer(m_coordinatesRgbaByteBufferID) == GL_TRUE);
            glBindBuffer(GL_ARRAY_BUFFER,
                         m_coordinatesRgbaByteBufferID);
            glBufferData(GL_ARRAY_BUFFER,
                         m_rgbaByte.size() * sizeof(GLubyte),
                         &m_rgbaByte[0],
                         GL_DYNAMIC_DRAW);
            glColorPointer(4,
                           GL_UNSIGNED_BYTE,
                           0,
                           (GLvoid*)0);
            
                /*
                 * Set the side normal vectors for drawing.
                 */
            CaretAssert(glIsBuffer(m_sidesNormalBufferID) == GL_TRUE);
                glBindBuffer(GL_ARRAY_BUFFER,
                             m_sidesNormalBufferID);
                glNormalPointer(GL_FLOAT,
                                0,
                                (GLvoid*)0);
                
                /*
                 * Draw the side triangle fans.
                 */
            CaretAssert(glIsBuffer(m_sidesTriangleFanBufferID) == GL_TRUE);
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
            glDisableClientState(GL_COLOR_ARRAY);
#endif // BRAIN_OPENGL_INFO_SUPPORTS_VERTEX_BUFFERS
            break;
    }
}

