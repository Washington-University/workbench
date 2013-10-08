
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

#include "CaretOpenGLInclude.h"

#define __BRAIN_OPEN_G_L_F_P_VOLUME_OBLIQUE_DRAWING_DECLARE__
#include "BrainOpenGLVolumeSliceDrawing.h"
#undef __BRAIN_OPEN_G_L_F_P_VOLUME_OBLIQUE_DRAWING_DECLARE__

#include "BoundingBox.h"
#include "Brain.h"
#include "BrainOpenGLPrimitiveDrawing.h"
#include "BrowserTabContent.h"
#include "CaretAssert.h"
#include "CaretLogger.h"
#include "CaretPreferences.h"
#include "CiftiMappableDataFile.h"
#include "DisplayPropertiesFoci.h"
#include "DisplayPropertiesLabels.h"
#include "DisplayPropertiesVolume.h"
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
 * \class caret::BrainOpenGLVolumeSliceDrawing 
 * \brief Performs oblique volume slice drawing.
 * \ingroup Brain
 *
 */

/**
 * Constructor.
 */
BrainOpenGLVolumeSliceDrawing::BrainOpenGLVolumeSliceDrawing()
: CaretObject()
{
    m_lookAtCenter[0] = 0.0;
    m_lookAtCenter[1] = 0.0;
    m_lookAtCenter[2] = 0.0;
}

/**
 * Destructor.
 */
BrainOpenGLVolumeSliceDrawing::~BrainOpenGLVolumeSliceDrawing()
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
 * @param sliceViewMode
 *   Slice viewing mode.
 * @param viewport
 *   Viewport in which drawing takes place.
 */
void
BrainOpenGLVolumeSliceDrawing::draw(BrainOpenGLFixedPipeline* fixedPipelineDrawing,
                                        BrowserTabContent* browserTabContent,
                                        std::vector<BrainOpenGLFixedPipeline::VolumeDrawInfo>& volumeDrawInfo,
                                        const VolumeSliceViewModeEnum::Enum sliceViewMode,
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
    
    m_sliceViewMode = sliceViewMode;
    
    const int32_t invalidSliceIndex = -1;
    
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
        bool drawMontageView = false;
        bool drawThreeSliceView = false;
        switch (sliceViewMode) {
            case VolumeSliceViewModeEnum::MONTAGE:
                drawMontageView = true;
                break;
            case VolumeSliceViewModeEnum::OBLIQUE:
                drawThreeSliceView = true;
                break;
            case VolumeSliceViewModeEnum::ORTHOGONAL:
                break;
        }
        
        if (drawMontageView) {
            drawSliceMontage(viewport);
        }
        else {
            glEnable(GL_DEPTH_TEST);
            
            const VolumeSliceViewPlaneEnum::Enum slicePlane = browserTabContent->getSliceViewPlane();
            switch (slicePlane) {
                case VolumeSliceViewPlaneEnum::ALL:
                {
                    const int gap = 2;
                    
                    const int vpHalfX = viewport[2] / 2;
                    const int vpHalfY = viewport[3] / 2;
                    
                    const int paraVP[4] = {
                        viewport[0],
                        viewport[1] + vpHalfY + gap,
                        vpHalfX - gap,
                        vpHalfY - gap
                    };
                    glPushMatrix();
                    drawSliceForSliceView(VolumeSliceViewPlaneEnum::PARASAGITTAL,
                                             DRAW_MODE_VOLUME_VIEW_SLICE_SINGLE,
                                             invalidSliceIndex,
                                             paraVP);
                    glPopMatrix();
                    
                    
                    const int coronalVP[4] = {
                        viewport[0] + vpHalfX + gap,
                        viewport[1] + vpHalfY + gap,
                        vpHalfX - gap,
                        vpHalfY - gap
                    };
                    glPushMatrix();
                    drawSliceForSliceView(VolumeSliceViewPlaneEnum::CORONAL,
                                             DRAW_MODE_VOLUME_VIEW_SLICE_SINGLE,
                                             invalidSliceIndex,
                                             coronalVP);
                    glPopMatrix();
                    
                    
                    const int axialVP[4] = {
                        viewport[0] + vpHalfX + gap,
                        viewport[1],
                        vpHalfX - gap,
                        vpHalfY - gap
                    };
                    glPushMatrix();
                    drawSliceForSliceView(VolumeSliceViewPlaneEnum::AXIAL,
                                             DRAW_MODE_VOLUME_VIEW_SLICE_SINGLE,
                                             invalidSliceIndex,
                                             axialVP);
                    glPopMatrix();
                    const int allVP[4] = {
                        viewport[0],
                        viewport[1],
                        vpHalfX - gap,
                        vpHalfY - gap
                    };
                    
                    
                    if (drawThreeSliceView) {
                        drawAllThreeSlicesForVolumeSliceView(allVP);
                    }
                }
                    break;
                case VolumeSliceViewPlaneEnum::AXIAL:
                case VolumeSliceViewPlaneEnum::CORONAL:
                case VolumeSliceViewPlaneEnum::PARASAGITTAL:
                    glPushMatrix();
                    drawSliceForSliceView(slicePlane,
                                             DRAW_MODE_VOLUME_VIEW_SLICE_SINGLE,
                                             invalidSliceIndex,
                                             viewport);
                    glPopMatrix();
                    break;
            }
        }
    }
    
    if (browserTabContent->getDisplayedWholeBrainModel() != NULL) {
        drawSlicesForAllStructuresView(viewport);
    }
}

/**
 * Draw an orthogonal volume slice montage
 * @param viewport
 *    The viewport.
 */
void
BrainOpenGLVolumeSliceDrawing::drawSliceMontage(const int viewport[4])
{
    const int numRows = m_browserTabContent->getMontageNumberOfRows();
    CaretAssert(numRows > 0);
    const int numCols = m_browserTabContent->getMontageNumberOfColumns();
    CaretAssert(numCols > 0);
    
    const CaretPreferences* caretPreferences = SessionManager::get()->getCaretPreferences();
    const int montageMargin = caretPreferences->getVolumeMontageGap();
    
    const int totalGapX = montageMargin * (numCols - 1);
    const int vpSizeX = (viewport[2] - totalGapX) / numCols;
    const int totalGapY = montageMargin * (numRows - 1);
    const int vpSizeY = (viewport[3] - totalGapY) / numRows;
    
    VolumeMappableInterface* underlayVolume = m_volumeDrawInfo[0].volumeFile;
    /*
     * Voxel sizes for underlay volume
     */
    float originX, originY, originZ;
    float x1, y1, z1;
    underlayVolume->indexToSpace(0, 0, 0, originX, originY, originZ);
    underlayVolume->indexToSpace(1, 1, 1, x1, y1, z1);
    float sliceThickness = 0.0;
    float sliceOrigin    = 0.0;
    
    AString axisLetter = "";
    
    int sliceIndex = -1;
    int maximumSliceIndex = -1;
    int64_t dimI, dimJ, dimK, numMaps, numComponents;
    m_volumeDrawInfo[0].volumeFile->getDimensions(dimI, dimJ, dimK, numMaps, numComponents);
    const int sliceStep = m_browserTabContent->getMontageSliceSpacing();
    const VolumeSliceViewPlaneEnum::Enum slicePlane = m_browserTabContent->getSliceViewPlane();
    switch (slicePlane) {
        case VolumeSliceViewPlaneEnum::ALL:
            sliceIndex = -1;
            break;
        case VolumeSliceViewPlaneEnum::AXIAL:
            sliceIndex = m_browserTabContent->getSliceIndexAxial(underlayVolume);
            maximumSliceIndex = dimK;
            sliceThickness = z1 - originZ;
            sliceOrigin = originZ;
            axisLetter = "Z";
            break;
        case VolumeSliceViewPlaneEnum::CORONAL:
            sliceIndex = m_browserTabContent->getSliceIndexCoronal(underlayVolume);
            maximumSliceIndex = dimJ;
            sliceThickness = y1 - originY;
            sliceOrigin = originY;
            axisLetter = "Y";
            break;
        case VolumeSliceViewPlaneEnum::PARASAGITTAL:
            sliceIndex = m_browserTabContent->getSliceIndexParasagittal(underlayVolume);
            maximumSliceIndex = dimI;
            sliceThickness = x1 - originX;
            sliceOrigin = originX;
            axisLetter = "X";
            break;
    }
    
    /*
     * Foreground color for slice coordinate text
     */
    const CaretPreferences* prefs = SessionManager::get()->getCaretPreferences();
    uint8_t foregroundRGB[3];
    prefs->getColorForeground(foregroundRGB);
    const bool showCoordinates = prefs->isVolumeMontageAxesCoordinatesDisplayed();
    
    
    /*
     * Determine a slice offset to selected slices is in
     * the center of the montage
     */
    const int numSlicesViewed = (numCols * numRows);
    const int sliceOffset = ((numSlicesViewed / 2)
                             * sliceStep);
    sliceIndex -= sliceOffset;
    
    for (int i = 0; i < numRows; i++) {
        for (int j = 0; j < numCols; j++) {
            if ((sliceIndex >= 0)
                && (sliceIndex < maximumSliceIndex)) {
                const int vpX = (j * (vpSizeX + montageMargin));
                const int vpY = (i * (vpSizeY + montageMargin));
                int vp[4] = {
                    viewport[0] + vpX,
                    viewport[1] + vpY,
                    vpSizeX,
                    vpSizeY
                };
                
                if ((vp[2] <= 0)
                    || (vp[3] <= 0)) {
                    continue;
                }
                
                drawSliceForSliceView(slicePlane,
                                         DRAW_MODE_VOLUME_VIEW_SLICE_SINGLE,
                                         sliceIndex,
                                         vp);
                const float sliceCoord = (sliceOrigin
                                          + sliceThickness * sliceIndex);
                if (showCoordinates) {
                    const AString coordText = (axisLetter
                                               + "="
                                               + AString::number(sliceCoord, 'f', 0)
                                               + "mm");
                    glColor3ubv(foregroundRGB);
                    m_fixedPipelineDrawing->drawTextWindowCoords((vpSizeX - 5),
                                               5,
                                               coordText,
                                               BrainOpenGLTextRenderInterface::X_RIGHT,
                                               BrainOpenGLTextRenderInterface::Y_BOTTOM);
                }
            }
            sliceIndex += sliceStep;
        }
    }
}

/**
 * Draw slices for the all structures view.
 *
 * @param viewport
 *    The viewport.
 */
void
BrainOpenGLVolumeSliceDrawing::drawSlicesForAllStructuresView(const int viewport[4])
{
    const int32_t invalidSliceIndex = -1;
    
    m_orthographicBounds[0] = m_fixedPipelineDrawing->orthographicLeft;
    m_orthographicBounds[1] = m_fixedPipelineDrawing->orthographicRight;
    m_orthographicBounds[2] = m_fixedPipelineDrawing->orthographicBottom;
    m_orthographicBounds[3] = m_fixedPipelineDrawing->orthographicTop;
    m_orthographicBounds[4] = m_fixedPipelineDrawing->orthographicNear;
    m_orthographicBounds[5] = m_fixedPipelineDrawing->orthographicFar;
    
    if (m_browserTabContent->isSliceAxialEnabled()) {
        glPushMatrix();
        drawSliceForSliceView(VolumeSliceViewPlaneEnum::AXIAL,
                                 DRAW_MODE_ALL_STRUCTURES_VIEW,
                                 invalidSliceIndex,
                                 viewport);
        glPopMatrix();
    }
    
    if (m_browserTabContent->isSliceCoronalEnabled()) {
        glPushMatrix();
        drawSliceForSliceView(VolumeSliceViewPlaneEnum::CORONAL,
                                 DRAW_MODE_ALL_STRUCTURES_VIEW,
                                 invalidSliceIndex,
                                 viewport);
        glPopMatrix();
    }
    
    if (m_browserTabContent->isSliceParasagittalEnabled()) {
        glPushMatrix();
        drawSliceForSliceView(VolumeSliceViewPlaneEnum::PARASAGITTAL,
                                 DRAW_MODE_ALL_STRUCTURES_VIEW,
                                 invalidSliceIndex,
                                 viewport);
        glPopMatrix();
    }
}

/**
 * Draw volume view's three slice mode for oblique drawing.
 *
 * @param viewport
 *    The viewport.
 */
void
BrainOpenGLVolumeSliceDrawing::drawAllThreeSlicesForVolumeSliceView(const int viewport[4])
{
    const int32_t invalidSliceIndex = -1;
    
    glViewport(viewport[0],
               viewport[1],
               viewport[2],
               viewport[3]);
    
    setOrthographicProjection(VolumeSliceViewPlaneEnum::ALL, viewport);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    
    /*
     * Need to set to an axial/all view, then draw slices
     * Add method getIdentStuff() and processIdentStuff()
     * Then set viewing transform and draw oblique or orthogonal
     */
    const double eyeX = 0;
    const double eyeY = 0;
    const double eyeZ = 100;
    const double centerX = 0;
    const double centerY = 0;
    const double centerZ = 0;
    const double upX = 0;
    const double upY = 1;
    const double upZ = 0;
    gluLookAt(eyeX, eyeY, eyeZ,
              centerX, centerY, centerZ,
              upX, upY, upZ);
    
    const float zoom = m_browserTabContent->getScaling();
    glScalef(zoom, zoom, zoom);
    
    glPushMatrix();
    drawSliceForSliceView(VolumeSliceViewPlaneEnum::AXIAL,
                             DRAW_MODE_VOLUME_VIEW_SLICE_3D,
                             invalidSliceIndex,
                             viewport);
    glPopMatrix();


    glPushMatrix();
    drawSliceForSliceView(VolumeSliceViewPlaneEnum::CORONAL,
                             DRAW_MODE_VOLUME_VIEW_SLICE_3D,
                             invalidSliceIndex,
                             viewport);
    glPopMatrix();
    
    
    glPushMatrix();
    drawSliceForSliceView(VolumeSliceViewPlaneEnum::PARASAGITTAL,
                             DRAW_MODE_VOLUME_VIEW_SLICE_3D,
                             invalidSliceIndex,
                             viewport);
    glPopMatrix();
    
    glPopMatrix();
}

/**
 * Set the orthographic projection.
 *
 * @param sliceViewPlane
 *    View plane that is displayed.
 * @param viewport
 *    The viewport.
 */
void
BrainOpenGLVolumeSliceDrawing::setOrthographicProjection(const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
                                                             const int viewport[4])
{

    /*
     * Determine model size in screen Y when viewed
     */
    BoundingBox boundingBox;
    m_volumeDrawInfo[0].volumeFile->getVoxelSpaceBoundingBox(boundingBox);
    double modelViewRangeY = 100.0;
    switch (sliceViewPlane) {
        case VolumeSliceViewPlaneEnum::ALL:
            modelViewRangeY = std::max(boundingBox.getDifferenceX(),
                                       std::max(boundingBox.getDifferenceY(),
                                                boundingBox.getDifferenceZ()));
            break;
        case VolumeSliceViewPlaneEnum::AXIAL:
            modelViewRangeY = boundingBox.getDifferenceY();
            break;
        case VolumeSliceViewPlaneEnum::CORONAL:
            modelViewRangeY = boundingBox.getDifferenceZ();
            break;
        case VolumeSliceViewPlaneEnum::PARASAGITTAL:
            modelViewRangeY = boundingBox.getDifferenceZ();
            break;
    }
    
    /*
     * Determine aspect ratio of viewport
     */
    const double viewportWidth = viewport[2];
    const double viewportHeight = viewport[3];
    const double aspectRatio = (viewportWidth
                                / viewportHeight);
    
    /*
     * Scale ratio makes region slightly larger than model
     */
    const double scaleRatio = 1.0 / 0.98;
 
    /*
     * Set bounds of orthographic projection
     */
    const double orthoSize = modelViewRangeY * scaleRatio;
    const double halfOrthoSize = orthoSize / 2.0;
    const double orthoBottom = -halfOrthoSize;
    const double orthoTop    =  halfOrthoSize;
    const double orthoRight  =  halfOrthoSize * aspectRatio;
    const double orthoLeft   = -halfOrthoSize * aspectRatio;
    const double nearDepth = -1000.0;
    const double farDepth  =  1000.0;
    m_orthographicBounds[0] = orthoLeft;
    m_orthographicBounds[1] = orthoRight;
    m_orthographicBounds[2] = orthoBottom;
    m_orthographicBounds[3] = orthoTop;
    m_orthographicBounds[4] = nearDepth;
    m_orthographicBounds[5] = farDepth;
    
    //std::cout << "New Orthographic Bounds: " << qPrintable(AString::fromNumbers(m_orthographicBounds, 6, ",")) << std::endl;
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(m_orthographicBounds[0],
            m_orthographicBounds[1],
            m_orthographicBounds[2],
            m_orthographicBounds[3],
            m_orthographicBounds[4],
            m_orthographicBounds[5]);
    glMatrixMode(GL_MODELVIEW);
}

/**
 * Set the volume slice viewing transformation.
 *
 * @param sliceViewPlane
 *    View plane that is displayed.
 * @param drawMode
 *    The drawing mode for slices.
 * @param planeOut
 *    Plane of slice after transforms (OUTPUT)
 * @param montageSliceIndex
 *    For mongtage drawing, indicates slice being drawn.
 * @param obliqueTransformationMatrixOut
 *    Output transformation matrix for oblique viewing.
 */
void
BrainOpenGLVolumeSliceDrawing::setVolumeSliceViewingTransformation(const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
                                                                       const DRAW_MODE drawMode,
                                                                       Plane& planeOut,
                                                                       const int32_t montageSliceIndex,
                                                                       Matrix4x4& obliqueTransformationMatrixOut)
{
    bool isSetViewTranform = false;
    bool isSetZoom = false;
    switch (drawMode) {
        case DRAW_MODE_ALL_STRUCTURES_VIEW:
            break;
        case DRAW_MODE_VOLUME_VIEW_SLICE_SINGLE:
            isSetViewTranform = true;
            isSetZoom = true;
            break;
        case DRAW_MODE_VOLUME_VIEW_SLICE_3D:
            break;
    }
    
    BoundingBox boundingBox;
    m_volumeDrawInfo[0].volumeFile->getVoxelSpaceBoundingBox(boundingBox);
    
    double centerX = boundingBox.getCenterX();
    double centerY = boundingBox.getCenterY();
    double centerZ = boundingBox.getCenterZ();
    
    float sliceNormalVector[3] = { 0.0, 0.0, 0.0 };
    
    double upX = 0.0;
    double upY = 0.0;
    double upZ = 0.0;
    
    const float sliceX = m_browserTabContent->getSliceCoordinateParasagittal();
    const float sliceY = m_browserTabContent->getSliceCoordinateCoronal();
    const float sliceZ = m_browserTabContent->getSliceCoordinateAxial();
    
    float sliceOffset[3] = {
        0.0,
        0.0,
        0.0
    };
    
    switch (sliceViewPlane) {
        case VolumeSliceViewPlaneEnum::ALL:
        case VolumeSliceViewPlaneEnum::AXIAL:
            sliceNormalVector[2] = 1.0;
            upY = 1.0;
            centerZ = sliceZ;
            sliceOffset[2] = sliceZ;
            break;
        case VolumeSliceViewPlaneEnum::CORONAL:
            sliceNormalVector[1] = -1.0;
            upZ = 1.0;
            centerY = sliceY;
            sliceOffset[1] = sliceY;
            break;
        case VolumeSliceViewPlaneEnum::PARASAGITTAL:
            sliceNormalVector[0] = -1.0;
            upZ = 1.0;
            centerX = sliceX;
            sliceOffset[0] = sliceX;
            break;
    }
    
    obliqueTransformationMatrixOut.identity();
    
    switch (m_sliceViewMode) {
        case VolumeSliceViewModeEnum::MONTAGE:
        if (montageSliceIndex >= 0) {
            /*
             * Montage overrides the selected coordinate of the viewing axis
             */
            const int64_t montIJK[3] = {
                montageSliceIndex,
                montageSliceIndex,
                montageSliceIndex
            };
            float montXYZ[3];
            m_volumeDrawInfo[0].volumeFile->indexToSpace(montIJK, montXYZ);
            
            switch (sliceViewPlane) {
                case VolumeSliceViewPlaneEnum::ALL:
                case VolumeSliceViewPlaneEnum::AXIAL:
                    sliceOffset[2] = montXYZ[2];
                    centerZ = montXYZ[2];
                    break;
                case VolumeSliceViewPlaneEnum::CORONAL:
                    sliceOffset[1] = montXYZ[1];
                    centerY = montXYZ[1];
                    break;
                case VolumeSliceViewPlaneEnum::PARASAGITTAL:
                    sliceOffset[0] = montXYZ[0];
                    centerX = montXYZ[0];
                    break;
            }
        }
            break;
        case VolumeSliceViewModeEnum::OBLIQUE:
        {
            /*
             * Get the rotation matrix
             */
            Matrix4x4 rotationMatrix = m_browserTabContent->getObliqueVolumeRotationMatrix(); //  m_browserTabContent->getRotationMatrix();
            
            /*
             * Create the transformation matrix
             */
            obliqueTransformationMatrixOut.postmultiply(rotationMatrix);
            
            /*
             * Translate for selected slice
             */
            obliqueTransformationMatrixOut.translate(sliceOffset[0],
                                           sliceOffset[1],
                                           sliceOffset[2]);
            /*
             * Transform the slice normal vector so that it
             * points orthogonal to the slice by rotating the
             * vector with the rotation matrix.
             */
            rotationMatrix.multiplyPoint3(sliceNormalVector);
            MathFunctions::normalizeVector(sliceNormalVector);
            
            /*
             * Need to rotate up vector
             */
            float upVector[3] = {
                upX,
                upY,
                upZ
            };
            rotationMatrix.multiplyPoint3(upVector);
            upX = upVector[0];
            upY = upVector[1];
            upZ = upVector[2];
        }
            break;
        case VolumeSliceViewModeEnum::ORTHOGONAL:
            break;
    }
    
    const float* translation = m_browserTabContent->getTranslation();
    const float zoom = m_browserTabContent->getScaling();
    
    centerX -= (translation[0]);
    centerY -= (translation[1]);
    centerZ -= (translation[2]);
    
    const double distanceEyeToCenter = 100.0;
    const double eyeX = centerX + (sliceNormalVector[0] * distanceEyeToCenter);
    const double eyeY = centerY + (sliceNormalVector[1] * distanceEyeToCenter);
    const double eyeZ = centerZ + (sliceNormalVector[2] * distanceEyeToCenter);
    
    Plane plane(sliceNormalVector,
                sliceOffset); //pointOnPlane);

    CaretLogFine("New View Transform center for "
                         + VolumeSliceViewPlaneEnum::toGuiName(sliceViewPlane)
                         + "\nCenter:"
                         + AString::number(centerX)
                         + ", "
                         + AString::number(centerY)
                         + ", "
                         + AString::number(centerZ)
                         + "\nEye: "
                         + AString::number(eyeX)
                         + ", "
                         + AString::number(eyeY)
                         + ", "
                         + AString::number(eyeZ)
                         + "\nUp: "
                         + AString::number(upX)
                         + ", "
                         + AString::number(upY)
                         + ", "
                         + AString::number(upZ)
                         + "\nSlice Plane: "
                         + plane.toString());
    
    m_lookAtCenter[0] = centerX;
    m_lookAtCenter[1] = centerY;
    m_lookAtCenter[2] = centerZ;
    
    if (isSetViewTranform) {
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        gluLookAt(eyeX, eyeY, eyeZ,
                  centerX, centerY, centerZ,
                  upX, upY, upZ);
    }
    if (isSetZoom) {
        glScalef(zoom, zoom, zoom);
    }
    
    planeOut = plane;
}

/**
 * Draw surface outlines on the volume slices
 *
 * @param plane
 *   Plane of the volume slice on which surface outlines are drawn.
 */
void
BrainOpenGLVolumeSliceDrawing::drawSurfaceOutline(const Plane& plane)
{
    if ( ! plane.isValidPlane()) {
        return;
    }
    
    CaretLogFine("\nSurface Outline Plane: "
                 + plane.toString());
    
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
BrainOpenGLVolumeSliceDrawing::drawVolumeSliceFoci(const Plane& plane)
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
    float minVoxelSpacing;
    float maxVoxelSpacing;
    if ( ! getMinMaxVoxelSpacing(underlayVolume, minVoxelSpacing, maxVoxelSpacing)) {
        return;
    }

    const float sliceThickness = maxVoxelSpacing;
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
                        glPushMatrix();
                        glTranslatef(xyz[0], xyz[1], xyz[2]);
                        if (isSelect) {
                            uint8_t idRGBA[4];
                            m_fixedPipelineDrawing->colorIdentification->addItem(idRGBA,
                                                               SelectionItemDataTypeEnum::FOCUS_VOLUME,
                                                               iFile, // file index
                                                               j, // focus index
                                                               k);// projection index
                            idRGBA[3] = 255;
                            if (drawAsSpheres) {
                                m_fixedPipelineDrawing->drawSphereWithDiameter(idRGBA,
                                                                               focusDiameter);
                            }
                            else {
                                glColor4ubv(idRGBA);
                                drawSquare(focusDiameter);
                            }
                        }
                        else {
                            if (drawAsSpheres) {
                                m_fixedPipelineDrawing->drawSphereWithDiameter(rgba,
                                                                               focusDiameter);
                            }
                            else {
                                glColor3fv(rgba);
                                drawSquare(focusDiameter);
                            }
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
 * Get the minimum and maximum distance between adjacent voxels in all
 * slices planes.  Output spacing value are always non-negative even if
 * a right-to-left orientation.
 *
 * @param volume
 *    Volume for which min/max spacing is requested.
 * @param minSpacingOut
 *    Output minimum spacing.
 * @param maxSpacingOut
 *    Output maximum spacing.
 * @return
 *    True if min and max spacing are greater than zero.
 */
bool
BrainOpenGLVolumeSliceDrawing::getMinMaxVoxelSpacing(const VolumeMappableInterface* volume,
                                                     float& minSpacingOut,
                                                     float& maxSpacingOut) const
{
    CaretAssert(volume);
    
    float originX, originY, originZ;
    float x1, y1, z1;
    volume->indexToSpace(0, 0, 0, originX, originY, originZ);
    volume->indexToSpace(1, 1, 1, x1, y1, z1);
    const float dx = std::fabs(x1 - originX);
    const float dy = std::fabs(y1 - originY);
    const float dz = std::fabs(z1 - originZ);
    
    minSpacingOut = std::min(std::min(dx, dy), dz);
    maxSpacingOut = std::max(std::max(dx, dy), dz);
    
    if ((minSpacingOut > 0.0)
        && (maxSpacingOut > 0.0)) {
        return true;
    }
    return false;
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
BrainOpenGLVolumeSliceDrawing::getVoxelCoordinateBoundsAndSpacing(float boundsOut[6],
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
BrainOpenGLVolumeSliceDrawing::drawSquare(const float size)
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
BrainOpenGLVolumeSliceDrawing::VoxelToDraw::VoxelToDraw(const float center[3],
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
BrainOpenGLVolumeSliceDrawing::VoxelToDraw::addVolumeValue(const int64_t sliceIndex,
                    const int64_t sliceOffset)
{
    CaretAssert(sliceIndex >= 0);
    CaretAssert(sliceOffset >= 0);
    m_sliceIndices.push_back(sliceIndex);
    m_sliceOffsets.push_back(sliceOffset);
}







/**
 * Draw a volume slice for a volume slice view.
 *
 * @param sliceViewPlane
 *   View plane (eg axial) of slice being drawn relative to the slice's normal vector.
 * @param drawMode
 *   The drawing mode
 * @param montageSliceIndex
 *   When drawing slice montage, this is the slice to draw.
 * @param viewport
 *   Viewport in which drawing takes place.
 */
void
BrainOpenGLVolumeSliceDrawing::drawSliceForSliceView(const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
                                                            const DRAW_MODE drawMode,
                                                            const int32_t montageSliceIndex,
                                                            const int viewport[4])
{
    bool isDrawingSlice2D = false;
    bool isSetViewport = false;
    switch (drawMode) {
        case DRAW_MODE_ALL_STRUCTURES_VIEW:
            break;
        case DRAW_MODE_VOLUME_VIEW_SLICE_3D:
            break;
        case DRAW_MODE_VOLUME_VIEW_SLICE_SINGLE:
            isDrawingSlice2D = true;
            isSetViewport = true;
            break;
    }
    
    if (isDrawingSlice2D) {
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
    }
    
    if (isSetViewport) {
        glViewport(viewport[0],
                   viewport[1],
                   viewport[2],
                   viewport[3]);
    }
    
    if (isDrawingSlice2D) {
        /*
         * Set the orthographic projection to fit the slice axis
         */
        setOrthographicProjection(sliceViewPlane,
                                  viewport);
    }
    
    /*
     * Set the viewing transformation to look at the slice
     */
    Plane slicePlane;
    Matrix4x4 obliqueTransformationMatrix;
    setVolumeSliceViewingTransformation(sliceViewPlane,
                                        drawMode,
                                        slicePlane,
                                        montageSliceIndex,
                                        obliqueTransformationMatrix);
    CaretAssert(slicePlane.isValidPlane());
    if (slicePlane.isValidPlane() == false) {
        return;
    }
        
    SelectionItemVoxel* voxelID = m_brain->getSelectionManager()->getVoxelIdentification();
    
    /*
     * Check for a 'selection' type mode
     */
    m_identificationModeFlag = false;
    switch (m_fixedPipelineDrawing->mode) {
        case BrainOpenGLFixedPipeline::MODE_DRAWING:
            break;
        case BrainOpenGLFixedPipeline::MODE_IDENTIFICATION:
            if (voxelID->isEnabledForSelection()) {
                m_identificationModeFlag = true;
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

    resetIdentification(10000);
    
    /*
     * Disable culling so that both sides of the triangles/quads are drawn.
     */
    GLboolean cullFaceOn = glIsEnabled(GL_CULL_FACE);
    glDisable(GL_CULL_FACE);
    
    switch (m_sliceViewMode) {
        case VolumeSliceViewModeEnum::OBLIQUE:
            drawObliqueSlice(sliceViewPlane,
                             slicePlane,
                             drawMode,
                             obliqueTransformationMatrix,
                             1.0);
            break;
        case VolumeSliceViewModeEnum::MONTAGE:
        case VolumeSliceViewModeEnum::ORTHOGONAL:
            drawOrthogonalSlice(sliceViewPlane,
                                slicePlane,
                                montageSliceIndex);
            break;
    }
    
    
    
    
        if ( ! m_identificationModeFlag) {
            if (slicePlane.isValidPlane()) {
                drawLayers(slicePlane,
                           obliqueTransformationMatrix,
                           m_volumeDrawInfo[0].volumeFile,
                           sliceViewPlane,
                           drawMode);
            }
        }
        
    /*
     * Process selection
     */
    if (m_identificationModeFlag) {
        processIdentification();
    }    
    
    if (cullFaceOn) {
        glEnable(GL_CULL_FACE);
    }
}
//montageSliceIndex
/**
 * Draw the layers type data.
 *
 * @param slicePlane
 *    Plane of the slice.
 * @param transformationMatrix
 *    The transformation matrix used for oblique viewing.
 * @param volume
 *    The underlay volume.
 * @param sliceViewPlane
 *    The slice view plane.
 * @param drawMode
 *    The slice drawing mode.
 */
void
BrainOpenGLVolumeSliceDrawing::drawLayers(const Plane& slicePlane,
                                              const Matrix4x4& transformationMatrix,
                                              const VolumeMappableInterface* volume,
                                              const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
                                              const DRAW_MODE drawMode)
{
    bool drawCrosshairsFlag = true;
    bool drawFibersFlag     = true;
    bool drawFociFlag       = true;
    bool drawOutlineFlag    = true;
    
    switch (drawMode) {
        case DRAW_MODE_ALL_STRUCTURES_VIEW:
            drawCrosshairsFlag = false;
            drawFibersFlag = false;
            drawFociFlag = false;
            break;
        case DRAW_MODE_VOLUME_VIEW_SLICE_3D:
            break;
        case DRAW_MODE_VOLUME_VIEW_SLICE_SINGLE:
            break;
    }
    
    if ( ! m_identificationModeFlag) {
        if (slicePlane.isValidPlane()) {
            /*
             * Disable culling so that both sides of the triangles/quads are drawn.
             */
            GLboolean cullFaceOn = glIsEnabled(GL_CULL_FACE);
            glDisable(GL_CULL_FACE);
            
            glPushMatrix();
            
            if (drawCrosshairsFlag) {
                drawAxesCrosshairs(transformationMatrix,
                                   volume,
                                   sliceViewPlane,
                                   drawMode);
            }
            
            /*
             * Use some polygon offset that will adjust the depth values of the
             * foci so that the foci depth values place the foci in front of
             * the volume slice.
             */
            glEnable(GL_POLYGON_OFFSET_FILL);
            glPolygonOffset(0.0, 1.0);
            
            if (drawFibersFlag) {
                m_fixedPipelineDrawing->drawFiberOrientations(&slicePlane);
                m_fixedPipelineDrawing->drawFiberTrajectories(&slicePlane);
            }
            if (drawFociFlag) {
                drawVolumeSliceFoci(slicePlane);
            }
            if (drawOutlineFlag) {
                drawSurfaceOutline(slicePlane);
            }
            
            glEnable(GL_POLYGON_OFFSET_FILL);
            
            glPopMatrix();
            
            if (cullFaceOn) {
                glEnable(GL_CULL_FACE);
            }
        }
    }
    
}


/**
 * Draw an orthogonal slice.
 *
 * @param sliceViewPlane
 *    The slice plane being viewed.
 * @param plane
 *    The plane equation for the slice.
 * @param montageSliceIndex
 *    The montage slice for drawing in montage mode.
 */
void
BrainOpenGLVolumeSliceDrawing::drawOrthogonalSlice(const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
                                                       const Plane& plane,
                                                       const int32_t montageSliceIndex)
{
    
    const int32_t browserTabIndex = m_browserTabContent->getTabNumber();
    const DisplayPropertiesLabels* displayPropertiesLabels = m_brain->getDisplayPropertiesLabels();
    const DisplayGroupEnum::Enum displayGroup = displayPropertiesLabels->getDisplayGroupForTab(browserTabIndex);
    const LabelDrawingTypeEnum::Enum labelDrawingType = displayPropertiesLabels->getDrawingType(displayGroup,
                                                                                                browserTabIndex);
    bool isOutlineMode = false;
    switch (labelDrawingType) {
        case LabelDrawingTypeEnum::DRAW_FILLED:
            break;
        case LabelDrawingTypeEnum::DRAW_OUTLINE:
            isOutlineMode = true;
            break;
    }
    
    /*
     * Enable alpha blending so voxels that are not drawn from higher layers
     * allow voxels from lower layers to be seen.
     */
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    /*
     * Flat shading voxels not interpolated
     */
    glShadeModel(GL_FLAT);
    
    
    /*
     * Compute coordinate of point in center of first slice
     */
    float selectedSliceCoordinate = 0.0;
    float sliceNormalVector[3] = { 0.0, 0.0, 0.0 };
    plane.getNormalVector(sliceNormalVector);
    switch (sliceViewPlane) {
        case VolumeSliceViewPlaneEnum::ALL:
            CaretAssert(0);
            break;
        case VolumeSliceViewPlaneEnum::AXIAL:
            selectedSliceCoordinate = m_browserTabContent->getSliceCoordinateAxial();
            break;
        case VolumeSliceViewPlaneEnum::CORONAL:
            selectedSliceCoordinate = m_browserTabContent->getSliceCoordinateCoronal();
            break;
        case VolumeSliceViewPlaneEnum::PARASAGITTAL:
            selectedSliceCoordinate = m_browserTabContent->getSliceCoordinateParasagittal();
            break;
    }
    
    /*
     * Override for montage slice drawing.
     */
    switch (m_sliceViewMode) {
        case VolumeSliceViewModeEnum::MONTAGE:
        {
            const int64_t montIJK[3] = {
                montageSliceIndex,
                montageSliceIndex,
                montageSliceIndex
            };
            float montXYZ[3];
            m_volumeDrawInfo[0].volumeFile->indexToSpace(montIJK,
                                                         montXYZ);
            switch (sliceViewPlane) {
                case VolumeSliceViewPlaneEnum::ALL:
                    CaretAssert(0);
                    break;
                case VolumeSliceViewPlaneEnum::AXIAL:
                    selectedSliceCoordinate = montXYZ[2];
                    break;
                case VolumeSliceViewPlaneEnum::CORONAL:
                    selectedSliceCoordinate = montXYZ[1];
                    break;
                case VolumeSliceViewPlaneEnum::PARASAGITTAL:
                    selectedSliceCoordinate = montXYZ[0];
                    break;
            }
        }
            break;
        case VolumeSliceViewModeEnum::OBLIQUE:
            break;
        case VolumeSliceViewModeEnum::ORTHOGONAL:
            break;
    }
    
    /*
     * Holds colors for voxels in the slice
     * Outside of loop to minimize allocations
     * It is faster to make one call to
     * NodeAndVoxelColoring::colorScalarsWithPalette() with
     * all voxels in the slice than it is to call it
     * separately for each voxel.
     */
    std::vector<uint8_t> sliceVoxelsRgbaVector;
    
    /*
     * Draw each of the volumes separately so that each
     * is drawn with the correct voxel slices.
     */
    const int32_t numberOfVolumesToDraw = static_cast<int32_t>(m_volumeDrawInfo.size());
    for (int32_t iVol = 0; iVol < numberOfVolumesToDraw; iVol++) {
        const BrainOpenGLFixedPipeline::VolumeDrawInfo& volInfo = m_volumeDrawInfo[iVol];
        const VolumeMappableInterface* volumeFile = volInfo.volumeFile;
        int64_t dimI, dimJ, dimK, numMaps, numComponents;
        volumeFile->getDimensions(dimI, dimJ, dimK, numMaps, numComponents);
        const int64_t mapIndex = volInfo.mapIndex;
        
        float originX, originY, originZ;
        volumeFile->indexToSpace(0, 0, 0, originX, originY, originZ);
        
        float x1, y1, z1;
        volumeFile->indexToSpace(1, 1, 1, x1, y1, z1);
        const float voxelStepX = x1 - originX;
        const float voxelStepY = y1 - originY;
        const float voxelStepZ = z1 - originZ;
        
        /*
         * Determine index of slice being viewed for the volume
         */
        float coordinateOnSlice[3] = {
            originX,
            originY,
            originZ
        };
        switch (sliceViewPlane) {
            case VolumeSliceViewPlaneEnum::ALL:
                CaretAssert(0);
                break;
            case VolumeSliceViewPlaneEnum::AXIAL:
                coordinateOnSlice[2] = selectedSliceCoordinate;
                break;
            case VolumeSliceViewPlaneEnum::CORONAL:
                coordinateOnSlice[1] = selectedSliceCoordinate;
                break;
            case VolumeSliceViewPlaneEnum::PARASAGITTAL:
                coordinateOnSlice[0] = selectedSliceCoordinate;
                break;
        }
        
        int64_t sliceIndicesForCoordinateOnSlice[3];
        volumeFile->enclosingVoxel(coordinateOnSlice[0],
                                   coordinateOnSlice[1],
                                   coordinateOnSlice[2],
                                   sliceIndicesForCoordinateOnSlice[0],
                                   sliceIndicesForCoordinateOnSlice[1],
                                   sliceIndicesForCoordinateOnSlice[2]);
        
        int64_t sliceIndexForDrawing = -1;
        int64_t numVoxelsInSlice = 0;
        switch (sliceViewPlane) {
            case VolumeSliceViewPlaneEnum::ALL:
                CaretAssert(0);
                break;
            case VolumeSliceViewPlaneEnum::AXIAL:
                sliceIndexForDrawing = sliceIndicesForCoordinateOnSlice[2];
                if ((sliceIndexForDrawing < 0)
                    || (sliceIndexForDrawing >= dimK)) {
                    continue;
                }
                numVoxelsInSlice = dimI * dimJ;
                break;
            case VolumeSliceViewPlaneEnum::CORONAL:
                sliceIndexForDrawing = sliceIndicesForCoordinateOnSlice[1];
                if ((sliceIndexForDrawing < 0)
                    || (sliceIndexForDrawing >= dimJ)) {
                    continue;
                }
                numVoxelsInSlice = dimI * dimK;
                break;
            case VolumeSliceViewPlaneEnum::PARASAGITTAL:
                sliceIndexForDrawing = sliceIndicesForCoordinateOnSlice[0];
                if ((sliceIndexForDrawing < 0)
                    || (sliceIndexForDrawing >= dimI)) {
                    continue;
                }
                numVoxelsInSlice = dimJ * dimK;
                break;
        }

        /*
         * Stores RGBA values for each voxel.
         * Use a vector for voxel colors so no worries about memory being freed.
         */
        const int64_t numVoxelsInSliceRGBA = numVoxelsInSlice * 4;
        if (numVoxelsInSliceRGBA > static_cast<int64_t>(sliceVoxelsRgbaVector.size())) {
            sliceVoxelsRgbaVector.resize(numVoxelsInSliceRGBA);
        }
        uint8_t* sliceVoxelsRGBA = &sliceVoxelsRgbaVector[0];
        
        /*
         * Get colors for all voxels in the slice.
         */
        volumeFile->getVoxelColorsForSliceInMap(m_brain->getPaletteFile(),
                                                mapIndex,
                                                sliceViewPlane,
                                                sliceIndexForDrawing,
                                                displayGroup,
                                                browserTabIndex,
                                                sliceVoxelsRGBA);
        
        /*
         * Is label outline mode?
         */
        if (m_volumeDrawInfo[iVol].mapFile->isMappedWithLabelTable()) {
            if (isOutlineMode) {
                int64_t xdim = 0;
                int64_t ydim = 0;
                switch (sliceViewPlane) {
                    case VolumeSliceViewPlaneEnum::ALL:
                        CaretAssert(0);
                        break;
                    case VolumeSliceViewPlaneEnum::AXIAL:
                        xdim = dimI;
                        ydim = dimJ;
                        break;
                    case VolumeSliceViewPlaneEnum::CORONAL:
                        xdim = dimI;
                        ydim = dimK;
                        break;
                    case VolumeSliceViewPlaneEnum::PARASAGITTAL:
                        xdim = dimJ;
                        ydim = dimK;
                        break;
                }
                
                NodeAndVoxelColoring::convertSliceColoringToOutlineMode(sliceVoxelsRGBA,
                                                                        xdim,
                                                                        ydim);
            }
        }
        
        int64_t selectedSliceIndices[3] = {
            m_browserTabContent->getSliceIndexParasagittal(volumeFile),
            m_browserTabContent->getSliceIndexCoronal(volumeFile),
            m_browserTabContent->getSliceIndexAxial(volumeFile)
        };
        /*
         * Override selected slice indices for montage.
         */
        switch (m_sliceViewMode) {
            case VolumeSliceViewModeEnum::MONTAGE:
            {
                switch (sliceViewPlane) {
                    case VolumeSliceViewPlaneEnum::ALL:
                        CaretAssert(0);
                        break;
                    case VolumeSliceViewPlaneEnum::AXIAL:
                        selectedSliceIndices[2] = sliceIndexForDrawing; //montageSliceIndex;
                        break;
                    case VolumeSliceViewPlaneEnum::CORONAL:
                        selectedSliceIndices[1] = sliceIndexForDrawing;
                        break;
                    case VolumeSliceViewPlaneEnum::PARASAGITTAL:
                        selectedSliceIndices[0] = sliceIndexForDrawing;
                        break;
                }
            }
                break;
            case VolumeSliceViewModeEnum::OBLIQUE:
                break;
            case VolumeSliceViewModeEnum::ORTHOGONAL:
                break;
        }
        
        const uint8_t volumeDrawingOpacity = static_cast<int8_t>(volInfo.opacity * 255.0);

        /*
         * Setup for drawing the voxels in the slice.
         */
        float startCoordinate[3] = {
            originX - (voxelStepX / 2.0),
            originY - (voxelStepY / 2.0),
            originZ - (voxelStepZ / 2.0)
        };
        
        float rowStep[3] = {
            0.0,
            0.0,
            0.0
        };
        
        float columnStep[3] = {
            0.0,
            0.0,
            0.0
        };
        
  
        int64_t numberOfRows = 0;
        int64_t numberOfColumns = 0;
        switch (sliceViewPlane) {
            case VolumeSliceViewPlaneEnum::ALL:
                CaretAssert(0);
                break;
            case VolumeSliceViewPlaneEnum::AXIAL:
                startCoordinate[2] = m_browserTabContent->getSliceCoordinateAxial();
                rowStep[1] = voxelStepY;
                columnStep[0] = voxelStepX;
                numberOfRows    = dimJ;
                numberOfColumns = dimI;
                break;
            case VolumeSliceViewPlaneEnum::CORONAL:
                startCoordinate[1] = m_browserTabContent->getSliceCoordinateCoronal();
                rowStep[2] = voxelStepZ;
                columnStep[0] = voxelStepX;
                numberOfRows    = dimK;
                numberOfColumns = dimI;
                break;
            case VolumeSliceViewPlaneEnum::PARASAGITTAL:
                startCoordinate[0] = m_browserTabContent->getSliceCoordinateParasagittal();
                rowStep[2] = voxelStepZ;
                columnStep[1] = voxelStepY;
                numberOfRows    = dimK;
                numberOfColumns = dimJ;
                break;
        }
        
        /*
         * Draw the voxels in the slice.
         */
        drawOrthogonalSliceVoxels(sliceViewPlane,
                                  sliceNormalVector,
                                  selectedSliceIndices,
                                  startCoordinate,
                                  rowStep,
                                  columnStep,
                                  numberOfColumns,
                                  numberOfRows,
                                  sliceVoxelsRgbaVector,
                                  iVol,
                                  mapIndex,
                                  volumeDrawingOpacity);
    }
    glDisable(GL_BLEND);
    glShadeModel(GL_SMOOTH);
}

/**
 * Draw the voxels in an orthogonal slice.
 *
 * @param sliceViewPlane
 *    The slice plane being viewed.
 * @param sliceNormalVector
 *    Normal vector of the slice plane.
 * @param selectedSliceIndices
 *    Selected slice indices.
 * @param coordinate
 *    Coordinate of first voxel in the slice (bottom left as begin viewed)
 * @param rowStep
 *    Three-dimensional step to next row.
 * @param columnStep
 *    Three-dimensional step to next column.
 * @param numberOfColumns
 *    Number of columns in the slice.
 * @param numberOfRows
 *    Number of rows in the slice.
 * @param sliceRGBA
 *    RGBA coloring for voxels in the slice.
 * @param volumeIndex
 *    Index of the volume being drawn.
 * @param mapIndex
 *    Selected map in the volume being drawn.
 * @param sliceOpacity
 *    Opacity from the overlay.
 */
void
BrainOpenGLVolumeSliceDrawing::drawOrthogonalSliceVoxels(const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
                                                             const float sliceNormalVector[3],
                                                             const int64_t selectedSliceIndices[3],
                                                             const float coordinate[3],
                                                             const float rowStep[3],
                                                             const float columnStep[3],
                                                             const int64_t numberOfColumns,
                                                             const int64_t numberOfRows,
                                                             const std::vector<uint8_t>& sliceRGBA,
                                                             const int32_t volumeIndex,
                                                             const int32_t mapIndex,
                                                             const uint8_t sliceOpacity)
{
    const int64_t numVoxelsInSlice = numberOfColumns * numberOfRows;
        
//    if (volumeIndex == 0) {
//        const float c2[3] = {
//            coordinate[0] + columnStep[0] * numberOfColumns,
//            coordinate[1] + columnStep[1] * numberOfColumns,
//            coordinate[2] + columnStep[2] * numberOfColumns
//        };
//        
//        const float c3[3] = {
//            coordinate[0] + columnStep[0] * numberOfColumns + rowStep[0] * numberOfRows,
//            coordinate[1] + columnStep[1] * numberOfColumns + rowStep[1] * numberOfRows,
//            coordinate[2] + columnStep[2] * numberOfColumns + rowStep[2] * numberOfRows
//        };
//        
//        const float c4[3] = {
//            coordinate[0] + rowStep[0] * numberOfRows,
//            coordinate[1] + rowStep[1] * numberOfRows,
//            coordinate[2] + rowStep[2] * numberOfRows
//        };
//        
//        m_fixedPipelineDrawing->setLineWidth(3.0);
//        glColor3f(0.0, 0.0, 1.0);
//        glBegin(GL_LINE_LOOP);
//        glVertex3fv(coordinate);
//        glVertex3fv(c2);
//        glVertex3fv(c3);
//        glVertex3fv(c4);
//        glEnd();
//    }
    
    /*
     * Allocate for quadrilateral drawing
     */
    const int64_t numQuadCoords = numVoxelsInSlice * 12;
    const int64_t numQuadRgba   = numVoxelsInSlice * 16;
    std::vector<float> voxelQuadCoordinates;
    std::vector<float> voxelQuadNormals;
    std::vector<uint8_t> voxelQuadRgba;
    voxelQuadCoordinates.reserve(numQuadCoords);
    voxelQuadNormals.reserve(numQuadCoords);
    voxelQuadRgba.reserve(numQuadRgba);
    
    /*
     * Step to next row or column voxel
     */
    const float rowStepX = rowStep[0];
    const float rowStepY = rowStep[1];
    const float rowStepZ = rowStep[2];
    const float columnStepX = columnStep[0];
    const float columnStepY = columnStep[1];
    const float columnStepZ = columnStep[2];
    
    /*
     * Draw each row
     */
    for (int64_t jRow = 0; jRow < numberOfRows; jRow++) {
        /*
         * Coordinates on left side of row
         */
        float rowBottomLeft[3] = {
            coordinate[0] + (jRow * rowStepX),
            coordinate[1] + (jRow * rowStepY),
            coordinate[2] + (jRow * rowStepZ)
        };
        float rowTopLeft[3] = {
            rowBottomLeft[0] + rowStepX,
            rowBottomLeft[1] + rowStepY,
            rowBottomLeft[2] + rowStepZ
        };
        
        /*
         * Draw each voxel in its column
         */
        for (int64_t iCol = 0; iCol < numberOfColumns; iCol++) {
            
            /*
             * Offset of voxel in coloring.
             */
            const int64_t sliceRgbaOffset = (4 * (iCol
                                                  + (numberOfColumns * jRow)));
            const int64_t alphaOffset = sliceRgbaOffset + 3;
            
            uint8_t rgba[4] = {
                0,
                0,
                0,
                0
            };
            
            /*
             * Negative alpha means do not display
             */
            CaretAssertVectorIndex(sliceRGBA, alphaOffset);
            if (sliceRGBA[alphaOffset] <= 0) {
                if (volumeIndex == 0) {
                    /*
                     * For first drawn volume, use black for voxel that would not be displayed.
                     */
                    rgba[3] = 255;
                }
            }
            else {
                /*
                 * Use overlay's opacity
                 */
                rgba[0] = sliceRGBA[sliceRgbaOffset];
                rgba[1] = sliceRGBA[sliceRgbaOffset + 1];
                rgba[2] = sliceRGBA[sliceRgbaOffset + 2];
                rgba[3] = sliceOpacity;
            }
            
            /*
             * Draw voxel based upon opacity
             */
            if (rgba[3] > 0) {
                if (m_identificationModeFlag) {
                    /*
                     * Add info about voxel for identication.
                     */
                    int64_t voxelI = 0;
                    int64_t voxelJ = 0;
                    int64_t voxelK = 0;
                    switch (sliceViewPlane) {
                        case VolumeSliceViewPlaneEnum::ALL:
                            CaretAssert(0);
                            break;
                        case VolumeSliceViewPlaneEnum::AXIAL:
                            voxelI = iCol;
                            voxelJ = jRow;
                            voxelK = selectedSliceIndices[2];
                            break;
                        case VolumeSliceViewPlaneEnum::CORONAL:
                            voxelI = iCol;
                            voxelJ = selectedSliceIndices[1];
                            voxelK = jRow;
                            break;
                        case VolumeSliceViewPlaneEnum::PARASAGITTAL:
                            voxelI = selectedSliceIndices[0];
                            voxelJ = iCol;
                            voxelK = jRow;
                            break;
                    }
                    addVoxelToIdentification(volumeIndex, mapIndex, voxelI, voxelJ, voxelK, rgba);
                }
                
                /*
                 * Set coordinates of voxel corners
                 */
                float voxelBottomLeft[3] = {
                    rowBottomLeft[0] + (iCol * columnStepX),
                    rowBottomLeft[1] + (iCol * columnStepY),
                    rowBottomLeft[2] + (iCol * columnStepZ)
                };
                float voxelBottomRight[3] = {
                    voxelBottomLeft[0] + columnStepX,
                    voxelBottomLeft[1] + columnStepY,
                    voxelBottomLeft[2] + columnStepZ
                };
                float voxelTopLeft[3] = {
                    rowTopLeft[0] + (iCol * columnStepX),
                    rowTopLeft[1] + (iCol * columnStepY),
                    rowTopLeft[2] + (iCol * columnStepZ)
                };
                float voxelTopRight[3] = {
                    voxelTopLeft[0] + columnStepX,
                    voxelTopLeft[1] + columnStepY,
                    voxelTopLeft[2] + columnStepZ
                };
                
                /*
                 * Add voxel to quadrilaterals
                 */
                voxelQuadCoordinates.push_back(voxelBottomLeft[0]);
                voxelQuadCoordinates.push_back(voxelBottomLeft[1]);
                voxelQuadCoordinates.push_back(voxelBottomLeft[2]);

                voxelQuadCoordinates.push_back(voxelBottomRight[0]);
                voxelQuadCoordinates.push_back(voxelBottomRight[1]);
                voxelQuadCoordinates.push_back(voxelBottomRight[2]);
                
                voxelQuadCoordinates.push_back(voxelTopRight[0]);
                voxelQuadCoordinates.push_back(voxelTopRight[1]);
                voxelQuadCoordinates.push_back(voxelTopRight[2]);
                
                voxelQuadCoordinates.push_back(voxelTopLeft[0]);
                voxelQuadCoordinates.push_back(voxelTopLeft[1]);
                voxelQuadCoordinates.push_back(voxelTopLeft[2]);
                
                
                for (int32_t iNormalAndColor = 0; iNormalAndColor < 4; iNormalAndColor++) {
                    voxelQuadRgba.push_back(rgba[0]);
                    voxelQuadRgba.push_back(rgba[1]);
                    voxelQuadRgba.push_back(rgba[2]);
                    voxelQuadRgba.push_back(rgba[3]);
                    
                    voxelQuadNormals.push_back(sliceNormalVector[0]);
                    voxelQuadNormals.push_back(sliceNormalVector[1]);
                    voxelQuadNormals.push_back(sliceNormalVector[2]);
                }
            }
        }
    }
    
    /*
     * Draw the voxels.
     */
    if (voxelQuadCoordinates.empty() == false) {
        BrainOpenGLPrimitiveDrawing::drawQuads(voxelQuadCoordinates,
                                               voxelQuadNormals,
                                               voxelQuadRgba);
    }
    //drawDebugSquare();
}


/**
 * Draw a volume slice's voxels.
 *
 * @param sliceViewPlane
 *   View plane (eg axial) of slice being drawn relative to the slice's normal vector.
 * @param plane
 *   The plane equation.
 * @param drawMode
 *   The drawing mode.
 * @param transformationMatrix
 *   Transformation matrix for screen to model.
 * @param zoom
 *   The slice zooming.
 */
void
BrainOpenGLVolumeSliceDrawing::drawObliqueSlice(const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
                                                    const Plane& plane,
                                                    const DRAW_MODE drawMode,
                                                    const Matrix4x4& transformationMatrix,
                                                    const float zoom)
{
    float m[16];
    glGetFloatv(GL_MODELVIEW_MATRIX, m);
    Matrix4x4 tm;
    tm.setMatrixFromOpenGL(m);
    CaretLogFine("Oblique drawing matrix for slice: "
                            + VolumeSliceViewPlaneEnum::toGuiName(sliceViewPlane)
                            + tm.toFormattedString("  "));
    
    const int32_t numVolumes = static_cast<int32_t>(m_volumeDrawInfo.size());
    
    /*
     * Get the maximum bounds of the voxels from all slices
     * and the smallest voxel spacing
     */
    float voxelBounds[6];
    float voxelSpacing[3];
    if (false == getVoxelCoordinateBoundsAndSpacing(voxelBounds,
                                                    voxelSpacing)) {
        return;
    }
    float voxelSize = std::min(voxelSpacing[0],
                                     std::min(voxelSpacing[1],
                                              voxelSpacing[2])); // / 5.0;
    
    /*
     * Use a larger voxel size for the 3D view in volume slice viewing
     * since it draws all three slices and this takes time
     */
    switch (drawMode) {
        case DRAW_MODE_VOLUME_VIEW_SLICE_SINGLE:
            break;
        case DRAW_MODE_VOLUME_VIEW_SLICE_3D:
            voxelSize *= 3.0;
            break;
        case DRAW_MODE_ALL_STRUCTURES_VIEW:
            break;
    }

    
    /*
     * Look at point is in center of volume
     */
    float screenOffsetX = 0.0;
    float screenOffsetY = 0.0;
    switch (sliceViewPlane) {
        case VolumeSliceViewPlaneEnum::ALL:
            CaretAssert(0);
            break;
        case VolumeSliceViewPlaneEnum::AXIAL:
            screenOffsetX = m_lookAtCenter[0];
            screenOffsetY = m_lookAtCenter[1];
            break;
        case VolumeSliceViewPlaneEnum::CORONAL:
            screenOffsetX = m_lookAtCenter[0];
            screenOffsetY = m_lookAtCenter[2];
            break;
        case VolumeSliceViewPlaneEnum::PARASAGITTAL:
            screenOffsetX = m_lookAtCenter[1];
            screenOffsetY = m_lookAtCenter[2];
            break;
    }
    
    float minScreenX = m_orthographicBounds[0] + screenOffsetX;
    float maxScreenX = m_orthographicBounds[1] + screenOffsetX;
    float minScreenY = m_orthographicBounds[2] + screenOffsetY;
    float maxScreenY = m_orthographicBounds[3] + screenOffsetY;
    
    const float screenScale = 2.0;
    minScreenX *= screenScale;
    maxScreenX *= screenScale;
    minScreenY *= screenScale;
    maxScreenY *= screenScale;
    
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
    
//    /*
//     * For debugging, draw box around oblique drawing region
//     */
//    {
//        std::cout
//        << qPrintable("Oblique BL: " + AString::fromNumbers(bottomLeft, 3, ",")
//                      + " BR: " + AString::fromNumbers(bottomRight, 3, ",")
//                      + " TR: " + AString::fromNumbers(topRight, 3, ",")
//                      + " TL: " + AString::fromNumbers(topLeft, 3, ","))
//        << std::endl;
//        m_fixedPipelineDrawing->setLineWidth(3.0);
//        glColor3f(1.0, 0.0, 0.0);
//        glBegin(GL_LINE_LOOP);
//        glVertex3fv(bottomLeft);
//        glVertex3fv(bottomRight);
//        glVertex3fv(topRight);
//        glVertex3fv(topLeft);
//        glEnd();
//        
//        GLdouble modelMatrix[16];
//        GLdouble projMatrix[16];
//        double objCoord[3];
//        GLint viewport[4];
//        
//        glGetIntegerv(GL_VIEWPORT, viewport);
//        glGetDoublev(GL_MODELVIEW_MATRIX, modelMatrix);
//        glGetDoublev(GL_PROJECTION_MATRIX, projMatrix);
//        
//        if (gluUnProject(0, 0, 0,
//                     modelMatrix, projMatrix, viewport,
//                         &objCoord[0], &objCoord[1], &objCoord[2]) == GL_TRUE) {
//            std::cout << "Oblique Bottom Corner: " << qPrintable(AString::fromNumbers(objCoord, 3, ",")) << std::endl;
//        }
//    }
    
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
                
                if (m_identificationModeFlag) {
                    VolumeMappableInterface* volMap = volumeSlices[sliceIndex].m_volumeMappableInterface;
                    int64_t voxelI, voxelJ, voxelK;
                    volMap->enclosingVoxel(vtd->m_center[0], vtd->m_center[1], vtd->m_center[2],
                                           voxelI, voxelJ, voxelK);
                    
                    if (volMap->indexValid(voxelI, voxelJ, voxelK)) {
                        addVoxelToIdentification(sliceIndex, volumeSlices[sliceIndex].m_mapIndex, voxelI, voxelJ, voxelK, voxelRGBA);
                    }
                }
            }
        }
        
        if (voxelRGBA[3] > 0) {
            float sliceNormalVector[3];
            plane.getNormalVector(sliceNormalVector);
            
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
        BrainOpenGLPrimitiveDrawing::drawQuads(quadCoordsVector,
                                               quadNormalsVector,
                                               quadRGBAsVector);
        glPopMatrix();
    }
    
    //drawDebugSquare();
} // plane

/**
 * Draw a square for debugging.
 */
void
BrainOpenGLVolumeSliceDrawing::drawDebugSquare()
{
    glColor3f(1.0, 0.0, 1.0);
    glBegin(GL_POLYGON);
    glVertex3f(-25.0, -25.0, 1.0);
    glVertex3f( 25.0, -25.0, 1.0);
    glVertex3f( 25.0,  25.0, 1.0);
    glVertex3f(-25.0,  25.0, 1.0);
    glEnd();
}

/**
 * Draw the axes crosshairs.
 *
 * @param transformationMatrix
 *    The transformation matrix used for oblique viewing.
 * @param volume
 *    The underlay volume.
 * @param sliceViewPlane
 *    The slice view plane.
 * @param drawMode
 *    The slice drawing mode.
 */
void
BrainOpenGLVolumeSliceDrawing::drawAxesCrosshairs(const Matrix4x4& transformationMatrix,
                                                      const VolumeMappableInterface* volume,
                                                      const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
                                                      const DRAW_MODE drawMode)
{
    bool isThreeSliceView = false;
    bool applyTransformationMatrixToAxes = false;
    switch (drawMode) {
        case DRAW_MODE_ALL_STRUCTURES_VIEW:
            return;
            break;
        case DRAW_MODE_VOLUME_VIEW_SLICE_3D:
            if (sliceViewPlane != VolumeSliceViewPlaneEnum::AXIAL) {
                /*
                 * This gets called three times (once for each slice)
                 * in this view but we only need to draw the axes once!
                 */
                return;
            }
            isThreeSliceView = true;
            break;
        case DRAW_MODE_VOLUME_VIEW_SLICE_SINGLE:
            break;
    }
    
    bool isMontageView = false;
    switch (m_sliceViewMode) {
        case VolumeSliceViewModeEnum::OBLIQUE:
            break;
        case VolumeSliceViewModeEnum::MONTAGE:
            isMontageView = true;
            break;
        case VolumeSliceViewModeEnum::ORTHOGONAL:
            break;
    }
    
    CaretPreferences* prefs = SessionManager::get()->getCaretPreferences();
    
    CaretAssert(volume);
    
    float bounds[6];
    float spacing[3];
    getVoxelCoordinateBoundsAndSpacing(bounds, spacing);
    const float minVoxelSize = std::min(std::min(spacing[0],
                                                 spacing[1]),
                                        spacing[2]);
    
    const float axesCrosshairRadius = 1.0 * minVoxelSize;
    
    /*
     * Ends of crosshairs
     */
    BoundingBox boundingBox;
    volume->getVoxelSpaceBoundingBox(boundingBox);
    const float scaler = 1.02;
    const float xMin = boundingBox.getMinX() * scaler;
    const float xMax = boundingBox.getMaxX() * scaler;
    const float yMin = boundingBox.getMinY() * scaler;
    const float yMax = boundingBox.getMaxY() * scaler;
    const float zMin = boundingBox.getMinZ() * scaler;
    const float zMax = boundingBox.getMaxZ() * scaler;
    
    /*
     * Determine which axes labels are drawn
     */
    bool isDrawAxialLabels = false;
    bool isDrawCoronalLabels = false;
    bool isDrawParasagittalLabels = false;
    if (prefs->isVolumeAxesLabelsDisplayed()) {
        isDrawAxialLabels = true;
        isDrawCoronalLabels = true;
        isDrawParasagittalLabels = true;
        
        if (isThreeSliceView) {
            applyTransformationMatrixToAxes = true;
        }
        else if (isMontageView) {
            isDrawAxialLabels = false;
            isDrawCoronalLabels = false;
            isDrawParasagittalLabels = false;
        }
        else {
            switch (sliceViewPlane) {
                case VolumeSliceViewPlaneEnum::ALL:
                    break;
                case VolumeSliceViewPlaneEnum::AXIAL:
                    isDrawAxialLabels = false;
                    break;
                case VolumeSliceViewPlaneEnum::CORONAL:
                    isDrawCoronalLabels = false;
                    break;
                case VolumeSliceViewPlaneEnum::PARASAGITTAL:
                    isDrawParasagittalLabels = false;
                    break;
            }
        }
    }
    
    const float centerXYZ[3] = {
        m_browserTabContent->getSliceCoordinateParasagittal(),
        m_browserTabContent->getSliceCoordinateCoronal(),
        m_browserTabContent->getSliceCoordinateAxial()
    };
    
    /*
     * Axial axis and labels
     */
    const float axialStartXYZ[3] = {
        centerXYZ[0],
        centerXYZ[1],
        centerXYZ[2] + zMin
    };
    
    const float axialEndXYZ[3] = {
        centerXYZ[0],
        centerXYZ[1],
        centerXYZ[2] + zMax
    };
    float rgba[4];
    getAxesColor(VolumeSliceViewPlaneEnum::AXIAL,
                 rgba);
    if (prefs->isVolumeAxesCrosshairsDisplayed()) {
        float startXYZ[3] = {
            axialStartXYZ[0],
            axialStartXYZ[1],
            axialStartXYZ[2]
        };
        float endXYZ[3] = {
            axialEndXYZ[0],
            axialEndXYZ[1],
            axialEndXYZ[2]
        };
        
        if (applyTransformationMatrixToAxes) {
            transformationMatrix.multiplyPoint3(startXYZ);
            transformationMatrix.multiplyPoint3(endXYZ);
        }
        
        m_fixedPipelineDrawing->drawCylinder(rgba,
                                             startXYZ,
                                             endXYZ,
                                             axesCrosshairRadius);
    }
    if (isDrawAxialLabels) {
        float startTextXYZ[3];
        float endTextXYZ[3];
        getAxesTextLabelsXYZ(axialStartXYZ,
                             axialEndXYZ,
                             startTextXYZ,
                             endTextXYZ);
        glColor4fv(rgba);
        
        if (applyTransformationMatrixToAxes) {
            transformationMatrix.multiplyPoint3(startTextXYZ);
            transformationMatrix.multiplyPoint3(endTextXYZ);
        }
        m_fixedPipelineDrawing->drawTextModelCoords(endTextXYZ[0],
                                                    endTextXYZ[1],
                                                    endTextXYZ[2],
                                                    "D");
        m_fixedPipelineDrawing->drawTextModelCoords(startTextXYZ[0],
                                                    startTextXYZ[1],
                                                    startTextXYZ[2],
                                                    "V");
    }
    
    /*
     * Coronal axis and labels
     */
    const float coronalStartXYZ[3] = {
        centerXYZ[0],
        centerXYZ[1] + yMin,
        centerXYZ[2]
    };
    
    const float coronalEndXYZ[3] = {
        centerXYZ[0],
        centerXYZ[1] + yMax,
        centerXYZ[2]
    };
    getAxesColor(VolumeSliceViewPlaneEnum::CORONAL,
                 rgba);
    if (prefs->isVolumeAxesCrosshairsDisplayed()) {
        float startXYZ[3] = {
            coronalStartXYZ[0],
            coronalStartXYZ[1],
            coronalStartXYZ[2]
        };
        float endXYZ[3] = {
            coronalEndXYZ[0],
            coronalEndXYZ[1],
            coronalEndXYZ[2]
        };
        
        if (applyTransformationMatrixToAxes) {
            transformationMatrix.multiplyPoint3(startXYZ);
            transformationMatrix.multiplyPoint3(endXYZ);
        }
        m_fixedPipelineDrawing->drawCylinder(rgba,
                                             startXYZ,
                                             endXYZ,
                                             axesCrosshairRadius);
    }
    if (isDrawCoronalLabels) {
        float startTextXYZ[3];
        float endTextXYZ[3];
        getAxesTextLabelsXYZ(coronalStartXYZ,
                             coronalEndXYZ,
                             startTextXYZ,
                             endTextXYZ);
        glColor4fv(rgba);
        if (applyTransformationMatrixToAxes) {
            transformationMatrix.multiplyPoint3(startTextXYZ);
            transformationMatrix.multiplyPoint3(endTextXYZ);
        }
        m_fixedPipelineDrawing->drawTextModelCoords(startTextXYZ[0],
                                                    startTextXYZ[1],
                                                    startTextXYZ[2],
                                                    "P");
        m_fixedPipelineDrawing->drawTextModelCoords(endTextXYZ[0],
                                                    endTextXYZ[1],
                                                    endTextXYZ[2],
                                                    "A");
    }
    
    /*
     * Parasagittal axis and labels
     */
    const float paraStartXYZ[3] = {
        centerXYZ[0] + xMin,
        centerXYZ[1],
        centerXYZ[2]
    };
    
    const float paraEndXYZ[3] = {
        centerXYZ[0] + xMax,
        centerXYZ[1],
        centerXYZ[2]
    };
    getAxesColor(VolumeSliceViewPlaneEnum::PARASAGITTAL,
                 rgba);
    if (prefs->isVolumeAxesCrosshairsDisplayed()) {
        float startXYZ[3] = {
            paraStartXYZ[0],
            paraStartXYZ[1],
            paraStartXYZ[2]
        };
        float endXYZ[3] = {
            paraEndXYZ[0],
            paraEndXYZ[1],
            paraEndXYZ[2]
        };
        
        if (applyTransformationMatrixToAxes) {
            transformationMatrix.multiplyPoint3(startXYZ);
            transformationMatrix.multiplyPoint3(endXYZ);
        }
        m_fixedPipelineDrawing->drawCylinder(rgba,
                                             startXYZ,
                                             endXYZ,
                                             axesCrosshairRadius);
    }
    if (isDrawParasagittalLabels) {
        float startTextXYZ[3];
        float endTextXYZ[3];
        getAxesTextLabelsXYZ(paraStartXYZ,
                             paraEndXYZ,
                             startTextXYZ,
                             endTextXYZ);
        glColor4fv(rgba);
        if (applyTransformationMatrixToAxes) {
            transformationMatrix.multiplyPoint3(startTextXYZ);
            transformationMatrix.multiplyPoint3(endTextXYZ);
        }
        m_fixedPipelineDrawing->drawTextModelCoords(endTextXYZ[0],
                                                    endTextXYZ[1],
                                                    endTextXYZ[2],
                                                    "R");
        m_fixedPipelineDrawing->drawTextModelCoords(startTextXYZ[0],
                                                    startTextXYZ[1],
                                                    startTextXYZ[2],
                                                    "L");
    }
}

/**
 * Get the location of the axes labels for the given slice plane.
 *
 * @param sliceViewPlane
 *    The slice view plane.
 * @param axesStartXYZ
 *    The start of the axes.
 * @param axesEndXYZ
 *    The end of the axes.
 * @param axesTextStartXYZ
 *    Output location of label for the 'negative' axes.
 * @param axesTextEndXYZ
 *    Output location of label for the 'positive' axes.
 */
void
BrainOpenGLVolumeSliceDrawing::getAxesTextLabelsXYZ(const float axesStartXYZ[3],
                          const float axesEndXYZ[3],
                          float axesTextStartXYZ[3],
                          float axesTextEndXYZ[3]) const
{
    float startOffset[3] = { 0.0, 0.0, 0.0 };
    float endOffset[3] = { 0.0, 0.0, 0.0 };
    
    const float offsetScale = 0.04;
    for (int32_t i = 0; i < 3; i++) {
        startOffset[i] = offsetScale * axesStartXYZ[i];
        endOffset[i]   = offsetScale * axesEndXYZ[i];
    }
    
    axesTextStartXYZ[0] = axesStartXYZ[0] + startOffset[0];
    axesTextStartXYZ[1] = axesStartXYZ[1] + startOffset[1];
    axesTextStartXYZ[2] = axesStartXYZ[2] + startOffset[2];

    axesTextEndXYZ[0] = axesEndXYZ[0] + endOffset[0];
    axesTextEndXYZ[1] = axesEndXYZ[1] + endOffset[1];
    axesTextEndXYZ[2] = axesEndXYZ[2] + endOffset[2];
}

/**
 * Get the RGBA coloring for a slice view plane.
 *
 * @param sliceViewPlane
 *    The slice view plane.
 * @param rgbaOut
 *    Output colors ranging 0.0 to 1.0
 */
void
BrainOpenGLVolumeSliceDrawing::getAxesColor(const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
                  float rgbaOut[4]) const
{
    switch (sliceViewPlane) {
        case VolumeSliceViewPlaneEnum::ALL:
            CaretAssert(0);
            break;
        case VolumeSliceViewPlaneEnum::AXIAL:
            rgbaOut[0] = 0.0;
            rgbaOut[1] = 0.0;
            rgbaOut[2] = 1.0;
            rgbaOut[3] = 1.0;
            break;
        case VolumeSliceViewPlaneEnum::CORONAL:
            rgbaOut[0] = 0.0;
            rgbaOut[1] = 1.0;
            rgbaOut[2] = 0.0;
            rgbaOut[3] = 1.0;
            break;
        case VolumeSliceViewPlaneEnum::PARASAGITTAL:
            rgbaOut[0] = 1.0;
            rgbaOut[1] = 0.0;
            rgbaOut[2] = 0.0;
            rgbaOut[3] = 1.0;
            break;
    }
}

/**
 * Reset for volume identification.
 *
 * @param estimatedNumberOfItems
 *    Estimated number of items for identification.
 */
void
BrainOpenGLVolumeSliceDrawing::resetIdentification(const int32_t estimatedNumberOfItems)
{
    if (estimatedNumberOfItems > static_cast<int32_t>(m_identificationIndices.size())) {
        m_identificationIndices.resize(estimatedNumberOfItems * IDENTIFICATION_INDICES_PER_VOXEL,
                                       0);
    }
    else {
        std::fill(m_identificationIndices.begin(),
                  m_identificationIndices.end(),
                  0);
    }
}

/**
 * Add a voxel to the identification indices.
 *
 * @param volumeIndex
 *    Index of the volume.
 * @param mapIndex
 *    Index of the volume map.
 * @param voxelI
 *    Voxel Index I
 * @param voxelJ
 *    Voxel Index J
 * @param voxelK
 *    Voxel Index K
 * @param rgbaForColorIdentificationOut
 *    Encoded identification in RGBA color OUTPUT
 */
void
BrainOpenGLVolumeSliceDrawing::addVoxelToIdentification(const int32_t volumeIndex,
                                                            const int32_t mapIndex,
                                                            const int32_t voxelI,
                                                            const int32_t voxelJ,
                                                            const int32_t voxelK,
                                                            uint8_t rgbaForColorIdentificationOut[4])
{
    const int32_t idIndex = m_identificationIndices.size() / IDENTIFICATION_INDICES_PER_VOXEL;
    
    m_fixedPipelineDrawing->colorIdentification->addItem(rgbaForColorIdentificationOut,
                                                         SelectionItemDataTypeEnum::VOXEL,
                                                         idIndex);
    rgbaForColorIdentificationOut[3] = 255;
    
    /*
     * If these items change, need to update reset and
     * processing of identification.
     */
    m_identificationIndices.push_back(volumeIndex);
    m_identificationIndices.push_back(mapIndex);
    m_identificationIndices.push_back(voxelI);
    m_identificationIndices.push_back(voxelJ);
    m_identificationIndices.push_back(voxelK);
}

/**
 * Process voxel identification.
 */
void
BrainOpenGLVolumeSliceDrawing::processIdentification()
{
    int32_t identifiedItemIndex;
    float depth = -1.0;
    m_fixedPipelineDrawing->getIndexFromColorSelection(SelectionItemDataTypeEnum::VOXEL,
                                                       m_fixedPipelineDrawing->mouseX,
                                                       m_fixedPipelineDrawing->mouseY,
                                                       identifiedItemIndex,
                                                       depth);
    if (identifiedItemIndex >= 0) {
        const int32_t idIndex = identifiedItemIndex * IDENTIFICATION_INDICES_PER_VOXEL;
        const int32_t volDrawInfoIndex = m_identificationIndices[idIndex];
        CaretAssertVectorIndex(m_volumeDrawInfo, volDrawInfoIndex);
        VolumeMappableInterface* vf = m_volumeDrawInfo[volDrawInfoIndex].volumeFile;
        //const int32_t mapIndex = identificationIndices[idIndex + 1];
        const int64_t voxelIndices[3] = {
            m_identificationIndices[idIndex + 2],
            m_identificationIndices[idIndex + 3],
            m_identificationIndices[idIndex + 4]
        };
        
        SelectionItemVoxel* voxelID = m_brain->getSelectionManager()->getVoxelIdentification();
        if (voxelID->isOtherScreenDepthCloserToViewer(depth)) {
            voxelID->setVolumeFile(vf);
            voxelID->setVoxelIJK(voxelIndices);
            voxelID->setScreenDepth(depth);
            
            float voxelCoordinates[3];
            vf->indexToSpace(voxelIndices[0], voxelIndices[1], voxelIndices[2],
                             voxelCoordinates[0], voxelCoordinates[1], voxelCoordinates[2]);
            
            m_fixedPipelineDrawing->setSelectedItemScreenXYZ(voxelID,
                                                             voxelCoordinates);
            CaretLogFinest("Selected Voxel (3D): " + AString::fromNumbers(voxelIndices, 3, ","));
        }
    }
}

/* ======================================================================= */

/**
 * Constructor
 *
 * @param volumeMappableInterface
 *   Volume that contains the data values.
 * @param mapIndex
 *   Index of selected map.
 */
BrainOpenGLVolumeSliceDrawing::VolumeSlice::VolumeSlice(VolumeMappableInterface* volumeMappableInterface,
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
BrainOpenGLVolumeSliceDrawing::VolumeSlice::addValue(const float value)
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
BrainOpenGLVolumeSliceDrawing::VolumeSlice::getRgbaForValueByIndex(const int64_t indx)
{
    CaretAssertVectorIndex(m_rgba, indx * 4);
    return &m_rgba[indx*4];
}

/**
 * Allocate colors for the voxel values
 */
void
BrainOpenGLVolumeSliceDrawing::VolumeSlice::allocateColors()
{
    m_rgba.resize(m_values.size() * 4);
}




