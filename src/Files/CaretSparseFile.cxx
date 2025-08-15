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

#include "ByteOrderEnum.h"
#include "ByteSwapping.h"
#include "CaretAssert.h"
#include "CaretLogger.h"
#include "FileInformation.h"

#include <QByteArray>

using namespace caret;
using namespace std;

const char magic[] = "\0\0\0\0cst\0";

CaretSparseFile::CaretSparseFile(const AString& fileName)
{
    readFile(fileName);
}

void CaretSparseFile::readFile(const AString& filename)
{
    m_file.close();
    if (filename.endsWith(".gz"))
    {
        throw DataFileException("wbsparse files cannot be read while compressed");
    }
    m_file.open(filename);
    FileInformation fileInfo(filename);//useful later for file size, but create it now to reduce the amount of time between file open and size check
    char buf[8];
    m_file.read(buf, 8);
    for (int i = 0; i < 8; ++i)
    {
        if (buf[i] != magic[i]) throw DataFileException("file has the wrong magic string");
    }
    m_file.read(m_dims, 2 * sizeof(int64_t));
    if (ByteOrderEnum::isSystemBigEndian())
    {
        ByteSwapping::swapBytes(m_dims, 2);
    }
    if (m_dims[0] < 1 || m_dims[1] < 1) throw DataFileException("both dimensions must be positive");
    m_indexArray.resize(m_dims[1] + 1);
    vector<int64_t> lengthArray(m_dims[1]);
    m_file.read(lengthArray.data(), m_dims[1] * sizeof(int64_t));
    if (ByteOrderEnum::isSystemBigEndian())
    {
        ByteSwapping::swapBytes(lengthArray.data(), m_dims[1]);
    }
    m_indexArray[0] = 0;
    for (int64_t i = 0; i < m_dims[1]; ++i)
    {
        if (lengthArray[i] > m_dims[0] || lengthArray[i] < 0) throw DataFileException("impossible value found in length array");
        m_indexArray[i + 1] = m_indexArray[i] + lengthArray[i];
    }
    m_valuesOffset = 8 + 2 * sizeof(int64_t) + m_dims[1] * sizeof(int64_t);
    int64_t xml_offset = m_valuesOffset + m_indexArray[m_dims[1]] * 2 * sizeof(int64_t);
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
    if (m_xml.getDimensionLength(CiftiXML::ALONG_ROW) != m_dims[0] || m_xml.getDimensionLength(CiftiXML::ALONG_COLUMN) != m_dims[1])
    {
        throw DataFileException("cifti XML doesn't match dimensions of sparse file");
    }
}

CaretSparseFile::~CaretSparseFile()
{
}

void CaretSparseFile::getRow(const int64_t& index, int64_t* rowOut)
{
    CaretAssert(index >= 0 && index < m_dims[1]);
    int64_t start = m_indexArray[index], end = m_indexArray[index + 1];
    int64_t numToRead = (end - start) * 2;
    m_scratchArray.resize(numToRead);
    m_file.seek(m_valuesOffset + start * sizeof(int64_t) * 2);
    m_file.read(m_scratchArray.data(), numToRead * sizeof(int64_t));
    if (ByteOrderEnum::isSystemBigEndian())
    {
        ByteSwapping::swapBytes(m_scratchArray.data(), numToRead);
    }
    int64_t curIndex = 0;
    for (int64_t i = 0; i < numToRead; i += 2)
    {
        int64_t index = m_scratchArray[i];
        if (index < curIndex || index >= m_dims[0]) throw DataFileException("impossible index value found in file");
        while (curIndex < index)
        {
            rowOut[curIndex] = 0;
            ++curIndex;
        }
        ++curIndex;
        rowOut[index] = m_scratchArray[i + 1];
    }
    while (curIndex < m_dims[0])
    {
        rowOut[curIndex] = 0;
        ++curIndex;
    }
}

void CaretSparseFile::getRowSparse(const int64_t& index, vector<int64_t>& indicesOut, vector<int64_t>& valuesOut)
{
    CaretAssert(index >= 0 && index < m_dims[1]);
    int64_t start = m_indexArray[index], end = m_indexArray[index + 1];
    int64_t numToRead = (end - start) * 2, numNonzero = end - start;
    m_scratchArray.resize(numToRead);
    m_file.seek(m_valuesOffset + start * sizeof(int64_t) * 2);
    m_file.read(m_scratchArray.data(), numToRead * sizeof(int64_t));
    if (ByteOrderEnum::isSystemBigEndian())
    {
        ByteSwapping::swapBytes(m_scratchArray.data(), numToRead);
    }
    indicesOut.resize(numNonzero);
    valuesOut.resize(numNonzero);
    int64_t lastIndex = -1;
    for (int64_t i = 0; i < numNonzero; ++i)
    {
        indicesOut[i] = m_scratchArray[i * 2];
        valuesOut[i] = m_scratchArray[i * 2 + 1];
        if (indicesOut[i] <= lastIndex || indicesOut[i] >= m_dims[0]) throw DataFileException("impossible index value found in file");
        lastIndex = indicesOut[i];
    }
}

void CaretSparseFile::getFibersRow(const int64_t& index, FiberFractions* rowOut)
{
    if (m_scratchRow.size() != (size_t)m_dims[0]) m_scratchRow.resize(m_dims[0]);
    getRow(index, (int64_t*)m_scratchRow.data());
    for (int64_t i = 0; i < m_dims[0]; ++i)
    {
        if (m_scratchRow[i] == 0)
        {
            rowOut[i].zero();
        } else {
             decodeFibers(m_scratchRow[i], rowOut[i]);
        }
    }
}

void CaretSparseFile::getFibersRowSparse(const int64_t& index, vector<int64_t>& indicesOut, vector<FiberFractions>& valuesOut)
{
    getRowSparse(index, indicesOut, m_scratchSparseRow);
    size_t numNonzero = m_scratchSparseRow.size();
    valuesOut.resize(numNonzero);
    for (size_t i = 0; i < numNonzero; ++i)
    {
        decodeFibers(((uint64_t*)m_scratchSparseRow.data())[i], valuesOut[i]);
    }
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
        throw DataFileException("error decoding value '" + AString::number(coded) + "' from workbench sparse trajectory file");
    }
    if (decoded.fiberFractions[2] < 0.0f) decoded.fiberFractions[2] = 0.0f;
}

void FiberFractions::zero()
{
    totalCount = 0;
    fiberFractions.clear();
    distance = 0.0f;
}

CaretSparseFileWriter::CaretSparseFileWriter(const AString& fileName, const CiftiXML& xml)
{
    if (!fileName.endsWith(".trajTEMP.wbsparse"))
    {//for now (and maybe forever), this format is single-purpose
        CaretLogWarning("sparse trajectory file '" + fileName + "' should be saved ending in .trajTEMP.wbsparse");
    }
    m_finished = false;
    int64_t dimensions[2] = { xml.getDimensionLength(CiftiXML::ALONG_ROW), xml.getDimensionLength(CiftiXML::ALONG_COLUMN) };
    if (dimensions[0] < 1 || dimensions[1] < 1) throw DataFileException("both dimensions must be positive");
    m_xml = xml;
    m_dims[0] = dimensions[0];//CiftiXML doesn't support 3 dimensions yet, so we do this
    m_dims[1] = dimensions[1];
    if (fileName.endsWith(".gz"))
    {
        throw DataFileException("wbsparse files cannot be written compressed");
    }//because after we finish writing the data, we have to come back and write the lengths array
    m_file.open(fileName, CaretBinaryFile::WRITE_TRUNCATE);
    m_file.write(magic, 8);
    int64_t tempdims[2] = { m_dims[0], m_dims[1] };
    if (ByteOrderEnum::isSystemBigEndian())
    {
        ByteSwapping::swapBytes(tempdims, 2);
    }
    m_file.write(tempdims, 2 * sizeof(int64_t));
    //write dummy placeholder bytes for currently-unknown length array
    m_lengthArray.resize(m_dims[1], 0);//initialize the memory so that valgrind won't complain
    m_file.write(m_lengthArray.data(), m_dims[1] * sizeof(uint64_t));//write it to get the file to the correct length
    m_nextRowIndex = 0;
    m_valuesOffset = 8 + 2 * sizeof(int64_t) + m_dims[1] * sizeof(int64_t);
}

void CaretSparseFileWriter::writeRow(const int64_t& index, const int64_t* row)
{
    CaretAssert(index < m_dims[1]);
    CaretAssert(index >= m_nextRowIndex);
    while (m_nextRowIndex < index)
    {
        m_lengthArray[m_nextRowIndex] = 0;
        ++m_nextRowIndex;
    }
    m_scratchArray.clear();
    int64_t count = 0;
    for (int64_t i = 0; i < m_dims[0]; ++i)
    {
        if (row[i] != 0)
        {
            m_scratchArray.push_back(i);
            m_scratchArray.push_back(row[i]);
            ++count;
        }
    }
    m_lengthArray[index] = count;
    if (ByteOrderEnum::isSystemBigEndian())
    {
        ByteSwapping::swapBytes(m_scratchArray.data(), m_scratchArray.size());
    }
    m_file.write(m_scratchArray.data(), m_scratchArray.size() * sizeof(int64_t));
    m_nextRowIndex = index + 1;
    if (m_nextRowIndex == m_dims[1]) finish();
}

void CaretSparseFileWriter::writeRowSparse(const int64_t& index, const vector<int64_t>& indices, const vector<int64_t>& values)
{
    CaretAssert(index < m_dims[1]);
    CaretAssert(index >= m_nextRowIndex);
    CaretAssert(indices.size() == values.size());
    while (m_nextRowIndex < index)
    {
        m_lengthArray[m_nextRowIndex] = 0;
        ++m_nextRowIndex;
    }
    m_scratchArray.clear();
    size_t numNonzero = indices.size();//assume no zeros
    m_lengthArray[index] = numNonzero;
    int64_t lastIndex = -1;
    for (size_t i = 0; i < numNonzero; ++i)
    {
        if (indices[i] <= lastIndex || indices[i] >= m_dims[0]) throw DataFileException("indices must be sorted when writing sparse rows");
        lastIndex = indices[i];
        m_scratchArray.push_back(indices[i]);
        m_scratchArray.push_back(values[i]);
    }
    if (ByteOrderEnum::isSystemBigEndian())
    {
        ByteSwapping::swapBytes(m_scratchArray.data(), m_scratchArray.size());
    }
    m_file.write(m_scratchArray.data(), m_scratchArray.size() * sizeof(int64_t));
    m_nextRowIndex = index + 1;
    if (m_nextRowIndex == m_dims[1]) finish();
}

void CaretSparseFileWriter::writeFibersRow(const int64_t& index, const FiberFractions* row)
{
    if (m_scratchRow.size() != (size_t)m_dims[0]) m_scratchRow.resize(m_dims[0]);
    for (int64_t i = 0; i < m_dims[0]; ++i)
    {
        if (row[i].totalCount == 0)
        {
            m_scratchRow[i] = 0;
        } else {
            encodeFibers(row[i], m_scratchRow[i]);
        }
    }
    writeRow(index, (int64_t*)m_scratchRow.data());
}

void CaretSparseFileWriter::writeFibersRowSparse(const int64_t& index, const vector<int64_t>& indices, const vector<FiberFractions>& values)
{
    size_t numNonzero = values.size();//assume no zeros
    m_scratchSparseRow.resize(numNonzero);
    for (size_t i = 0; i < numNonzero; ++i)
    {
        encodeFibers(values[i], ((uint64_t*)m_scratchSparseRow.data())[i]);
    }
    writeRowSparse(index, indices, m_scratchSparseRow);
}

void CaretSparseFileWriter::finish()
{
    if (m_finished) return;
    m_finished = true;
    while (m_nextRowIndex < m_dims[1])
    {
        m_lengthArray[m_nextRowIndex] = 0;
        ++m_nextRowIndex;
    }
    QByteArray myXMLBytes = m_xml.writeXMLToQByteArray();
    m_file.write(myXMLBytes.constData(), myXMLBytes.size());
    m_file.seek(8 + 2 * sizeof(int64_t));
    if (ByteOrderEnum::isSystemBigEndian())
    {
        ByteSwapping::swapBytes(m_lengthArray.data(), m_lengthArray.size());
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
