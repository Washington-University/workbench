
/*LICENSE_START*/
/*
 *  Copyright (C) 2020 Washington University School of Medicine
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

#define __MEDIA_FILE_DECLARE__
#include "MediaFile.h"
#undef __MEDIA_FILE_DECLARE__

#include "CaretAssert.h"
#include "CaretLogger.h"
#include "CziUtilities.h"
#include "DataFileContentInformation.h"
#include "GraphicsUtilitiesOpenGL.h"
#include "ImageFile.h"
#include "MediaFileChannelInfo.h"
#include "Plane.h"
#include "SceneClass.h"
#include "SceneClassAssistant.h"

using namespace caret;


    
/**
 * \class caret::MediaFile 
 * \brief Base class for media type files (image, movie)
 * \ingroup Files
 */

/**
 * Constructor.
 * @param dataFileType
 *    Type of data file
 */
MediaFile::MediaFile(const DataFileTypeEnum::Enum dataFileType)
: CaretDataFile(dataFileType)
{
    switch (dataFileType) {
        case DataFileTypeEnum::CZI_IMAGE_FILE:
        case DataFileTypeEnum::IMAGE:
            break;
        default:
        {
            const AString msg("Invalid data file type="
                               + DataFileTypeEnum::toName(dataFileType)
                               + ".  Has new file type been added?");
            CaretAssertMessage(0, msg);
            CaretLogSevere(msg);
        }
    }
    
    initializeMembersMediaFile();
    
}

/**
 * Copy constructor.
 * @param mediaFile
 *    Media file that is copied.
 */
MediaFile::MediaFile(const MediaFile& mediaFile)
: CaretDataFile(mediaFile)
{
    copyMediaFileMembers(mediaFile);
}


/**
 * Copy constructor.
 * @param mediaFile
 *    Media file that is copied.
 */
void
MediaFile::copyMediaFileMembers(const MediaFile& mediaFile)
{
    initializeMembersMediaFile();

    if (mediaFile.m_mediaFileChannelInfo) {
        m_mediaFileChannelInfo.reset(new MediaFileChannelInfo(*mediaFile.m_mediaFileChannelInfo.get()));
    }
    
    m_mediaFileTransforms = mediaFile.m_mediaFileTransforms;
    
    m_scaledToPlaneMatrix = mediaFile.m_scaledToPlaneMatrix;
    
    m_planeBoundingBox = mediaFile.m_planeBoundingBox;
    
    m_stereotaxicXyzBoundingBox = mediaFile.m_stereotaxicXyzBoundingBox;
    
    m_planeXyzRect = mediaFile.m_planeXyzRect;
    
    m_planeXyzBottomLeft = mediaFile.m_planeXyzBottomLeft;
    
    m_planeXyzBottomRight = mediaFile.m_planeXyzBottomRight;
    
    m_planeXyzTopRight = mediaFile.m_planeXyzTopRight;
    
    m_planeXyzTopLeft = mediaFile.m_planeXyzTopLeft;
    
    m_planeXyzValidFlag = mediaFile.m_planeXyzValidFlag;
    
    m_stereotaxicXyzBottomLeft = mediaFile.m_stereotaxicXyzBottomLeft;
    
    m_stereotaxicXyzBottomRight = mediaFile.m_stereotaxicXyzBottomRight;
    
    m_stereotaxicXyzTopRight = mediaFile.m_stereotaxicXyzTopRight;
    
    m_stereotaxicXyzTopLeft = mediaFile.m_stereotaxicXyzTopLeft;
    
    if (mediaFile.m_stereotaxicPlane) {
        m_stereotaxicPlane.reset(new Plane(*mediaFile.m_stereotaxicPlane));
    }
    m_stereotaxicPlaneInvalidFlag = mediaFile.m_stereotaxicPlaneInvalidFlag;
    
    if (mediaFile.m_planeCoordinatesPlane) {
        m_planeCoordinatesPlane.reset(new Plane(*mediaFile.m_planeCoordinatesPlane));
    }
    
    m_planeCoordinatesPlaneInvalidFlag = mediaFile.m_planeCoordinatesPlaneInvalidFlag;
    
    m_scaledToPlaneMatrixValidFlag     = mediaFile.m_scaledToPlaneMatrixValidFlag;
}

/**
 * Destructor.
 */
MediaFile::~MediaFile()
{
}

/**
 * Initialize members of media file
 */
void
MediaFile::initializeMembersMediaFile()
{
    resetMatricesPrivate();
    m_mediaFileChannelInfo.reset(new MediaFileChannelInfo());
    m_sceneAssistant = std::unique_ptr<SceneClassAssistant>(new SceneClassAssistant());
}

/**
 * @return This instance cloned as an ImageFile or NULL if cloning failed.
 * @param errorMessageOut
 *    Contains error information if failure.
 */
ImageFile*
MediaFile::cloneAsImageFile(AString& errorMessageOut) const
{
    const int32_t width(getWidth());
    const int32_t height(getHeight());
    
    const int32_t maxWH(GraphicsUtilitiesOpenGL::getTextureWidthHeightMaximumDimension());
    if ((width > maxWH)
        || (height > maxWH)) {
        errorMessageOut.appendWithNewLine("Media file is too large to clone as image.  "
                                          "Width=" + AString::number(width)
                                          + "Height=" + AString::number(height)
                                          + " cannot be greater than "
                                          + AString::number(maxWH));
        return NULL;
    }
    
    {
        /*
         * If this is an image file, simply copy it
         */
        const ImageFile* imageFile(castToImageFile());
        if (imageFile != NULL) {
            return new ImageFile(*imageFile);
        }
    }
    
    
    ImageFile* imageFile(new ImageFile(width,
                                       height));
    if (imageFile != NULL) {
        imageFile->copyMediaFileMembers(*this);
    }
    else {
        errorMessageOut = "Memory failure cloning as image file";
    }
    
    return imageFile;
}

/**
 * Clone the image with the maximum width and/or height.  If this media file has a width
 * or height greater than the maximumWidthHeight, the image is resized while keeping
 * the aspect ratio.
 * @param maximumWidthHeight
 *    Maximum width/height for image
 * @return This instance cloned as an ImageFile or NULL if cloning failed.
 * @param errorMessageOut
 *    Contains error information if failure.
 */
ImageFile*
MediaFile::cloneAsImageFileMaximumWidthHeight(const int32_t maximimumWidthHeight,
                                              AString& errorMessageOut) const
{
    const int32_t maxWH(GraphicsUtilitiesOpenGL::getTextureWidthHeightMaximumDimension());
    if (maximimumWidthHeight > maxWH) {
        errorMessageOut = ("Maximimum Width Height for cloning image must be less than OpenGL limit of "
                           + AString::number(maxWH));
        return NULL;
    }
    else if (maximimumWidthHeight <= 0) {
        errorMessageOut = ("Maximimum Width Height for cloning image must be positive, value= "
                           + AString::number(maximimumWidthHeight));
    }
    const int32_t width(getWidth());
    const int32_t height(getHeight());
    
    if (maximimumWidthHeight <= 0) {
        if ((width > maxWH)
            || (height > maxWH)) {
            errorMessageOut = ("Media file is too large to clone as image.  "
                               "Width=" + AString::number(width)
                               + "Height=" + AString::number(height)
                               + " cannot be greater than OpenGL limit of "
                               + AString::number(maxWH));
            return NULL;
        }
    }
    
    if (maximimumWidthHeight <= 0) {
        /*
         * If this is an image file, simply copy it
         */
        const ImageFile* imageFile(castToImageFile());
        if (imageFile != NULL) {
            return new ImageFile(*imageFile);
        }
    }
    
    const float aspectRatio(static_cast<float>(height) / static_cast<float>(width));
    int32_t newWidth(width);
    int32_t newHeight(height);
    if (maximimumWidthHeight > 0) {
        if (newWidth > newHeight) {
            if (newWidth > maximimumWidthHeight) {
                newWidth = maximimumWidthHeight;
                newHeight = newWidth * aspectRatio;
            }
        }
        else {
            if (newHeight > maximimumWidthHeight) {
                newHeight = maximimumWidthHeight;
                newWidth  = ((aspectRatio > 0)
                             ? (newHeight / aspectRatio)
                             : newHeight);
            }
        }
    }
    ImageFile* imageFile(new ImageFile(newWidth,
                                       newHeight));
    if (imageFile != NULL) {
        imageFile->copyMediaFileMembers(*this);
        if ((newWidth != width)
            || (newHeight != height)) {
            imageFile->m_mediaFileTransforms.adjustForNewMediaFileSize(width,
                                                                       height,
                                                                       newWidth,
                                                                       newHeight);
        }
    }
    else {
        errorMessageOut = "Memory failure cloning as image file";
    }
        
    
    return imageFile;

}

/**
 * @return Name of frame at given index.
 * @param frameIndex Index of the frame
 */
AString
MediaFile::getFrameName(const int32_t frameIndex) const
{
    CaretAssert((frameIndex >= 0) && (frameIndex < getNumberOfFrames()));
    const AString defaultFrameName(AString::number(frameIndex+1));
    return defaultFrameName;
}

/**
 * @return The units for the 'interval' between two consecutive frames.
 */
NiftiTimeUnitsEnum::Enum
MediaFile::getFrameIntervalUnits() const
{
    return NiftiTimeUnitsEnum::NIFTI_UNITS_UNKNOWN;
}

/**
 * Get the units value for the first frame and the
 * quantity of units between consecutive frames.  If the
 * units for the frame is unknown, value of one (1) are
 * returned for both output values.
 *
 * @param firstFrameUnitsValueOut
 *     Output containing units value for first frame.
 * @param frameIntervalStepValueOut
 *     Output containing number of units between consecutive frame.
 */
void
MediaFile::getFrameIntervalStartAndStep(float& firstFrameUnitsValueOut,
                                          float& frameIntervalStepValueOut) const
{
    firstFrameUnitsValueOut   = 1.0;
    frameIntervalStepValueOut = 1.0;
}

/**
 * @return Pointer to channel information
 */
const MediaFileChannelInfo*
MediaFile::getMediaFileChannelInfo() const
{
    if ( ! m_mediaFileChannelInfo) {
        m_mediaFileChannelInfo.reset(new MediaFileChannelInfo());
    }
    CaretAssert(m_mediaFileChannelInfo.get());
    return m_mediaFileChannelInfo.get();
}

/**
 * @return Pointer to channel information
 */
MediaFileChannelInfo*
MediaFile::getMediaFileChannelInfo()
{
    if ( ! m_mediaFileChannelInfo) {
        m_mediaFileChannelInfo.reset(new MediaFileChannelInfo());
    }
    CaretAssert(m_mediaFileChannelInfo.get());
    return m_mediaFileChannelInfo.get();
}

/**
 * Replace the media file channel information.  This method should be called by subclasses
 * that support channels.
 * @param mediaFileChannelInfo
 *    The new channel information.  This class will COPY the data.
 */
void
MediaFile::replaceMediaFileChannelInfo(const MediaFileChannelInfo& mediaFileChannelInfo)
{
    m_mediaFileChannelInfo.reset(new MediaFileChannelInfo(mediaFileChannelInfo));
}

/**
 * @return The structure for this file.
 */
StructureEnum::Enum
MediaFile::getStructure() const
{
    return StructureEnum::INVALID;
}

/**
 * Set the structure for this file.
 * @param structure
 *   New structure for this file.
 */
void
MediaFile::setStructure(const StructureEnum::Enum /*structure */)
{
    /* File does not support structures */
}


/**
 * Save subclass data to the scene.
 *
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    restoring the scene.
 *
 * @param sceneClass
 *     sceneClass to which data members should be added.  Will always
 *     be valid (non-NULL).
 */
void
MediaFile::saveFileDataToScene(const SceneAttributes* sceneAttributes,
                                            SceneClass* sceneClass)
{
    CaretDataFile::saveFileDataToScene(sceneAttributes,
                                       sceneClass);
    m_sceneAssistant->saveMembers(sceneAttributes,
                                  sceneClass);
}

/**
 * Restore file data from the scene.
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
MediaFile::restoreFileDataFromScene(const SceneAttributes* sceneAttributes,
                                                 const SceneClass* sceneClass)
{
    CaretDataFile::restoreFileDataFromScene(sceneAttributes,
                                            sceneClass);
    m_sceneAssistant->restoreMembers(sceneAttributes,
                                     sceneClass);
}

/**
 * @return File casted to an media file (avoids use of dynamic_cast that can be slow)
 * Overidden in MediaFile
 */
MediaFile*
MediaFile::castToMediaFile()
{
    return this;
}

/**
 * @return File casted to an media file (avoids use of dynamic_cast that can be slow)
 * Overidden in ImageFile
 */
const MediaFile*
MediaFile::castToMediaFile() const
{
    return this;
}

/**
 * @return A pixel index converted from a pixel logical index.
 * @param pixelLogicalIndex
 *    The logical pixel index.
 */
PixelIndex
MediaFile::pixelLogicalIndexToPixelIndex(const PixelLogicalIndex& pixelLogicalIndex) const
{
    return m_mediaFileTransforms.logicalPixelIndexToPixelIndex(pixelLogicalIndex);
}

/**
 * @return A pixel logical index converted from a pixel index.
 * @param pixelIndex
 *    The  pixel index.
 */
PixelLogicalIndex
MediaFile::pixelIndexToPixelLogicalIndex(const PixelIndex& pixelIndex) const
{
    return m_mediaFileTransforms.pixelIndexToLogicalPixelIndex(pixelIndex);
}

/**
 * @return The Rect defining the plane coordinates.
 * Bounds are always parallel to X and Y axes.  If there is rotation due to
 * 'pixel to plane' matrix, bounds encloses image data.
 */
QRectF
MediaFile::getPlaneXyzRect() const
{
    return m_planeXyzRect;
}

/**
 * convert a pixel index to a stereotaxic coordinate
 * @param pixelIndex
 *    Pixel index
 * @param xyzOut
 *    Output with the XYZ coordinate
 *    @param
 * @return
 *    True if conversion successful, else false.
 */
bool
MediaFile::pixelIndexToStereotaxicXYZ(const PixelIndex& pixelIndex,
                                        Vector3D& xyzOut) const
{
    return m_mediaFileTransforms.pixelIndexToStereotaxicXYZ(pixelIndex, xyzOut);
}

/**
 * Convert a stereotaxic xyz coordinate to a pixel index
 * @param xyz
 *    The coordinate
 * @param pixelIndexOut
 *    Output pixel index
 * @return
 *    True if successful, else false.
 */
bool
MediaFile::stereotaxicXyzToPixelIndex(const Vector3D& xyz,
                                        PixelIndex& pixelIndexOut) const
{
    return m_mediaFileTransforms.stereotaxicXyzToPixelIndex(xyz, pixelIndexOut);
}

/**
 * Convert a stereotaxic xyz coordinate to a pixel index
 * @param xyz
 *    The coordinate
 * @param pixelLogicalIndexOut
 *    Output logical pixel index
 * @return
 *    True if successful, else false.
 */
bool
MediaFile::stereotaxicXyzToLogicalPixelIndex(const Vector3D& xyz,
                                               PixelLogicalIndex& pixelLogicalIndexOut) const
{
    return m_mediaFileTransforms.stereotaxicXyzToLogicalPixelIndex(xyz, pixelLogicalIndexOut);
}

/**
 * Convert a pixel index to a plane XYZ.  If not supported, output is same as input.
 * @param pixelIndex
 *    Index of pixel
 * @param planeXyzOut
 *    Output with XYZ in plane
 * @return True if successful, else false.
 */
bool
MediaFile::pixelIndexToPlaneXYZ(const PixelIndex& pixelIndex,
                                  Vector3D& planeXyzOut) const
{
    return m_mediaFileTransforms.pixelIndexToPlaneXYZ(pixelIndex, planeXyzOut);
}

/**
 * Convert a pixel index to a plane XYZ.  If not supported, output is same as input.
 * @param pixelIndex
 *    Index of pixel
 * @param planeXyzOut
 *    Output with XYZ in plane
 * @return True if successful, else false.
 */
bool
MediaFile::logicalPixelIndexToPlaneXYZ(const PixelLogicalIndex& pixelLogicalIndex,
                                         Vector3D& planeXyzOut) const
{
    return m_mediaFileTransforms.logicalPixelIndexToPlaneXYZ(pixelLogicalIndex, planeXyzOut);
}

/**
 * Convert a pixel index to a plane XYZ.  If not supported, output is same as input.
 * @param logicalX
 *    logtical X Index of pixel
 * @param logicalY
 *    logtical Y Index of pixel
 * @param planeXyzOut
 *    Output with XYZ in plane
 * @return True if successful, else false.
 */
bool
MediaFile::logicalPixelIndexToPlaneXYZ(const float logicalX,
                                       const float logicalY,
                                       Vector3D& planeXyzOut) const
{
    return m_mediaFileTransforms.logicalPixelIndexToPlaneXYZ(logicalX, logicalY, planeXyzOut);
}

/**
 * Convert a pixel index to a plane XYZ.  If not supported, output is same as input.
 * @param pixelIndex
 *    Index of pixel
 * @param stereotaxicXyzOut
 *    Output with XYZ in stereotaxic
 * @return True if successful, else false.
 */
bool
MediaFile::logicalPixelIndexToStereotaxicXYZ(const PixelLogicalIndex& pixelLogicalIndex,
                                             Vector3D& stereotaxicXyzOut) const
{
    return m_mediaFileTransforms.logicalPixelIndexToStereotaxicXYZ(pixelLogicalIndex, stereotaxicXyzOut);
}

/**
 * Convert a pixel index to a stereotaxic  XYZ.  If not supported, output is same as input.
 * @param logicalX
 *    logtical X Index of pixel
 * @param logicalY
 *    logtical Y Index of pixel
 * @param stereotaxicXyzOut
 *    Output with XYZ in stereotaxic
 * @return True if successful, else false.
 */
bool
MediaFile::logicalPixelIndexToStereotaxicXYZ(const float logicalX,
                                             const float logicalY,
                                             Vector3D& stereotaxicXyzOut) const
{
    return m_mediaFileTransforms.logicalPixelIndexToStereotaxicXYZ(logicalX, logicalY, stereotaxicXyzOut);
}

/**
 * Convert a pixel XYZ to a pixel index.  If not supported, output is same as input.
 * @param planeXyz
 *     XYZ in plane
 * @param pixelIndexOut
 *    Index of pixel
 * @return True if successful, else false.
 */
bool
MediaFile::planeXyzToPixelIndex(const Vector3D& planeXyz,
                                  PixelIndex& pixelIndexOut) const
{
    return m_mediaFileTransforms.planeXyzToPixelIndex(planeXyz, pixelIndexOut);
}

/**
 * Convert a pixel XYZ to a logical pixel index.  If not supported, output is same as input.
 * @param planeXyz
 *     XYZ in plane
 * @param logicalPixelIndexOut
 *    Index of pixel
 * @return True if successful, else false.
 */
bool
MediaFile::planeXyzToLogicalPixelIndex(const Vector3D& planeXyz,
                                         PixelLogicalIndex& pixelLogicalIndexOut) const
{
    return m_mediaFileTransforms.planeXyzToLogicalPixelIndex(planeXyz, pixelLogicalIndexOut);
}

/**
 * Convert a plane XYZ to stereotaxic XYZ
 * @param planeXyz
 *     XYZ in plane
 * @param stereotaxicXyzOut
 *    Output with stereotaxic XYZ
 * @return True if successful, else false.
 */
bool
MediaFile::planeXyzToStereotaxicXyz(const Vector3D& planeXyz,
                                    Vector3D& stereotaxicXyzOut) const
{
    return m_mediaFileTransforms.planeXyzToStereotaxicXyz(planeXyz, stereotaxicXyzOut);
}

/**
 * Convert a plane XYZ to stereotaxic XYZ
 * @param planeXyz
 *     XYZ in plane
 * @param stereotaxicNoNonLinearXyzOut
 *    Output with stereotaxic XYZ but without non-linear offset
 * @param stereotaxicXyzOut
 *    Output with stereotaxic XYZ
 * @return True if successful, else false.
 */
bool
MediaFile::planeXyzToStereotaxicXyz(const Vector3D& planeXyz,
                                    Vector3D& stereotaxicNoNonLinearXyzOut,
                                    Vector3D& stereotaxicXyzOut) const
{
    return m_mediaFileTransforms.planeXyzToStereotaxicXyz(planeXyz,
                                                          stereotaxicNoNonLinearXyzOut,
                                                          stereotaxicXyzOut);
}

/**
 * Converrt a stereotaxic coordinate to a plane coordinate
 * @param stereotaxicXyz
 *    Input stereotaxic coordinate
 * @param planeXyzOut
 *    Output plane coordinate
 * @return True if successful, else false
 */
bool
MediaFile::stereotaxicXyzToPlaneXyz(const Vector3D& stereotaxicXyz,
                                    Vector3D& planeXyzOut) const
{
    return m_mediaFileTransforms.stereotaxicXyzToPlaneXyz(stereotaxicXyz, planeXyzOut);
}

/**
 * Converrt a stereotaxic coordinate to a plane coordinate
 * @param stereotaxicXyz
 *    Input stereotaxic coordinate
 * @param planeNoNonLinearXyzOut
 *    Output plane coordinate without non-linear warping
 * @param planeXyzOut
 *    Output plane coordinate
 * @return True if successful, else false
 */
bool
MediaFile::stereotaxicXyzToPlaneXyz(const Vector3D& stereotaxicXyz,
                                    Vector3D& planeNoNonLinearXyzOut,
                                    Vector3D& planeXyzOut) const
{
    return m_mediaFileTransforms.stereotaxicXyzToPlaneXyz(stereotaxicXyz,
                                                          planeNoNonLinearXyzOut,
                                                          planeXyzOut);
}

/**
 * Reset the matrices.
 */
void
MediaFile::resetMatricesPrivate()
{
    m_planeXyzRect = QRectF();
    m_planeBoundingBox.resetZeros();
    m_stereotaxicXyzBoundingBox.resetZeros();
    m_scaledToPlaneMatrix.identity();
    
    m_planeXyzTopLeft     = Vector3D();
    m_planeXyzTopRight    = Vector3D();
    m_planeXyzBottomLeft  = Vector3D();
    m_planeXyzBottomRight = Vector3D();
    m_planeXyzValidFlag   = false;
    
    m_stereotaxicXyzTopLeft = Vector3D();
    m_stereotaxicXyzTopRight = Vector3D();
    m_stereotaxicXyzBottomLeft = Vector3D();
    m_stereotaxicXyzBottomRight = Vector3D();

    m_scaledToPlaneMatrixValidFlag      = false;
    
    m_stereotaxicPlane.reset();
    m_stereotaxicPlaneInvalidFlag = false;
    
    m_planeCoordinatesPlane.reset();
    m_planeCoordinatesPlaneInvalidFlag = false;
}

/**
 * Reset the matrices.  This method is virtual so cannot call from constructor.
 */
void
MediaFile::resetMatrices()
{
    resetMatricesPrivate();
}

/**
 * @return True if scaled to plane matrix is valid
 */
bool
MediaFile::isScaledToPlaneMatrixValid() const
{
    return m_scaledToPlaneMatrixValidFlag;
}


/**
 * @return The scaled to plane matrix
 * converts normalized pixel (0 to 1) to plane coordinate
 * @param validFlagOut
 *    Pointer to boolean.  If not NULL, will be true if matrix is valid, else false.
 */
Matrix4x4
MediaFile::getScaledToPlaneMatrix(bool* validFlagOut) const
{
    if (validFlagOut != NULL) {
        *validFlagOut = m_scaledToPlaneMatrixValidFlag;
    }
    return m_scaledToPlaneMatrix;
}

/**
 * @return True if plane to millimeters matrix is valid
 */
bool
MediaFile::isPlaneToMillimetersMatrixValid() const
{
    return m_mediaFileTransforms.isPlaneToMillimetersMatrixValid();
}

/**
 * @return Matrix that converts from plane coordiantes to millimeters
 * @param validFlagOut
 *    Pointer to boolean.  If not NULL, will be true if matrix is valid, else false.
 */
Matrix4x4
MediaFile::getPlaneToMillimetersMatrix(bool* validFlagOut) const
{
    if (validFlagOut != NULL) {
        *validFlagOut = isPlaneToMillimetersMatrixValid();
    }
    return m_mediaFileTransforms.getPlaneToMillimetersMatrix();
}

/**
 * Set the matrices for display drawing.
 * @param scaledToPlaneMatrix
 *    The scaled to plane matrix.
 * @param scaledToPlaneMatrixValidFlag
 *    Validity of the scaled to plane matrix.
 * @param planeToMillimetersMatrix
 *    Matrix for converting from plane coords to millimeter coords
 * @param planeToMillimetersMatrixValidFlag
 *    Validity of the plane to millimeters matrix
 */
void
MediaFile::setTransformMatrices(const Matrix4x4& scaledToPlaneMatrix,
                                const bool scaledToPlaneMatrixValidFlag,
                                const Matrix4x4& planeToMillimetersMatrix,
                                const bool planeToMillimetersMatrixValidFlag,
                                std::shared_ptr<CziNonLinearTransform>& toStereotaxicNonLinearTransform,
                                std::shared_ptr<CziNonLinearTransform>& fromStereotaxicNonLinearTransform)
{
    resetMatrices();
    
    /**
     * If the scaled to plane matrix is invalid, then the plane to millimeters matrix is useless
     */
    if ( ! scaledToPlaneMatrixValidFlag) {
        return;
    }
    
    /*
     * Translate by 1/2 pixel (move to center of pixel)
     */
    Matrix4x4 shiftMat;
    shiftMat.identity();
    shiftMat.setTranslation(0.5,
                            0.5,
                            0.0);
    
    /*
     * Matrix to convert pixel range so zero to one
     */
    const float scaleFactor(std::max(getWidth(), getHeight()));
    CaretAssert(scaleFactor >= 1.0);
    Matrix4x4 scaleMat;
    scaleMat.identity();
    scaleMat.scale((1.0 / scaleFactor),
                   (1.0 / scaleFactor),
                   1.0);
    
    m_scaledToPlaneMatrix = scaledToPlaneMatrix;
    CaretAssert(scaledToPlaneMatrixValidFlag);
    m_scaledToPlaneMatrixValidFlag = scaledToPlaneMatrixValidFlag;
    
    Matrix4x4 indexToPlane = scaledToPlaneMatrix;
    indexToPlane.premultiply(scaleMat);
    indexToPlane.premultiply(shiftMat);
    
    
    const bool testFlag(false);
    if (testFlag) {
        std::cout << "---- File: " << getFileName() << std::endl;
        std::cout << "Shift Mat: " << shiftMat.toString() << std::endl;
        std::cout << "Scale Mat: " << scaleMat.toString() << std::endl;
        std::cout << "ScaledToPlane: " << scaledToPlaneMatrix.toString() << std::endl;
        std::cout << "PlaneToMM" << planeToMillimetersMatrix.toString() << std::endl;
        std::cout << "Index to Plane: " << indexToPlane.toString() << std::endl;
        std::cout << "Start Index to Plane ----------" << std::endl;
        indexToPlaneTest(indexToPlane, planeToMillimetersMatrix, 0, 0, "top left");
        indexToPlaneTest(indexToPlane, planeToMillimetersMatrix, getWidth() - 1, 0, "top right");
        indexToPlaneTest(indexToPlane, planeToMillimetersMatrix, 0, getHeight() - 1, "bottom left");
        indexToPlaneTest(indexToPlane, planeToMillimetersMatrix, getWidth() - 1, getHeight() - 1, "bottom right");

        const float w(getWidth() - 1);
        const float h(getHeight() - 1);
        lengthsTest(indexToPlane, 0, 0, w, 0, "top");
        lengthsTest(indexToPlane, 0, h, w, h, "bottom");
        lengthsTest(indexToPlane, 0, 0, 0, h, "left");
        lengthsTest(indexToPlane, 0, w, w, h, "right");

        std::cout << "   -- Separate ----------" << std::endl;
        indexToPlaneTest(scaledToPlaneMatrix, shiftMat, scaleMat, planeToMillimetersMatrix, 0, 0, "top left");
        indexToPlaneTest(scaledToPlaneMatrix, shiftMat, scaleMat, planeToMillimetersMatrix, getWidth() - 1, 0, "top right");
        indexToPlaneTest(scaledToPlaneMatrix, shiftMat, scaleMat, planeToMillimetersMatrix, 0, getHeight() - 1, "bottom left");
        indexToPlaneTest(scaledToPlaneMatrix, shiftMat, scaleMat, planeToMillimetersMatrix, getWidth() - 1, getHeight() - 1, "bottom right");
        std::cout << "   -------------------------------" << std::endl;
    }
    
    Matrix4x4 pixelIndexToPlaneMatrix = indexToPlane;
    
    MediaFileTransforms::Inputs inputs(getLogicalBoundsRect(),
                                       pixelIndexToPlaneMatrix,
                                       true, /* above matrix valid */
                                       planeToMillimetersMatrix,
                                       planeToMillimetersMatrixValidFlag,
                                       toStereotaxicNonLinearTransform,
                                       fromStereotaxicNonLinearTransform);
    setMediaFileTransforms(inputs);
    
        const QRectF logicalRect(getLogicalBoundsRect());
        const PixelLogicalIndex topLeft(logicalRect.left(),
                                        logicalRect.top(),
                                        0);
        const PixelLogicalIndex topRight(logicalRect.right(),
                                         logicalRect.top(),
                                         0);
        const PixelLogicalIndex bottomLeft(logicalRect.left(),
                                           logicalRect.bottom(),
                                           0);
        const PixelLogicalIndex bottomRight(logicalRect.right(),
                                            logicalRect.bottom(),
                                            0);
        
        logicalPixelIndexToPlaneXYZ(topLeft,     m_planeXyzTopLeft);
        logicalPixelIndexToPlaneXYZ(topRight,    m_planeXyzTopRight);
        logicalPixelIndexToPlaneXYZ(bottomLeft,  m_planeXyzBottomLeft);
        logicalPixelIndexToPlaneXYZ(bottomRight, m_planeXyzBottomRight);
        m_planeXyzValidFlag = true;
        
        m_planeBoundingBox.resetForUpdate();
        m_planeBoundingBox.update(m_planeXyzTopLeft);
        m_planeBoundingBox.update(m_planeXyzTopRight);
        m_planeBoundingBox.update(m_planeXyzBottomLeft);
        m_planeBoundingBox.update(m_planeXyzBottomRight);

        m_planeXyzRect.setLeft(m_planeBoundingBox.getMinX());
        m_planeXyzRect.setRight(m_planeBoundingBox.getMaxX());
        m_planeXyzRect.setTop(m_planeBoundingBox.getMinY());
        m_planeXyzRect.setBottom(m_planeBoundingBox.getMaxY());
        
        planeXyzToStereotaxicXyz(m_planeXyzTopLeft, m_stereotaxicXyzTopLeft);
        planeXyzToStereotaxicXyz(m_planeXyzTopRight, m_stereotaxicXyzTopRight);
        planeXyzToStereotaxicXyz(m_planeXyzBottomLeft, m_stereotaxicXyzBottomLeft);
        planeXyzToStereotaxicXyz(m_planeXyzBottomRight, m_stereotaxicXyzBottomRight);
        
        m_stereotaxicXyzBoundingBox.resetForUpdate();
        m_stereotaxicXyzBoundingBox.update(m_stereotaxicXyzTopLeft);
        m_stereotaxicXyzBoundingBox.update(m_stereotaxicXyzTopRight);
        m_stereotaxicXyzBoundingBox.update(m_stereotaxicXyzBottomLeft);
        m_stereotaxicXyzBoundingBox.update(m_stereotaxicXyzBottomRight);
        
        if (m_mediaFileTransforms.isPlaneToMillimetersMatrixValid()) {
            /*
             * Create the plane from XYZ coordinates
             */
            m_stereotaxicPlane.reset(new Plane(m_stereotaxicXyzTopLeft,
                                               m_stereotaxicXyzBottomLeft,
                                               m_stereotaxicXyzBottomRight));
            if ( ! m_stereotaxicPlane->isValidPlane()) {
                /*
                 * Plane invalid
                 */
                m_stereotaxicPlane.reset();
                m_stereotaxicPlaneInvalidFlag = true;
                CaretLogSevere(getFileNameNoPath()
                               + "Failed to create stereotaxic coordinates plane, computation of plane failed.");
            }
        }
        else {
            CaretLogSevere(getFileNameNoPath()
                           + "Failed to create stereotaxic coordinates plane, pixel to coordinate transform failed.");
            m_stereotaxicPlaneInvalidFlag = true;
        }
        
        const bool testFlag2(false);
        if (testFlag2) {
            std::cout << "---- File: " << getFileName() << std::endl;
            indexToPlaneTest(indexToPlane, planeToMillimetersMatrix, 0, 0, "top left");
            indexToPlaneTest(indexToPlane, planeToMillimetersMatrix, getWidth() - 1, 0, "top right");
            indexToPlaneTest(indexToPlane, planeToMillimetersMatrix, 0, getHeight() - 1, "bottom left");
            indexToPlaneTest(indexToPlane, planeToMillimetersMatrix, getWidth() - 1, getHeight() - 1, "bottom right");
            std::cout << "Plane bounding box: " << m_planeBoundingBox.toString() << std::endl;
            std::cout << "Plane XYZ Rect: " << CziUtilities::qRectToString(m_planeXyzRect) << std::endl;
        }

    if (testFlag) {
        std::cout << "-------------------------------" << std::endl;
    }
}

void
MediaFile::indexToPlaneTest(const Matrix4x4& indexToPlane,
                            const Matrix4x4& planeToMM,
                            const int32_t i,
                            const int32_t j,
                            const AString& name)
{
    Vector3D ij(j,
                i,
                1.0f);
    
    Vector3D xy(ij);
    indexToPlane.multiplyPoint3(xy);
    float y = xy[0];  /* yes correct Y is first */
    float x = xy[1];
    std::cout << name << " plane i, j, k: " << i << ", " << j << " xyz: " << x << ", " << y << ", " << xy[2] << std::endl;
    
    planeToMM.multiplyPoint3(xy);
    y = xy[0];
    x = xy[1];
    std::cout << "         mm x, y, z: " << i << ", " << j << " xyz: " << x << ", " << y << ", " << xy[2] << std::endl;
}

void
MediaFile::lengthsTest(const Matrix4x4& indexToPlane,
                       const int32_t i1,
                       const int32_t j1,
                       const int32_t i2,
                       const int32_t j2,
                       const AString& name)
{
    const Vector3D v1Pixel(i1, j1, 0.0);
    const Vector3D v2Pixel(i2, j2, 0.0);
    Vector3D v1(v1Pixel);
    Vector3D v2(v2Pixel);
    indexToPlane.multiplyPoint3(v1);
    indexToPlane.multiplyPoint3(v2);
    const Vector3D dxy(v2 - v1);
    std::cout << name << " Index (" << AString::fromNumbers(v1Pixel) << ") to (" << AString::fromNumbers(v2Pixel) << ") "
    << " Plane XYZ ( " << AString::fromNumbers(v1) << ") to (" << AString::fromNumbers(v2) << ") length: "
    << dxy.length() << std::endl;
}

void
MediaFile::indexToPlaneTest(const Matrix4x4& scaledToPlane,
                            const Matrix4x4& shiftMat,
                            const Matrix4x4& scaleMat,
                            const Matrix4x4& planeToMM,
                            const int32_t i,
                            const int32_t j,
                            const AString& name)
{
    const Vector3D ij(j,
                      i,
                      1.0f);
    Vector3D xy(j,
                i,
                1.0f);
    
    std::cout << "Input (i, j): " << i << ", " << j << " " << name << std::endl;
    shiftMat.multiplyPoint3(xy);
    std::cout << "   After Shift (x, y, z): " << xy[1] << ", " << xy[0] << ", " << xy[2] <<std::endl;
    scaleMat.multiplyPoint3(xy);
    std::cout << "   After scale (x, y, z): " << xy[1] << ", " << xy[0] << ", " << xy[2] <<std::endl;
    scaledToPlane.multiplyPoint3(xy);
    std::cout << "   Result (x, y, z): " << xy[1] << ", " << xy[0] << ", " << xy[2] <<std::endl;
    
    planeToMM.multiplyPoint3(xy);
    std::cout << "   Result Spatial (x, y, z): " << xy[1] << "mm, " << xy[0] << ", " << xy[2] << "mm" << std::endl;
}

/**
 * Add information about plane coordinates to the data file information
 */
void
MediaFile::addPlaneCoordsToDataFileContentInformation(DataFileContentInformation& dataFileInformation)
{
    if (m_planeXyzValidFlag) {
        dataFileInformation.addNameAndValue("Plane Top Left",     m_planeXyzTopLeft);
        dataFileInformation.addNameAndValue("Plane Bottom Left",  m_planeXyzBottomLeft);
        dataFileInformation.addNameAndValue("Plane Bottom Right", m_planeXyzBottomRight);
        dataFileInformation.addNameAndValue("Plane Top Right",    m_planeXyzTopRight);
        
        dataFileInformation.addNameAndValue("Stereotaxic Top Left",     m_stereotaxicXyzTopLeft);
        dataFileInformation.addNameAndValue("Stereotaxic Bottom Left",  m_stereotaxicXyzBottomLeft);
        dataFileInformation.addNameAndValue("Stereotaxic Bottom Right", m_stereotaxicXyzBottomRight);
        dataFileInformation.addNameAndValue("Stereotaxic Top Right",    m_stereotaxicXyzTopRight);

        Vector3D stl, str, sbl, sbr;
        Vector3D stlNN, strNN, sblNN, sbrNN;
        planeXyzToStereotaxicXyz(m_planeXyzTopLeft, stlNN, stl);
        planeXyzToStereotaxicXyz(m_planeXyzBottomLeft, sblNN, sbl);
        planeXyzToStereotaxicXyz(m_planeXyzBottomRight, sbrNN, sbr);
        planeXyzToStereotaxicXyz(m_planeXyzTopRight, strNN, str);
        
        const float tol(0.001);
        const bool tlError((m_stereotaxicXyzTopLeft - stl).length() > tol);
        const bool blError((m_stereotaxicXyzBottomLeft - sbl).length() > tol);
        const bool brError((m_stereotaxicXyzBottomRight - sbr).length() > tol);
        const bool trError((m_stereotaxicXyzTopRight - str).length() > tol);
        if (tlError || blError || brError || trError) {
            dataFileInformation.addNameAndValue("", "Stereotaxic Coords do not match pre-calculated stereotaxic coords above");
            dataFileInformation.addNameAndValue("Stereotaxic Top Left",     stl);
            dataFileInformation.addNameAndValue("Stereotaxic Bottom Left",  sbl);
            dataFileInformation.addNameAndValue("Stereotaxic Bottom Right", sbr);
            dataFileInformation.addNameAndValue("Stereotaxic Top Right",    str);
        }

        dataFileInformation.addNameAndValue("Stereotaxic Top Left (No-Non-Linear)",     stlNN);
        dataFileInformation.addNameAndValue("Stereotaxic Bottom Left (No-Non-Linear)",  sblNN);
        dataFileInformation.addNameAndValue("Stereotaxic Bottom Right (No-Non-Linear)", sbrNN);
        dataFileInformation.addNameAndValue("Stereotaxic Top Right (No-Non-Linear)",    strNN);
    }
    else {
        dataFileInformation.addNameAndValue("Pixel to Plane Matrix", "Invalid");
    }
}

/**
 * @return Return a rectangle that defines the bounds of the media data
 */
QRectF
MediaFile::getLogicalBoundsRect() const
{
    QRectF rect(0, 0, getWidth(), getHeight());
    return rect;
}

/**
 * @return Plane computed from stereotaxic coordinates (NULL if not valid)
 */
const Plane*
MediaFile::getStereotaxicImagePlane() const
{
    /*
     * Has plane already been created?
     */
    if (m_stereotaxicPlane) {
        /*
         * Plane was previously computed
         */
        return m_stereotaxicPlane.get();
    }
    
    return NULL;
}

/**
 * @return Plane computed from plane coordinates (NULL if not valid)
 */
const Plane*
MediaFile::getPlaneCoordinatesPlane() const
{
    /*
     * Has plane already been created?
     */
    if (m_planeCoordinatesPlane) {
        /*
         * Plane was previously computed
         */
        return m_planeCoordinatesPlane.get();
    }
    
    if (m_planeCoordinatesPlaneInvalidFlag) {
        /*
         * Tried to create plane previously but failed
         */
        return NULL;
    }
    
    /*
     * Create the plane from XYZ coordinates
     */
    m_planeCoordinatesPlane.reset(new Plane(m_planeXyzTopLeft,
                                            m_planeXyzBottomLeft,
                                            m_planeXyzBottomRight));
    if (m_planeCoordinatesPlane->isValidPlane()) {
        return m_planeCoordinatesPlane.get();
    }
    else {
        /*
         * Plane invalid
         */
        m_planeCoordinatesPlane.reset();
        m_planeCoordinatesPlaneInvalidFlag = true;
    }

    return NULL;
}

/**
 * Find the Pixel nearest the given XYZ coordinate
 * @param xyz
 *    The coordinate
 * @param signedDistanceToPixelMillimetersOut
 *    Output with signed distance to the pixel in millimeters
 * @param pixelLogicalIndexOut
 *    Output with logical pixel index
 * @return
 *    True if successful, else false.
 */
bool
MediaFile::findPixelNearestStereotaxicXYZ(const Vector3D& xyz,
                                          float& signedDistanceToPixelMillimetersOut,
                                          PixelLogicalIndex& pixelLogicalIndexOut) const
{
    Vector3D planeXYZ;
    if (findPlaneCoordinateNearestStereotaxicXYZ(xyz,
                                                 signedDistanceToPixelMillimetersOut,
                                                 planeXYZ)) {
        PixelIndex pixelIndex;
        if (planeXyzToPixelIndex(planeXYZ,
                                 pixelIndex)) {
            pixelLogicalIndexOut = pixelIndexToPixelLogicalIndex(pixelIndex);
            return true;
        }
    }
    return false;
}

/**
 * Find the plane coordinate nearest the given XYZ coordinate
 * @param stereotaxicXYZ
 *    The stereotaxic coordinate
 * @param signedDistanceToPixelMillimetersOut
 *    Output with signed distance in millimeters from stereotaxic coordinate to stereotaxic plane
 * @param planeXyzOut
 *    Output with plane coordinate
 * @return
 *    True if successful, else false.
 */
bool
MediaFile::findPlaneCoordinateNearestStereotaxicXYZ(const Vector3D& stereotaxicXYZ,
                                                      float& signedDistanceToPlaneMillimetersOut,
                                                      Vector3D& planeXyzOut) const
{
    const Plane* planeCoordinatesPlane(getPlaneCoordinatesPlane());
    if (planeCoordinatesPlane == NULL) {
        return false;
    }
    
    const Plane* stereotaxicCoordinatesPlane(getStereotaxicImagePlane());
    if (stereotaxicCoordinatesPlane == NULL) {
        return false;
    }
    
    /*
     * Convert stereotaxic coordinate to a plane coordinate
     */
    Vector3D planeXyz;
    if (stereotaxicXyzToPlaneXyz(stereotaxicXYZ,
                                 planeXyz)) {
        
        /*
         * Project the plane coordinate so that it is on the
         * plane.  This is our output coordinate.
         */
        planeCoordinatesPlane->projectPointToPlane(planeXyz,
                                                   planeXyzOut);

        /*
         * Find the distance of the stereotaxic coordinate
         * to the sterotaxic plane.  This value is millimeters.
         */
        signedDistanceToPlaneMillimetersOut = stereotaxicCoordinatesPlane->signedDistanceToPlane(stereotaxicXYZ);
            
        return true;
    }
    
    return false;
}

/**
 * @return True if this media supports coordinates
 */
bool
MediaFile::isPlaneXyzSupported() const
{
    return m_planeXyzValidFlag;
}

/**
 * @return Coordinate at bottom left of media
 */
Vector3D
MediaFile::getPlaneXyzBottomLeft() const
{
    return m_planeXyzBottomLeft;
}

/**
 * @return Coordinate at bottom left of media
 */
Vector3D
MediaFile::getPlaneXyzBottomRight() const
{
    return m_planeXyzBottomRight;
}

/**
 * @return Coordinate at bottom left of media
 */
Vector3D
MediaFile::getPlaneXyzTopRight() const
{
    return m_planeXyzTopRight;
}

/**
 * @return Coordinate at bottom left of media
 */
Vector3D
MediaFile::getPlaneXyzTopLeft() const
{
    return m_planeXyzTopLeft;
}

/**
 * @return Bounding box of plane coordinates
 */
BoundingBox
MediaFile::getPlaneXyzBoundingBox() const
{
    return m_planeBoundingBox;
}

/**
 * @return Bounding box of stereotaxic coordinates
 */
BoundingBox
MediaFile::getStereotaxicXyzBoundingBox() const
{
    return m_stereotaxicXyzBoundingBox;
}

/**
 * @return Stereotaxic Coordinate at bottom left of media
 */
Vector3D
MediaFile::getStereotaxicXyzBottomLeft() const
{
    return m_stereotaxicXyzBottomLeft;
}

/**
 * @return Stereotaxic Coordinate at bottom left of media
 */
Vector3D
MediaFile::getStereotaxicXyzBottomRight() const
{
    return m_stereotaxicXyzBottomRight;
}

/**
 * @return Stereotaxic Coordinate at bottom left of media
 */
Vector3D
MediaFile::getStereotaxicXyzTopRight() const
{
    return m_stereotaxicXyzTopRight;
}

/**
 * @return Stereotaxic Coordinate at bottom left of media
 */
Vector3D
MediaFile::getStereotaxicXyzTopLeft() const
{
    return m_stereotaxicXyzTopLeft;
}

/**
 * Get the identification text for the pixel at the given pixel index with origin at bottom left.
 * @param tabIndex
 *    Index of the tab in which identification took place
 * @param frameIndices
 *    Indics of the frames
 * @param planeCoordinate
 *    The plane coordinate
 * @param histologyIdFlag
 *    True if identification from histology file
 * @param columnOneTextOut
 *    Text for column one that is displayed to user.
 * @param columnTwoTextOut
 *    Text for column two that is displayed to user.
 * @param toolTipTextOut
 *    Text for tooltip
 */
void
MediaFile::getPixelPlaneIdentificationTextForFrames(const int32_t tabIndex,
                                                    const std::vector<int32_t>& frameIndices,
                                                    const Vector3D& planeCoordinate,
                                                    const bool histologyIdFlag,
                                                    std::vector<AString>& columnOneTextOut,
                                                    std::vector<AString>& columnTwoTextOut,
                                                    std::vector<AString>& toolTipTextOut) const
{
    PixelLogicalIndex pixelLogicalIndex;
    if ( ! planeXyzToLogicalPixelIndex(planeCoordinate,
                                       pixelLogicalIndex)) {
        return;
    }
    std::vector<int32_t> validFrameIndices;
    for (int32_t frameIndex : frameIndices) {
        if (isPixelIndexInFrameValid(frameIndex,
                                     pixelLogicalIndex)) {
            validFrameIndices.push_back(frameIndex);
        }
    }
    if (validFrameIndices.empty()) {
        return;
    }
    
    
    std::vector<AString> leftRgbaText;
    std::vector<AString> rightRgbaText;
    for (int32_t frameIndex : validFrameIndices) {
        uint8_t rgba[4];
        const bool rgbaValidFlag = getPixelRGBA(tabIndex,
                                                frameIndex,
                                                pixelLogicalIndex,
                                                rgba);
        if (rgbaValidFlag) {
            leftRgbaText.push_back("Frame "
                                   + AString::number(frameIndex + 1));
            rightRgbaText.push_back("RGBA ("
                                    + (rgbaValidFlag
                                       ? AString::fromNumbers(rgba, 4, ",")
                                       : "Invalid")
                                    + ")");
        }
    }
    CaretAssert(leftRgbaText.size() == rightRgbaText.size());
    const int32_t numRgbaText(static_cast<int32_t>(leftRgbaText.size()));
    
    const PixelIndex pixelIndex(pixelLogicalIndexToPixelIndex(pixelLogicalIndex));
    const int64_t fullResPixelI(pixelIndex.getI());
    const int64_t fullResPixelJ(pixelIndex.getJ());
    const AString pixelText("Pixel IJ ("
                            + AString::number(fullResPixelI)
                            + ","
                            + AString::number(fullResPixelJ)
                            + ")");
    
    const AString logicalText("Logical IJ ("
                              + AString::number(pixelLogicalIndex.getI(), 'f', 3)
                              + ","
                              + AString::number(pixelLogicalIndex.getJ(), 'f', 3)
                              + ")");
    
    const AString planeText("Plane XYZ ("
                            + AString::fromNumbers(planeCoordinate)
                            + ")");
    
    Vector3D stereoXYZ;
    AString mmText;
    if (planeXyzToStereotaxicXyz(planeCoordinate, stereoXYZ)) {
        mmText = ("Stereotaxicz XYZ ("
                  + AString::fromNumbers(stereoXYZ)
                  + ")");
    }

    columnOneTextOut.push_back("Filename");
    columnTwoTextOut.push_back(getFileNameNoPath());
    
    columnOneTextOut.push_back(pixelText);
    columnTwoTextOut.push_back(logicalText);
    
    if ( ! histologyIdFlag) {
        columnOneTextOut.push_back(mmText);
        columnTwoTextOut.push_back(planeText);
    }
    
    Vector3D xyz;
    if (logicalPixelIndexToStereotaxicXYZ(pixelLogicalIndex, xyz)) {
        columnOneTextOut.push_back("Stereotaxic XYZ");
        columnTwoTextOut.push_back(AString::fromNumbers(xyz, 3, ", "));
    }
    
    for (int32_t i = 0; i < numRgbaText; i++) {
        CaretAssertVectorIndex(leftRgbaText, i);
        CaretAssertVectorIndex(rightRgbaText, i);
        toolTipTextOut.push_back(leftRgbaText[i]
                                 + ": "
                                 + rightRgbaText[i]);
        columnOneTextOut.push_back(leftRgbaText[i]);
        columnTwoTextOut.push_back(rightRgbaText[i]);
    }
    toolTipTextOut.push_back(pixelText);
    toolTipTextOut.push_back(logicalText);
    toolTipTextOut.push_back(mmText);
    
    CaretAssert(columnOneTextOut.size() == columnTwoTextOut.size());
}

/**
 * Get the identification text for the pixel at the given pixel index with origin at bottom left.
 * @param tabIndex
 *    Index of the tab in which identification took place
 * @param frameIndices
 *    Indics of the frames
 * @param planeCoordinate
 *    The plane coordinate
 * @param histologyIdFlag
 *    True if identification from histology file
 * @param columnOneTextOut
 *    Text for column one that is displayed to user.
 * @param columnTwoTextOut
 *    Text for column two that is displayed to user.
 * @param toolTipTextOut
 *    Text for tooltip
 */
void
MediaFile::getPixelPlaneIdentificationTextForHistology(const int32_t tabIndex,
                                                    const std::vector<int32_t>& frameIndices,
                                                    const Vector3D& planeCoordinate,
                                                    std::vector<AString>& columnOneTextOut,
                                                    std::vector<AString>& columnTwoTextOut,
                                                    std::vector<AString>& toolTipTextOut) const
{
    PixelLogicalIndex pixelLogicalIndex;
    if ( ! planeXyzToLogicalPixelIndex(planeCoordinate,
                                       pixelLogicalIndex)) {
        return;
    }
    std::vector<int32_t> validFrameIndices;
    for (int32_t frameIndex : frameIndices) {
        if (isPixelIndexInFrameValid(frameIndex,
                                     pixelLogicalIndex)) {
            validFrameIndices.push_back(frameIndex);
        }
    }
    if (validFrameIndices.empty()) {
        return;
    }
    
    
    std::vector<AString> leftRgbaText;
    std::vector<AString> rightRgbaText;
    for (int32_t frameIndex : validFrameIndices) {
        uint8_t rgba[4];
        const bool rgbaValidFlag = getPixelRGBA(tabIndex,
                                                frameIndex,
                                                pixelLogicalIndex,
                                                rgba);
        if (rgbaValidFlag) {
            leftRgbaText.push_back("Frame "
                                   + AString::number(frameIndex + 1));
            rightRgbaText.push_back("RGBA ("
                                    + (rgbaValidFlag
                                       ? AString::fromNumbers(rgba, 4, ",")
                                       : "Invalid")
                                    + ")");
        }
    }
    CaretAssert(leftRgbaText.size() == rightRgbaText.size());
    const int32_t numRgbaText(static_cast<int32_t>(leftRgbaText.size()));
    
    const PixelIndex pixelIndex(pixelLogicalIndexToPixelIndex(pixelLogicalIndex));
    const int64_t fullResPixelI(pixelIndex.getI());
    const int64_t fullResPixelJ(pixelIndex.getJ());
    const AString pixelText("Pixel IJ ("
                            + AString::number(fullResPixelI)
                            + ","
                            + AString::number(fullResPixelJ)
                            + ")");
    
    const AString logicalText("Logical IJ ("
                              + AString::number(pixelLogicalIndex.getI(), 'f', 3)
                              + ","
                              + AString::number(pixelLogicalIndex.getJ(), 'f', 3)
                              + ")");
    
    columnOneTextOut.push_back("Filename");
    columnTwoTextOut.push_back(getFileNameNoPath()
                               + " ("
                               + getFilePath()
                               + ")");
    
    columnOneTextOut.push_back(pixelText);
    columnTwoTextOut.push_back(logicalText);
    
    
    for (int32_t i = 0; i < numRgbaText; i++) {
        CaretAssertVectorIndex(leftRgbaText, i);
        CaretAssertVectorIndex(rightRgbaText, i);
        toolTipTextOut.push_back(leftRgbaText[i]
                                 + ": "
                                 + rightRgbaText[i]);
        columnOneTextOut.push_back(leftRgbaText[i]);
        columnTwoTextOut.push_back(rightRgbaText[i]);
    }
    toolTipTextOut.push_back(pixelText);
    
    CaretAssert(columnOneTextOut.size() == columnTwoTextOut.size());
}

/**
 * Converrt a plane rectangle to a logical rectangle
 * @param planeRect
 *    The plane rectangle
 * @return
 *    The logical rectangle
 */
QRectF
MediaFile::planeRectToLogicalRect(const QRectF& planeRect) const
{
    QRectF rect;
    
    const Vector3D topLeftCoord(planeRect.left(),
                                planeRect.top(),
                                0.0);
    const Vector3D bottomRightCoord(planeRect.right(),
                                    planeRect.bottom(),
                                    0.0);
    
    PixelLogicalIndex topLeftPixel;
    PixelLogicalIndex bottomRightPixel;
    if (planeXyzToLogicalPixelIndex(topLeftCoord, topLeftPixel)
        && planeXyzToLogicalPixelIndex(bottomRightCoord, bottomRightPixel)) {
        rect.setCoords(topLeftPixel.getI(), topLeftPixel.getJ(),
                       bottomRightPixel.getI(), bottomRightPixel.getJ());
    }
    return rect;
}

/**
 * Convert a logical rect to a plane rect
 * @param logicalRect
 *    The logical rectangle
 * @return
 *    The plane rectangle
 */
QRectF
MediaFile::logicalRectToPlaneRect(const QRectF& logicalRect) const
{
    QRectF rect;
    
    const PixelLogicalIndex topLeftPixel(logicalRect.left(),
                                         logicalRect.top(),
                                         0.0f);
    const PixelLogicalIndex bottomRightPixel(logicalRect.right(),
                                             logicalRect.bottom(),
                                             0.0f);
    Vector3D topLeftCoord;
    Vector3D bottomRightCoord;
    if (logicalPixelIndexToPlaneXYZ(topLeftPixel, topLeftCoord)
        && logicalPixelIndexToPlaneXYZ(bottomRightPixel, bottomRightCoord)) {
        rect.setCoords(topLeftCoord[0], topLeftCoord[1],
                       bottomRightCoord[0], bottomRightCoord[1]);
    }
    return rect;
}

/**
 * Converrt a stereotaxic rectangle to a logical rectangle
 * @param stereotaxicRect
 *    The stereotaxic rectangle
 * @return
 *    The logical rectangle
 */
QRectF
MediaFile::stereotaxicRectToLogicalRect(const QRectF& stereotaxicRect) const
{
    QRectF rect;
    
    const Vector3D topLeftCoord(stereotaxicRect.left(),
                                stereotaxicRect.top(),
                                0.0);
    const Vector3D bottomRightCoord(stereotaxicRect.right(),
                                    stereotaxicRect.bottom(),
                                    0.0);
    
    PixelLogicalIndex topLeftPixel;
    PixelLogicalIndex bottomRightPixel;
    if (stereotaxicXyzToLogicalPixelIndex(topLeftCoord, topLeftPixel)
        && stereotaxicXyzToLogicalPixelIndex(bottomRightCoord, bottomRightPixel)) {
        rect.setCoords(topLeftPixel.getI(), topLeftPixel.getJ(),
                       bottomRightPixel.getI(), bottomRightPixel.getJ());
    }
    return rect;
}

/**
 * Convert a logical rect to a stereotaxic rect
 * @param logicalRect
 *    The logical rectangle
 * @return
 *    The stereotaxic rectangle
 */
QRectF
MediaFile::logicalRectToStereotaxicRect(const QRectF& logicalRect) const
{
    QRectF rect;
    
    const PixelLogicalIndex topLeftPixel(logicalRect.left(),
                                         logicalRect.top(),
                                         0.0f);
    const PixelLogicalIndex bottomRightPixel(logicalRect.right(),
                                             logicalRect.bottom(),
                                             0.0f);
    Vector3D topLeftCoord;
    Vector3D bottomRightCoord;
    if (logicalPixelIndexToStereotaxicXYZ(topLeftPixel, topLeftCoord)
        && logicalPixelIndexToStereotaxicXYZ(bottomRightPixel, bottomRightCoord)) {
        rect.setCoords(topLeftCoord[0], topLeftCoord[1],
                       bottomRightCoord[0], bottomRightCoord[1]);
    }
    return rect;
}

/**
 * Set the media file transforms
 * @param mediaFileTransforms
 *    New instance of media file transforms.
 */
void
MediaFile::setMediaFileTransforms(const MediaFileTransforms& mediaFileTransforms)
{
    m_mediaFileTransforms = mediaFileTransforms;
}


