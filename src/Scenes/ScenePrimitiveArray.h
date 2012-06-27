#ifndef __SCENE_PRIMITIVE_ARRAY__H_
#define __SCENE_PRIMITIVE_ARRAY__H_

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


#include "SceneObjectArray.h"

namespace caret {

    class ScenePrimitiveArray : public SceneObjectArray {
        
    public:
        virtual ~ScenePrimitiveArray();

    protected:
        ScenePrimitiveArray(const QString& name,
                            const SceneObjectDataTypeEnum::Enum dataType,
                            const int32_t numberOfArrayElements);
        
    private:
        ScenePrimitiveArray(const ScenePrimitiveArray&);

        ScenePrimitiveArray& operator=(const ScenePrimitiveArray&);
        
    public:

        // ADD_NEW_METHODS_HERE
        
        /** 
         * Get the values as a boolean. 
         * @param arrayIndex
         *    Index of element.
         * @return The value.
         */
        virtual bool booleanValue(const int32_t arrayIndex) const = 0;

        /** 
         * Get the values as a float. 
         * @param arrayIndex
         *    Index of element.
         * @return The value.
         */
        virtual float floatValue(const int32_t arrayIndex) const = 0;
        
        /** 
         * Get the values as a integer. 
         * @param arrayIndex
         *    Index of element.
         * @return The value.
         */
        virtual int32_t integerValue(const int32_t arrayIndex) const = 0;
        
        /** 
         * Get the values as a string. 
         * @param arrayIndex
         *    Index of element.
         * @return The value.
         */
        virtual AString stringValue(const int32_t arrayIndex) const = 0;
        
        virtual void booleanValues(bool valuesOut[],
                                   const int32_t arrayNumberOfElements,
                                   const bool defaultValue) const;
        
        virtual void booleanValues(std::vector<bool>& valuesOut,
                                   const bool defaultValue) const;
        
        virtual void floatValues(float valuesOut[],
                           const int32_t arrayNumberOfElements,
                           const float defaultValue) const;
        
        virtual void floatValues(std::vector<float>& valuesOut,
                                   const float defaultValue) const;
        
        virtual void integerValues(int32_t valuesOut[],
                                 const int32_t arrayNumberOfElements,
                                 const int32_t defaultValue) const;
        
        virtual void integerValues(std::vector<int32_t>& valuesOut,
                                 const int32_t defaultValue) const;
        
        virtual void stringValues(AString valuesOut[],
                                   const int32_t arrayNumberOfElements,
                                   const AString& defaultValue) const;
        
        virtual void stringValues(std::vector<AString>& valuesOut,
                                   const AString& defaultValue) const;
        
        
    private:

        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __SCENE_PRIMITIVE_ARRAY_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __SCENE_PRIMITIVE_ARRAY_DECLARE__

} // namespace
#endif  //__SCENE_PRIMITIVE_ARRAY__H_
