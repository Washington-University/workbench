#ifndef __SCENE__H_
#define __SCENE__H_

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
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITqNESS FOR A PARTICULAR PURPOSE
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
#include "SceneTypeEnum.h"
#include "XmlException.h"

namespace caret {
    class SceneAttributes;
    class SceneClass;
    class XmlWriter;
    
    class Scene : public CaretObject {
        
    public:
        Scene(const SceneTypeEnum::Enum sceneType);
        
        virtual ~Scene();
        
    private:
        Scene(const Scene&);

        Scene& operator=(const Scene&);
        
    public:

        const SceneAttributes* getSceneAttributes() const;

        SceneAttributes* getSceneAttributes();

        int32_t getNumberOfSceneClasses() const;
        
        const SceneClass* getSceneClassAtIndex(const int32_t indx) const;

        const SceneClass* getSceneClassWithName(const AString& sceneClassName) const;
        
        void writeAsXML(XmlWriter& xmlWriter,
                        const int32_t sceneIndex) throw (XmlException);
        
     
        AString getSceneName() const;

        void setSceneName(const AString& sceneName);


        // ADD_NEW_METHODS_HERE

        /** XML Tag for scene */
        static const AString XML_TAG_SCENE;
        
        /** XML Tag for scene name */
        static const AString XML_TAG_SCENE_NAME;
        
        /** XML Attributes for scene index */
        static const AString XML_ATTRIBUTE_SCENE_INDEX;
        
        
    private:

        /** Attributes of the scene*/
        SceneAttributes* m_sceneAttributes;

        /** Classes contained in the scene*/
        std::vector<SceneClass*> m_sceneClasses;


        /** name of scene*/
        AString m_sceneName;


        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __SCENE_DECLARE__
    const AString Scene::XML_TAG_SCENE = "Scene";
    const AString Scene::XML_TAG_SCENE_NAME = "SceneName";
    const AString Scene::XML_ATTRIBUTE_SCENE_INDEX = "SceneIndex";
#endif // __SCENE_DECLARE__

} // namespace
#endif  //__SCENE__H_
