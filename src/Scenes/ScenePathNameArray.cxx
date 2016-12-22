
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

#define __SCENE_PATH_NAME_ARRAY_DECLARE__
#include "ScenePathNameArray.h"
#undef __SCENE_PATH_NAME_ARRAY_DECLARE__

#include "CaretAssert.h"
#include "SceneClass.h"
#include "ScenePathName.h"

using namespace caret;


    
/**
 * \class caret::ScenePathNameArray 
 * \brief Stores an array of scene path names.
 * \ingroup Scene
 *
 * See the documentation in the class Scene for how to use the Scene system.
 */

/**
 * Constructor.
 *
 * @param name
 *    Name of the array.
 * @param pathNameValues 
 *    Values in the array.
 * @param numberOfArrayElements
 *    Number of elements in the array.
 */
ScenePathNameArray::ScenePathNameArray(const AString& name,
                const AString pathNameValues[],
                const int32_t numberOfArrayElements)
: SceneObjectArray(name,
                   SceneObjectDataTypeEnum::SCENE_PATH_NAME)
{
    m_values.resize(numberOfArrayElements);
    for (int32_t i = 0; i < numberOfArrayElements; i++) {
        m_values[i] = new ScenePathName(name,
                                        pathNameValues[i]);
    }
}

/**
 * Constructor.
 *
 * @param name
 *    Name of the array.
 * @param pathNameValues 
 *    Vector containing values for the array.
 */
ScenePathNameArray::ScenePathNameArray(const AString& name,
                const std::vector<AString>& pathNameValues)
: SceneObjectArray(name,
                   SceneObjectDataTypeEnum::SCENE_PATH_NAME)
{
    const int32_t numberOfArrayElements = static_cast<int32_t>(pathNameValues.size());
    for (int32_t i = 0; i < numberOfArrayElements; i++) {
        m_values.push_back(new ScenePathName(name,
                                        pathNameValues[i]));
    }
}

/**
 * Constructor that creates an array of NULL values.
 *
 * @param name
 *    Name of the array.
 * @param numberOfArrayElements
 *    Number of elements in the array.
 */
ScenePathNameArray::ScenePathNameArray(const AString& name,
                const int numberOfArrayElements)
: SceneObjectArray(name,
                   SceneObjectDataTypeEnum::SCENE_PATH_NAME)
{
    m_values.resize(numberOfArrayElements);
    std::fill(m_values.begin(),
              m_values.end(),
              (ScenePathName*)NULL);
}

ScenePathNameArray::ScenePathNameArray(const ScenePathNameArray& rhs): SceneObjectArray(rhs.getName(), SceneObjectDataTypeEnum::SCENE_PATH_NAME)
{
    for (std::vector<ScenePathName*>::const_iterator iter = rhs.m_values.begin(); iter != rhs.m_values.end(); ++iter)
    {
        m_values.push_back(new ScenePathName(**iter));
    }
}

/**
 * Destructor.
 */
ScenePathNameArray::~ScenePathNameArray()
{
    for (std::vector<ScenePathName*>::iterator iter = m_values.begin();
         iter != m_values.end();
         iter++) {
        delete *iter;
    }
    m_values.clear();
}

/**
 * @return All descendant SceneClasses (children, grandchildren, etc.) of this instance.
 */
std::vector<SceneObject*>
ScenePathNameArray::getDescendants() const
{
    std::vector<SceneObject*> descendants;
    
    for (std::vector<ScenePathName*>::const_iterator iter = m_values.begin();
         iter != m_values.end();
         iter++) {
        descendants.push_back(*iter);
    }
    
    return descendants;
}
/**
 * Set the class for an array index.
 * @param arrayIndex
 *     Index of element.
 * @param sceneFileName
 *     Full path of scene file.
 * @param pathNameValue
 *     New value for array element.
 */
void 
ScenePathNameArray::setScenePathNameAtIndex(const int32_t arrayIndex,
                                            const AString& sceneFileName,
                                            const AString& pathNameValue)
{
    CaretAssertVectorIndex(m_values, arrayIndex);
    if (m_values[arrayIndex] != NULL) {
        delete m_values[arrayIndex];
    }
    ScenePathName* spn = new ScenePathName(getName(),
                                           pathNameValue);
    spn->setValueToAbsolutePath(sceneFileName,
                                pathNameValue);
    m_values[arrayIndex] = spn;
}

///**
// * Get the element for a given array index.
// * @param arrayIndex
// *     Index of array element.
// * @return
// *     Class at the given array index.
// */
//SceneClass* 
//ScenePathNameArray::getValue(const int32_t arrayIndex)
//{
//    CaretAssertVectorIndex(m_values, arrayIndex);
//    return m_values[arrayIndex];
//}

/**
 * Get the element for a given array index.
 * @param arrayIndex
 *     Index of array element.
 * @return
 *     Pathname at the given array index.
 */
ScenePathName*
ScenePathNameArray::getScenePathNameAtIndex(const int32_t arrayIndex) const
{
    CaretAssertVectorIndex(m_values, arrayIndex);
    ScenePathName* spm = m_values[arrayIndex];
    if (spm != NULL) {
        spm->m_restoredFlag = true;
    }
    return spm;
}

