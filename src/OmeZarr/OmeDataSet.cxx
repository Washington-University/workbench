
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

#define __OME_DATA_SET_DECLARE__
#include "OmeDataSet.h"
#undef __OME_DATA_SET_DECLARE__

#include "CaretAssert.h"
#include "ZarrImageReader.h"
using namespace caret;


    
/**
 * \class caret::OmeDataSet 
 * \brief Models an OME "datasets" element
 * \ingroup OmeZarr
 */

/**
 * Constructor.
 */
OmeDataSet::OmeDataSet()
: CaretObject()
{
    
}

/**
 * Destructor.
 */
OmeDataSet::~OmeDataSet()
{
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
OmeDataSet::OmeDataSet(const OmeDataSet& obj)
: CaretObject(obj)
{
    this->copyHelperOmeDataSet(obj);
}

/**
 * Assignment operator.
 * @param obj
 *    Data copied from obj to this.
 * @return 
 *    Reference to this object.
 */
OmeDataSet&
OmeDataSet::operator=(const OmeDataSet& obj)
{
    if (this != &obj) {
        CaretObject::operator=(obj);
        this->copyHelperOmeDataSet(obj);
    }
    return *this;    
}

/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void 
OmeDataSet::copyHelperOmeDataSet(const OmeDataSet& obj)
{
    m_path         = obj.m_path;
    m_scaling      = obj.m_scaling;
    m_translation  = obj.m_translation;
    m_zarrDataType = obj.m_zarrDataType;
    m_dimensions   = obj.m_dimensions;
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
OmeDataSet::toString() const
{
    AString txt("OmeDataSet:");
    txt.appendWithNewLine("   Path: " + m_path);
    txt.appendWithNewLine("   Scaling: " + AString::fromNumbers(m_scaling));
    txt.appendWithNewLine("   Translation: " + AString::fromNumbers(m_translation));
    txt.appendWithNewLine("   ZARR data type: " + ZarrDataTypeEnum::toGuiName(m_zarrDataType));
    txt.appendWithNewLine("   Dimensions: " + AString::fromNumbers(m_dimensions));
    return txt;
}


/**
 * @return The subdirectory path
 */
AString
OmeDataSet::getPath() const
{
    return m_path;
}

/**
 * Set The subdirectory path
 *
 * @param path
 *    New value for The subdirectory path
 */
void
OmeDataSet::setPath(const AString& path)
{
    m_path = path;
}

/**
 * @return scaling
 */
std::vector<float>
OmeDataSet::getScaling() const
{
    return m_scaling;
}

/**
 * Set scaling
 *
 * @param scaling
 *    New value for scaling
 */
void
OmeDataSet::setScaling(const std::vector<float>& scaling)
{
    m_scaling = scaling;
}

/**
 * @return translation
 */
std::vector<float>
OmeDataSet::getTranslation() const
{
    return m_translation;
}

/**
 * Set translation
 *
 * @param translation
 *    New value for translation
 */
void
OmeDataSet::setTranslation(const std::vector<float>& translation)
{
    m_translation = translation;
}

/**
 * @return the ZARR data type
 */
ZarrDataTypeEnum::Enum 
OmeDataSet::getZarrDataType() const
{
    return m_zarrDataType;
}

/**
 * Set the ZARR data type
 *
 * @param zarrDataType
 *    New value for the ZARR data type
 */
void
OmeDataSet::setZarrDataType(const ZarrDataTypeEnum::Enum zarrDataType)
{
    m_zarrDataType = zarrDataType;
}

/**
 * @return The dimensions
 */
std::vector<int64_t>
OmeDataSet::getDimensions() const
{
    return m_dimensions;
}

/**
 * Set the dimensions
 *
 * @param dimensions
 *    New value for the dimensions
 */
void
OmeDataSet::setDimensions(const std::vector<int64_t>& dimensions)
{
    m_dimensions = dimensions;
}

/**
 * Set the ZARR image reader for reading this data set
 * @param zarrImageReader
 *    The ZARR image reader
 */
void
OmeDataSet::setZarrImageReader(ZarrImageReader* zarrImageReader)
{
    m_zarrImageReader.reset(zarrImageReader);
}

/**
 * Set the indices for each of the dimensions
 * @param dimensionIndexX
 *    Dimension of X (width)
 * @param dimensionIndexY
 *    Dimension of Y (height)
 * @param dimensionIndexZ
 *    Dimension of Z (number of slices)
 * @param dimensionIndexTime
 *    Dimension of time
 * @param dimensionIndexChannel
 *    Dimension of channels
 */
void
OmeDataSet::setDimensionIndices(const int32_t dimensionIndexX,
                                const int32_t dimensionIndexY,
                                const int32_t dimensionIndexZ,
                                const int32_t dimensionIndexTime,
                                const int32_t dimensionIndexChannel)
{
    m_dimensionIndexX       = dimensionIndexX;
    m_dimensionIndexY       = dimensionIndexY;
    m_dimensionIndexZ       = dimensionIndexZ;
    m_dimensionIndexTime    = dimensionIndexTime;
    m_dimensionIndexChannel = dimensionIndexChannel;
}

/**
 * @return Width of the data image or -1 if unknown
 */
int64_t
OmeDataSet::getWidth() const
{
    if ((m_dimensionIndexX >= 0)
        && (m_dimensionIndexX < static_cast<int32_t>(m_dimensions.size()))) {
        return m_dimensions[m_dimensionIndexX];
    }
    return -1;
}

/**
 * @return Height of the data image or -1 if unknown
 */
int64_t
OmeDataSet::getHeight() const
{
    if ((m_dimensionIndexY >= 0)
        && (m_dimensionIndexY < static_cast<int32_t>(m_dimensions.size()))) {
        return m_dimensions[m_dimensionIndexY];
    }
    return -1;
}

/**
 * @return Number of slices in data image or -1 if unknown
 */
int64_t
OmeDataSet::getNumberOfSlices() const
{
    if ((m_dimensionIndexZ >= 0)
        && (m_dimensionIndexZ < static_cast<int32_t>(m_dimensions.size()))) {
        return m_dimensions[m_dimensionIndexZ];
    }
    return -1;
}

/**
 * @return Number of time points in the data image or -1 if unknown
 */
int64_t
OmeDataSet::getNumberOfTimePoints() const
{
    if ((m_dimensionIndexTime >= 0)
        && (m_dimensionIndexX < static_cast<int32_t>(m_dimensions.size()))) {
        return m_dimensions[m_dimensionIndexTime];
    }
    return -1;
}

/**
 * @return Number of channels in the data image or -1 if unknown
 */
int64_t
OmeDataSet::getNumberOfChannels() const
{
    if ((m_dimensionIndexChannel >= 0)
        && (m_dimensionIndexChannel < static_cast<int32_t>(m_dimensions.size()))) {
        return m_dimensions[m_dimensionIndexChannel];
    }
    return -1;
}

/**
 * Read the given data set from the ZARR file for display in an image.
 * @param sliceIndex
 *    Index of the slice.
 */
FunctionResultValue<unsigned char*>
OmeDataSet::readDataSetForImage(const int64_t sliceIndex) const
{
    FunctionResultValue<unsigned char*> imageResult(readDataSet(sliceIndex));
    if (imageResult.isError()) {
        return imageResult;
    }
    const unsigned char* data(imageResult.getValue());
    CaretAssert(data);
    
    const std::vector<int64_t> dims(getDimensions());
    const int64_t numX(dims[m_dimensionIndexX]);
    const int64_t numY(dims[m_dimensionIndexY]);
    const int64_t sizeXY(numX * numY);
    const int64_t numSlices(1);
    const int64_t sizeXYZ(sizeXY * numSlices);
    CaretAssert(getNumberOfChannels() == 4);
    const int64_t numData(sizeXYZ * getNumberOfChannels());
    
    unsigned char* rgba(new unsigned char[numData]);
    
    const bool flipJFlag(true);
    const int64_t k(0);
    for (int64_t i = 0; i < numX; i++) {
        for (int64_t j = 0; j < numY; j++) {
            for (int64_t c = 0; c < 4; c++) {
                const int64_t offset(i
                                     + (j * numX)
                                     + (k * sizeXY)
                                     + (c * sizeXYZ));
                CaretAssert((offset >= 0)
                            && (offset < numData));
                
                const int64_t rowIndex((flipJFlag
                                        ? j
                                        : (numY - j - 1)));
                const int64_t pixelIndex(((rowIndex * numX)
                                          + i) * 4);
                rgba[pixelIndex + c] = data[offset];
            }
        }
    }

    return FunctionResultValue<unsigned char*>(rgba,
                                               "",
                                               true);
}

/**
 * Read the given data set from the ZARR file.
 * @param sliceIndex
 *    Index of the slice.
 */
FunctionResultValue<unsigned char*>
OmeDataSet::readDataSet(const int64_t sliceIndex) const
{
    if ((sliceIndex < 0)
        || (sliceIndex >= getNumberOfSlices())) {
        return FunctionResultValue<unsigned char*>(NULL,
                                                   ("Invalid slice index=" + QString::number(sliceIndex)),
                                                   false);
    }
    if ( ! m_zarrImageReader) {
        return FunctionResultValue<unsigned char*>(NULL,
                                                   ("ZarrImageReader in OmeDataSet is invalid."),
                                                   false);
    }

    /*
     * Loads one Z-Slice
     */
    const std::vector<int64_t> dims(getDimensions());
    const int32_t numDims(dims.size());
    std::vector<int64_t> dimOffset(numDims, 0);
    std::vector<int64_t> dimLengths(numDims, 0);
    dimLengths[m_dimensionIndexX] = dims[m_dimensionIndexX];
    dimLengths[m_dimensionIndexY] = dims[m_dimensionIndexY];
    dimLengths[m_dimensionIndexZ] = dims[m_dimensionIndexZ];
    dimLengths[m_dimensionIndexChannel] = dims[m_dimensionIndexChannel];
    
    dimOffset[m_dimensionIndexZ]  = sliceIndex;
    dimLengths[m_dimensionIndexZ] = 1;
    
    
    FunctionResultValue<unsigned char*> readResult(m_zarrImageReader->readData(dimOffset,
                                                                               dimLengths,
                                                                               ZarrDataTypeEnum::UINT_8));
    return readResult;
}




