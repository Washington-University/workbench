
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
 * Constructor.
 *
 * @param primitive
 *     Line primitive converted to rectangles.
 * @param lineThicknessPixels
 *     Thickness of line in pixels.
 */
GraphicsOpenGLLineDrawing::GraphicsOpenGLLineDrawing(const GraphicsPrimitive* primitive,
                                                     const float lineThicknessPixels)
: CaretObject(),
m_inputPrimitive(primitive),
m_lineThicknessPixels(lineThicknessPixels)
{
    
}

/**
 * Destructor.
 */
GraphicsOpenGLLineDrawing::~GraphicsOpenGLLineDrawing()
{
}

/**
 * Run to convert the lines to rectangles.
 * 
 * @param errorMessageOut
 *     Upon exit contains error information.
 * @return 
 *     True if successful, otherwise false.
 */
bool
GraphicsOpenGLLineDrawing::run(AString& errorMessageOut)
{
    /*
     * Save OpenGL state
     */
    glPushAttrib(GL_LIGHTING_BIT | GL_ENABLE_BIT);
    glDisable(GL_LIGHTING);
    glDisable(GL_COLOR_MATERIAL);
    GLint savedPolygonMode[2]; // front and back
    glGetIntegerv(GL_POLYGON_MODE, savedPolygonMode);
    
    GLint savedViewport[4];
    glGetIntegerv(GL_VIEWPORT,
                  savedViewport);
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    
    errorMessageOut.clear();
    
    convertPointsToWindowCoordinates();
    
    convertLineSegmentsToQuads();
    
    drawQuads();

    /*
     * Restore OpenGL state
     */
    glPolygonMode(GL_FRONT, savedPolygonMode[0]);
    glPolygonMode(GL_BACK, savedPolygonMode[1]);
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    glViewport(savedViewport[0], savedViewport[1], savedViewport[2], savedViewport[3]);
    
    glPopAttrib();
    
    return false;
}

/**
 * Convert the points from model to window coordinates
 */
void
GraphicsOpenGLLineDrawing::convertPointsToWindowCoordinates()
{
    GLdouble depthRange[2];
    glGetDoublev(GL_DEPTH_RANGE,
                 depthRange);
    if (m_debugFlag) std::cout << "Depth Range: " << depthRange[0] << ", " << depthRange[1] << std::endl;
    
//    {
//        glDepthRange(-5.0, 5.0);
//        GLfloat depthRangeTest[2];
//        glGetFloatv(GL_DEPTH_RANGE, depthRangeTest);
//        std::cout << "Setting depth range to -5,5 opengl uses " << depthRangeTest[0] << " " << depthRangeTest[1] << std::endl;
//    }
//    
    GLdouble projectionArray[16];
    glGetDoublev(GL_PROJECTION_MATRIX,
                projectionArray);
    Matrix4x4 projectionMatrix;
    projectionMatrix.setMatrixFromOpenGL(projectionArray);
    
    GLdouble modelviewArray[16];
    glGetDoublev(GL_MODELVIEW_MATRIX,
                modelviewArray);
    Matrix4x4 modelviewMatrix;
    modelviewMatrix.setMatrixFromOpenGL(modelviewArray);
    
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT,
                  viewport);
    
    Matrix4x4 projectionTimesModelMatrix(projectionMatrix);
    projectionTimesModelMatrix.postmultiply(modelviewMatrix);
    
    const std::vector<float>& xyz = m_inputPrimitive->getFloatXYZ();
    
    m_windowLineSegmentsXYZ.reserve(xyz.size());
    
    const int32_t numPoints = static_cast<int32_t>(xyz.size() / 3);
    for (int32_t i = 0; i < numPoints; i++) {
        const int32_t i3 = i * 3;
        
        /*
         * Convert model coordinate to a window coordinate
         * as described by the documentation from gluProject().
         */
        float xyzw[4] = { xyz[i3], xyz[i3+1], xyz[i3+2], 1.0f };
        projectionTimesModelMatrix.multiplyPoint4(xyzw);
        const float windowX = viewport[0] + (viewport[2] * ((xyzw[0] + 1.0f) / 2.0f));
        const float windowY = viewport[1] + (viewport[3] * ((xyzw[1] + 1.0f) / 2.0f));
        float windowZ = (xyzw[2] + 1.0f) / 2.0f;
        
        /*
         * From OpenGL Programming Guide 3rd Ed, p 133:
         *
         * If the near value is 1.0 and the far value is 3.0,
         * objects must have z-coordinates between -1.0 and -3.0 in order to be visible.
         * So, negate the Z-value to be negative.
         */
        windowZ = -windowZ;

        m_windowLineSegmentsXYZ.push_back(windowX);
        m_windowLineSegmentsXYZ.push_back(windowY);
        m_windowLineSegmentsXYZ.push_back(windowZ);
        
        if (m_debugFlag) {
            GLdouble winX, winY, winZ;
            gluProject(xyz[i3], xyz[i3+1], xyz[i3+2],
                       modelviewArray, projectionArray, viewport,
                       &winX, &winY, &winZ);
            std::cout << "Input: " << xyz[i3] << ", " << xyz[i3+1] << ", " << xyz[i3+2] << std::endl;
            std::cout << "   Pt " << i << ": "
            << windowX << ", " << windowY << ", " << windowZ << "  w=" << xyzw[3] << std::endl;
            std::cout << "   gluProject " << winX << ", " << winY << ", " << winZ << std::endl;
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
    const int32_t numPoints = static_cast<int32_t>(m_windowLineSegmentsXYZ.size() / 3);
    m_windowQuadsXYZ.reserve(numPoints * 6);  // 2 points in line to 4 points in quad
    
    const int32_t numPointsMinusOne = static_cast<int32_t>((m_windowLineSegmentsXYZ.size() / 3) - 1);
    for (int32_t i = 0; i < numPointsMinusOne; i++) {
        const int32_t i3 = i * 3;
        
        float x1 = m_windowLineSegmentsXYZ[i3];
        float y1 = m_windowLineSegmentsXYZ[i3 + 1];
        float z1 = m_windowLineSegmentsXYZ[i3 + 2];
        float x2 = m_windowLineSegmentsXYZ[i3 + 3];
        float y2 = m_windowLineSegmentsXYZ[i3 + 4];
        float z2 = m_windowLineSegmentsXYZ[i3 + 5];
        
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
    }
    
}

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
    glPolygonMode(GL_BACK, GL_LINE);
    glColor3f(0.0, 1.0, 0.0);
    glLineWidth(1.0);
    glBegin(GL_QUADS);  //GL_QUADS)
    for (int32_t j = 0; j < 4; j++) {
        glNormal3f(0.0, 0.0, -1.0);
        glVertex3fv(&m_windowQuadsXYZ[j*3]);
    }
    glEnd();
    
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

