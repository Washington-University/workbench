
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

#define __SCENE_PRIMITIVE_DECLARE__
#include "ScenePrimitive.h"
#undef __SCENE_PRIMITIVE_DECLARE__

using namespace caret;


    
/**
 * \class caret::ScenePrimitive 
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
ScenePrimitive::ScenePrimitive(const QString& name,
                               const SceneObjectDataTypeEnum::Enum dataType)
: SceneObject(name,
              SceneObjectContainerTypeEnum::SINGLE,
              dataType)
{
    
}

/**
 * Destructor.
 */
ScenePrimitive::~ScenePrimitive()
{
    
}

/**
 * Cast an instance of SceneObject to a ScenePrimitive.
 * Is used to avoid dynamic casting and overridden by the class.
 *
 * @return Valid pointer (non-NULL) this is ScenePrimitive
 */
const ScenePrimitive*
ScenePrimitive::castToScenePrimitive() const
{
    return this;
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString
ScenePrimitive::toString() const
{
    QString objectInfo(SceneObject::toString()
                       + " value="
                       + stringValue());
    return objectInfo;
}


