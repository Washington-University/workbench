
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

/*=========================================================================
 *  Code for reading OME-ZARR is adapted/copied from
 *  https://github.com/InsightSoftwareConsortium/ITKIOOMEZarrNGFF/blob/main/src/itkOMEZarrNGFFImageIO.cxx#L328
 *
 *  Copyright NumFOCUS
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *         https://www.apache.org/licenses/LICENSE-2.0.txt
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *=========================================================================*/

#define __OME_ZARR_READER_DECLARE__
#include "OmeZarrReader.h"
#undef __OME_ZARR_READER_DECLARE__

#include <QFile>
#include <QImage>
#include <QImageWriter>

#include "CaretAssert.h"
#include "ZarrImageReader.h"

using namespace caret;


    
/**
 * \class caret::OmeZarrReader 
 * \brief Helps with reading an OME-ZARR file.
 * \ingroup OmeZarr
 */

/**
 * Constructor.
 */
OmeZarrReader::OmeZarrReader()
: CaretObject()
{
    
}

/**
 * Destructor.
 */
OmeZarrReader::~OmeZarrReader()
{
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
OmeZarrReader::OmeZarrReader(const OmeZarrReader& obj)
: CaretObject(obj)
{
    this->copyHelperOmeZarrReader(obj);
}

/**
 * Assignment operator.
 * @param obj
 *    Data copied from obj to this.
 * @return
 *    Reference to this object.
 */
OmeZarrReader&
OmeZarrReader::operator=(const OmeZarrReader& obj)
{
    if (this != &obj) {
        CaretObject::operator=(obj);
        this->copyHelperOmeZarrReader(obj);
    }
    return *this;
}

/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void
OmeZarrReader::copyHelperOmeZarrReader(const OmeZarrReader& obj)
{
    m_tensorstoreContext = obj.m_tensorstoreContext;
    m_driverName         = obj.m_driverName;
    m_zarrFormatNumber   = obj.m_zarrFormatNumber;
    m_omeVersion         = obj.m_omeVersion;
    m_axes               = obj.m_axes;
    m_dataSets           = obj.m_dataSets;
}

/**
 * Initialize by reading metadata from the OME-ZARR file
 * @param omeZarrPath
 *    Path to OME-ZARR file
 * @return OK if able to read data from file or Error if there is a problem.
 */
FunctionResult
OmeZarrReader::initialize(const AString& omeZarrPath)
{
    
    /*
     * Context
     */
    m_tensorstoreContext = tensorstore::Context::Default();
    
    /*
     * Name of tensorstore driver is derived from the name of the path
     * (ie: a zip file uses the ZIP driver)
     */
    const FunctionResultString driverNameResult(ZarrImageReader::getDriverNameFromFilename(omeZarrPath));
    if ( ! driverNameResult.isOk()) {
        return driverNameResult;
    }
    m_driverName = driverNameResult.getValue();
    
    /*
     * Get zarr_format from .zgroup file
     */
    const FunctionResultInt32 zarrFormatResult(ZarrImageReader::readZgroupFileZarrFormat(m_tensorstoreContext,
                                                                                         m_driverName,
                                                                                         omeZarrPath));
    if ( ! zarrFormatResult.isOk()) {
        return zarrFormatResult;
    }
    m_zarrFormatNumber = zarrFormatResult.getValue();
    
    /*
     * Read and parse Json from .zattrs file
     */
    const auto zattrsJsonResult(ZarrImageReader::readZattrsFileJson(m_tensorstoreContext,
                                                                    m_driverName,
                                                                    omeZarrPath));
    if ( ! zattrsJsonResult.isOk()) {
        return zattrsJsonResult;
    }
    parseZattrsJson(zattrsJsonResult.getValue());
    

    /*
     * Number of data sets in the OME-ZARR file
     */
    const int32_t numDataSets(m_dataSets.size());
    for (int32_t i = 0; i < numDataSets; i++) {
        if (i > 0) {
            std::cout << "Exiting loop after first image" << std::endl;
            break;
        }
        CaretAssertVectorIndex(m_dataSets, i);
        OmeDataSet& dataSet = m_dataSets[i];
        const AString dataSetName(omeZarrPath
                                  + "/"
                                  + dataSet.getPath());
        
        ZarrImageReader* zarrImageReader(new ZarrImageReader());
        const FunctionResult zarrayResult(zarrImageReader->readZarrayFile(m_tensorstoreContext,
                                                                          dataSetName,
                                                                          m_driverName));
        if (zarrayResult.isError()) {
            return zarrayResult;
        }
        
        dataSet.setZarrImageReader(zarrImageReader);
        dataSet.setZarrDataType(zarrImageReader->getDataType());
        dataSet.setDimensions(zarrImageReader->getDimensions());
        dataSet.setDimensionIndices(m_dimensionIndexX,
                                    m_dimensionIndexY,
                                    m_dimensionIndexZ,
                                    m_dimensionIndexTime,
                                    m_dimensionIndexChannel);

        const bool loadTestFlag(false);
        if (loadTestFlag) {
            {
                /*
                 * Loads one Z-Slice
                 */
                const std::vector<int64_t> dims(dataSet.getDimensions());
                const int32_t numDims(dims.size());
                std::vector<int64_t> dimOffset(numDims, 0);
                std::vector<int64_t> dimLengths(numDims, 0);
                dimLengths[m_dimensionIndexX] = dims[m_dimensionIndexX];
                dimLengths[m_dimensionIndexY] = dims[m_dimensionIndexY];
                dimLengths[m_dimensionIndexZ] = dims[m_dimensionIndexZ];
                dimLengths[m_dimensionIndexChannel] = dims[m_dimensionIndexChannel];
                
                dimOffset[m_dimensionIndexZ] = 2;
                dimLengths[m_dimensionIndexZ] = 1;
                FunctionResultValue<unsigned char*> readResult(zarrImageReader->readData(dimOffset,
                                                                                         dimLengths,
                                                                                         ZarrDataTypeEnum::UINT_8));
                if (readResult.isOk()) {
                    /*
                     * Writes image
                     */
                    writeImageFromRawZarr(readResult.getValue(),
                                          dimOffset,
                                          dimLengths,
                                          "/Users/john/images/single_image_zarr");
                }
                else {
                    std::cout << "READ ONE TEST FAILED: " << readResult.getErrorMessage() << std::endl;
                }
            }

            {
                /*
                 * Loads ALL Z-Slices
                 */
                const std::vector<int64_t> dims(dataSet.getDimensions());
                const int32_t numDims(dims.size());
                std::vector<int64_t> dimOffset(numDims, 0);
                std::vector<int64_t> dimLengths(numDims, 0);
                dimLengths[m_dimensionIndexX] = dims[m_dimensionIndexX];
                dimLengths[m_dimensionIndexY] = dims[m_dimensionIndexY];
                dimLengths[m_dimensionIndexZ] = dims[m_dimensionIndexZ];
                dimLengths[m_dimensionIndexChannel] = dims[m_dimensionIndexChannel];
                FunctionResultValue<unsigned char*> readResult(zarrImageReader->readData(dimOffset,
                                                                                         dimLengths,
                                                                                         ZarrDataTypeEnum::UINT_8));
                if (readResult.isOk()) {
                    /*
                     * Writes all Z images
                     */
                    writeImageFromRawZarr(readResult.getValue(),
                                          dimOffset,
                                          dimLengths,
                                          "/Users/john/images/image_zarr");
                }
                else {
                    std::cout << "READ ALL TEST FAILED: " << readResult.getErrorMessage() << std::endl;
                }
            }
        }
    }

    return FunctionResult::ok();
}


/**
 * Read OME specific data from the .zattrs json
 * @param json
 *    The json read from the .zattrs file
 * @return
 *    Result with success or error
 */
FunctionResult
OmeZarrReader::parseZattrsJson(const nlohmann::json& jsonIn)
{
    try {
        AString dummyValue;
        
        /*
         * Find 'multiscales' array
         */
        const auto json(jsonIn);
        if ( ! json.contains("multiscales")) {
            return FunctionResult::error("multiscales not found in .attrs file");
        }
        const auto multiscalesJson = json.at("multiscales")[0];
        
        /*
         * Find 'version'
         */
        const std::string versionText(multiscalesJson.at("version").get<std::string>());
        bool versionValidFlag(false);
        m_omeVersion = OmeVersionEnum::fromGuiName(versionText,
                                                &versionValidFlag);
        if ( ! versionValidFlag) {
            return FunctionResult::error(("OME-NGFF version"
                                          + versionText
                                          + " is not supported"));
        }
        
        /*
         * Read 'axes'
         */
        if (multiscalesJson.contains("axes")) {
            FunctionResult result(parseZattsAxesJson(multiscalesJson.at("axes")));
            if (result.isError()) {
                return result;
            }
        }
        
        /*
         * Read 'datasets'
         */
        if (multiscalesJson.contains("datasets")) {
            FunctionResult result(parseZattsDatasetsJson(multiscalesJson.at("datasets")));
            if (result.isError()) {
                return result;
            }
        }
    }
    catch (const std::exception& e) {
        return FunctionResult::error("Uncaught exception while reading .zattrs file: "
                                     + AString(e.what()));
    }
    
    return FunctionResult::ok();
}

/**
 * Read the 'axes' from the .zattrs file jason
 * @param json
 *    The JSON
 * @return
 *    Result with success or error
 */
FunctionResult
OmeZarrReader::parseZattsAxesJson(const nlohmann::json& json)
{
    for (const auto& axis : json) {
        const AString axisDimName(AString::fromStdString(axis.at("name")));
        const AString axisTypeName(AString::fromStdString(axis.at("type")));
        const AString axisUnitName(AString::fromStdString(axis.contains("unit")
                                                          ? axis.at("unit")
                                                          : ""));

        AString errorMessage;
        
        OmeAxis omeAxis;
        
        if (axisDimName.isEmpty()) {
            errorMessage.appendWithNewLine("axes name is empty.");
        }
        else {
            omeAxis.setName(axisDimName);
        }
        
        bool axisTypeValidFlag(false);
        OmeAxisTypeEnum::Enum axisType(OmeAxisTypeEnum::fromLowerCaseName(axisTypeName,
                                                                          &axisTypeValidFlag));
        if (axisTypeValidFlag) {
            omeAxis.setAxisType(axisType);
        }
        else {
            errorMessage.appendWithNewLine("axes type invalid='" + axisTypeName + "'");
        }
        
        switch (axisType) {
            case OmeAxisTypeEnum::UNKNOWN:
                break;
            case OmeAxisTypeEnum::CHANNEL:
                break;
            case OmeAxisTypeEnum::SPACE:
            {
                bool spaceUnitValidFlag(false);
                OmeSpaceUnitEnum::Enum spaceUnit(OmeSpaceUnitEnum::fromLowerCaseName(axisUnitName,
                                                                                     &spaceUnitValidFlag));
                if (spaceUnitValidFlag) {
                    omeAxis.setSpaceUnit(spaceUnit);
                }
                else {
                    errorMessage.appendWithNewLine("axes space unit invalid='" + axisUnitName + "'");
                }
            }
                break;
            case OmeAxisTypeEnum::TIME:
                bool timeUnitValidFlag(false);
                OmeTimeUnitEnum::Enum timeUnit(OmeTimeUnitEnum::fromLowerCaseName(axisUnitName,
                                                                                  &timeUnitValidFlag));
                if (timeUnitValidFlag) {
                    omeAxis.setTimeUnit(timeUnit);
                }
                else {
                    errorMessage.appendWithNewLine("axes time unit invalid='" + axisUnitName + "'");
                }
                break;
        }
        
        if ( ! errorMessage.isEmpty()) {
            return FunctionResult::error(errorMessage);
        }
        
        m_axes.push_back(omeAxis);
    }
    
    int32_t dimensionIndex(0);
    for (const auto& axis : m_axes) {
        switch (axis.getAxisType()) {
            case OmeAxisTypeEnum::UNKNOWN:
                return FunctionResult::error("Axis dimension index="
                                             + AString::number(dimensionIndex)
                                             + " is of UNKNOWN type.");
                break;
            case OmeAxisTypeEnum::CHANNEL:
                if (m_dimensionIndexChannel >= 0) {
                    return FunctionResult::error("More than one dimension is of type channel. "
                                                 "This is not allowed.");
                }
                else {
                    m_dimensionIndexChannel = dimensionIndex;
                }
                break;
            case OmeAxisTypeEnum::TIME:
                if (m_dimensionIndexTime >= 0) {
                    return FunctionResult::error("More than one dimension is of type type. "
                                                 "This is not allowed.");
                }
                else {
                    m_dimensionIndexTime = dimensionIndex;
                }
                break;
            case OmeAxisTypeEnum::SPACE:
            {
                if (axis.getName().toLower() == "x") {
                    if (m_dimensionIndexX >= 0) {
                        return FunctionResult::error("More than one spatial dimension is for 'x'");
                    }
                    else {
                        m_dimensionIndexX = dimensionIndex;
                    }
                }
                else if (axis.getName().toLower() == "y") {
                    if (m_dimensionIndexY >= 0) {
                        return FunctionResult::error("More than one spatial dimension is for 'y'");
                    }
                    else {
                        m_dimensionIndexY = dimensionIndex;
                    }
                }
                else if (axis.getName().toLower() == "z") {
                    if (m_dimensionIndexZ >= 0) {
                        return FunctionResult::error("More than one spatial dimension is for 'z'");
                    }
                    else {
                        m_dimensionIndexZ = dimensionIndex;
                    }
                }
                else {
                    return FunctionResult::error(axis.getName()
                                                 + " is not a recognized as a spatial index for an axis");
                }
            }
        }
        
        ++dimensionIndex;
    }
    
    if (m_dimensionIndexX < 0) {
        return FunctionResult::error("Dimension for 'X' not found.");
    }
    if (m_dimensionIndexY < 0) {
        return FunctionResult::error("Dimension for 'Y' not found.");
    }
    if (m_dimensionIndexZ < 0) {
        return FunctionResult::error("Dimension for 'Z' not found.");
    }
    
    return FunctionResult::ok();
}

/**
 * Read the 'datasets' from the .zattrs file jason
 * @param json
 *    The JSON
 * @return
 *    Result with success or error
 */
FunctionResult
OmeZarrReader::parseZattsDatasetsJson(const nlohmann::json& json)
{
    for (const auto& dataset : json) {
        const AString pathName(AString::fromStdString(dataset.at("path")));

        OmeDataSet dataSet;
        dataSet.setPath(pathName);
        
        FunctionResult result(parseZattsCoordinateTransformationsJson(dataset.at("coordinateTransformations"),
                                                                      dataSet));
        if (result.isError()) {
            return result;
        }
        
        m_dataSets.push_back(dataSet);
    }
    
    return FunctionResult::ok();
}

/**
 * Read the 'coordinateTransformations' from the .zattrs file jason
 * @param json
 *    The JSON
 * @param dataSet
 *    Dataset being read
 * @return
 *    Result with success or error
 */
FunctionResult
OmeZarrReader::parseZattsCoordinateTransformationsJson(const nlohmann::json& json,
                                                       OmeDataSet& dataSet)
{
    const int numCoordTrans(json.size());
    for (const auto& elem : json) {
        if (elem.contains("scale")) {
            const auto scaleArray(elem.at("scale"));
            const int32_t numElements(scaleArray.size());
            std::vector<float> scaleVector;
            for (int32_t i = 0; i < numElements; i++) {
                scaleVector.push_back(scaleArray[i].get<float>());
            }
            dataSet.setScaling(scaleVector);
        }
        if (elem.contains("translate")) {
            const auto translateArray(elem.at("translate"));
            const int32_t numElements(translateArray.size());
            std::vector<float> translateVector;
            for (int32_t i = 0; i < numElements; i++) {
                translateVector.push_back(translateArray[i].get<float>());
            }
            dataSet.setTranslation(translateVector);
        }
        const AString coordTransType(AString::fromStdString(elem.at("type").get<std::string>()));
    }
    return FunctionResult::ok();
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
OmeZarrReader::toString() const
{
    AString s("OmeZarrReader");
    s.appendWithNewLine("   Version: "
                        + OmeVersionEnum::toGuiName(m_omeVersion));
    
    for (const auto& axis : m_axes) {
        s.appendWithNewLine("   " + axis.toString());
    }
    
    for (const auto& ds : m_dataSets) {
        s.appendWithNewLine("   " + ds.toString());
    }
    
    s.appendWithNewLine("   Dimension X: "
                        + AString::number(m_dimensionIndexX));
    s.appendWithNewLine("   Dimension Y: "
                        + AString::number(m_dimensionIndexY));
    s.appendWithNewLine("   Dimension Z: "
                        + AString::number(m_dimensionIndexZ));
    s.appendWithNewLine("   Dimension Channel: "
                        + AString::number(m_dimensionIndexChannel));
    s.appendWithNewLine("   Dimension Time: "
                        + AString::number(m_dimensionIndexTime));
    return s;
}

/**
 * Write to a QImage file
 * @param filename
 *    Name of image file
 * @param data
 *    Pointer to image data
 * @param width
 *    Width of image
 * @param height
 *    Height of image
 * @param bytesPerPixel
 *    Number of bytes per pixel
 */
void
OmeZarrReader::writeToQImage(const QString& filename,
                             const uint8_t* data,
                             const int32_t width,
                             const int32_t height,
                             const int32_t bytesPerPixel) const
{
    CaretAssert(data);
    
    QImage::Format qtFormat = QImage::Format_Invalid;
    
    if (bytesPerPixel != 4) {
        std::cout << "Only 4 bytes per pixel supported for writing image" << std::endl;
    }
    switch (bytesPerPixel) {
        case 1:
            qtFormat = QImage::Format_Grayscale8;
            break;
        case 3:
            qtFormat = QImage::Format_RGB888;
            break;
        case 4:
            qtFormat = QImage::Format_RGBA8888;
            qtFormat = QImage::Format_ARGB32;
            break;
    }

    int64_t dataIndex(0);
    QImage image(width,
                 height,
                 qtFormat);
    
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            uint8_t red(data[dataIndex]);
            uint8_t green(data[dataIndex+1]);
            uint8_t blue(data[dataIndex+2]);
            uint8_t alpha(data[dataIndex+3]);
            
            red = data[dataIndex+3];
            green = data[dataIndex+2];
            blue = data[dataIndex+1];
            alpha = data[dataIndex+0];
            dataIndex += 4;
            image.setPixelColor(x, y, QColor(red, green, blue, alpha));
        }
    }
    
    QImageWriter writer(filename);
    if ( ! writer.write(image)) {
        std::cout << "Failed writing image: "
        << filename
        << " Error: "
        << writer.errorString()
        << std::endl;
    }
}


/**
 * Write to a QImage file
 * @param data
 *    Pointer to image data
 * @param offsets
 *    Offsets from beginning of each dimension
 * @param lengths
 *    Lengths for each dimension
 * @param imageFilenamePrefix
 *    Name for image file.
 */
void
OmeZarrReader::writeImageFromRawZarr(const uint8_t* data,
                                     const std::vector<int64_t>& offsets,
                                     const std::vector<int64_t>& lengths,
                                     const AString imageFilenamePrefix)
{
    
    CaretAssert(offsets.size() == 4);
    CaretAssert(lengths.size() == 4);
    const int64_t numChannels(lengths[0]);
    const int64_t numZ(lengths[1]);
    const int64_t numY(lengths[2]);
    const int64_t numX(lengths[3]);
    const int64_t numData(numX * numY * numZ * numChannels);
    
    QImage image(numX, numY, QImage::Format_RGBA8888);// QImage::Format_ARGB32);
    
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
                    rgba[c] = data[offset];
                }
                
                image.setPixelColor(i, numY - j - 1, QColor(rgba[0], rgba[1], rgba[2], rgba[3]));
            }
        }
        QImageWriter writer(imageFilenamePrefix + "_" + AString::number(k) + ".png");
        if ( ! writer.write(image)) {
            std::cout << "Failed writing image: "
            << writer.fileName()
            << " Error: "
            << writer.errorString()
            << std::endl;
        }
    }
}

/**
 * @return Number of data sets
 */
int32_t
OmeZarrReader::getNumberOfDataSets() const
{
    return m_dataSets.size();
}

/**
 * @return Data set at the given index
 * @param index
 */
const OmeDataSet&
OmeZarrReader::getDataSet(const int32_t index) const
{
    CaretAssertVectorIndex(m_dataSets, index);
    return m_dataSets[index];
}
