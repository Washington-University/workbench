
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

#include <limits>

#define __SCENE_UNSIGNED_BYTE_DECLARE__
#include "SceneUnsignedByte.h"
#undef __SCENE_UNSIGNED_BYTE_DECLARE__

using namespace caret;


    
/**
 * \class caret::SceneUnsignedByte 
 * \brief For storage of an integer value in a scene.
 * \ingroup Scene
 *
 * See the documentation in the class Scene for how to use the Scene system.
 */

/**
 * Constructor.
 *
 * @param name
 *   Name of object.
 * @param value
 *   Value of object.
 */
SceneUnsignedByte::SceneUnsignedByte(const AString& name,
                       const uint8_t value)
: ScenePrimitive(name,
                 SceneObjectDataTypeEnum::SCENE_UNSIGNED_BYTE)
{
    m_value = value;
}

SceneUnsignedByte::SceneUnsignedByte(const SceneUnsignedByte& rhs): ScenePrimitive(rhs.getName(), SceneObjectDataTypeEnum::SCENE_UNSIGNED_BYTE)
{
    m_value = rhs.m_value;
}

SceneObject* SceneUnsignedByte::clone() const
{
    return new SceneUnsignedByte(*this);
}

/**
 * Destructor.
 */
SceneUnsignedByte::~SceneUnsignedByte()
{
    
}

/**
 * Set the value.
 * @param value
 *    The new value.
 */
void 
SceneUnsignedByte::setValue(const uint8_t value)
{
    m_value = value;
}

/**
 * @return The value as a boolean data type.
 */
bool 
SceneUnsignedByte::booleanValue() const
{
    m_restoredFlag = true;
    const bool b = ((m_value != 0) ? true : false);
    return b;
}

/**
 * @return The value as a float data type.
 */
float
SceneUnsignedByte::floatValue() const
{
    m_restoredFlag = true;
    const float f = static_cast<float>(m_value);
    return f;
}

/**
 * @return The value as a integer data type.
 */
int32_t 
SceneUnsignedByte::integerValue() const
{
    m_restoredFlag = true;
    return m_value;
}

/**
 * @return The value as a string data type.
 */
AString 
SceneUnsignedByte::stringValue() const
{
    m_restoredFlag = true;
    const AString s = AString::number(m_value);
    return s;
}

/**
 * @return The value as an unsigned byte data type.
 */
uint8_t
SceneUnsignedByte::unsignedByteValue() const
{
    m_restoredFlag = true;
    return m_value;
}
