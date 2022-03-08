
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

#define __SCENE_CLASS_ARRAY_DECLARE__
#include "SceneClassArray.h"
#undef __SCENE_CLASS_ARRAY_DECLARE__

#include "CaretAssert.h"
#include "SceneClass.h"

using namespace caret;


    
/**
 * \class caret::SceneClassArray 
 * \brief Stores an array of scene classes.
 * \ingroup Scene
 *
 * See the documentation in the class Scene for how to use the Scene system.
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
                SceneClass* values[],
                const int32_t numberOfArrayElements)
: SceneObjectArray(name,
                   SceneObjectDataTypeEnum::SCENE_CLASS)
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
                const std::vector<SceneClass*>& values)
: SceneObjectArray(name,
                   SceneObjectDataTypeEnum::SCENE_CLASS)
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
                   SceneObjectDataTypeEnum::SCENE_CLASS)
{
    m_values.resize(numberOfArrayElements);
    std::fill(m_values.begin(),
              m_values.end(),
              (SceneClass*)NULL);
}

SceneClassArray::SceneClassArray(const caret::SceneClassArray& rhs) : SceneObjectArray(rhs.getName(), SceneObjectDataTypeEnum::SCENE_CLASS)
{
    for (std::vector<SceneClass*>::const_iterator iter = rhs.m_values.begin(); iter != rhs.m_values.end(); ++iter)
    {
        m_values.push_back(new SceneClass(**iter));
    }
}

/**
 * Destructor.
 */
SceneClassArray::~SceneClassArray()
{
    for (std::vector<SceneClass*>::iterator iter = m_values.begin();
         iter != m_values.end();
         iter++) {
        delete *iter;
    }
    m_values.clear();
}

/**
 * Cast an instance of SceneObjectArray to a SceneClassArray.
 * Is used to avoid dynamic casting and overridden by the class.
 *
 * @return Valid pointer (non-NULL) this is SceneClass
 */
SceneClassArray*
SceneClassArray::castToSceneClassArray()
{
    return this;
}

/**
 * Cast an instance of SceneObjectArray to a SceneClassArray.
 * Is used to avoid dynamic casting and overridden by the class.
 *
 * @return Valid pointer (non-NULL) this is SceneClass
 */
const SceneClassArray*
SceneClassArray::castToSceneClassArray() const
{
    return this;
}

/**
 * @return All descendant SceneClasses (children, grandchildren, etc.) of this instance.
 */
std::vector<SceneObject*>
SceneClassArray::getDescendants() const
{
    std::vector<SceneObject*> descendants;
    
    for (std::vector<SceneClass*>::const_iterator iter = m_values.begin();
         iter != m_values.end();
         iter++) {
        SceneClass* sceneClass = *iter;
        descendants.push_back(sceneClass);
        
        std::vector<SceneObject*> classDescendants = sceneClass->getDescendants();
        descendants.insert(descendants.end(),
                           classDescendants.begin(),
                           classDescendants.end());
    }
    
    return descendants;
}

/**
 * Set the class for an array index.
 * @param arrayIndex
 *     Index of element.
 * @param sceneClass
 *     New value for array element.
 */
void 
SceneClassArray::setClassAtIndex(const int32_t arrayIndex,
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
SceneClass*
SceneClassArray::getClassAtIndex(const int32_t arrayIndex)
{
    CaretAssertVectorIndex(m_values, arrayIndex);
    SceneClass* sc = m_values[arrayIndex];
    if (sc != NULL) {
        sc->m_restoredFlag = true;
    }
    return sc;
}

/**
 * Get the element for a given array index.
 * @param arrayIndex
 *     Index of array element.
 * @return
 *     Class at the given array index.
 */
const SceneClass* 
SceneClassArray::getClassAtIndex(const int32_t arrayIndex) const
{
    CaretAssertVectorIndex(m_values, arrayIndex);
    const SceneClass* sc = m_values[arrayIndex];
    if (sc != NULL) {
        sc->m_restoredFlag = true;
    }
    return sc;
}
