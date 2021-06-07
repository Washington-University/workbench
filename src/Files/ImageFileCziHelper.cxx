
/*LICENSE_START*/
/*
 *  Copyright (C) 2021 Washington University School of Medicine
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

#define __IMAGE_FILE_CZI_HELPER_DECLARE__
#include "ImageFileCziHelper.h"
#undef __IMAGE_FILE_CZI_HELPER_DECLARE__

#include <algorithm>

#include <QFile>
#include <QImage>

#include "CaretAssert.h"
#include "DataFileException.h"
using namespace caret;

static bool cziDebugFlag(false);
    
/**
 * \class caret::ImageFileCziHelper 
 * \brief Helper for reading CZI files
 * \ingroup Files
 */

/**
 * Constructor.
 */
ImageFileCziHelper::ImageFileCziHelper()
: CaretObject()
{
    
}

/**
 * Destructor.
 */
ImageFileCziHelper::~ImageFileCziHelper()
{
}

/**
 * Read a CZI file.
 * @param filename
 * @param errorMessageOut
 * @return Pointer to a QImage containing image that was read of NULL if there was an error.
 */
QImage*
ImageFileCziHelper::readFile(const AString& filename,
                             AString& errorMessageOut)
{
    errorMessageOut.clear();
    QImage* imageOut(NULL);
    
    if (filename.isEmpty()) {
        errorMessageOut = "Filename is empty";
        return imageOut;
    }
    if ( ! QFile::exists(filename)) {
        errorMessageOut = (filename + " does not exist");
        return imageOut;
    }
    
    try {
        auto stream = libCZI::CreateStreamFromFile(filename.toStdWString().c_str());
        CaretAssert(stream);
        auto reader = libCZI::CreateCZIReader();
        CaretAssert(reader);
        reader->Open(stream);
        auto subBlockStatistics = reader->GetStatistics();
        
        auto roi = subBlockStatistics.boundingBox;
        if (cziDebugFlag) std::cout << "CZI Bounding Box (x,y,w,h): " << roi.x << " " << roi.y << " " << roi.w << " " << roi.h << std::endl;
        
        if ((roi.w <= 0)
            || (roi.h <= 0)) {
            errorMessageOut = (filename + " has invalid width/height");
            return imageOut;
        }
        
        roi.x += (roi.w / 2);
        roi.y += (roi.h / 2);
        if (roi.w > 4096) {
            roi.w = 4096;
        }
        if (roi.h > 4096) {
            roi.h = 4096;
        }
        if (cziDebugFlag) std::cout << "   Read Bounding Box (x,y,w,h): " << roi.x << " " << roi.y << " " << roi.w << " " << roi.h << std::endl;
        
        libCZI::CDimCoordinate coordinate;
        coordinate.Set(libCZI::DimensionIndex::C, 0);
        
        libCZI::ISingleChannelTileAccessor::Options sctaOptions;
        sctaOptions.Clear();
        sctaOptions.sortByM = true;
        sctaOptions.drawTileBorder = false;
        
        auto accessor = reader->CreateSingleChannelTileAccessor();
        auto bitmapData = accessor->Get(roi, &coordinate, &sctaOptions);
        
        imageOut = createImageData(bitmapData.get(),
                                   filename,
                                   errorMessageOut);
    }
    catch (const std::exception& e) {
        errorMessageOut = (filename + QString(e.what()));
    }
    
    if ( ! errorMessageOut.isEmpty()) {
        if (imageOut != NULL) {
            delete imageOut;
        }
    }
    
    return imageOut;
}

/**
 * Read a CZI file.
 * @param filename
 * @param errorMessageOut
 * @return Pointer to a QImage containing image that was read of NULL if there was an error.
 */
QImage*
ImageFileCziHelper::readFileScaled(const AString& filename,
                                   const int32_t maxWidthHeightInPixels,
                                   AString& errorMessageOut)
{
    errorMessageOut.clear();
    QImage* imageOut(NULL);
    
    if (filename.isEmpty()) {
        errorMessageOut = "Filename is empty";
        return imageOut;
    }
    if ( ! QFile::exists(filename)) {
        errorMessageOut = (filename + " does not exist");
        return imageOut;
    }
    
    try {
        auto stream = libCZI::CreateStreamFromFile(filename.toStdWString().c_str());
        CaretAssert(stream);
        auto reader = libCZI::CreateCZIReader();
        CaretAssert(reader);
        reader->Open(stream);
        auto subBlockStatistics = reader->GetStatistics();
        
        auto roi = subBlockStatistics.boundingBox;
        if (cziDebugFlag) std::cout << "CZI Bounding Box (x,y,w,h): " << roi.x << " " << roi.y << " " << roi.w << " " << roi.h << std::endl;
        
        if ((roi.w <= 0)
            || (roi.h <= 0)) {
            errorMessageOut = (filename + " has invalid width/height");
            return imageOut;
        }
        
        float zoom(1.0);
        const auto maxWidthHeight(std::max(roi.w, roi.h));
        if (maxWidthHeightInPixels < maxWidthHeight) {
            zoom = static_cast<float>(maxWidthHeightInPixels) / static_cast<float>(maxWidthHeight);
        }
        
        libCZI::CDimCoordinate coordinate;
        coordinate.Set(libCZI::DimensionIndex::C, 0);
        
        libCZI::ISingleChannelScalingTileAccessor::Options scstaOptions; scstaOptions.Clear();
        scstaOptions.backGroundColor.r = 0.0;
        scstaOptions.backGroundColor.g = 0.0;
        scstaOptions.backGroundColor.b = 0.0;

        auto accessor = reader->CreateSingleChannelScalingTileAccessor();
        auto bitmapData = accessor->Get(roi, &coordinate, zoom, &scstaOptions);
        
        imageOut = createImageData(bitmapData.get(),
                                   filename,
                                   errorMessageOut);
    }
    catch (const std::exception& e) {
        errorMessageOut = (filename + QString(e.what()));
    }
    
    if ( ! errorMessageOut.isEmpty()) {
        if (imageOut != NULL) {
            delete imageOut;
        }
    }
    
    return imageOut;
}

/**
 * Create image data from the given bit map image data
 * @param bitmapData
 *    The bitmap data containing image pixels
 * @param filename
 *    Name of file.
 * @param errorMessageOut
 *    Output with error information
 * @return Pointer to QImage or NULL if there is an error.
 */
QImage*
ImageFileCziHelper::createImageData(libCZI::IBitmapData* bitmapData,
                                    const AString& filename,
                                    AString& errorMessageOut)
{
    CaretAssert(bitmapData);
    errorMessageOut.clear();
    QImage* imageOut(NULL);
    
    const auto width(bitmapData->GetWidth());
    const auto height(bitmapData->GetHeight());
    if (cziDebugFlag) std::cout << "Image width/height: " << width << ", " << height << std::endl;
    
    if ((width <= 0)
        || (height <= 0)) {
        errorMessageOut = (filename
                           + " data has invalid width or height");
        return imageOut;
    }
    std::vector<unsigned char> imageData(width * height * 4);
    auto bitMapInfo = bitmapData->Lock();
    if (cziDebugFlag) std::cout << "   Stride: " << bitMapInfo.stride << std::endl;
    if (cziDebugFlag) std::cout << "   Size: " << bitMapInfo.size << std::endl;
    
    AString colorName;
    switch (bitmapData->GetPixelType()){
        case libCZI::PixelType::Bgr24:
            colorName = "Bgr24";
            break;
        case libCZI::PixelType::Bgr48:
            colorName = "Bgr48";
            break;
        case libCZI::PixelType::Gray16:
            colorName = "Gray16";
            break;
        case libCZI::PixelType::Gray8:
            colorName = "Gray8";
            break;
        case libCZI::PixelType::Bgra32:
            colorName = "Bgra32";
            break;
        default:
            colorName = "Invalid";
            errorMessageOut = (filename
                               +  " pixeltype "
                               + AString::number((int)bitmapData->GetPixelType())
                               + " not supported/unknown");
            if (imageOut != NULL) {
                delete imageOut;
            }
            return imageOut;
            break;
    }
    if (cziDebugFlag) std::cout << "   Color: " << colorName << std::endl << std::flush;
    
    unsigned char* cziPtr8 = (unsigned char*)bitMapInfo.ptrDataRoi;
    unsigned short* cziPtr16 = (unsigned short*)bitMapInfo.ptrDataRoi;
    
    /*
     * Documentation for QImage states that setPixel may be very costly
     * and recommends using the scanLine() method to access pixel data.
     */
    imageOut = new QImage(width,
                          height,
                          QImage::Format_ARGB32);
    
    const bool isOriginAtTop(true);
    for (int64_t y = 0; y < height; y++) {
        const int32_t scanLineIndex = (isOriginAtTop
                                       ? y
                                       : height - y - 1);
        QRgb* rgbScanLine = (QRgb*)imageOut->scanLine(scanLineIndex);
        
        int64_t cziDataRowOffset = (y * bitMapInfo.stride);
        
        for (int64_t x = 0; x < width; x++) {
            int32_t red(0);
            int32_t green(0);
            int32_t blue(0);
            int32_t alpha(0);
            
            switch (bitmapData->GetPixelType()){
                case libCZI::PixelType::Bgr24:
                {
                    int64_t pixelOffset(cziDataRowOffset + (x * 3));
                    red   = cziPtr8[pixelOffset + 2];
                    green = cziPtr8[pixelOffset + 1];
                    blue  = cziPtr8[pixelOffset];
                    alpha = 255;
                }
                    break;
                case libCZI::PixelType::Bgr48:
                {
                    int64_t pixelOffset(cziDataRowOffset + (x * 3));
                    red   = cziPtr16[pixelOffset + 2] / 2;
                    green = cziPtr16[pixelOffset + 1] / 2;
                    blue  = cziPtr16[pixelOffset] / 2;
                    alpha = 255;
                }
                    break;
                case libCZI::PixelType::Gray16:
                {
                    int64_t pixelOffset(cziDataRowOffset + x);
                    red   = cziPtr16[pixelOffset] / 2;
                    green = red;
                    blue  = red;
                    alpha = 255;
                }
                    break;
                case libCZI::PixelType::Gray8:
                {
                    int64_t pixelOffset(cziDataRowOffset + x);
                    red   = cziPtr8[pixelOffset];
                    green = red;
                    blue  = red;
                    alpha = 255;
                }
                    break;
                case libCZI::PixelType::Bgra32:
                {
                    int64_t pixelOffset(cziDataRowOffset + (x * 4));
                    red   = cziPtr8[pixelOffset + 2];
                    green = cziPtr8[pixelOffset + 1];
                    blue  = cziPtr8[pixelOffset];
                    alpha = cziPtr8[pixelOffset + 3];
                }
                    break;
                default:
                    CaretAssert(0);
                    break;
            }
            QColor color(red,
                         green,
                         blue,
                         alpha);
            
            QRgb* pixel = &rgbScanLine[x];
            *pixel = color.rgba();
        }
    }
    bitmapData->Unlock();

    return imageOut;
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
ImageFileCziHelper::toString() const
{
    return "ImageFileCziHelper";
}

