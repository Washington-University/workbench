
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

#define __BRAIN_OPEN_G_L_VIEWPORT_CONTENT_DECLARE__
#include "BrainOpenGLViewportContent.h"
#undef __BRAIN_OPEN_G_L_VIEWPORT_CONTENT_DECLARE__

using namespace caret;


    
/**
 * \class BrainOpenGLViewportContent 
 * \brief Dimensions and model for a viewport in the graphics window.
 *
 * Dimensions and model for a viewport in the graphics window.
 */
/**
 * Constructor.
 * @param windowViewport
 *    Viewport of WINDOW in which drawing takes place.
 * @param modelViewport
 *    Viewport for MODEL in which drawing takes place.
 * @param highlightTabFlag
 *    True indicates that the tab is highlighted (used in 
 *    Tile Tabs mode so user knows graphics region corresponding
 *    to the tab that was recently selected).
 * @param brain
 *    Brain that is the source of the data.
 * @param browserTabContent
 *    Tab's content that is being drawn.
 */
BrainOpenGLViewportContent::BrainOpenGLViewportContent(const int windowViewport[4],
                                                       const int modelViewport[4],
                                                       const bool highlightTabFlag,
                                                       Brain* brain,
                                                       BrowserTabContent* browserTabContent)
: CaretObject()
{
    m_windowViewport[0] = windowViewport[0];
    m_windowViewport[1] = windowViewport[1];
    m_windowViewport[2] = windowViewport[2];
    m_windowViewport[3] = windowViewport[3];
    
    m_modelViewport[0] = modelViewport[0];
    m_modelViewport[1] = modelViewport[1];
    m_modelViewport[2] = modelViewport[2];
    m_modelViewport[3] = modelViewport[3];
    
    m_brain = brain;
    
    m_browserTabContent = browserTabContent;
    
    m_highlightTab = highlightTabFlag;
}

/**
 * Destructor.
 */
BrainOpenGLViewportContent::~BrainOpenGLViewportContent()
{
    
}

/**
 * @return  True indicates that the tab is highlighted (used in
 *    Tile Tabs mode so user knows graphics region corresponding
 *    to the tab that was recently selected).
 */
bool
BrainOpenGLViewportContent::isTabHighlighted() const
{
    return m_highlightTab;
}

/**
 * Get the viewport for drawing the model.
 * @param modelViewport
 *    Output into which model viewport dimensions are loaded.
 */
void
BrainOpenGLViewportContent::getModelViewport(int modelViewport[4]) const
{
    modelViewport[0] = m_modelViewport[0];
    modelViewport[1] = m_modelViewport[1];
    modelViewport[2] = m_modelViewport[2];
    modelViewport[3] = m_modelViewport[3];
}

/**
 * @return Pointer to the viewport for drawing the model.
 */
const int*
BrainOpenGLViewportContent::getModelViewport() const
{
    return m_modelViewport;
}

/**
 * @return Pointer to the viewport for the window.
 */
const int*
BrainOpenGLViewportContent::getWindowViewport() const
{
    return m_windowViewport;
}

Brain*
BrainOpenGLViewportContent::getBrain()
{
    return m_brain;
}

/**
 * @return Pointer to tab content in viewport.
 */
BrowserTabContent* 
BrainOpenGLViewportContent::getBrowserTabContent()
{
    return m_browserTabContent;
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
BrainOpenGLViewportContent::toString() const
{
    return "BrainOpenGLViewportContent";
}
