
#ifndef __SURFACE_PROJECTED_ITEM_SAX_READER_H__
#define __SURFACE_PROJECTED_ITEM_SAX_READER_H__

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

    class SurfaceProjectedItem;
    class XmlAttributes;
    
    /**
     * class for reading SurfaceProjectedItem with a SAX Parser
     */
    class SurfaceProjectedItemSaxReader : public CaretObject, public XmlSaxParserHandlerInterface {
    public:
        SurfaceProjectedItemSaxReader(SurfaceProjectedItem* surfaceProjectedItem);
        
        virtual ~SurfaceProjectedItemSaxReader();
        
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
        SurfaceProjectedItemSaxReader(const SurfaceProjectedItemSaxReader&);
        SurfaceProjectedItemSaxReader& operator=(const SurfaceProjectedItemSaxReader&);
        
    protected:
        /// file reading states
        enum STATE {
            /// no state
            STATE_NONE,
            /// processing SurfaceProjectedItem tags
            STATE_SURFACE_PROJECTED_ITEM,
            /// processing Barycentric tags
            STATE_BARYCENTRIC,
            /// processing Van Essen tags
            STATE_VAN_ESSEN
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
        SurfaceProjectedItem* surfaceProjectedItem;
        
    };
    
} // namespace

#endif // __SURFACE_PROJECTED_ITEM_SAX_READER_H__

