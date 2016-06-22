
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

#define __SCENE_ENUMERATED_TYPE_ARRAY_DECLARE__
#include "SceneEnumeratedTypeArray.h"
#undef __SCENE_ENUMERATED_TYPE_ARRAY_DECLARE__

#include "CaretAssert.h"

using namespace caret;


    
/**
 * \class caret::SceneEnumeratedTypeArray 
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
SceneEnumeratedTypeArray::SceneEnumeratedTypeArray(const AString& name,
                                                   const AString enumeratedValuesAsStrings[],
                                                   const int32_t numberOfArrayElements)
: SceneObjectArray(name,
                 SceneObjectDataTypeEnum::SCENE_ENUMERATED_TYPE)
{
    m_values.resize(numberOfArrayElements);
    for (int32_t i = 0; i < numberOfArrayElements; i++) {
        m_values[i] = enumeratedValuesAsStrings[i];
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
SceneEnumeratedTypeArray::SceneEnumeratedTypeArray(const AString& name,
                                                   const std::vector<AString>& enumeratedValuesAsStrings)
: SceneObjectArray(name,
                      SceneObjectDataTypeEnum::SCENE_ENUMERATED_TYPE)
{
    m_values = enumeratedValuesAsStrings;
}

/**
 * Constructor that initializes all values to false.
 *
 * @param name
 *   Name of object.
 * @param numberOfArrayElements
 *   Number of values in the array.
 */
SceneEnumeratedTypeArray::SceneEnumeratedTypeArray(const AString& name,
                                                   const int numberOfArrayElements)
: SceneObjectArray(name,
                      SceneObjectDataTypeEnum::SCENE_ENUMERATED_TYPE)
{
    m_values.resize(numberOfArrayElements);
    std::fill(m_values.begin(),
              m_values.end(),
              "");
}

SceneEnumeratedTypeArray::SceneEnumeratedTypeArray(const SceneEnumeratedTypeArray& rhs): SceneObjectArray(rhs.getName(), SceneObjectDataTypeEnum::SCENE_ENUMERATED_TYPE)
{
    m_values = rhs.m_values;
}

/**
 * Destructor.
 */
SceneEnumeratedTypeArray::~SceneEnumeratedTypeArray()
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
SceneEnumeratedTypeArray::setValue(const int32_t arrayIndex,
                                   const AString enumeratedValueAsString)
{
    CaretAssertVectorIndex(m_values, arrayIndex);
    m_values[arrayIndex] = enumeratedValueAsString;
}

/** 
 * Get the values as a string. 
 * @param arrayIndex
 *    Index of element.
 * @return The value.
 */
AString 
SceneEnumeratedTypeArray::stringValue(const int32_t arrayIndex) const
{    
    CaretAssertVectorIndex(m_values, arrayIndex);
    return m_values[arrayIndex];
}

/**
 * Load the array with string values of the enumerated type.
 * @param valuesOut
 *    Array into which string values are loaded.
 * @param arrayNumberOfElements
 *    Number of elements in the array.  If this value is greater
 *    than the number of elements in the scene, the remaining
 *    elements will be filled with the default value
 * @param defaultValue
 *    Default value used when output array contains more elements
 *    than are in this instance's array.
 */
void 
SceneEnumeratedTypeArray::stringValues(AString enumeratedValuesAsString[],
                  const int32_t numberOfArrayElements,
                  const AString defaultValue) const
{
    const int32_t numElem = std::min(numberOfArrayElements,
                                     getNumberOfArrayElements());
    for (int32_t i = 0; i < numElem; i++) {
        enumeratedValuesAsString[i] = m_values[i];
    }
    
    for (int32_t i = numElem; i < numberOfArrayElements; i++) {//TSC: this used to use number of elements in internal array, not output array
        enumeratedValuesAsString[i] = defaultValue;
    }
}


