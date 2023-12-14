
/*LICENSE_START*/
/*
 *  Copyright (C) 2023 Washington University School of Medicine
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

#define __GRAPHICS_TEXTURE_RECTANGLE_DECLARE__
#include "GraphicsTextureRectangle.h"
#undef __GRAPHICS_TEXTURE_RECTANGLE_DECLARE__

#include "CaretAssert.h"
using namespace caret;


    
/**
 * \class caret::GraphicsTextureRectangle 
 * \brief Assists with manipulating OpenGL texture data
 * \ingroup Graphics
 */

/**
 * Constructor.
 * @param rgba
 *    Pointer to RGBA with 'width' * 'height' * 4 number of texels
 * @param width
 *    Width of texture
 * @param height
 *    Height of texture
 */
GraphicsTextureRectangle::GraphicsTextureRectangle(uint8_t* rgba,
                                                   const int64_t width,
                                                   const int64_t height)
: CaretObject(),
m_rgba(rgba),
m_width(width),
m_height(height),
m_length(m_width * m_height * 4)
{
    CaretAssert(m_rgba);
}

/**
 * Destructor.
 */
GraphicsTextureRectangle::~GraphicsTextureRectangle()
{
}

/**
 * Copy the region from the source rectrangle to this rectangle
 * @param sourceX
 *    X in source rectangle
 * @param sourceY
 *    Y in source rectangle
 * @param width
 *    Width of the region
 * @param height
 *    Height of the region
 * @param destinationX
 *    X in destination (this) rectangle
 * @param destinationY
 *    Y in destination (this) rectangle
 */
void
GraphicsTextureRectangle::copy(const GraphicsTextureRectangle& sourceRectangle,
                               const int64_t sourceX,
                               const int64_t sourceY,
                               const int64_t width,
                               const int64_t height,
                               const int64_t destinationX,
                               const int64_t destinationY)
{
    const bool debugFlag(false);
    if (debugFlag) {
        std::cout << "Source XY: " << sourceX << ", " << sourceY
        << " Width/Height: " << width << ", " << height << std::endl;
        std::cout << "   Dest XY: " << destinationX << ", " << destinationY << std::endl;
    }
    
    bool firstFlag(true);
    
    for (int64_t x = 0; x < width; x++) {
        for (int64_t y = 0; y < height; y++) {
            const int64_t sx(sourceX + x);
            CaretAssert((sx >= 0) && (sx < sourceRectangle.m_width));
            const int64_t sy(sourceY + y);
            CaretAssert((sy >= 0) && (sy < sourceRectangle.m_height));
            
            const int64_t sourceOffset(sourceRectangle.getOffset(sx,
                                                                 sy));
            const int64_t dx(destinationX + x);
            CaretAssert((dx >= 0) && (dx < m_width));
            const int64_t dy(destinationY + y);
            CaretAssert((dy >= 0) && (dy < m_height));

            const int64_t destOffset(getOffset(dx,
                                               dy));
            
            if (debugFlag) {
                if (firstFlag) {
                    firstFlag = false;
                    std::cout << "   Source Offset: " << sourceOffset << std::endl;
                    std::cout << "   Dest Offset:   " << destOffset << std::endl;
                }
            }
            for (int64_t k = 0; k < 4; k++) {
                m_rgba[destOffset + k] = sourceRectangle.m_rgba[sourceOffset + k];
            }
        }
    }
}

/**
 * Copy the region from the source rectrangle to this rectangle and generate
 * OpenGL drawing coordinates.
 * @param sourceX
 *    X in source rectangle
 * @param sourceY
 *    Y in source rectangle
 * @param width
 *    Width of the region
 * @param height
 *    Height of the region
 * @param destinationX
 *    X in destination (this) rectangle
 * @param destinationY
 *    Y in destination (this) rectangle
 * @param triangleXyzOut
 *    Output with triangle cartesian coordinates for the region of the image
 * @param triangleStrOut
 *    Output with triangle texture coordinates for the region of the image
 */
void
GraphicsTextureRectangle::copy(const GraphicsTextureRectangle& sourceRectangle,
                               const GraphicsTextureRectangle& targetRectangle,
                               const int64_t sourceX,
                               const int64_t sourceY,
                               const int64_t width,
                               const int64_t height,
                               const int64_t destinationX,
                               const int64_t destinationY,
                               std::vector<Vector3D>& triangleXyzOut,
                               std::vector<Vector3D>& triangleStrOut)
{
    triangleXyzOut.clear();
    triangleStrOut.clear();
    
    copy(sourceRectangle,
         sourceX,
         sourceY,
         width,
         height,
         destinationX,
         destinationY);
    
    /*
     * XYZ cartesian coordinate are in original "too big" image
     * STR texture coordinates are in new "packed" texture
     */
    const float z(0.0);
    const float r(0.0);
    
    /*
     * Need floats for correct texture coordinate calculations
     * (Otherwise, will get 0s and 1s).
     */
    const float destXF(destinationX);
    const float destYF(destinationY);
    const float widthF(width);
    const float heightF(height);
    const float textureWidthF(targetRectangle.m_width);
    const float textureHeightF(targetRectangle.m_height);
    
    const Vector3D blXYZ(sourceX, sourceY, z);
    const Vector3D blSTR(destXF / textureWidthF,
                         destYF / textureHeightF,
                         r);
    
    const Vector3D brXYZ(sourceX + width, sourceY, z);
    const Vector3D brSTR((destXF + widthF) / textureWidthF,
                         destYF / textureHeightF,
                         r);
    
    const Vector3D trXYZ(sourceX + width, sourceY + height, z);
    const Vector3D trSTR((destXF + widthF) / textureWidthF,
                         (destYF + heightF) / textureHeightF,
                         r);
    
    const Vector3D tlXYZ(sourceX, sourceY + height, z);
    const Vector3D tlSTR(destXF / textureWidthF,
                         (destYF + heightF) / textureHeightF,
                         r);
    
    triangleXyzOut.push_back(blXYZ);
    triangleStrOut.push_back(blSTR);
    triangleXyzOut.push_back(brXYZ);
    triangleStrOut.push_back(brSTR);
    triangleXyzOut.push_back(trXYZ);
    triangleStrOut.push_back(trSTR);
    
    triangleXyzOut.push_back(blXYZ);
    triangleStrOut.push_back(blSTR);
    triangleXyzOut.push_back(trXYZ);
    triangleStrOut.push_back(trSTR);
    triangleXyzOut.push_back(tlXYZ);
    triangleStrOut.push_back(tlSTR);
}


/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
GraphicsTextureRectangle::toString() const
{
    return "GraphicsTextureRectangle";
}

