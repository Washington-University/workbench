
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

#define __GRAPHICS_PRIMITIVE_SELECTION_HELPER_DECLARE__
#include "GraphicsPrimitiveSelectionHelper.h"
#undef __GRAPHICS_PRIMITIVE_SELECTION_HELPER_DECLARE__

#include "CaretAssert.h"
#include "CaretLogger.h"
#include "GraphicsPrimitive.h"

using namespace caret;


    
/**
 * \class caret::GraphicsPrimitiveSelectionHelper 
 * \brief Helps with identification of graphics primitives
 * \ingroup Graphics
 */

/**
 * Constructor.
 */
GraphicsPrimitiveSelectionHelper::GraphicsPrimitiveSelectionHelper(GraphicsPrimitive* parentGraphicsPrimitive)
: CaretObject(),
m_parentGraphicsPrimitive(parentGraphicsPrimitive)
{
    CaretAssert(m_parentGraphicsPrimitive);
}

/**
 * Destructor.
 */
GraphicsPrimitiveSelectionHelper::~GraphicsPrimitiveSelectionHelper()
{
    
}

/**
 * Setup selection data just before drawing.
 */
void
GraphicsPrimitiveSelectionHelper::setupSelectionBeforeDrawing()
{
    int32_t numberOfVertices = 0;
    
    switch (m_parentGraphicsPrimitive->getVertexDataType()) {
        case GraphicsPrimitive::VertexDataType::FLOAT_XYZ:
            numberOfVertices = m_parentGraphicsPrimitive->getFloatXYZ().size() / 3;
            break;
    }
    
    m_numberOfVerticesPerPrimitive = 0;
    m_vertexOffsetForPrimitive     = 0;
    
    switch (m_parentGraphicsPrimitive->getPrimitiveType()) {
        case GraphicsPrimitive::PrimitiveType::OPENGL_LINE_LOOP:
        case GraphicsPrimitive::PrimitiveType::MODEL_SPACE_POLYGONAL_LINE_LOOP_BEVEL_JOIN:
        case GraphicsPrimitive::PrimitiveType::MODEL_SPACE_POLYGONAL_LINE_LOOP_MITER_JOIN:
        case GraphicsPrimitive::PrimitiveType::POLYGONAL_LINE_LOOP_BEVEL_JOIN:
        case GraphicsPrimitive::PrimitiveType::POLYGONAL_LINE_LOOP_MITER_JOIN:
            m_numberOfVerticesPerPrimitive = numberOfVertices;
            break;
        case GraphicsPrimitive::PrimitiveType::MODEL_SPACE_POLYGONAL_LINE_STRIP_BEVEL_JOIN:
        case GraphicsPrimitive::PrimitiveType::MODEL_SPACE_POLYGONAL_LINE_STRIP_MITER_JOIN:
        case GraphicsPrimitive::PrimitiveType::OPENGL_LINE_STRIP:
        case GraphicsPrimitive::PrimitiveType::POLYGONAL_LINE_STRIP_BEVEL_JOIN:
        case GraphicsPrimitive::PrimitiveType::POLYGONAL_LINE_STRIP_MITER_JOIN:
            m_numberOfVerticesPerPrimitive = numberOfVertices;
            break;
        case GraphicsPrimitive::PrimitiveType::MODEL_SPACE_POLYGONAL_LINES:
        case GraphicsPrimitive::PrimitiveType::OPENGL_LINES:
        case GraphicsPrimitive::PrimitiveType::POLYGONAL_LINES:
            m_numberOfVerticesPerPrimitive = 2;
            break;
        case GraphicsPrimitive::PrimitiveType::OPENGL_POINTS:
            m_numberOfVerticesPerPrimitive = 1;
            break;
        case GraphicsPrimitive::PrimitiveType::OPENGL_TRIANGLE_FAN:
            /*
             * Assuming vertices start at ZERO
             * Color is at vertex (i + 2):
             * Vertex[2] is color for first triangle,
             * Vertex[3] is color for second triangle...
             */
            m_numberOfVerticesPerPrimitive = 1;
            m_vertexOffsetForPrimitive     = 2;
            break;
        case GraphicsPrimitive::PrimitiveType::OPENGL_TRIANGLE_STRIP:
            /*
             * Assuming vertices start at ZERO
             * Color is at vertex (i + 2):
             * Vertex[2] is color for first triangle,
             * Vertex[3] is color for second triangle...
             */
            m_numberOfVerticesPerPrimitive = 1;
            m_vertexOffsetForPrimitive     = 2;
            break;
        case GraphicsPrimitive::PrimitiveType::OPENGL_TRIANGLES:
            m_numberOfVerticesPerPrimitive = 3;
            break;
        case GraphicsPrimitive::PrimitiveType::SPHERES:
            m_numberOfVerticesPerPrimitive = numberOfVertices;
            break;
    }
    
    if (numberOfVertices > 0) {
        const int32_t numberOfPrimitives = (numberOfVertices / m_numberOfVerticesPerPrimitive) - m_vertexOffsetForPrimitive;
        const int32_t maxPrimitivesSupported = 255 * 255 * 255;
        if (numberOfPrimitives > maxPrimitivesSupported) {
            const AString msg("Number of primitives for selection="
                              + AString::number(numberOfPrimitives)
                              + " exceeds maximum allowed="
                              + AString::number(maxPrimitivesSupported));
            CaretAssertMessage(0, msg);
            CaretLogSevere(msg);
        }
        else {
            const int32_t selectionRgbaSize = numberOfVertices * 4;
            
            if (selectionRgbaSize != static_cast<int32_t>(m_selectionEncodedRGBA.size())) {
                const GraphicsPrimitive::ColorDataType colorType = m_parentGraphicsPrimitive->getColorDataType();
                
                m_selectionEncodedRGBA.resize(selectionRgbaSize);
                
                /*
                 * Encode the primitive index into the RGBA selection data.
                 */
                int32_t indexRGBA = 0;
                const int32_t startIndex = m_vertexOffsetForPrimitive;
                const int32_t endIndex   = m_vertexOffsetForPrimitive + numberOfPrimitives;
                for (int32_t primitiveIndex = startIndex; primitiveIndex < endIndex; primitiveIndex++) {
                    uint8_t blue  = (uint8_t)(primitiveIndex         & 0xff);
                    uint8_t green = (uint8_t)((primitiveIndex >> 8)  & 0xff);
                    uint8_t red   = (uint8_t)((primitiveIndex >> 16) & 0xff);
                    
                    /*
                     * Same ID color at all vertices of a primitive
                     */
                    for (int32_t j = 0; j < m_numberOfVerticesPerPrimitive; j++) {
                        CaretAssertVectorIndex(m_selectionEncodedRGBA, indexRGBA+3);
                        m_selectionEncodedRGBA[indexRGBA]   = red;
                        m_selectionEncodedRGBA[indexRGBA+1] = green;
                        m_selectionEncodedRGBA[indexRGBA+2] = blue;
                        
                        uint8_t vertexAlpha = 0;
                        const int32_t vertexAlphaIndex = indexRGBA + 3;
                        switch (colorType) {
                            case GraphicsPrimitive::ColorDataType::NONE:
                                vertexAlpha = 255;
                                break;
                            case GraphicsPrimitive::ColorDataType::FLOAT_RGBA:
                                CaretAssertVectorIndex(m_parentGraphicsPrimitive->m_floatRGBA,
                                                       vertexAlphaIndex);
                                vertexAlpha = static_cast<uint8_t>(m_parentGraphicsPrimitive->m_floatRGBA[vertexAlphaIndex] * 255);
                                break;
                            case GraphicsPrimitive::ColorDataType::UNSIGNED_BYTE_RGBA:
                                CaretAssertVectorIndex(m_parentGraphicsPrimitive->m_unsignedByteRGBA,
                                                       vertexAlphaIndex);
                                vertexAlpha = m_parentGraphicsPrimitive->m_unsignedByteRGBA[vertexAlphaIndex];
                                break;
                        }
                        
                        m_selectionEncodedRGBA[indexRGBA+3] = vertexAlpha;
                        
                        if (primitiveIndex == startIndex) {
                            if (startIndex > 0) {
                                /*
                                 * For THE FIRST triangle in a triangle strip or fan, 
                                 * its color is at vertex 2 (assuming first index is ZERO).
                                 * Here we copy the color from vertex 2 to vertices
                                 * 0 and 1.  This is not absolutely necessary but it 
                                 * ensures correct color values and avoid a possible
                                 * NaN from unintialized values.
                                 */
                                for (int32_t m = 0; m < startIndex; m++) {
                                    const int32_t m4 = m * 4;
                                    for (int32_t n = 0; n < 4; n++) {
                                        m_selectionEncodedRGBA[m4 + n] = m_selectionEncodedRGBA[indexRGBA + n];
                                    }
                                }
                            }
                        }
                        indexRGBA += 4;
                    }
                }
            }
        }
    }
    else {
        m_selectionEncodedRGBA.clear();
    }
}

/**
 * Get the index of the primitive that was selected using the encoded RGBA.
 *
 * @param rgba
 *     RGBA with encoded selection information.
 * @return
 *     Index of selected primitive or -1 if none selected.
 */
int32_t
GraphicsPrimitiveSelectionHelper::getPrimitiveIndexFromEncodedRGBA(const uint8_t rgba[4]) const
{
    int32_t primitiveIndex(-1);
    
    const int32_t numberOfVertices = static_cast<int32_t>(m_selectionEncodedRGBA.size() / 4);
    if (numberOfVertices > 0) {
        CaretAssert(m_numberOfVerticesPerPrimitive > 0);
        for (int32_t i = 0; i < numberOfVertices; i++) {
            const int32_t i4 = i * 4;
            CaretAssertVectorIndex(m_selectionEncodedRGBA, i4+3);
            if ((rgba[0] == m_selectionEncodedRGBA[i4])
                && (rgba[1] == m_selectionEncodedRGBA[i4+1])
                && (rgba[2] == m_selectionEncodedRGBA[i4+2])) {
                if (m_selectionEncodedRGBA[i4+3] > 0) {
                    primitiveIndex = (i / m_numberOfVerticesPerPrimitive) + m_vertexOffsetForPrimitive;
                }
                break;
            }
        }
    }

    return primitiveIndex;
}

/**
 * @return The RGBA data in which identification information is encoded.
 */
const
std::vector<uint8_t>&
GraphicsPrimitiveSelectionHelper::getSelectionEncodedRGBA() const
{
    return m_selectionEncodedRGBA;
}


/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
GraphicsPrimitiveSelectionHelper::toString() const
{
    return "GraphicsPrimitiveSelectionHelper";
}

