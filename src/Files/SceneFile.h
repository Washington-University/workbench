#ifndef __SCENE_FILE__H_
#define __SCENE_FILE__H_

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


#include "CaretDataFile.h"

namespace caret {

    class Scene;
    
    class SceneFile : public CaretDataFile {
        
    public:
        SceneFile();
        
        virtual ~SceneFile();
        
    private:
        SceneFile(const SceneFile&);

        SceneFile& operator=(const SceneFile&);
        
    public:

        void clear();
        
        void readFile(const AString& filename) throw (DataFileException);
        
        void writeFile(const AString& filename) throw (DataFileException);
        
        bool isEmpty() const;

        void addScene(Scene* scene);
        
        int32_t getNumberOfScenes() const;
        
        Scene* getSceneAtIndex(const int32_t indx);
        
        void removeSceneAtIndex(const int32_t indx);
        
        StructureEnum::Enum getStructure() const;
        
        void setStructure(const StructureEnum::Enum structure);
        
        GiftiMetaData* getFileMetaData();
        
        const GiftiMetaData* getFileMetaData() const;
        

        // ADD_NEW_METHODS_HERE

        /** XML Tag for scene file */
        static const AString XML_TAG_SCENE_FILE;
        
        /** XML Tag for Version attribute */
        static const AString XML_ATTRIBUTE_VERSION;
        
    private:

        /** the scenes*/
        std::vector<Scene*> m_scenes;

        /** the metadata */
        GiftiMetaData* m_metadata;

        // ADD_NEW_MEMBERS_HERE

        /** Version of this SceneFile */
        static const float sceneFileVersion;
    };
    
#ifdef __SCENE_FILE_DECLARE__
    const AString SceneFile::XML_TAG_SCENE_FILE = "SceneFile";
    const AString SceneFile::XML_ATTRIBUTE_VERSION = "Version";
    const float SceneFile::sceneFileVersion = 1.0;
#endif // __SCENE_FILE_DECLARE__

} // namespace
#endif  //__SCENE_FILE__H_
