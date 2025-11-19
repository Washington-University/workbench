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

#include "OxfordSparseThreeFile.h"
#include "ByteSwapping.h"
#include "CaretAssert.h"
#include "FileInformation.h"
#include <fstream>

using namespace caret;
using namespace std;

OxfordSparseThreeFile::OxfordSparseThreeFile(const AString& dimFileName, const AString& indexFileName, const AString& valueFileName)
{
    m_valueFile = NULL;
    fstream dimFile(dimFileName.toLocal8Bit().constData(), fstream::in);
    if (!dimFile) throw DataFileException("error opening dimensions file");
    dimFile >> m_dims[0];
    if (!dimFile) throw DataFileException("error reading dimensions from file");
    dimFile >> m_dims[1];
    if (!dimFile) throw DataFileException("error reading dimensions from file");
    if (m_dims[0] < 1 || m_dims[1] < 1) throw DataFileException("both dimensions must be positive");
    m_indexArray.resize(m_dims[1] + 1);
    vector<int64_t> lengthArray(m_dims[1]);
    FileInformation indexFileInfo(indexFileName);
    if (!indexFileInfo.exists()) throw DataFileException("index file doesn't exist");
    if (indexFileInfo.size() != 8 * m_dims[1]) throw DataFileException("index file is the wrong size");
    FILE* indexFile = fopen(indexFileName.toLocal8Bit().constData(), "rb");
    if (indexFile == NULL) throw DataFileException("error opening index file");
    if (fread(lengthArray.data(), sizeof(int64_t), m_dims[1], indexFile) != (size_t)m_dims[1]) throw DataFileException("error reading index file");
    if (ByteSwapping::isSystemBigEndian())
    {
        ByteSwapping::swapArray(lengthArray.data(), m_dims[1]);
    }
    m_indexArray[0] = 0;
    for (int64_t i = 0; i < m_dims[1]; ++i)
    {
        if (lengthArray[i] > m_dims[0] || lengthArray[i] < 0) throw DataFileException("impossible value found in length array");
        m_indexArray[i + 1] = m_indexArray[i] + lengthArray[i];
    }
    FileInformation valueFileInfo(valueFileName);
    if (!valueFileInfo.exists()) throw DataFileException("value file doesn't exist");
    if (valueFileInfo.size() != (int64_t)(2 * sizeof(uint64_t) * m_indexArray[m_dims[1]])) throw DataFileException("value file is the wrong size");
    m_valueFile = fopen(valueFileName.toLocal8Bit().constData(), "rb");
    if (m_valueFile == NULL) throw DataFileException("error opening value file");
}

OxfordSparseThreeFile::~OxfordSparseThreeFile()
{
    if (m_valueFile != NULL) fclose(m_valueFile);
}

void OxfordSparseThreeFile::getRow(const int64_t& index, int64_t* rowOut)
{
    CaretAssert(index >= 0 && index < m_dims[1]);
    int64_t start = m_indexArray[index], end = m_indexArray[index + 1];
    int64_t numToRead = (end - start) * 2;
    m_scratchArray.resize(numToRead);
    if (fseek(m_valueFile, start * sizeof(int64_t) * 2, SEEK_SET) != 0) throw DataFileException("failed to seek in value file");
    if (fread(m_scratchArray.data(), sizeof(int64_t), numToRead, m_valueFile) != (size_t)numToRead) throw DataFileException("error reading from value file");
    if (ByteSwapping::isSystemBigEndian())
    {
        ByteSwapping::swapArray(m_scratchArray.data(), numToRead);
    }
    int64_t curIndex = 0;
    for (int64_t i = 0; i < numToRead; i += 2)
    {
        int64_t index = m_scratchArray[i];
        if (index < curIndex || index >= m_dims[0]) throw DataFileException("impossible index value found in value file");
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

void OxfordSparseThreeFile::getRowSparse(const int64_t& index, vector<int64_t>& indicesOut, vector<int64_t>& valuesOut)
{
    CaretAssert(index >= 0 && index < m_dims[1]);
    int64_t start = m_indexArray[index], end = m_indexArray[index + 1];
    int64_t numToRead = (end - start) * 2, numNonzero = end - start;
    m_scratchArray.resize(numToRead);
    if (fseek(m_valueFile, start * sizeof(int64_t) * 2, SEEK_SET) != 0) throw DataFileException("failed to seek in value file");
    if (fread(m_scratchArray.data(), sizeof(int64_t), numToRead, m_valueFile) != (size_t)numToRead) throw DataFileException("error reading from value file");
    if (ByteSwapping::isSystemBigEndian())
    {
        ByteSwapping::swapArray(m_scratchArray.data(), numToRead);
    }
    indicesOut.resize(numNonzero);
    valuesOut.resize(numNonzero);
    int64_t lastIndex = -1;
    for (int64_t i = 0; i < numNonzero; ++i)
    {
        indicesOut[i] = m_scratchArray[i * 2];
        valuesOut[i] = m_scratchArray[i * 2 + 1];
        if (indicesOut[i] <= lastIndex || indicesOut[i] >= m_dims[0])
        {
            throw DataFileException("impossible index value found in file");
        }
        lastIndex = indicesOut[i];
    }
}

void OxfordSparseThreeFile::getFibersRow(const int64_t& index, FiberFractions* rowOut)
{
    if (m_scratchRow.size() != (size_t)m_dims[0]) m_scratchRow.resize(m_dims[0]);
    getRow(index, (int64_t*)m_scratchRow.data());
    for (int64_t i = 0; i < m_dims[0]; ++i)
    {
        if (m_scratchRow[i] == 0)
        {
            rowOut[i].clear();
        } else {
            decodeFibers(m_scratchRow[i], rowOut[i]);
        }
    }
}

void OxfordSparseThreeFile::getFibersRowSparse(const int64_t& index, vector<int64_t>& indicesOut, vector<FiberFractions>& valuesOut)
{
    getRowSparse(index, indicesOut, m_scratchSparseRow);
    size_t numNonzero = m_scratchSparseRow.size();
    valuesOut.resize(numNonzero);
    for (size_t i = 0; i < numNonzero; ++i)
    {
        decodeFibers(((uint64_t*)m_scratchSparseRow.data())[i], valuesOut[i]);
    }
}

void OxfordSparseThreeFile::decodeFibers(const uint64_t& coded, FiberFractions& decoded)
{
    decoded.fiberFractions.resize(3);
    decoded.totalCount = coded>>32;
    uint32_t temp = coded & ((1LL<<32) - 1);
    decoded.distance = (temp % 1001);
    temp = temp / 1001;
    decoded.fiberFractions[1] = (temp % 1001) / 1000.0f;
    temp = temp / 1001;
    decoded.fiberFractions[0] = temp / 1000.0f;
    decoded.fiberFractions[2] = 1.0f - decoded.fiberFractions[0] - decoded.fiberFractions[1];
    if (decoded.fiberFractions[2] < -0.002f || temp > 1000)
    {
        throw DataFileException("error decoding value '" + AString::number(coded) + "' from oxford 3-file");
    }
    if (decoded.fiberFractions[2] < 0.0f) decoded.fiberFractions[2] = 0.0f;
}
