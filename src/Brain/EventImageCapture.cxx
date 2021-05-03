
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

#define __EVENT_IMAGE_CAPTURE_DECLARE__
#include "EventImageCapture.h"
#undef __EVENT_IMAGE_CAPTURE_DECLARE__

#include "CaretAssert.h"
#include "EventTypeEnum.h"
#include "DataFileContentInformation.h"
#include "ImageFile.h"
#include "UnitsConversion.h"

using namespace caret;


    
/**
 * \class caret::EventImageCapture 
 * \brief Event for capturing images.
 * \ingroup GuiQt
 */

/**
 * Constructor for capturing image of window with the image
 * dimensions identical to the dimensions of the window.
 *
 * @param browserWindowIndex
 *    The browser window index.
 */
EventImageCapture::EventImageCapture(const int32_t browserWindowIndex)
: EventImageCapture(browserWindowIndex,
                    0,
                    0,
                    0,
                    0,
                    0,
                    0)
{
}

/**
 * Constructor for capturing image of window with the given sizing.  If
 * the X & Y sizes are both zero, the no image resizing is performed.
 *
 * @param browserWindowIndex
 *    The browser window index.
 * @param captureOffsetX
 *    X-offset for capturing image.
 * @param captureOffsetY
 *    Y-offset for capturing image.
 * @param captureWidth
 *    Width for capturing image.
 * @param captureHeight
 *    Height for capturing image.
 * @param outputWidth
 *    Width of output image in PIXELS
 * @param outputHeight
 *    Height of output image in PIXELS
 */
EventImageCapture::EventImageCapture(const int32_t browserWindowIndex,
                                     const int32_t captureOffsetX,
                                     const int32_t captureOffsetY,
                                     const int32_t captureWidth,
                                     const int32_t captureHeight,
                                     const int32_t outputWidth,
                                     const int32_t outputHeight)
: Event(EventTypeEnum::EVENT_IMAGE_CAPTURE),
m_browserWindowIndex(browserWindowIndex),
m_captureOffsetX(captureOffsetX),
m_captureOffsetY(captureOffsetY),
m_captureWidth(captureWidth),
m_captureHeight(captureHeight),
m_outputWidth(outputWidth),
m_outputHeight(outputHeight)
{
    m_backgroundColor[0] = 0;
    m_backgroundColor[1] = 0;
    m_backgroundColor[2] = 0;
    
    /*
     * 300 DPI is default
     */
    setPixelsPerResolutionUnitValue(ImageResolutionUnitsEnum::PIXELS_PER_INCH,
                                    300.0f);
    
    setMargin(0);
}

/**
 * Constructor for capturing image of window with the given sizing.  If
 * the X & Y sizes are both zero, the no image resizing is performed.
 *
 * @param browserWindowIndex
 *    The browser window index.
 * @param captureOffsetX
 *    X-offset for capturing image.
 * @param captureOffsetY
 *    Y-offset for capturing image.
 * @param captureWidth
 *    Width for capturing image.
 * @param captureHeight
 *    Height for capturing image.
 * @param outputWidth
 *    Width of output image in spatial units
 * @param outputHeight
 *    Height of output image in spatial units
 * @param spatialUnits
 *   Spatial units of imageWidth and imageHeight
 * @param resolutionUnits
 *   Resolution units for pixels
 * @param pixelsPerResolutionUnit
 *   Number of pixels per resolution unit
 */
EventImageCapture::EventImageCapture(const int32_t browserWindowIndex,
                                     const int32_t captureOffsetX,
                                     const int32_t captureOffsetY,
                                     const int32_t captureWidth,
                                     const int32_t captureHeight,
                                     const float outputWidth,
                                     const float outputHeight,
                                     const ImageSpatialUnitsEnum::Enum spatialUnits,
                                     const ImageResolutionUnitsEnum::Enum resolutionUnits,
                                     const float pixelsPerResolutionUnit)
: Event(EventTypeEnum::EVENT_IMAGE_CAPTURE),
m_browserWindowIndex(browserWindowIndex),
m_captureOffsetX(captureOffsetX),
m_captureOffsetY(captureOffsetY),
m_captureWidth(captureWidth),
m_captureHeight(captureHeight),
m_outputWidth(0),
m_outputHeight(0)
{
    setOutputImageWidthAndHeight(outputWidth,
                                 outputHeight,
                                 spatialUnits,
                                 resolutionUnits,
                                 pixelsPerResolutionUnit);
}


/**
 * Destructor.
 */
EventImageCapture::~EventImageCapture()
{
}

/**
 * @return The browser window index.
 */
int32_t
EventImageCapture::getBrowserWindowIndex() const
{
    return m_browserWindowIndex;
}

/**
 * @return The capture X offset
 */
int32_t
EventImageCapture::getCaptureOffsetX() const
{
    return m_captureOffsetX;
}

/**
 * @return The capture Y offset
 */
int32_t
EventImageCapture::getCaptureOffsetY() const
{
    return m_captureOffsetY;
}

/**
 * @return The capture width
 */
int32_t
EventImageCapture::getCaptureWidth() const
{
    return m_captureWidth;
}

/**
 * @return The capture height
 */
int32_t
EventImageCapture::getCaptureHeight() const
{
    return m_captureHeight;
}

/**
 * @return The output image width including the margin
 */
int32_t
EventImageCapture::getOutputWidthIncludingMargin() const
{
    return m_outputWidth;
}

/**
 * @return The output image height including the margin
 */
int32_t
EventImageCapture::getOutputHeightIncludingMargin() const
{
    return m_outputHeight;
}

/**
 * @return The output image width excluding the margin
 */
int32_t
EventImageCapture::getOutputWidthExcludingMargin() const
{
    /*
     * Zero indicates use size of window for image
     */
    if (m_outputWidth <= 0) {
        return 0;
    }
    
    float width(m_outputWidth - (m_margin * 2.0f));
    if (width < 10) {
        width = 10;
    }
    return width;
}

/**
 * @return The output image height excluding the margin
 */
int32_t
EventImageCapture::getOutputHeightExcludingMargin() const
{
    /*
     * Zero indicates use size of window for image
     */
    if (m_outputHeight <= 0) {
        return 0;
    }

    float height(m_outputHeight - (m_margin * 2.0f));
    if (height < 10) {
        height = 10;
    }
    return height;
}

/**
 * @return Width of image in the given spatial units
 * @param spatialUnits
 *    The spatial units type
 */
float
EventImageCapture::getImageWidthInSpatialUnits(const ImageSpatialUnitsEnum::Enum spatialUnits) const
{
    return convertPixelsToSpatialUnits(spatialUnits,
                                       getOutputWidthIncludingMargin());
}

/**
 * @return Height of image in the given spatial units
 * @param spatialUnits
 *    The spatial units type
 */
float
EventImageCapture::getImageHeightInSpatialUnits(const ImageSpatialUnitsEnum::Enum spatialUnits) const
{
    return convertPixelsToSpatialUnits(spatialUnits,
                                       getOutputHeightIncludingMargin());
}

/**
 * @return Pixel value converted to the given spatial units
 * @param spatialUnits
 *    The spatial units type
 * @param numberOfPixels
 *    The number of pixelzx
 */
float
EventImageCapture::convertPixelsToSpatialUnits(const ImageSpatialUnitsEnum::Enum spatialUnits,
                                               const float numberOfPixels) const
{
    float valueOut(0.0);
    
    float pixelsPerUnit(0.0);
    switch (spatialUnits) {
        case ImageSpatialUnitsEnum::CENTIMETERS:
            pixelsPerUnit = getPixelsPerResolutionUnitValue(ImageResolutionUnitsEnum::PIXELS_PER_CENTIMETER);
            break;
        case ImageSpatialUnitsEnum::INCHES:
            pixelsPerUnit = getPixelsPerResolutionUnitValue(ImageResolutionUnitsEnum::PIXELS_PER_INCH);
            break;
        case ImageSpatialUnitsEnum::METERS:
            pixelsPerUnit = getPixelsPerResolutionUnitValue(ImageResolutionUnitsEnum::PIXELS_PER_METER);
            break;
        case ImageSpatialUnitsEnum::MILLIMETERS:
            pixelsPerUnit = getPixelsPerResolutionUnitValue(ImageResolutionUnitsEnum::PIXELS_PER_MILLIMETER);
            break;
        case ImageSpatialUnitsEnum::PIXELS:
            pixelsPerUnit = 1.0f;
            break;
    }
    
    if (pixelsPerUnit > 0.0) {
        valueOut = numberOfPixels / pixelsPerUnit;
    }
    
    return valueOut;
}


/**
 * Get the graphics area's background color.
 *
 * @param backgroundColor
 *    RGB components of background color [0, 255]
 */
void
EventImageCapture::getBackgroundColor(uint8_t backgroundColor[3]) const
{
    backgroundColor[0] = m_backgroundColor[0];
    backgroundColor[1] = m_backgroundColor[1];
    backgroundColor[2] = m_backgroundColor[2];
}

/**
 * Set the graphics area's background color.
 *
 * @param backgroundColor
 *    RGB components of background color [0, 255]
 */
void
EventImageCapture::setBackgroundColor(const uint8_t backgroundColor[3])
{
    m_backgroundColor[0] = backgroundColor[0];
    m_backgroundColor[1] = backgroundColor[1];
    m_backgroundColor[2] = backgroundColor[2];
}

/**
 * @return The captured image.
 */
QImage
EventImageCapture::getCapturedImage() const
{
    ImageFile imageFileOut;
    imageFileOut.setFromQImage(m_image);
    
    if (m_margin > 0) {
        /*
         * While it would be more efficent to create a larger image that includes the margin and drawn
         * into a sub-viewport in the image, we intentionallly do not do that.  The reason is that there
         * are limitations on OpenGL rendering width and height and if a large margin is used, that limit
         * may be hit.  The offscreen Mesa system limits the viewport to 4096 (at least in version we use).
         */
        imageFileOut.addMargin(m_margin,
                                m_backgroundColor);
    }
    
    const float resolutionPixelsPerMeter = getPixelsPerResolutionUnitValue(ImageResolutionUnitsEnum::PIXELS_PER_METER);
    imageFileOut.setDotsPerMeter(resolutionPixelsPerMeter,
                                  resolutionPixelsPerMeter);
    
    return *imageFileOut.getAsQImage();
}

/**
 * Set the captured image.
 */
void
EventImageCapture::setCapturedImage(const QImage& image)
{
    m_image = image;
}

/**
 * @return The margin
 */
int32_t
EventImageCapture::getMargin() const
{
    return m_margin;
}

/**
 * Set the margin
 * @param margin
 *    New margin value
 */
void
EventImageCapture::setMargin(const int32_t margin)
{
    m_margin = margin;
}

/**
 * @return The pixels per resolution for the given resolution units
 * @param resolutionUnits
 *    Units for resolution
 */
float
EventImageCapture::getPixelsPerResolutionUnitValue(const ImageResolutionUnitsEnum::Enum resolutionUnits) const
{
    float resolutionOut(1.0f);
    
    UnitsConversion::LengthUnits fromUnits(UnitsConversion::LengthUnits::METERS);
    const UnitsConversion::LengthUnits toUnits(UnitsConversion::LengthUnits::INCHES);
    switch (resolutionUnits) {
        case ImageResolutionUnitsEnum::PIXELS_PER_CENTIMETER:
            fromUnits = UnitsConversion::LengthUnits::CENTIMETERS;
            break;
        case ImageResolutionUnitsEnum::PIXELS_PER_INCH:
            fromUnits = UnitsConversion::LengthUnits::INCHES;
            break;
        case ImageResolutionUnitsEnum::PIXELS_PER_METER:
            fromUnits = UnitsConversion::LengthUnits::METERS;
            break;
        case ImageResolutionUnitsEnum::PIXELS_PER_MILLIMETER:
            fromUnits = UnitsConversion::LengthUnits::MILLIMETERS;
            break;
    }
    
    resolutionOut = UnitsConversion::convertLength(fromUnits,
                                                   toUnits,
                                                   m_resolutionPixelsPerInch);
    return resolutionOut;
}

/**
 * Set the pixels per resolution unit
 * @param resolutionUnits
 *    Units for resolution
 * @param resolutionUnitsValue
 *    Resolution value
 */
void
EventImageCapture::setPixelsPerResolutionUnitValue(const ImageResolutionUnitsEnum::Enum resolutionUnits,
                                                   const float resolutionUnitsValue)
{
    /*
     * from/to units are reversed since 'pixels per unit'
     */
    const UnitsConversion::LengthUnits fromUnits(UnitsConversion::LengthUnits::INCHES);
    UnitsConversion::LengthUnits toUnits(UnitsConversion::LengthUnits::METERS);
    switch (resolutionUnits) {
        case ImageResolutionUnitsEnum::PIXELS_PER_CENTIMETER:
            toUnits = UnitsConversion::LengthUnits::CENTIMETERS;
            break;
        case ImageResolutionUnitsEnum::PIXELS_PER_INCH:
            toUnits = UnitsConversion::LengthUnits::INCHES;
            break;
        case ImageResolutionUnitsEnum::PIXELS_PER_METER:
            toUnits = UnitsConversion::LengthUnits::METERS;
            break;
        case ImageResolutionUnitsEnum::PIXELS_PER_MILLIMETER:
            toUnits = UnitsConversion::LengthUnits::MILLIMETERS;
            break;
    }
    
    m_resolutionPixelsPerInch = UnitsConversion::convertLength(fromUnits,
                                                               toUnits,
                                                               resolutionUnitsValue);
}

/**
 * Set the output image width and height for the given units
 * @param imageWidth
 *   Width for output image
 * @param imageHeight
 *   Height for output image
 * @param spatialUnits
 *   Spatial units of imageWidth and imageHeight
 * @param resolutionUnits
 *   Resolution units for pixels
 * @param pixelsPerResolutionUnit
 *   Number of pixels per resolution unit
 */
void
EventImageCapture::setOutputImageWidthAndHeight(const float imageWidth,
                                                const float imageHeight,
                                                const ImageSpatialUnitsEnum::Enum spatialUnits,
                                                const ImageResolutionUnitsEnum::Enum resolutionUnits,
                                                const float pixelsPerResolutionUnit)
{
    /*
     * Set the resolution with the given resolution units
     */
    setPixelsPerResolutionUnitValue(resolutionUnits,
                                    pixelsPerResolutionUnit);


    float pixelWidth(imageWidth);
    float pixelHeight(imageHeight);
    
    if (spatialUnits != ImageSpatialUnitsEnum::PIXELS) {
        /*
         * Convert the width/height using the given spatial units to pixels
         */
        UnitsConversion::LengthUnits fromUnits = UnitsConversion::LengthUnits::INCHES;
        switch (spatialUnits) {
            case ImageSpatialUnitsEnum::CENTIMETERS:
                fromUnits = UnitsConversion::LengthUnits::CENTIMETERS;
                break;
            case ImageSpatialUnitsEnum::INCHES:
                fromUnits = UnitsConversion::LengthUnits::INCHES;
                break;
            case ImageSpatialUnitsEnum::METERS:
                fromUnits = UnitsConversion::LengthUnits::METERS;
                break;
            case ImageSpatialUnitsEnum::MILLIMETERS:
                fromUnits = UnitsConversion::LengthUnits::MILLIMETERS;
                break;
            case ImageSpatialUnitsEnum::PIXELS:
                break;
        }
        
        const float inchesWidth = UnitsConversion::convertLength(fromUnits,
                                                                 UnitsConversion::LengthUnits::INCHES,
                                                                 imageWidth);
        const float inchesHeight = UnitsConversion::convertLength(fromUnits,
                                                                  UnitsConversion::LengthUnits::INCHES,
                                                                  imageHeight);

        const float pixelsPerInch = getPixelsPerResolutionUnitValue(ImageResolutionUnitsEnum::PIXELS_PER_INCH);
        
        pixelWidth  = inchesWidth  * pixelsPerInch;
        pixelHeight = inchesHeight * pixelsPerInch;
    }
    
    m_outputWidth  = static_cast<int32_t>(pixelWidth);
    m_outputHeight = static_cast<int32_t>(pixelHeight);
}

AString
EventImageCapture::toImageInfoText(const AString& optionalImageName) const
{
    DataFileContentInformation dataFileInfo;
    
    if ( ! optionalImageName.isEmpty()) {
        const float aspectRatio((m_outputWidth > 0)
                                ? (static_cast<float>(m_outputHeight)
                                   / static_cast<float>(m_outputWidth))
                                : 0.0f);
        dataFileInfo.addNameAndValue("File Name", optionalImageName);
        dataFileInfo.addNameAndValue("Width",  getOutputWidthIncludingMargin());
        dataFileInfo.addNameAndValue("Height", getOutputHeightIncludingMargin());
        dataFileInfo.addNameAndValue("Aspect (h/w)", aspectRatio);

        std::vector<ImageSpatialUnitsEnum::Enum> allSpatialUnits;
        ImageSpatialUnitsEnum::getAllEnums(allSpatialUnits);
        for (const auto spatialUnit : allSpatialUnits) {
            dataFileInfo.addNameAndValue(QString("Width (%1)").arg(ImageSpatialUnitsEnum::toName(spatialUnit)),
                                         getImageWidthInSpatialUnits(spatialUnit));
            dataFileInfo.addNameAndValue(QString("Height (%1)").arg(ImageSpatialUnitsEnum::toName(spatialUnit)),
                                         getImageHeightInSpatialUnits(spatialUnit));
        }
        dataFileInfo.addNameAndValue("Margin", getMargin());
        
        std::vector<ImageResolutionUnitsEnum::Enum> allResUnits;
        ImageResolutionUnitsEnum::getAllEnums(allResUnits);
        for (const auto resUnit : allResUnits) {
            dataFileInfo.addNameAndValue(QString("%1").arg(ImageResolutionUnitsEnum::toName(resUnit)),
                                         getPixelsPerResolutionUnitValue(resUnit));
        }
    }
    
    return dataFileInfo.getInformationInString();
}



