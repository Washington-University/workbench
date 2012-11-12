
#ifndef __FOCI_FILE_SAX_READER_H__
#define __FOCI_FILE_SAX_READER_H__

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

#include "XmlSaxParserException.h"
#include "XmlSaxParserHandlerInterface.h"


namespace caret {

    class Focus;
    class FociFile;
    class GiftiLabelTableSaxReader;
    class GiftiMetaDataSaxReader;
    class StudyMetaDataLinkSetSaxReader;
    class SurfaceProjectedItem;
    class SurfaceProjectedItemSaxReader;
    class XmlAttributes;
    class XmlException;
    
    class FociFileSaxReader : public CaretObject, public XmlSaxParserHandlerInterface {
    public:
        FociFileSaxReader(FociFile* fociFile);
        
        virtual ~FociFileSaxReader();
        
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
            /// processing FociFile tag
            STATE_FOCI_FILE,
            /// processing MetaData tag
            STATE_METADATA,
            /// processing version one color table tag
            STATE_VERSION_ONE_COLOR_TABLE,
            /// processing class color table tag
            STATE_CLASS_COLOR_TABLE,
            /// processing name color table tag
            STATE_NAME_COLOR_TABLE,
            /// processing focus
            STATE_FOCUS,
            /// processing StudyMetaDataLinkSet tag
            STATE_STUDY_META_DATA_LINK_SET,
            /// processing SurfaceProjectedItem tag
            STATE_SURFACE_PROJECTED_ITEM
        };
        
        /// file reading state
        STATE m_state;
        
        /// the state stack used when reading a file
        std::stack<STATE> m_stateStack;
        
        /// the error message
        AString m_errorMessage;
        
        /// Foci file that is being read
        FociFile* m_fociFile;
        
        /// Focus that is being read
        Focus* m_focus;
        
        /// Counts projections as they are read
        int32_t m_projectionCounter;
        
        /// surface projected item that is being read
        SurfaceProjectedItem* m_surfaceProjectedItem;
        
        /// Reads a SurfaceProjectedItem
        SurfaceProjectedItemSaxReader* m_surfaceProjectedItemSaxReader;
        
        /// element text
        AString m_elementText;
        
        /// GIFTI meta data sax reader
        GiftiMetaDataSaxReader* m_metaDataSaxReader;   
        
        /// GIFTI Label Table SAX Reader;
        GiftiLabelTableSaxReader* m_labelTableSaxReader;

        /** 
         * Version 1 had only one color table that contained names and classes 
         * After reading, split into name and class color tables
         */
        GiftiLabelTable* m_versionOneColorTable;
        
        /** Version of file being read */
        int32_t m_versionBeingRead;
        
        /// Study meta data link set reader
        StudyMetaDataLinkSetSaxReader* m_studyMetaDataLinkSetSaxReader;
    };

} // namespace

#endif // __FOCI_FILE_SAX_READER_H__

