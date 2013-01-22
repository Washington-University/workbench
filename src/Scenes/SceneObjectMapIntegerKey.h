#ifndef __SCENE_OBJECT_MAP_INTEGER_KEY_H__
#define __SCENE_OBJECT_MAP_INTEGER_KEY_H__

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

#include <map>

#include "CaretAssert.h"
#include "SceneObject.h"

namespace caret {
    
    class SceneClass;
    class SceneObjectMapIntegerKey : public SceneObject {
        
    public:
        virtual ~SceneObjectMapIntegerKey();
        
        SceneObjectMapIntegerKey(const QString& name,
                           const SceneObjectDataTypeEnum::Enum valueDataType);
        
    private:
        SceneObjectMapIntegerKey(const SceneObjectMapIntegerKey&);
        
        SceneObjectMapIntegerKey& operator=(const SceneObjectMapIntegerKey&);
        
    public:
        
        // ADD_NEW_METHODS_HERE
        
        void addBoolean(const int32_t key,
                        const bool value);
        
        void addInteger(const int32_t key,
                        const int32_t value);
        
        void addFloat(const int32_t key,
                      const float value);
        
        void addString(const int32_t key,
                       const AString& value);
        
        void addEnumeratedType(const int32_t key,
                               const AString& value);
        
        void addPathName(const int32_t key,
                         const AString& value);
        
        /**
         * Add the given enumerated type value to the map using the given key.
         * @param key
         *    The key.
         * @param value
         *    The value.
         */
        template<class T, typename ET>
        void addEnumeratedType(const int32_t key,
                               ET enumeratedValue) {
            CaretAssert(getDataType() == SceneObjectDataTypeEnum::SCENE_ENUMERATED_TYPE);
            const AString stringValue = T::toName(enumeratedValue);
            addEnumeratedType(key, stringValue);
        }
        
        void addClass(const int32_t key,
                      SceneClass* value);
        
        std::vector<int32_t> getKeys() const;
        
        const std::map<int32_t, SceneObject*>& getMap() const;
        
        const SceneObject* getObject(const int32_t key) const;
        
        const SceneClass* classValue(const int32_t key) const;
        
        AString enumeratedTypeValue(const int32_t key) const;
        
        /**
         * Get an enumerated type value
         * @param key
         *    Kye of enumerated type value.
         */
        template <class T, typename ET> 
        ET getEnumeratedTypeValue(const int32_t key) const {
            const AString stringValue = enumeratedTypeValue(key);
            bool valid = false;
            ET value = T::fromName(stringValue,
                                   &valid);
            return value;
        }
        
        bool booleanValue(const int32_t key) const;
        
        float floatValue(const int32_t key) const;
        
        int32_t integerValue(const int32_t key) const;
        
        AString stringValue(const int32_t key) const;
        
        AString pathNameValue(const int32_t key) const;
        
    private:
        typedef std::map<int32_t, SceneObject*> DATA_MAP;
        
        typedef DATA_MAP::const_iterator DATA_MAP_CONST_ITERATOR;
        
        // ADD_NEW_MEMBERS_HERE
        
        DATA_MAP m_dataMap;
    };
    
#ifdef __SCENE_OBJECT_MAP_INTEGER_KEY_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __SCENE_OBJECT_MAP_INTEGER_KEY_DECLARE__
    
} // namespace
#endif  //__SCENE_OBJECT_MAP_INTEGER_KEY_H__
