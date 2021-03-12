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

#include "CiftiScalarsMap.h"

#include "CaretAssert.h"
#include "DataFileException.h"
#include "CaretLogger.h"
//HACK: to compare metadata in a const function, we make a copy and remove the palette data - but metadata's copy intentionally breaks == because of the UUID, so we need to reset it
#include "GiftiMetaDataXmlElements.h"

using namespace caret;

void CiftiScalarsMap::clear()
{
    m_maps.clear();
}

GiftiMetaData* CiftiScalarsMap::getMapMetadata(const int64_t& index) const
{
    CaretAssertVectorIndex(m_maps, index);
    return &(m_maps[index].m_metaData);
}

const QString& CiftiScalarsMap::getMapName(const int64_t& index) const
{
    CaretAssertVectorIndex(m_maps, index);
    return m_maps[index].m_name;
}

int64_t CiftiScalarsMap::getIndexFromNumberOrName(const QString& numberOrName) const
{
    bool ok = false;
    int64_t ret = numberOrName.toLongLong(&ok) - 1;//quirk: use string "1" as the first index
    if (ok)
    {
        if (ret < 0 || ret >= getLength()) return -1;//if it is a number, do not try to use it as a name, under any circumstances
        return ret;
    } else {
        int64_t length = getLength();
        for (int64_t i = 0; i < length; ++i)
        {
            if (numberOrName == getMapName(i)) return i;
        }
        return -1;
    }
}

PaletteColorMapping* CiftiScalarsMap::getMapPalette(const int64_t& index) const
{
    CaretAssertVectorIndex(m_maps, index);
    return m_maps[index].getPalette();
}

PaletteColorMapping* CiftiScalarsMap::ScalarMap::getPalette() const
{
    if (m_palette != NULL)
    {
        return m_palette;
    }
    m_palette.grabNew(new PaletteColorMapping());
    if (m_metaData.exists("PaletteColorMapping"))
    {
        try
        {
            m_palette->decodeFromStringXML(m_metaData.get("PaletteColorMapping"));
        } catch (XmlException& e) {
            CaretLogWarning("failed to parse palette settings from metadata: " + e.whatString());
        }
    }
    return m_palette;
}

void CiftiScalarsMap::setLength(const int64_t& length)
{
    CaretAssert(length > 0);
    m_maps.resize(length);
}

void CiftiScalarsMap::setMapName(const int64_t& index, const QString& mapName) const
{
    CaretAssertVectorIndex(m_maps, index);
    m_maps[index].m_name = mapName;
}

bool CiftiScalarsMap::approximateMatch(const CiftiMappingType& rhs, QString* explanation) const
{
    switch (rhs.getType())
    {
        case SCALARS:
        case SERIES://maybe?
        case LABELS:
            if (getLength() != rhs.getLength())
            {
                if (explanation != NULL) *explanation = "mappings have different length";
                return false;
            } else return true;
        default:
            if (explanation != NULL) *explanation = CiftiMappingType::mappingTypeToName(rhs.getType()) + " mapping never matches " + CiftiMappingType::mappingTypeToName(getType());
            return false;
    }
}

bool CiftiScalarsMap::operator==(const CiftiMappingType& rhs) const
{
    if (rhs.getType() != getType()) return false;
    const CiftiScalarsMap& myrhs = dynamic_cast<const CiftiScalarsMap&>(rhs);
    return (m_maps == myrhs.m_maps);
}

bool CiftiScalarsMap::ScalarMap::operator==(const CiftiScalarsMap::ScalarMap& rhs) const
{
    if (m_name != rhs.m_name) return false;
    if (*(getPalette()) != *(rhs.getPalette())) return false;
    GiftiMetaData mytemp = m_metaData, rhstemp = rhs.m_metaData;
    mytemp.remove("PaletteColorMapping");//we already compared the true palettes, so don't compare the metadata that may or may not encode them
    if (m_metaData.exists(GiftiMetaDataXmlElements::METADATA_NAME_UNIQUE_ID))//HACK: fix the copy-breaks-UUID silliness
    {
        mytemp.set(GiftiMetaDataXmlElements::METADATA_NAME_UNIQUE_ID, m_metaData.get(GiftiMetaDataXmlElements::METADATA_NAME_UNIQUE_ID));
    }
    rhstemp.remove("PaletteColorMapping");
    if (rhs.m_metaData.exists(GiftiMetaDataXmlElements::METADATA_NAME_UNIQUE_ID))
    {
        rhstemp.set(GiftiMetaDataXmlElements::METADATA_NAME_UNIQUE_ID, rhs.m_metaData.get(GiftiMetaDataXmlElements::METADATA_NAME_UNIQUE_ID));
    }
    return (mytemp == rhstemp);
}

CiftiScalarsMap::ScalarMap::ScalarMap(const CiftiScalarsMap::ScalarMap& rhs)
{
    m_name = rhs.m_name;
    m_metaData = rhs.m_metaData;
    if (rhs.m_palette != NULL) m_palette.grabNew(new PaletteColorMapping(*(rhs.m_palette)));//we need to copy the palette, because it may no longer match the metadata
}

CiftiScalarsMap::ScalarMap& CiftiScalarsMap::ScalarMap::operator=(const CiftiScalarsMap::ScalarMap& rhs)
{
    if (&rhs == this) return *this;
    m_name = rhs.m_name;
    m_metaData = rhs.m_metaData;
    if (rhs.m_palette == NULL)
    {
        m_palette.grabNew(NULL);
    } else {
        m_palette.grabNew(new PaletteColorMapping(*(rhs.m_palette)));//we need to copy the palette object, because it may no longer match the metadata
    }
    return *this;
}

void CiftiScalarsMap::readXML1(QXmlStreamReader& xml)
{
    CaretLogFiner("parsing nonstandard scalars mapping type in cifti-1");
    clear();
    for (xml.readNext(); !xml.atEnd() && !xml.isEndElement(); xml.readNext())
    {
        switch (xml.tokenType())
        {
            case QXmlStreamReader::StartElement:
            {
                if (xml.name() != QLatin1String("NamedMap"))
                {
                    throw DataFileException("unexpected element in scalars map: " + xml.name().toString());
                }
                m_maps.push_back(ScalarMap());//HACK: because operator= is deliberately broken by GiftiMetadata for UUID
                m_maps.back().readXML1(xml);
                if (xml.hasError()) return;
                break;
            }
            default:
                break;
        }
    }
    CaretAssert(xml.isEndElement() && xml.name() == QLatin1String("MatrixIndicesMap"));
}

void CiftiScalarsMap::readXML2(QXmlStreamReader& xml)
{
    clear();
    for (xml.readNext(); !xml.atEnd() && !xml.isEndElement(); xml.readNext())
    {
        switch (xml.tokenType())
        {
            case QXmlStreamReader::StartElement:
            {
                if (xml.name() != QLatin1String("NamedMap"))
                {
                    throw DataFileException("unexpected element in scalars map: " + xml.name().toString());
                }
                m_maps.push_back(ScalarMap());//HACK: because operator= is deliberately broken by GiftiMetadata for UUID
                m_maps.back().readXML2(xml);
                if (xml.hasError()) return;
                break;
            }
            default:
                break;
        }
    }
    CaretAssert(xml.isEndElement() && xml.name() == QLatin1String("MatrixIndicesMap"));
}

void CiftiScalarsMap::ScalarMap::readXML1(QXmlStreamReader& xml)
{
    bool haveName = false, haveMetaData = false;
    for (xml.readNext(); !xml.atEnd() && !xml.isEndElement(); xml.readNext())
    {
        switch (xml.tokenType())
        {
            case QXmlStreamReader::StartElement:
            {
                auto name = xml.name();
                if (name == QLatin1String("MetaData"))
                {
                    if (haveMetaData)
                    {
                        throw DataFileException("MetaData specified multiple times in one NamedMap");
                    }
                    m_metaData.readCiftiXML1(xml);
                    if (xml.hasError()) return;
                    haveMetaData = true;
                } else if (name == QLatin1String("MapName")) {
                    if (haveName)
                    {
                        throw DataFileException("MapName specified multiple times in one NamedMap");
                    }
                    m_name = xml.readElementText();//raises error if element encountered
                    if (xml.hasError()) return;
                    haveName = true;
                } else if (name == QLatin1String("LabelTable")) {
                    CaretLogWarning("ignoring LabelTable in Cifti-1 Scalars mapping");
                    xml.readElementText(QXmlStreamReader::SkipChildElements);//accept some malformed Cifti-1 files
                    if (xml.hasError()) return;
                } else {
                    throw DataFileException("unexpected element in NamedMap: " + name.toString());
                }
                break;
            }
            default:
                break;
        }
    }
    if (!haveName)
    {
        throw DataFileException("NamedMap missing required child element MapName");
    }
    CaretAssert(xml.isEndElement() && xml.name() == QLatin1String("NamedMap"));
}

void CiftiScalarsMap::ScalarMap::readXML2(QXmlStreamReader& xml)
{
    bool haveName = false, haveMetaData = false;
    for (xml.readNext(); !xml.atEnd() && !xml.isEndElement(); xml.readNext())
    {
        switch (xml.tokenType())
        {
            case QXmlStreamReader::StartElement:
            {
                auto name = xml.name();
                if (name == QLatin1String("MetaData"))
                {
                    if (haveMetaData)
                    {
                        throw DataFileException("MetaData specified multiple times in one NamedMap");
                    }
                    m_metaData.readCiftiXML2(xml);
                    if (xml.hasError()) return;
                    haveMetaData = true;
                } else if (name == QLatin1String("MapName")) {
                    if (haveName)
                    {
                        throw DataFileException("MapName specified multiple times in one NamedMap");
                    }
                    m_name = xml.readElementText();//raises error if element encountered
                    if (xml.hasError()) return;
                    haveName = true;
                } else {
                    throw DataFileException("unexpected element in NamedMap: " + name.toString());
                }
                break;
            }
            default:
                break;
        }
    }
    if (!haveName)
    {
        throw DataFileException("NamedMap missing required child element MapName");
    }
    CaretAssert(xml.isEndElement() && xml.name() == QLatin1String("NamedMap"));
}

void CiftiScalarsMap::writeXML1(QXmlStreamWriter& xml) const
{
    CaretLogFiner("writing nonstandard scalars mapping type in cifti-1");
    xml.writeAttribute("IndicesMapToDataType", "CIFTI_INDEX_TYPE_SCALARS");
    int64_t numMaps = (int64_t)m_maps.size();
    for (int64_t i = 0; i < numMaps; ++i)
    {
        xml.writeStartElement("NamedMap");
        xml.writeTextElement("MapName", m_maps[i].m_name);
        if (m_maps[i].m_palette != NULL)
        {
            m_maps[i].m_metaData.set("PaletteColorMapping", m_maps[i].m_palette->encodeInXML());
        }
        m_maps[i].m_metaData.writeCiftiXML1(xml);
        xml.writeEndElement();
    }
}

void CiftiScalarsMap::writeXML2(QXmlStreamWriter& xml) const
{
    int64_t numMaps = (int64_t)m_maps.size();
    xml.writeAttribute("IndicesMapToDataType", "CIFTI_INDEX_TYPE_SCALARS");
    for (int64_t i = 0; i < numMaps; ++i)
    {
        xml.writeStartElement("NamedMap");
        xml.writeTextElement("MapName", m_maps[i].m_name);
        if (m_maps[i].m_palette != NULL)
        {
            m_maps[i].m_metaData.set("PaletteColorMapping", m_maps[i].m_palette->encodeInXML());
        }
        m_maps[i].m_metaData.writeCiftiXML1(xml);
        xml.writeEndElement();
    }
}

//support for internal objects that track modified status
CiftiScalarsMap::CiftiScalarsMap()
{
    m_namesModified = false;
}

CiftiScalarsMap::CiftiScalarsMap(const CiftiScalarsMap& rhs)
{
    m_maps = rhs.m_maps;
    clearMutablesModified();//this newly created map is not modified
}

CiftiScalarsMap& CiftiScalarsMap::operator=(const CiftiScalarsMap& rhs)
{
    m_maps = rhs.m_maps;
    clearMutablesModified();//this map isn't const, so don't count it as modified
    return *this;
}

CiftiScalarsMap::CiftiScalarsMap(const int64_t& length)
{
    m_namesModified = false;
    setLength(length);
}

bool CiftiScalarsMap::mutablesModified() const
{
    if (m_namesModified) return true;
    for (int64_t i = 0; i < getLength(); ++i)
    {
        if (m_maps[i].m_palette != NULL && m_maps[i].m_palette->isModified()) return true;
        if (getMapMetadata(i)->isModified()) return true;
    }
    return false;
}

void CiftiScalarsMap::clearMutablesModified() const
{
    m_namesModified = false;
    for (int64_t i = 0; i < getLength(); ++i)
    {
        if (m_maps[i].m_palette != NULL) m_maps[i].m_palette->clearModified();
        getMapMetadata(i)->clearModified();
    }
}
