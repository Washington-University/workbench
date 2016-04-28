
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

#define __SCENE_STRING_DECLARE__
#include "SceneString.h"
#undef __SCENE_STRING_DECLARE__

using namespace caret;


    
/**
 * \class caret::SceneString 
 * \brief For storage of a string value in a scene.
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
SceneString::SceneString(const AString& name,
                           const AString& value)
: ScenePrimitive(name,
                 SceneObjectDataTypeEnum::SCENE_STRING)
{
    m_value = value;
}

/**
 * Destructor.
 */
SceneString::~SceneString()
{
    
}

/**
 * Set the value.
 * @param value
 *   New value.
 */
void 
SceneString::setValue(const AString& value)
{
    m_value = value;
}

/**
 * @return The value as a boolean data type.
 */
bool 
SceneString::booleanValue() const
{
    const bool b = m_value.toBool();
    return b;
}

/**
 * @return The value as a float data type.
 * If the string does not convert to a float number,
 * 0.0 is returned.
 */
float
SceneString::floatValue() const
{
    bool isValid = false;
    float f = m_value.toFloat(&isValid);
    if (isValid == false) {
        f = 0.0;
    }
    return f;
}

/**
 * @return The value as a integer data type.
 * If the string does not convert to an integer number,
 * 0 is returned.
 */
int32_t 
SceneString::integerValue() const
{
    bool isValid = false;
    int32_t i = m_value.toInt(&isValid);
    if (isValid == false) {
        i = 0;
    }
    return i;
}

/**
 * @return The value as a string data type.
 */
AString 
SceneString::stringValue() const
{
    return m_value;
}

/**
 * Get the values as an unsigned byte.
 * @param arrayIndex
 *    Index of element.
 * @return The value.
 */
uint8_t
SceneString::unsignedByteValue() const
{
    bool isValid = false;
    const uint32_t i = m_value.toUInt(&isValid);
    if ( ! isValid) {
        return 0;
    }
    
    if (i > std::numeric_limits<uint8_t>::max()) {
        return std::numeric_limits<uint8_t>::max();
    }
    else if (i < std::numeric_limits<uint8_t>::min()) {
        return std::numeric_limits<uint8_t>::min();
    }
    
    const uint8_t b = static_cast<uint8_t>(i);
    return b;
}

