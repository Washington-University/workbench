#ifndef __CARET_VOLUME_EXTENSION__
#define __CARET_VOLUME_EXTENSION__

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

#include "GiftiLabelTable.h"
#include "PaletteColorMapping.h"
#include "CaretPointer.h"
#include <vector>
#include "XmlWriter.h"
#include "XmlSaxParserHandlerInterface.h"
#include "PaletteColorMappingSaxReader.h"
#include "GiftiLabelTableSaxReader.h"
#include "GiftiMetaDataSaxReader.h"


namespace caret
{
    class GiftiMetaData;
    class XmlUnexpectedElementSaxParser;

    struct StudyMetadataLinkSet
    {
        void writeAsXML(XmlWriter& xmlWriter);
    };//TODO: make this do something useful
    
    struct SubvolumeAttributes
    {
        enum VolumeType
        {
            UNKNOWN,
            ANATOMY,
            FUNCTIONAL,
            LABEL,
            RGB,
            SEGMENTATION,
            VECTOR
        };//TODO: make this into a caret enum class?
        AString m_comment;
        AString m_guiLabel;
        CaretPointer<GiftiLabelTable> m_labelTable;
        CaretPointer<GiftiMetaData> m_metadata;
        StudyMetadataLinkSet m_studyMetadata;
        CaretPointer<PaletteColorMapping> m_palette;
        VolumeType m_type;
        SubvolumeAttributes() { m_type = UNKNOWN; }
        void writeAsXML(XmlWriter& xmlWriter, int index);
    };
    
    struct CaretVolumeExtension
    {
        mutable CaretPointer<GiftiMetaData> m_metadata;
        AString m_comment;
        AString m_date;//TODO: make a class to handle ISO-8601 dates
        std::vector<CaretPointer<SubvolumeAttributes> > m_attributes;
        void writeAsXML(XmlWriter& xmlWriter);
        void readFromXmlString(const AString& s);
        void clear();
    };
    
    class CaretVolumeExtensionXMLReader : public XmlSaxParserHandlerInterface
    {
        enum State
        {
            INVALID,
            CARET_EXTENSION,
            ROOT_COMMENT,
            DATE,
            VOLUME_INFORMATION,
            VI_COMMENT,
            GUI_LABEL,
            LABEL_TABLE,
            STUDY_META_DATA_LINK_SET,
            PALETTE_COLOR_MAPPING,
            VOLUME_TYPE,
            ROOT_META_DATA,
            VI_META_DATA,
            UNEXPECTED_XML
        };
        std::vector<State> m_stateStack;
        CaretVolumeExtension* m_toFill;
        std::vector<AString> m_charDataStack;
        int m_viIndex;
        CaretPointer<PaletteColorMappingSaxReader> m_paletteReader;
        CaretPointer<GiftiLabelTableSaxReader> m_labelReader;
        CaretPointer<GiftiMetaDataSaxReader> m_metadataReader;
        CaretPointer<XmlUnexpectedElementSaxParser> m_unexpectedXmlElementSaxParser;
        CaretVolumeExtensionXMLReader();//disallow default construction
        CaretVolumeExtensionXMLReader(const CaretVolumeExtensionXMLReader&);//disallow copy
        CaretVolumeExtensionXMLReader& operator=(const CaretVolumeExtensionXMLReader&);//disallow assignment
    public:
        CaretVolumeExtensionXMLReader(CaretVolumeExtension* toFill);
        virtual void startElement(const AString& uri,
                                  const AString& localName,
                                  const AString& qName,
                                  const XmlAttributes& atts) 
                     ;
        virtual void endElement(const AString& namespaceURI,
                                       const AString& localName,
                                       const AString& qualifiedName) 
                     ;
        virtual void characters(const char* ch);
        virtual void warning(const XmlSaxParserException& exception);
        virtual void error(const XmlSaxParserException& exception);
        virtual void fatalError(const XmlSaxParserException& exception);
        virtual void startDocument();
        virtual void endDocument();
    };

}

#endif //__CARET_VOLUME_EXTENSION__
