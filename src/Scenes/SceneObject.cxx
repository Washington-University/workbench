
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

#define __SCENE_OBJECT_DECLARE__
#include "SceneObject.h"
#undef __SCENE_OBJECT_DECLARE__

#include "CaretAssert.h"

using namespace caret;


    
/**
 * \class caret::SceneObject 
 * \brief Abstract class for any item saved to a scene.
 * \ingroup Scene
 *
 * See the documentation in the class Scene for how to use the Scene system.
 */

/**
 * Constructor.
 * @param name
 *    Name of the item.
 * @param dataType
 *    Data type of the primitive.
 */
SceneObject::SceneObject(const QString& name,
                         const SceneObjectDataTypeEnum::Enum dataType)
#ifdef CARET_SCENE_DEBUG
: CaretObject(), m_name(name), m_dataType(dataType)
#else   // CARET_SCENE_DEBUG
: m_name(name), m_dataType(dataType)
#endif  // CARET_SCENE_DEBUG
{
    CaretAssert(name.isEmpty() == false);
}

/**
 * Destructor.
 */
SceneObject::~SceneObject()
{
    
}

/**
 * @return Name of the item
 */
QString
SceneObject::getName() const
{
    return m_name;
}

/**
 * @return Data type of the object.
 */
SceneObjectDataTypeEnum::Enum 
SceneObject::getDataType() const
{
    return m_dataType;
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
SceneObject::toString() const
{
    return "SceneObject";
}
