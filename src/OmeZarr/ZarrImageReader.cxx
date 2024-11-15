
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

#define __ZARR_IMAGE_READER_DECLARE__
#include "ZarrImageReader.h"
#undef __ZARR_IMAGE_READER_DECLARE__

#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <thread>

#include "xtensor/xarray.hpp"

// factory functions to create files, groups and datasets
#include "z5/factory.hxx"
// handles for z5 filesystem objects
#include "z5/filesystem/handle.hxx"
// io for xtensor multi-arrays
#include "z5/multiarray/xtensor_access.hxx"

#include "CaretAssert.h"
#include "FileInformation.h"
#include "ZarrHelper.h"
#include "ZarrV2ArrayJsonFile.h"

using namespace caret;

/**
 * \class caret::ZarrImageReader 
 * \brief Reads from a ZARR file
 * \ingroup OmeZarr
 *
 * Reads ZARR data from a directory containing a
 * .zarray file
 */

/**
 * Constructor.
 */
ZarrImageReader::ZarrImageReader()
: CaretObject()
{
    
}

/**
 * Destructor.
 */
ZarrImageReader::~ZarrImageReader()
{
}

/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void 
ZarrImageReader::copyHelperZarrImageReader(const ZarrImageReader& obj)
{
    m_shapeSizes = obj.m_shapeSizes;
    m_dataType   = obj.m_dataType;
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
ZarrImageReader::toString() const
{
    AString txt("ZarrImageReader");
    txt.appendWithNewLine("   Shape Sizes: " + AString::fromNumbers(m_shapeSizes));
    txt.appendWithNewLine("   Data Type: " + ZarrDataTypeEnum::toGuiName(m_dataType));
    return txt;
}


/**
 * @return file shape sizes
 */
std::vector<int64_t>
ZarrImageReader::getShapeSizes() const
{
    return m_shapeSizes;
}

/**
 * @return data type
 */
ZarrDataTypeEnum::Enum
ZarrImageReader::getDataType() const
{
    return m_dataType;
}

/**
 * Initialize this zarr image reader prior to reading data
 * @param driverType
 *    Type of ZARR driver
 * @param zarrPath
 *    Top level path (could be a directory, zip file, web address, etc.)
 * @param relativePath
 *    Path within the zarr path
 * @return
 *    Result of initialization
 */
FunctionResult
ZarrImageReader::initialize(const ZarrDriverTypeEnum::Enum driverType,
                            const AString& zarrPath,
                            const AString& relativePath)
{
    m_driverType   = driverType;
    m_zarrPath     = zarrPath;
    m_relativePath = relativePath;
    
    if (m_driverType == ZarrDriverTypeEnum::INVALID) {
        return FunctionResult::error("Driver type is invalid.");
    }
    if (m_zarrPath.isEmpty()) {
        return FunctionResult::error("Zarr path is invalid.");
    }
    if (m_relativePath.isEmpty()) {
        return FunctionResult::error("Zarr relative path is invalid.");
    }

    /*
     * Read .zarray file
     */
    FunctionResultValue<ZarrV2ArrayJsonFile*> zarrArrayResultV2(ZarrHelper::readZarrV2ArrayJsonFile(m_driverType,
                                                                                                    m_zarrPath,
                                                                                                    m_relativePath));
    if (zarrArrayResultV2.isError()) {
        m_status = Status::INITIALIZATION_FAILED;
        return FunctionResult::error(zarrArrayResultV2.getErrorMessage());
    }
    m_zarrayFile.reset(zarrArrayResultV2.getValue());
    CaretAssert(m_zarrayFile);

    m_shapeSizes = m_zarrayFile->getShapeSizes();
    m_dataType   = m_zarrayFile->getDataType();
    
    if (m_shapeSizes.empty()) {
        return FunctionResult::error("Shape sizes for ZARR Image are invalid (empty).");
    }
    if (m_dataType == ZarrDataTypeEnum::UNKNOWN) {
        return FunctionResult::error("Data type is unknown.");
    }
    
    m_status = Status::INITIALIZATION_SUCCESSFUL;

    return FunctionResult::ok();
}

/**
 * @return A function result for a data reading error with the given error message
 * @param errorMessage
 *    The error message
 */
FunctionResultValue<xt::xarray<uint8_t>*>
ZarrImageReader::readDataErrorResult(const AString& errorMessage)
{
    return FunctionResultValue<xt::xarray<uint8_t>*>(NULL,
                                                     errorMessage,
                                                     false);
}

/**
 * Read data from the ZARR file
 * @param dimOffset
 *    Starting offset for reading from each of the dimensions
 * @param dimLengths
 *    Lengths of data to read from each of the dimensions
 * @return
 *    Function result containing data or error
 */
FunctionResultValue<xt::xarray<uint8_t>*>
ZarrImageReader::readData(const std::vector<int64_t>& dimOffsets,
                          const std::vector<int64_t>& dimLengths)
{
    switch (m_status) {
        case Status::INITIALIZATION_SUCCESSFUL:
            break;
        case Status::INITIALIZATION_FAILED:
            return readDataErrorResult("ZarrImageReader initialized had failed");
            break;
        case Status::UNINITIALIZED:
            return readDataErrorResult("ZarrImageReader has not been initialized");
            break;
    }
    
    CaretAssert(m_zarrayFile);
        
    if (dimOffsets.size() != m_shapeSizes.size()) {
        return readDataErrorResult("Offsets are different size that ZARR Image shape sizes.");
    }
    if (dimLengths.size() != m_shapeSizes.size()) {
        return readDataErrorResult("Lengths are different size that ZARR Image shape sizes.");
    }
    
    const QString msg("Driver type not supported: "
                      + ZarrDriverTypeEnum::toGuiName(m_driverType));
    FunctionResultValue<xt::xarray<uint8_t>*> result(NULL, msg, false);
    switch (m_driverType) {
        case ZarrDriverTypeEnum::INVALID:
            break;
        case ZarrDriverTypeEnum::LOCAL_FILE:
            result = readLocalFile(m_zarrPath,
                                   m_relativePath,
                                   dimOffsets,
                                   dimLengths);
            break;
        case ZarrDriverTypeEnum::LOCAL_ZIP_FILE:
            break;
    }
    
    if (result.isOk()) {
        const xt::xarray<uint8_t>* dataPtr(result.getValue());
        const int32_t numDataDims(dataPtr->dimension());
        std::vector<int64_t> dataDimLengths;
        for (int32_t i = 0; i < numDataDims; i++) {
            dataDimLengths.push_back(dataPtr->shape(i));
        }
        CaretAssert(numDataDims == static_cast<int32_t>(dataDimLengths.size()));
        const int32_t numDimLengths(dimLengths.size());
        bool matchFlag(false);
        if (numDataDims == numDimLengths) {
            matchFlag = true;
            for (int32_t i = 0; i < numDataDims; i++) {
                if (dataDimLengths[i] != dimLengths[i]) {
                    matchFlag = false;
                    break;
                }
            }
        }
        if ( ! matchFlag) {
            AString msg("Requested data dimensions: "
                        + AString::fromNumbers(dimLengths)
                        + " but read: "
                        + AString::fromNumbers(dataDimLengths));
            result = FunctionResultValue<xt::xarray<uint8_t>*>(NULL,
                                                               msg,
                                                               false);
        }
    }
    
    return result;
}

/**
 * Read data from a local ZARR file
 * @param zarrPath
 *    Top level path (could be a directory, zip file, web address, etc.)
 * @param relativePath
 *    Path within the zarr path
 * @param dimOffset
 *    Starting offset for reading from each of the dimensions
 * @param dimLengths
 *    Lengths of data to read from each of the dimensions
 * @return
 *    Result containing the data that was read
 */
FunctionResultValue<xt::xarray<uint8_t>*>
ZarrImageReader::readLocalFile(const AString& zarrPath,
                               const AString& relativePath,
                               const std::vector<int64_t>& dimOffsets,
                               const std::vector<int64_t>& dimLengths)
{
    try {
        /*
         * Path to file
         */
        const std::string filePath(zarrPath.toStdString()
                                   + "/"
                                   + relativePath.toStdString());
        z5::filesystem::handle::File file(zarrPath.toStdString());
        
        /*
         * Open the data set
         */
        std::unique_ptr<z5::Dataset> dataSet;
        try {
            dataSet = z5::openDataset(file,
                                      relativePath.toStdString());
        }
        catch (const std::exception& re) {
            const AString msg("Opening dataset failed (z5::openDataset execption), path="
                              + AString(filePath)
                              + AString("  error: ")
                              + AString(re.what()));
            return readDataErrorResult(msg);
        }
        
        if ( ! dataSet) {
            return readDataErrorResult("Opening data set failed " + filePath);
        }
        
        /*
         * Read all data from the file
         */
        z5::types::ShapeType offset;
        for (const int64_t dimO : dimOffsets) {
            offset.push_back(dimO);
        }
        
        xt::xarray<int64_t>::shape_type shape;
        for (const int64_t dl : dimLengths) {
            shape.push_back(dl);
        }
        try {
            const int32_t numThreads(std::thread::hardware_concurrency());
            std::unique_ptr<xt::xarray<uint8_t>> arrayStorage(new xt::xarray<uint8_t>(shape));
            z5::multiarray::readSubarray<uint8_t>(dataSet,
                                                  *arrayStorage.get(),
                                                  offset.begin(),
                                                  numThreads);
            return FunctionResultValue<xt::xarray<uint8_t>*>(arrayStorage.release(),
                                                             "",
                                                             true);
        }
        catch(const std::exception& re) {
            const AString msg("Z5::readSubarray exeception: "
                              + AString(re.what()));
            return readDataErrorResult(msg);
        }
    }
    catch (const std::exception& e) {
        return readDataErrorResult("Exception was thrown by Z5: "
                                   + AString(e.what()));
    }

    CaretAssert(0);
    return readDataErrorResult("Should never get here");
}

