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

#include "CaretAssert.h"

#include "DataFileException.h"
#include "DataFileTypeEnum.h"
#include "GiftiFile.h"
#include "MathFunctions.h"
#include "RgbaFile.h"

using namespace caret;

/**
 * Constructor.
 */
RgbaFile::RgbaFile()
: GiftiTypeFile(DataFileTypeEnum::RGBA)
{
    this->initializeMembersRgbaFile();
}

/**
 * Copy constructor.
 *
 * @param sf
 *     Surface file that is copied.
 */
RgbaFile::RgbaFile(const RgbaFile& sf)
: GiftiTypeFile(sf)
{
    this->copyHelperRgbaFile(sf);
}


/**
 * Assignment operator.
 *
 * @param sf
 *     Surface file that is copied.
 * @return
 *     This surface file with content replaced
 *     by the RgbaFile parameter.
 */
RgbaFile& 
RgbaFile::operator=(const RgbaFile& sf)
{
    if (this != &sf) {
        GiftiTypeFile::operator=(sf);
        this->copyHelperRgbaFile(sf);
    }
    return *this;
}

/**
 * Destructor.
 */
RgbaFile::~RgbaFile()
{
    
}

/**
 * Clear the surface file.
 */
void 
RgbaFile::clear()
{
    GiftiTypeFile::clear();
}

/**
 * Validate the contents of the file after it
 * has been read such as correct number of 
 * data arrays and proper data types/dimensions.
 */
void 
RgbaFile::validateDataArraysAfterReading()
{
    this->initializeMembersRgbaFile();
    
    const int32_t numberOfDataArrays = this->giftiFile->getNumberOfDataArrays();
    if (numberOfDataArrays != 1) {
        throw DataFileException(getFileName(),
                                "GIFTI RGBA files must have one GIFTI Data Array");
    }
    
    const GiftiDataArray* gda = this->giftiFile->getDataArray(0);
    if (gda->getNumberOfRows() <= 0) {
        throw DataFileException(getFileName(),
                                "GIFTI RGBA file is empty (number of rows is zero)");
    }

    m_numberOfComponents = gda->getNumberOfComponents();
    if ((m_numberOfComponents < 3)
        || (m_numberOfComponents > 4)) {
        throw DataFileException(getFileName(),
                                "GIFTI RGBA file must have four components but contains "
                                + AString::number(m_numberOfComponents)
                                + " components.");
    }
    if (gda->getDataType() != NiftiDataTypeEnum::NIFTI_TYPE_FLOAT32) {
        throw DataFileException("GIFTI RGBA files must be FLOAT data type.  "
                                "Type of data array is: "
                                + NiftiDataTypeEnum::toName(gda->getDataType()));
    }
    
    this->verifyDataArraysHaveSameNumberOfRows(3, 4);
    
    m_dataArray          = this->giftiFile->getDataArray(0);
    m_numberOfVertices   = gda->getNumberOfRows();
    
    bool rescaleZeroToOneFlag = false;
    const int32_t numValues = m_numberOfComponents * m_numberOfVertices;
    float* floatData = m_dataArray->getDataPointerFloat();
    for (int32_t i = 0; i < numValues; i++) {
        if (floatData[i] > 1.1f) {
            rescaleZeroToOneFlag = true;
            break;
        }
    }

    if (rescaleZeroToOneFlag) {
        for (int32_t i = 0; i < numValues; i++) {
            floatData[i] /= 255.0;
        }
    }
    
}

/**
 * Get the number of nodes.
 *
 * @return
 *    The number of nodes.
 */
int32_t
RgbaFile::getNumberOfNodes() const
{
    return m_numberOfVertices;
}

/**
 * Get the number of columns.
 *
 * @return
 *   The number of columns.
 */
int32_t
RgbaFile::getNumberOfColumns() const
{
    const int32_t numCols = this->giftiFile->getNumberOfDataArrays();
    return numCols;
}


/**
 * Initialize members of this class.
 */
void 
RgbaFile::initializeMembersRgbaFile()
{
    m_dataArray = 0;
    m_numberOfVertices   = 0;
    m_numberOfComponents = 0;
}

/**
 * Helps copying files.
 *
 * @param sf
 *    File that is copied.
 */
void 
RgbaFile::copyHelperRgbaFile(const RgbaFile& /*sf*/)
{
    this->validateDataArraysAfterReading();
}

/**
 * Get the RGBA data for the given vertex.
 *
 * @param vertexIndex
 *     Index of the vertex.
 * @param rgbaOut
 *     Output containing RGBA values for vertex.
 */
void
RgbaFile::getVertexRGBA(const int32_t vertexIndex,
                        float* rgbaOut) const
{
    CaretAssert((vertexIndex >= 0)
                && (vertexIndex < m_numberOfVertices));
    
    const float* floatData = m_dataArray->getDataPointerFloat();
    
    if (m_numberOfComponents == 4) {
        const int32_t v4 = vertexIndex * 4;
        CaretAssertArrayIndex(floatData, m_numberOfVertices * m_numberOfComponents, v4 + 3);
        rgbaOut[0] = floatData[v4];
        rgbaOut[1] = floatData[v4+1];
        rgbaOut[2] = floatData[v4+2];
        rgbaOut[3] = floatData[v4+3];
    }
    else if (m_numberOfComponents == 3) {
        const int32_t v3 = vertexIndex * 3;
        CaretAssertArrayIndex(floatData, m_numberOfVertices * m_numberOfComponents, v3 + 2);
        rgbaOut[0] = floatData[v3];
        rgbaOut[1] = floatData[v3+1];
        rgbaOut[2] = floatData[v3+2];
        rgbaOut[3] = 1.0f;
    }
    else {
        CaretAssert(0);
    }
}



