
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

#define __SCENE_STRING_ARRAY_DECLARE__
#include "SceneStringArray.h"
#undef __SCENE_STRING_ARRAY_DECLARE__

#include "CaretAssert.h"

using namespace caret;


    
/**
 * \class caret::SceneStringArray 
 * \brief For storage of a boolean value in a scene.
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
SceneStringArray::SceneStringArray(const AString& name,
                                     const AString values[],
                                     const int32_t numberOfArrayElements)
: ScenePrimitiveArray(name,
                 SceneObjectDataTypeEnum::SCENE_STRING)
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
SceneStringArray::SceneStringArray(const AString& name,
                                     const std::vector<AString>& values)
: ScenePrimitiveArray(name,
                      SceneObjectDataTypeEnum::SCENE_STRING)
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
SceneStringArray::SceneStringArray(const AString& name,
                                     const int numberOfArrayElements)
: ScenePrimitiveArray(name,
                      SceneObjectDataTypeEnum::SCENE_STRING)
{
    m_values.resize(numberOfArrayElements);
    std::fill(m_values.begin(),
              m_values.end(),
              "");
}

SceneStringArray::SceneStringArray(const SceneStringArray& rhs): ScenePrimitiveArray(rhs.getName(), SceneObjectDataTypeEnum::SCENE_STRING)
{
    m_values = rhs.m_values;
}

/**
 * Destructor.
 */
SceneStringArray::~SceneStringArray()
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
SceneStringArray::setValue(const int32_t arrayIndex,
                            const AString value)
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
SceneStringArray::booleanValue(const int32_t arrayIndex) const
{    
    m_restoredFlag = true;
    CaretAssertVectorIndex(m_values, arrayIndex);
    const bool b = m_values[arrayIndex].toBool();
    return b;
}

/** 
 * Get the values as a float. 
 * @param arrayIndex
 *    Index of element.
 * @return The value.
 */
float 
SceneStringArray::floatValue(const int32_t arrayIndex) const
{    
    m_restoredFlag = true;
    CaretAssertVectorIndex(m_values, arrayIndex);
    bool isValid = false;
    float f = m_values[arrayIndex].toFloat(&isValid);
    if (isValid == false) {
        f = 0.0;
    }
    return f;
}

/** 
 * Get the values as a integer. 
 * @param arrayIndex
 *    Index of element.
 * @return The value.
 */
int32_t 
SceneStringArray::integerValue(const int32_t arrayIndex) const
{    
    m_restoredFlag = true;
    CaretAssertVectorIndex(m_values, arrayIndex);
    bool isValid = false;
    int32_t i = m_values[arrayIndex].toInt(&isValid);
    if (isValid == false) {
        i = 0;
    }
    return i;
}

/** 
 * Get the values as a string. 
 * @param arrayIndex
 *    Index of element.
 * @return The value.
 */
AString 
SceneStringArray::stringValue(const int32_t arrayIndex) const
{    
    m_restoredFlag = true;
    CaretAssertVectorIndex(m_values, arrayIndex);
    return m_values[arrayIndex];
}

/**
 * Get the values as a integer.
 * @param arrayIndex
 *    Index of element.
 * @return The value.
 */
uint8_t
SceneStringArray::unsignedByteValue(const int32_t arrayIndex) const
{
    m_restoredFlag = true;
    bool isValid = false;
    const uint32_t i = m_values[arrayIndex].toUInt(&isValid);
    if ( ! isValid) {
        return 0;
    }
    
    /*
     * Note since "i" is unsigned there is no need
     * to compare with std::numeric_limits<uint8_t>::min()
     * since it is zero.
     */
    if (i > std::numeric_limits<uint8_t>::max()) {
        return std::numeric_limits<uint8_t>::max();
    }
    
    const uint8_t b = static_cast<uint8_t>(i);
    return b;
}


