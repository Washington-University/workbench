
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
 * @param displayGroup
 *    The display group
 * @param tabIndex
 *    Index of the tab
 * @param inputMediaFile
 *    Image file used for spatial information
 */
VolumeToImageMapping::VolumeToImageMapping(const VolumeMappableInterface* volumeInterface,
                                           const int32_t volumeFileMapIndex,
                                           const DisplayGroupEnum::Enum displayGroup,
                                           const int32_t tabIndex,
                                           const MediaFile* inputMediaFile)
: CaretObject(),
m_volumeInterface(volumeInterface),
m_volumeFileMapIndex(volumeFileMapIndex),
m_displayGroup(displayGroup),
m_tabIndex(tabIndex),
m_inputMediaFile(inputMediaFile)
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
    if (validateInputs(errorMessageOut)) {
        if (performMapping(errorMessageOut)) {
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
        }
    }
    //std::cout << "Time to map volume to image: " << timer.getElapsedTimeMilliseconds() << "ms" << std::endl;
    
    return successFlag;
}

/**
 * @return True if the inputs are valid, else false.
 * @param errorMessageOut
 *   In inputs are not valid, contains error information
 */
bool
VolumeToImageMapping::performMapping(AString& errorMessageOut)
{
    errorMessageOut.clear();
    
    const ImageFile* inputImageFile(m_inputMediaFile->castToImageFile());
    if (inputImageFile == NULL) {
        errorMessageOut = ("Mapping volume to image supports only image files for "
                           + m_inputMediaFile->getFileName());
        return false;
    }
        
    m_outputImageFile.reset(new ImageFile(*inputImageFile));
    m_outputImageFile->setFileName("VolumeMapping.png");
        
    if ( ! validateMediaFile(m_outputImageFile.get(),
                             "Output image file copied from input image file ",
                             errorMessageOut)) {
        return false;
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
    
    int32_t unassignedLabelKey(-1);
    if (m_volumeMappableDataFile->isMappedWithLabelTable()) {
        const GiftiLabelTable* labelTable = m_volumeMappableDataFile->getMapLabelTable(m_volumeFileMapIndex);
        CaretAssert(labelTable);
        
        unassignedLabelKey = labelTable->getUnassignedLabelKey();
    }
    
    const int64_t imageWidth(m_outputImageFile->getWidth());
    const int64_t imageHeight(m_outputImageFile->getHeight());
    
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
            if (m_outputImageFile->pixelIndexToStereotaxicXYZ(pixelIndex,
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
                m_outputImageFile->setPixelRGBA(invalidTabIndex,
                                                invalidOverlayIndex,
                                                pixelIndex,
                                                pixelRGBA.data());
            }
        } /* for iCol */
        
        if (rowFlag) {
            m_outputImageFile->setPixelRowRGBA(jRow,
                                               rowRGBA);
        }
    } /* for jRow */
    
    return (validPixelCounter > 0);
}

/**
 * @return True if the inputs are valid, else false.
 * @param errorMessageOut
 *   In inputs are not valid, contains error information
 */
bool
VolumeToImageMapping::validateInputs(AString& errorMessageOut)
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
        
    if (validateMediaFile(m_inputMediaFile,
                          "Input media file ",
                          errorMessageOut)) {
    }
    
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
 * @return Image file that was created by the mapping process.
 * This "takes" the image file that was created and caller is responsible
 * for destroying the returned image file.  NULL is returned if the
 * mapping process failed or if this method is called more than one time.
 */
ImageFile*
VolumeToImageMapping::takeOutputImageFile()
{
    ImageFile* file(NULL);
    if (m_outputImageFile) {
        file = m_outputImageFile.release();
    }
    return file;
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

