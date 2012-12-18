
/*LICENSE_START*/
/*
 * Copyright 2012 Washington University,
 * All rights reserved.
 *
 * Connectome DB and Connectome Workbench are part of the integrated Connectome 
 * Informatics Platform.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    * Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *    * Neither the names of Washington University nor the
 *      names of its contributors may be used to endorse or promote products
 *      derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR  
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
 * SUBSTITUTE GOODS OR  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
 * POSSIBILITY OF SUCH DAMAGE.
 */
/*LICENSE_END*/

#define __VOLUME_FILE_VOXEL_COLORIZER_DECLARE__
#include "VolumeFileVoxelColorizer.h"
#undef __VOLUME_FILE_VOXEL_COLORIZER_DECLARE__

#include "CaretAssert.h"
#include "CaretLogger.h"
#include "ElapsedTimer.h"
#include "NodeAndVoxelColoring.h"
#include "VolumeFile.h"

using namespace caret;


    
/**
 * \class caret::VolumeFileVoxelColorizer 
 * \brief Delegate for coloring a volumes voxels.
 */

/**
 * Constructor.
 *
 * @param volumeFile
 *    Volume file on which this instance colors voxels.
 */
VolumeFileVoxelColorizer::VolumeFileVoxelColorizer(VolumeFile* volumeFile)
: CaretObject()
{
    CaretAssert(volumeFile);
    
    m_volumeFile = volumeFile;
    
    int64_t dimNumberOfComponents;
    m_volumeFile->getDimensions(m_dimI,
                                m_dimJ,
                                m_dimK,
                                m_mapCount,
                                dimNumberOfComponents);
    
    m_voxelCountPerMap = m_dimI * m_dimJ * m_dimK;
    m_mapRGBACount = m_voxelCountPerMap * 4;
    
    for (int64_t i = 0; i < m_mapCount; i++) {
        m_mapRGBA.push_back(new uint8_t[m_mapRGBACount]);
        m_mapColoringValid.push_back(false);
    }
}

/**
 * Destructor.
 */
VolumeFileVoxelColorizer::~VolumeFileVoxelColorizer()
{
    for (int64_t i = 0; i < m_mapCount; i++) {
        delete[] m_mapRGBA[i];
    }
    m_mapRGBA.clear();
}

/**
 * Assign voxel coloring for a map.
 *
 * @param mapIndex
 *     Index of map.
 * @param palette  
 *     Palette used for scalar color assignment.  May be NULL for data
 *     not mapped with a palette.
 * @param thresholdVolume
 *     Volume that contains thresholding (if NULL indicates no thresholding).
 * @param thresholdVolumeMapIndex
 *     Index of map in thresholding volume.
 */
void
VolumeFileVoxelColorizer::assignVoxelColorsForMap(const int32_t mapIndex,
                                                  const Palette* palette,
                                                  const VolumeFile* thresholdVolume,
                                                  const int32_t thresholdVolumeMapIndex)
{
    CaretAssertVectorIndex(m_mapRGBA, mapIndex);
    
    ElapsedTimer timer;
    timer.start();
    
    /*
     * Pointer to map's data 
     */
    float* mapDataPointer = (m_volumeFile->m_data
                             + (m_voxelCountPerMap * mapIndex));
    
    /*
     * Get access to threshold data
     */
    float* thresholdDataPointer = NULL;
    bool ignoreThresholding = true;
    if (thresholdVolume != NULL) {
        int64_t threshI, threshJ, threshK, threshMapCount, threshNumberOfComponents;
        thresholdVolume->getDimensions(threshI,
                                       threshJ,
                                       threshK,
                                       threshMapCount,
                                       threshNumberOfComponents);
        if ((threshI != m_dimI)
            || (threshJ != m_dimJ)
            || (threshK != m_dimK)) {
            CaretLogSevere("Threshold volume ("
                           + thresholdVolume->getFileNameNoPath()
                           + ") dimensions do not match "
                           + m_volumeFile->getFileNameNoPath());
        }
        else {
            /*
             * Can use same voxel counter per map since volumes are
             * identical dimensions;
             */
            thresholdDataPointer = thresholdVolume->m_data + m_voxelCountPerMap;
            ignoreThresholding = false;
        }
    }
    
    switch (m_volumeFile->getType()) {
        case SubvolumeAttributes::UNKNOWN:
        case SubvolumeAttributes::ANATOMY:
        case SubvolumeAttributes::FUNCTIONAL:
            CaretAssert(palette);
            NodeAndVoxelColoring::colorScalarsWithPaletteParallel(m_volumeFile->getMapFastStatistics(mapIndex),
                                                          m_volumeFile->getMapPaletteColorMapping(mapIndex),
                                                          palette,
                                                          mapDataPointer,
                                                          mapDataPointer,
                                                          m_voxelCountPerMap,
                                                          m_mapRGBA[mapIndex],
                                                          ignoreThresholding);
            m_mapColoringValid[mapIndex] = true;
            break;
        case SubvolumeAttributes::LABEL:
            if (m_voxelCountPerMap > 0) {
                std::vector<int32_t> labelIndices(m_voxelCountPerMap);
                for (int32_t i = 0; i < m_voxelCountPerMap; i++) {
                    labelIndices[i] = static_cast<int32_t>(mapDataPointer[i]);
                }
                
                NodeAndVoxelColoring::colorIndicesWithLabelTable(m_volumeFile->getMapLabelTable(mapIndex),
                                                                 &labelIndices[0],
                                                                 m_voxelCountPerMap,
                                                                 m_mapRGBA[mapIndex]);
                m_mapColoringValid[mapIndex] = true;
            }
            break;
        case SubvolumeAttributes::RGB:
            break;
        case SubvolumeAttributes::SEGMENTATION:
            break;
        case SubvolumeAttributes::VECTOR:
            break;
    }
    
    CaretLogFine("Time to color map named \""
                   + m_volumeFile->getMapName(mapIndex)
                   + " in volume file "
                   + m_volumeFile->getFileNameNoPath()
                   + " was "
                   + AString::number(timer.getElapsedTimeMilliseconds())
                   + " milliseconds");
}

/**
 * Assign voxel coloring for a map in the background.  This method will
 * launch the assignment of voxel coloring in a separate thread and then
 * return.
 *
 * @param mapIndex
 *     Index of map.
 * @param palette
 *     Palette used for scalar color assignment.
 * @param thresholdVolume
 *     Volume that contains thresholding (if NULL indicates no thresholding).
 * @param thresholdVolumeMapIndex
 *     Index of map in thresholding volume.
 */
void
VolumeFileVoxelColorizer::assignVoxelColorsForMapInBackground(const int32_t mapIndex,
                                                              const Palette* palette,
                                                              const VolumeFile* thresholdVolume,
                                                              const int32_t thresholdVolumeMapIndex)
{
    assignVoxelColorsForMap(mapIndex,
                            palette,
                            thresholdVolume,
                            thresholdVolumeMapIndex);
}

/**
 * Invalidate the RGBA coloring for all maps.
 */
void
VolumeFileVoxelColorizer::invalidateColoring()
{
    std::fill(m_mapColoringValid.begin(),
              m_mapColoringValid.end(),
              false);
}

/**
 * Get voxel coloring for a slice in a map.  If voxel coloring is not ready
 * (it may be running in a different thread) this method will wait until the 
 * coloring is valid prior to returning the slice's coloring.
 *
 * @param mapIndex
 *     Index of map.
 * @param slicePlane
 *    Plane of the slice.
 * @param sliceIndex
 *    Index of the slice.
 * @param rgbaOut
 *    RGBA color components out.
 */
void
VolumeFileVoxelColorizer::getVoxelColorsForSliceInMap(const int32_t mapIndex,
                                                      const VolumeSliceViewPlaneEnum::Enum slicePlane,
                                                      const int64_t sliceIndex,
                                                      uint8_t* rgbaOut) const
{
    CaretAssertVectorIndex(m_mapRGBA, mapIndex);
    CaretAssert(sliceIndex >= 0);
    CaretAssert(rgbaOut);
    
    int64_t iStart = 0;
    int64_t iEnd   = m_dimI - 1;
    int64_t jStart = 0;
    int64_t jEnd   = m_dimJ - 1;
    int64_t kStart = 0;
    int64_t kEnd   = m_dimK - 1;
    switch (slicePlane) {
        case VolumeSliceViewPlaneEnum::ALL:
            CaretAssert(0);
            break;
        case VolumeSliceViewPlaneEnum::AXIAL:
            kStart = sliceIndex;
            kEnd   = sliceIndex;
            break;
        case VolumeSliceViewPlaneEnum::CORONAL:
            jStart = sliceIndex;
            jEnd   = sliceIndex;
            break;
        case VolumeSliceViewPlaneEnum::PARASAGITTAL:
            iStart = sliceIndex;
            iEnd   = sliceIndex;
            break;
    }

    /*
     * Pointer to maps RGBA values
     */
    const uint8_t* mapRGBA = m_mapRGBA[mapIndex];
    
    /*
     * Output RGBA values for slice
     */
    int64_t rgbaOutIndex = 0;
    for (int64_t k = kStart; k <= kEnd; k++) {
        for (int64_t j = jStart; j <= jEnd; j++) {
            for (int64_t i = iStart; i <= iEnd; i++) {
                int64_t rgbaOffset = -1;
//                switch (slicePlane) {
//                    case VolumeSliceViewPlaneEnum::ALL:
//                        CaretAssert(0);
//                        break;
//                    case VolumeSliceViewPlaneEnum::AXIAL:
//                        rgbaOffset = (i + (j * m_dimI));
//                        break;
//                    case VolumeSliceViewPlaneEnum::CORONAL:
//                        rgbaOffset = (i + (k * m_dimI));
//                        break;
//                    case VolumeSliceViewPlaneEnum::PARASAGITTAL:
//                        rgbaOffset = (j + (k * m_dimJ));
//                        break;
//                }
                
                rgbaOffset = m_volumeFile->getIndex(i,
                                                    j,
                                                    k,
                                                    mapIndex);
                rgbaOffset *= 4;
                CaretAssertArrayIndex(mapRGBA, m_mapRGBACount, rgbaOffset);
                rgbaOut[rgbaOutIndex]   = mapRGBA[rgbaOffset];
                rgbaOut[rgbaOutIndex+1] = mapRGBA[rgbaOffset+1];
                rgbaOut[rgbaOutIndex+2] = mapRGBA[rgbaOffset+2];
                rgbaOut[rgbaOutIndex+3] = mapRGBA[rgbaOffset+3];
                rgbaOutIndex += 4;
            }
        }
    }
}

/**
 * Get the RGBA color components for voxel.
 *
 * @param i
 *    Parasaggital index
 * @param j
 *    Coronal index
 * @param k
 *    Axial index
 * @param mapIndex
 *    Index of map.
 * @param rgbaOut
 *    Contains voxel coloring on exit.
 */
void
VolumeFileVoxelColorizer::getVoxelColorInMap(const int64_t i,
                               const int64_t j,
                               const int64_t k,
                               const int64_t mapIndex,
                               uint8_t rgbaOut[4]) const
{
    /*
     * Pointer to maps RGBA values
     */
    CaretAssertVectorIndex(m_mapRGBA, mapIndex);
    const uint8_t* mapRGBA = m_mapRGBA[mapIndex];
    int64_t rgbaOffset = m_volumeFile->getIndex(i,
                                        j,
                                        k,
                                        mapIndex);
    rgbaOffset *= 4;
    CaretAssertArrayIndex(mapRGBA, m_mapRGBACount, rgbaOffset);
    rgbaOut[0] = mapRGBA[rgbaOffset];
    rgbaOut[1] = mapRGBA[rgbaOffset+1];
    rgbaOut[2] = mapRGBA[rgbaOffset+2];
    rgbaOut[3] = mapRGBA[rgbaOffset+3];    
}

/**
 * Clear the voxel coloring for the given map.
 * @param mapIndex
 *    Index of map.
 */
void
VolumeFileVoxelColorizer::clearVoxelColoringForMap(const int64_t mapIndex)
{
    CaretAssertVectorIndex(m_mapRGBA, mapIndex);
    uint8_t* mapRGBA = m_mapRGBA[mapIndex];
    
    for (int64_t i = 0; i < m_mapRGBACount; i++) {
        mapRGBA[i] = 0.0;
    }
}

/**
 * Set the RGBA coloring for a voxel in a map.
 *
 * @param i
 *    Parasaggital index
 * @param j
 *    Coronal index
 * @param k
 *    Axial index
 * @param mapIndex
 *    Index of map.
 * @param rgbaFloat
 *    RGBA color components for voxel.
 */
void
VolumeFileVoxelColorizer::setVoxelColorInMap(const int64_t i,
                                             const int64_t j,
                                             const int64_t k,
                                             const int64_t mapIndex,
                                             const float rgbaFloat[4])

{
    /*
     * Pointer to maps RGBA values
     */
    CaretAssertVectorIndex(m_mapRGBA, mapIndex);
    uint8_t* mapRGBA = m_mapRGBA[mapIndex];
    int64_t rgbaOffset = m_volumeFile->getIndex(i,
                                                j,
                                                k,
                                                mapIndex);
    rgbaOffset *= 4;
    CaretAssertArrayIndex(mapRGBA, m_mapRGBACount, rgbaOffset);
    mapRGBA[rgbaOffset]   = static_cast<uint8_t>(rgbaFloat[0] * 255.0);
    mapRGBA[rgbaOffset+1] = static_cast<uint8_t>(rgbaFloat[1] * 255.0);
    mapRGBA[rgbaOffset+2] = static_cast<uint8_t>(rgbaFloat[2] * 255.0);
    float alpha = rgbaFloat[3];
    if (alpha < 0.0) {
        alpha = 0.0;
    }
    mapRGBA[rgbaOffset+3] = static_cast<uint8_t>(alpha * 255.0);
}
