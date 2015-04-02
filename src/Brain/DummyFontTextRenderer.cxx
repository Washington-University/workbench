
/*LICENSE_START*/
/*
 *  Copyright (C) 2014 Washington University School of Medicine
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

#define __DUMMY_FONT_TEXT_RENDERER_DECLARE__
#include "DummyFontTextRenderer.h"
#undef __DUMMY_FONT_TEXT_RENDERER_DECLARE__

#include "CaretAssert.h"
using namespace caret;


    
/**
 * \class caret::DummyFontTextRenderer 
 * \brief A dummy font text renderer used when no valid text renderer is available.
 * \ingroup Brain
 *
 * This "dummy" text renderer is used when a valid text renderer (Qt, FTGL) is
 * not available.  Without this dummy text renderer, any graphics code that
 * draws text would need to check that the text renderer is valid.  This
 * dummy text renderer does nothing other than prevent the software from 
 * crashing.
 */

/**
 * Constructor.
 */
DummyFontTextRenderer::DummyFontTextRenderer()
: BrainOpenGLTextRenderInterface()
{
    
}

/**
 * Destructor.
 */
DummyFontTextRenderer::~DummyFontTextRenderer()
{
}

/**
 * @return The font system is valid.
 */
bool
DummyFontTextRenderer::isValid() const
{
    return true;
}

/**
 * Draw text at the given VIEWPORT coordinates.
 *
 * The origin (0, 0) is at the bottom left corner
 * of the viewport and (viewport-width, viewport-height)
 * is at the top right corner of the viewport.
 *
 * @param viewport
 *   The current viewport.
 * @param windowX
 *   X-coordinate of the text.
 * @param windowY
 *   Y-coordinate of the text.
 * @param annotationText
 *   Text and attributes for text drawing.
 */
void
DummyFontTextRenderer::drawTextAtViewportCoords(const int* /*viewport[4]*/,
                                             const double /*windowX*/,
                                             const double /*windowY*/,
                                              const AnnotationText& /*annotationText*/)
{
}

/**
 * Draw vertical text at the given window coordinates.
 *
 * @param viewport
 *   The current viewport.
 * @param windowX
 *   X-coordinate in the window of first text character
 *   using the 'alignment'
 * @param windowY
 *   Y-coordinate in the window at which bottom of text is placed.
 * @param annotationText
 *   Text and attributes for text drawing.
 */
void
DummyFontTextRenderer::drawVerticalTextAtWindowCoords(const int* /*viewport[4]*/,
                                                      const double /*windowX*/,
                                                      const double /*windowY*/,
                                                      const AnnotationText& /*annotationText*/)
{
}

/**
 * Get the bounds of text (in pixels) using the given text
 * attributes.
 *
 * See http://ftgl.sourceforge.net/docs/html/metrics.png
 *
 * @param annotationText
 *   Text and attributes for text drawing.
 * @param xMinOut
 *    Minimum X of text.
 * @param xMaxOut
 *    Maximum X of text.
 * @param yMinOut
 *    Minimum Y of text.
 * @param yMaxOut
 *    Maximum Y of text.
 */
void
DummyFontTextRenderer::getTextBoundsInPixels(const AnnotationText& /*annotationText*/,
                                             double& /*xMinOut*/,
                                             double& /*xMaxOut*/,
                                             double& /*yMinOut*/,
                                             double& /*yMaxOut*/)
{
}

/**
 * Draw text at the given model coordinates.
 *
 * @param modelX
 *   X-coordinate in model space of first text character
 * @param modelY
 *   Y-coordinate in model space.
 * @param modelZ
 *   Z-coordinate in model space.
 * @param annotationText
 *   Text and attributes for text drawing.
 */
void
DummyFontTextRenderer::drawTextAtModelCoords(const double /*modelX*/,
                                            const double /*modelY*/,
                                            const double /*modelZ*/,
                                             const AnnotationText& /*annotationText*/)
{
}

/**
 * @return Name of the text renderer.
 */
AString
DummyFontTextRenderer::getName() const
{
    return "Dummy (No OpenGL font system)";
}

