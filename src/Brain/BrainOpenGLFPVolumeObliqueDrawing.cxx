
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
            
//            const int surfaceVP[4] = {
//                viewport[0],
//                viewport[1],
//                vpHalfX,
//                vpHalfY
//            };
//            drawSurfaces(fixedPipelineDrawing,
//                         browserTabContent,
//                         surfaceVP);
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
                                            const VolumeSliceViewPlaneEnum::Enum slicePlane,
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
BrainOpenGLFPVolumeObliqueDrawing::drawFibers(BrainOpenGLFixedPipeline* fixedPipelineDrawing,
           const VolumeSliceViewPlaneEnum::Enum slicePlane,
           const Matrix4x4& rotationMatrix)
{
//    double m16[16];
//    Matrix4x4 inverseMatrix(rotationMatrix);
//    inverseMatrix.invert();
//    inverseMatrix.getMatrixForOpenGL(m16);
//    glPushMatrix();
//    glMultMatrixd(m16);
    
    /*
     * Apply the transformation matrix to the point
     */
    const float screenCorners[3][2] = {
        { fixedPipelineDrawing->orthographicLeft, fixedPipelineDrawing->orthographicBottom },
        { fixedPipelineDrawing->orthographicRight, fixedPipelineDrawing->orthographicBottom },
        { fixedPipelineDrawing->orthographicRight, fixedPipelineDrawing->orthographicTop }
    };
    
    float p1[3];
    float p2[3];
    float p3[3];
    switch (slicePlane) {
        case VolumeSliceViewPlaneEnum::ALL:
            CaretAssert(0);
            break;
        case VolumeSliceViewPlaneEnum::AXIAL:
            p1[0] = screenCorners[0][0];
            p1[1] = screenCorners[0][1];
            p1[2] = 0;
            p2[0] = screenCorners[1][0];
            p2[1] = screenCorners[1][1];
            p2[2] = 0;
            p3[0] = screenCorners[2][0];
            p3[1] = screenCorners[2][1];
            p3[2] = 0;
            break;
        case VolumeSliceViewPlaneEnum::CORONAL:
            p1[0] = screenCorners[0][0];
            p1[1] = 0;
            p1[2] = screenCorners[0][1];
            p2[0] = screenCorners[1][0];
            p2[1] = 0;
            p2[2] = screenCorners[1][1];
            p3[0] = screenCorners[2][0];
            p3[1] = 0;
            p3[2] = screenCorners[2][1];
            break;
        case VolumeSliceViewPlaneEnum::PARASAGITTAL:
            p1[0] = 0;
            p1[1] = screenCorners[0][0];
            p1[2] = screenCorners[0][1];
            p2[0] = 0;
            p2[1] = screenCorners[1][0];
            p2[2] = screenCorners[1][1];
            p3[0] = 0;
            p3[1] = screenCorners[2][0];
            p3[2] = screenCorners[2][1];
            break;
    }
    
    /*
     * Transforms screen into plane
     */
    rotationMatrix.multiplyPoint3(p1);
    rotationMatrix.multiplyPoint3(p2);
    rotationMatrix.multiplyPoint3(p3);
    
    Plane plane(p1, p2, p3);
//    Plane plane(p3, p2, p1);
    if (plane.isValidPlane()) {
        fixedPipelineDrawing->drawFiberOrientations(&plane);
        fixedPipelineDrawing->drawFiberTrajectories(&plane);
    }
    
//    glPopMatrix();
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
                                        const VolumeSliceViewPlaneEnum::Enum slicePlane,
                                        const bool setViewingTransformForVolumeSliceView)
{
    CaretAssert(brain);
//    ModelVolume* volumeModel = browserTabContent->getDisplayedVolumeModel();
//    CaretAssert(volumeModel);

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
//    const float minScreenZ = fixedPipelineDrawing->orthographicNear;
    
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
     * Get coordinate of rotation point
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
     *
     * 1) Translate so rotation point at origin
     * 2) Rotate
     * 3) Translate back to rotation point
     */
    Matrix4x4 transformationMatrix;
    transformationMatrix.translate(-selectedSliceOrigin[0],
                                   -selectedSliceOrigin[1],
                                   -selectedSliceOrigin[2]);
    transformationMatrix.postmultiply(rotationMatrix);
//    transformationMatrix.postmultiply(rotationMatrix);
    transformationMatrix.translate(selectedSliceOrigin[0],
                                   selectedSliceOrigin[1],
                                   selectedSliceOrigin[2]);
    
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
            sliceNormalVector[0] = 1.0;
            sliceNormalVector[1] = 0.0;
            sliceNormalVector[2] = 0.0;
            sliceOffset[0] = selectedSliceOrigin[0];
            break;
    }
    transformationMatrix.translate(sliceOffset[0],
                             sliceOffset[1],
                             sliceOffset[2]);
    transformationMatrix.multiplyPoint3(sliceNormalVector);
//    rotationMatrix.multiplyPoint3(sliceNormalVector);
    MathFunctions::normalizeVector(sliceNormalVector);
    
    
    std::cout
    << qPrintable(VolumeSliceViewPlaneEnum::toGuiName(slicePlane))
    << " slice normal vector: "
    << qPrintable(AString::fromNumbers(sliceNormalVector, 3, ", "))
    << std::endl;
    
    
    if (setViewingTransformForVolumeSliceView) {
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
        
        const float distanceFromEyeToCenter = 100.0;
        const float eye[3] = {
            sliceNormalVector[0] * distanceFromEyeToCenter,
            sliceNormalVector[1] * distanceFromEyeToCenter,
            sliceNormalVector[2] * distanceFromEyeToCenter
        };
        
        //    transformationMatrix.multiplyPoint3(upVector);
        rotationMatrix.multiplyPoint3(upVector);
        MathFunctions::normalizeVector(upVector);
        
        /*
         * Use gluLookAt to view the slice
         */
        gluLookAt(eye[0],
                  eye[1],
                  eye[2],
                  selectedSliceOrigin[0],
                  selectedSliceOrigin[1],
                  selectedSliceOrigin[2],
                  upVector[0],
                  upVector[1],
                  upVector[2]);
        
        std::cout << "Center: " << qPrintable(AString::fromNumbers(selectedSliceOrigin, 3, ", ")) << std::endl;
        std::cout << "Eye: " << qPrintable(AString::fromNumbers(eye, 3, ", ")) << std::endl;
        std::cout << "up: " << qPrintable(AString::fromNumbers(upVector, 3, ", ")) << std::endl;
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
    
//    float halfVoxelSizeX = halfVoxelSize;
//    float halfVoxelSizeY = halfVoxelSize;
//    float halfVoxelSizeZ = halfVoxelSize;
    
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
//            halfVoxelSizeZ = 0.0;
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
//            halfVoxelSizeY = 0.0;
            break;
        case VolumeSliceViewPlaneEnum::PARASAGITTAL:
            leftBottomCorner[0] = 0.0;
            leftBottomCorner[1] = -cornerCoord;
            leftBottomCorner[2] = -cornerCoord;
            leftTopCorner[0] = 0.0;
            leftTopCorner[1] = -cornerCoord;
            leftTopCorner[2] = cornerCoord;
            rightBottomCorner[0] = 0.0;
            rightBottomCorner[1] = cornerCoord;
            rightBottomCorner[2] = -cornerCoord;
            rightTopCorner[0] = 0.0;
            rightTopCorner[1] = cornerCoord;
            rightTopCorner[2] = cornerCoord;
//            halfVoxelSizeX = 0.0;
            break;
    }

    transformationMatrix.multiplyPoint3(leftBottomCorner);
    transformationMatrix.multiplyPoint3(leftTopCorner);
    transformationMatrix.multiplyPoint3(rightBottomCorner);
    transformationMatrix.multiplyPoint3(rightTopCorner);
//    rotationMatrix.multiplyPoint3(leftBottomCorner);
//    rotationMatrix.multiplyPoint3(leftTopCorner);
//    rotationMatrix.multiplyPoint3(rightBottomCorner);
//    rotationMatrix.multiplyPoint3(rightTopCorner);
//    std::cout << "Left bottom: " << qPrintable(AString::fromNumbers(leftBottomCorner, 3, ", ")) << std::endl;
//    std::cout << "Left top: " << qPrintable(AString::fromNumbers(leftTopCorner, 3, ", ")) << std::endl;
//    std::cout << "Right bottom: " << qPrintable(AString::fromNumbers(rightBottomCorner, 3, ", ")) << std::endl;
//    std::cout << "Right top: " << qPrintable(AString::fromNumbers(rightTopCorner, 3, ", ")) << std::endl;
    
    Plane plane(leftBottomCorner,
                rightBottomCorner,
                rightTopCorner);
    std::cout << qPrintable(VolumeSliceViewPlaneEnum::toGuiName(slicePlane))
    << " plane: " << qPrintable(plane.toString()) << std::endl;
    
    
//    for (float x = leftBottomCorner[0]; x <= leftTopCorner[0]; x += minVoxelSize) {
//        for (float y = leftBottomCorner[1]; y <= leftTopCorner[1]; y += minVoxelSize) {
//            for (float z = leftBottomCorner[2]; z <= leftTopCorner[2]; z += minVoxelSize) {
//                /*
//                 * Find the voxel in each volume for drawing
//                 */
//                uint8_t voxelRGBA[4] = { 0, 0, 0, 0 };
//                
//                for (int32_t iVol = 0; iVol < numVolumes; iVol++) {
//                    const BrainOpenGLFixedPipeline::VolumeDrawInfo& vdi = volumeDrawInfo[iVol];
//                    const VolumeMappableInterface* volInter = vdi.volumeFile;
//                    int64_t voxelI, voxelJ, voxelK;
//                    volInter->enclosingVoxel(x, y, z,
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
//                    quadRGBAs.push_back(voxelRGBA[0]);
//                    quadRGBAs.push_back(voxelRGBA[1]);
//                    quadRGBAs.push_back(voxelRGBA[2]);
//                    quadRGBAs.push_back(voxelRGBA[3]);
//                    
//                    quadNormals.push_back(sliceNormalVector[0]);
//                    quadNormals.push_back(sliceNormalVector[1]);
//                    quadNormals.push_back(sliceNormalVector[2]);
//                    
//                    const float xmin = x - halfVoxelSizeX;
//                    const float xmax = x + halfVoxelSizeX;
//                    const float ymin = y - halfVoxelSizeY;
//                    const float ymax = y + halfVoxelSizeY;
//                    const float zmin = z - halfVoxelSizeZ;
//                    const float zmax = z + halfVoxelSizeZ;
//                    
//                    switch (slicePlane) {
//                        case VolumeSliceViewPlaneEnum::ALL:
//                            CaretAssert(0);
//                            break;
//                        case VolumeSliceViewPlaneEnum::AXIAL:
//                            break;
//                            quadCoords.push_back(xmin);
//                            quadCoords.push_back(ymin);
//                            quadCoords.push_back(zmin);
//                            
//                            quadCoords.push_back(xmax);
//                            quadCoords.push_back(ymin);
//                            quadCoords.push_back(zmin);
//                            
//                            quadCoords.push_back(xmax);
//                            quadCoords.push_back(ymax);
//                            quadCoords.push_back(0.0);
//                            
//                            quadCoords.push_back(xmin);
//                            quadCoords.push_back(ymax);
//                            quadCoords.push_back(0.0);
//                        case VolumeSliceViewPlaneEnum::CORONAL:
//                            break;
//                        case VolumeSliceViewPlaneEnum::PARASAGITTAL:
//                            break;
//                    }
//                }
//            }
//        }
//    }
    
    
    Matrix4x4 inverseMatrix(transformationMatrix);
    inverseMatrix.invert();
    
    float coordsForPlane[3][3];
    bool coordsForPlaneValid = false;
    
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
//            rotationMatrix.multiplyPoint3(pt);
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
                        pt1[0] = 0.0;
                        pt1[1] = xmin;
                        pt1[2] = ymin;
                        pt2[0] = 0.0;
                        pt2[1] = xmax;
                        pt2[2] = ymin;
                        pt3[0] = 0.0;
                        pt3[1] = xmax;
                        pt3[2] = ymax;
                        pt4[0] = 0.0;
                        pt4[1] = xmin;
                        pt4[2] = ymax;
                        break;
                }
                rotationMatrix.multiplyPoint3(pt1);
                rotationMatrix.multiplyPoint3(pt2);
                rotationMatrix.multiplyPoint3(pt3);
                rotationMatrix.multiplyPoint3(pt4);
                
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
                }
                //        drawFibers(fixedPipelineDrawing,
                //                   slicePlane,
                //                   rotationMatrix);
                //        drawAxis(fixedPipelineDrawing,
                //                 slicePlane,
                //                 rotationMatrix);
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
}

void
BrainOpenGLFPVolumeObliqueDrawing::drawSliceForSliceViewOld(BrainOpenGLFixedPipeline* fixedPipelineDrawing,
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
     * Initialized the model view matrix
     */
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    /*
     * Translate to select sliced coordinates
     */
    
    /*
     * Get corners of the selected slice
     */
    
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
    //    const float minScreenZ = fixedPipelineDrawing->orthographicNear;
    
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
    const float center[3] = {
        browserTabContent->getSliceCoordinateParasagittal(),
        browserTabContent->getSliceCoordinateCoronal(),
        browserTabContent->getSliceCoordinateAxial()
    };
    //    const float centerX = browserTabContent->getSliceCoordinateParasagittal();
    //    const float centerY = browserTabContent->getSliceCoordinateCoronal();
    //    const float centerZ = browserTabContent->getSliceCoordinateAxial();
    Matrix4x4 transformationMatrix;
    transformationMatrix.translate(center[0],
                                   center[1],
                                   center[2]);
    //    transformationMatrix.translate(centerX,
    //                                   centerY,
    //                                   centerZ);
    
    /*
     * Apply rotation
     */
    Matrix4x4 rotationMatrix = browserTabContent->getRotationMatrix();
    transformationMatrix.premultiply(rotationMatrix);
    //    transformationMatrix.postmultiply(rotationMatrix);
    
    /*
     * Transform a unit vector for the selected slice view
     * to get the normal vector of the slice.
     *
     * Also set the "up" vector for the slice
     */
    float sliceNormalVector[3] = { 0.0, 0.0, 1.0 };
    float upVector[3] = { 0.0, 0.0, 0.0 };
    switch (slicePlane) {
        case VolumeSliceViewPlaneEnum::ALL:
            CaretAssert(0);
            break;
        case VolumeSliceViewPlaneEnum::AXIAL:
            sliceNormalVector[0] = 0.0;
            sliceNormalVector[1] = 0.0;
            sliceNormalVector[2] = 1.0;
            upVector[0] = 0.0;
            upVector[1] = 1.0;
            upVector[2] = 0.0;
            break;
        case VolumeSliceViewPlaneEnum::CORONAL:
            sliceNormalVector[0] = 0.0;
            sliceNormalVector[1] = -1.0;
            sliceNormalVector[2] = 0.0;
            upVector[0] = 0.0;
            upVector[1] = 0.0;
            upVector[2] = 1.0;
            break;
        case VolumeSliceViewPlaneEnum::PARASAGITTAL:
            sliceNormalVector[0] = 1.0;
            sliceNormalVector[1] = 0.0;
            sliceNormalVector[2] = 0.0;
            upVector[0] = 0.0;
            upVector[1] = 0.0;
            upVector[2] = 1.0;
            break;
    }
    //    transformationMatrix.multiplyPoint3(sliceNormalVector);
    rotationMatrix.multiplyPoint3(sliceNormalVector);
    MathFunctions::normalizeVector(sliceNormalVector);
    
    std::cout
    << "Slice normal vector: "
    << qPrintable(AString::fromNumbers(sliceNormalVector, 3, ", "))
    << std::endl;
    
    const float distanceFromEyeToCenter = 100.0;
    const float eye[3] = {
        sliceNormalVector[0] * distanceFromEyeToCenter,
        sliceNormalVector[1] * distanceFromEyeToCenter,
        sliceNormalVector[2] * distanceFromEyeToCenter
    };
    
    //    transformationMatrix.multiplyPoint3(upVector);
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
    
    std::cout << "Center: " << qPrintable(AString::fromNumbers(center, 3, ", ")) << std::endl;
    std::cout << "Eye: " << qPrintable(AString::fromNumbers(eye, 3, ", ")) << std::endl;
    std::cout << "up: " << qPrintable(AString::fromNumbers(upVector, 3, ", ")) << std::endl;
    
    
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
    
    //    float halfVoxelSizeX = halfVoxelSize;
    //    float halfVoxelSizeY = halfVoxelSize;
    //    float halfVoxelSizeZ = halfVoxelSize;
    
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
            //            halfVoxelSizeZ = 0.0;
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
            //            halfVoxelSizeY = 0.0;
            break;
        case VolumeSliceViewPlaneEnum::PARASAGITTAL:
            leftBottomCorner[0] = 0.0;
            leftBottomCorner[1] = -cornerCoord;
            leftBottomCorner[2] = -cornerCoord;
            leftTopCorner[0] = 0.0;
            leftTopCorner[1] = -cornerCoord;
            leftTopCorner[2] = cornerCoord;
            rightBottomCorner[0] = 0.0;
            rightBottomCorner[1] = cornerCoord;
            rightBottomCorner[2] = -cornerCoord;
            rightTopCorner[0] = 0.0;
            rightTopCorner[1] = cornerCoord;
            rightTopCorner[2] = cornerCoord;
            //            halfVoxelSizeX = 0.0;
            break;
    }
    
    transformationMatrix.multiplyPoint3(leftBottomCorner);
    transformationMatrix.multiplyPoint3(leftTopCorner);
    transformationMatrix.multiplyPoint3(rightBottomCorner);
    transformationMatrix.multiplyPoint3(rightTopCorner);
    //    rotationMatrix.multiplyPoint3(leftBottomCorner);
    //    rotationMatrix.multiplyPoint3(leftTopCorner);
    //    rotationMatrix.multiplyPoint3(rightBottomCorner);
    //    rotationMatrix.multiplyPoint3(rightTopCorner);
    //    std::cout << "Left bottom: " << qPrintable(AString::fromNumbers(leftBottomCorner, 3, ", ")) << std::endl;
    //    std::cout << "Left top: " << qPrintable(AString::fromNumbers(leftTopCorner, 3, ", ")) << std::endl;
    //    std::cout << "Right bottom: " << qPrintable(AString::fromNumbers(rightBottomCorner, 3, ", ")) << std::endl;
    //    std::cout << "Right top: " << qPrintable(AString::fromNumbers(rightTopCorner, 3, ", ")) << std::endl;
    
    Plane plane(leftBottomCorner,
                rightBottomCorner,
                rightTopCorner);
    
    //    for (float x = leftBottomCorner[0]; x <= leftTopCorner[0]; x += minVoxelSize) {
    //        for (float y = leftBottomCorner[1]; y <= leftTopCorner[1]; y += minVoxelSize) {
    //            for (float z = leftBottomCorner[2]; z <= leftTopCorner[2]; z += minVoxelSize) {
    //                /*
    //                 * Find the voxel in each volume for drawing
    //                 */
    //                uint8_t voxelRGBA[4] = { 0, 0, 0, 0 };
    //
    //                for (int32_t iVol = 0; iVol < numVolumes; iVol++) {
    //                    const BrainOpenGLFixedPipeline::VolumeDrawInfo& vdi = volumeDrawInfo[iVol];
    //                    const VolumeMappableInterface* volInter = vdi.volumeFile;
    //                    int64_t voxelI, voxelJ, voxelK;
    //                    volInter->enclosingVoxel(x, y, z,
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
    //                    quadRGBAs.push_back(voxelRGBA[0]);
    //                    quadRGBAs.push_back(voxelRGBA[1]);
    //                    quadRGBAs.push_back(voxelRGBA[2]);
    //                    quadRGBAs.push_back(voxelRGBA[3]);
    //
    //                    quadNormals.push_back(sliceNormalVector[0]);
    //                    quadNormals.push_back(sliceNormalVector[1]);
    //                    quadNormals.push_back(sliceNormalVector[2]);
    //
    //                    const float xmin = x - halfVoxelSizeX;
    //                    const float xmax = x + halfVoxelSizeX;
    //                    const float ymin = y - halfVoxelSizeY;
    //                    const float ymax = y + halfVoxelSizeY;
    //                    const float zmin = z - halfVoxelSizeZ;
    //                    const float zmax = z + halfVoxelSizeZ;
    //
    //                    switch (slicePlane) {
    //                        case VolumeSliceViewPlaneEnum::ALL:
    //                            CaretAssert(0);
    //                            break;
    //                        case VolumeSliceViewPlaneEnum::AXIAL:
    //                            break;
    //                            quadCoords.push_back(xmin);
    //                            quadCoords.push_back(ymin);
    //                            quadCoords.push_back(zmin);
    //
    //                            quadCoords.push_back(xmax);
    //                            quadCoords.push_back(ymin);
    //                            quadCoords.push_back(zmin);
    //
    //                            quadCoords.push_back(xmax);
    //                            quadCoords.push_back(ymax);
    //                            quadCoords.push_back(0.0);
    //
    //                            quadCoords.push_back(xmin);
    //                            quadCoords.push_back(ymax);
    //                            quadCoords.push_back(0.0);
    //                        case VolumeSliceViewPlaneEnum::CORONAL:
    //                            break;
    //                        case VolumeSliceViewPlaneEnum::PARASAGITTAL:
    //                            break;
    //                    }
    //                }
    //            }
    //        }
    //    }
    
    
    Matrix4x4 inverseMatrix(transformationMatrix);
    inverseMatrix.invert();
    
    float coordsForPlane[3][3];
    bool coordsForPlaneValid = false;
    
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
            //            rotationMatrix.multiplyPoint3(pt);
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
                        pt1[0] = 0.0;
                        pt1[1] = xmin;
                        pt1[2] = ymin;
                        pt2[0] = 0.0;
                        pt2[1] = xmax;
                        pt2[2] = ymin;
                        pt3[0] = 0.0;
                        pt3[1] = xmax;
                        pt3[2] = ymax;
                        pt4[0] = 0.0;
                        pt4[1] = xmin;
                        pt4[2] = ymax;
                        break;
                }
                rotationMatrix.multiplyPoint3(pt1);
                rotationMatrix.multiplyPoint3(pt2);
                rotationMatrix.multiplyPoint3(pt3);
                rotationMatrix.multiplyPoint3(pt4);
                
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
    
    if (false == quadCoords.empty()) {
        drawQuads(quadCoords,
                  quadNormals,
                  quadRGBAs);
        
        if (coordsForPlaneValid) {
            Plane slicePlane(coordsForPlane[0],
                             coordsForPlane[1],
                             coordsForPlane[2]);
            if (slicePlane.isValidPlane()) {
                fixedPipelineDrawing->drawFiberOrientations(&slicePlane);
                fixedPipelineDrawing->drawFiberTrajectories(&slicePlane);
            }
            //        drawFibers(fixedPipelineDrawing,
            //                   slicePlane,
            //                   rotationMatrix);
            //        drawAxis(fixedPipelineDrawing,
            //                 slicePlane,
            //                 rotationMatrix);
        }
    }
}
void
BrainOpenGLFPVolumeObliqueDrawing::drawSliceOld(BrainOpenGLFixedPipeline* fixedPipelineDrawing,
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
                            voxelRGBA[0] = rgba[0];
                            voxelRGBA[1] = rgba[1];
                            voxelRGBA[2] = rgba[2];
                            voxelRGBA[3] = rgba[3];
                        }
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
        
        //        drawFibers(fixedPipelineDrawing,
        //                   slicePlane,
        //                   rotationMatrix);
        //        drawAxis(fixedPipelineDrawing,
        //                 slicePlane,
        //                 rotationMatrix);
        
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
    
    const uint8_t alpha = 125;
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


/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
BrainOpenGLFPVolumeObliqueDrawing::toString() const
{
    return "BrainOpenGLFPVolumeObliqueDrawing";
}

