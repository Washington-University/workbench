
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

#define __ZARR_V2_ARRAY_JSON_FILE_DECLARE__
#include "ZarrV2ArrayJsonFile.h"
#undef __ZARR_V2_ARRAY_JSON_FILE_DECLARE__

#include <cmath>
#include <iostream>
#include <sstream>

#include <nlohmann/json.hpp>

#include "CaretAssert.h"
using namespace caret;


    
/**
 * \class caret::ZarrV2ArrayJsonFile 
 * \brief Group JSON file for Zarr Version 2
 * \ingroup OmeZarr
 *
 * https://zarr-specs.readthedocs.io/en/latest/v2/v2.0.html
 */

/**
 * Constructor.
 */
ZarrV2ArrayJsonFile::ZarrV2ArrayJsonFile()
: ZarrJsonFileBase()
{
    
}

/**
 * Destructor.
 */
ZarrV2ArrayJsonFile::~ZarrV2ArrayJsonFile()
{
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
ZarrV2ArrayJsonFile::ZarrV2ArrayJsonFile(const ZarrV2ArrayJsonFile& obj)
: ZarrJsonFileBase(obj)
{
    this->copyHelperZarrV2ArrayJsonFile(obj);
}

/**
 * Assignment operator.
 * @param obj
 *    Data copied from obj to this.
 * @return 
 *    Reference to this object.
 */
ZarrV2ArrayJsonFile&
ZarrV2ArrayJsonFile::operator=(const ZarrV2ArrayJsonFile& obj)
{
    if (this != &obj) {
        ZarrJsonFileBase::operator=(obj);
        this->copyHelperZarrV2ArrayJsonFile(obj);
    }
    return *this;    
}

/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void 
ZarrV2ArrayJsonFile::copyHelperZarrV2ArrayJsonFile(const ZarrV2ArrayJsonFile& obj)
{
    m_chunkSizes = obj.m_chunkSizes;
    
    m_shapeSizes = obj.m_shapeSizes;
    
    m_zarrFormat = obj.m_zarrFormat;
    
    m_fillValue = obj.m_fillValue;
    
    m_dimensionSeparator = obj.m_dimensionSeparator;
    
    m_dataType  = obj.m_dataType;
    
    m_dataTypeByteOrder = obj.m_dataTypeByteOrder;
    
    m_dataTypeLength = obj.m_dataTypeLength;
    
    m_rowColumnMajorOrderType = obj.m_rowColumnMajorOrderType;
    
    m_compressorType = obj.m_compressorType;
    
    m_compressorParametersMap = obj.m_compressorParametersMap;
}

/**
 * Clear the file
 */
void
ZarrV2ArrayJsonFile::clear()
{
    ZarrJsonFileBase::clear();
    m_chunkSizes.clear();
    
    m_shapeSizes.clear();
    
    m_zarrFormat = -1;
    
    m_fillValue = -1;
    
    m_dimensionSeparator = ZarrDimensionSeparatorEnum::DOT;
    
    m_dataType  = ZarrDataTypeEnum::UNKNOWN;
    
    m_dataTypeByteOrder = ZarrDataTypeByteOrderEnum::UNKNOWN;
    
    m_dataTypeLength = -1;
    
    m_rowColumnMajorOrderType = ZarrRowColumnMajorOrderTypeEnum::UNKNOWN;
    
    m_compressorType = ZarrCompressorTypeEnum::NO_COMPRESSOR;
    
    m_compressorParametersMap.clear();
}

/**
 * @return The chunk sizes
 */
std::vector<int64_t>
ZarrV2ArrayJsonFile::getChunkSizes() const
{
    return m_chunkSizes;
}

/**
 * Set the chunk sizes
 * @param chunkSizes
 *    Dimensions of chunks
 */
void
ZarrV2ArrayJsonFile::setChunkSizes(const std::vector<int64_t>& chunkSizes)
{
    m_chunkSizes = chunkSizes;
}

/**
 * @return The shape sizes (dimensions)
 */
std::vector<int64_t>
ZarrV2ArrayJsonFile::getShapeSizes() const
{
    return m_shapeSizes;
}

/**
 * Set the dimensions
 * @param shapeSizes
 *    Dimensions of shape
 */
void
ZarrV2ArrayJsonFile::setShapeSizes(const std::vector<int64_t>& shapeSizes)
{
    m_shapeSizes = shapeSizes;
}

/**
 * @return The Zarr format number
 */
int32_t
ZarrV2ArrayJsonFile::getZarrFormat() const
{
    return m_zarrFormat;
}

/**
 * Set the Zarr format
 * @param zarrFormat
 *    New value for zarr format
 */
void
ZarrV2ArrayJsonFile::setZarrFormat(const int32_t zarrFormat)
{
    m_zarrFormat = zarrFormat;
}

/**
 * @return The fill value for undefined data
 */
float
ZarrV2ArrayJsonFile::getFillValue() const
{
    return m_fillValue;
}

/**
 * Set the fill value
 * @param fillValue
 *    New fill value
 */
void
ZarrV2ArrayJsonFile::setFillValue(const float fillValue)
{
    m_fillValue = fillValue;
}

/**
 * @return The separator for dimensions
 */
ZarrDimensionSeparatorEnum::Enum
ZarrV2ArrayJsonFile::getDimensionSeparator() const
{
    return m_dimensionSeparator;
}

/**
 * Set the dimension separator
 * @param dimensionSeparator
 *    The separator
 */
void
ZarrV2ArrayJsonFile::setDimensionSeparator(const ZarrDimensionSeparatorEnum::Enum dimensionSeparator)
{
    m_dimensionSeparator = dimensionSeparator;
}

/**
 * @return The data type
 */
ZarrDataTypeEnum::Enum
ZarrV2ArrayJsonFile::getDataType() const
{
    return m_dataType;
}

/**
 * Set the data type
 * @param dataType
 */
void
ZarrV2ArrayJsonFile::setDataType(const ZarrDataTypeEnum::Enum dataType)
{
    m_dataType = dataType;
}

/**
 * @return The data type byte ordering
 */
ZarrDataTypeByteOrderEnum::Enum
ZarrV2ArrayJsonFile::getDataTypeByteOrder() const
{
    return m_dataTypeByteOrder;
}

/**
 * Set the data type byte ordering
 * @param dataTypeByteOrder
 *    The byte ordering
 */
void
ZarrV2ArrayJsonFile::setDataTypeByteOrder(const ZarrDataTypeByteOrderEnum::Enum dataTypeByteOrder)
{
    m_dataTypeByteOrder = dataTypeByteOrder;
}

/**
 * @return The data type length
 */
int32_t
ZarrV2ArrayJsonFile::getDataTypeLength() const
{
    return m_dataTypeLength;
}

/**
 * Set data type length
 * @param dataTypeLength
 *    The data type length
 */
void
ZarrV2ArrayJsonFile::setDataTypeLength(const int32_t dataTypeLength)
{
    m_dataTypeLength = dataTypeLength;
}

/**
 * @return The row/column major order type
 */
ZarrRowColumnMajorOrderTypeEnum::Enum
ZarrV2ArrayJsonFile::getRowColumnMajorOrderType() const
{
    return m_rowColumnMajorOrderType;
}

/**
 * Set the row/column major order type
 * @param rowColumnMajorOrderType
 *    The new type
 */
void
ZarrV2ArrayJsonFile::setRowColumnMajorOrderType(const ZarrRowColumnMajorOrderTypeEnum::Enum rowColumnMajorOrderType)
{
    m_rowColumnMajorOrderType = rowColumnMajorOrderType;
}

/**
 * @return the compressor type
 */
ZarrCompressorTypeEnum::Enum
ZarrV2ArrayJsonFile::getCompressorType() const
{
    return m_compressorType;
}

const std::map<AString, AString>&
ZarrV2ArrayJsonFile::getCompressorParameters() const
{
    return m_compressorParametersMap;
}

/**
 * Set the compressor info
 * @param compressorType
 *    Type of compressor
 * @param compressorParameters
 *    The compressor parameters
 */
void
ZarrV2ArrayJsonFile::setCompressorInfo(const ZarrCompressorTypeEnum::Enum compressorType,
                             const std::map<AString, AString>& compressorParameters)
{
    m_compressorType          = compressorType;
    m_compressorParametersMap = compressorParameters;
}

/**
 * Must be override by child classes to read the file's content from JSON
 * @param json
 *    The JSON containing the file's content
 * @return
 *    Function result with Ok/Error.
 */
FunctionResult
ZarrV2ArrayJsonFile::readContentFromJson(const nlohmann::json& json)
{
    const nlohmann::json::object_t jsonObject = json;
    
    clear();
    
    m_zarrFormat = -1;
    
    /*
     * Spec states dimension separator is option and default is "."
     */
    setDimensionSeparator(ZarrDimensionSeparatorEnum::DOT);
    
    AString errorMessage;
    
    for (auto& elem : jsonObject) {
        const std::string key(elem.first);

        if (key == "chunks") {
            if (elem.second.is_array()) {
                std::vector<int64_t> chunkSizes;
                for (const auto& value : elem.second) {
                    chunkSizes.push_back(value.get<int32_t>());
                }
                setChunkSizes(chunkSizes);
            }
            else {
                errorMessage.appendWithNewLine("Value for chunks is not an array.");
            }
        }
        else if (key == "compressor") {
            if (elem.second.is_object()) {
                const nlohmann::json::object_t compressorObject(elem.second);
                ZarrCompressorTypeEnum::Enum compressorType(ZarrCompressorTypeEnum::NO_COMPRESSOR);
                typeof(m_compressorParametersMap) paramsMap;
                bool compressorTypeValidFlag(false);
                for (const auto& compElem: compressorObject) {
                    const auto& key(compElem.first);
                    if (key == "id") {
                        compressorType = ZarrCompressorTypeEnum::fromEncodingName(compElem.second.get<std::string>(),
                                                                                  &compressorTypeValidFlag);
                    }
                    else {
                        std::string value;
                        if (compElem.second.is_number()) {
                            const int32_t v(compElem.second.get<float>());
                            std::ostringstream s;
                            s << v;
                            value = s.str();
                        }
                        else if (compElem.second.is_string()) {
                            value = compElem.second.get<std::string>();
                        }
                        paramsMap.insert(std::make_pair(key,
                                                        value));
                    }
                }
                if (compressorTypeValidFlag) {
                    setCompressorInfo(compressorType,
                                      paramsMap);
                }
                else {
                    errorMessage.appendWithNewLine("Value for compressor id is not recognized as valid compressor type.");
                }
            }
            else if (elem.second.is_null()){
                
            }
            else {
                errorMessage.appendWithNewLine("Value for compressor is neither object nor null.");
            }
        }
        else if (key == "dimension_separator") {
            if (elem.second.is_string()) {
                const std::string separator(elem.second);
                bool validFlag(false);
                setDimensionSeparator(ZarrDimensionSeparatorEnum::fromEncoding(separator,
                                                                               &validFlag));
                if ( ! validFlag) {
                    std::cerr << key << " \"" << separator << "\" is invalid.  Using \""
                    << ZarrDimensionSeparatorEnum::toEncoding(getDimensionSeparator()) << "\"." << std::endl;
                }
            }
            else {
                errorMessage.appendWithNewLine("Value for dimension_separator is not a string.");
            }
        }
        else if (key == "dtype") {
            if (elem.second.is_string()) {
                const std::string dtypeString(elem.second.get<std::string>());
                const int32_t numChars(dtypeString.length());
                if (numChars >= 3) {
                    if (dtypeString[0] == '[') {
                        errorMessage.appendWithNewLine("Structured dtype is not supported.");
                    }
                    else {
                        /*
                         * Examine endian
                         */
                        setDataTypeByteOrder(ZarrDataTypeByteOrderEnum::UNKNOWN);
                        const char endianChar(dtypeString[0]);
                        switch (endianChar) {
                            case '<':
                                setDataTypeByteOrder(ZarrDataTypeByteOrderEnum::ENDIAN_LITTLE);
                                break;
                            case '>':
                                setDataTypeByteOrder(ZarrDataTypeByteOrderEnum::ENDIAN_BIG);
                                break;
                            case '|':
                                setDataTypeByteOrder(ZarrDataTypeByteOrderEnum::NOT_RELEVANT);
                                break;
                            case '[':
                                errorMessage.appendWithNewLine("dtype is a structured data type and not supported.");
                                break;
                            default:
                                errorMessage.appendWithNewLine("First chacter of dtype is not one of \"<>|\".");
                                break;
                        }
                        
                        /*
                         * Examine data type
                         */
                        enum class DTYPE { D_NOT_SUPPORTED, D_INT, D_UINT, D_FLOAT };
                        DTYPE dType(DTYPE::D_NOT_SUPPORTED);
                        const char basicTypeChar(dtypeString[1]);
                        switch (basicTypeChar) {
                            case 'b':
                                break;
                            case 'i':
                                dType = DTYPE::D_INT;
                                break;
                            case 'u':
                                dType = DTYPE::D_UINT;
                                break;
                            case 'f':
                                dType = DTYPE::D_FLOAT;
                                break;
                            case 'c':
                                break;
                            case 'm':
                                break;
                            case 'M':
                                break;
                            case 'S':
                                break;
                            case 'U':
                                break;
                            case 'V':
                                break;
                            default:
                                errorMessage.appendWithNewLine("Second chacter of dtype is not one of \"biufcmMSUV\".");
                                break;
                        }
                        if ((getDataTypeByteOrder() != ZarrDataTypeByteOrderEnum::UNKNOWN)
                            && (dType != DTYPE::D_NOT_SUPPORTED)) {
                            const std::string numBytesString(dtypeString.substr(2));
                            const int32_t numBytes(std::stoi(numBytesString));
                            setDataType(ZarrDataTypeEnum::UNKNOWN);
                            switch (numBytes) {
                                case 1:
                                    switch (dType) {
                                        case DTYPE::D_NOT_SUPPORTED:
                                            break;
                                        case DTYPE::D_INT:
                                            setDataType(ZarrDataTypeEnum::INT_8);
                                            break;
                                        case DTYPE::D_UINT:
                                            setDataType(ZarrDataTypeEnum::UINT_8);
                                            break;
                                        case DTYPE::D_FLOAT:
                                            errorMessage.appendWithNewLine("8 bit FLOAT dtype not supported.");
                                            break;
                                    }
                                    break;
                                case 2:
                                    switch (dType) {
                                        case DTYPE::D_NOT_SUPPORTED:
                                            break;
                                        case DTYPE::D_INT:
                                            setDataType(ZarrDataTypeEnum::INT_16);
                                            break;
                                        case DTYPE::D_UINT:
                                            setDataType(ZarrDataTypeEnum::UINT_16);
                                            break;
                                        case DTYPE::D_FLOAT:
                                            errorMessage.appendWithNewLine("8 bit FLOAT dtype not supported.");
                                            break;
                                    }
                                    break;
                                case 4:
                                    switch (dType) {
                                        case DTYPE::D_NOT_SUPPORTED:
                                            break;
                                        case DTYPE::D_INT:
                                            setDataType(ZarrDataTypeEnum::INT_32);
                                            break;
                                        case DTYPE::D_UINT:
                                            setDataType(ZarrDataTypeEnum::UINT_32);
                                            break;
                                        case DTYPE::D_FLOAT:
                                            setDataType(ZarrDataTypeEnum::FLOAT_32);
                                            break;
                                    }
                                    break;
                                default:
                                    errorMessage.appendWithNewLine("length in dtype is not supported.");
                                    break;
                            }
                            if (getDataType() == ZarrDataTypeEnum::UNKNOWN) {
                                errorMessage.appendWithNewLine("dtype is not supported.");
                            }
                        }
                        else {
                            errorMessage.appendWithNewLine("dtype is not supported.");
                        }
                    }
                }
                else {
                    errorMessage.appendWithNewLine("Value for dtype must be at least three characters.");
                }
            }
            else {
                errorMessage.appendWithNewLine("Value for dtype is not a string.");
            }
        }
        else if (key == "fill_value") {
            if (elem.second.is_null()) {
                setFillValue(0.0f);
            }
            else if (elem.second.is_number()) {
                setFillValue(elem.second.get<float>());
                
            }
            else if (elem.second.is_string()) {
                const std::string fillString(elem.second);
                if (fillString == "NaN") {
                    errorMessage.appendWithNewLine("Nan not supported for fill_value");
                }
                else if (fillString == "Infinity") {
                    errorMessage.appendWithNewLine("Infinity not supported for fill_value");
                }
                else if (fillString == "-Infinity") {
                    errorMessage.appendWithNewLine("-Infinity not supported for fill_vvalue");
                }
                else {
                    errorMessage.append("fill_value not supported.");
                }
            }
        }
        else if (key == "filters") {
            if (elem.second.is_null()) {
                
            }
            else {
                
            }
        }
        else if (key == "order") {
            if (elem.second.is_string()) {
                const std::string orderText(elem.second);
                bool validFlag(false);
                setRowColumnMajorOrderType(ZarrRowColumnMajorOrderTypeEnum::fromEncoding(orderText,
                                                                                         &validFlag));
                if ( ! validFlag) {
                    std::cerr << key << " \"" << orderText << "\" is invalid.  Using \""
                    << ZarrRowColumnMajorOrderTypeEnum::toEncoding(getRowColumnMajorOrderType()) << "\"." << std::endl;
                }
            }
            else {
                errorMessage.appendWithNewLine("Value for order is not a string");
            }
        }
        else if (key == "shape") {
            if (elem.second.is_array()) {
                std::vector<int64_t> shapeSizes;
                for (const auto& value : elem.second) {
                    shapeSizes.push_back(value.get<int64_t>());
                }
                setShapeSizes(shapeSizes);
            }
            else {
                errorMessage.appendWithNewLine("Value for shape is not an array.");
            }
        }
        else if (key == "zarr_format") {
            if (elem.second.is_number()) {
                setZarrFormat(elem.second.get<int>());
                if (getZarrFormat() != 2) {
                    errorMessage.appendWithNewLine("zarr_format MUST BE 2");
                }
            }
            else {
                errorMessage.appendWithNewLine("Value for zarr_format is not a number.");
            }
        }
        else {
            errorMessage.appendWithNewLine("Unrecognized name \"" + key + "\" ignored.");
        }
    }

    const bool debugFlag(false);
    if (debugFlag) {
        std::cout << ".zarray content: " << std::endl;
        std::cout << this->toString() << std::endl;
    }
    
    return FunctionResult(errorMessage,
                          errorMessage.isEmpty());
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString
ZarrV2ArrayJsonFile::toString() const
{
    QString msg;
    
    msg += ("chunks=");
    for (const auto& c : m_chunkSizes) {
        msg += (" " + QString::number(c));
    }
    msg += "\n";
    
    msg += ("shape=");
    for (const auto& s : m_shapeSizes) {
        msg += (" " + QString::number(s));
    }
    msg += "\n";
    
    msg += ("zarr_format=" + QString::number(m_zarrFormat) + "\n");
    
    msg += ("fill_value=" + QString::number(m_fillValue) + "\n");
    
    msg += ("dimension_separator=" + ZarrDimensionSeparatorEnum::toGuiName(m_dimensionSeparator) + "\n");
    
    msg += ("dtype: basic type=" + ZarrDataTypeEnum::toGuiName(m_dataType) + "\n");
    
    msg += ("dtype: byte order=" + ZarrDataTypeByteOrderEnum::toGuiName(m_dataTypeByteOrder) + "\n");
    
    msg += ("dtype: length=" + QString::number(m_dataTypeLength) + "\n");
    
    msg += ("order=" + ZarrRowColumnMajorOrderTypeEnum::toGuiName(m_rowColumnMajorOrderType) + "\n");
    
    msg += ("compressor type=" + ZarrCompressorTypeEnum::toEncodingName(m_compressorType) + "\n");
    for (const auto& compIter : m_compressorParametersMap) {
        msg += ("    compressor parameter: " + compIter.first
                + " value: " + compIter.second + "\n");
    }
    return msg;
}


