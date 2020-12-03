/*LICENSE_START*/
/*
 *  Copyright (C) 2014  Washington University School of Medicine
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

#include <cmath>
#include <cstdint>

#include <QBuffer>
#include <QColor>
#include <QImage>
#include <QImageWriter>
#include <QTime>

#include "CaretAssert.h"
#include "CaretLogger.h"
#include "ControlPointFile.h"
#include "ControlPoint3D.h"
#include "DataFileException.h"
#include "DataFileContentInformation.h"
#include "FileInformation.h"
#include "GiftiMetaData.h"
#include "ImageCaptureSettings.h"
#include "ImageFile.h"
#include "Matrix4x4.h"
#include "MathFunctions.h"
#include "SceneClass.h"
#include "VolumeFile.h"

using namespace caret;

const float ImageFile::s_defaultWindowDepthPercentage = 990;

/**
 * Constructor.
 */
ImageFile::ImageFile()
: MediaFile(DataFileTypeEnum::IMAGE)
{
    m_controlPointFile.grabNew(new ControlPointFile());
    
    m_fileMetaData.grabNew(new GiftiMetaData());
    
    m_image   = new QImage();
}

/**
 * Constructor
 * @param qimage
 *    QImage that is copied to this image file.
 */
ImageFile::ImageFile(const QImage& qimage)
: MediaFile(DataFileTypeEnum::IMAGE)
{
    m_controlPointFile.grabNew(new ControlPointFile());
    
    m_fileMetaData.grabNew(new GiftiMetaData());
    
    m_image = new QImage(qimage);
}

/**
 * Constructs an image file from image data.
 *
 * @param imageDataRGBA
 *     Image data unsigned bytes with one byte for each
 *     red, green, blue, alpha.
 * @param imageWidth
 *     Width of image.
 * @param imageHeight
 *     Height of image.
 * @param imageOrigin
 *     Location of first pixel in the image data.
 */
ImageFile::ImageFile(const unsigned char* imageDataRGBA,
                     const int imageWidth,
                     const int imageHeight,
                     const IMAGE_DATA_ORIGIN_LOCATION imageOrigin)
: MediaFile(DataFileTypeEnum::IMAGE)
{
    m_controlPointFile.grabNew(new ControlPointFile());
    
    m_fileMetaData.grabNew(new GiftiMetaData());
    
    m_image = new QImage(imageWidth,
                             imageHeight,
                             QImage::Format_RGB32);
    
    bool isOriginAtTop = false;
    switch (imageOrigin) {
        case IMAGE_DATA_ORIGIN_AT_BOTTOM:
            isOriginAtTop = false;
            break;
        case IMAGE_DATA_ORIGIN_AT_TOP:
            isOriginAtTop = true;
            break;
    }
    
    /*
     * Documentation for QImage states that setPixel may be very costly
     * and recommends using the scanLine() method to access pixel data.
     */
    for (int y = 0; y < imageHeight; y++) {
        const int scanLineIndex = (isOriginAtTop
                                   ? y
                                   : imageHeight -y - 1);
        QRgb* rgbScanLine = (QRgb*)m_image->scanLine(scanLineIndex);
        
        for (int x = 0; x < imageWidth; x++) {
            const int32_t contentOffset = (((y * imageWidth) * 4)
                                           + (x * 4));
            const int red   = imageDataRGBA[contentOffset];
            const int green = imageDataRGBA[contentOffset+1];
            const int blue  = imageDataRGBA[contentOffset+2];
            const int alpha  = imageDataRGBA[contentOffset+3];
            QColor color(red,
                         green,
                         blue,
                         alpha);
            
            QRgb* pixel = &rgbScanLine[x];
            *pixel = color.rgba();
        }
    }
}

/**
 * Destructor.
 */
ImageFile::~ImageFile()
{
    if (m_image != NULL) {
        delete m_image;
        m_image = NULL;
    }
}

/**
 * Clears current file data in memory.
 */
void
ImageFile::clear()
{
    if (m_image != NULL) {
        delete m_image;
    }
    m_image = new QImage();
    this->clearModified();
}

/**
 * @return Number of frames in the file
 */
int32_t
ImageFile::getNumberOfFrames() const
{
    if (m_image != NULL) {
        return 1;
    }
    return 0;
}

/**
 * @return Get access to the file's metadata.
 */
GiftiMetaData*
ImageFile::getFileMetaData()
{
    return m_fileMetaData;
}

/**
 * @return Get access to unmodifiable file's metadata.
 */
const GiftiMetaData*
ImageFile::getFileMetaData() const
{
    return m_fileMetaData;
}

/**
 * @return true if the file is is empty (image contains no pixels).
 */

bool
ImageFile::isEmpty() const
{
    return (m_image->width() <= 0);
}

///**
// * @return A pointer to the QImage in this file.
// * Note that manipulating the pointer's data will
// * alter the contents of this file.
// */
//QImage*
//ImageFile::getAsQImage()
//{
//    return m_image;
//}

/**
 * @return A pointer to the QImage in this file.
 */
const QImage*
ImageFile::getAsQImage() const
{
    return m_image;
}

/**
 * Set the image in this file from a QImage.
 * @param qimage
 *    Image that is copied to this file.
 */
void
ImageFile::setFromQImage(const QImage& qimage)
{
    if (m_image != NULL) {
        delete m_image;
    }
    m_image = new QImage(qimage);
    this->setModified();
}

/**
 * Set the dots per meter.
 *
 * @param x
 *    Dots per meter for X dimension.
 * @param y
 *    Dots per meter for Y dimension.
 */
void
ImageFile::setDotsPerMeter(const int x,
                           const int y)
{
    m_image->setDotsPerMeterX(x);
    m_image->setDotsPerMeterY(y);
}


/**
 * Examines the image to find the rectangular of the object in the image
 * by examining pixels in the background color.
 * @param backgroundColor
 *    RGB components range 0-255.
 * @param objectBoundsOut
 *    4-dimensional array containing the region that excludes
 *    the backround around the image's object.
 */
void
ImageFile::findImageObject(const uint8_t backgroundColor[3],
                           int objectBoundsOut[4]) const
{
    //
    // Dimensions of image
    //
    const int numX = m_image->width();
    const int numY = m_image->height();
    
    //
    // Initialize output
    //
    objectBoundsOut[0] = 0;
    objectBoundsOut[1] = 0;
    objectBoundsOut[2] = numX - 1;
    objectBoundsOut[3] = numY - 1;
    
    //
    // Find left
    //
    bool gotPixelFlag = false;
    for (int i = 0; i < numX; i++) {
        for (int j = 0; j < numY; j++) {
            const QRgb pixel = m_image->pixel(i, j);
            if ((qRed(pixel) != backgroundColor[0]) ||
                (qGreen(pixel) != backgroundColor[1]) ||
                (qBlue(pixel)  != backgroundColor[2])) {
                objectBoundsOut[0] = i;
                gotPixelFlag = true;
                break;
            }
        }
        if (gotPixelFlag) {
            break;
        }
    }
    
    //
    // Find right
    //
    gotPixelFlag = false;
    for (int i = (numX - 1); i >= 0; i--) {
        for (int j = 0; j < numY; j++) {
            const QRgb pixel = m_image->pixel(i, j);
            if ((qRed(pixel) != backgroundColor[0]) ||
                (qGreen(pixel) != backgroundColor[1]) ||
                (qBlue(pixel)  != backgroundColor[2])) {
                objectBoundsOut[2] = i;
                gotPixelFlag = true;
                break;
            }
        }
        if (gotPixelFlag) {
            break;
        }
    }
    
    //
    // Find top
    //
    gotPixelFlag = false;
    for (int j = 0; j < numY; j++) {
        for (int i = 0; i < numX; i++) {
            const QRgb pixel = m_image->pixel(i, j);
            if ((qRed(pixel) != backgroundColor[0]) ||
                (qGreen(pixel) != backgroundColor[1]) ||
                (qBlue(pixel)  != backgroundColor[2])) {
                objectBoundsOut[1] = j;
                gotPixelFlag = true;
                break;
            }
        }
        if (gotPixelFlag) {
            break;
        }
    }
    
    //
    // Find bottom
    //
    gotPixelFlag = false;
    for (int j = (numY - 1); j >= 0; j--) {
        for (int i = 0; i < numX; i++) {
            const QRgb pixel = m_image->pixel(i, j);
            if ((qRed(pixel) != backgroundColor[0]) ||
                (qGreen(pixel) != backgroundColor[1]) ||
                (qBlue(pixel)  != backgroundColor[2])) {
                objectBoundsOut[3] = j;
                gotPixelFlag = true;
                break;
            }
        }
        if (gotPixelFlag) {
            break;
        }
    }
}

/**
 * Add a margin to this image.
 * @param marginSize
 *    Number of pixels in the margin.
 * @param backgroundColor used for the added pixels.
 *    RGB components range 0-255.
 */
void
ImageFile::addMargin(const int marginSize,
                     const uint8_t backgroundColor[3])
{
    this->addMargin(marginSize, marginSize, backgroundColor);
    /*
     if (marginSize <= 0) {
     return;
     }
     
     //
     // Add margin
     //
     const int width = image.width();
     const int height = image.height();
     const int newWidth = width + marginSize * 2;
     const int newHeight = height + marginSize * 2;
     QRgb backgroundColorRGB = qRgba(backgroundColor[0],
     backgroundColor[1],
     backgroundColor[2],
     0);
     
     //
     // Insert image
     //
     ImageFile imageFile;
     imageFile.setImage(QImage(newWidth, newHeight, image.format()));
     imageFile.getImage()->fill(backgroundColorRGB);
     try {
     imageFile.insertImage(image, marginSize, marginSize);
     image = (*imageFile.getImage());
     }
     catch (DataFileException&) {
     }
     */
}

/**
 * Add a margin to this image.
 * @param image
 *    Image to which margin is added.
 * @param marginSizeX
 *    Number of pixels in the margin along x-axis.
 * @param marginSizeY
 *    Number of pixels in the margin along y-axis.
 * @param backgroundColor used for the added pixels.
 *    RGB components range 0-255.
 */
void
ImageFile::addMargin(const int marginSizeX,
                     const int marginSizeY,
                     const uint8_t backgroundColor[3])
{
    if ((marginSizeX <= 0) && (marginSizeY <= 0)) {
        return;
    }
    
    //
    // Add margin
    //
    const int width = m_image->width();
    const int height = m_image->height();
    const int newWidth = width + marginSizeX * 2;
    const int newHeight = height + marginSizeY * 2;
    QRgb backgroundColorRGB = qRgba(backgroundColor[0],
                                    backgroundColor[1],
                                    backgroundColor[2],
                                    0);
    
    //
    // Insert image
    //
    ImageFile imageFile;
    imageFile.setFromQImage(QImage(newWidth, newHeight, m_image->format()));
    imageFile.m_image->fill(backgroundColorRGB);
    try {
        imageFile.insertImage(*m_image, marginSizeX, marginSizeY);
        this->setFromQImage(*imageFile.getAsQImage());
    }
    catch (DataFileException& e) {
        CaretLogWarning(e.whatString());
    }
    this->setModified();
}

/**
 * Crop an image by removing the background from the object in the image
 * but keeping a margin of the given size around the image.
 * @param marginSize
 *    Number of pixels in the margin around the image's object.
 * @param backgroundColor
 *    Color of background that ranges 0-255.
 */
void
ImageFile::cropImageRemoveBackground(const int marginSize,
                                     const uint8_t backgroundColor[3])
{
    //
    // Get cropping bounds
    //
    int leftTopRightBottom[4];
    this->findImageObject(backgroundColor,
                          leftTopRightBottom);
    CaretLogFine("cropping: "
                   + AString::fromNumbers(leftTopRightBottom, 4, " "));
    
    const int currentWidth = m_image->width();
    const int currentHeight = m_image->height();
    
    //
    // If cropping is valid
    //
    const int width = leftTopRightBottom[2] - leftTopRightBottom[0] + 1;
    const int height = leftTopRightBottom[3] - leftTopRightBottom[1] + 1;
    if ((width != currentWidth)
        || (height != currentHeight)) {
        if ((width > 1) &&
            (height > 1)) {
            QImage copyImage = this->getAsQImage()->copy(leftTopRightBottom[0],
                                                         leftTopRightBottom[1],
                                                         width,
                                                         height);
            if (copyImage.isNull() == false) {
                if ((copyImage.width() > 0)
                    && (copyImage.height() > 0)) {
                    
                    this->setFromQImage(copyImage);
                }
            }
            
            this->setModified();
        }
    }
    
    //
    // Process margin
    //
    if (marginSize > 0) {
        this->addMargin(marginSize,
                        backgroundColor);
    }
}

/**
 * Replace the contents of this image by combining the input images and
 * retaining aspect and stretching and filling if needed.
 * @param imageFiles
 *    Images that are combined.
 * @param numImagesPerRow
 *    Number of images in each row.
 * @param backgroundColor
 *    Color of background that ranges 0-255.
 */
void
ImageFile::combinePreservingAspectAndFillIfNeeded(const std::vector<ImageFile*>& imageFiles,
                                                  const int numImagesPerRow,
                                                  const uint8_t backgroundColor[3])
{
    const int numImages = static_cast<int>(imageFiles.size());
    if (numImages <= 0) {
        return;
    }
    if (numImages == 1) {
        this->setFromQImage(*imageFiles[0]->m_image);
        return;
    }
    
    QRgb backgroundColorRGB = qRgba(backgroundColor[0],
                                    backgroundColor[1],
                                    backgroundColor[2],
                                    0);
    //
    // Resize all images but do not stretch
    // need to retain aspect ratio but all must
    // be the same size in X & Y
    //
    
    //
    // Find max width and height of input images
    //
    int maxImageWidth = 0;
    int maxImageHeight = 0;
    for (int i = 0; i < numImages; i++) {
        //
        // Track max width/height
        //
        maxImageWidth = std::max(maxImageWidth, imageFiles[i]->m_image->width());
        maxImageHeight = std::max(maxImageHeight, imageFiles[i]->m_image->height());
    }
    
    //
    // Compute size of output image and create it
    //
    const int outputImageSizeX = maxImageWidth * numImagesPerRow;
    const int numberOfRows = (numImages / numImagesPerRow)
    + (((numImages % numImagesPerRow) != 0) ? 1 : 0);
    const int outputImageSizeY = maxImageHeight * numberOfRows;
    QImage combinedImage(outputImageSizeX,
                         outputImageSizeY,
                         imageFiles[0]->m_image->format());
    combinedImage.fill(backgroundColorRGB);
    
    
    //
    // Loop through the images
    //
    int rowCounter = 0;
    int columnCounter = 0;
    for (int i = 0; i < numImages; i++) {
        //
        // Scale image
        //
        const QImage imageScaled = imageFiles[i]->m_image->scaled(maxImageWidth,
                                                                maxImageHeight,
                                                                Qt::KeepAspectRatio,
                                                                Qt::SmoothTransformation);
        
        //
        // Compute position of where image should be inserted
        //
        const int marginX = (maxImageWidth - imageScaled.width()) / 2;
        const int marginY = (maxImageHeight - imageScaled.height()) / 2;
        const int positionX = columnCounter * maxImageWidth + marginX;
        const int positionY = rowCounter * maxImageHeight + marginY;
        
        //
        // Insert into output image
        //
        try {
            ImageFile::insertImage(imageScaled,
                                   combinedImage,
                                   positionX,
                                   positionY);
        }
        catch (DataFileException& e) {
            CaretLogWarning("QImageFile::insertImage() error: "
                            + e.whatString());
        }
        
        //
        // Update row and column counters
        //
        columnCounter++;
        if (columnCounter >= numImagesPerRow) {
            columnCounter = 0;
            rowCounter++;
        }
    }
    
    this->setFromQImage(combinedImage);
}

/**
 * Read the image file.
 * @param filename
 *    Name of image file.
 * @throws DataFileException
 *    If error reading image.
 */
void
ImageFile::readFile(const AString& filename)
{
    clear();
    
    checkFileReadability(filename);
    
    if (filename.isEmpty()) {
        throw DataFileException(filename + "Filename for reading is isEmpty");
    }
    
    this->setFileName(filename);
    
    if (m_image->load(filename) == false) {
        clear();
        throw DataFileException(filename + "Unable to load file.");
    }
    
    this->clearModified();
}

/**
 * Append an image file to the bottom of this image file.
 * @param img
 *    Image that is appended.
 */
void
ImageFile::appendImageAtBottom(const ImageFile& img)
{
    //
    // Determine size of new image
    //
    const QImage* otherImage = img.getAsQImage();
    const int newWidth = std::max(m_image->width(), otherImage->width());
    const int newHeight = m_image->height() + otherImage->height();
    const int oldHeight = m_image->height();
    
    //
    // Copy the current image
    //
    const QImage currentImage = *m_image;
    //   std::cout << "cw: " << currentImage.width() << std::endl;
    //   std::cout << "ch: " << currentImage.height() << std::endl;
    
    //
    // Create the new image and make it "this" image
    //
    QImage newImage(newWidth, newHeight, QImage::Format_RGB32);
    //   std::cout << "nw: " << newImage.width() << std::endl;
    //   std::cout << "nh: " << newImage.height() << std::endl;
    setFromQImage(newImage);
    //   std::cout << "iw2: " << image.width() << std::endl;
    //   std::cout << "ih2: " << image.height() << std::endl;
    
    //
    // Insert current image into new image
    //
    insertImage(currentImage, 0, 0);
    
    //
    // Insert other image into new image
    //
    insertImage(*otherImage, 0, oldHeight);
    
    this->setModified();
}

/**
 * Insert an image into this image which must be large enough for insertion of image.
 * @param otherImage
 *    Image that is inserted into this image.
 * @param x
 *    X position of where image is inserted.
 * @param y
 *    Y position of where image is inserted.
 * @throws DataFileException
 *    If error inserting image.
 */
void
ImageFile::insertImage(const QImage& otherImage,
                       const int x,
                       const int y)
{
    ImageFile::insertImage(otherImage,
                           *m_image,
                           x,
                           y);
    this->setModified();
}

/**
 * insert an image into another image.
 *
 * Insert an image into another image which must be large enough for insertion of image.
 * @param insertThisImage
 *    Image that is inserted into other image.
 * @param intoThisImage
 *    Image that receives insertions of other image.
 * @param x
 *    X position of where image is inserted.
 * @param y
 *    Y position of where image is inserted.
 * @throws DataFileException
 *    If error inserting image.
 */
void
ImageFile::insertImage(const QImage& insertThisImage,
                       QImage& intoThisImage,
                       const int positionX,
                       const int positionY)
{
    if (positionX < 0) {
        throw DataFileException("X position is less than zero.");
    }
    if (positionY < 0) {
        throw DataFileException("Y position is less than zero.");
    }
    
    const int otherWidth = insertThisImage.width();
    const int otherHeight = insertThisImage.height();
    
    const int myWidth = intoThisImage.width();
    const int myHeight = intoThisImage.height();
    
    if ((otherWidth + positionX) > myWidth) {
        throw DataFileException("This image is not large enough to insert other image.");
    }
    if ((otherHeight + positionY) > myHeight) {
        throw DataFileException("This image is not large enough to insert other image.");
    }
    
    for (int i = 0; i < otherWidth; i++) {
        for (int j = 0; j < otherHeight; j++) {
            intoThisImage.setPixel(positionX + i,
                                   positionY + j,
                                   insertThisImage.pixel(i, j));
        }
    }
}

/**
 * Scale the given image to the given width and height while preserving
 * the aspect ratio.  If the
 *
 * @param image
 *     The image
 * @param width
 *     Width of the image
 * @param height
 *     Height of the image
 * @param fillColor
 *     If not NULL, padded region is this color
 * @return
 *     Image that will be the requested width and height or
 *     a null image (.isNull()) if error.
 */
QImage
ImageFile::scaleToSizeWithPadding(const QImage& image,
                                  const int width,
                                  const int height,
                                  const QColor* fillColor)
{
    /*
     * Invalid image tests
     */
    if (image.isNull()) {
        return image;
    }
    if ((image.width() <= 0)
        || (image.height() <= 0)) {
        return QImage();
    }
    
    /*
     * Nothing to do if image is correct size
     */
    if ((image.width() == width)
        && (image.height() == height)) {
        return image;
    }
    
    const QImage scaledImage = image.scaled(width,
                                      height,
                                      Qt::KeepAspectRatio,
                                      Qt::SmoothTransformation);
    const int scaledWidth  = scaledImage.width();
    const int scaledHeight = scaledImage.height();
    if ((scaledWidth == width)
        && (scaledHeight == height)) {
        return scaledImage;
    }
    else if (scaledWidth > width) {
        CaretLogSevere("Image scale width was made larger="
                       + QString::number(scaledWidth)
                       + " than requested="
                       + QString::number(width));
        return QImage();
    }
    else if (scaledHeight > height) {
        CaretLogSevere("Image scale height was made larger="
                       + QString::number(scaledHeight)
                       + " than requested="
                       + QString::number(height));
        return QImage();
    }

    QImage outputImage(width,
                       height,
                       image.format());
    if (fillColor != NULL) {
        outputImage.fill(*fillColor);
    }
    else {
        outputImage.fill(Qt::black);
    }
    
    const int insertX = (width - scaledWidth) / 2;
    const int insertY = (height - scaledHeight) / 2;
    
    try {
        ImageFile::insertImage(scaledImage,
                               outputImage,
                               insertX,
                               insertY);
    }
    catch (const DataFileException& dfe) {
        CaretLogSevere(dfe.whatString());
        outputImage = QImage();
    }
    
    return outputImage;
}

/**
 * Compare a file for unit testing (tolerance ignored).
 *
 * @param dataFile
 *    Data files that is compared to this data file.
 * @param tolerance
 *    Allowable difference at each pixel.
 * @param messageOut
 *    Message describing differences.
 * @return
 *    True if files are within tolerance, else false.
 */
bool
ImageFile::compareFileForUnitTesting(const DataFile* dataFile,
                                     const float tolerance,
                                     AString& messageOut) const
{
    //
    // Cast to an image file
    //
    const ImageFile* img = dynamic_cast<const ImageFile*>(dataFile);
    if (img == NULL) {
        messageOut = ("ERROR: File for comparison ("
                      + dataFile->getFileName()
                      + " does not appear to be an image file.");
        return false;
    }
    
    //
    // Get the image from the other file
    //
    const QImage* otherImage = img->getAsQImage();
    
    //
    // Confirm width/height
    //
    const int width = m_image->width();
    const int height = m_image->height();
    if ((width != otherImage->width()) ||
        (height != otherImage->height())) {
        messageOut = "The images are of different height and/or width.";
        return false;
    }
    
    //
    // compare pixels
    //
    int pixelCount = 0;
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            QColor im1 = m_image->pixel(i, j);
            QColor im2 = otherImage->pixel(i, j);
            if ((abs(im1.red() - im2.red()) > tolerance) ||
                (abs(im1.green() - im2.green()) > tolerance) ||
                (abs(im1.blue() - im2.blue()) > tolerance)) {
                pixelCount++;
            }
        }
    }
    
    if (pixelCount > 0) {
        const float pct = static_cast<float>(pixelCount * 100.0)
        / static_cast<float>(width * height);
        messageOut = QString::number(pct, 'f', 2)
        + "% pixels in the image do not match.";
        return false;
    }
    
    return true;
}

/**
 * Write the image file.
 * @param filename
 *    Name of image file.
 * @throws DataFileException
 *    If error writing image.
 */
void
ImageFile::writeFile(const AString& filename)
{
    checkFileWritability(filename);
    
    this->setFileName(filename);
    
    AString errorMessage;
    if (m_image->width() <= 0) {
        errorMessage = "Image width is zero.";
    }
    if (m_image->height() <= 0) {
        if (errorMessage.isEmpty() == false) errorMessage += "\n";
        errorMessage = "Image height is zero.";
    }
    if (errorMessage.isEmpty() == false) {
        throw DataFileException(filename + "  " + errorMessage);
    }
    
    FileInformation fileInfo(this->getFileName());
    AString format = fileInfo.getFileExtension().toUpper();
    if (format == "JPG") {
        format = "JPEG";
    }
    
    QImageWriter writer(filename, format.toLatin1());
    if (writer.supportsOption(QImageIOHandler::Quality)) {
        if (format.compare("png", Qt::CaseInsensitive) == 0) {
            const int quality = 1;
            writer.setQuality(quality);
        }
        else {
            const int quality = 100;
            writer.setQuality(quality);
        }
    }
    
    if (writer.supportsOption(QImageIOHandler::CompressionRatio)) {
        writer.setCompression(1);
    }
    if (writer.write(*m_image) == false) {
        throw DataFileException(writer.errorString());
    }
    
    this->clearModified();
}

/**
 * Get the image file extensions for the supported image types.
 * The extensions do not include the leading period.
 *
 * @param imageFileExtensions
 *    Output filled with extensions for supported image types.
 * @param defaultExtension
 *    The default extension (preference is png, jpg, jpeg)
 */
void
ImageFile::getImageFileExtensions(std::vector<AString>& imageFileExtensions,
                                  AString& defaultExtension)
{
    imageFileExtensions.clear();
    defaultExtension = "";
    
    QString firstExtension;
    QString pngExtension;
    QString jpegExtension;
    QString jpgExtension;
    QString tifExtension;
    QString tiffExtension;
    
    QList<QByteArray> imageFormats = QImageWriter::supportedImageFormats();
    const int numFormats = imageFormats.count();
    for (int i = 0; i < numFormats; i++) {
        AString extension = QString(imageFormats.at(i)).toLower();
        imageFileExtensions.push_back(extension);
        
        if (i == 0) {
            firstExtension = extension;
        }
        if (extension == "png") {
            pngExtension = extension;
        }
        else if (extension == "jpg") {
            jpgExtension = extension;
        }
        else if (extension == "jpeg") {
            jpegExtension = extension;
        }
        else if (extension == "tif") {
            tifExtension = extension;
        }
        else if (extension == "tiff") {
            tiffExtension = extension;
        }
    }
    
    if (pngExtension.isEmpty() == false) {
        defaultExtension = pngExtension;
    }
    else if (jpgExtension.isEmpty() == false) {
        defaultExtension = jpgExtension;
    }
    else if (jpegExtension.isEmpty() == false) {
        defaultExtension = jpegExtension;
    }
    else if (tifExtension.isEmpty() == false) {
        defaultExtension = tifExtension;
    }
    else if (tiffExtension.isEmpty() == false) {
        defaultExtension = tiffExtension;
    }
    else {
        defaultExtension = firstExtension;
    }
}

/**
 * Get the image file filters for the supported image types.
 *
 * @param imageFileFilters
 *    Output filled with the filters for supported image types.
 * @param defaultFilter
 *    Filter for the preferred image type.
 */
void
ImageFile::getImageFileFilters(std::vector<AString>& imageFileFilters,
                               AString& defaultFilter)
{
    imageFileFilters.clear();
    defaultFilter.clear();
    
    std::vector<AString> imageFileExtensions;
    AString defaultExtension;
    ImageFile::getImageFileExtensions(imageFileExtensions,
                                      defaultExtension);
    
    const int32_t numExtensions = static_cast<int32_t>(imageFileExtensions.size());
    for (int32_t i = 0; i < numExtensions; i++) {
        const AString ext = imageFileExtensions[i];
        const AString filter = (ext.toUpper()
                                + " Image File (*."
                                + ext
                                + ")");
        imageFileFilters.push_back(filter);
        
        if (ext == defaultExtension) {
            defaultFilter = filter;
        }
    }
    
    if (defaultFilter.isEmpty()) {
        if (imageFileFilters.empty() == false) {
            defaultFilter = imageFileFilters[0];
        }
    }
}

/**
 * Resize the image to the given width while preserving the aspect ratio
 * of the image.
 *
 * @param width
 *     Width for image.
 */
void
ImageFile::resizeToWidth(const int32_t width)
{
    CaretAssert(m_image);
    *m_image = m_image->scaledToWidth(width,
                                      Qt::SmoothTransformation);
}

/**
 * Resize the image to the given height while preserving the aspect ratio
 * of the image.
 *
 * @param height
 *     Height for image.
 */
void
ImageFile::resizeToHeight(const int32_t height)
{
    CaretAssert(m_image);
    *m_image = m_image->scaledToHeight(height,
                                       Qt::SmoothTransformation);
}

/**
 * Resize the image so that its width is no larger than the given value.
 * If the image's current width is less than the given value, no
 * resizing takes place.
 *
 * @param maximumWidth
 *     Maximum width for the image.
 */
void
ImageFile::resizeToMaximumWidth(const int32_t maximumWidth)
{
    CaretAssert(m_image);
    const int32_t width = m_image->width();
    
    if (width > maximumWidth) {
        *m_image = m_image->scaledToWidth(maximumWidth,
                                          Qt::SmoothTransformation);
    }
}

/**
 * Resize the image so that its height is no larger than the given value.
 * If the image's current height is less than the given value, no
 * resizing takes place.
 *
 * @param maximumHeight
 *     Maximum height for the image.
 */
void
ImageFile::resizeToMaximumHeight(const int32_t maximumHeight)
{
    CaretAssert(m_image);
    const int32_t height = m_image->height();
    
    if (height > maximumHeight) {
        *m_image = m_image->scaledToHeight(maximumHeight,
                                          Qt::SmoothTransformation);
    }
}

/**
 * Resize the image so that its maximum dimension is the given value
 * yet preserves the aspect ratio of the image.  If the maximum dimension
 * is less than the given value, no resizing takes place.
 *
 * @param maximumWidthOrHeight
 *     Maximum dimension for the image.
 */
void
ImageFile::resizeToMaximumWidthOrHeight(const int32_t maximumWidthOrHeight)
{
    CaretAssert(m_image);
    
    const int32_t width = m_image->width();
    const int32_t height = m_image->height();
    
    if ((width > 0)
        && (height > 0)) {
        if (width > height) {
            resizeToMaximumWidth(maximumWidthOrHeight);
        }
        else {
            if (height > maximumWidthOrHeight) {
                resizeToMaximumWidth(maximumWidthOrHeight);
            }
        }
    }
}

/**
 * Get the RGBA bytes from the image.
 *
 * @param bytesRGBA
 *    The RGBA bytes in the image.
 * @param widthOut
 *    Width of the image.
 * @param heightOut
 *    Height of the image.
 * @param imageOrigin
 *     Location of first pixel in the image data.
 * @return
 *    True if the bytes, width, and height are valid, else false.
 */
bool
ImageFile::getImageBytesRGBA(const IMAGE_DATA_ORIGIN_LOCATION imageOrigin,
                             std::vector<uint8_t>& bytesRGBA,
                             int32_t& widthOut,
                             int32_t& heightOut) const
{
    bytesRGBA.clear();
    widthOut = 0;
    heightOut = 0;
    
    if (m_image != NULL) {
        widthOut  = m_image->width();
        heightOut = m_image->height();
        if ((widthOut > 0)
            && (heightOut > 0)) {
            
            bytesRGBA.resize(widthOut * heightOut * 4);
            
            bool isOriginAtTop = false;
            switch (imageOrigin) {
                case IMAGE_DATA_ORIGIN_AT_BOTTOM:
                    isOriginAtTop = false;
                    break;
                case IMAGE_DATA_ORIGIN_AT_TOP:
                    isOriginAtTop = true;
                    break;
            }
            
            /*
             * Documentation for QImage states that setPixel may be very costly
             * and recommends using the scanLine() method to access pixel data.
             */
            for (int y = 0; y < heightOut; y++) {
                const int scanLineIndex = (isOriginAtTop
                                           ? y
                                           : heightOut -y - 1);
                const uchar* scanLine = m_image->scanLine(scanLineIndex);
                QRgb* rgbScanLine = (QRgb*)scanLine;
                
                for (int x = 0; x < widthOut; x++) {
                    const int32_t contentOffset = (((y * widthOut) * 4)
                                                   + (x * 4));
                    QRgb& rgb = rgbScanLine[x];
                    bytesRGBA[contentOffset] = static_cast<uint8_t>(qRed(rgb));
                    bytesRGBA[contentOffset+1] = static_cast<uint8_t>(qGreen(rgb));
                    bytesRGBA[contentOffset+2] = static_cast<uint8_t>(qBlue(rgb));
                    bytesRGBA[contentOffset+3] = 255;
                }
            }
            return true;
        }
    }
    
    return false;
}

/**
 * Get the pixel RGBA at the given pixel I and J.
 *
 * @param imageOrigin
 *    Location of first pixel in the image data.
 * @param pixelI
 *     Image I index
 * @param pixelJ
 *     Image J index
 * @param pixelRGBAOut
 *     RGBA at Pixel I, J
 * @return
 *     True if valid, else false.
 */
bool
ImageFile::getImagePixelRGBA(const IMAGE_DATA_ORIGIN_LOCATION imageOrigin,
                             const int32_t pixelI,
                             const int32_t pixelJ,
                             uint8_t pixelRGBAOut[4]) const
{
    if (m_image != NULL) {
        const int32_t w = m_image->width();
        const int32_t h = m_image->height();
        
        if ((pixelI >= 0)
            && (pixelI < w)
            && (pixelJ >= 0)
            && (pixelJ < h)) {
            
            int32_t imageJ = pixelJ;
            switch (imageOrigin) {
                case IMAGE_DATA_ORIGIN_AT_BOTTOM:
                    imageJ = h - pixelJ - 1;
                    break;
                case IMAGE_DATA_ORIGIN_AT_TOP:
                    break;
            }
            
            if ((imageJ >= 0)
                && (imageJ < h)) {
                const QRgb rgb = m_image->pixel(pixelI,
                                                imageJ);
                pixelRGBAOut[0] = static_cast<uint8_t>(qRed(rgb));
                pixelRGBAOut[1] = static_cast<uint8_t>(qGreen(rgb));
                pixelRGBAOut[2] = static_cast<uint8_t>(qBlue(rgb));
                pixelRGBAOut[3] = 255;
                
                return true;
            }
            else {
                CaretLogSevere("Invalid image J");
            }
        }
    }
    
    return false;
}


/**
 * Get the RGBA bytes from the image resized into the given width and height.
 *
 * @param imageOrigin
 *    Location of first pixel in the image data.
 * @param resizeToWidth
 *    New width for image.
 * @param resizeToHeight
 *    New height of the image.
 * @param bytesRGBAOut
 *    The RGBA bytes in the image.
 * @return
 *    True if the bytes, width, and height are valid, else false.
 */
bool
ImageFile::getImageResizedBytes(const IMAGE_DATA_ORIGIN_LOCATION imageOrigin,
                                const int32_t resizeToWidth,
                                const int32_t resizeToHeight,
                                std::vector<uint8_t>& bytesRGBAOut) const
{
    bytesRGBAOut.clear();
    
    if (m_image == NULL) {
        return false;
    }
    
    const int32_t numBytes = resizeToWidth * resizeToHeight;
    if (numBytes <= 0) {
        return false;
    }
    
    const int32_t colorComponentsPerByte = 4;
    bytesRGBAOut.resize(numBytes * colorComponentsPerByte);
    
    QImage scaledImage = m_image->scaled(resizeToWidth,
                                   resizeToHeight,
                                   Qt::IgnoreAspectRatio,
                                   Qt::SmoothTransformation);

    /*
     * QImage::scaled() failed.
     */
    if (scaledImage.isNull()) {
        return false;
    }
    
    bool isOriginAtTop = false;
    switch (imageOrigin) {
        case IMAGE_DATA_ORIGIN_AT_BOTTOM:
            isOriginAtTop = false;
            break;
        case IMAGE_DATA_ORIGIN_AT_TOP:
            isOriginAtTop = true;
            break;
    }
    
    /*
     * Documentation for QImage states that setPixel may be very costly
     * and recommends using the scanLine() method to access pixel data.
     */
    for (int y = 0; y < resizeToHeight; y++) {
        const int scanLineIndex = (isOriginAtTop
                                   ? y
                                   : resizeToHeight -y - 1);
        const uchar* scanLine = scaledImage.scanLine(scanLineIndex);
        QRgb* rgbScanLine = (QRgb*)scanLine;
        
        for (int x = 0; x < resizeToWidth; x++) {
            const int32_t contentOffset = (((y * resizeToWidth) * 4)
                                           + (x * 4));
            QRgb& rgb = rgbScanLine[x];
            CaretAssertVectorIndex(bytesRGBAOut, contentOffset + 3);
            bytesRGBAOut[contentOffset] = static_cast<uint8_t>(qRed(rgb));
            bytesRGBAOut[contentOffset+1] = static_cast<uint8_t>(qGreen(rgb));
            bytesRGBAOut[contentOffset+2] = static_cast<uint8_t>(qBlue(rgb));
            bytesRGBAOut[contentOffset+3] = 255;
        }
    }
    return true;
}

/**
 * @return width of image (zero if image is invalid)
 */
int32_t
ImageFile::getWidth() const
{
    int32_t w = 0;
    
    if (m_image != NULL) {
        w = m_image->width();
    }
    
    return w;
}

/**
 * @return height of image (zero if image is invalid)
 */
int32_t
ImageFile::getHeight() const
{
    int32_t h = 0;
    
    if (m_image != NULL) {
        h = m_image->height();
    }
    
    return h;
}

/**
 * Essentially writes the image file to a byte array using the given format.
 *
 * @param byteArrayOut
 *    Byte array into which the image is written.
 * @param format
 *    Format for the image (jpg, ppm, etc.).
 */
void
ImageFile::getImageInByteArray(QByteArray& byteArrayOut,
                               const AString& format) const
{
    byteArrayOut.clear();
    
    if (m_image != NULL) {
        QBuffer buffer(&byteArrayOut);
        if ( ! buffer.open(QIODevice::WriteOnly)) {
            throw DataFileException(getFileName(),
                                    "PROGRAM ERROR: Unable to open byte array for output of image.");
        }
        
        bool successFlag = false;
        if (format.isEmpty()) {
            successFlag = m_image->save(&buffer);
        }
        else {
            successFlag = m_image->save(&buffer,
                                        format.toLatin1().data());
        }
        
        if ( ! successFlag) {
            throw DataFileException(getFileName(),
                                    "Failed to write image to byte array.  "
                                    + buffer.errorString());
        }
    }
}

/**
 * Essentially reads the image file from a byte array using the given format.
 *
 * @param byteArray
 *    Byte array from which the image is read.
 * @param format
 *    Format for the image (jpg, ppm, etc.) or empty if unknown.
 */
bool
ImageFile::setImageFromByteArray(const QByteArray& byteArray,
                                 const AString& format)
{
    bool successFlag = false;
    if (format.isEmpty()) {
        successFlag = m_image->loadFromData(byteArray);
    }
    else {
        successFlag = m_image->loadFromData(byteArray,
                                            format.toLatin1().data());
    }
    
    if ( ! successFlag) {
        CaretLogSevere(getFileName()
                       + " Failed to create image from byte array.");
    }

    return successFlag;
}

/**
 * Convert this image into a Volume File using the 
 * encapsulated Control Point File whose matrix 
 * must have been updated.
 *
 * @param colorMode
 *     Color mode for conversion.
 * @param errorMessageOut
 *     Contains error message if conversion fails.
 * @return
 *     Pointer to volume file or NULL if there is an error.
 *     Name of volume file is the name of the image but
 *     the file extension is changed to a volume file extension.
 *
 */
VolumeFile*
ImageFile::convertToVolumeFile(const CONVERT_TO_VOLUME_COLOR_MODE colorMode,
                               AString& errorMessageOut) const
{
    errorMessageOut.clear();
    
    std::vector<uint8_t> rgbaBytes;
    int32_t width = 0;
    int32_t height = 0;
    getImageBytesRGBA(ImageFile::IMAGE_DATA_ORIGIN_AT_BOTTOM,
                      rgbaBytes,
                      width,
                      height);
    if ((width <= 0)
        || (height <= 0)) {
        errorMessageOut = "Image width and/or height is invalid.";
        return NULL;
    }
    
    const Matrix4x4* transformationMatrix = m_controlPointFile->getLandmarkTransformationMatrix();
    
    float firstPixel[3] = { 0, 0, 0 };
    transformationMatrix->multiplyPoint3(firstPixel);
    std::cout << "First pixel coord: " << AString::fromNumbers(firstPixel, 3, ",") << std::endl;
    
    float lastPixel[3] = { (float)(width - 1), (float)(height - 1), 0.0f };
    transformationMatrix->multiplyPoint3(lastPixel);
    std::cout << "Last pixel coord: " << AString::fromNumbers(lastPixel, 3, ",") << std::endl;
    
    {
        float bl[3] = { 0.0, 0.0, 0.0 };
        transformationMatrix->multiplyPoint3(bl);
        ControlPoint3D bottomLeft(0, 0, 0, bl[0], bl[1], bl[2]);
        
        float br[3] = { (float)(width - 1.0), 0.0, 0.0 };
        transformationMatrix->multiplyPoint3(br);
        ControlPoint3D bottomRight(width - 1.0, 0.0, 0.0, br[0], br[1], br[2]);
        
        float tr[3] = { (float)(width - 1.0), (float)(height - 1.0), 0.0 };
        transformationMatrix->multiplyPoint3(tr);
        ControlPoint3D topRight((float)(width - 1.0), (float)(height - 1.0), 0.0, tr[0], tr[1], tr[2]);
        
        ControlPointFile volumeControlPointFile;
        volumeControlPointFile.addControlPoint(bottomLeft);
        volumeControlPointFile.addControlPoint(bottomRight);
        volumeControlPointFile.addControlPoint(topRight);
        
        if ( ! volumeControlPointFile.updateLandmarkTransformationMatrix(errorMessageOut)) {
            errorMessageOut.insert(0, "Volume Matrix: ");
            return NULL;
        }
    }
    
    std::vector<int64_t> dimensions;
    dimensions.push_back(width);  // I
    dimensions.push_back(height); // J
    dimensions.push_back(1); // K
    
    /*
     * Convert matrix4x4 to volume file vector of vectors.
     */
    std::vector<float> row1;
    std::vector<float> row2;
    std::vector<float> row3;
    std::vector<float> row4;
    
    for (int j = 0; j < 4; j++) {
        row1.push_back(transformationMatrix->getMatrixElement(0, j));
        row2.push_back(transformationMatrix->getMatrixElement(1, j));
        row3.push_back(transformationMatrix->getMatrixElement(2, j));
        row4.push_back(transformationMatrix->getMatrixElement(3, j));
    }
    std::vector<std::vector<float> > indexToSpace;
    indexToSpace.push_back(row1);
    indexToSpace.push_back(row2);
    indexToSpace.push_back(row3);
    indexToSpace.push_back(row4);
    
    int64_t numComponents = 1;
    SubvolumeAttributes::VolumeType whatType = SubvolumeAttributes::FUNCTIONAL;
    switch (colorMode) {
        case CONVERT_TO_VOLUME_COLOR_GRAYSCALE:
            break;
        case CONVERT_TO_VOLUME_COLOR_RGB:
            numComponents = 3;
            whatType = SubvolumeAttributes::RGB;
            break;
    }
    VolumeFile* volumeFile = new VolumeFile(dimensions,
                                            indexToSpace,
                                            numComponents,
                                            whatType);
    
    FileInformation fileInfo(getFileName());
    const AString volumeFileName = FileInformation::assembleFileComponents(fileInfo.getAbsolutePath(),
                                                                           fileInfo.getFileNameNoExtension(),
                                                                           DataFileTypeEnum::toFileExtension(DataFileTypeEnum::VOLUME));
    volumeFile->setFileName(volumeFileName);
    
    int32_t rgbaIndex = 0;
    const int64_t k = 0;
    const int64_t mapIndex = 0;
    for (int64_t j = 0; j < height; j++) {
        for (int64_t i = 0; i < width; i++) {
            switch (colorMode) {
                case CONVERT_TO_VOLUME_COLOR_GRAYSCALE:
                {
                    /*
                     * Luminosity conversion from GIMP
                     * http://docs.gimp.org/2.6/en/gimp-tool-desaturate.html
                     */
                    float intensity = ((rgbaBytes[rgbaIndex] * 0.21)
                                       + (rgbaBytes[rgbaIndex + 1] * 0.72)
                                       + (rgbaBytes[rgbaIndex + 2] * 0.07));
                    if (intensity > 255.0) intensity = 255.0;
                    else if (intensity < 0.0) intensity = 0.0;
                    
                    if (rgbaBytes[rgbaIndex + 3] <= 0.0) {
                        intensity = 0.0;
                    }
                    volumeFile->setValue(intensity, i, j, k, mapIndex, 0);
                    
                    rgbaIndex += 4;
                }
                    break;
                case CONVERT_TO_VOLUME_COLOR_RGB:
                {
                    CaretAssertVectorIndex(rgbaBytes, rgbaIndex);
                    volumeFile->setValue(rgbaBytes[rgbaIndex], i, j, k, mapIndex, 0);
                    CaretAssertVectorIndex(rgbaBytes, rgbaIndex);
                    volumeFile->setValue(rgbaBytes[rgbaIndex+1], i, j, k, mapIndex, 1);
                    CaretAssertVectorIndex(rgbaBytes, rgbaIndex);
                    volumeFile->setValue(rgbaBytes[rgbaIndex+2], i, j, k, mapIndex, 2);
                    if (numComponents == 4) {
                        CaretAssertVectorIndex(rgbaBytes, rgbaIndex);
                        volumeFile->setValue(rgbaBytes[rgbaIndex+3], i, j, k, mapIndex, 3);
                    }
                    rgbaIndex += 4;
                }
                    break;
            }
        }
    }
    
    switch (colorMode) {
        case CONVERT_TO_VOLUME_COLOR_GRAYSCALE:
        {
            PaletteColorMapping* pcm = volumeFile->getMapPaletteColorMapping(mapIndex);
            pcm->setSelectedPaletteToGrayInterpolated();
            pcm->setDisplayNegativeDataFlag(false);
            pcm->setDisplayZeroDataFlag(false);
            pcm->setDisplayPositiveDataFlag(true);
            pcm->setScaleMode(PaletteScaleModeEnum::MODE_AUTO_SCALE);
        }
            break;
        case CONVERT_TO_VOLUME_COLOR_RGB:
            break;
    }
    
    volumeFile->clearVoxelColoringForMap(mapIndex);
    volumeFile->updateScalarColoringForMap(mapIndex);
    
    return volumeFile;
}

/**
 * @return The control point file.
 */
ControlPointFile*
ImageFile::getControlPointFile()
{
    return m_controlPointFile;
}

/**
 * @return The control point file.
 */
const ControlPointFile*
ImageFile::getControlPointFile() const
{
    return m_controlPointFile;
}

/**
 * @return The graphics primitive for drawing the image as a texture in media drawing model.
 */
GraphicsPrimitiveV3fT3f*
ImageFile::getGraphicsPrimitiveForMediaDrawing() const
{
    if (m_image == NULL) {
        return NULL;
    }
    
    if (m_graphicsPrimitiveForMediaDrawing == NULL) {
        std::vector<uint8_t> bytesRGBA;
        int32_t width(0);
        int32_t height(0);
        
        /*
         * Some images may use a color table so convert images
         * if there are not in preferred format prior to
         * getting colors of pixels
         */
        bool validRGBA(false);
        if (m_image->format() != QImage::Format_RGB32) {
            QImage image = m_image->convertToFormat(QImage::Format_RGB32);
            if (! image.isNull()) {
                ImageFile convImageFile;
                convImageFile.setFromQImage(image);
                validRGBA = convImageFile.getImageBytesRGBA(IMAGE_DATA_ORIGIN_AT_BOTTOM,
                                                            bytesRGBA,
                                                            width,
                                                            height);
            }
        }
        else {
            validRGBA = getImageBytesRGBA(IMAGE_DATA_ORIGIN_AT_BOTTOM,
                                          bytesRGBA,
                                          width,
                                          height);
        }
        
        if (validRGBA) {
            GraphicsPrimitiveV3fT3f* primitive = GraphicsPrimitive::newPrimitiveV3fT3f(GraphicsPrimitive::PrimitiveType::OPENGL_TRIANGLE_STRIP,
                                                                                       &bytesRGBA[0],
                                                                                       width,
                                                                                       height,
                                                                                       GraphicsPrimitive::TextureWrappingType::CLAMP,
                                                                                       GraphicsPrimitive::TextureFilteringType::LINEAR);
            /*
             * A Triangle Strip (consisting of two triangles) is used
             * for drawing the image.  At this time, the XYZ coordinates
             * do not matter and they will be updated when the annotation
             * is drawn by a call to ::setVertexBounds().
             * The order of the vertices in the triangle strip is
             * Top Left, Bottom Left, Top Right, Bottom Right.  If this
             * order changes, ::setVertexBounds must be updated.
             *
             * Zeros are used for the X- and Y-coordinates.
             * The third and fourth parameters are the texture
             * S and T coordinates.
             */
            const float halfWidth(width * 0.5);
            const float halfHeight(height * 0.5);
            primitive->addVertex(-halfWidth,  halfHeight, 0, 1);  /* Top Left */
            primitive->addVertex(-halfWidth, -halfHeight, 0, 0);  /* Bottom Left */
            primitive->addVertex(halfWidth,   halfHeight, 1, 1);  /* Top Right */
            primitive->addVertex(halfWidth,  -halfHeight, 1, 0);  /* Bottom Right */
            
            m_graphicsPrimitiveForMediaDrawing.reset(primitive);
        }
    }
    
    return m_graphicsPrimitiveForMediaDrawing.get();
}

/**
 * Save file data from the scene.  For subclasses that need to
 * save to a scene, this method should be overriden.  sceneClass
 * will be valid and any scene data should be added to it.
 *
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    restoring the scene.
 *
 * @param sceneClass
 *     sceneClass to which data members should be added.
 */
void
ImageFile::saveFileDataToScene(const SceneAttributes* sceneAttributes,
                                   SceneClass* sceneClass)
{
    MediaFile::saveFileDataToScene(sceneAttributes,
                                   sceneClass);
    
    if (m_controlPointFile != NULL) {
        sceneClass->addClass(m_controlPointFile->saveToScene(sceneAttributes,
                                                             "m_controlPointFile"));
    }
}

/**
 * Restore file data from the scene.  For subclasses that need to
 * restore from a scene, this method should be overridden. The scene class
 * will be valid and any scene data may be obtained from it.
 *
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    restoring the scene.
 *
 * @param sceneClass
 *     sceneClass for the instance of a class that implements
 *     this interface.  Will NEVER be NULL.
 */
void
ImageFile::restoreFileDataFromScene(const SceneAttributes* sceneAttributes,
                                        const SceneClass* sceneClass)
{
    MediaFile::restoreFileDataFromScene(sceneAttributes,
                                        sceneClass);
    
    m_controlPointFile->restoreFromScene(sceneAttributes,
                                         sceneClass->getClass("m_controlPointFile"));
}

/**
 * @return File casted to an image file (avoids use of dynamic_cast that can be slow)
 */
ImageFile*
ImageFile::castToImageFile()
{
    return this;
}

/**
 * @return File casted to an image file (avoids use of dynamic_cast that can be slow)
 * Overidden in ImageFile
 */
const ImageFile*
ImageFile::castToImageFile() const
{
    return this;
}

/**
 * @param dataFileInformation
 *    Item to which information is added.
 */
void
ImageFile::addToDataFileContentInformation(DataFileContentInformation& dataFileInformation)
{
    MediaFile::addToDataFileContentInformation(dataFileInformation);
    
    if (m_image != NULL) {
        dataFileInformation.addNameAndValue("Width", m_image->width());
        dataFileInformation.addNameAndValue("Height", m_image->height());
        dataFileInformation.addNameAndValue("Color Table", (m_image->colorTable().empty()
                                                            ? "No"
                                                            : "Yes"));
    }
}
