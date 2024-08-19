
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

#include "tensorstore/index_space/dim_expression.h"
#include "tensorstore/index_space/index_domain.h"
#include "tensorstore/index_space/index_domain_builder.h"

#include "CaretAssert.h"
#include "FileInformation.h"
using namespace caret;


    
/**
 * \class caret::ZarrImageReader 
 * \brief Reads from a ZARR file
 * \ingroup OmeZarr
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
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
ZarrImageReader::ZarrImageReader(const ZarrImageReader& obj)
: CaretObject(obj)
{
    this->copyHelperZarrImageReader(obj);
}

/**
 * Assignment operator.
 * @param obj
 *    Data copied from obj to this.
 * @return 
 *    Reference to this object.
 */
ZarrImageReader&
ZarrImageReader::operator=(const ZarrImageReader& obj)
{
    if (this != &obj) {
        CaretObject::operator=(obj);
        this->copyHelperZarrImageReader(obj);
    }
    return *this;    
}

/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void 
ZarrImageReader::copyHelperZarrImageReader(const ZarrImageReader& obj)
{
    m_dimensions = obj.m_dimensions;
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
    txt.appendWithNewLine("   Dimensions: " + AString::fromNumbers(m_dimensions));
    txt.appendWithNewLine("   Data Type: " + ZarrDataTypeEnum::toGuiName(m_dataType));
    return txt;
}


/**
 * @return file dimensions
 */
std::vector<int64_t>
ZarrImageReader::getDimensions() const
{
    return m_dimensions;
}

/**
 * Set file dimensions
 *
 * @param dimensions
 *    New value for file dimensions
 */
void
ZarrImageReader::setDimensions(const std::vector<int64_t> dimensions)
{
    m_dimensions = dimensions;
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
 * Set data type
 *
 * @param dataType
 *    New value for data type
 */
void
ZarrImageReader::setDataType(const ZarrDataTypeEnum::Enum dataType)
{
    m_dataType = dataType;
}

/**
 * Read Info from a .zarray file for a ZARR image
 * @param context
 *    The tensorstore context
 * @param zarrayFilename
 *    Name of the .zarray file
 * @param driverName
 *    Name of ZARR driver
 * @return
 *    Result with success or error
 */
FunctionResult
ZarrImageReader::readZarrayFile(tensorstore::Context& context,
                                const AString& zarrayFilename,
                                const AString& driverName)
{
    if ( ! FileInformation(zarrayFilename).exists()) {
        /*
         * Tensorstore will crash if file does not exist
         */
        return FunctionResult::error(zarrayFilename
                                     + " does not exist for reading.");
    }
    
    nlohmann::json readSpec = { { "driver",
                                  "zarr" },
                                { "kvstore",
                                   { { "driver", driverName.toStdString() }, { "path", zarrayFilename.toStdString() } } } };
    auto openFuture = tensorstore::Open(readSpec,
                                        context,
                                        tensorstore::OpenMode::open,
                                        tensorstore::RecheckCached{ false },
                                        tensorstore::ReadWriteMode::read);
    m_store = openFuture.value();
    auto shapeDimensions = m_store.domain().shape();
    
    std::vector<int64_t> dims;
    const int32_t numDim(shapeDimensions.size());
    for (int32_t i = 0; i < numDim; i++) {
        dims.push_back(shapeDimensions[i]);
    }
    setDimensions(dims);
    
    const tensorstore::DataType dataType(m_store.dtype());
    const tensorstore::DataTypeId dataTypeID(dataType.id());
    
    ZarrDataTypeEnum::Enum zarrDataType(ZarrDataTypeEnum::UNKNOWN);
    switch (dataTypeID) {
        case tensorstore::DataTypeId::char_t:
        case tensorstore::DataTypeId::int8_t:
            zarrDataType = ZarrDataTypeEnum::INT_8;
            break;
        case tensorstore::DataTypeId::byte_t:
        case tensorstore::DataTypeId::uint8_t:
            zarrDataType = ZarrDataTypeEnum::UINT_8;
            break;
        case tensorstore::DataTypeId::int16_t:
            zarrDataType = ZarrDataTypeEnum::INT_16;
            break;
        case tensorstore::DataTypeId::uint16_t:
            zarrDataType = ZarrDataTypeEnum::UINT_16;
            break;
        case tensorstore::DataTypeId::int32_t:
            zarrDataType = ZarrDataTypeEnum::INT_32;
            break;
        case tensorstore::DataTypeId::uint32_t:
            zarrDataType = ZarrDataTypeEnum::UINT_32;
            break;
        case tensorstore::DataTypeId::int64_t:
            zarrDataType = ZarrDataTypeEnum::INT_64;
            break;
        case tensorstore::DataTypeId::uint64_t:
            zarrDataType = ZarrDataTypeEnum::UINT_64;
            break;
        case tensorstore::DataTypeId::float32_t:
            zarrDataType = ZarrDataTypeEnum::FLOAT_32;
            break;
        case tensorstore::DataTypeId::float64_t:
            zarrDataType = ZarrDataTypeEnum::FLOAT_64;
            break;
        default:
            zarrDataType = ZarrDataTypeEnum::UNKNOWN;
            break;
    }
    
    setDataType(zarrDataType);
    
    return FunctionResult::ok();
}

/**
 * @return Name of tensorstore driver appropriate for the given filename or error
 * @param filename
 *    Name of file
 */
FunctionResultString
ZarrImageReader::getDriverNameFromFilename(const AString& filename)
{
    AString driverName;
    
    AString errorMessage;
    if (filename.startsWith("http")) {
        driverName = "http";
    }
    else if (filename.endsWith(".zip")) {
        driverName = "zip";
    }
    else if ( ! filename.isEmpty()) {
        driverName = "file";
    }
    else {
        errorMessage = ("Filename is empty.");
    }
    
    FunctionResultString result(driverName,
                                errorMessage,
                                errorMessage.isEmpty());
    return result;
}

/**
 * Read Json from the given filename using the given tensorstore driver
 * @param context
 *    The tensorstore context
 * @param driverName
 *    Name of tensorstore driver
 * @param zarrFilename
 *    Name of ZARR file (eg: QM23.50.001.CX.43.01.ome.zarr.zip or directory: QM23.50.001.CX.43.01.ome.zarr/)
 * @param fileInZarrFilename
 *    Name of file in the ZARR (eg. .zarray or .zgroup)
 * @return
 *    Result with JSON or error.
 */
FunctionResultValue<nlohmann::json>
ZarrImageReader::readJson(tensorstore::Context& context,
                          const AString& driverName,
                          const AString& zarrFilename,
                          const AString& jsonFilename)
{
    nlohmann::json jsonRead = nlohmann::json::object_t();
    AString errorMessage;
    
    /*
     * JSON that describes how to open and read the file
     */
    const AString fullPathName(zarrFilename
                               + "/"
                               + jsonFilename);
    nlohmann::json readSpec = { { "driver", "json" },
        { "kvstore", { { "driver", driverName.toStdString() },
            { "path", fullPathName.toStdString() } } } };
    
    if (zarrFilename.endsWith(".zip")) {
        readSpec = { { "driver", "zip" },
            { "kvstore", zarrFilename.toStdString(),
                { "path", jsonFilename.toStdString() } } };
    }
    
    /*
     * Handle to driver
     */
    auto handle(tensorstore::Open<nlohmann::json, 0>(readSpec, context).result().value());
    
    /*
     * Read the JSON
     */
    auto jsonResult = tensorstore::Read(handle).result();
    
    if (jsonResult.ok())
    {
        jsonRead = jsonResult.value()();
    }
    else if (absl::IsNotFound(jsonResult.status()))
    {
        errorMessage = ("File not found: "
                        + fullPathName);
    }
    else // another error
    {
        errorMessage = ("Unknown error trying to read: "
                        + fullPathName);
    }
    
    FunctionResultValue<nlohmann::json> result(jsonRead,
                                               errorMessage,
                                               errorMessage.isEmpty());
    return result;
}

/**
 * Read the zarr_format from the .zgroup file.
 *
 * @param context
 *    The tensorstore context
 * @param driverName
 *    Name of tensorstore driver
 * @param zarrFilename
 *    Name of ZARR file
 * @return
 *    Result with zarr_format or error
 */
FunctionResultInt32
ZarrImageReader::readZgroupFileZarrFormat(tensorstore::Context& context,
                                            const AString& driverName,
                                            const AString& zarrFilename)
{
    int32_t zarrFormatNumber(-1);
    AString errorMessage;
    
    /*
     * Read the .zgroup file
     */
    const auto zGroupJsonResult(ZarrImageReader::readJson(context,
                                                          driverName,
                                                          zarrFilename,
                                                          ".zgroup"));
    if (zGroupJsonResult.isOk()) {
        /*
         * Get ZARR version from .zgroup file
         */
        const nlohmann::json& zgroupJson = zGroupJsonResult.getValue();
        const auto zarrFormatElement(zgroupJson.at("zarr_format"));
        if (zarrFormatElement.is_null()) {
            errorMessage = "zarr_format in .zgroup file is missing or non-numeric";
        }
        else {
            zarrFormatNumber = zarrFormatElement.get<int>();
            if (zarrFormatNumber != 2) {
                errorMessage = ("zarr_format="
                                + AString::number(zarrFormatNumber)
                                + " is not supported");
            }
        }
    }
    else {
        errorMessage = zGroupJsonResult.getErrorMessage();
    }
    
    FunctionResultInt32 result(zarrFormatNumber,
                               errorMessage,
                               errorMessage.isEmpty());
    return result;
}

/**
 * Read the JSON from the .zattrs file
 *
 * @param context
 *    The tensorstore context
 * @param driverName
 *    Name of tensorstore driver
 * @param zarrFilename
 *    Name of ZARR file
 * @return
 *    Result with JSON from .zattrs file or errror
 */
FunctionResultValue<nlohmann::json>
ZarrImageReader::readZattrsFileJson(tensorstore::Context& context,
                                      const AString& driverName,
                                      const AString& zarrFilename)
{
    AString errorMessage;
    
    /*
     * Read the .zgroup file
     */
    const auto zAttrsJsonResult(ZarrImageReader::readJson(context,
                                                          driverName,
                                                          zarrFilename,
                                                          ".zattrs"));
    return zAttrsJsonResult;
}

/**
 * Read data from the ZARR file
 * @param dimOffset
 *    Starting offset for reading from each of the dimensions
 * @param dimLengths
 *    Lengths of data to read from each of the dimensions
 * @param dataType
 *    Data type for output of data read
 */
FunctionResultValue<unsigned char*> 
ZarrImageReader::readData(const std::vector<int64_t>& dimOffsets,
                                             const std::vector<int64_t>& dimLengths,
                                             const ZarrDataTypeEnum::Enum dataTypeToRead)
{
    if (m_dimensions.empty()) {
        FunctionResultValue<unsigned char*> result(NULL,
                                                   "Dimensions for ZARR Image are invalid (empty).",
                                                   false);
        return result;
    }
    
    if (dimOffsets.size() != m_dimensions.size()) {
        FunctionResultValue<unsigned char*> result(NULL,
                                                   "Offsets are different size that ZARR Image dimensions.",
                                                   false);
        return result;
    }
    if (dimLengths.size() != m_dimensions.size()) {
        FunctionResultValue<unsigned char*> result(NULL,
                                                   "Lengths are different size that ZARR Image dimensions.",
                                                   false);
        return result;
    }
    const auto dimension = m_store.rank();
    std::vector<tensorstore::Index> indices(dimension);
    std::vector<tensorstore::Index> sizes(dimension);

    int64_t bufferSize(1);
    const int32_t numDims(m_dimensions.size());
    for (int32_t i = 0; i < numDims; i++) {
        CaretAssertVectorIndex(dimOffsets, i);
        CaretAssertVectorIndex(dimLengths, i);
        indices[i] = dimOffsets[i];
        sizes[i]   = dimLengths[i];
        bufferSize *= dimLengths[i];
    }
    if (bufferSize <= 1) {
        FunctionResultValue<unsigned char*> result(NULL,
                                                   "Lengths of region to read are empty (0 size).",
                                                   false);
        return result;
    }
    
    const bool readAllFlag(false);
    if (readAllFlag) {
        unsigned char* buffer(new unsigned char[bufferSize]);
        CaretAssert(buffer);
        auto arr = tensorstore::Array(buffer, m_store.domain().shape(), tensorstore::c_order);
        tensorstore::Read(m_store, tensorstore::UnownedToShared(arr)).value();

        FunctionResultValue<unsigned char*> result(buffer,
                                                   "",
                                                   true);
        return result;
    }
    else {
        unsigned char* buffer(new unsigned char[bufferSize]);
        CaretAssert(buffer);
        auto indexDomain = tensorstore::IndexDomainBuilder(dimension).origin(indices).shape(sizes).Finalize().value();
        
        auto arr = tensorstore::Array(buffer, indexDomain.shape(), tensorstore::c_order);
        auto indexedStore = m_store | tensorstore::AllDims().SizedInterval(indices, sizes);
        tensorstore::Read(indexedStore, tensorstore::UnownedToShared(arr)).value();
        
        FunctionResultValue<unsigned char*> result(buffer,
                                                   "",
                                                   true);
        return result;
    }
    
    FunctionResultValue<unsigned char*> result(NULL,
                                               "Should not get here",
                                               false);
    return result;
}
