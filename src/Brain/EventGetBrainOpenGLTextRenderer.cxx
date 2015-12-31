
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

#define __EVENT_GET_BRAIN_OPEN_G_L_TEXT_RENDERER_DECLARE__
#include "EventGetBrainOpenGLTextRenderer.h"
#undef __EVENT_GET_BRAIN_OPEN_G_L_TEXT_RENDERER_DECLARE__

#include "CaretAssert.h"
#include "EventTypeEnum.h"

using namespace caret;


    
/**
 * \class caret::EventGetBrainOpenGLTextRenderer 
 * \brief Event to get a text renderer for a window
 * \ingroup Brain
 */

/**
 * Constructor.
 */
EventGetBrainOpenGLTextRenderer::EventGetBrainOpenGLTextRenderer(const int32_t windowIndex)
: Event(EventTypeEnum::EVENT_GET_TEXT_RENDERER_FOR_WINDOW),
m_windowIndex(windowIndex)
{
    m_textRenderer = NULL;
}

/**
 * Destructor.
 */
EventGetBrainOpenGLTextRenderer::~EventGetBrainOpenGLTextRenderer()
{
}

/**
 * @return Index of the window.
 */
int32_t
EventGetBrainOpenGLTextRenderer::getWindowIndex() const
{
    return m_windowIndex;
}

/**
 * @return The text renderer found for the window.
 * (NULL if not found).
 */
BrainOpenGLTextRenderInterface*
EventGetBrainOpenGLTextRenderer::getTextRenderer() const
{
    return m_textRenderer;
}

/**
 * Set the text renderer.
 *
 * @param textRenderer
 *     The text renderer.
 */
void
EventGetBrainOpenGLTextRenderer::setTextRenderer(BrainOpenGLTextRenderInterface* textRenderer)
{
    m_textRenderer = textRenderer;
}


