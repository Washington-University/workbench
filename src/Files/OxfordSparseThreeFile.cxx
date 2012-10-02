/*LICENSE_START*/
/*
 *  Copyright 1995-2011 Washington University School of Medicine
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

#include "OxfordSparseThreeFile.h"
#include "ByteOrderEnum.h"
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

void OxfordSparseThreeFile::getRow(int64_t* rowOut, const int64_t& index)
{
    CaretAssert(index >= 0 && index < m_dims[1]);
    int64_t start = m_indexArray[index], end = m_indexArray[index + 1];
    int64_t numToRead = (end - start) * 2;
    m_scratchArray.resize(numToRead);
    if (fseek(m_valueFile, start * sizeof(int64_t) * 2, SEEK_SET) != 0) throw DataFileException("failed to seek in value file");
    if (fread(m_scratchArray.data(), sizeof(int64_t), numToRead, m_valueFile) != (size_t)numToRead) throw DataFileException("error reading from value file");
    if (ByteOrderEnum::isSystemBigEndian())
    {
        ByteSwapping::swapBytes(m_scratchArray.data(), numToRead);
    }
    int64_t curIndex = 0;
    for (int64_t i = 0; i < numToRead; i += 2)
    {
        int64_t index = m_scratchArray[i];
        if (index < 0 || index >= m_dims[0]) throw DataFileException("impossible index value found in value file");
        while (curIndex < index)
        {
            rowOut[curIndex] = 0;
            ++curIndex;
        }
        rowOut[index] = m_scratchArray[i + 1];
    }
    while (curIndex < m_dims[0])
    {
        rowOut[curIndex] = 0;
        ++curIndex;
    }
}

void OxfordSparseThreeFile::getFibersRow(FiberFractions* rowOut, const int64_t& index)
{
    if (m_scratchRow.size() != (size_t)m_dims[0]) m_scratchRow.resize(m_dims[0]);
    getRow((int64_t*)m_scratchRow.data(), index);
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
    if (decoded.fiberFractions[2] < -0.001f || temp > 1000) throw DataFileException("error decoding value '" + AString::number(coded) + "' from oxford 3-file");
}
