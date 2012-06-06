
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

#include <limits>

#define __SCENE_FLOAT_DECLARE__
#include "SceneFloat.h"
#undef __SCENE_FLOAT_DECLARE__

using namespace caret;


    
/**
 * \class caret::SceneFloat 
 * \brief For storage of a float value in a scene.
 */

/**
 * Constructor.
 *
 * @param name
 *   Name of object.
 * @param value
 *   Value of object.
 */
SceneFloat::SceneFloat(const AString& name,
                       const float value)
: ScenePrimitive(name,
                 SceneDataTypeEnum::SCENE_FLOAT)
{
    m_value = value;
}

/**
 * Destructor.
 */
SceneFloat::~SceneFloat()
{
    
}

/**
 * @return The value as a boolean data type.
 */
bool 
SceneFloat::booleanValue() const
{
    const bool b = ((m_value != 0.0) ? true : false);
    return b;
}

/**
 * @return The value as a float data type.
 */
float
SceneFloat::floatValue() const
{
    return m_value;
}

/**
 * @return The value as a integer data type.
 */
int32_t 
SceneFloat::integerValue() const
{
    if (m_value > std::numeric_limits<int32_t>::max()) {
        return std::numeric_limits<int32_t>::max();
    }
    else if (m_value < std::numeric_limits<int32_t>::min()) {
        return std::numeric_limits<int32_t>::min();
    }
    
    const float i = static_cast<int32_t>(m_value);
    return i;
}

/**
 * @return The value as a string data type.
 */
AString 
SceneFloat::stringValue() const
{
    const AString s = AString::number(m_value);
    return s;
}

