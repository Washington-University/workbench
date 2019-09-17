
/*LICENSE_START*/
/*
 *  Copyright (C) 2015 Washington University School of Medicine
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

#define __IMAGE_CAPTURE_SETTINGS_DECLARE__
#include "ImageCaptureSettings.h"
#undef __IMAGE_CAPTURE_SETTINGS_DECLARE__

#include "CaretAssert.h"
#include "SceneClass.h"
#include "SceneClassAssistant.h"

using namespace caret;


    
/**
 * \class caret::ImageCaptureSettings 
 * \brief Image capture properties
 * \ingroup Files
 *
 * Allows one to adjust dimensions for both pixels and spatial units.
 * Internally, spatial processing is in centimeters.
 */

/**
 * Constructor.
 */
ImageCaptureSettings::ImageCaptureSettings()
: CaretObject(),
SceneableInterface()
{
    m_aspectRatio = 1.0;
    m_pixelsPerCentimeter = 72.0 / CENTIMETERS_PER_INCH;
    
    m_croppingEnabled = false;
    m_croppingMargin  = 10;
    m_copyToClipboardEnabled = true;
    m_saveToFileEnabled      = false;
    m_scaleProportionatelyEnabled = true;
    m_imageFileName          = "untitled.png";
    
    m_cropToTabWindowLockAspectRegionEnabled = true;
    
    m_dimensionsMode       = ImageCaptureDimensionsModeEnum::IMAGE_CAPTURE_DIMENSIONS_MODE_WINDOW_SIZE;
    m_imageResolutionUnits = ImageResolutionUnitsEnum::PIXELS_PER_INCH;
    m_spatialUnits         = ImageSpatialUnitsEnum::INCHES;
    
    setPixelWidthAndHeight(512, 512);
    
    /* WB-868 Defautl Custom, 300 DPI, 7.5 inches width, copy to clipboard OFF, save to file ON, no file name*/
    m_dimensionsMode = ImageCaptureDimensionsModeEnum::IMAGE_CAPTURE_DIMENSIONS_MODE_CUSTOM;
    setImageResolutionInSelectedUnits(300.0);
    setSpatialWidth(7.5);
    m_saveToFileEnabled = true;
    m_copyToClipboardEnabled = false;
    m_imageFileName = "";
    
    
    m_sceneAssistant = new SceneClassAssistant();
    m_sceneAssistant->add("m_pixelWidth",
                          &m_pixelWidth);
    m_sceneAssistant->add("m_pixelHeight",
                          &m_pixelHeight);
    m_sceneAssistant->add("m_centimetersWidth",
                          &m_centimetersWidth);
    m_sceneAssistant->add("m_centimetersHeight",
                          &m_centimetersHeight);
    m_sceneAssistant->add("m_pixelsPerCentimeter",
                          &m_pixelsPerCentimeter);
    m_sceneAssistant->add("m_aspectRatio",
                          &m_aspectRatio);

    m_sceneAssistant->add("m_croppingEnabled",
                          &m_croppingEnabled);
    m_sceneAssistant->add("m_croppingMargin",
                          &m_croppingMargin);
    m_sceneAssistant->add("m_copyToClipboardEnabled",
                          &m_copyToClipboardEnabled);
    m_sceneAssistant->add("m_saveToFileEnabled",
                          &m_saveToFileEnabled);
    m_sceneAssistant->add("m_scaleProportionatelyEnabled",
                          &m_scaleProportionatelyEnabled);
    
    m_sceneAssistant->add("m_cropToTabWindowLockAspectRegionEnabled",
                          &m_cropToTabWindowLockAspectRegionEnabled);
    
    m_sceneAssistant->add<ImageCaptureDimensionsModeEnum, ImageCaptureDimensionsModeEnum::Enum>("m_dimensionsMode",
                                                                                                &m_dimensionsMode);
    m_sceneAssistant->add<ImageResolutionUnitsEnum, ImageResolutionUnitsEnum::Enum>("m_imageResolutionUnits",
                                                                                                &m_imageResolutionUnits);
    m_sceneAssistant->add<ImageSpatialUnitsEnum, ImageSpatialUnitsEnum::Enum>("m_spatialUnits",
                                                                              &m_spatialUnits);
}

/**
 * Destructor.
 */
ImageCaptureSettings::~ImageCaptureSettings()
{
    delete m_sceneAssistant;
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
ImageCaptureSettings::ImageCaptureSettings(const ImageCaptureSettings& obj)
: CaretObject(obj),
SceneableInterface(obj)
{
    this->copyHelperImageDimensionsModel(obj);
}

/**
 * Assignment operator.
 * @param obj
 *    Data copied from obj to this.
 * @return 
 *    Reference to this object.
 */
ImageCaptureSettings&
ImageCaptureSettings::operator=(const ImageCaptureSettings& obj)
{
    if (this != &obj) {
        CaretObject::operator=(obj);
        this->copyHelperImageDimensionsModel(obj);
    }
    return *this;    
}

/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void 
ImageCaptureSettings::copyHelperImageDimensionsModel(const ImageCaptureSettings& obj)
{
    m_pixelWidth                  = obj.m_pixelWidth;
    m_pixelHeight                 = obj.m_pixelHeight;
    m_centimetersWidth            = obj.m_centimetersWidth;
    m_centimetersHeight           = obj.m_centimetersHeight;
    m_pixelsPerCentimeter         = obj.m_pixelsPerCentimeter;
    m_aspectRatio                 = obj.m_aspectRatio;
    m_scaleProportionatelyEnabled = obj.m_scaleProportionatelyEnabled;
    m_croppingMargin              = obj.m_croppingMargin;
    m_croppingEnabled             = obj.m_croppingEnabled;
    m_copyToClipboardEnabled      = obj.m_copyToClipboardEnabled;
    m_saveToFileEnabled           = obj.m_saveToFileEnabled;
    m_imageFileName               = obj.m_imageFileName;
    m_dimensionsMode              = obj.m_dimensionsMode;
    m_imageResolutionUnits        = obj.m_imageResolutionUnits;
    m_spatialUnits                = obj.m_spatialUnits;
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
ImageCaptureSettings::toString() const
{
    return "ImageCaptureSettings";
}

/**
 * @return The width in pixels.
 */
int32_t
ImageCaptureSettings::getPixelWidth() const
{
    /*
     * Internally, pixels are float, so round (by adding 0.5)
     * and return as integer.
     */
    return static_cast<int32_t>(m_pixelWidth + 0.5);
}

/**
 * @return The height in pixels.
 */
int32_t
ImageCaptureSettings::getPixelHeight() const
{
    /*
     * Internally, pixels are float, so round (by adding 0.5)
     * and return as integer.
     */
    return static_cast<int32_t>(m_pixelHeight + 0.5);
}

/**
 * Get the spatial width.
 *
 * @return
 *   Width in spatial units.
 */
float
ImageCaptureSettings::getSpatialWidth() const
{
    float width = 1.0;
    
    switch (m_spatialUnits) {
        case ImageSpatialUnitsEnum::CENTIMETERS:
            width = m_centimetersWidth;
            break;
        case ImageSpatialUnitsEnum::INCHES:
            width = m_centimetersWidth / CENTIMETERS_PER_INCH;
            break;
        case ImageSpatialUnitsEnum::MILLIMETERS:
            width = m_centimetersWidth * MILLIMETERS_PER_CENTIMETER;
            break;
    }
    
    return width;
}

/**
 * Get the spatial height.
 *
 * @return
 *   Height in spatial units.
 */
float
ImageCaptureSettings::getSpatialHeight() const
{
    float height = 1.0;
    
    switch (m_spatialUnits) {
        case ImageSpatialUnitsEnum::CENTIMETERS:
            height = m_centimetersHeight;
            break;
        case ImageSpatialUnitsEnum::INCHES:
            height = m_centimetersHeight / CENTIMETERS_PER_INCH;
            break;
        case ImageSpatialUnitsEnum::MILLIMETERS:
            height = m_centimetersHeight * MILLIMETERS_PER_CENTIMETER;
            break;
    }
    
    return height;
}

/**
 * Get the image resolution (number of pixels per given spatial unit).
 *
 * @return
 *   Number of pixels per spatial unit.
 */
float
ImageCaptureSettings::getImageResolutionInCentimeters() const
{
    return m_pixelsPerCentimeter;
}


/**
 * Get the image resolution in the selected units
 *
 * @return
 *   Number of pixels per selected resolution unit.
 */
float
ImageCaptureSettings::getImageResolutionInSelectedUnits() const
{
    float pixelsPerUnit = 1.0;
    
    switch (m_imageResolutionUnits) {
        case ImageResolutionUnitsEnum::PIXEL_PER_CENTIMETER:
            pixelsPerUnit = m_pixelsPerCentimeter;
            break;
        case ImageResolutionUnitsEnum::PIXELS_PER_INCH:
            pixelsPerUnit = m_pixelsPerCentimeter * CENTIMETERS_PER_INCH;
            break;
    }
    
    return pixelsPerUnit;
}

/**
 * Set the width and height of the image in pixels.
 *
 * @param pixelWidth
 *    New pixel width.
 * @param pixelHeight
 *    New pixel height.
 */
void
ImageCaptureSettings::setPixelWidthAndHeight(const int32_t pixelWidth,
                                             const int32_t pixelHeight)
{
    CaretAssert(pixelWidth > 0);
    CaretAssert(pixelHeight > 0);
    
    m_pixelWidth = pixelWidth;
    m_pixelHeight = pixelHeight;
    updateSpatialWidthAndHeightFromPixelWidthAndHeight();
}

/**
 * Set the pixel width to the given width.
 * Width may change if proportionate scaling is enabled.
 *
 * @param pixelWidth
 *    New pixel width.
 */
void
ImageCaptureSettings::setPixelWidth(const int32_t pixelWidth)
{
    CaretAssert(pixelWidth > 0);
    
    const float aspectRatio = getAspectRatio();
    
    m_pixelWidth = pixelWidth;
    
    if (m_scaleProportionatelyEnabled) {
        m_pixelHeight = m_pixelWidth * aspectRatio;
    }
    
    updateSpatialWidthAndHeightFromPixelWidthAndHeight();
}

/**
 * Set the pixel height to the given height.
 * Width may change if proportionate scaling is enabled.
 *
 * @param pixelHeight
 *    New pixel height.
 */
void
ImageCaptureSettings::setPixelHeight(const int32_t pixelHeight)
{
    CaretAssert(pixelHeight > 0);
    
    const float aspectRatio = getAspectRatio();
    
    m_pixelHeight = pixelHeight;
    
    if (m_scaleProportionatelyEnabled) {
        m_pixelWidth = m_pixelHeight / aspectRatio;
    }
    
    updateSpatialWidthAndHeightFromPixelWidthAndHeight();
}

/**
 * Set the spatial width to the given width.
 * Height may change if proportionate scaling is enabled.
 *
 * @param spatialWidth
 *    New spatial width.
 */
void
ImageCaptureSettings::setSpatialWidth(const float spatialWidth)
{
    CaretAssert(spatialWidth > 0);
    
    const float aspectRatio = getAspectRatio();
    
    switch (m_spatialUnits) {
        case ImageSpatialUnitsEnum::MILLIMETERS:
            m_centimetersWidth = spatialWidth / MILLIMETERS_PER_CENTIMETER;
            break;
        case ImageSpatialUnitsEnum::INCHES:
            m_centimetersWidth = spatialWidth * CENTIMETERS_PER_INCH;
            break;
        case ImageSpatialUnitsEnum::CENTIMETERS:
            m_centimetersWidth = spatialWidth;
            break;
    }
    
    if (m_scaleProportionatelyEnabled) {
        m_centimetersHeight = m_centimetersWidth * aspectRatio;
    }
    
    updatePixelWidthAndHeightFromSpatialWidthAndHeight();
}

/**
 * Set the spatial height to the given height.
 * Width may change if proportionate scaling is enabled.
 *
 * @param spatialHeight
 *    New spatial height.
 */
void
ImageCaptureSettings::setSpatialHeight(const float spatialHeight)
{
    CaretAssert(spatialHeight > 0);
    
    const float aspectRatio = getAspectRatio();
    
    switch (m_spatialUnits) {
        case ImageSpatialUnitsEnum::MILLIMETERS:
            m_centimetersHeight = spatialHeight / MILLIMETERS_PER_CENTIMETER;
            break;
        case ImageSpatialUnitsEnum::INCHES:
            m_centimetersHeight = spatialHeight * CENTIMETERS_PER_INCH;
            break;
        case ImageSpatialUnitsEnum::CENTIMETERS:
            m_centimetersHeight = spatialHeight;
            break;
    }
    
    if (m_scaleProportionatelyEnabled) {
        m_centimetersWidth = m_centimetersHeight / aspectRatio;
    }
    
    updatePixelWidthAndHeightFromSpatialWidthAndHeight();
}

/**
 * Set the image resolution (number of pixels per spatial unit to the given value).
 *
 * @param imageResolutionInSelectedUnits
 *    New value for number of pixels per selected resolution unit
 */
void
ImageCaptureSettings::setImageResolutionInSelectedUnits(const float imageResolutionInSelectedUnits)
{
    CaretAssert(imageResolutionInSelectedUnits > 0);
    
    switch (m_imageResolutionUnits) {
        case ImageResolutionUnitsEnum::PIXELS_PER_INCH:
            m_pixelsPerCentimeter = imageResolutionInSelectedUnits / CENTIMETERS_PER_INCH;
            break;
        case ImageResolutionUnitsEnum::PIXEL_PER_CENTIMETER:
            m_pixelsPerCentimeter = imageResolutionInSelectedUnits;
            break;
    }
    
    updatePixelWidthAndHeightFromSpatialWidthAndHeight();
}

/**
 * Update the pixel width and height after a change to the spatial width
 * and/or height.
 */
void
ImageCaptureSettings::updatePixelWidthAndHeightFromSpatialWidthAndHeight()
{
    m_pixelWidth  = m_centimetersWidth  * m_pixelsPerCentimeter;
    m_pixelHeight = m_centimetersHeight * m_pixelsPerCentimeter;
}

/**
 * Update the spatial width and height after a change to the pixel width
 * and/or height.
 */
void
ImageCaptureSettings::updateSpatialWidthAndHeightFromPixelWidthAndHeight()
{
    m_centimetersWidth  = m_pixelWidth  / m_pixelsPerCentimeter;
    m_centimetersHeight = m_pixelHeight / m_pixelsPerCentimeter;
}

/**
 * @return the aspect ration (height / width).
 */
float
ImageCaptureSettings::getAspectRatio() const
{
    return m_aspectRatio;
}

/**
 * Update the pixel and image dimensions for the aspect ratio
 * determined by the given width and height.  Aspect ratio
 * is (height / width) and the height will be changed using
 * the aspect ratio.
 *
 * @param width
 *    Width used in denominator of aspect ratio.
 * @param height
 *    Height used in numerator of aspect ratio.
 */
void
ImageCaptureSettings::updateForAspectRatio(const float width,
                                           const float height)
{
    if ((width > 0.0)
        && (height > 0.0)) {
        m_aspectRatio = height / width;
        m_pixelHeight = m_aspectRatio * m_pixelWidth;
        
        updateSpatialWidthAndHeightFromPixelWidthAndHeight();
    }
}

/**
 * @return The cropping margin.
 */
int32_t
ImageCaptureSettings::getCroppingMargin() const
{
    return m_croppingMargin;
}

/**
 * Set the cropping margin.
 * 
 * @param croppingMargin
 *     New cropping margin.
 */
void
ImageCaptureSettings::setCroppingMargin(const int32_t croppingMargin)
{
    m_croppingMargin = croppingMargin;
}

/**
 * @return Is cropping enabled?
 */
bool
ImageCaptureSettings::isCroppingEnabled() const
{
    return m_croppingEnabled;
}

/**
 * Set cropping enabled.
 *
 * @param enabled
 *     New enabled status.
 */
void
ImageCaptureSettings::setCroppingEnabled(const bool enabled)
{
    m_croppingEnabled = enabled;
}

/**
 * @return Is copy to clipboard enabled?
 */
bool
ImageCaptureSettings::isCopyToClipboardEnabled() const
{
    return m_copyToClipboardEnabled;
}

/**
 * Set copy to clipboard enabled.
 *
 * @param enabled
 *     New enabled status.
 */
void
ImageCaptureSettings::setCopyToClipboardEnabled(const bool enabled)
{
    m_copyToClipboardEnabled = enabled;
}

/**
 * @return Is save to file enabled?
 */
bool
ImageCaptureSettings::isSaveToFileEnabled() const
{
    return m_saveToFileEnabled;
}

/**
 * Set save to file enabled.
 *
 * @param enabled
 *     New enabled status.
 */
void
ImageCaptureSettings::setSaveToFileEnabled(const bool enabled)
{
    m_saveToFileEnabled = enabled;
}

/**
 * @return Image file name
 */
AString
ImageCaptureSettings::getImageFileName() const
{
    return m_imageFileName;
}

/**
 * @return Is scale proportionately enabled?
 */
bool
ImageCaptureSettings::isScaleProportionately() const
{
    return m_scaleProportionatelyEnabled;
}

/**
 * Set scale proportionately enabled.
 *
 * @param enabled
 *     New enabled status.
 */
void
ImageCaptureSettings::setScaleProportionately(const bool enabled)
{
    m_scaleProportionatelyEnabled = enabled;
}

/**
 * Set the image file name.
 *
 * @param filename
 *     Name for image file.
 */
void
ImageCaptureSettings::setImageFileName(const AString& filename)
{
    m_imageFileName = filename;
}

/**
 * @return Image dimensions mode
 */
ImageCaptureDimensionsModeEnum::Enum
ImageCaptureSettings::getImageCaptureDimensionsMode() const
{
    return m_dimensionsMode;
}

/**
 * @return Image resolution units
 */
ImageResolutionUnitsEnum::Enum
ImageCaptureSettings::getImageResolutionUnits() const
{
    return m_imageResolutionUnits;
}

/**
 * Set the image resolution units
 *
 * @param imageResolutionUnits
 *     New value for image resolution units
 */
void
ImageCaptureSettings::setImageResolutionUnits(const ImageResolutionUnitsEnum::Enum imageResolutionUnits)
{
    m_imageResolutionUnits = imageResolutionUnits;
    updateSpatialWidthAndHeightFromPixelWidthAndHeight();
}

/**
 * @return The spatial units.
 */
ImageSpatialUnitsEnum::Enum
ImageCaptureSettings::getSpatialUnits() const
{
    return m_spatialUnits;
}

/**
 * Set the spatial units.
 *
 * @param spatialUnits
 *     New value for spatial units.
 */
void
ImageCaptureSettings::setSpatialUnits(const ImageSpatialUnitsEnum::Enum spatialUnits)
{
    m_spatialUnits = spatialUnits;
    updatePixelWidthAndHeightFromSpatialWidthAndHeight();
}

/**
 * Set the image dimensions mode.
 *
 * @param mode
 *     New mode.
 */
void
ImageCaptureSettings::setImageCaptureDimensionsMode(const ImageCaptureDimensionsModeEnum::Enum mode)
{
    m_dimensionsMode = mode;
}

/**
 * @param Is crop to tab/window lock aspect ratio enabled?
 */
bool
ImageCaptureSettings::isCropToTabWindowLockAspectRegionEnabled() const
{
    return m_cropToTabWindowLockAspectRegionEnabled;
}

/**
 * Set crop to tab/window lock aspect ratio enabled.
 *
 * @param enabled
 *    New status.
 */
void
ImageCaptureSettings::setCropToTabWindowLockAspectRegionEnabled(const bool enabled)
{
    m_cropToTabWindowLockAspectRegionEnabled = enabled;
}


/**
 * Save information specific to this type of model to the scene.
 *
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    saving the scene.
 *
 * @param instanceName
 *    Name of instance in the scene.
 */
SceneClass*
ImageCaptureSettings::saveToScene(const SceneAttributes* sceneAttributes,
                                 const AString& instanceName)
{
    SceneClass* sceneClass = new SceneClass(instanceName,
                                            "ImageCaptureSettings",
                                            1);
    m_sceneAssistant->saveMembers(sceneAttributes,
                                  sceneClass);

    sceneClass->addPathName("m_imageFileName",
                            m_imageFileName);
    
    // Uncomment if sub-classes must save to scene
    //saveSubClassDataToScene(sceneAttributes,
    //                        sceneClass);
    
    return sceneClass;
}

/**
 * Restore information specific to the type of model from the scene.
 *
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    restoring the scene.
 *
 * @param sceneClass
 *     sceneClass from which model specific information is obtained.
 */
void
ImageCaptureSettings::restoreFromScene(const SceneAttributes* sceneAttributes,
                                      const SceneClass* sceneClass)
{
    if (sceneClass == NULL) {
        return;
    }
    
    m_sceneAssistant->restoreMembers(sceneAttributes,
                                     sceneClass);    
    
    m_imageFileName = sceneClass->getPathNameValue("m_imageFileName",
                                                   "untitled.png");
    
    //Uncomment if sub-classes must restore from scene
    //restoreSubClassDataFromScene(sceneAttributes,
    //                             sceneClass);
    
}


