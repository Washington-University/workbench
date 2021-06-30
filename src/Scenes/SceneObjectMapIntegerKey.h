#ifndef __SCENE_OBJECT_MAP_INTEGER_KEY_H__
#define __SCENE_OBJECT_MAP_INTEGER_KEY_H__

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
        
        virtual SceneObjectMapIntegerKey* castToSceneObjectMapIntegerKey();
        
        virtual const SceneObjectMapIntegerKey* castToSceneObjectMapIntegerKey() const;
        
        virtual std::vector<SceneObject*> getDescendants() const;
        
    private:
        SceneObjectMapIntegerKey(const SceneObjectMapIntegerKey&);
        
        SceneObjectMapIntegerKey& operator=(const SceneObjectMapIntegerKey&);
        
    public:
        
        // ADD_NEW_METHODS_HERE
        
        void addBoolean(const int32_t key,
                        const bool value);
        
        void addInteger(const int32_t key,
                        const int32_t value);
        
        void addLongInteger(const int32_t key,
                            const int64_t value);
        
        void addFloat(const int32_t key,
                      const float value);
        
        void addString(const int32_t key,
                       const AString& value);
        
        void addUnsignedByte(const int32_t key,
                             const uint8_t value);
        
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
        
        int64_t longIntegerValue(const int32_t key) const;
        
        AString stringValue(const int32_t key) const;
        
        AString pathNameValue(const int32_t key) const;
        
        virtual SceneObject* clone() const;
        
        bool isEmpty() const;
        
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
