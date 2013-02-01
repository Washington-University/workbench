
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

#define __SCENE_PRIMITIVE_ARRAY_DECLARE__
#include "ScenePrimitiveArray.h"
#undef __SCENE_PRIMITIVE_ARRAY_DECLARE__

using namespace caret;


    
/**
 * \class caret::ScenePrimitiveArray 
 * \brief Abstract class for 'primitive' data types for scenes.
 * \ingroup Scene
 *
 * See the documentation in the class Scene for how to use the Scene system.
 *
 * Design loosely based upon Java.lang.Number but also including
 * boolean and string values.
 */

/**
 * Constructor.
 * @param name
 *    Name of primitive.
 * @param dataType
 *    Data type of the primitive.
 */
ScenePrimitiveArray::ScenePrimitiveArray(const QString& name,
                               const SceneObjectDataTypeEnum::Enum dataType,
                                         const int32_t numberOfArrayElements)
: SceneObjectArray(name, 
                   dataType,
                   numberOfArrayElements)
{
    
}

/**
 * Destructor.
 */
ScenePrimitiveArray::~ScenePrimitiveArray()
{
    
}

/**
 * Load the array with boolean values.
 * @param valuesOut
 *    Array into which boolean values are loaded.
 * @param arrayNumberOfElements
 *    Number of elements in the array.  If this value is greater
 *    than the number of elements in the scene, the remaining
 *    elements will be filled with the default value
 * @param defaultValue
 *    Default value used when output array contains more elements
 *    than are in this instance's array.
 */
void
ScenePrimitiveArray::booleanValues(bool valuesOut[],
                                   const int32_t arrayNumberOfElements,
                                   const bool defaultValue) const
{    
    const int32_t numElem = std::min(arrayNumberOfElements,
                                     m_numberOfArrayElements);
    for (int32_t i = 0; i < numElem; i++) {
        valuesOut[i] = booleanValue(i);
    }
    
    for (int32_t i = numElem; i < arrayNumberOfElements; i++) {
        valuesOut[i] = defaultValue;
    }
}

/**
 * Load the vector with the boolean values from the scene.
 * @param valuesOut
 *     Vector into which values are loaded.  The vector
 *     will contain the number of elements that were saved
 *     to the scene.
 * @param defaultValue
 *    Default value used when output vector contains more elements
 *    than are in this instance's array.
 */
void 
ScenePrimitiveArray::booleanValues(std::vector<bool>& valuesOut,
                                   const bool defaultValue) const
{
    const int32_t valuesOutNumberOfElements = static_cast<int32_t>(valuesOut.size());
    const int32_t numElem = std::min(valuesOutNumberOfElements,
                                     m_numberOfArrayElements);
    
    for (int32_t i = 0; i < numElem; i++) {
        valuesOut[i] = booleanValue(i);
    }
    for (int32_t i = numElem; i < valuesOutNumberOfElements; i++) {
        valuesOut[i] = defaultValue;
    }
}

/**
 * Load the array with float values.
 * @param valuesOut
 *    Array into which boolean values are loaded.
 * @param arrayNumberOfElements
 *    Number of elements in the array.  If this value is greater
 *    than the number of elements in the scene, the remaining
 *    elements will be filled with the default value
 * @param defaultValue
 *    Default value used when output array contains more elements
 *    than are in this instance's array.
 */
void
ScenePrimitiveArray::floatValues(float valuesOut[],
                                   const int32_t arrayNumberOfElements,
                                   const float defaultValue) const
{    
    const int32_t numElem = std::min(arrayNumberOfElements,
                                     m_numberOfArrayElements);
    for (int32_t i = 0; i < numElem; i++) {
        valuesOut[i] = floatValue(i);
    }
    
    for (int32_t i = numElem; i < arrayNumberOfElements; i++) {
        valuesOut[i] = defaultValue;
    }
}

/**
 * Load the vector with the float values from the scene.
 * @param valuesOut
 *     Vector into which values are loaded.  The vector
 *     will contain the number of elements that were saved
 *     to the scene.
 * @param defaultValue
 *    Default value used when output vector contains more elements
 *    than are in this instance's array.
 */
void 
ScenePrimitiveArray::floatValues(std::vector<float>& valuesOut,
                                   const float defaultValue) const
{
    const int32_t valuesOutNumberOfElements = static_cast<int32_t>(valuesOut.size());
    const int32_t numElem = std::min(valuesOutNumberOfElements,
                                     m_numberOfArrayElements);
    
    for (int32_t i = 0; i < numElem; i++) {
        valuesOut[i] = floatValue(i);
    }
    for (int32_t i = numElem; i < valuesOutNumberOfElements; i++) {
        valuesOut[i] = defaultValue;
    }
}

/**
 * Load the array with integer values.
 * @param valuesOut
 *    Array into which boolean values are loaded.
 * @param arrayNumberOfElements
 *    Number of elements in the array.  If this value is greater
 *    than the number of elements in the scene, the remaining
 *    elements will be filled with the default value
 * @param defaultValue
 *    Default value used when output array contains more elements
 *    than are in this instance's array.
 */
void
ScenePrimitiveArray::integerValues(int32_t valuesOut[],
                                 const int32_t arrayNumberOfElements,
                                 const int32_t defaultValue) const
{    
    const int32_t numElem = std::min(arrayNumberOfElements,
                                     m_numberOfArrayElements);
    for (int32_t i = 0; i < numElem; i++) {
        valuesOut[i] = integerValue(i);
    }
    
    for (int32_t i = numElem; i < arrayNumberOfElements; i++) {
        valuesOut[i] = defaultValue;
    }
}

/**
 * Load the vector with the integer values from the scene.
 * @param valuesOut
 *     Vector into which values are loaded.  The vector
 *     will contain the number of elements that were saved
 *     to the scene.
 * @param defaultValue
 *    Default value used when output vector contains more elements
 *    than are in this instance's array.
 */
void 
ScenePrimitiveArray::integerValues(std::vector<int32_t>& valuesOut,
                                 const int32_t defaultValue) const
{
    const int32_t valuesOutNumberOfElements = static_cast<int32_t>(valuesOut.size());
    const int32_t numElem = std::min(valuesOutNumberOfElements,
                                     m_numberOfArrayElements);
    
    for (int32_t i = 0; i < numElem; i++) {
        valuesOut[i] = integerValue(i);
    }
    for (int32_t i = numElem; i < valuesOutNumberOfElements; i++) {
        valuesOut[i] = defaultValue;
    }
}

/**
 * Load the array with string values.
 * @param valuesOut
 *    Array into which boolean values are loaded.
 * @param arrayNumberOfElements
 *    Number of elements in the array.  If this value is greater
 *    than the number of elements in the scene, the remaining
 *    elements will be filled with the default value
 * @param defaultValue
 *    Default value used when output array contains more elements
 *    than are in this instance's array.
 */
void
ScenePrimitiveArray::stringValues(AString valuesOut[],
                                   const int32_t arrayNumberOfElements,
                                   const AString& defaultValue) const
{    
    const int32_t numElem = std::min(arrayNumberOfElements,
                                     m_numberOfArrayElements);
    for (int32_t i = 0; i < numElem; i++) {
        valuesOut[i] = stringValue(i);
    }
    
    for (int32_t i = numElem; i < arrayNumberOfElements; i++) {
        valuesOut[i] = defaultValue;
    }
}

/**
 * Load the vector with the string values from the scene.
 * @param valuesOut
 *     Vector into which values are loaded.  The vector
 *     will contain the number of elements that were saved
 *     to the scene.
 * @param defaultValue
 *    Default value used when output vector contains more elements
 *    than are in this instance's array.
 */
void 
ScenePrimitiveArray::stringValues(std::vector<AString>& valuesOut,
                                   const AString& defaultValue) const
{
    const int32_t valuesOutNumberOfElements = static_cast<int32_t>(valuesOut.size());
    const int32_t numElem = std::min(valuesOutNumberOfElements,
                                     m_numberOfArrayElements);
    
    for (int32_t i = 0; i < numElem; i++) {
        valuesOut[i] = stringValue(i);
    }
    for (int32_t i = numElem; i < valuesOutNumberOfElements; i++) {
        valuesOut[i] = defaultValue;
    }
}




