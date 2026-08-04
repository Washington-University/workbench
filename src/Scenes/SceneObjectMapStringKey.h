#ifndef __SCENE_OBJECT_MAP_STRING_KEY_H__
#define __SCENE_OBJECT_MAP_STRING_KEY_H__

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
    class SceneObjectMapStringKey : public SceneObject {
        
    public:
        virtual ~SceneObjectMapStringKey();
        
        SceneObjectMapStringKey(const QString& name,
                           const SceneObjectDataTypeEnum::Enum valueDataType);
        
        virtual SceneObjectMapStringKey* castToSceneObjectMapStringKey() override;
        
        virtual const SceneObjectMapStringKey* castToSceneObjectMapStringKey() const override;
        
        virtual std::vector<SceneObject*> getDescendants() const;
        
    private:
        SceneObjectMapStringKey(const SceneObjectMapStringKey&);
        
        SceneObjectMapStringKey& operator=(const SceneObjectMapStringKey&);
        
    public:
        
        // ADD_NEW_METHODS_HERE
        
        void addBoolean(const AString& key,
                        const bool value);
        
        void addInteger(const AString& key,
                        const int32_t value);
        
        void addLongInteger(const AString& key,
                            const int64_t value);
        
        void addFloat(const AString& key,
                      const float value);
        
        void addString(const AString& key,
                       const AString& value);
        
        void addUnsignedByte(const AString& key,
                             const uint8_t value);
        
        void addEnumeratedType(const AString& key,
                               const AString& value);
        
        void addPathName(const AString& key,
                         const AString& value);
        
        /**
         * Add the given enumerated type value to the map using the given key.
         * @param key
         *    The key.
         * @param value
         *    The value.
         */
        template<class T, typename ET>
        void addEnumeratedType(const AString& key,
                               ET enumeratedValue) {
            CaretAssert(getDataType() == SceneObjectDataTypeEnum::SCENE_ENUMERATED_TYPE);
            const AString stringValue = T::toName(enumeratedValue);
            addEnumeratedType(key, stringValue);
        }
        
        void addClass(const AString& key,
                      SceneClass* value);
        
        std::vector<AString> getKeys() const;

        const std::map<AString, SceneObject*>& getMap() const;

        const SceneObject* getObject(const AString& key) const;
        
        const SceneClass* classValue(const AString& key) const;
        
        AString enumeratedTypeValue(const AString& key) const;
        
        /**
         * Get an enumerated type value
         * @param key
         *    Kye of enumerated type value.
         */
        template <class T, typename ET> 
        ET getEnumeratedTypeValue(const AString& key) const {
            const AString stringValue = enumeratedTypeValue(key);
            bool valid = false;
            ET value = T::fromName(stringValue,
                                   &valid);
            return value;
        }
        
        bool booleanValue(const AString& key) const;
        
        float floatValue(const AString& key) const;
        
        int32_t integerValue(const AString& key) const;
        
        int64_t longIntegerValue(const AString& key) const;
        
        AString stringValue(const AString& key) const;
        
        AString pathNameValue(const AString& key) const;
        
        virtual SceneObject* clone() const;
        
        bool isEmpty() const;
        
    private:
        typedef std::map<AString, SceneObject*> DATA_MAP;
        
        typedef DATA_MAP::const_iterator DATA_MAP_CONST_ITERATOR;
        
        // ADD_NEW_MEMBERS_HERE
        
        DATA_MAP m_dataMap;
    };
    
#ifdef __SCENE_OBJECT_MAP_STRING_KEY_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __SCENE_OBJECT_MAP_STRING_KEY_DECLARE__
    
} // namespace
#endif  //__SCENE_OBJECT_MAP_STRING_KEY_H__
