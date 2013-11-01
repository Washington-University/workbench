
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

#define __BRAIN_OPEN_G_L_PRIMITIVE_DRAWING_DECLARE__
#include "BrainOpenGLPrimitiveDrawing.h"
#undef __BRAIN_OPEN_G_L_PRIMITIVE_DRAWING_DECLARE__

#include "BrainOpenGL.h"
#include "CaretOpenGLInclude.h"

#include "CaretAssert.h"
#include "CaretLogger.h"

#include "AString.h"

using namespace caret;


    
/**
 * \class caret::BrainOpenGLPrimitiveDrawing 
 * \brief Draws OpenGL Primitives (Triangles, Quads, etc.)
 * \ingroup Brain
 *
 * As OpenGL has evolved, functionality for drawing primitives (triangles, quads,
 * etc.) has been added.  Users of this class only need to provide the
 * vertices, colors, normal vectors, and type of primitive for drawing.  This
 * class will use the OpenGL primitive drawing most appropriate for the 
 * OpenGL capabilities available at runtime.
 *
 * All public methods are static methods.
 */

/**
 * Constructor.
 */
BrainOpenGLPrimitiveDrawing::BrainOpenGLPrimitiveDrawing()
{
    
}

/**
 * Destructor.
 */
BrainOpenGLPrimitiveDrawing::~BrainOpenGLPrimitiveDrawing()
{
}

/**
 * Draw quads.
 *
 * @param coordinates
 *    Coordinates of the quads.
 * @param normals
 *    Normal vectors for the quads.
 * @param rgbaColors
 *    RGBA colors for the quads.
 */
void
BrainOpenGLPrimitiveDrawing::drawQuads(const std::vector<float>& coordinates,
                                             const std::vector<float>& normals,
                                             const std::vector<uint8_t>& rgbaColors)
{
    const uint64_t numCoords  = coordinates.size() / 3;
    const uint64_t numNormals = normals.size() / 3;
    const uint64_t numColors  = rgbaColors.size() / 4;
    const uint64_t numQuads = numCoords / 4;  // 4 three-d coords per quad
    
    if (numQuads <= 0) {
        return;
    }
    
    
    if (numNormals != numCoords) {
        const AString message = ("Size of normals must equal size of coordinates. "
                                 "Coordinate size: " + AString::number(coordinates.size())
                                 + "  Normals size: " + AString::number(normals.size()));
        CaretLogSevere(message);
        CaretAssertMessage(0, message);
        return;
    }
    
    if (numColors != numCoords) {
        const AString message = ("Size of RGBA colors must be 4/3 size of coordinates. "
                                 "Coordinate size: " + AString::number(coordinates.size())
                                 + "  RGBA size: " + AString::number(rgbaColors.size()));
        CaretLogSevere(message);
        CaretAssertMessage(0, message);
        return;
    }
    
    bool wasDrawnWithVertexBuffers = false;
#ifdef BRAIN_OPENGL_INFO_SUPPORTS_VERTEX_BUFFERS
    if (BrainOpenGL::getBestDrawingMode() == BrainOpenGL::DRAW_MODE_VERTEX_BUFFERS) {
        drawQuadsVertexBuffers(coordinates,
                               normals,
                               rgbaColors);
        wasDrawnWithVertexBuffers = true;
    }
#endif // BRAIN_OPENGL_INFO_SUPPORTS_VERTEX_BUFFERS
    
    
    if ( ! wasDrawnWithVertexBuffers) {
        
        drawQuadsVertexArrays(coordinates,
                              normals,
                              rgbaColors);
        //    drawQuadsImmediateMode(coordinates,
        //                           normals,
        //                           rgbaColors);
    }
}

/**
 * Draw quads using immediate mode.
 *
 * @param coordinates
 *    Coordinates of the quads.
 * @param normals
 *    Normal vectors for the quads.
 * @param rgbaColors
 *    RGBA colors for the quads.
 */
void
BrainOpenGLPrimitiveDrawing::drawQuadsImmediateMode(const std::vector<float>& coordinates,
                                                          const std::vector<float>& normals,
                                                          const std::vector<uint8_t>& rgbaColors)
{
    const uint64_t numCoords  = coordinates.size() / 3;
    const uint64_t numQuads = numCoords / 4;  // 4 three-d coords per quad
    
    const float* coordPtr = &coordinates[0];
    const float* normalPtr = &normals[0];
    const uint8_t* colorPtr = &rgbaColors[0];
    
    glBegin(GL_QUADS);
    uint64_t iColor = 0;
    uint64_t iNormal = 0;
    for (uint64_t i = 0; i < numQuads; i++) {
        const uint64_t iCoord = i * 12;
        
        glNormal3fv(&normalPtr[iNormal]);
        iNormal += 3;
        glColor4ubv(&colorPtr[iColor]);
        iColor += 4;
        glVertex3fv(&coordPtr[iCoord]);
        
        glNormal3fv(&normalPtr[iNormal]);
        iNormal += 3;
        glColor4ubv(&colorPtr[iColor]);
        iColor += 4;
        glVertex3fv(&coordPtr[iCoord+3]);
        
        glNormal3fv(&normalPtr[iNormal]);
        iNormal += 3;
        glColor4ubv(&colorPtr[iColor]);
        iColor += 4;
        glVertex3fv(&coordPtr[iCoord+6]);
        
        glNormal3fv(&normalPtr[iNormal]);
        iNormal += 3;
        glColor4ubv(&colorPtr[iColor]);
        iColor += 4;
        glVertex3fv(&coordPtr[iCoord+9]);
    }
    glEnd();
    
}

/**
 * Draw quads using vertex arrays.
 *
 * @param coordinates
 *    Coordinates of the quads.
 * @param normals
 *    Normal vectors for the quads.
 * @param rgbaColors
 *    RGBA colors for the quads.
 */
void
BrainOpenGLPrimitiveDrawing::drawQuadsVertexArrays(const std::vector<float>& coordinates,
                                                         const std::vector<float>& normals,
                                                         const std::vector<uint8_t>& rgbaColors)
{
    const uint64_t numCoords  = coordinates.size() / 3;
    
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
    glVertexPointer(3,
                    GL_FLOAT,
                    0,
                    reinterpret_cast<const GLvoid*>(&coordinates[0]));
    glColorPointer(4,
                   GL_UNSIGNED_BYTE,
                   0,
                   reinterpret_cast<const GLvoid*>(&rgbaColors[0]));
    glNormalPointer(GL_FLOAT,
                    0,
                    reinterpret_cast<const GLvoid*>(&normals[0]));
    
    glDrawArrays(GL_QUADS,
                 0,
                 numCoords);
    
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
}

/**
 * Draw quads using vertex buffers.
 *
 * @param coordinates
 *    Coordinates of the quads.
 * @param normals
 *    Normal vectors for the quads.
 * @param rgbaColors
 *    RGBA colors for the quads.
 */
void
BrainOpenGLPrimitiveDrawing::drawQuadsVertexBuffers(const std::vector<float>& coordinates,
                                                          const std::vector<float>& normals,
                                                          const std::vector<uint8_t>& rgbaColors)
{
    const uint64_t numCoords  = coordinates.size() / 3;
    
#ifdef BRAIN_OPENGL_INFO_SUPPORTS_VERTEX_BUFFERS
        /*
         * Put vertices (coordinates) into its buffer.
         */
        GLuint vertexBufferID = -1;
        glGenBuffers(1, &vertexBufferID);
        glBindBuffer(GL_ARRAY_BUFFER,
                     vertexBufferID);
        glBufferData(GL_ARRAY_BUFFER,
                     coordinates.size() * sizeof(GLfloat),
                     &coordinates[0],
                     GL_STREAM_DRAW);
        
        /*
         * Put normals into its buffer.
         */
        GLuint normalBufferID = -1;
        glGenBuffers(1, &normalBufferID);
        glBindBuffer(GL_ARRAY_BUFFER,
                     normalBufferID);
        glBufferData(GL_ARRAY_BUFFER,
                     normals.size() * sizeof(GLfloat),
                     &normals[0],
                     GL_STREAM_DRAW);
        
        /*
         * Put colors into its buffer.
         */
        GLuint colorBufferID = -1;
        glGenBuffers(1, &colorBufferID);
        glBindBuffer(GL_ARRAY_BUFFER,
                     colorBufferID);
        glBufferData(GL_ARRAY_BUFFER,
                     rgbaColors.size() * sizeof(uint8_t),
                     &rgbaColors[0],
                     GL_STREAM_DRAW);
        
        //            /*
        //             * Put triangle strips into its buffer.
        //             */
        //        GLuint quadsBufferID = -1;
        //        glGenBuffers(1, &quadsBufferID);
        //            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,
        //                         quadsBufferID);
        //            glBufferData(GL_ELEMENT_ARRAY_BUFFER,
        //                         quads.size() * sizeof(GLuint),
        //                         &quads[0],
        //                         GL_STREAM_DRAW);
        
        /*
         * Deselect active buffer.
         */
        glBindBuffer(GL_ARRAY_BUFFER,
                     0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,
                     0);
        
        
        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_COLOR_ARRAY);
        glEnableClientState(GL_NORMAL_ARRAY);
        
        /*
         * Set the vertices for drawing.
         */
        glBindBuffer(GL_ARRAY_BUFFER,
                     vertexBufferID);
        glVertexPointer(3,
                        GL_FLOAT,
                        0,
                        (GLvoid*)0);
        
        /*
         * Set the normal vectors for drawing.
         */
        glBindBuffer(GL_ARRAY_BUFFER,
                     normalBufferID);
        glNormalPointer(GL_FLOAT,
                        0,
                        (GLvoid*)0);
        
        /*
         * Set the rgba colors for drawing
         */
        glBindBuffer(GL_ARRAY_BUFFER,
                     colorBufferID);
        glColorPointer(4,
                       GL_UNSIGNED_BYTE,
                       0,
                       (GLvoid*)0);
        
        /*
         * Draw the triangle strips.
         */
        glDrawArrays(GL_QUADS,
                     0,
                     numCoords);
        //        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,
        //                     quadsBufferID);
        //        glDrawElements(GL_TRIANGLE_STRIP,
        //                       quads.size(),
        //                       GL_UNSIGNED_INT,
        //                       (GLvoid*)0);
        /*
         * Deselect active buffer.
         */
        glBindBuffer(GL_ARRAY_BUFFER,
                     0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,
                     0);
        
        glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);
        glDisableClientState(GL_NORMAL_ARRAY);
        
        glDeleteBuffers(1, &vertexBufferID);
        glDeleteBuffers(1, &normalBufferID);
        glDeleteBuffers(1, &colorBufferID);
#endif // BRAIN_OPENGL_INFO_SUPPORTS_VERTEX_BUFFERS
}

