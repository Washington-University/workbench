
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

#define __SCENE_OBJECT_DECLARE__
#include "SceneObject.h"
#undef __SCENE_OBJECT_DECLARE__

#include "CaretAssert.h"

using namespace caret;


    
/**
 * \class caret::SceneObject 
 * \brief Abstract class for any item saved to a scene.
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
: CaretObject(), m_name(name), m_dataType(dataType)
{
    CaretAssert(name.isEmpty());
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
