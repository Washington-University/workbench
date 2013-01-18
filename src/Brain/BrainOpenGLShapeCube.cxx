
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
 */
BrainOpenGLShapeCube::BrainOpenGLShapeCube(const float cubeSize)
: BrainOpenGLShape(),
  m_cubeSize(cubeSize)
{
    m_displayList    = 0;
    m_vertexBufferID = 0;
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
BrainOpenGLShapeCube::setupShape(const BrainOpenGL::DrawMode drawMode)
{
    GLfloat cubeVertices[8][3] = {
        { -1.0, -1.0,  1.0 }, // 0: left, near, top
        {  1.0, -1.0,  1.0 }, // 1: right, near, top
        { -1.0,  1.0,  1.0 }, // 2: left, far, top
        {  1.0,  1.0,  1.0 }, // 3: right, far, top
        
        { -1.0, -1.0, -1.0 }, // 4: left, near, bottom
        {  1.0, -1.0, -1.0 }, // 5: right, near, bottom
        { -1.0,  1.0, -1.0 }, // 6: left, far, bottom
        {  1.0,  1.0, -1.0 }  // 7: right, far, bottom
    };
    
    const GLfloat halfCubeSize = m_cubeSize * 0.5;
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 3; j++) {
            cubeVertices[i][j] *= halfCubeSize;
        }
    }
    
    const GLint lnt = 0;
    const GLint rnt = 1;
    const GLint lft = 2;
    const GLint rft = 3;
    const GLint lnb = 4;
    const GLint rnb = 5;
    const GLint lfb = 6;
    const GLint rfb = 7;
    
//    const GLfloat normalLeftFace[3]   = { -1.0,  0.0,  0.0 };
//    const GLfloat normalRightFace[3]  = {  1.0,  0.0,  0.0 };
//    const GLfloat normalNearFace[3]   = {  0.0, -1.0,  0.0 };
//    const GLfloat normalFarFace[3]    = {  0.0,  1.0,  0.0 };
//    const GLfloat normalTopFace[3]    = {  0.0,  0.0,  1.0 };
//    const GLfloat normalBottomFace[3] = {  0.0,  0.0, -1.0 };
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
    
    CaretAssert(m_coordinates.size() == m_normals.size());
    
    bool smoothCubesFlag = true;
    if (smoothCubesFlag) {
        const unsigned int numPoints = m_coordinates.size() / 3;
        for (unsigned int i = 0; i < numPoints; i++) {
            const unsigned int i3 = i * 3;
            m_normals[i3] = m_coordinates[i3];
            m_normals[i3+1] = m_coordinates[i3+1];
            m_normals[i3+2] = m_coordinates[i3+2];
            
            MathFunctions::normalizeVector(&m_normals[i3]);
        }
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
            }
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

    m_coordinates.push_back(coordinates[v2][0]);
    m_coordinates.push_back(coordinates[v2][1]);
    m_coordinates.push_back(coordinates[v2][2]);
    m_normals.push_back(normalVector[0]);
    m_normals.push_back(normalVector[1]);
    m_normals.push_back(normalVector[2]);
    
    m_coordinates.push_back(coordinates[v3][0]);
    m_coordinates.push_back(coordinates[v3][1]);
    m_coordinates.push_back(coordinates[v3][2]);
    m_normals.push_back(normalVector[0]);
    m_normals.push_back(normalVector[1]);
    m_normals.push_back(normalVector[2]);

    
    m_coordinates.push_back(coordinates[v3][0]);
    m_coordinates.push_back(coordinates[v3][1]);
    m_coordinates.push_back(coordinates[v3][2]);
    m_normals.push_back(normalVector[0]);
    m_normals.push_back(normalVector[1]);
    m_normals.push_back(normalVector[2]);

    m_coordinates.push_back(coordinates[v4][0]);
    m_coordinates.push_back(coordinates[v4][1]);
    m_coordinates.push_back(coordinates[v4][2]);
    m_normals.push_back(normalVector[0]);
    m_normals.push_back(normalVector[1]);
    m_normals.push_back(normalVector[2]);

    m_coordinates.push_back(coordinates[v1][0]);
    m_coordinates.push_back(coordinates[v1][1]);
    m_coordinates.push_back(coordinates[v1][2]);
    m_normals.push_back(normalVector[0]);
    m_normals.push_back(normalVector[1]);
    m_normals.push_back(normalVector[2]);
    
}

/**
 * Draw the sphere.
 * @param drawMode
 *   How to draw the shape.
 */
void
BrainOpenGLShapeCube::drawShape(const BrainOpenGL::DrawMode drawMode)
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
            if (BrainOpenGL::isVertexBuffersSupported()) {
                glEnableClientState(GL_VERTEX_ARRAY);
                glEnableClientState(GL_NORMAL_ARRAY);
                
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
                const GLsizei numberOfTriangles = m_coordinates.size();
                glDrawArrays(GL_TRIANGLES,
                             0,
                             numberOfTriangles);

                /*
                 * Deselect active buffer.
                 */
                glBindBuffer(GL_ARRAY_BUFFER,
                             0);
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,
                             0);
                
                glDisableClientState(GL_VERTEX_ARRAY);
                glDisableClientState(GL_NORMAL_ARRAY);
            }
#endif // BRAIN_OPENGL_INFO_SUPPORTS_VERTEX_BUFFERS
            break;
    }
}

