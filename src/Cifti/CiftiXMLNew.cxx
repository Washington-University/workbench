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

#include "CiftiXMLNew.h"

#include "CaretAssert.h"
#include "CaretException.h"
#include "CaretLogger.h"
#include "GiftiMetaData.h"
#include "PaletteColorMapping.h"

#include <QStringList>

#include <set>

using namespace std;
using namespace caret;

CiftiXMLNew::CiftiXMLNew(const CiftiXMLNew& rhs)
{
    copyHelper(rhs);
}

CiftiXMLNew& CiftiXMLNew::operator=(const CiftiXMLNew& rhs)
{
    if (this != &rhs) copyHelper(rhs);
    return *this;
}

void CiftiXMLNew::copyHelper(const CiftiXMLNew& rhs)
{
    int numDims = (int)rhs.m_indexMaps.size();
    m_indexMaps.resize(numDims);
    for (int i = 0; i < numDims; ++i)
    {
        m_indexMaps[i] = CaretPointer<CiftiIndexMap>(rhs.m_indexMaps[i]->clone());
    }
}

const CiftiIndexMap* CiftiXMLNew::getMap(const int& direction) const
{
    CaretAssertVectorIndex(m_indexMaps, direction);
    return m_indexMaps[direction];
}

GiftiMetaData* CiftiXMLNew::getFileMetadata() const
{
    return &m_fileMetaData;
}

PaletteColorMapping* CiftiXMLNew::getFilePalette() const
{
    if (m_filePalette != NULL)
    {
        return m_filePalette;
    }
    m_filePalette.grabNew(new PaletteColorMapping());
    if (m_fileMetaData.exists("PaletteColorMapping"))
    {
        try
        {
            m_filePalette->decodeFromStringXML(m_fileMetaData.get("PaletteColorMapping"));
        } catch (XmlException& e) {
            CaretLogWarning("failed to parse palette settings from metadata: " + e.whatString());
        }
    }
    return m_filePalette;
}

const CiftiBrainModelsMap& CiftiXMLNew::getBrainModelsMap(const int& direction) const
{
    CaretAssertVectorIndex(m_indexMaps, direction);
    CaretAssert(getMappingType(direction) == CiftiIndexMap::BRAIN_MODELS);//assert so we catch it in debug
    return dynamic_cast<const CiftiBrainModelsMap&>(*getMap(direction));//let release throw bad_cast or segfault
}

const CiftiLabelsMap& CiftiXMLNew::getLabelsMap(const int& direction) const
{
    CaretAssertVectorIndex(m_indexMaps, direction);
    CaretAssert(getMappingType(direction) == CiftiIndexMap::LABELS);//assert so we catch it in debug
    return dynamic_cast<const CiftiLabelsMap&>(*getMap(direction));//let release throw bad_cast or segfault
}

const CiftiParcelsMap& CiftiXMLNew::getParcelsMap(const int& direction) const
{
    CaretAssertVectorIndex(m_indexMaps, direction);
    CaretAssert(getMappingType(direction) == CiftiIndexMap::PARCELS);//assert so we catch it in debug
    return dynamic_cast<const CiftiParcelsMap&>(*getMap(direction));//let release throw bad_cast or segfault
}

const CiftiScalarsMap& CiftiXMLNew::getScalarsMap(const int& direction) const
{
    CaretAssertVectorIndex(m_indexMaps, direction);
    CaretAssert(getMappingType(direction) == CiftiIndexMap::SCALARS);//assert so we catch it in debug
    return dynamic_cast<const CiftiScalarsMap&>(*getMap(direction));//let release throw bad_cast or segfault
}

const CiftiSeriesMap& CiftiXMLNew::getSeriesMap(const int& direction) const
{
    CaretAssertVectorIndex(m_indexMaps, direction);
    CaretAssert(getMappingType(direction) == CiftiIndexMap::SERIES);//assert so we catch it in debug
    return dynamic_cast<const CiftiSeriesMap&>(*getMap(direction));//let release throw bad_cast or segfault
}

int64_t CiftiXMLNew::getDimensionLength(const int& direction) const
{
    const CiftiIndexMap* tempMap = getMap(direction);
    CaretAssert(tempMap != NULL);
    return tempMap->getLength();
}

CiftiIndexMap::MappingType CiftiXMLNew::getMappingType(const int& direction) const
{
    CaretAssertVectorIndex(m_indexMaps, direction);
    CaretAssert(m_indexMaps[direction] != NULL);
    return m_indexMaps[direction]->getType();
}

void CiftiXMLNew::setMap(const int& direction, const CiftiIndexMap& mapIn)
{
    CaretAssertVectorIndex(m_indexMaps, direction);
    m_indexMaps[direction] = CaretPointer<CiftiIndexMap>(mapIn.clone());
}

void CiftiXMLNew::setNumberOfDimensions(const int& num)
{
    m_indexMaps.resize(num);
}

void CiftiXMLNew::readXML(const QString& text)
{
    QXmlStreamReader xml(text);
    readXML(xml);
}

void CiftiXMLNew::readXML(const QByteArray& data)
{
    QString text(data);//constructing a qstring appears to be the simplest way to remove trailing nulls, which otherwise trip an "Extra content at end of document" error
    readXML(text);//then put it through the string reader, just to simplify code paths
}

void CiftiXMLNew::readXML(QXmlStreamReader& xml)
{
    try
    {
        bool haveCifti = false;
        for (; !xml.atEnd(); xml.readNext())
        {
            if (xml.isStartElement())
            {
                QStringRef name = xml.name();
                if (name == "CIFTI")
                {
                    if (haveCifti)
                    {
                        throw CaretException("CIFTI element may only be specified once");
                    }
                    QXmlStreamAttributes attributes = xml.attributes();
                    if(attributes.hasAttribute("Version"))
                    {
                        m_parsedVersion = CiftiVersion(attributes.value("Version").toString());
                    } else {
                        throw CaretException("Cifti XML missing Version attribute.");
                    }
                    if (m_parsedVersion == CiftiVersion(1, 0))//switch/case on major/minor would be much harder to read
                    {
                        parseCIFTI1(xml);
                        if (xml.hasError()) break;
                    } else if (m_parsedVersion == CiftiVersion(1, 1)) {
                        //CaretLogWarning("parsing cifti version '1.1', this should not exist in the wild");
                        parseCIFTI2(xml);//we will use "1.1" to test our cifti-2 implementation
                        if (xml.hasError()) break;
                    /*} else if (m_parsedVersion == CiftiVersion(2, 0)) {
                        parseCIFTI2(xml);
                        if (xml.hasError()) break;//*/ //leave commented out until cifti-2 is finalized
                    } else {
                        throw CaretException("unknown Cifti Version: '" + m_parsedVersion.toString());
                    }
                    haveCifti = true;
                } else {
                    throw CaretException("unknown element in Cifti XML: " + name.toString());
                }
            }
        }
        if (!xml.hasError() && !haveCifti)
        {
            throw CaretException("CIFTI element not found");
        }
    } catch (CaretException& e) {
        throw CaretException("Cifti XML error: " + e.whatString());//so we can throw on error instead of doing a bunch of dancing with xml.raiseError and xml.hasError
    }
    if(xml.hasError())
    {
        throw CaretException("Cifti XML error: " + xml.errorString());
    }
}

void CiftiXMLNew::parseCIFTI1(QXmlStreamReader& xml)
{
    QXmlStreamAttributes attributes = xml.attributes();
    if (attributes.hasAttribute("NumberOfMatrices"))
    {
        if (attributes.value("NumberOfMatrices") != "1")
        {
            throw CaretException("attribute NumberOfMatrices in CIFTI is required to be 1 for CIFTI-1");
        }
    } else {
        throw CaretException("missing attribute NumberOfMatrices in CIFTI");
    }
    bool haveMatrix = false;
    while (!xml.atEnd())
    {
        xml.readNext();
        if (xml.isStartElement())
        {
            QStringRef name = xml.name();
            if (name == "Matrix")
            {
                if (haveMatrix)
                {
                    throw CaretException("Matrix element may only be specified once");
                }
                parseMatrix1(xml);
                if (xml.hasError()) return;
                haveMatrix = true;
            } else {
                throw CaretException("unknown element in CIFTI: " + name.toString());
            }
        } else if (xml.isEndElement()) {
            break;
        }
    }
    if (!haveMatrix)
    {
        throw CaretException("Matrix element not found in CIFTI");
    }
    if (xml.hasError()) return;
    CaretAssert(xml.isEndElement() && xml.name() == "CIFTI");
}

void CiftiXMLNew::parseCIFTI2(QXmlStreamReader& xml)//yes, these will often have largely similar code, but it seems cleaner than having only some functions split, or constantly rechecking the version
{//also, helps keep changes to cifti-2 away from code that parses cifti-1
    bool haveMatrix = false;
    while (!xml.atEnd())
    {
        xml.readNext();
        if (xml.hasError()) return;
        if (xml.isStartElement())
        {
            QStringRef name = xml.name();
            if (name == "Matrix")
            {
                if (haveMatrix)
                {
                    throw CaretException("Matrix element may only be specified once");
                }
                parseMatrix2(xml);
                if (xml.hasError()) return;
                haveMatrix = true;
            } else {
                throw CaretException("unknown element in CIFTI: " + name.toString());
            }
        } else if (xml.isEndElement()) {
            break;
        }
    }
    if (!haveMatrix)
    {
        throw CaretException("Matrix element not found in CIFTI");
    }
    CaretAssert(xml.isEndElement() && xml.name() == "CIFTI");
}

void CiftiXMLNew::parseMatrix1(QXmlStreamReader& xml)
{
    VolumeSpace fileVolSpace;
    bool haveVolSpace = false, haveMetadata = false;
    while (!xml.atEnd())
    {
        xml.readNext();
        if (xml.hasError()) return;
        if (xml.isStartElement())
        {
            QStringRef name = xml.name();
            if (name == "MetaData")
            {
                if (haveMetadata)
                {
                    throw CaretException("MetaData may only be specified once in Matrix");
                }
                m_fileMetaData.readCiftiXML1(xml);
                if (xml.hasError()) return;
                haveMetadata = true;
            } else if (name == "MatrixIndicesMap") {
                parseMatrixIndicesMap1(xml);
                if (xml.hasError()) return;
            } else if (name == "Volume") {
                if (haveVolSpace)
                {
                    throw CaretException("Volume may only be specified once in Matrix");
                }
                fileVolSpace.readCiftiXML1(xml);
                if (xml.hasError()) return;
                haveVolSpace = true;
            } else if (name == "LabelTable") {
                CaretLogFiner("skipping unused LabelTable element in Matrix in CIFTI-1");
                xml.readElementText(QXmlStreamReader::SkipChildElements);
            } else {
                throw CaretException("unknown element in Matrix: " + name.toString());
            }
        } else if (xml.isEndElement()) {
            break;
        }
    }
    for (int i = 0; i < (int)m_indexMaps.size(); ++i)
    {
        if (m_indexMaps[i] == NULL)
        {
            int displaynum = i;
            if (displaynum < 2) displaynum = 1 - displaynum;//re-invert so that it shows the same number as the XML is missing
            throw CaretException("missing mapping for dimension '" + QString::number(displaynum) + "'");
        }
        switch (m_indexMaps[i]->getType())
        {
            case CiftiIndexMap::BRAIN_MODELS:
            {
                CiftiBrainModelsMap& myMap = dynamic_cast<CiftiBrainModelsMap&>(*(m_indexMaps[i]));
                if (myMap.hasVolumeData())
                {
                    if (haveVolSpace)
                    {
                        myMap.setVolumeSpace(fileVolSpace);//also does the needed checking of voxel indices
                    } else {
                        throw CaretException("BrainModels map uses voxels, but no Volume element exists");
                    }
                }
                break;
            }
            case CiftiIndexMap::PARCELS:
            {
                CiftiParcelsMap& myMap = dynamic_cast<CiftiParcelsMap&>(*(m_indexMaps[i]));
                if (myMap.hasVolumeData())
                {
                    if (haveVolSpace)
                    {
                        myMap.setVolumeSpace(fileVolSpace);//ditto
                    } else {
                        throw CaretException("Parcels map uses voxels, but no Volume element exists");
                    }
                }
                break;
            }
            default:
                break;
        }
    }
    CaretAssert(xml.isEndElement() && xml.name() == "Matrix");
}

void CiftiXMLNew::parseMatrix2(QXmlStreamReader& xml)
{
    bool haveMetadata = false;
    while (!xml.atEnd())
    {
        xml.readNext();
        if (xml.hasError()) return;
        if (xml.isStartElement())
        {
            QStringRef name = xml.name();
            if (name == "MetaData")
            {
                if (haveMetadata)
                {
                    throw CaretException("MetaData may only be specified once in Matrix");
                }
                m_fileMetaData.readCiftiXML2(xml);
                if (xml.hasError()) return;
                haveMetadata = true;
            } else if (name == "MatrixIndicesMap") {
                parseMatrixIndicesMap2(xml);
                if (xml.hasError()) return;
            } else {
                throw CaretException("unknown element in Matrix: " + name.toString());
            }
        } else if (xml.isEndElement()) {
            break;
        }
    }
    for (int i = 0; i < (int)m_indexMaps.size(); ++i)
    {
        if (m_indexMaps[i] == NULL)
        {
            throw CaretException("missing mapping for dimension '" + QString::number(i) + "'");
        }
    }
    CaretAssert(xml.isEndElement() && xml.name() == "Matrix");
}

void CiftiXMLNew::parseMatrixIndicesMap1(QXmlStreamReader& xml)
{
    QXmlStreamAttributes attributes = xml.attributes();
    if (!attributes.hasAttribute("AppliesToMatrixDimension"))
    {
        throw CaretException("missing attribute AppliesToMatrixDimension in MatrixIndicesMap");
    }
    if (!attributes.hasAttribute("IndicesMapToDataType"))
    {
        throw CaretException("missing attribute IndicesMapToDataType in MatrixIndicesMap");
    }
    QStringList values = attributes.value("AppliesToMatrixDimension").toString().split(',');
    bool ok = false;
    set<int> used;
    for(int i = 0; i < values.size(); i++)
    {
        int parsed = values[i].toInt(&ok);
        if (!ok || parsed < 0)
        {
            throw CaretException("bad value in AppliesToMatrixDimension list: " + values[i]);
        }
        if (parsed < 2) parsed = 1 - parsed;//in other words, 0 becomes 1 and 1 becomes 0, since cifti-1 had them reversed
        if (used.find(parsed) != used.end())
        {
            throw CaretException("AppliesToMatrixDimension contains repeated value: " + values[i]);
        }
        used.insert(parsed);
    }
    CaretPointer<CiftiIndexMap> toRead;
    QStringRef type = attributes.value("IndicesMapToDataType");
    if (type == "CIFTI_INDEX_TYPE_BRAIN_MODELS")
    {
        toRead = CaretPointer<CiftiBrainModelsMap>(new CiftiBrainModelsMap());
    } else if (type == "CIFTI_INDEX_TYPE_TIME_POINTS") {
        toRead = CaretPointer<CiftiSeriesMap>(new CiftiSeriesMap());
    } else if (type == "CIFTI_INDEX_TYPE_LABELS") {//this and below are nonstandard
        toRead = CaretPointer<CiftiLabelsMap>(new CiftiLabelsMap());
    } else if (type == "CIFTI_INDEX_TYPE_PARCELS") {
        toRead = CaretPointer<CiftiParcelsMap>(new CiftiParcelsMap());
    } else if (type == "CIFTI_INDEX_TYPE_SCALARS") {
        toRead = CaretPointer<CiftiScalarsMap>(new CiftiScalarsMap());
    } else {
        throw CaretException("invalid value for IndicesMapToDataType in CIFTI-1: " + type.toString());
    }
    toRead->readXML1(xml);//this will warn (with 'finer' log level?) if it is nonstandard
    if (xml.hasError()) return;
    bool first = true;
    for (set<int>::iterator iter = used.begin(); iter != used.end(); ++iter)
    {
        if (*iter >= (int)m_indexMaps.size()) m_indexMaps.resize(*iter + 1);
        if (first)
        {
            m_indexMaps[*iter] = toRead;
            first = false;
        } else {
            m_indexMaps[*iter] = CaretPointer<CiftiIndexMap>(toRead->clone());//make in-memory information independent per-dimension, rather than dealing with deduplication everywhere
        }
    }
    CaretAssert(xml.isEndElement() && xml.name() == "MatrixIndicesMap");
}

void CiftiXMLNew::parseMatrixIndicesMap2(QXmlStreamReader& xml)
{
    QXmlStreamAttributes attributes = xml.attributes();
    if (!attributes.hasAttribute("AppliesToMatrixDimension"))
    {
        throw CaretException("missing attribute AppliesToMatrixDimension in MatrixIndicesMap");
    }
    if (!attributes.hasAttribute("IndicesMapToDataType"))
    {
        throw CaretException("missing attribute IndicesMapToDataType in MatrixIndicesMap");
    }
    QStringList values = attributes.value("AppliesToMatrixDimension").toString().split(',');
    bool ok = false;
    set<int> used;
    for(int i = 0; i < values.size(); i++)
    {
        int parsed = values[i].toInt(&ok);
        if (!ok || parsed < 0)
        {
            throw CaretException("bad value in AppliesToMatrixDimension list: " + values[i]);
        }
        if (used.find(parsed) != used.end())
        {
            throw CaretException("AppliesToMatrixDimension contains repeated value: " + values[i]);
        }
        used.insert(parsed);
    }
    CaretPointer<CiftiIndexMap> toRead;
    QStringRef type = attributes.value("IndicesMapToDataType");
    if (type == "CIFTI_INDEX_TYPE_BRAIN_MODELS")
    {
        toRead = CaretPointer<CiftiBrainModelsMap>(new CiftiBrainModelsMap());
    } else if (type == "CIFTI_INDEX_TYPE_LABELS") {
        toRead = CaretPointer<CiftiLabelsMap>(new CiftiLabelsMap());
    } else if (type == "CIFTI_INDEX_TYPE_PARCELS") {
        toRead = CaretPointer<CiftiParcelsMap>(new CiftiParcelsMap());
    } else if (type == "CIFTI_INDEX_TYPE_SCALARS") {
        toRead = CaretPointer<CiftiScalarsMap>(new CiftiScalarsMap());
    } else if (type == "CIFTI_INDEX_TYPE_SERIES") {
        toRead = CaretPointer<CiftiSeriesMap>(new CiftiSeriesMap());
    } else {
        throw CaretException("invalid value for IndicesMapToDataType in CIFTI-1: " + type.toString());
    }
    toRead->readXML2(xml);
    if (xml.hasError()) return;
    bool first = true;
    for (set<int>::iterator iter = used.begin(); iter != used.end(); ++iter)
    {
        if (*iter >= (int)m_indexMaps.size()) m_indexMaps.resize(*iter + 1);
        if (first)
        {
            m_indexMaps[*iter] = toRead;
            first = false;
        } else {
            m_indexMaps[*iter] = CaretPointer<CiftiIndexMap>(toRead->clone());//make in-memory information independent per-dimension, rather than dealing with deduplication everywhere
        }
    }
    CaretAssert(xml.isEndElement() && xml.name() == "MatrixIndicesMap");
}

QByteArray CiftiXMLNew::writeXMLToQByteArray(const CiftiVersion& writingVersion) const
{
    QByteArray ret;
    QXmlStreamWriter xml(&ret);
    xml.setAutoFormatting(true);
    writeXML(xml, writingVersion);
    return ret;
}

QString CiftiXMLNew::writeXMLToString(const CiftiVersion& writingVersion) const
{
    QString ret;
    QXmlStreamWriter xml(&ret);
    xml.setAutoFormatting(true);
    writeXML(xml, writingVersion);
    return ret;
}

void CiftiXMLNew::writeXML(QXmlStreamWriter& xml, const CiftiVersion& writingVersion) const
{
    xml.writeStartElement("CIFTI");
    xml.writeAttribute("Version", writingVersion.toString());
    if (writingVersion == CiftiVersion(1, 0))//switch/case on major/minor would be much harder to read
    {
        xml.writeAttribute("NumberOfMatrices", "1");
        writeMatrix1(xml);
    } else if (writingVersion == CiftiVersion(1, 1)) {//we will use "1.1" to test our cifti-2 implementation
        //CaretLogWarning("writing cifti version '1.1', this should not exist in the wild");
        writeMatrix2(xml);
    /*} else if (writingVersion == CiftiVersion(2, 0)) {
        writeMatrix2(xml);//*/ //leave commented out until cifti-2 is finalized
    } else {
        throw CaretException("unknown Cifti Version: '" + writingVersion.toString());
    }
    xml.writeEndElement();
}

void CiftiXMLNew::writeMatrix1(QXmlStreamWriter& xml) const
{
    int numDims = (int)m_indexMaps.size();
    bool haveVolData = false;
    VolumeSpace volSpace;
    for (int i = 0; i < numDims; ++i)
    {
        if (m_indexMaps[i] == NULL) throw CaretException("dimension " + QString::number(i) + " was not given a mapping");
        switch (m_indexMaps[i]->getType())
        {
            case CiftiIndexMap::BRAIN_MODELS:
            {
                const CiftiBrainModelsMap& myMap = dynamic_cast<const CiftiBrainModelsMap&>(*(m_indexMaps[i]));
                if (myMap.hasVolumeData())
                {
                    if (haveVolData)
                    {
                        if (myMap.getVolumeSpace() != volSpace)
                        {
                            throw CaretException("cannot write different volume spaces for different dimensions in CIFTI-1");
                        }
                    } else {
                        haveVolData = true;
                        volSpace = myMap.getVolumeSpace();
                    }
                }
                break;
            }
            case CiftiIndexMap::PARCELS:
            {
                const CiftiParcelsMap& myMap = dynamic_cast<const CiftiParcelsMap&>(*(m_indexMaps[i]));
                if (myMap.hasVolumeData())
                {
                    if (haveVolData)
                    {
                        if (myMap.getVolumeSpace() != volSpace)
                        {
                            throw CaretException("cannot write different volume spaces for different dimensions in CIFTI-1");
                        }
                    } else {
                        haveVolData = true;
                        volSpace = myMap.getVolumeSpace();
                    }
                }
                break;
            }
            default:
                break;
        }
    }
    xml.writeStartElement("Matrix");
    if (m_filePalette != NULL)
    {
        m_fileMetaData.set("PaletteColorMapping", m_filePalette->encodeInXML());
    }
    m_fileMetaData.writeCiftiXML1(xml);
    if (haveVolData)
    {
        volSpace.writeCiftiXML1(xml);
    }
    vector<bool> used(numDims, false);
    for (int i = 0; i < numDims; ++i)
    {
        if (!used[i])
        {
            used[i] = true;
            int outputNum = i;
            if (outputNum < 2) outputNum = 1 - outputNum;//ie, swap 0 and 1
            QString appliesTo = QString::number(outputNum);//initialize containing just the current dimension
            for (int j = i + 1; j < numDims; ++j)//compare to all later unused dimensions for deduplication
            {//technically, shouldn't need to check for previously used as long as equality is exact, but means maybe fewer comparisons, and to prevent a bug in == from getting stranger behavior
                if (!used[j])
                {
                    if ((*m_indexMaps[i]) == (*m_indexMaps[j]))
                    {
                        outputNum = j;
                        if (outputNum < 2) outputNum = 1 - outputNum;
                        appliesTo += "," + QString::number(outputNum);
                        used[j] = true;
                    }
                }
            }
            xml.writeStartElement("MatrixIndicesMap");//should the CiftiIndexMap do this instead, and we pass appliesTo to it as string?  probably not important, we won't use them in any other xml
            xml.writeAttribute("AppliesToMatrixDimension", appliesTo);
            m_indexMaps[i]->writeXML1(xml);
            xml.writeEndElement();
        }
    }
    xml.writeEndElement();
}

void CiftiXMLNew::writeMatrix2(QXmlStreamWriter& xml) const
{
    int numDims = (int)m_indexMaps.size();
    for (int i = 0; i < numDims; ++i)
    {
        if (m_indexMaps[i] == NULL) throw CaretException("dimension " + QString::number(i) + " was not given a mapping");
    }
    xml.writeStartElement("Matrix");
    if (m_filePalette != NULL)
    {
        m_fileMetaData.set("PaletteColorMapping", m_filePalette->encodeInXML());
    }
    m_fileMetaData.writeCiftiXML2(xml);
    vector<bool> used(numDims, false);
    for (int i = 0; i < numDims; ++i)
    {
        if (!used[i])
        {
            used[i] = true;
            QString appliesTo = QString::number(i);//initialize containing just the current dimension
            for (int j = i + 1; j < numDims; ++j)//compare to all later unused dimensions for deduplication
            {//technically, shouldn't need to check for previously used as long as equality is exact, but means maybe fewer comparisons, and to prevent a bug in == from getting stranger behavior
                if (!used[j])
                {
                    if ((*m_indexMaps[i]) == (*m_indexMaps[j]))
                    {
                        appliesTo += "," + QString::number(j);
                        used[j] = true;
                    }
                }
            }
            xml.writeStartElement("MatrixIndicesMap");//should the CiftiIndexMap do this instead, and we pass appliesTo to it as string?  probably not important, we won't use them in any other xml
            xml.writeAttribute("AppliesToMatrixDimension", appliesTo);
            m_indexMaps[i]->writeXML2(xml);
            xml.writeEndElement();
        }
    }
    xml.writeEndElement();
}
