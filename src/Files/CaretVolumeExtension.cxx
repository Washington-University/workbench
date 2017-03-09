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

#include "CaretVolumeExtension.h"
#include "XmlSaxParser.h"
#include "GiftiMetaData.h"
#include "GiftiMetaDataSaxReader.h"
#include "GiftiMetaDataXmlElements.h"
#include "GiftiXmlElements.h"
#include "PaletteColorMapping.h"
#include "PaletteColorMappingSaxReader.h"
#include "PaletteColorMappingXmlElements.h"
#include "PaletteNormalizationModeEnum.h"
#include "CaretLogger.h"
#include "XmlUnexpectedElementSaxParser.h"
#include <ctime>

using namespace caret;
using namespace std;

//where should these go?
static const AString CARET_VOL_EXT_ROOT = "CaretExtension";
static const AString CARET_VOL_EXT_COMMENT = "Comment";
static const AString CARET_VOL_EXT_DATE = "Date";
static const AString CARET_VOL_EXT_VOL_INFO = "VolumeInformation";
static const AString CARET_VOL_EXT_VI_COMMENT = "Comment";
static const AString CARET_VOL_EXT_VI_GUI_LABEL = "GuiLabel";
static const AString CARET_VOL_EXT_VI_STUDY_META_SET = "StudyMetaDataLinkSet";
static const AString CARET_VOL_EXT_VI_STUDY_META_LINK = "StudyMetaDataLink";
static const AString CARET_VOL_EXT_VI_TYPE = "VolumeType";

void
CaretVolumeExtension::clear() {
    m_comment.clear();
    m_date.clear();
    m_attributes.clear();
    m_metadata.clear();
}

void CaretVolumeExtension::readFromXmlString(const AString& s)
{
    CaretVolumeExtensionXMLReader myReader(this);
    CaretPointer<XmlSaxParser> myParser(XmlSaxParser::createXmlParser());
    try
    {
        myParser->parseString(s, &myReader);
    } catch (XmlSaxParserException& e) {
        CaretLogWarning(AString("Failed to parse caret volume extension: ") + e.whatString());
    }
}

void CaretVolumeExtension::writeAsXML(XmlWriter& xmlWriter)
{
    xmlWriter.writeStartDocument("1.0");
    xmlWriter.writeStartElement(CARET_VOL_EXT_ROOT);
    if (!m_comment.isEmpty()) xmlWriter.writeElementCData(CARET_VOL_EXT_COMMENT, m_comment);
    time_t mytime = time(NULL);//we don't have a class to deal with ISO 8601 dates, so use some C
    struct tm* timeinfo = localtime(&mytime);//note: this is a pointer to a static global in C library code, don't try to delete
    char buf[101];//we actually only need 20 bytes, but hey
    strftime(buf, 100, "%Y-%m-%dT%H:%M:%S", timeinfo);
    xmlWriter.writeElementCData(CARET_VOL_EXT_DATE, AString(buf));
    int numVols = (int)m_attributes.size();
    for (int i = 0; i < numVols; ++i)
    {
        m_attributes[i]->writeAsXML(xmlWriter, i);
    }
    if ( ! m_metadata.isEmpty()) {
        /*
         * Prior to "WB-664 Data normalization should be saved in file", VolumeFile did not contain
         * file metadata.  If one attempts to read a volume file containing metadata with older
         * versions of wb_view/wb_command or Caret5, XML parsing of the CaretVolumeExtension will
         * immediately cease if an unexpected element is encountered.  So, to minimize isses with
         * older versions of software, DO NOT write file metadata if it contains only one
         * element that is the default value (selected map) of palette normalization.
         */
        bool writeFileMetaDataFlag = true;
        if (m_metadata.getNumberOfMetaData() == 1) {
            const AString normalizationValueString = m_metadata.get(GiftiMetaDataXmlElements::METADATA_PALETTE_NORMALIZATION_MODE);
            if ( ! normalizationValueString.isEmpty()) {
                if (normalizationValueString ==
                    PaletteNormalizationModeEnum::toName(PaletteNormalizationModeEnum::NORMALIZATION_SELECTED_MAP_DATA)) {
                    writeFileMetaDataFlag = false;
                }
            }
        }
        if (writeFileMetaDataFlag) {
            m_metadata.writeAsXML(xmlWriter);
        }
    }
    xmlWriter.writeEndElement();//just to make it clean
    xmlWriter.writeEndDocument();//so, this just flushes
}

void SubvolumeAttributes::writeAsXML(XmlWriter& xmlWriter, int index)
{
    XmlAttributes myattrs;
    myattrs.addAttribute("Index", AString::number(index));
    xmlWriter.writeStartElement(CARET_VOL_EXT_VOL_INFO, myattrs);
    if (!m_comment.isEmpty()) xmlWriter.writeElementCData(CARET_VOL_EXT_VI_COMMENT, m_comment);
    if (!m_guiLabel.isEmpty()) xmlWriter.writeElementCData(CARET_VOL_EXT_VI_GUI_LABEL, m_guiLabel);
    if (m_labelTable != NULL) m_labelTable->writeAsXML(xmlWriter);//expect the extension to not have stuff it doesn't need, so just write everything it has
    m_studyMetadata.writeAsXML(xmlWriter);
    if (m_palette != NULL) m_palette->writeAsXML(xmlWriter);
    AString typeString;
    switch (m_type)
    {
        case ANATOMY:
            typeString = "Anatomy";
            break;
        case FUNCTIONAL:
            typeString = "Functional";
            break;
        case LABEL:
            typeString = "Label";
            break;
        case RGB:
            typeString = "RGB";
            break;
        case SEGMENTATION:
            typeString = "Segmentation";
            break;
        case VECTOR:
            typeString = "Vector";
            break;
        default:
            typeString = "Unknown";
    }
    xmlWriter.writeElementCData(CARET_VOL_EXT_VI_TYPE, typeString);
    if ( ! m_metadata.isEmpty()) {
        m_metadata.writeAsXML(xmlWriter);
    }
    xmlWriter.writeEndElement();
}

void StudyMetadataLinkSet::writeAsXML(XmlWriter& xmlWriter)
{//TODO: something
    xmlWriter.writeStartElement(CARET_VOL_EXT_VI_STUDY_META_SET);
    xmlWriter.writeEndElement();
}

CaretVolumeExtensionXMLReader::CaretVolumeExtensionXMLReader(CaretVolumeExtension* toFill): XmlSaxParserHandlerInterface()
{
    CaretAssert(toFill != NULL);
    m_toFill = toFill;
    m_viIndex = -1;
    m_unexpectedXmlElementSaxParser.grabNew(NULL);
}

void CaretVolumeExtensionXMLReader::characters(const char* ch)
{
    CaretAssert(m_charDataStack.size() != 0);
    CaretAssert(m_stateStack.size() != 0);
    switch (m_stateStack.back())
    {
        case LABEL_TABLE:
            CaretAssert(m_labelReader != NULL);
            m_labelReader->characters(ch);
            break;
        case PALETTE_COLOR_MAPPING:
            CaretAssert(m_paletteReader != NULL);
            m_paletteReader->characters(ch);
            break;
        case ROOT_META_DATA:
        case VI_META_DATA:
            CaretAssert(m_metadataReader);
            m_metadataReader->characters(ch);
            break;
        case UNEXPECTED_XML:
            CaretAssert(m_unexpectedXmlElementSaxParser);
            m_unexpectedXmlElementSaxParser->characters(ch);
            break;
        default:
            m_charDataStack.back() += ch;
    }
}

void CaretVolumeExtensionXMLReader::endDocument()
{
    if (m_stateStack.size() != 0)
    {
        throw XmlSaxParserException("end of document while still in an element state");
    }
}

void CaretVolumeExtensionXMLReader::endElement(const AString& namespaceURI, const AString& localName, const AString& qualifiedName)
{
    CaretAssert(m_charDataStack.size() != 0);
    CaretAssert(m_stateStack.size() != 0);
    AString elemCharData = m_charDataStack.back();
    State myState = m_stateStack.back();
    bool popState = true;
    switch (myState)
    {
        case INVALID:
            throw XmlSaxParserException("encountered end element in INVALID state");
            break;
        case CARET_EXTENSION:
            break;
        case ROOT_COMMENT:
            m_toFill->m_comment = elemCharData;
            break;
        case DATE:
            m_toFill->m_date = elemCharData;
            break;
        case VOLUME_INFORMATION:
            m_viIndex = -1;
            break;
        case VI_COMMENT:
            CaretAssertVectorIndex(m_toFill->m_attributes, m_viIndex);
            m_toFill->m_attributes[m_viIndex]->m_comment = elemCharData;
            break;
        case GUI_LABEL:
            CaretAssertVectorIndex(m_toFill->m_attributes, m_viIndex);
            m_toFill->m_attributes[m_viIndex]->m_guiLabel = elemCharData;
            break;
        case LABEL_TABLE:
            CaretAssert(m_labelReader != NULL);
            m_labelReader->endElement(namespaceURI, localName, qualifiedName);
            if (qualifiedName == GiftiXmlElements::TAG_LABEL_TABLE)
            {
                m_labelReader->endDocument();
                m_labelReader.grabNew(NULL);//make it delete now so that it can't make related bugs more confusing
            } else {
                popState = false;
            }
            break;
        case STUDY_META_DATA_LINK_SET:
            if (qualifiedName == CARET_VOL_EXT_VI_STUDY_META_SET)
            {//TODO: something
            } else {
                popState = false;
            }
            break;
        case PALETTE_COLOR_MAPPING:
            CaretAssert(m_paletteReader != NULL);
            m_paletteReader->endElement(namespaceURI, localName, qualifiedName);
            if (qualifiedName == PaletteColorMappingXmlElements::XML_TAG_PALETTE_COLOR_MAPPING)
            {
                m_paletteReader->endDocument();
                m_paletteReader.grabNew(NULL);//ditto
            } else {
                popState = false;
            }
            break;
        case ROOT_META_DATA:
        case VI_META_DATA:
            CaretAssert(m_metadataReader);
            m_metadataReader->endElement(namespaceURI, localName, qualifiedName);
            if (qualifiedName == GiftiXmlElements::TAG_METADATA) {
                m_metadataReader->endDocument();
                m_metadataReader.grabNew(NULL);
            }
            else {
                popState = false;
            }
            break;
        case VOLUME_TYPE:
            CaretAssertVectorIndex(m_toFill->m_attributes, m_viIndex);
            if (elemCharData == "Anatomy")
            {
                m_toFill->m_attributes[m_viIndex]->m_type = SubvolumeAttributes::ANATOMY;
            } else if (elemCharData == "Functional") {
                m_toFill->m_attributes[m_viIndex]->m_type = SubvolumeAttributes::FUNCTIONAL;
            } else if (elemCharData == "Label") {
                m_toFill->m_attributes[m_viIndex]->m_type = SubvolumeAttributes::LABEL;
            } else if (elemCharData == "RGB") {
                m_toFill->m_attributes[m_viIndex]->m_type = SubvolumeAttributes::RGB;
            } else if (elemCharData == "Segmentation") {
                m_toFill->m_attributes[m_viIndex]->m_type = SubvolumeAttributes::SEGMENTATION;
            } else if (elemCharData == "Vector") {
                m_toFill->m_attributes[m_viIndex]->m_type = SubvolumeAttributes::VECTOR;
            } else {
                m_toFill->m_attributes[m_viIndex]->m_type = SubvolumeAttributes::UNKNOWN;
            }
            break;
        case UNEXPECTED_XML:
            CaretAssert(m_unexpectedXmlElementSaxParser);
            switch (m_unexpectedXmlElementSaxParser->endElement(namespaceURI, localName, qualifiedName)) {
                case XmlUnexpectedElementSaxParser::ReturnCodeEnum::DONE:
                    warning(XmlSaxParserException("Unexpected XML ignored:\n"
                                                  + m_unexpectedXmlElementSaxParser->getUnexpectedContentXML()));
                    m_unexpectedXmlElementSaxParser.grabNew(NULL);
                    break;
                case XmlUnexpectedElementSaxParser::ReturnCodeEnum::ERROR:
                    throw XmlSaxParserException("Processing of unexpected elements failed: "
                                                + m_unexpectedXmlElementSaxParser->getUnexpectedContentXML());
                    break;
                case XmlUnexpectedElementSaxParser::ReturnCodeEnum::NOT_DONE:
                    popState = false;
                    break;
            }
            break;
    }
    if (popState)
    {
        m_stateStack.pop_back();
        m_charDataStack.pop_back();
    }
}

void CaretVolumeExtensionXMLReader::error(const XmlSaxParserException& exception)
{
    CaretLogWarning(AString("encountered non-fatal XML error in CaretVolumeExtension: ") + exception.whatString());
}

void CaretVolumeExtensionXMLReader::fatalError(const XmlSaxParserException& exception)
{//all of our members are self-deleting, no worries, just throw
    throw XmlSaxParserException(exception);//throw a copy of it rather than the original reference, not sure if it matters
}

void CaretVolumeExtensionXMLReader::startDocument()
{
}

void CaretVolumeExtensionXMLReader::startElement(const AString& uri, const AString& localName, const AString& qName, const XmlAttributes& atts)
{
    bool addState = true;
    State nextState = INVALID;
    AString invalidInfo = qName;
    if(m_stateStack.size() == 0)
    {
        if (qName != CARET_VOL_EXT_ROOT)
        {
            throw XmlSaxParserException(AString("CaretVolumeExtension encountered unexpected root element: ") + qName);
        }
        nextState = CARET_EXTENSION;
    } else {
        switch (m_stateStack.back())
        {
            case INVALID://should NEVER happen, INVALID throws instead of pushing, so should never be on the stack
                throw XmlSaxParserException("something has gone wrong in the CaretVolumeExtension parser");
                break;
            case ROOT_COMMENT://these should not have child elements, let INVALID catch them
            case DATE:
            case VI_COMMENT:
            case GUI_LABEL:
            case VOLUME_TYPE:
                break;
            case CARET_EXTENSION:
                if (qName == CARET_VOL_EXT_COMMENT)
                {
                    nextState = ROOT_COMMENT;
                } else if (qName == CARET_VOL_EXT_DATE) {
                    nextState = DATE;
                } else if (qName == CARET_VOL_EXT_VOL_INFO) {
                    nextState = VOLUME_INFORMATION;
                    m_viIndex = atts.getValueAsInt("Index");
                    if (m_viIndex < 0)
                    {
                        throw XmlSaxParserException("negative number encountered in VolumeInformation index");
                    }
                    if ((int)m_toFill->m_attributes.size() <= m_viIndex)
                    {
                        m_toFill->m_attributes.resize(m_viIndex + 1);//don't worry, CaretPointer copy is relatively cheap
                    }
                    m_toFill->m_attributes[m_viIndex].grabNew(new SubvolumeAttributes());
                } else if (qName == GiftiXmlElements::TAG_METADATA) {
                    nextState = ROOT_META_DATA;
                    m_metadataReader.grabNew(new GiftiMetaDataSaxReader(&m_toFill->m_metadata));
                    m_metadataReader->startDocument();
                    m_metadataReader->startElement(uri, localName, qName, atts);
                }//anything else gets caught in INVALID below
                break;
            case VOLUME_INFORMATION:
                if (qName == CARET_VOL_EXT_VI_COMMENT)
                {
                    nextState = VI_COMMENT;
                } else if (qName == CARET_VOL_EXT_VI_GUI_LABEL) {
                    nextState = GUI_LABEL;
                } else if (qName == GiftiXmlElements::TAG_LABEL_TABLE) {
                    nextState = LABEL_TABLE;
                    CaretAssertVectorIndex(m_toFill->m_attributes, m_viIndex);
                    m_toFill->m_attributes[m_viIndex]->m_labelTable.grabNew(new GiftiLabelTable());
                    m_labelReader.grabNew(new GiftiLabelTableSaxReader(m_toFill->m_attributes[m_viIndex]->m_labelTable));
                    m_labelReader->startDocument();
                    m_labelReader->startElement(uri, localName, qName, atts);
                } else if (qName == CARET_VOL_EXT_VI_STUDY_META_SET) {
                    nextState = STUDY_META_DATA_LINK_SET;//TODO: something
                } else if (qName == PaletteColorMappingXmlElements::XML_TAG_PALETTE_COLOR_MAPPING) {
                    nextState = PALETTE_COLOR_MAPPING;
                    CaretAssertVectorIndex(m_toFill->m_attributes, m_viIndex);
                    m_toFill->m_attributes[m_viIndex]->m_palette.grabNew(new PaletteColorMapping);
                    m_paletteReader.grabNew(new PaletteColorMappingSaxReader(m_toFill->m_attributes[m_viIndex]->m_palette));
                    m_paletteReader->startDocument();
                    m_paletteReader->startElement(uri, localName, qName, atts);
                } else if (qName == GiftiXmlElements::TAG_METADATA) {
                    nextState = VI_META_DATA;
                    CaretAssertVectorIndex(m_toFill->m_attributes, m_viIndex);
                    m_metadataReader.grabNew(new GiftiMetaDataSaxReader(&m_toFill->m_attributes[m_viIndex]->m_metadata));
                    m_metadataReader->startDocument();
                    m_metadataReader->startElement(uri, localName, qName, atts);
                } else if (qName == CARET_VOL_EXT_VI_TYPE) {
                    nextState = VOLUME_TYPE;
                }
                break;
            case LABEL_TABLE:
                addState = false;
                CaretAssert(m_labelReader != NULL);
                m_labelReader->startElement(uri, localName, qName, atts);
                break;
            case STUDY_META_DATA_LINK_SET:
                addState = false;//TODO: something
                break;
            case PALETTE_COLOR_MAPPING:
                addState = false;
                CaretAssert(m_paletteReader != NULL);
                m_paletteReader->startElement(uri, localName, qName, atts);
                break;
            case ROOT_META_DATA:
            case VI_META_DATA:
                addState = false;
                CaretAssert(m_metadataReader);
                m_metadataReader->startElement(uri, localName, qName, atts);
                break;
            case UNEXPECTED_XML:
                addState = false;
                CaretAssert(m_unexpectedXmlElementSaxParser);
                m_unexpectedXmlElementSaxParser->startElement(uri, localName, qName, atts);
                break;
        }
    }
    if (addState)
    {
        if (nextState == INVALID)
        {
            /*
             * If the invalid element name is not empty,
             * then we are processing child elements of the
             * invalid element.
             */
            CaretAssert(m_unexpectedXmlElementSaxParser == NULL);
            m_unexpectedXmlElementSaxParser.grabNew(new XmlUnexpectedElementSaxParser());
            m_unexpectedXmlElementSaxParser->startElement(uri, localName, qName, atts);
            nextState = UNEXPECTED_XML;
        }
        m_stateStack.push_back(nextState);
        m_charDataStack.push_back(AString());
    }
}

void CaretVolumeExtensionXMLReader::warning(const caret::XmlSaxParserException& exception)
{
    CaretLogWarning(AString("encountered XML warning in CaretVolumeExtension: ") + exception.whatString());
}
