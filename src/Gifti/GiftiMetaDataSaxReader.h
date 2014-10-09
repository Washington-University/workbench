
#ifndef __GIFTI_META_DATA_SAX_READER_H__
#define __GIFTI_META_DATA_SAX_READER_H__

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
#include <AString.h>
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
        AString errorMessage;
        
        /// meta data name
        AString metaDataName;
        
        /// meta data value
        AString metaDataValue;
        
        /// element text
        AString elementText;
        
        /// GIFTI meta data being read
        GiftiMetaData* metaData;
        
    };
    
} // namespace

#endif // __GIFTI_META_DATA_SAX_READER_H__

