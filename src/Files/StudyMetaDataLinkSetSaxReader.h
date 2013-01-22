
#ifndef __STUDY_META_DATA_LINK_SET_SAX_READER_H__
#define __STUDY_META_DATA_LINK_SET_SAX_READER_H__

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
#include <stdint.h>

#include "AString.h"
#include "XmlSaxParserException.h"
#include "XmlSaxParserHandlerInterface.h"


namespace caret {

    class StudyMetaDataLink;
    class StudyMetaDataLinkSet;
    class XmlAttributes;
    
    /**
     * class for reading StudyMetaDataLinkSet with a SAX Parser
     */
    class StudyMetaDataLinkSetSaxReader : public CaretObject, public XmlSaxParserHandlerInterface {
    public:
        StudyMetaDataLinkSetSaxReader(StudyMetaDataLinkSet* studyMetaDataLinKSet);
        
        virtual ~StudyMetaDataLinkSetSaxReader();
        
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
        
        
    private:
        StudyMetaDataLinkSetSaxReader(const StudyMetaDataLinkSetSaxReader&);
        StudyMetaDataLinkSetSaxReader& operator=(const StudyMetaDataLinkSetSaxReader&);
        
    protected:
        /// file reading states
        enum STATE {
            /// no state
            STATE_NONE,
            /// processing SurfaceProjectedItem tags
            STATE_STUDY_META_DATA_LINK_SET,
            /// processing StudyMetaDataLink tags
            STATE_STUDY_META_DATA_LINK,
        };
        
        /// file reading state
        STATE m_state;
        
        /// the state stack used when reading a file
        std::stack<STATE> m_stateStack;
        
        /// the error message
        AString m_errorMessage;
        
        /// meta data name
        AString m_metaDataName;
        
        /// meta data value
        AString m_metaDataValue;
        
        /// element text
        AString m_elementText;
        
        /// GIFTI meta data being read
        StudyMetaDataLinkSet* m_studyMetaDataLinkSet;
        
        StudyMetaDataLink* m_studyMetaDataLinkBeingRead;
        
    };
    
} // namespace

#endif // __STUDY_META_DATA_LINK_SET_SAX_READER_H__

