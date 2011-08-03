
#ifndef __GIFTI_META_DATA_SAX_READER_H__
#define __GIFTI_META_DATA_SAX_READER_H__

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
#include <string>
#include <stdint.h>

#include "XmlSaxParserException.h"
#include "XmlSaxParserHandlerInterface.h"


namespace caret {

    class GiftiMetaData;
    class XmlAttributes;
    
    /**
     * class for reading Gifti Metadata with a SAX Parser
     */
    class GiftiMetaDataSaxReader : public CaretObject, public XmlSaxParserHandlerInterface {
    public:
        GiftiMetaDataSaxReader(GiftiMetaData* metaData);
        
        virtual ~GiftiMetaDataSaxReader();
        
        void startElement(const std::string& namespaceURI,
                          const std::string& localName,
                          const std::string& qName,
                          const XmlAttributes& attributes) throw (XmlSaxParserException);
        
        void endElement(const std::string& namspaceURI,
                        const std::string& localName,
                        const std::string& qName) throw (XmlSaxParserException);
        
        void characters(const char* ch) throw (XmlSaxParserException);
        
        void fatalError(const XmlSaxParserException& e) throw (XmlSaxParserException);
        
        void warning(const XmlSaxParserException& e) throw (XmlSaxParserException);
        
        void error(const XmlSaxParserException& e) throw (XmlSaxParserException);
        
        void startDocument() throw (XmlSaxParserException);
        
        void endDocument() throw (XmlSaxParserException);
        
        
    private:
        GiftiMetaDataSaxReader(const GiftiMetaDataSaxReader&);
        GiftiMetaDataSaxReader& operator=(const GiftiMetaDataSaxReader&);
        
    protected:
        /// file reading states
        enum STATE {
            /// no state
            STATE_NONE,
            /// processing MetaData tag
            STATE_METADATA,
            /// processing MetaData MD child tag
            STATE_METADATA_MD,
            /// processing MetaData MD Name tag
            STATE_METADATA_MD_NAME,
            /// processing MetaData MD Value tag
            STATE_METADATA_MD_VALUE
        };
        
        /// file reading state
        STATE state;
        
        /// the state stack used when reading a file
        std::stack<STATE> stateStack;
        
        /// the error message
        std::string errorMessage;
        
        /// meta data name
        std::string metaDataName;
        
        /// meta data value
        std::string metaDataValue;
        
        /// element text
        std::string elementText;
        
        /// GIFTI meta data being read
        GiftiMetaData* metaData;
        
    };
    
} // namespace

#endif // __GIFTI_META_DATA_SAX_READER_H__

