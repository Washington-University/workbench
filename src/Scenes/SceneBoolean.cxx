
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

#define __SCENE_BOOLEAN_DECLARE__
#include "SceneBoolean.h"
#undef __SCENE_BOOLEAN_DECLARE__

using namespace caret;


    
/**
 * \class caret::SceneBoolean 
 * \brief For storage of a boolean value in a scene.
 */

/**
 * Constructor.
 *
 * @param name
 *   Name of object.
 * @param value
 *   Value of object.
 */
SceneBoolean::SceneBoolean(const AString& name,
                           const bool value)
: ScenePrimitive(name,
                 SceneDataTypeEnum::SCENE_BOOLEAN)
{
    m_value = value;
}

/**
 * Destructor.
 */
SceneBoolean::~SceneBoolean()
{
    
}

/**
 * @return The value as a boolean data type.
 */
bool 
SceneBoolean::booleanValue() const
{
    return m_value;
}

/**
 * @return The value as a float data type.
 */
float
SceneBoolean::floatValue() const
{
    const float f = (m_value ? 1.0 : 0.0);
    return f;
}

/**
 * @return The value as a integer data type.
 */
int32_t 
SceneBoolean::integerValue() const
{
    const float i = (m_value ? 1 : 0);
    return i;
}

/**
 * @return The value as a string data type.
 */
AString 
SceneBoolean::stringValue() const
{
    const AString s = (m_value ? "true" : "false");
    return s;
}

