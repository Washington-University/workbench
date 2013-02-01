
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
                 SceneObjectDataTypeEnum::SCENE_ENUMERATED_TYPE,
                 numberOfArrayElements)
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
                      SceneObjectDataTypeEnum::SCENE_ENUMERATED_TYPE,
                      enumeratedValuesAsStrings.size())
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
                      SceneObjectDataTypeEnum::SCENE_ENUMERATED_TYPE,
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
                                     m_numberOfArrayElements);
    for (int32_t i = 0; i < numElem; i++) {
        enumeratedValuesAsString[i] = m_values[i];
    }
    
    for (int32_t i = numElem; i < m_numberOfArrayElements; i++) {
        enumeratedValuesAsString[i] = defaultValue;
    }
}


