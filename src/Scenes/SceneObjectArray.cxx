
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
              dataType)
{
    
}

/**
 * Destructor.
 */
SceneObjectArray::~SceneObjectArray()
{
    
}

