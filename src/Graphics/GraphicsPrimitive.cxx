
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

#include "CaretAssert.h"
#include "CaretLogger.h"
#include "EventManager.h"
#include "GraphicsEngineDataOpenGL.h"
#include "GraphicsPrimitiveV3f.h"
#include "GraphicsPrimitiveV3fC4f.h"
#include "GraphicsPrimitiveV3fC4ub.h"

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
 * @param primitiveType
 *     Type of primitive drawn (triangles, lines, etc.)
 */
GraphicsPrimitive::GraphicsPrimitive(const VertexType       vertexType,
                                     const NormalVectorType normalVectorType,
                                     const ColorType        colorType,
                                     const PrimitiveType    primitiveType)
: CaretObject(),
 EventListenerInterface(),
 m_vertexType(vertexType),
 m_normalVectorType(normalVectorType),
 m_colorType(colorType),
 m_primitiveType(primitiveType)
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
m_primitiveType(obj.m_primitiveType)
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
    m_xyz       = obj.m_xyz;
    m_floatNormalVectorXYZ = obj.m_floatNormalVectorXYZ;
    m_floatRGBA = obj.m_floatRGBA;
    m_unsignedByteRGBA = obj.m_unsignedByteRGBA;
    m_graphicsEngineDataForOpenGL.reset();
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
        
        uint32_t numColorRGBA = 0;
        switch (m_colorType) {
            case ColorType::FLOAT_RGBA:
                numColorRGBA = m_floatRGBA.size();
                break;
            case ColorType::UNSIGNED_BYTE_RGBA:
                numColorRGBA = m_unsignedByteRGBA.size();
                break;
        }
        if ((numXYZ / 3) != (numColorRGBA / 4)) {
            CaretLogWarning("ERROR: GraphicsPrimitive XYZ size does not match RGBA size");
            return false;
        }
        
        switch (m_primitiveType) {
            case PrimitiveType::LINE_LOOP:
                if (numXYZ < 3) {
                    CaretLogWarning("Line loop must have at least 3 vertices.");
                }
                break;
            case PrimitiveType::LINE_STRIP:
                if (numXYZ < 2) {
                    CaretLogWarning("Line strip must have at least 2 vertices.");
                }
                break;
            case PrimitiveType::LINES:
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
            case PrimitiveType::POINTS:
                break;
            case PrimitiveType::POLYGON:
                if (numXYZ < 3) {
                    CaretLogWarning("Polygon must have at least 3 vertices.");
                }
                break;
            case PrimitiveType::QUAD_STRIP:
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
            case PrimitiveType::QUADS:
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
            case PrimitiveType::TRIANGLE_FAN:
                if (numXYZ < 3) {
                    CaretLogWarning("Triangle fan must have at least 3 vertices.");
                }
                break;
            case PrimitiveType::TRIANGLE_STRIP:
                if (numXYZ < 3) {
                    CaretLogWarning("Triangle strip must have at least 3 vertices.");
                }
                break;
            case PrimitiveType::TRIANGLES:
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
 * Convert to string.
 *
 * @param includeAllDataFlag
 *     If true, numerical values are included.  Else, only type of data.
 */
AString
GraphicsPrimitive::toStringPrivate(const bool includeAllDataFlag) const
{
    AString s;
    
    switch (m_primitiveType) {
        case PrimitiveType::LINE_LOOP:
            s.appendWithNewLine("Primitive: Line Loop");
            break;
        case PrimitiveType::LINE_STRIP:
            s.appendWithNewLine("Primitive: Line Strip");
            break;
        case PrimitiveType::LINES:
            s.appendWithNewLine("Primitive: Lines");
            break;
        case PrimitiveType::POINTS:
            s.appendWithNewLine("Primitive: Points");
            break;
        case PrimitiveType::POLYGON:
            s.appendWithNewLine("Primitive: Polygon");
            break;
        case PrimitiveType::QUAD_STRIP:
            s.appendWithNewLine("Primitive: Quad Strip");
            break;
        case PrimitiveType::QUADS:
            s.appendWithNewLine("Primitive: Quads");
            break;
        case PrimitiveType::TRIANGLE_FAN:
            s.appendWithNewLine("Primitive: Triangle Fan");
            break;
        case PrimitiveType::TRIANGLE_STRIP:
            s.appendWithNewLine("Primitive: Triangle Strip");
            break;
        case PrimitiveType::TRIANGLES:
            s.appendWithNewLine("Primitive: Triangles");
            break;
    }
    switch (m_vertexType) {
        case VertexType::FLOAT_XYZ:
            s.appendWithNewLine("Vertex: " + AString::number(m_xyz.size()) + " Float XYZ.  ");
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
    
    switch (m_normalVectorType) {
        case NormalVectorType::FLOAT_XYZ:
            s.appendWithNewLine("Normal Vector: " + AString::number(m_floatNormalVectorXYZ.size()) + " Float XYZ.  ");
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
        case ColorType::FLOAT_RGBA:
            s.appendWithNewLine("Color: " + AString::number(m_floatRGBA.size()) + " Float RGBA 0.0 to 1.0.  ");
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
            s.appendWithNewLine("Color: " + AString::number(m_unsignedByteRGBA.size()) + " Unsigned Byte RGBA  0 to 255");
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
    
    return s;
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
GraphicsPrimitive::receiveEvent(Event* event)
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

