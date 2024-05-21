
/*LICENSE_START*/
/*
 *  Copyright (C) 2024 Washington University School of Medicine
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

#define __VOLUME_TO_IMAGE_MAPPING_DECLARE__
#include "VolumeToImageMapping.h"
#undef __VOLUME_TO_IMAGE_MAPPING_DECLARE__

#include <array>

#include "CaretAssert.h"
#include "CaretOMP.h"
#include "CiftiMappableDataFile.h"
#include "DataFileContentInformation.h"
#include "DataFileException.h"
#include "ElapsedTimer.h"
#include "GiftiLabel.h"
#include "GroupAndNameHierarchyItem.h"
#include "HistologySlice.h"
#include "HistologySliceImage.h"
#include "ImageFile.h"
#include "VolumeFile.h"

using namespace caret;


    
/**
 * \class caret::VolumeToImageMapping 
 * \brief Map a volume file to an image that has stereotaxic information available
 * \ingroup Files
 */

/**
 * Constructor.
 * @param volumeFile
 *    Volume file mapped to image
 * @param volumeFileMapIndex
 *    Index of volume file for mapping to image
 * @param volumeMappingMode
 *    Mode for volume mapping
 * @param volumeSliceThickness
 *    Thickness of volume slice for some modes
 * @param displayGroup
 *    The display group
 * @param tabIndex
 *    Index of the tab
 * @param inputMediaFile
 *    Image file used for spatial information
 */
VolumeToImageMapping::VolumeToImageMapping(const VolumeMappableInterface* volumeInterface,
                                           const int32_t volumeFileMapIndex,
                                           const VolumeToImageMappingModeEnum::Enum volumeMappingMode,
                                           const float volumeSliceThickness,
                                           const DisplayGroupEnum::Enum displayGroup,
                                           const int32_t tabIndex,
                                           const MediaFile* inputMediaFile)
: CaretObject(),
m_volumeInterface(volumeInterface),
m_volumeFileMapIndex(volumeFileMapIndex),
m_inputVolumeMappingMode(volumeMappingMode),
m_volumeSliceThickness(volumeSliceThickness),
m_displayGroup(displayGroup),
m_tabIndex(tabIndex),
m_inputMediaFile(inputMediaFile),
m_histologySlice(NULL)
{
    CaretAssert(m_volumeInterface);
    CaretAssert(m_inputMediaFile);
    
    m_volumeMappableDataFile = m_volumeInterface->castToVolumeMappableDataFile();
    CaretAssert(m_volumeMappableDataFile);
    CaretAssert((volumeFileMapIndex >= 0)
                && (volumeFileMapIndex < m_volumeMappableDataFile->getNumberOfMaps()));
    m_volumeFile = dynamic_cast<const VolumeFile*>(m_volumeMappableDataFile);
    m_ciftiMappableDataFile = dynamic_cast<const CiftiMappableDataFile*>(m_volumeMappableDataFile);
}

/**
 * Constructor.
 * @param volumeFile
 *    Volume file mapped to image
 * @param volumeFileMapIndex
 *    Index of volume file for mapping to image
 * @param volumeMappingMode
 *    Mode for volume mapping
 * @param volumeSliceThickness
 *    Thickness of volume slice for some modes
 * @param displayGroup
 *    The display group
 * @param tabIndex
 *    Index of the tab
 * @param histologySlice
 *    Histology slice for spatial information
 */
VolumeToImageMapping::VolumeToImageMapping(const VolumeMappableInterface* volumeInterface,
                                           const int32_t volumeFileMapIndex,
                                           const VolumeToImageMappingModeEnum::Enum volumeMappingMode,
                                           const float volumeSliceThickness,
                                           const DisplayGroupEnum::Enum displayGroup,
                                           const int32_t tabIndex,
                                           const HistologySlice* histologySlice)
: CaretObject(),
m_volumeInterface(volumeInterface),
m_volumeFileMapIndex(volumeFileMapIndex),
m_inputVolumeMappingMode(volumeMappingMode),
m_volumeSliceThickness(volumeSliceThickness),
m_displayGroup(displayGroup),
m_tabIndex(tabIndex),
m_inputMediaFile(NULL),
m_histologySlice(histologySlice)
{
    CaretAssert(m_volumeInterface);
    CaretAssert(m_histologySlice);
    
    m_volumeMappableDataFile = m_volumeInterface->castToVolumeMappableDataFile();
    CaretAssert(m_volumeMappableDataFile);
    CaretAssert((volumeFileMapIndex >= 0)
                && (volumeFileMapIndex < m_volumeMappableDataFile->getNumberOfMaps()));
    m_volumeFile = dynamic_cast<const VolumeFile*>(m_volumeMappableDataFile);
    m_ciftiMappableDataFile = dynamic_cast<const CiftiMappableDataFile*>(m_volumeMappableDataFile);
}


/**
 * Destructor.
 */
VolumeToImageMapping::~VolumeToImageMapping()
{
}

/**
 * Map the input volume to the image file
 * @param errorMessageOut
 *    Contains error information if mapping fails
 * @return True if mapping was successful, else false.
 */
bool
VolumeToImageMapping::runMapping(AString& errorMessageOut)
{
    bool successFlag(false);
    ElapsedTimer timer;
    timer.start();
    
    std::vector<const MediaFile*> allInputMediaFiles;
    
    if (m_histologySlice != NULL) {
        const int32_t numImages(m_histologySlice->getNumberOfHistologySliceImages());
        for (int32_t i = 0; i < numImages; i++) {
            const HistologySliceImage* hsi(m_histologySlice->getHistologySliceImage(i));
            CaretAssert(hsi);
            const MediaFile* mediaFile(hsi->getMediaFile());
            CaretAssert(mediaFile);
            allInputMediaFiles.push_back(mediaFile);
        }
    }
    else if (m_inputMediaFile != NULL) {
        allInputMediaFiles.push_back(m_inputMediaFile);
    }
    
    const int32_t numInputMediaFiles(allInputMediaFiles.size());
    if (numInputMediaFiles <= 0) {
        errorMessageOut.appendWithNewLine("No input images are available for mapping with volume data.");
        return false;
    }
    
    for (int32_t iMediaFileIndex = 0; iMediaFileIndex < numInputMediaFiles; iMediaFileIndex++) {
        const MediaFile* mediaFile(allInputMediaFiles[iMediaFileIndex]);
        CaretAssert(mediaFile);
        
        if (validateInputs(mediaFile,
                           errorMessageOut)) {
            if (performMapping(mediaFile,
                               errorMessageOut)) {
                successFlag = true;
            }
        }
        
        if ( ! successFlag) {
            AString fileMsg;
            AString volumeFileName("Invalid volume file name");
            AString mediaFileName("Invalied media file name");
            if (m_volumeMappableDataFile != NULL) {
                fileMsg.appendWithNewLine("Volume File Name: " + m_volumeMappableDataFile->getFileName());
                fileMsg.appendWithNewLine("Media File Name: " + mediaFileName);
                errorMessageOut = (fileMsg
                                   + "\n"
                                   + errorMessageOut);
                
                /*
                 * Exit loop since there is an error
                 */
                break;
            }
        }
    }
    //std::cout << "Time to map volume to image: " << timer.getElapsedTimeMilliseconds() << "ms" << std::endl;
    
    return successFlag;
}

/**
 * @return True if the inputs are valid, else false.
 * @param mediaFile
 *    Media file that is being mapped to volume
 * @param errorMessageOut
 *   In inputs are not valid, contains error information
 */
bool
VolumeToImageMapping::performMapping(const MediaFile* mediaFile,
                                     AString& errorMessageOut)
{
    errorMessageOut.clear();

    /*
     * Label volumes only allow "NORMAL" mapping
     * Palette volumes support the average/min/max modes
     */
    bool intensityMappingFlag(false);
    if (m_volumeMappableDataFile->isMappedWithPalette()) {
        switch (m_inputVolumeMappingMode) {
            case VolumeToImageMappingModeEnum::NORMAL:
                break;
            case VolumeToImageMappingModeEnum::AVERAGE_INTENSITY:
                intensityMappingFlag = true;
                break;
            case VolumeToImageMappingModeEnum::MAXIMUM_INTENSITY:
                intensityMappingFlag = true;
                break;
            case VolumeToImageMappingModeEnum::MINIMUM_INTENSITY:
                intensityMappingFlag = true;
                break;
        }
    }
    
    const bool limitDimensionsFlag(false);
    if (limitDimensionsFlag) {
        const int32_t bestDimension(getImageBestDimension(mediaFile,
                                                          m_volumeInterface));
        std::cout << "Best dimension: " << bestDimension << std::endl;
    }

    /*
     * Create output image file by cloning as an image file
     */
    const int32_t maxImageDim(2000);
    ImageFile* outputImageFile(mediaFile->cloneAsImageFileMaximumWidthHeight(maxImageDim,
                                                                             errorMessageOut));
    if (outputImageFile == NULL) {
        return false;
    }
    outputImageFile->setFileName("VolumeMapping_"
                                 + AString::number(m_outputImageFiles.size())
                                 + ".png");
    std::unique_ptr<ImageFile> ptrOutputImageFile(outputImageFile);
        
    if ( ! validateMediaFile(outputImageFile,
                             "Output image file copied from input image file ",
                             errorMessageOut)) {
        return false;
    }

    bool resultFlag(false);
    
    if (intensityMappingFlag) {
        resultFlag = performIntensityMapping(mediaFile,
                                             outputImageFile,
                                             errorMessageOut);
    }
    else {
        resultFlag = performRgbaMapping(outputImageFile,
                                        errorMessageOut);
    }
    
    if (resultFlag) {
        m_outputImageFiles.push_back(std::move(ptrOutputImageFile));
    }
    
    return resultFlag;
}

/**
 * Perform RGBA mapping from volume to image
 * @param outputImageFile
 *    The output image file
 * @param errorMessageOut
 *    Contains error information if error
 */
bool
VolumeToImageMapping::performRgbaMapping(ImageFile* outputImageFile,
                                         AString& errorMessageOut)
{
    bool cacheValuesFlag(false);
    if (m_ciftiMappableDataFile != NULL) {
        /*
         * Accessing values from a CIFTI file can be very slow and
         * it is possible that a CIFTI file is on a remote server.
         * During testing, caching values is much faster.
         */
        cacheValuesFlag = true;
    }
    if (m_volumeFile != NULL) {
        /*
         * During testing, using cached values seemed to be slightly
         * slower than just getting values from the volume file.
         */
        cacheValuesFlag = false;
    }
    
    int32_t unassignedLabelKey(-1);
    if (m_volumeMappableDataFile->isMappedWithLabelTable()) {
        const GiftiLabelTable* labelTable = m_volumeMappableDataFile->getMapLabelTable(m_volumeFileMapIndex);
        CaretAssert(labelTable);
        
        unassignedLabelKey = labelTable->getUnassignedLabelKey();
    }
    
    const int64_t imageWidth(outputImageFile->getWidth());
    const int64_t imageHeight(outputImageFile->getHeight());
    
    bool rowFlag(true);
    std::vector<uint8_t> rowRGBA;
    if (rowFlag) {
        rowRGBA.resize(imageWidth * 4);
    }
    
    /*
     * Note: Keep pixelIndex outside of loop as it inherits other classes
     * and construction/deconstruction many, many times becomes
     * slow.
     */
    PixelIndex pixelIndex;
    
    int64_t validPixelCounter(0);
    std::array<uint8_t, 4> pixelRGBA;
    for (int64_t jRow = 0; jRow < imageHeight; jRow++) {
        for (int64_t iCol = 0; iCol < imageWidth; iCol++) {
            /*
             * Initialize alpha to zero so nothing displayed
             */
            pixelRGBA.fill(0);
            
            /*
             * Note: Some pixels may not map to a stereotaxic coordinate
             */
            pixelIndex.setIJK(iCol, jRow, 0);
            Vector3D xyz;
            if (outputImageFile->pixelIndexToStereotaxicXYZ(pixelIndex,
                                                            xyz)) {
                ++validPixelCounter;
                
                int64_t ijk[3];
                m_volumeInterface->enclosingVoxel(xyz,
                                                  ijk);
                if (m_volumeInterface->indexValid(ijk)) {
                    bool havePixelRgbaFlag(false);
                    const VoxelIJK ijkVoxel(ijk);
                    if (cacheValuesFlag) {
                        const auto iter(m_ijkRgbaMap.find(ijkVoxel));
                        if (iter != m_ijkRgbaMap.end()) {
                            pixelRGBA = iter->second;
                            havePixelRgbaFlag = true;
                        }
                    }
                    
                    if ( ! havePixelRgbaFlag) {
                        /*
                         * Note: Calling this method may assign colors for entire volume
                         */
                        m_volumeInterface->getVoxelColorInMap(ijk[0], ijk[1], ijk[2],
                                                              m_volumeFileMapIndex,
                                                              pixelRGBA.data());
                        
                        
                        
                        if (m_volumeMappableDataFile->isMappedWithLabelTable()) {
                            /*
                             * For label data, we need to get the data value
                             * so that we can use it to get the label and test
                             * the label's selection status
                             */
                            int32_t dataValueInt(0.0);
                            bool dataValueValidFlag(false);
                            
                            if (dataValueValidFlag) {
                                /* nothing, have value */
                            }
                            else if (m_volumeFile != NULL) {
                                /*
                                 * This method might read from disk or if file is on remote server
                                 * so it must be in a critical section (need to verify this)
                                 */
                                dataValueInt = static_cast<int32_t>(m_volumeFile->getValue(ijk[0],
                                                                                           ijk[1],
                                                                                           ijk[2],
                                                                                           m_volumeFileMapIndex));
                                dataValueValidFlag = true;
                            }
                            else if (m_ciftiMappableDataFile != NULL) {
                                dataValueInt = m_ciftiMappableDataFile->getVoxelValue(xyz,
                                                                                      &dataValueValidFlag,
                                                                                      m_volumeFileMapIndex);
                            }
                            else {
                                CaretAssertMessage(0, "File is not a volume file");
                            }
                            const GiftiLabelTable* labelTable = m_volumeMappableDataFile->getMapLabelTable(m_volumeFileMapIndex);
                            CaretAssert(labelTable);
                            
                            
                            if ((dataValueInt != unassignedLabelKey)
                                && (dataValueValidFlag)) {
                                const GiftiLabel* label = labelTable->getLabel(dataValueInt);
                                if (label != NULL) {
                                    const GroupAndNameHierarchyItem* item = label->getGroupNameSelectionItem();
                                    if (item != NULL) {
                                        if ( ! item->isSelected(m_displayGroup,
                                                                m_tabIndex)) {
                                            pixelRGBA[3] = 0;
                                        }
                                    }
                                }
                            }
                        }
                        
                        /*
                         * Cache the RGBA value for the voxel
                         */
                        if (cacheValuesFlag) {
                            m_ijkRgbaMap[ijkVoxel] = pixelRGBA;
                        }
                    }
                }
                
            }
            
            const int32_t invalidTabIndex(-1);
            const int32_t invalidOverlayIndex(-1);
            if (rowFlag) {
                const int64_t i4(iCol * 4);
                CaretAssertVectorIndex(rowRGBA, i4+3);
                rowRGBA[i4]   = pixelRGBA[0];
                rowRGBA[i4+1] = pixelRGBA[1];
                rowRGBA[i4+2] = pixelRGBA[2];
                rowRGBA[i4+3] = pixelRGBA[3];
            }
            else {
                outputImageFile->setPixelRGBA(invalidTabIndex,
                                              invalidOverlayIndex,
                                              pixelIndex,
                                              pixelRGBA.data());
            }
        } /* for iCol */
        
        if (rowFlag) {
            outputImageFile->setPixelRowRGBA(jRow,
                                             rowRGBA);
        }
    } /* for jRow */
    
    if (validPixelCounter <= 0) {
        errorMessageOut.appendWithNewLine("No intersection between image and volume");
    }
    
    return (validPixelCounter > 0);

}

/**
 * @return True if the inputs are valid, else false.
 * @param mediaFile
 *    Media file that is being mapped to volume
 * @param errorMessageOut
 *   In inputs are not valid, contains error information
 */
bool
VolumeToImageMapping::validateInputs(const MediaFile* mediaFile,
                                     AString& errorMessageOut)
{
    errorMessageOut.clear();
    
    if (m_volumeMappableDataFile == NULL) {
        errorMessageOut.appendWithNewLine("Input volume file is invalid (NULL)");
    }
    else {
        if ((m_volumeFileMapIndex < 0)
            || (m_volumeFileMapIndex >= m_volumeMappableDataFile->getNumberOfMaps())) {
            errorMessageOut.appendWithNewLine("Volume file map index is invalid="
                                           + AString::number(m_volumeFileMapIndex));
        }
        
        std::vector<int64_t> dims;
        m_volumeInterface->getDimensions(dims);
        if ((dims[0] <= 0)
            || (dims[1] <= 0)
            || (dims[2] <= 0)) {
            errorMessageOut.appendWithNewLine("Volume dimensions are invalid: "
                                           + AString::fromNumbers(dims));
        }
    }

    validateMediaFile(mediaFile,
                      "Input media file ",
                      errorMessageOut);

    return (errorMessageOut.isEmpty());
}

/**
 * @return True if the image file is valid, else false.
 * @param mediaFile
 *    The media file
 * @param errorMessagePrefix
 *   Prefix for error messages
 * @param errorMessageInOut
 *   In image is not valid, contains error information.  This is both an
 *   input and an output so DO NOT clear this.
 */
bool
VolumeToImageMapping::validateMediaFile(const MediaFile* mediaFile,
                                        const AString errorMessagePrefix,
                                        AString& errorMessageInOut)
{
    if (mediaFile == NULL) {
        errorMessageInOut.appendWithNewLine(errorMessagePrefix
                                            + "is invalid (NULL)");
    }
    else {
        if ((mediaFile->getWidth() <= 0)
            || (mediaFile->getHeight() <= 0)) {
            errorMessageInOut.appendWithNewLine(errorMessagePrefix
                                                + "has invalid width="
                                                + AString::number(mediaFile->getWidth())
                                                + " and/or height="
                                                + AString::number(mediaFile->getHeight()));
        }
        
        if ( ! mediaFile->isPlaneToMillimetersMatrixValid()) {
            errorMessageInOut = (errorMessagePrefix
                                 + "does not support conversion of Pixels to Millimeters (Stereotaxic coordinates)");
        }
    }
    
    return (errorMessageInOut.isEmpty());
}

/**
 * @return Number of output image files
 */
int32_t
VolumeToImageMapping::getNumberOfOutputImageFiles() const
{
    return m_outputImageFiles.size();
}

/**
 * @return Image file that was created by the mapping process.
 * This "takes" the image file that was created and caller is responsible
 * for destroying the returned image file.  NULL is returned if the
 * mapping process failed or if this method is called more than one time.
 * @param index
 *    Index of the image file
 */
ImageFile*
VolumeToImageMapping::takeOutputImageFile(const int32_t index)
{
    ImageFile* file(NULL);
    CaretAssertVectorIndex(m_outputImageFiles, index);
    if (m_outputImageFiles[index]) {
        file = m_outputImageFiles[index].release();
    }
    return file;
}

/**
 * @return Best maximum dimension for output image file
 * @param mediaFile
 *    Media file for mapping to
 * @param volumeInterface
 *    Volume Interface mapped to image
 */
int32_t
VolumeToImageMapping::getImageBestDimension(const MediaFile* mediaFile,
                                            const VolumeMappableInterface* volumeInterface) const
{
    int32_t dimOut(-1);
    
    CaretAssert(mediaFile);
    CaretAssert(volumeInterface);
    
    const PixelIndex topLeftPixel(0, 0);
    const PixelIndex topRightPixel(0, mediaFile->getWidth() - 1);
    Vector3D topLeftStereotaxicXYZ;
    Vector3D topRightStereotaxicXYZ;
    
    if (mediaFile->pixelIndexToStereotaxicXYZ(topLeftPixel,
                                                     topLeftStereotaxicXYZ)
        && mediaFile->pixelIndexToStereotaxicXYZ(topRightPixel,
                                                        topRightStereotaxicXYZ)) {
        const float imageWidthMM((topLeftStereotaxicXYZ - topRightStereotaxicXYZ).length());
        const float imageWidthPixels(mediaFile->getWidth());
        const float imagePixelsPerMillimeter(imageWidthPixels
                                             / imageWidthMM);
        const float voxelSizeMM(volumeInterface->getMaximumVoxelSpacing());
        const float pixelsPerVoxel(imagePixelsPerMillimeter * voxelSizeMM);
        
        
        std::cout << "Pixels Per MM: " << imagePixelsPerMillimeter << std::endl;
        std::cout << "Pixels Per Voxel: " << pixelsPerVoxel << std::endl;
        
    }
    return dimOut;
}


/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
VolumeToImageMapping::toString() const
{
    return "VolumeToImageMapping";
}

/**
 * Perform Intensity mapping from volume to image
 * @param mediaFile
 *    Media file that is being mapped to volume
 * @param outputImageFile
 *    The output image file
 * @param errorMessageOut
 *    Contains error information if error
 */
bool
VolumeToImageMapping::performIntensityMapping(const MediaFile* mediaFile,
                                              ImageFile* outputImageFile,
                                              AString& errorMessageOut)
{
    Vector3D imageStereotaxicNormalVector(0.0, 0.0, 1.0);
    const Plane* plane = mediaFile->getStereotaxicImagePlane();
    if (plane->isValidPlane()) {
        imageStereotaxicNormalVector = plane->getNormalVector();
    }
    else {
        errorMessageOut.appendWithNewLine("Input media file does not have valid stereotaxic normal vector.");
        return false;
    }

    bool averageFlag(false);
    bool minimumFlag(false);
    bool maximumFlag(false);
    switch (m_inputVolumeMappingMode) {
        case VolumeToImageMappingModeEnum::AVERAGE_INTENSITY:
            averageFlag = true;
            break;
        case VolumeToImageMappingModeEnum::MAXIMUM_INTENSITY:
            maximumFlag = true;
            break;
        case VolumeToImageMappingModeEnum::MINIMUM_INTENSITY:
            minimumFlag = true;
            break;
        case VolumeToImageMappingModeEnum::NORMAL:
            CaretAssert(0);
            break;
    }
    
    if (averageFlag
        || minimumFlag
        || maximumFlag) {
        /* OK */
    }
    else {
        CaretAssert(0);
    }
    
    bool cacheValuesFlag(false);
    if (m_ciftiMappableDataFile != NULL) {
        /*
         * Accessing values from a CIFTI file can be very slow and
         * it is possible that a CIFTI file is on a remote server.
         * During testing, caching values is much faster.
         */
        cacheValuesFlag = true;
    }
    if (m_volumeFile != NULL) {
        /*
         * During testing, using cached values seemed to be slightly
         * slower than just getting values from the volume file.
         */
        cacheValuesFlag = false;
    }
    
    const int64_t imageWidth(outputImageFile->getWidth());
    const int64_t imageHeight(outputImageFile->getHeight());
    
    bool rowFlag(true);
    std::vector<uint8_t> rowRGBA;
    if (rowFlag) {
        rowRGBA.resize(imageWidth * 4);
    }
    
    Vector3D offsetXYZ(0.0, 0.0, 0.0);
    Vector3D offsetStepXYZ(0.0, 0.0, 0.0);
    int32_t numPixelSteps = 0;
    if (m_volumeSliceThickness > 0.0) {
        const float voxelSize(m_volumeInterface->getMaximumVoxelSpacing());
        const float halfVoxelSize(voxelSize / 2.0);
        if (halfVoxelSize > 0.0) {
            numPixelSteps = static_cast<int32_t>(std::ceil(m_volumeSliceThickness / halfVoxelSize));
            if (numPixelSteps >= 1) {
                /*
                 * Starting position for stepping along normal vector
                 */
                const float halfSliceThickness(m_volumeSliceThickness / 2.0);
                offsetXYZ = (-imageStereotaxicNormalVector
                             * halfSliceThickness);
                
                const float stepDistance(m_volumeSliceThickness
                                           / static_cast<float>(numPixelSteps));
                offsetStepXYZ = (imageStereotaxicNormalVector
                                 * stepDistance);
            }
            else {
                numPixelSteps = 0;
            }
        }
    }
    
    /*
     * Note: Keep pixelIndex outside of loop as it inherits other classes
     * and construction/deconstruction many, many times becomes
     * slow.
     */
    PixelIndex pixelIndex;
    
    const int64_t jMiddle(-1);//imageHeight / 2);
    const int64_t iMiddle(-1);//imageWidth / 2);
    int64_t validPixelCounter(0);
    for (int64_t jRow = 0; jRow < imageHeight; jRow++) {
        for (int64_t iCol = 0; iCol < imageWidth; iCol++) {
            std::array<uint8_t, 4> pixelRGBA;
            pixelRGBA.fill(0);
            
            /*
             * Note: Some pixels may not map to a stereotaxic coordinate
             */
            pixelIndex.setIJK(iCol, jRow, 0);
            Vector3D pixelXYZ;
            if (outputImageFile->pixelIndexToStereotaxicXYZ(pixelIndex,
                                                            pixelXYZ)) {
                ++validPixelCounter;
                
                std::array<float, 4> pixelFloatRGBA;
                int32_t pixelFloatRGBACounter(0);
                
                /*
                 * Initialize alpha to zero so nothing displayed
                 */
                if (averageFlag) {
                    pixelFloatRGBA.fill(0.0);
                }
                else if (minimumFlag) {
                    pixelFloatRGBA.fill(std::numeric_limits<float>::max());
                }
                else if (maximumFlag) {
                    pixelFloatRGBA.fill(-std::numeric_limits<float>::max());
                }
                pixelFloatRGBA[3] = 0.0;

                /*
                 * Tracks voxels so we don't use the same voxel
                 * more than one time when stepping through volume
                 */
                std::set<std::array<int64_t, 3>> pixelsUsed;
                
                /*
                 * NEED TO MAKE SURE WE DO NOT ACCESS A PIXEL MORE THAN
                 * ONCE WHEN STEPPING THROUGH PIXELS
                 *
                 * USE CACHE ?????
                 */
                if ((jRow == jMiddle)
                    && (iCol == iMiddle)) {
                    std::cout << "PixelXYZ=" << pixelXYZ.toString() << std::endl;
                }
                for (int32_t iPixel = 0; iPixel <= numPixelSteps; iPixel++) {
                    Vector3D xyz = (pixelXYZ
                                    + offsetXYZ
                                    + (offsetStepXYZ * static_cast<float>(iPixel)));
                    if ((jRow == jMiddle)
                        && (iCol == iMiddle)) {
                        std::cout << "   iPixel=" << iPixel << " xyz=" << xyz.toString() << std::endl;
                    }
                    std::array<int64_t, 3> ijk;
                    m_volumeInterface->enclosingVoxel(xyz,
                                                      ijk.data());
                    if (m_volumeInterface->indexValid(ijk.data())) {
                        if (pixelsUsed.find(ijk) != pixelsUsed.end()) {
                            /*
                             * Voxel was already used when iterating
                             * through the voxels
                             */
                        }
                        else {
                            pixelsUsed.insert(ijk);
                            
                            std::array<uint8_t, 4> voxelRGBA;
                            bool voxelRGBAValidFlag(false);
                            const VoxelIJK ijkVoxel(ijk.data());
                            if (cacheValuesFlag) {
                                const auto iter(m_ijkRgbaMap.find(ijkVoxel));
                                if (iter != m_ijkRgbaMap.end()) {
                                    voxelRGBA = iter->second;
                                    voxelRGBAValidFlag = true;
                                }
                            }
                            
                            if ( ! voxelRGBAValidFlag) {
                                /*
                                 * Note: Calling this method may assign colors for entire volume
                                 */
                                m_volumeInterface->getVoxelColorInMap(ijk[0], ijk[1], ijk[2],
                                                                      m_volumeFileMapIndex,
                                                                      voxelRGBA.data());
                                voxelRGBAValidFlag = true;
                                
                                /*
                                 * Cache the RGBA value for the voxel
                                 */
                                if (cacheValuesFlag) {
                                    m_ijkRgbaMap[ijkVoxel] = voxelRGBA;
                                }
                            }
                            
                            if (voxelRGBAValidFlag) {
                                if (voxelRGBA[3] > 0) {
                                    const float red(voxelRGBA[0]);
                                    const float green(voxelRGBA[1]);
                                    const float blue(voxelRGBA[2]);
                                    const float alpha(voxelRGBA[3]);
                                    
                                    if (averageFlag) {
                                        pixelFloatRGBA[0] += red;
                                        pixelFloatRGBA[1] += green;
                                        pixelFloatRGBA[2] += blue;
                                        pixelFloatRGBA[3] += alpha;
                                    }
                                    else if (minimumFlag) {
                                        if (red < pixelFloatRGBA[0]) {
                                            pixelFloatRGBA[0] = red;
                                            pixelFloatRGBA[1] = green;
                                            pixelFloatRGBA[2] = blue;
                                            pixelFloatRGBA[3] = alpha;
                                        }
                                    }
                                    else if (maximumFlag) {
                                        if (red > pixelFloatRGBA[0]) {
                                            pixelFloatRGBA[0] = red;
                                            pixelFloatRGBA[1] = green;
                                            pixelFloatRGBA[2] = blue;
                                            pixelFloatRGBA[3] = alpha;
                                        }
                                    }
                                    
                                    ++pixelFloatRGBACounter;
                                }
                            }
                        }
                    }
                }
                
                if (pixelFloatRGBACounter > 0) {
                    if (averageFlag) {
                        const float num(pixelFloatRGBACounter);
                        pixelFloatRGBA[0] /= num;
                        pixelFloatRGBA[1] /= num;
                        pixelFloatRGBA[2] /= num;
                        pixelFloatRGBA[3] /= num;
                    }
                    
                    pixelRGBA[0] = static_cast<uint8_t>(pixelFloatRGBA[0]);
                    pixelRGBA[1] = static_cast<uint8_t>(pixelFloatRGBA[1]);
                    pixelRGBA[2] = static_cast<uint8_t>(pixelFloatRGBA[2]);
                    pixelRGBA[3] = static_cast<uint8_t>(pixelFloatRGBA[3]);
                }
            }
                        
            const int32_t invalidTabIndex(-1);
            const int32_t invalidOverlayIndex(-1);
            if (rowFlag) {
                const int64_t i4(iCol * 4);
                CaretAssertVectorIndex(rowRGBA, i4+3);
                rowRGBA[i4]   = pixelRGBA[0];
                rowRGBA[i4+1] = pixelRGBA[1];
                rowRGBA[i4+2] = pixelRGBA[2];
                rowRGBA[i4+3] = pixelRGBA[3];
            }
            else {
                outputImageFile->setPixelRGBA(invalidTabIndex,
                                              invalidOverlayIndex,
                                              pixelIndex,
                                              pixelRGBA.data());
            }
        } /* for iCol */
        
        if (rowFlag) {
            outputImageFile->setPixelRowRGBA(jRow,
                                             rowRGBA);
        }
    } /* for jRow */
    
    if (validPixelCounter <= 0) {
        errorMessageOut.appendWithNewLine("No intersection between image and volume");
    }
    
    return (validPixelCounter > 0);
    
}

