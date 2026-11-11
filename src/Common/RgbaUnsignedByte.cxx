/*LICENSE_START*/
/*
 *  Copyright (C) 2026 Washington University School of Medicine
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

#define __RGBA_UNSIGNED_BYTE_DECLARE__
#include "RgbaUnsignedByte.h"
#undef __RGBA_UNSIGNED_BYTE_DECLARE__

#include <algorithm>

#include "CaretAssert.h"

using namespace caret;

/**
 * \class caret::RgbaUnsignedByte
 * \brief Stores a color as four unsigned bytes: red, green, blue, alpha
 *
 * The components are stored contiguously in the order R, G, B, A so that
 * the pointer returned by getRgbaPointer() may be passed directly to
 * APIs that expect an array of four unsigned bytes (e.g. OpenGL).
 */

/**
 * Constructor.  Initializes to black (0, 0, 0, 255).
 */
RgbaUnsignedByte::RgbaUnsignedByte()
{
    m_rgba = reinterpret_cast<uint8_t*>(&m_unsignedIntRGBA);
    setRgba(0, 0, 0, 255);
}

/**
 * Constructor.
 *
 * @param red
 *    Red component.
 * @param green
 *    Green component.
 * @param blue
 *    Blue component.
 * @param alpha
 *    Alpha component.
 */
RgbaUnsignedByte::RgbaUnsignedByte(const uint8_t red,
                                   const uint8_t green,
                                   const uint8_t blue,
                                   const uint8_t alpha)
{
    m_rgba = reinterpret_cast<uint8_t*>(&m_unsignedIntRGBA);
    setRgba(red, green, blue, alpha);
}

/**
 * Constructor.
 *
 * @param rgba
 *    Array of four components: red, green, blue, alpha.
 */
RgbaUnsignedByte::RgbaUnsignedByte(const uint8_t rgba[4])
{
    m_rgba = reinterpret_cast<uint8_t*>(&m_unsignedIntRGBA);
    setRgba(rgba);
}

/**
 * Destructor.
 */
RgbaUnsignedByte::~RgbaUnsignedByte()
{
}

/**
 * Copy constructor.
 *
 * @param obj
 *    Object that is copied.
 */
RgbaUnsignedByte::RgbaUnsignedByte(const RgbaUnsignedByte& obj)
{
    m_rgba = reinterpret_cast<uint8_t*>(&m_unsignedIntRGBA);
    this->copyHelperRgbaUnsignedByte(obj);
}

/**
 * Assignment operator.
 *
 * @param obj
 *    Data copied from obj to this.
 * @return
 *    Reference to this object.
 */
RgbaUnsignedByte&
RgbaUnsignedByte::operator=(const RgbaUnsignedByte& obj)
{
    if (this != &obj) {
        this->copyHelperRgbaUnsignedByte(obj);
    }
    return *this;
}


/**
 * Equality operator.
 *
 * @param obj
 *    Object compared to this.
 * @return
 *    True if all four components are equal, else false.
 */
bool
RgbaUnsignedByte::operator==(const RgbaUnsignedByte& obj) const
{
    if (this == &obj) {
        return true;
    }
    return (m_unsignedIntRGBA == obj.m_unsignedIntRGBA);
}

/**
 * Less than operator.
 *
 * @param obj
 *    Object compared to this.
 * @return
 *    True if all four components are equal, else false.
 */
bool
RgbaUnsignedByte::operator<(const RgbaUnsignedByte& obj) const
{
    if (this == &obj) {
        return true;
    }
    
    if (m_unsignedIntRGBA == obj.m_unsignedIntRGBA) {
        return false;
    }
    
    uint8_t rgbaOne[4], rgbaTwo[4];
    getRgba(rgbaOne);
    obj.getRgba(rgbaTwo);
    
    return std::lexicographical_compare(rgbaOne, rgbaOne + 4,
                                        rgbaTwo, rgbaTwo + 4);
}

/*
 bool RgbaUnsignedByte::operator<(const RgbaUnsignedByte& obj) const
 {
 return std::lexicographical_compare(m_rgba, m_rgba + 4,
 obj.m_rgba, obj.m_rgba + 4);
 }
 */
/**
 * Helper for copy constructor and assignment operator.
 *
 * @param obj
 *    Object that is copied.
 */
void
RgbaUnsignedByte::copyHelperRgbaUnsignedByte(const RgbaUnsignedByte& obj)
{
    m_unsignedIntRGBA = obj.m_unsignedIntRGBA;
}

/**
 * @return Pointer to the four-element RGBA array (red, green, blue, alpha).
 *
 * The pointer is valid only for the lifetime of this object.
 */
const uint8_t*
RgbaUnsignedByte::getRgbaPointer() const
{
    return m_rgba;
}

/**
 * Get the color components into an array.
 *
 * @param rgba
 *    Output array with room for four elements: red, green, blue, alpha.
 */
void
RgbaUnsignedByte::getRgba(uint8_t rgba[4]) const
{
    std::copy(m_rgba, m_rgba + 4, rgba);
}

/**
 * Get the color components.
 *
 * @param red
 *    Output red component.
 * @param green
 *    Output green component.
 * @param blue
 *    Output blue component.
 * @param alpha
 *    Output alpha component.
 */
void
RgbaUnsignedByte::getRgba(uint8_t& red,
                          uint8_t& green,
                          uint8_t& blue,
                          uint8_t& alpha) const
{
    red   = m_rgba[0];
    green = m_rgba[1];
    blue  = m_rgba[2];
    alpha = m_rgba[3];
}

/**
 * Set the color components from an array.
 *
 * @param rgba
 *    Array of four components: red, green, blue, alpha.
 */
void
RgbaUnsignedByte::setRgba(const uint8_t rgba[4])
{
    CaretAssert(rgba);
    std::copy(rgba, rgba + 4, m_rgba);
}

/**
 * Set the color components.
 *
 * @param red
 *    Red component.
 * @param green
 *    Green component.
 * @param blue
 *    Blue component.
 * @param alpha
 *    Alpha component.
 */
void
RgbaUnsignedByte::setRgba(const uint8_t red,
                          const uint8_t green,
                          const uint8_t blue,
                          const uint8_t alpha)
{
    m_rgba[0] = red;
    m_rgba[1] = green;
    m_rgba[2] = blue;
    m_rgba[3] = alpha;
}

/**
 * Set the RGBA from an unsigned int
 * @param rgbaUnsignedInt
 *    Unsigned int containing RGBA
 */
void
RgbaUnsignedByte::setUnsignedIntRGBA(const uint32_t rgbaUnsignedInt)
{
    m_unsignedIntRGBA = rgbaUnsignedInt;
}


/**
 * @return the RGBA coloring packed into an unsigned 32-bit integer
 */
uint32_t
RgbaUnsignedByte::getUnsignedIntRGBA() const
{
    return m_unsignedIntRGBA;
}

/**
 * @return An unsigned int representation with the color components divided then multipled
 * by the given divisor.  The purpose is to map similar colors to one RGBA.
 */
RgbaUnsignedByte
RgbaUnsignedByte::getScaledRgba(const uint32_t divisor) const
{
    uint8_t rgbaByte[4];
    getRgba(rgbaByte);
    
    for (int32_t i = 0; i < 4; i++) {
        rgbaByte[i] = ((rgbaByte[i] / divisor) * divisor);
    }
    
    const RgbaUnsignedByte rgbaOut(rgbaByte);
    return rgbaOut;
}

/**
 * @return String with RGBA components
 */
AString
RgbaUnsignedByte::toString() const
{
    return ("RGBA=("
            + AString::fromNumbers(m_rgba, 4)
            + ")");
}

