
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
#include <cstdlib>
#include <iostream>
#include <limits>

#ifdef CARET_OS_WINDOWS
#include <Windows.h>
#endif
#ifdef CARET_OS_MACOSX
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif

#define __BRAIN_OPEN_G_L_F_P_VOLUME_OBLIQUE_DRAWING_DECLARE__
#include "BrainOpenGLFPVolumeObliqueDrawing.h"
#undef __BRAIN_OPEN_G_L_F_P_VOLUME_OBLIQUE_DRAWING_DECLARE__

#include "BoundingBox.h"
#include "Brain.h"
#include "BrowserTabContent.h"
#include "CaretAssert.h"
#include "CaretLogger.h"
#include "CaretPreferences.h"
#include "DisplayPropertiesLabels.h"
#include "IdentificationWithColor.h"
#include "MathFunctions.h"
#include "Matrix4x4.h"
#include "Model.h"
#include "ModelVolume.h"
#include "Plane.h"
#include "SelectionItemVoxel.h"
#include "SelectionManager.h"
#include "SessionManager.h"
#include "Surface.h"
#include "VolumeMappableInterface.h"
#include "VolumeSurfaceOutlineModel.h"
#include "VolumeSurfaceOutlineColorOrTabModel.h"
#include "VolumeSurfaceOutlineSetModel.h"

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
    
    glEnable(GL_DEPTH_TEST);
    
    if (volumeDrawInfo.empty()) {
        return;
    }
    
    const VolumeSliceViewPlaneEnum::Enum slicePlane = browserTabContent->getSliceViewPlane();
    switch (slicePlane) {
        case VolumeSliceViewPlaneEnum::ALL:
        {
            const int vpHalfX = viewport[2] / 2;
            const int vpHalfY = viewport[3] / 2;
            
            const int allVP[4] = {
                viewport[0],
                viewport[1],
                vpHalfX,
                vpHalfY
            };
            
            glLoadIdentity();
            drawSlicesForAllView(fixedPipelineDrawing,
                                 browserTabContent,
                                 volumeDrawInfo,
                                 allVP);
//            drawSurfaces(fixedPipelineDrawing,
//                         browserTabContent,
//                         allVP);
            
            const int paraVP[4] = {
                viewport[0],
                viewport[1] + vpHalfY,
                vpHalfX,
                vpHalfY
            };
            drawSliceForSliceView(fixedPipelineDrawing,
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
            drawSliceForSliceView(fixedPipelineDrawing,
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
            drawSliceForSliceView(fixedPipelineDrawing,
                      browserTabContent,
                      volumeDrawInfo,
                      VolumeSliceViewPlaneEnum::AXIAL,
                      axialVP);
        }
            break;
        case VolumeSliceViewPlaneEnum::AXIAL:
        case VolumeSliceViewPlaneEnum::CORONAL:
        case VolumeSliceViewPlaneEnum::PARASAGITTAL:
            drawSliceForSliceView(fixedPipelineDrawing,
                      browserTabContent,
                      volumeDrawInfo,
                      slicePlane,
                      viewport);
            break;
    }
}

void
BrainOpenGLFPVolumeObliqueDrawing::drawAxis(BrainOpenGLFixedPipeline* fixedPipelineDrawing,
                                            const Matrix4x4& rotationMatrix)
{
    if (SessionManager::get()->getCaretPreferences()->isVolumeAxesCrosshairsDisplayed()) {
        fixedPipelineDrawing->setLineWidth(1.0);
        
        Matrix4x4 inverseMatrix(rotationMatrix);
        inverseMatrix.invert();
        
        const float bigNumber = 10000;
        for (int32_t iPlane = 0; iPlane < 3; iPlane++) {
            float p1[3] = { 0.0, 0.0, 0.0 };
            float p2[3] = { 0.0, 0.0, 0.0 };
            unsigned char lineColor[3] = { 0, 0, 0 };
            
            switch (iPlane) {
                case 0:
                    p1[2] = -bigNumber;
                    p1[2] =  bigNumber;
                    lineColor[2] = 255;
                    break;
                case 1:
                    p1[1] = -bigNumber;
                    p1[1] =  bigNumber;
                    lineColor[1] = 255;
                    break;
                case 2:
                    p1[0] = -bigNumber;
                    p1[0] =  bigNumber;
                    lineColor[0] = 255;
                    break;
            }

            inverseMatrix.multiplyPoint3(p1);
            inverseMatrix.multiplyPoint3(p2);
            
            glColor3ubv(lineColor);
            glBegin(GL_LINES);
            glVertex3fv(p1);
            glVertex3fv(p2);
            glEnd();
        }
    }
}

void
BrainOpenGLFPVolumeObliqueDrawing::drawSlicesForAllView(BrainOpenGLFixedPipeline* fixedPipelineDrawing,
                          BrowserTabContent* browserTabContent,
                          std::vector<BrainOpenGLFixedPipeline::VolumeDrawInfo>& volumeDrawInfo,
                          const int viewport[4])
{
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    
    Model* model = browserTabContent->getModelControllerForDisplay();
    Brain* brain = model->getBrain();
    
    /*
     * Setup the viewport so left on left, bottom at bottom, and near towards viewer
     */
    fixedPipelineDrawing->setViewportAndOrthographicProjection(viewport,
                                                               ProjectionViewTypeEnum::PROJECTION_VIEW_LEFT_LATERAL);

    drawSlice(brain,
              fixedPipelineDrawing,
              browserTabContent,
              volumeDrawInfo,
              VolumeSliceViewPlaneEnum::AXIAL,
              false);
    
    drawSlice(brain,
              fixedPipelineDrawing,
              browserTabContent,
              volumeDrawInfo,
              VolumeSliceViewPlaneEnum::CORONAL,
              false);
    
    drawSlice(brain,
              fixedPipelineDrawing,
              browserTabContent,
              volumeDrawInfo,
              VolumeSliceViewPlaneEnum::PARASAGITTAL,
              false);
    
    glPopMatrix();
}

void
BrainOpenGLFPVolumeObliqueDrawing::drawSliceForSliceView(BrainOpenGLFixedPipeline* fixedPipelineDrawing,
                                                            BrowserTabContent* browserTabContent,
                                                            std::vector<BrainOpenGLFixedPipeline::VolumeDrawInfo>& volumeDrawInfo,
                                                            const VolumeSliceViewPlaneEnum::Enum slicePlane,
                                                            const int viewport[4])
{
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    
    Model* model = browserTabContent->getModelControllerForDisplay();
    Brain* brain = model->getBrain();
    
    /*
     * Setup the viewport so left on left, bottom at bottom, and near towards viewer
     */
    fixedPipelineDrawing->setViewportAndOrthographicProjection(viewport,
                                                               ProjectionViewTypeEnum::PROJECTION_VIEW_LEFT_LATERAL);
    
    drawSlice(brain,
              fixedPipelineDrawing,
              browserTabContent,
              volumeDrawInfo,
              slicePlane,
              true);
    
    glPopMatrix();
}

void
BrainOpenGLFPVolumeObliqueDrawing::drawSlice(Brain* brain,
                                             BrainOpenGLFixedPipeline* fixedPipelineDrawing,
                                        BrowserTabContent* browserTabContent,
                                        std::vector<BrainOpenGLFixedPipeline::VolumeDrawInfo>& volumeDrawInfo,
                                        const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
                                        const bool isSliceView)
{
    const int32_t numVolumes = static_cast<int32_t>(volumeDrawInfo.size());
    if (numVolumes <= 0) {
        return;
    }

    AString debugString;
    
    
    const AString planeName = VolumeSliceViewPlaneEnum::toGuiName(sliceViewPlane);
    debugString.appendWithNewLine("\n"
                                  + AString(isSliceView ? "Slice View " : "All View")
                                  + planeName);
    
    CaretAssert(brain);

    PaletteFile* paletteFile = brain->getPaletteFile();
    const DisplayPropertiesLabels* dsl = brain->getDisplayPropertiesLabels();
    const DisplayGroupEnum::Enum displayGroup = dsl->getDisplayGroupForTab(fixedPipelineDrawing->windowTabIndex);
    

    const int32_t tabIndex = browserTabContent->getTabNumber();
    
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

    SelectionItemVoxel* voxelID = brain->getSelectionManager()->getVoxelIdentification();
    
    /*
     * Check for a 'selection' type mode
     */
    bool isSelect = false;
    switch (fixedPipelineDrawing->mode) {
        case BrainOpenGLFixedPipeline::MODE_DRAWING:
            break;
        case BrainOpenGLFixedPipeline::MODE_IDENTIFICATION:
            if (voxelID->isEnabledForSelection()) {
                isSelect = true;
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            }
            else {
                return;
            }
            break;
        case BrainOpenGLFixedPipeline::MODE_PROJECTION:
            return;
            break;
    }
    
    glPushMatrix();
    
    /*
     * When selecting, need to use flat shading so that colors do 
     * not get interpolated as identification information is in
     * the color components.
     */
    if (isSelect) {
        glShadeModel(GL_FLAT);
    }
    else {
        glShadeModel(GL_SMOOTH);
    }
    
    /*
     * For identification, five items per voxel
     * 1) volume index
     * 2) map index
     * 3) index I
     * 4) index J
     * 5) index K
     */
    const int32_t idPerVoxelCount = 5;
    std::vector<int32_t> identificationIndices;
    if (isSelect) {
        identificationIndices.reserve(10000 * idPerVoxelCount);
    }
    
    /*
     * Disable culling so that both sides of the triangles/quads are drawn.
     */
    GLboolean cullFaceOn = glIsEnabled(GL_CULL_FACE);
    glDisable(GL_CULL_FACE);
    
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
//    const float halfVoxelSize = minVoxelSize / 2.0;
    
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
     * Get coordinate of selected slices
     */
    const float selectedSliceOrigin[3] = {
        browserTabContent->getSliceCoordinateParasagittal(),
        browserTabContent->getSliceCoordinateCoronal(),
        browserTabContent->getSliceCoordinateAxial()
    };
    
    /*
     * Get the rotation matrix
     */
    Matrix4x4 rotationMatrix = browserTabContent->getRotationMatrix();

    /*
     * Create the transformation matrix
     */
    Matrix4x4 transformationMatrix;
    transformationMatrix.postmultiply(rotationMatrix);
    
    /*
     * Transform a unit vector for the selected slice view
     * to get the normal vector of the slice.
     */
    float sliceNormalVector[3] = { 0.0, 0.0, 1.0 };
    float sliceOffset[3] = { 0.0, 0.0, 0.0 };
    switch (sliceViewPlane) {
        case VolumeSliceViewPlaneEnum::ALL:
            CaretAssert(0);
            break;
        case VolumeSliceViewPlaneEnum::AXIAL:
            sliceNormalVector[0] = 0.0;
            sliceNormalVector[1] = 0.0;
            sliceNormalVector[2] = 1.0;
            sliceOffset[2] = selectedSliceOrigin[2];
            break;
        case VolumeSliceViewPlaneEnum::CORONAL:
            sliceNormalVector[0] = 0.0;
            sliceNormalVector[1] = -1.0;
            sliceNormalVector[2] = 0.0;
            sliceOffset[1] = selectedSliceOrigin[1];
            break;
        case VolumeSliceViewPlaneEnum::PARASAGITTAL:
            sliceNormalVector[0] = -1.0;
            sliceNormalVector[1] = 0.0;
            sliceNormalVector[2] = 0.0;
            sliceOffset[0] = selectedSliceOrigin[0];
            break;
    }
    
    /*
     * Translate for selected slice
     */
    transformationMatrix.translate(sliceOffset[0],
                             sliceOffset[1],
                             sliceOffset[2]);
    
    /*
     * Transform the slice normal vector so that it
     * points orthogonal to the slice by rotating the
     * vector with the rotation matrix.
     */
    rotationMatrix.multiplyPoint3(sliceNormalVector);
    MathFunctions::normalizeVector(sliceNormalVector);
    
    
    debugString.appendWithNewLine("Slice normal vector: "
                                  + AString::fromNumbers(sliceNormalVector, 3, ", "));
    
    Plane slicePlane(sliceNormalVector,
                      sliceOffset);
    debugString.appendWithNewLine("Slice plane from normal/pt: "
                                      + slicePlane.toString());
    
    /*
     * If this is a volume slice view,
     * (1) Set the 'center' to the center of the slice
     * (2) Set the 'eye' to an offset from the slice
     * (3) Set the 'up vector'
     * (4) Use the above with gluLookAt to set the viewing position
     */
    if (isSliceView) {
        /*
         * Set the "up" vector for the slice
         */
        float upVector[3] = { 0.0, 0.0, 0.0 };
        switch (sliceViewPlane) {
            case VolumeSliceViewPlaneEnum::ALL:
                CaretAssert(0);
                break;
            case VolumeSliceViewPlaneEnum::AXIAL:
                upVector[0] = 0.0;
                upVector[1] = 1.0;
                upVector[2] = 0.0;
                break;
            case VolumeSliceViewPlaneEnum::CORONAL:
                upVector[0] = 0.0;
                upVector[1] = 0.0;
                upVector[2] = 1.0;
                break;
            case VolumeSliceViewPlaneEnum::PARASAGITTAL:
                upVector[0] = 0.0;
                upVector[1] = 0.0;
                upVector[2] = 1.0;
                break;
        }
        
        float center[3] = {
            sliceOffset[0],
            sliceOffset[1],
            sliceOffset[2]
        };
        
        const float distanceFromEyeToCenter = 100.0;
        const float eye[3] = {
            center[0] + sliceNormalVector[0] * distanceFromEyeToCenter,
            center[1] + sliceNormalVector[1] * distanceFromEyeToCenter,
            center[2] + sliceNormalVector[2] * distanceFromEyeToCenter
        };
        
        /*
         * Need to orient the "up vector"
         */
        rotationMatrix.multiplyPoint3(upVector);
        MathFunctions::normalizeVector(upVector);
        
        /*
         * Use gluLookAt to view the slice
         */
        gluLookAt(eye[0],
                  eye[1],
                  eye[2],
                  center[0],
                  center[1],
                  center[2],
                  upVector[0],
                  upVector[1],
                  upVector[2]);
        
        debugString.appendWithNewLine("Center: " + AString::fromNumbers(center, 3, ", "));
        debugString.appendWithNewLine("Eye: " + AString::fromNumbers(eye, 3, ", "));
        debugString.appendWithNewLine("Up: " + AString::fromNumbers(upVector, 3, ", "));
    }
    
//    BoundingBox sliceBoundingBox;
//    sliceBoundingBox.resetForUpdate();
//    const int32_t numVolumes = static_cast<int32_t>(volumeDrawInfo.size());
//    for (int32_t iVol = 0; iVol < numVolumes; iVol++) {
//        const BrainOpenGLFixedPipeline::VolumeDrawInfo& vdi = volumeDrawInfo[iVol];
//        const VolumeMappableInterface* volInter = vdi.volumeFile;
//        BoundingBox boundingBox;
//        volInter->getVoxelSpaceBoundingBox(boundingBox);
//        
//        sliceBoundingBox.update(boundingBox.getMinXYZ());
//        sliceBoundingBox.update(boundingBox.getMaxXYZ());
//    }
//    
//    float cornerCoord = std::fabs(sliceBoundingBox.getMinX());
//    cornerCoord = std::max(cornerCoord, std::fabs(sliceBoundingBox.getMaxX()));
//    cornerCoord = std::max(cornerCoord, std::fabs(sliceBoundingBox.getMinY()));
//    cornerCoord = std::max(cornerCoord, std::fabs(sliceBoundingBox.getMaxY()));
//    cornerCoord = std::max(cornerCoord, std::fabs(sliceBoundingBox.getMinZ()));
//    cornerCoord = std::max(cornerCoord, std::fabs(sliceBoundingBox.getMaxZ()));
//
//    /*
//     * Get coordinates of virtual slice corners
//     * MAY NEED ASPECT RATION OF VIEWPORT
//     */
//    float leftBottomCorner[3] = { 0.0, 0.0, 0.0 };
//    float rightBottomCorner[3] = { 0.0, 0.0, 0.0 };
//    float rightTopCorner[3] = { 0.0, 0.0, 0.0 };
//    float leftTopCorner[3] = { 0.0, 0.0, 0.0 };
//    
//    switch (slicePlane) {
//        case VolumeSliceViewPlaneEnum::ALL:
//            CaretAssert(0);
//            break;
//        case VolumeSliceViewPlaneEnum::AXIAL:
//            leftBottomCorner[0] = -cornerCoord;
//            leftBottomCorner[1] = -cornerCoord;
//            leftBottomCorner[2] = 0.0;
//            leftTopCorner[0] = -cornerCoord;
//            leftTopCorner[1] = cornerCoord;
//            leftTopCorner[2] = 0.0;
//            rightBottomCorner[0] = cornerCoord;
//            rightBottomCorner[1] = -cornerCoord;
//            rightBottomCorner[2] = 0.0;
//            rightTopCorner[0] = cornerCoord;
//            rightTopCorner[1] = cornerCoord;
//            rightTopCorner[2] = 0.0;
//            break;
//        case VolumeSliceViewPlaneEnum::CORONAL:
//            leftBottomCorner[0] = -cornerCoord;
//            leftBottomCorner[1] = 0.0;
//            leftBottomCorner[2] = -cornerCoord;
//            leftTopCorner[0] = -cornerCoord;
//            leftTopCorner[1] = 0.0;
//            leftTopCorner[2] = cornerCoord;
//            rightBottomCorner[0] = cornerCoord;
//            rightBottomCorner[1] = 0.0;
//            rightBottomCorner[2] = -cornerCoord;
//            rightTopCorner[0] = cornerCoord;
//            rightTopCorner[1] = 0.0;
//            rightTopCorner[2] = cornerCoord;
//            break;
//        case VolumeSliceViewPlaneEnum::PARASAGITTAL:
////            leftBottomCorner[0] = 0.0;
////            leftBottomCorner[1] = -cornerCoord;
////            leftBottomCorner[2] = -cornerCoord;
////            leftTopCorner[0] = 0.0;
////            leftTopCorner[1] = -cornerCoord;
////            leftTopCorner[2] = cornerCoord;
////            rightBottomCorner[0] = 0.0;
////            rightBottomCorner[1] = cornerCoord;
////            rightBottomCorner[2] = -cornerCoord;
////            rightTopCorner[0] = 0.0;
////            rightTopCorner[1] = cornerCoord;
////            rightTopCorner[2] = cornerCoord;
//            leftBottomCorner[0] = 0.0;
//            leftBottomCorner[1] = cornerCoord;
//            leftBottomCorner[2] = -cornerCoord;
//            leftTopCorner[0] = 0.0;
//            leftTopCorner[1] = cornerCoord;
//            leftTopCorner[2] = cornerCoord;
//            rightBottomCorner[0] = 0.0;
//            rightBottomCorner[1] = -cornerCoord;
//            rightBottomCorner[2] = -cornerCoord;
//            rightTopCorner[0] = 0.0;
//            rightTopCorner[1] = -cornerCoord;
//            rightTopCorner[2] = cornerCoord;
//            break;
//    }
//
//    transformationMatrix.multiplyPoint3(leftBottomCorner);
//    transformationMatrix.multiplyPoint3(leftTopCorner);
//    transformationMatrix.multiplyPoint3(rightBottomCorner);
//    transformationMatrix.multiplyPoint3(rightTopCorner);
////    std::cout << "Left bottom: " << qPrintable(AString::fromNumbers(leftBottomCorner, 3, ", ")) << std::endl;
////    std::cout << "Left top: " << qPrintable(AString::fromNumbers(leftTopCorner, 3, ", ")) << std::endl;
////    std::cout << "Right bottom: " << qPrintable(AString::fromNumbers(rightBottomCorner, 3, ", ")) << std::endl;
////    std::cout << "Right top: " << qPrintable(AString::fromNumbers(rightTopCorner, 3, ", ")) << std::endl;
//    
//    Plane plane(leftBottomCorner,
//                rightBottomCorner,
//                rightTopCorner);
//    debugString.appendWithNewLine("Plane from corners: "
//                                  + plane.toString());
    
    float coordsForPlane[3][3];
    bool coordsForPlaneValid = false;
    
    const float zoom = browserTabContent->getScaling();
    glScalef(zoom, zoom, zoom);
    
    bool isDrawWithInterpolation = true;
    if (isDrawWithInterpolation) {
        const float screenBounds[4] = {
            minScreenX,
            maxScreenX,
            minScreenY,
            maxScreenY
        };
        
        drawSliceVoxelsWithInterpolation(fixedPipelineDrawing,
                                     volumeDrawInfo,
                                     sliceViewPlane,
                                     identificationIndices,
                                     paletteFile,
                                     idPerVoxelCount,
                                     transformationMatrix,
                                     displayGroup,
                                     tabIndex,
                                     screenBounds,
                                     sliceNormalVector,
                                     minVoxelSize,
                                     isSelect);
//        float bottomLeft[3];
//        float bottomRight[3];
//        float topRight[3];
//        float topLeft[3];
//        switch (sliceViewPlane) {
//            case VolumeSliceViewPlaneEnum::ALL:
//                CaretAssert(0);
//                break;
//            case VolumeSliceViewPlaneEnum::AXIAL:
//                bottomLeft[0] = minScreenX;
//                bottomLeft[1] = minScreenY;
//                bottomLeft[2] = 0;
//                bottomRight[0] = maxScreenX;
//                bottomRight[1] = minScreenY;
//                bottomRight[2] = 0;
//                topRight[0] = maxScreenX;
//                topRight[1] = maxScreenY;
//                topRight[2] = 0;
//                topLeft[0] = minScreenX;
//                topLeft[1] = maxScreenY;
//                topLeft[2] = 0;
//                break;
//            case VolumeSliceViewPlaneEnum::CORONAL:
//                bottomLeft[0] = minScreenX;
//                bottomLeft[1] = 0;
//                bottomLeft[2] = minScreenY;
//                bottomRight[0] = maxScreenX;
//                bottomRight[1] = 0;
//                bottomRight[2] = minScreenY;
//                topRight[0] = maxScreenX;
//                topRight[1] = 0;
//                topRight[2] = maxScreenY;
//                topLeft[0] = minScreenX;
//                topLeft[1] = 0;
//                topLeft[2] = maxScreenY;
//                break;
//            case VolumeSliceViewPlaneEnum::PARASAGITTAL:
//                bottomLeft[0] = 0;
//                bottomLeft[1] = minScreenX;
//                bottomLeft[2] = minScreenY;
//                bottomRight[0] = 0;
//                bottomRight[1] = maxScreenX;
//                bottomRight[2] = minScreenY;
//                topRight[0] = 0;
//                topRight[1] = maxScreenX;
//                topRight[2] = maxScreenY;
//                topLeft[0] = 0;
//                topLeft[1] = minScreenX;
//                topLeft[2] = maxScreenY;
//                break;
//        }
//        
//        transformationMatrix.multiplyPoint3(bottomLeft);
//        transformationMatrix.multiplyPoint3(bottomRight);
//        transformationMatrix.multiplyPoint3(topRight);
//        transformationMatrix.multiplyPoint3(topLeft);
//        
//        
//        double bottomLeftToTopLeftUnitVector[3] = {
//            topLeft[0] - bottomLeft[0],
//            topLeft[1] - bottomLeft[1],
//            topLeft[2] - bottomLeft[2],
//        };
//        MathFunctions::normalizeVector(bottomLeftToTopLeftUnitVector);
//        
//        double bottomLeftToBottomRightUnitVector[3] = {
//            bottomRight[0] - bottomRight[0],
//            bottomRight[1] - bottomRight[1],
//            bottomRight[2] - bottomRight[2]
//        };
//        MathFunctions::normalizeVector(bottomLeftToBottomRightUnitVector);
//        
//        double bottomRightToTopRightUnitVector[3] = {
//            topRight[0] - bottomRight[0],
//            topRight[1] - bottomRight[1],
//            topRight[2] - bottomRight[2]
//        };
//        MathFunctions::normalizeVector(bottomRightToTopRightUnitVector);
//        
//        const double bottomLeftToTopLeftDistance = MathFunctions::distance3D(bottomLeft,
//                                                                             topLeft);
//        const double bottomRightToTopRightDistance = MathFunctions::distance3D(bottomRight,
//                                                                               topRight);
//        if ((bottomLeftToTopLeftDistance > 0)
//            && (bottomRightToTopRightDistance > 0)) {
//
//            const double bottomLeftToTopLeftStep = minVoxelSize;
//            const double numLeftSteps = (bottomLeftToTopLeftDistance / bottomLeftToTopLeftStep);
//            
//            const double bottomRightToTopRightStep = (bottomRightToTopRightDistance
//                                                      / numLeftSteps);
//            
//            const double dtVertical = bottomLeftToTopLeftStep / bottomLeftToTopLeftDistance;
//
//            double leftEdgeCoord[3];
//            double nextRowLeftEdgeCoord[3];
//            double rightEdgeCoord[3];
//            double nextRowRightEdgeCoord[3];
//            for (double tVertical = 0.0, dLeft = 0.0, dRight = 0.0;
//                 tVertical < 1.0;
//                 tVertical += dtVertical, dLeft += bottomLeftToTopLeftStep, dRight += bottomRightToTopRightStep) {
//                /*
//                 * First coordinate on left side of row
//                 */
//                leftEdgeCoord[0] = bottomLeft[0] + (dLeft * bottomLeftToTopLeftUnitVector[0]);
//                leftEdgeCoord[1] = bottomLeft[1] + (dLeft * bottomLeftToTopLeftUnitVector[1]);
//                leftEdgeCoord[2] = bottomLeft[2] + (dLeft * bottomLeftToTopLeftUnitVector[2]);
//
//                /*
//                 * Last coordinate on right side of row
//                 */
//                rightEdgeCoord[0] = bottomRight[0] + (dRight * bottomRightToTopRightUnitVector[0]);
//                rightEdgeCoord[1] = bottomRight[1] + (dRight * bottomRightToTopRightUnitVector[1]);
//                rightEdgeCoord[2] = bottomRight[2] + (dRight * bottomRightToTopRightUnitVector[2]);
//
//                /*
//                 * First coordinate on left side of NEXT row
//                 */
//                nextRowLeftEdgeCoord[0] = bottomLeft[0] + ((dLeft + bottomLeftToTopLeftStep) * bottomLeftToTopLeftUnitVector[0]);
//                nextRowLeftEdgeCoord[1] = bottomLeft[1] + ((dLeft + bottomLeftToTopLeftStep) * bottomLeftToTopLeftUnitVector[1]);
//                nextRowLeftEdgeCoord[2] = bottomLeft[2] + ((dLeft + bottomLeftToTopLeftStep) * bottomLeftToTopLeftUnitVector[2]);
//                
//                /*
//                 * Last coordinate on right side of NEXT row
//                 */
//                nextRowRightEdgeCoord[0] = bottomRight[0] + ((dRight + bottomRightToTopRightStep) * bottomRightToTopRightUnitVector[0]);
//                nextRowRightEdgeCoord[1] = bottomRight[1] + ((dRight + bottomRightToTopRightStep) * bottomRightToTopRightUnitVector[1]);
//                nextRowRightEdgeCoord[2] = bottomRight[2] + ((dRight + bottomRightToTopRightStep) * bottomRightToTopRightUnitVector[2]);
//                
//                
//                const double leftToRightDistance = MathFunctions::distance3D(leftEdgeCoord, rightEdgeCoord);
//                const double leftToRightStep = minVoxelSize;
//                const double dt = (leftToRightStep / leftToRightDistance);
//                
//                bool firstFlag = false;
//                for (double t = 0; t < 1.0; t += dt) {
//                    const double oneMinusT = (1.0 - t);
//                    float pt1[3] = {
//                        leftEdgeCoord[0] * t + rightEdgeCoord[0] * oneMinusT,
//                        leftEdgeCoord[1] * t + rightEdgeCoord[1] * oneMinusT,
//                        leftEdgeCoord[2] * t + rightEdgeCoord[2] * oneMinusT
//                    };
//                    
//                    /*
//                     * Find the voxel in each volume for drawing
//                     */
//                    uint8_t voxelRGBA[4] = { 0, 0, 0, 0 };
//                    
//                    for (int32_t iVol = 0; iVol < numVolumes; iVol++) {
//                        const BrainOpenGLFixedPipeline::VolumeDrawInfo& vdi = volumeDrawInfo[iVol];
//                        const VolumeMappableInterface* volInter = vdi.volumeFile;
//                        int64_t voxelI, voxelJ, voxelK;
//                        volInter->enclosingVoxel(pt1[0], pt1[1], pt1[2],
//                                                 voxelI, voxelJ, voxelK);
//                        
//                        uint8_t rgba[4] = { 0, 0, 0, 0 };
//                        if (volInter->indexValid(voxelI, voxelJ, voxelK)) {
//                            volInter->getVoxelColorInMap(paletteFile,
//                                                         voxelI, voxelJ, voxelK, vdi.mapIndex,
//                                                         displayGroup,
//                                                         tabIndex,
//                                                         rgba);
//                            if (rgba[3] > 0) {
//                                if ((rgba[0] > 0)
//                                    && (rgba[1] > 0)
//                                    && (rgba[2] > 0)) {
//                                    if (isSelect) {
//                                        const int32_t idIndex = identificationIndices.size() / idPerVoxelCount;
//                                        fixedPipelineDrawing->colorIdentification->addItem(rgba,
//                                                                                           SelectionItemDataTypeEnum::VOXEL,
//                                                                                           idIndex);
//                                        rgba[3] = 255;
//                                        identificationIndices.push_back(iVol);
//                                        identificationIndices.push_back(vdi.mapIndex);
//                                        identificationIndices.push_back(voxelI);
//                                        identificationIndices.push_back(voxelJ);
//                                        identificationIndices.push_back(voxelK);
//                                    }
//                                    
//                                    voxelRGBA[0] = rgba[0];
//                                    voxelRGBA[1] = rgba[1];
//                                    voxelRGBA[2] = rgba[2];
//                                    voxelRGBA[3] = rgba[3];
//                                }
//                            }
//                        }
//                    }
//                    
//                    if (voxelRGBA[3] > 0) {
//                        const double tNext = t + dt;
//                        const double oneMinusTNext = 1.0 - tNext;
//                        
//                        /*
//                         * Right bottom corner of voxel
//                         */
//                        const double pt2[3] = {
//                            leftEdgeCoord[0] * tNext + rightEdgeCoord[0] * oneMinusTNext,
//                            leftEdgeCoord[1] * tNext + rightEdgeCoord[1] * oneMinusTNext,
//                            leftEdgeCoord[2] * tNext + rightEdgeCoord[2] * oneMinusTNext
//                        };
//                        
//                        /*
//                         * Top right corner of voxel
//                         */
//                        const double pt3[3] = {
//                            nextRowLeftEdgeCoord[0] * tNext + nextRowRightEdgeCoord[0] * oneMinusTNext,
//                            nextRowLeftEdgeCoord[1] * tNext + nextRowRightEdgeCoord[1] * oneMinusTNext,
//                            nextRowLeftEdgeCoord[2] * tNext + nextRowRightEdgeCoord[2] * oneMinusTNext
//                        };
//                        
//                        /*
//                         * Top left corner of voxel
//                         */
//                        const double pt4[3] = {
//                            nextRowLeftEdgeCoord[0] * t + nextRowRightEdgeCoord[0] * oneMinusT,
//                            nextRowLeftEdgeCoord[1] * t + nextRowRightEdgeCoord[1] * oneMinusT,
//                            nextRowLeftEdgeCoord[2] * t + nextRowRightEdgeCoord[2] * oneMinusT
//                        };
//                        
//                        if (firstFlag) {
//                            firstFlag = false;
//                            std::cout << "pt1: " << qPrintable(AString::fromNumbers(pt1, 3, ", ")) << std::endl;
//                            std::cout << "pt2: " << qPrintable(AString::fromNumbers(pt2, 3, ", ")) << std::endl;
//                            std::cout << "pt3: " << qPrintable(AString::fromNumbers(pt3, 3, ", ")) << std::endl;
//                            std::cout << "pt4: " << qPrintable(AString::fromNumbers(pt4, 3, ", ")) << std::endl;
//                        }
//                        quadRGBAs.push_back(voxelRGBA[0]);
//                        quadRGBAs.push_back(voxelRGBA[1]);
//                        quadRGBAs.push_back(voxelRGBA[2]);
//                        quadRGBAs.push_back(voxelRGBA[3]);
//                        
//                        quadNormals.push_back(sliceNormalVector[0]);
//                        quadNormals.push_back(sliceNormalVector[1]);
//                        quadNormals.push_back(sliceNormalVector[2]);
//                        
//                        quadCoords.push_back(pt1[0]);
//                        quadCoords.push_back(pt1[1]);
//                        quadCoords.push_back(pt1[2]);
//                        
//                        quadCoords.push_back(pt2[0]);
//                        quadCoords.push_back(pt2[1]);
//                        quadCoords.push_back(pt2[2]);
//                        
//                        quadCoords.push_back(pt3[0]);
//                        quadCoords.push_back(pt3[1]);
//                        quadCoords.push_back(pt3[2]);
//                        
//                        quadCoords.push_back(pt4[0]);
//                        quadCoords.push_back(pt4[1]);
//                        quadCoords.push_back(pt4[2]);
//                        
//                        if ( ! coordsForPlaneValid) {
//                            coordsForPlane[0][0] = pt1[0];
//                            coordsForPlane[0][1] = pt1[1];
//                            coordsForPlane[0][2] = pt1[2];
//                            coordsForPlane[1][0] = pt2[0];
//                            coordsForPlane[1][1] = pt2[1];
//                            coordsForPlane[1][2] = pt2[2];
//                            coordsForPlane[2][0] = pt3[0];
//                            coordsForPlane[2][1] = pt3[1];
//                            coordsForPlane[2][2] = pt3[2];
//                            coordsForPlaneValid = true;
//                        }
//                    }
//                }
//            }
//        }
        
    }
    else {
        const float screenBounds[4] = {
            minScreenX,
            maxScreenX,
            minScreenY,
            maxScreenY
        };
        
        drawSliceVoxelsWithTransform(fixedPipelineDrawing,
                                     volumeDrawInfo,
                                     sliceViewPlane,
                                     identificationIndices,
                                     paletteFile,
                                     idPerVoxelCount,
                                     transformationMatrix,
                                     displayGroup,
                                     tabIndex,
                                     screenBounds,
                                     sliceNormalVector,
                                     minVoxelSize,
                                     isSelect);
//        /*
//         * Set the voxels in the 'virtual' slice
//         */
//        for (float x = minScreenX; x < maxScreenX; x += minVoxelSize) {
//            for (float y = minScreenY; y < maxScreenY; y += minVoxelSize) {
//                /*
//                 * Apply the transformation matrix to the point
//                 */
//                float pt[3] = { x, y, 0.0 };
//                switch (sliceViewPlane) {
//                    case VolumeSliceViewPlaneEnum::ALL:
//                        CaretAssert(0);
//                        break;
//                    case VolumeSliceViewPlaneEnum::AXIAL:
//                        pt[0] = x;
//                        pt[1] = y;
//                        pt[2] = 0;
//                        break;
//                    case VolumeSliceViewPlaneEnum::CORONAL:
//                        pt[0] = x;
//                        pt[1] = 0.0;
//                        pt[2] = y;
//                        break;
//                    case VolumeSliceViewPlaneEnum::PARASAGITTAL:
//                        pt[0] = 0.0;
//                        pt[1] = x;
//                        pt[2] = y;
//                        break;
//                }
//                
//                /*
//                 * Transforms screen into plane
//                 */
//                transformationMatrix.multiplyPoint3(pt);
//                
//                /*
//                 * Find the voxel in each volume for drawing
//                 */
//                uint8_t voxelRGBA[4] = { 0, 0, 0, 0 };
//                
//                for (int32_t iVol = 0; iVol < numVolumes; iVol++) {
//                    const BrainOpenGLFixedPipeline::VolumeDrawInfo& vdi = volumeDrawInfo[iVol];
//                    const VolumeMappableInterface* volInter = vdi.volumeFile;
//                    int64_t voxelI, voxelJ, voxelK;
//                    volInter->enclosingVoxel(pt[0], pt[1], pt[2],
//                                             voxelI, voxelJ, voxelK);
//                    
//                    uint8_t rgba[4] = { 0, 0, 0, 0 };
//                    if (volInter->indexValid(voxelI, voxelJ, voxelK)) {
//                        volInter->getVoxelColorInMap(paletteFile,
//                                                     voxelI, voxelJ, voxelK, vdi.mapIndex,
//                                                     displayGroup,
//                                                     tabIndex,
//                                                     rgba);
//                        if (rgba[3] > 0) {
//                            if ((rgba[0] > 0)
//                                && (rgba[1] > 0)
//                                && (rgba[2] > 0)) {
//                                if (isSelect) {
//                                    const int32_t idIndex = identificationIndices.size() / idPerVoxelCount;
//                                    fixedPipelineDrawing->colorIdentification->addItem(rgba,
//                                                                                       SelectionItemDataTypeEnum::VOXEL,
//                                                                                       idIndex);
//                                    rgba[3] = 255;
//                                    identificationIndices.push_back(iVol);
//                                    identificationIndices.push_back(vdi.mapIndex);
//                                    identificationIndices.push_back(voxelI);
//                                    identificationIndices.push_back(voxelJ);
//                                    identificationIndices.push_back(voxelK);
//                                }
//                                
//                                voxelRGBA[0] = rgba[0];
//                                voxelRGBA[1] = rgba[1];
//                                voxelRGBA[2] = rgba[2];
//                                voxelRGBA[3] = rgba[3];
//                            }
//                        }
//                    }
//                }
//                
//                if (voxelRGBA[3] > 0) {
//                    float pt1[3];
//                    float pt2[3];
//                    float pt3[3];
//                    float pt4[3];
//                    
//                    const float xmin = x - halfVoxelSize;
//                    const float xmax = x + halfVoxelSize;
//                    const float ymin = y - halfVoxelSize;
//                    const float ymax = y + halfVoxelSize;
//                    
//                    switch (sliceViewPlane) {
//                        case VolumeSliceViewPlaneEnum::ALL:
//                            CaretAssert(0);
//                            break;
//                        case VolumeSliceViewPlaneEnum::AXIAL:
//                            pt1[0] = xmin;
//                            pt1[1] = ymin;
//                            pt1[2] = 0.0;
//                            pt2[0] = xmax;
//                            pt2[1] = ymin;
//                            pt2[2] = 0.0;
//                            pt3[0] = xmax;
//                            pt3[1] = ymax;
//                            pt3[2] = 0.0;
//                            pt4[0] = xmin;
//                            pt4[1] = ymax;
//                            pt4[2] = 0.0;
//                            break;
//                        case VolumeSliceViewPlaneEnum::CORONAL:
//                            pt1[0] = xmin;
//                            pt1[1] = 0.0;
//                            pt1[2] = ymin;
//                            pt2[0] = xmax;
//                            pt2[1] = 0.0;
//                            pt2[2] = ymin;
//                            pt3[0] = xmax;
//                            pt3[1] = 0.0;
//                            pt3[2] = ymax;
//                            pt4[0] = xmin;
//                            pt4[1] = 0.0;
//                            pt4[2] = ymax;
//                            break;
//                        case VolumeSliceViewPlaneEnum::PARASAGITTAL:
//                            pt1[0] = 0.0;
//                            pt1[1] = xmax;
//                            pt1[2] = ymin;
//                            pt2[0] = 0.0;
//                            pt2[1] = xmin;
//                            pt2[2] = ymin;
//                            pt3[0] = 0.0;
//                            pt3[1] = xmin;
//                            pt3[2] = ymax;
//                            pt4[0] = 0.0;
//                            pt4[1] = xmax;
//                            pt4[2] = ymax;
//                            break;
//                    }
//                    
//                    transformationMatrix.multiplyPoint3(pt1);
//                    transformationMatrix.multiplyPoint3(pt2);
//                    transformationMatrix.multiplyPoint3(pt3);
//                    transformationMatrix.multiplyPoint3(pt4);
//                    
//                    
//                    quadRGBAs.push_back(voxelRGBA[0]);
//                    quadRGBAs.push_back(voxelRGBA[1]);
//                    quadRGBAs.push_back(voxelRGBA[2]);
//                    quadRGBAs.push_back(voxelRGBA[3]);
//                    
//                    quadNormals.push_back(sliceNormalVector[0]);
//                    quadNormals.push_back(sliceNormalVector[1]);
//                    quadNormals.push_back(sliceNormalVector[2]);
//                    
//                    quadCoords.push_back(pt1[0]);
//                    quadCoords.push_back(pt1[1]);
//                    quadCoords.push_back(pt1[2]);
//                    
//                    quadCoords.push_back(pt2[0]);
//                    quadCoords.push_back(pt2[1]);
//                    quadCoords.push_back(pt2[2]);
//                    
//                    quadCoords.push_back(pt3[0]);
//                    quadCoords.push_back(pt3[1]);
//                    quadCoords.push_back(pt3[2]);
//                    
//                    quadCoords.push_back(pt4[0]);
//                    quadCoords.push_back(pt4[1]);
//                    quadCoords.push_back(pt4[2]);
//                    
//                    if ( ! coordsForPlaneValid) {
//                        coordsForPlane[0][0] = pt1[0];
//                        coordsForPlane[0][1] = pt1[1];
//                        coordsForPlane[0][2] = pt1[2];
//                        coordsForPlane[1][0] = pt2[0];
//                        coordsForPlane[1][1] = pt2[1];
//                        coordsForPlane[1][2] = pt2[2];
//                        coordsForPlane[2][0] = pt3[0];
//                        coordsForPlane[2][1] = pt3[1];
//                        coordsForPlane[2][2] = pt3[2];
//                        coordsForPlaneValid = true;
//                    }
//                }
//            }
//        }
    }
    
    if (coordsForPlaneValid) {
        Plane quadPlane(coordsForPlane[0],
                         coordsForPlane[1],
                         coordsForPlane[2]);
        debugString.appendWithNewLine("Plane from first quad: "
                                      + quadPlane.toString());
    }
    
//    if (false == quadCoords.empty()) {
//        drawQuads(quadCoords,
//                  quadNormals,
//                  quadRGBAs);
//        
//
        glPushMatrix();
        
        if ( ! isSelect) {
            if (slicePlane.isValidPlane()) {
                fixedPipelineDrawing->drawFiberOrientations(&slicePlane);
                fixedPipelineDrawing->drawFiberTrajectories(&slicePlane);
                
                drawSurfaceOutline(fixedPipelineDrawing,
                                   browserTabContent,
                                   volumeDrawInfo[0].volumeFile,
                                   slicePlane);
            }
        }
        
        glPopMatrix();
//    }
    
    if (isSelect) {
        int32_t identifiedItemIndex;
        float depth = -1.0;
        fixedPipelineDrawing->getIndexFromColorSelection(SelectionItemDataTypeEnum::VOXEL,
                                         fixedPipelineDrawing->mouseX,
                                         fixedPipelineDrawing->mouseY,
                                         identifiedItemIndex,
                                         depth);
        if (identifiedItemIndex >= 0) {
            const int32_t idIndex = identifiedItemIndex * idPerVoxelCount;
            const int32_t volDrawInfoIndex = identificationIndices[idIndex];
            CaretAssertVectorIndex(volumeDrawInfo, volDrawInfoIndex);
            VolumeMappableInterface* vf = volumeDrawInfo[volDrawInfoIndex].volumeFile;
            //const int32_t mapIndex = identificationIndices[idIndex + 1];
            const int64_t voxelIndices[3] = {
                identificationIndices[idIndex + 2],
                identificationIndices[idIndex + 3],
                identificationIndices[idIndex + 4]
            };
            
            if (voxelID->isOtherScreenDepthCloserToViewer(depth)) {
                voxelID->setVolumeFile(vf);
                voxelID->setVoxelIJK(voxelIndices);
                voxelID->setScreenDepth(depth);
                
                float voxelCoordinates[3];
                vf->indexToSpace(voxelIndices[0], voxelIndices[1], voxelIndices[2],
                                 voxelCoordinates[0], voxelCoordinates[1], voxelCoordinates[2]);
                
                fixedPipelineDrawing->setSelectedItemScreenXYZ(voxelID,
                                               voxelCoordinates);
                CaretLogFine("Selected Voxel (3D): " + AString::fromNumbers(voxelIndices, 3, ","));
            }
        }
    }
    if (cullFaceOn) {
        glEnable(GL_CULL_FACE);
    }
    
    glPopMatrix();
    
    //std::cout << qPrintable(debugString) << std::endl;
}

void
BrainOpenGLFPVolumeObliqueDrawing::drawSliceVoxelsWithTransform(BrainOpenGLFixedPipeline* fixedPipelineDrawing,
                                  std::vector<BrainOpenGLFixedPipeline::VolumeDrawInfo>& volumeDrawInfo,
                                  const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
                                                                std::vector<int32_t>& identificationIndices,
                                                                const PaletteFile* paletteFile,
                                                                const int32_t idPerVoxelCount,
                                                                const Matrix4x4& transformationMatrix,
                                                                const DisplayGroupEnum::Enum displayGroup,
                                                                const int32_t tabIndex,
                                                                const float screenBounds[4],
                                                                const float sliceNormalVector[3],
                                                                const float voxelSize,
                                                                const bool isSelectionMode)
{
    const int32_t numVolumes = static_cast<int32_t>(volumeDrawInfo.size());
    const float halfVoxelSize = voxelSize / 2.0;
    
    const float minScreenX = screenBounds[0];
    const float maxScreenX = screenBounds[1];
    const float minScreenY = screenBounds[2];
    const float maxScreenY = screenBounds[3];
    
    /*
     * quadCoords is the coordinates for all four corners of a 'quad'
     * that is used to draw a voxel.  quadRGBA is the colors for each
     * voxel drawn as a 'quad'.
     *
     * Reserve estimate maximum number of voxels to avoid memory reallocations.
     * Each voxel requires 4 XYZ coordinates (all four corners)
     * and 4 color components.
     */
    const int64_t estimatedVoxelsScreenX = ((maxScreenX - minScreenX) / voxelSize) + 5;
    const int64_t estimatedVoxelsScreenY = ((maxScreenY - minScreenY) / voxelSize) + 5;
    const int64_t estimatedVoxelCount = estimatedVoxelsScreenX * estimatedVoxelsScreenY;
    std::vector<float> quadCoords;
    quadCoords.reserve(estimatedVoxelCount * 4 * 3);
    std::vector<float> quadNormals;
    quadNormals.reserve(estimatedVoxelCount * 3);
    std::vector<uint8_t> quadRGBAs;
    quadRGBAs.reserve(estimatedVoxelCount * 4);
    
    /*
     * Draw the voxels
     */
    for (float x = minScreenX; x < maxScreenX; x += voxelSize) {
        for (float y = minScreenY; y < maxScreenY; y += voxelSize) {
            /*
             * Apply the transformation matrix to the point
             */
            float pt[3] = { x, y, 0.0 };
            switch (sliceViewPlane) {
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
                        if ((rgba[0] > 0)
                            && (rgba[1] > 0)
                            && (rgba[2] > 0)) {
                            if (isSelectionMode) {
                                const int32_t idIndex = identificationIndices.size() / idPerVoxelCount;
                                fixedPipelineDrawing->colorIdentification->addItem(rgba,
                                                                                   SelectionItemDataTypeEnum::VOXEL,
                                                                                   idIndex);
                                rgba[3] = 255;
                                identificationIndices.push_back(iVol);
                                identificationIndices.push_back(vdi.mapIndex);
                                identificationIndices.push_back(voxelI);
                                identificationIndices.push_back(voxelJ);
                                identificationIndices.push_back(voxelK);
                            }
                            
                            voxelRGBA[0] = rgba[0];
                            voxelRGBA[1] = rgba[1];
                            voxelRGBA[2] = rgba[2];
                            voxelRGBA[3] = rgba[3];
                        }
                    }
                }
            }
            
            if (voxelRGBA[3] > 0) {
                float pt1[3];
                float pt2[3];
                float pt3[3];
                float pt4[3];
                
                const float xmin = x - halfVoxelSize;
                const float xmax = x + halfVoxelSize;
                const float ymin = y - halfVoxelSize;
                const float ymax = y + halfVoxelSize;
                
                switch (sliceViewPlane) {
                    case VolumeSliceViewPlaneEnum::ALL:
                        CaretAssert(0);
                        break;
                    case VolumeSliceViewPlaneEnum::AXIAL:
                        pt1[0] = xmin;
                        pt1[1] = ymin;
                        pt1[2] = 0.0;
                        pt2[0] = xmax;
                        pt2[1] = ymin;
                        pt2[2] = 0.0;
                        pt3[0] = xmax;
                        pt3[1] = ymax;
                        pt3[2] = 0.0;
                        pt4[0] = xmin;
                        pt4[1] = ymax;
                        pt4[2] = 0.0;
                        break;
                    case VolumeSliceViewPlaneEnum::CORONAL:
                        pt1[0] = xmin;
                        pt1[1] = 0.0;
                        pt1[2] = ymin;
                        pt2[0] = xmax;
                        pt2[1] = 0.0;
                        pt2[2] = ymin;
                        pt3[0] = xmax;
                        pt3[1] = 0.0;
                        pt3[2] = ymax;
                        pt4[0] = xmin;
                        pt4[1] = 0.0;
                        pt4[2] = ymax;
                        break;
                    case VolumeSliceViewPlaneEnum::PARASAGITTAL:
                        pt1[0] = 0.0;
                        pt1[1] = xmax;
                        pt1[2] = ymin;
                        pt2[0] = 0.0;
                        pt2[1] = xmin;
                        pt2[2] = ymin;
                        pt3[0] = 0.0;
                        pt3[1] = xmin;
                        pt3[2] = ymax;
                        pt4[0] = 0.0;
                        pt4[1] = xmax;
                        pt4[2] = ymax;
                        break;
                }
                
                transformationMatrix.multiplyPoint3(pt1);
                transformationMatrix.multiplyPoint3(pt2);
                transformationMatrix.multiplyPoint3(pt3);
                transformationMatrix.multiplyPoint3(pt4);
                
                
                quadRGBAs.push_back(voxelRGBA[0]);
                quadRGBAs.push_back(voxelRGBA[1]);
                quadRGBAs.push_back(voxelRGBA[2]);
                quadRGBAs.push_back(voxelRGBA[3]);
                
                quadNormals.push_back(sliceNormalVector[0]);
                quadNormals.push_back(sliceNormalVector[1]);
                quadNormals.push_back(sliceNormalVector[2]);
                
                quadCoords.push_back(pt1[0]);
                quadCoords.push_back(pt1[1]);
                quadCoords.push_back(pt1[2]);
                
                quadCoords.push_back(pt2[0]);
                quadCoords.push_back(pt2[1]);
                quadCoords.push_back(pt2[2]);
                
                quadCoords.push_back(pt3[0]);
                quadCoords.push_back(pt3[1]);
                quadCoords.push_back(pt3[2]);
                
                quadCoords.push_back(pt4[0]);
                quadCoords.push_back(pt4[1]);
                quadCoords.push_back(pt4[2]);
                
//                if ( ! coordsForPlaneValid) {
//                    coordsForPlane[0][0] = pt1[0];
//                    coordsForPlane[0][1] = pt1[1];
//                    coordsForPlane[0][2] = pt1[2];
//                    coordsForPlane[1][0] = pt2[0];
//                    coordsForPlane[1][1] = pt2[1];
//                    coordsForPlane[1][2] = pt2[2];
//                    coordsForPlane[2][0] = pt3[0];
//                    coordsForPlane[2][1] = pt3[1];
//                    coordsForPlane[2][2] = pt3[2];
//                    coordsForPlaneValid = true;
//                }
            }
        }
    }
    
    if ( ! quadCoords.empty()) {
        drawQuads(quadCoords,
                  quadNormals,
                  quadRGBAs);
    }
}

void
BrainOpenGLFPVolumeObliqueDrawing::drawSliceVoxelsWithInterpolation(BrainOpenGLFixedPipeline* fixedPipelineDrawing,
                                      std::vector<BrainOpenGLFixedPipeline::VolumeDrawInfo>& volumeDrawInfo,
                                      const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
                                      std::vector<int32_t>& identificationIndices,
                                      const PaletteFile* paletteFile,
                                      const int32_t idPerVoxelCount,
                                      const Matrix4x4& transformationMatrix,
                                      const DisplayGroupEnum::Enum displayGroup,
                                      const int32_t tabIndex,
                                      const float screenBounds[4],
                                      const float sliceNormalVector[3],
                                      const float voxelSize,
                                      const bool isSelectionMode)
{
    const int32_t numVolumes = static_cast<int32_t>(volumeDrawInfo.size());
    const float halfVoxelSize = voxelSize / 2.0;
    
    const float minScreenX = screenBounds[0];
    const float maxScreenX = screenBounds[1];
    const float minScreenY = screenBounds[2];
    const float maxScreenY = screenBounds[3];
    
    float bottomLeft[3];
    float bottomRight[3];
    float topRight[3];
    float topLeft[3];
    switch (sliceViewPlane) {
        case VolumeSliceViewPlaneEnum::ALL:
            CaretAssert(0);
            break;
        case VolumeSliceViewPlaneEnum::AXIAL:
            bottomLeft[0] = minScreenX;
            bottomLeft[1] = minScreenY;
            bottomLeft[2] = 0;
            bottomRight[0] = maxScreenX;
            bottomRight[1] = minScreenY;
            bottomRight[2] = 0;
            topRight[0] = maxScreenX;
            topRight[1] = maxScreenY;
            topRight[2] = 0;
            topLeft[0] = minScreenX;
            topLeft[1] = maxScreenY;
            topLeft[2] = 0;
            break;
        case VolumeSliceViewPlaneEnum::CORONAL:
            bottomLeft[0] = minScreenX;
            bottomLeft[1] = 0;
            bottomLeft[2] = minScreenY;
            bottomRight[0] = maxScreenX;
            bottomRight[1] = 0;
            bottomRight[2] = minScreenY;
            topRight[0] = maxScreenX;
            topRight[1] = 0;
            topRight[2] = maxScreenY;
            topLeft[0] = minScreenX;
            topLeft[1] = 0;
            topLeft[2] = maxScreenY;
            break;
        case VolumeSliceViewPlaneEnum::PARASAGITTAL:
            bottomLeft[0] = 0;
            bottomLeft[1] = minScreenX;
            bottomLeft[2] = minScreenY;
            bottomRight[0] = 0;
            bottomRight[1] = maxScreenX;
            bottomRight[2] = minScreenY;
            topRight[0] = 0;
            topRight[1] = maxScreenX;
            topRight[2] = maxScreenY;
            topLeft[0] = 0;
            topLeft[1] = minScreenX;
            topLeft[2] = maxScreenY;
            break;
    }
    
    transformationMatrix.multiplyPoint3(bottomLeft);
    transformationMatrix.multiplyPoint3(bottomRight);
    transformationMatrix.multiplyPoint3(topRight);
    transformationMatrix.multiplyPoint3(topLeft);
    
    
    double bottomLeftToTopLeftUnitVector[3] = {
        topLeft[0] - bottomLeft[0],
        topLeft[1] - bottomLeft[1],
        topLeft[2] - bottomLeft[2],
    };
    MathFunctions::normalizeVector(bottomLeftToTopLeftUnitVector);
    
//    double bottomLeftToBottomRightUnitVector[3] = {
//        bottomRight[0] - bottomRight[0],
//        bottomRight[1] - bottomRight[1],
//        bottomRight[2] - bottomRight[2]
//    };
//    MathFunctions::normalizeVector(bottomLeftToBottomRightUnitVector);
    
    double bottomRightToTopRightUnitVector[3] = {
        topRight[0] - bottomRight[0],
        topRight[1] - bottomRight[1],
        topRight[2] - bottomRight[2]
    };
    MathFunctions::normalizeVector(bottomRightToTopRightUnitVector);
    
    const double bottomLeftToTopLeftDistance = MathFunctions::distance3D(bottomLeft,
                                                                         topLeft);
    const double bottomRightToTopRightDistance = MathFunctions::distance3D(bottomRight,
                                                                           topRight);
    /*
     * quadCoords is the coordinates for all four corners of a 'quad'
     * that is used to draw a voxel.  quadRGBA is the colors for each
     * voxel drawn as a 'quad'.
     *
     * Reserve estimate maximum number of voxels to avoid memory reallocations.
     * Each voxel requires 4 XYZ coordinates (all four corners)
     * and 4 color components.
     */
    const int64_t estimatedVoxelsScreenX = ((maxScreenX - minScreenX) / voxelSize) + 5;
    const int64_t estimatedVoxelsScreenY = ((maxScreenY - minScreenY) / voxelSize) + 5;
    const int64_t estimatedVoxelCount = estimatedVoxelsScreenX * estimatedVoxelsScreenY;
    std::vector<float> quadCoords;
    quadCoords.reserve(estimatedVoxelCount * 4 * 3);
    std::vector<float> quadNormals;
    quadNormals.reserve(estimatedVoxelCount * 3);
    std::vector<uint8_t> quadRGBAs;
    quadRGBAs.reserve(estimatedVoxelCount * 4);
    
    const bool doPerVertexNormalsAndColors = false;
    if (doPerVertexNormalsAndColors) {
        quadNormals.reserve(estimatedVoxelCount * 3 * 4);
        quadRGBAs.reserve(estimatedVoxelCount * 4 * 4);
    }

    if ((bottomLeftToTopLeftDistance > 0)
        && (bottomRightToTopRightDistance > 0)) {
        
        const double bottomLeftToTopLeftStep = voxelSize;
        const double numLeftSteps = (bottomLeftToTopLeftDistance / bottomLeftToTopLeftStep);
        
        const double bottomRightToTopRightStep = (bottomRightToTopRightDistance
                                                  / numLeftSteps);
        
        const double dtVertical = bottomLeftToTopLeftStep / bottomLeftToTopLeftDistance;
        
        double leftEdgeCoord[3];
        double nextRowLeftEdgeCoord[3];
        double rightEdgeCoord[3];
        double nextRowRightEdgeCoord[3];
        for (double tVertical = 0.0, dLeft = 0.0, dRight = 0.0;
             tVertical < 1.0;
             tVertical += dtVertical, dLeft += bottomLeftToTopLeftStep, dRight += bottomRightToTopRightStep) {
            /*
             * First coordinate on left side of row
             */
            leftEdgeCoord[0] = bottomLeft[0] + (dLeft * bottomLeftToTopLeftUnitVector[0]);
            leftEdgeCoord[1] = bottomLeft[1] + (dLeft * bottomLeftToTopLeftUnitVector[1]);
            leftEdgeCoord[2] = bottomLeft[2] + (dLeft * bottomLeftToTopLeftUnitVector[2]);
            
            /*
             * Last coordinate on right side of row
             */
            rightEdgeCoord[0] = bottomRight[0] + (dRight * bottomRightToTopRightUnitVector[0]);
            rightEdgeCoord[1] = bottomRight[1] + (dRight * bottomRightToTopRightUnitVector[1]);
            rightEdgeCoord[2] = bottomRight[2] + (dRight * bottomRightToTopRightUnitVector[2]);
            
            /*
             * First coordinate on left side of NEXT row
             */
            nextRowLeftEdgeCoord[0] = bottomLeft[0] + ((dLeft + bottomLeftToTopLeftStep) * bottomLeftToTopLeftUnitVector[0]);
            nextRowLeftEdgeCoord[1] = bottomLeft[1] + ((dLeft + bottomLeftToTopLeftStep) * bottomLeftToTopLeftUnitVector[1]);
            nextRowLeftEdgeCoord[2] = bottomLeft[2] + ((dLeft + bottomLeftToTopLeftStep) * bottomLeftToTopLeftUnitVector[2]);
            
            /*
             * Last coordinate on right side of NEXT row
             */
            nextRowRightEdgeCoord[0] = bottomRight[0] + ((dRight + bottomRightToTopRightStep) * bottomRightToTopRightUnitVector[0]);
            nextRowRightEdgeCoord[1] = bottomRight[1] + ((dRight + bottomRightToTopRightStep) * bottomRightToTopRightUnitVector[1]);
            nextRowRightEdgeCoord[2] = bottomRight[2] + ((dRight + bottomRightToTopRightStep) * bottomRightToTopRightUnitVector[2]);
            
            const double bottomVoxelEdgeDistance = MathFunctions::distance3D(leftEdgeCoord,
                                                                             rightEdgeCoord);
            double bottomEdgeUnitVector[3];
            MathFunctions::createUnitVector(leftEdgeCoord, rightEdgeCoord, bottomEdgeUnitVector);
            const int64_t numVoxelsInRow = bottomVoxelEdgeDistance / voxelSize;
            const double bottomEdgeVoxelSize = bottomVoxelEdgeDistance / numVoxelsInRow;
            const double bottomVoxelEdgeDX = bottomEdgeVoxelSize * bottomEdgeUnitVector[0];
            const double bottomVoxelEdgeDY = bottomEdgeVoxelSize * bottomEdgeUnitVector[1];
            const double bottomVoxelEdgeDZ = bottomEdgeVoxelSize * bottomEdgeUnitVector[2];
            
            const double topVoxelEdgeDistance = MathFunctions::distance3D(nextRowLeftEdgeCoord,
                                                                          nextRowRightEdgeCoord);
            double topEdgeUnitVector[3];
            MathFunctions::createUnitVector(nextRowLeftEdgeCoord, nextRowRightEdgeCoord, topEdgeUnitVector);
            const double topEdgeVoxelSize = topVoxelEdgeDistance / numVoxelsInRow;
            const double topVoxelEdgeDX = topEdgeVoxelSize * topEdgeUnitVector[0];
            const double topVoxelEdgeDY = topEdgeVoxelSize * topEdgeUnitVector[1];
            const double topVoxelEdgeDZ = topEdgeVoxelSize * topEdgeUnitVector[2];
            
            float bottomLeftVoxelCoord[3] = {
                leftEdgeCoord[0],
                leftEdgeCoord[1],
                leftEdgeCoord[2]
            };
            float topLeftVoxelCoord[3] = {
                nextRowLeftEdgeCoord[0],
                nextRowLeftEdgeCoord[1],
                nextRowLeftEdgeCoord[2]
            };
            
            bool firstFlag = false;
            for (int64_t i = 0; i < numVoxelsInRow; i++) {
                /*
                 * Find the voxel in each volume for drawing
                 */
                uint8_t voxelRGBA[4] = { 0, 0, 0, 0 };
                
                for (int32_t iVol = 0; iVol < numVolumes; iVol++) {
                    const BrainOpenGLFixedPipeline::VolumeDrawInfo& vdi = volumeDrawInfo[iVol];
                    const VolumeMappableInterface* volInter = vdi.volumeFile;
                    int64_t voxelI, voxelJ, voxelK;
                    volInter->enclosingVoxel(bottomLeftVoxelCoord[0], bottomLeftVoxelCoord[1], bottomLeftVoxelCoord[2],
                                             voxelI, voxelJ, voxelK);
                    
                    uint8_t rgba[4] = { 0, 0, 0, 0 };
                    if (volInter->indexValid(voxelI, voxelJ, voxelK)) {
                        volInter->getVoxelColorInMap(paletteFile,
                                                     voxelI, voxelJ, voxelK, vdi.mapIndex,
                                                     displayGroup,
                                                     tabIndex,
                                                     rgba);
                        if (rgba[3] > 0) {
                            if ((rgba[0] > 0)
                                && (rgba[1] > 0)
                                && (rgba[2] > 0)) {
                                if (isSelectionMode) {
                                    const int32_t idIndex = identificationIndices.size() / idPerVoxelCount;
                                    fixedPipelineDrawing->colorIdentification->addItem(rgba,
                                                                                       SelectionItemDataTypeEnum::VOXEL,
                                                                                       idIndex);
                                    rgba[3] = 255;
                                    identificationIndices.push_back(iVol);
                                    identificationIndices.push_back(vdi.mapIndex);
                                    identificationIndices.push_back(voxelI);
                                    identificationIndices.push_back(voxelJ);
                                    identificationIndices.push_back(voxelK);
                                }
                                
                                voxelRGBA[0] = rgba[0];
                                voxelRGBA[1] = rgba[1];
                                voxelRGBA[2] = rgba[2];
                                voxelRGBA[3] = rgba[3];
                            }
                        }
                    }
                }
                
                if (voxelRGBA[3] > 0) {
                    /*
                     * Bottom right corner of voxel
                     */
                    const double bottomRightVoxelCoord[3] = {
                        bottomLeftVoxelCoord[0] + bottomVoxelEdgeDX,
                        bottomLeftVoxelCoord[1] + bottomVoxelEdgeDY,
                        bottomLeftVoxelCoord[2] + bottomVoxelEdgeDZ
                    };
                    
                    /*
                     * Top right corner of voxel
                     */
                    const double topRightVoxelCoord[3] = {
                        topLeftVoxelCoord[0] + topVoxelEdgeDX,
                        topLeftVoxelCoord[1] + topVoxelEdgeDY,
                        topLeftVoxelCoord[2] + topVoxelEdgeDZ
                    };

                    if (firstFlag) {
                        firstFlag = false;
                        std::cout << "bl: " << qPrintable(AString::fromNumbers(bottomLeftVoxelCoord, 3, ", ")) << std::endl;
                        std::cout << "br: " << qPrintable(AString::fromNumbers(bottomRightVoxelCoord, 3, ", ")) << std::endl;
                        std::cout << "tr: " << qPrintable(AString::fromNumbers(topRightVoxelCoord, 3, ", ")) << std::endl;
                        std::cout << "tl: " << qPrintable(AString::fromNumbers(topLeftVoxelCoord, 3, ", ")) << std::endl;
                    }
                    
                    quadRGBAs.push_back(voxelRGBA[0]);
                    quadRGBAs.push_back(voxelRGBA[1]);
                    quadRGBAs.push_back(voxelRGBA[2]);
                    quadRGBAs.push_back(voxelRGBA[3]);
                    
                    quadNormals.push_back(sliceNormalVector[0]);
                    quadNormals.push_back(sliceNormalVector[1]);
                    quadNormals.push_back(sliceNormalVector[2]);
                    
                    if (doPerVertexNormalsAndColors) {
                        quadRGBAs.push_back(voxelRGBA[0]);
                        quadRGBAs.push_back(voxelRGBA[1]);
                        quadRGBAs.push_back(voxelRGBA[2]);
                        quadRGBAs.push_back(voxelRGBA[3]);
                        
                        quadNormals.push_back(sliceNormalVector[0]);
                        quadNormals.push_back(sliceNormalVector[1]);
                        quadNormals.push_back(sliceNormalVector[2]);

                        quadRGBAs.push_back(voxelRGBA[0]);
                        quadRGBAs.push_back(voxelRGBA[1]);
                        quadRGBAs.push_back(voxelRGBA[2]);
                        quadRGBAs.push_back(voxelRGBA[3]);
                        
                        quadNormals.push_back(sliceNormalVector[0]);
                        quadNormals.push_back(sliceNormalVector[1]);
                        quadNormals.push_back(sliceNormalVector[2]);
                        
                        quadRGBAs.push_back(voxelRGBA[0]);
                        quadRGBAs.push_back(voxelRGBA[1]);
                        quadRGBAs.push_back(voxelRGBA[2]);
                        quadRGBAs.push_back(voxelRGBA[3]);
                        
                        quadNormals.push_back(sliceNormalVector[0]);
                        quadNormals.push_back(sliceNormalVector[1]);
                        quadNormals.push_back(sliceNormalVector[2]);
                    }
                    
                    quadCoords.push_back(bottomLeftVoxelCoord[0]);
                    quadCoords.push_back(bottomLeftVoxelCoord[1]);
                    quadCoords.push_back(bottomLeftVoxelCoord[2]);
                    
                    quadCoords.push_back(bottomRightVoxelCoord[0]);
                    quadCoords.push_back(bottomRightVoxelCoord[1]);
                    quadCoords.push_back(bottomRightVoxelCoord[2]);
                    
                    quadCoords.push_back(topRightVoxelCoord[0]);
                    quadCoords.push_back(topRightVoxelCoord[1]);
                    quadCoords.push_back(topRightVoxelCoord[2]);
                    
                    quadCoords.push_back(topLeftVoxelCoord[0]);
                    quadCoords.push_back(topLeftVoxelCoord[1]);
                    quadCoords.push_back(topLeftVoxelCoord[2]);
                    
                    
                    //                    if ( ! coordsForPlaneValid) {
                    //                        coordsForPlane[0][0] = pt1[0];
                    //                        coordsForPlane[0][1] = pt1[1];
                    //                        coordsForPlane[0][2] = pt1[2];
                    //                        coordsForPlane[1][0] = pt2[0];
                    //                        coordsForPlane[1][1] = pt2[1];
                    //                        coordsForPlane[1][2] = pt2[2];
                    //                        coordsForPlane[2][0] = pt3[0];
                    //                        coordsForPlane[2][1] = pt3[1];
                    //                        coordsForPlane[2][2] = pt3[2];
                    //                        coordsForPlaneValid = true;
                    //                    }
                }
                
                bottomLeftVoxelCoord[0] += bottomVoxelEdgeDX;
                bottomLeftVoxelCoord[1] += bottomVoxelEdgeDY;
                bottomLeftVoxelCoord[2] += bottomVoxelEdgeDZ;
                topLeftVoxelCoord[0] += topVoxelEdgeDX;
                topLeftVoxelCoord[1] += topVoxelEdgeDY;
                topLeftVoxelCoord[2] += topVoxelEdgeDZ;
            }
        }
    }

    if ( ! quadCoords.empty()) {
        drawQuads(quadCoords,
                  quadNormals,
                  quadRGBAs);
    }
}


//void
//BrainOpenGLFPVolumeObliqueDrawing::drawSliceVoxelsWithInterpolationOld(BrainOpenGLFixedPipeline* fixedPipelineDrawing,
//                                                                    std::vector<BrainOpenGLFixedPipeline::VolumeDrawInfo>& volumeDrawInfo,
//                                                                    const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
//                                                                    std::vector<int32_t>& identificationIndices,
//                                                                    const PaletteFile* paletteFile,
//                                                                    const int32_t idPerVoxelCount,
//                                                                    const Matrix4x4& transformationMatrix,
//                                                                    const DisplayGroupEnum::Enum displayGroup,
//                                                                    const int32_t tabIndex,
//                                                                    const float screenBounds[4],
//                                                                    const float sliceNormalVector[3],
//                                                                    const float voxelSize,
//                                                                    const bool isSelectionMode)
//{
//    const int32_t numVolumes = static_cast<int32_t>(volumeDrawInfo.size());
//    const float halfVoxelSize = voxelSize / 2.0;
//    
//    const float minScreenX = screenBounds[0];
//    const float maxScreenX = screenBounds[1];
//    const float minScreenY = screenBounds[2];
//    const float maxScreenY = screenBounds[3];
//    
//    float bottomLeft[3];
//    float bottomRight[3];
//    float topRight[3];
//    float topLeft[3];
//    switch (sliceViewPlane) {
//        case VolumeSliceViewPlaneEnum::ALL:
//            CaretAssert(0);
//            break;
//        case VolumeSliceViewPlaneEnum::AXIAL:
//            bottomLeft[0] = minScreenX;
//            bottomLeft[1] = minScreenY;
//            bottomLeft[2] = 0;
//            bottomRight[0] = maxScreenX;
//            bottomRight[1] = minScreenY;
//            bottomRight[2] = 0;
//            topRight[0] = maxScreenX;
//            topRight[1] = maxScreenY;
//            topRight[2] = 0;
//            topLeft[0] = minScreenX;
//            topLeft[1] = maxScreenY;
//            topLeft[2] = 0;
//            break;
//        case VolumeSliceViewPlaneEnum::CORONAL:
//            bottomLeft[0] = minScreenX;
//            bottomLeft[1] = 0;
//            bottomLeft[2] = minScreenY;
//            bottomRight[0] = maxScreenX;
//            bottomRight[1] = 0;
//            bottomRight[2] = minScreenY;
//            topRight[0] = maxScreenX;
//            topRight[1] = 0;
//            topRight[2] = maxScreenY;
//            topLeft[0] = minScreenX;
//            topLeft[1] = 0;
//            topLeft[2] = maxScreenY;
//            break;
//        case VolumeSliceViewPlaneEnum::PARASAGITTAL:
//            bottomLeft[0] = 0;
//            bottomLeft[1] = minScreenX;
//            bottomLeft[2] = minScreenY;
//            bottomRight[0] = 0;
//            bottomRight[1] = maxScreenX;
//            bottomRight[2] = minScreenY;
//            topRight[0] = 0;
//            topRight[1] = maxScreenX;
//            topRight[2] = maxScreenY;
//            topLeft[0] = 0;
//            topLeft[1] = minScreenX;
//            topLeft[2] = maxScreenY;
//            break;
//    }
//    
//    transformationMatrix.multiplyPoint3(bottomLeft);
//    transformationMatrix.multiplyPoint3(bottomRight);
//    transformationMatrix.multiplyPoint3(topRight);
//    transformationMatrix.multiplyPoint3(topLeft);
//    
//    
//    double bottomLeftToTopLeftUnitVector[3] = {
//        topLeft[0] - bottomLeft[0],
//        topLeft[1] - bottomLeft[1],
//        topLeft[2] - bottomLeft[2],
//    };
//    MathFunctions::normalizeVector(bottomLeftToTopLeftUnitVector);
//    
//    double bottomLeftToBottomRightUnitVector[3] = {
//        bottomRight[0] - bottomRight[0],
//        bottomRight[1] - bottomRight[1],
//        bottomRight[2] - bottomRight[2]
//    };
//    MathFunctions::normalizeVector(bottomLeftToBottomRightUnitVector);
//    
//    double bottomRightToTopRightUnitVector[3] = {
//        topRight[0] - bottomRight[0],
//        topRight[1] - bottomRight[1],
//        topRight[2] - bottomRight[2]
//    };
//    MathFunctions::normalizeVector(bottomRightToTopRightUnitVector);
//    
//    const double bottomLeftToTopLeftDistance = MathFunctions::distance3D(bottomLeft,
//                                                                         topLeft);
//    const double bottomRightToTopRightDistance = MathFunctions::distance3D(bottomRight,
//                                                                           topRight);
//    /*
//     * quadCoords is the coordinates for all four corners of a 'quad'
//     * that is used to draw a voxel.  quadRGBA is the colors for each
//     * voxel drawn as a 'quad'.
//     *
//     * Reserve estimate maximum number of voxels to avoid memory reallocations.
//     * Each voxel requires 4 XYZ coordinates (all four corners)
//     * and 4 color components.
//     */
//    const int64_t estimatedVoxelsScreenX = ((maxScreenX - minScreenX) / voxelSize) + 5;
//    const int64_t estimatedVoxelsScreenY = ((maxScreenY - minScreenY) / voxelSize) + 5;
//    const int64_t estimatedVoxelCount = estimatedVoxelsScreenX * estimatedVoxelsScreenY;
//    std::vector<float> quadCoords;
//    quadCoords.reserve(estimatedVoxelCount * 4 * 3);
//    std::vector<float> quadNormals;
//    quadNormals.reserve(estimatedVoxelCount * 3);
//    std::vector<uint8_t> quadRGBAs;
//    quadRGBAs.reserve(estimatedVoxelCount * 4);
//    
//    if ((bottomLeftToTopLeftDistance > 0)
//        && (bottomRightToTopRightDistance > 0)) {
//        
//        const double bottomLeftToTopLeftStep = voxelSize;
//        const double numLeftSteps = (bottomLeftToTopLeftDistance / bottomLeftToTopLeftStep);
//        
//        const double bottomRightToTopRightStep = (bottomRightToTopRightDistance
//                                                  / numLeftSteps);
//        
//        const double dtVertical = bottomLeftToTopLeftStep / bottomLeftToTopLeftDistance;
//        
//        double leftEdgeCoord[3];
//        double nextRowLeftEdgeCoord[3];
//        double rightEdgeCoord[3];
//        double nextRowRightEdgeCoord[3];
//        for (double tVertical = 0.0, dLeft = 0.0, dRight = 0.0;
//             tVertical < 1.0;
//             tVertical += dtVertical, dLeft += bottomLeftToTopLeftStep, dRight += bottomRightToTopRightStep) {
//            /*
//             * First coordinate on left side of row
//             */
//            leftEdgeCoord[0] = bottomLeft[0] + (dLeft * bottomLeftToTopLeftUnitVector[0]);
//            leftEdgeCoord[1] = bottomLeft[1] + (dLeft * bottomLeftToTopLeftUnitVector[1]);
//            leftEdgeCoord[2] = bottomLeft[2] + (dLeft * bottomLeftToTopLeftUnitVector[2]);
//            
//            /*
//             * Last coordinate on right side of row
//             */
//            rightEdgeCoord[0] = bottomRight[0] + (dRight * bottomRightToTopRightUnitVector[0]);
//            rightEdgeCoord[1] = bottomRight[1] + (dRight * bottomRightToTopRightUnitVector[1]);
//            rightEdgeCoord[2] = bottomRight[2] + (dRight * bottomRightToTopRightUnitVector[2]);
//            
//            /*
//             * First coordinate on left side of NEXT row
//             */
//            nextRowLeftEdgeCoord[0] = bottomLeft[0] + ((dLeft + bottomLeftToTopLeftStep) * bottomLeftToTopLeftUnitVector[0]);
//            nextRowLeftEdgeCoord[1] = bottomLeft[1] + ((dLeft + bottomLeftToTopLeftStep) * bottomLeftToTopLeftUnitVector[1]);
//            nextRowLeftEdgeCoord[2] = bottomLeft[2] + ((dLeft + bottomLeftToTopLeftStep) * bottomLeftToTopLeftUnitVector[2]);
//            
//            /*
//             * Last coordinate on right side of NEXT row
//             */
//            nextRowRightEdgeCoord[0] = bottomRight[0] + ((dRight + bottomRightToTopRightStep) * bottomRightToTopRightUnitVector[0]);
//            nextRowRightEdgeCoord[1] = bottomRight[1] + ((dRight + bottomRightToTopRightStep) * bottomRightToTopRightUnitVector[1]);
//            nextRowRightEdgeCoord[2] = bottomRight[2] + ((dRight + bottomRightToTopRightStep) * bottomRightToTopRightUnitVector[2]);
//            
//            
//            const double leftToRightDistance = MathFunctions::distance3D(leftEdgeCoord, rightEdgeCoord);
//            const double leftToRightStep = voxelSize;
//            const double dt = (leftToRightStep / leftToRightDistance);
//            
//            bool firstFlag = false;
//            for (double t = 0; t < 1.0; t += dt) {
//                const double oneMinusT = (1.0 - t);
//                float pt1[3] = {
//                    leftEdgeCoord[0] * t + rightEdgeCoord[0] * oneMinusT,
//                    leftEdgeCoord[1] * t + rightEdgeCoord[1] * oneMinusT,
//                    leftEdgeCoord[2] * t + rightEdgeCoord[2] * oneMinusT
//                };
//                
//                /*
//                 * Find the voxel in each volume for drawing
//                 */
//                uint8_t voxelRGBA[4] = { 0, 0, 0, 0 };
//                
//                for (int32_t iVol = 0; iVol < numVolumes; iVol++) {
//                    const BrainOpenGLFixedPipeline::VolumeDrawInfo& vdi = volumeDrawInfo[iVol];
//                    const VolumeMappableInterface* volInter = vdi.volumeFile;
//                    int64_t voxelI, voxelJ, voxelK;
//                    volInter->enclosingVoxel(pt1[0], pt1[1], pt1[2],
//                                             voxelI, voxelJ, voxelK);
//                    
//                    uint8_t rgba[4] = { 0, 0, 0, 0 };
//                    if (volInter->indexValid(voxelI, voxelJ, voxelK)) {
//                        volInter->getVoxelColorInMap(paletteFile,
//                                                     voxelI, voxelJ, voxelK, vdi.mapIndex,
//                                                     displayGroup,
//                                                     tabIndex,
//                                                     rgba);
//                        if (rgba[3] > 0) {
//                            if ((rgba[0] > 0)
//                                && (rgba[1] > 0)
//                                && (rgba[2] > 0)) {
//                                if (isSelectionMode) {
//                                    const int32_t idIndex = identificationIndices.size() / idPerVoxelCount;
//                                    fixedPipelineDrawing->colorIdentification->addItem(rgba,
//                                                                                       SelectionItemDataTypeEnum::VOXEL,
//                                                                                       idIndex);
//                                    rgba[3] = 255;
//                                    identificationIndices.push_back(iVol);
//                                    identificationIndices.push_back(vdi.mapIndex);
//                                    identificationIndices.push_back(voxelI);
//                                    identificationIndices.push_back(voxelJ);
//                                    identificationIndices.push_back(voxelK);
//                                }
//                                
//                                voxelRGBA[0] = rgba[0];
//                                voxelRGBA[1] = rgba[1];
//                                voxelRGBA[2] = rgba[2];
//                                voxelRGBA[3] = rgba[3];
//                            }
//                        }
//                    }
//                }
//                
//                if (voxelRGBA[3] > 0) {
//                    const double tNext = t + dt;
//                    const double oneMinusTNext = 1.0 - tNext;
//                    
//                    /*
//                     * Right bottom corner of voxel
//                     */
//                    const double pt2[3] = {
//                        leftEdgeCoord[0] * tNext + rightEdgeCoord[0] * oneMinusTNext,
//                        leftEdgeCoord[1] * tNext + rightEdgeCoord[1] * oneMinusTNext,
//                        leftEdgeCoord[2] * tNext + rightEdgeCoord[2] * oneMinusTNext
//                    };
//                    
//                    /*
//                     * Top right corner of voxel
//                     */
//                    const double pt3[3] = {
//                        nextRowLeftEdgeCoord[0] * tNext + nextRowRightEdgeCoord[0] * oneMinusTNext,
//                        nextRowLeftEdgeCoord[1] * tNext + nextRowRightEdgeCoord[1] * oneMinusTNext,
//                        nextRowLeftEdgeCoord[2] * tNext + nextRowRightEdgeCoord[2] * oneMinusTNext
//                    };
//                    
//                    /*
//                     * Top left corner of voxel
//                     */
//                    const double pt4[3] = {
//                        nextRowLeftEdgeCoord[0] * t + nextRowRightEdgeCoord[0] * oneMinusT,
//                        nextRowLeftEdgeCoord[1] * t + nextRowRightEdgeCoord[1] * oneMinusT,
//                        nextRowLeftEdgeCoord[2] * t + nextRowRightEdgeCoord[2] * oneMinusT
//                    };
//                    
//                    if (firstFlag) {
//                        firstFlag = false;
//                        std::cout << "pt1: " << qPrintable(AString::fromNumbers(pt1, 3, ", ")) << std::endl;
//                        std::cout << "pt2: " << qPrintable(AString::fromNumbers(pt2, 3, ", ")) << std::endl;
//                        std::cout << "pt3: " << qPrintable(AString::fromNumbers(pt3, 3, ", ")) << std::endl;
//                        std::cout << "pt4: " << qPrintable(AString::fromNumbers(pt4, 3, ", ")) << std::endl;
//                    }
//                    quadRGBAs.push_back(voxelRGBA[0]);
//                    quadRGBAs.push_back(voxelRGBA[1]);
//                    quadRGBAs.push_back(voxelRGBA[2]);
//                    quadRGBAs.push_back(voxelRGBA[3]);
//                    
//                    quadNormals.push_back(sliceNormalVector[0]);
//                    quadNormals.push_back(sliceNormalVector[1]);
//                    quadNormals.push_back(sliceNormalVector[2]);
//                    
//                    quadCoords.push_back(pt1[0]);
//                    quadCoords.push_back(pt1[1]);
//                    quadCoords.push_back(pt1[2]);
//                    
//                    quadCoords.push_back(pt2[0]);
//                    quadCoords.push_back(pt2[1]);
//                    quadCoords.push_back(pt2[2]);
//                    
//                    quadCoords.push_back(pt3[0]);
//                    quadCoords.push_back(pt3[1]);
//                    quadCoords.push_back(pt3[2]);
//                    
//                    quadCoords.push_back(pt4[0]);
//                    quadCoords.push_back(pt4[1]);
//                    quadCoords.push_back(pt4[2]);
//                    
//                    //                    if ( ! coordsForPlaneValid) {
//                    //                        coordsForPlane[0][0] = pt1[0];
//                    //                        coordsForPlane[0][1] = pt1[1];
//                    //                        coordsForPlane[0][2] = pt1[2];
//                    //                        coordsForPlane[1][0] = pt2[0];
//                    //                        coordsForPlane[1][1] = pt2[1];
//                    //                        coordsForPlane[1][2] = pt2[2];
//                    //                        coordsForPlane[2][0] = pt3[0];
//                    //                        coordsForPlane[2][1] = pt3[1];
//                    //                        coordsForPlane[2][2] = pt3[2];
//                    //                        coordsForPlaneValid = true;
//                    //                    }
//                }
//            }
//        }
//    }
//    
//    if ( ! quadCoords.empty()) {
//        drawQuads(quadCoords,
//                  quadNormals,
//                  quadRGBAs);
//    }
//}

void
BrainOpenGLFPVolumeObliqueDrawing::drawSurfaceOutline(BrainOpenGLFixedPipeline* fixedPipelineDrawing,
                                                      BrowserTabContent* browserTabContent,
                                                      VolumeMappableInterface* underlayVolume,
                                                      const Plane& plane)
{
    CaretAssert(browserTabContent);
    CaretAssert(underlayVolume);
    
    if ( ! plane.isValidPlane()) {
        return;
    }
    
    float intersectionPoint1[3];
    float intersectionPoint2[3];
    
    fixedPipelineDrawing->enableLineAntiAliasing();
    
    VolumeSurfaceOutlineSetModel* outlineSet = browserTabContent->getVolumeSurfaceOutlineSet();
    
    /*
     * Process each surface outline
     */
    const int32_t numberOfOutlines = outlineSet->getNumberOfDislayedVolumeSurfaceOutlines();
    for (int io = 0;
         io < numberOfOutlines;
         io++) {
        VolumeSurfaceOutlineModel* outline = outlineSet->getVolumeSurfaceOutlineModel(io);
        if (outline->isDisplayed()) {
            Surface* surface = outline->getSurface();
            if (surface != NULL) {
                const float thickness = outline->getThickness();
                const float lineWidth = fixedPipelineDrawing->modelSizeToPixelSize(thickness);
                
                int numTriangles = surface->getNumberOfTriangles();
                
                CaretColorEnum::Enum outlineColor = CaretColorEnum::BLACK;
                int32_t colorSourceBrowserTabIndex = -1;
                
                VolumeSurfaceOutlineColorOrTabModel* colorOrTabModel = outline->getColorOrTabModel();
                VolumeSurfaceOutlineColorOrTabModel::Item* selectedColorOrTabItem = colorOrTabModel->getSelectedItem();
                switch (selectedColorOrTabItem->getItemType()) {
                    case VolumeSurfaceOutlineColorOrTabModel::Item::ITEM_TYPE_BROWSER_TAB:
                        colorSourceBrowserTabIndex = selectedColorOrTabItem->getBrowserTabIndex();
                        break;
                    case VolumeSurfaceOutlineColorOrTabModel::Item::ITEM_TYPE_COLOR:
                        outlineColor = selectedColorOrTabItem->getColor();
                        break;
                }
                const bool surfaceColorFlag = (colorSourceBrowserTabIndex >= 0);
                
                float* nodeColoringRGBA = NULL;
                if (surfaceColorFlag) {
                    nodeColoringRGBA = fixedPipelineDrawing->surfaceNodeColoring->colorSurfaceNodes(NULL, /*modelDisplayController*/
                                                                                    surface,
                                                                                    colorSourceBrowserTabIndex);
                }
                
                glColor3fv(CaretColorEnum::toRGB(outlineColor));
                fixedPipelineDrawing->setLineWidth(lineWidth);
                
                /*
                 * Examine each triangle to see if it intersects the Plane
                 * in which the slice exists.
                 */
                glBegin(GL_LINES);
                for (int it = 0; it < numTriangles; it++) {
                    const int32_t* triangleNodes = surface->getTriangle(it);
                    const float* c1 = surface->getCoordinate(triangleNodes[0]);
                    const float* c2 = surface->getCoordinate(triangleNodes[1]);
                    const float* c3 = surface->getCoordinate(triangleNodes[2]);
                    
                    if (plane.triangleIntersectPlane(c1, c2, c3,
                                                     intersectionPoint1,
                                                     intersectionPoint2)) {
                        if (surfaceColorFlag) {
                            /*
                             * Use coloring assigned to the first node in the triangle
                             * but only if Alpha is valid (greater than zero).
                             */
                            const int64_t colorIndex = triangleNodes[0] * 4;
                            if (nodeColoringRGBA[colorIndex + 3] > 0.0) {
                                glColor3fv(&nodeColoringRGBA[triangleNodes[0] * 4]);
                            }
                            else {
                                continue;
                            }
                        }
                        
                        /*
                         * Draw the line where the triangle intersections the slice
                         */
                        glVertex3fv(intersectionPoint1);
                        glVertex3fv(intersectionPoint2);
//                        switch(slicePlane) {
//                            case VolumeSliceViewPlaneEnum::ALL:
//                                break;
//                            case VolumeSliceViewPlaneEnum::PARASAGITTAL:
//                                glVertex3f(p1[0], intersectionPoint1[1], intersectionPoint1[2]);
//                                glVertex3f(p1[0], intersectionPoint2[1], intersectionPoint2[2]);
//                                break;
//                            case VolumeSliceViewPlaneEnum::CORONAL:
//                                glVertex3f(intersectionPoint1[0], p1[1], intersectionPoint1[2]);
//                                glVertex3f(intersectionPoint2[0], p1[1], intersectionPoint2[2]);
//                                break;
//                            case VolumeSliceViewPlaneEnum::AXIAL:
//                                glVertex3f(intersectionPoint1[0], intersectionPoint1[1], p1[2]);
//                                glVertex3f(intersectionPoint2[0], intersectionPoint2[1], p1[2]);
//                                break;
//                        }
                    }
                }
                glEnd();
            }
        }
    }
    
    fixedPipelineDrawing->disableLineAntiAliasing();
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
BrainOpenGLFPVolumeObliqueDrawing::drawSurfaces(BrainOpenGLFixedPipeline* fixedPipelineDrawing,
                                                BrowserTabContent* browserTabContent,
                                                const int viewport[4])
{
    Model* model = browserTabContent->getModelControllerForDisplay();
    Brain* brain = model->getBrain();
    
    std::vector<const Surface*> surfaces = brain->getVolumeInteractionSurfaces();
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    for (std::vector<const Surface*>::iterator iter = surfaces.begin();
         iter != surfaces.end();
         iter++) {
        const Surface* sf = *iter;
        
        const int32_t numNodes = sf->getNumberOfNodes();
        if (numNodes > 0) {
            std::vector<float> nodeColors(numNodes * 4);
            for (int32_t i = 0; i < numNodes; i++) {
                const int32_t i4 = i * 4;
                CaretAssertVectorIndex(nodeColors, i4 + 3);
                nodeColors[i4] = 0.7;
                nodeColors[i4+1] = 0.7;
                nodeColors[i4+2] = 0.7;
                nodeColors[i4+3] = 0.5; // 1.0;
            }
            
            fixedPipelineDrawing->setViewportAndOrthographicProjection(viewport,
                                                                       ProjectionViewTypeEnum::PROJECTION_VIEW_LEFT_LATERAL);
            
            const float center[3] = { 0.0, 0.0, 0.0 };
            fixedPipelineDrawing->applyViewingTransformations(center,
                                              ProjectionViewTypeEnum::PROJECTION_VIEW_LEFT_LATERAL);
            
            glMatrixMode(GL_MODELVIEW);
            glLoadIdentity();
            fixedPipelineDrawing->drawSurface(const_cast<Surface*>(sf),
                                              &nodeColors[0]);
            
        }
    }
    
    glPushMatrix();
    glLoadIdentity();
    
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
    
    double openGLMatrix[16];
    transformationMatrix.getMatrixForOpenGL(openGLMatrix);
    glMultMatrixd(openGLMatrix);
    
    GLboolean cullFaceOn = glIsEnabled(GL_CULL_FACE);
    glDisable(GL_CULL_FACE);

    const float minCoord = -150.0;
    const float maxCoord = 150.0;
    
    const uint8_t maxAlpha = 255;
    
    const uint8_t minIntensity = 100;
    const uint8_t maxIntensity = 255;
    
    const float lineThickness = 3.0;
    
    /*
     * 'Parasagittal' Line
     */
    {
        std::vector<float> coords;
        std::vector<uint8_t> rgba;
        
        coords.push_back(minCoord);
        coords.push_back(0.0);
        coords.push_back(0.0);
        
        rgba.push_back(minIntensity);
        rgba.push_back(0);
        rgba.push_back(0);
        rgba.push_back(maxAlpha);
        
        coords.push_back(maxCoord);
        coords.push_back(0.0);
        coords.push_back(0.0);
        
        rgba.push_back(maxIntensity);
        rgba.push_back(0);
        rgba.push_back(0);
        rgba.push_back(maxAlpha);
        
        drawLines(fixedPipelineDrawing,
                  coords,
                  rgba,
                  lineThickness);
    }
    
    /*
     * 'Cornonal' Line
     */
    {
        std::vector<float> coords;
        std::vector<uint8_t> rgba;
        
        coords.push_back(0.0);
        coords.push_back(minCoord);
        coords.push_back(0.0);
        
        rgba.push_back(0);
        rgba.push_back(minIntensity);
        rgba.push_back(0);
        rgba.push_back(maxAlpha);
        
        coords.push_back(0.0);
        coords.push_back(maxCoord);
        coords.push_back(0.0);
        
        rgba.push_back(0);
        rgba.push_back(maxIntensity);
        rgba.push_back(0);
        rgba.push_back(maxAlpha);
        
        drawLines(fixedPipelineDrawing,
                  coords,
                  rgba,
                  lineThickness);
    }
    
    /*
     * 'Axial' Line
     */
    {
        std::vector<float> coords;
        std::vector<uint8_t> rgba;
        
        coords.push_back(0.0);
        coords.push_back(0.0);
        coords.push_back(minCoord);
        
        rgba.push_back(0);
        rgba.push_back(0);
        rgba.push_back(minIntensity);
        rgba.push_back(maxAlpha);
        
        coords.push_back(0.0);
        coords.push_back(0.0);
        coords.push_back(maxCoord);
        
        rgba.push_back(0);
        rgba.push_back(0);
        rgba.push_back(maxIntensity);
        rgba.push_back(maxAlpha);
        
        drawLines(fixedPipelineDrawing,
                  coords,
                  rgba,
                  lineThickness);
    }

//    /*
//     * 'Axial' Plane
//     */
//    {
//        std::vector<float> coords;
//        std::vector<float> normals;
//        std::vector<uint8_t> rgba;
//        
//        coords.push_back(minCoord);
//        coords.push_back(minCoord);
//        coords.push_back(0.0);
//        rgba.push_back(0);
//        rgba.push_back(0);
//        rgba.push_back(minIntensity);
//        rgba.push_back(alpha);
//        
//        coords.push_back(maxCoord);
//        coords.push_back(minCoord);
//        coords.push_back(0.0);
//        rgba.push_back(0);
//        rgba.push_back(0);
//        rgba.push_back(minIntensity);
//        rgba.push_back(alpha);
//        
//        coords.push_back(maxCoord);
//        coords.push_back(maxCoord);
//        coords.push_back(0.0);
//        rgba.push_back(0);
//        rgba.push_back(0);
//        rgba.push_back(maxIntensity);
//        rgba.push_back(alpha);
//        
//        coords.push_back(minCoord);
//        coords.push_back(maxCoord);
//        coords.push_back(0.0);
//        rgba.push_back(0);
//        rgba.push_back(0);
//        rgba.push_back(maxIntensity);
//        rgba.push_back(alpha);
//        
//        normals.push_back(0.0);
//        normals.push_back(0.0);
//        normals.push_back(1.0);
//
//        for (int32_t i = 0; i < 4; i++) {
//            float* xyz = &coords[i*3];
//            rotationMatrix.multiplyPoint3(xyz);
//        }
//        rotationMatrix.multiplyPoint3(&normals[0]);
//        
//        drawQuads(coords,
//                  normals,
//                  rgba);
//    }
//    
//    /*
//     * 'Coronal'
//     */
//    {
//        std::vector<float> coords;
//        std::vector<float> normals;
//        std::vector<uint8_t> rgba;
//        
//        coords.push_back(minCoord);
//        coords.push_back(0.0);
//        coords.push_back(minCoord);
//        rgba.push_back(0);
//        rgba.push_back(minIntensity);
//        rgba.push_back(0);
//        rgba.push_back(alpha);
//        
//        coords.push_back(maxCoord);
//        coords.push_back(0.0);
//        coords.push_back(minCoord);
//        rgba.push_back(0);
//        rgba.push_back(minIntensity);
//        rgba.push_back(0);
//        rgba.push_back(alpha);
//        
//        coords.push_back(maxCoord);
//        coords.push_back(0.0);
//        coords.push_back(maxCoord);
//        rgba.push_back(0);
//        rgba.push_back(maxIntensity);
//        rgba.push_back(0);
//        rgba.push_back(alpha);
//        
//        coords.push_back(minCoord);
//        coords.push_back(0.0);
//        coords.push_back(maxCoord);
//        rgba.push_back(0);
//        rgba.push_back(maxIntensity);
//        rgba.push_back(0);
//        rgba.push_back(alpha);
//        
//        normals.push_back(0.0);
//        normals.push_back(1.0);
//        normals.push_back(0.0);
//        
//        
//        for (int32_t i = 0; i < 4; i++) {
//            float* xyz = &coords[i*3];
//            rotationMatrix.multiplyPoint3(xyz);
//        }
//        rotationMatrix.multiplyPoint3(&normals[0]);
//        
//        drawQuads(coords,
//                  normals,
//                  rgba);
//    }
//    
//    /*
//     * Parasagittal
//     */
//    {
//        std::vector<float> coords;
//        std::vector<float> normals;
//        std::vector<uint8_t> rgba;
//        
//        coords.push_back(0.0);
//        coords.push_back(minCoord);
//        coords.push_back(minCoord);
//        rgba.push_back(minIntensity);
//        rgba.push_back(0);
//        rgba.push_back(0);
//        rgba.push_back(alpha);
//        
//        coords.push_back(0.0);
//        coords.push_back(maxCoord);
//        coords.push_back(minCoord);
//        rgba.push_back(maxIntensity);
//        rgba.push_back(0);
//        rgba.push_back(0);
//        rgba.push_back(alpha);
//        
//        coords.push_back(0.0);
//        coords.push_back(maxCoord);
//        coords.push_back(maxCoord);
//        rgba.push_back(maxIntensity);
//        rgba.push_back(0);
//        rgba.push_back(0);
//        rgba.push_back(alpha);
//        
//        coords.push_back(0.0);
//        coords.push_back(minCoord);
//        coords.push_back(maxCoord);
//        rgba.push_back(minIntensity);
//        rgba.push_back(0);
//        rgba.push_back(0);
//        rgba.push_back(alpha);
//        
//        normals.push_back(1.0);
//        normals.push_back(0.0);
//        normals.push_back(0.0);
//        
//        
//        for (int32_t i = 0; i < 4; i++) {
//            float* xyz = &coords[i*3];
//            rotationMatrix.multiplyPoint3(xyz);
//        }
//        rotationMatrix.multiplyPoint3(&normals[0]);
//        
//        drawQuads(coords,
//                  normals,
//                  rgba);
//    }
    
    if (cullFaceOn) {
        glEnable(GL_CULL_FACE);
    }
    glDisable(GL_BLEND);
    glPopMatrix();
}

void
BrainOpenGLFPVolumeObliqueDrawing::drawLines(BrainOpenGLFixedPipeline* fixedPipelineDrawing,
                                             const std::vector<float>& coordinates,
                                             const std::vector<uint8_t>& rgbaColors,
                                             const float thickness)
{
    fixedPipelineDrawing->setLineWidth(thickness);
    
    const uint64_t numCoords  = coordinates.size() / 3;
    const uint64_t numColors  = rgbaColors.size() / 4;
    
    const uint64_t numLines = numCoords / 2;  // 2 three-d coords per line
    
    bool oneColorPerVertex = false;
    if (numColors  == numCoords) {
        oneColorPerVertex = true;
    }
    else if (numColors == numLines) {
        oneColorPerVertex = false;
    }
    else {
        const AString message = ("There must be one rgba color per vertex or one rgba color per quad (four vertices)");
        CaretAssertMessage(0, message);
        CaretLogSevere(message);
        return;
    }
    
    const float* coordPtr = &coordinates[0];
    const uint8_t* colorPtr = &rgbaColors[0];
    
    glBegin(GL_LINES);
    uint64_t iColor = 0;
    for (uint64_t i = 0; i < numLines; i++) {
        const uint64_t iCoord = i * 6;
        
        glColor4ubv(&colorPtr[iColor]);
        iColor += 4;
        glVertex3fv(&coordPtr[iCoord]);
        
        if (oneColorPerVertex) {
            glColor4ubv(&colorPtr[iColor]);
            iColor += 4;
        }
        glVertex3fv(&coordPtr[iCoord+3]);
        
    }
    glEnd();
}

void
BrainOpenGLFPVolumeObliqueDrawing::drawQuads(const std::vector<float>& coordinates,
                                             const std::vector<float>& normals,
                                             const std::vector<uint8_t>& rgbaColors)
{
//    drawQuadsVertexArrays(coordinates,
//                          normals,
//                          rgbaColors);
    drawQuadsImmediateMode(coordinates,
                           normals,
                           rgbaColors);
}

void
BrainOpenGLFPVolumeObliqueDrawing::drawQuadsImmediateMode(const std::vector<float>& coordinates,
                            const std::vector<float>& normals,
                            const std::vector<uint8_t>& rgbaColors)
{
    const uint64_t numCoords  = coordinates.size() / 3;
    const uint64_t numNormals = normals.size() / 3;
    const uint64_t numColors  = rgbaColors.size() / 4;
    
    const uint64_t numQuads = numCoords / 4;  // 4 three-d coords per quad
    
    bool oneNormalPerVertex = false;
    if (numNormals == numCoords) {
        oneNormalPerVertex = true;
    }
    else if (numNormals == numQuads) {
        oneNormalPerVertex = false;
    }
    else {
        const AString message = ("There must be one normal per vertex or one normal per quad (four vertices)");
        CaretAssertMessage(0, message);
        CaretLogSevere(message);
        return;
    }
    
    bool oneColorPerVertex = false;
    if (numColors  == numCoords) {
        oneColorPerVertex = true;
    }
    else if (numColors == numQuads) {
        oneColorPerVertex = false;
    }
    else {
        const AString message = ("There must be one rgba color per vertex or one rgba color per quad (four vertices)");
        CaretAssertMessage(0, message);
        CaretLogSevere(message);
        return;
    }
    
    const float* coordPtr = &coordinates[0];
    const float* normalPtr = &normals[0];
    const uint8_t* colorPtr = &rgbaColors[0];
    
    glBegin(GL_QUADS);
    uint64_t iColor = 0;
    uint64_t iNormal = 0;
    for (uint64_t i = 0; i < numQuads; i++) {
        const uint64_t iCoord = i * 12;
        
        glNormal3fv(&normalPtr[iNormal]);
        iNormal += 3;
        glColor4ubv(&colorPtr[iColor]);
        iColor += 4;
        glVertex3fv(&coordPtr[iCoord]);
        
        if (oneNormalPerVertex) {
            glNormal3fv(&normalPtr[iNormal]);
            iNormal += 3;
        }
        if (oneColorPerVertex) {
            glColor4ubv(&colorPtr[iColor]);
            iColor += 4;
        }
        glVertex3fv(&coordPtr[iCoord+3]);
        
        if (oneNormalPerVertex) {
            glNormal3fv(&normalPtr[iNormal]);
            iNormal += 3;
        }
        if (oneColorPerVertex) {
            glColor4ubv(&colorPtr[iColor]);
            iColor += 4;
        }
        glVertex3fv(&coordPtr[iCoord+6]);
        
        if (oneNormalPerVertex) {
            glNormal3fv(&normalPtr[iNormal]);
            iNormal += 3;
        }
        if (oneColorPerVertex) {
            glColor4ubv(&colorPtr[iColor]);
            iColor += 4;
        }
        glVertex3fv(&coordPtr[iCoord+9]);
    }
    glEnd();
    
}

void
BrainOpenGLFPVolumeObliqueDrawing::drawQuadsVertexArrays(const std::vector<float>& coordinates,
                           const std::vector<float>& normals,
                           const std::vector<uint8_t>& rgbaColors)
{
    const uint64_t numCoords  = coordinates.size() / 3;
    const uint64_t numNormals = normals.size() / 3;
    const uint64_t numColors  = rgbaColors.size() / 4;
    
    const uint64_t numQuads = numCoords / 4;  // 4 three-d coords per quad
    
    float* normalPointer = NULL;
    std::vector<float> normalForEachVertex;
    bool oneNormalPerVertex = false;
    if (numNormals == numCoords) {
        oneNormalPerVertex = true;
        normalPointer = const_cast<float*>(&normals[0]);
    }
    else if (numNormals == numQuads) {
        normalForEachVertex.reserve(coordinates.size());
        
        for (uint64_t i = 0; i < numNormals; i++) {
            const uint64_t i3 = i * 3;
            for (uint64_t j = 0; j < 4; j++) {
                normalForEachVertex.push_back(normals[i3]);
                normalForEachVertex.push_back(normals[i3+1]);
                normalForEachVertex.push_back(normals[i3+2]);
            }
        }
        CaretAssert(normalForEachVertex.size() == coordinates.size());
        normalPointer = &normalForEachVertex[0];
        oneNormalPerVertex = false;
    }
    else {
        const AString message = ("There must be one normal per vertex or one normal per quad (four vertices)");
        CaretAssertMessage(0, message);
        CaretLogSevere(message);
        return;
    }
    
    uint8_t* colorPointer = NULL;
    std::vector<uint8_t> colorForEachVertex;
    bool oneColorPerVertex = false;
    if (numColors  == numCoords) {
        oneColorPerVertex = true;
        colorPointer = const_cast<uint8_t*>(&rgbaColors[0]);
    }
    else if (numColors == numQuads) {
        colorForEachVertex.reserve(numColors * 4);
        for (uint64_t i = 0; i < numColors; i++) {
            const uint64_t i4 = i * 4;
            for (uint64_t j = 0; j < 4; j++) {
                colorForEachVertex.push_back(rgbaColors[i4]);
                colorForEachVertex.push_back(rgbaColors[i4+1]);
                colorForEachVertex.push_back(rgbaColors[i4+2]);
                colorForEachVertex.push_back(rgbaColors[i4+3]);
            }
        }
        CaretAssert(colorForEachVertex.size() == numCoords * 4);
        colorPointer = const_cast<uint8_t*>(&colorForEachVertex[0]);
        oneColorPerVertex = false;
    }
    else {
        const AString message = ("There must be one rgba color per vertex or one rgba color per quad (four vertices)");
        CaretAssertMessage(0, message);
        CaretLogSevere(message);
        return;
    }
    
    const float* coordPtr = &coordinates[0];
    
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
    glVertexPointer(3,
                    GL_FLOAT,
                    0,
                    reinterpret_cast<const GLvoid*>(&coordPtr[0]));
    glColorPointer(4,
                   GL_UNSIGNED_BYTE,
                   0,
                   reinterpret_cast<const GLvoid*>(colorPointer));
    glNormalPointer(GL_FLOAT,
                    0,
                    reinterpret_cast<const GLvoid*>(normalPointer));
    
    glDrawArrays(GL_QUADS,
                 0,
                 numCoords);
    
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
}


void
BrainOpenGLFPVolumeObliqueDrawing::drawSliceOld(Brain* brain,
                                             BrainOpenGLFixedPipeline* fixedPipelineDrawing,
                                             BrowserTabContent* browserTabContent,
                                             std::vector<BrainOpenGLFixedPipeline::VolumeDrawInfo>& volumeDrawInfo,
                                             const VolumeSliceViewPlaneEnum::Enum slicePlane,
                                             const bool isSliceView)
{
    AString debugString;
    
    
    const AString planeName = VolumeSliceViewPlaneEnum::toGuiName(slicePlane);
    debugString.appendWithNewLine("\n"
                                  + AString(isSliceView ? "Slice View " : "All View")
                                  + planeName);
    
    CaretAssert(brain);
    
    PaletteFile* paletteFile = brain->getPaletteFile();
    const DisplayPropertiesLabels* dsl = brain->getDisplayPropertiesLabels();
    const DisplayGroupEnum::Enum displayGroup = dsl->getDisplayGroupForTab(fixedPipelineDrawing->windowTabIndex);
    
    
    const int32_t tabIndex = browserTabContent->getTabNumber();
    
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
    
    SelectionItemVoxel* voxelID = brain->getSelectionManager()->getVoxelIdentification();
    
    /*
     * Check for a 'selection' type mode
     */
    bool isSelect = false;
    switch (fixedPipelineDrawing->mode) {
        case BrainOpenGLFixedPipeline::MODE_DRAWING:
            break;
        case BrainOpenGLFixedPipeline::MODE_IDENTIFICATION:
            if (voxelID->isEnabledForSelection()) {
                isSelect = true;
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            }
            else {
                return;
            }
            break;
        case BrainOpenGLFixedPipeline::MODE_PROJECTION:
            return;
            break;
    }
    
    glPushMatrix();
    
    /*
     * When selecting, need to use flat shading so that colors do
     * not get interpolated as identification information is in
     * the color components.
     */
    if (isSelect) {
        glShadeModel(GL_FLAT);
    }
    else {
        glShadeModel(GL_SMOOTH);
    }
    
    /*
     * For identification, five items per voxel
     * 1) volume index
     * 2) map index
     * 3) index I
     * 4) index J
     * 5) index K
     */
    const int32_t idPerVoxelCount = 5;
    std::vector<int32_t> identificationIndices;
    if (isSelect) {
        identificationIndices.reserve(10000 * idPerVoxelCount);
    }
    
    /*
     * Disable culling so that both sides of the triangles/quads are drawn.
     */
    GLboolean cullFaceOn = glIsEnabled(GL_CULL_FACE);
    glDisable(GL_CULL_FACE);
    
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
     * Get coordinate of selected slices
     */
    const float selectedSliceOrigin[3] = {
        browserTabContent->getSliceCoordinateParasagittal(),
        browserTabContent->getSliceCoordinateCoronal(),
        browserTabContent->getSliceCoordinateAxial()
    };
    
    /*
     * Get the rotation matrix
     */
    Matrix4x4 rotationMatrix = browserTabContent->getRotationMatrix();
    
    /*
     * Create the transformation matrix
     */
    Matrix4x4 transformationMatrix;
    transformationMatrix.postmultiply(rotationMatrix);
    
    /*
     * Transform a unit vector for the selected slice view
     * to get the normal vector of the slice.
     */
    float sliceNormalVector[3] = { 0.0, 0.0, 1.0 };
    float sliceOffset[3] = { 0.0, 0.0, 0.0 };
    switch (slicePlane) {
        case VolumeSliceViewPlaneEnum::ALL:
            CaretAssert(0);
            break;
        case VolumeSliceViewPlaneEnum::AXIAL:
            sliceNormalVector[0] = 0.0;
            sliceNormalVector[1] = 0.0;
            sliceNormalVector[2] = 1.0;
            sliceOffset[2] = selectedSliceOrigin[2];
            break;
        case VolumeSliceViewPlaneEnum::CORONAL:
            sliceNormalVector[0] = 0.0;
            sliceNormalVector[1] = -1.0;
            sliceNormalVector[2] = 0.0;
            sliceOffset[1] = selectedSliceOrigin[1];
            break;
        case VolumeSliceViewPlaneEnum::PARASAGITTAL:
            sliceNormalVector[0] = -1.0;
            sliceNormalVector[1] = 0.0;
            sliceNormalVector[2] = 0.0;
            sliceOffset[0] = selectedSliceOrigin[0];
            break;
    }
    
    /*
     * Translate for selected slice
     */
    transformationMatrix.translate(sliceOffset[0],
                                   sliceOffset[1],
                                   sliceOffset[2]);
    
    /*
     * Transform the slice normal vector so that it
     * points orthogonal to the slice by rotating the
     * vector with the rotation matrix.
     */
    rotationMatrix.multiplyPoint3(sliceNormalVector);
    MathFunctions::normalizeVector(sliceNormalVector);
    
    
    debugString.appendWithNewLine("Slice normal vector: "
                                  + AString::fromNumbers(sliceNormalVector, 3, ", "));
    
    {
        Plane normalPlane(sliceNormalVector,
                          sliceOffset);
        debugString.appendWithNewLine("Plane from normal/pt: "
                                      + normalPlane.toString());
    }
    
    /*
     * If this is a volume slice view,
     * (1) Set the 'center' to the center of the slice
     * (2) Set the 'eye' to an offset from the slice
     * (3) Set the 'up vector'
     * (4) Use the above with gluLookAt to set the viewing position
     */
    if (isSliceView) {
        /*
         * Set the "up" vector for the slice
         */
        float upVector[3] = { 0.0, 0.0, 0.0 };
        switch (slicePlane) {
            case VolumeSliceViewPlaneEnum::ALL:
                CaretAssert(0);
                break;
            case VolumeSliceViewPlaneEnum::AXIAL:
                upVector[0] = 0.0;
                upVector[1] = 1.0;
                upVector[2] = 0.0;
                break;
            case VolumeSliceViewPlaneEnum::CORONAL:
                upVector[0] = 0.0;
                upVector[1] = 0.0;
                upVector[2] = 1.0;
                break;
            case VolumeSliceViewPlaneEnum::PARASAGITTAL:
                upVector[0] = 0.0;
                upVector[1] = 0.0;
                upVector[2] = 1.0;
                break;
        }
        
        float center[3] = {
            sliceOffset[0],
            sliceOffset[1],
            sliceOffset[2]
        };
        
        const float distanceFromEyeToCenter = 100.0;
        const float eye[3] = {
            center[0] + sliceNormalVector[0] * distanceFromEyeToCenter,
            center[1] + sliceNormalVector[1] * distanceFromEyeToCenter,
            center[2] + sliceNormalVector[2] * distanceFromEyeToCenter
        };
        
        /*
         * Need to orient the "up vector"
         */
        rotationMatrix.multiplyPoint3(upVector);
        MathFunctions::normalizeVector(upVector);
        
        /*
         * Use gluLookAt to view the slice
         */
        gluLookAt(eye[0],
                  eye[1],
                  eye[2],
                  center[0],
                  center[1],
                  center[2],
                  upVector[0],
                  upVector[1],
                  upVector[2]);
        
        debugString.appendWithNewLine("Center: " + AString::fromNumbers(center, 3, ", "));
        debugString.appendWithNewLine("Eye: " + AString::fromNumbers(eye, 3, ", "));
        debugString.appendWithNewLine("Up: " + AString::fromNumbers(upVector, 3, ", "));
    }
    else {
        /*
         * Drawing in All (3D) view so need to translate
         * so that the slice is drawn in the correct location.
         */
        //        glTranslatef(sliceOffset[0],
        //                     sliceOffset[1],
        //                     sliceOffset[2]);
    }
    
    
    BoundingBox sliceBoundingBox;
    sliceBoundingBox.resetForUpdate();
    const int32_t numVolumes = static_cast<int32_t>(volumeDrawInfo.size());
    for (int32_t iVol = 0; iVol < numVolumes; iVol++) {
        const BrainOpenGLFixedPipeline::VolumeDrawInfo& vdi = volumeDrawInfo[iVol];
        const VolumeMappableInterface* volInter = vdi.volumeFile;
        BoundingBox boundingBox;
        volInter->getVoxelSpaceBoundingBox(boundingBox);
        
        sliceBoundingBox.update(boundingBox.getMinXYZ());
        sliceBoundingBox.update(boundingBox.getMaxXYZ());
    }
    
    float cornerCoord = std::fabs(sliceBoundingBox.getMinX());
    cornerCoord = std::max(cornerCoord, std::fabs(sliceBoundingBox.getMaxX()));
    cornerCoord = std::max(cornerCoord, std::fabs(sliceBoundingBox.getMinY()));
    cornerCoord = std::max(cornerCoord, std::fabs(sliceBoundingBox.getMaxY()));
    cornerCoord = std::max(cornerCoord, std::fabs(sliceBoundingBox.getMinZ()));
    cornerCoord = std::max(cornerCoord, std::fabs(sliceBoundingBox.getMaxZ()));
    
    /*
     * Get coordinates of virtual slice corners
     * MAY NEED ASPECT RATION OF VIEWPORT
     */
    float leftBottomCorner[3] = { 0.0, 0.0, 0.0 };
    float rightBottomCorner[3] = { 0.0, 0.0, 0.0 };
    float rightTopCorner[3] = { 0.0, 0.0, 0.0 };
    float leftTopCorner[3] = { 0.0, 0.0, 0.0 };
    
    switch (slicePlane) {
        case VolumeSliceViewPlaneEnum::ALL:
            CaretAssert(0);
            break;
        case VolumeSliceViewPlaneEnum::AXIAL:
            leftBottomCorner[0] = -cornerCoord;
            leftBottomCorner[1] = -cornerCoord;
            leftBottomCorner[2] = 0.0;
            leftTopCorner[0] = -cornerCoord;
            leftTopCorner[1] = cornerCoord;
            leftTopCorner[2] = 0.0;
            rightBottomCorner[0] = cornerCoord;
            rightBottomCorner[1] = -cornerCoord;
            rightBottomCorner[2] = 0.0;
            rightTopCorner[0] = cornerCoord;
            rightTopCorner[1] = cornerCoord;
            rightTopCorner[2] = 0.0;
            break;
        case VolumeSliceViewPlaneEnum::CORONAL:
            leftBottomCorner[0] = -cornerCoord;
            leftBottomCorner[1] = 0.0;
            leftBottomCorner[2] = -cornerCoord;
            leftTopCorner[0] = -cornerCoord;
            leftTopCorner[1] = 0.0;
            leftTopCorner[2] = cornerCoord;
            rightBottomCorner[0] = cornerCoord;
            rightBottomCorner[1] = 0.0;
            rightBottomCorner[2] = -cornerCoord;
            rightTopCorner[0] = cornerCoord;
            rightTopCorner[1] = 0.0;
            rightTopCorner[2] = cornerCoord;
            break;
        case VolumeSliceViewPlaneEnum::PARASAGITTAL:
            //            leftBottomCorner[0] = 0.0;
            //            leftBottomCorner[1] = -cornerCoord;
            //            leftBottomCorner[2] = -cornerCoord;
            //            leftTopCorner[0] = 0.0;
            //            leftTopCorner[1] = -cornerCoord;
            //            leftTopCorner[2] = cornerCoord;
            //            rightBottomCorner[0] = 0.0;
            //            rightBottomCorner[1] = cornerCoord;
            //            rightBottomCorner[2] = -cornerCoord;
            //            rightTopCorner[0] = 0.0;
            //            rightTopCorner[1] = cornerCoord;
            //            rightTopCorner[2] = cornerCoord;
            leftBottomCorner[0] = 0.0;
            leftBottomCorner[1] = cornerCoord;
            leftBottomCorner[2] = -cornerCoord;
            leftTopCorner[0] = 0.0;
            leftTopCorner[1] = cornerCoord;
            leftTopCorner[2] = cornerCoord;
            rightBottomCorner[0] = 0.0;
            rightBottomCorner[1] = -cornerCoord;
            rightBottomCorner[2] = -cornerCoord;
            rightTopCorner[0] = 0.0;
            rightTopCorner[1] = -cornerCoord;
            rightTopCorner[2] = cornerCoord;
            break;
    }
    
    transformationMatrix.multiplyPoint3(leftBottomCorner);
    transformationMatrix.multiplyPoint3(leftTopCorner);
    transformationMatrix.multiplyPoint3(rightBottomCorner);
    transformationMatrix.multiplyPoint3(rightTopCorner);
    //    std::cout << "Left bottom: " << qPrintable(AString::fromNumbers(leftBottomCorner, 3, ", ")) << std::endl;
    //    std::cout << "Left top: " << qPrintable(AString::fromNumbers(leftTopCorner, 3, ", ")) << std::endl;
    //    std::cout << "Right bottom: " << qPrintable(AString::fromNumbers(rightBottomCorner, 3, ", ")) << std::endl;
    //    std::cout << "Right top: " << qPrintable(AString::fromNumbers(rightTopCorner, 3, ", ")) << std::endl;
    
    Plane plane(leftBottomCorner,
                rightBottomCorner,
                rightTopCorner);
    debugString.appendWithNewLine("Plane from corners: "
                                  + plane.toString());
    
    float coordsForPlane[3][3];
    bool coordsForPlaneValid = false;
    
    const float zoom = browserTabContent->getScaling();
    glScalef(zoom, zoom, zoom);
    
    /*
     * Set the voxels in the 'virtual' slice
     */
    for (float x = minScreenX; x < maxScreenX; x += minVoxelSize) {
        for (float y = minScreenY; y < maxScreenY; y += minVoxelSize) {
            /*
             * Apply the transformation matrix to the point
             */
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
                        if ((rgba[0] > 0)
                            && (rgba[1] > 0)
                            && (rgba[2] > 0)) {
                            if (isSelect) {
                                const int32_t idIndex = identificationIndices.size() / idPerVoxelCount;
                                fixedPipelineDrawing->colorIdentification->addItem(rgba,
                                                                                   SelectionItemDataTypeEnum::VOXEL,
                                                                                   idIndex);
                                rgba[3] = 255;
                                identificationIndices.push_back(iVol);
                                identificationIndices.push_back(vdi.mapIndex);
                                identificationIndices.push_back(voxelI);
                                identificationIndices.push_back(voxelJ);
                                identificationIndices.push_back(voxelK);
                            }
                            
                            voxelRGBA[0] = rgba[0];
                            voxelRGBA[1] = rgba[1];
                            voxelRGBA[2] = rgba[2];
                            voxelRGBA[3] = rgba[3];
                        }
                    }
                }
            }
            
            if (voxelRGBA[3] > 0) {
                float pt1[3];
                float pt2[3];
                float pt3[3];
                float pt4[3];
                
                const float xmin = x - halfVoxelSize;
                const float xmax = x + halfVoxelSize;
                const float ymin = y - halfVoxelSize;
                const float ymax = y + halfVoxelSize;
                
                switch (slicePlane) {
                    case VolumeSliceViewPlaneEnum::ALL:
                        CaretAssert(0);
                        break;
                    case VolumeSliceViewPlaneEnum::AXIAL:
                        pt1[0] = xmin;
                        pt1[1] = ymin;
                        pt1[2] = 0.0;
                        pt2[0] = xmax;
                        pt2[1] = ymin;
                        pt2[2] = 0.0;
                        pt3[0] = xmax;
                        pt3[1] = ymax;
                        pt3[2] = 0.0;
                        pt4[0] = xmin;
                        pt4[1] = ymax;
                        pt4[2] = 0.0;
                        break;
                    case VolumeSliceViewPlaneEnum::CORONAL:
                        pt1[0] = xmin;
                        pt1[1] = 0.0;
                        pt1[2] = ymin;
                        pt2[0] = xmax;
                        pt2[1] = 0.0;
                        pt2[2] = ymin;
                        pt3[0] = xmax;
                        pt3[1] = 0.0;
                        pt3[2] = ymax;
                        pt4[0] = xmin;
                        pt4[1] = 0.0;
                        pt4[2] = ymax;
                        break;
                    case VolumeSliceViewPlaneEnum::PARASAGITTAL:
                        //                        pt1[0] = 0.0;
                        //                        pt1[1] = xmin;
                        //                        pt1[2] = ymin;
                        //                        pt2[0] = 0.0;
                        //                        pt2[1] = xmax;
                        //                        pt2[2] = ymin;
                        //                        pt3[0] = 0.0;
                        //                        pt3[1] = xmax;
                        //                        pt3[2] = ymax;
                        //                        pt4[0] = 0.0;
                        //                        pt4[1] = xmin;
                        //                        pt4[2] = ymax;
                        pt1[0] = 0.0;
                        pt1[1] = xmax;
                        pt1[2] = ymin;
                        pt2[0] = 0.0;
                        pt2[1] = xmin;
                        pt2[2] = ymin;
                        pt3[0] = 0.0;
                        pt3[1] = xmin;
                        pt3[2] = ymax;
                        pt4[0] = 0.0;
                        pt4[1] = xmax;
                        pt4[2] = ymax;
                        break;
                }
                
                //                rotationMatrix.multiplyPoint3(pt1);
                //                rotationMatrix.multiplyPoint3(pt2);
                //                rotationMatrix.multiplyPoint3(pt3);
                //                rotationMatrix.multiplyPoint3(pt4);
                transformationMatrix.multiplyPoint3(pt1);
                transformationMatrix.multiplyPoint3(pt2);
                transformationMatrix.multiplyPoint3(pt3);
                transformationMatrix.multiplyPoint3(pt4);
                
                
                quadRGBAs.push_back(voxelRGBA[0]);
                quadRGBAs.push_back(voxelRGBA[1]);
                quadRGBAs.push_back(voxelRGBA[2]);
                quadRGBAs.push_back(voxelRGBA[3]);
                
                quadNormals.push_back(sliceNormalVector[0]);
                quadNormals.push_back(sliceNormalVector[1]);
                quadNormals.push_back(sliceNormalVector[2]);
                
                quadCoords.push_back(pt1[0]);
                quadCoords.push_back(pt1[1]);
                quadCoords.push_back(pt1[2]);
                
                quadCoords.push_back(pt2[0]);
                quadCoords.push_back(pt2[1]);
                quadCoords.push_back(pt2[2]);
                
                quadCoords.push_back(pt3[0]);
                quadCoords.push_back(pt3[1]);
                quadCoords.push_back(pt3[2]);
                
                quadCoords.push_back(pt4[0]);
                quadCoords.push_back(pt4[1]);
                quadCoords.push_back(pt4[2]);
                
                if ( ! coordsForPlaneValid) {
                    coordsForPlane[0][0] = pt1[0];
                    coordsForPlane[0][1] = pt1[1];
                    coordsForPlane[0][2] = pt1[2];
                    coordsForPlane[1][0] = pt2[0];
                    coordsForPlane[1][1] = pt2[1];
                    coordsForPlane[1][2] = pt2[2];
                    coordsForPlane[2][0] = pt3[0];
                    coordsForPlane[2][1] = pt3[1];
                    coordsForPlane[2][2] = pt3[2];
                    coordsForPlaneValid = true;
                }
            }
        }
    }
    
    if (coordsForPlaneValid) {
        Plane quadPlane(coordsForPlane[0],
                        coordsForPlane[1],
                        coordsForPlane[2]);
        debugString.appendWithNewLine("Plane from first quad: "
                                      + quadPlane.toString());
    }
    
    if (false == quadCoords.empty()) {
        drawQuads(quadCoords,
                  quadNormals,
                  quadRGBAs);
        
        
        glPushMatrix();
        
        if ( ! isSelect) {
            if (coordsForPlaneValid) {
                Plane slicePlane(coordsForPlane[0],
                                 coordsForPlane[1],
                                 coordsForPlane[2]);
                if (slicePlane.isValidPlane()) {
                    fixedPipelineDrawing->drawFiberOrientations(&slicePlane);
                    fixedPipelineDrawing->drawFiberTrajectories(&slicePlane);
                    
                    drawSurfaceOutline(fixedPipelineDrawing,
                                       browserTabContent,
                                       volumeDrawInfo[0].volumeFile,
                                       plane);
                }
            }
        }
        
        glPopMatrix();
    }
    
    if (isSelect) {
        int32_t identifiedItemIndex;
        float depth = -1.0;
        fixedPipelineDrawing->getIndexFromColorSelection(SelectionItemDataTypeEnum::VOXEL,
                                                         fixedPipelineDrawing->mouseX,
                                                         fixedPipelineDrawing->mouseY,
                                                         identifiedItemIndex,
                                                         depth);
        if (identifiedItemIndex >= 0) {
            const int32_t idIndex = identifiedItemIndex * idPerVoxelCount;
            const int32_t volDrawInfoIndex = identificationIndices[idIndex];
            CaretAssertVectorIndex(volumeDrawInfo, volDrawInfoIndex);
            VolumeMappableInterface* vf = volumeDrawInfo[volDrawInfoIndex].volumeFile;
            //const int32_t mapIndex = identificationIndices[idIndex + 1];
            const int64_t voxelIndices[3] = {
                identificationIndices[idIndex + 2],
                identificationIndices[idIndex + 3],
                identificationIndices[idIndex + 4]
            };
            
            if (voxelID->isOtherScreenDepthCloserToViewer(depth)) {
                voxelID->setVolumeFile(vf);
                voxelID->setVoxelIJK(voxelIndices);
                voxelID->setScreenDepth(depth);
                
                float voxelCoordinates[3];
                vf->indexToSpace(voxelIndices[0], voxelIndices[1], voxelIndices[2],
                                 voxelCoordinates[0], voxelCoordinates[1], voxelCoordinates[2]);
                
                fixedPipelineDrawing->setSelectedItemScreenXYZ(voxelID,
                                                               voxelCoordinates);
                CaretLogFine("Selected Voxel (3D): " + AString::fromNumbers(voxelIndices, 3, ","));
            }
        }
    }
    if (cullFaceOn) {
        glEnable(GL_CULL_FACE);
    }
    
    glPopMatrix();
    
    //std::cout << qPrintable(debugString) << std::endl;
}
