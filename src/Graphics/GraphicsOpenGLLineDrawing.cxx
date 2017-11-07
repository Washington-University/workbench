
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
 * Draw the given graphics primitive containing lines
 * by converting the lines to polygons.
 *
 * @param openglContextPointer
 *     Pointer to OpenGL context.
 * @param primtive
 *     The graphics primitive.
 */
bool
GraphicsOpenGLLineDrawing::draw(void* openglContextPointer,
                                const GraphicsPrimitive* primitive)
{
    CaretAssert(primitive);
    if (primitive->isValid()) {
        float lineWidth = primitive->m_lineWidthValue;
        
        switch (primitive->m_lineWidthType) {
            case GraphicsPrimitive::SizeType::PERCENTAGE_VIEWPORT_HEIGHT:
            {
                GLint viewport[4];
                glGetIntegerv(GL_VIEWPORT, viewport);
                lineWidth = (lineWidth / 100.0) * viewport[3];
            }
                break;
            case GraphicsPrimitive::SizeType::PIXELS:
                break;
        }
        
        ColorType colorType = ColorType::FLOAT_RGBA_PER_VERTEX;
        switch (primitive->m_colorType) {
            case GraphicsPrimitive::ColorType::NONE:
                CaretLogSevere("INVALID color type NONE for graphics primitive");
                return false;
                break;
            case GraphicsPrimitive::ColorType::FLOAT_RGBA:
                colorType = ColorType::FLOAT_RGBA_PER_VERTEX;
                break;
            case GraphicsPrimitive::ColorType::UNSIGNED_BYTE_RGBA:
                colorType = ColorType::BYTE_RGBA_PER_VERTEX;
                break;
        }
        
        LineType lineType = LineType::LINES;
        switch (primitive->m_primitiveType) {
            case GraphicsPrimitive::PrimitiveType::OPENGL_LINE_LOOP:
                lineType = LineType::LINE_LOOP;
                break;
            case GraphicsPrimitive::PrimitiveType::OPENGL_LINE_STRIP:
                lineType = LineType::LINE_STRIP;
                break;
            case GraphicsPrimitive::PrimitiveType::OPENGL_LINES:
                lineType = LineType::LINES;
                break;
            case GraphicsPrimitive::PrimitiveType::OPENGL_POINTS:
                CaretLogSevere("POINTS is not a valid line drawing type");
                return false;
                break;
            case GraphicsPrimitive::PrimitiveType::OPENGL_POLYGON:
                CaretLogSevere("POLYGON is not a valid line drawing type");
                return false;
                break;
            case GraphicsPrimitive::PrimitiveType::OPENGL_QUAD_STRIP:
                CaretLogSevere("QUAD_STRIP is not a valid line drawing type");
                return false;
                break;
            case GraphicsPrimitive::PrimitiveType::OPENGL_QUADS:
                CaretLogSevere("QUADS is not a valid line drawing type");
                return false;
                break;
            case GraphicsPrimitive::PrimitiveType::OPENGL_TRIANGLE_FAN:
                CaretLogSevere("TRIANGLE_FAN is not a valid line drawing type");
                return false;
                break;
            case GraphicsPrimitive::PrimitiveType::OPENGL_TRIANGLE_STRIP:
                CaretLogSevere("TRIANGLE_STRIP is not a valid line drawing type");
                return false;
                break;
            case GraphicsPrimitive::PrimitiveType::OPENGL_TRIANGLES:
                CaretLogSevere("TRIANGLES is not a valid line drawing type");
                return false;
                break;
        }
        
        return drawLinesPrivate(openglContextPointer,
                                primitive->m_xyz,
                                primitive->m_floatRGBA,
                                primitive->m_unsignedByteRGBA,
                                primitive->m_primitiveRestartIndices,
                                lineWidth,
                                colorType,
                                lineType);
    }
    
    return false;
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
 * @param vertexPrimitiveRestartIndices
 *     Contains indices at which the primitive should restart.
 *     The primitive will stop at the index and not connect to the next index.
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
                                                     const std::set<int32_t>& vertexPrimitiveRestartIndices,
                                                     const float lineThicknessPixels,
                                                     const ColorType colorType,
                                                     const LineType lineType)
: m_openglContextPointer(openglContextPointer),
m_inputXYZ(xyz),
m_inputFloatRGBA(floatRGBA),
m_inputByteRGBA(byteRGBA),
m_vertexPrimitiveRestartIndices(vertexPrimitiveRestartIndices),
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
    std::set<int32_t> emptyPrimitiveRestartIndices;
    std::vector<uint8_t> emptyByteRGBA;
    return drawLinesPrivate(openglContextPointer,
                            xyz,
                            rgba,
                            emptyByteRGBA,
                            emptyPrimitiveRestartIndices,
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
    
    std::set<int32_t> emptyPrimitiveRestartIndices;
    std::vector<uint8_t> emptyByteRGBA;
    return drawLinesPrivate(openglContextPointer,
                            xyz,
                            rgbaVector,
                            emptyByteRGBA,
                            emptyPrimitiveRestartIndices,
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
    std::set<int32_t> emptyPrimitiveRestartIndices;
    
    return drawLinesPrivate(openglContextPointer,
                            xyz,
                            rgbaVector,
                            emptyByteRGBA,
                            emptyPrimitiveRestartIndices,
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
    std::set<int32_t> emptyPrimitiveRestartIndices;
    
    return drawLinesPrivate(openglContextPointer,
                            xyz,
                            rgbaVector,
                            emptyByteRGBA,
                            emptyPrimitiveRestartIndices,
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
    std::set<int32_t> emptyPrimitiveRestartIndices;
    
    return drawLinesPrivate(openglContextPointer,
                            xyz,
                            emptyFloatRGBA,
                            rgba,
                            emptyPrimitiveRestartIndices,
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
    std::set<int32_t> emptyPrimitiveRestartIndices;
    
    return drawLinesPrivate(openglContextPointer,
                            xyz,
                            emptyFloatRGBA,
                            rgbaVector,
                            emptyPrimitiveRestartIndices,
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
    std::set<int32_t> emptyPrimitiveRestartIndices;
    
    return drawLinesPrivate(openglContextPointer,
                            xyz,
                            emptyFloatRGBA,
                            rgbaVector,
                            emptyPrimitiveRestartIndices,
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
    std::set<int32_t> emptyPrimitiveRestartIndices;
    
    return drawLinesPrivate(openglContextPointer,
                            xyz,
                            emptyFloatRGBA,
                            rgbaVector,
                            emptyPrimitiveRestartIndices,
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
 * @param vertexPrimitiveRestartIndices
 *     Contains indices at which the primitive should restart.
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
                                            const std::set<int32_t>& vertexPrimitiveRestartIndices,
                                            const float lineThicknessPixels,
                                            const ColorType colorType,
                                            const LineType lineType)
{
    GraphicsOpenGLLineDrawing lineDrawing(openglContextPointer,
                                          xyz,
                                          floatRGBA,
                                          byteRGBA,
                                          vertexPrimitiveRestartIndices,
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
 */
void
GraphicsOpenGLLineDrawing::createQuadFromWindowVertices(const int32_t indexOne,
                                                        const int32_t indexTwo)
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
    
    m_quadInfo.push_back(QuadInfo(indexOne, indexTwo));
}

/**
 * Join quads to create connected line segments
 * without cracks where the quads join together.
 */
void
GraphicsOpenGLLineDrawing::joinQuads()
{
    CaretAssert(m_primitive);

    const int32_t numQuads = static_cast<int32_t>(m_quadInfo.size());
    const int32_t numQuadsMinusOne = numQuads - 1;
    
    int32_t numQuadsToConnect = 0;
    switch (m_lineType) {
        case LineType::LINES:
            numQuadsToConnect = 0;
            break;
        case LineType::LINE_LOOP:
            numQuadsToConnect = numQuads;
            break;
        case LineType::LINE_STRIP:
            numQuadsToConnect = numQuadsMinusOne;
            break;
    }
    
    /*
     * Perform joins of connected quads
     */
    for (int32_t i = 0; i < numQuadsToConnect; i++) {
        CaretAssertVectorIndex(m_quadInfo, i);
        const QuadInfo& quad = m_quadInfo[i];
        
        const int32_t nextQuadIndex = ((i < numQuadsMinusOne) ? (i + 1) : 0);
        CaretAssertVectorIndex(m_quadInfo, nextQuadIndex);
        const QuadInfo& nextQuad = m_quadInfo[nextQuadIndex];
        
        /*
         * Does quad "i" connect to quad "i+1" ?
         */
        if (quad.m_vertexTwoIndex == nextQuad.m_vertexOneIndex) {
            const int32_t quadOneVertexIndex = i * 4;
            const int32_t quadTwoVertexIndex = nextQuadIndex * 4;
            
            performJoin(quad.m_vertexOneIndex,
                        quad.m_vertexTwoIndex,
                        nextQuad.m_vertexTwoIndex,
                        quadOneVertexIndex,
                        quadTwoVertexIndex);
        }
    }
}

/**
 * Perform a join using the last two vertices in quad one
 * and the first two vertices in quad two
 *
 * @param windowVertexIndexOne
 *     Index of first window vertex from line segment
 * @param windowVertexIndexTwo
 *     Index of second window from line segment
 * @param windowVertexIndexThree
 *     Index of three window from line segment
 * @param quadOneVertexIndex
 *     Index of first vertex in first quad
 * @param quadTwoVertexIndex
 *     Index of first vertex in second quad
 */
void
GraphicsOpenGLLineDrawing::performJoin(const int32_t windowVertexIndexOne,
                                       const int32_t windowVertexIndexTwo,
                                       const int32_t windowVertexIndexThree,
                                       const int32_t quadOneVertexIndex,
                                       const int32_t quadTwoVertexIndex)
{
    if (m_debugFlag) std::cout << "Join: " << windowVertexIndexOne << ", " << windowVertexIndexTwo << ", " << windowVertexIndexThree << std::endl;
    CaretAssert(m_primitive);
    
    /*
     * Determine if the junction of the two consecutive 
     * line segments in window coordinates makes a left
     * or right turn
     */
    CaretAssertVectorIndex(m_vertexWindowXYZ, (windowVertexIndexOne * 3) + 2);
    CaretAssertVectorIndex(m_vertexWindowXYZ, (windowVertexIndexTwo * 3) + 2);
    CaretAssertVectorIndex(m_vertexWindowXYZ, (windowVertexIndexThree * 3) + 2);
    const float signedArea = MathFunctions::triangleAreaSigned2D(&m_vertexWindowXYZ[windowVertexIndexOne * 3],
                                                                 &m_vertexWindowXYZ[windowVertexIndexTwo * 3],
                                                                 &m_vertexWindowXYZ[windowVertexIndexThree * 3]);


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
     * Miter limit is a multiple of the line thickness
     */
    const float miterLimit = m_lineThicknessPixels;// * 2.0f;
    const float miterLimitSquared = miterLimit * miterLimit;
    
    /*
     * Find location of where the edges in the quads intersect
     */
    float intersectionXYZ[3];
    if (MathFunctions::vectorIntersection2D(q1v1, q1v2, q2v1, q2v2, 0.0f, intersectionXYZ)) {
        intersectionXYZ[2] = q1v1[2];
        if (m_debugFlag) std::cout << "      Intersection: " << AString::fromNumbers(intersectionXYZ, 3, ",") << std::endl;
        
        /*
         * When the angle is very sharp, the miter point can be very long and pointy
         * so limit to a multiplier of the line thickness
         */
        CaretAssertVectorIndex(m_vertexWindowXYZ, (windowVertexIndexTwo * 3) + 2);
        const float* joinPointXYZ = &m_vertexWindowXYZ[(windowVertexIndexTwo * 3)];
        const float distanceSquared = MathFunctions::distanceSquared3D(joinPointXYZ,
                                                                       intersectionXYZ);
        if (distanceSquared > miterLimitSquared) {
            /*
             * Extend lines by the miter limit to create a new quad
             */
            float newV1[3];
            createMiterJoinVertex(q1v1, q1v2, miterLimit, newV1);
            float newV2[3];
            createMiterJoinVertex(q2v2, q2v1, miterLimit, newV2);
            
            m_quadJoins.push_back(QuadJoin(q1v2,
                                           newV1,
                                           q2v1,
                                           newV2,
                                           quadOneSideEndVertexIndex,
                                           quadTwoSideBeginVertexIndex));
        }
        else {
            /*
             * Move the existing quad vertices to their intersection point
             */
            m_primitive->replaceVertexFloatXYZ(quadOneSideEndVertexIndex, intersectionXYZ);
            m_primitive->replaceVertexFloatXYZ(quadTwoSideBeginVertexIndex, intersectionXYZ);
        }
    }
}

/**
 * Create a vertex for a miter join when the length limit is exceeded
 *
 * @param v1
 *     First vertex
 * @param v2
 *     Second vertex that is connected to the miter join vertex.
 * @param miterLength
 *     Length of output vertex from v2
 * @param xyzOut
 *     Output with vertex used for miter join.
 */
void
GraphicsOpenGLLineDrawing::createMiterJoinVertex(const float v1[3],
                                                 const float v2[3],
                                                 const float miterLength,
                                                 float xyzOut[3])
{
    float n2[3];
    MathFunctions::subtractVectors(v2, v1, n2);
    MathFunctions::normalizeVector(n2);
    n2[0] *= miterLength;
    n2[1] *= miterLength;
    n2[2] *= miterLength;
    xyzOut[0] = v2[0] + n2[0];
    xyzOut[1] = v2[1] + n2[1];
    xyzOut[2] = v2[2] + n2[2];
}

/**
 * When the quads are joined, additional quads may be
 * created to to fill in the gaps.
 */
void
GraphicsOpenGLLineDrawing::addMiterJoinQuads()
{
    const int32_t numJoinQuads = static_cast<int32_t>(m_quadJoins.size());
    for (int32_t i = 0; i < numJoinQuads; i++) {
        CaretAssertVectorIndex(m_quadJoins, i);
        const QuadJoin& quadJoin = m_quadJoins[i];
        
        /*
         * Add the join quad to the primitive
         */
        switch (m_primitive->getColorType()) {
            case GraphicsPrimitive::ColorType::NONE:
                CaretAssert(0);
                break;
            case GraphicsPrimitive::ColorType::FLOAT_RGBA:
            {
                float rgba[4];
                m_primitiveFloatColor->getVertexFloatRGBA(quadJoin.m_primitiveOneTwoIndex, rgba);
                m_primitiveFloatColor->addVertex(&quadJoin.m_xyz[0], rgba);
                m_primitiveFloatColor->addVertex(&quadJoin.m_xyz[3], rgba);
                m_primitiveFloatColor->addVertex(&quadJoin.m_xyz[6], rgba);
                m_primitiveFloatColor->addVertex(&quadJoin.m_xyz[9], rgba);
            }
                break;
            case GraphicsPrimitive::ColorType::UNSIGNED_BYTE_RGBA:
            {
                uint8_t rgba[4];
                m_primitiveByteColor->getVertexByteRGBA(quadJoin.m_primitiveThreeFourIndex, rgba);
                m_primitiveByteColor->addVertex(&quadJoin.m_xyz[0], rgba);
                m_primitiveByteColor->addVertex(&quadJoin.m_xyz[3], rgba);
                m_primitiveByteColor->addVertex(&quadJoin.m_xyz[6], rgba);
                m_primitiveByteColor->addVertex(&quadJoin.m_xyz[9], rgba);
            }
                break;
        }
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
    
    /*
     * When estimating number of quads,
     * include both the quads formed by the vertices
     * and possible quads added for joining end points
     */
    int32_t estimatedNumberOfQuads = 0;
    switch (m_lineType) {
        case LineType::LINES:
            estimatedNumberOfQuads = numVertices / 2;
            break;
        case LineType::LINE_LOOP:
            estimatedNumberOfQuads = numVertices * 2;
            break;
        case LineType::LINE_STRIP:
            estimatedNumberOfQuads = (numVertices - 1) * 2;
            break;
    }
    
    m_primitive = NULL;
    switch (m_colorType) {
        case ColorType::BYTE_RGBA_PER_VERTEX:
        case ColorType::BYTE_RGBA_SOLID:
            m_primitiveByteColor.reset(GraphicsPrimitive::newPrimitiveV3fC4ub(GraphicsPrimitive::PrimitiveType::OPENGL_QUADS));
            m_primitiveByteColor->reserveForNumberOfVertices(estimatedNumberOfQuads * 4);
            m_primitive = m_primitiveByteColor.get();
            break;
        case ColorType::FLOAT_RGBA_PER_VERTEX:
        case ColorType::FLOAT_RGBA_SOLID:
            m_primitiveFloatColor.reset(GraphicsPrimitive::newPrimitiveV3fC4f(GraphicsPrimitive::PrimitiveType::OPENGL_QUADS));
            m_primitiveFloatColor->reserveForNumberOfVertices(estimatedNumberOfQuads * 4);
            m_primitive = m_primitiveFloatColor.get();
            break;
    }
    CaretAssert(m_primitive);

    m_primitive->setUsageType(GraphicsPrimitive::UsageType::MODIFIED_ONCE_DRAWN_FEW_TIMES);
    
    const int32_t numVerticesMinusOne = numVertices - 1;

    std::vector<bool> restartVertexFlags(numVertices, false);
    for (auto index : m_vertexPrimitiveRestartIndices) {
        restartVertexFlags[index] = true;
    }
    
    /*
     * Restart is disabled because it removes vertices that break joins
     * Might want to draw all quads and use alpha=0 for quads that
     * should not be joined.
     */
    const bool restartEnabledFlag = true;
    
    switch (m_lineType) {
        case LineType::LINES:
        {
            const int32_t numLineSegments = (numVertices / 2);
            for (int32_t i = 0; i < numLineSegments; i++) {
                const int32_t i2 = i * 2;
                createQuadFromWindowVertices(i2, i2 + 1);
            }
        }
            break;
        case LineType::LINE_LOOP:
            for (int32_t i = 0; i < numVerticesMinusOne; i++) {
                CaretAssertVectorIndex(restartVertexFlags, i + 1);
                if (restartVertexFlags[i + 1]) {
                    if (restartEnabledFlag) continue;
                }
                createQuadFromWindowVertices(i, i + 1);
            }
            createQuadFromWindowVertices(numVerticesMinusOne, 0);
            break;
        case LineType::LINE_STRIP:
            for (int32_t i = 0; i < numVerticesMinusOne; i++) {
                CaretAssertVectorIndex(restartVertexFlags, i + 1);
                if (restartVertexFlags[i + 1]) {
                    if (restartEnabledFlag) continue;
                }
                createQuadFromWindowVertices(i, i + 1);
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

