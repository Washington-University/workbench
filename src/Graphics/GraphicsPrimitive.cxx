
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
    m_floatRGBA = obj.m_floatRGBA;
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
            case PrimitiveType::QUADS:
            {
                const uint32_t extraVertices = numXYZ % 4;
                if (extraVertices > 0) {
                    CaretLogWarning("Extra vertices for drawing quads ignored.");
                }
            }
                break;
            case PrimitiveType::TRIANGLES:
            {
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
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString
GraphicsPrimitive::toString() const
{
    AString s;
    
    switch (m_vertexType) {
        case VertexType::FLOAT_XYZ:
            s.append("Vertex: " + AString::number(m_xyz.size()) + " Float XYZ.  ");
            break;
    }
    
    switch (m_normalVectorType) {
        case NormalVectorType::FLOAT_XYZ:
            s.append("Normal Vector: " + AString::number(m_floatNormalVectorXYZ.size()) + " Float XYZ.  ");
            break;
        case NormalVectorType::NONE:
            s.append("Normal Vector: None.  ");
            break;
    }
    
    switch (m_colorType) {
        case ColorType::FLOAT_RGBA:
            s.append("Color: " + AString::number(m_floatRGBA.size()) + " Float RGBA 0.0 to 1.0.  ");
            break;
        case ColorType::UNSIGNED_BYTE_RGBA:
            s.append("Color: " + AString::number(m_unsignedByteRGBA.size()) + " Unsigned Byte RGBA  0 to 255");
            break;
    }
    
    return s;
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

