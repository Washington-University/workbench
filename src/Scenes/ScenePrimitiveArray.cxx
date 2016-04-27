
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

/**
 * Load the array with unsigned byte values.
 * @param valuesOut
 *    Array into which unsigned byte values are loaded.
 * @param arrayNumberOfElements
 *    Number of elements in the array.  If this value is greater
 *    than the number of elements in the scene, the remaining
 *    elements will be filled with the default value
 * @param defaultValue
 *    Default value used when output array contains more elements
 *    than are in this instance's array.
 */
void
ScenePrimitiveArray::unsignedByteValues(uint8_t valuesOut[],
                                   const int32_t arrayNumberOfElements,
                                   const uint8_t defaultValue) const
{
    const int32_t numElem = std::min(arrayNumberOfElements,
                                     m_numberOfArrayElements);
    for (int32_t i = 0; i < numElem; i++) {
        valuesOut[i] = unsignedByteValue(i);
    }
    
    for (int32_t i = numElem; i < arrayNumberOfElements; i++) {
        valuesOut[i] = defaultValue;
    }
}

/**
 * Load the vector with the unsigned byte values from the scene.
 * @param valuesOut
 *     Vector into which values are loaded.  The vector
 *     will contain the number of elements that were saved
 *     to the scene.
 * @param defaultValue
 *    Default value used when output vector contains more elements
 *    than are in this instance's array.
 */
void
ScenePrimitiveArray::unsignedByteValues(std::vector<uint8_t>& valuesOut,
                                   const uint8_t defaultValue) const
{
    const int32_t valuesOutNumberOfElements = static_cast<int32_t>(valuesOut.size());
    const int32_t numElem = std::min(valuesOutNumberOfElements,
                                     m_numberOfArrayElements);
    
    for (int32_t i = 0; i < numElem; i++) {
        valuesOut[i] = unsignedByteValue(i);
    }
    for (int32_t i = numElem; i < valuesOutNumberOfElements; i++) {
        valuesOut[i] = defaultValue;
    }
}




