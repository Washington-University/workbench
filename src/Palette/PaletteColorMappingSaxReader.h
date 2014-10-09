
#ifndef __PALETTE_COLOR_MAPPING_SAX_READER_H__
#define __PALETTE_COLOR_MAPPING_SAX_READER_H__

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

    class PaletteColorMapping;
    class XmlAttributes;
    
    /**
     * Class for reading a PaletteColorMapping object with a SAX Parser
     */
    class PaletteColorMappingSaxReader : public CaretObject, public XmlSaxParserHandlerInterface {
    public:
        PaletteColorMappingSaxReader(PaletteColorMapping* paletteColorMapping);
        
        virtual ~PaletteColorMappingSaxReader();
        
    private:
        PaletteColorMappingSaxReader(const PaletteColorMappingSaxReader&);
        PaletteColorMappingSaxReader& operator=(const PaletteColorMappingSaxReader&);
        
    public:
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
            /// reading elements
            STATE_READING_ELEMENTS
        };
        
        /// file reading state
        STATE state;
        
        /// the state stack used when reading a file
        std::stack<STATE> stateStack;
        
        /// the error message
        AString errorMessage;
        
        /// element text
        AString elementText;
        
        /// GIFTI label table being read
        PaletteColorMapping* paletteColorMapping;
        
        /// label index
        int labelIndex;
        
        /// label color component
        float labelRed;
        
        /// label color component
        float labelGreen;
        
        /// label color component
        float labelBlue;
        
        /// label color component
        float labelAlpha;
        
        /// label's X-coordinate
        float labelX;
        
        /// label's Y-coordinate
        float labelY;
        
        /// label's Z-coordinate
        float labelZ;
    };

} // namespace

#endif // __PALETTE_COLOR_MAPPING_SAX_READER_H__

