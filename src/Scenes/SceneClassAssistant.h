#ifndef __SCENE_CLASS_ASSISTANT__H_
#define __SCENE_CLASS_ASSISTANT__H_

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


#include "CaretObject.h"
#include "SceneAttributes.h"
#include "SceneClass.h"

namespace caret {

    class SceneableInterface;
    
    class SceneClassAssistant : public CaretObject {
        
    public:
        SceneClassAssistant();
        
        virtual ~SceneClassAssistant();
        
        void restoreMembers(const SceneAttributes* sceneAttributes,
                            const SceneClass* sceneClass);
        
        void saveMembers(const SceneAttributes* sceneAttributes,
                         SceneClass* sceneClass);
        
    private:
        SceneClassAssistant(const SceneClassAssistant&);

        SceneClassAssistant& operator=(const SceneClassAssistant&);
        
    public:

        void add(const AString& name,
                 float* floatAddress);
        
        void add(const AString& name,
                 int32_t* intAddress);
        
        void add(const AString& name,
                 bool* boolAddress);
        
        void add(const AString& name,
                 AString* stringAddress);
        
//        void add(const AString& name,
//                 const AString& className,
//                 SceneableInterface** sceneClass);
        
        void add(const AString& name,
                 const AString& className,
                 SceneableInterface* sceneClass);
        
        /**
         * Add a enumerated type member.
         * @param name
         *    Name of member.
         * @param enumeratedValueAddress
         *    Address of the member.
         * @param defaultValue
         *    Value used if the member is not found when restoring scene.
         */
        template<class T, typename ET>
        void add(const AString& name,
                 ET* enumeratedValueAddress) {
            EnumeratedTypeData<T, ET>* etd = new EnumeratedTypeData<T, ET>(name,
                                              enumeratedValueAddress,
                                              *enumeratedValueAddress);
            m_dataStorage.push_back(etd);
        }
        
        void addArray(const AString& name,
                      bool* booleanArray,
                      const int32_t numberOfElements,
                      const bool defaultValue);
        
        void addArray(const AString& name,
                      float* floatArray,
                      const int32_t numberOfElements,
                      const float defaultValue);
        
        void addArray(const AString& name,
                      int32_t* integerArray,
                      const int32_t numberOfElements,
                      const int32_t defaultValue);
        
        /**
         * Add an enumerated type array.
         * @param name
         *    Name of array.
         * @param enumeratedValueArray
         *    The array.
         * @param numberOfElements
         *    Number of elements in the array.
         * @param defaultValue
         *    Value used for items not found.
         */
        template<class T, typename ET>
        void addArray(const AString& name,
                      ET* enumeratedValueArray,
                      const int32_t numberOfElements,
                      const ET defaultValue) {
            EnumeratedTypeArrayData<T, ET>* eta = new EnumeratedTypeArrayData<T, ET>(name,
                                                                             enumeratedValueArray,
                                                                             numberOfElements,
                                                                             defaultValue);
            m_dataStorage.push_back(eta);
        }
        
//        void addArray(const AString& name,
//                      SceneableInterface* sceneableInterfaceArray[],
//                      const int32_t numberOfElements);
        
        void addArray(const AString& name,
                      AString* stringArray,
                      const int32_t numberOfElements,
                      const AString& defaultValue);
        
        void addTabIndexedBooleanArray(const AString& name,
                                     bool* booleanArray);
        
        void addTabIndexedIntegerArray(const AString& name,
                                     int32_t* integerArray);
        
        void addTabIndexedFloatArray(const AString& name,
                                   float* floatArray);
        
        
        /**
         * Add a tab-indexed enumerated type array.
         * @param name
         *    Name of array.
         * @param enumeratedValueArray
         *    The array.
         */
        template<class T, typename ET>
        void addTabIndexedEnumeratedTypeArray(const AString& name,
                                              ET* enumeratedValueArray) {
            EnumeratedTypeTabIndexArrayMapData<T, ET>* etmap = new EnumeratedTypeTabIndexArrayMapData<T, ET>(name,
                                                                                              enumeratedValueArray);
            m_dataStorage.push_back(etmap);
        }
                                              
        // ADD_NEW_METHODS_HERE

    private:

        // ADD_NEW_MEMBERS_HERE

    private:
        
        // ADD_NEW_MEMBERS_HERE
        
        /* ========================================= */
        class Data {
        public:
            Data(const AString& name);
            virtual ~Data() { }
            
            virtual void restore(const SceneAttributes& sceneAttributes,
                                 const SceneClass& sceneClass) = 0;
            virtual void save(const SceneAttributes& sceneAttributes,
                              SceneClass& sceneClass) = 0;
        protected:
            const AString m_name;
            
        };
        /* ========================================= */
        /**
         * \class caret::SceneClassAssistant::EnumeratedTypeData 
         * \brief An enumeration value added to a scene class.
         * \ingroup Scene
         *
         * See the documentation in the class Scene for how to use the Scene system.
         */
        template <class T, typename ET>
        class EnumeratedTypeData : public Data {
        public:
            /**
             * Constructor.
             * @param name
             *    Name of data.
             * @param dataPointer
             *    Pointer to data.
             * @param defaultValue
             *    Default value used when restoring and data with name not found.
             */
            EnumeratedTypeData(const AString& name,
                               ET* dataPointer,
                               const ET defaultValue)
            : Data(name),
            m_dataPointer(dataPointer),
            m_defaultValue(defaultValue){
                
            }
            
            virtual ~EnumeratedTypeData() { }
            
            /**
             * Restore the data from the scene.
             * @param sceneAttributes
             *    Attributes of the scene.
             * @param sceneClass
             *    Class from  which data is restored.
             */
            void restore(const SceneAttributes& /*sceneAttributes*/,
                         const SceneClass& sceneClass) {
                *m_dataPointer = sceneClass.getEnumeratedTypeValue<T, ET>(m_name, 
                                                                          m_defaultValue);
            }
            
            /**
             * Save the data to the scene.
             * @param sceneAttributes
             *    Attributes for the scene.
             * @param sceneClass
             *    Class to which data is saved.
             */
            void save(const SceneAttributes& /*sceneAttributes*/,
                      SceneClass& sceneClass) {
                sceneClass.addEnumeratedType<T, ET>(m_name,
                                                    *m_dataPointer);
            }
            
        private:
            ET* m_dataPointer;
            const ET m_defaultValue; 
        };
        
        /* ========================================= */
        class FloatData : public Data {
        public:
            FloatData(const AString& name,
                      float* dataPointer,
                      const float defaultValue);
            
            virtual ~FloatData() { }
            
            void restore(const SceneAttributes& sceneAttributes,
                         const SceneClass& sceneClass);
            void save(const SceneAttributes& sceneAttributes,
                      SceneClass& sceneClass);
            
        private:
            float* m_dataPointer;
            float m_defaultValue; 
        };
        
        /* ========================================= */
        class IntegerData : public Data {
        public:
            IntegerData(const AString& name,
                      int32_t* dataPointer,
                        const int32_t defaultValue);
            
            virtual ~IntegerData() { }
            
            void restore(const SceneAttributes& sceneAttributes,
                         const SceneClass& sceneClass);
            void save(const SceneAttributes& sceneAttributes,
                      SceneClass& sceneClass);
            
        private:
            int32_t* m_dataPointer;
            int32_t m_defaultValue; 
        };
        
        /* ========================================= */
        class BooleanData : public Data {
        public:
            BooleanData(const AString& name,
                    bool* dataPointer,
                     const bool defaultValue);
            
            virtual ~BooleanData() { }
            
            void restore(const SceneAttributes& sceneAttributes,
                         const SceneClass& sceneClass);
            void save(const SceneAttributes& sceneAttributes,
                      SceneClass& sceneClass);
            
        private:
            bool* m_dataPointer;
            bool m_defaultValue; 
        };
        
        /* ========================================= */
        class StringData : public Data {
        public:
            StringData(const AString& name,
                        AString* dataPointer,
                        const AString& defaultValue);
            
            virtual ~StringData() { }
            
            void restore(const SceneAttributes& sceneAttributes,
                         const SceneClass& sceneClass);
            void save(const SceneAttributes& sceneAttributes,
                      SceneClass& sceneClass);
            
        private:
            AString* m_dataPointer;
            AString m_defaultValue; 
        };
        
        /* ========================================= */
        class ClassData : public Data {
        public:
            ClassData(const AString& name,
                      const AString& className,
                      SceneableInterface** sceneClassHandle);
            
            ClassData(const AString& name,
                      const AString& className,
                      SceneableInterface* sceneClassPointer);
            
            virtual ~ClassData() { }
            
            void restore(const SceneAttributes& sceneAttributes,
                         const SceneClass& sceneClass);
            void save(const SceneAttributes& sceneAttributes,
                      SceneClass& sceneClass);
            
        private:
            const AString m_className;
            SceneableInterface** m_sceneClassHandle;
            SceneableInterface*  m_sceneClassPointer;
        };
        
        /* ========================================= */
        class TabIndexArrayMapData : public Data {
        public:
            TabIndexArrayMapData(const AString& name);
            virtual ~TabIndexArrayMapData() { }
        };
        
        /* ========================================= */
        class BooleanTabIndexArrayMapData : public TabIndexArrayMapData {
        public:
            BooleanTabIndexArrayMapData(const AString& name,
                                        bool* booleanArray);
            virtual ~BooleanTabIndexArrayMapData() { }
            
            void restore(const SceneAttributes& sceneAttributes,
                         const SceneClass& sceneClass);
            void save(const SceneAttributes& sceneAttributes,
                      SceneClass& sceneClass);
        private:
            bool* m_booleanArray;
        };
        
        /* ========================================= */
        class IntegerTabIndexArrayMapData : public TabIndexArrayMapData {
        public:
            IntegerTabIndexArrayMapData(const AString& name,
                                        int32_t* integerArray);
            virtual ~IntegerTabIndexArrayMapData() { }
            
            void restore(const SceneAttributes& sceneAttributes,
                         const SceneClass& sceneClass);
            void save(const SceneAttributes& sceneAttributes,
                      SceneClass& sceneClass);
        private:
            int32_t* m_integerArray;
        };
        
        /* ========================================= */
        class FloatTabIndexArrayMapData : public TabIndexArrayMapData {
        public:
            FloatTabIndexArrayMapData(const AString& name,
                                        float* floatArray);
            virtual ~FloatTabIndexArrayMapData() { }
            
            void restore(const SceneAttributes& sceneAttributes,
                         const SceneClass& sceneClass);
            void save(const SceneAttributes& sceneAttributes,
                      SceneClass& sceneClass);
        private:
            float* m_floatArray;
        };
        
        /* ========================================= */
        template <class T, typename ET>
        class EnumeratedTypeTabIndexArrayMapData : public TabIndexArrayMapData {
        public:
            EnumeratedTypeTabIndexArrayMapData(const AString& name,
                                    ET* enumValueArray) 
            : TabIndexArrayMapData(name),
            m_enumValueArray(enumValueArray) {
                
            }
            
            virtual ~EnumeratedTypeTabIndexArrayMapData() { }
            
            void restore(const SceneAttributes& /*sceneAttributes*/,
                         const SceneClass& sceneClass) {
                sceneClass.getEnumerateTypeArrayForTabIndices<T, ET>(m_name, 
                                                                     m_enumValueArray);
            }
            void save(const SceneAttributes& sceneAttributes,
                      SceneClass& sceneClass) {
                sceneClass.addEnumeratedTypeArrayForTabIndices<T, ET>(m_name, 
                                                                     m_enumValueArray,
                                                                     sceneAttributes.getIndicesOfTabsForSavingToScene());
            }
        private:
            ET* m_enumValueArray;
        };
        
        /* ========================================= */
//        class ClassTabIndexArrayMapData : public TabIndexArrayMapData {
//        public:
//            ClassTabIndexArrayMapData(const AString& name,
//                                      SceneableInterface* sceneInterface);
//            virtual ~ClassTabIndexArrayMapData() { }
//            
//            void restore(const SceneAttributes& sceneAttributes,
//                         const SceneClass& sceneClass);
//            void save(const SceneAttributes& sceneAttributes,
//                      SceneClass& sceneClass);
//        private:
//            SceneableInterface* m_sceneInterface;
//        };
        
        /* ========================================= */
        class ArrayData : public Data {
        public:
            ArrayData(const AString& name,
                      const int32_t numberOfArrayElements);
            
            virtual ~ArrayData() {} 
        protected:
            int32_t m_numberOfArrayElements;
        };
        
        /* ========================================= */
        class BooleanArrayData : public ArrayData {
        public:
            BooleanArrayData(const AString& name,
                             bool* booleanArray,
                             const int32_t numberOfArrayElements,
                             const bool defaultValue);
            
            virtual ~BooleanArrayData() { }
            void restore(const SceneAttributes& sceneAttributes,
                         const SceneClass& sceneClass);
            void save(const SceneAttributes& sceneAttributes,
                      SceneClass& sceneClass);
        private:
            bool* m_booleanArray;
            bool m_defaultValue;
        };
        
        /* ========================================= */
        class IntegerArrayData : public ArrayData {
        public:
            IntegerArrayData(const AString& name,
                             int32_t* integerArray,
                             const int32_t numberOfArrayElements,
                             const int32_t defaultValue);
            
            virtual ~IntegerArrayData() { }
            void restore(const SceneAttributes& sceneAttributes,
                         const SceneClass& sceneClass);
            void save(const SceneAttributes& sceneAttributes,
                      SceneClass& sceneClass);
        private:
            int32_t* m_integerArray;
            int32_t m_defaultValue;
        };
        
        /* ========================================= */
        class FloatArrayData : public ArrayData {
        public:
            FloatArrayData(const AString& name,
                             float* floatArray,
                             const int32_t numberOfArrayElements,
                             const float defaultValue);
            
            virtual ~FloatArrayData() { }
            void restore(const SceneAttributes& sceneAttributes,
                         const SceneClass& sceneClass);
            void save(const SceneAttributes& sceneAttributes,
                      SceneClass& sceneClass);
        private:
            float* m_floatArray;
            float m_defaultValue;
        };
        
        /* ========================================= */
        class StringArrayData : public ArrayData {
        public:
            StringArrayData(const AString& name,
                           AString* stringArray,
                           const int32_t numberOfArrayElements,
                           const AString defaultValue);
            
            virtual ~StringArrayData() { }
            void restore(const SceneAttributes& sceneAttributes,
                         const SceneClass& sceneClass);
            void save(const SceneAttributes& sceneAttributes,
                      SceneClass& sceneClass);
        private:
            AString* m_stringArray;
            AString m_defaultValue;
        };
        
        /* ========================================= */
//        class ClassArrayData : public ArrayData {
//        public:
//            ClassArrayData(const AString& name,
//                            SceneableInterface* sceneInterfaces[],
//                            const int32_t numberOfArrayElements);
//            
//            virtual ~ClassArrayData() { }
//            void restore(const SceneAttributes& sceneAttributes,
//                         const SceneClass& sceneClass);
//            void save(const SceneAttributes& sceneAttributes,
//                      SceneClass& sceneClass);
//        private:
//            SceneableInterface** m_sceneInterfaces;
//        };
        
        /* ========================================= */
        template <class T, typename ET>
        class EnumeratedTypeArrayData : public ArrayData {
        public:
            EnumeratedTypeArrayData(const AString& name,
                                    ET* enumValueArray,
                                    const int32_t numberOfArrayElements,
                                    const ET defaultValue) 
            : ArrayData(name,
             numberOfArrayElements),
             m_enumValueArray(enumValueArray),
             m_defaultValue(defaultValue) {
                
            }
            
            virtual ~EnumeratedTypeArrayData() { }
            
            void restore(const SceneAttributes& /*sceneAttributes*/,
                         const SceneClass& sceneClass) {
                if (m_numberOfArrayElements > 0) {
                    sceneClass.getEnumerateTypeArray<T, ET>(m_name,
                                                           m_enumValueArray,
                                                           m_numberOfArrayElements,
                                                           m_defaultValue);
                }
                
            }
            void save(const SceneAttributes& /*sceneAttributes*/,
                      SceneClass& sceneClass) {
                sceneClass.addEnumeratedTypeArray<T, ET>(m_name,
                                                        m_enumValueArray, 
                                                        m_numberOfArrayElements);
            }
        private:
            ET* m_enumValueArray;
            const ET m_defaultValue;
        };
        
        /* ========================================= */
//        class BooleanVectorData : public Data {
//        public:
//            BooleanVectorData(const AString& name,
//                              std::vector<bool>& booleanVectorReference,
//                              const bool defaultValue);
//            
//            virtual~BooleanVectorData() { }
//            
//            void restore(const SceneAttributes& sceneAttributes,
//                         const SceneClass& sceneClass);
//            void save(const SceneAttributes& sceneAttributes,
//                      SceneClass& sceneClass);
//        private:
//            std::vector<bool>& m_booleanVectorReference;
//            const bool m_defaultValue; 
//        };
        
        /* ========================================= */
//        class FloatVectorData : public Data {
//        public:
//            FloatVectorData(const AString& name,
//                            std::vector<float>* floatVectorPointer,
//                            const float defaultValue);
//            
//            virtual~FloatVectorData() { }
//            
//            void restore(const SceneAttributes& sceneAttributes,
//                         const SceneClass& sceneClass);
//            void save(const SceneAttributes& sceneAttributes,
//                      SceneClass& sceneClass);
//        private:
//            std::vector<float>* m_booleanVectorPointer;
//            const float m_defaultValue; 
//        };
        
        /* ========================================= */
        typedef std::vector<Data*> DataStorage;
        typedef DataStorage::iterator DataStorageIterator;
        DataStorage m_dataStorage;
        
    };
    
#ifdef __SCENE_CLASS_ASSISTANT_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __SCENE_CLASS_ASSISTANT_DECLARE__

} // namespace
#endif  //__SCENE_CLASS_ASSISTANT__H_
