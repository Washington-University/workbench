#ifndef __SCENE_CLASS__H_
#define __SCENE_CLASS__H_

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


#include "SceneObjectMapIntegerKey.h"
#include "SceneObject.h"

namespace caret {

    class SceneClassArray;
    class SceneEnumeratedType;
    class SceneObjectMapIntegerKey;
    class ScenePathName;
    class ScenePrimitive;
    class ScenePrimitiveArray;
    
    class SceneClass : public SceneObject {
        
    public:
        SceneClass(const AString& name,
                   const AString& className,
                   const int32_t versionNumber);
        
        virtual ~SceneClass();
        
        AString getClassName() const;
        
        int32_t getVersionNumber() const;
        
        void addBoolean(const AString& name,
                        const bool value);
        
        void addBooleanArray(const AString& name,
                             const bool values[],
                             const int32_t arrayNumberOfElements);
        
        void addClass(SceneClass* sceneClass);
        
        /**
         * Add an enumerated type.
         * @param name
         *    Name of enumerated type.
         * @param value
         *    Enumerated value.
         */
        template <class T, typename ET> 
        void addEnumeratedType(const AString& name,
                              const ET value) {
            addEnumeratedType(name,
                              T::toName(value));
        }
        
        /**
         * Get an enumerated type value
         * @param name
         *    Name of enumerated type.
         * @param defaultValue
         *    Enumerated value returned if named value is not found.
         */
        template <class T, typename ET> 
        ET getEnumeratedTypeValue(const AString& name,
                             const ET defaultValue) const {
            const AString stringValue = getEnumeratedTypeValue(name,
                                                               T::toName(defaultValue));
            bool valid = false;
            ET value = T::fromName(stringValue,
                                   &valid);
            return value;
        }
        
        /**
         * Add all elements in an enumerated type array.
         * @param name
         *    Name of enumerated type array.
         * @param value
         *    Array values.
         * @param arrayNumberOfElements
         *    Number of values in the array.
         */
        template <class T, typename ET> 
        void addEnumeratedTypeArray(const AString& name,
                                   const ET value[],
                                   const int32_t arrayNumberOfElements) {
            std::vector<AString> stringVector;
            
            for (int32_t i = 0; i < arrayNumberOfElements; i++) {
                const AString stringValue = T::toName(value[i]);
                stringVector.push_back(stringValue);
            }
            addEnumeratedTypeVector(name, stringVector);
        }
        
        /**
         * Restore all elements in an enumerated type array.
         * @param name
         *    Name of enumerated type array.
         * @param value
         *    Output array values.
         * @param arrayNumberOfElements
         *    Number of values in the array.
         * @param defaultValue
         *    Value used for missing array elements.
         */
        template <class T, typename ET> 
        int32_t getEnumerateTypeArray(const AString& name,
                                   ET value[],
                                   const int32_t arrayNumberOfElements,
                                      const ET defaultValue) const {
            std::vector<AString> stringValues;
            stringValues.resize(arrayNumberOfElements);
            const int32_t numRestored = getEnumeratedTypeArrayValue(name,
                                                                    &stringValues[0],
                                                                    arrayNumberOfElements,
                                                                    T::toName(defaultValue));
            for (int32_t i = 0; i < numRestored; i++) {
                bool valid = false;
                value[i] = T::fromName(stringValues[i], &valid);
            }
            return numRestored;
        }
        
        /**
         * Add elements from an enumerated type array but
         * only for the given tab indices using an
         * integer keyed map.
         *
         * @param name
         *    Name of enumerated type array.
         * @param value
         *    Array values.
         * @param tabIndices
         *    Indices of the tabs for which the values are saved.
         */
        template <class T, typename ET> 
        void addEnumeratedTypeArrayForTabIndices(const AString& name,
                                                const ET value[],
                                                const std::vector<int32_t>& tabIndices) {
            SceneObjectMapIntegerKey* sceneMap = new SceneObjectMapIntegerKey(name,
                                                                  SceneObjectDataTypeEnum::SCENE_ENUMERATED_TYPE);
            const int32_t numTabs = static_cast<int32_t>(tabIndices.size());
            for (int32_t i = 0; i < numTabs; i++) {
                const int32_t tabIndex = tabIndices[i];
                const AString stringValue = T::toName(value[tabIndex]);
                sceneMap->addEnumeratedType(tabIndex, stringValue);
            }
            
            addChild(sceneMap);
        }
        
        /**
         * Add elements from an enumerated type array but
         * only for the given tab indices using an
         * integer keyed map.
         *
         * @param name
         *    Name of enumerated type array.
         * @param value
         *    Array values.
         * @param tabIndices
         *    Indices of the tabs for which the values are saved.
         */
        template <class T, typename ET> 
        void getEnumerateTypeArrayForTabIndices(const AString& name,
                                                ET value[]) const {
            const SceneObjectMapIntegerKey* sceneMap = getMapIntegerKey(name);
            if (sceneMap == NULL) {
                return;
            }
            const std::vector<int32_t> mapKeys = sceneMap->getKeys();
            const int numKeys = static_cast<int32_t>(mapKeys.size());
            for (int32_t i = 0; i < numKeys; i++) {
                const int32_t key = mapKeys[i];
                const AString valueString = sceneMap->enumeratedTypeValue(key);
                bool valid = false;
                value[key] = T::fromName(valueString,
                                         &valid);
            }
        }
        
        void addFloat(const AString& name,
                      const float value);
        
        void addFloatArray(const AString& name,
                             const float values[],
                             const int32_t arrayNumberOfElements);
        
        void addInteger(const AString& name,
                        const int32_t value);
        
        void addIntegerArray(const AString& name,
                             const int32_t values[],
                             const int32_t arrayNumberOfElements);
        
        void addString(const AString& name,
                       const AString& value);
        
        void addPathName(const AString& name,
                         const AString& value);
        
        void addStringArray(const AString& name,
                             const AString values[],
                             const int32_t arrayNumberOfElements);
        
        void addChild(SceneObject* sceneObject);
        
        bool getBooleanValue(const AString& name,
                             const bool defaultValue = false) const;
        
        int32_t getBooleanArrayValue(const AString& name,
                                  bool values[],
                                  const int32_t arrayNumberOfElements,
                                  const bool defaultValue = false) const;
        
        const SceneClass* getClass(const AString& name) const;
        
        SceneClass* getClass(const AString& name);
        
        const SceneClassArray* getClassArray(const AString& name) const;
        
        SceneClassArray* getClassArray(const AString& name);
        
        float getFloatValue(const AString& name,
                             const float defaultValue = 0.0) const;
        
        int32_t getFloatArrayValue(const AString& name,
                                 float values[],
                                 const int32_t arrayNumberOfElements,
                                 const float defaultValue = 0.0) const;
        
        int32_t getIntegerValue(const AString& name,
                             const int32_t defaultValue = 0) const;
        
        int32_t getIntegerArrayValue(const AString& name,
                                   int32_t values[],
                                   const int32_t arrayNumberOfElements,
                                   const int32_t defaultValue = 0.0) const;
        
        AString getPathNameValue(const AString& name,
                                 const AString& defaultValue = "") const;
        
        AString getStringValue(const AString& name,
                             const AString& defaultValue = "") const;
        
        int32_t getStringArrayValue(const AString& name,
                                     AString values[],
                                     const int32_t arrayNumberOfElements,
                                     const AString& defaultValue = "") const;
        
        const ScenePrimitive* getPrimitive(const AString& name) const;
        
        const ScenePrimitiveArray* getPrimitiveArray(const AString& name) const;
        
        const ScenePathName* getPathName(const AString& name) const;
        
        const SceneObjectMapIntegerKey* getMapIntegerKey(const AString& name) const;
        
        int32_t getNumberOfObjects() const;
        
        const SceneObject* getObjectAtIndex(const int32_t indx) const;
        
        const SceneObject* getObjectWithName(const AString& name) const;
        
        // ADD_NEW_METHODS_HERE

//    private: // and are not used
//        void addBooleanVector(const AString& name,
//                              const std::vector<bool>& values);
//        
//        void addClassArray(const AString& name,
//                           SceneClass* values[],
//                           const int32_t arrayNumberOfElements);
//        
//        void addEnumeratedTypeArray(const AString& name,
//                                    const AString value[],
//                                    const int32_t arrayNumberOfElements);
        
        
    private:
        SceneClass(const SceneClass&);
        
        SceneClass& operator=(const SceneClass&);

        void addEnumeratedType(const AString& name,
                               const AString& value);
        
        AString getEnumeratedTypeValue(const AString& name,
                                       const AString& defaultValue = "") const;
        
        int32_t getEnumeratedTypeArrayValue(const AString& name,
                                            AString values[],
                                            const int32_t arrayNumberOfElements,
                                            const AString& defaultValue) const;
        
        void addEnumeratedTypeVector(const AString& name,
                                     const std::vector<AString>& value);
        
        void logMissing(const AString& missingInfo) const;
        
        AString m_className;
        
        const int32_t m_versionNumber;
        
        std::vector<SceneObject*> m_childObjects;
        
        // ADD_NEW_MEMBERS_HERE
        
    };
    
#ifdef __SCENE_CLASS_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __SCENE_CLASS_DECLARE__

} // namespace
#endif  //__SCENE_CLASS__H_
