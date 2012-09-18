

/*
 *  Copyright 1995-2002 Washington University School of Medicine
 *
 *  http://brainmap.wustl.edu
 *
 *  This file is part of CARET.
 *
 *  CARET is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  CARET is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with CARET; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */
/*LICENSE_END*/

#include <cstdlib>

#define __BRAIN_OPENGL_DEFINE_H
#include "BrainOpenGL.h"
#undef __BRAIN_OPENGL_DEFINE_H

using namespace caret;

/**
 * Constructor.
 *
 * @param textRenderer
 *   The text renderer is used for text rendering.
 *   This parameter may be NULL in which case no text
 *   rendering is performed.
 */
BrainOpenGL::BrainOpenGL(BrainOpenGLTextRenderInterface* textRenderer)
{
    this->textRenderer = textRenderer;
}

/**
 * Destructor.
 */
BrainOpenGL::~BrainOpenGL()
{
}

/**
 * Get the minimum and maximum values for the size of a point.
 * @param minPointSizeOut
 *    Gets minimum size of point.
 * @param maxPointSizeOut
 *    Gets maximum size of point.
 */
void 
BrainOpenGL::getMinMaxPointSize(float& minPointSizeOut, float& maxPointSizeOut)
{
    minPointSizeOut = BrainOpenGL::minPointSize;
    maxPointSizeOut = BrainOpenGL::maxPointSize;
}

/**
 * Get the minimum and maximum values for the width of a line.
 * @param minLineWidthOut
 *    Gets minimum line width.
 * @param maxLineWidthOut
 *    Gets maximum line width.
 */
void 
BrainOpenGL::getMinMaxLineWidth(float& minLineWidthOut, float& maxLineWidthOut)
{
    minLineWidthOut = BrainOpenGL::minLineWidth;
    maxLineWidthOut = BrainOpenGL::maxLineWidth;
}

/**
 * Set the border being drawn by the user.  If not NULL then 
 * subclasses should draw the border.
 *
 * @param borderBeingDrawn
 *    Pointer to border that is being drawn.
 */
void 
BrainOpenGL::setBorderBeingDrawn(Border* borderBeingDrawn)
{
    this->borderBeingDrawn = borderBeingDrawn;
}

/**
 * @return The runtime version of OpenGL (e.g. 1.0, 2.1, etc.)
 */
float
BrainOpenGL::getRuntimeVersionOfOpenGL()
{
    return BrainOpenGL::versionOfOpenGL;
}


