
/*LICENSE_START*/
/*
 *  Copyright (C) 2015 Washington University School of Medicine
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

#define __BRAIN_OPEN_G_L_TEXT_RENDER_INTERFACE_DECLARE__
#include "BrainOpenGLTextRenderInterface.h"
#undef __BRAIN_OPEN_G_L_TEXT_RENDER_INTERFACE_DECLARE__

#include "CaretAssert.h"
using namespace caret;


    
/**
 * \class caret::BrainOpenGLTextRenderInterface 
 * \brief Interface for rendering of text to OpenGL.
 * \ingroup test
 */

/**
 * Constructor.
 */
BrainOpenGLTextRenderInterface::BrainOpenGLTextRenderInterface()
: CaretObject()
{
    
}

/**
 * Destructor.
 */
BrainOpenGLTextRenderInterface::~BrainOpenGLTextRenderInterface()
{
}

/**
 * Get the bounds of text (in pixels) using the given text
 * attributes.
 *
 * See http://ftgl.sourceforge.net/docs/html/metrics.png
 *
 * @param annotationText
 *   Text that is to be drawn.
 * @param viewportX
 *    Viewport X-coordinate.
 * @param viewportY
 *    Viewport Y-coordinate.
 * @param viewportZ
 *    Viewport Z-coordinate.
 * @param viewportHeight
 *    Height of the viewport needed for percentage height text.
 * @param bottomLeftOut
 *    The bottom left corner of the text bounds.
 * @param bottomRightOut
 *    The bottom right corner of the text bounds.
 * @param topRightOut
 *    The top right corner of the text bounds.
 * @param topLeftOut
 *    The top left corner of the text bounds.
 */
void
BrainOpenGLTextRenderInterface::getBoundsForTextAtViewportCoords(const AnnotationText& annotationText,
                                                                 const float viewportX,
                                                                 const float viewportY,
                                                                 const float viewportZ,
                                                                 const float viewportHeight,
                                                                 float bottomLeftOut[3],
                                                                 float bottomRightOut[3],
                                                                 float topRightOut[3],
                                                                 float topLeftOut[3])
{
    double bottomLeft[3];
    double bottomRight[3];
    double topRight[3];
    double topLeft[3];
    
    getBoundsForTextAtViewportCoords(annotationText,
                                     viewportX,
                                     viewportY,
                                     viewportZ,
                                     viewportHeight,
                                     bottomLeft,
                                     bottomRight,
                                     topRight,
                                     topLeft);
    
    for (int32_t i = 0; i < 3; i++) {
        bottomLeftOut[i]  = bottomLeft[i];
        bottomRightOut[i] = bottomRight[i];
        topRightOut[i]    = topRight[i];
        topLeftOut[i]     = topLeft[i];
    }
}

/**
 * Convert point size to pixels.
 * One point is 1/72 inch.
 *
 * @param pointSize
 *     The point size.
 * @return 
 *     The size in pixels.
 */
float
BrainOpenGLTextRenderInterface::pointSizeToPixels(const float pointSize)
{
    const float inches = pointSize / 72.0;
    float pixels = inches * s_pixelsPerInch;
    
    return pixels;
}

/**
 * Convert pixels to point size.
 * One point is 1/72 inch.
 *
 * @param pixels
 *     The pixel size.
 * @return
 *     The point size.
 */
float
BrainOpenGLTextRenderInterface::pixelsToPointSize(const float pixels)
{
    const float inches = pixels / s_pixelsPerInch;
    const float pointSize = 72.0 * inches;
    
    return pointSize;
}

/**
 * @return The pixels per inch (PPI).
 */
float
BrainOpenGLTextRenderInterface::getPixelsPerInch()
{
    return s_pixelsPerInch;
}

/**
 * Set the pixels per inch.
 *
 * @param pixelsPerInch
 *     The new value for pixels per inch.
 */
void
BrainOpenGLTextRenderInterface::setPixelsPerInch(const float pixelsPerInch)
{
    s_pixelsPerInch = pixelsPerInch;
}


