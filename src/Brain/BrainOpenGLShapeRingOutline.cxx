
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

#define __BRAIN_OPEN_GL_SHAPE_RING_OUTLINE_DECLARE__
#include "BrainOpenGLShapeRingOutline.h"
#undef __BRAIN_OPEN_GL_SHAPE_RING_OUTLINE_DECLARE__

#include "CaretAssert.h"
#include "MathFunctions.h"

using namespace caret;



/**
 * \class caret::BrainOpenGLShapeRingOutline
 * \brief Create a ring shape drawn with a line for use with OpenGL.
 */

/**
 * Constructor.
 * @param numberOfSides
 *    Number of sides in the ring.
 */
BrainOpenGLShapeRingOutline::BrainOpenGLShapeRingOutline(const int32_t numberOfSides,
                                                         const float radius,
                                                         const float lineThickness)
: BrainOpenGLShape(),
m_numberOfSides(numberOfSides),
m_radius(radius),
m_lineThickness(lineThickness)
{
    m_isApplyColoring = true;
    
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
        const float x = m_radius * std::cos(t);
        const float y = m_radius * std::sin(t);
        
        m_lineLoop.push_back(m_coordinates.size() / 3);
        
        m_coordinates.push_back(x);
        m_coordinates.push_back(y);
        m_coordinates.push_back(z);
        
        m_normals.push_back(0.0);
        m_normals.push_back(0.0);
        m_normals.push_back(1.0);
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
        
        //std::cout << "Sides: ";
        //printTriangleStrip(m_triangleStrip);
        //std::cout << std::endl;
    }
    
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
BrainOpenGLShapeRingOutline::~BrainOpenGLShapeRingOutline()
{
    
}

void
BrainOpenGLShapeRingOutline::setupOpenGLForShape(const BrainOpenGL::DrawMode drawMode)
{
    m_displayList    = 0;
    
    m_coordinatesBufferID = 0;
    m_coordinatesRgbaByteBufferID = 0;
    m_normalBufferID = 0;
    m_lineLoopBufferID = 0;
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
            m_normalBufferID = createBufferID();
            glBindBuffer(GL_ARRAY_BUFFER,
                         m_normalBufferID);
            glBufferData(GL_ARRAY_BUFFER,
                         m_normals.size() * sizeof(GLfloat),
                         &m_normals[0],
                         GL_STATIC_DRAW);
            
            /*
             * Put line loop into its buffer.
             */
            m_lineLoopBufferID = createBufferID();
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,
                         m_lineLoopBufferID);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                         m_lineLoop.size() * sizeof(GLuint),
                         &m_lineLoop[0],
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
BrainOpenGLShapeRingOutline::drawShape(const BrainOpenGL::DrawMode drawMode,
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
BrainOpenGLShapeRingOutline::drawShape(const BrainOpenGL::DrawMode drawMode,
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
            glLineWidth(m_lineThickness);
            if (m_isApplyColoring) {
                glColor4ubv(rgba);
            }
            const int32_t numVertices = static_cast<int32_t>(m_lineLoop.size());
            glBegin(GL_LINE_LOOP);
            for (int32_t j = 0; j < numVertices; j++) {
                const int32_t vertexIndex = m_lineLoop[j] * 3;
                
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
             * Put BYTE colors into its buffer
             */
            const int64_t numRGBA = static_cast<int64_t>(m_rgbaByte.size()) / 4;
            for (int64_t ir = 0; ir < numRGBA; ir++) {
                const int64_t ir4 = ir * 4;
                CaretAssertVectorIndex(m_rgbaByte, ir4+3);
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
            glBindBuffer(GL_ARRAY_BUFFER,
                         m_normalBufferID);
            glNormalPointer(GL_FLOAT,
                            0,
                            (GLvoid*)0);
            
            /*
             * Apply the line width
             */
            glLineWidth(m_lineThickness);
            
            /*
             * Draw the side triangle fans.
             */
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,
                         m_lineLoopBufferID);
            glDrawElements(GL_LINE_LOOP,
                           m_lineLoop.size(),
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
#endif // __BRAIN_OPEN_GL_SHAPE_RING_OUTLINE_DECLARE__
            break;
    }
}

