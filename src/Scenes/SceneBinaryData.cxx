
/*LICENSE_START*/
/*
 *  Copyright (C) 2026 Washington University School of Medicine
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

#define __SCENE_BINARY_DATA_DECLARE__
#include "SceneBinaryData.h"
#undef __SCENE_BINARY_DATA_DECLARE__

#include <cstring>

#include "CaretAssert.h"
#include "CaretLogger.h"

using namespace caret;
    
/**
 * \class caret::SceneBinaryData 
 * \brief Store binary data in a scene
 * \ingroup Scenes
 */

/**
 * Constructor for float vector
 * @param name
 *    Name of instance
 * @param binaryDataEncodedAsText
 *    Binary data encoded as text
 * @param encodingType
 *    Encoding of the binary data
 * @param uncompressedNumberOfBytes
 *    Number of bytes after data is uncompressed
 */
SceneBinaryData::SceneBinaryData(const AString& name,
                                 const AString& binaryDataEncodedAsText,
                                 const SceneBinaryEncodingTypeEnum::Enum encodingType,
                                 const int64_t uncompressedNumberOfBytes)
: SceneObject(name,
              SceneObjectContainerTypeEnum::SINGLE,
              SceneObjectDataTypeEnum::SCENE_BINARY_DATA)
{
    reset();
    
    m_binaryDataEncodedAsText         = binaryDataEncodedAsText;
    m_binaryEncoding                  = encodingType;
    m_binaryUncompressedNumberOfBytes = uncompressedNumberOfBytes;
}

/**
 * Constructor for float vector
 * @param name
 *    Name of instance
 * @param floatVector
 *    Float vector for binary data
 */
SceneBinaryData::SceneBinaryData(const AString& name,
                                 const std::vector<float>& floatVector)
: SceneObject(name,
              SceneObjectContainerTypeEnum::SINGLE,
              SceneObjectDataTypeEnum::SCENE_BINARY_DATA)
{
    reset();
    
    if ( ! floatVector.empty()) {
        encodeAsText((uint8_t*)floatVector.data(),
                     floatVector.size() * sizeof(float),
                     SceneBinaryEncodingTypeEnum::GZIP_BASE64);
    }
}

/**
 * Constructor.
 * @param name
 *    Name of instance
 * @param binaryData
 *    Pointer to binary data
 * @param numberOfBytes
 *    Number of bytes in binary data
 */
SceneBinaryData::SceneBinaryData(const AString& name,
                                 const uint8_t* binaryData,
                                 const int64_t numberOfBytes)
: SceneObject(name,
              SceneObjectContainerTypeEnum::SINGLE,
              SceneObjectDataTypeEnum::SCENE_BINARY_DATA)
{
    reset();
    
    if ( (binaryData != NULL)
        && (numberOfBytes > 0)) {
        encodeAsText(binaryData,
                     numberOfBytes,
                     SceneBinaryEncodingTypeEnum::GZIP_BASE64);
    }
}

/**
 * Destructor.
 */
SceneBinaryData::~SceneBinaryData()
{
}

/**
 * Copy constructor.
 * @param rhs
 *    Object that is copied.
 */
SceneBinaryData::SceneBinaryData(const SceneBinaryData& rhs)
: SceneObject(rhs.getName(),
              SceneObjectContainerTypeEnum::SINGLE,
              SceneObjectDataTypeEnum::SCENE_BINARY_DATA)
{
    this->copyHelperSceneBinaryData(rhs);
}

/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void 
SceneBinaryData::copyHelperSceneBinaryData(const SceneBinaryData& obj)
{
    m_binaryDataEncodedAsText         = obj.m_binaryDataEncodedAsText;
    m_binaryEncoding                  = obj.m_binaryEncoding;
    m_binaryUncompressedNumberOfBytes = obj.m_binaryUncompressedNumberOfBytes;

    
    
    m_binaryDataEncodedAsText = obj.m_binaryDataEncodedAsText;
    m_binaryEncoding          = obj.m_binaryEncoding;
}

/**
 * @return Copy of this instance
 */
SceneObject*
SceneBinaryData::clone() const
{
    return new SceneBinaryData(*this);
}

/**
 * Cast an instance of SceneObject to a SceneBinaryData.
 * Is used to avoid dynamic casting and overridden by the class.
 *
 * @return Valid pointer (non-NULL) this is SceneBinaryData
 */
SceneBinaryData*
SceneBinaryData::castToSceneBinaryData()
{
    return this;
}

/**
 * Cast an instance of SceneObject to a SceneBinaryData.
 * Is used to avoid dynamic casting and overridden by the class.
 *
 * @return Valid pointer (non-NULL) this is SceneBinaryData
 */
const SceneBinaryData*
SceneBinaryData::castToSceneBinaryData() const
{
    return this;
}

/**
 * @return The binary encoding
 */
SceneBinaryEncodingTypeEnum::Enum
SceneBinaryData::getBinaryEncoding() const
{
    return m_binaryEncoding;
}

/**
 * @return The binary data encoded as text.
 */
AString
SceneBinaryData::getBinaryDataEncodedAsText() const
{
    return m_binaryDataEncodedAsText;
}

/**
 * @return Number of bytes expected when uncompressed
 */
int64_t
SceneBinaryData::getBinaryUncompressedNumberOfBytes() const
{
    return m_binaryUncompressedNumberOfBytes;
}

/**
 * @return The binary data in a float vector
 */
std::vector<float>
SceneBinaryData::getAsFloatVector() const
{
    std::vector<float> floatVectorOut;
    
    const int64_t numFloat(m_binaryUncompressedNumberOfBytes / sizeof(float));
    if (numFloat > 0) {
        floatVectorOut.resize(numFloat);
        FunctionResult result(decodeFromText((uint8_t*)floatVectorOut.data()));
        if ( result.isError()) {
            floatVectorOut.clear();
            CaretLogWarning(result.getErrorMessage());
        }
    }
    
    return floatVectorOut;
}

/**
 * Decode text into binary data
 * @param dataPointer
 *    Pointer to where decoded binary data is placed
 */
FunctionResult
SceneBinaryData::decodeFromText(uint8_t* dataPointer) const
{
    CaretAssert(dataPointer);
    
    AString errorMessage;
    
    if (m_binaryDataEncodedAsText.isEmpty()) {
        errorMessage.appendWithNewLine("Text for decoding is empty.");
    }
    if (m_binaryUncompressedNumberOfBytes <= 0) {
        errorMessage.appendWithNewLine("Uncompressed number of bytes is invalid.");
    }
    switch (m_binaryEncoding) {
        case SceneBinaryEncodingTypeEnum::INVALID:
            errorMessage.appendWithNewLine("Text binary encoding is invalid.");
            break;
        case SceneBinaryEncodingTypeEnum::GZIP_BASE64:
            break;
    }
    if ( ! errorMessage.isEmpty()) {
        errorMessage.insert(0,
                            ("SceneBinary named="
                             + getName()
                             + "\n"));
        return FunctionResult::error(errorMessage);
    }
    
    switch (m_binaryEncoding) {
        case SceneBinaryEncodingTypeEnum::INVALID:
            CaretAssert(0);
            break;
        case SceneBinaryEncodingTypeEnum::GZIP_BASE64:
        {
            /*
             * Convert from base64 to binary
             */
            const QByteArray binaryData(QByteArray::fromBase64(m_binaryDataEncodedAsText.toUtf8()));
            const QByteArray uncompressedBytes(qUncompress(binaryData));
            if (m_binaryUncompressedNumberOfBytes == uncompressedBytes.size()) {
                std::memcpy(dataPointer,                /* destination */
                            uncompressedBytes.data(),   /* source */
                            uncompressedBytes.size());
            }
            else {
                errorMessage = ("Failed to uncompress binary data.  "
                                "Uncompressed expected size="
                                + AString::number(m_binaryUncompressedNumberOfBytes)
                                + " but got "
                                + AString::number(uncompressedBytes.size())
                                + " for SceneBinaryData named="
                                + getName());
                return FunctionResult::error(errorMessage);
                
            }
            
        }
            break;
    }
    
    return FunctionResult::ok();
}

/**
 * Encode the given binary data as text
 * @param dataPointer
 *    Pointer to data
 * @param numberOfBytes
 *    Number of bytes in data
 */
void
SceneBinaryData::encodeAsText(const uint8_t* dataPointer,
                              const int64_t numberOfBytes,
                              const SceneBinaryEncodingTypeEnum::Enum encodingType)
{
    reset();

    if ( (dataPointer != NULL)
        && (numberOfBytes > 0) ) {
        switch (encodingType) {
            case SceneBinaryEncodingTypeEnum::INVALID:
                CaretLogSevere("Binary encoding is invalid.");
                CaretAssert(0);
                break;
            case SceneBinaryEncodingTypeEnum::GZIP_BASE64:
            {
                /*
                 * Compress the data loaded and encode in base64
                 * for saving to the scene as a string.
                 */
                const QByteArray compressedData(qCompress((const uchar*)dataPointer,
                                                          numberOfBytes));
                const QByteArray base64Data(compressedData.toBase64());
                m_binaryDataEncodedAsText         = base64Data;
                m_binaryEncoding                  = encodingType;
                m_binaryUncompressedNumberOfBytes = numberOfBytes;
            }
                break;
        }
    }
}


/**
 * Reset this instance
 */
void
SceneBinaryData::reset()
{
    m_binaryDataEncodedAsText = "";
    m_binaryUncompressedNumberOfBytes = 0;
    m_binaryEncoding = SceneBinaryEncodingTypeEnum::GZIP_BASE64;
}



