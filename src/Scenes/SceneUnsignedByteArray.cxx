
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

#include <algorithm>

#define __SCENE_UNSIGNED_BYTE_ARRAY_DECLARE__
#include "SceneUnsignedByteArray.h"
#undef __SCENE_UNSIGNED_BYTE_ARRAY_DECLARE__

#include "CaretAssert.h"

using namespace caret;


    
/**
 * \class caret::SceneUnsignedByteArray 
 * \brief For storage of a integer value in a scene.
 * \ingroup Scene
 *
 * See the documentation in the class Scene for how to use the Scene system.
 */

/**
 * Constructor.
 *
 * @param name
 *   Name of object.
 * @param values
 *   Value in the array.
 * @param numberOfArrayElements
 *   Number of values in the array.
 */
SceneUnsignedByteArray::SceneUnsignedByteArray(const AString& name,
                                     const uint8_t values[],
                                     const int32_t numberOfArrayElements)
: ScenePrimitiveArray(name,
                 SceneObjectDataTypeEnum::SCENE_UNSIGNED_BYTE)
{
    m_values.resize(numberOfArrayElements);
    for (int32_t i = 0; i < numberOfArrayElements; i++) {
        m_values[i] = values[i];
    }
}

/**
 * Constructor.
 *
 * @param name
 *   Name of object.
 * @param values
 *   Value in the array.
 */
SceneUnsignedByteArray::SceneUnsignedByteArray(const AString& name,
                                     const std::vector<uint8_t>& values)
: ScenePrimitiveArray(name,
                      SceneObjectDataTypeEnum::SCENE_UNSIGNED_BYTE)
{
    m_values = values;
}

/**
 * Constructor that initializes all values to false.
 *
 * @param name
 *   Name of object.
 * @param numberOfArrayElements
 *   Number of values in the array.
 */
SceneUnsignedByteArray::SceneUnsignedByteArray(const AString& name,
                                     const int numberOfArrayElements)
: ScenePrimitiveArray(name,
                      SceneObjectDataTypeEnum::SCENE_UNSIGNED_BYTE)
{
    m_values.resize(numberOfArrayElements);
    std::fill(m_values.begin(),
              m_values.end(),
              0);
}

SceneUnsignedByteArray::SceneUnsignedByteArray(const SceneUnsignedByteArray& rhs): ScenePrimitiveArray(rhs.getName(), SceneObjectDataTypeEnum::SCENE_UNSIGNED_BYTE)
{
    m_values = rhs.m_values;
}

/**
 * Destructor.
 */
SceneUnsignedByteArray::~SceneUnsignedByteArray()
{
    
}


/**
 * Set a value.
 * @param arrayIndex
 *    Index of the element.
 * @param value
 *    Value of element.
 */
void 
SceneUnsignedByteArray::setValue(const int32_t arrayIndex,
                            const uint8_t value)
{
    CaretAssertVectorIndex(m_values, arrayIndex);
    m_values[arrayIndex] = value;
}

/** 
 * Get the values as a boolean. 
 * @param arrayIndex
 *    Index of element.
 * @return The value.
 */
bool 
SceneUnsignedByteArray::booleanValue(const int32_t arrayIndex) const
{    
    CaretAssertVectorIndex(m_values, arrayIndex);
    const bool b = ((m_values[arrayIndex] != 0.0) ? true : false);
    return b;
}

/** 
 * Get the values as a float. 
 * @param arrayIndex
 *    Index of element.
 * @return The value.
 */
float 
SceneUnsignedByteArray::floatValue(const int32_t arrayIndex) const
{    
    CaretAssertVectorIndex(m_values, arrayIndex);
    const float f = static_cast<float>(m_values[arrayIndex]);
    return f;
}

/** 
 * Get the values as a integer. 
 * @param arrayIndex
 *    Index of element.
 * @return The value.
 */
int32_t 
SceneUnsignedByteArray::integerValue(const int32_t arrayIndex) const
{    
    CaretAssertVectorIndex(m_values, arrayIndex);
    const int32_t i = static_cast<int32_t>(m_values[arrayIndex]);
    return i;
}

/** 
 * Get the values as a string. 
 * @param arrayIndex
 *    Index of element.
 * @return The value.
 */
AString 
SceneUnsignedByteArray::stringValue(const int32_t arrayIndex) const
{    
    CaretAssertVectorIndex(m_values, arrayIndex);
    const AString s = AString::number(m_values[arrayIndex]);
    return s;
}

/**
 * Get the values as an unsigned byte.
 * @param arrayIndex
 *    Index of element.
 * @return The value.
 */
uint8_t
SceneUnsignedByteArray::unsignedByteValue(const int32_t arrayIndex) const
{
    CaretAssertVectorIndex(m_values, arrayIndex);
    return m_values[arrayIndex];
}


