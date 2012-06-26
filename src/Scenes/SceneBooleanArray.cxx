
/*LICENSE_START*/
/*
 * Copyright 2012 Washington University,
 * All rights reserved.
 *
 * Connectome DB and Connectome Workbench are part of the integrated Connectome 
 * Informatics Platform.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    * Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *    * Neither the names of Washington University nor the
 *      names of its contributors may be used to endorse or promote products
 *      derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR  
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
 * SUBSTITUTE GOODS OR  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
 * POSSIBILITY OF SUCH DAMAGE.
 */
/*LICENSE_END*/

#include <algorithm>

#define __SCENE_BOOLEAN_ARRAY_DECLARE__
#include "SceneBooleanArray.h"
#undef __SCENE_BOOLEAN_ARRAY_DECLARE__

#include "CaretAssert.h"

using namespace caret;


    
/**
 * \class caret::SceneBooleanArray 
 * \brief For storage of a boolean value in a scene.
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
SceneBooleanArray::SceneBooleanArray(const AString& name,
                                     const bool values[],
                                     const int32_t numberOfArrayElements)
: ScenePrimitiveArray(name,
                 SceneObjectDataTypeEnum::SCENE_BOOLEAN,
                 numberOfArrayElements)
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
SceneBooleanArray::SceneBooleanArray(const AString& name,
                                     const std::vector<bool>& values)
: ScenePrimitiveArray(name,
                      SceneObjectDataTypeEnum::SCENE_BOOLEAN,
                      values.size())
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
SceneBooleanArray::SceneBooleanArray(const AString& name,
                                     const int numberOfArrayElements)
: ScenePrimitiveArray(name,
                      SceneObjectDataTypeEnum::SCENE_BOOLEAN,
                      numberOfArrayElements)
{
    m_values.resize(numberOfArrayElements);
    std::fill(m_values.begin(),
              m_values.end(),
              false);
}

/**
 * Destructor.
 */
SceneBooleanArray::~SceneBooleanArray()
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
SceneBooleanArray::setValue(const int32_t arrayIndex,
                            const bool value)
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
SceneBooleanArray::booleanValue(const int32_t arrayIndex) const
{    
    CaretAssertVectorIndex(m_values, arrayIndex);
    return m_values[arrayIndex];
}

/** 
 * Get the values as a float. 
 * @param arrayIndex
 *    Index of element.
 * @return The value.
 */
float 
SceneBooleanArray::floatValue(const int32_t arrayIndex) const
{    
    CaretAssertVectorIndex(m_values, arrayIndex);
    const float f = (m_values[arrayIndex] ? 1.0 : 0.0);
    return f;
}

/** 
 * Get the values as a integer. 
 * @param arrayIndex
 *    Index of element.
 * @return The value.
 */
int32_t 
SceneBooleanArray::integerValue(const int32_t arrayIndex) const
{    
    CaretAssertVectorIndex(m_values, arrayIndex);
    const float i = (m_values[arrayIndex] ? 1 : 0);
    return i;
}

/** 
 * Get the values as a string. 
 * @param arrayIndex
 *    Index of element.
 * @return The value.
 */
AString 
SceneBooleanArray::stringValue(const int32_t arrayIndex) const
{    
    CaretAssertVectorIndex(m_values, arrayIndex);
    const AString s = (m_values[arrayIndex] ? "true" : "false");
    return s;
}

/**
 * Load the array with boolean values.
 * @param valuesOut
 *    Array into which boolean values are loaded.
 * @param arrayNumberOfElements
 *    Number of elements in the array.  If this value is greater
 *    than the number of elements in the scene, the remaining
 *    elements will be filled with the value of 'defaultValue'.
 * @param defaultValue
 *    Value loaded into array when 'arrayNumberOfElements' is
 *    less than the number of elements in the scene.
 */
void 
SceneBooleanArray::booleanValues(bool valuesOut[],
                                 const int32_t arrayNumberOfElements,
                                 const bool defaultValue) const
{    
    const int32_t numElem = std::min(arrayNumberOfElements,
                                 m_numberOfArrayElements);
    for (int32_t i = 0; i < numElem; i++) {
        valuesOut[i] = m_values[i];
    }
    
    for (int32_t i = numElem; i < arrayNumberOfElements; i++) {
        valuesOut[i] = defaultValue;
    }
}

/**
 * Load the vector with the values from the scene.
 * @param valuesOut
 *     Vector into which values are loaded.  The vector
 *     will contain the number of elements that were saved
 *     to the scene.
 */
void 
SceneBooleanArray::booleanValues(std::vector<bool>& valuesOut) const
{    
    const int32_t vectorSize = static_cast<int32_t>(valuesOut.size());
    if (vectorSize != m_numberOfArrayElements) {
        valuesOut.resize(m_numberOfArrayElements);
    }
    
    for (int32_t i = 0; i < m_numberOfArrayElements; i++) {
        valuesOut[i] = m_values[i];
    }
}

