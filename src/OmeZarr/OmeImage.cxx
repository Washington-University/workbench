
/*LICENSE_START*/
/*
 *  Copyright (C) 2024 Washington University School of Medicine
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

#define __OME_IMAGE_DECLARE__
#include "OmeImage.h"
#undef __OME_IMAGE_DECLARE__

#include <QImage>
#include <QImageWriter>

#include "CaretAssert.h"
#include "OmeDimensionIndices.h"

using namespace caret;


    
/**
 * \class caret::OmeImage
 * \brief Simplifies access to a ZARR image read from OME-ZARR file
 * \ingroup OmeZarr
 *
 * Z5 reads data into an xt::xarray.  This class wraps this array and provides
 * methods to simplify getting elements using x (i), y (j), z (k), time, and
 * channel (rgba) indices.
 */

/**
 * Constructor.
 * @param dataArray
 *    Xarray containing the data.  This instance will take ownership of this data
 *    and delete it when this instance is deleted.
 * @param dimensionIndices
 *    The dimension indices
 */
OmeImage::OmeImage(xt::xarray<uint8_t>* dataArray,
                   const OmeDimensionIndices& dimensionIndices)
: CaretObject(),
m_dataArray(dataArray),
m_xDimensionIndex(dimensionIndices.getIndexForX()),
m_yDimensionIndex(dimensionIndices.getIndexForY()),
m_zDimensionIndex(dimensionIndices.getIndexForZ()),
m_timeDimensionIndex(dimensionIndices.getIndexForTime()),
m_channelDimensionIndex(dimensionIndices.getIndexForChannel())
{
    m_numberOfValidDimensionIndices = 0;
    if (m_xDimensionIndex >= 0) {
        ++m_numberOfValidDimensionIndices;
    }
    if (m_yDimensionIndex >= 0) {
        ++m_numberOfValidDimensionIndices;
    }
    if (m_zDimensionIndex >= 0) {
        ++m_numberOfValidDimensionIndices;
    }
    if (m_timeDimensionIndex >= 0) {
        ++m_numberOfValidDimensionIndices;
    }
    if (m_channelDimensionIndex >= 0) {
        ++m_numberOfValidDimensionIndices;
    }
    
    if (m_dataArray) {
        const int64_t numDims(m_dataArray->dimension());
        std::vector<int64_t> dims;
        for (int32_t i = 0; i < numDims; i++) {
            dims.push_back(m_dataArray->shape(i));
        }
        
        int64_t numX(-1);
        if (m_xDimensionIndex >= 0) {
            CaretAssertVectorIndex(dims, m_xDimensionIndex);
            numX = dims[m_xDimensionIndex];
        }
        int64_t numY(-1);
        if (m_yDimensionIndex >= 0) {
            CaretAssertVectorIndex(dims, m_yDimensionIndex);
            numY = dims[m_yDimensionIndex];
        }
        int64_t numZ(-1);
        if (m_zDimensionIndex >= 0) {
            CaretAssertVectorIndex(dims, m_zDimensionIndex);
            numZ = dims[m_zDimensionIndex];
        }
        int64_t numTime(-1);
        if (m_timeDimensionIndex >= 0) {
            CaretAssertVectorIndex(dims, m_timeDimensionIndex);
            numTime = dims[m_timeDimensionIndex];
        }
        int64_t numChannels(-1);
        if (m_channelDimensionIndex >= 0) {
            CaretAssertVectorIndex(dims, m_channelDimensionIndex);
            numChannels = dims[m_channelDimensionIndex];
        }
     
        m_dimensionSizes = OmeDimensionSizes(numX,
                                             numY,
                                             numZ,
                                             numTime,
                                             numChannels);
    }
}

/**
 * Destructor.
 */
OmeImage::~OmeImage()
{
}

/**
 * @return True if the image data is valid
 */
bool
OmeImage::isValid() const
{
    return ((m_dataArray.get() != NULL)
            && (m_numberOfValidDimensionIndices > 0));
}

/**
 * @return Element at the give indices
 * @param xIndex
 *    X-index into data
 * @param yIndex
 *    Y-index into data
 * @param zIndex
 *    Z-index into data
 * @param timeIndex
 *    Time-index into data
 * @param channelIndex
 *    Channel-index into data
 */
uint8_t
OmeImage::getElement(const int64_t xIndex,
                         const int64_t yIndex,
                         const int64_t zIndex,
                         const int64_t timeIndex,
                         const int64_t channelIndex) const
{
    uint8_t value(0);
    
    std::vector<uint64_t> dataIndices;
    if (elementIndicesToDataIndices(xIndex, 
                                    yIndex,
                                    zIndex,
                                    timeIndex,
                                    channelIndex,
                                    dataIndices)) {
        switch (dataIndices.size()) {
            case 1:
                value = m_dataArray->at(dataIndices[0]);
                break;
            case 2:
                value = m_dataArray->at(dataIndices[0],
                                        dataIndices[1]);
                break;
            case 3:
                value = m_dataArray->at(dataIndices[0],
                                        dataIndices[1],
                                        dataIndices[2]);
                break;
            case 4:
                value = m_dataArray->at(dataIndices[0],
                                        dataIndices[1],
                                        dataIndices[2],
                                        dataIndices[3]);
                break;
            case 5:
                value = m_dataArray->at(dataIndices[0],
                                        dataIndices[1],
                                        dataIndices[2],
                                        dataIndices[3],
                                        dataIndices[4]);
                break;
        }
    }
    
    return value;
}

/**
 * Convert element indices into xarray data indices
 * @param xIndex
 *    X-index into data
 * @param yIndex
 *    Y-index into data
 * @param zIndex
 *    Z-index into data
 * @param timeIndex
 *    Time-index into data
 * @param channelIndex
 *    Channel-index into data
 * @param dataIndicesOut
 *    Output vector containing indices for xarray data
 */
bool 
OmeImage::elementIndicesToDataIndices(const int64_t xIndex,
                                          const int64_t yIndex,
                                          const int64_t zIndex,
                                          const int64_t timeIndex,
                                          const int64_t channelIndex,
                                          std::vector<uint64_t>& dataIndicesOut) const
{
    if (m_numberOfValidDimensionIndices > 0) {
        dataIndicesOut.resize(m_numberOfValidDimensionIndices);
        
        if (m_xDimensionIndex >= 0) {
            CaretAssertVectorIndex(dataIndicesOut, m_xDimensionIndex);
            dataIndicesOut[m_xDimensionIndex] = xIndex;
        }
        if (m_yDimensionIndex >= 0) {
            CaretAssertVectorIndex(dataIndicesOut, m_yDimensionIndex);
            dataIndicesOut[m_yDimensionIndex] = yIndex;
        }
        if (m_zDimensionIndex >= 0) {
            CaretAssertVectorIndex(dataIndicesOut, m_zDimensionIndex);
            dataIndicesOut[m_zDimensionIndex] = zIndex;
        }
        if (m_timeDimensionIndex >= 0) {
            CaretAssertVectorIndex(dataIndicesOut, m_timeDimensionIndex);
            dataIndicesOut[m_timeDimensionIndex] = timeIndex;
        }
        if (m_channelDimensionIndex >= 0) {
            CaretAssertVectorIndex(dataIndicesOut, m_channelDimensionIndex);
            dataIndicesOut[m_channelDimensionIndex] = channelIndex;
        }

        return true;
    }
    
    return false;
}

/**
 * @return Number of dimensions
 */
int64_t 
OmeImage::getNumberOfDimensions() const
{
    if (m_dataArray) {
        return m_dataArray->dimension();
    }
    return 0;
}

/**
 * @return Reference to dimension sizes
 */
const OmeDimensionSizes&
OmeImage::getDimensionSizes() const
{
    return m_dimensionSizes;
}

/**
 * @return Data formatted for use as an OpenGL texture.
 * Caller MUST delete the returned array pointer
 */
FunctionResultValue<uint8_t*>
OmeImage::getDataForOpenGLTexture() const
{
    const int64_t numX(m_dimensionSizes.getSizeX());
    const int64_t numY(m_dimensionSizes.getSizeY());
    const int64_t numZ(m_dimensionSizes.getSizeZ());
    const int64_t numChannels(m_dimensionSizes.getSizeChannels());
    
    const int64_t numData(numX * numY * numZ * numChannels);
    uint8_t* rgba = new uint8_t[numData];
    
    const int64_t sizeXY(numX * numY);
    for (int64_t k = 0; k < numZ; k++) {
        for (int64_t i = 0; i < numX; i++) {
            for (int64_t j = 0; j < numY; j++) {
                for (int64_t c = 0; c < 4; c++) {
                    const int64_t offset((k * sizeXY * numChannels)
                                         + (j * numX * numChannels)
                                         + (i * numChannels)
                                         + c);
                    CaretAssert((offset >= 0)
                                && (offset < numData));
                    rgba[offset] = getElement(i, j, k, 0, c);
                }
            }
        }
    }
    
    return FunctionResultValue<uint8_t*>(rgba,
                                         "",
                                         true);
}

/**
 * Write this OME image as a QImage file
 * @param filename
 *    Name of file
 * @return
 *    Result from writing image file
 */
FunctionResult
OmeImage::writeAsQImage(const AString& filename) const
{
    if ( ! m_dataArray) {
        return FunctionResult::error("writeAsQImage: Image is invalid.");
    }
    const int64_t numDims(getNumberOfDimensions());
    if (numDims < 2) {
        return FunctionResult::error("writeAsQImage: Image is less than two dimensions.");
    }
    const int64_t numChannels(m_dimensionSizes.getSizeChannels());
    const int64_t numTime(m_dimensionSizes.getSizeTime());
    const int64_t numX(m_dimensionSizes.getSizeX());
    const int64_t numY(m_dimensionSizes.getSizeY());
    const int64_t numZ(m_dimensionSizes.getSizeZ());

    const int64_t numData(numX * numY * numZ * numChannels);
    
    QImage image(numX, numY, QImage::Format_RGBA8888);
    
    const int64_t sizeXY(numX * numY);
    const int64_t sizeXYZ(sizeXY * numZ);
    for (int64_t k = 0; k < numZ; k++) {
        for (int64_t i = 0; i < numX; i++) {
            for (int64_t j = 0; j < numY; j++) {
                int32_t rgba[4];
                for (int64_t c = 0; c < 4; c++) {
                    const int64_t offset(i
                                         + (j * numX)
                                         + (k * sizeXY)
                                         + (c * sizeXYZ));
                    CaretAssert((offset >= 0)
                                && (offset < numData));
                    rgba[c] = getElement(i, j, k, 0, c);
                }
                
                image.setPixelColor(i, numY - j - 1, QColor(rgba[0], rgba[1], rgba[2], rgba[3]));
            }
        }
        QImageWriter writer(filename);
        if ( ! writer.write(image)) {
            return FunctionResult::error("Failed writing image: "
                                         + writer.fileName()
                                         + " Error: "
                                         + writer.errorString());
        }
    }
    
    return FunctionResult::ok();
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
OmeImage::toString() const
{
    return "OmeZarrImage";
}

