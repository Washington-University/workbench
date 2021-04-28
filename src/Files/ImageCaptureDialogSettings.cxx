
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

#define __IMAGE_CAPTURE_DIALOG_SETTINGS_DECLARE__
#include "ImageCaptureDialogSettings.h"
#undef __IMAGE_CAPTURE_DIALOG_SETTINGS_DECLARE__

#include "CaretAssert.h"
#include "DataFileContentInformation.h"
#include "SceneClass.h"
#include "SceneClassAssistant.h"

using namespace caret;


    
/**
 * \class caret::ImageCaptureDialogSettings 
 * \brief Image capture properties
 * \ingroup Files
 *
 * Allows one to adjust dimensions for both pixels and spatial units.
 * Internally, spatial processing is in centimeters.
 */

/**
 * Constructor.
 */
ImageCaptureDialogSettings::ImageCaptureDialogSettings()
: CaretObject(),
SceneableInterface()
{
    m_aspectRatio = 1.0;
    m_pixelsPerCentimeter = 72.0 / CENTIMETERS_PER_INCH;
    
    m_croppingEnabled = false;
    m_margin  = 0;
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
    m_sceneAssistant->add("m_margin",
                          &m_margin);
    m_sceneAssistant->add("m_croppingEnabled",
                          &m_croppingEnabled);
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
ImageCaptureDialogSettings::~ImageCaptureDialogSettings()
{
    delete m_sceneAssistant;
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
ImageCaptureDialogSettings::ImageCaptureDialogSettings(const ImageCaptureDialogSettings& obj)
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
ImageCaptureDialogSettings&
ImageCaptureDialogSettings::operator=(const ImageCaptureDialogSettings& obj)
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
ImageCaptureDialogSettings::copyHelperImageDimensionsModel(const ImageCaptureDialogSettings& obj)
{
    m_pixelWidth                  = obj.m_pixelWidth;
    m_pixelHeight                 = obj.m_pixelHeight;
    m_centimetersWidth            = obj.m_centimetersWidth;
    m_centimetersHeight           = obj.m_centimetersHeight;
    m_pixelsPerCentimeter         = obj.m_pixelsPerCentimeter;
    m_aspectRatio                 = obj.m_aspectRatio;
    m_scaleProportionatelyEnabled = obj.m_scaleProportionatelyEnabled;
    m_margin                      = obj.m_margin;
    m_croppingEnabled             = obj.m_croppingEnabled;
    m_copyToClipboardEnabled      = obj.m_copyToClipboardEnabled;
    m_saveToFileEnabled           = obj.m_saveToFileEnabled;
    m_imageFileName               = obj.m_imageFileName;
    m_dimensionsMode              = obj.m_dimensionsMode;
    m_imageResolutionUnits        = obj.m_imageResolutionUnits;
    m_spatialUnits                = obj.m_spatialUnits;
}

/**
 * Get the settings in a string for display to user
 * @param windowIndices
 *    Indices of the windows
 * @param windowWidths
 *    Widths of windows from scene
 * @param windowHeights
 *    Heights of windows from scene
 */
AString
ImageCaptureDialogSettings::getSettingsAsText(const std::vector<int32_t>& windowIndices,
                                        const std::vector<int32_t>& windowWidths,
                                        const std::vector<int32_t>& windowHeights) const
{
    CaretAssert(windowIndices.size() == windowWidths.size());
    CaretAssert(windowWidths.size()  == windowHeights.size());
    
    DataFileContentInformation info;
    
    const int32_t numWindows(windowIndices.size());
    for (int32_t i = 0; i < numWindows; i++) {
        QString name(QString("Window %1 ").arg(windowIndices[i] + 1));
        info.addNameAndValue(name + "Width", windowWidths[i]);
        info.addNameAndValue(name + "Height", windowHeights[i]);
    }
    
    info.addNameAndValue("Dimensions", ImageCaptureDimensionsModeEnum::toGuiName(m_dimensionsMode));
    switch (m_dimensionsMode) {
        case ImageCaptureDimensionsModeEnum::IMAGE_CAPTURE_DIMENSIONS_MODE_CUSTOM:
        {
            info.addNameAndValue("Pixel Width", getPixelWidth());
            info.addNameAndValue("Pixel Height", getPixelHeight());
            
            const AString imageSpatialUnitsName(" " + ImageSpatialUnitsEnum::toGuiName(getSpatialUnits()));
            
            info.addNameAndValue("Image Width", AString::number(getSpatialWidth(), 'f', 2) + imageSpatialUnitsName);
            info.addNameAndValue("Image Height", AString::number(getSpatialHeight(), 'f', 2) + imageSpatialUnitsName);
            
            const AString resolutionUnitsName(" " + ImageResolutionUnitsEnum::toGuiName(getImageResolutionUnits()));
            info.addNameAndValue("Resolution", AString::number(getImageResolutionInSelectedUnits(), 'f', 2) + resolutionUnitsName);
        }
            break;
        case ImageCaptureDimensionsModeEnum::IMAGE_CAPTURE_DIMENSIONS_MODE_WINDOW_SIZE:
            break;
    }
    
    info.addNameAndValue("Crop to Tab/Window Lock Aspect Region", isCropToTabWindowLockAspectRegionEnabled());
    info.addNameAndValue("Crop Image with Margin", isCroppingEnabled());
    info.addNameAndValue("Image Margin", getMargin());
    
    return info.getInformationInString();
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString
ImageCaptureDialogSettings::toString() const
{
    std::vector<int32_t> emptyVector;
    return getSettingsAsText(emptyVector, emptyVector, emptyVector);
}

/**
 * @return The custom width in pixels.
 */
int32_t
ImageCaptureDialogSettings::getPixelWidth() const
{
    /*
     * Internally, pixels are float, so round (by adding 0.5)
     * and return as integer.
     */
    return static_cast<int32_t>(m_pixelWidth + 0.5);
}

/**
 * @return The custom height in pixels.
 */
int32_t
ImageCaptureDialogSettings::getPixelHeight() const
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
ImageCaptureDialogSettings::getSpatialWidth() const
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
ImageCaptureDialogSettings::getSpatialHeight() const
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
ImageCaptureDialogSettings::getImageResolutionInCentimeters() const
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
ImageCaptureDialogSettings::getImageResolutionInSelectedUnits() const
{
    float pixelsPerUnit = 1.0;
    
    switch (m_imageResolutionUnits) {
        case ImageResolutionUnitsEnum::PIXELS_PER_CENTIMETER:
            pixelsPerUnit = m_pixelsPerCentimeter;
            break;
        case ImageResolutionUnitsEnum::PIXELS_PER_INCH:
            pixelsPerUnit = m_pixelsPerCentimeter * CENTIMETERS_PER_INCH;
            break;
        case ImageResolutionUnitsEnum::PIXELS_PER_METER:
            pixelsPerUnit = m_pixelsPerCentimeter / CENTIMETERS_PER_METER;
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
ImageCaptureDialogSettings::setPixelWidthAndHeight(const int32_t pixelWidth,
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
ImageCaptureDialogSettings::setPixelWidth(const int32_t pixelWidth)
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
ImageCaptureDialogSettings::setPixelHeight(const int32_t pixelHeight)
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
ImageCaptureDialogSettings::setSpatialWidth(const float spatialWidth)
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
ImageCaptureDialogSettings::setSpatialHeight(const float spatialHeight)
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
ImageCaptureDialogSettings::setImageResolutionInSelectedUnits(const float imageResolutionInSelectedUnits)
{
    CaretAssert(imageResolutionInSelectedUnits > 0);
    
    switch (m_imageResolutionUnits) {
        case ImageResolutionUnitsEnum::PIXELS_PER_INCH:
            m_pixelsPerCentimeter = imageResolutionInSelectedUnits / CENTIMETERS_PER_INCH;
            break;
        case ImageResolutionUnitsEnum::PIXELS_PER_CENTIMETER:
            m_pixelsPerCentimeter = imageResolutionInSelectedUnits;
            break;
        case ImageResolutionUnitsEnum::PIXELS_PER_METER:
            m_pixelsPerCentimeter = imageResolutionInSelectedUnits / CENTIMETERS_PER_METER;
            break;
    }
    
    updatePixelWidthAndHeightFromSpatialWidthAndHeight();
}

/**
 * Update the pixel width and height after a change to the spatial width
 * and/or height.
 */
void
ImageCaptureDialogSettings::updatePixelWidthAndHeightFromSpatialWidthAndHeight()
{
    m_pixelWidth  = m_centimetersWidth  * m_pixelsPerCentimeter;
    m_pixelHeight = m_centimetersHeight * m_pixelsPerCentimeter;
}

/**
 * Update the spatial width and height after a change to the pixel width
 * and/or height.
 */
void
ImageCaptureDialogSettings::updateSpatialWidthAndHeightFromPixelWidthAndHeight()
{
    m_centimetersWidth  = m_pixelWidth  / m_pixelsPerCentimeter;
    m_centimetersHeight = m_pixelHeight / m_pixelsPerCentimeter;
}

/**
 * @return the aspect ration (height / width).
 */
float
ImageCaptureDialogSettings::getAspectRatio() const
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
ImageCaptureDialogSettings::updateForAspectRatio(const float width,
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
 * @return The  margin.
 */
int32_t
ImageCaptureDialogSettings::getMargin() const
{
    return m_margin;
}

/**
 * Set the  margin.
 * @param margin
 *     New  margin.
 */
void
ImageCaptureDialogSettings::setMargin(const int32_t margin)
{
    m_margin = margin;
}

/**
 * @return Is cropping enabled?
 */
bool
ImageCaptureDialogSettings::isCroppingEnabled() const
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
ImageCaptureDialogSettings::setCroppingEnabled(const bool enabled)
{
    m_croppingEnabled = enabled;
}

/**
 * @return Is copy to clipboard enabled?
 */
bool
ImageCaptureDialogSettings::isCopyToClipboardEnabled() const
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
ImageCaptureDialogSettings::setCopyToClipboardEnabled(const bool enabled)
{
    m_copyToClipboardEnabled = enabled;
}

/**
 * @return Is save to file enabled?
 */
bool
ImageCaptureDialogSettings::isSaveToFileEnabled() const
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
ImageCaptureDialogSettings::setSaveToFileEnabled(const bool enabled)
{
    m_saveToFileEnabled = enabled;
}

/**
 * @return Image file name
 */
AString
ImageCaptureDialogSettings::getImageFileName() const
{
    return m_imageFileName;
}

/**
 * @return Is scale proportionately enabled?
 */
bool
ImageCaptureDialogSettings::isScaleProportionately() const
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
ImageCaptureDialogSettings::setScaleProportionately(const bool enabled)
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
ImageCaptureDialogSettings::setImageFileName(const AString& filename)
{
    m_imageFileName = filename;
}

/**
 * @return Image dimensions mode
 */
ImageCaptureDimensionsModeEnum::Enum
ImageCaptureDialogSettings::getImageCaptureDimensionsMode() const
{
    return m_dimensionsMode;
}

/**
 * @return Image resolution units
 */
ImageResolutionUnitsEnum::Enum
ImageCaptureDialogSettings::getImageResolutionUnits() const
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
ImageCaptureDialogSettings::setImageResolutionUnits(const ImageResolutionUnitsEnum::Enum imageResolutionUnits)
{
    m_imageResolutionUnits = imageResolutionUnits;
    updateSpatialWidthAndHeightFromPixelWidthAndHeight();
}

/**
 * @return The spatial units.
 */
ImageSpatialUnitsEnum::Enum
ImageCaptureDialogSettings::getSpatialUnits() const
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
ImageCaptureDialogSettings::setSpatialUnits(const ImageSpatialUnitsEnum::Enum spatialUnits)
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
ImageCaptureDialogSettings::setImageCaptureDimensionsMode(const ImageCaptureDimensionsModeEnum::Enum mode)
{
    m_dimensionsMode = mode;
}

/**
 * @param Is crop to tab/window lock aspect ratio enabled?
 */
bool
ImageCaptureDialogSettings::isCropToTabWindowLockAspectRegionEnabled() const
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
ImageCaptureDialogSettings::setCropToTabWindowLockAspectRegionEnabled(const bool enabled)
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
ImageCaptureDialogSettings::saveToScene(const SceneAttributes* sceneAttributes,
                                 const AString& instanceName)
{
    SceneClass* sceneClass = new SceneClass(instanceName,
                                            "ImageCaptureDialogSettings",
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
ImageCaptureDialogSettings::restoreFromScene(const SceneAttributes* sceneAttributes,
                                      const SceneClass* sceneClass)
{
    if (sceneClass == NULL) {
        return;
    }
    
    /**
     * Try to restore old cropping value that used variable name "m_croppingMargin".
     * Name was changed to m_margin on 4/27/2021
     */
    const int32_t invalidCroppingMargin(-999999);
    const int32_t croppingMargin(sceneClass->getIntegerValue("m_croppingMargin",
                                                             invalidCroppingMargin));
    if (croppingMargin != invalidCroppingMargin) {
        /*
         * 4/27/2021
         * Older scene before m_croppingMargin was renamed to m_margin.
         * Only use scene value if cropping is enabled as at time scene was
         * created, margin was only used if cropping was enabled
         */
        if (isCroppingEnabled()) {
            m_margin = sceneClass->getIntegerValue("m_croppingMargin");
        }
        else {
            m_margin = 0;
        }
    }
    
    m_sceneAssistant->restoreMembers(sceneAttributes,
                                     sceneClass);    
    
    m_imageFileName = sceneClass->getPathNameValue("m_imageFileName",
                                                   "untitled.png");
    
    //Uncomment if sub-classes must restore from scene
    //restoreSubClassDataFromScene(sceneAttributes,
    //                             sceneClass);
    
}


