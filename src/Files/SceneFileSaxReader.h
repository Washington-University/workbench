
#ifndef __SCENE_FILE_SAX_READER_H__
#define __SCENE_FILE_SAX_READER_H__

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
#include <stack>
#include <stdint.h>

#include "AString.h"
#include "SceneSaxReader.h"
#include "XmlSaxParserException.h"
#include "XmlSaxParserHandlerInterface.h"


namespace caret {

    class Scene;
    class SceneFile;
    class SceneInfo;
    class SceneInfoSaxReader;
    class GiftiMetaDataSaxReader;
    class XmlAttributes;
    class XmlException;
    
    class SceneFileSaxReader : public CaretObject, public XmlSaxParserHandlerInterface {
    public:
        SceneFileSaxReader(SceneFile* sceneFile);
        
        virtual ~SceneFileSaxReader();
        
        void startElement(const AString& namespaceURI,
                          const AString& localName,
                          const AString& qName,
                          const XmlAttributes& attributes);
        
        void endElement(const AString& namspaceURI,
                        const AString& localName,
                        const AString& qName);
        
        void characters(const char* ch);
        
        void fatalError(const XmlSaxParserException& e);
        
        void warning(const XmlSaxParserException& e);
        
        void error(const XmlSaxParserException& e);
        
        void startDocument();
        
        void endDocument();
        
        
    protected:
        /// file reading states
        enum STATE {
            /// no state
            STATE_NONE,
            /// processing SceneFile tag
            STATE_SCENE_FILE,
            /// processing MetaData tag
            STATE_METADATA,
            /// processing SceneInfoDirectory tag
            STATE_SCENE_INFO_DIRECTORY,
            /// processing scene info Balsa Study ID tag
            STATE_SCENE_INFO_BALSA_STUDY_ID,
            /// processing scene info Balsa Study Title tag
            STATE_SCENE_INFO_BALSA_STUDY_TITLE,
            /// processing scene info Base Directory tag
            STATE_SCENE_INFO_BALSA_BASE_DIRECTORY,
            /// processing scene info Balsa Extract to Directory
            STATE_SCENE_INFO_BALSA_EXTRACT_TO_DIRECTORY,
            /// processing SceneInfo tag
            STATE_SCENE_INFO,
            /// processing Scene tag
            STATE_SCENE
        };
        
        /// file reading state
        STATE m_state;
        
        /// the state stack used when reading a file
        std::stack<STATE> m_stateStack;
        
        /// the error message
        AString m_errorMessage;
        
        /// scene file that is being read
        SceneFile* m_sceneFile;
        
        /// scene that is being read
        Scene* m_scene;
        
        /// scene info that is being read
        SceneInfo* m_sceneInfo;
        
        /// index attribute of scene info being read
        int32_t m_sceneInfoIndex;
        
        /// element text
        AString m_elementText;
        
        /// GIFTI meta data sax reader
        GiftiMetaDataSaxReader* m_metaDataSaxReader;   
        
        /// Scene sax reader
        SceneSaxReader* m_sceneSaxReader;
        
        /// Scene info sax reader
        SceneInfoSaxReader* m_sceneInfoSaxReader;
        
        /// map that stores scene info by index
        std::map<int32_t, SceneInfo*> m_sceneInfoMap;
    };

} // namespace

#endif // __SCENE_FILE_SAX_READER_H__

