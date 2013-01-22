
#ifndef __SCENE_FILE_SAX_READER_H__
#define __SCENE_FILE_SAX_READER_H__

/*LICENSE_START*/
/*
 *  Copyright 1995-2002 Washington University School of Medicine
 *
 *  http://brainmap.wustl.edu
 *
 *  This file is part of CARET.
 *
 *  CARET is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  CARET is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with CARET; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */
/*LICENSE_END*/

#include <stack>
#include <AString.h>
#include <stdint.h>

#include "SceneSaxReader.h"
#include "XmlSaxParserException.h"
#include "XmlSaxParserHandlerInterface.h"


namespace caret {

    class Scene;
    class SceneFile;
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
                          const XmlAttributes& attributes) throw (XmlSaxParserException);
        
        void endElement(const AString& namspaceURI,
                        const AString& localName,
                        const AString& qName) throw (XmlSaxParserException);
        
        void characters(const char* ch) throw (XmlSaxParserException);
        
        void fatalError(const XmlSaxParserException& e) throw (XmlSaxParserException);
        
        void warning(const XmlSaxParserException& e) throw (XmlSaxParserException);
        
        void error(const XmlSaxParserException& e) throw (XmlSaxParserException);
        
        void startDocument() throw (XmlSaxParserException);
        
        void endDocument() throw (XmlSaxParserException);
        
        
    protected:
        /// file reading states
        enum STATE {
            /// no state
            STATE_NONE,
            /// processing SceneFile tag
            STATE_SCENE_FILE,
            /// processing MetaData tag
            STATE_METADATA,
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
        
        /// element text
        AString m_elementText;
        
        /// GIFTI meta data sax reader
        GiftiMetaDataSaxReader* m_metaDataSaxReader;   
        
        /// Scene sax reader
        SceneSaxReader* m_sceneSaxReader;
    };

} // namespace

#endif // __SCENE_FILE_SAX_READER_H__

