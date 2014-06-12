
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

#define __BRAIN_OPEN_G_L_SHAPE_SPHERE_DECLARE__
#include "BrainOpenGLShapeSphere.h"
#undef __BRAIN_OPEN_G_L_SHAPE_SPHERE_DECLARE__

#include "AString.h"
#include "CaretAssert.h"
#include "MathFunctions.h"

using namespace caret;


    
/**
 * \class caret::BrainOpenGLShapeSphere 
 * \brief Create a spherical shape for use with OpenGL.
 */

/**
 * Constructor.
 */
BrainOpenGLShapeSphere::BrainOpenGLShapeSphere(const int32_t numberOfLatitudeAndLongitude,
                                               const float radius)
: BrainOpenGLShape(),
  m_numberOfLatitudeAndLongitude(numberOfLatitudeAndLongitude),
  m_radius(radius)
{
    m_isApplyColoring = true;
    
    bool debugFlag = false;
    
    const int numLat = m_numberOfLatitudeAndLongitude;
    const int numLon = m_numberOfLatitudeAndLongitude;
    
    const float degToRad = M_PI / 180.0;
    
    const float dLat = 180.0 / numLat;
    for (int iLat = 0; iLat < numLat; iLat++) {
        const float latDeg = -90.0 + (iLat * dLat);
        const float latRad = latDeg * degToRad;
        
        float z1 = m_radius * std::sin(latRad);
        
        const float latDeg2 = -90.0 + ((iLat + 1) * dLat);
        const float latRad2 = latDeg2 * degToRad;
        
        float z2 = m_radius * std::sin(latRad2);
        
        std::vector<GLuint> strip;
        
        if (debugFlag) std::cout << "Strip Start: " << std::endl;
        const float dLon = 360.0 / numLon;
        for (int iLon = 0; iLon <= numLon; iLon++) {
            const float lonDeg = iLon * dLon;
            const float lonRad = lonDeg * degToRad;
            
            float x1 = m_radius * std::cos(latRad) * std::cos(lonRad);
            float y1 = m_radius * std::cos(latRad) * std::sin(lonRad);
            if (iLat == 0) {
                x1 =  0.0;
                y1 =  0.0;
                z1 = -m_radius;
            }
            
            float x2 = m_radius * std::cos(latRad2) * std::cos(lonRad);
            float y2 = m_radius * std::cos(latRad2) * std::sin(lonRad);
            if (iLat == (numLat - 1)) {
                x2 = 0.0;
                y2 = 0.0;
                z2 = m_radius;
            }
            
            strip.push_back(static_cast<GLuint>(m_coordinates.size() / 3));
            const int32_t indx1 = static_cast<int32_t>(m_coordinates.size() / 3);
            m_coordinates.push_back(x2);
            m_coordinates.push_back(y2);
            m_coordinates.push_back(z2);
            const float length2 = std::sqrt(x2*x2 + y2*y2 + z2*z2);
            m_normals.push_back(x2 / length2);
            m_normals.push_back(y2 / length2);
            m_normals.push_back(z2 / length2);
            
            strip.push_back(static_cast<GLuint>(m_coordinates.size() / 3));
            const int32_t indx2 = static_cast<int32_t>(m_coordinates.size() / 3);
            m_coordinates.push_back(x1);
            m_coordinates.push_back(y1);
            m_coordinates.push_back(z1);
            const float length1 = std::sqrt(x1*x1 + y1*y1 + z1*z1);
            m_normals.push_back(x1 / length1);
            m_normals.push_back(y1 / length1);
            m_normals.push_back(z1 / length1);
            
            if (debugFlag) {
                std::cout << "   " << iLat << ", " << iLon << std::endl;
                std::cout << "       " << indx1 << ":" << latDeg << ", " << lonDeg << ", " << x1 << ", " << y1 << ", " << z1 << std::endl;
                std::cout << "       " << indx2 << ":" << latDeg2 << ", " << lonDeg << ", " << x2 << ", " << y2 << ", " << z2 << std::endl;
            }
        }
        
        if (strip.empty() == false) {
            m_triangleStrips.push_back(strip);
            
            if (debugFlag) {
                const float* c1 = &m_coordinates[strip[0]* 3];
                const float* c2 = &m_coordinates[strip[1]* 3];
                const float* c3 = &m_coordinates[strip[2]* 3];
                float normal[3];
                MathFunctions::normalVector(c1, c2, c3, normal);
                std::cout << "Normal Vector" << qPrintable(AString::fromNumbers(normal, 3, ",")) << std::endl;
            }
        }
        if (debugFlag) std::cout << std::endl;
    }
    
    //    if (debugFlag) {
    //        const int32_t numTriangleStrips = m_triangleStrips.size();
    //        for (int32_t i = 0; i < numTriangleStrips; i++) {
    //            std::cout << "Strip " << i << ":";
    //            printTriangleStrip(m_triangleStrips[i]);
    //        }
    //    }
    
    /*
     * Concatente into a single strip
     */
    contatenateTriangleStrips(m_triangleStrips,
                              m_singleTriangleStrip);
    
    if (debugFlag) {
        std::cout << "NEW STRIPS" << std::endl;
        const int32_t numTriangleStrips = m_triangleStrips.size();
        for (int32_t i = 0; i < numTriangleStrips; i++) {
            std::cout << "Strip " << i << ":";
            printTriangleStrip(m_triangleStrips[i]);
        }
        
        std::cout << "SINGLE STRIP: ";
        printTriangleStrip(m_singleTriangleStrip);
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
BrainOpenGLShapeSphere::~BrainOpenGLShapeSphere()
{
    
}

void
BrainOpenGLShapeSphere::setupOpenGLForShape(const BrainOpenGL::DrawMode drawMode)
{
    m_displayList    = 0;
    m_vertexBufferID = 0;
    
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
                 * Put vertices (coordinates) into its buffer.
                 */
                m_vertexBufferID = createBufferID();
                glBindBuffer(GL_ARRAY_BUFFER,
                             m_vertexBufferID);
                glBufferData(GL_ARRAY_BUFFER,
                             m_coordinates.size() * sizeof(GLfloat),
                             &m_coordinates[0],
                             GL_STATIC_DRAW);
                
            /*
             * For RGBA coloring
             */
            m_vertexRgbaByteBufferID = createBufferID();
            glBindBuffer(GL_ARRAY_BUFFER,
                         m_vertexRgbaByteBufferID);
            glBufferData(GL_ARRAY_BUFFER,
                         m_rgbaByte.size() * sizeof(GLubyte),
                         &m_rgbaByte[0],
                         GL_DYNAMIC_DRAW);
            
                /*
                 * Put normals into its buffer.
                 */
                m_normalBufferID = createBufferID();
                glBindBuffer(GL_ARRAY_BUFFER,
                             m_normalBufferID);
                glBufferData(GL_ARRAY_BUFFER,
                             m_normals.size() * sizeof(GLfloat),
                             &m_normals[0],
                             GL_STATIC_DRAW);
                
                /*
                 * Put triangle strips into its buffer.
                 */
                m_triangleStripBufferID = createBufferID();
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,
                             m_triangleStripBufferID);
                glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                             m_singleTriangleStrip.size() * sizeof(GLuint),
                             &m_singleTriangleStrip[0],
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
BrainOpenGLShapeSphere::drawShape(const BrainOpenGL::DrawMode drawMode,
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
BrainOpenGLShapeSphere::drawShape(const BrainOpenGL::DrawMode drawMode,
                                  const uint8_t rgba[4])
{
    if (m_isApplyColoring) {
        glColor4ubv(rgba);
    }
    
    
    bool useOneTriangleStrip = true;
    
    switch (drawMode) {
        case BrainOpenGL::DRAW_MODE_DISPLAY_LISTS:
        {
            if (m_displayList > 0) {
                glCallList(m_displayList);
            }
        }
            break;
        case BrainOpenGL::DRAW_MODE_IMMEDIATE:
            if (useOneTriangleStrip) {
                const int32_t numVertices = static_cast<int32_t>(m_singleTriangleStrip.size());
                glBegin(GL_TRIANGLE_STRIP);
                for (int32_t j = 0; j < numVertices; j++) {
                    const int32_t vertexIndex = m_singleTriangleStrip[j] * 3;
                    
                    CaretAssertVectorIndex(m_normals, vertexIndex);
                    CaretAssertVectorIndex(m_coordinates, vertexIndex);
                    
                    glNormal3fv(&m_normals[vertexIndex]);
                    glVertex3fv(&m_coordinates[vertexIndex]);
                }
                glEnd();
            }
            else {
                const int32_t numTriangleStrips = m_triangleStrips.size();
                for (int32_t i = 0; i < numTriangleStrips; i++) {
                    const std::vector<GLuint>& strip = m_triangleStrips[i];
                    const int32_t numVertices = static_cast<int32_t>(strip.size());
                    glBegin(GL_TRIANGLE_STRIP);
                    for (int32_t j = 0; j < numVertices; j++) {
                        const int32_t vertexIndex = strip[j] * 3;
                        
                        CaretAssertVectorIndex(m_normals, vertexIndex);
                        CaretAssertVectorIndex(m_coordinates, vertexIndex);
                        
                        glNormal3fv(&m_normals[vertexIndex]);
                        glVertex3fv(&m_coordinates[vertexIndex]);
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
#ifdef BRAIN_OPENGL_INFO_SUPPORTS_VERTEX_BUFFERS
                glEnableClientState(GL_VERTEX_ARRAY);
                glEnableClientState(GL_NORMAL_ARRAY);
            glEnableClientState(GL_COLOR_ARRAY);
            
                /*
                 * Set the vertices for drawing.
                 */
                glBindBuffer(GL_ARRAY_BUFFER,
                             m_vertexBufferID);
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
            CaretAssert(glIsBuffer(m_vertexRgbaByteBufferID) == GL_TRUE);
            glBindBuffer(GL_ARRAY_BUFFER,
                         m_vertexRgbaByteBufferID);
            glBufferData(GL_ARRAY_BUFFER,
                         m_rgbaByte.size() * sizeof(GLubyte),
                         &m_rgbaByte[0],
                         GL_DYNAMIC_DRAW);
            glColorPointer(4,
                           GL_UNSIGNED_BYTE,
                           0,
                           (GLvoid*)0);
            
                /*
                 * Set the normal vectors for drawing.
                 */
                glBindBuffer(GL_ARRAY_BUFFER,
                             m_normalBufferID);
                glNormalPointer(GL_FLOAT,
                                0,
                                (GLvoid*)0);
                
                /*
                 * Draw the triangle strips.
                 */
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,
                             m_triangleStripBufferID);
                glDrawElements(GL_TRIANGLE_STRIP,
                               m_singleTriangleStrip.size(),
                               GL_UNSIGNED_INT,
                               (GLvoid*)0);
                /*
                 * Deselect active buffer.
                 */
                glBindBuffer(GL_ARRAY_BUFFER,
                             0);
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,
                             0);
                
                glDisableClientState(GL_VERTEX_ARRAY);
                glDisableClientState(GL_NORMAL_ARRAY);
            glDisableClientState(GL_COLOR_ARRAY);
#endif // BRAIN_OPENGL_INFO_SUPPORTS_VERTEX_BUFFERS
            break;
    }
}

