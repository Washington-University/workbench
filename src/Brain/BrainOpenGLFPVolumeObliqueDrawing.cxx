
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
#include "CiftiMappableDataFile.h"
#include "DisplayPropertiesFoci.h"
#include "DisplayPropertiesLabels.h"
#include "FociFile.h"
#include "Focus.h"
#include "GiftiLabel.h"
#include "GroupAndNameHierarchyModel.h"
#include "IdentificationWithColor.h"
#include "MathFunctions.h"
#include "Matrix4x4.h"
#include "Model.h"
#include "ModelVolume.h"
#include "NodeAndVoxelColoring.h"
#include "Plane.h"
#include "SelectionItemFocusVolume.h"
#include "SelectionItemVoxel.h"
#include "SelectionManager.h"
#include "SessionManager.h"
#include "Surface.h"
#include "VolumeFile.h"
#include "VolumeMappableInterface.h"
#include "VolumeSurfaceOutlineModel.h"
#include "VolumeSurfaceOutlineColorOrTabModel.h"
#include "VolumeSurfaceOutlineSetModel.h"

using namespace caret;
    
/**
 * \class caret::BrainOpenGLFPVolumeObliqueDrawing 
 * \brief Performs oblique volume slice drawing.
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

/**
 * Draw the oblique slice(s) selected in the browser content.
 *
 * Draw all slice planes for an all view.
 *
 * @param fixedPipelineDrawing
 *   The fixed pipeline drawing.
 * @param browserTabContent
 *   Content of the browser tab being drawn.
 * @param volumeDrawInfo
 *   Vector containing about volumes selected as overlays for drawing.
 * @param viewport
 *   Viewport in which drawing takes place.
 */
void
BrainOpenGLFPVolumeObliqueDrawing::draw(BrainOpenGLFixedPipeline* fixedPipelineDrawing,
                                        BrowserTabContent* browserTabContent,
                                        std::vector<BrainOpenGLFixedPipeline::VolumeDrawInfo>& volumeDrawInfo,
                                        const int viewport[4])
{
    if (volumeDrawInfo.empty()) {
        return;
    }
    
    CaretAssert(fixedPipelineDrawing);
    CaretAssert(browserTabContent);
    
    /*
     * Initialize class members which help reduce the number of 
     * parameters that are passed to methods.
     */
    Model* model = browserTabContent->getModelControllerForDisplay();
    CaretAssert(model);
    
    m_brain = model->getBrain();
    CaretAssert(m_brain);
    
    m_fixedPipelineDrawing = fixedPipelineDrawing;
    
    m_volumeDrawInfo = volumeDrawInfo;
    
    m_browserTabContent = browserTabContent;
    
    m_paletteFile = browserTabContent->getModelControllerForDisplay()->getBrain()->getPaletteFile();
    CaretAssert(m_paletteFile);
    
    const DisplayPropertiesLabels* dsl = m_brain->getDisplayPropertiesLabels();
    m_displayGroup = dsl->getDisplayGroupForTab(m_fixedPipelineDrawing->windowTabIndex);
    
    m_tabIndex = m_browserTabContent->getTabNumber();
    
    
    /*
     * Cifti files are slow at getting individual voxels since they
     * provide no access to individual voxels.  The reason is that
     * the data may be on a server (Dense data) and accessing a single
     * voxel would require requesting the entire map.  So, for 
     * each Cifti file, get the enter map.  This also, eliminate multiple
     * requests for the same map when drawing an ALL view.
     */
    const int32_t numVolumes = static_cast<int32_t>(m_volumeDrawInfo.size());
    for (int32_t i = 0; i < numVolumes; i++) {
        std::vector<float> ciftiMapData;
        m_ciftiMappableFileData.push_back(ciftiMapData);
        
        const CiftiMappableDataFile* ciftiMapFile = dynamic_cast<const CiftiMappableDataFile*>(m_volumeDrawInfo[i].volumeFile);
        if (ciftiMapFile != NULL) {
            ciftiMapFile->getMapData(m_volumeDrawInfo[i].mapIndex,
                                     m_ciftiMappableFileData[i]);
        }
    }
    
    if (browserTabContent->getDisplayedVolumeModel() != NULL) {
        glEnable(GL_DEPTH_TEST);
        
        const VolumeSliceViewPlaneEnum::Enum slicePlane = browserTabContent->getSliceViewPlane();
        switch (slicePlane) {
            case VolumeSliceViewPlaneEnum::ALL:
            {
                const int gap = 2;
                
                const int vpHalfX = viewport[2] / 2;
                const int vpHalfY = viewport[3] / 2;
                
                const int allVP[4] = {
                    viewport[0],
                    viewport[1],
                    vpHalfX - gap,
                    vpHalfY - gap
                };
                
                glLoadIdentity();
                drawSlicesForAllSlicesView(allVP,
                                     DRAW_MODE_VOLUME_VIEW_SLICE_3D);
                //            drawSurfaces(fixedPipelineDrawing,
                //                         browserTabContent,
                //                         allVP);
                
                const int paraVP[4] = {
                    viewport[0],
                    viewport[1] + vpHalfY + gap,
                    vpHalfX - gap,
                    vpHalfY - gap
                };
                drawSliceForSliceView(VolumeSliceViewPlaneEnum::PARASAGITTAL,
                                      paraVP);
                
                
                const int coronalVP[4] = {
                    viewport[0] + vpHalfX + gap,
                    viewport[1] + vpHalfY + gap,
                    vpHalfX - gap,
                    vpHalfY - gap
                };
                drawSliceForSliceView(VolumeSliceViewPlaneEnum::CORONAL,
                                      coronalVP);
                
                
                const int axialVP[4] = {
                    viewport[0] + vpHalfX + gap,
                    viewport[1],
                    vpHalfX - gap,
                    vpHalfY - gap
                };
                drawSliceForSliceView(VolumeSliceViewPlaneEnum::AXIAL,
                                      axialVP);
            }
                break;
            case VolumeSliceViewPlaneEnum::AXIAL:
            case VolumeSliceViewPlaneEnum::CORONAL:
            case VolumeSliceViewPlaneEnum::PARASAGITTAL:
                drawSliceForSliceView(slicePlane,
                                      viewport);
                break;
        }
    }
    
    if (browserTabContent->getDisplayedWholeBrainModel() != NULL) {
        setOrthographicBounds(DRAW_MODE_ALL_VIEW);

//        glPushMatrix();
//        const Matrix4x4 obliqueRotationMatrix = m_browserTabContent->getObliqueVolumeRotationMatrix();
//        double matrix[16];
//        obliqueRotationMatrix.getMatrixForOpenGL(matrix);
//        glMultMatrixd(matrix);
        
//        float m[16];
//        glGetFloatv(GL_MODELVIEW_MATRIX, m);
//        Matrix4x4 mv;
//        mv.setMatrixFromOpenGL(m);
//        std::cout << "ModelView Before Slice: " << qPrintable(mv.toString()) << std::endl;

//        double rotations[3];
//        obliqueRotationMatrix.getRotation(rotations[0], rotations[1], rotations[2]);
//        mv.setRotation(rotations[0], rotations[1], rotations[2]);
//        double mv16[16];
//        mv.getMatrixForOpenGL(mv16);
//        glLoadMatrixd(mv16);
        
        if (m_browserTabContent->isSliceAxialEnabled()) {
            glPushMatrix();
//            const Matrix4x4 obliqueRotationMatrix = m_browserTabContent->getObliqueVolumeRotationMatrix();
//            double matrix[16];
//            obliqueRotationMatrix.getMatrixForOpenGL(matrix);
//            glMultMatrixd(matrix);
            drawSlice(VolumeSliceViewPlaneEnum::AXIAL,
                      DRAW_MODE_ALL_VIEW);
            glPopMatrix();
        }
        
        if (m_browserTabContent->isSliceCoronalEnabled()) {
            glPushMatrix();
//            Matrix4x4 obliqueRotationMatrix = m_browserTabContent->getObliqueVolumeRotationMatrix();
//            double rx, ry, rz;
//            obliqueRotationMatrix.getRotation(rx, ry, rz);
//            obliqueRotationMatrix.setRotation(rx, 0.0, rz);
//            double matrix[16];
//            obliqueRotationMatrix.getMatrixForOpenGL(matrix);
//            glMultMatrixd(matrix);
//
            drawSlice(VolumeSliceViewPlaneEnum::CORONAL,
                      DRAW_MODE_ALL_VIEW);
            glPopMatrix();
        }
        
        if (m_browserTabContent->isSliceParasagittalEnabled()) {
            glPushMatrix();
//            const Matrix4x4 obliqueRotationMatrix = m_browserTabContent->getObliqueVolumeRotationMatrix();
//            double matrix[16];
//            obliqueRotationMatrix.getMatrixForOpenGL(matrix);
//            glMultMatrixd(matrix);
            drawSlice(VolumeSliceViewPlaneEnum::PARASAGITTAL,
                      DRAW_MODE_ALL_VIEW);
            glPopMatrix();
        }
        
    }
}

//void
//BrainOpenGLFPVolumeObliqueDrawing::drawAxis(BrainOpenGLFixedPipeline* fixedPipelineDrawing,
//                                            const Matrix4x4& rotationMatrix)
//{
//    if (SessionManager::get()->getCaretPreferences()->isVolumeAxesCrosshairsDisplayed()) {
//        fixedPipelineDrawing->setLineWidth(1.0);
//        
//        Matrix4x4 inverseMatrix(rotationMatrix);
//        inverseMatrix.invert();
//        
//        const float bigNumber = 10000;
//        for (int32_t iPlane = 0; iPlane < 3; iPlane++) {
//            float p1[3] = { 0.0, 0.0, 0.0 };
//            float p2[3] = { 0.0, 0.0, 0.0 };
//            unsigned char lineColor[3] = { 0, 0, 0 };
//            
//            switch (iPlane) {
//                case 0:
//                    p1[2] = -bigNumber;
//                    p1[2] =  bigNumber;
//                    lineColor[2] = 255;
//                    break;
//                case 1:
//                    p1[1] = -bigNumber;
//                    p1[1] =  bigNumber;
//                    lineColor[1] = 255;
//                    break;
//                case 2:
//                    p1[0] = -bigNumber;
//                    p1[0] =  bigNumber;
//                    lineColor[0] = 255;
//                    break;
//            }
//
//            inverseMatrix.multiplyPoint3(p1);
//            inverseMatrix.multiplyPoint3(p2);
//            
//            glColor3ubv(lineColor);
//            glBegin(GL_LINES);
//            glVertex3fv(p1);
//            glVertex3fv(p2);
//            glEnd();
//        }
//    }
//}

/**
 * Draw all slice planes for an all view.
 *
 * @param viewport
 *   Viewport in which drawing takes place.
 */
void
BrainOpenGLFPVolumeObliqueDrawing::drawSlicesForAllSlicesView(const int viewport[4],
                                                        const DRAW_MODE drawMode)
{
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    
    m_fixedPipelineDrawing->setViewportAndOrthographicProjection(viewport,
                                                               ProjectionViewTypeEnum::PROJECTION_VIEW_LEFT_LATERAL);

    setOrthographicBounds(drawMode);
    
    drawSlice(VolumeSliceViewPlaneEnum::AXIAL,
              drawMode);
    
    drawSlice(VolumeSliceViewPlaneEnum::CORONAL,
              drawMode);
    
    drawSlice(VolumeSliceViewPlaneEnum::PARASAGITTAL,
              drawMode);
    
    glPopMatrix();
}

/**
 * Draw a volume slice for a volume slice view.
 *
 * @param sliceViewPlane
 *   View plane (eg axial) of slice being drawn relative to the slice's normal vector.
 * @param viewport
 *   Viewport in which drawing takes place.
 */
void
BrainOpenGLFPVolumeObliqueDrawing::drawSliceForSliceView(const VolumeSliceViewPlaneEnum::Enum slicePlane,
                                                            const int viewport[4])
{
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    
    /*
     * Setup the viewport so left on left, bottom at bottom, and near towards viewer
     */
    m_fixedPipelineDrawing->setViewportAndOrthographicProjection(viewport,
                                                               ProjectionViewTypeEnum::PROJECTION_VIEW_LEFT_LATERAL);
    
    setOrthographicBounds(DRAW_MODE_VOLUME_VIEW_SLICE_SINGLE);
    
    drawSlice(slicePlane,
              DRAW_MODE_VOLUME_VIEW_SLICE_SINGLE);
    
    glPopMatrix();
}

/**
 * Set the orthographic bounds based upon the given mode so that slices
 * 'fill' the alotted space.
 * 
 * @param drawMode
 *    The drawing mode.
 */
void
BrainOpenGLFPVolumeObliqueDrawing::setOrthographicBounds(const DRAW_MODE drawMode)
{
    bool updateBoundsForSliceView = false;
    
    switch (drawMode) {
        case DRAW_MODE_ALL_VIEW:
            break;
        case DRAW_MODE_VOLUME_VIEW_SLICE_SINGLE:
            updateBoundsForSliceView = true;
            break;
        case DRAW_MODE_VOLUME_VIEW_SLICE_3D:
            updateBoundsForSliceView = true;
            break;
    }
    
    m_orthographicBounds[0] = m_fixedPipelineDrawing->orthographicLeft;
    m_orthographicBounds[1] = m_fixedPipelineDrawing->orthographicRight;
    m_orthographicBounds[2] = m_fixedPipelineDrawing->orthographicBottom;
    m_orthographicBounds[3] = m_fixedPipelineDrawing->orthographicTop;
    m_orthographicBounds[4] = m_fixedPipelineDrawing->orthographicNear;
    m_orthographicBounds[5] = m_fixedPipelineDrawing->orthographicFar;

    if (updateBoundsForSliceView) {
        float coordBounds[6];
        float spacing[3];
        getVoxelCoordinateBoundsAndSpacing(coordBounds, spacing);
        float maxCoordBound = 0;
        for (int32_t i = 0; i < 6; i++) {
            const float absBound = std::fabs(coordBounds[i]);
            maxCoordBound = std::max(maxCoordBound, absBound);
        }
        if (maxCoordBound > 0.0) {
            const double rightOrTop = std::min(std::fabs(m_fixedPipelineDrawing->orthographicLeft),
                                               std::fabs(m_fixedPipelineDrawing->orthographicTop));
//            const float scaleToFitWindow = (m_fixedPipelineDrawing->orthographicRight
//                                            / maxBound) * 1.10;
//            const float scaleToFitWindow = (maxCoordBound
//                                            / rightOrTop) * 1.05;
            const float scaleToFitWindow = (maxCoordBound
                                            / rightOrTop);
            //        glScalef(scaleToFitWindow,
            //                 scaleToFitWindow,
            //                 scaleToFitWindow);
            
            m_orthographicBounds[0] = m_fixedPipelineDrawing->orthographicLeft * scaleToFitWindow;
            m_orthographicBounds[1] = m_fixedPipelineDrawing->orthographicRight * scaleToFitWindow;
            m_orthographicBounds[2] = m_fixedPipelineDrawing->orthographicBottom * scaleToFitWindow;
            m_orthographicBounds[3] = m_fixedPipelineDrawing->orthographicTop * scaleToFitWindow;
            m_orthographicBounds[4] = m_fixedPipelineDrawing->orthographicNear;
            m_orthographicBounds[5] = m_fixedPipelineDrawing->orthographicFar;
            
            glMatrixMode(GL_PROJECTION);
            glLoadIdentity();
            glOrtho(m_orthographicBounds[0], m_orthographicBounds[1],
                    m_orthographicBounds[2], m_orthographicBounds[3],
                    m_orthographicBounds[4], m_orthographicBounds[5]);
            glMatrixMode(GL_MODELVIEW);
        }
    }
}


/**
 * Draw a volume slice for the given slice plane.
 *
 * @param orthoBounds
 *   Bounds of the orthographic projection.
 * @param sliceViewPlane
 *   View plane (eg axial) of slice being drawn relative to the slice's normal vector.
 * @param drawMode
 *   The drawing mode
 */
void
BrainOpenGLFPVolumeObliqueDrawing::drawSlice(const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
                                        const DRAW_MODE drawMode)
{
    const int32_t numVolumes = static_cast<int32_t>(m_volumeDrawInfo.size());
    if (numVolumes <= 0) {
        return;
    }

    AString debugString;
    
    
    const AString planeName = VolumeSliceViewPlaneEnum::toGuiName(sliceViewPlane);
    debugString.appendWithNewLine("\n"
                                  //+ AString(isSliceView ? "Slice View " : "All View")
                                  + planeName);
    
    float voxelBounds[6];
    float voxelSpacing[3];
    
    /*
     * Get the maximum bounds of the voxels from all slices
     * and the smallest voxel spacing
     */
    if (false == getVoxelCoordinateBoundsAndSpacing(voxelBounds,
                                                    voxelSpacing)) {
        return;
    }

    SelectionItemVoxel* voxelID = m_brain->getSelectionManager()->getVoxelIdentification();
    
    /*
     * Check for a 'selection' type mode
     */
    bool isSelect = false;
    switch (m_fixedPipelineDrawing->mode) {
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
    
    
    float windowAdjustmentTranslation[3] = { 0.0, 0.0, 0.0 };
    float windowAdjustmentScaling = 1.0;
    m_fixedPipelineDrawing->getVolumeFitToWindowScalingAndTranslation(m_volumeDrawInfo[0].volumeFile,
                                                                      sliceViewPlane,
                                                                      m_orthographicBounds,
                                                                      windowAdjustmentTranslation,
                                                                      windowAdjustmentScaling);
    
    glPushMatrix();
    
    switch (drawMode) {
        case DRAW_MODE_ALL_VIEW:
            break;
        case DRAW_MODE_VOLUME_VIEW_SLICE_3D:
            break;
        case DRAW_MODE_VOLUME_VIEW_SLICE_SINGLE:
        {
            /*
             * User's translation.
             */
            const float* translation = m_browserTabContent->getTranslation();
            float translationadj[3] = { translation[0], translation[1], translation[2] };
            switch (sliceViewPlane) {//prevents going outside near/far?
                case VolumeSliceViewPlaneEnum::ALL:
                    break;
                case VolumeSliceViewPlaneEnum::AXIAL:
                    translationadj[2] = 0.0;
                    break;
                case VolumeSliceViewPlaneEnum::CORONAL:
                    translationadj[1] = translationadj[2];
                    translationadj[2] = 0.0;
                    break;
                case VolumeSliceViewPlaneEnum::PARASAGITTAL:
                    translationadj[0] = -translationadj[1];
                    translationadj[1] = translationadj[2];
                    translationadj[2] = 0.0;
                    break;
            }
            
            glTranslatef(translationadj[0], 
                         translationadj[1], 
                         translationadj[2]);
            
            switch (sliceViewPlane) {
                case VolumeSliceViewPlaneEnum::ALL:
                    break;
                case VolumeSliceViewPlaneEnum::AXIAL:
                    windowAdjustmentTranslation[2] = 0.0;
                    break;
                case VolumeSliceViewPlaneEnum::CORONAL:
                    windowAdjustmentTranslation[1] = windowAdjustmentTranslation[2];
                    windowAdjustmentTranslation[2] = 0.0;
                    break;
                case VolumeSliceViewPlaneEnum::PARASAGITTAL:
                    windowAdjustmentTranslation[0] = -windowAdjustmentTranslation[1];
                    windowAdjustmentTranslation[1] =  windowAdjustmentTranslation[2];
                    windowAdjustmentTranslation[2] =  0.0;
                    break;
            }
            glTranslatef(windowAdjustmentTranslation[0],
                         windowAdjustmentTranslation[1],
                         windowAdjustmentTranslation[2]);
            glScalef(windowAdjustmentScaling,
                     windowAdjustmentScaling,
                     windowAdjustmentScaling);
        }
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
    const float minScreenX = m_orthographicBounds[0];
    const float maxScreenX = m_orthographicBounds[1];
    const float minScreenY = m_orthographicBounds[2];
    const float maxScreenY = m_orthographicBounds[3];
//    const float minScreenX = m_fixedPipelineDrawing->orthographicLeft;
//    const float maxScreenX = m_fixedPipelineDrawing->orthographicRight;
//    const float minScreenY = m_fixedPipelineDrawing->orthographicBottom;
//    const float maxScreenY = m_fixedPipelineDrawing->orthographicTop;

    const float minVoxelSize = std::min(voxelSpacing[0],
                                        std::min(voxelSpacing[1],
                                                 voxelSpacing[2]));
    
    CaretAssert(maxScreenX - minScreenX);
    CaretAssert(maxScreenY - minScreenY);
    CaretAssert(minVoxelSize > 0.0);

    /*
     * Get coordinate of selected slices
     */
    const float selectedSliceOrigin[3] = {
        m_browserTabContent->getSliceCoordinateParasagittal(),
        m_browserTabContent->getSliceCoordinateCoronal(),
        m_browserTabContent->getSliceCoordinateAxial()
    };
    
    /*
     * Get the rotation matrix
     */
    Matrix4x4 rotationMatrix = m_browserTabContent->getObliqueVolumeRotationMatrix(); //  m_browserTabContent->getRotationMatrix();

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
    

    bool setEyePositionFlag = false;
    switch (drawMode) {
        case DRAW_MODE_ALL_VIEW:
            break;
        case DRAW_MODE_VOLUME_VIEW_SLICE_SINGLE:
            setEyePositionFlag = true;
            break;
        case DRAW_MODE_VOLUME_VIEW_SLICE_3D:
            break;
    }
    
    /*
     * If this is a volume slice view,
     * (1) Set the 'center' to the center of the slice
     * (2) Set the 'eye' to an offset from the slice
     * (3) Set the 'up vector'
     * (4) Use the above with gluLookAt to set the viewing position
     */
    if (setEyePositionFlag) {
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
        
        /*
         * 'Center' that is 'looked at'
         */
        float center[3] = {
            sliceOffset[0],
            sliceOffset[1],
            sliceOffset[2]
        };
        
        /*
         * Viewers 'eye'
         */
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

    /*
     * Might allow transformations
     */
    bool allowTransformationsFlag = false;
    switch (drawMode) {
        case DRAW_MODE_ALL_VIEW:
            break;
        case DRAW_MODE_VOLUME_VIEW_SLICE_SINGLE:
            allowTransformationsFlag = true;
            break;
        case DRAW_MODE_VOLUME_VIEW_SLICE_3D:
            allowTransformationsFlag = true;
            break;
    }
    const float zoom = (allowTransformationsFlag
                        ? (m_browserTabContent->getScaling())
                        //? (m_browserTabContent->getScaling() * windowAdjustmentScaling)
                        : 1.0);
    
    /*
     * Draw slice
     */
    const float screenBounds[4] = {
        minScreenX,
        maxScreenX,
        minScreenY,
        maxScreenY
    };
    drawSliceVoxels(sliceViewPlane,
                    identificationIndices,
                    idPerVoxelCount,
                    transformationMatrix,
                    screenBounds,
                    sliceNormalVector,
                    minVoxelSize,
                    zoom,
                    isSelect);
    
    if ( ! isSelect) {
        bool isDrawLayers = false;
        switch (drawMode) {
            case DRAW_MODE_ALL_VIEW:
                break;
            case DRAW_MODE_VOLUME_VIEW_SLICE_SINGLE:
                isDrawLayers = true;
                break;
            case DRAW_MODE_VOLUME_VIEW_SLICE_3D:
                isDrawLayers = true;
                break;
        }
        if (isDrawLayers) {
            if (slicePlane.isValidPlane()) {
                glPushMatrix();
                glScalef(zoom, zoom, zoom);
                
                m_fixedPipelineDrawing->drawFiberOrientations(&slicePlane);
                m_fixedPipelineDrawing->drawFiberTrajectories(&slicePlane);
                drawVolumeSliceFoci(slicePlane);
                
                glPopMatrix();
            }
        }
        
        if (slicePlane.isValidPlane()) {
            glPushMatrix();
            glScalef(zoom, zoom, zoom);
            
            drawSurfaceOutline(slicePlane);
            
            glPopMatrix();
        }
    }
    
    /*
     * Process selection
     */
    if (isSelect) {
        int32_t identifiedItemIndex;
        float depth = -1.0;
        m_fixedPipelineDrawing->getIndexFromColorSelection(SelectionItemDataTypeEnum::VOXEL,
                                         m_fixedPipelineDrawing->mouseX,
                                         m_fixedPipelineDrawing->mouseY,
                                         identifiedItemIndex,
                                         depth);
        if (identifiedItemIndex >= 0) {
            const int32_t idIndex = identifiedItemIndex * idPerVoxelCount;
            const int32_t volDrawInfoIndex = identificationIndices[idIndex];
            CaretAssertVectorIndex(m_volumeDrawInfo, volDrawInfoIndex);
            VolumeMappableInterface* vf = m_volumeDrawInfo[volDrawInfoIndex].volumeFile;
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
                
                m_fixedPipelineDrawing->setSelectedItemScreenXYZ(voxelID,
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

/**
 * Draw a volume slice's voxels.
 *
 * @param sliceViewPlane
 *   View plane (eg axial) of slice being drawn relative to the slice's normal vector.
 * @param identificationIndices
 *   Indices into with identification information may be added.
 * @param idPerVoxelCount
 *   Number of items per voxel identification for identificationIndices
 * @param transformationMatrix
 *   Transformation matrix for screen to model.
 * @param screenBounds
 *   Bounds of the screen.
 * @param sliceNormalVector
 *   Unit normal vector of slice being drawn.
 * @param voxelSize
 *   Size of voxels.
 * @param isSelectionMode
 *   True if performing selection.
 */
void
BrainOpenGLFPVolumeObliqueDrawing::drawSliceVoxels(const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
                                      std::vector<int32_t>& identificationIndices,
                                      const int32_t idPerVoxelCount,
                                      const Matrix4x4& transformationMatrix,
                                      const float screenBounds[4],
                                      const float sliceNormalVector[3],
                                      const float voxelSize,
                                                                          const float zoom,
                                      const bool isSelectionMode)
{
    const int32_t numVolumes = static_cast<int32_t>(m_volumeDrawInfo.size());
    
    const float minScreenX = screenBounds[0];
    const float maxScreenX = screenBounds[1];
    const float minScreenY = screenBounds[2];
    const float maxScreenY = screenBounds[3];
    
    /*
     * Set the corners of the screen for the respective view
     */
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
    
    /*
     * Transform the corners of the screen into model coordinates
     */
    transformationMatrix.multiplyPoint3(bottomLeft);
    transformationMatrix.multiplyPoint3(bottomRight);
    transformationMatrix.multiplyPoint3(topRight);
    transformationMatrix.multiplyPoint3(topLeft);
    
    /*
     * Unit vector and distance in model coords along left side of screen
     */
    double bottomLeftToTopLeftUnitVector[3] = {
        topLeft[0] - bottomLeft[0],
        topLeft[1] - bottomLeft[1],
        topLeft[2] - bottomLeft[2],
    };
    MathFunctions::normalizeVector(bottomLeftToTopLeftUnitVector);
    const double bottomLeftToTopLeftDistance = MathFunctions::distance3D(bottomLeft,
                                                                         topLeft);
    
    /*
     * Unit vector and distance in model coords along right side of screen
     */
    double bottomRightToTopRightUnitVector[3] = {
        topRight[0] - bottomRight[0],
        topRight[1] - bottomRight[1],
        topRight[2] - bottomRight[2]
    };
    MathFunctions::normalizeVector(bottomRightToTopRightUnitVector);    
    const double bottomRightToTopRightDistance = MathFunctions::distance3D(bottomRight,
                                                                           topRight);
    /*
     * For fastest coloring, need to color data values as a group
     */
    std::vector<VolumeSlice> volumeSlices;
    for (int32_t i = 0; i < numVolumes; i++) {
        volumeSlices.push_back(VolumeSlice(m_volumeDrawInfo[i].volumeFile,
                                           m_volumeDrawInfo[i].mapIndex));
        
    }
    
    /*
     * Track voxels that will be drawn
     */
    std::vector<VoxelToDraw*> voxelsToDraw;
    
    if ((bottomLeftToTopLeftDistance > 0)
        && (bottomRightToTopRightDistance > 0)) {
        
        const double bottomLeftToTopLeftStep = voxelSize;
        const double numLeftSteps = (bottomLeftToTopLeftDistance / bottomLeftToTopLeftStep);
        
        const double bottomRightToTopRightStep = (bottomRightToTopRightDistance
                                                  / numLeftSteps);
        
        const double dtVertical = bottomLeftToTopLeftStep / bottomLeftToTopLeftDistance;
        
        /*
         * Voxels are drawn in rows, left to right, across the screen, 
         * starting at the bottom.
         */
        double leftEdgeBottomCoord[3];
        double leftEdgeTopCoord[3];
        double rightEdgeBottomCoord[3];
        double rightEdgeTopCoord[3];
        for (double tVertical = 0.0, dLeft = 0.0, dRight = 0.0;
             tVertical < 1.0;
             tVertical += dtVertical, dLeft += bottomLeftToTopLeftStep, dRight += bottomRightToTopRightStep) {
            /*
             * Coordinate on left edge at BOTTOM of current row
             */
            leftEdgeBottomCoord[0] = bottomLeft[0] + (dLeft * bottomLeftToTopLeftUnitVector[0]);
            leftEdgeBottomCoord[1] = bottomLeft[1] + (dLeft * bottomLeftToTopLeftUnitVector[1]);
            leftEdgeBottomCoord[2] = bottomLeft[2] + (dLeft * bottomLeftToTopLeftUnitVector[2]);
            
            /*
             * Coordinate on right edge at BOTTOM of current row
             */
            rightEdgeBottomCoord[0] = bottomRight[0] + (dRight * bottomRightToTopRightUnitVector[0]);
            rightEdgeBottomCoord[1] = bottomRight[1] + (dRight * bottomRightToTopRightUnitVector[1]);
            rightEdgeBottomCoord[2] = bottomRight[2] + (dRight * bottomRightToTopRightUnitVector[2]);
            
            /*
             * Coordinate on left edge at TOP of current row
             */
            leftEdgeTopCoord[0] = bottomLeft[0] + ((dLeft + bottomLeftToTopLeftStep) * bottomLeftToTopLeftUnitVector[0]);
            leftEdgeTopCoord[1] = bottomLeft[1] + ((dLeft + bottomLeftToTopLeftStep) * bottomLeftToTopLeftUnitVector[1]);
            leftEdgeTopCoord[2] = bottomLeft[2] + ((dLeft + bottomLeftToTopLeftStep) * bottomLeftToTopLeftUnitVector[2]);
            
            /*
             * Coordinate on right edge at TOP of current row
             */
            rightEdgeTopCoord[0] = bottomRight[0] + ((dRight + bottomRightToTopRightStep) * bottomRightToTopRightUnitVector[0]);
            rightEdgeTopCoord[1] = bottomRight[1] + ((dRight + bottomRightToTopRightStep) * bottomRightToTopRightUnitVector[1]);
            rightEdgeTopCoord[2] = bottomRight[2] + ((dRight + bottomRightToTopRightStep) * bottomRightToTopRightUnitVector[2]);
            
            /*
             * Determine change in XYZ per voxel along the bottom of the current row
             */
            const double bottomVoxelEdgeDistance = MathFunctions::distance3D(leftEdgeBottomCoord,
                                                                             rightEdgeBottomCoord);
            double bottomEdgeUnitVector[3];
            MathFunctions::createUnitVector(leftEdgeBottomCoord, rightEdgeBottomCoord, bottomEdgeUnitVector);
            const int64_t numVoxelsInRow = bottomVoxelEdgeDistance / voxelSize;
            const double bottomEdgeVoxelSize = bottomVoxelEdgeDistance / numVoxelsInRow;
            const double bottomVoxelEdgeDX = bottomEdgeVoxelSize * bottomEdgeUnitVector[0];
            const double bottomVoxelEdgeDY = bottomEdgeVoxelSize * bottomEdgeUnitVector[1];
            const double bottomVoxelEdgeDZ = bottomEdgeVoxelSize * bottomEdgeUnitVector[2];
            
            /*
             * Determine change in XYZ per voxel along top of the current row
             */
            const double topVoxelEdgeDistance = MathFunctions::distance3D(leftEdgeTopCoord,
                                                                          rightEdgeTopCoord);
            double topEdgeUnitVector[3];
            MathFunctions::createUnitVector(leftEdgeTopCoord, rightEdgeTopCoord, topEdgeUnitVector);
            const double topEdgeVoxelSize = topVoxelEdgeDistance / numVoxelsInRow;
            const double topVoxelEdgeDX = topEdgeVoxelSize * topEdgeUnitVector[0];
            const double topVoxelEdgeDY = topEdgeVoxelSize * topEdgeUnitVector[1];
            const double topVoxelEdgeDZ = topEdgeVoxelSize * topEdgeUnitVector[2];
            
            /*
             * Initialize bottom and top left coordinate of first voxel in row
             */
            double bottomLeftVoxelCoord[3] = {
                leftEdgeBottomCoord[0],
                leftEdgeBottomCoord[1],
                leftEdgeBottomCoord[2]
            };
            double topLeftVoxelCoord[3] = {
                leftEdgeTopCoord[0],
                leftEdgeTopCoord[1],
                leftEdgeTopCoord[2]
            };
            
            const bool useInterpolatedVoxel = true;
            
            /*
             * Draw the voxels in the row
             */
            for (int64_t i = 0; i < numVoxelsInRow; i++) {
                /*
                 * Top right corner of voxel
                 */
                const double topRightVoxelCoord[3] = {
                    topLeftVoxelCoord[0] + topVoxelEdgeDX,
                    topLeftVoxelCoord[1] + topVoxelEdgeDY,
                    topLeftVoxelCoord[2] + topVoxelEdgeDZ
                };
                
                const float voxelCenter[3] = {
                    (bottomLeftVoxelCoord[0] + topRightVoxelCoord[0]) * 0.5,
                    (bottomLeftVoxelCoord[1] + topRightVoxelCoord[1]) * 0.5,
                    (bottomLeftVoxelCoord[2] + topRightVoxelCoord[2]) * 0.5
                };

                
                /*
                 * Loop through the volumes selected as overlays.
                 */
                VoxelToDraw* voxelDrawingInfo = NULL;
                
                for (int32_t iVol = 0; iVol < numVolumes; iVol++) {
                    const BrainOpenGLFixedPipeline::VolumeDrawInfo& vdi = m_volumeDrawInfo[iVol];
                    const VolumeMappableInterface* volInter = vdi.volumeFile;
                    const VolumeFile* volumeFile = volumeSlices[iVol].m_volumeFile;
                    
                    float value = 0;
                    bool valueValidFlag = false;
                    
                    bool isPaletteMappedVolumeFile = false;
                    if (volumeFile != NULL) {
                        if (volumeFile->isMappedWithPalette()) {
                            isPaletteMappedVolumeFile = true;
                        }
                    }
                    const CiftiMappableDataFile* ciftiMappableFile = volumeSlices[iVol].m_ciftiMappableDataFile; // dynamic_cast<const CiftiMappableDataFile*>(volInter);
                    
                    if (useInterpolatedVoxel
                        && isPaletteMappedVolumeFile) {
                        value = volumeFile->interpolateValue(voxelCenter,
                                                             VolumeFile::CUBIC,
                                                             &valueValidFlag,
                                                             vdi.mapIndex);
//                        if (valueValidFlag) {
//                            if (MathFunctions::isNaN(value)) {
//                                std::cout << "Not a number: " << qPrintable(AString::fromNumbers(voxelCenter, 3, ",")) << std::endl;
//                            }
//                        }
                    }
                    else if (ciftiMappableFile != NULL) {
                        const int64_t voxelOffset = ciftiMappableFile->getMapDataOffsetForVoxelAtCoordinate(voxelCenter,
                                                                                                            vdi.mapIndex);
                        if (voxelOffset >= 0) {
                            CaretAssertVectorIndex(m_ciftiMappableFileData, iVol);
                            const std::vector<float>& data = m_ciftiMappableFileData[iVol];
                            CaretAssertVectorIndex(data, voxelOffset);
                            value = data[voxelOffset];
                            valueValidFlag = true;
                        }
                    }
                    else {
                        value = volInter->getVoxelValue(voxelCenter,
                                                        &valueValidFlag,
                                                        vdi.mapIndex);
                    }
                    
                    if (valueValidFlag) {
                        if (voxelDrawingInfo == NULL) {
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
                            
                            voxelDrawingInfo = new VoxelToDraw(voxelCenter,
                                                               bottomLeftVoxelCoord,
                                                           bottomRightVoxelCoord,
                                                           topRightVoxelCoord,
                                                           topLeftVoxelCoord);
                            voxelsToDraw.push_back(voxelDrawingInfo);
                        }
                        
                        const int64_t offset = volumeSlices[iVol].addValue(value);
                        voxelDrawingInfo->addVolumeValue(iVol, offset);
                    }                    
                }
                                
                /*
                 * Move to the next voxel in the row
                 */
                bottomLeftVoxelCoord[0] += bottomVoxelEdgeDX;
                bottomLeftVoxelCoord[1] += bottomVoxelEdgeDY;
                bottomLeftVoxelCoord[2] += bottomVoxelEdgeDZ;
                topLeftVoxelCoord[0] += topVoxelEdgeDX;
                topLeftVoxelCoord[1] += topVoxelEdgeDY;
                topLeftVoxelCoord[2] += topVoxelEdgeDZ;
            }
        }
    }

    /*
     * Color voxel values
     */
    for (int32_t i = 0; i < numVolumes; i++) {
        const int64_t numValues = static_cast<int64_t>(volumeSlices[i].m_values.size());
        if (numValues > 0) {
            volumeSlices[i].allocateColors();
    
            VolumeMappableInterface* volume = volumeSlices[i].m_volumeMappableInterface;
            CaretMappableDataFile* mappableFile = dynamic_cast<CaretMappableDataFile*>(volume);
            CaretAssert(mappableFile);
            const int32_t mapIndex = volumeSlices[i].m_mapIndex;
            const float* values = &volumeSlices[i].m_values[0];
            uint8_t* rgba = &volumeSlices[i].m_rgba[0];
            
            if (mappableFile->isMappedWithPalette()) {
                const PaletteColorMapping* paletteColorMapping = mappableFile->getMapPaletteColorMapping(mapIndex);
                const AString paletteName = paletteColorMapping->getSelectedPaletteName();
                const Palette* palette = m_paletteFile->getPaletteByName(paletteName);
                if (palette != NULL) {
                    CaretAssertVectorIndex(m_volumeDrawInfo, i);
                    NodeAndVoxelColoring::colorScalarsWithPalette(m_volumeDrawInfo[i].statistics,
                                                                  paletteColorMapping,
                                                                  palette,
                                                                  values,
                                                                  values,
                                                                  numValues,
                                                                  rgba);
                }
                else {
                    CaretLogWarning("Missing palette named: "
                                    + paletteName);
                }
            }
            else if (mappableFile->isMappedWithLabelTable()) {
                GiftiLabelTable* labelTable = mappableFile->getMapLabelTable(mapIndex);
                NodeAndVoxelColoring::colorIndicesWithLabelTable(labelTable,
                                                                 values,
                                                                 numValues,
                                                                 rgba);
            }
            else {
                CaretAssert(0);
            }
        }
    }
    
    const int64_t numVoxelsToDraw = static_cast<int64_t>(voxelsToDraw.size());
    
    /*
     * quadCoords is the coordinates for all four corners of a 'quad'
     * that is used to draw a voxel.  quadRGBA is the colors for each
     * voxel drawn as a 'quad'.
     */
    std::vector<float> quadCoordsVector;
    std::vector<float> quadNormalsVector;
    std::vector<uint8_t> quadRGBAsVector;
    
    /*
     * Reserve space to avoid reallocations
     */
    const int64_t coordinatesPerQuad = 4;
    const int64_t componentsPerCoordinate = 3;
    const int64_t colorComponentsPerCoordinate = 4;
    quadCoordsVector.resize(numVoxelsToDraw
                            * coordinatesPerQuad
                            * componentsPerCoordinate);
    quadNormalsVector.resize(quadCoordsVector.size());
    quadRGBAsVector.resize(numVoxelsToDraw *
                           coordinatesPerQuad *
                           colorComponentsPerCoordinate);

    int64_t coordOffset = 0;
    int64_t normalOffset = 0;
    int64_t rgbaOffset = 0;

    float*   quadCoords  = &quadCoordsVector[0];
    float*   quadNormals = &quadNormalsVector[0];
    uint8_t* quadRGBAs   = &quadRGBAsVector[0];
    
    for (int64_t iVox = 0; iVox < numVoxelsToDraw; iVox++) {
        CaretAssertVectorIndex(voxelsToDraw, iVox);
        VoxelToDraw* vtd = voxelsToDraw[iVox];
        CaretAssert(vtd);
        
        uint8_t voxelRGBA[4] = { 0, 0, 0, 0 };
        
        const int32_t numSlicesForVoxel = static_cast<int32_t>(vtd->m_sliceIndices.size());
        for (int32_t iSlice = 0; iSlice < numSlicesForVoxel; iSlice++) {
            CaretAssertVectorIndex(vtd->m_sliceIndices, iSlice);
            CaretAssertVectorIndex(vtd->m_sliceOffsets, iSlice);
            const int32_t sliceIndex = vtd->m_sliceIndices[iSlice];
            const int64_t voxelOffset = vtd->m_sliceOffsets[iSlice];
            
            const uint8_t* rgba = volumeSlices[sliceIndex].getRgbaForValueByIndex(voxelOffset);
            if (rgba[3] > 0) {
                voxelRGBA[0] = rgba[0];
                voxelRGBA[1] = rgba[1];
                voxelRGBA[2] = rgba[2];
                voxelRGBA[3] = rgba[3];
                
                if (isSelectionMode) {
                    VolumeMappableInterface* volMap = volumeSlices[sliceIndex].m_volumeMappableInterface;
                    int64_t voxelI, voxelJ, voxelK;
                    volMap->enclosingVoxel(vtd->m_center[0], vtd->m_center[1], vtd->m_center[2],
                                             voxelI, voxelJ, voxelK);
                    
                    if (volMap->indexValid(voxelI, voxelJ, voxelK)) {
                        /*
                         * Performing a selection?
                         */
                        const int32_t idIndex = identificationIndices.size() / idPerVoxelCount;
                        m_fixedPipelineDrawing->colorIdentification->addItem(voxelRGBA,
                                                                           SelectionItemDataTypeEnum::VOXEL,
                                                                           idIndex);
                        voxelRGBA[3] = 255;
                        
                        identificationIndices.push_back(sliceIndex);
                        identificationIndices.push_back(volumeSlices[sliceIndex].m_mapIndex);
                        identificationIndices.push_back(voxelI);
                        identificationIndices.push_back(voxelJ);
                        identificationIndices.push_back(voxelK);
                    }
                }
            }
        }
        
        if (voxelRGBA[3] > 0) {
            CaretAssertVectorIndex(quadRGBAsVector, rgbaOffset + 3);
            quadRGBAs[rgbaOffset]   = voxelRGBA[0];
            quadRGBAs[rgbaOffset+1] = voxelRGBA[1];
            quadRGBAs[rgbaOffset+2] = voxelRGBA[2];
            quadRGBAs[rgbaOffset+3] = voxelRGBA[3];
            rgbaOffset += 4;
            
            CaretAssertVectorIndex(quadNormalsVector, normalOffset + 2);
            quadNormals[normalOffset]   = sliceNormalVector[0];
            quadNormals[normalOffset+1] = sliceNormalVector[1];
            quadNormals[normalOffset+2] = sliceNormalVector[2];
            normalOffset += 3;
            
            CaretAssertVectorIndex(quadRGBAsVector, rgbaOffset + 3);
            quadRGBAs[rgbaOffset]   = voxelRGBA[0];
            quadRGBAs[rgbaOffset+1] = voxelRGBA[1];
            quadRGBAs[rgbaOffset+2] = voxelRGBA[2];
            quadRGBAs[rgbaOffset+3] = voxelRGBA[3];
            rgbaOffset += 4;
            
            CaretAssertVectorIndex(quadNormalsVector, normalOffset + 2);
            quadNormals[normalOffset]   = sliceNormalVector[0];
            quadNormals[normalOffset+1] = sliceNormalVector[1];
            quadNormals[normalOffset+2] = sliceNormalVector[2];
            normalOffset += 3;
            
            CaretAssertVectorIndex(quadRGBAsVector, rgbaOffset + 3);
            quadRGBAs[rgbaOffset]   = voxelRGBA[0];
            quadRGBAs[rgbaOffset+1] = voxelRGBA[1];
            quadRGBAs[rgbaOffset+2] = voxelRGBA[2];
            quadRGBAs[rgbaOffset+3] = voxelRGBA[3];
            rgbaOffset += 4;
            
            CaretAssertVectorIndex(quadNormalsVector, normalOffset + 2);
            quadNormals[normalOffset]   = sliceNormalVector[0];
            quadNormals[normalOffset+1] = sliceNormalVector[1];
            quadNormals[normalOffset+2] = sliceNormalVector[2];
            normalOffset += 3;
            
            CaretAssertVectorIndex(quadRGBAsVector, rgbaOffset + 3);
            quadRGBAs[rgbaOffset]   = voxelRGBA[0];
            quadRGBAs[rgbaOffset+1] = voxelRGBA[1];
            quadRGBAs[rgbaOffset+2] = voxelRGBA[2];
            quadRGBAs[rgbaOffset+3] = voxelRGBA[3];
            rgbaOffset += 4;
            
            CaretAssertVectorIndex(quadNormalsVector, normalOffset + 2);
            quadNormals[normalOffset]   = sliceNormalVector[0];
            quadNormals[normalOffset+1] = sliceNormalVector[1];
            quadNormals[normalOffset+2] = sliceNormalVector[2];
            normalOffset += 3;
            
            CaretAssertVectorIndex(quadCoordsVector, coordOffset + 11);
            for (int32_t iq = 0; iq < 12; iq++) {
                quadCoords[coordOffset + iq] = vtd->m_coordinates[iq];
            }
            coordOffset += 12;
        }
//        if (voxelRGBA[3] > 0) {
//            quadRGBAs.push_back(voxelRGBA[0]);
//            quadRGBAs.push_back(voxelRGBA[1]);
//            quadRGBAs.push_back(voxelRGBA[2]);
//            quadRGBAs.push_back(voxelRGBA[3]);
//            
//            quadNormals.push_back(sliceNormalVector[0]);
//            quadNormals.push_back(sliceNormalVector[1]);
//            quadNormals.push_back(sliceNormalVector[2]);
//            
//            if (doPerVertexNormalsAndColors) {
//                quadRGBAs.push_back(voxelRGBA[0]);
//                quadRGBAs.push_back(voxelRGBA[1]);
//                quadRGBAs.push_back(voxelRGBA[2]);
//                quadRGBAs.push_back(voxelRGBA[3]);
//                
//                quadNormals.push_back(sliceNormalVector[0]);
//                quadNormals.push_back(sliceNormalVector[1]);
//                quadNormals.push_back(sliceNormalVector[2]);
//                
//                quadRGBAs.push_back(voxelRGBA[0]);
//                quadRGBAs.push_back(voxelRGBA[1]);
//                quadRGBAs.push_back(voxelRGBA[2]);
//                quadRGBAs.push_back(voxelRGBA[3]);
//                
//                quadNormals.push_back(sliceNormalVector[0]);
//                quadNormals.push_back(sliceNormalVector[1]);
//                quadNormals.push_back(sliceNormalVector[2]);
//                
//                quadRGBAs.push_back(voxelRGBA[0]);
//                quadRGBAs.push_back(voxelRGBA[1]);
//                quadRGBAs.push_back(voxelRGBA[2]);
//                quadRGBAs.push_back(voxelRGBA[3]);
//                
//                quadNormals.push_back(sliceNormalVector[0]);
//                quadNormals.push_back(sliceNormalVector[1]);
//                quadNormals.push_back(sliceNormalVector[2]);
//            }
//            
//            for (int32_t iq = 0; iq < 12; iq++) {
//                quadCoords.push_back(vtd->m_coordinates[iq]);
//            }
//        }
    }
    
    quadCoordsVector.resize(coordOffset);
    quadNormalsVector.resize(normalOffset);
    quadRGBAsVector.resize(rgbaOffset);
    
    for (std::vector<VoxelToDraw*>::iterator iter = voxelsToDraw.begin();
         iter != voxelsToDraw.end();
         iter++) {
        VoxelToDraw* vtd = *iter;
        delete vtd;
    }
    voxelsToDraw.clear();
    
    if ( ! quadCoordsVector.empty()) {
        glPushMatrix();
        glScalef(zoom, zoom, zoom);
        drawQuads(quadCoordsVector,
                  quadNormalsVector,
                  quadRGBAsVector);
        glPopMatrix();
    }
}

/**
 * Draw surface outlines on the volume slices
 *
 * @param plane
 *   Plane of the volume slice on which surface outlines are drawn.
 */
void
BrainOpenGLFPVolumeObliqueDrawing::drawSurfaceOutline(const Plane& plane)
{
    if ( ! plane.isValidPlane()) {
        return;
    }
    
    float intersectionPoint1[3];
    float intersectionPoint2[3];
    
    m_fixedPipelineDrawing->enableLineAntiAliasing();
    
    VolumeSurfaceOutlineSetModel* outlineSet = m_browserTabContent->getVolumeSurfaceOutlineSet();
    
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
                const float lineWidth = m_fixedPipelineDrawing->modelSizeToPixelSize(thickness);
                
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
                    nodeColoringRGBA = m_fixedPipelineDrawing->surfaceNodeColoring->colorSurfaceNodes(NULL, /*modelDisplayController*/
                                                                                    surface,
                                                                                    colorSourceBrowserTabIndex);
                }
                
                glColor3fv(CaretColorEnum::toRGB(outlineColor));
                m_fixedPipelineDrawing->setLineWidth(lineWidth);
                
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
                    }
                }
                glEnd();
            }
        }
    }
    
    m_fixedPipelineDrawing->disableLineAntiAliasing();
}

/**
 * Draw foci on volume slice.
 *
 * @param plane
 *   Plane of the volume slice on which surface outlines are drawn.
 */
void
BrainOpenGLFPVolumeObliqueDrawing::drawVolumeSliceFoci(const Plane& plane)
{
    SelectionItemFocusVolume* idFocus = m_brain->getSelectionManager()->getVolumeFocusIdentification();
    
    /*
     * Check for a 'selection' type mode
     */
    bool isSelect = false;
    switch (m_fixedPipelineDrawing->mode) {
        case BrainOpenGLFixedPipeline::MODE_DRAWING:
            break;
        case BrainOpenGLFixedPipeline::MODE_IDENTIFICATION:
            if (idFocus->isEnabledForSelection()) {
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
    
    VolumeMappableInterface* underlayVolume = m_volumeDrawInfo[0].volumeFile;
//    int64_t dimI, dimJ, dimK, numMaps, numComponents;
//    underlayVolume->getDimensions(dimI, dimJ, dimK, numMaps, numComponents);
//    
//    /*
//     * Slice thicknesses
//     */
//    float sliceXYZ[3];
//    float sliceNextXYZ[3];
//    underlayVolume->indexToSpace(0, 0, 0, sliceXYZ[0], sliceXYZ[1], sliceXYZ[2]);
//    underlayVolume->indexToSpace(1, 1, 1, sliceNextXYZ[0], sliceNextXYZ[1], sliceNextXYZ[2]);
//    const float sliceThicknesses[3] = {
//        sliceNextXYZ[0] - sliceXYZ[0],
//        sliceNextXYZ[1] - sliceXYZ[1],
//        sliceNextXYZ[2] - sliceXYZ[2]
//    };
    const float sliceThickness = 2.0;
    const float halfSliceThickness = sliceThickness * 0.5;
    
    
    const DisplayPropertiesFoci* fociDisplayProperties = m_brain->getDisplayPropertiesFoci();
    const DisplayGroupEnum::Enum displayGroup = fociDisplayProperties->getDisplayGroupForTab(m_fixedPipelineDrawing->windowTabIndex);
    
    if (fociDisplayProperties->isDisplayed(displayGroup,
                                           m_fixedPipelineDrawing->windowTabIndex) == false) {
        return;
    }
    const float focusDiameter = fociDisplayProperties->getFociSize(displayGroup,
                                                                   m_fixedPipelineDrawing->windowTabIndex);
    const FeatureColoringTypeEnum::Enum fociColoringType = fociDisplayProperties->getColoringType(displayGroup,
                                                                                                  m_fixedPipelineDrawing->windowTabIndex);
    
    bool drawAsSpheres = false;
    switch (fociDisplayProperties->getDrawingType(displayGroup,
                                                  m_fixedPipelineDrawing->windowTabIndex)) {
        case FociDrawingTypeEnum::DRAW_AS_SPHERES:
            drawAsSpheres = true;
            break;
        case FociDrawingTypeEnum::DRAW_AS_SQUARES:
            break;
    }
    
    /*
     * Use some polygon offset that will adjust the depth values of the 
     * foci so that the foci depth values place the foci in front of
     * the volume slice.
     */
    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(0.0, -1.0);
    
    /*
     * Process each foci file
     */
    const int32_t numberOfFociFiles = m_brain->getNumberOfFociFiles();
    for (int32_t iFile = 0; iFile < numberOfFociFiles; iFile++) {
        FociFile* fociFile = m_brain->getFociFile(iFile);
        
        const GroupAndNameHierarchyModel* classAndNameSelection = fociFile->getGroupAndNameHierarchyModel();
        if (classAndNameSelection->isSelected(displayGroup,
                                              m_fixedPipelineDrawing->windowTabIndex) == false) {
            continue;
        }
        
        const GiftiLabelTable* classColorTable = fociFile->getClassColorTable();
        const GiftiLabelTable* nameColorTable = fociFile->getNameColorTable();
        
        const int32_t numFoci = fociFile->getNumberOfFoci();
        
        for (int32_t j = 0; j < numFoci; j++) {
            Focus* focus = fociFile->getFocus(j);
            
            const GroupAndNameHierarchyItem* groupNameItem = focus->getGroupNameSelectionItem();
            if (groupNameItem != NULL) {
                if (groupNameItem->isSelected(displayGroup,
                                              m_fixedPipelineDrawing->windowTabIndex) == false) {
                    continue;
                }
            }
            
            float rgba[4] = { 0.0, 0.0, 0.0, 1.0 };
            switch (fociColoringType) {
                case FeatureColoringTypeEnum::FEATURE_COLORING_TYPE_CLASS:
                    if (focus->isClassRgbaValid() == false) {
                        const GiftiLabel* colorLabel = classColorTable->getLabelBestMatching(focus->getClassName());
                        if (colorLabel != NULL) {
                            focus->setClassRgba(colorLabel->getColor());
                        }
                        else {
                            focus->setClassRgba(rgba);
                        }
                    }
                    focus->getClassRgba(rgba);
                    break;
                case FeatureColoringTypeEnum::FEATURE_COLORING_TYPE_NAME:
                    if (focus->isNameRgbaValid() == false) {
                        const GiftiLabel* colorLabel = nameColorTable->getLabelBestMatching(focus->getName());
                        if (colorLabel != NULL) {
                            focus->setNameRgba(colorLabel->getColor());
                        }
                        else {
                            focus->setNameRgba(rgba);
                        }
                    }
                    focus->getNameRgba(rgba);
                    break;
            }
            
            glColor3fv(rgba);
            
            
            const int32_t numProjections = focus->getNumberOfProjections();
            for (int32_t k = 0; k < numProjections; k++) {
                const SurfaceProjectedItem* spi = focus->getProjection(k);
                if (spi->isVolumeXYZValid()) {
                    float xyz[3];
                    spi->getVolumeXYZ(xyz);
                    
                    bool drawIt = false;
                    if (plane.absoluteDistanceToPlane(xyz) < halfSliceThickness) {
                        drawIt = true;
                    }
                    
                    if (drawIt) {
                        if (isSelect) {
                            uint8_t idRGB[3];
                            m_fixedPipelineDrawing->colorIdentification->addItem(idRGB,
                                                               SelectionItemDataTypeEnum::FOCUS_VOLUME,
                                                               iFile, // file index
                                                               j, // focus index
                                                               k);// projection index
                            glColor3ubv(idRGB);
                        }
                        
                        glPushMatrix();
                        glTranslatef(xyz[0], xyz[1], xyz[2]);
                        if (drawAsSpheres) {
                            m_fixedPipelineDrawing->drawSphereWithDiameter(focusDiameter);
                        }
                        else {
                            drawSquare(focusDiameter);
                        }
                        glPopMatrix();
                    }
                }
            }
        }
    }
    
    /*
     * Disable the polygon offset
     */
    glDisable(GL_POLYGON_OFFSET_FILL);

    if (isSelect) {
        int32_t fociFileIndex = -1;
        int32_t focusIndex = -1;
        int32_t focusProjectionIndex = -1;
        float depth = -1.0;
        m_fixedPipelineDrawing->getIndexFromColorSelection(SelectionItemDataTypeEnum::FOCUS_VOLUME,
                                         m_fixedPipelineDrawing->mouseX,
                                         m_fixedPipelineDrawing->mouseY,
                                         fociFileIndex,
                                         focusIndex,
                                         focusProjectionIndex,
                                         depth);
        if (fociFileIndex >= 0) {
            if (idFocus->isOtherScreenDepthCloserToViewer(depth)) {
                Focus* focus = m_brain->getFociFile(fociFileIndex)->getFocus(focusIndex);
                idFocus->setBrain(m_brain);
                idFocus->setFocus(focus);
                idFocus->setFociFile(m_brain->getFociFile(fociFileIndex));
                idFocus->setFocusIndex(focusIndex);
                idFocus->setFocusProjectionIndex(focusProjectionIndex);
                idFocus->setVolumeFile(underlayVolume);
                idFocus->setScreenDepth(depth);
                float xyz[3];
                const SurfaceProjectedItem* spi = focus->getProjection(focusProjectionIndex);
                spi->getVolumeXYZ(xyz);
                m_fixedPipelineDrawing->setSelectedItemScreenXYZ(idFocus, xyz);
                CaretLogFine("Selected Volume Focus Identification Symbol: " + QString::number(focusIndex));
            }
        }
    }
}


/**
 * Get the maximum bounds that enclose the volumes and the minimum
 * voxel spacing from the volumes.
 *
 * @param boundsOut
 *    Bounds of the volumes.
 * @param spacingOut
 *    Minimum voxel spacing from the volumes.
 *
 */
bool
BrainOpenGLFPVolumeObliqueDrawing::getVoxelCoordinateBoundsAndSpacing(float boundsOut[6],
                                                                      float spacingOut[3])
{
    const int32_t numberOfVolumesToDraw = static_cast<int32_t>(m_volumeDrawInfo.size());
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
        const VolumeMappableInterface* volumeFile = m_volumeDrawInfo[i].volumeFile;
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
BrainOpenGLFPVolumeObliqueDrawing::drawSurfaces(const int viewport[4])
{
    std::vector<const Surface*> surfaces = m_brain->getVolumeInteractionSurfaces();
    
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
            
            m_fixedPipelineDrawing->setViewportAndOrthographicProjection(viewport,
                                                                       ProjectionViewTypeEnum::PROJECTION_VIEW_LEFT_LATERAL);
            
            const float center[3] = { 0.0, 0.0, 0.0 };
            m_fixedPipelineDrawing->applyViewingTransformations(center,
                                              ProjectionViewTypeEnum::PROJECTION_VIEW_LEFT_LATERAL);
            
            glMatrixMode(GL_MODELVIEW);
            glLoadIdentity();
            m_fixedPipelineDrawing->drawSurface(const_cast<Surface*>(sf),
                                              &nodeColors[0]);
            
        }
    }
    
    glPushMatrix();
    glLoadIdentity();
    
    /*
     * Translate to selected location
     */
    const float centerX = m_browserTabContent->getSliceCoordinateParasagittal();
    const float centerY = m_browserTabContent->getSliceCoordinateCoronal();
    const float centerZ = m_browserTabContent->getSliceCoordinateAxial();
    Matrix4x4 transformationMatrix;
    transformationMatrix.translate(centerX,
                                   centerY,
                                   centerZ);
    
    /*
     * Apply rotation
     */
    Matrix4x4 rotationMatrix = m_browserTabContent->getRotationMatrix();
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
        
        drawLines(coords,
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
        
        drawLines(coords,
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
        
        drawLines(coords,
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

/**
 * Draw lines segments (2 coords per segment).
 *
 * @param coordinates
 *    Coordinates of the lines.
 * @param rgbaColors
 *    RGBA colors for the lines.
 * @param thickness
 *    Thickness of the lines
 */
void
BrainOpenGLFPVolumeObliqueDrawing::drawLines(const std::vector<float>& coordinates,
                                             const std::vector<uint8_t>& rgbaColors,
                                             const float thickness)
{
    m_fixedPipelineDrawing->setLineWidth(thickness);
    
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

/**
 * Draw quads.
 *
 * @param coordinates
 *    Coordinates of the quads.
 * @param normals
 *    Normal vectors for the quads.
 * @param rgbaColors
 *    RGBA colors for the quads.
 */
void
BrainOpenGLFPVolumeObliqueDrawing::drawQuads(const std::vector<float>& coordinates,
                                             const std::vector<float>& normals,
                                             const std::vector<uint8_t>& rgbaColors)
{
    const uint64_t numCoords  = coordinates.size() / 3;
    const uint64_t numNormals = normals.size() / 3;
    const uint64_t numColors  = rgbaColors.size() / 4;
    const uint64_t numQuads = numCoords / 4;  // 4 three-d coords per quad
    
    if (numQuads <= 0) {
        return;
    }
    
    
    if (numNormals != numCoords) {
        const AString message = ("Size of normals must equal size of coordinates. "
                                 "Coordinate size: " + AString::number(coordinates.size())
                                 + "  Normals size: " + AString::number(normals.size()));
        CaretLogSevere(message);
        CaretAssertMessage(0, message);
        return;
    }
    
    if (numColors != numCoords) {
        const AString message = ("Size of RGBA colors must be 4/3 size of coordinates. "
                                 "Coordinate size: " + AString::number(coordinates.size())
                                 + "  RGBA size: " + AString::number(rgbaColors.size()));
        CaretLogSevere(message);
        CaretAssertMessage(0, message);
        return;
    }
    
    bool wasDrawnWithVertexBuffers = false;
#ifdef BRAIN_OPENGL_INFO_SUPPORTS_VERTEX_BUFFERS
    if (BrainOpenGL::isVertexBuffersSupported()) {
        drawQuadsVertexBuffers(coordinates,
                               normals,
                               rgbaColors);
        wasDrawnWithVertexBuffers = true;
        std::cout << "Using vertex buffers!!" << std::endl;
    }
#endif // BRAIN_OPENGL_INFO_SUPPORTS_VERTEX_BUFFERS
    
    
    if ( ! wasDrawnWithVertexBuffers) {

        drawQuadsVertexArrays(coordinates,
                          normals,
                          rgbaColors);
//    drawQuadsImmediateMode(coordinates,
//                           normals,
//                           rgbaColors);
    }
}

/**
 * Draw quads using immediate mode.
 *
 * @param coordinates
 *    Coordinates of the quads.
 * @param normals
 *    Normal vectors for the quads.
 * @param rgbaColors
 *    RGBA colors for the quads.
 */
void
BrainOpenGLFPVolumeObliqueDrawing::drawQuadsImmediateMode(const std::vector<float>& coordinates,
                            const std::vector<float>& normals,
                            const std::vector<uint8_t>& rgbaColors)
{
    const uint64_t numCoords  = coordinates.size() / 3;
    const uint64_t numQuads = numCoords / 4;  // 4 three-d coords per quad
    
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
        
        glNormal3fv(&normalPtr[iNormal]);
        iNormal += 3;
        glColor4ubv(&colorPtr[iColor]);
        iColor += 4;
        glVertex3fv(&coordPtr[iCoord+3]);
        
        glNormal3fv(&normalPtr[iNormal]);
        iNormal += 3;
        glColor4ubv(&colorPtr[iColor]);
        iColor += 4;
        glVertex3fv(&coordPtr[iCoord+6]);
        
        glNormal3fv(&normalPtr[iNormal]);
        iNormal += 3;
        glColor4ubv(&colorPtr[iColor]);
        iColor += 4;
        glVertex3fv(&coordPtr[iCoord+9]);
    }
    glEnd();
    
}

/**
 * Draw quads using vertex arrays.
 *
 * @param coordinates
 *    Coordinates of the quads.
 * @param normals
 *    Normal vectors for the quads.
 * @param rgbaColors
 *    RGBA colors for the quads.
 */
void
BrainOpenGLFPVolumeObliqueDrawing::drawQuadsVertexArrays(const std::vector<float>& coordinates,
                           const std::vector<float>& normals,
                           const std::vector<uint8_t>& rgbaColors)
{
    const uint64_t numCoords  = coordinates.size() / 3;
    
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
    glVertexPointer(3,
                    GL_FLOAT,
                    0,
                    reinterpret_cast<const GLvoid*>(&coordinates[0]));
    glColorPointer(4,
                   GL_UNSIGNED_BYTE,
                   0,
                   reinterpret_cast<const GLvoid*>(&rgbaColors[0]));
    glNormalPointer(GL_FLOAT,
                    0,
                    reinterpret_cast<const GLvoid*>(&normals[0]));
    
    glDrawArrays(GL_QUADS,
                 0,
                 numCoords);
    
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
}

/**
 * Draw quads using vertex buffers.
 *
 * @param coordinates
 *    Coordinates of the quads.
 * @param normals
 *    Normal vectors for the quads.
 * @param rgbaColors
 *    RGBA colors for the quads.
 */
void
BrainOpenGLFPVolumeObliqueDrawing::drawQuadsVertexBuffers(const std::vector<float>& coordinates,
                                                          const std::vector<float>& normals,
                                                          const std::vector<uint8_t>& rgbaColors)
{
    const uint64_t numCoords  = coordinates.size() / 3;
    
#ifdef BRAIN_OPENGL_INFO_SUPPORTS_VERTEX_BUFFERS
    if (BrainOpenGL::isVertexBuffersSupported()) {
            /*
             * Put vertices (coordinates) into its buffer.
             */
        GLuint vertexBufferID = -1;
        glGenBuffers(1, &vertexBufferID);
            glBindBuffer(GL_ARRAY_BUFFER,
                         vertexBufferID);
            glBufferData(GL_ARRAY_BUFFER,
                         coordinates.size() * sizeof(GLfloat),
                         &coordinates[0],
                         GL_STREAM_DRAW);
            
            /*
             * Put normals into its buffer.
             */
        GLuint normalBufferID = -1;
        glGenBuffers(1, &normalBufferID);
            glBindBuffer(GL_ARRAY_BUFFER,
                         normalBufferID);
            glBufferData(GL_ARRAY_BUFFER,
                         normals.size() * sizeof(GLfloat),
                         &normals[0],
                         GL_STREAM_DRAW);
            
        /*
         * Put colors into its buffer.
         */
        GLuint colorBufferID = -1;
        glGenBuffers(1, &colorBufferID);
        glBindBuffer(GL_ARRAY_BUFFER,
                     colorBufferID);
        glBufferData(GL_ARRAY_BUFFER,
                     rgbaColors.size() * sizeof(uint8_t),
                     &rgbaColors[0],
                     GL_STREAM_DRAW);
        
//            /*
//             * Put triangle strips into its buffer.
//             */
//        GLuint quadsBufferID = -1;
//        glGenBuffers(1, &quadsBufferID);
//            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,
//                         quadsBufferID);
//            glBufferData(GL_ELEMENT_ARRAY_BUFFER,
//                         quads.size() * sizeof(GLuint),
//                         &quads[0],
//                         GL_STREAM_DRAW);
        
            /*
             * Deselect active buffer.
             */
            glBindBuffer(GL_ARRAY_BUFFER,
                         0);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,
                         0);
        
        
        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_COLOR_ARRAY);
        glEnableClientState(GL_NORMAL_ARRAY);
        
        /*
         * Set the vertices for drawing.
         */
        glBindBuffer(GL_ARRAY_BUFFER,
                     vertexBufferID);
        glVertexPointer(3,
                        GL_FLOAT,
                        0,
                        (GLvoid*)0);
        
        /*
         * Set the normal vectors for drawing.
         */
        glBindBuffer(GL_ARRAY_BUFFER,
                     normalBufferID);
        glNormalPointer(GL_FLOAT,
                        0,
                        (GLvoid*)0);
        
        /*
         * Set the rgba colors for drawing
         */
        glBindBuffer(GL_ARRAY_BUFFER,
                     colorBufferID);
        glColorPointer(4,
                       GL_UNSIGNED_BYTE,
                       0,
                       (GLvoid*)0);
        
        /*
         * Draw the triangle strips.
         */
        glDrawArrays(GL_QUADS,
                     0,
                     numCoords);
//        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,
//                     quadsBufferID);
//        glDrawElements(GL_TRIANGLE_STRIP,
//                       quads.size(),
//                       GL_UNSIGNED_INT,
//                       (GLvoid*)0);
        /*
         * Deselect active buffer.
         */
        glBindBuffer(GL_ARRAY_BUFFER,
                     0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,
                     0);
        
        glDisableClientState(GL_VERTEX_ARRAY);
        glDisableClientState(GL_NORMAL_ARRAY);

        glDeleteBuffers(1, &vertexBufferID);
        glDeleteBuffers(1, &normalBufferID);
        glDeleteBuffers(1, &colorBufferID);
    }
#endif // BRAIN_OPENGL_INFO_SUPPORTS_VERTEX_BUFFERS
}

/**
 * Draw a one millimeter square facing the user.
 * NOTE: This method will alter the current
 * modelviewing matrices so caller may need
 * to enclose the call to this method within
 * glPushMatrix() and glPopMatrix().
 *
 * @param size
 *     Size of square.
 */
void
BrainOpenGLFPVolumeObliqueDrawing::drawSquare(const float size)
{
    const float length = size * 0.5;
    
    /*
     * Draw both front and back side since in some instances,
     * such as surface montage, we are viweing from the far
     * side (from back of monitor)
     */
    glBegin(GL_QUADS);
    glNormal3f(0.0, 0.0, 1.0);
    glVertex3f(-length, -length, 0.0);
    glVertex3f( length, -length, 0.0);
    glVertex3f( length,  length, 0.0);
    glVertex3f(-length,  length, 0.0);
    glNormal3f(0.0, 0.0, -1.0);
    glVertex3f(-length, -length, 0.0);
    glVertex3f(-length,  length, 0.0);
    glVertex3f( length,  length, 0.0);
    glVertex3f( length, -length, 0.0);
    glEnd();
}

/* ======================================================================= */
/**
 * Create a voxel for drawing.
 *
 * @param center
 *    Center of voxel.
 * @param leftBottom
 *    Left bottom coordinate of voxel.
 * @param rightBottom
 *    Right bottom coordinate of voxel.
 * @param rightTop
 *    Right top coordinate of voxel.
 * @param leftTop
 *    Left top coordinate of voxel.
 */
BrainOpenGLFPVolumeObliqueDrawing::VoxelToDraw::VoxelToDraw(const float center[3],
            const double leftBottom[3],
            const double rightBottom[3],
            const double rightTop[3],
            const double leftTop[3])
{
    m_center[0] = center[0];
    m_center[1] = center[1];
    m_center[2] = center[2];
    
    m_coordinates[0]  = leftBottom[0];
    m_coordinates[1]  = leftBottom[1];
    m_coordinates[2]  = leftBottom[2];
    m_coordinates[3]  = rightBottom[0];
    m_coordinates[4]  = rightBottom[1];
    m_coordinates[5]  = rightBottom[2];
    m_coordinates[6]  = rightTop[0];
    m_coordinates[7]  = rightTop[1];
    m_coordinates[8]  = rightTop[2];
    m_coordinates[9]  = leftTop[0];
    m_coordinates[10] = leftTop[1];
    m_coordinates[11] = leftTop[2];
    
    const int64_t numSlices = 5;
    m_sliceIndices.reserve(numSlices);
    m_sliceOffsets.reserve(numSlices);
}

/**
 * Add a value from a volume slice.
 *
 * @param sliceIndex
 *    Index of the slice.
 * @param sliceOffset
 *    Offset of value in the slice.
 */
void
BrainOpenGLFPVolumeObliqueDrawing::VoxelToDraw::addVolumeValue(const int64_t sliceIndex,
                    const int64_t sliceOffset)
{
    CaretAssert(sliceIndex >= 0);
    CaretAssert(sliceOffset >= 0);
    m_sliceIndices.push_back(sliceIndex);
    m_sliceOffsets.push_back(sliceOffset);
}


/* ======================================================================= */

/**
 * Constructor
 *
 * @param volumeMappableInterface
 *   Volume that contains the data values.
 */
BrainOpenGLFPVolumeObliqueDrawing::VolumeSlice::VolumeSlice(VolumeMappableInterface* volumeMappableInterface,
            const int32_t mapIndex)
{
    m_volumeMappableInterface = volumeMappableInterface;
    m_volumeFile = dynamic_cast<VolumeFile*>(m_volumeMappableInterface);
    m_ciftiMappableDataFile = dynamic_cast<CiftiMappableDataFile*>(m_volumeMappableInterface);
    
    CaretAssert(m_volumeMappableInterface);
    m_mapIndex = mapIndex;
    CaretAssert(m_mapIndex >= 0);
    
    const int64_t sliceDim = 300;
    const int64_t numVoxels = sliceDim * sliceDim;
    m_values.reserve(numVoxels);
}

/**
 * Add a value and return its index.
 *
 * @param value
 *     Value that is added.
 * @return
 *     The index for the value.
 */
int64_t
BrainOpenGLFPVolumeObliqueDrawing::VolumeSlice::addValue(const float value)
{
    const int64_t indx = static_cast<int64_t>(m_values.size());
    m_values.push_back(value);
    return indx;
}

/**
 * Return RGBA colors for value using the value's index
 * returned by addValue().
 *
 * @param indx
 *    Index of the value.
 * @return
 *    RGBA coloring for value.
 */
uint8_t*
BrainOpenGLFPVolumeObliqueDrawing::VolumeSlice::getRgbaForValueByIndex(const int64_t indx)
{
    CaretAssertVectorIndex(m_rgba, indx * 4);
    return &m_rgba[indx*4];
}

/**
 * Allocate colors for the voxel values
 */
void
BrainOpenGLFPVolumeObliqueDrawing::VolumeSlice::allocateColors()
{
    m_rgba.resize(m_values.size() * 4);
}

