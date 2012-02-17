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
#include <ctime>

using namespace caret;
using namespace std;

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
    if (!m_comment.isEmpty()) xmlWriter.writeElementCharacters(CARET_VOL_EXT_COMMENT, m_comment);
    time_t mytime = time(NULL);//we don't have a class to deal with ISO 8601 dates, so use some C
    struct tm* timeinfo = localtime(mytime);//note: this is a pointer to a static global in C library code, don't try to delete
    char buf[101];//we actually only need 20 bytes, but hey
    strftime(buf, 100, "%Y-%m-%dT%H:%M:%S", timeinfo);
    xmlWriter.writeElementCharacters(CARET_VOL_EXT_DATE, AString(buf));
    int numVols = (int)m_attributes.size();
    for (int i = 0; i < numVols; ++i)
    {
        m_attributes[i].writeAsXML(xmlWriter, i);
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
}
