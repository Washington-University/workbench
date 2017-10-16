
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
    errorMessageOut.clear();
    
    GLfloat projectionArray[16];
    glGetFloatv(GL_PROJECTION_MATRIX,
                projectionArray);
    
    GLfloat modelviewArray[16];
    glGetFloatv(GL_MODELVIEW_MATRIX,
                modelviewArray);
    
    GLint viewportArray[4];
    glGetIntegerv(GL_VIEWPORT,
                  viewportArray);
    
    Matrix4x4 matrixToViewport;
    
    
    const std::vector<float>& xyz = m_inputPrimitive->getFloatXYZ();
    
    const int32_t numPointsMinusOne = static_cast<int32_t>(xyz.size() - 1);
    for (int32_t i = 0; i < numPointsMinusOne; i++) {
        const int32_t i3 = i * 3;
        convertLineToRectangle(&xyz[i3],
                               &xyz[i3 + 3]);
        
    }
    return false;
}

void
GraphicsOpenGLLineDrawing::convertLineToRectangle(const float* startXYZ,
                                                  const float* endXYZ)
{
    float x1 = startXYZ[0];
    float y1 = startXYZ[1];
    float x2 = endXYZ[0];
    float y2 = endXYZ[1];
    
    float perpendicularVector[3] = {
        y2 - y1,
        x2 - x1,
        0.0f
    };
    
    const float halfThickness = m_lineThicknessPixels / 2.0f;
    MathFunctions::normalizeVector(perpendicularVector);
    const float halfX = perpendicularVector[0] * halfThickness;
    const float halfY = perpendicularVector[1] * halfThickness;
    
    float rectangleXY[8] = {
        x1 - halfX,
        y1 - halfY,
        x1 + halfX,
        y1 + halfY,
        x2 + halfX,
        y2 + halfY,
        x2 - halfX,
        x2 - halfY
    };
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

