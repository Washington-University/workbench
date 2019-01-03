
#ifndef __SCENE_INFO_SAX_READER_H__
#define __SCENE_INFO_SAX_READER_H__

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

#include <stack>
#include <stdint.h>

#include "AString.h"
#include "XmlSaxParserException.h"
#include "XmlSaxParserHandlerInterface.h"


namespace caret {

    class SceneInfo;
    class XmlAttributes;
    
    /**
     * class for reading a Scene Info with a SAX Parser
     */
    class SceneInfoSaxReader : public CaretObject, public XmlSaxParserHandlerInterface {
    public:
        SceneInfoSaxReader(const AString& sceneFileName,
                       SceneInfo* sceneInfo);
        
        virtual ~SceneInfoSaxReader();
        
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
            /// processing Scene Info tag
            STATE_SCENE_INFO,
            /// processing Balsa Scene ID tag
            STATE_SCENE_INFO_BALSA_ID,
            /// processing Scene Info Name tag
            STATE_SCENE_INFO_NAME,
            /// processing Scene Info Description tag
            STATE_SCENE_INFO_DESCRIPTION,
            /// processing Scene Info thumbnail tag
            STATE_SCENE_INFO_IMAGE_THUMBNAIL,
            /// processing Scene Info macro group tag
            STATE_SCENE_INFO_MACRO_GROUP,
            /// process an unrecognized element in Scene Info
            STATE_SCENE_INFO_UNRECOGNIZED
        };
        
        /// name of scene file
        AString m_sceneFileName;
        
        /// file reading state
        STATE m_state;
        
        /// the state stack used when reading a file
        std::stack<STATE> m_stateStack;
        
        /// the error message
        AString m_errorMessage;
        
        /// element text
        AString m_elementText;
        
        /// Scene being read
        SceneInfo* m_sceneInfo;
        
        /** Index of scene info */
        int32_t m_sceneInfoIndex;
       
        /** image format */
        AString m_imageFormat;
        
        /** image encoding */
        AString m_imageEncoding;
    };

} // namespace

#endif // __SCENE_INFO_SAX_READER_H__

