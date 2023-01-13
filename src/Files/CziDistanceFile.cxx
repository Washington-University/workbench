
/*LICENSE_START*/
/*
 *  Copyright (C) 2022 Washington University School of Medicine
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

#define __CZI_DISTANCE_FILE_H_DECLARE__
#include "CziDistanceFile.h"
#undef __CZI_DISTANCE_FILE_H_DECLARE__

#include <algorithm>
#include <limits>

#include "CaretAssert.h"
#include "CaretLogger.h"
#include "DataFileException.h"
#include "ElapsedTimer.h"
#include "FileInformation.h"
#include "GraphicsPrimitiveV3fT2f.h"
#include "Matrix4x4.h"
#include "VolumeFile.h"

using namespace caret;

/**
 * \class caret::CziDistanceFile
 * \brief Loads and uses a NIFTI file containing distance data for image
 * \ingroup Files
 */

/**
 * Constructor.
 * @param filename
 *    Name of file
 */
CziDistanceFile::CziDistanceFile(const AString& filename)
: CaretObject(),
m_filename(filename)
{
    
}

/**
 * Destructor.
 */
CziDistanceFile::~CziDistanceFile()
{
}

/**
 * Does this distance file match the other distance file in dimensions and coordinates
 * @param rhs
 *    The other distance file
 * @param errorMessageOut
 *    Contains description of files do not mach
 * @return
 *   True if the files match, else false
 */
bool
CziDistanceFile::match(const CziDistanceFile& rhs,
                       AString& errorMessageOut) const
{
    errorMessageOut.clear();
    
    if ((m_status != Status::VALID)
        || (rhs.m_status != Status::VALID)) {
        errorMessageOut = "Distance File Comparison: One or both files are not valid (data not read)";
        return false;
    }

    CaretAssert(m_volumeFile);
    CaretAssert(rhs.m_volumeFile);
    
    std::vector<int64_t> myDims, rhsDims;
    m_volumeFile->getDimensions(myDims);
    rhs.m_volumeFile->getDimensions(rhsDims);
    if (myDims != rhsDims) {
        errorMessageOut = "Dimensions are different";
        return false;
    }
    
    const int64_t maxI(myDims[0]);
    const int64_t maxJ(myDims[1]);
    const int64_t maxK(myDims[2]);
    if ((maxI <= 0)
        || (maxJ <= 0)
        || (maxK != 1)) {
        errorMessageOut = "I and/or J dimension is zero OR K dimensions is not 1";
        return false;
    }

    const float tolerance(0.001);
    
    Vector3D myMin, rhsMin;
    m_volumeFile->indexToSpace(0, 0, 0, myMin);
    rhs.m_volumeFile->indexToSpace(0, 0, 0, rhsMin);
    if ((myMin - rhsMin).length() > tolerance) {
        errorMessageOut = "Different coordinates at corner with index (0, 0, 0)";
        return false;
    }
    
    Vector3D myMaxIJ, rhsMaxIJ;
    m_volumeFile->indexToSpace(    maxI - 1, maxJ - 1, 0, myMaxIJ);
    rhs.m_volumeFile->indexToSpace(maxI - 1, maxJ - 1, 0, rhsMaxIJ);
    if ((myMaxIJ - rhsMaxIJ).length() > tolerance) {
        errorMessageOut = "Different coordinates at corner with index (maxI - 1, maxJ - 1, 0)";
        return false;
    }
    
    return true;
}


/**
 * @return Status of the transform
 */
CziDistanceFile::Status
CziDistanceFile::getStatus() const
{
    return m_status;
}

/**
 * @return Name of file used by this non-linear transform
 */
AString
CziDistanceFile::getFilename() const
{
    return m_filename;
}

/**
 * Load the NIFTI file with the given name
 * @param filename
 *    Name of NIFTI file
 */
void
CziDistanceFile::load() const
{
    switch (m_status) {
        case Status::INVALID:
            return;
            break;
        case Status::UNREAD:
            break;
        case Status::VALID:
            return;
            break;
    }
    
    m_status = Status::INVALID;
    
    m_volumeFile.reset(new VolumeFile());
    try {
        m_volumeFile->readFile(m_filename);
        
        std::vector<int64_t> dims;
        m_volumeFile->getDimensions(dims);
        if (dims.size() < 5) {
            throw DataFileException("Dimensions should be 5 but are "
                                    + AString::number(dims.size()));
        }
        
        if (dims[3] != 1) {
            throw DataFileException("4th dimension should be 1 but is "
                                    + AString::number(dims[3]));
        }
        
        std::vector<std::vector<float>> sform(m_volumeFile->getSform());
        m_sformMatrix.reset(new Matrix4x4(sform));
        
        /*
         * Generate inverse of sform matrix
         */
        m_inverseSformMatrix.reset(new Matrix4x4(*m_sformMatrix));
        if ( ! m_inverseSformMatrix->invert()) {
            throw DataFileException("Failed to invert sform matrix: "
                                    + m_sformMatrix->toFormattedString("  "));
        }

        m_status = Status::VALID;
        
        if (s_debugFlag) {
            std::cout << getFilename() << std::endl;
            std::cout << "   Distance File W=" << dims[0] << ", H=" << dims[1] << std::endl;
            
            Vector3D planeTopLeft(0.0, 0.0, 0.0);
            m_sformMatrix->multiplyPoint3(planeTopLeft);
            std::cout << "   Plane top left: " << planeTopLeft.toString(5) << std::endl;
            
            Vector3D planeBottomRight(dims[0], dims[1], 0.0);
            m_sformMatrix->multiplyPoint3(planeBottomRight);
            std::cout << "   Plane bottom right: " << planeBottomRight.toString(5) << std::endl;
        }
    }
    catch (const DataFileException& dfe) {
        CaretLogWarning("Failed to read "
                        + m_filename
                        + " "
                        + " ERROR: "
                        + dfe.whatString());
        m_volumeFile.reset();
    }
}

/**
 * Get a pointer to the data
 * @param dataLengthOut
 *    Output with length of data
 * @return
 *    Pointer to data or NULL if not valid.
 */
const float*
CziDistanceFile::getDataPointer(int64_t& dataLengthOut) const
{
    if (m_status == Status::UNREAD) {
        load();
    }
    
    if (m_status == Status::VALID) {
        std::vector<int64_t> dims;
        m_volumeFile->getDimensions(dims);
        CaretAssert(dims.size() >= 3);
        dataLengthOut = (dims[0] * dims[1] * dims[2]);
        if (dataLengthOut > 0) {
            const float* pointer(m_volumeFile->getFrame(0));
            return pointer;
        }
    }

    /**
     * Invalid
     */
    dataLengthOut = 0;
    return NULL;
}

/**
 * @param planeXYZ
 *    The plane coordinate
 * @param distanceValueOut
 *    The output distance value
 * @return True if output distance value is valid, else false.
 */
bool
CziDistanceFile::getDistanceValue(const Vector3D& planeXYZ,
                                  float& distanceValueOut) const
{
    distanceValueOut = 0.0;
    
    if (m_status == Status::UNREAD) {
        load();
    }

    if (m_status == Status::VALID) {
        Vector3D indexIJK(planeXYZ);
        m_inverseSformMatrix->multiplyPoint3(indexIJK);
        const int64_t niftiI(static_cast<int64_t>(indexIJK[0]));
        const int64_t niftiJ(static_cast<int64_t>(indexIJK[1]));
        if (s_debugFlag) {
            std::cout << "TO millimeters Index: " << niftiI << ", " << niftiJ << std::endl;
        }
        
        const int64_t niftiK(0);
        if (m_volumeFile->indexValid(niftiI, niftiJ, niftiK)) {
            /*
             * Use pixel index to obtain non-linearity from NIFTI data
             */
            distanceValueOut = m_volumeFile->getValue(niftiI, niftiJ, niftiK, 0);
            return true;
        }
    }
    
    return false;
}


/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
CziDistanceFile::toString() const
{
    return "CziDistanceFile";
}

/**
 * Convert distance data to graphics primitives that can be used as a OpenGL stencil mask
 * @param distanceFiles
 *    The distance files.  Must all be same dimensions and coordinates.
 * @param maskingPrimitivesOut
 *    Output with pointers to masking primitives.  Caller takes ownership and must delete this data.
 * @param errorMessageOut
 *    Contains error information.
 * @return
 *    True if successful, else false.
 */
bool
CziDistanceFile::createMaskingPrimitives(const std::vector<const CziDistanceFile*>& distanceFiles,
                                         std::vector<GraphicsPrimitiveV3fT2f*>& maskingPrimitivesOut,
                                         AString& errorMessageOut)
{
    ElapsedTimer timer;
    timer.start();
    
    errorMessageOut.clear();
    maskingPrimitivesOut.clear();
    
    const int32_t numFiles(distanceFiles.size());
    if (numFiles <= 0) {
        errorMessageOut = "No distance files";
        return false;
    }
    CaretAssert(distanceFiles[0]);
    const CziDistanceFile* firstFile(distanceFiles[0]);
    CaretAssert(firstFile);
    
    /*
     * Verify second and other files have same dimensions as first file
     */
    for (int32_t iFile = 1; iFile < numFiles; iFile++) {
        CaretAssertVectorIndex(distanceFiles, iFile);
        CaretAssert(distanceFiles[iFile]);
        
        /*
         * Match will also fail if either files is invalid
         */
        if ( ! firstFile->match(*distanceFiles[iFile],
                                errorMessageOut)) {
            return false;
        }
    }
    
    /*
     * Get pointers to distance data
     */
    std::vector<const float*> distanceDataPointers;
    for (int32_t iFile = 0; iFile < numFiles; iFile++) {
        int64_t dataLength(0);
        const float* dataPtr(distanceFiles[iFile]->getDataPointer(dataLength));
        CaretAssert(dataPtr);
        if (dataPtr == NULL) {
            errorMessageOut = ("Distance file has invalid data pointer "
                               + distanceFiles[iFile]->getFilename());
            return false;
        }
        distanceDataPointers.push_back(dataPtr);
    }
    
    /*
     * Size of image
     */
    std::vector<int64_t> dims;
    firstFile->m_volumeFile->getDimensions(dims);
    const int64_t imageWidth(dims[0]);
    const int64_t imageHeight(dims[1]);
    const int64_t imageSlices(1);
    
    /*
     * Allocate image data with shared pointer for use with graphics primitive
     * (gets correct deleter)
     */
    const std::array<float, 4> textureBorderColorRGBA { 0.0, 0.0, 0.0, 0.0 };
    
    std::vector<std::shared_ptr<uint8_t>> imageDataPointers;
    for (int32_t iFile = 0; iFile < numFiles; iFile++) {
        int64_t numberOfBytesAllocated(-1);
        std::shared_ptr<uint8_t> imageData(GraphicsTextureSettings::allocateImageRgbaData(imageWidth,
                                                                                          imageHeight,
                                                                                          imageSlices,
                                                                                          &numberOfBytesAllocated));
        
        /*
         * Fill image data with zeros (mask off)
         */
        CaretAssert(numberOfBytesAllocated > 0);
        std::fill(imageData.get(),
                  imageData.get() + numberOfBytesAllocated,
                  0);
        
        imageDataPointers.push_back(imageData);
    }
    CaretAssert(numFiles == static_cast<int32_t>(imageDataPointers.size()));
    
    /*
     * Use distance data to create masks
     */
    const int64_t numDistanceBytes(imageWidth * imageHeight);
    for (int64_t iByte = 0; iByte < numDistanceBytes; iByte++) {
        float minValue(std::numeric_limits<float>::max());
        int32_t minFileIndex(-1);
        for (int32_t iFile = 0; iFile < numFiles; iFile++) {
            const float dataValue(distanceDataPointers[iFile][iByte]);
            if (dataValue < minValue) {
                minValue = dataValue;
                minFileIndex = iFile;
            }
        }
        
        if (minFileIndex >= 0) {
            const int32_t i4(iByte * 4);
            CaretAssertVectorIndex(imageDataPointers, minFileIndex);
            uint8_t* imageDataPtr(imageDataPointers[minFileIndex].get());
            imageDataPtr[i4]   = 255;
            imageDataPtr[i4+1] = 255;
            imageDataPtr[i4+2] = 255;
            imageDataPtr[i4+3] = 255;
        }
    }
    
    /*
     * Create coordinates
     */
    CaretAssert(firstFile);
    CaretAssert(firstFile->m_volumeFile);
    Vector3D planeTopLeft, planeTopRight, planeBottomLeft, planeBottomRight;
    const int64_t iMax(dims[0] - 1);
    const int64_t jMax(dims[1] - 1);
    firstFile->m_volumeFile->indexToSpace(0, 0, 0, planeTopLeft);
    firstFile->m_volumeFile->indexToSpace(iMax, 0, 0, planeTopRight);
    firstFile->m_volumeFile->indexToSpace(0, jMax, 0, planeBottomLeft);
    firstFile->m_volumeFile->indexToSpace(iMax, jMax, 0, planeBottomRight);

    /*
     * Create primitives
     */
    for (int32_t iFile = 0; iFile < numFiles; iFile++) {
        CaretAssertVectorIndex(imageDataPointers, iFile);
        CaretAssert(imageDataPointers[iFile]);
        GraphicsTextureSettings textureSettings(imageDataPointers[iFile],
                                                imageWidth,
                                                imageHeight,
                                                imageSlices,
                                                GraphicsTextureSettings::DimensionType::FLOAT_STR_2D,
                                                GraphicsTextureSettings::PixelFormatType::RGBA,
                                                GraphicsTextureSettings::PixelOrigin::TOP_LEFT,
                                                GraphicsTextureSettings::WrappingType::CLAMP_TO_BORDER,
                                                GraphicsTextureSettings::MipMappingType::ENABLED,
                                                GraphicsTextureSettings::CompressionType::DISABLED,
                                                GraphicsTextureMagnificationFilterEnum::LINEAR,
                                                GraphicsTextureMinificationFilterEnum::LINEAR,
                                                textureBorderColorRGBA);
        GraphicsPrimitiveV3fT2f* p(GraphicsPrimitive::newPrimitiveV3fT2f(GraphicsPrimitive::PrimitiveType::OPENGL_TRIANGLE_STRIP,
                                                                         textureSettings));
        
        /*
         * Coordinates for two triangles drawn as a triangle strip
         */
        p->addVertex(planeTopLeft, 0, 0);
        p->addVertex(planeBottomLeft, 0, 1);
        p->addVertex(planeTopRight, 1, 0);
        p->addVertex(planeBottomRight, 1, 1);
        
        maskingPrimitivesOut.push_back(p);
        
        const bool writeMasksFlag(false);
        if (writeMasksFlag) {
            const AString maskFileName("Mask_"
                                       + AString::number(iFile + 1)
                                       + ".png");
            AString errMsg;
            if ( ! p->exportTextureToImageFile(maskFileName, errMsg)) {
                std::cout << "Error writing mask file: " << errMsg << std::endl;
            }
        }
    }
    
    CaretAssert(maskingPrimitivesOut.size() == distanceFiles.size());
    
    if (s_debugFlag) {
        std::cout << "Time to create masking primitives: "
        << timer.getElapsedTimeSeconds() << " seconds" << std::endl;
    }
    
    return true;
}


