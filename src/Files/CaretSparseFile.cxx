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

#include "CaretSparseFile.h"

#include "CaretAssert.h"
#include "CaretLogger.h"
#include "FileInformation.h"

#include <QByteArray>

using namespace caret;
using namespace std;

const char magic1[] = "\0\0\0\0cst\0";
const char magic2[] = "\0\0\0\0cs2\0";

CaretSparseFile::CaretSparseFile(const AString& fileName)
{
    readFile(fileName);
}

void CaretSparseFile::readFile(const AString& filename)
{
    m_file.close();
    if (filename.endsWith(".gz"))
    {
        throw DataFileException("wbsparse files cannot be read while compressed: " + filename);
    }
    m_file.open(filename);
    FileInformation fileInfo(filename);//useful later for file size, but create it now to reduce the amount of time between file open and size check
    char buf[8];
    m_file.read(buf, 8);
    bool pass = true;
    for (int i = 0; i < 8; ++i)
    {
        if (buf[i] != magic1[i])
        {
            pass = false;
            break;
        }
    }
    if (pass)
    {
        readFileV1(fileInfo);
    } else {
        pass = true;
        for (int i = 0; i < 8; ++i)
        {
            if (buf[i] != magic2[i])
            {
                pass = false;
                break;
            }
        }
        if (pass)
        {
            readFileV2(fileInfo);
        } else {
            throw DataFileException("file does not have a recognized magic string for wbsparse: " + filename);
        }
    }
}

void CaretSparseFile::readFileV1(FileInformation& fileInfo)
{
    //NOTE: m_file starts AFTER the magic
    m_header.longIndex = 1;
    m_header.valueType = Fibers;
    m_header.minorVersion = -1; //HACK: use this to signal V1 header, if it ever matters
    m_file.read(m_header.dims, 2 * sizeof(int64_t));
    if (ByteSwapping::isSystemBigEndian())
    {
        ByteSwapping::swapArray(m_header.dims, 2);
    }
    if (m_header.dims[0] < 1 || m_header.dims[1] < 1) throw DataFileException("both dimensions must be positive");
    m_indexArray.resize(m_header.dims[1] + 1);
    vector<int64_t> lengthArray(m_header.dims[1]);
    m_file.read(lengthArray.data(), m_header.dims[1] * sizeof(int64_t));
    if (ByteSwapping::isSystemBigEndian())
    {
        ByteSwapping::swapArray(lengthArray.data(), m_header.dims[1]);
    }
    m_indexArray[0] = 0;
    for (int64_t i = 0; i < m_header.dims[1]; ++i)
    {
        if (lengthArray[i] > m_header.dims[0] || lengthArray[i] < 0) throw DataFileException("impossible value found in length array");
        m_indexArray[i + 1] = m_indexArray[i] + lengthArray[i];
    }
    m_valuesOffset = 8 + 2 * sizeof(int64_t) + m_header.dims[1] * sizeof(int64_t);
    int64_t xml_offset = m_valuesOffset + m_indexArray[m_header.dims[1]] * 2 * sizeof(int64_t);
    if (xml_offset >= fileInfo.size()) throw DataFileException("file is truncated");
    int64_t xml_length = fileInfo.size() - xml_offset;
    if (xml_length < 1) throw DataFileException("file is truncated");
    m_file.seek(xml_offset);
    const int64_t seekResult = m_file.pos();
    if (seekResult != xml_offset) {
        const AString msg = ("Tried to seek to "
                             + AString::number(xml_offset)
                             + " but got an offset of "
                             + AString::number(seekResult));
        throw DataFileException(msg);
    }
    
    QByteArray myXMLBytes(xml_length, '\0');
    m_file.read(myXMLBytes.data(), xml_length);
    m_xml.readXML(myXMLBytes);
    if (m_xml.getDimensionLength(CiftiXML::ALONG_ROW) != m_header.dims[0] || m_xml.getDimensionLength(CiftiXML::ALONG_COLUMN) != m_header.dims[1])
    {
        throw DataFileException("cifti XML doesn't match dimensions of sparse file");
    }
}

void CaretSparseFile::readFileV2(FileInformation& fileInfo)
{
    //NOTE: m_file/header starts AFTER the magic
    m_header.read(m_file); //also does swapping if needed
    m_indexArray.resize(m_header.dims[1] + 1);
    vector<int64_t> lengthArray(m_header.dims[1]);
    //WARNING: length array is ALWAYS int64, even when longIndex = 0
    m_file.read(lengthArray.data(), m_header.dims[1] * sizeof(int64_t));
    if (ByteSwapping::isSystemBigEndian())
    {
        ByteSwapping::swapArray(lengthArray.data(), m_header.dims[1]);
    }
    m_indexArray[0] = 0;
    for (int64_t i = 0; i < m_header.dims[1]; ++i)
    {
        if (lengthArray[i] > m_header.dims[0] || lengthArray[i] < 0) throw DataFileException("impossible value found in length array: " + m_file.getFilename());
        m_indexArray[i + 1] = m_indexArray[i] + lengthArray[i];
    }
    m_valuesOffset = 8 + sizeof(HeaderV2) + m_header.dims[1] * sizeof(int64_t);
    int64_t xml_offset = m_valuesOffset + m_indexArray[m_header.dims[1]] * (m_header.indexSize() + m_header.valueSize());
    if (xml_offset >= fileInfo.size()) throw DataFileException("file is truncated: " + m_file.getFilename());
    int64_t xml_length = fileInfo.size() - xml_offset;
    if (xml_length < 1) throw DataFileException("file is truncated: " + m_file.getFilename());
    m_file.seek(xml_offset);
    const int64_t seekResult = m_file.pos();
    if (seekResult != xml_offset) {
        const AString msg = ("Tried to seek to "
                             + AString::number(xml_offset)
                             + " but got an offset of "
                             + AString::number(seekResult)
                             + ": " + m_file.getFilename());
        throw DataFileException(msg);
    }
    
    QByteArray myXMLBytes(xml_length, '\0');
    m_file.read(myXMLBytes.data(), xml_length);
    m_xml.readXML(myXMLBytes);
    if (m_xml.getDimensionLength(CiftiXML::ALONG_ROW) != m_header.dims[0] || m_xml.getDimensionLength(CiftiXML::ALONG_COLUMN) != m_header.dims[1])
    {
        throw DataFileException("cifti XML doesn't match dimensions of sparse file: " + m_file.getFilename());
    }
}

void CaretSparseFile::HeaderV2::read(CaretBinaryFile& file)
{
    CaretAssert(sizeof(HeaderV2) == 21);
    if (sizeof(HeaderV2) != 21) throw DataFileException("wbsparse v2 header struct has the wrong size, add no-padding directive and recompile: " + sizeof(HeaderV2));
    file.read(this, sizeof(HeaderV2));
    if (ByteSwapping::isSystemBigEndian())
    {
        ByteSwapping::swap(minorVersion);
        ByteSwapping::swap(valueType);
        ByteSwapping::swapArray(dims, 2);
        ByteSwapping::swap(longIndex);//int8, no-op
    }
    if (minorVersion > 0) throw DataFileException("unsupported version of wbsparse: " + file.getFilename());
    if (dims[0] < 1 || dims[1] < 1) throw DataFileException("both dimensions must be positive: " + file.getFilename());
    if (valueType < 1 || valueType > 5) throw DataFileException("invalid value type: " + file.getFilename());
}

void CaretSparseFile::HeaderV2::write(CaretBinaryFile& file) const
{
    CaretAssert(sizeof(HeaderV2) == 21);
    if (sizeof(HeaderV2) != 21) throw DataFileException("wbsparse v2 header struct has the wrong size, add no-padding directive and recompile: " + sizeof(HeaderV2));
    HeaderV2 temp(*this);
    if (ByteSwapping::isSystemBigEndian())
    {
        ByteSwapping::swap(temp.minorVersion);
        ByteSwapping::swap(temp.valueType);
        ByteSwapping::swapArray(temp.dims, 2);
        ByteSwapping::swap(temp.longIndex);//int8, no-op
    }
    file.write(&temp, sizeof(HeaderV2));
}

CaretSparseFile::~CaretSparseFile()
{
}

void CaretSparseFile::decodeFibers(const uint64_t& coded, FiberFractions& decoded)
{
    decoded.fiberFractions.resize(3);
    decoded.totalCount = coded>>32;
    uint32_t temp = coded & ((1LL<<32) - 1);
    const static uint32_t MASK = ((1<<10) - 1);
    decoded.distance = (temp & MASK);
    decoded.fiberFractions[1] = ((temp>>10) & MASK) / 1000.0f;
    decoded.fiberFractions[0] = ((temp>>20) & MASK) / 1000.0f;
    decoded.fiberFractions[2] = 1.0f - decoded.fiberFractions[0] - decoded.fiberFractions[1];
    if (decoded.fiberFractions[2] < -0.002f || (temp & (3<<30)))
    {
        throw DataFileException("error decoding value '" + AString::number(coded) + "' from workbench sparse trajectory file " + m_file.getFilename());
    }
    if (decoded.fiberFractions[2] < 0.0f) decoded.fiberFractions[2] = 0.0f;
}

void FiberFractions::clear()
{
    totalCount = 0;
    fiberFractions.clear();
    distance = 0.0f;
}

CaretSparseFileWriter::CaretSparseFileWriter(const AString& fileName, const CiftiXML& xml, const CaretSparseFile::ValueType writingType, const int forceVersion)
{
    if (!fileName.endsWith(".wbsparse"))
    {//TODO: suggest endings based on cifti xml
        CaretLogWarning("sparse file '" + fileName + "' should be saved ending in .wbsparse");
    }
    m_finished = false;
    int64_t dimensions[2] = { xml.getDimensionLength(CiftiXML::ALONG_ROW), xml.getDimensionLength(CiftiXML::ALONG_COLUMN) };
    if (dimensions[0] < 1 || dimensions[1] < 1) throw DataFileException("both dimensions must be positive");
    m_xml = xml;
    m_header.dims[0] = dimensions[0]; //wbsparse doesn't support 3 dimensions yet
    m_header.dims[1] = dimensions[1];
    m_header.valueType = writingType;
    if (forceVersion == -1)
    { //prefer older version if possible
        if (m_header.valueType == CaretSparseFile::Fibers)
        {
            m_header.minorVersion = -1; //HACK: use this to signal V1
        } else {
            m_header.minorVersion = 0;
        }
    } else {
        if (forceVersion == 1 && m_header.valueType != CaretSparseFile::Fibers)
        {
            throw DataFileException("cannot write non-fibers wbsparse file '" + fileName + "' in V1 format");
        }
        switch (forceVersion)
        {
            case 1:
                m_header.minorVersion = -1;
                ;;
            case 2:
                m_header.minorVersion = 0;
                ;;
            default:
                CaretAssert(0);
                throw DataFileException("unknown wbsparse version: " + AString::number(forceVersion));
                ;;
        }
    }
    switch (m_header.minorVersion)
    {
        case -1: //V1
            m_header.longIndex = 1;
            break;
        case 0: //V2.0
            m_header.longIndex = (m_header.dims[0] > numeric_limits<uint32_t>::max());
            break;
        default:
            CaretAssert(0);
            throw DataFileException("unhandled wbsparse minor version: " + AString::number(m_header.minorVersion));
    }
    if (fileName.endsWith(".gz"))
    {
        throw DataFileException("wbsparse files cannot be written compressed");
    }//because after we finish writing the data, we have to come back and write the lengths array
    m_file.open(fileName, CaretBinaryFile::WRITE_TRUNCATE);
    if (m_header.minorVersion == -1)
    {
        m_file.write(magic1, 8);
        int64_t tempdims[2] = { m_header.dims[0], m_header.dims[1] };
        if (ByteSwapping::isSystemBigEndian())
        {
            ByteSwapping::swapArray(tempdims, 2);
        }
        m_file.write(tempdims, 2 * sizeof(int64_t));
        m_valuesOffset = 8 + 2 * sizeof(int64_t) + m_header.dims[1] * sizeof(int64_t);
    } else {
        m_file.write(magic2, 8);
        m_header.write(m_file); //handles swapping
        m_valuesOffset = 8 + sizeof(CaretSparseFile::HeaderV2) + m_header.dims[1] * sizeof(int64_t);
    }
    //write dummy placeholder bytes for currently-unknown length array
    m_lengthArray.resize(m_header.dims[1], 0);//initialize the memory so that valgrind won't complain
    m_file.write(m_lengthArray.data(), m_header.dims[1] * sizeof(int64_t));//write it to get the file to the correct length
    m_nextRowIndex = 0;
}

void CaretSparseFileWriter::finish()
{
    if (m_finished) return;
    m_finished = true;
    while (m_nextRowIndex < m_header.dims[1])
    {
        m_lengthArray[m_nextRowIndex] = 0;
        ++m_nextRowIndex;
    }
    QByteArray myXMLBytes = m_xml.writeXMLToQByteArray();
    m_file.write(myXMLBytes.constData(), myXMLBytes.size());
    if (m_header.minorVersion == -1)
    {
        m_file.seek(8 + 2 * sizeof(int64_t)); //V1
    } else {
        m_file.seek(8 + sizeof(CaretSparseFile::HeaderV2));
    }
    if (ByteSwapping::isSystemBigEndian())
    { //WARNING: leaves length array broken
        ByteSwapping::swapArray(m_lengthArray.data(), m_lengthArray.size());
    }
    m_file.write(m_lengthArray.data(), m_lengthArray.size() * sizeof(uint64_t));
    m_file.close();
}

CaretSparseFileWriter::~CaretSparseFileWriter()
{
    finish();
}

void CaretSparseFileWriter::encodeFibers(const FiberFractions& orig, uint64_t& coded)
{
    coded = (((uint64_t)orig.totalCount)<<32) | (myclamp(orig.fiberFractions[0] * 1000.0f + 0.5f)<<20) |
            (myclamp(orig.fiberFractions[1] * 1000.0f + 0.5f)<<10) | (myclamp(orig.distance));
}

uint32_t CaretSparseFileWriter::myclamp(const int& x)
{
    if (x >= 1000) return 1000;
    if (x <= 0) return 0;
    return x;
}
