
/*LICENSE_START*/
/*
 * Copyright 2013 Washington University,
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

#include <algorithm>
#include <cmath>
#include <limits>

#define __BRAIN_OPEN_G_L_F_P_VOLUME_OBLIQUE_DRAWING_DECLARE__
#include "BrainOpenGLFPVolumeObliqueDrawing.h"
#undef __BRAIN_OPEN_G_L_F_P_VOLUME_OBLIQUE_DRAWING_DECLARE__

#include "Brain.h"
#include "BrowserTabContent.h"
#include "CaretAssert.h"
#include "CaretLogger.h"
#include "DisplayPropertiesLabels.h"
#include "Matrix4x4.h"
#include "ModelVolume.h"
#include "VolumeMappableInterface.h"

using namespace caret;
    
/**
 * \class caret::BrainOpenGLFPVolumeObliqueDrawing 
 * \brief Oblique Volume Drawing.
 * \ingroup Brain
 *
 */

/**
 * Constructor.
 */
BrainOpenGLFPVolumeObliqueDrawing::BrainOpenGLFPVolumeObliqueDrawing()
: CaretObject()
{
    
}

/**
 * Destructor.
 */
BrainOpenGLFPVolumeObliqueDrawing::~BrainOpenGLFPVolumeObliqueDrawing()
{
}

void
BrainOpenGLFPVolumeObliqueDrawing::draw(BrainOpenGLFixedPipeline* fixedPipelineDrawing,
                                        BrowserTabContent* browserTabContent,
                                        std::vector<BrainOpenGLFixedPipeline::VolumeDrawInfo>& volumeDrawInfo,
                                        const int viewport[4])
{
    CaretAssert(fixedPipelineDrawing);
    CaretAssert(browserTabContent);
    
    if (volumeDrawInfo.empty()) {
        return;
    }
    
    const VolumeSliceViewPlaneEnum::Enum slicePlane = browserTabContent->getSliceViewPlane();
    switch (slicePlane) {
        case VolumeSliceViewPlaneEnum::ALL:
        {
            const int vpHalfX = viewport[2] / 2;
            const int vpHalfY = viewport[3] / 2;
            
            const int paraVP[4] = {
                viewport[0],
                viewport[1] + vpHalfY,
                vpHalfX,
                vpHalfY
            };
            drawSlice(fixedPipelineDrawing,
                      browserTabContent,
                      volumeDrawInfo,
                      VolumeSliceViewPlaneEnum::PARASAGITTAL,
                      paraVP);
            
            
            const int coronalVP[4] = {
                viewport[0] + vpHalfX,
                viewport[1] + vpHalfY,
                vpHalfX,
                vpHalfY
            };
            drawSlice(fixedPipelineDrawing,
                      browserTabContent,
                      volumeDrawInfo,
                      VolumeSliceViewPlaneEnum::CORONAL,
                      coronalVP);
            
            
            const int axialVP[4] = {
                viewport[0] + vpHalfX,
                viewport[1],
                vpHalfX,
                vpHalfY
            };
            drawSlice(fixedPipelineDrawing,
                      browserTabContent,
                      volumeDrawInfo,
                      VolumeSliceViewPlaneEnum::AXIAL,
                      axialVP);
        }
            break;
        case VolumeSliceViewPlaneEnum::AXIAL:
        case VolumeSliceViewPlaneEnum::CORONAL:
        case VolumeSliceViewPlaneEnum::PARASAGITTAL:
            drawSlice(fixedPipelineDrawing,
                      browserTabContent,
                      volumeDrawInfo,
                      slicePlane,
                      viewport);
            break;
    }
}

void
BrainOpenGLFPVolumeObliqueDrawing::drawSlice(BrainOpenGLFixedPipeline* fixedPipelineDrawing,
                                        BrowserTabContent* browserTabContent,
                                        std::vector<BrainOpenGLFixedPipeline::VolumeDrawInfo>& volumeDrawInfo,
                                        const VolumeSliceViewPlaneEnum::Enum slicePlane,
                                        const int viewport[4])
{
    ModelVolume* volumeModel = browserTabContent->getDisplayedVolumeModel();
    CaretAssert(volumeModel);

    Brain* brain = volumeModel->getBrain();
    PaletteFile* paletteFile = brain->getPaletteFile();
    const DisplayPropertiesLabels* dsl = brain->getDisplayPropertiesLabels();
    const DisplayGroupEnum::Enum displayGroup = dsl->getDisplayGroupForTab(fixedPipelineDrawing->windowTabIndex);
    
    /*
     * Setup the viewport so left on left, bottom at bottom, and near towards viewer
     */
    fixedPipelineDrawing->setViewportAndOrthographicProjection(viewport,
                                                               ProjectionViewTypeEnum::PROJECTION_VIEW_LEFT_LATERAL);

    const int32_t tabIndex = browserTabContent->getTabNumber();
    
    /*
     * Initialized the viewing matrix
     */
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    /*
     * Apply clipping planes
     */
    fixedPipelineDrawing->applyClippingPlanes();
    
    float voxelBounds[6];
    float voxelSpacing[3];
    
    /*
     * Get the maximum bounds of the voxels from all slices
     * and the smallest voxel spacing
     */
    if (false == getVoxelCoordinateBoundsAndSpacing(volumeDrawInfo,
                                                    voxelBounds,
                                                    voxelSpacing)) {
        return;
    }

    /*
     * Determine the larget slice coordinate range and the minimum voxel spacing
     */
    const float minScreenX = fixedPipelineDrawing->orthographicLeft;
    const float maxScreenX = fixedPipelineDrawing->orthographicRight;
    const float minScreenY = fixedPipelineDrawing->orthographicBottom;
    const float maxScreenY = fixedPipelineDrawing->orthographicTop;
    
    const float minVoxelSize = std::min(voxelSpacing[0],
                                        std::min(voxelSpacing[1],
                                                 voxelSpacing[2]));
    CaretAssert(maxScreenX - minScreenX);
    CaretAssert(maxScreenY - minScreenY);
    CaretAssert(minVoxelSize > 0.0);
    const float halfVoxelSize = minVoxelSize / 2.0;
    
    /*
     * quadCoords is the coordinates for all four corners of a 'quad'
     * that is used to draw a voxel.  quadRGBA is the colors for each
     * voxel drawn as a 'quad'.
     *
     * Reserve estimate maximum number of voxels to avoid memory reallocations.
     * Each voxel requires 4 XYZ coordinates (all four corners)
     * and 4 color components.
     */
    const int64_t estimatedVoxelsScreenX = ((maxScreenX - minScreenX) / minVoxelSize) + 5;
    const int64_t estimatedVoxelsScreenY = ((maxScreenY - minScreenY) / minVoxelSize) + 5;
    const int64_t estimatedVoxelCount = estimatedVoxelsScreenX * estimatedVoxelsScreenY;
    std::vector<float> quadCoords;
    quadCoords.reserve(estimatedVoxelCount * 4 * 3);
    std::vector<float> quadNormals;
    quadNormals.reserve(estimatedVoxelCount * 3);
    std::vector<uint8_t> quadRGBAs;
    quadRGBAs.reserve(estimatedVoxelCount * 4);
    
    /*
     * Translate to selected location
     */
    const float centerX = browserTabContent->getSliceCoordinateParasagittal();
    const float centerY = browserTabContent->getSliceCoordinateCoronal();
    const float centerZ = browserTabContent->getSliceCoordinateAxial();
    Matrix4x4 transformationMatrix;
    transformationMatrix.translate(centerX,
                                   centerY,
                                   centerZ);
    
    /*
     * Apply rotation
     */
    Matrix4x4 rotationMatrix = browserTabContent->getRotationMatrix();
    transformationMatrix.premultiply(rotationMatrix);
    
    /*
     * Set the voxels in the 'virtual' slice
     */
    const int32_t numVolumes = static_cast<int32_t>(volumeDrawInfo.size());
    for (float x = minScreenX; x < maxScreenX; x += minVoxelSize) {
        for (float y = minScreenY; y < maxScreenY; y += minVoxelSize) {
            //
            // Apply the transformation matrix to the point
            //
            float pt[3] = { x, y, 0.0 };
            switch (slicePlane) {
                case VolumeSliceViewPlaneEnum::ALL:
                    CaretAssert(0);
                    break;
                case VolumeSliceViewPlaneEnum::AXIAL:
                    pt[0] = x;
                    pt[1] = y;
                    pt[2] = 0;
                    break;
                case VolumeSliceViewPlaneEnum::CORONAL:
                    pt[0] = x;
                    pt[1] = 0.0;
                    pt[2] = y;
                    break;
                case VolumeSliceViewPlaneEnum::PARASAGITTAL:
                    pt[0] = 0.0;
                    pt[1] = x;
                    pt[2] = y;
                    break;
            }
            
            /*
             * Transforms screen into plane
             */
            transformationMatrix.multiplyPoint3(pt);
            
            /*
             * Find the voxel in each volume for drawing
             */
            uint8_t voxelRGBA[4] = { 0, 0, 0, 0 };
            
            for (int32_t iVol = 0; iVol < numVolumes; iVol++) {
                const BrainOpenGLFixedPipeline::VolumeDrawInfo& vdi = volumeDrawInfo[iVol];
                const VolumeMappableInterface* volInter = vdi.volumeFile;
                int64_t voxelI, voxelJ, voxelK;
                volInter->enclosingVoxel(pt[0], pt[1], pt[2],
                                         voxelI, voxelJ, voxelK);
                
                uint8_t rgba[4] = { 0, 0, 0, 0 };
                if (volInter->indexValid(voxelI, voxelJ, voxelK)) {
                    volInter->getVoxelColorInMap(paletteFile,
                                                 voxelI, voxelJ, voxelK, vdi.mapIndex,
                                                 displayGroup,
                                                 tabIndex,
                                                 rgba);
                    if (rgba[3] > 0) {
                        voxelRGBA[0] = rgba[0];
                        voxelRGBA[1] = rgba[1];
                        voxelRGBA[2] = rgba[2];
                        voxelRGBA[3] = rgba[3];
                    }
                }
            }
            
            if (voxelRGBA[3] > 0) {
                quadRGBAs.push_back(voxelRGBA[0]);
                quadRGBAs.push_back(voxelRGBA[1]);
                quadRGBAs.push_back(voxelRGBA[2]);
                quadRGBAs.push_back(voxelRGBA[3]);
                
                quadNormals.push_back(0.0);
                quadNormals.push_back(0.0);
                quadNormals.push_back(1.0);
                
                const float xmin = x - halfVoxelSize;
                const float xmax = x + halfVoxelSize;
                const float ymin = y - halfVoxelSize;
                const float ymax = y + halfVoxelSize;
                
                quadCoords.push_back(xmin);
                quadCoords.push_back(ymin);
                quadCoords.push_back(0.0);

                quadCoords.push_back(xmax);
                quadCoords.push_back(ymin);
                quadCoords.push_back(0.0);

                quadCoords.push_back(xmax);
                quadCoords.push_back(ymax);
                quadCoords.push_back(0.0);
                
                quadCoords.push_back(xmin);
                quadCoords.push_back(ymax);
                quadCoords.push_back(0.0);
            }
        }
    }
    
    if (false == quadCoords.empty()) {
        drawQuads(quadCoords,
                  quadNormals,
                  quadRGBAs);
    }
}

bool
BrainOpenGLFPVolumeObliqueDrawing::getVoxelCoordinateBoundsAndSpacing(const std::vector<BrainOpenGLFixedPipeline::VolumeDrawInfo>& volumeDrawInfo,
                                                                      float boundsOut[6],
                                                                      float spacingOut[3])
{
    const int32_t numberOfVolumesToDraw = static_cast<int32_t>(volumeDrawInfo.size());
    if (numberOfVolumesToDraw <= 0) {
        return false;
    }
    
    /*
     * Find maximum extent of all voxels and smallest voxel
     * size in each dimension.
     */
    float minVoxelX = std::numeric_limits<float>::max();
    float maxVoxelX = -std::numeric_limits<float>::max();
    float minVoxelY = std::numeric_limits<float>::max();
    float maxVoxelY = -std::numeric_limits<float>::max();
    float minVoxelZ = std::numeric_limits<float>::max();
    float maxVoxelZ = -std::numeric_limits<float>::max();
    float voxelStepX = std::numeric_limits<float>::max();
    float voxelStepY = std::numeric_limits<float>::max();
    float voxelStepZ = std::numeric_limits<float>::max();
    float sliceCoordinate = 0.0;
    for (int32_t i = 0; i < numberOfVolumesToDraw; i++) {
        const VolumeMappableInterface* volumeFile = volumeDrawInfo[i].volumeFile;
        int64_t dimI, dimJ, dimK, numMaps, numComponents;
        volumeFile->getDimensions(dimI, dimJ, dimK, numMaps, numComponents);
        
        float originX, originY, originZ;
        float x1, y1, z1;
        float lastX, lastY, lastZ;
        volumeFile->indexToSpace(0, 0, 0, originX, originY, originZ);
        volumeFile->indexToSpace(1, 1, 1, x1, y1, z1);
        volumeFile->indexToSpace(dimI - 1, dimJ - 1, dimK - 1, lastX, lastY, lastZ);
        const float dx = x1 - originX;
        const float dy = y1 - originY;
        const float dz = z1 - originZ;
        voxelStepX = std::min(voxelStepX, std::fabs(dx));
        voxelStepY = std::min(voxelStepY, std::fabs(dy));
        voxelStepZ = std::min(voxelStepZ, std::fabs(dz));
        
        minVoxelX = std::min(minVoxelX, std::min(originX, lastX));
        maxVoxelX = std::max(maxVoxelX, std::max(originX, lastX));
        minVoxelY = std::min(minVoxelY, std::min(originY, lastY));
        maxVoxelY = std::max(maxVoxelY, std::max(originY, lastY));
        minVoxelZ = std::min(minVoxelZ, std::min(originZ, lastZ));
        maxVoxelZ = std::max(maxVoxelZ, std::max(originZ, lastZ));
        
    }
    int64_t numVoxelsX = std::ceil(((maxVoxelX - minVoxelX) / voxelStepX) + 1);
    int64_t numVoxelsY = std::ceil(((maxVoxelY - minVoxelY) / voxelStepY) + 1);
    int64_t numVoxelsZ = std::ceil(((maxVoxelZ - minVoxelZ) / voxelStepZ) + 1);
    
    const AString voxelInfo = ("Volume Coord Min/Max: X("
                               + AString::number(minVoxelX) + ", "
                               + AString::number(maxVoxelX) + ")  Y("
                               + AString::number(minVoxelY) + ", "
                               + AString::number(maxVoxelY) + ") Z("
                               + AString::number(minVoxelZ) + ", "
                               + AString::number(maxVoxelZ) + ") "
                               + " Min Voxel Sizes: ("
                               + AString::number(voxelStepX) + ", "
                               + AString::number(voxelStepY) + ", "
                               + AString::number(voxelStepZ) + ") "
                               + " Number of Voxels: ("
                               + AString::number(numVoxelsX) + ", "
                               + AString::number(numVoxelsY) + ", "
                               + AString::number(numVoxelsZ) + ") Slice Coordinate ("
                               + AString::number(sliceCoordinate) + ")");
    CaretLogFine(voxelInfo);
    
    boundsOut[0] = minVoxelX;
    boundsOut[1] = maxVoxelX;
    boundsOut[2] = minVoxelY;
    boundsOut[3] = maxVoxelY;
    boundsOut[4] = minVoxelZ;
    boundsOut[5] = maxVoxelZ;
    
    spacingOut[0] = voxelStepX;
    spacingOut[1] = voxelStepY;
    spacingOut[2] = voxelStepZ;
    
    bool valid = false;
    
    if ((maxVoxelX > minVoxelX)
        && (maxVoxelY > minVoxelY)
        && (maxVoxelZ > minVoxelZ)
        && (voxelStepX > 0.0)
        && (voxelStepY > 0.0)
        && (voxelStepZ > 0.0)) {
        valid = true;
    }
    
    return valid;
}

void
BrainOpenGLFPVolumeObliqueDrawing::drawQuads(const std::vector<float>& coordinates,
                                             const std::vector<float>& normals,
                                             const std::vector<uint8_t>& rgbaColors)
{
    const uint64_t numCoords  = coordinates.size();
    const uint64_t numNormals = normals.size();
    const uint64_t numColors  = rgbaColors.size();
    
    const uint64_t numQuads = numCoords / 12;  // 12 three-d coords per quad
    CaretAssert(numNormals == (numQuads * 3));
    CaretAssert(numColors == (numQuads * 4));
    
    const float* coordPtr = &coordinates[0];
    const float* normalPtr = &normals[0];
    const uint8_t* colorPtr = &rgbaColors[0];
    
    glBegin(GL_QUADS);
    for (uint64_t i = 0; i < numQuads; i++) {
        const uint64_t iCoord = i * 12;
        const uint64_t iColor = i * 4;
        const uint64_t iNormal = i * 3;
        
        glColor4ubv(&colorPtr[iColor]);
        glNormal3fv(&normalPtr[iNormal]);
        
        glVertex3fv(&coordPtr[iCoord]);
        glVertex3fv(&coordPtr[iCoord+3]);
        glVertex3fv(&coordPtr[iCoord+6]);
        glVertex3fv(&coordPtr[iCoord+9]);
    }
    glEnd();
}


/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
BrainOpenGLFPVolumeObliqueDrawing::toString() const
{
    return "BrainOpenGLFPVolumeObliqueDrawing";
}

