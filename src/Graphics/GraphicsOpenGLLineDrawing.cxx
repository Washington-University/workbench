
/*LICENSE_START*/
/*
 *  Copyright (C) 2017 Washington University School of Medicine
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

#define __GRAPHICS_OPEN_G_L_LINE_DRAWING_DECLARE__
#include "GraphicsOpenGLLineDrawing.h"
#undef __GRAPHICS_OPEN_G_L_LINE_DRAWING_DECLARE__

#include "CaretAssert.h"
#include "CaretLogger.h"
#include "GraphicsEngineDataOpenGL.h"
#include "CaretOpenGLInclude.h"
#include "GraphicsPrimitive.h"
#include "GraphicsPrimitiveV3fC4f.h"
#include "GraphicsPrimitiveV3fC4ub.h"
#include "MathFunctions.h"
#include "Matrix4x4.h"

using namespace caret;


    
/**
 * \class caret::GraphicsOpenGLLineDrawing 
 * \brief Converts lines to rectangles
 * \ingroup Graphics
 *
 * Convert lines to rectangles.
 * OpenGL lines have a limited width whose maximum is often exceeded.
 * Convert the lines to rectangles.  All drawing is done in
 * two dimensional windows coordinates.
 */

/**
 * Destructor.
 */
GraphicsOpenGLLineDrawing::~GraphicsOpenGLLineDrawing()
{
}

/**
 * Constructor.
 *
 * @param openglContextPointer
 *     Pointer to OpenGL context.
 * @param xyz
 *     The XYZ vertices.
 * @param floatRGBA
 *     The Float RGBA coloring.
 * @param byteRGBA
 *     The Byte RGBA coloring.
 * @param lineThicknessPixels
 *     Thickness of lines in pixels.
 * @param colorType
 *     Type of color (solid or per-vertex)
 * @param lineType
 *     Type of lines drawn.
 */
GraphicsOpenGLLineDrawing::GraphicsOpenGLLineDrawing(void* openglContextPointer,
                                                     const std::vector<float>& xyz,
                                                     const std::vector<float>& floatRGBA,
                                                     const std::vector<uint8_t>& byteRGBA,
                                                     const float lineThicknessPixels,
                                                     const ColorType colorType,
                                                     const LineType lineType)
: m_openglContextPointer(openglContextPointer),
m_inputXYZ(xyz),
m_inputFloatRGBA(floatRGBA),
m_inputByteRGBA(byteRGBA),
m_lineThicknessPixels(lineThicknessPixels),
m_colorType(colorType),
m_lineType(lineType)
{
    
}

/**
 * Draw lines where each pair of vertices is drawn as an independent
 * line segment.  Same as OpenGL GL_LINES mode with glBegin().
 *
 * @param openglContextPointer
 *     Pointer to OpenGL context.
 * @param xyz
 *     The vertices.
 * @param rgba
 *     RGBA color for the lines with values ranging [0.0, 1.0]
 * @param lineThicknessPixels
 *     Thickness of line in pixels.
 * @return
 *     True if drawn, or false if there was an error.
 */
bool
GraphicsOpenGLLineDrawing::drawLinesPerVertexFloatColor(void* openglContextPointer,
                                                   const std::vector<float>& xyz,
                                                   const std::vector<float>& rgba,
                                                   const float lineThicknessPixels)
{
    std::vector<uint8_t> emptyByteRGBA;
    return drawLinesPrivate(openglContextPointer,
                            xyz,
                            rgba,
                            emptyByteRGBA,
                            lineThicknessPixels,
                            ColorType::FLOAT_RGBA_PER_VERTEX,
                            LineType::LINES);
}

/**
 * Draw lines where each pair of vertices is drawn as an independent
 * line segment.  Same as OpenGL GL_LINES mode with glBegin().
 *
 * @param openglContextPointer
 *     Pointer to OpenGL context.
 * @param xyz
 *     The vertices.
 * @param rgba
 *     RGBA color for the lines with values ranging [0.0, 1.0]
 * @param lineThicknessPixels
 *     Thickness of line in pixels.
 * @return
 *     True if drawn, or false if there was an error.
 */
bool
GraphicsOpenGLLineDrawing::drawLinesSolidFloatColor(void* openglContextPointer,
                                               const std::vector<float>& xyz,
                                               const float rgba[4],
                                               const float lineThicknessPixels)
{
    std::vector<float> rgbaVector = { rgba[0], rgba[1], rgba[2], rgba[3] };
    
    std::vector<uint8_t> emptyByteRGBA;
    return drawLinesPrivate(openglContextPointer,
                            xyz,
                            rgbaVector,
                            emptyByteRGBA,
                            lineThicknessPixels,
                            ColorType::FLOAT_RGBA_SOLID,
                            LineType::LINES);
}

/**
 * Draw a connect line strip.  Same as OpenGL GL_LINE_STRIP mode with glBegin().
 *
 * @param openglContextPointer
 *     Pointer to OpenGL context.
 * @param xyz
 *     The vertices.
 * @param rgba
 *     RGBA color for the lines with values ranging [0.0, 1.0]
 * @param lineThicknessPixels
 *     Thickness of line in pixels.
 * @return
 *     True if drawn, or false if there was an error.
 */
bool
GraphicsOpenGLLineDrawing::drawLineStripSolidFloatColor(void* openglContextPointer,
                                                   const std::vector<float>& xyz,
                                               const float rgba[4],
                                               const float lineThicknessPixels)
{
    std::vector<float> rgbaVector = { rgba[0], rgba[1], rgba[2], rgba[3] };
    std::vector<uint8_t> emptyByteRGBA;
    
    return drawLinesPrivate(openglContextPointer,
                            xyz,
                            rgbaVector,
                            emptyByteRGBA,
                            lineThicknessPixels,
                            ColorType::FLOAT_RGBA_SOLID,
                            LineType::LINE_STRIP);
}

/**
 * Draw a connect line loop (last is connected to first).  Same as OpenGL GL_LINE_LOOP mode with glBegin().
 *
 * @param openglContextPointer
 *     Pointer to OpenGL context.
 * @param xyz
 *     The vertices.
 * @param rgba
 *     RGBA color for the lines with values ranging [0.0, 1.0]
 * @param lineThicknessPixels
 *     Thickness of line in pixels.
 * @return
 *     True if drawn, or false if there was an error.
 */
bool
GraphicsOpenGLLineDrawing::drawLineLoopSolidFloatColor(void* openglContextPointer,
                                                  const std::vector<float>& xyz,
                                                   const float rgba[4],
                                                   const float lineThicknessPixels)
{
    std::vector<float> rgbaVector = { rgba[0], rgba[1], rgba[2], rgba[3] };
    std::vector<uint8_t> emptyByteRGBA;
    return drawLinesPrivate(openglContextPointer,
                            xyz,
                            rgbaVector,
                            emptyByteRGBA,
                            lineThicknessPixels,
                            ColorType::FLOAT_RGBA_SOLID,
                            LineType::LINE_LOOP);
}





/**
 * Draw lines where each pair of vertices is drawn as an independent
 * line segment.  Same as OpenGL GL_LINES mode with glBegin().
 *
 * @param openglContextPointer
 *     Pointer to OpenGL context.
 * @param xyz
 *     The vertices.
 * @param rgba
 *     RGBA color for the lines with values ranging [0, 255]
 * @param lineThicknessPixels
 *     Thickness of line in pixels.
 * @return
 *     True if drawn, or false if there was an error.
 */
bool
GraphicsOpenGLLineDrawing::drawLinesPerVertexByteColor(void* openglContextPointer,
                                                        const std::vector<float>& xyz,
                                                        const std::vector<uint8_t>& rgba,
                                                        const float lineThicknessPixels)
{
    std::vector<float> emptyFloatRGBA;
    return drawLinesPrivate(openglContextPointer,
                            xyz,
                            emptyFloatRGBA,
                            rgba,
                            lineThicknessPixels,
                            ColorType::BYTE_RGBA_PER_VERTEX,
                            LineType::LINES);
}

/**
 * Draw lines where each pair of vertices is drawn as an independent
 * line segment.  Same as OpenGL GL_LINES mode with glBegin().
 *
 * @param openglContextPointer
 *     Pointer to OpenGL context.
 * @param xyz
 *     The vertices.
 * @param rgba
 *     RGBA color for the lines with values ranging [0, 255]
 * @param lineThicknessPixels
 *     Thickness of line in pixels.
 * @return
 *     True if drawn, or false if there was an error.
 */
bool
GraphicsOpenGLLineDrawing::drawLinesSolidByteColor(void* openglContextPointer,
                                                    const std::vector<float>& xyz,
                                                    const uint8_t rgba[4],
                                                    const float lineThicknessPixels)
{
    std::vector<uint8_t> rgbaVector = { rgba[0], rgba[1], rgba[2], rgba[3] };
    
    std::vector<float> emptyFloatRGBA;
    return drawLinesPrivate(openglContextPointer,
                            xyz,
                            emptyFloatRGBA,
                            rgbaVector,
                            lineThicknessPixels,
                            ColorType::BYTE_RGBA_SOLID,
                            LineType::LINES);
}

/**
 * Draw a connect line strip.  Same as OpenGL GL_LINE_STRIP mode with glBegin().
 *
 * @param openglContextPointer
 *     Pointer to OpenGL context.
 * @param xyz
 *     The vertices.
 * @param rgba
 *     RGBA color for the lines with values ranging [0, 255]
 * @param lineThicknessPixels
 *     Thickness of line in pixels.
 * @return
 *     True if drawn, or false if there was an error.
 */
bool
GraphicsOpenGLLineDrawing::drawLineStripSolidByteColor(void* openglContextPointer,
                                                        const std::vector<float>& xyz,
                                                        const uint8_t rgba[4],
                                                        const float lineThicknessPixels)
{
    std::vector<uint8_t> rgbaVector = { rgba[0], rgba[1], rgba[2], rgba[3] };
    std::vector<float> emptyFloatRGBA;
    
    return drawLinesPrivate(openglContextPointer,
                            xyz,
                            emptyFloatRGBA,
                            rgbaVector,
                            lineThicknessPixels,
                            ColorType::BYTE_RGBA_SOLID,
                            LineType::LINE_STRIP);
}

/**
 * Draw a connect line loop (last is connected to first).  Same as OpenGL GL_LINE_LOOP mode with glBegin().
 *
 * @param openglContextPointer
 *     Pointer to OpenGL context.
 * @param xyz
 *     The vertices.
 * @param rgba
 *     RGBA color for the lines with values ranging [0, 255]
 * @param lineThicknessPixels
 *     Thickness of line in pixels.
 * @return
 *     True if drawn, or false if there was an error.
 */
bool
GraphicsOpenGLLineDrawing::drawLineLoopSolidByteColor(void* openglContextPointer,
                                                       const std::vector<float>& xyz,
                                                       const uint8_t rgba[4],
                                                       const float lineThicknessPixels)
{
    std::vector<uint8_t> rgbaVector = { rgba[0], rgba[1], rgba[2], rgba[3] };
    std::vector<float> emptyFloatRGBA;
    return drawLinesPrivate(openglContextPointer,
                            xyz,
                            emptyFloatRGBA,
                            rgbaVector,
                            lineThicknessPixels,
                            ColorType::BYTE_RGBA_SOLID,
                            LineType::LINE_LOOP);
}


/**
 * Draw lines where each pair of vertices is drawn as an independent
 * line segment.  Same as OpenGL GL_LINES mode with glBegin().
 *
 * @param openglContextPointer
 *     Pointer to OpenGL context.
 * @param xyz
 *     The vertices.
 * @param floatRGBA
 *     The Float RGBA coloring.
 * @param byteRGBA
 *     The Byte RGBA coloring.
 * @param lineThicknessPixels
 *     Thickness of line in pixels.
 * @param colorType
 *     Type of color (solid or per-vertex)
 * @param lineType
 *     Type of lines drawn.
 * @return
 *     True if drawn, or false if there was an error.
 */
bool
GraphicsOpenGLLineDrawing::drawLinesPrivate(void* openglContextPointer,
                                            const std::vector<float>& xyz,
                                            const std::vector<float>& floatRGBA,
                                            const std::vector<uint8_t>& byteRGBA,
                                            const float lineThicknessPixels,
                                            const ColorType colorType,
                                            const LineType lineType)
{
    GraphicsOpenGLLineDrawing lineDrawing(openglContextPointer,
                                          xyz,
                                          floatRGBA,
                                          byteRGBA,
                                          lineThicknessPixels,
                                          colorType,
                                          lineType);
    return lineDrawing.performDrawing();
}

/**
 * Perform the line drawinhg.
 *
 * @return
 *     True if no errors, false if errors.
 */
bool
GraphicsOpenGLLineDrawing::performDrawing()
{
    const int32_t numVertices = static_cast<int32_t>(m_inputXYZ.size() / 3);
    if (numVertices < 2) {
        CaretLogWarning("Invalid number of vertices="
                        + AString::number(numVertices)
                        + " for line drawing.  Must be at least 2.");
        return false;
    }
    
    const int32_t numFloatRGBA = static_cast<int32_t>(m_inputFloatRGBA.size());
    const int32_t numByteRGBA = static_cast<int32_t>(m_inputByteRGBA.size());
    switch (m_colorType) {
        case ColorType::BYTE_RGBA_PER_VERTEX:
        {
            const int32_t numRgbaVertices = numByteRGBA / 4;
            if (numVertices != numRgbaVertices) {
                CaretLogSevere("Mismatch in vertices and coloring.  There are "
                               + AString::number(numVertices)
                               + " vertices but have byte coloring for "
                               + AString::number(numRgbaVertices)
                               + " vertices");
                return false;
            }
        }
            break;
        case ColorType::BYTE_RGBA_SOLID:
            if (numByteRGBA < 4) {
                CaretLogSevere("Must have at last 4 color components for solid color line drawing"
                               "Number of color components is "
                               + AString::number(numByteRGBA));
                return false;
            }
            break;
        case ColorType::FLOAT_RGBA_PER_VERTEX:
        {
            const int32_t numRgbaVertices = numFloatRGBA / 4;
            if (numVertices != numRgbaVertices) {
                CaretLogSevere("Mismatch in vertices and coloring.  There are "
                               + AString::number(numVertices)
                               + " vertices but have float coloring for "
                               + AString::number(numRgbaVertices)
                               + " vertices");
                return false;
            }
        }
            break;
        case ColorType::FLOAT_RGBA_SOLID:
            if (numFloatRGBA < 4) {
                CaretLogSevere("Must have at last 4 color components for solid color line drawing"
                               "Number of color components is "
                               + AString::number(numFloatRGBA));
                return false;
            }
            break;
    }
    
    saveOpenGLState();
    
    createProjectionMatrix();
    
    createWindowCoordinatesFromVertices();
    
    const int32_t numWindowPoints = static_cast<int32_t>(m_vertexWindowXYZ.size() / 3);
    if (numWindowPoints >= 2) {
        convertLineSegmentsToQuads();
        
        joinQuads();
        
        CaretAssert(m_primitive);
        if (m_primitive->isValid()) {
            drawQuads();
        }
    }
    
    restoreOpenGLState();
    
    return true;
}

/**
 * Save the OpenGL State.
 */
void
GraphicsOpenGLLineDrawing::saveOpenGLState()
{
    glPushAttrib(GL_COLOR_BUFFER_BIT
                 | GL_ENABLE_BIT
                 | GL_HINT_BIT
                 | GL_LIGHTING_BIT
                 | GL_POLYGON_BIT);
    
    /*
     * For anti-aliasing requires sorting polygons
     * and turning of depth testing
     */
    const bool drawWithAntiAliasingFlag = false;
    if (drawWithAntiAliasingFlag) {
        glBlendFunc (GL_SRC_ALPHA_SATURATE, GL_ONE);
        glEnable(GL_BLEND);
        glHint(GL_POLYGON_SMOOTH_HINT,
               GL_NICEST);
        glEnable(GL_POLYGON_SMOOTH);
    }
    
    glDisable(GL_LIGHTING);
    glDisable(GL_COLOR_MATERIAL);
    glGetIntegerv(GL_POLYGON_MODE,
                  m_savedPolygonMode);
    
    glGetIntegerv(GL_VIEWPORT,
                  m_savedViewport);
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
}

/**
 * Restore the OpenGL State.
 */
void
GraphicsOpenGLLineDrawing::restoreOpenGLState()
{
    glPolygonMode(GL_FRONT, m_savedPolygonMode[0]);
    glPolygonMode(GL_BACK, m_savedPolygonMode[1]);
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    glViewport(m_savedViewport[0], m_savedViewport[1], m_savedViewport[2], m_savedViewport[3]);
    
    glPopAttrib();
}

/**
 * Create the projection matrix.
 */
void
GraphicsOpenGLLineDrawing::createProjectionMatrix()
{
    GLdouble modelviewArray[16];
    glGetDoublev(GL_MODELVIEW_MATRIX,
                 modelviewArray);
    Matrix4x4 modelviewMatrix;
    modelviewMatrix.setMatrixFromOpenGL(modelviewArray);
    
    GLdouble projectionArray[16];
    glGetDoublev(GL_PROJECTION_MATRIX,
                 projectionArray);
    
    m_projectionMatrix.setMatrixFromOpenGL(projectionArray);
    //m_projectionMatrix.postmultiply(modelviewMatrix);
    m_projectionMatrix.premultiply(modelviewMatrix);
}


/**
 * Create window coordinates from the input vertices.
 */
void
GraphicsOpenGLLineDrawing::createWindowCoordinatesFromVertices()
{
    const float coincidentMaxDistSQ = (0.01f * 0.01f);
    
    GLdouble depthRange[2];
    glGetDoublev(GL_DEPTH_RANGE,
                 depthRange);

    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT,
                  viewport);
    
    int32_t numInputPoints = static_cast<int32_t>(m_inputXYZ.size() / 3);
    int32_t iStep = 1;
    switch (m_lineType) {
        case LineType::LINES:
            iStep = 2;
            if (MathFunctions::isOddNumber(numInputPoints)) {
                CaretLogWarning("Odd number of points for drawing line segment pairs, should be even number.");
                --numInputPoints;
            }
            break;
        case LineType::LINE_LOOP:
            iStep = 1;
            break;
        case LineType::LINE_STRIP:
            iStep = 1;
            break;
    }
    
    m_vertexWindowXYZ.reserve(numInputPoints * 3);
    m_vertexWindowInputIndices.reserve(numInputPoints);
    
    /*
     * Coincident points that result in zero length
     * line segments are filtered out
     */
    for (int32_t i = 0; i < numInputPoints; i += iStep) {
        const int32_t i3 = i * 3;
        
        /*
         * Convert model coordinate to a window coordinate
         * as described by the documentation from gluProject().
         */
        float windowXYZ[3];
        CaretAssertVectorIndex(m_inputXYZ, i3 + 1);
        convertFromModelToWindowCoordinate(&m_inputXYZ[i3], windowXYZ);
        
        if (iStep == 1) {
            if (i > 0) {
                const float previousPointOffset = (m_vertexWindowXYZ.size() - 3);
                CaretAssertVectorIndex(m_vertexWindowXYZ, previousPointOffset + 2);
                const float distSQ = MathFunctions::distanceSquared3D(windowXYZ,
                                                                      &m_vertexWindowXYZ[previousPointOffset]);
                if (distSQ < coincidentMaxDistSQ) {
                    CaretLogSevere("Filtered out connected segment conincident point " + AString::number(i));
                    continue;
                }
            }

            m_vertexWindowXYZ.push_back(windowXYZ[0]);
            m_vertexWindowXYZ.push_back(windowXYZ[1]);
            m_vertexWindowXYZ.push_back(windowXYZ[2]);
            m_vertexWindowInputIndices.push_back(i);
        }
        else if (iStep == 2) {
            const int32_t iNext3 = (i + 1) * 3;
            float windowNextXYZ[3];
            CaretAssertVectorIndex(m_inputXYZ, iNext3 + 2);
            convertFromModelToWindowCoordinate(&m_inputXYZ[iNext3],
                                               windowNextXYZ);
            
            const float distSQ = MathFunctions::distanceSquared3D(windowXYZ,
                                                                  windowNextXYZ);
            if (distSQ < coincidentMaxDistSQ) {
                CaretLogSevere("Filtered out line pairs conincident points "
                               + AString::number(i)
                               + " and "
                               + AString::number(i + 1));
                continue;
            }
            m_vertexWindowXYZ.push_back(windowXYZ[0]);
            m_vertexWindowXYZ.push_back(windowXYZ[1]);
            m_vertexWindowXYZ.push_back(windowXYZ[2]);
            m_vertexWindowInputIndices.push_back(i);

            m_vertexWindowXYZ.push_back(windowNextXYZ[0]);
            m_vertexWindowXYZ.push_back(windowNextXYZ[1]);
            m_vertexWindowXYZ.push_back(windowNextXYZ[2]);
            m_vertexWindowInputIndices.push_back(i + 1);
        }
        else {
            CaretAssertMessage(0, "Invalid step value");
        }
        
        if (m_debugFlag) {
            GLdouble modelviewArray[16];
            glGetDoublev(GL_MODELVIEW_MATRIX, modelviewArray);
            GLdouble projectionArray[16];
            glGetDoublev(GL_PROJECTION_MATRIX, projectionArray);
            GLdouble winX, winY, winZ;
            gluProject(m_inputXYZ[i3], m_inputXYZ[i3+1], m_inputXYZ[i3+2],
                       modelviewArray, projectionArray, viewport,
                       &winX, &winY, &winZ);
            std::cout << "Input: " << m_inputXYZ[i3] << ", " << m_inputXYZ[i3+1] << ", " << m_inputXYZ[i3+2] << std::endl;
            std::cout << "   Pt " << i << ": "
            << winX << ", " << winY << ", " << winZ << std::endl;
            std::cout << "   gluProject " << winX << ", " << winY << ", " << winZ << std::endl;
        }
    }
    
    CaretAssert(m_vertexWindowXYZ.size() == (m_vertexWindowInputIndices.size() * 3));
}

/**
 * Convert from a model coordinate to a window coordinate.
 *
 * @param modelXYZ
 *     The coordinate in model space.
 * @param windowXYZOut
 *     Output with coordinate in window space.
 */
void
GraphicsOpenGLLineDrawing::convertFromModelToWindowCoordinate(const float modelXYZ[3],
                                                              float windowXYZOut[3]) const
{
    float xyzw[4] = { modelXYZ[0], modelXYZ[1], modelXYZ[2], 1.0f };
    m_projectionMatrix.multiplyPoint4(xyzw);
    windowXYZOut[0] = m_savedViewport[0] + (m_savedViewport[2] * ((xyzw[0] + 1.0f) / 2.0f));
    windowXYZOut[1] = m_savedViewport[1] + (m_savedViewport[3] * ((xyzw[1] + 1.0f) / 2.0f));
    windowXYZOut[2] = (xyzw[2] + 1.0f) / 2.0f;
    
    /*
     * From OpenGL Programming Guide 3rd Ed, p 133:
     *
     * If the near value is 1.0 and the far value is 3.0,
     * objects must have z-coordinates between -1.0 and -3.0 in order to be visible.
     * So, negate the Z-value to be negative.
     */
    windowXYZOut[2] = -windowXYZOut[2];
}

/**
 * Create a quad from the two window vertices
 *
 * @param indexOne
 *     Index of first vertex.
 * @param indexTwo
 *     Index of second vertex.
 * @param lastVertexFlag
 *     Flag indicating last vertex used when joining line loop last to first
 */
void
GraphicsOpenGLLineDrawing::createQuadFromWindowVertices(const int32_t indexOne,
                                                        const int32_t indexTwo,
                                                        const bool lastVertexFlag)
{
    const int32_t iOne3 = indexOne * 3;
    CaretAssertVectorIndex(m_vertexWindowXYZ, iOne3 + 2);
    const int32_t iTwo3 = indexTwo * 3;
    CaretAssertVectorIndex(m_vertexWindowXYZ, iTwo3 + 2);
    float x1 = m_vertexWindowXYZ[iOne3];
    float y1 = m_vertexWindowXYZ[iOne3 + 1];
    float z1 = m_vertexWindowXYZ[iOne3 + 2];
    float x2 = m_vertexWindowXYZ[iTwo3];
    float y2 = m_vertexWindowXYZ[iTwo3 + 1];
    float z2 = m_vertexWindowXYZ[iTwo3 + 2];
    
    /*
     * Normalized vector from start to end on line segment in 2D coordinates
     */
    float startToEndVector[3] = {
        x2 - x1,
        y2 - y1,
        0.0f
    };
    const float lineLength = MathFunctions::normalizeVector(startToEndVector);
    if (lineLength < 0.001f) {
        CaretLogWarning("This should not happen: Failure to filter coincident points with indices="
                       + AString::number(indexOne)
                       + ", "
                       + AString::number(indexTwo)
                       + " Coordinates: "
                       + AString::fromNumbers(&m_vertexWindowXYZ[iOne3], 3, ",")
                       + "    "
                       + AString::fromNumbers(&m_vertexWindowXYZ[iOne3], 3, ","));
    }
    
    /*
     * Create a perpendicualar vector to the line segment
     */
    float perpendicularVector[3] = {
        startToEndVector[1],
        -startToEndVector[0],
        0.0f
    };
    
    /*
     * "Width" of rectangle
     */
    const float halfWidth = m_lineThicknessPixels / 2.0f;
    const float halfWidthX = perpendicularVector[0] * halfWidth;
    const float halfWidthY = perpendicularVector[1] * halfWidth;
    
    /*
     * Points of the rectangle
     * They should be counter-clockwise oriented
     */
    float p1[3] = {
        x1 - halfWidthX,
        y1 - halfWidthY,
        z1
    };
    float p2[3] = {
        x1 + halfWidthX,
        y1 + halfWidthY,
        z1
    };
    float p3[3] = {
        x2 + halfWidthX,
        y2 + halfWidthY,
        z2
    };
    float p4[3] = {
        x2 - halfWidthX,
        y2 - halfWidthY,
        z2
    };
    
    /*
     * Indices to input coloring.
     * Note that coincident points may be removed
     */
    CaretAssertVectorIndex(m_vertexWindowInputIndices, indexOne);
    const int32_t indexOneRgba = m_vertexWindowInputIndices[indexOne];
    CaretAssertVectorIndex(m_vertexWindowInputIndices, indexTwo);
    const int32_t indexTwoRgba = m_vertexWindowInputIndices[indexTwo];

    /*
     * RGBA for the points P1, P2, P3, P4
     */
    int32_t rgbaOffsetP1P2 = 0;
    int32_t rgbaOffsetP3P4 = 0;
    switch (m_colorType) {
        case ColorType::BYTE_RGBA_PER_VERTEX:
            rgbaOffsetP1P2 = indexOneRgba * 4;
            rgbaOffsetP3P4 = indexTwoRgba * 4;
            break;
        case ColorType::BYTE_RGBA_SOLID:
            break;
        case ColorType::FLOAT_RGBA_PER_VERTEX:
            rgbaOffsetP1P2 = indexOneRgba * 4;
            rgbaOffsetP3P4 = indexTwoRgba * 4;
            break;
        case ColorType::FLOAT_RGBA_SOLID:
            break;
    }
    
    float crossProduct[3];
    MathFunctions::crossProduct(startToEndVector, perpendicularVector, crossProduct);
    if (crossProduct[2] == 0.0f) {
        /* This should get caught by vector length check above */
    }
    
    switch (m_colorType) {
        case ColorType::BYTE_RGBA_PER_VERTEX:
        case ColorType::BYTE_RGBA_SOLID:
        {
            CaretAssertVectorIndex(m_inputByteRGBA, rgbaOffsetP1P2 + 3);
            const uint8_t* rgbaP12 = &m_inputByteRGBA[rgbaOffsetP1P2];
            CaretAssertVectorIndex(m_inputByteRGBA, rgbaOffsetP3P4 + 3);
            const uint8_t* rgbaP34 = &m_inputByteRGBA[rgbaOffsetP3P4];
            
            CaretAssert(m_primitiveByteColor.get());
            if (crossProduct[2] <= 0.0f) {
                m_primitiveByteColor->addVertex(p1, rgbaP12);
                m_primitiveByteColor->addVertex(p2, rgbaP12);
                m_primitiveByteColor->addVertex(p3, rgbaP34);
                m_primitiveByteColor->addVertex(p4, rgbaP34);
            }
            else {
                std::cout << "Flipped Quad" << std::endl;
                m_primitiveByteColor->addVertex(p2, rgbaP12);
                m_primitiveByteColor->addVertex(p1, rgbaP12);
                m_primitiveByteColor->addVertex(p4, rgbaP34);
                m_primitiveByteColor->addVertex(p3, rgbaP34);
            }
        }
            break;
        case ColorType::FLOAT_RGBA_PER_VERTEX:
        case ColorType::FLOAT_RGBA_SOLID:
        {
            CaretAssertVectorIndex(m_inputFloatRGBA, rgbaOffsetP1P2 + 3);
            const float* rgbaP12 = &m_inputFloatRGBA[rgbaOffsetP1P2];
            CaretAssertVectorIndex(m_inputFloatRGBA, rgbaOffsetP3P4 + 3);
            const float* rgbaP34 = &m_inputFloatRGBA[rgbaOffsetP3P4];
            
            CaretAssert(m_primitiveFloatColor.get());
            if (crossProduct[2] <= 0.0f) {
                m_primitiveFloatColor->addVertex(p1, rgbaP12);
                m_primitiveFloatColor->addVertex(p2, rgbaP12);
                m_primitiveFloatColor->addVertex(p3, rgbaP34);
                m_primitiveFloatColor->addVertex(p4, rgbaP34);
            }
            else {
                std::cout << "Flipped Quad" << std::endl;
                m_primitiveFloatColor->addVertex(p2, rgbaP12);
                m_primitiveFloatColor->addVertex(p1, rgbaP12);
                m_primitiveFloatColor->addVertex(p4, rgbaP34);
                m_primitiveFloatColor->addVertex(p3, rgbaP34);
            }
        }
            break;
    }
    
//    /*
//     * Perform joins of connected lines
//     */
//    switch (m_lineType) {
//        case LineType::LINES:
//            return;
//            break;
//        case LineType::LINE_LOOP:
//            break;
//        case LineType::LINE_STRIP:
//            break;
//    }
//    
//    if (indexTwo >= 2) {
//        CaretAssert(m_primitive);
//        const int32_t numQuadVertices = m_primitive->getNumberOfVertices();
//        
//        switch (m_joinType) {
//            case JoinType::NONE:
//                return;
//                break;
//            case JoinType::MITER:
//                if (lastVertexFlag) {
//                    /* Join last quad to first quad */
//                    performJoin(indexOne,
//                                indexTwo,
//                                0,
//                                numQuadVertices - 4,
//                                0);
//                }
//                else {
//                    performJoin(indexOne - 1,
//                                indexOne,
//                                indexTwo,
//                                numQuadVertices - 8,
//                                numQuadVertices - 4);
//                }
//                break;
//        }
//    }
}

void
GraphicsOpenGLLineDrawing::joinQuads()
{
    /*
     * Perform joins of connected lines
     */
    
    CaretAssert(m_primitive);
    const int32_t numberOfLineVertices = (m_vertexWindowXYZ.size() / 3);
    const int32_t lastVertexIndex = numberOfLineVertices - 1;
    int32_t numberOfLines = numberOfLineVertices;

    switch (m_lineType) {
        case LineType::LINES:
            numberOfLines = 0;
            break;
        case LineType::LINE_LOOP:
            break;
        case LineType::LINE_STRIP:
            numberOfLines = numberOfLines - 2;
            break;
    }

    switch (m_joinType) {
        case JoinType::NONE:
            numberOfLines = 0;
            break;
        case JoinType::MITER:
            for (int32_t indexOne = 0; indexOne < numberOfLines; indexOne++) {
                const int32_t indexTwo = (((indexOne + 1) <= lastVertexIndex) ? (indexOne + 1) : 0);
                const int32_t indexThree = (((indexTwo + 1) <= lastVertexIndex) ? (indexTwo + 1) : 0);
                if (m_debugFlag) {
                    std::cout << "JOIN: " << indexOne << ", " << indexTwo << ", " << indexThree << std::endl;
                }
                
                const int32_t quadOneVertexIndex = indexOne * 4;
                const int32_t quadTwoVertexIndex = indexTwo * 4;
                performJoin(indexOne, indexTwo, indexThree, quadOneVertexIndex, quadTwoVertexIndex);
            }
            break;
    }
}

/**
 * Perform a join using the last two vertices in quad one
 * and the first two vertices in quad two
 *
 * @param lineVertexIndexOne
 *     Index of first vertex in line segment
 * @param lineVertexIndexTwo
 *     Index of second vertex in line segment
 * @param lineVertexIndexThree
 *     Index of three vertex in line segment
 * @param quadOneVertexIndex
 *     Index of first vertex in first quad
 * @param quadTwoVertexIndex
 *     Index of first vertex in second quad
 */
void
GraphicsOpenGLLineDrawing::performJoin(const int32_t lineVertexIndexOne,
                                       const int32_t lineVertexIndexTwo,
                                       const int32_t lineVertexIndexThree,
                                       const int32_t quadOneVertexIndex,
                                       const int32_t quadTwoVertexIndex)
{
    if (m_debugFlag) std::cout << "Join: " << lineVertexIndexOne << ", " << lineVertexIndexTwo << ", " << lineVertexIndexThree << std::endl;
    CaretAssert(m_primitive);
    
    /*
     * Determine if the junction of the two consecutive 
     * line segments in window coordinates makes a left
     * or right turn
     */
    CaretAssertVectorIndex(m_vertexWindowXYZ, (lineVertexIndexOne * 3) + 2);
    CaretAssertVectorIndex(m_vertexWindowXYZ, (lineVertexIndexTwo * 3) + 2);
    CaretAssertVectorIndex(m_vertexWindowXYZ, (lineVertexIndexThree * 3) + 2);
    const float signedArea = MathFunctions::triangleAreaSigned2D(&m_vertexWindowXYZ[lineVertexIndexOne * 3],
                                                                 &m_vertexWindowXYZ[lineVertexIndexTwo * 3],
                                                                 &m_vertexWindowXYZ[lineVertexIndexThree * 3]);


    int32_t quadOneSideBeginVertexIndex = -1;
    int32_t quadOneSideEndVertexIndex = -1;
    int32_t quadTwoSideBeginVertexIndex = -1;
    int32_t quadTwoSideEndVertexIndex = -1;
    const float smallNumber = 0.001;
    if (signedArea > smallNumber) {
        if (m_debugFlag) std::cout << "   Left Turn" << std::endl;
        /*
         * Right side is vertices two and three
         */
        quadOneSideBeginVertexIndex = quadOneVertexIndex + 1;
        quadOneSideEndVertexIndex = quadOneVertexIndex + 2;
        quadTwoSideBeginVertexIndex = quadTwoVertexIndex + 1;
        quadTwoSideEndVertexIndex = quadTwoVertexIndex + 2;
    }
    else if (signedArea < -smallNumber) {
        if (m_debugFlag) std::cout << "   Right Turn" << std::endl;
        /*
         * Right side is vertices 1 and 4
         */
        quadOneSideBeginVertexIndex = quadOneVertexIndex;
        quadOneSideEndVertexIndex = quadOneVertexIndex + 3;
        quadTwoSideBeginVertexIndex = quadTwoVertexIndex;
        quadTwoSideEndVertexIndex = quadTwoVertexIndex + 3;
    }
    else {
        if (m_debugFlag) std::cout << "   Parallel" << std::endl;
        return;
    }
  
    const std::vector<float>& xyz = m_primitive->getFloatXYZ();
    CaretAssertVectorIndex(xyz, (quadOneSideBeginVertexIndex * 3) + 2);
    CaretAssertVectorIndex(xyz, (quadOneSideEndVertexIndex * 3) + 2);
    CaretAssertVectorIndex(xyz, (quadTwoSideBeginVertexIndex * 3) + 2);
    CaretAssertVectorIndex(xyz, (quadTwoSideEndVertexIndex * 3) + 2);
    const float* q1v1 = &xyz[quadOneSideBeginVertexIndex * 3];
    const float* q1v2 = &xyz[quadOneSideEndVertexIndex * 3];
    const float* q2v1 = &xyz[quadTwoSideBeginVertexIndex * 3];
    const float* q2v2 = &xyz[quadTwoSideEndVertexIndex * 3];
    
    /*
     * Find location of where the edges in the quads intersect
     */
    float intersectionXYZ[3];
    if (MathFunctions::vectorIntersection2D(q1v1, q1v2, q2v1, q2v2, 0.0f, intersectionXYZ)) {
        intersectionXYZ[2] = q1v1[2];
        if (m_debugFlag) std::cout << "      Intersection: " << AString::fromNumbers(intersectionXYZ, 3, ",") << std::endl;
        m_primitive->replaceVertexFloatXYZ(quadOneSideEndVertexIndex, intersectionXYZ);
        m_primitive->replaceVertexFloatXYZ(quadTwoSideBeginVertexIndex, intersectionXYZ);
    }
}

/**
 * Convert each line segment into a rectangle
 * using the line width.  "Length" of the
 * rectangle is the length of the line segment
 * and "width" of the rectangle is the line width.
 */
void
GraphicsOpenGLLineDrawing::convertLineSegmentsToQuads()
{
    
    const int32_t numVertices = static_cast<int32_t>(m_vertexWindowXYZ.size() / 3);
    
    int32_t estimatedNumberOfQuads = 0;
    switch (m_lineType) {
        case LineType::LINES:
            estimatedNumberOfQuads = numVertices / 2;
            break;
        case LineType::LINE_LOOP:
            estimatedNumberOfQuads = numVertices;
            break;
        case LineType::LINE_STRIP:
            estimatedNumberOfQuads = numVertices - 1;
            break;
    }
    
    m_primitive = NULL;
    switch (m_colorType) {
        case ColorType::BYTE_RGBA_PER_VERTEX:
        case ColorType::BYTE_RGBA_SOLID:
            m_primitiveByteColor.reset(GraphicsPrimitive::newPrimitiveV3fC4ub(GraphicsPrimitive::PrimitiveType::QUADS));
            m_primitiveByteColor->reserveForNumberOfVertices(estimatedNumberOfQuads * 4);
            m_primitive = m_primitiveByteColor.get();
            break;
        case ColorType::FLOAT_RGBA_PER_VERTEX:
        case ColorType::FLOAT_RGBA_SOLID:
            m_primitiveFloatColor.reset(GraphicsPrimitive::newPrimitiveV3fC4f(GraphicsPrimitive::PrimitiveType::QUADS));
            m_primitiveFloatColor->reserveForNumberOfVertices(estimatedNumberOfQuads * 4);
            m_primitive = m_primitiveFloatColor.get();
            break;
    }
    CaretAssert(m_primitive);

    m_primitive->setUsageType(GraphicsPrimitive::UsageType::MODIFIED_ONCE_DRAWN_FEW_TIMES);
    
    const int32_t numVerticesMinusOne = numVertices - 1;
    
    switch (m_lineType) {
        case LineType::LINES:
        {
            const int32_t numLineSegments = (numVertices / 2);
            for (int32_t i = 0; i < numLineSegments; i++) {
                const int32_t i2 = i * 2;
                createQuadFromWindowVertices(i2, i2 + 1, false);
            }
        }
            break;
        case LineType::LINE_LOOP:
            for (int32_t i = 0; i < numVerticesMinusOne; i++) {
                createQuadFromWindowVertices(i, i + 1, false);
            }
            createQuadFromWindowVertices(numVerticesMinusOne, 0, true);
            break;
        case LineType::LINE_STRIP:
            for (int32_t i = 0; i < numVerticesMinusOne; i++) {
                createQuadFromWindowVertices(i, i + 1, false);
            }
            break;
    }
}

/**
 * Draw the lines using quads.
 */
void
GraphicsOpenGLLineDrawing::drawQuads()
{
    GLdouble depthRange[2];
    glGetDoublev(GL_DEPTH_RANGE,
                 depthRange);
    
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT,
                  viewport);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(viewport[0], viewport[0] + viewport[2],
            viewport[1], viewport[1] + viewport[3],
            0, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    glPolygonMode(GL_FRONT, GL_FILL);
    //glPolygonMode(GL_BACK, GL_LINE);
    
    CaretAssert(m_primitive);
    GraphicsEngineDataOpenGL::draw(m_openglContextPointer,
                                   m_primitive);
    if (m_debugFlag) {
        std::cout << std::endl << "Quad Primitive: " << m_primitive->toString() << std::endl;
        std::cout << "viewport: " << AString::fromNumbers(viewport, 4, ",") << std::endl;
    }
    
//    switch (m_colorType) {
//        case ColorType::BYTE_RGBA_PER_VERTEX:
//        case ColorType::BYTE_RGBA_SOLID:
//            CaretAssert(m_primitiveByteColor.get());
//            GraphicsEngineDataOpenGL::draw(m_openglContextPointer,
//                                           m_primitiveByteColor.get());
//            if (m_debugFlag) {
//                std::cout << std::endl << "Quad Primitive: " << m_primitiveByteColor->toString() << std::endl;
//                std::cout << "viewport: " << AString::fromNumbers(viewport, 4, ",") << std::endl;
//            }
//            break;
//        case ColorType::FLOAT_RGBA_PER_VERTEX:
//        case ColorType::FLOAT_RGBA_SOLID:
//            CaretAssert(m_primitiveFloatColor.get());
//            GraphicsEngineDataOpenGL::draw(m_openglContextPointer,
//                                           m_primitiveFloatColor.get());
//            if (m_debugFlag) {
//                std::cout << std::endl << "Quad Primitive: " << m_primitiveFloatColor->toString() << std::endl;
//                std::cout << "viewport: " << AString::fromNumbers(viewport, 4, ",") << std::endl;
//            }
//            break;
//    }
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
GraphicsOpenGLLineDrawing::toString() const
{
    return "GraphicsOpenGLLineDrawing";
}

