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

#include "CaretVolumeExtension.h"
#include "XmlWriter.h"
#include "XmlSaxParser.h"
#include "GiftiXmlElements.h"
#include "PaletteColorMappingXmlElements.h"
#include "CaretLogger.h"
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

void CaretVolumeExtension::readFromXmlString(const AString& s)
{
    CaretVolumeExtensionXMLReader myReader(this);
    XmlSaxParser* myParser = XmlSaxParser::createXmlParser();
    myParser->parseString(s, &myReader);
    delete myParser;
}

void CaretVolumeExtension::writeAsXML(XmlWriter& xmlWriter)
{
    xmlWriter.writeStartDocument("1.0");
    xmlWriter.writeStartElement(CARET_VOL_EXT_ROOT);
    if (!m_comment.isEmpty()) xmlWriter.writeElementCData(CARET_VOL_EXT_COMMENT, m_comment);
    time_t mytime = time(NULL);//we don't have a class to deal with ISO 8601 dates, so use some C
    struct tm* timeinfo = localtime(mytime);//note: this is a pointer to a static global in C library code, don't try to delete
    char buf[101];//we actually only need 20 bytes, but hey
    strftime(buf, 100, "%Y-%m-%dT%H:%M:%S", timeinfo);
    xmlWriter.writeElementCData(CARET_VOL_EXT_DATE, AString(buf));
    int numVols = (int)m_attributes.size();
    for (int i = 0; i < numVols; ++i)
    {
        m_attributes[i]->writeAsXML(xmlWriter, i);
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
    if (m_type == LABEL && m_labelTable != NULL) m_labelTable->writeAsXML(xmlWriter);
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
}

void CaretVolumeExtensionXMLReader::characters(const char* ch) throw (XmlSaxParserException)
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
        default:
            m_charDataStack.back() += ch;
    }
}

void CaretVolumeExtensionXMLReader::endDocument() throw (XmlSaxParserException)
{
    if (m_stateStack.size() != 0)
    {
        throw XmlSaxParserException("end of document while still in an element state");
    }
}

void CaretVolumeExtensionXMLReader::endElement(const AString& namespaceURI, const AString& localName, const AString& qualifiedName) throw (XmlSaxParserException)
{
    CaretAssert(m_charDataStack.size() != 0);
    CaretAssert(m_stateStack.size() != 0);
    AString elemCharData = m_charDataStack.back();
    State myState = m_stateStack.back();
    bool popState = true;
    switch (myState)
    {
        case INVALID:
            throw XmlSaxParserException("encountered end element in INIT state");
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
        case STUDY_META_DATA_LINK_SET://TODO: something
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
    }
    if (popState)
    {
        m_stateStack.pop_back();
        m_charDataStack.pop_back();
    }
}

void CaretVolumeExtensionXMLReader::error(const XmlSaxParserException& exception) throw (XmlSaxParserException)
{
    CaretLogWarning(AString("encountered non-fatal XML error in CaretVolumeExtension: ") + exception.whatString());
}

void CaretVolumeExtensionXMLReader::fatalError(const XmlSaxParserException& exception) throw (XmlSaxParserException)
{//all of our members are self-deleting, no worries, just throw
    throw XmlSaxParserException(exception.whatString());
}

void CaretVolumeExtensionXMLReader::startDocument()
{
}

void CaretVolumeExtensionXMLReader::startElement(const AString& uri, const AString& localName, const AString& qName, const XmlAttributes& atts) throw (XmlSaxParserException)
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
                    if (m_toFill->m_attributes.size() <= m_viIndex)
                    {
                        m_toFill->m_attributes.resize(m_viIndex + 1);//don't worry, CaretPointer copy is relatively cheap
                    }
                    m_toFill->m_attributes[m_viIndex].grabNew(new SubvolumeAttributes());
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
                    nextState = STUDY_META_DATA_LINK_SET;
                } else if (qName == PaletteColorMappingXmlElements::XML_TAG_PALETTE_COLOR_MAPPING) {
                    nextState = PALETTE_COLOR_MAPPING;
                } else if (qName == CARET_VOL_EXT_VI_TYPE) {
                    nextState = VOLUME_TYPE;
                }
                break;
        }
    }
    if (addState)
    {
        if (nextState == INVALID)
        {
            throw XmlSaxParserException(AString("CaretVolumeExtension encountered an unexpected element: ") + invalidInfo);
        }
        m_stateStack.push_back(nextState);
        m_charDataStack.push_back(AString());
    }
}
