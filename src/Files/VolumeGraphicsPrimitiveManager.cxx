
/*LICENSE_START*/
/*
 *  Copyright (C) 2021 Washington University School of Medicine
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

#define __VOLUME_GRAPHICS_PRIMITIVE_MANAGER_DECLARE__
#include "VolumeGraphicsPrimitiveManager.h"
#undef __VOLUME_GRAPHICS_PRIMITIVE_MANAGER_DECLARE__

#include "CaretAssert.h"
#include "CaretLogger.h"
#include "CaretMappableDataFile.h"
#include "GraphicsPrimitiveV3fT2f.h"
#include "GraphicsPrimitiveV3fT3f.h"
#include "GraphicsUtilitiesOpenGL.h"
#include "ImageFile.h"
#include "VolumeMappableInterface.h"
#include "VolumeToImageMapping.h"

using namespace caret;


    
/**
 * \class caret::VolumeGraphicsPrimitiveManager
 * \brief Generates graphics primitives for drawing volumes using textures
 * \ingroup Files
 */

/**
 * Constructor.
 * @param mapDataFile
 *    The mappable data file
 * @param volumeInterface
 *    The file's volume interface
 */
VolumeGraphicsPrimitiveManager::VolumeGraphicsPrimitiveManager(CaretMappableDataFile* mapDataFile,
                                                               VolumeMappableInterface* volumeInterface)
: CaretObject(),
m_mapDataFile(mapDataFile),
m_volumeInterface(volumeInterface)
{
    CaretAssert(m_mapDataFile);
    CaretAssert(m_volumeInterface);
}

/**
 * Destructor.
 */
VolumeGraphicsPrimitiveManager::~VolumeGraphicsPrimitiveManager()
{
}

/**
 * Clear this instance
 */
void
VolumeGraphicsPrimitiveManager::clear()
{
    m_mapGraphicsTriangleFanPrimitives.clear();
    m_mapGraphicsTriangleStripPrimitives.clear();
    m_mapGraphicsTrianglesPrimitives.clear();
    m_mapIntersectionImageFiles.clear();
}

/**
 * Invalidate primitive for the given map
 * @param mapIndex
 *    Index of the map
 */
void
VolumeGraphicsPrimitiveManager::invalidateAllColoring()
{
    for (auto& p : m_mapGraphicsTriangleFanPrimitives) {
        p.reset();
    }
    for (auto& p : m_mapGraphicsTriangleStripPrimitives) {
        p.reset();
    }
    for (auto& p : m_mapGraphicsTrianglesPrimitives) {
        p.reset();
    }
    m_mapIntersectionImageFiles.clear();
}

/**
 * Invalidate primitive for the given map
 * @param mapIndex
 *    Index of the map
 */
void
VolumeGraphicsPrimitiveManager::invalidateColoringForMap(const int32_t mapIndex)
{
    if ((mapIndex >= 0)
        && (mapIndex < static_cast<int32_t>(m_mapGraphicsTriangleFanPrimitives.size()))) {
        m_mapGraphicsTriangleFanPrimitives[mapIndex].reset();
    }
    if ((mapIndex >= 0)
        && (mapIndex < static_cast<int32_t>(m_mapGraphicsTriangleStripPrimitives.size()))) {
        m_mapGraphicsTriangleStripPrimitives[mapIndex].reset();
    }
    if ((mapIndex >= 0)
        && (mapIndex < static_cast<int32_t>(m_mapGraphicsTrianglesPrimitives.size()))) {
        m_mapGraphicsTrianglesPrimitives[mapIndex].reset();
    }
    
    /*
     * Remove any items with key that contains map index
     * (1) Get the key while avoiding invalidating the iterator
     * (2) Remove items with keys
     */
    std::vector<ImageIntersectionKey> removeItemKeys;
    for (auto& m : m_mapIntersectionImageFiles) {
        if (m.first.m_mapIndex == mapIndex) {
            removeItemKeys.push_back(m.first);
        }
    }
    for (auto& key : removeItemKeys) {
        m_mapIntersectionImageFiles.erase(key);
    }
}

/**
 * Get the graphics primitive for drawing a volume's map
 *
 * @param primitiveShape
 *    Shape for primitive drawing
 * @param mapIndex
 *    Index of the map.
 * @param displayGroup
 *    The selected display group.
 * @param tabIndex
 *    Index of selected tab.
 * @return
 *    Graphics primitive or NULL if unable to draw
 */
GraphicsPrimitiveV3fT3f*
VolumeGraphicsPrimitiveManager::getVolumeDrawingPrimitiveForMap(const PrimitiveShape primitiveShape,
                                                                const int32_t mapIndex,
                                                                const DisplayGroupEnum::Enum displayGroup,
                                                                const int32_t tabIndex) const
{
    if (m_mapDataFile->getNumberOfMaps() != static_cast<int32_t>(m_mapGraphicsTriangleFanPrimitives.size())) {
        m_mapGraphicsTriangleFanPrimitives.resize(mapIndex + 1);
    }
    if (m_mapDataFile->getNumberOfMaps() != static_cast<int32_t>(m_mapGraphicsTriangleStripPrimitives.size())) {
        m_mapGraphicsTriangleStripPrimitives.resize(mapIndex + 1);
    }
    if (m_mapDataFile->getNumberOfMaps() != static_cast<int32_t>(m_mapGraphicsTrianglesPrimitives.size())) {
        m_mapGraphicsTrianglesPrimitives.resize(mapIndex + 1);
    }

    GraphicsPrimitiveV3fT3f* primitiveOut(NULL);
    switch (primitiveShape) {
        case PrimitiveShape::TRIANGLE_FAN:
            CaretAssertVectorIndex(m_mapGraphicsTriangleFanPrimitives, mapIndex);
            primitiveOut = m_mapGraphicsTriangleFanPrimitives[mapIndex].get();
            break;
        case PrimitiveShape::TRIANGLE_STRIP:
            CaretAssertVectorIndex(m_mapGraphicsTriangleStripPrimitives, mapIndex);
            primitiveOut = m_mapGraphicsTriangleStripPrimitives[mapIndex].get();
            break;
        case PrimitiveShape::TRIANGLES:
            CaretAssertVectorIndex(m_mapGraphicsTrianglesPrimitives, mapIndex);
            primitiveOut = m_mapGraphicsTrianglesPrimitives[mapIndex].get();
            break;
    }
    
    if (primitiveOut != NULL) {
        const VoxelColorUpdate* voxelColorUpdate(getVoxelColorUpdate(mapIndex));
        if (voxelColorUpdate != NULL) {
            if (voxelColorUpdate->isValid()) {
                /*
                 * Put the voxel color update in the graphics primitive
                 * that will get used next time the primitive is drawn
                 */
                primitiveOut->setVoxelColorUpdate(*voxelColorUpdate);
            }
            
            /*
             * Data has been used so reset it
             */
            resetVoxelColorUpdate(mapIndex);
        }
    }
    
    if (primitiveOut == NULL) {
        AString errorMessage;
        primitiveOut = VolumeGraphicsPrimitiveManager::createPrimitive(primitiveShape,
                                                                       mapIndex,
                                                                       displayGroup,
                                                                       tabIndex,
                                                                       errorMessage);
        if (primitiveOut != NULL) {
            switch (primitiveShape) {
                case PrimitiveShape::TRIANGLE_FAN:
                    CaretAssertVectorIndex(m_mapGraphicsTriangleFanPrimitives, mapIndex);
                    m_mapGraphicsTriangleFanPrimitives[mapIndex].reset(primitiveOut);
                    break;
                case PrimitiveShape::TRIANGLE_STRIP:
                    CaretAssertVectorIndex(m_mapGraphicsTriangleStripPrimitives, mapIndex);
                    m_mapGraphicsTriangleStripPrimitives[mapIndex].reset(primitiveOut);
                    break;
                case PrimitiveShape::TRIANGLES:
                    CaretAssertVectorIndex(m_mapGraphicsTrianglesPrimitives, mapIndex);
                    m_mapGraphicsTrianglesPrimitives[mapIndex].reset(primitiveOut);
                    break;
            }
        }
        else {
            CaretLogSevere(m_mapDataFile->getFileNameNoPath()
                           + errorMessage);
        }
    }
    
    return primitiveOut;
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
VolumeGraphicsPrimitiveManager::toString() const
{
    return "VolumeGraphicsPrimitiveManager";
}

/**
 * Generate a graphics primitive for drawing the volumes as RGBA or as identification
 * @param primitiveShape
 *    Shape for primitive drawing
 * @param mapIndex
 *    Map index for creating the primitive
 * @param displayGroup
 *    Display gtroup selected
 * @param tabIndex
 *    Index of tab
 * @param errorMessageOut
 *    Contains information if error occurs
 * @return
 *    Pointer to graphics primitive or NULL if failure
 */
GraphicsPrimitiveV3fT3f*
VolumeGraphicsPrimitiveManager::createPrimitive(const PrimitiveShape primitiveShape,
                                                const int32_t mapIndex,
                                                const DisplayGroupEnum::Enum displayGroup,
                                                const int32_t tabIndex,
                                                AString& errorMessageOut) const
{
    CaretAssert(m_volumeInterface);
    errorMessageOut.clear();
    
    std::vector<int64_t> dims(5);
    m_volumeInterface->getDimensions(dims);

    int64_t numberOfSlices(dims[2]);
    int64_t numberOfRows(dims[1]);
    int64_t numberOfColumns(dims[0]);

    const int64_t maxTextureSize(GraphicsUtilitiesOpenGL::getTextureDepthMaximumDimension());
    AString sizeMsg;
    if (numberOfColumns > maxTextureSize) {
        sizeMsg.appendWithNewLine("Width="
                                  + AString::number(numberOfColumns)
                                  + " is too big for 3D Texture.");
    }
    if (numberOfRows > maxTextureSize) {
        sizeMsg.appendWithNewLine("Height="
                                  + AString::number(numberOfRows)
                                  + " is too big for 3D Texture.");
    }
    if (numberOfSlices > maxTextureSize) {
        sizeMsg.appendWithNewLine("Slices="
                                  + AString::number(numberOfSlices)
                                  + " is too big for 3D Texture.");
    }
    if ( ! sizeMsg.isEmpty()) {
        sizeMsg.appendWithNewLine("Maximum texture dimension="
                                  + AString::number(maxTextureSize));
        errorMessageOut = sizeMsg;
        return NULL;
    }
    
    /*
     * Allocate storage for rgba data that is used by the graphics primitive
     */
    int64_t numberOfTextureBytes(0);
    std::shared_ptr<uint8_t> imageRgbaData = GraphicsTextureSettings::allocateImageRgbaData(numberOfColumns,
                                                                                            numberOfRows,
                                                                                            numberOfSlices,
                                                                                            &numberOfTextureBytes);
    uint8_t* imageRgbaPtr(imageRgbaData.get()); /* simplify access for loading */
    
    const int64_t numSliceBytes = (numberOfRows * numberOfColumns * 4);
    std::vector<uint8_t> rgbaSlice(numSliceBytes);

    for (int64_t k = 0; k < numberOfSlices; k++) {
        int64_t firstVoxelIJK[3] = { 0, 0, k };
        int64_t rowStepIJK[3] = { 0, 1, 0 };
        int64_t columnStepIJK[3] = { 1, 0, 0 };
        
        std::fill(rgbaSlice.begin(), rgbaSlice.end(), 0);
        m_volumeInterface->getVoxelColorsForSliceInMap(mapIndex,
                                                     firstVoxelIJK,
                                                     rowStepIJK,
                                                     columnStepIJK,
                                                     numberOfRows,
                                                     numberOfColumns,
                                                     displayGroup,
                                                     tabIndex,
                                                     &rgbaSlice[0]);
        
        for (int64_t j = 0; j < numberOfRows; j++) {
            for (int64_t i = 0; i < numberOfColumns; i++) {
                const int64_t sliceOffset = ((j * numberOfColumns) + i) * 4;
                const int64_t textureOffset = ((k * numberOfColumns * numberOfRows)
                                               + (j * numberOfColumns) + i) * 4;
                for (int64_t m = 0; m < 4; m++) {
                    CaretAssertArrayIndex(imageRgbaPtr, numberOfTextureBytes, (textureOffset + m));
                    CaretAssertVectorIndex(rgbaSlice, sliceOffset + m);
                    imageRgbaPtr[textureOffset + m] = rgbaSlice[sliceOffset + m];
                }
            }
        }
    }

    const bool useMipMaps(true);
    GraphicsTextureSettings::MipMappingType mipMap(GraphicsTextureSettings::MipMappingType::DISABLED);
    GraphicsTextureMagnificationFilterEnum::Enum magFilter(GraphicsTextureMagnificationFilterEnum::LINEAR);
    GraphicsTextureMinificationFilterEnum::Enum minFilter(GraphicsTextureMinificationFilterEnum::LINEAR);
    if (useMipMaps) {
        mipMap    = GraphicsTextureSettings::MipMappingType::ENABLED;
        minFilter = GraphicsTextureMinificationFilterEnum::LINEAR_MIPMAP_LINEAR;
        
        /*
         * 31 March 2023 Disable mip maps until I (John H) can test on Linux and Windows
         */
        mipMap    = GraphicsTextureSettings::MipMappingType::DISABLED;
        minFilter = GraphicsTextureMinificationFilterEnum::LINEAR;
    }
    
    if (m_mapDataFile->isMappedWithRGBA()
        || m_mapDataFile->isMappedWithLabelTable()) {
        mipMap    = GraphicsTextureSettings::MipMappingType::DISABLED;
        magFilter  = GraphicsTextureMagnificationFilterEnum::NEAREST;
        minFilter  = GraphicsTextureMinificationFilterEnum::NEAREST;
    }
    std::array<float, 4> backgroundColor { 0.0, 0.0, 0.0, 0.0 };
    GraphicsTextureSettings textureSettings(imageRgbaData,
                                            numberOfColumns,
                                            numberOfRows,
                                            numberOfSlices,
                                            GraphicsTextureSettings::DimensionType::FLOAT_STR_3D,
                                            GraphicsTextureSettings::PixelFormatType::RGBA,
                                            GraphicsTextureSettings::PixelOrigin::BOTTOM_LEFT,
                                            GraphicsTextureSettings::WrappingType::CLAMP_TO_BORDER,
                                            mipMap,
                                            GraphicsTextureSettings::CompressionType::DISABLED,
                                            magFilter,
                                            minFilter,
                                            backgroundColor);
    
    GraphicsPrimitive::PrimitiveType primType(GraphicsPrimitive::PrimitiveType::OPENGL_TRIANGLE_STRIP);
    int32_t numVertices(0);
    switch (primitiveShape) {
        case PrimitiveShape::TRIANGLE_FAN:
            primType = GraphicsPrimitive::PrimitiveType::OPENGL_TRIANGLE_FAN;
            numVertices = 8;
            break;
        case PrimitiveShape::TRIANGLE_STRIP:
            primType = GraphicsPrimitive::PrimitiveType::OPENGL_TRIANGLE_STRIP;
            numVertices = 4;
            break;
        case PrimitiveShape::TRIANGLES:
            primType = GraphicsPrimitive::PrimitiveType::OPENGL_TRIANGLES;
            numVertices = 18;
            break;
    }

    GraphicsPrimitiveV3fT3f* primitiveOut(GraphicsPrimitive::newPrimitiveV3fT3f(primType,
                                                                                textureSettings));
    CaretAssert(primitiveOut);
    
    /*
     * Need four vertices and texture coordinates but their values do no matter here.
     * They will be replaced when a volume slice is drawn.
     */
    const float xyz[3] { 0.0, 0.0, 0.0 };
    const float str[3] { 0.0, 0.0, 0.0 };
    for (int32_t i = 0; i < numVertices; i++) {
        primitiveOut->addVertex(xyz, str);
    }

    return primitiveOut;
}

/**
 * Generate a graphics primitive for an image intersection with the volume
 * 
 * @param mediaFile
 *    Media file for intersection
 * @param mapIndex
 *    Map index for creating the primitive
 * @param displayGroup
 *    Display gtroup selected
 * @param tabIndex
 *    Index of tab
 * @param errorMessageOut
 *    Contains error information if failure to create primitive
 * @return
 *    Pointer to graphics primitive or NULL if failure
 */
GraphicsPrimitiveV3fT2f*
VolumeGraphicsPrimitiveManager::getImageIntersectionDrawingPrimtiveForMap(const MediaFile* mediaFile,
                                                                          const int32_t mapIndex,
                                                                          const DisplayGroupEnum::Enum displayGroup,
                                                                          const int32_t tabIndex,
                                                                          AString& errorMessageOut) const
{
    errorMessageOut.clear();
    
    GraphicsPrimitiveV3fT2f* primitiveOut(NULL);
    
    ImageIntersectionKey key(const_cast<MediaFile*>(mediaFile),
                             mapIndex,
                             tabIndex);
    
    ImageFile* imageFile(NULL);
    auto iter(m_mapIntersectionImageFiles.find(key));
    if (iter != m_mapIntersectionImageFiles.end()) {
        imageFile = iter->second.get();
    }
    else {
        VolumeToImageMapping mapper(m_volumeInterface,
                                    mapIndex,
                                    displayGroup,
                                    tabIndex,
                                    mediaFile);
        if (mapper.runMapping(errorMessageOut)) {
            imageFile = mapper.takeOutputImageFile();
            m_mapIntersectionImageFiles.insert(std::make_pair(key,
                                                              imageFile));
        }
    }
    
    if (imageFile != NULL) {
        int32_t invalidOverlayIndex(-1);
        primitiveOut = imageFile->getGraphicsPrimitiveForPlaneXyzDrawing(tabIndex,
                                                                         invalidOverlayIndex);
    }
    
    return primitiveOut;
}

/**
 * Update the voxel coloring for the given voxels in the given map with the given RGBA coloring.
 * This method is used by voxel editing to avoid recoloring all voxels in the volume
 * @param voxelColorUpdate
 *    Information about the color update
 */
void
VolumeGraphicsPrimitiveManager::updateVoxelColorsInMapTexture(const VoxelColorUpdate& voxelColorUpdate)
{
    const int32_t mapIndex(voxelColorUpdate.getMapIndex());
    
    updateNumberOfVoxelColorUpdates(mapIndex);
    CaretAssertVectorIndex(m_voxelColorUpdates, mapIndex);
    m_voxelColorUpdates[mapIndex] = voxelColorUpdate;
}

/**
 * @return The voxel color update at the given index
 * @param mapIndex
 *    index of the map
 */
const VoxelColorUpdate*
VolumeGraphicsPrimitiveManager::getVoxelColorUpdate(const int32_t mapIndex) const
{
    updateNumberOfVoxelColorUpdates(mapIndex);
    CaretAssertVectorIndex(m_voxelColorUpdates, mapIndex);
    return &m_voxelColorUpdates[mapIndex];
}

/**
 * Reset (invalidate) the voxel color update at the given index
 * @param mapIndex
 *    index of the map
 */
void
VolumeGraphicsPrimitiveManager::resetVoxelColorUpdate(const int32_t mapIndex) const
{
    updateNumberOfVoxelColorUpdates(mapIndex);
    CaretAssertVectorIndex(m_voxelColorUpdates, mapIndex);
    m_voxelColorUpdates[mapIndex].clear();
}

/**
 * Update the number of voxel color updates so that it is valid for the given map index
 * @param mapIndex
 *    index of the map
 */
void
VolumeGraphicsPrimitiveManager::updateNumberOfVoxelColorUpdates(const int32_t mapIndex) const
{
    if ((mapIndex + 1) > static_cast<int32_t>(m_voxelColorUpdates.size())) {
        m_voxelColorUpdates.resize(mapIndex + 1);
    }
}



