
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
 * font name = SANS_SERIF
 * font height = 14,
 * bold disabled
 * italic disabled
 * underline disabled.
 * horizontal alighnment at left edge of first character
 * vertical alignment at center of characters
 * characters oriented left-to-right
 * foreground color = white with alpha = 1.0
 * background color = black with alpha = 0.0
 *     Background color is used when its alpha component is
 *     greater than zero.
 */
BrainOpenGLTextAttributes::BrainOpenGLTextAttributes()
: CaretObject()
{
    m_fontName            = SANS_SERIF;
    m_fontHeight          = 14;
    m_boldEnabled         = false;
    m_italicEnabled       = false;
    m_underlineEnabled    = false;
    m_horizontalAlignment = X_LEFT;
    m_verticalAlignment   = Y_CENTER;
    m_orientation         = LEFT_TO_RIGHT;
    
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
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
BrainOpenGLTextAttributes::BrainOpenGLTextAttributes(const BrainOpenGLTextAttributes& obj)
: CaretObject(obj)
{
    this->copyHelperBrainOpenGLTextAttributes(obj);
}

/**
 * Assignment operator.
 * @param obj
 *    Data copied from obj to this.
 * @return
 *    Reference to this object.
 */
BrainOpenGLTextAttributes&
BrainOpenGLTextAttributes::operator=(const BrainOpenGLTextAttributes& obj)
{
    if (this != &obj) {
        CaretObject::operator=(obj);
        this->copyHelperBrainOpenGLTextAttributes(obj);
    }
    return *this;
}

/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void
BrainOpenGLTextAttributes::copyHelperBrainOpenGLTextAttributes(const BrainOpenGLTextAttributes& obj)
{
    m_fontName            = obj.m_fontName;
    m_fontHeight          = obj.m_fontHeight;
    m_boldEnabled         = obj.m_boldEnabled;
    m_italicEnabled       = obj.m_italicEnabled;
    m_underlineEnabled    = obj.m_underlineEnabled;
    m_horizontalAlignment = obj.m_horizontalAlignment;
    m_verticalAlignment   = obj.m_verticalAlignment;
    m_orientation         = obj.m_orientation;
    
    m_colorForeground[0] = obj.m_colorForeground[0];
    m_colorForeground[1] = obj.m_colorForeground[1];
    m_colorForeground[2] = obj.m_colorForeground[2];
    m_colorForeground[3] = obj.m_colorForeground[3];
    
    m_colorBackground[0] = obj.m_colorBackground[0];
    m_colorBackground[1] = obj.m_colorBackground[1];
    m_colorBackground[2] = obj.m_colorBackground[2];
    m_colorBackground[3] = obj.m_colorBackground[3];
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
 * @return An encoded name that contains the
 * name of the font, the font height, and the
 * font style used by font rendering to provide
 * a name for cached fonts.
 */
AString
BrainOpenGLTextAttributes::getFontRenderingEncodedName() const
{
    AString encodedName;
    encodedName.reserve(50);
    
    switch (m_fontName) {
        case SANS_SERIF:
            encodedName.append("SansSerif");
            break;
        case SANS_SERIF_MONO:
            encodedName.append("SansSerifMono");
            break;
        case SERIF:
            encodedName.append("Serif");
            break;
    }
    
    encodedName.append(AString::number(m_fontHeight));
    
    if (m_boldEnabled) {
        encodedName.append("B");
    }
    
    if (m_italicEnabled) {
        encodedName.append("I");
    }
    
    if (m_underlineEnabled) {
        encodedName.append("U");
    }
    
    return encodedName;
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

