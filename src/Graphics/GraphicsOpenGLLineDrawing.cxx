
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
#include "CaretOpenGLInclude.h"
#include "GraphicsPrimitive.h"
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
 * @param xyz
 *     The XYZ vertices.
 * @param rgba
 *     The RGBA coloring.
 * @param lineThicknessPixels
 *     Thickness of lines in pixels.
 * @param colorType
 *     Type of color (solid or per-vertex)
 * @param lineType
 *     Type of lines drawn.
 */
GraphicsOpenGLLineDrawing::GraphicsOpenGLLineDrawing(const std::vector<float>& xyz,
                          const std::vector<float>& rgba,
                          const float lineThicknessPixels,
                          const ColorType colorType,
                          const LineType lineType)
: m_inputPrimitive(NULL),
m_inputXYZ(xyz),
m_inputRGBA(rgba),
m_lineThicknessPixels(lineThicknessPixels),
m_colorType(colorType),
m_lineType(lineType)
{
    
}


/**
 * Draw lines where each pair of vertices is drawn as an independent
 * line segment.  Same as OpenGL GL_LINES mode with glBegin().
 *
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
GraphicsOpenGLLineDrawing::drawLinesSolidColor(const std::vector<float>& xyz,
                                               const float rgba[4],
                                               const float lineThicknessPixels)
{
    std::vector<float> rgbaVector = { rgba[0], rgba[1], rgba[2], rgba[3] };
    
    return drawLinesPrivate(xyz,
                            rgbaVector,
                            lineThicknessPixels,
                            ColorType::RGBA_SOLID,
                            LineType::LINES);
    
//    const int32_t numPoints = static_cast<int32_t>(xyz.size() / 3);
//    if (numPoints < 2) {
//        CaretLogWarning("Invalid number of points="
//                        + AString::number(numPoints)
//                        + " for line drawing.  Must be at least 2.");
//        return false;
//    }
//    
//    std::vector<float> rgbaVector = { rgba[0], rgba[1], rgba[2], rgba[3] };
//    
//    GraphicsOpenGLLineDrawing lineDrawing(xyz,
//                                          rgbaVector,
//                                          lineThicknessPixels,
//                                          ColorType::RGBA_SOLID,
//                                          LineType::LINES);
//    lineDrawing.performDrawing();
//    
//    return true;
}

/**
 * Draw a connect line strip.  Same as OpenGL GL_LINE_STRIP mode with glBegin().
 *
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
GraphicsOpenGLLineDrawing::drawLineStripSolidColor(const std::vector<float>& xyz,
                                               const float rgba[4],
                                               const float lineThicknessPixels)
{
    std::vector<float> rgbaVector = { rgba[0], rgba[1], rgba[2], rgba[3] };
    
    return drawLinesPrivate(xyz,
                            rgbaVector,
                            lineThicknessPixels,
                            ColorType::RGBA_SOLID,
                            LineType::LINE_STRIP);
    
}

/**
 * Draw a connect line loop (last is connected to first).  Same as OpenGL GL_LINE_LOOP mode with glBegin().
 *
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
GraphicsOpenGLLineDrawing::drawLineLoopSolidColor(const std::vector<float>& xyz,
                                                   const float rgba[4],
                                                   const float lineThicknessPixels)
{
    std::vector<float> rgbaVector = { rgba[0], rgba[1], rgba[2], rgba[3] };
    
    return drawLinesPrivate(xyz,
                            rgbaVector,
                            lineThicknessPixels,
                            ColorType::RGBA_SOLID,
                            LineType::LINE_LOOP);
    
}

/**
 * Draw lines where each pair of vertices is drawn as an independent
 * line segment.  Same as OpenGL GL_LINES mode with glBegin().
 *
 * @param xyz
 *     The vertices.
 * @param rgba
 *     RGBA color for the lines with values ranging [0.0, 1.0]
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
GraphicsOpenGLLineDrawing::drawLinesPrivate(const std::vector<float>& xyz,
                                            const std::vector<float>& rgba,
                                            const float lineThicknessPixels,
                                            const ColorType colorType,
                                            const LineType lineType)
{
    GraphicsOpenGLLineDrawing lineDrawing(xyz,
                                          rgba,
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
    
    const int32_t numRGBA = static_cast<int32_t>(m_inputRGBA.size());
    switch (m_colorType) {
        case ColorType::RGBA_PER_VERTEX:
        {
            const int32_t numRgbaVertices = numRGBA / 4;
            if (numVertices != numRgbaVertices) {
                CaretLogSevere("Mismatch in vertices and coloring.  There are "
                               + AString::number(numVertices)
                               + " vertices but have coloring for "
                               + AString::number(numRgbaVertices)
                               + " vertices");
                return false;
            }
        }
            break;
        case ColorType::RGBA_SOLID:
            if (numRGBA < 4) {
                CaretLogSevere("Must have at last 4 color components for solid color line drawing"
                               "Number of color components is "
                               + AString::number(numRGBA));
                return false;
            }
            break;
    }
    
    saveOpenGLState();
    
    createProjectionMatrix();
    
    createWindowCoordinatesFromVertices();
    
    convertLineSegmentsToQuads();
    
    drawQuads();
    
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
    m_projectionMatrix.postmultiply(modelviewMatrix);
}


/**
 * Create window coordinates from the input vertices.
 */
void
GraphicsOpenGLLineDrawing::createWindowCoordinatesFromVertices()
{
    GLdouble depthRange[2];
    glGetDoublev(GL_DEPTH_RANGE,
                 depthRange);

    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT,
                  viewport);
    
    const int32_t numPoints = static_cast<int32_t>(m_inputXYZ.size() / 3);
    m_vertexWindowXYZ.reserve(numPoints * 3);
    
    for (int32_t i = 0; i < numPoints; i++) {
        const int32_t i3 = i * 3;
        
        /*
         * Convert model coordinate to a window coordinate
         * as described by the documentation from gluProject().
         */
        float windowXYZ[3];
        convertFromModelToWindowCoordinate(&m_inputXYZ[i3], windowXYZ);
        m_vertexWindowXYZ.push_back(windowXYZ[0]);
        m_vertexWindowXYZ.push_back(windowXYZ[1]);
        m_vertexWindowXYZ.push_back(windowXYZ[2]);
        
//        if (m_debugFlag) {
//            GLdouble winX, winY, winZ;
//            gluProject(xyz[i3], xyz[i3+1], xyz[i3+2],
//                       modelviewArray, projectionArray, viewport,
//                       &winX, &winY, &winZ);
//            std::cout << "Input: " << xyz[i3] << ", " << xyz[i3+1] << ", " << xyz[i3+2] << std::endl;
//            std::cout << "   Pt " << i << ": "
//            << windowX << ", " << windowY << ", " << windowZ << "  w=" << xyzw[3] << std::endl;
//            std::cout << "   gluProject " << winX << ", " << winY << ", " << winZ << std::endl;
//        }
    }
    
    CaretAssert(m_inputXYZ.size() == m_vertexWindowXYZ.size());
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
     * Vector from start to end on line segment in 2D coordinates
     */
    float startToEndVector[3] = {
        x2 - x1,
        y2 - y1,
        0.0f
    };
    
    /*
     * Create a perpendicualar vector to the line segment
     */
    MathFunctions::normalizeVector(startToEndVector);
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
    
    float crossProduct[3];
    MathFunctions::crossProduct(startToEndVector, perpendicularVector, crossProduct);
    if (crossProduct[2] < 0.0) {
        m_windowQuadsXYZ.insert(m_windowQuadsXYZ.end(), p1, p1 + 3);
        m_windowQuadsXYZ.insert(m_windowQuadsXYZ.end(), p2, p2 + 3);
        m_windowQuadsXYZ.insert(m_windowQuadsXYZ.end(), p3, p3 + 3);
        m_windowQuadsXYZ.insert(m_windowQuadsXYZ.end(), p4, p4 + 3);
    }
    else {
        std::cout << "Flipped Quad" << std::endl;
        m_windowQuadsXYZ.insert(m_windowQuadsXYZ.end(), p1, p1 + 3);
        m_windowQuadsXYZ.insert(m_windowQuadsXYZ.end(), p4, p4 + 3);
        m_windowQuadsXYZ.insert(m_windowQuadsXYZ.end(), p3, p3 + 3);
        m_windowQuadsXYZ.insert(m_windowQuadsXYZ.end(), p2, p2 + 3);
    }
 
    switch (m_colorType) {
        case ColorType::RGBA_PER_VERTEX:
        {
            CaretAssertVectorIndex(m_inputRGBA, indexOne * 4 * 3);
            const float* rgbaOneOffset = &m_inputRGBA[indexOne * 4];
            m_windowQuadsRGBA.insert(m_windowQuadsRGBA.end(), rgbaOneOffset, rgbaOneOffset + 4);
            m_windowQuadsRGBA.insert(m_windowQuadsRGBA.end(), rgbaOneOffset, rgbaOneOffset + 4);
            
            CaretAssertVectorIndex(m_inputRGBA, indexTwo * 4 * 3);
            const float* rgbaTwoOffset = &m_inputRGBA[indexTwo * 4];
            m_windowQuadsRGBA.insert(m_windowQuadsRGBA.end(), rgbaTwoOffset, rgbaTwoOffset + 4);
            m_windowQuadsRGBA.insert(m_windowQuadsRGBA.end(), rgbaOneOffset, rgbaOneOffset + 4);
        }
            break;
        case ColorType::RGBA_SOLID:
            CaretAssertVectorIndex(m_inputRGBA, 3);
            m_windowQuadsRGBA.insert(m_windowQuadsRGBA.end(), m_inputRGBA.begin(), m_inputRGBA.begin() + 4);
            m_windowQuadsRGBA.insert(m_windowQuadsRGBA.end(), m_inputRGBA.begin(), m_inputRGBA.begin() + 4);
            m_windowQuadsRGBA.insert(m_windowQuadsRGBA.end(), m_inputRGBA.begin(), m_inputRGBA.begin() + 4);
            m_windowQuadsRGBA.insert(m_windowQuadsRGBA.end(), m_inputRGBA.begin(), m_inputRGBA.begin() + 4);
            break;
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
    m_windowQuadsXYZ.reserve(numVertices * 6);  // 2 points in line to 4 points in quad
    
    const int32_t numVerticesMinusOne = numVertices - 1;
    
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
                createQuadFromWindowVertices(i, i + 1);
            }
            createQuadFromWindowVertices(numVerticesMinusOne, 0);
            break;
        case LineType::LINE_STRIP:
            for (int32_t i = 0; i < numVerticesMinusOne; i++) {
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
    const int32_t numVertices = static_cast<int32_t>(m_windowQuadsXYZ.size() / 3);
    const int32_t numRGBA     = static_cast<int32_t>(m_windowQuadsRGBA.size() / 4);
    CaretAssert(numVertices == numRGBA);
    
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
    
    switch (m_colorType) {
        case ColorType::RGBA_PER_VERTEX:
            break;
        case ColorType::RGBA_SOLID:
            glBegin(GL_QUADS);  //GL_QUADS)
            for (int32_t j = 0; j < numVertices; j++) {
                CaretAssertVectorIndex(m_windowQuadsRGBA, j*4 + 3);
                glColor4fv(&m_windowQuadsRGBA[j*4]);
                CaretAssertVectorIndex(m_windowQuadsXYZ, j*3 + 2);
                glVertex3fv(&m_windowQuadsXYZ[j*3]);
            }
            glEnd();
            break;
    }
    
    if (m_debugFlag) {
        std::cout << std::endl << "QUADS: " << AString::fromNumbers(m_windowQuadsXYZ, ",") << std::endl;
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

