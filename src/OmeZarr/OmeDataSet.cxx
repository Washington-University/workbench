
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
 *
 * An OME-ZARR file may contain multiple datasets where
 * each dataset is a different resolution (dimensions).  Each
 * of these datasets is also in the .zattrs file in located in the
 * top level directory.  Each of these datasets are in
 * subdirectories 0, 1, 2, etc.
 *
 * https://ngff.openmicroscopy.org/0.4/index.html#multiscale-md
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
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
OmeDataSet::toString() const
{
    AString txt("OmeDataSet:");
    txt.appendWithNewLine("   Relative Path: " + m_relativePath);
    txt.appendWithNewLine("   ZARR data type: " + ZarrDataTypeEnum::toGuiName(m_zarrDataType));
    txt.appendWithNewLine("   Dimensions: " + AString::fromNumbers(m_dimensions));
    txt.appendWithNewLine("   Indices: " + m_dimensionIndices.toString());
    for (const auto& oct : m_coordinateTransformations) {
        txt.appendWithNewLine("   Coord Transform: " + oct.toString());
    }
    return txt;
}

/**
 * @return The subdirectory path
 */
AString
OmeDataSet::getRelativePath() const
{
    return m_relativePath;
}

/**
 * Set The subdirectory path
 *
 * @param relativePath
 *    New value for The subdirectory path
 */
void
OmeDataSet::setRelativePath(const AString& relativePath)
{
    m_relativePath = relativePath;
}

/**
 * Number of coordinate transformations
 */
int32_t
OmeDataSet::getNumberOfCoordinateTransformations() const
{
    return m_coordinateTransformations.size();
}

/**
 * Add the coordinate transformation.
 * @param oct
 *    The coordinate transformation to add
 */
void
OmeDataSet::addCoordinateTransformation(const OmeCoordinateTransformations& oct)
{
    m_coordinateTransformations.push_back(oct);
}

/**
 * @return the coordinate transformation at the given index
 * @param index
 *    Index of coordinate transformation
 */
OmeCoordinateTransformations
OmeDataSet::getCoordinateTransfomation(const int32_t index) const
{
    CaretAssertVectorIndex(m_coordinateTransformations, index);
    return m_coordinateTransformations[index];
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
 * @return The dimensions
 */
std::vector<int64_t>
OmeDataSet::getDimensions() const
{
    return m_dimensions;
}

/**
 * Set the indices for each of the dimensions
 * @param dimensionIndices
 *    The dimension indices
 */
void
OmeDataSet::setDimensionIndices(const OmeDimensionIndices& dimensionIndices)
{
    m_dimensionIndices = dimensionIndices;
}

/**
 * @return Width of the data image or -1 if unknown
 */
int64_t
OmeDataSet::getWidth() const
{
    const int64_t index(m_dimensionIndices.getIndexForX());
    if ((index >= 0)
        && (index < static_cast<int32_t>(m_dimensions.size()))) {
        return m_dimensions[index];
    }
    return -1;
}

/**
 * @return Height of the data image or -1 if unknown
 */
int64_t
OmeDataSet::getHeight() const
{
    const int64_t index(m_dimensionIndices.getIndexForY());
    if ((index >= 0)
        && (index < static_cast<int32_t>(m_dimensions.size()))) {
        return m_dimensions[index];
    }
    return -1;
}

/**
 * @return Number of slices in data image or -1 if unknown
 */
int64_t
OmeDataSet::getNumberOfSlices() const
{
    const int64_t index(m_dimensionIndices.getIndexForZ());
    if ((index >= 0)
        && (index < static_cast<int32_t>(m_dimensions.size()))) {
        return m_dimensions[index];
    }
    return -1;
}

/**
 * @return Number of time points in the data image or -1 if unknown
 */
int64_t
OmeDataSet::getNumberOfTimePoints() const
{
    const int64_t index(m_dimensionIndices.getIndexForTime());
    if ((index >= 0)
        && (index < static_cast<int32_t>(m_dimensions.size()))) {
        return m_dimensions[index];
    }
    return -1;
}

/**
 * @return Number of channels in the data image or -1 if unknown
 */
int64_t
OmeDataSet::getNumberOfChannels() const
{
    const int64_t index(m_dimensionIndices.getIndexForChannel());
    if ((index >= 0)
        && (index < static_cast<int32_t>(m_dimensions.size()))) {
        return m_dimensions[index];
    }
    return -1;
}

/**
 * Initialize this data set with a zarr image reader prior to reading data
 * @param driverType
 *    Type of ZARR driver
 * @param zarrPath
 *    Top level path (could be a directory, zip file, web address, etc.)
 * @return
 *    Result of initialization
 */
FunctionResult
OmeDataSet::initializeForReading(const ZarrDriverTypeEnum::Enum driverType,
                                 const AString& zarrPath)
{
    m_zarrImageReader.reset(new ZarrImageReader());
    const FunctionResult initReaderResult(m_zarrImageReader->initialize(driverType,
                                                                        zarrPath,
                                                                        m_relativePath));
    if (initReaderResult.isError()) {
        m_zarrImageReader.reset();
        return initReaderResult;
    }
    
    m_dimensions = m_zarrImageReader->getShapeSizes();
    m_zarrDataType = m_zarrImageReader->getDataType();
    
    return FunctionResult::ok();
}

/**
 * Read the given data set from the ZARR file for display in an image.
 * @param sliceIndex
 *    Index of the slice.
 */
FunctionResultValue<uint8_t*>
OmeDataSet::readDataSetForImage(const int64_t sliceIndex) const
{
    FunctionResultValue<OmeImage*> dataResult(readSlice(sliceIndex));
    if (dataResult.isError()) {
        return FunctionResultValue<uint8_t*>(NULL,
                                             dataResult.getErrorMessage(),
                                             false);
    }
    
    const std::unique_ptr<OmeImage> omeImage(dataResult.getValue());
    CaretAssert(omeImage);
    
    const std::vector<int64_t> dims(getDimensions());
    const int64_t numX(dims[m_dimensionIndices.getIndexForX()]);
    const int64_t numY(dims[m_dimensionIndices.getIndexForY()]);
    const int64_t sizeXY(numX * numY);
    const int64_t numSlices(1);
    const int64_t sizeXYZ(sizeXY * numSlices);
    CaretAssert(getNumberOfChannels() == 4);
    const int64_t numData(sizeXYZ * getNumberOfChannels());
    
    unsigned char* rgba(new unsigned char[numData]);
    
    const int64_t timeIndex(0);
    
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
                rgba[pixelIndex + c] = omeImage->getElement(i, j, k, timeIndex, c);
            }
        }
    }
    
    return FunctionResultValue<uint8_t*>(rgba,
                                         "",
                                         true);
}

/**
 * Read the given data set from the ZARR file.
 * @param sliceIndex
 *    Index of the slice.
 */
FunctionResultValue<OmeImage*>
OmeDataSet::readSlice(const int64_t sliceIndex) const
{
    if ((sliceIndex < 0)
        || (sliceIndex >= getNumberOfSlices())) {
        return FunctionResultValue<OmeImage*>(NULL,
                                                   ("Invalid slice index=" + QString::number(sliceIndex)),
                                                   false);
    }
    if ( ! m_zarrImageReader) {
        return FunctionResultValue<OmeImage*>(NULL,
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
    dimLengths[m_dimensionIndices.getIndexForX()] = dims[m_dimensionIndices.getIndexForX()];
    dimLengths[m_dimensionIndices.getIndexForY()] = dims[m_dimensionIndices.getIndexForY()];
    dimLengths[m_dimensionIndices.getIndexForZ()] = dims[m_dimensionIndices.getIndexForZ()];
    dimLengths[m_dimensionIndices.getIndexForChannel()] = dims[m_dimensionIndices.getIndexForChannel()];
    
    dimOffset[m_dimensionIndices.getIndexForZ()]  = sliceIndex;
    dimLengths[m_dimensionIndices.getIndexForZ()] = 1;
    
    FunctionResultValue<xt::xarray<uint8_t>*> zarrDataResult(m_zarrImageReader->readData(dimOffset,
                                                                                         dimLengths));
    if (zarrDataResult.isOk()) {
        return FunctionResultValue<OmeImage*>(new OmeImage(zarrDataResult.getValue(),
                                                           m_dimensionIndices),
                                              "",
                                              true);
    }
    
    return FunctionResultValue<OmeImage*>(NULL,
                                          zarrDataResult.getErrorMessage(),
                                          zarrDataResult.isOk());
}

/**
 * Read the given pixel from the ZARR file.
 * @param sliceIndex
 *    Index of the slice.
 * @param pixelI
 *    Pixel index I
 * @param pixelJ
 *    Pixel index J
 */
FunctionResultValue<std::array<uint8_t, 4>>
OmeDataSet::readSlicePixel(const int64_t sliceIndex,
                           const int64_t pixelI,
                           const int64_t pixelJ) const
{
    std::array<uint8_t, 4> rgba { 0, 0, 0, 0 };
    
    if ((sliceIndex < 0)
        || (sliceIndex >= getNumberOfSlices())) {
        return FunctionResultValue<std::array<uint8_t, 4>>(rgba,
                                              ("Invalid slice index=" + QString::number(sliceIndex)),
                                              false);
    }
    if ( ! m_zarrImageReader) {
        return FunctionResultValue<std::array<uint8_t, 4>>(rgba,
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
    dimLengths[m_dimensionIndices.getIndexForX()] = dims[m_dimensionIndices.getIndexForX()];
    dimLengths[m_dimensionIndices.getIndexForY()] = dims[m_dimensionIndices.getIndexForY()];
    dimLengths[m_dimensionIndices.getIndexForZ()] = dims[m_dimensionIndices.getIndexForZ()];
    dimLengths[m_dimensionIndices.getIndexForChannel()] = dims[m_dimensionIndices.getIndexForChannel()];
    
    dimOffset[m_dimensionIndices.getIndexForX()] = pixelI;
    dimOffset[m_dimensionIndices.getIndexForY()] = pixelJ;
    dimOffset[m_dimensionIndices.getIndexForZ()]  = sliceIndex;
    dimLengths[m_dimensionIndices.getIndexForX()] = 1;
    dimLengths[m_dimensionIndices.getIndexForY()] = 1;
    dimLengths[m_dimensionIndices.getIndexForZ()] = 1;
    
    FunctionResultValue<xt::xarray<uint8_t>*> zarrDataResult(m_zarrImageReader->readData(dimOffset,
                                                                                         dimLengths));
    if (zarrDataResult.isOk()) {
        xt::xarray<uint8_t>* dataRead(zarrDataResult.getValue());
        auto iter(dataRead->begin());
        rgba[0] = *iter++;
        rgba[1] = *iter++;
        rgba[2] = *iter++;
        rgba[3] = *iter++;
        delete dataRead;

        return FunctionResultValue<std::array<uint8_t, 4>>(rgba,
                                               "",
                                               true);
    }
    
    return FunctionResultValue<std::array<uint8_t, 4>>(rgba,
                                           zarrDataResult.getErrorMessage(),
                                           zarrDataResult.isOk());

}

/**
 * @return The coordinate at the given pixel index
 * @param pixelI
 *    Pixel index I
 * @param pixelJ
 *    Pixel index J
 * @param pixelK
 *    Pixel index K
 */
Vector3D 
OmeDataSet::getPixelCoordinate(const int64_t pixelI,
                               const int64_t pixelJ,
                               const int64_t pixelK) const
{
    Vector3D xyz(pixelI, pixelJ, pixelK);
    
    const int64_t indexX(m_dimensionIndices.getIndexForX());
    const int64_t indexY(m_dimensionIndices.getIndexForY());
    const int64_t indexZ(m_dimensionIndices.getIndexForZ());
    
    for (const OmeCoordinateTransformations& oct : m_coordinateTransformations) {
        const std::vector<float> t(oct.getTransformValues());
        const int64_t numT(t.size());
        
        if ((indexX >= 0)
            && (indexX < numT)) {
            switch (oct.getType()) {
                case OmeCoordinateTransformationTypeEnum::INVALID:
                    break;
                case OmeCoordinateTransformationTypeEnum::SCALE:
                    CaretAssertVectorIndex(t, indexX);
                    xyz[0] *= t[indexX];
                    break;
                case OmeCoordinateTransformationTypeEnum::TRANSLATE:
                    CaretAssertVectorIndex(t, indexX);
                    xyz[0] += t[indexX];
                    break;
            }
        }
        if ((indexY >= 0)
            && (indexY < numT)) {
            switch (oct.getType()) {
                case OmeCoordinateTransformationTypeEnum::INVALID:
                    break;
                case OmeCoordinateTransformationTypeEnum::SCALE:
                    CaretAssertVectorIndex(t, indexY);
                    xyz[1] *= t[indexY];
                    break;
                case OmeCoordinateTransformationTypeEnum::TRANSLATE:
                    CaretAssertVectorIndex(t, indexY);
                    xyz[1] += t[indexY];
                    break;
            }
        }
        if ((indexZ >= 0)
            && (indexZ < numT)) {
            switch (oct.getType()) {
                case OmeCoordinateTransformationTypeEnum::INVALID:
                    break;
                case OmeCoordinateTransformationTypeEnum::SCALE:
                    CaretAssertVectorIndex(t, indexZ);
                    xyz[2] *= t[indexZ];
                    break;
                case OmeCoordinateTransformationTypeEnum::TRANSLATE:
                    CaretAssertVectorIndex(t, indexZ);
                    xyz[2] += t[indexZ];
                    break;
            }
        }
    }
    return xyz;
}



