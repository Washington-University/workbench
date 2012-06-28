
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

#define __SCENE_CLASS_ARRAY_DECLARE__
#include "SceneClassArray.h"
#undef __SCENE_CLASS_ARRAY_DECLARE__

#include "CaretAssert.h"

using namespace caret;


    
/**
 * \class caret::SceneClassArray 
 * \brief Stores an array of scene classes.
 */

/**
 * Constructor.
 *
 * @param name
 *    Name of the array.
 * @param values 
 *    Values in the array.
 * @param numberOfArrayElements
 *    Number of elements in the array.
 */
SceneClassArray::SceneClassArray(const AString& name,
                const SceneClass* values[],
                const int32_t numberOfArrayElements)
: SceneObjectArray(name,
                   SceneObjectDataTypeEnum::SCENE_CLASS,
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
 *    Name of the array.
 * @param values 
 *    Vector containing values for the array.
 */
SceneClassArray::SceneClassArray(const AString& name,
                const std::vector<const SceneClass*>& values)
: SceneObjectArray(name,
                   SceneObjectDataTypeEnum::SCENE_CLASS,
                   values.size())
{
    m_values = values;
}

/**
 * Constructor that creates an array of NULL values.
 *
 * @param name
 *    Name of the array.
 * @param numberOfArrayElements
 *    Number of elements in the array.
 */
SceneClassArray::SceneClassArray(const AString& name,
                const int numberOfArrayElements)
: SceneObjectArray(name,
                   SceneObjectDataTypeEnum::SCENE_CLASS,
                   numberOfArrayElements)
{
    m_values.resize(numberOfArrayElements);
    std::fill(m_values.begin(),
              m_values.end(),
              (const SceneClass*)NULL);
}

/**
 * Destructor.
 */
SceneClassArray::~SceneClassArray()
{
    
}

/**
 * Set the class for an array index.
 * @param arrayIndex
 *     Index of element.
 * @param sceneClass
 *     New value for array element.
 */
void 
SceneClassArray::setValue(const int32_t arrayIndex,
              SceneClass* sceneClass)
{
    CaretAssertVectorIndex(m_values, arrayIndex);
    m_values[arrayIndex] = sceneClass;    
}

///**
// * Get the element for a given array index.
// * @param arrayIndex
// *     Index of array element.
// * @return
// *     Class at the given array index.
// */
//SceneClass* 
//SceneClassArray::getValue(const int32_t arrayIndex)
//{
//    CaretAssertVectorIndex(m_values, arrayIndex);
//    return m_values[arrayIndex];
//}

/**
 * Get the element for a given array index.
 * @param arrayIndex
 *     Index of array element.
 * @return
 *     Class at the given array index.
 */
const SceneClass* 
SceneClassArray::getValue(const int32_t arrayIndex) const
{
    CaretAssertVectorIndex(m_values, arrayIndex);
    return m_values[arrayIndex];
}

