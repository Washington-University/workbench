
#ifndef __SPEC_FILE_SAX_READER_H__
#define __SPEC_FILE_SAX_READER_H__

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

    class SpecFile;
    class SpecFileDataFileTypeGroup;
    class GiftiMetaDataSaxReader;
    class XmlAttributes;
    class XmlException;
    
    /// class for reading a Spec File with a SAX Parser
    class SpecFileSaxReader : public CaretObject, public XmlSaxParserHandlerInterface {
    public:
        SpecFileSaxReader(SpecFile* specFileIn);
        
        virtual ~SpecFileSaxReader();
        
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
            /// processing SpecFile tag
            STATE_SPEC_FILE,
            /// processing MetaData tag
            STATE_METADATA,
            /// processing DataFile tag
            STATE_DATA_FILE
        };
        
        /// file reading state
        STATE state;
        
        /// the state stack used when reading a file
        std::stack<STATE> stateStack;
        
        /// the error message
        AString errorMessage;
        
        /// Spec file that is being read
        SpecFile* specFile;
        
        /// element text
        AString elementText;
        
        /// GIFTI meta data sax reader
        GiftiMetaDataSaxReader* metaDataSaxReader;
        
        /** value of File Structure attribute */
        AString fileAttributeStructureName;
        
        /** value of File type attribute */
        AString fileAttributeTypeName;
        
        /** value of File selection status */
        bool fileAttributeSelectionStatus;
    };

} // namespace

#endif // __SPEC_FILE_SAX_READER_H__

