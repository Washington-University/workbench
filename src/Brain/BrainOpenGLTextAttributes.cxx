
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

#define __BRAIN_OPEN_G_L_TEXT_ATTRIBUTES_DECLARE__
#include "BrainOpenGLTextAttributes.h"
#undef __BRAIN_OPEN_G_L_TEXT_ATTRIBUTES_DECLARE__

#include "CaretAssert.h"
using namespace caret;


    
/**
 * \class caret::BrainOpenGLTextAttributes 
 * \brief Attributes for drawing text.
 * \ingroup Brain
 */

/**
 * Constructor that creates the attributes as: 
 * (1) font height = 14, 
 * (2) horizontal alighnment at left edge of first character
 * (3) vertical alignment at center of characters
 * (4) characters oriented left-to-right
 * (5) style = normal
 * (6) foreground color = white with alpha = 1.0
 * (7) background color = black with alpha = 0.0
 *        Background color is used when its alpha component is
 *        greater than zero.
 */
BrainOpenGLTextAttributes::BrainOpenGLTextAttributes()
: CaretObject()
{
    m_fontHeight          = 14;
    m_horizontalAlignment = X_LEFT;
    m_verticalAlignment   = Y_CENTER;
    m_orientation         = LEFT_TO_RIGHT;
    m_style               = NORMAL;
    
    m_colorForeground[0] = 1.0;
    m_colorForeground[1] = 1.0;
    m_colorForeground[2] = 1.0;
    m_colorForeground[3] = 1.0;
    
    m_colorBackground[0] = 0.0;
    m_colorBackground[1] = 0.0;
    m_colorBackground[2] = 0.0;
    m_colorBackground[3] = 0.0;
}

/**
 * Destructor.
 */
BrainOpenGLTextAttributes::~BrainOpenGLTextAttributes()
{
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
BrainOpenGLTextAttributes::toString() const
{
    return "BrainOpenGLTextAttributes";
}

/**
 * Set the foreground color.
 *
 * @param rgbaOut
 *    RGBA components (red, green, blue, alpha) each of which ranges [0.0, 1.0].
 */
void
BrainOpenGLTextAttributes::getForegroundColor(float rgbaOut[4]) const
{
    rgbaOut[0] = m_colorForeground[0];
    rgbaOut[1] = m_colorForeground[1];
    rgbaOut[2] = m_colorForeground[2];
    rgbaOut[3] = m_colorForeground[3];
}

/**
 * Set the foreground color with floats.
 *
 * @param rgba
 *    RGBA components (red, green, blue, alpha) each of which ranges [0.0, 1.0].
 */
void
BrainOpenGLTextAttributes::setForegroundColor(const float rgba[4])
{
    m_colorForeground[0] = rgba[0];
    m_colorForeground[1] = rgba[1];
    m_colorForeground[2] = rgba[2];
    m_colorForeground[3] = rgba[3];
}

/**
 * Set the foreground color with unsigned bytes.
 *
 * @param rgba
 *    RGBA components (red, green, blue, alpha) each of which ranges [0, 255].
 */
void
BrainOpenGLTextAttributes::setForegroundColor(const uint8_t rgba[4])
{
    m_colorForeground[0] = rgba[0] / 255.0;
    m_colorForeground[1] = rgba[1] / 255.0;
    m_colorForeground[2] = rgba[2] / 255.0;
    m_colorForeground[3] = rgba[3] / 255.0;
}

/**
 * Set the background color.
 *
 * @param rgbaOut
 *    RGBA components (red, green, blue, alpha) each of which ranges [0.0, 1.0].
 */
void
BrainOpenGLTextAttributes::getBackgroundColor(float rgbaOut[4]) const
{
    rgbaOut[0] = m_colorBackground[0];
    rgbaOut[1] = m_colorBackground[1];
    rgbaOut[2] = m_colorBackground[2];
    rgbaOut[3] = m_colorBackground[3];
}

/**
 * Set the background color with floats.
 * The background color is applied only when its alpha component is greater than zero.
 *
 * @param rgba
 *    RGBA components (red, green, blue, alpha) each of which ranges [0.0, 1.0].
 */
void
BrainOpenGLTextAttributes::setBackgroundColor(const float rgba[4])
{
    m_colorBackground[0] = rgba[0];
    m_colorBackground[1] = rgba[1];
    m_colorBackground[2] = rgba[2];
    m_colorBackground[3] = rgba[3];
}

/**
 * Set the background color with bytes.
 * The background color is applied only when its alpha component is greater than zero.
 *
 * @param rgba
 *    RGBA components (red, green, blue, alpha) each of which ranges [0, 255].
 */
void
BrainOpenGLTextAttributes::setBackgroundColor(const uint8_t rgba[4])
{
    m_colorBackground[0] = rgba[0] / 255.0;
    m_colorBackground[1] = rgba[1] / 255.0;
    m_colorBackground[2] = rgba[2] / 255.0;
    m_colorBackground[3] = rgba[3] / 255.0;
}

