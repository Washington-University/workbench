
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

#define __BRAIN_OPEN_G_L_SHAPE_CUBE_DECLARE__
#include "BrainOpenGLShapeCube.h"
#undef __BRAIN_OPEN_G_L_SHAPE_CUBE_DECLARE__

#include "AString.h"
#include "CaretAssert.h"
#include "MathFunctions.h"

using namespace caret;


    
/**
 * \class caret::BrainOpenGLShapeCube 
 * \brief Create a cube shape for use with OpenGL.
 */

/**
 * Constructor.
 *
 * @param cubeSize
 *    Size of cube from one face to the opposite face.
 * @param cubeType
 *    Type of cube.
 */
BrainOpenGLShapeCube::BrainOpenGLShapeCube(const float cubeSize,
                                           const CUBE_TYPE cubeType)
: BrainOpenGLShape(),
  m_cubeSize(cubeSize),
  m_cubeType(cubeType)
{
    m_isApplyColoring = true;

    GLfloat cubeVertices[8][3] = {
        { -0.5, -0.5,  0.5 }, // 0: left, near, top
        {  0.5, -0.5,  0.5 }, // 1: right, near, top
        { -0.5,  0.5,  0.5 }, // 2: left, far, top
        {  0.5,  0.5,  0.5 }, // 3: right, far, top
        
        { -0.5, -0.5, -0.5 }, // 4: left, near, bottom
        {  0.5, -0.5, -0.5 }, // 5: right, near, bottom
        { -0.5,  0.5, -0.5 }, // 6: left, far, bottom
        {  0.5,  0.5, -0.5 }  // 7: right, far, bottom
    };
    
    const GLint lnt = 0;
    const GLint rnt = 1;
    const GLint lft = 2;
    const GLint rft = 3;
    const GLint lnb = 4;
    const GLint rnb = 5;
    const GLint lfb = 6;
    const GLint rfb = 7;
    
    const GLfloat cubeNormals[6][3] = {
        { -1.0,  0.0,  0.0 },
        {  1.0,  0.0,  0.0 },
        {  0.0, -1.0,  0.0 },
        {  0.0,  1.0,  0.0 },
        {  0.0,  0.0,  1.0 },
        {  0.0,  0.0, -1.0 }
    };
    const GLint normalIndexLeftFace   = 0;
    const GLint normalIndexRightFace  = 1;
    const GLint normalIndexNearFace   = 2;
    const GLint normalIndexFarFace    = 3;
    const GLint normalIndexTopFace    = 4;
    const GLint normalIndexBottomFace = 5;
    
    /*
     * Near Face
     */
    addFace(cubeVertices,
            lnt, lnb, rnb, rnt,
            cubeNormals[normalIndexNearFace]);
    
    /*
     * Far Face
     */
    addFace(cubeVertices,
            rfb, lfb, lft, rft,
            cubeNormals[normalIndexFarFace]);

    /*
     * Left Face
     */
    addFace(cubeVertices,
            lfb, lnb, lnt, lft,
            cubeNormals[normalIndexLeftFace]);
    
    /*
     * Right Face
     */
    addFace(cubeVertices,
            rnb, rfb, rft, rnt,
            cubeNormals[normalIndexRightFace]);
    
    /*
     * Bottom Face
     */
    addFace(cubeVertices,
            rnb, lnb, lfb, rfb,
            cubeNormals[normalIndexBottomFace]);
    
    /*
     * Top Face
     */
    addFace(cubeVertices,
            lnt, rnt, rft, lft,
            cubeNormals[normalIndexTopFace]);
    
    
    
    
//    /*
//     * Near Face
//     */
//    addFace(cubeVertices,
//            lnt, lnb, rnb, rnt,
//            cubeNormals[normalIndexNearFace]);
//    
//    /*
//     * Far Face
//     */
//    addFace(cubeVertices,
//            rfb, lfb, lft, rft,
//            cubeNormals[normalIndexFarFace]);
//    
//    /*
//     * Left Face
//     */
//    addFace(cubeVertices,
//            lfb, lnb, lnt, lft,
//            cubeNormals[normalIndexLeftFace]);
//    
//    /*
//     * Right Face
//     */
//    addFace(cubeVertices,
//            rnb, rfb, rft, rnt,
//            cubeNormals[normalIndexRightFace]);
//    
//    /*
//     * Bottom Face
//     */
//    addFace(cubeVertices,
//            rnb, lnb, lfb, rfb,
//            cubeNormals[normalIndexBottomFace]);
//    
//    /*
//     * Top Face
//     */
//    addFace(cubeVertices,
//            lnt, rnt, rft, lft,
//            cubeNormals[normalIndexTopFace]);
    
    CaretAssert(m_coordinates.size() == m_normals.size());
    
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
    
    switch (m_cubeType) {
        case NORMAL:
            break;
        case ROUNDED:
        {
            const unsigned int numPoints = m_coordinates.size() / 3;
            for (unsigned int i = 0; i < numPoints; i++) {
                const unsigned int i3 = i * 3;
                m_normals[i3] = m_coordinates[i3];
                m_normals[i3+1] = m_coordinates[i3+1];
                m_normals[i3+2] = m_coordinates[i3+2];
                
                MathFunctions::normalizeVector(&m_normals[i3]);
            }
        }
            break;
    }    
}

/**
 * Destructor.
 */
BrainOpenGLShapeCube::~BrainOpenGLShapeCube()
{
    
}

/**
 * Setup the shape for drawing.
 * 
 * @param drawMode
 *    The drawing mode.
 */
void
BrainOpenGLShapeCube::setupOpenGLForShape(const BrainOpenGL::DrawMode drawMode)
{
    m_displayList    = 0;
    
    m_vertexBufferID = 0;
    m_coordinatesRgbaByteBufferID = 0;
    m_normalBufferID = 0;
    m_trianglesBufferID = 0;
    
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
            m_coordinatesRgbaByteBufferID = createBufferID();
            glBindBuffer(GL_ARRAY_BUFFER,
                         m_coordinatesRgbaByteBufferID);
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
                m_trianglesBufferID = createBufferID();
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,
                             m_trianglesBufferID);
                glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                             m_triangles.size() * sizeof(GLuint),
                             &m_triangles[0],
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
 * Add a face (two triangles) to vertices and normals for drawing.
 * Vertex indices must be in counter-clockwise order.
 * 
 * @param vertices
 *     Array of vertices.
 * @param v1
 *     First face coordinate index.
 * @param v2
 *     Second face coordinate index.
 * @param v3
 *     Third face coordinate index.
 * @param v4
 *     Fourth face coordinate index.
 * @param normalVector
 *     Normal vector for the face.
 */
void
BrainOpenGLShapeCube::addFace(const GLfloat coordinates[][3],
                              const GLint v1,
                              const GLint v2,
                              const GLint v3,
                              const GLint v4,
                              const GLfloat normalVector[3])
{
    m_coordinates.push_back(coordinates[v1][0]);
    m_coordinates.push_back(coordinates[v1][1]);
    m_coordinates.push_back(coordinates[v1][2]);
    m_normals.push_back(normalVector[0]);
    m_normals.push_back(normalVector[1]);
    m_normals.push_back(normalVector[2]);
    m_triangles.push_back(m_coordinates.size() / 3 - 1);

    m_coordinates.push_back(coordinates[v2][0]);
    m_coordinates.push_back(coordinates[v2][1]);
    m_coordinates.push_back(coordinates[v2][2]);
    m_normals.push_back(normalVector[0]);
    m_normals.push_back(normalVector[1]);
    m_normals.push_back(normalVector[2]);
    m_triangles.push_back(m_coordinates.size() / 3 - 1);
    
    m_coordinates.push_back(coordinates[v3][0]);
    m_coordinates.push_back(coordinates[v3][1]);
    m_coordinates.push_back(coordinates[v3][2]);
    m_normals.push_back(normalVector[0]);
    m_normals.push_back(normalVector[1]);
    m_normals.push_back(normalVector[2]);
    m_triangles.push_back(m_coordinates.size() / 3 - 1);

    
    m_coordinates.push_back(coordinates[v3][0]);
    m_coordinates.push_back(coordinates[v3][1]);
    m_coordinates.push_back(coordinates[v3][2]);
    m_normals.push_back(normalVector[0]);
    m_normals.push_back(normalVector[1]);
    m_normals.push_back(normalVector[2]);
    m_triangles.push_back(m_coordinates.size() / 3 - 1);

    m_coordinates.push_back(coordinates[v4][0]);
    m_coordinates.push_back(coordinates[v4][1]);
    m_coordinates.push_back(coordinates[v4][2]);
    m_normals.push_back(normalVector[0]);
    m_normals.push_back(normalVector[1]);
    m_normals.push_back(normalVector[2]);
    m_triangles.push_back(m_coordinates.size() / 3 - 1);

    m_coordinates.push_back(coordinates[v1][0]);
    m_coordinates.push_back(coordinates[v1][1]);
    m_coordinates.push_back(coordinates[v1][2]);
    m_normals.push_back(normalVector[0]);
    m_normals.push_back(normalVector[1]);
    m_normals.push_back(normalVector[2]);
    m_triangles.push_back(m_coordinates.size() / 3 - 1);
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
BrainOpenGLShapeCube::drawShape(const BrainOpenGL::DrawMode drawMode,
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
BrainOpenGLShapeCube::drawShape(const BrainOpenGL::DrawMode drawMode,
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
            const int32_t numVertices = static_cast<int32_t>(m_coordinates.size() / 3);
            glBegin(GL_TRIANGLES);
            for (int32_t j = 0; j < numVertices; j++) {
                const int32_t vertexIndex = j * 3;
                
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
                 * Set the normal vectors for drawing.
                 */
                glBindBuffer(GL_ARRAY_BUFFER,
                             m_normalBufferID);
                glNormalPointer(GL_FLOAT,
                                0,
                                (GLvoid*)0);
                
                /*
                 * Draw the triangles.
                 */
            CaretAssert(m_triangles.size() == (m_coordinates.size() / 3));
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,
                         m_trianglesBufferID);
            glDrawElements(GL_TRIANGLES,
                           m_triangles.size(),
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

