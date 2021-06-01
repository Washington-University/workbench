
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
    m_spatialBoundingBox.reset(new BoundingBox());
    m_volumeSpace.reset(new VolumeSpace());
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
MediaFile::saveSubClassDataToScene(const SceneAttributes* sceneAttributes,
                                            SceneClass* sceneClass)
{
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
MediaFile::restoreSubClassDataFromScene(const SceneAttributes* sceneAttributes,
                                                 const SceneClass* sceneClass)
{
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
 */
void
MediaFile::initializeVolumeSpace(const int32_t imageWidth,
                                 const int32_t imageHeight,
                                 const std::array<float,3> firstPixelXYZ,
                                 const std::array<float,3> pixelStepXYZ)
{
    m_volumeSpace.reset(new VolumeSpace());
    m_spatialBoundingBox->resetZeros();
    
    if ((imageWidth < 2)
        || (imageHeight < 2)) {
        return;
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
    
    m_volumeSpace.reset(new VolumeSpace(dims, sform));
    
    const float minIJK[3] { -0.5, -0.5, -0.5 };
    float minX, minY, minZ;
    m_volumeSpace->indexToSpace(minIJK, minX, minY, minZ);
    
    float maxIJK[3] { imageWidth - 0.5f, imageHeight - 0.5f, 0.5f };
    float maxX, maxY, maxZ;
    m_volumeSpace->indexToSpace(maxIJK, maxX, maxY, maxZ);
    
    m_spatialBoundingBox->set(minX, maxX,
                              minY, maxY,
                              minZ, maxZ);
    
    const bool testFlag(false);
    if (testFlag) {
        float x, y, z;
        
        m_volumeSpace->indexToSpace(0, 0, 0, x, y, z);
        std::cout << "First Pixel XYZ: " << x << ", " << y << ", " << z << std::endl;
        
        m_volumeSpace->indexToSpace(imageWidth - 1, imageHeight - 1, imageStackSize - 1, x, y, z);
        std::cout << "Last Pixel XYZ: " << x << ", " << y << ", " << z << std::endl;
        
        std::cout << "Min XYZ: " << minX << ", " << minY << ", " << minZ << std::endl;
        
        std::cout << "Max XYZ: " << maxX << ", " << maxY << ", " << maxZ << std::endl;
        
        std::cout << std::flush;
    }
}

/**
 * @return Pointer to spatial bounding box (outer edges of pixels; not center of pixels)
 */
const BoundingBox*
MediaFile::getSpatialBoudingBox() const
{
    CaretAssert(m_spatialBoundingBox.get());
    return m_spatialBoundingBox.get();
}

/**
 * @return True if the given pixel index is valid, else false
 * @param pixelIndex
 *    The pixel index
 */
bool
MediaFile::indexValid(const PixelIndex& pixelIndex) const
{
    return m_volumeSpace->indexValid(pixelIndex.m_ijk);
}

/**
 * Convert the given spatial coordinates to image pixel indices
 * @param coordinate
 *    The coordinate
 * @return
 *    The Pixel Index (may not be a valid index)
 */
MediaFile::PixelIndex
MediaFile::spaceToIndex(const PixelCoordinate& coordinate) const
{
    PixelCoordinate indexOut(0,0,0);
    m_volumeSpace->spaceToIndex(coordinate,
                                indexOut);
    
    return PixelIndex(indexOut);
}

/**
 * Convert the given spatial coordinates to image pixel indices
 * Convert the given spatial coordinates to image pixel indices
 * @param coordinate
 *    The coordinate
 * @param pixelIndexOut
 *    Output containing the pixel index
 * @return True if output index is valid for the image, else false.
 */
bool
MediaFile::spaceToIndexValid(const PixelCoordinate& coordinate,
                             PixelIndex& pixelIndexOut) const
{
    pixelIndexOut = spaceToIndex(coordinate);
    return indexValid(pixelIndexOut);
}

/**
 * @return Half of the height for the orthographic  media drawing viewport
 */
float
MediaFile::getMediaDrawingOrthographicHalfHeight()
{
    return 500.0;
}

