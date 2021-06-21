
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

#include <QImage>

#include "BoundingBox.h"
#include "CaretAssert.h"
#include "CaretLogger.h"
#include "SceneClass.h"
#include "SceneClassAssistant.h"
#include "VolumeSpace.h"

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
    initializeMembersMediaFile();
    
//    if (mediaFile.m_spatialBoundingBox) {
//        m_spatialBoundingBox.reset(new BoundingBox(*mediaFile.m_spatialBoundingBox));
//    }
//    if (mediaFile.m_volumeSpace) {
//        m_volumeSpace.reset(new VolumeSpace(*mediaFile.m_volumeSpace));
//    }
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
//    m_spatialBoundingBox.reset(new BoundingBox());
//    m_volumeSpace.reset(new VolumeSpace());
    m_sceneAssistant = std::unique_ptr<SceneClassAssistant>(new SceneClassAssistant());
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
    return NULL;
}

/**
 * @return File casted to an media file (avoids use of dynamic_cast that can be slow)
 * Overidden in ImageFile
 */
const MediaFile*
MediaFile::castToMediaFile() const
{
    return NULL;
}

/**
 * Initialize the volume space
 * @param imageWidth
 *    Width of the image
 * @param imageHeight
 *    Height of the image
 * @param firstPixelXYZ
 *    Coordinates of first pixel at center of pixel
 * @param pixelStepXYZ
 *    Step to next adjacent pixel
 *@return
 *    Pair containing volume space and bounding box.  Caller takes ownership of the returned values.
 */
std::pair<VolumeSpace*,BoundingBox*>
MediaFile::initializeVolumeSpace(const int32_t imageWidth,
                                 const int32_t imageHeight,
                                 const std::array<float,3> firstPixelXYZ,
                                 const std::array<float,3> pixelStepXYZ)
{
    VolumeSpace* volumeSpace = new VolumeSpace();
    BoundingBox* boundingBox = new BoundingBox();
    boundingBox->resetZeros();
    
    if ((imageWidth < 2)
        || (imageHeight < 2)) {
        return std::make_pair(volumeSpace,
                              boundingBox);
    }
    
    std::vector<float> rowOne   { pixelStepXYZ[0], 0.0, 0.0, firstPixelXYZ[0] };
    std::vector<float> rowTwo   { 0.0, pixelStepXYZ[1], 0.0, firstPixelXYZ[1] };
    std::vector<float> rowThree { 0.0, 0.0, pixelStepXYZ[2], firstPixelXYZ[2] };
    std::vector<float> rowFour  { 0.0, 0.0, 0.0, 1.0 };
    
    std::vector<std::vector<float>> sform;
    sform.push_back(rowOne);
    sform.push_back(rowTwo);
    sform.push_back(rowThree);
    sform.push_back(rowFour);
    
    const int32_t imageStackSize(1);
    const int64_t dims[3] { imageWidth, imageHeight, imageStackSize };
    
    delete volumeSpace;
    volumeSpace = new VolumeSpace(dims, sform);
    
    const float minIJK[3] { -0.5, -0.5, -0.5 };
    float minX, minY, minZ;
    volumeSpace->indexToSpace(minIJK, minX, minY, minZ);
    
    float maxIJK[3] { imageWidth - 0.5f, imageHeight - 0.5f, 0.5f };
    float maxX, maxY, maxZ;
    volumeSpace->indexToSpace(maxIJK, maxX, maxY, maxZ);
    
    boundingBox->set(minX, maxX,
                     minY, maxY,
                     minZ, maxZ);

    const bool testFlag(false);
    if (testFlag) {
        float x, y, z;
        
        volumeSpace->indexToSpace(0, 0, 0, x, y, z);
        std::cout << "First Pixel XYZ: " << x << ", " << y << ", " << z << std::endl;
        
        volumeSpace->indexToSpace(imageWidth - 1, imageHeight - 1, imageStackSize - 1, x, y, z);
        std::cout << "Last Pixel XYZ: " << x << ", " << y << ", " << z << std::endl;
        
        std::cout << "Min XYZ: " << minX << ", " << minY << ", " << minZ << std::endl;
        
        std::cout << "Max XYZ: " << maxX << ", " << maxY << ", " << maxZ << std::endl;
        
        std::cout << std::flush;
    }
    
    return std::make_pair(volumeSpace,
                          boundingBox);
}

/**
 * Set the default spatial coordinates
 * @param qImage
 *    The QImage instance
 *@param spatialOrigin
 *    Origin (0,0) for spatial coordinates
 */
std::pair<VolumeSpace*,BoundingBox*>
MediaFile::setDefaultSpatialCoordinates(const QImage* qImage,
                                        const SpatialOrigin spatialOrigin)
{
    CaretAssert(qImage);
    std::array<float, 3> pixelBottomLeftSpatialXYZ;
    pixelBottomLeftSpatialXYZ.fill(0.0);
    
    std::array<float, 3> pixelTopRightSpatialXYZ;
    pixelTopRightSpatialXYZ.fill(1.0);
    
    std::array<float, 3> pixelStepXYZ;
    pixelStepXYZ.fill(1.0);
    
    int64_t imageWidthInt(0);
    int64_t imageHeightInt(0);
    
    if (qImage != NULL) {
        if ( ! qImage->isNull()) {
            imageWidthInt = qImage->width();
            imageHeightInt = qImage->height();
            
            const float imageWidth(qImage->width());
            const float imageHeight(qImage->height());
            
            if ((imageWidth >= 1.0f)
                && (imageHeight >= 1.0f)) {
                switch (spatialOrigin) {
                    case SpatialOrigin::BOTTOM_LEFT:
                        /*
                         * Uses default valuses for bottom left and step
                         */
                        break;
                    case SpatialOrigin::CENTER:
                    {
                        float minX(0.0), maxX(0.0);
                        getDefaultSpatialValues(imageWidth,
                                                minX,
                                                maxX,
                                                pixelBottomLeftSpatialXYZ[0],
                                                pixelTopRightSpatialXYZ[0],
                                                pixelStepXYZ[0]);
                        
                        float minY, maxY;
                        getDefaultSpatialValues(imageHeight,
                                                minY,
                                                maxY,
                                                pixelBottomLeftSpatialXYZ[1],
                                                pixelTopRightSpatialXYZ[1],
                                                pixelStepXYZ[1]);
                    }
                        break;
                }
            }
        }
    }
    
    return initializeVolumeSpace(imageWidthInt,
                                 imageHeightInt,
                                 pixelBottomLeftSpatialXYZ,
                                 pixelStepXYZ);
}

/**
 * Setup the spatial values for an image
 *
 * @param numPixels
 *    Number of pixels in the dimension
 * @param spatialMinimumValue
 *    Spatial value at left/bottom edge of first pixel
 * @param spatialMaximumValue
 *    Spatial value at right/top edge of first pixel
 * @param firstPixelSpatialValue
 *    Spatial value at middle of first pixel
 * @param lastPixelSpatialValue
 *    Spatial value at middle of last pixel
 * @param pixelSpatialStepValue
 *    Spatial step to next adjacent pixel
 */
void
MediaFile::getSpatialValues(const float numPixels,
                            const float spatialMinimumValue,
                            const float spatialMaximumValue,
                            float& firstPixelSpatialValue,
                            float& lastPixelSpatialValue,
                            float& pixelSpatialStepValue)
{
    CaretAssert(numPixels >= 1.0f);
    pixelSpatialStepValue  = (spatialMaximumValue - spatialMinimumValue) / numPixels;
    const float halfStepValue(pixelSpatialStepValue / 2.0);
    firstPixelSpatialValue = spatialMinimumValue + halfStepValue;
    lastPixelSpatialValue  = firstPixelSpatialValue + (pixelSpatialStepValue * (numPixels - 1));
}

/**
 * Setup the spatial values for an image with no available spatial coordinates.  The origin
 * will be in the center of the image.  Spatial minimum/maximum will be half the number of pixels.
 * Example: if image is 400 pixels, min spatial = -200; max spatial = 200
 *
 * @param numPixels
 *    Number of pixels in the dimension
 * @param minSpatialValueOut
 *    Spatial value at left/bottom edge of first pixel
 * @param maxSpatialValueOut
 *    Spatial value at right/top edge of first pixel
 * @param firstPixelSpatialValue
 *    Spatial value at middle of first pixel
 * @param lastPixelSpatialValue
 *    Spatial value at middle of last pixel
 * @param pixelSpatialStepValue
 *    Spatial step to next adjacent pixel
 */
void
MediaFile::getDefaultSpatialValues(const float numPixels,
                                   float& minSpatialValueOut,
                                   float& maxSpatialValueOut,
                                   float& firstPixelSpatialValue,
                                   float& lastPixelSpatialValue,
                                   float& pixelSpatialStepValue)
{
    const float halfValue(numPixels / 2.0);
    minSpatialValueOut = -halfValue;
    maxSpatialValueOut =  halfValue;
    MediaFile::getSpatialValues(numPixels,
                                minSpatialValueOut,
                                maxSpatialValueOut,
                                firstPixelSpatialValue,
                                lastPixelSpatialValue,
                                pixelSpatialStepValue);
}

///**
// * @return Pointer to spatial bounding box (outer edges of pixels; not center of pixels)
// */
//const BoundingBox*
//MediaFile::getSpatialBoudingBox() const
//{
//    CaretAssert(m_spatialBoundingBox.get());
//    return m_spatialBoundingBox.get();
//}

/**
 * @return True if the given pixel index is valid, else false
 * @param tabIndex
 *    Index of the tab.
 * @param pixelIndex
 *    The pixel index
 */
bool
MediaFile::indexValid(const int32_t tabIndex,
                      const PixelIndex& pixelIndex) const
{
    const auto xform(getPixelToCoordinateTransform(tabIndex));
    CaretAssert(xform);
    return xform->indexValid(pixelIndex.m_ijk);
}

/**
 * Convert the given spatial coordinates to image pixel indices
 * @param tabIndex
 *    Index of the tab.
 * @param coordinate
 *    The coordinate
 * @return
 *    The Pixel Index (may not be a valid index)
 */
MediaFile::PixelIndex
MediaFile::spaceToIndex(const int32_t tabIndex,
                        const PixelCoordinate& coordinate) const
{
    PixelCoordinate indexOut(0,0,0);
    const auto xform(getPixelToCoordinateTransform(tabIndex));
    CaretAssert(xform);
    xform->spaceToIndex(coordinate,
                        indexOut);
    
    return PixelIndex(indexOut);
}

/**
 * Convert the given spatial coordinates to image pixel indices
 * Convert the given spatial coordinates to image pixel indices
 * @param tabIndex
 *    Index of the tab.
 * @param coordinate
 *    The coordinate
 * @param pixelIndexOut
 *    Output containing the pixel index
 * @return True if output index is valid for the image, else false.
 */
bool
MediaFile::spaceToIndexValid(const int32_t tabIndex,
                             const PixelCoordinate& coordinate,
                             PixelIndex& pixelIndexOut) const
{
    pixelIndexOut = spaceToIndex(tabIndex,
                                 coordinate);
    return indexValid(tabIndex,
                      pixelIndexOut);
}

/**
 * @return Half of the height for the orthographic  media drawing viewport
 */
float
MediaFile::getMediaDrawingOrthographicHalfHeight()
{
    return 500.0;
}

