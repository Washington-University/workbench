
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

#define __SCENE_STRING_DECLARE__
#include "SceneString.h"
#undef __SCENE_STRING_DECLARE__

using namespace caret;


    
/**
 * \class caret::SceneString 
 * \brief For storage of a string value in a scene.
 */

/**
 * Constructor.
 *
 * @param name
 *   Name of object.
 * @param value
 *   Value of object.
 */
SceneString::SceneString(const AString& name,
                           const AString& value)
: ScenePrimitive(name,
                 SceneObjectDataTypeEnum::SCENE_STRING)
{
    m_value = value;
}

/**
 * Destructor.
 */
SceneString::~SceneString()
{
    
}

/**
 * Set the value.
 * @param value
 *   New value.
 */
void 
SceneString::setValue(const AString& value)
{
    m_value = value;
}

/**
 * @return The value as a boolean data type.
 */
bool 
SceneString::booleanValue() const
{
    const bool b = m_value.toBool();
    return b;
}

/**
 * @return The value as a float data type.
 * If the string does not convert to a float number,
 * 0.0 is returned.
 */
float
SceneString::floatValue() const
{
    bool isValid = false;
    float f = m_value.toFloat(&isValid);
    if (isValid == false) {
        f = 0.0;
    }
    return f;
}

/**
 * @return The value as a integer data type.
 * If the string does not convert to an integer number,
 * 0 is returned.
 */
int32_t 
SceneString::integerValue() const
{
    bool isValid = false;
    int32_t i = m_value.toInt(&isValid);
    if (isValid == false) {
        i = 0;
    }
    return i;
}

/**
 * @return The value as a string data type.
 */
AString 
SceneString::stringValue() const
{
    return m_value;
}

