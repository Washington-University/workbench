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

namespace caret {

    class SceneAttributes;
    class SceneClass;
    class SceneableInterface;
    
    class SceneClassAssistant : public CaretObject {
        
    public:
        SceneClassAssistant();
        
        virtual ~SceneClassAssistant();
        
        void restoreMembers(const SceneAttributes& sceneAttributes,
                            const SceneClass& sceneClass);
        
        void saveMembers(const SceneAttributes& sceneAttributes,
                         SceneClass& sceneClass);
        
    private:
        SceneClassAssistant(const SceneClassAssistant&);

        SceneClassAssistant& operator=(const SceneClassAssistant&);
        
    public:

        void add(const AString& name,
                 float* floatAddress,
                 const float defaultValue);
        
        void add(const AString& name,
                 int32_t* intAddress,
                 const int32_t defaultValue);
        
        void add(const AString& name,
                 bool* boolAddress,
                 const bool defaultValue);
        
        void add(const AString& name,
                 const AString& className,
                 SceneableInterface** sceneClass);
        
        void add(const AString& name,
                 const AString& className,
                 SceneableInterface* sceneClass);
        
        // ADD_NEW_METHODS_HERE

    private:

        // ADD_NEW_MEMBERS_HERE

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
        
        typedef std::vector<Data*> DataStorage;
        typedef DataStorage::iterator DataStorageIterator;
        DataStorage m_dataStorage;
        
    };
    
#ifdef __SCENE_CLASS_ASSISTANT_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __SCENE_CLASS_ASSISTANT_DECLARE__

} // namespace
#endif  //__SCENE_CLASS_ASSISTANT__H_
