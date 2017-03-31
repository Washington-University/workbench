
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

#define __BRAIN_OPEN_G_L_PRIMITIVE_DRAWING_DECLARE__
#include "BrainOpenGLPrimitiveDrawing.h"
#undef __BRAIN_OPEN_G_L_PRIMITIVE_DRAWING_DECLARE__

#include "AString.h"
#include "BrainOpenGL.h"
#include "CaretOpenGLInclude.h"
#include "CaretAssert.h"
#include "CaretLogger.h"
#include "MathFunctions.h"


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
                                       const std::vector<float>& rgbaColors)
{
    std::vector<uint8_t> rgbaBytes;
    for (std::vector<float>::const_iterator iter = rgbaColors.begin();
         iter != rgbaColors.end();
         iter++) {
        rgbaBytes.push_back(static_cast<uint8_t>(*iter * 255.0));
    }
    
    drawQuads(coordinates,
              normals,
              rgbaBytes);
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
    
    /*
     * Use the number of voxels in a 300 x 300 slice as
     * as the most maximum number of voxels to draw in
     * in one call to OpenGL drawing.
     */
    const int64_t maximumCoordinatesToDraw = 300 * 300 * 4;
    CaretAssert(maximumCoordinatesToDraw == ((maximumCoordinatesToDraw / 4) * 4));
        //std::cout << "Max vertices to draw: " << maximumCoordinatesToDraw << std::endl;
        int64_t coordinateOffset = 0;
        bool done = false;
        while ( ! done) {
            const int64_t coordinateToDrawCount = std::min(int64_t(numCoords - coordinateOffset),
                                           maximumCoordinatesToDraw);
            if (coordinateToDrawCount > 0) {
                //std::cout << "Drawing offset " << coordinateOffset << " count " << coordinateToDrawCount << std::endl;
                bool wasDrawnWithVertexBuffers = false;
#ifdef BRAIN_OPENGL_INFO_SUPPORTS_VERTEX_BUFFERS
                if (BrainOpenGL::getBestDrawingMode() == BrainOpenGL::DRAW_MODE_VERTEX_BUFFERS) {
                    CaretAssertMessage(0, "See John Harwell");
                    drawQuadsVertexBuffers(coordinates,
                                           normals,
                                           rgbaColors,
                                           coordinateOffset,
                                           coordinateToDrawCount);
                    
                    wasDrawnWithVertexBuffers = true;
                }
#endif // BRAIN_OPENGL_INFO_SUPPORTS_VERTEX_BUFFERS
                
                
                if ( ! wasDrawnWithVertexBuffers) {
                    drawQuadsVertexArrays(coordinates,
                                          normals,
                                          rgbaColors,
                                          coordinateOffset,
                                          coordinateToDrawCount);
                    
//                    drawQuadsImmediateMode(coordinates,
//                                           normals,
//                                           rgbaColors,
//                                           coordinateOffset,
//                                           coordinateToDrawCount);
                }
                coordinateOffset += maximumCoordinatesToDraw;
            }
            else {
                done = true;
            }
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
                                                    const std::vector<uint8_t>& rgbaColors,
                                                    const int64_t coordinateOffset,
                                                    const int64_t coordinateCount)
{
//    const uint64_t numCoords  = coordinates.size() / 3;
//    const uint64_t numQuads = numCoords / 4;  // 4 three-d coords per quad
    
    const float* coordPtr = &coordinates[0];
    const float* normalPtr = &normals[0];
    const uint8_t* colorPtr = &rgbaColors[0];
    
    CaretAssertVectorIndex(coordinates, ((coordinateOffset * 3)
                                         + (coordinateCount * 3) - 1));
    CaretAssertVectorIndex(normals, ((coordinateOffset * 3)
                                     + (coordinateCount * 3) - 1));
    CaretAssertVectorIndex(rgbaColors, ((coordinateOffset * 4)
                                        + (coordinateCount * 4) - 1));
    
    glBegin(GL_QUADS);
    uint64_t iColor  = coordinateOffset * 4;
    uint64_t iNormal = coordinateOffset * 3;
    uint64_t iCoord  = coordinateOffset * 3;
    for (int64_t i = 0; i < coordinateCount; i++) {
        glNormal3fv(&normalPtr[iNormal]);
        iNormal += 3;
        glColor4ubv(&colorPtr[iColor]);
        iColor += 4;
        glVertex3fv(&coordPtr[iCoord]);
        iCoord += 3;
    }
    
    glEnd();
//    for (uint64_t i = 0; i < numQuads; i++) {
//        glNormal3fv(&normalPtr[iNormal]);
//        iNormal += 3;
//        glColor4ubv(&colorPtr[iColor]);
//        iColor += 4;
//        glVertex3fv(&coordPtr[iCoord]);
//        iCoord += 3;
//        
//        glNormal3fv(&normalPtr[iNormal]);
//        iNormal += 3;
//        glColor4ubv(&colorPtr[iColor]);
//        iColor += 4;
//        glVertex3fv(&coordPtr[iCoord]);
//        iCoord += 3;
//        
//        glNormal3fv(&normalPtr[iNormal]);
//        iNormal += 3;
//        glColor4ubv(&colorPtr[iColor]);
//        iColor += 4;
//        glVertex3fv(&coordPtr[iCoord]);
//        iCoord += 3;
//        
//        glNormal3fv(&normalPtr[iNormal]);
//        iNormal += 3;
//        glColor4ubv(&colorPtr[iColor]);
//        iColor += 4;
//        glVertex3fv(&coordPtr[iCoord]);
//        iCoord += 3;
//    }
//    glEnd();
    
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
                                                   const std::vector<uint8_t>& rgbaColors,
                                                   const int64_t coordinateOffset,
                                                   const int64_t coordinateCount)
{
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
    
//    const int64_t maxVertices = 1000;
//    const int64_t numQuads = maxVertices / 4;
//    CaretAssert(maxVertices == (numQuads * 4));
//    if (numCoords > maxVertices) {
//        std::cout << "Max vertices: " << maxVertices << std::endl;
//        int64_t offset = 0;
//        bool done = false;
//        while ( ! done) {
//            const int64_t count = std::min(int64_t(numCoords - offset),
//                                           maxVertices);
//            if (count > 0) {
//                std::cout << "Drawing offset " << offset << " count " << count << std::endl;
//                glDrawArrays(GL_QUADS,
//                             offset,
//                             count);
//                offset += maxVertices;
//            }
//            else {
//                done = true;
//            }
//        }
//    }
//    else {
//        glDrawArrays(GL_QUADS,
//                     0,
//                     numCoords);
//    }
    
    glDrawArrays(GL_QUADS,
                 coordinateOffset,
                 coordinateCount);
    
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
                                                    const std::vector<uint8_t>& rgbaColors,
                                                    const int64_t coordinateOffset,
                                                    const int64_t coordinateCount)
{
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
                     coordinateOffset,
                     coordinateCount);
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

/**
 * Draw quads strips
 *
 * @param coordinates
 *    Coordinates of the quads.
 * @param normals
 *    Normal vectors for the quads.
 * @param rgbaColors
 *    RGBA colors for the quads.
 * @param quadStripIndices
 *    Indices into the quad strip coordinates.
 */
void
BrainOpenGLPrimitiveDrawing::drawQuadStrips(const std::vector<float>& coordinates,
                           const std::vector<float>& normals,
                                            const std::vector<uint8_t>& rgbaColors,
                                            const std::vector<uint32_t>& quadStripIndices)
{
    drawQuadStripsVertexArrays(coordinates,
                               normals,
                               rgbaColors,
                               quadStripIndices);
}

/**
 * Draw quads strips
 *
 * @param coordinates
 *    Coordinates of the quads.
 * @param normals
 *    Normal vectors for the quads.
 * @param rgbaColors
 *    RGBA colors for the quads.
 * @param quadStripIndices
 *    Indices into the quad strip coordinates.
 */
void
BrainOpenGLPrimitiveDrawing::drawQuadStripsVertexArrays(const std::vector<float>& coordinates,
                                       const std::vector<float>& normals,
                                                        const std::vector<uint8_t>& rgbaColors,
                                                        const std::vector<uint32_t>& quadStripIndices)
{
    if (quadStripIndices.empty()) {
        return;
    }
    
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
    
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
    
    
    glDrawElements(GL_QUAD_STRIP,
                   quadStripIndices.size(),
                   GL_UNSIGNED_INT,
                   &quadStripIndices[0]);
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
}


/**
 * Draw quad with indices
 *
 * @param coordinates
 *    Coordinates of the quads.
 * @param normals
 *    Normal vectors for the quads.
 * @param rgbaColors
 *    RGBA colors for the quads.
 * @param quadIndices
 *    Indices of the quads (4 per quad).
 */
void
BrainOpenGLPrimitiveDrawing::drawQuadIndices(const std::vector<float>& coordinates,
                                             const std::vector<float>& normals,
                                             const std::vector<uint8_t>& rgbaColors,
                                             const std::vector<uint32_t>& quadIndices)
{
    drawQuadIndicesVertexArrays(coordinates,
                                normals,
                                rgbaColors,
                                quadIndices);
}

/**
 * Draw quad with indices
 *
 * @param coordinates
 *    Coordinates of the quads.
 * @param normals
 *    Normal vectors for the quads.
 * @param rgbaColors
 *    RGBA colors for the quads.
 * @param quadIndices
 *    Indices of the quads (4 per quad).
 */
void
BrainOpenGLPrimitiveDrawing::drawQuadIndicesVertexArrays(const std::vector<float>& coordinates,
                                                         const std::vector<float>& normals,
                                                         const std::vector<uint8_t>& rgbaColors,
                                                         const std::vector<uint32_t>& quadIndices)
{
    const int64_t numQuadIndices = static_cast<int64_t>(quadIndices.size());
    if (numQuadIndices <= 0) {
        return;
    }
    
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
    
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
    

    glDrawElements(GL_QUADS,
                   quadIndices.size(),
                   GL_UNSIGNED_INT,
                   &quadIndices[0]);
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
}

/**
 * Draw a line between each pair of three-dimensional coordinates in the given color
 * with optional normal vectors.
 *
 * @param coordinates
 *     Three-Dimensional coordinates (XYZ).
 * @param rgba
 *     One RGBA colors (4 elements) ranging [0.0, 1.0] that is used for drawing all of the lines.
 * @param lineWidth
 *     Line width used by line drawing modes.
 */
void
BrainOpenGLPrimitiveDrawing::drawLineLoop(const std::vector<float>& coordinates,
                                       const float rgba[4],
                                       const float lineWidth)
{
    const uint8_t rgbaByte[4] = {
        static_cast<uint8_t>(rgba[0] * 255.0),
        static_cast<uint8_t>(rgba[1] * 255.0),
        static_cast<uint8_t>(rgba[2] * 255.0),
        static_cast<uint8_t>(rgba[3] * 255.0)
    };
    
    std::vector<float> emptyNormals;
    drawPrimitiveWithVertexArrays(GL_LINE_LOOP,
                                  coordinates,
                                  emptyNormals,
                                  rgbaByte,
                                  lineWidth);
}

/**
 * Draw a line between each pair of three-dimensional coordinates in the given color
 * with optional normal vectors.
 *
 * @param coordinates
 *     Three-Dimensional coordinates (XYZ).
 * @param rgba
 *     One RGBA colors (4 elements) ranging [0, 255] that is used for drawing all of the lines.
 * @param lineWidth
 *     Line width used by line drawing modes.
 */
void
BrainOpenGLPrimitiveDrawing::drawLineLoop(const std::vector<float>& coordinates,
                                       const uint8_t rgba[4],
                                       const float lineWidth)
{
    std::vector<float> emptyNormals;
    drawPrimitiveWithVertexArrays(GL_LINE_LOOP,
                                  coordinates,
                                  emptyNormals,
                                  rgba,
                                  lineWidth);
}

/**
 * Draw a line between each pair of three-dimensional coordinates in the given color
 * with optional normal vectors.
 *
 * @param coordinates
 *     Three-Dimensional coordinates (XYZ).
 * @param rgba
 *     One RGBA colors (4 elements) ranging [0.0, 1.0] that is used for drawing all of the lines.
 * @param lineWidth
 *     Line width used by line drawing modes.
 */
void
BrainOpenGLPrimitiveDrawing::drawLines(const std::vector<float>& coordinates,
                                       const float rgba[4],
                                       const float lineWidth)
{
    const uint8_t rgbaByte[4] = {
        static_cast<uint8_t>(rgba[0] * 255.0),
        static_cast<uint8_t>(rgba[1] * 255.0),
        static_cast<uint8_t>(rgba[2] * 255.0),
        static_cast<uint8_t>(rgba[3] * 255.0)
    };
    
    std::vector<float> emptyNormals;
    drawPrimitiveWithVertexArrays(GL_LINES,
                                  coordinates,
                                  emptyNormals,
                                  rgbaByte,
                                  lineWidth);
}

/**
 * Draw a line between each pair of three-dimensional coordinates in the given color
 * with optional normal vectors.
 *
 * @param coordinates
 *     Three-Dimensional coordinates (XYZ).
 * @param rgba
 *     One RGBA colors (4 elements) ranging [0, 255] that is used for drawing all of the lines.
 * @param lineWidth
 *     Line width used by line drawing modes.
 */
void
BrainOpenGLPrimitiveDrawing::drawLines(const std::vector<float>& coordinates,
                                       const uint8_t rgba[4],
                                       const float lineWidth)
{
    std::vector<float> emptyNormals;
    drawPrimitiveWithVertexArrays(GL_LINES,
                                  coordinates,
                                  emptyNormals,
                                  rgba,
                                  lineWidth);
}

/**
 * Draw a line between each pair of three-dimensional coordinates in the given color
 * with optional normal vectors.
 *
 * @param coordinates
 *     Three-Dimensional coordinates (XYZ).
 * @param rgba
 *     RGBA Colors [0.0, 1.0] for each vertex.
 * @param lineWidth
 *     Line width used by line drawing modes.
 */
void
BrainOpenGLPrimitiveDrawing::drawLines(const std::vector<float>& coordinates,
                                       const std::vector<float>& rgbaColors,
                                       const float lineWidth)
{
    std::vector<float> emptyNormals;
    
    std::vector<uint8_t> rgbaBytes;
    rgbaBytes.reserve(rgbaColors.size());
    for (const float component : rgbaColors) {
        rgbaBytes.push_back(static_cast<uint8_t>(component * 255.0));
    }
    
    drawPrimitiveWithVertexArrays(GL_LINES,
                                  coordinates,
                                  emptyNormals,
                                  rgbaBytes,
                                  lineWidth);
}

/**
 * Draw a line between each pair of three-dimensional coordinates in the given color
 * with optional normal vectors.
 *
 * @param coordinates
 *     Three-Dimensional coordinates (XYZ).
 * @param rgba
 *     RGBA Colors [0, 255] for each vertex.
 * @param lineWidth
 *     Line width used by line drawing modes.
 */
void
BrainOpenGLPrimitiveDrawing::drawLines(const std::vector<float>& coordinates,
                                       const std::vector<uint8_t>& rgbaColors,
                                       const float lineWidth)
{
    std::vector<float> emptyNormals;
    drawPrimitiveWithVertexArrays(GL_LINES,
                                  coordinates,
                                  emptyNormals,
                                  rgbaColors,
                                  lineWidth);
}

/**
 * Draw a line between each pair of three-dimensional coordinates in the given color
 * with optional normal vectors.
 *
 * @param coordinates
 *     Three-Dimensional coordinates (XYZ).
 * @param rgba
 *     RGBA Colors [0.0, 1.0] for each vertex.
 * @param lineWidth
 *     Line width used by line drawing modes.
 */
void
BrainOpenGLPrimitiveDrawing::drawLineStrip(const std::vector<float>& coordinates,
                                       const std::vector<float>& rgbaColors,
                                       const float lineWidth)
{
    std::vector<float> emptyNormals;
    
    std::vector<uint8_t> rgbaBytes;
    rgbaBytes.reserve(rgbaColors.size());
    for (const float component : rgbaColors) {
        rgbaBytes.push_back(static_cast<uint8_t>(component * 255.0));
    }
    
    drawPrimitiveWithVertexArrays(GL_LINE_STRIP,
                                  coordinates,
                                  emptyNormals,
                                  rgbaBytes,
                                  lineWidth);
}

/**
 * Draw a line between each pair of three-dimensional coordinates in the given color
 * with optional normal vectors.
 *
 * @param coordinates
 *     Three-Dimensional coordinates (XYZ).
 * @param rgba
 *     RGBA Colors [0, 255] for each vertex.
 * @param lineWidth
 *     Line width used by line drawing modes.
 */
void
BrainOpenGLPrimitiveDrawing::drawLineStrip(const std::vector<float>& coordinates,
                                       const std::vector<uint8_t>& rgbaColors,
                                       const float lineWidth)
{
    std::vector<float> emptyNormals;
    drawPrimitiveWithVertexArrays(GL_LINE_STRIP,
                                  coordinates,
                                  emptyNormals,
                                  rgbaColors,
                                  lineWidth);
}

/**
 * Draw a line strip with the given coordinates in the given color
 * with optional normal vectors.
 *
 * @param coordinates
 *     Three-Dimensional coordinates (XYZ).
 * @param rgba
 *     One RGBA colors (4 elements) ranging [0.0, 1.0] that is used for drawing all of the lines.
 * @param lineWidth
 *     Line width used by line drawing modes.
 */
void
BrainOpenGLPrimitiveDrawing::drawLineStrip(const std::vector<float>& coordinates,
                                       const float rgba[4],
                                       const float lineWidth)
{
    const uint8_t rgbaByte[4] = {
        static_cast<uint8_t>(rgba[0] * 255.0),
        static_cast<uint8_t>(rgba[1] * 255.0),
        static_cast<uint8_t>(rgba[2] * 255.0),
        static_cast<uint8_t>(rgba[3] * 255.0)
    };
    
    std::vector<float> emptyNormals;
    drawPrimitiveWithVertexArrays(GL_LINE_STRIP,
                                  coordinates,
                                  emptyNormals,
                                  rgbaByte,
                                  lineWidth);
}

/**
 * Draw a line strip with the given coordinates in the given color
 * with optional normal vectors.
 *
 * @param coordinates
 *     Three-Dimensional coordinates (XYZ).
 * @param rgba
 *     One RGBA colors (4 elements) ranging [0, 255] that is used for drawing all of the lines.
 * @param lineWidth
 *     Line width used by line drawing modes.
 */
void
BrainOpenGLPrimitiveDrawing::drawLineStrip(const std::vector<float>& coordinates,
                                       const uint8_t rgba[4],
                                       const float lineWidth)
{
    std::vector<float> emptyNormals;
    drawPrimitiveWithVertexArrays(GL_LINE_STRIP,
                                  coordinates,
                                  emptyNormals,
                                  rgba,
                                  lineWidth);
}

/**
 * Draw a polygon with optional normal vectors.
 *
 * @param coordinates
 *     Three-Dimensional coordinates (XYZ).
 * @param normals
 *     Normal vectors.  There number of elements must match the numer of
 *     coordinates (in which case normal vectors are applied) or empty
 *     (in which case normal vectors are not applied).
 * @param rgba
 *     One RGBA colors (4 elements) ranging [0.0, 1.0] that is used for drawing all of the lines.
 */
void
BrainOpenGLPrimitiveDrawing::drawPolygon(const std::vector<float>& coordinates,
                                         const std::vector<float>& normals,
                                         const float rgba[4])
{
    const uint8_t rgbaByte[4] = {
        static_cast<uint8_t>(rgba[0] * 255.0),
        static_cast<uint8_t>(rgba[1] * 255.0),
        static_cast<uint8_t>(rgba[2] * 255.0),
        static_cast<uint8_t>(rgba[3] * 255.0)
    };
    
    const float lineWidth = 1.0;
    drawPrimitiveWithVertexArrays(GL_POLYGON,
                                  coordinates,
                                  normals,
                                  rgbaByte,
                                  lineWidth);
}

/**
 * Draw a polygon with optional normal vectors.
 *
 * @param coordinates
 *     Three-Dimensional coordinates (XYZ).
 * @param normals
 *     Normal vectors.  There number of elements must match the numer of
 *     coordinates (in which case normal vectors are applied) or empty
 *     (in which case normal vectors are not applied).
 * @param rgba
 *     One RGBA colors (4 elements) ranging [0.0, 1.0] that is used for drawing all of the lines.
 */
void
BrainOpenGLPrimitiveDrawing::drawPolygon(const std::vector<float>& coordinates,
                                         const std::vector<float>& normals,
                                         const uint8_t rgba[4])
{
    const float lineWidth = 1.0;
    drawPrimitiveWithVertexArrays(GL_POLYGON,
                                  coordinates,
                                  normals,
                                  rgba,
                                  lineWidth);
}

/**
 * Draw a line between each pair of three-dimensional coordinates in the given color
 * with optional normal vectors using the given OpenGL mode.
 *
 * @param mode
 *     The OpenGL drawing mode.
 * @param coordinates
 *     Three-Dimensional coordinates (XYZ).
 * @param normals
 *     Normal vectors.  There number of elements must match the numer of
 *     coordinates (in which case normal vectors are applied) or empty
 *     (in which case normal vectors are not applied).
 * @param rgba
 *     One RGBA color (size=4) that is used for drawing all of the lines.
 * @param lineWidth
 *     Line width used by line drawing modes.
 */
void
BrainOpenGLPrimitiveDrawing::drawPrimitiveWithVertexArrays(GLenum mode,
                                                           const std::vector<float>& coordinates,
                                                           const std::vector<float>& normals,
                                                           const uint8_t rgba[4],
                                                           const float lineWidth)
{
    const int64_t numCoords = static_cast<int32_t>(coordinates.size() / 3);
    if (numCoords < 1) {
        CaretLogWarning("Invalid number of coordinates=" + QString::number(numCoords));
        return;
    }
    
    std::vector<GLubyte> rgbaVector;
    rgbaVector.reserve(numCoords * 4);
    for (int32_t i = 0; i < numCoords; i++) {
        rgbaVector.push_back(rgba[0]);
        rgbaVector.push_back(rgba[1]);
        rgbaVector.push_back(rgba[2]);
        rgbaVector.push_back(rgba[3]);
    }
    
    CaretAssert(numCoords == (rgbaVector.size() / 4));
    
    const int64_t numNormals = static_cast<int64_t>(normals.size() / 3);
    if (numNormals > 0) {
        if (numCoords != numNormals) {
            const AString msg("Number of coordinates "
                              + QString::number(numCoords)
                              + " does not equal number of normals="
                              + QString::number(numNormals));
            CaretAssertMessage(0, msg);
            CaretLogWarning(msg);
            return;
        }
    }
    

    if ((mode == GL_LINES)
        || (mode == GL_LINE_LOOP)
        || (mode == GL_LINE_STRIP)) {
        glLineWidth(lineWidth);
    }
    
    glVertexPointer(3,
                    GL_FLOAT,
                    0,
                    reinterpret_cast<const GLvoid*>(&coordinates[0]));
    glColorPointer(4,
                   GL_UNSIGNED_BYTE,
                   0,
                   reinterpret_cast<const GLvoid*>(&rgbaVector[0]));
    
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    if (numNormals > 0) {
        glNormalPointer(GL_FLOAT,
                        0,
                        reinterpret_cast<const GLvoid*>(&normals[0]));
        glEnableClientState(GL_NORMAL_ARRAY);
    }
    
    glDrawArrays(mode, 0, numCoords);
    
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);
    if (numNormals > 0) {
        glDisableClientState(GL_NORMAL_ARRAY);
    }
}

/**
 * Draw a line between each pair of three-dimensional coordinates in the given colors
 * per vertex with optional normal vectors using the given OpenGL mode.
 *
 * @param mode
 *     The OpenGL drawing mode.
 * @param coordinates
 *     Three-Dimensional coordinates (XYZ).
 * @param normals
 *     Normal vectors.  There number of elements must match the numer of
 *     coordinates (in which case normal vectors are applied) or empty
 *     (in which case normal vectors are not applied).
 * @param rgba
 *     4 components per vertex..
 * @param lineWidth
 *     Line width used by line drawing modes.
 */
void
BrainOpenGLPrimitiveDrawing::drawPrimitiveWithVertexArrays(GLenum mode,
                                                           const std::vector<float>& coordinates,
                                                           const std::vector<float>& normals,
                                                           const std::vector<uint8_t>& rgbaColors,
                                                           const float lineWidth)
{
    const int64_t numCoords = static_cast<int32_t>(coordinates.size() / 3);
    if (numCoords < 1) {
        CaretLogWarning("Invalid number of coordinates=" + QString::number(numCoords));
        return;
    }
    
//    std::vector<GLubyte> rgbaVector;
//    rgbaVector.reserve(numCoords * 4);
//    for (int32_t i = 0; i < numCoords; i++) {
//        rgbaVector.push_back(rgba[0]);
//        rgbaVector.push_back(rgba[1]);
//        rgbaVector.push_back(rgba[2]);
//        rgbaVector.push_back(rgba[3]);
//    }
    
    CaretAssert(numCoords == (rgbaColors.size() / 4));
    
    const int64_t numNormals = static_cast<int64_t>(normals.size() / 3);
    if (numNormals > 0) {
        if (numCoords != numNormals) {
            const AString msg("Number of coordinates "
                              + QString::number(numCoords)
                              + " does not equal number of normals="
                              + QString::number(numNormals));
            CaretAssertMessage(0, msg);
            CaretLogWarning(msg);
            return;
        }
    }
    
    
    if ((mode == GL_LINES)
        || (mode == GL_LINE_LOOP)
        || (mode == GL_LINE_STRIP)) {
        glLineWidth(lineWidth);
    }
    
    glVertexPointer(3,
                    GL_FLOAT,
                    0,
                    reinterpret_cast<const GLvoid*>(&coordinates[0]));
    glColorPointer(4,
                   GL_UNSIGNED_BYTE,
                   0,
                   reinterpret_cast<const GLvoid*>(&rgbaColors[0]));
    
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    if (numNormals > 0) {
        glNormalPointer(GL_FLOAT,
                        0,
                        reinterpret_cast<const GLvoid*>(&normals[0]));
        glEnableClientState(GL_NORMAL_ARRAY);
    }
    
    glDrawArrays(mode, 0, numCoords);
    
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);
    if (numNormals > 0) {
        glDisableClientState(GL_NORMAL_ARRAY);
    }
}

static void
createRectangle(const float center[3],
                const float leftToRightUnitVector[3],
                const float bottomToTopUnitVector[3],
                const float width,
                const float height,
                float bottomLeftOut[3],
                float bottomRightOut[3],
                float topRightOut[3],
                float topLeftOut[3]) {
    
    const float halfWidth = width / 2.0;
    const float halfHeight = height / 2.0;

    for (int32_t i = 0; i < 3; i++) {
        bottomLeftOut[i] = (center[i]
                            - (bottomToTopUnitVector[i] * halfHeight)
                            - (leftToRightUnitVector[i] * halfWidth));
        bottomRightOut[i] = (center[i]
                            - (bottomToTopUnitVector[i] * halfHeight)
                            + (leftToRightUnitVector[i] * halfWidth));
        topRightOut[i] = (center[i]
                            + (bottomToTopUnitVector[i] * halfHeight)
                            + (leftToRightUnitVector[i] * halfWidth));
        topLeftOut[i] = (center[i]
                            + (bottomToTopUnitVector[i] * halfHeight)
                            - (leftToRightUnitVector[i] * halfWidth));
    }
}

/**
 * Draw a outlined rectangle.
 *
 * @param bottomLeft
 *     Bottom left corner.
 * @param bottomRight
 *     Bottom right corner.
 * @param topRight
 *     Top right corner.
 * @param topLeft
 *     Top left corner.
 * @param lineThickness
 *     Thickness of outline.
 * @param rgba
 *     RGBA colors (4 elements) ranging [0.0, 1.0] that is used for drawing all of the lines.
 */
void
BrainOpenGLPrimitiveDrawing::drawRectangleOutline(const float bottomLeft[3],
                                                  const float bottomRight[3],
                                                  const float topRight[3],
                                                  const float topLeft[3],
                                                  const float lineThickness,
                                                  const float rgba[4])
{
    float leftToRightUnitVector[3];
    MathFunctions::createUnitVector(bottomLeft, bottomRight, leftToRightUnitVector);
    const float width = MathFunctions::distance3D(bottomLeft, bottomRight);
    
    float bottomToTopUnitVector[3];
    MathFunctions::createUnitVector(bottomLeft, topLeft, bottomToTopUnitVector);
    const float height = MathFunctions::distance3D(bottomLeft, topLeft);
    
    const float center[3] = {
        (bottomLeft[0] + bottomRight[0] + topRight[0] + topLeft[0]) / 4.0,
        (bottomLeft[1] + bottomRight[1] + topRight[1] + topLeft[1]) / 4.0,
        (bottomLeft[2] + bottomRight[2] + topRight[2] + topLeft[2]) / 4.0
    };
    
    float normalVector[3];
    MathFunctions::normalVector(topLeft, bottomLeft, bottomRight, normalVector);
    
    const float halfThickness = std::min(lineThickness / 2.0f,
                                         std::min(width / 2.0f,
                                                  height / 2.0f));
    
    std::vector<float> coords;
    coords.reserve(8*3);
    
    {
        float bli[3];
        float bri[3];
        float tri[3];
        float tli[3];
        createRectangle(center,
                        leftToRightUnitVector, bottomToTopUnitVector,
                        width - halfThickness*2, height - halfThickness*2,
                        bli, bri, tri, tli);
        
        coords.insert(coords.end(),
                               bli, bli + 3);
        coords.insert(coords.end(),
                               bri, bri + 3);
        coords.insert(coords.end(),
                               tri, tri + 3);
        coords.insert(coords.end(),
                               tli, tli + 3);
        
        
        
//        std::vector<float> innerRectCoords;
//        innerRectCoords.insert(innerRectCoords.end(),
//                               bli, bli + 3);
//        innerRectCoords.insert(innerRectCoords.end(),
//                               bri, bri + 3);
//        innerRectCoords.insert(innerRectCoords.end(),
//                               tri, tri + 3);
//        innerRectCoords.insert(innerRectCoords.end(),
//                               tli, tli + 3);
//        drawLineLoop(innerRectCoords, rgba, 1);
    }
    
    {
        float blo[3];
        float bro[3];
        float tro[3];
        float tlo[3];
        createRectangle(center,
                        leftToRightUnitVector, bottomToTopUnitVector,
                        width + halfThickness*2, height + halfThickness*2,
                        blo, bro, tro, tlo);
        
        coords.insert(coords.end(),
                               blo, blo + 3);
        coords.insert(coords.end(),
                               bro, bro + 3);
        coords.insert(coords.end(),
                               tro, tro + 3);
        coords.insert(coords.end(),
                               tlo, tlo + 3);
//        std::vector<float> outerRectCoords;
//        outerRectCoords.insert(outerRectCoords.end(),
//                               blo, blo + 3);
//        outerRectCoords.insert(outerRectCoords.end(),
//                               bro, bro + 3);
//        outerRectCoords.insert(outerRectCoords.end(),
//                               tro, tro + 3);
//        outerRectCoords.insert(outerRectCoords.end(),
//                               tlo, tlo + 3);
//        drawLineLoop(outerRectCoords, rgba, 1);
    }
    
    const int32_t numCoords = static_cast<int32_t>(coords.size() / 3);
    
    std::vector<float> normals;
    normals.reserve(numCoords*3);
    std::vector<uint8_t> rgbaBytes;
    rgbaBytes.reserve(numCoords*4);
    
    const uint8_t colorBytes[4] = {
        static_cast<uint8_t>(rgba[0] * 255.0),
        static_cast<uint8_t>(rgba[1] * 255.0),
        static_cast<uint8_t>(rgba[2] * 255.0),
        static_cast<uint8_t>(rgba[3] * 255.0)
    };
    
    for (int32_t i = 0; i < numCoords; i++) {
        normals.insert(normals.end(),
                       normalVector, normalVector + 3);
        rgbaBytes.insert(rgbaBytes.end(),
                         colorBytes, colorBytes + 4);
    }
    
    CaretAssert(coords.size() == normals.size());
    CaretAssert(coords.size() == ((rgbaBytes.size() / 4) * 3));
    
    std::vector<uint32_t> indices;
    indices.push_back(0);
    indices.push_back(4);
    indices.push_back(1);
    indices.push_back(5);
    indices.push_back(2);
    indices.push_back(6);
    indices.push_back(3);
    indices.push_back(7);
    indices.push_back(0);
    indices.push_back(4);

    const bool enableAntiAliasingFlag = false;
    if (enableAntiAliasingFlag) {
        glPushAttrib(GL_COLOR_BUFFER_BIT
                     | GL_HINT_BIT
                     | GL_POLYGON_BIT);
        
        glBlendFunc (GL_SRC_ALPHA_SATURATE, GL_ONE);
        //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glEnable(GL_BLEND);
        
        glHint(GL_POLYGON_SMOOTH_HINT,
               GL_NICEST);
        glEnable(GL_POLYGON_SMOOTH);
    }
    
    drawQuadStrips(coords,
                   normals,
                   rgbaBytes,
                   indices);
    
    if (enableAntiAliasingFlag) {
        glPopAttrib();
    }
}
