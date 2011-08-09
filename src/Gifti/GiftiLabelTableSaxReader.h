
#ifndef __GIFTI_LABEL_TABLE_SAX_READER_H__
#define __GIFTI_LABEL_TABLE_SAX_READER_H__

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
#include <QString>
#include <stdint.h>

#include "XmlSaxParserException.h"
#include "XmlSaxParserHandlerInterface.h"


namespace caret {

    class GiftiLabelTable;
    class XmlAttributes;
    
    /**
     * class for reading a GiftiLabelTable with a SAX Parser
     */
    class GiftiLabelTableSaxReader : public CaretObject, public XmlSaxParserHandlerInterface {
    public:
        GiftiLabelTableSaxReader(GiftiLabelTable* labelTable);
        
        virtual ~GiftiLabelTableSaxReader();
        
        void startElement(const QString& namespaceURI,
                          const QString& localName,
                          const QString& qName,
                          const XmlAttributes& attributes) throw (XmlSaxParserException);
        
        void endElement(const QString& namspaceURI,
                        const QString& localName,
                        const QString& qName) throw (XmlSaxParserException);
        
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
            /// processing LabelTable tag
            STATE_LABEL_TABLE,
            /// processing LabelTable Label
            STATE_LABEL_TABLE_LABEL
        };
        
        /// file reading state
        STATE state;
        
        /// the state stack used when reading a file
        std::stack<STATE> stateStack;
        
        /// the error message
        QString errorMessage;
        
        /// element text
        QString elementText;
        
        /// GIFTI label table being read
        GiftiLabelTable* labelTable;
        
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

#endif // __GIFTI_LABEL_TABLE_SAX_READER_H__

