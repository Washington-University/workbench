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

#include "CiftiXML.h"

#include "CaretAssert.h"
#include "DataFileException.h"
#include "CaretLogger.h"
#include "GiftiMetaData.h"
#include "PaletteColorMapping.h"

#include <QStringList>

#include <set>

using namespace std;
using namespace caret;

int CiftiXML::directionFromString(const QString& input)
{
    bool ok = false;
    int converted = input.toInt(&ok);
    if (ok)
    {
        if (converted < 1) throw DataFileException("invalid integer direction, use 1 or greater");
        return converted - 1;//use 1-indexed convention for input
    }
    if (input == "ROW") return ALONG_ROW;
    if (input == "COLUMN") return ALONG_COLUMN;//should we also allow STACK?  integers seem cleaner
    throw DataFileException("unrecognized direction string, please use an integer, 'ROW', or 'COLUMN'");
}

QString CiftiXML::directionFromStringExplanation()
{
    return "The direction can be either an integer starting from 1, or the strings 'ROW' or 'COLUMN'.";
}

CiftiXML::CiftiXML(const CiftiXML& rhs)
{
    copyHelper(rhs);
}

CiftiXML& CiftiXML::operator=(const CiftiXML& rhs)
{
    if (this != &rhs) copyHelper(rhs);
    return *this;
}

void CiftiXML::copyHelper(const CiftiXML& rhs)
{
    int numDims = (int)rhs.m_indexMaps.size();
    m_indexMaps.resize(numDims);
    for (int i = 0; i < numDims; ++i)
    {
        m_indexMaps[i] = CaretPointer<CiftiMappingType>(rhs.m_indexMaps[i]->clone());
    }
    m_parsedVersion = rhs.m_parsedVersion;
    m_fileMetaData = rhs.m_fileMetaData;
    if (rhs.m_filePalette != NULL)
    {
        m_filePalette.grabNew(new PaletteColorMapping());
        *(m_filePalette) = *(rhs.m_filePalette);
    } else {
        m_filePalette.grabNew(NULL);
    }
}

bool CiftiXML::operator==(const CiftiXML& rhs) const
{
    int numDims = getNumberOfDimensions();
    if (rhs.getNumberOfDimensions() != numDims) return false;
    if (m_fileMetaData != rhs.m_fileMetaData) return false;
    if ((*getFilePalette()) != (*rhs.getFilePalette())) return false;
    for (int i = 0; i < numDims; ++i)
    {
        const CiftiMappingType* left = getMap(i), *right = rhs.getMap(i);
        if (left == NULL && right == NULL) continue;
        if (left == NULL || right == NULL) return false;//only one NULL, due to above test
        if ((*left) != (*right)) return false;//finally can dereference them
    }
    return true;
}

bool CiftiXML::approximateMatch(const CiftiXML& rhs) const
{
    int numDims = getNumberOfDimensions();
    if (rhs.getNumberOfDimensions() != numDims) return false;
    for (int i = 0; i < numDims; ++i)
    {
        const CiftiMappingType* left = getMap(i), *right = rhs.getMap(i);
        if (left == NULL && right == NULL) continue;
        if (left == NULL || right == NULL) return false;//only one NULL, due to above test
        if (!left->approximateMatch(*right)) return false;//finally can dereference them
    }
    return true;
}

const CiftiMappingType* CiftiXML::getMap(const int& direction) const
{
    CaretAssertVectorIndex(m_indexMaps, direction);
    return m_indexMaps[direction];
}

CiftiMappingType* CiftiXML::getMap(const int& direction)
{
    CaretAssertVectorIndex(m_indexMaps, direction);
    return m_indexMaps[direction];
}

GiftiMetaData* CiftiXML::getFileMetaData() const
{
    return &m_fileMetaData;
}

PaletteColorMapping* CiftiXML::getFilePalette() const
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

const CiftiBrainModelsMap& CiftiXML::getBrainModelsMap(const int& direction) const
{
    CaretAssertVectorIndex(m_indexMaps, direction);
    CaretAssert(getMappingType(direction) == CiftiMappingType::BRAIN_MODELS);//assert so we catch it in debug
    return dynamic_cast<const CiftiBrainModelsMap&>(*getMap(direction));//let release throw bad_cast or segfault
}

CiftiBrainModelsMap& CiftiXML::getBrainModelsMap(const int& direction)
{
    CaretAssertVectorIndex(m_indexMaps, direction);
    CaretAssert(getMappingType(direction) == CiftiMappingType::BRAIN_MODELS);//assert so we catch it in debug
    return dynamic_cast<CiftiBrainModelsMap&>(*getMap(direction));//let release throw bad_cast or segfault
}

const CiftiLabelsMap& CiftiXML::getLabelsMap(const int& direction) const
{
    CaretAssertVectorIndex(m_indexMaps, direction);
    CaretAssert(getMappingType(direction) == CiftiMappingType::LABELS);//assert so we catch it in debug
    return dynamic_cast<const CiftiLabelsMap&>(*getMap(direction));//let release throw bad_cast or segfault
}

CiftiLabelsMap& CiftiXML::getLabelsMap(const int& direction)
{
    CaretAssertVectorIndex(m_indexMaps, direction);
    CaretAssert(getMappingType(direction) == CiftiMappingType::LABELS);//assert so we catch it in debug
    return dynamic_cast<CiftiLabelsMap&>(*getMap(direction));//let release throw bad_cast or segfault
}

const CiftiParcelsMap& CiftiXML::getParcelsMap(const int& direction) const
{
    CaretAssertVectorIndex(m_indexMaps, direction);
    CaretAssert(getMappingType(direction) == CiftiMappingType::PARCELS);//assert so we catch it in debug
    return dynamic_cast<const CiftiParcelsMap&>(*getMap(direction));//let release throw bad_cast or segfault
}

CiftiParcelsMap& CiftiXML::getParcelsMap(const int& direction)
{
    CaretAssertVectorIndex(m_indexMaps, direction);
    CaretAssert(getMappingType(direction) == CiftiMappingType::PARCELS);//assert so we catch it in debug
    return dynamic_cast<CiftiParcelsMap&>(*getMap(direction));//let release throw bad_cast or segfault
}

const CiftiScalarsMap& CiftiXML::getScalarsMap(const int& direction) const
{
    CaretAssertVectorIndex(m_indexMaps, direction);
    CaretAssert(getMappingType(direction) == CiftiMappingType::SCALARS);//assert so we catch it in debug
    return dynamic_cast<const CiftiScalarsMap&>(*getMap(direction));//let release throw bad_cast or segfault
}

CiftiScalarsMap& CiftiXML::getScalarsMap(const int& direction)
{
    CaretAssertVectorIndex(m_indexMaps, direction);
    CaretAssert(getMappingType(direction) == CiftiMappingType::SCALARS);//assert so we catch it in debug
    return dynamic_cast<CiftiScalarsMap&>(*getMap(direction));//let release throw bad_cast or segfault
}

const CiftiSeriesMap& CiftiXML::getSeriesMap(const int& direction) const
{
    CaretAssertVectorIndex(m_indexMaps, direction);
    CaretAssert(getMappingType(direction) == CiftiMappingType::SERIES);//assert so we catch it in debug
    return dynamic_cast<const CiftiSeriesMap&>(*getMap(direction));//let release throw bad_cast or segfault
}

CiftiSeriesMap& CiftiXML::getSeriesMap(const int& direction)
{
    CaretAssertVectorIndex(m_indexMaps, direction);
    CaretAssert(getMappingType(direction) == CiftiMappingType::SERIES);//assert so we catch it in debug
    return dynamic_cast<CiftiSeriesMap&>(*getMap(direction));//let release throw bad_cast or segfault
}

int64_t CiftiXML::getDimensionLength(const int& direction) const
{
    const CiftiMappingType* tempMap = getMap(direction);
    CaretAssert(tempMap != NULL);
    return tempMap->getLength();
}

vector<int64_t> CiftiXML::getDimensions() const
{
    vector<int64_t> ret(getNumberOfDimensions());
    for (int i = 0; i < (int)ret.size(); ++i)
    {
        ret[i] = getDimensionLength(i);
    }
    return ret;
}

CiftiMappingType::MappingType CiftiXML::getMappingType(const int& direction) const
{
    CaretAssertVectorIndex(m_indexMaps, direction);
    CaretAssert(m_indexMaps[direction] != NULL);
    return m_indexMaps[direction]->getType();
}

void CiftiXML::setMap(const int& direction, const CiftiMappingType& mapIn)
{
    CaretAssertVectorIndex(m_indexMaps, direction);
    if (mapIn.getType() == CiftiMappingType::LABELS)
    {
        for (int i = 0; i < getNumberOfDimensions(); ++i)
        {
            if (i != direction && m_indexMaps[i] != NULL && m_indexMaps[i]->getType() == CiftiMappingType::LABELS)
            {
                throw DataFileException("Cifti XML cannot contain a label mapping on more than one dimension");
            }
        }
    }
    m_indexMaps[direction] = CaretPointer<CiftiMappingType>(mapIn.clone());
}

void CiftiXML::setNumberOfDimensions(const int& num)
{
    m_indexMaps.resize(num);
}

void CiftiXML::clear()
{
    setNumberOfDimensions(0);
    m_filePalette.grabNew(NULL);
    m_fileMetaData.clear(false);
    m_parsedVersion = CiftiVersion();
}

void CiftiXML::readXML(const QString& text)
{
    QXmlStreamReader xml(text);
    readXML(xml);
}

void CiftiXML::readXML(const QByteArray& data)
{
    QString text(data);//constructing a qstring appears to be the simplest way to remove trailing nulls, which otherwise trip an "Extra content at end of document" error
    readXML(text);//then put it through the string reader, just to simplify code paths
}

int32_t CiftiXML::getIntentInfo(const CiftiVersion& writingVersion, char intentNameOut[16]) const
{
    int32_t ret;
    const char* name = NULL;
    if (writingVersion == CiftiVersion(1, 0))//cifti-1: unknown didn't exist, and "ConnDense" was default
    {
        ret = 3001;//default
        name = "ConnDense";
        if (getNumberOfDimensions() > 0 && getMappingType(0) == CiftiMappingType::SERIES) { ret = 3002; name = "ConnDenseTime"; }//same logic as was actually used in CiftiFile
        if (getNumberOfDimensions() > 1 && getMappingType(1) == CiftiMappingType::SERIES) { ret = 3002; name = "ConnDenseTime"; }//NOTE: name for this code is different than cifti-2
    } else if (writingVersion == CiftiVersion(1, 1) || writingVersion == CiftiVersion(2, 0)) {//cifti-2
        ret = 3000;//default
        name = "ConnUnknown";
        switch (getNumberOfDimensions())
        {
            case 2:
            {
                CiftiMappingType::MappingType first = getMappingType(0), second = getMappingType(1);
                if (first == CiftiMappingType::BRAIN_MODELS && second == CiftiMappingType::BRAIN_MODELS) { ret = 3001; name = "ConnDense"; }
                if (first == CiftiMappingType::SERIES && second == CiftiMappingType::BRAIN_MODELS) { ret = 3002; name = "ConnDenseSeries"; }
                if (first == CiftiMappingType::PARCELS && second == CiftiMappingType::PARCELS) { ret = 3003; name = "ConnParcels"; }
                if (first == CiftiMappingType::SERIES && second == CiftiMappingType::PARCELS) { ret = 3004; name = "ConnParcelSries"; }//NOTE: 3005 is reserved but not used
                if (first == CiftiMappingType::SCALARS && second == CiftiMappingType::BRAIN_MODELS) { ret = 3006; name = "ConnDenseScalar"; }
                if (first == CiftiMappingType::LABELS && second == CiftiMappingType::BRAIN_MODELS) { ret = 3007; name = "ConnDenseLabel"; }
                if (first == CiftiMappingType::SCALARS && second == CiftiMappingType::PARCELS) { ret = 3008; name = "ConnParcelScalr"; }
                if (first == CiftiMappingType::BRAIN_MODELS && second == CiftiMappingType::PARCELS) { ret = 3009; name = "ConnParcelDense"; }
                if (first == CiftiMappingType::PARCELS && second == CiftiMappingType::BRAIN_MODELS) { ret = 3010; name = "ConnDenseParcel"; }
                break;
            }
            case 3:
            {
                CiftiMappingType::MappingType first = getMappingType(0), second = getMappingType(1), third = getMappingType(2);
                if (first == CiftiMappingType::PARCELS && second == CiftiMappingType::PARCELS && third == CiftiMappingType::SERIES) { ret = 3011; name = "ConnPPSr"; }
                if (first == CiftiMappingType::PARCELS && second == CiftiMappingType::PARCELS && third == CiftiMappingType::SCALARS) { ret = 3012; name = "ConnPPSc"; }
                break;
            }
            default:
                break;
        }
    } else {
        throw DataFileException("unknown cifti version: " + writingVersion.toString());
    }
    int i;
    for (i = 0; i < 16 && name[i] != '\0'; ++i) intentNameOut[i] = name[i];
    for (; i < 16; ++i) intentNameOut[i] = '\0';
    return ret;
}

void CiftiXML::readXML(QXmlStreamReader& xml)
{
    clear();
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
                        throw DataFileException("CIFTI element may only be specified once");
                    }
                    QXmlStreamAttributes attributes = xml.attributes();
                    if(attributes.hasAttribute("Version"))
                    {
                        m_parsedVersion = CiftiVersion(attributes.value("Version").toString());
                    } else {
                        throw DataFileException("Cifti XML missing Version attribute.");
                    }
                    if (m_parsedVersion == CiftiVersion(1, 0))//switch/case on major/minor would be much harder to read
                    {
                        parseCIFTI1(xml);
                        if (xml.hasError()) break;
                    } else if (m_parsedVersion == CiftiVersion(1, 1)) {
                        CaretLogWarning("parsing cifti version '1.1', this should not exist in the wild");
                        parseCIFTI2(xml);//we used "1.1" to test our cifti-2 implementation
                        if (xml.hasError()) break;
                    } else if (m_parsedVersion == CiftiVersion(2, 0)) {
                        parseCIFTI2(xml);
                        if (xml.hasError()) break;
                    } else {
                        throw DataFileException("unknown Cifti Version: '" + m_parsedVersion.toString());
                    }
                    haveCifti = true;
                } else {
                    throw DataFileException("unexpected root element in Cifti XML: " + name.toString());
                }
            }
        }
        if (!xml.hasError() && !haveCifti)
        {
            throw DataFileException("CIFTI element not found");
        }
    } catch (CaretException& e) {
        throw DataFileException("Cifti XML error: " + e.whatString());//so we can throw on error instead of doing a bunch of dancing with xml.raiseError and xml.hasError
    }
    if(xml.hasError())
    {
        throw DataFileException("Cifti XML error: " + xml.errorString());
    }
}

void CiftiXML::parseCIFTI1(QXmlStreamReader& xml)
{
    QXmlStreamAttributes attributes = xml.attributes();
    if (attributes.hasAttribute("NumberOfMatrices"))
    {
        if (attributes.value("NumberOfMatrices") != "1")
        {
            throw DataFileException("attribute NumberOfMatrices in CIFTI is required to be 1 for CIFTI-1");
        }
    } else {
        throw DataFileException("missing attribute NumberOfMatrices in CIFTI");
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
                    throw DataFileException("Matrix element may only be specified once");
                }
                parseMatrix1(xml);
                if (xml.hasError()) return;
                haveMatrix = true;
            } else {
                throw DataFileException("unexpected element in CIFTI: " + name.toString());
            }
        } else if (xml.isEndElement()) {
            break;
        }
    }
    if (!haveMatrix)
    {
        throw DataFileException("Matrix element not found in CIFTI");
    }
    if (xml.hasError()) return;
    CaretAssert(xml.isEndElement() && xml.name() == "CIFTI");
}

void CiftiXML::parseCIFTI2(QXmlStreamReader& xml)//yes, these will often have largely similar code, but it seems cleaner than having only some functions split, or constantly rechecking the version
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
                    throw DataFileException("Matrix element may only be specified once");
                }
                parseMatrix2(xml);
                if (xml.hasError()) return;
                haveMatrix = true;
            } else {
                throw DataFileException("unexpected element in CIFTI: " + name.toString());
            }
        } else if (xml.isEndElement()) {
            break;
        }
    }
    if (!haveMatrix)
    {
        throw DataFileException("Matrix element not found in CIFTI");
    }
    CaretAssert(xml.isEndElement() && xml.name() == "CIFTI");
}

void CiftiXML::parseMatrix1(QXmlStreamReader& xml)
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
                    throw DataFileException("MetaData may only be specified once in Matrix");
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
                    throw DataFileException("Volume may only be specified once in Matrix");
                }
                fileVolSpace.readCiftiXML1(xml);
                if (xml.hasError()) return;
                haveVolSpace = true;
            } else if (name == "LabelTable") {
                CaretLogFiner("skipping unused LabelTable element in Matrix in CIFTI-1");
                xml.readElementText(QXmlStreamReader::SkipChildElements);
            } else {
                throw DataFileException("unexpected element in Matrix: " + name.toString());
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
            throw DataFileException("missing mapping for dimension '" + QString::number(displaynum) + "'");
        }
        switch (m_indexMaps[i]->getType())
        {
            case CiftiMappingType::BRAIN_MODELS:
            {
                CiftiBrainModelsMap& myMap = dynamic_cast<CiftiBrainModelsMap&>(*(m_indexMaps[i]));
                if (myMap.hasVolumeData())
                {
                    if (haveVolSpace)
                    {
                        myMap.setVolumeSpace(fileVolSpace);//also does the needed checking of voxel indices
                    } else {
                        throw DataFileException("BrainModels map uses voxels, but no Volume element exists");
                    }
                }
                break;
            }
            case CiftiMappingType::PARCELS:
            {
                CiftiParcelsMap& myMap = dynamic_cast<CiftiParcelsMap&>(*(m_indexMaps[i]));
                if (myMap.hasVolumeData())
                {
                    if (haveVolSpace)
                    {
                        myMap.setVolumeSpace(fileVolSpace);//ditto
                    } else {
                        throw DataFileException("Parcels map uses voxels, but no Volume element exists");
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

void CiftiXML::parseMatrix2(QXmlStreamReader& xml)
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
                    throw DataFileException("MetaData may only be specified once in Matrix");
                }
                m_fileMetaData.readCiftiXML2(xml);
                if (xml.hasError()) return;
                haveMetadata = true;
            } else if (name == "MatrixIndicesMap") {
                parseMatrixIndicesMap2(xml);
                if (xml.hasError()) return;
            } else {
                throw DataFileException("unexpected element in Matrix: " + name.toString());
            }
        } else if (xml.isEndElement()) {
            break;
        }
    }
    for (int i = 0; i < (int)m_indexMaps.size(); ++i)
    {
        if (m_indexMaps[i] == NULL)
        {
            throw DataFileException("missing mapping for dimension '" + QString::number(i) + "'");
        }
    }
    CaretAssert(xml.isEndElement() && xml.name() == "Matrix");
}

void CiftiXML::parseMatrixIndicesMap1(QXmlStreamReader& xml)
{
    QXmlStreamAttributes attributes = xml.attributes();
    if (!attributes.hasAttribute("AppliesToMatrixDimension"))
    {
        throw DataFileException("missing attribute AppliesToMatrixDimension in MatrixIndicesMap");
    }
    if (!attributes.hasAttribute("IndicesMapToDataType"))
    {
        throw DataFileException("missing attribute IndicesMapToDataType in MatrixIndicesMap");
    }
    QStringList values = attributes.value("AppliesToMatrixDimension").toString().split(',');
    bool ok = false;
    set<int> used;
    for(int i = 0; i < values.size(); i++)
    {
        int parsed = values[i].toInt(&ok);
        if (!ok || parsed < 0)
        {
            throw DataFileException("bad value in AppliesToMatrixDimension list: " + values[i]);
        }
        if (parsed < 2) parsed = 1 - parsed;//in other words, 0 becomes 1 and 1 becomes 0, since cifti-1 had them reversed
        if (used.find(parsed) != used.end())
        {
            throw DataFileException("AppliesToMatrixDimension contains repeated value: " + values[i]);
        }
        used.insert(parsed);
    }
    CaretPointer<CiftiMappingType> toRead;
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
        throw DataFileException("invalid value for IndicesMapToDataType in CIFTI-1: " + type.toString());
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
            m_indexMaps[*iter] = CaretPointer<CiftiMappingType>(toRead->clone());//make in-memory information independent per-dimension, rather than dealing with deduplication everywhere
        }
    }
    CaretAssert(xml.isEndElement() && xml.name() == "MatrixIndicesMap");
}

void CiftiXML::parseMatrixIndicesMap2(QXmlStreamReader& xml)
{
    QXmlStreamAttributes attributes = xml.attributes();
    if (!attributes.hasAttribute("AppliesToMatrixDimension"))
    {
        throw DataFileException("missing attribute AppliesToMatrixDimension in MatrixIndicesMap");
    }
    if (!attributes.hasAttribute("IndicesMapToDataType"))
    {
        throw DataFileException("missing attribute IndicesMapToDataType in MatrixIndicesMap");
    }
    QStringList values = attributes.value("AppliesToMatrixDimension").toString().split(',');
    bool ok = false;
    set<int> used;
    for(int i = 0; i < values.size(); i++)
    {
        int parsed = values[i].toInt(&ok);
        if (!ok || parsed < 0)
        {
            throw DataFileException("bad value in AppliesToMatrixDimension list: " + values[i]);
        }
        if (used.find(parsed) != used.end())
        {
            throw DataFileException("AppliesToMatrixDimension contains repeated value: " + values[i]);
        }
        used.insert(parsed);
    }
    CaretPointer<CiftiMappingType> toRead;
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
        throw DataFileException("invalid value for IndicesMapToDataType in CIFTI-1: " + type.toString());
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
            m_indexMaps[*iter] = CaretPointer<CiftiMappingType>(toRead->clone());//make in-memory information independent per-dimension, rather than dealing with deduplication everywhere
        }
    }
    CaretAssert(xml.isEndElement() && xml.name() == "MatrixIndicesMap");
}

QByteArray CiftiXML::writeXMLToQByteArray(const CiftiVersion& writingVersion) const
{
    QByteArray ret;
    QXmlStreamWriter xml(&ret);
    xml.setAutoFormatting(true);
    xml.writeStartDocument();
    writeXML(xml, writingVersion);
    xml.writeEndDocument();
    return ret;
}

QString CiftiXML::writeXMLToString(const CiftiVersion& writingVersion) const
{
    QString ret;
    QXmlStreamWriter xml(&ret);
    xml.setAutoFormatting(true);
    xml.writeStartDocument();
    writeXML(xml, writingVersion);
    xml.writeEndDocument();
    return ret;
}

void CiftiXML::writeXML(QXmlStreamWriter& xml, const CiftiVersion& writingVersion) const
{
    xml.writeStartElement("CIFTI");
    xml.writeAttribute("Version", writingVersion.toString());
    if (writingVersion == CiftiVersion(1, 0))//switch/case on major/minor would be much harder to read
    {
        xml.writeAttribute("NumberOfMatrices", "1");
        writeMatrix1(xml);
    } else if (writingVersion == CiftiVersion(1, 1)) {//we used "1.1" to test our cifti-2 implementation - should we even allow this?
        CaretLogWarning("writing cifti version '1.1', this should not exist in the wild");
        writeMatrix2(xml);
    } else if (writingVersion == CiftiVersion(2, 0)) {
        writeMatrix2(xml);
    } else {
        throw DataFileException("unknown Cifti writing version: '" + writingVersion.toString() + "'");
    }
    xml.writeEndElement();
}

void CiftiXML::writeMatrix1(QXmlStreamWriter& xml) const
{
    int numDims = (int)m_indexMaps.size();
    bool haveVolData = false;
    VolumeSpace volSpace;
    for (int i = 0; i < numDims; ++i)
    {
        if (m_indexMaps[i] == NULL) throw DataFileException("dimension " + QString::number(i) + " was not given a mapping");
        switch (m_indexMaps[i]->getType())
        {
            case CiftiMappingType::BRAIN_MODELS:
            {
                const CiftiBrainModelsMap& myMap = dynamic_cast<const CiftiBrainModelsMap&>(*(m_indexMaps[i]));
                if (myMap.hasVolumeData())
                {
                    if (haveVolData)
                    {
                        if (myMap.getVolumeSpace() != volSpace)
                        {
                            throw DataFileException("cannot write different volume spaces for different dimensions in CIFTI-1");
                        }
                    } else {
                        haveVolData = true;
                        volSpace = myMap.getVolumeSpace();
                    }
                }
                break;
            }
            case CiftiMappingType::PARCELS:
            {
                const CiftiParcelsMap& myMap = dynamic_cast<const CiftiParcelsMap&>(*(m_indexMaps[i]));
                if (myMap.hasVolumeData())
                {
                    if (haveVolData)
                    {
                        if (myMap.getVolumeSpace() != volSpace)
                        {
                            throw DataFileException("cannot write different volume spaces for different dimensions in CIFTI-1");
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

void CiftiXML::writeMatrix2(QXmlStreamWriter& xml) const
{
    int numDims = (int)m_indexMaps.size();
    for (int i = 0; i < numDims; ++i)
    {
        if (m_indexMaps[i] == NULL) throw DataFileException("dimension " + QString::number(i) + " was not given a mapping");
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

bool CiftiXML::mutablesModified() const
{
    if (m_fileMetaData.isModified()) return true;
    if (m_filePalette != NULL && m_filePalette->isModified()) return true;
    for (int d = 0; d < (int)m_indexMaps.size(); ++d)
    {
        if (m_indexMaps[d] != NULL && m_indexMaps[d]->mutablesModified()) return true;
    }
    return false;
}

void CiftiXML::clearMutablesModified() const
{
    m_fileMetaData.clearModified();
    if (m_filePalette != NULL) m_filePalette->clearModified();
    for (int d = 0; d < (int)m_indexMaps.size(); ++d)
    {
        if (m_indexMaps[d] != NULL) m_indexMaps[d]->clearMutablesModified();
    }
}
