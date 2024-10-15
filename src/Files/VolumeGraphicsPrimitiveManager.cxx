
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
#include "HistologySlice.h"
#include "ImageFile.h"
#include "TabDrawingInfo.h"
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
    clear();
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
    clearIntersectionImagePrimitives();
}

/**
 * Clear the image intersection primitives
 */
void
VolumeGraphicsPrimitiveManager::clearIntersectionImagePrimitives()
{
    for (auto& mif : m_mapIntersectionImageFiles) {
        std::vector<ImageFile*>& imageFiles = mif.second;
        for (auto& image : imageFiles) {
            CaretAssert(image);
            delete image;
        }
    }
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
    m_mapGraphicsTrianglesPrimitives.clear();
    m_mapGraphicsTriangleFanPrimitives.clear();
    m_mapGraphicsTriangleStripPrimitives.clear();
//    for (auto& p : m_mapGraphicsTriangleFanPrimitives) {
//        p.reset();
//    }
//    for (auto& p : m_mapGraphicsTriangleStripPrimitives) {
//        p.reset();
//    }
//    for (auto& p : m_mapGraphicsTrianglesPrimitives) {
//        p.reset();
//    }
    clearIntersectionImagePrimitives();
}

/**
 * Invalidate primitive for the given map
 * @param mapIndex
 *    Index of the map
 */
void
VolumeGraphicsPrimitiveManager::invalidateColoringForMap(const int32_t mapIndex)
{
    {
        std::vector<PrimitiveKey> removeKeys;
        for (auto& m : m_mapGraphicsTriangleFanPrimitives) {
            if (m.first.m_mapIndex == mapIndex)
                removeKeys.push_back(m.first);
        }
        for (auto key : removeKeys) {
            m_mapGraphicsTriangleFanPrimitives.erase(key);
        }
    }
    
    {
        std::vector<PrimitiveKey> removeKeys;
        for (auto& m : m_mapGraphicsTriangleStripPrimitives) {
            if (m.first.m_mapIndex == mapIndex)
                removeKeys.push_back(m.first);
        }
        for (auto key : removeKeys) {
            m_mapGraphicsTriangleStripPrimitives.erase(key);
        }
    }
    
    {
        std::vector<PrimitiveKey> removeKeys;
        for (auto& m : m_mapGraphicsTrianglesPrimitives) {
            if (m.first.m_mapIndex == mapIndex)
                removeKeys.push_back(m.first);
        }
        for (auto key : removeKeys) {
            m_mapGraphicsTrianglesPrimitives.erase(key);
        }
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
            std::vector<ImageFile*>& imageFiles = m.second;
            for (auto& image : imageFiles) {
                CaretAssert(image);
                delete image;
            }
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
 * @param tabDrawingInfo
 *    Info for drawing tab.
 * @return
 *    Graphics primitive or NULL if unable to draw
 */
GraphicsPrimitiveV3fT3f*
VolumeGraphicsPrimitiveManager::getVolumeDrawingPrimitiveForMap(const PrimitiveShape primitiveShape,
                                                                const int32_t mapIndex,
                                                                const TabDrawingInfo& tabDrawingInfo) const
{
    /*
     * Each map in a volume requires its own primitive since each map is
     * colored differently.
     *
     * For label volume types: Display of particular labels is performed in each tab
     * and may be unique for each tab.  Therefore if this is a label volume,
     * we must also have a unique primitive for each tab.
     *
     * For all other volume types: Coloring is the same in all tabs so use
     * 'tabZeroIndex' for this volume types.
     */
    const bool labelVolumeFlag(tabDrawingInfo.getMapFile()->isMappedWithLabelTable());
    const int32_t tabZeroIndex(0);
    PrimitiveKey key(tabDrawingInfo.getMapIndex(),
                     (labelVolumeFlag ? tabDrawingInfo.getTabIndex() : tabZeroIndex));
    
    GraphicsPrimitiveV3fT3f* primitiveOut(NULL);
    switch (primitiveShape) {
        case PrimitiveShape::TRIANGLE_FAN:
        {
            auto iter(m_mapGraphicsTriangleFanPrimitives.find(key));
            if (iter != m_mapGraphicsTriangleFanPrimitives.end()) {
                primitiveOut = iter->second.get();
            }
        }
            break;
        case PrimitiveShape::TRIANGLE_STRIP:
        {
            auto iter(m_mapGraphicsTriangleStripPrimitives.find(key));
            if (iter != m_mapGraphicsTriangleStripPrimitives.end()) {
                primitiveOut = iter->second.get();
            }
        }
            break;
        case PrimitiveShape::TRIANGLES:
        {
            auto iter(m_mapGraphicsTrianglesPrimitives.find(key));
            if (iter != m_mapGraphicsTrianglesPrimitives.end()) {
                primitiveOut = iter->second.get();
            }
        }
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
                                                                       tabDrawingInfo,
                                                                       errorMessage);
        if (primitiveOut != NULL) {
            std::unique_ptr<GraphicsPrimitiveV3fT3f> ptr(primitiveOut);
            switch (primitiveShape) {
                case PrimitiveShape::TRIANGLE_FAN:
                    m_mapGraphicsTriangleFanPrimitives.insert(std::make_pair(key,
                                                                             std::move(ptr)));
                    break;
                case PrimitiveShape::TRIANGLE_STRIP:
                    m_mapGraphicsTriangleStripPrimitives.insert(std::make_pair(key,
                                                                               std::move(ptr)));
                    break;
                case PrimitiveShape::TRIANGLES:
                    m_mapGraphicsTrianglesPrimitives.insert(std::make_pair(key,
                                                                           std::move(ptr)));
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
 * @param tabDrawingInfo
 *    Info for drawing tab.
 * @param errorMessageOut
 *    Contains information if error occurs
 * @return
 *    Pointer to graphics primitive or NULL if failure
 */
GraphicsPrimitiveV3fT3f*
VolumeGraphicsPrimitiveManager::createPrimitive(const PrimitiveShape primitiveShape,
                                                const int32_t mapIndex,
                                                const TabDrawingInfo& tabDrawingInfo,
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
                                                     tabDrawingInfo,
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
 * @param tabDrawingInfo
 *    Info for drawing tab.
 * @param volumeMappingMode
 *    Mode for volume mapping
 * @param volumeSliceThickness
 *    Thickness of volume slice for some modes
 * @param errorMessageOut
 *    Contains error information if failure to create primitive
 * @return
 *    Pointer to graphics primitive or NULL if failure
 */
GraphicsPrimitiveV3fT2f*
VolumeGraphicsPrimitiveManager::getImageIntersectionDrawingPrimitiveForMap(const MediaFile* mediaFile,
                                                                           const int32_t mapIndex,
                                                                           const TabDrawingInfo& tabDrawingInfo,
                                                                           const VolumeToImageMappingModeEnum::Enum volumeMappingMode,
                                                                           const float volumeSliceThickness,
                                                                           AString& errorMessageOut) const
{
    CaretLogSevere("This function is no longer used.  Use method that takes HistlogySlice.");
    CaretAssertToDoFatal();
    
    errorMessageOut.clear();
    
    GraphicsPrimitiveV3fT2f* primitiveOut(NULL);
    
    ImageIntersectionKey key((void*)mediaFile,
                             mapIndex,
                             tabDrawingInfo.getTabIndex(),
                             volumeMappingMode,
                             volumeSliceThickness);
    std::cout << "Slice thickness1: " << volumeSliceThickness << std::endl;
    
    std::vector<ImageFile*> allImageFiles;
    auto iter(m_mapIntersectionImageFiles.find(key));
    if (iter != m_mapIntersectionImageFiles.end()) {
        const auto& imageFilesVector = iter->second;
        const int32_t numImages(imageFilesVector.size());
        for (int32_t i = 0; i < numImages; i++) {
            CaretAssertVectorIndex(imageFilesVector, i);
            allImageFiles.push_back(imageFilesVector[i]);
        }
    }
    else {
        VolumeToImageMapping mapper(m_volumeInterface,
                                    mapIndex,
                                    volumeMappingMode,
                                    volumeSliceThickness,
                                    tabDrawingInfo.getDisplayGroup(),
                                    tabDrawingInfo.getTabIndex(),
                                    mediaFile);
        if (mapper.runMapping(errorMessageOut)) {
            const int32_t numImageFiles(mapper.getNumberOfOutputImageFiles());
            for (int32_t i = 0; i < numImageFiles; i++) {
                ImageFile* imageFile(mapper.takeOutputImageFile(i));
                CaretAssert(imageFile);
                allImageFiles.push_back(imageFile);
            }
        }
        m_mapIntersectionImageFiles.insert(std::make_pair(key,
                                                          allImageFiles));
    }
    
    if ( ! allImageFiles.empty()) {
        for (auto& imageFile : allImageFiles) {
            int32_t invalidOverlayIndex(-1);
            primitiveOut = imageFile->getGraphicsPrimitiveForPlaneXyzDrawing(tabDrawingInfo.getTabIndex(),
                                                                             invalidOverlayIndex);
        }
    }
    
    return primitiveOut;
}

/**
 * Generate a graphics primitive for an image intersection with the volume
 *
 * @param histologySlice
 *    Histology slice for intersection
 * @param mapIndex
 *    Map index for creating the primitive
 * @param tabDrawingInfo
 *    Info for drawing tab.
 * @param volumeMappingMode
 *    Mode for volume mapping
 * @param volumeSliceThickness
 *    Thickness of volume slice for some modes
 * @param errorMessageOut
 *    Contains error information if failure to create primitive
 * @return
 *    Pointer to graphics primitive or NULL if failure
 */
std::vector<GraphicsPrimitive*>
VolumeGraphicsPrimitiveManager::getImageIntersectionDrawingPrimitiveForMap(const HistologySlice* histologySlice,
                                                                           const int32_t mapIndex,
                                                                           const TabDrawingInfo& tabDrawingInfo,
                                                                           const VolumeToImageMappingModeEnum::Enum volumeMappingMode,
                                                                           const float volumeSliceThickness,
                                                                           AString& errorMessageOut) const
{
    errorMessageOut.clear();
    
    std::vector<GraphicsPrimitive*> primitivesOut;
    
    ImageIntersectionKey key((void*)histologySlice,
                             mapIndex,
                             tabDrawingInfo.getTabIndex(),
                             volumeMappingMode,
                             volumeSliceThickness);

    std::vector<ImageFile*> allImageFiles;
    auto iter(m_mapIntersectionImageFiles.find(key));
    if (iter != m_mapIntersectionImageFiles.end()) {
        const auto& imageFilesVector = iter->second;
        const int32_t numImages(imageFilesVector.size());
        for (int32_t i = 0; i < numImages; i++) {
            CaretAssertVectorIndex(imageFilesVector, i);
            allImageFiles.push_back(imageFilesVector[i]);
        }
    }
    else {
        VolumeToImageMapping mapper(m_volumeInterface,
                                    mapIndex,
                                    volumeMappingMode,
                                    volumeSliceThickness,
                                    tabDrawingInfo.getDisplayGroup(),
                                    tabDrawingInfo.getTabIndex(),
                                    histologySlice);
        if (mapper.runMapping(errorMessageOut)) {
            const int32_t numImageFiles(mapper.getNumberOfOutputImageFiles());
            for (int32_t i = 0; i < numImageFiles; i++) {
                ImageFile* imageFile(mapper.takeOutputImageFile(i));
                CaretAssert(imageFile);
                allImageFiles.push_back(imageFile);
            }
        }
        m_mapIntersectionImageFiles.insert(std::make_pair(key,
                                                          allImageFiles));
    }

    if ( ! allImageFiles.empty()) {
        for (auto& imageFile : allImageFiles) {
            int32_t invalidOverlayIndex(-1);
            GraphicsPrimitiveV3fT2f* primitive = imageFile->getGraphicsPrimitiveForPlaneXyzDrawing(tabDrawingInfo.getTabIndex(),
                                                                                                   invalidOverlayIndex);
            if (primitive != NULL) {
                primitivesOut.push_back(primitive);
            }
        }
    }
    
    return primitivesOut;
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



