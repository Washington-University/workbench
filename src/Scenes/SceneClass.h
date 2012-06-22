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


#include "SceneObject.h"

namespace caret {

    class SceneEnumeratedType;
    class ScenePrimitive;
    
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
        
        void addClass(SceneClass* sceneClass);
        
        void addEnumeratedType(const AString& name,
                               const AString& value);
        
        void addFloat(const AString& name,
                      const float value);
        
        void addInteger(const AString& name,
                        const int32_t value);
        
        void addString(const AString& name,
                       const AString& value);
        
        
        bool getBooleanValue(const AString& name,
                             const bool defaultValue = false) const;
        
        const SceneClass* getClass(const AString& name) const;
        
        SceneClass* getClass(const AString& name);
        
        AString getEnumeratedTypeValue(const AString& name,
                                       const AString& defaultValue = "") const;
        
        float getFloatValue(const AString& name,
                             const float defaultValue = 0.0) const;
        
        int32_t getIntegerValue(const AString& name,
                             const int32_t defaultValue = 0) const;
        
        AString getStringValue(const AString& name,
                             const AString& defaultValue = "") const;
        
        int32_t getNumberOfPrimitives() const;
        
        const ScenePrimitive* getPrimitiveAtIndex(const int32_t indx) const;
        
        int32_t getNumberOfEnumeratedTypes() const;
        
        const SceneEnumeratedType* getEnumeratedTypeAtIndex(const int32_t indx) const;
        
        int32_t getNumberOfClasses() const;
        
        const SceneClass* getClassAtIndex(const int32_t indx) const;
        
        // ADD_NEW_METHODS_HERE

    private:
        SceneClass(const SceneClass&);
        
        SceneClass& operator=(const SceneClass&);

        const ScenePrimitive* getPrimitive(const AString& name) const;
        
        AString m_className;
        
        const int32_t m_versionNumber;
        
        std::vector<SceneClass*> m_childClasses;
        
        std::vector<ScenePrimitive*> m_childPrimitives;

        std::vector<SceneEnumeratedType*> m_childEnumeratedTypes;
        
        // ADD_NEW_MEMBERS_HERE
        
    };
    
#ifdef __SCENE_CLASS_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __SCENE_CLASS_DECLARE__

} // namespace
#endif  //__SCENE_CLASS__H_
