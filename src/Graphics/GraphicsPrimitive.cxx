
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

#define __GRAPHICS_PRIMITIVE_DECLARE__
#include "GraphicsPrimitive.h"
#undef __GRAPHICS_PRIMITIVE_DECLARE__

#include "BoundingBox.h"
#include "CaretAssert.h"
#include "CaretLogger.h"
#include "EventManager.h"
#include "GraphicsEngineDataOpenGL.h"
#include "GraphicsPrimitiveV3f.h"
#include "GraphicsPrimitiveV3fC4f.h"
#include "GraphicsPrimitiveV3fC4ub.h"
#include "GraphicsPrimitiveV3fN3f.h"
#include "GraphicsPrimitiveV3fT3f.h"

using namespace caret;


    
/**
 * \class caret::GraphicsPrimitive 
 * \brief Abstract class for a graphics primitive.
 * \ingroup Graphics
 *
 */

/**
 * Constructs an instance with the given vertex, normal vector, and color type.
 *
 * @param vertexType
 *     Type of the vertices.
 * @param normalVectorType
 *     Type of the normal vectors.
 * @param colorType
 *     Type of the colors.
 * @param textureType
 *     Type of texture coordinates.
 * @param primitiveType
 *     Type of primitive drawn (triangles, lines, etc.)
 */
GraphicsPrimitive::GraphicsPrimitive(const VertexType       vertexType,
                                     const NormalVectorType normalVectorType,
                                     const ColorType        colorType,
                                     const TextureType      textureType,
                                     const PrimitiveType    primitiveType)
: CaretObject(),
 EventListenerInterface(),
 m_vertexType(vertexType),
 m_normalVectorType(normalVectorType),
 m_colorType(colorType),
 m_textureType(textureType),
 m_primitiveType(primitiveType),
 m_boundingBoxValid(false)
{
    
}

/**
 * Destructor.
 */
GraphicsPrimitive::~GraphicsPrimitive()
{
    EventManager::get()->removeAllEventsFromListener(this);    
}

/**
 * Copy constructor for graphics primitive.
 * @param obj
 *    Object that is copied.
 */
GraphicsPrimitive::GraphicsPrimitive(const GraphicsPrimitive& obj)
: CaretObject(obj),
 EventListenerInterface(),
 m_vertexType(obj.m_vertexType),
 m_normalVectorType(obj.m_normalVectorType),
 m_colorType(obj.m_colorType),
 m_textureType(obj.m_textureType),
 m_primitiveType(obj.m_primitiveType),
 m_boundingBoxValid(false)
{
    this->copyHelperGraphicsPrimitive(obj);
}

/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void 
GraphicsPrimitive::copyHelperGraphicsPrimitive(const GraphicsPrimitive& obj)
{
    m_boundingBoxValid            = false;
    m_xyz                         = obj.m_xyz;
    m_floatNormalVectorXYZ        = obj.m_floatNormalVectorXYZ;
    m_floatRGBA                   = obj.m_floatRGBA;
    m_unsignedByteRGBA            = obj.m_unsignedByteRGBA;
    m_floatTextureSTR             = obj.m_floatTextureSTR;
    m_lineWidthType               = obj.m_lineWidthType;
    m_lineWidthValue              = obj.m_lineWidthValue;
    m_pointSizeType               = obj.m_pointSizeType;
    m_pointDiameterValue          = obj.m_pointDiameterValue;
    m_polygonalLinePrimitiveRestartIndices     = obj.m_polygonalLinePrimitiveRestartIndices;
    m_sphereSizeType              = obj.m_sphereSizeType;
    m_sphereDiameterValue         = obj.m_sphereDiameterValue;
    m_textureImageBytesRGBA       = obj.m_textureImageBytesRGBA;
    m_textureImageWidth           = obj.m_textureImageWidth;
    m_textureImageHeight          = obj.m_textureImageHeight;

    m_graphicsEngineDataForOpenGL.reset();
}

/**
 * Request the capacity of the primitive for the given number of vertices.
 * Using this method can help reduce reallocations as data is added 
 * to the primitive.  Note: the primitive data is stored in 
 * std::vector's and this method functions identically to
 * std::vector::reserve().
 *
 * @param numberOfVertices
 *     Number of vertices
 */
void
GraphicsPrimitive::reserveForNumberOfVertices(const int32_t numberOfVertices)
{
    switch (m_vertexType) {
        case VertexType::FLOAT_XYZ:
            m_xyz.reserve(numberOfVertices * 3);
            break;
    }
    
    switch (m_normalVectorType) {
        case NormalVectorType::FLOAT_XYZ:
            m_floatNormalVectorXYZ.reserve(numberOfVertices * 3);
            break;
        case NormalVectorType::NONE:
            break;
    }
    
    switch (m_colorType) {
        case GraphicsPrimitive::ColorType::NONE:
            break;
        case ColorType::FLOAT_RGBA:
            m_floatRGBA.reserve(numberOfVertices * 4);
            break;
        case ColorType::UNSIGNED_BYTE_RGBA:
            m_unsignedByteRGBA.reserve(numberOfVertices * 4);
            break;
    }
    
    switch (m_textureType) {
        case GraphicsPrimitive::TextureType::NONE:
            break;
        case GraphicsPrimitive::TextureType::FLOAT_STR:
            m_floatTextureSTR.reserve(numberOfVertices * 3);
            break;
    }
    
    m_boundingBoxValid = false;
}

/**
 * @return The coordinates usage type hint for graphics system.
 */
GraphicsPrimitive::UsageType
GraphicsPrimitive::getUsageTypeCoordinates() const
{
    return m_usageTypeCoordinates;
}
/**
 * @return The normals usage type hint for graphics system.
 */
GraphicsPrimitive::UsageType
GraphicsPrimitive::getUsageTypeNormals() const
{
    return m_usageTypeNormals;
}

/**
 * @return The colors usage type hint for graphics system.
 */
GraphicsPrimitive::UsageType
GraphicsPrimitive::getUsageTypeColors() const
{
    return m_usageTypeColors;
}

/**
 * @return The texture coordinates usage type hint for graphics system.
 */
GraphicsPrimitive::UsageType
GraphicsPrimitive::getUsageTypeTextureCoordinates() const
{
    return m_usageTypeTextureCoordinates;
}

/**
 * Set the usage type hint for all data types for graphics system.
 * This will override any of the individual (vertex, normal, etc)
 * usage types so they should be called after calling this method.
 *
 * @param usageType
 *     New value for usage type.
 */
void
GraphicsPrimitive::setUsageTypeAll(const UsageType usageType)
{
    m_usageTypeCoordinates = usageType;
    m_usageTypeNormals  = usageType;
    m_usageTypeColors   = usageType;
    m_usageTypeTextureCoordinates  = usageType;
}
/**
 * Set the coordinates usage type hint for graphics system.
 *
 * @param usageType
 *     New value for usage type.
 */
void
GraphicsPrimitive::setUsageTypeCoordinates(const UsageType usageType)
{
    m_usageTypeCoordinates = usageType;
}

/**
 * Set the normals usage type hint for graphics system.
 *
 * @param usageType
 *     New value for usage type.
 */
void
GraphicsPrimitive::setUsageTypeNormals(const UsageType usageType)
{
    m_usageTypeNormals = usageType;
}

/**
 * Set the colors usage type hint for graphics system.
 *
 * @param usageType
 *     New value for usage type.
 */
void
GraphicsPrimitive::setUsageTypeColors(const UsageType usageType)
{
    m_usageTypeColors = usageType;
}

/**
 * Set the texture coordinates usage type hint for graphics system.
 *
 * @param usageType
 *     New value for usage type.
 */
void
GraphicsPrimitive::setUsageTypeTextureCoordinates(const UsageType usageType)
{
    m_usageTypeTextureCoordinates = usageType;
}

/**
 * @return Is this graphics primitive valid.
 * Primitive is valid if all of these conditions are met
 * (A) ((count(xyz) * 3) == (count(rgba) * 4))
 * (B) (count(xyz) > 0)
 * (C) ((count(xyz) == count(normals)) OR (count(normals) == 0)
 */
bool
GraphicsPrimitive::isValid() const
{
    switch (m_vertexType) {
        case VertexType::FLOAT_XYZ:
            break;
    }
    
    const uint32_t numXYZ = m_xyz.size();
    if (numXYZ > 0) {
        switch (m_normalVectorType) {
            case NormalVectorType::NONE:
                break;
            case NormalVectorType::FLOAT_XYZ:
            {
                const uint32_t numNormalXYZ = m_floatNormalVectorXYZ.size();
                if (numNormalXYZ > 0) {
                    if (numNormalXYZ != numXYZ) {
                        CaretLogWarning("ERROR: GraphicsPrimitive XYZ size not matching surface normal size.");
                        return false;
                    }
                }
            }
                break;
        }
        
        bool haveRgbaFlag = false;
        uint32_t numColorRGBA = 0;
        switch (m_colorType) {
            case GraphicsPrimitive::ColorType::NONE:
                haveRgbaFlag = false;
                break;
            case ColorType::FLOAT_RGBA:
                numColorRGBA = m_floatRGBA.size();
                haveRgbaFlag = true;
                break;
            case ColorType::UNSIGNED_BYTE_RGBA:
                numColorRGBA = m_unsignedByteRGBA.size();
                haveRgbaFlag = true;
                break;
        }
        if (haveRgbaFlag) {
            if ((numXYZ / 3) != (numColorRGBA / 4)) {
                CaretLogWarning("ERROR: GraphicsPrimitive XYZ size does not match RGBA size");
                return false;
            }
        }
        
        bool haveTextureFlag = false;
        uint32_t numTextureSTR = 0;
        switch (m_textureType) {
            case TextureType::NONE:
                break;
            case TextureType::FLOAT_STR:
                numTextureSTR = m_floatTextureSTR.size();
                haveTextureFlag = true;
                break;
        }
        if (haveTextureFlag) {
            if (numXYZ != numTextureSTR) {
                CaretLogWarning("ERROR: GraphicsPrimitive XYZ size does not match Texture STR size");
                return false;
            }
            
            if (m_textureImageBytesRGBA.empty()) {
                CaretLogWarning("ERROR: GraphicsPrimitive has invalid texture data");
            }
        }
        
        switch (m_primitiveType) {
            case PrimitiveType::OPENGL_LINE_LOOP:
            case PrimitiveType::POLYGONAL_LINE_LOOP:
                if (numXYZ < 3) {
                    CaretLogWarning("Line loop must have at least 3 vertices.");
                }
                break;
            case PrimitiveType::OPENGL_LINE_STRIP:
            case PrimitiveType::POLYGONAL_LINE_STRIP:
                if (numXYZ < 2) {
                    CaretLogWarning("Line strip must have at least 2 vertices.");
                }
                break;
            case PrimitiveType::OPENGL_LINES:
            case PrimitiveType::POLYGONAL_LINES:
                if (numXYZ < 2) {
                    CaretLogWarning("Lines must have at least 2 vertices.");
                }
                else {
                    const uint32_t extraVertices = numXYZ % 2;
                    if (extraVertices > 0) {
                        CaretLogWarning("Extra vertices for drawing lines ignored.");
                    }
                }
                break;
            case PrimitiveType::OPENGL_POINTS:
                break;
            case PrimitiveType::OPENGL_POLYGON:
                if (numXYZ < 3) {
                    CaretLogWarning("Polygon must have at least 3 vertices.");
                }
                break;
            case PrimitiveType::OPENGL_QUAD_STRIP:
                if (numXYZ < 4) {
                    CaretLogWarning("Quad strip must have at least 4 vertices.");
                }
                else {
                    const uint32_t extraVertices = numXYZ % 2;
                    if (extraVertices > 0) {
                        CaretLogWarning("Extra vertices for drawing quads ignored.");
                    }
                }
                break;
            case PrimitiveType::OPENGL_QUADS:
                if (numXYZ < 4) {
                    CaretLogWarning("Quads must have at least 4 vertices.");
                }
                else {
                    const uint32_t extraVertices = numXYZ % 4;
                    if (extraVertices > 0) {
                        CaretLogWarning("Extra vertices for drawing quads ignored.");
                    }
                }
                break;
            case PrimitiveType::OPENGL_TRIANGLE_FAN:
                if (numXYZ < 3) {
                    CaretLogWarning("Triangle fan must have at least 3 vertices.");
                }
                break;
            case PrimitiveType::OPENGL_TRIANGLE_STRIP:
                if (numXYZ < 3) {
                    CaretLogWarning("Triangle strip must have at least 3 vertices.");
                }
                break;
            case PrimitiveType::OPENGL_TRIANGLES:
                if (numXYZ < 3) {
                    CaretLogWarning("Triangles must have at least 3 vertices.");
                }
                else {
                    const uint32_t extraVertices = numXYZ % 3;
                    if (extraVertices > 0) {
                        CaretLogWarning("Extra vertices for drawing triangles ignored.");
                    }
                }
                break;
            case PrimitiveType::SPHERES:
                if (numXYZ < 1) {
                    CaretLogWarning("Spheres must have at least 1 vertices.");
                }
                break;
        }

        return true;
    }
    
    return false;
}

/**
 * Print the primitive's data, data may be long!
 */
void
GraphicsPrimitive::print() const
{
    std::cout << toStringPrivate(true) << std::endl;
}


/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString
GraphicsPrimitive::toString() const
{
    return toStringPrivate(false);
}

/**
 * @return Type of primitive as text.
 */
AString
GraphicsPrimitive::getPrimitiveTypeAsText() const
{
    AString s;
    
    switch (m_primitiveType) {
        case PrimitiveType::OPENGL_LINE_LOOP:
            s = "OpenGL Line Loop";
            break;
        case PrimitiveType::OPENGL_LINE_STRIP:
            s = "OpenGL Line Strip";
            break;
        case PrimitiveType::OPENGL_LINES:
            s = "OpenGL Lines";
            break;
        case PrimitiveType::OPENGL_POINTS:
            s = "OpenGL Points";
            break;
        case PrimitiveType::OPENGL_POLYGON:
            s = "OpenGL Polygon";
            break;
        case PrimitiveType::OPENGL_QUAD_STRIP:
            s = "OpenGL Quad Strip";
            break;
        case PrimitiveType::OPENGL_QUADS:
            s = "OpenGL Quads";
            break;
        case PrimitiveType::OPENGL_TRIANGLE_FAN:
            s = "OpenGL Triangle Fan";
            break;
        case PrimitiveType::OPENGL_TRIANGLE_STRIP:
            s = "OpenGL Triangle Strip";
            break;
        case PrimitiveType::OPENGL_TRIANGLES:
            s = "OpenGL Triangles";
            break;
        case PrimitiveType::POLYGONAL_LINE_LOOP:
            s = "Polygonal Line Loop";
            break;
        case PrimitiveType::POLYGONAL_LINE_STRIP:
            s = "Polygonal Line Strip";
            break;
        case PrimitiveType::POLYGONAL_LINES:
            s = "Polygonal Lines";
            break;
        case PrimitiveType::SPHERES:
            s = "Spheres";
            break;
    }
    
    return s;
}

/**
 * @return Size type as text for the given size type.
 *
 * @param sizeType
 *     The size type.
 */
AString
GraphicsPrimitive::getSizeTypeAsText(const SizeType sizeType) const
{
    AString s;
    
    switch (sizeType) {
        case SizeType::PERCENTAGE_VIEWPORT_HEIGHT:
            s = "Percentage Viewport Height";
            break;
        case SizeType::PIXELS:
            s = "Pixels";
            break;
    }
    return s;
}

/**
 * Convert to string.
 *
 * @param includeAllDataFlag
 *     If true, numerical values are included.  Else, only type of data.
 */
AString
GraphicsPrimitive::toStringPrivate(const bool includeAllDataFlag) const
{
    AString s("Primitive: ");
    
    s.append(getPrimitiveTypeAsText());

    switch (m_vertexType) {
        case VertexType::FLOAT_XYZ:
            s.appendWithNewLine("Vertex Count: " + AString::number(m_xyz.size() / 3) + " Float XYZ.  ");
            if (includeAllDataFlag ) {
                s.append("\n   ");
                const int32_t count = static_cast<int32_t>(m_xyz.size());
                for (int32_t i = 0; i < count; i++) {
                    s.append(AString::number(m_xyz[i]) + " ");
                    if (((i+1) % 3) == 0) {
                        s.append("\n   ");
                    }
                }
                s.append("\n");
            }
            break;
    }
    
    if ( ! m_polygonalLinePrimitiveRestartIndices.empty()) {
        s.appendWithNewLine("Polygonal Line Primitive Restart Indices Count: " + AString::number(m_polygonalLinePrimitiveRestartIndices.size()));
        if (includeAllDataFlag) {
            std::vector<int32_t> temp(m_polygonalLinePrimitiveRestartIndices.begin(),
                                      m_polygonalLinePrimitiveRestartIndices.end());
            s.appendWithNewLine("    " + AString::fromNumbers(temp, ", "));
        }
    }
    
    switch (m_normalVectorType) {
        case NormalVectorType::FLOAT_XYZ:
            s.appendWithNewLine("Normal Vector Count: " + AString::number(m_floatNormalVectorXYZ.size() / 3) + " Float XYZ.  ");
            break;
            if (includeAllDataFlag ) {
                s.append("\n   ");
                const int32_t count = static_cast<int32_t>(m_floatNormalVectorXYZ.size());
                for (int32_t i = 0; i < count; i++) {
                    s.append(AString::number(m_floatNormalVectorXYZ[i]) + " ");
                    if (((i+1) % 3) == 0) {
                        s.append("\n   ");
                    }
                }
            }
        case NormalVectorType::NONE:
            s.appendWithNewLine("Normal Vector: None.  ");
            break;
    }
    
    switch (m_colorType) {
        case GraphicsPrimitive::ColorType::NONE:
            break;
        case ColorType::FLOAT_RGBA:
            s.appendWithNewLine("Color Count: " + AString::number(m_floatRGBA.size() / 4) + " Float RGBA 0.0 to 1.0.  ");
            if (includeAllDataFlag ) {
                s.append("\n   ");
                const int32_t count = static_cast<int32_t>(m_floatRGBA.size());
                for (int32_t i = 0; i < count; i++) {
                    s.append(AString::number(m_floatRGBA[i]) + " ");
                    if (((i+1) % 4) == 0) {
                        s.append("\n   ");
                    }
                }
            }
            break;
        case ColorType::UNSIGNED_BYTE_RGBA:
            s.appendWithNewLine("Color Count: " + AString::number(m_unsignedByteRGBA.size() / 4) + " Unsigned Byte RGBA  0 to 255");
            if (includeAllDataFlag ) {
                s.append("\n   ");
                const int32_t count = static_cast<int32_t>(m_unsignedByteRGBA.size());
                for (int32_t i = 0; i < count; i++) {
                    s.append(AString::number(m_unsignedByteRGBA[i]) + " ");
                    if (((i+1) % 4) == 0) {
                        s.append("\n   ");
                    }
                }
            }
            break;
    }
    
    switch (m_textureType) {
        case TextureType::NONE:
            break;
        case TextureType::FLOAT_STR:
            s.appendWithNewLine("Texture: " + AString::number(m_floatTextureSTR.size()) + " Float Texture 0.0 to 1.0.  ");
            s.appendWithNewLine("   Width: " + AString::number(m_textureImageWidth)
                                + " Height: " + AString::number(m_textureImageHeight));
            if (includeAllDataFlag ) {
                s.append("\n   ");
                const int32_t count = static_cast<int32_t>(m_floatTextureSTR.size());
                for (int32_t i = 0; i < count; i++) {
                    s.append(AString::number(m_floatTextureSTR[i]) + " ");
                    if (((i+1) % 3) == 0) {
                        s.append("\n   ");
                    }
                }
            }
            break;
    }
    
    bool addLineWidthFlag  = false;
    bool addPointSizeFlag  = false;
    bool addSphereSizeFlag = false;
    
    switch (m_primitiveType) {
        case PrimitiveType::OPENGL_LINE_LOOP:
            addLineWidthFlag = true;
            break;
        case PrimitiveType::OPENGL_LINE_STRIP:
            addLineWidthFlag = true;
            break;
        case PrimitiveType::OPENGL_LINES:
            addLineWidthFlag = true;
            break;
        case PrimitiveType::OPENGL_POINTS:
            addPointSizeFlag = true;
            break;
        case PrimitiveType::OPENGL_POLYGON:
            break;
        case PrimitiveType::OPENGL_QUAD_STRIP:
            break;
        case PrimitiveType::OPENGL_QUADS:
            break;
        case PrimitiveType::OPENGL_TRIANGLE_FAN:
            break;
        case PrimitiveType::OPENGL_TRIANGLE_STRIP:
            break;
        case PrimitiveType::OPENGL_TRIANGLES:
            break;
        case PrimitiveType::POLYGONAL_LINE_LOOP:
            addLineWidthFlag = true;
            break;
        case PrimitiveType::POLYGONAL_LINE_STRIP:
            addLineWidthFlag = true;
            break;
        case PrimitiveType::POLYGONAL_LINES:
            addLineWidthFlag = true;
            break;
        case PrimitiveType::SPHERES:
            addSphereSizeFlag = true;
            break;
    }
    
    if (addLineWidthFlag) {
        s.appendWithNewLine("Line Width Type: "
                            + getSizeTypeAsText(m_lineWidthType)
                            + "; Width Value: "
                            + AString::number(m_lineWidthValue, 'f', 3));
    }
    
    if (addPointSizeFlag) {
        s.appendWithNewLine("Point Diameter Type: "
                            + getSizeTypeAsText(m_pointSizeType)
                            + "; Diameter Value: "
                            + AString::number(m_pointDiameterValue, 'f', 3));
    }
    
    if (addSphereSizeFlag) {
        s.appendWithNewLine("Sphere Diameter Type: "
                            + getSizeTypeAsText(m_sphereSizeType)
                            + "; Diameter Value: "
                            + AString::number(m_sphereDiameterValue, 'f', 3));
    }

    return s;
}

/**
 * Add a vertex and its normal vector
 *
 * @param xyz
 *     Coordinate of vertex.
 * @param normalVector
 *     Normal vector for the vertex.
 * @param rgbaFloat
 *     Float RGBA color components.
 * @param rgbaByte
 *     Byte RGBA color components.
 */
void
GraphicsPrimitive::addVertexProtected(const float xyz[3],
                                      const float normalVector[3],
                                      const float rgbaFloat[4],
                                      const uint8_t rgbaByte[4],
                                      const float textureSTR[3])
{
    m_xyz.insert(m_xyz.end(),
                 xyz, xyz + 3);
    
    switch (m_normalVectorType) {
        case NormalVectorType::FLOAT_XYZ:
            CaretAssert(normalVector);
            m_floatNormalVectorXYZ.insert(m_floatNormalVectorXYZ.end(),
                                          normalVector,
                                          normalVector + 3);
            break;
        case NormalVectorType::NONE:
            break;
    }
    
    switch (m_colorType) {
        case ColorType::NONE:
            break;
        case ColorType::FLOAT_RGBA:
            CaretAssert(rgbaFloat);
            m_floatRGBA.insert(m_floatRGBA.end(),
                               rgbaFloat,
                               rgbaFloat + 4);
            break;
        case ColorType::UNSIGNED_BYTE_RGBA:
            CaretAssert(rgbaByte);
            m_unsignedByteRGBA.insert(m_unsignedByteRGBA.end(),
                                      rgbaByte,
                                      rgbaByte + 4);
            break;
    }
    
    switch (m_textureType) {
        case TextureType::NONE:
            break;
        case TextureType::FLOAT_STR:
            CaretAssert(textureSTR);
            m_floatTextureSTR.insert(m_floatTextureSTR.end(),
                                     textureSTR,
                                     textureSTR + 3);
            break;
    }
    
    m_boundingBoxValid = false;
    
    /*
     * The triangle strip primitve vertces are filled after two
     * vertices are added after requesting a restart
     */
    if (m_triangleStripPrimitiveRestartIndex == getNumberOfVertices()) {
        m_triangleStripPrimitiveRestartIndex = -1;
        fillTriangleStripPrimitiveRestartVertices();
    }
}

/**
 * Replace the existing XYZ coordinates with the given
 * XYZ coordinates.  The existing and new coordinates
 * MUST BE the same size.
 *
 * @param xyz
 *     The new XYZ coordinates.
 */
void
GraphicsPrimitive::replaceFloatXYZ(const std::vector<float>& xyz)
{
    if (xyz.size() == m_xyz.size()) {
        m_xyz = xyz;
        
        if (m_graphicsEngineDataForOpenGL != NULL) {
            m_graphicsEngineDataForOpenGL->invalidateCoordinates();
        }
    }
    else {
        const AString msg("Replacement XYZ must be same size as existing xyz");
        CaretLogWarning(msg);
        CaretAssertMessage(0, msg);
    }
    
    m_boundingBoxValid = false;
}

/**
 * Replace the XYZ coordinate at the given index
 *
 * @param vertexIndex
 *     Index of the vertex
 * @param xyz
 *     The new XYZ coordinate
 */
void
GraphicsPrimitive::replaceVertexFloatXYZ(const int32_t vertexIndex,
                                         const float xyz[3])
{
    const int32_t offset = vertexIndex * 3;
    CaretAssertVectorIndex(m_xyz, offset + 2);
    m_xyz[offset]     = xyz[0];
    m_xyz[offset + 1] = xyz[1];
    m_xyz[offset + 2] = xyz[2];

    if (m_graphicsEngineDataForOpenGL != NULL) {
        m_graphicsEngineDataForOpenGL->invalidateCoordinates();
    }
}

/**
 * Get the float RGBA coloring for a vertex.
 *
 * @param vertexIndex
 *     Index of the vertex.
 * @param rgbaOut
 *     Output with RGBA.
 */
void
GraphicsPrimitive::getVertexFloatRGBA(const int32_t vertexIndex,
                                      float rgbaOut[4]) const
{
    const int32_t i4 = vertexIndex * 4;
    switch (m_colorType) {
        case ColorType::NONE:
            CaretAssert(0);
            break;
        case ColorType::FLOAT_RGBA:
            CaretAssertVectorIndex(m_floatRGBA, i4 + 3);
            rgbaOut[0] = m_floatRGBA[i4];
            rgbaOut[1] = m_floatRGBA[i4 + 1];
            rgbaOut[2] = m_floatRGBA[i4 + 2];
            rgbaOut[3] = m_floatRGBA[i4 + 3];
            break;
        case ColorType::UNSIGNED_BYTE_RGBA:
            CaretAssertMessage(0, "Getting float RGBA from primitive but coloring type is Byte");
            CaretLogWarning("Getting float RGBA from primitive but coloring type is Byte");
            break;
    }
}

/**
 * Replace the float RGBA coloring for a vertex.
 *
 * @param vertexIndex
 *     Index of the vertex.
 * @param rgba
 *     New RGBA for vertex.
 */
void
GraphicsPrimitive::replaceVertexFloatRGBA(const int32_t vertexIndex,
                                          const float rgba[4])
{
    const int32_t i4 = vertexIndex * 4;
    switch (m_colorType) {
        case ColorType::NONE:
            CaretAssert(0);
            break;
        case ColorType::FLOAT_RGBA:
            CaretAssertVectorIndex(m_floatRGBA, i4 + 3);
            m_floatRGBA[i4]     = rgba[0];
            m_floatRGBA[i4 + 1] = rgba[1];
            m_floatRGBA[i4 + 2] = rgba[2];
            m_floatRGBA[i4 + 3] = rgba[3];
            break;
        case ColorType::UNSIGNED_BYTE_RGBA:
            CaretAssertMessage(0, "Replacing float RGBA in primitive but coloring type is Byte");
            CaretLogWarning("Replacing float RGBA in primitive but coloring type is Byte");
            break;
    }

    if (m_graphicsEngineDataForOpenGL != NULL) {
        m_graphicsEngineDataForOpenGL->invalidateColors();
    }
}

/**
 * Get the byte RGBA coloring for a vertex.
 *
 * @param vertexIndex
 *     Index of the vertex.
 * @param rgbaOut
 *     Output with RGBA.
 */
void
GraphicsPrimitive::getVertexByteRGBA(const int32_t vertexIndex,
                                     uint8_t rgbaOut[4]) const
{
    const int32_t i4 = vertexIndex * 4;
    switch (m_colorType) {
        case ColorType::NONE:
            CaretAssert(0);
            break;
        case ColorType::FLOAT_RGBA:
            CaretAssertMessage(0, "Getting Byte RGBA in primitive but coloring type is Float");
            CaretLogWarning("Getting Byte RGBA in primitive but coloring type is Float");
            break;
        case ColorType::UNSIGNED_BYTE_RGBA:
            CaretAssertVectorIndex(m_unsignedByteRGBA, i4 + 3);
            rgbaOut[0] = m_unsignedByteRGBA[i4];
            rgbaOut[1] = m_unsignedByteRGBA[i4 + 1];
            rgbaOut[2] = m_unsignedByteRGBA[i4 + 2];
            rgbaOut[3] = m_unsignedByteRGBA[i4 + 3];
            break;
    }
}

/**
 * Replace the float RGBA coloring for a vertex.
 *
 * @param vertexIndex
 *     Index of the vertex.
 * @param rgba
 *     New RGBA for vertex.
 */
void
GraphicsPrimitive::replaceVertexByteRGBA(const int32_t vertexIndex,
                                         const uint8_t rgba[4])
{
    const int32_t i4 = vertexIndex * 4;
    switch (m_colorType) {
        case ColorType::NONE:
            CaretAssert(0);
            break;
        case ColorType::FLOAT_RGBA:
            CaretAssertMessage(0, "Replacing Byte RGBA in primitive but coloring type is Float");
            CaretLogWarning("Replacing Byte RGBA in primitive but coloring type is Float");
            break;
        case ColorType::UNSIGNED_BYTE_RGBA:
            CaretAssertVectorIndex(m_unsignedByteRGBA, i4 + 3);
            m_unsignedByteRGBA[i4]     = rgba[0];
            m_unsignedByteRGBA[i4 + 1] = rgba[1];
            m_unsignedByteRGBA[i4 + 2] = rgba[2];
            m_unsignedByteRGBA[i4 + 3] = rgba[3];
            break;
    }

    if (m_graphicsEngineDataForOpenGL != NULL) {
        m_graphicsEngineDataForOpenGL->invalidateColors();
    }
}

/**
 * Replace all vertex RGBA with the given solid color RGBA.
 *
 * @param rgba
 *     RGBA values for all vertices
 */
void
GraphicsPrimitive::replaceAllVertexSolidByteRGBA(const uint8_t rgba[4])
{
    switch (m_colorType) {
        case ColorType::NONE:
            CaretAssert(0);
            break;
        case ColorType::FLOAT_RGBA:
            CaretAssertMessage(0, "Replacing Byte RGBA in primitive but coloring type is Float");
            CaretLogWarning("Replacing Byte RGBA in primitive but coloring type is Float");
            break;
        case ColorType::UNSIGNED_BYTE_RGBA:
        {
            const int32_t numRGBA = static_cast<int32_t>(m_unsignedByteRGBA.size() / 4);
            for (int32_t i = 0; i < numRGBA; i++) {
                const int32_t i4 = i * 4;
                CaretAssertVectorIndex(m_unsignedByteRGBA, i4 + 3);
                m_unsignedByteRGBA[i4]     = rgba[0];
                m_unsignedByteRGBA[i4 + 1] = rgba[1];
                m_unsignedByteRGBA[i4 + 2] = rgba[2];
                m_unsignedByteRGBA[i4 + 3] = rgba[3];
            }
        }
    }
    
    if (m_graphicsEngineDataForOpenGL != NULL) {
        m_graphicsEngineDataForOpenGL->invalidateColors();
    }
}

/**
 * Replace all vertex RGBA with the given solid color RGBA.
 *
 * @param rgba
 *     RGBA values for all vertices
 */
void
GraphicsPrimitive::replaceAllVertexSolidFloatRGBA(const float rgba[4])
{
    switch (m_colorType) {
        case ColorType::NONE:
            CaretAssert(0);
            break;
        case ColorType::FLOAT_RGBA:
        {
            const int32_t numRGBA = static_cast<int32_t>(m_floatRGBA.size() / 4);
            for (int32_t i = 0; i < numRGBA; i++) {
                const int32_t i4 = i * 4;
                CaretAssertVectorIndex(m_floatRGBA, i4 + 3);
                m_floatRGBA[i4]     = rgba[0];
                m_floatRGBA[i4 + 1] = rgba[1];
                m_floatRGBA[i4 + 2] = rgba[2];
                m_floatRGBA[i4 + 3] = rgba[3];
            }
        }
            break;
        case ColorType::UNSIGNED_BYTE_RGBA:
            CaretAssertMessage(0, "Replacing Byte RGBA in primitive but coloring type is Float");
            CaretLogWarning("Replacing Byte RGBA in primitive but coloring type is Float");
            break;
    }
}




/**
 * Get a bounds box for the vertex coordinates.
 *
 * @param boundingBoxOut
 *     Output bounding box.
 * @return
 *     True if bounding box is valid, else false.
 */
bool
GraphicsPrimitive::getVertexBounds(BoundingBox& boundingBoxOut) const
{
    if ( ! m_boundingBoxValid) {
        if (m_boundingBox == NULL) {
            m_boundingBox.reset(new BoundingBox());
        }
        m_boundingBox->resetForUpdate();
        
        const int32_t numberOfVertices = static_cast<int32_t>(m_xyz.size() / 3);
        for (int32_t i = 0; i < numberOfVertices; i++) {
            const int32_t i3 = i * 3;
            CaretAssertVectorIndex(m_xyz, i3 + 2);
            m_boundingBox->update(&m_xyz[i3]);
        }
        
        m_boundingBoxValid = true;
    }
    
    boundingBoxOut = *m_boundingBox;
    return m_boundingBoxValid;
}

/**
 * There may be instances where a primitive should stop and restart at
 * non-consecutive vertices (such as a gap in connected line segment).
 * Typically, this requires creating separate primitives around
 * the gap.
 * 
 * When this method is called, it indicates that any previously
 * added points will not connect to any points after this method
 * is called.  
 *
 * This functionality is similar to that of glPrimitiveRestartIndex()
 * that is not available in versions of OpenGL prior to 3.1.
 *
 * At this time, this functionality if available only for 
 * line strip and line loop.
 */
void
GraphicsPrimitive::addPrimitiveRestart()
{
    bool polygonalLineFlag = false;
    bool triangleStripFlag = false;
    
    switch (m_primitiveType) {
        case PrimitiveType::OPENGL_LINE_LOOP:
            break;
        case PrimitiveType::OPENGL_LINE_STRIP:
            break;
        case PrimitiveType::OPENGL_LINES:
            break;
        case PrimitiveType::OPENGL_POINTS:
            break;
        case PrimitiveType::OPENGL_POLYGON:
            break;
        case PrimitiveType::OPENGL_QUAD_STRIP:
            break;
        case PrimitiveType::OPENGL_QUADS:
            break;
        case PrimitiveType::OPENGL_TRIANGLE_FAN:
            break;
        case PrimitiveType::OPENGL_TRIANGLE_STRIP:
            triangleStripFlag = true;
            break;
        case PrimitiveType::OPENGL_TRIANGLES:
            break;
        case PrimitiveType::POLYGONAL_LINE_LOOP:
            polygonalLineFlag = true;
            break;
        case PrimitiveType::POLYGONAL_LINE_STRIP:
            polygonalLineFlag = true;
            break;
        case PrimitiveType::POLYGONAL_LINES:
            break;
        case PrimitiveType::SPHERES:
            break;
    }
    
    if (polygonalLineFlag) {
        /*
         * A set is used because a vertex could be added more than one time
         */
        m_polygonalLinePrimitiveRestartIndices.insert(getNumberOfVertices());
    }
    else if (triangleStripFlag) {
        const int32_t numVertices = getNumberOfVertices();
        if (numVertices > 3) {
            /*
             * Add 18 dummy vertices that will be used for the
             * 6 degenerate triangles at a later time
             */
            float float4[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
            uint8_t byte4[4] = { 0, 0, 0, 0 };
            for (int32_t i = 0; i < 18; i++) {
                addVertexProtected(float4, float4, float4, byte4, float4);
            }
            
            /*
             * When the number of vertices becomes the index below,
             * the degenerate triangles will be added.
             */
            m_triangleStripPrimitiveRestartIndex = getNumberOfVertices() + 2;
        }
        else {
            CaretLogSevere("Must have at least three vertices before requesting a triangle strip primitive restart.");
        }
    }
    else {
        CaretLogSevere("Requested primitive restart on not yet supported primitive type: "
                       + getPrimitiveTypeAsText());
    }
}

/**
 * Copy a vertex to another vertex.
 *
 * @param copyFromIndex
 *     Index of the 'copied' vertex.
 * @param copyToIndex
 */
void
GraphicsPrimitive::copyVertex(const int32_t copyFromIndex,
                              const int32_t copyToIndex)
{
    const int32_t from3 = copyFromIndex * 3;
    const int32_t to3   = copyToIndex * 3;
    
    for (int32_t i = 0; i < 3; i++) {
        CaretAssertVectorIndex(m_xyz, from3 + i);
        CaretAssertVectorIndex(m_xyz, to3 + i);
        m_xyz[to3 + i] = m_xyz[from3 + i];
        
        switch (m_normalVectorType) {
            case NormalVectorType::FLOAT_XYZ:
                CaretAssertVectorIndex(m_floatNormalVectorXYZ, from3 + i);
                CaretAssertVectorIndex(m_floatNormalVectorXYZ, to3 + i);
                m_floatNormalVectorXYZ[to3 + i] = m_floatNormalVectorXYZ[from3 + i];
                break;
            case NormalVectorType::NONE:
                break;
        }
        
        switch (m_textureType) {
            case TextureType::NONE:
                break;
            case TextureType::FLOAT_STR:
                CaretAssertVectorIndex(m_floatTextureSTR, from3 + i);
                CaretAssertVectorIndex(m_floatTextureSTR, to3 + i);
                m_floatTextureSTR[to3 + i] = m_floatTextureSTR[from3 + i];
                break;
        }
    }
    
    const int32_t from4 = copyFromIndex * 4;
    const int32_t to4   = copyToIndex * 4;
    
    for (int32_t i = 0; i < 4; i++) {
        switch (m_colorType) {
            case ColorType::NONE:
                break;
            case ColorType::FLOAT_RGBA:
                CaretAssertVectorIndex(m_floatRGBA, from4 + i);
                CaretAssertVectorIndex(m_floatRGBA, to4 + i);
                m_floatRGBA[to4 + i] = m_floatRGBA[from4 + i];
                break;
            case ColorType::UNSIGNED_BYTE_RGBA:
                CaretAssertVectorIndex(m_unsignedByteRGBA, from4 + i);
                CaretAssertVectorIndex(m_unsignedByteRGBA, to4 + i);
                m_unsignedByteRGBA[to4 + i] = m_unsignedByteRGBA[from4 + i];
                break;
        }
    }
    
    m_boundingBoxValid = false;
}

/**
 * Fill the degenerate vertices for the triangle strip.
 * Since OpenGL's primitive restart (glPrimitiveRestartIndices())
 * function is not available prior to OpenGL 3.1, add
 * degenerate triangles as explained in the following links.
 *
 * @param newStripVertexOneXYZ
 *     First vertex in new strip that will be added after calling this method.
 * @param newStripVertexTwoXYZ
 *     Second vertex in new strip that will be added after calling this method.
 *
 * @see http://www.gamedev.net/page/resources/_/technical/graphics-programming-and-theory/concatenating-triangle-strips-r1871
 * @see http://en.wikipedia.org/wiki/Triangle_strip
 */
void
GraphicsPrimitive::fillTriangleStripPrimitiveRestartVertices()
{
    const int32_t numVertices = getNumberOfVertices();
    
    /* 
     * Vertices "F" and "F" were added just before the call to addPrimitiveRestart()
     * Vertices "G" and "H" were added after the call to addPrimitiveRestart() and
     * this method was called immediately after "H" was added
     * addPrimitiveRestart() added 18 vertices:
     * DEF + EFF + FFG + FGG + GGH + GHG + GGH + GHI
     *
     * From: https://www.gamedev.net/articles/programming/graphics/concatenating-triangle-strips-r1871
     */
    
    const int32_t vertexH = numVertices - 1;
    const int32_t vertexG = numVertices - 2;
    const int32_t vertexF = numVertices - 21;
    const int32_t vertexE = numVertices - 22;
    int32_t copyToIndex = vertexF + 1;
    
    copyVertex(vertexE, copyToIndex++);
    copyVertex(vertexF, copyToIndex++);
    copyVertex(vertexF, copyToIndex++);
    
    copyVertex(vertexF, copyToIndex++);
    copyVertex(vertexF, copyToIndex++);
    copyVertex(vertexG, copyToIndex++);
    
    copyVertex(vertexF, copyToIndex++);
    copyVertex(vertexG, copyToIndex++);
    copyVertex(vertexG, copyToIndex++);
    
    copyVertex(vertexG, copyToIndex++);
    copyVertex(vertexG, copyToIndex++);
    copyVertex(vertexH, copyToIndex++);
    
    copyVertex(vertexG, copyToIndex++);
    copyVertex(vertexH, copyToIndex++);
    copyVertex(vertexG, copyToIndex++);
    
    copyVertex(vertexG, copyToIndex++);
    copyVertex(vertexG, copyToIndex++);
    copyVertex(vertexH, copyToIndex++);
    
    CaretAssert(copyToIndex == vertexG);
}

/**
 * Get the point diameter.
 *
 * @param sizeTypeOut
 *     Type of sizing.
 * @param pointDiameterOut
 *     Diameter of point.
 */
void
GraphicsPrimitive::getPointDiameter(SizeType& sizeTypeOut,
                                    float& pointDiameterOut) const
{
    sizeTypeOut  = m_pointSizeType;
    pointDiameterOut = m_pointDiameterValue;
}

/**
 * Set the point diameter.
 *
 * @param sizeType
 *     Type of sizing.
 * @param pointDiameter
 *     Diameter of point.
 */
void
GraphicsPrimitive::setPointDiameter(const SizeType sizeType,
                                    const float pointDiameter)
{
    m_pointSizeType  = sizeType;
    m_pointDiameterValue = pointDiameter;
}

/**
 * Get the line width.
 *
 * @param sizeTypeOut
 *     Type of sizing.
 * @param lineWidthOut
 *     Width of line.
 */
void
GraphicsPrimitive::getLineWidth(SizeType& widthTypeOut,
                                float lineWidthOut) const
{
    widthTypeOut = m_lineWidthType;
    lineWidthOut = m_lineWidthValue;
}

/**
 * Set the line width.
 *
 * @param sizeType
 *     Type of sizing.
 * @param lineWidth
 *     Width of line.
 */
void
GraphicsPrimitive::setLineWidth(const SizeType widthType,
                                const float lineWidth)
{
    m_lineWidthType = widthType;
    m_lineWidthValue = lineWidth;
}

/**
 * Get the point diameter.
 *
 * @param sizeTypeOut
 *     Type of sizing.
 * @param pointDiameterOut
 *     Diameter of point.
 */
void
GraphicsPrimitive::getSphereDiameter(SizeType& sizeTypeOut,
                                     float& sphereDiameterOut) const
{
    sizeTypeOut  = m_sphereSizeType;
    sphereDiameterOut = m_sphereDiameterValue;
}

/**
 * Set the point diameter.
 *
 * @param sizeType
 *     Type of sizing.
 * @param pointDiameter
 *     Diameter of point.
 */
void
GraphicsPrimitive::setSphereDiameter(const SizeType sizeType,
                                     const float sphereDiameter)
{
    m_sphereSizeType  = sizeType;
    m_sphereDiameterValue = sphereDiameter;
}

/**
 * Set the image for the texture.
 *
 * @param imageBytesRGBA
 *     Bytes containing the image data.  4 bytes per pixel.
 * @param imageWidth
 *     Width of the actual image.
 * @param imageHeight
 *     Height of the image.
 */
void
GraphicsPrimitive::setTextureImage(const uint8_t* imageBytesRGBA,
                                   const int32_t imageWidth,
                                   const int32_t imageHeight)
{
    m_textureImageBytesRGBA.clear();
    m_textureImageWidth  = -1;
    m_textureImageHeight = -1;
    
    const int32_t numBytes = imageWidth * imageHeight * 4;
    if (numBytes > 0) {
        m_textureImageBytesRGBA.reserve(numBytes);
        m_textureImageWidth  = imageWidth;
        m_textureImageHeight = imageHeight;
        m_textureImageBytesRGBA.insert(m_textureImageBytesRGBA.end(),
                                       imageBytesRGBA, imageBytesRGBA + numBytes);
    }
}

/**
 * Get the OpenGL graphics engine data in this instance.
 *
 * @return
 *     OpenGL graphics engine data or NULL if not found.
 */
GraphicsEngineDataOpenGL*
GraphicsPrimitive::getGraphicsEngineDataForOpenGL()
{
    return m_graphicsEngineDataForOpenGL.get();
}

/**
 * Set the OpenGL graphics engine data in this instance.
 *
 * @param graphicsEngineForOpenGL
 *     OpenGLgraphics engine for which graphics engine data is desired.  This value may
 *     NULL to remove graphics engine data for the given graphics engine.
 *     This instance will take ownership of this data and handle deletion of it.
 */
void
GraphicsPrimitive::setGraphicsEngineDataForOpenGL(GraphicsEngineDataOpenGL* graphicsEngineDataForOpenGL)
{
    m_graphicsEngineDataForOpenGL.reset(graphicsEngineDataForOpenGL);
}

/**
 * Receive an event.
 *
 * @param event
 *    An event for which this instance is listening.
 */
void
GraphicsPrimitive::receiveEvent(Event* /*event*/)
{
//    if (event->getEventType() == EventTypeEnum::) {
//        <EVENT_CLASS_NAME*> eventName = dynamic_cast<EVENT_CLASS_NAME*>(event);
//        CaretAssert(eventName);
//
//        event->setEventProcessed();
//    }
}

/**
 * @return A new primitive for XYZ with solid color float RGBA.  Caller is responsible
 * for deleting the returned pointer.
 *
 * @param primitiveType
 *     Type of primitive drawn (triangles, lines, etc.)
 */
GraphicsPrimitiveV3f*
GraphicsPrimitive::newPrimitiveV3f(const GraphicsPrimitive::PrimitiveType primitiveType,
                                 const float floatRGBA[4])
{
    GraphicsPrimitiveV3f* primitive = new GraphicsPrimitiveV3f(primitiveType,
                                                               floatRGBA);
    return primitive;
}

/**
 * @return A new primitive for XYZ with solid color float RGBA.  Caller is responsible
 * for deleting the returned pointer.
 *
 * @param primitiveType
 *     Type of primitive drawn (triangles, lines, etc.)
 */
GraphicsPrimitiveV3fN3f*
GraphicsPrimitive::newPrimitiveV3fN3f(const GraphicsPrimitive::PrimitiveType primitiveType,
                                      const uint8_t unsignedByteRGBA[4])
{
    GraphicsPrimitiveV3fN3f* primitive = new GraphicsPrimitiveV3fN3f(primitiveType,
                                                                     unsignedByteRGBA);
    return primitive;
}

/**
 * @return A new primitive for XYZ with solid color unsigned byte RGBA.  Caller is responsible
 * for deleting the returned pointer.
 *
 * @param primitiveType
 *     Type of primitive drawn (triangles, lines, etc.)
 */
GraphicsPrimitiveV3f*
GraphicsPrimitive::newPrimitiveV3f(const GraphicsPrimitive::PrimitiveType primitiveType,
                                 const uint8_t unsignedByteRGBA[4])
{
    GraphicsPrimitiveV3f* primitive = new GraphicsPrimitiveV3f(primitiveType,
                                                               unsignedByteRGBA);
    return primitive;
}

/**
 * @return A new primitive for XYZ with float RGBA.  Caller is responsible
 * for deleting the returned pointer.
 *
 * @param primitiveType
 *     Type of primitive drawn (triangles, lines, etc.)
 */
GraphicsPrimitiveV3fC4f*
GraphicsPrimitive::newPrimitiveV3fC4f(const GraphicsPrimitive::PrimitiveType primitiveType)
{
    GraphicsPrimitiveV3fC4f* primitive = new GraphicsPrimitiveV3fC4f(primitiveType);
    return primitive;
}

/**
 * @return A new primitive for XYZ with unsigned byte RGBA.  Caller is responsible
 * for deleting the returned pointer.
 *
 * @param primitiveType
 *     Type of primitive drawn (triangles, lines, etc.)
 */
GraphicsPrimitiveV3fC4ub*
GraphicsPrimitive::newPrimitiveV3fC4ub(const GraphicsPrimitive::PrimitiveType primitiveType)
{
    GraphicsPrimitiveV3fC4ub* primitive = new GraphicsPrimitiveV3fC4ub(primitiveType);
    return primitive;
}

GraphicsPrimitiveV3fT3f*
GraphicsPrimitive::newPrimitiveV3fT3f(const GraphicsPrimitive::PrimitiveType primitiveType,
                                                   const uint8_t* imageBytesRGBA,
                                                   const int32_t imageWidth,
                                                   const int32_t imageHeight)
{
    GraphicsPrimitiveV3fT3f* primitive = new GraphicsPrimitiveV3fT3f(primitiveType,
                                                                     imageBytesRGBA,
                                                                     imageWidth,
                                                                     imageHeight);
    return primitive;
}

