
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
#include <limits>

#define __SCENE_INTEGER_ARRAY_DECLARE__
#include "SceneIntegerArray.h"
#undef __SCENE_INTEGER_ARRAY_DECLARE__

#include "CaretAssert.h"

using namespace caret;


    
/**
 * \class caret::SceneIntegerArray 
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
SceneIntegerArray::SceneIntegerArray(const AString& name,
                                     const int32_t values[],
                                     const int32_t numberOfArrayElements)
: ScenePrimitiveArray(name,
                 SceneObjectDataTypeEnum::SCENE_INTEGER)
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
SceneIntegerArray::SceneIntegerArray(const AString& name,
                                     const std::vector<int32_t>& values)
: ScenePrimitiveArray(name,
                      SceneObjectDataTypeEnum::SCENE_INTEGER)
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
SceneIntegerArray::SceneIntegerArray(const AString& name,
                                     const int numberOfArrayElements)
: ScenePrimitiveArray(name,
                      SceneObjectDataTypeEnum::SCENE_INTEGER)
{
    m_values.resize(numberOfArrayElements);
    std::fill(m_values.begin(),
              m_values.end(),
              0);
}

SceneIntegerArray::SceneIntegerArray(const SceneIntegerArray& rhs): ScenePrimitiveArray(rhs.getName(), SceneObjectDataTypeEnum::SCENE_INTEGER)
{
    m_values = rhs.m_values;
}

/**
 * Destructor.
 */
SceneIntegerArray::~SceneIntegerArray()
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
SceneIntegerArray::setValue(const int32_t arrayIndex,
                            const int32_t value)
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
SceneIntegerArray::booleanValue(const int32_t arrayIndex) const
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
SceneIntegerArray::floatValue(const int32_t arrayIndex) const
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
SceneIntegerArray::integerValue(const int32_t arrayIndex) const
{    
    CaretAssertVectorIndex(m_values, arrayIndex);
    return m_values[arrayIndex];
}

/** 
 * Get the values as a string. 
 * @param arrayIndex
 *    Index of element.
 * @return The value.
 */
AString 
SceneIntegerArray::stringValue(const int32_t arrayIndex) const
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
SceneIntegerArray::unsignedByteValue(const int32_t arrayIndex) const
{
    CaretAssertVectorIndex(m_values, arrayIndex);
    const int32_t i = m_values[arrayIndex];
    if (i > std::numeric_limits<uint8_t>::max()) {
        return std::numeric_limits<uint8_t>::max();
    }
    else if (i < std::numeric_limits<uint8_t>::min()) {
        return std::numeric_limits<uint8_t>::min();
    }
    
    const uint8_t b = static_cast<uint8_t>(i);
    return b;
}

