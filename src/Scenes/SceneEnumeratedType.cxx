
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

#define __SCENE_ENUMERATED_TYPE_DECLARE__
#include "SceneEnumeratedType.h"
#undef __SCENE_ENUMERATED_TYPE_DECLARE__

using namespace caret;


    
/**
 * \class caret::SceneEnumeratedType 
 * \brief For storage of an emumerated type value in a scene.
 * \ingroup Scene
 *
 * See the documentation in the class Scene for how to use the Scene system.
 */

/**
 * Constructor.
 */
SceneEnumeratedType::SceneEnumeratedType(const AString& name,
                                         const AString& enumeratedValueAsString)
: SceneObject(name,
              SceneObjectContainerTypeEnum::SINGLE,
              SceneObjectDataTypeEnum::SCENE_ENUMERATED_TYPE)
{
    m_enumeratedValueAsString = enumeratedValueAsString;
}

/**
 * Destructor.
 */
SceneEnumeratedType::~SceneEnumeratedType()
{
    
}

/**
 * Cast an instance of SceneObject to a SceneEnumeratedType.
 * Is used to avoid dynamic casting and overridden by the class.
 *
 * @return Valid pointer (non-NULL) this is SceneEnumeratedType
 */
SceneEnumeratedType*
SceneEnumeratedType::castToSceneEnumeratedType()
{
    return this;
}

/**
 * Cast an instance of SceneObject to a SceneEnumeratedType.
 * Is used to avoid dynamic casting and overridden by the class.
 *
 * @return Valid pointer (non-NULL) this is SceneEnumeratedType
 */
const SceneEnumeratedType*
SceneEnumeratedType::castToSceneEnumeratedType() const
{
    return this;
}


/**
 * @param enumeratedValueAsString
 *   New value.
 */
void 
SceneEnumeratedType::setValue(const AString& enumeratedValueAsString)
{
    m_enumeratedValueAsString = enumeratedValueAsString;
}

/**
 * @return The string representing the value of the
 * enumerated type.
 */
AString 
SceneEnumeratedType::stringValue() const
{
    return m_enumeratedValueAsString;
}

SceneObject* SceneEnumeratedType::clone() const
{
    return new SceneEnumeratedType(getName(), stringValue());
}

