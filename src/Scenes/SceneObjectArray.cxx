
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

#define __SCENE_OBJECT_ARRAY_DECLARE__
#include "SceneObjectArray.h"
#undef __SCENE_OBJECT_ARRAY_DECLARE__

using namespace caret;


    
/**
 * \class caret::SceneObjectArray 
 * \brief Abstract class for arrays (and other containers).
 * \ingroup Scene
 *
 * See the documentation in the class Scene for how to use the Scene system.
 */

/**
 * Constructor.
 * @param name
 *    Name of the item.
 * @param dataType
 *    Data type of the object.
 * @param numberOfArrayElements
 *    Number of elements in the array.
 */
SceneObjectArray::SceneObjectArray(const QString& name,
                                   const SceneObjectDataTypeEnum::Enum dataType)
: SceneObject(name,
              SceneObjectContainerTypeEnum::ARRAY,
              dataType)
{
    
}

/**
 * Destructor.
 */
SceneObjectArray::~SceneObjectArray()
{
    
}

/**
 * Cast an instance of SceneObject to a SceneObjectArray.
 * Is used to avoid dynamic casting and overridden by the class.
 *
 * @return Valid pointer (non-NULL) this is SceneObjectArray
 */
const SceneObjectArray*
SceneObjectArray::castToSceneObjectArray() const
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
SceneObjectArray::castToSceneClassArray() const
{
    return NULL;
}

/**
 * Cast an instance of SceneObjectArray to a SceneEnumeratedTypeArray.
 * Is used to avoid dynamic casting and overridden by the class.
 *
 * @return Valid pointer (non-NULL) this is SceneClass
 */
const SceneEnumeratedTypeArray*
SceneObjectArray::castToSceneEnumeratedTypeArray() const
{
    return NULL;
}

/**
 * Cast an instance of SceneObjectArray to a ScenePathNameArray.
 * Is used to avoid dynamic casting and overridden by the class.
 *
 * @return Valid pointer (non-NULL) this is SceneClass
 */
const ScenePathNameArray*
SceneObjectArray::castToScenePathNameArray() const
{
    return NULL;
}

/**
 * Cast an instance of SceneObjectArray to a ScenePrimitiveArray.
 * Is used to avoid dynamic casting and overridden by the class.
 *
 * @return Valid pointer (non-NULL) this is SceneClass
 */
const ScenePrimitiveArray*
SceneObjectArray::castToScenePrimitiveArray() const
{
    return NULL;
}
