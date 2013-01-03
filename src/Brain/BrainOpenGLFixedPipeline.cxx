

/*
 *  Copyright 1995-2002 Washington University School of Medicine
 *
 *  http://brainmap.wustl.edu
 *
 *  This file is part of CARET.
 *
 *  CARET is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  CARET is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with CARET; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */
/*LICENSE_END*/

#include <cstdlib>

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

#define __BRAIN_OPENGL_FIXED_PIPELINE_DEFINE_H
#include "BrainOpenGLFixedPipeline.h"
#undef __BRAIN_OPENGL_FIXED_PIPELINE_DEFINE_H

#include <algorithm>
#include <limits>
#include <cmath>

#include <QStringList>

#include "Border.h"
#include "BorderFile.h"
#include "Brain.h"
#include "BrainOpenGLShapeCone.h"
#include "BrainOpenGLShapeSphere.h"
#include "BrainOpenGLViewportContent.h"
#include "BrainStructure.h"
#include "BrowserTabContent.h"
#include "BoundingBox.h"
#include "CaretAssert.h"
#include "CaretLogger.h"
#include "CaretMappableDataFile.h"
#include "CaretPreferences.h"
#include "CiftiBrainordinateFile.h"
#include "CiftiConnectivityMatrixDataFile.h"
#include "CiftiFiberOrientationFile.h"
#include "CiftiFiberTrajectoryFile.h"
#include "ConnectivityLoaderFile.h"
#include "DescriptiveStatistics.h"
#include "DisplayGroupEnum.h"
#include "DisplayPropertiesBorders.h"
#include "DisplayPropertiesFiberOrientation.h"
#include "DisplayPropertiesFiberTrajectory.h"
#include "DisplayPropertiesFoci.h"
#include "DisplayPropertiesSurface.h"
#include "DisplayPropertiesVolume.h"
#include "ElapsedTimer.h"
#include "EventManager.h"
#include "EventModelSurfaceGet.h"
#include "FastStatistics.h"
#include "Fiber.h"
#include "FiberOrientation.h"
#include "FiberOrientationTrajectory.h"
#include "FociFile.h"
#include "Focus.h"
#include "GiftiLabel.h"
#include "GiftiLabelTable.h"
#include "GroupAndNameHierarchyModel.h"
#include "IdentifiedItemNode.h"
#include "IdentificationManager.h"
#include "SelectionItemBorderSurface.h"
#include "SelectionItemFocusSurface.h"
#include "SelectionItemFocusVolume.h"
#include "SelectionItemSurfaceNode.h"
#include "SelectionItemSurfaceNodeIdentificationSymbol.h"
#include "SelectionItemSurfaceTriangle.h"
#include "SelectionItemVoxel.h"
#include "IdentificationWithColor.h"
#include "SelectionManager.h"
#include "MathFunctions.h"
#include "ModelSurface.h"
#include "ModelSurfaceMontage.h"
#include "ModelVolume.h"
#include "ModelWholeBrain.h"
#include "NodeAndVoxelColoring.h"
#include "Overlay.h"
#include "OverlaySet.h"
#include "Palette.h"
#include "PaletteColorMapping.h"
#include "PaletteFile.h"
#include "PaletteScalarAndColor.h"
#include "Plane.h"
#include "SessionManager.h"
#include "Surface.h"
#include "SurfaceMontageViewport.h"
#include "SurfaceNodeColoring.h"
#include "SurfaceProjectedItem.h"
#include "SurfaceProjectionBarycentric.h"
#include "SurfaceProjectionVanEssen.h"
#include "SurfaceSelectionModel.h"
#include "TopologyHelper.h"
#include "VolumeFile.h"
#include "VolumeSurfaceOutlineColorOrTabModel.h"
#include "VolumeSurfaceOutlineModel.h"
#include "VolumeSurfaceOutlineSetModel.h"

using namespace caret;

/**
 * Constructor.
 *
 * @param parentGLWidget
 *   The optional text renderer is used for text rendering.
 *   This parameter may be NULL in which case no text
 *   rendering is performed.
 */
BrainOpenGLFixedPipeline::BrainOpenGLFixedPipeline(BrainOpenGLTextRenderInterface* textRenderer)
: BrainOpenGL(textRenderer)
{
    this->initializeMembersBrainOpenGL();
    this->colorIdentification   = new IdentificationWithColor();
    m_shapeSphere = NULL;
    m_shapeCone   = NULL;
    this->surfaceNodeColoring = new SurfaceNodeColoring();
    m_brain = NULL;
}

/**
 * Destructor.
 */
BrainOpenGLFixedPipeline::~BrainOpenGLFixedPipeline()
{
    if (m_shapeSphere != NULL) {
        delete m_shapeSphere;
        m_shapeSphere = NULL;
    }
    if (m_shapeCone != NULL) {
        delete m_shapeCone;
        m_shapeCone = NULL;
    }
    if (this->surfaceNodeColoring != NULL) {
        delete this->surfaceNodeColoring;
        this->surfaceNodeColoring = NULL;
    }
    delete this->colorIdentification;
    this->colorIdentification = NULL;
}

/**
 * Selection on a model.
 *
 * @param viewportConent
 *    Viewport content in which mouse was clicked
 * @param mouseX
 *    X position of mouse click
 * @param mouseY
 *    Y position of mouse click
 * @param applySelectionBackgroundFiltering
 *    If true (which is in most cases), if there are multiple items
 *    selected, those items "behind" other items are not reported.
 *    For example, suppose a focus is selected and there is a node
 *    the focus.  If this parameter is true, the node will NOT be
 *    selected.  If this parameter is false, the node will be 
 *    selected.
 */
void 
BrainOpenGLFixedPipeline::selectModel(BrainOpenGLViewportContent* viewportContent,
                                      const int32_t mouseX,
                                      const int32_t mouseY,
                                      const bool applySelectionBackgroundFiltering)
{
    m_brain = viewportContent->getBrain();
    CaretAssert(m_brain);
    
    this->inverseRotationMatrixValid = false;
    
    /*
     * For identification, set the background
     * to white.
     */
    glClearColor(1.0, 1.0, 1.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);    
    
    this->mouseX = mouseX;
    this->mouseY = mouseY;
    
    this->colorIdentification->reset();

    this->drawModelInternal(MODE_IDENTIFICATION,
                            viewportContent);

    m_brain->getSelectionManager()->filterSelections(applySelectionBackgroundFiltering);
    
    m_brain = NULL;
}

/**
 * Project the given window coordinate to the active models.
 * If the projection is successful, The 'original' XYZ
 * coordinate in 'projectionOut' will be valid.  In addition,
 * the barycentric coordinate may also be valid in 'projectionOut'.
 *
 * @param viewportContent
 *    Viewport content in which mouse was clicked
 * @param mouseX
 *    X position of mouse click
 * @param mouseY
 *    Y position of mouse click
 */
void 
BrainOpenGLFixedPipeline::projectToModel(BrainOpenGLViewportContent* viewportContent,
                                         const int32_t mouseX,
                                         const int32_t mouseY,
                                         SurfaceProjectedItem& projectionOut)
{
    m_brain = viewportContent->getBrain();
    CaretAssert(m_brain);
    
    m_brain->getSelectionManager()->reset();
    
    this->modeProjectionData = &projectionOut;
    this->modeProjectionData->reset();
    this->modeProjectionScreenDepth = std::numeric_limits<double>::max();
    
    /*
     * For projection which uses colors for finding triangles, 
     * set the background to white.
     */
    glClearColor(1.0, 1.0, 1.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);    
    
    this->mouseX = mouseX;
    this->mouseY = mouseY;
    
    this->colorIdentification->reset();
    
    this->drawModelInternal(MODE_PROJECTION,
                            viewportContent);
    
    this->modeProjectionData = NULL;
    m_brain = NULL;
}

/**
 * Draw models in their respective viewports.
 *
 * @param viewportContents
 *    Viewport info for drawing.
 */
void 
BrainOpenGLFixedPipeline::drawModels(std::vector<BrainOpenGLViewportContent*>& viewportContents)
{
    this->inverseRotationMatrixValid = false;
    
    this->checkForOpenGLError(NULL, "At beginning of drawModels()");
    
    float backgroundColor[3];
    CaretPreferences* prefs = SessionManager::get()->getCaretPreferences();
    prefs->getColorBackground(backgroundColor);
    glClearColor(backgroundColor[0],
                 backgroundColor[1],
                 backgroundColor[2],
                 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);    
    
    this->checkForOpenGLError(NULL, "At middle of drawModels()");
    
    /*
     * Size of all graphics area containing model(s).
     */
//    if (viewportContents.empty() == false) {
//        const int* windowVP = viewportContents[0]->getWindowViewport();
//        glViewport(windowVP[0], windowVP[1], windowVP[2], windowVP[3]);
//    }
    
    for (int32_t i = 0; i < static_cast<int32_t>(viewportContents.size()); i++) {
        /*
         * Viewport of window.
         */
        const int* windowVP = viewportContents[i]->getWindowViewport();
        glViewport(windowVP[0], windowVP[1], windowVP[2], windowVP[3]);
        
        CaretLogFine("Drawing Model "
                       + AString::number(i)
                       + ": "
                       + AString::fromNumbers(viewportContents[i]->getModelViewport(), 4, ", "));
        m_brain = viewportContents[i]->getBrain();
        CaretAssert(m_brain);
        this->drawModelInternal(MODE_DRAWING,
                                viewportContents[i]);
        m_brain = NULL;
    }
    
    this->checkForOpenGLError(NULL, "At end of drawModels()");
    
}
/**
 * Draw a model.
 *
 * @param mode
 *    The mode of operations (draw, select, etc).
 * @param viewportContent
 *    Viewport contents for drawing.
 */
void 
BrainOpenGLFixedPipeline::drawModelInternal(Mode mode,
                               BrainOpenGLViewportContent* viewportContent)
{
    ElapsedTimer et;
    et.start();
    
    this->browserTabContent= viewportContent->getBrowserTabContent();
    Model* modelDisplayController = NULL;
    
    if (this->browserTabContent != NULL) {
        Model* modelDisplayController = this->browserTabContent->getModelControllerForDisplay();
        this->windowTabIndex = this->browserTabContent->getTabNumber();
        int viewport[4];
        viewportContent->getModelViewport(viewport);
        
        
        this->mode = mode;
        
        this->checkForOpenGLError(modelDisplayController, "At beginning of drawModelInternal()");
        
        /*
         * Update transformations with those from the yoked 
         * group.  Does nothing if not yoked.
         */
        browserTabContent->updateTransformationsForYoking();
        
        if(modelDisplayController != NULL) {
            CaretAssert((this->windowTabIndex >= 0) && (this->windowTabIndex < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS));
            
            ModelSurface* surfaceController = dynamic_cast<ModelSurface*>(modelDisplayController);
            ModelSurfaceMontage* surfaceMontageController = dynamic_cast<ModelSurfaceMontage*>(modelDisplayController);
            ModelVolume* volumeController = dynamic_cast<ModelVolume*>(modelDisplayController);
            ModelWholeBrain* wholeBrainController = dynamic_cast<ModelWholeBrain*>(modelDisplayController);
            if (surfaceController != NULL) {
                this->drawSurfaceController(surfaceController, viewport);
            }
            else if (surfaceMontageController != NULL) {
                this->drawSurfaceMontageModel(browserTabContent, 
                                              surfaceMontageController, 
                                              viewport);
            }
            else if (volumeController != NULL) {
                this->drawVolumeController(browserTabContent,
                                           volumeController, viewport);
            }
            else if (wholeBrainController != NULL) {
                this->drawWholeBrainController(browserTabContent,
                                               wholeBrainController, viewport);
            }
            else {
                CaretAssertMessage(0, "Unknown type of model display controller for drawing");
            }
            
            int viewport[4];
            viewportContent->getModelViewport(viewport);
            this->drawAllPalettes(modelDisplayController->getBrain());
        }
    }
    
    glFlush();
    
    this->checkForOpenGLError(modelDisplayController, "At end of drawModelInternal()");
    
    if (modelDisplayController != NULL) {
        CaretLogFine("Time to draw " 
                    + modelDisplayController->getNameForGUI(false)
                    + " was "
                    + AString::number(et.getElapsedTimeSeconds())
                    + " seconds");
    }
}

/**
 * Set the viewport.
 * @param viewport
 *   Values for viewport (x, y, x-size, y-size)
 * @param rotationMatrixIndex
 *    Viewing rotation matrix index.
 */
void 
BrainOpenGLFixedPipeline::setViewportAndOrthographicProjection(const int32_t viewport[4],
                                                               const Model::ViewingTransformIndex rotationMatrixIndex)
{
    glViewport(viewport[0], 
               viewport[1], 
               viewport[2], 
               viewport[3]); 
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    this->setOrthographicProjection(viewport,
                                    rotationMatrixIndex);
    glMatrixMode(GL_MODELVIEW);
}

/**
 * Apply the clipping planes.
 */
void
BrainOpenGLFixedPipeline::applyClippingPlanes()
{
    glDisable(GL_CLIP_PLANE0);
    glDisable(GL_CLIP_PLANE1);
    glDisable(GL_CLIP_PLANE2);
    glDisable(GL_CLIP_PLANE3);
    glDisable(GL_CLIP_PLANE4);
    glDisable(GL_CLIP_PLANE5);
    
    if (browserTabContent != NULL) {
        /*
         * Slab along X-Axis
         */
        if (browserTabContent->isClippingPlaneEnabled(0)) {
            const float halfThick = (browserTabContent->getClippingPlaneThickness(0)
                                     * 0.5);
            const float minValue = (browserTabContent->getClippingPlaneCoordinate(0)
                                    - halfThick);
            const float maxValue = (browserTabContent->getClippingPlaneCoordinate(0)
                                    + halfThick);
            
            GLdouble planeMin[4] = {
                1.0,
                0.0,
                0.0,
                -minValue
            };
            
            glClipPlane(GL_CLIP_PLANE0,
                        planeMin);
            glEnable(GL_CLIP_PLANE0);

            GLdouble planeMax[4] = {
               -1.0,
                0.0,
                0.0,
                maxValue
            };
            
            glClipPlane(GL_CLIP_PLANE1,
                        planeMax);
            glEnable(GL_CLIP_PLANE1);
        }
        
        /*
         * Slab along Y-Axis
         */
        if (browserTabContent->isClippingPlaneEnabled(1)) {
            const float halfThick = (browserTabContent->getClippingPlaneThickness(1)
                                     * 0.5);
            const float minValue = (browserTabContent->getClippingPlaneCoordinate(1)
                                    - halfThick);
            const float maxValue = (browserTabContent->getClippingPlaneCoordinate(1)
                                    + halfThick);
            
            GLdouble planeMin[4] = {
                0.0,
                1.0,
                0.0,
                -minValue
            };
            
            glClipPlane(GL_CLIP_PLANE2,
                        planeMin);
            glEnable(GL_CLIP_PLANE2);
            
            GLdouble planeMax[4] = {
                0.0,
               -1.0,
                0.0,
                maxValue
            };
            
            glClipPlane(GL_CLIP_PLANE3,
                        planeMax);
            glEnable(GL_CLIP_PLANE3);
        }
        
        /*
         * Slab along Z-Axis
         */
        if (browserTabContent->isClippingPlaneEnabled(2)) {
            const float halfThick = (browserTabContent->getClippingPlaneThickness(2)
                                     * 0.5);
            const float minValue = (browserTabContent->getClippingPlaneCoordinate(2)
                                    - halfThick);
            const float maxValue = (browserTabContent->getClippingPlaneCoordinate(2)
                                    + halfThick);
            
            GLdouble planeMin[4] = {
                0.0,
                0.0,
                1.0,
                -minValue
            };
            
            glClipPlane(GL_CLIP_PLANE4,
                        planeMin);
            glEnable(GL_CLIP_PLANE4);
            
            GLdouble planeMax[4] = {
                0.0,
                0.0,
               -1.0,
                maxValue
            };
            
            glClipPlane(GL_CLIP_PLANE5,
                        planeMax);
            glEnable(GL_CLIP_PLANE5);
        }
    }
}


/**
 * Apply the viewing transformations for the model controller
 * in the given tab.
 *
 * @param modelDisplayController
 *    Model controller being viewed.
 * @param tabIndex
 *    Index of tab containing the controller.
 * @param objectCenterXYZ
 *    If not NULL, contains center of object about
 *    which rotation should take place.
 * @param rotationMatrixIndex
 *    Viewing rotation matrix index.
 */
void 
BrainOpenGLFixedPipeline::applyViewingTransformations(const Model* modelDisplayController,
                                         const int32_t tabIndex,
                                         const float objectCenterXYZ[3],
                                         const Model::ViewingTransformIndex viewingMatrixIndex)
{
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    const float* translation = modelDisplayController->getTranslation(tabIndex,
                                                                      viewingMatrixIndex);
    glTranslatef(translation[0], 
                 translation[1], 
                 translation[2]);
    
    const Matrix4x4* rotationMatrix = modelDisplayController->getViewingRotationMatrix(tabIndex,
                                                                                       viewingMatrixIndex);
    double rotationMatrixElements[16];
    rotationMatrix->getMatrixForOpenGL(rotationMatrixElements);
    glMultMatrixd(rotationMatrixElements);
    
    /*
     * Save the inverse rotation matrix which may be used
     * later by some drawing functions.
     */
    Matrix4x4 inverseMatrix(*rotationMatrix);
    this->inverseRotationMatrixValid = inverseMatrix.invert();
    if (this->inverseRotationMatrixValid) {
        inverseMatrix.getMatrixForOpenGL(this->inverseRotationMatrix);
    }
    
    const float scale = modelDisplayController->getScaling(tabIndex);
    glScalef(scale, 
             scale, 
             scale);    

    if (objectCenterXYZ != NULL) {
        /*
         * Place center of surface at origin.
         */
        glTranslatef(-objectCenterXYZ[0], -objectCenterXYZ[1], -objectCenterXYZ[2]); 
    }
    
    applyClippingPlanes();
}

/**
 * Apply the viewing transformations for the model controller 
 * in the given tab for viewing a volume slice in a plane.
 *
 * @param ModelVolume
 *    Volume model controller being viewed.
 * @param tabIndex
 *    Index of tab containing the controller.
 * @param viewPlane
 *    View plane of slice.
 */
void 
BrainOpenGLFixedPipeline::applyViewingTransformationsVolumeSlice(const ModelVolume* modelDisplayControllerVolume,
                                            const int32_t tabIndex,
                                            const VolumeSliceViewPlaneEnum::Enum viewPlane)
{
    VolumeFile* vf = modelDisplayControllerVolume->getUnderlayVolumeFile(tabIndex);

    /*
     * Apply some scaling and translation so that the volume slice, by default
     * is not larger than the window in which it is being viewed.
     */
    float fitToWindowScaling = -1.0;
    float fitToWindowTranslation[3] = { 0.0, 0.0, 0.0 };
    
    if (vf != NULL) {        
        BoundingBox boundingBox = vf->getSpaceBoundingBox();
        
        std::vector<int64_t> dimensions;
        vf->getDimensions(dimensions);
        if ((dimensions[0] > 2) 
            && (dimensions[1] > 2)
            && (dimensions[2] > 2)) {
            /*int64_t centerVoxel[3] = {
                dimensions[0] / 2,
                dimensions[1] / 2,
                dimensions[2] / 2
            };//*///don't use "center voxel", use center of bounding box
            
            float volumeCenter[3] = { (boundingBox.getMinX() + boundingBox.getMaxX()) / 2,
                                      (boundingBox.getMinY() + boundingBox.getMaxY()) / 2,
                                      (boundingBox.getMinZ() + boundingBox.getMaxZ()) / 2 };
            //vf->indexToSpace(centerVoxel, volumeCenter);
            
            /*
             * Translate so that the center voxel (by dimenisons)
             * is at the center of the screen.
             */
            fitToWindowTranslation[0] = -volumeCenter[0];
            fitToWindowTranslation[1] = -volumeCenter[1];
            fitToWindowTranslation[2] = -volumeCenter[2];
            switch (viewPlane) {//prevents going outside near/far?
                case VolumeSliceViewPlaneEnum::ALL:
                    break;
                case VolumeSliceViewPlaneEnum::AXIAL:
                    fitToWindowTranslation[2] = 0.0;
                    break;
                case VolumeSliceViewPlaneEnum::CORONAL:
                    fitToWindowTranslation[1] = 0.0;
                    break;
                case VolumeSliceViewPlaneEnum::PARASAGITTAL:
                    fitToWindowTranslation[0] = 0.0;
                    break;
            }
            
            
            /*
             * Scale so volume fills, but does not extend out of window.
             */
            const float xExtent = (boundingBox.getMaxX() - boundingBox.getMinX()) / 2;
            const float yExtent = (boundingBox.getMaxY() - boundingBox.getMinY()) / 2;
            const float zExtent = (boundingBox.getMaxZ() - boundingBox.getMinZ()) / 2;

            const float orthoExtentX = std::min(std::fabs(this->orthographicRight),
                                                std::fabs(this->orthographicLeft));
            const float orthoExtentY = std::min(std::fabs(this->orthographicTop),
                                                std::fabs(this->orthographicBottom));

            /*float scaleWindowX = 1.0;
            float scaleWindowY = 1.0;
            switch (viewPlane) {
                case VolumeSliceViewPlaneEnum::ALL:
                    break;
                case VolumeSliceViewPlaneEnum::AXIAL:
                    scaleWindowX = (orthoExtentX / xExtent);
                    scaleWindowY = (orthoExtentY / yExtent);
                    break;
                case VolumeSliceViewPlaneEnum::CORONAL:
                    scaleWindowX = (orthoExtentX / xExtent);
                    scaleWindowY = (orthoExtentY / zExtent);
                    break;
                case VolumeSliceViewPlaneEnum::PARASAGITTAL:
                    scaleWindowX = (orthoExtentX / yExtent);
                    scaleWindowY = (orthoExtentY / zExtent);
                    break;
            }//*///this method came up with potentially 3 different scalings in ALL view, bad idea, so now it will compute exactly 1 scaling for a volume without checking what view it is
            float temp;
            float scaleWindowX = (orthoExtentX / xExtent);
            temp = (orthoExtentX / yExtent);//parasaggital y is screen x
            if (temp < scaleWindowX) scaleWindowX = temp;
            float scaleWindowY = (orthoExtentY / zExtent);
            temp = (orthoExtentY / yExtent);//axial y is screen y
            if (temp < scaleWindowY) scaleWindowY = temp;
            fitToWindowScaling = std::min(scaleWindowX,
                                          scaleWindowY);
            fitToWindowScaling *= 0.98;
        }        
    }
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    /*
     * User's rotation.
     */
    Matrix4x4 rotationMatrix;
    switch (viewPlane) {
        case VolumeSliceViewPlaneEnum::ALL:
            break;
        case VolumeSliceViewPlaneEnum::AXIAL:
            break;
        case VolumeSliceViewPlaneEnum::CORONAL:
            rotationMatrix.rotateX(-90.0);
            //rotationMatrix.rotateY(180.0);
            break;
        case VolumeSliceViewPlaneEnum::PARASAGITTAL:
            rotationMatrix.rotateY(90.0);
            rotationMatrix.rotateZ(90.0);
            break;
    }
    
    
    double rotationMatrixElements[16];
    rotationMatrix.getMatrixForOpenGL(rotationMatrixElements);
    glMultMatrixd(rotationMatrixElements);
    
    /*
     * Save the inverse rotation matrix which may be used
     * later by some drawing functions.
     */
    Matrix4x4 inverseMatrix(rotationMatrix);
    this->inverseRotationMatrixValid = inverseMatrix.invert();
    if (this->inverseRotationMatrixValid) {
        inverseMatrix.getMatrixForOpenGL(this->inverseRotationMatrix);
    }
    
    /*
     * Scaling to fit window.
     */
    if (fitToWindowScaling > 0.0) {
        glScalef(fitToWindowScaling,
                 fitToWindowScaling,
                 fitToWindowScaling);
    }
    
    /*
     * Users scaling.
     */
    const float scale = modelDisplayControllerVolume->getScaling(tabIndex);
    glScalef(scale, 
             scale, 
             scale);

    /*
     * User's translation.
     */
    const float* translation = modelDisplayControllerVolume->getTranslation(tabIndex,
                                                                            Model::VIEWING_TRANSFORM_NORMAL);
    float translationadj[3] = { translation[0], translation[1], translation[2] };
    switch (viewPlane) {//prevents going outside near/far?
        case VolumeSliceViewPlaneEnum::ALL:
            break;
        case VolumeSliceViewPlaneEnum::AXIAL:
            translationadj[2] = 0.0;
            break;
        case VolumeSliceViewPlaneEnum::CORONAL:
            translationadj[1] = 0.0;
            break;
        case VolumeSliceViewPlaneEnum::PARASAGITTAL:
            translationadj[0] = 0.0;
            break;
    }
    
    glTranslatef(translationadj[0], 
                 translationadj[1], 
                 translationadj[2]);
    
    /*
     * Translate so that center of volume is at center
     * of window
     */
    glTranslatef(fitToWindowTranslation[0],
                 fitToWindowTranslation[1],
                 fitToWindowTranslation[2]);
    
    applyClippingPlanes();
}

void 
BrainOpenGLFixedPipeline::initializeMembersBrainOpenGL()
{
    this->initializedOpenGLFlag = false;
    this->modeProjectionData = NULL;
}
/**
 * Initialize OpenGL.
 */
void 
BrainOpenGLFixedPipeline::initializeOpenGL()
{
    if (s_staticInitialized == false) {
        s_staticInitialized = true;
        
        BrainOpenGL::initializeOpenGL();
    }
    
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glClearDepth(1.0);
    glFrontFace(GL_CCW);
    
    /*
     * As normal vectors are multiplied by transformation matrices, their 
     * lengths may no longer be one and cause drawing errors.
     * GL_NORMALIZE will rescale normal vectors to one to prevent this problem.
     * GL_RESCALE_NORMAL was added in later versions of OpenGL and
     * is reported to be more efficient.  However, GL_RESCALE_NORMAL 
     * does not seem to work with OpenGL 4.2 on Linux, whereas GL_NORMALIZE
     * seems to work on all operating systems and versions of OpenGL.
     */    
    glEnable(GL_NORMALIZE);
    
    //
    // Avoid drawing backfacing polygons
    //
    glCullFace(GL_BACK);
    glEnable(GL_CULL_FACE);
    
    glShadeModel(GL_SMOOTH);
    
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE);
    glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_FALSE);
    float lightColor[] = { 0.9f, 0.9f, 0.9f, 1.0f };
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightColor);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, lightColor);
    glEnable(GL_LIGHT0);
    glDisable(GL_LIGHT1);
    
    float materialColor[] = { 0.8f, 0.8f, 0.8f, 1.0f };
    glMaterialfv(GL_FRONT, GL_DIFFUSE, materialColor);
    glColorMaterial(GL_FRONT, GL_DIFFUSE);
    
    float ambient[] = { 0.8f, 0.8f, 0.8f, 1.0f };
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambient); 
        
    if (m_shapeSphere == NULL) {
        m_shapeSphere = new BrainOpenGLShapeSphere(5,
                                                   1.0);
    }
    if (m_shapeCone == NULL) {
        m_shapeCone = new BrainOpenGLShapeCone(8);
    }
    
    if (this->initializedOpenGLFlag) {
        return;
    }
    this->initializedOpenGLFlag = true;
    
    /*
     * Remaining items need to executed only once.
     */
}

/**
 * Enable lighting based upon the current mode.
 */
void 
BrainOpenGLFixedPipeline::enableLighting()
{
    float lightPosition[] = { 0.0f, 0.0f, 1000.0f, 0.0f };
    switch (this->mode) {
        case MODE_DRAWING:
            glPushMatrix();
            glLoadIdentity();
            glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
            glEnable(GL_LIGHT0);
            
            //
            // Light 1 position is opposite of light 0
            //
            lightPosition[0] = -lightPosition[0];
            lightPosition[1] = -lightPosition[1];
            lightPosition[2] = -lightPosition[2];
            glLightfv(GL_LIGHT1, GL_POSITION, lightPosition);      
            glEnable(GL_LIGHT1);
            glPopMatrix();
            
            glEnable(GL_LIGHTING);
            glEnable(GL_COLOR_MATERIAL);
            break;
        case MODE_IDENTIFICATION:
        case MODE_PROJECTION:
            this->disableLighting();
            break;
    }
}

/**
 * Disable lighting.
 */
void 
BrainOpenGLFixedPipeline::disableLighting()
{
    glDisable(GL_LIGHTING);
    glDisable(GL_COLOR_MATERIAL);
}

/**
 * Enable line anti-aliasing (line smoothing) which also required blending.
 */
void 
BrainOpenGLFixedPipeline::enableLineAntiAliasing()
{
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
}

/**
 * Disable line anti-aliasing (line smoothing) which also required blending.
 */
void 
BrainOpenGLFixedPipeline::disableLineAntiAliasing()
{
    glDisable(GL_LINE_SMOOTH);
    glDisable(GL_BLEND);
}

/**
 * Draw contents of a surface controller.
 * @param surfaceController
 *    Controller that is drawn.
 * @param viewport
 *    Viewport for drawing region.
 */
void 
BrainOpenGLFixedPipeline::drawSurfaceController(ModelSurface* surfaceController,
                                   const int32_t viewport[4])
{
    Surface* surface = surfaceController->getSurface();
    float center[3];
    surface->getBoundingBox()->getCenter(center);

    Model::ViewingTransformIndex rotationMatrixIndex = Model::VIEWING_TRANSFORM_NORMAL;
    const bool isRightSurfaceLateralMedialYoked = 
        this->browserTabContent->isDisplayedModelSurfaceRightLateralMedialYoked();
    if (isRightSurfaceLateralMedialYoked) {
        rotationMatrixIndex = Model::VIEWING_TRANSFORM_RIGHT_LATERAL_MEDIAL_YOKED;
    }
    
    this->setViewportAndOrthographicProjection(viewport,
                                               rotationMatrixIndex);
    
    this->applyViewingTransformations(surfaceController,
                                      this->windowTabIndex,
                                      center,
                                      rotationMatrixIndex);
    
    const float* nodeColoringRGBA = this->surfaceNodeColoring->colorSurfaceNodes(surfaceController, 
                                                                                 surface, 
                                                                                 this->windowTabIndex);
    
    this->drawSurface(surface,
                      nodeColoringRGBA);
    
// Disable on individual surface for Matt's NX crash
//    if (surface->getSurfaceType() == SurfaceTypeEnum::ANATOMICAL) {
//        this->drawSurfaceFiberOrientations();
//    }
}

/**
 * Draw the surface axes.
 */
void 
BrainOpenGLFixedPipeline::drawSurfaceAxes()
{
    const float bigNumber = 1000000.0;
    glPushMatrix();
    glColor3f(1.0, 0.0, 0.0);
    glBegin(GL_LINES);
    glVertex3f(-bigNumber, 0.0, 0.0);
    glVertex3f( bigNumber, 0.0, 0.0);
    glVertex3f(0.0, -bigNumber, 0.0);
    glVertex3f(0.0,  bigNumber, 0.0);
    glVertex3f(0.0, 0.0, -bigNumber);
    glVertex3f(0.0, 0.0, bigNumber);
    glEnd();
    glPopMatrix();
    
}

/**
 * Draw a surface.
 * @param surface
 *    Surface that is drawn.
 * @param nodeColoringRGBA
 *    RGBA coloring for the nodes.
 */
void 
BrainOpenGLFixedPipeline::drawSurface(Surface* surface,
                                      const float* nodeColoringRGBA)
{
    const DisplayPropertiesSurface* dps = m_brain->getDisplayPropertiesSurface();
    
    glMatrixMode(GL_MODELVIEW);
    
    glEnable(GL_DEPTH_TEST);
    
    this->enableLighting();
    
    const SurfaceDrawingTypeEnum::Enum drawingType = dps->getSurfaceDrawingType();
    switch (this->mode)  {
        case MODE_DRAWING:
            switch (drawingType) {
                case SurfaceDrawingTypeEnum::DRAW_HIDE:
                    break;
                case SurfaceDrawingTypeEnum::DRAW_AS_LINKS:
                    /*
                     * Draw first as triangles without coloring which uses
                     * the background color.  This prevents edges on back 
                     * from being seen.
                     */
                    glPolygonOffset(1.0, 1.0);
                    glEnable(GL_POLYGON_OFFSET_FILL);
                    disableLighting();
                    this->drawSurfaceTrianglesWithVertexArrays(surface,
                                                               NULL);
                    glDisable(GL_POLYGON_OFFSET_FILL);
                    
                    /*
                     * Now draw as polygon but outline only, do not fill.
                     */
                    enableLighting();
                    setLineWidth(dps->getLinkSize());
                    glPolygonMode(GL_FRONT, GL_LINE);
                    this->drawSurfaceTrianglesWithVertexArrays(surface,
                                                               nodeColoringRGBA);
                    glPolygonMode(GL_FRONT, GL_FILL);
                    break;
                case SurfaceDrawingTypeEnum::DRAW_AS_NODES:
                    this->drawSurfaceNodes(surface,
                                           nodeColoringRGBA);
                    break;
                case SurfaceDrawingTypeEnum::DRAW_AS_TRIANGLES:
                    this->drawSurfaceTrianglesWithVertexArrays(surface,
                                                               nodeColoringRGBA);
                    break;
            }
            if (dps->isDisplayNormalVectors()) {
                drawSurfaceNormalVectors(surface);
            }
            this->drawSurfaceBorders(surface);
            this->drawSurfaceFoci(surface);
            this->drawSurfaceNodeAttributes(surface);
            this->drawSurfaceBorderBeingDrawn(surface);
            break;
        case MODE_IDENTIFICATION:
            glShadeModel(GL_FLAT); // Turn off shading since ID info encoded in colors
            if (drawingType != SurfaceDrawingTypeEnum::DRAW_HIDE) {
                this->drawSurfaceNodes(surface,
                                       nodeColoringRGBA);
                this->drawSurfaceTriangles(surface,
                                           nodeColoringRGBA);
            }
            this->drawSurfaceBorders(surface);
            this->drawSurfaceFoci(surface);
            this->drawSurfaceNodeAttributes(surface);
            glShadeModel(GL_SMOOTH);
            break;
        case MODE_PROJECTION:
            glShadeModel(GL_FLAT); // Turn off shading since ID info encoded in colors
            if (drawingType != SurfaceDrawingTypeEnum::DRAW_HIDE) {
                this->drawSurfaceTriangles(surface,
                                           nodeColoringRGBA);
            }
            glShadeModel(GL_SMOOTH);
            break;
    }
    
    this->disableLighting();
}

/**
 * Draw a surface as individual triangles.
 * @param surface
 *    Surface that is drawn.
 * @param nodeColoringRGBA
 *    RGBA coloring for the nodes.
 */
void 
BrainOpenGLFixedPipeline::drawSurfaceTriangles(Surface* surface,
                                               const float* nodeColoringRGBA)
{
    const int numTriangles = surface->getNumberOfTriangles();
    
    const int32_t* triangles = surface->getTriangle(0);
    const float* coordinates = surface->getCoordinate(0);
    const float* normals     = surface->getNormalVector(0);

    SelectionItemSurfaceTriangle* triangleID = NULL;
    /*
     * Check for a 'selection' type mode
     */
    bool isSelect = false;
    bool isProjection = false;
    switch (this->mode) {
        case MODE_DRAWING:
            break;
        case MODE_IDENTIFICATION:
            triangleID = m_brain->getSelectionManager()->getSurfaceTriangleIdentification();
            if (triangleID->isEnabledForSelection()) {
                isSelect = true;
            }
            else {
                return;
            }
            break;
        case MODE_PROJECTION:
            isSelect = true;
            isProjection = true;
            break;
    }
    
    if (isSelect) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }
    
    uint8_t rgb[3];
    
    glBegin(GL_TRIANGLES);
    for (int32_t i = 0; i < numTriangles; i++) {
        const int32_t i3 = i * 3;
        const int32_t n1 = triangles[i3];
        const int32_t n2 = triangles[i3+1];
        const int32_t n3 = triangles[i3+2];
        
        if (isSelect) {
            this->colorIdentification->addItem(rgb, SelectionItemDataTypeEnum::SURFACE_TRIANGLE, i);
            glColor3ubv(rgb);
            glNormal3fv(&normals[n1*3]);
            glVertex3fv(&coordinates[n1*3]);
            glNormal3fv(&normals[n2*3]);
            glVertex3fv(&coordinates[n2*3]);
            glNormal3fv(&normals[n3*3]);
            glVertex3fv(&coordinates[n3*3]);
        }
        else {
            glColor4fv(&nodeColoringRGBA[n1*4]);
            glNormal3fv(&normals[n1*3]);
            glVertex3fv(&coordinates[n1*3]);
            glColor4fv(&nodeColoringRGBA[n2*4]);
            glNormal3fv(&normals[n2*3]);
            glVertex3fv(&coordinates[n2*3]);
            glColor4fv(&nodeColoringRGBA[n3*4]);
            glNormal3fv(&normals[n3*3]);
            glVertex3fv(&coordinates[n3*3]);
        }
    }
    glEnd();
    
    if (isSelect) {
        int32_t triangleIndex = -1;
        float depth = -1.0;
        this->getIndexFromColorSelection(SelectionItemDataTypeEnum::SURFACE_TRIANGLE, 
                                         this->mouseX, 
                                         this->mouseY,
                                         triangleIndex,
                                         depth);
        
        
        if (triangleIndex >= 0) {
            bool isTriangleIdAccepted = false;
            if (triangleID != NULL) {
                if (triangleID->isOtherScreenDepthCloserToViewer(depth)) {
                    triangleID->setBrain(surface->getBrainStructure()->getBrain());
                    triangleID->setSurface(surface);
                    triangleID->setTriangleNumber(triangleIndex);
                    const int32_t* triangleNodeIndices = surface->getTriangle(triangleIndex);
                    triangleID->setNearestNode(triangleNodeIndices[0]);
                    triangleID->setScreenDepth(depth);
                    isTriangleIdAccepted = true;
                    CaretLogFine("Selected Triangle: " + triangleID->toString());   
                }
                else {
                    CaretLogFine("Rejecting Selected Triangle but still using: " + triangleID->toString());   
                }
            }
            
            /*
             * Node indices
             */
            const int32_t n1 = triangles[triangleIndex*3];
            const int32_t n2 = triangles[triangleIndex*3 + 1];
            const int32_t n3 = triangles[triangleIndex*3 + 2];
            
            /*
             * Node coordinates
             */
            const float* c1 = &coordinates[n1*3];
            const float* c2 = &coordinates[n2*3];
            const float* c3 = &coordinates[n3*3];
            
            const float average[3] = {
                c1[0] + c2[0] + c3[0],
                c1[1] + c2[1] + c3[1],
                c1[2] + c2[2] + c3[2]
            };
            if (triangleID != NULL) {
                if (isTriangleIdAccepted) {
                    this->setSelectedItemScreenXYZ(triangleID, average);
                }
            }
                   
            GLdouble selectionModelviewMatrix[16];
            glGetDoublev(GL_MODELVIEW_MATRIX, selectionModelviewMatrix);
            
            GLdouble selectionProjectionMatrix[16];
            glGetDoublev(GL_PROJECTION_MATRIX, selectionProjectionMatrix);
            
            GLint selectionViewport[4];
            glGetIntegerv(GL_VIEWPORT, selectionViewport);
            
            /*
             * Window positions of each coordinate
             */
            double dc1[3] = { c1[0], c1[1], c1[2] };
            double dc2[3] = { c2[0], c2[1], c2[2] };
            double dc3[3] = { c3[0], c3[1], c3[2] };
            double wc1[3], wc2[3], wc3[3];
            if (gluProject(dc1[0], 
                           dc1[1], 
                           dc1[2],
                           selectionModelviewMatrix,
                           selectionProjectionMatrix,
                           selectionViewport,
                           &wc1[0],
                           &wc1[1],
                           &wc1[2])
                && gluProject(dc2[0], 
                              dc2[1], 
                              dc2[2],
                              selectionModelviewMatrix,
                              selectionProjectionMatrix,
                              selectionViewport,
                              &wc2[0],
                              &wc2[1],
                              &wc2[2])
                && gluProject(dc3[0], 
                              dc3[1], 
                              dc3[2],
                              selectionModelviewMatrix,
                              selectionProjectionMatrix,
                              selectionViewport,
                              &wc3[0],
                              &wc3[1],
                              &wc3[2])) {
                    const double d1 = MathFunctions::distanceSquared2D(wc1[0], 
                                                                       wc1[1], 
                                                                       this->mouseX, 
                                                                       this->mouseY);
                    const double d2 = MathFunctions::distanceSquared2D(wc2[0], 
                                                                       wc2[1], 
                                                                       this->mouseX, 
                                                                       this->mouseY);
                    const double d3 = MathFunctions::distanceSquared2D(wc3[0], 
                                                                       wc3[1], 
                                                                       this->mouseX, 
                                                                       this->mouseY);
                    if (triangleID != NULL) {
                        if (isTriangleIdAccepted) {
                            triangleID->setNearestNode(n3);
                            triangleID->setNearestNodeScreenXYZ(wc3);
                            triangleID->setNearestNodeModelXYZ(dc3);
                            if ((d1 < d2) && (d1 < d3)) {
                                triangleID->setNearestNode(n1);
                                triangleID->setNearestNodeScreenXYZ(wc1);
                                triangleID->setNearestNodeModelXYZ(dc1);
                            }
                            else if ((d2 < d1) && (d2 < d3)) {
                                triangleID->setNearestNode(n2);
                                triangleID->setNearestNodeScreenXYZ(wc2);
                                triangleID->setNearestNodeModelXYZ(dc2);
                            }
                        }
                    }
                    
                    /*
                     * Getting projected position?
                     */
                    if (isProjection) {
                        /*
                         * Place window coordinates of triangle's nodes
                         * onto the screen by setting Z-coordinate to zero
                         */
                        wc1[2] = 0.0;
                        wc2[2] = 0.0;
                        wc3[2] = 0.0;
                        
                        /*
                         * Area of triangle when projected to display
                         */
                        const double triangleDisplayArea = 
                            MathFunctions::triangleArea(wc1, wc2, wc3);
                        
                        /*
                         * If area of triangle on display is small,
                         * use a coordinate from the triangle
                         */
                        if (triangleDisplayArea < 0.001) {
                            //this->modeProjectionData->setStereotaxicXYZ(c1);
                            float barycentricAreas[3] = { 1.0, 0.0, 0.0 };
                            int barycentricNodes[3] = { n1, n1, n1 };
                            
                            this->setProjectionModeData(depth, 
                                                        c1, 
                                                        surface->getStructure(), 
                                                        barycentricAreas, 
                                                        barycentricNodes, 
                                                        surface->getNumberOfNodes());
                        }
                        else {
                            /*
                             * Determine position in triangle using barycentric coordinates
                             */
                            double displayXYZ[3] = { 
                                this->mouseX, 
                                this->mouseY, 
                                0.0 
                            };
                            
                            const double areaU = (MathFunctions::triangleArea(displayXYZ, wc2, wc3)
                                                  / triangleDisplayArea);
                            const double areaV = (MathFunctions::triangleArea(displayXYZ, wc3, wc1)
                                                  / triangleDisplayArea);
                            const double areaW = (MathFunctions::triangleArea(displayXYZ, wc1, wc2)
                                                  / triangleDisplayArea);
                            double totalArea = areaU + areaV + areaW;
                            if (totalArea <= 0) {
                                totalArea = 1.0;
                            }
                            if ((areaU < 0.0) || (areaV < 0.0) || (areaW < 0.0)) {
                                CaretLogWarning("Invalid tile area: less than zero when projecting to surface.");
                            }
                            else {
                                /*
                                 * Convert to surface coordinates
                                 */
                                const float projectedXYZ[3] = {
                                    (dc1[0]*areaU + dc2[0]*areaV + dc3[0]*areaW) / totalArea,
                                    (dc1[1]*areaU + dc2[1]*areaV + dc3[1]*areaW) / totalArea,
                                    (dc1[2]*areaU + dc2[2]*areaV + dc3[2]*areaW) / totalArea
                                };
                                //this->modeProjectionData->setStereotaxicXYZ(projectedXYZ);
                                
                                const float barycentricAreas[3] = {
                                    areaU,
                                    areaV,
                                    areaW
                                };
                                
                                const int32_t barycentricNodes[3] = {
                                    n1,
                                    n2,
                                    n3
                                };
                            
                                this->setProjectionModeData(depth, 
                                                            projectedXYZ, 
                                                            surface->getStructure(), 
                                                            barycentricAreas, 
                                                            barycentricNodes, 
                                                            surface->getNumberOfNodes());
                                /*
                                this->modeProjectionData->setStructure(surface->getStructure());
                                SurfaceProjectionBarycentric* barycentric =
                                    this->modeProjectionData->getBarycentricProjection();
                                barycentric->setProjectionSurfaceNumberOfNodes(surface->getNumberOfNodes());
                                barycentric->setTriangleAreas(barycentricAreas);
                                barycentric->setTriangleNodes(barycentricNodes);
                                barycentric->setValid(true);
                                */
                            }
                        }
                    }
            }
            CaretLogFine("Selected Triangle: " + QString::number(triangleIndex));
        }
        
    }
}

/**
 * During projection mode, set the projected data.  If the 
 * projection data is already set, it will be overridden
 * if the new data is closer, in screen depth, to the user.
 *
 * @param screenDepth
 *    Screen depth of data.
 * @param xyz
 *    Stereotaxic coordinate of projected position.
 * @param structure
 *    Structure to which data projects.
 * @param barycentricAreas
 *    Barycentric areas of projection, if to surface with valid structure.
 * @param barycentricNodes
 *    Barycentric nodes of projection, if to surface with valid structure
 * @param numberOfNodes
 *    Number of nodes in surface, if to surface with valid structure.
 */
void 
BrainOpenGLFixedPipeline::setProjectionModeData(const float screenDepth,
                                  const float xyz[3],
                                  const StructureEnum::Enum structure,
                                  const float barycentricAreas[3],
                                  const int barycentricNodes[3],
                                  const int numberOfNodes)
{
    CaretAssert(this->modeProjectionData);
    if (screenDepth < this->modeProjectionScreenDepth) {
        this->modeProjectionScreenDepth = screenDepth;
        this->modeProjectionData->setStructure(structure);
        this->modeProjectionData->setStereotaxicXYZ(xyz);
        SurfaceProjectionBarycentric* barycentric =
        this->modeProjectionData->getBarycentricProjection();
        barycentric->setProjectionSurfaceNumberOfNodes(numberOfNodes);
        barycentric->setTriangleAreas(barycentricAreas);
        barycentric->setTriangleNodes(barycentricNodes);
        barycentric->setValid(true);  
        
        CaretLogFiner("Projected to surface " 
                      + StructureEnum::toName(structure)
                      + " with depth "
                      + screenDepth);
    }
}

/**
 * Draw a surface as individual nodes.
 * @param surface
 *    Surface that is drawn.
 * @param nodeColoringRGBA
 *    RGBA coloring for the nodes.
 */
void 
BrainOpenGLFixedPipeline::drawSurfaceNodes(Surface* surface,
                                           const float* nodeColoringRGBA)
{
    const DisplayPropertiesSurface* dps = m_brain->getDisplayPropertiesSurface();
    
    const int numNodes = surface->getNumberOfNodes();
    
    const float* coordinates = surface->getCoordinate(0);
    const float* normals     = surface->getNormalVector(0);
    
    SelectionItemSurfaceNode* nodeID = 
    m_brain->getSelectionManager()->getSurfaceNodeIdentification();
    /*
     * Check for a 'selection' type mode
     */
    bool isSelect = false;
    switch (this->mode) {
        case MODE_DRAWING:
            break;
        case MODE_IDENTIFICATION:
            if (nodeID->isEnabledForSelection()) {
                isSelect = true;
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);            
            }
            else {
                return;
            }
            break;
        case MODE_PROJECTION:
            break;
    }
    
    uint8_t rgb[3];

    float pointSize = dps->getNodeSize();
    if (isSelect) {
        if (pointSize < 2.0) {
            pointSize = 2.0;
        }
    }
    setPointSize(pointSize);
    
    glBegin(GL_POINTS);
    for (int32_t i = 0; i < numNodes; i++) {
        const int32_t i3 = i * 3;
        
        if (isSelect) {
            this->colorIdentification->addItem(rgb, SelectionItemDataTypeEnum::SURFACE_NODE, i);
            glColor3ubv(rgb);
            glNormal3fv(&normals[i3]);
            glVertex3fv(&coordinates[i3]);
        }
        else {
            glColor4fv(&nodeColoringRGBA[i*4]);
            glNormal3fv(&normals[i3]);
            glVertex3fv(&coordinates[i3]);
        }
    }
    glEnd();
    
    if (isSelect) {
        int nodeIndex = -1;
        float depth = -1.0;
        this->getIndexFromColorSelection(SelectionItemDataTypeEnum::SURFACE_NODE, 
                                         this->mouseX, 
                                         this->mouseY,
                                         nodeIndex,
                                         depth);
        if (nodeIndex >= 0) {
            if (nodeID->isOtherScreenDepthCloserToViewer(depth)) {
                nodeID->setBrain(surface->getBrainStructure()->getBrain());
                nodeID->setSurface(surface);
                nodeID->setNodeNumber(nodeIndex);
                nodeID->setScreenDepth(depth);
                this->setSelectedItemScreenXYZ(nodeID, &coordinates[nodeIndex * 3]);
                CaretLogFine("Selected Vertex: " + nodeID->toString());   
            }
            else {
                CaretLogFine("Rejecting Selected Vertex: " + nodeID->toString());
            }
        }
    }
}


/**
 * Draw a surface triangles with vertex arrays.
 * @param surface
 *    Surface that is drawn.
 * @param nodeColoringRGBA
 *    RGBA coloring for the nodes.
 */
void 
BrainOpenGLFixedPipeline::drawSurfaceTrianglesWithVertexArrays(const Surface* surface,
                                                               const float* nodeColoringRGBA)
{
    glEnableClientState(GL_VERTEX_ARRAY);
    if (nodeColoringRGBA != NULL) {
        glEnableClientState(GL_COLOR_ARRAY);
    }
    glEnableClientState(GL_NORMAL_ARRAY);
    glVertexPointer(3, 
                    GL_FLOAT, 
                    0, 
                    reinterpret_cast<const GLvoid*>(surface->getCoordinate(0)));
    if (nodeColoringRGBA != NULL) {
        glColorPointer(4,
                       GL_FLOAT,
                       0,
                       reinterpret_cast<const GLvoid*>(nodeColoringRGBA));
    }
    else {
        CaretPreferences* prefs = SessionManager::get()->getCaretPreferences();
        float rgba[4];
        prefs->getColorBackground(rgba);
        glColor3f(rgba[0], rgba[1], rgba[2]);
    }
    glNormalPointer(GL_FLOAT,
                    0, 
                    reinterpret_cast<const GLvoid*>(surface->getNormalVector(0)));
    
    const int numTriangles = surface->getNumberOfTriangles();
    glDrawElements(GL_TRIANGLES, 
                   (3 * numTriangles), 
                   GL_UNSIGNED_INT,
                   reinterpret_cast<const GLvoid*>(surface->getTriangle(0)));
    
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
}

/**
 * Draw a surface's normal vectors.
 * @param surface
 *     Surface on which normal vectors are drawn.
 */
void
BrainOpenGLFixedPipeline::drawSurfaceNormalVectors(const Surface* surface)
{
    disableLighting();
    
    const float length = 10.0;    
    CaretPointer<TopologyHelper> topoHelper = surface->getTopologyHelper();
    setLineWidth(1.0);
    glColor3f(1.0, 0.0, 0.0);
    
    const int32_t numNodes = surface->getNumberOfNodes();
    glBegin(GL_LINES);
    for (int32_t i = 0; i < numNodes; i++) {
        if (topoHelper->getNodeHasNeighbors(i)) {
            const float* xyz = surface->getCoordinate(i);
            const float* normal = surface->getNormalVector(i);
            float vector[3] = {
                xyz[0] + length * normal[0],
                xyz[1] + length * normal[1],
                xyz[2] + length * normal[2]
            };
            
            glVertex3fv(xyz);
            glVertex3fv(vector);
        }
    }
    glEnd();
    
    enableLighting();
}

/**
 * Draw attributes for the given surface.
 * @param surface
 *    Surface for which attributes are drawn.
 */
void 
BrainOpenGLFixedPipeline::drawSurfaceNodeAttributes(Surface* surface)
{
    BrainStructure* brainStructure = surface->getBrainStructure();
    CaretAssert(brainStructure);
    Brain* brain = brainStructure->getBrain();
    CaretAssert(brain);
    const StructureEnum::Enum structure = surface->getStructure();
    
    const int numNodes = surface->getNumberOfNodes();
    
    const float* coordinates = surface->getCoordinate(0);

    IdentificationManager* idManager = brain->getIdentificationManager();
    
    SelectionItemSurfaceNodeIdentificationSymbol* symbolID = 
        m_brain->getSelectionManager()->getSurfaceNodeIdentificationSymbol();
    
    /*
     * Check for a 'selection' type mode
     */
    bool isSelect = false;
    switch (this->mode) {
        case MODE_DRAWING:
            break;
        case MODE_IDENTIFICATION:
            if (symbolID->isEnabledForSelection()) {
                isSelect = true;
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);            
            }
            else {
                return;
            }
            break;
        case MODE_PROJECTION:
            return;
            break;
    }
    
    const std::vector<IdentifiedItemNode> identifiedNodes = idManager->getNodeIdentifiedItemsForSurface(structure,
                                                                                                        numNodes);
    uint8_t idRGB[4];
    for (std::vector<IdentifiedItemNode>::const_iterator iter = identifiedNodes.begin();
         iter != identifiedNodes.end();
         iter++) {
        const IdentifiedItemNode& nodeID = *iter;
        
        const int32_t nodeIndex = nodeID.getNodeIndex();
        const float symbolSize = nodeID.getSymbolSize();
        
        if (isSelect) {
            this->colorIdentification->addItem(idRGB,
                                               SelectionItemDataTypeEnum::SURFACE_NODE_IDENTIFICATION_SYMBOL,
                                               nodeIndex);
            glColor3ubv(idRGB);
        }
        else {
            if (structure == nodeID.getStructure()) {
                glColor3fv(nodeID.getSymbolRGB());
            }
            else {
                glColor3fv(nodeID.getContralateralSymbolRGB());
            }
        }
        
        const int32_t i3 = nodeIndex * 3;
        glPushMatrix();
        glTranslatef(coordinates[i3], coordinates[i3+1], coordinates[i3+2]);
        this->drawSphere(symbolSize);
        glPopMatrix();
    }
    
    if (isSelect) {
        int nodeIndex = -1;
        float depth = -1.0;
        this->getIndexFromColorSelection(SelectionItemDataTypeEnum::SURFACE_NODE_IDENTIFICATION_SYMBOL, 
                                         this->mouseX, 
                                         this->mouseY,
                                         nodeIndex,
                                         depth);
        if (nodeIndex >= 0) {
            if (symbolID->isOtherScreenDepthCloserToViewer(depth)) {
                symbolID->setBrain(surface->getBrainStructure()->getBrain());
                symbolID->setSurface(surface);
                symbolID->setNodeNumber(nodeIndex);
                symbolID->setScreenDepth(depth);
                this->setSelectedItemScreenXYZ(symbolID, &coordinates[nodeIndex * 3]);
                CaretLogFine("Selected Vertex Identification Symbol: " + QString::number(nodeIndex));   
            }
        }
    }
}

/**
 * Draw a border on a surface.  The color must be set prior
 * to calling this method.
 *
 * @param surface
 *   Surface on which borders are drawn.
 * @param border
 *   Border that is drawn on the surface.
 * @param borderFileIndex
 *   Index of border file.
 * @param borderIndex
 *   Index of border.
 * @param isSelect
 *   Selection mode is active.
 */
void 
BrainOpenGLFixedPipeline::drawBorder(const Surface* surface,
                                     const Border* border,
                                     const int32_t borderFileIndex,
                                     const int32_t borderIndex,
                                     const bool isSelect,
                                     const bool isContralateralEnabled)
{
    CaretAssert(surface);
    CaretAssert(border);

    const StructureEnum::Enum surfaceStructure = surface->getStructure();
    const StructureEnum::Enum contralateralSurfaceStructure = StructureEnum::getContralateralStructure(surfaceStructure);
    
    const int32_t numBorderPoints = border->getNumberOfPoints();
    
    float pointSize = 2.0;
    float lineWidth  = 2.0;
    BorderDrawingTypeEnum::Enum drawType = BorderDrawingTypeEnum::DRAW_AS_POINTS_SPHERES;
    if (borderFileIndex >= 0) {
        const BrainStructure* bs = surface->getBrainStructure();
        const Brain* brain = bs->getBrain();
        const DisplayPropertiesBorders* dpb = brain->getDisplayPropertiesBorders();
        const DisplayGroupEnum::Enum displayGroup = dpb->getDisplayGroupForTab(this->windowTabIndex);
        pointSize = dpb->getPointSize(displayGroup,
                                      this->windowTabIndex);
        lineWidth  = dpb->getLineWidth(displayGroup,
                                       this->windowTabIndex);
        drawType  = dpb->getDrawingType(displayGroup,
                                        this->windowTabIndex);
    }
    
    bool drawSphericalPoints = false;
    bool drawSquarePoints = false;
    bool drawLines  = false;
    switch (drawType) {
        case BorderDrawingTypeEnum::DRAW_AS_LINES:
            drawLines = true;
            break;
        case BorderDrawingTypeEnum::DRAW_AS_POINTS_SPHERES:
            drawSphericalPoints = true;
            break;
        case BorderDrawingTypeEnum::DRAW_AS_POINTS_SQUARES:
            drawSquarePoints = true;
            break;
        case BorderDrawingTypeEnum::DRAW_AS_POINTS_AND_LINES:
            drawLines = true;
            drawSphericalPoints = true;
            break;
    }
    
    const float drawAtDistanceAboveSurface = 0.0;

    std::vector<float> pointXYZ;
    std::vector<int32_t> pointIndex;
    
    /*
     * Find points valid for this surface
     */
    for (int32_t i = 0; i < numBorderPoints; i++) {
        const SurfaceProjectedItem* p = border->getPoint(i);
        
        /*
         * If surface structure does not match the point's structure,
         * check to see if contralateral display is enabled and 
         * compare contralateral surface structure to point's structure.
         */
        const StructureEnum::Enum pointStructure = p->getStructure();
        bool structureMatches = true;
        if (surfaceStructure != pointStructure) {
            structureMatches = false;
            if (isContralateralEnabled) {
                if (contralateralSurfaceStructure == pointStructure) {
                    structureMatches = true;
                }
            }
        }
        if (structureMatches == false) {
            continue;
        }
        
        float xyz[3];
        const bool isXyzValid = p->getProjectedPositionAboveSurface(*surface, 
                                                                    xyz,
                                                                    drawAtDistanceAboveSurface);
        
        if (isXyzValid) {
            pointXYZ.push_back(xyz[0]);
            pointXYZ.push_back(xyz[1]);
            pointXYZ.push_back(xyz[2]);
            pointIndex.push_back(i);
        }
    }    

    const int32_t numPointsToDraw = static_cast<int32_t>(pointXYZ.size() / 3);
    
    /*
     * Draw spherical points
     */
    if (drawSphericalPoints
        || drawSquarePoints) {
        const float pointSymbolRadius = pointSize / 2.0;
        for (int32_t i = 0; i < numPointsToDraw; i++) {
            const int32_t i3 = i * 3;
            if (isSelect) {
                uint8_t idRGB[3];
                this->colorIdentification->addItem(idRGB, 
                                                   SelectionItemDataTypeEnum::BORDER_SURFACE, 
                                                   borderFileIndex,
                                                   borderIndex,
                                                   pointIndex[i]);
                glColor3ubv(idRGB);
            }
            
            const float* xyz = &pointXYZ[i3];
            glPushMatrix();
            glTranslatef(xyz[0], xyz[1], xyz[2]);
            if (drawSphericalPoints) {
                this->drawSphere(pointSymbolRadius);
            }
            else {
                this->drawSquare(pointSymbolRadius);
            }
            glPopMatrix();
        }
    }
    
    /*
     * Draw lines
     */
    if (drawLines
        && (numPointsToDraw > 1)) {    
        const float lineWidthInPixels = this->modelSizeToPixelSize(lineWidth);
        this->setLineWidth(lineWidthInPixels);
        
        this->disableLighting();
        
        if (isSelect) {
            /*
             * Start at one, since need two points for each line
             */
            for (int32_t i = 1; i < numPointsToDraw; i++) {
                const int32_t i3 = i * 3;
                uint8_t idRGB[3];
                this->colorIdentification->addItem(idRGB, 
                                                   SelectionItemDataTypeEnum::BORDER_SURFACE, 
                                                   borderFileIndex,
                                                   borderIndex,
                                                   pointIndex[i]);
                glColor3ubv(idRGB);
                
                const float* xyz1 = &pointXYZ[i3 - 3];
                const float* xyz2 = &pointXYZ[i3];
                glBegin(GL_LINES);
                glVertex3fv(xyz1);
                glVertex3fv(xyz2);
                glEnd();
            }
        }
        else {
            glBegin(GL_LINE_STRIP);
            for (int32_t i = 0; i < numPointsToDraw; i++) {
                const int32_t i3 = i * 3;
                const float* xyz = &pointXYZ[i3];
                glVertex3fv(xyz);
            }            
            glEnd();
        }
        
        this->enableLighting();
    }
}

/**
 * Set the OpenGL line width.  Value is clamped
 * to minimum and maximum values to prevent
 * OpenGL error caused by invalid line width.
 */
void 
BrainOpenGLFixedPipeline::setLineWidth(const float lineWidth)
{
    if (lineWidth > s_maxLineWidth) {
        glLineWidth(s_maxLineWidth);
    }
    else if (lineWidth < s_minLineWidth) {
        glLineWidth(s_minLineWidth);
    }
    else {
        glLineWidth(lineWidth);
    }
}

/**
 * Set the OpenGL point size.  Value is clamped
 * to minimum and maximum values to prevent
 * OpenGL error caused by invalid point size.
 */
void
BrainOpenGLFixedPipeline::setPointSize(const float pointSize)
{
    if (pointSize > s_maxPointSize) {
        glPointSize(s_maxPointSize);
    }
    else if (pointSize < s_minPointSize) {
        glPointSize(s_minPointSize);
    }
    else {
        glPointSize(pointSize);
    }
}


/**
 * Draw foci on a surface.
 * @param surface
 *   Surface on which foci are drawn.
 */
void 
BrainOpenGLFixedPipeline::drawSurfaceFoci(Surface* surface)
{
    SelectionItemFocusSurface* idFocus = m_brain->getSelectionManager()->getSurfaceFocusIdentification();
    
    /*
     * Check for a 'selection' type mode
     */
    bool isSelect = false;
    switch (this->mode) {
        case MODE_DRAWING:
            break;
        case MODE_IDENTIFICATION:
            if (idFocus->isEnabledForSelection()) {
                isSelect = true;
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);            
            }
            else {
                return;
            }
            break;
        case MODE_PROJECTION:
            return;
            break;
    }
    
    Brain* brain = surface->getBrainStructure()->getBrain();
    const DisplayPropertiesFoci* fociDisplayProperties = brain->getDisplayPropertiesFoci();
    const DisplayGroupEnum::Enum displayGroup = fociDisplayProperties->getDisplayGroupForTab(this->windowTabIndex);
    
    if (fociDisplayProperties->isDisplayed(displayGroup,
                                           this->windowTabIndex) == false) {
        return;
    }
    const float focusRadius = fociDisplayProperties->getFociSize(displayGroup,
                                                                 this->windowTabIndex) / 2.0;
    const FeatureColoringTypeEnum::Enum fociColoringType = fociDisplayProperties->getColoringType(displayGroup,
                                                                                               this->windowTabIndex);
    
    const StructureEnum::Enum surfaceStructure = surface->getStructure();
    const StructureEnum::Enum surfaceContralateralStructure = StructureEnum::getContralateralStructure(surfaceStructure);
    
    bool drawAsSpheres = false;
    switch (fociDisplayProperties->getDrawingType(displayGroup,
                                                  this->windowTabIndex)) {
        case FociDrawingTypeEnum::DRAW_AS_SPHERES:
            drawAsSpheres = true;
            break;
        case FociDrawingTypeEnum::DRAW_AS_SQUARES:
            break;
    }
    
    const bool isPasteOntoSurface = fociDisplayProperties->isPasteOntoSurface(displayGroup,
                                                                              this->windowTabIndex);
    
    const bool isContralateralEnabled = fociDisplayProperties->isContralateralDisplayed(displayGroup,
                                                                                        this->windowTabIndex);
    const int32_t numFociFiles = brain->getNumberOfFociFiles();
    for (int32_t i = 0; i < numFociFiles; i++) {
        FociFile* fociFile = brain->getFociFile(i);
        
        const GroupAndNameHierarchyModel* classAndNameSelection = fociFile->getGroupAndNameHierarchyModel();
        if (classAndNameSelection->isSelected(displayGroup,
                                              this->windowTabIndex) == false) {
            continue;
        }
        
        const GiftiLabelTable* classColorTable = fociFile->getClassColorTable();
        const GiftiLabelTable* nameColorTable = fociFile->getNameColorTable();
        
        const int32_t numFoci = fociFile->getNumberOfFoci();
        
        for (int32_t j = 0; j < numFoci; j++) {
            Focus* focus = fociFile->getFocus(j);
            float rgba[4] = { 0.0, 0.0, 0.0, 1.0 };
            
            const GroupAndNameHierarchyItem* nameItem = focus->getGroupNameSelectionItem();
            if (nameItem != NULL) {
                if (nameItem->isSelected(displayGroup,
                                         this->windowTabIndex) == false) {
                    continue;
                }
            }
            
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
                case FeatureColoringTypeEnum::FEATURE_COLORING_TYPE_COLOR_LIST:
                {
                    const float* colorRGBA = CaretColorEnum::toRGB(focus->getColor());
                    rgba[0] = colorRGBA[0];
                    rgba[1] = colorRGBA[1];
                    rgba[2] = colorRGBA[2];
                    rgba[3] = colorRGBA[3];
                }
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
                float xyz[3];
                if (spi->getProjectedPosition(*surface,
                                              xyz,
                                              isPasteOntoSurface)) {
                    const StructureEnum::Enum focusStructure = spi->getStructure();
                    bool drawIt = false;
                    if (focusStructure == surfaceStructure) {
                        drawIt = true;
                    }
                    else if (focusStructure == StructureEnum::INVALID) {
                        drawIt = true;
                    }
                    else if (isContralateralEnabled) {
                        if (focusStructure == surfaceContralateralStructure) {
                            drawIt = true;
                        }
                    }
                    
                    if (drawIt) {
                        if (isSelect) {
                            uint8_t idRGB[3];
                            this->colorIdentification->addItem(idRGB, 
                                                               SelectionItemDataTypeEnum::FOCUS_SURFACE, 
                                                               i, // file index
                                                               j, // focus index
                                                               k);// projection index
                            glColor3ubv(idRGB);
                        }
                        
                        glPushMatrix();
                        glTranslatef(xyz[0], xyz[1], xyz[2]);
                        if (drawAsSpheres) {
                            this->drawSphere(focusRadius);
                        }
                        else {
                            this->drawSquare(focusRadius);
                        }
                        glPopMatrix();
                    }
                }                
            }
        }
    }
    
    if (isSelect) {
        int32_t fociFileIndex = -1;
        int32_t focusIndex = -1;
        int32_t focusProjectionIndex = -1;
        float depth = -1.0;
        this->getIndexFromColorSelection(SelectionItemDataTypeEnum::FOCUS_SURFACE, 
                                         this->mouseX, 
                                         this->mouseY,
                                         fociFileIndex,
                                         focusIndex,
                                         focusProjectionIndex,
                                         depth);
        if (fociFileIndex >= 0) {
            if (idFocus->isOtherScreenDepthCloserToViewer(depth)) {
                Focus* focus = brain->getFociFile(fociFileIndex)->getFocus(focusIndex);
                idFocus->setBrain(brain);
                idFocus->setFocus(focus);
                idFocus->setFociFile(brain->getFociFile(fociFileIndex));
                idFocus->setFocusIndex(focusIndex);
                idFocus->setFocusProjectionIndex(focusProjectionIndex);
                idFocus->setSurface(surface);
                idFocus->setScreenDepth(depth);
                float xyz[3];
                const SurfaceProjectedItem* spi = focus->getProjection(focusProjectionIndex);
                spi->getProjectedPosition(*surface,
                                            xyz,
                                            false);
                this->setSelectedItemScreenXYZ(idFocus, xyz);
                CaretLogFine("Selected Focus Identification Symbol: " + QString::number(focusIndex));   
            }
        }
    }
}


/**
 * Draw borders on a surface.
 * @param surface
 *   Surface on which borders are drawn.
 */
void 
BrainOpenGLFixedPipeline::drawSurfaceBorders(Surface* surface)
{
    SelectionItemBorderSurface* idBorder = m_brain->getSelectionManager()->getSurfaceBorderIdentification();
    
    /*
     * Check for a 'selection' type mode
     */
    bool isSelect = false;
    switch (this->mode) {
        case MODE_DRAWING:
            break;
        case MODE_IDENTIFICATION:
            if (idBorder->isEnabledForSelection()) {
                isSelect = true;
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);            
            }
            else {
                return;
            }
            break;
        case MODE_PROJECTION:
            return;
            break;
    }
    

    Brain* brain = surface->getBrainStructure()->getBrain();
    const DisplayPropertiesBorders* borderDisplayProperties = brain->getDisplayPropertiesBorders();
    const DisplayGroupEnum::Enum displayGroup = borderDisplayProperties->getDisplayGroupForTab(this->windowTabIndex);
    if (borderDisplayProperties->isDisplayed(displayGroup,
                                             this->windowTabIndex) == false) {
        return;
    }
    
    const FeatureColoringTypeEnum::Enum borderColoringType = borderDisplayProperties->getColoringType(displayGroup,
                                                                                                      this->windowTabIndex);
    const bool isContralateralEnabled = borderDisplayProperties->isContralateralDisplayed(displayGroup,
                                                                                          this->windowTabIndex);
    const int32_t numBorderFiles = brain->getNumberOfBorderFiles();
    for (int32_t i = 0; i < numBorderFiles; i++) {
        BorderFile* borderFile = brain->getBorderFile(i);

        const GroupAndNameHierarchyModel* classAndNameSelection = borderFile->getGroupAndNameHierarchyModel();
        if (classAndNameSelection->isSelected(displayGroup,
                                              this->windowTabIndex) == false) {
            continue;
        }
        
        const GiftiLabelTable* classColorTable = borderFile->getClassColorTable();
        const GiftiLabelTable* nameColorTable  = borderFile->getNameColorTable();
        
        const int32_t numBorders = borderFile->getNumberOfBorders();
        
        for (int32_t j = 0; j < numBorders; j++) {
            Border* border = borderFile->getBorder(j);
            if (borderFile->isBorderDisplayed(displayGroup,
                                              this->windowTabIndex,
                                              border) == false) {
                continue;
            }
//            const int32_t selectionClassKey = border->getSelectionClassKey();
//            const int32_t selectionNameKey  = border->getSelectionNameKey();
//            if (classAndNameSelection->isClassSelected(displayGroup, 
//                                                       this->windowTabIndex,
//                                                       selectionClassKey) == false) {
//                continue;
//            }
//            if (classAndNameSelection->isNameSelected(displayGroup, 
//                                                      this->windowTabIndex,
//                                                      selectionClassKey, 
//                                                      selectionNameKey) == false) {
//                continue;
//            }
            
            float rgba[4] = { 0.0, 0.0, 0.0, 1.0 };
            switch (borderColoringType) {
                case FeatureColoringTypeEnum::FEATURE_COLORING_TYPE_CLASS:
                    if (border->isClassRgbaValid() == false) {
                        const GiftiLabel* colorLabel = classColorTable->getLabelBestMatching(border->getClassName());
                        if (colorLabel != NULL) {
                            border->setClassRgba(colorLabel->getColor());
                        }
                        else {
                            border->setClassRgba(rgba);
                        }
                    }
                    border->getClassRgba(rgba);
                    break;
                case FeatureColoringTypeEnum::FEATURE_COLORING_TYPE_COLOR_LIST:
                {
                    const float* colorRGBA = CaretColorEnum::toRGB(border->getColor());
                    rgba[0] = colorRGBA[0];
                    rgba[1] = colorRGBA[1];
                    rgba[2] = colorRGBA[2];
                    rgba[3] = colorRGBA[3];
                }
                    break;
                case FeatureColoringTypeEnum::FEATURE_COLORING_TYPE_NAME:
                    if (border->isNameRgbaValid() == false) {
                        const GiftiLabel* colorLabel = nameColorTable->getLabelBestMatching(border->getName());
                        if (colorLabel != NULL) {
                            border->setNameRgba(colorLabel->getColor());
                        }
                        else {
                            border->setNameRgba(rgba);
                        }
                    }
                    border->getNameRgba(rgba);
                    break;
            }
            glColor3fv(rgba);
                        
            this->drawBorder(surface, 
                             border,
                             i,
                             j,
                             isSelect,
                             isContralateralEnabled);
        }
    }
    
    if (isSelect) {
        int32_t borderFileIndex = -1;
        int32_t borderIndex = -1;
        int32_t borderPointIndex;
        float depth = -1.0;
        this->getIndexFromColorSelection(SelectionItemDataTypeEnum::BORDER_SURFACE, 
                                         this->mouseX, 
                                         this->mouseY,
                                         borderFileIndex,
                                         borderIndex,
                                         borderPointIndex,
                                         depth);
        if (borderFileIndex >= 0) {
            if (idBorder->isOtherScreenDepthCloserToViewer(depth)) {
                Border* border = brain->getBorderFile(borderFileIndex)->getBorder(borderIndex);
                idBorder->setBrain(brain);
                idBorder->setBorder(border);
                idBorder->setBorderFile(brain->getBorderFile(borderFileIndex));
                idBorder->setBorderIndex(borderIndex);
                idBorder->setBorderPointIndex(borderPointIndex);
                idBorder->setSurface(surface);
                idBorder->setScreenDepth(depth);
                float xyz[3];
                border->getPoint(borderPointIndex)->getProjectedPosition(*surface,
                                                                         xyz,
                                                                         false);
                this->setSelectedItemScreenXYZ(idBorder, xyz);
                CaretLogFine("Selected Border Identification Symbol: " + QString::number(borderIndex));   
            }
        }
    }
}

/**
 * Draw the border that is begin drawn.
 * @param surface
 *    Surface on which border is being drawn.
 */
void 
BrainOpenGLFixedPipeline::drawSurfaceBorderBeingDrawn(const Surface* surface)
{
    glColor3f(1.0, 0.0, 0.0);
    
    if (this->borderBeingDrawn != NULL) {
        this->drawBorder(surface, 
                         this->borderBeingDrawn,
                         -1,
                         -1,
                         false,
                         false);
    }
}


/**
 * Setup volume drawing information for an overlay set.
 *
 * @param browserTabContent
 *    Content in the browser tab.
 * @param paletteFile
 *    File from which palette is obtained.
 * @param volumeDrawInfoOut
 *    Output containing information for volume drawing.
 */
void 
BrainOpenGLFixedPipeline::setupVolumeDrawInfo(BrowserTabContent* browserTabContent,
                                 Brain* brain,
                                 std::vector<VolumeDrawInfo>& volumeDrawInfoOut)
{
    volumeDrawInfoOut.clear();
    
    PaletteFile* paletteFile = brain->getPaletteFile();
    OverlaySet* overlaySet = browserTabContent->getOverlaySet();
    const int32_t numberOfOverlays = overlaySet->getNumberOfDisplayedOverlays();
    for (int32_t iOver = (numberOfOverlays - 1); iOver >= 0; iOver--) {
        Overlay* overlay = overlaySet->getOverlay(iOver);
        if (overlay->isEnabled()) {
            CaretMappableDataFile* mapFile;
            int32_t mapIndex;
            overlay->getSelectionData(mapFile,
                                      mapIndex);
            if (mapFile != NULL) {
                if (mapFile->isVolumeMappable()) {
                    VolumeFile* vf = NULL;
                    CiftiBrainordinateFile* ciftiBrainFile = dynamic_cast<CiftiBrainordinateFile*>(mapFile);
                    ConnectivityLoaderFile* connLoadFile = dynamic_cast<ConnectivityLoaderFile*>(mapFile);
                    CiftiConnectivityMatrixDataFile* ciftiMatrixFile = dynamic_cast<CiftiConnectivityMatrixDataFile*>(mapFile);
                    if (connLoadFile != NULL) {
                        vf = connLoadFile->getConnectivityVolumeFile();  
                    }
                    else if (ciftiMatrixFile != NULL) {
                        vf = ciftiMatrixFile->getMapVolume(mapIndex);
                    }
                    else if (ciftiBrainFile != NULL) {
                        vf = ciftiBrainFile->getMapVolume(mapIndex);
                    }
                    else {
                        vf = dynamic_cast<VolumeFile*>(mapFile);
                    }
                    if (vf != NULL) {
                        float opacity = overlay->getOpacity();
                        if (volumeDrawInfoOut.empty()) {
                            opacity = 1.0;
                        }
                        
                        if (vf->isMappedWithPalette()) {
                            PaletteColorMapping* paletteColorMapping = vf->getMapPaletteColorMapping(mapIndex);
                            if (connLoadFile != NULL) {
                                paletteColorMapping = connLoadFile->getPaletteColorMapping(mapIndex);
                            }
                            Palette* palette = paletteFile->getPaletteByName(paletteColorMapping->getSelectedPaletteName());
                            if (palette != NULL) {
                                bool useIt = true;
                                
                                if (volumeDrawInfoOut.empty() == false) {
                                    /*
                                     * If previous volume is the same as this
                                     * volume, there is no need to draw it twice.
                                     */
                                    const VolumeDrawInfo& vdi = volumeDrawInfoOut[volumeDrawInfoOut.size() - 1];
                                    if ((vdi.volumeFile == vf) 
                                        && (opacity >= 1.0)
                                        && (mapIndex == vdi.mapIndex)
                                        && (*paletteColorMapping == *vdi.paletteColorMapping)) {
                                        useIt = false;
                                    }
                                }
                                if (useIt) {
                                    const FastStatistics* statistics = 
                                        (connLoadFile != NULL) 
                                        ? connLoadFile->getMapFastStatistics(mapIndex)
                                        : vf->getMapFastStatistics(mapIndex);
                                    
                                        VolumeDrawInfo vdi(vf,
                                                           brain,
                                                           paletteColorMapping,
                                                           statistics,
                                                           mapIndex,
                                                           opacity);
                                        volumeDrawInfoOut.push_back(vdi);
                                }
                            }
                            else {
                                CaretLogWarning("No valid palette for drawing volume file: "
                                                + vf->getFileNameNoPath());
                            }
                        }
                        else {
                            VolumeDrawInfo vdi(vf,
                                               NULL,
                                               NULL,
                                               NULL,
                                               mapIndex,
                                               opacity);
                            volumeDrawInfoOut.push_back(vdi);
                        }
                    }
                }
            }
        }
    }
}

/**
 * Draw the volume slices.
 * @param browserTabContent
 *    Content of the window.
 * @param volumeController
 *    Controller for slices.
 * @param viewport
 *    Region of drawing.
 */
void 
BrainOpenGLFixedPipeline::drawVolumeController(BrowserTabContent* browserTabContent,
                                  ModelVolume* volumeController,
                                  const int32_t viewport[4])
{
    glDisable(GL_DEPTH_TEST);
    
    const int32_t tabNumber = browserTabContent->getTabNumber();
    volumeController->updateController(tabNumber);
    Brain* brain = volumeController->getBrain();
    
    /*
     * Determine volumes that are to be drawn
     */
    std::vector<VolumeDrawInfo> volumeDrawInfo;
    this->setupVolumeDrawInfo(browserTabContent,
                              brain,
                              volumeDrawInfo);
    
    if (volumeDrawInfo.empty() == false) {
        VolumeSliceCoordinateSelection* selectedSlices = volumeController->getSelectedVolumeSlices(tabNumber);
        
        VolumeFile* underlayVolumeFile = volumeDrawInfo[0].volumeFile;
        selectedSlices->updateForVolumeFile(underlayVolumeFile);
        float selectedVoxelXYZ[3] = {
            selectedSlices->getSliceCoordinateParasagittal(),
            selectedSlices->getSliceCoordinateCoronal(),
            selectedSlices->getSliceCoordinateAxial()
        };

        switch (volumeController->getSliceViewMode(tabNumber)) {
            case VolumeSliceViewModeEnum::MONTAGE:
            {
                const int numRows = volumeController->getMontageNumberOfRows(tabNumber);
                CaretAssert(numRows > 0);
                const int numCols = volumeController->getMontageNumberOfColumns(tabNumber);
                CaretAssert(numCols > 0);
                const int vpSizeX = viewport[2] / numCols;
                const int vpSizeY = viewport[3] / numRows;
                
                /*
                 * Voxel sizes for underlay volume
                 */
                float originX, originY, originZ;
                float x1, y1, z1;
                underlayVolumeFile->indexToSpace(0, 0, 0, originX, originY, originZ);
                underlayVolumeFile->indexToSpace(1, 1, 1, x1, y1, z1);
                float sliceThickness = 0.0;
                float sliceOrigin    = 0.0;
                
                AString axisLetter = "";
                
                int sliceIndex = -1;
                int maximumSliceIndex = -1;
                std::vector<int64_t> dimensions;
                volumeDrawInfo[0].volumeFile->getDimensions(dimensions);
                const int sliceStep = volumeController->getMontageSliceSpacing(tabNumber);
                const VolumeSliceViewPlaneEnum::Enum slicePlane = volumeController->getSliceViewPlane(this->windowTabIndex);
                switch (slicePlane) {
                    case VolumeSliceViewPlaneEnum::ALL:
                        sliceIndex = -1;
                        break;
                    case VolumeSliceViewPlaneEnum::AXIAL:
                        sliceIndex = selectedSlices->getSliceIndexAxial(underlayVolumeFile);
                        maximumSliceIndex = dimensions[2];
                        sliceThickness = z1 - originZ;
                        sliceOrigin = originZ;
                        axisLetter = "Z";
                        break;
                    case VolumeSliceViewPlaneEnum::CORONAL:
                        sliceIndex = selectedSlices->getSliceIndexCoronal(underlayVolumeFile);
                        maximumSliceIndex = dimensions[1];
                        sliceThickness = y1 - originY;
                        sliceOrigin = originY;
                        axisLetter = "Y";
                        break;
                    case VolumeSliceViewPlaneEnum::PARASAGITTAL:
                        sliceIndex = selectedSlices->getSliceIndexParasagittal(underlayVolumeFile);
                        maximumSliceIndex = dimensions[0];
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
                                const int vpX = j * vpSizeX;
                                const int vpY = i * vpSizeY;
                                const int vp[4] = { 
                                    viewport[0] + vpX, 
                                    viewport[1] + vpY, 
                                    vpSizeX, 
                                    vpSizeY };
                                
                                this->setViewportAndOrthographicProjection(vp,
                                                                           Model::VIEWING_TRANSFORM_NORMAL);
                                this->applyViewingTransformationsVolumeSlice(volumeController, 
                                                                             this->windowTabIndex, 
                                                                             slicePlane);
                                this->drawVolumeOrthogonalSliceVolumeViewer(slicePlane,
                                                                sliceIndex, 
                                                                volumeDrawInfo);
                                this->drawVolumeSurfaceOutlines(brain,
                                                                volumeController,
                                                                browserTabContent,
                                                                slicePlane, 
                                                                sliceIndex, 
                                                                underlayVolumeFile);
                                this->drawVolumeFoci(brain,
                                                     volumeController,
                                                     browserTabContent,
                                                     slicePlane,
                                                     sliceIndex,
                                                     underlayVolumeFile);
                                this->drawVolumeFibers(brain,
                                                     volumeController,
                                                     browserTabContent,
                                                     slicePlane,
                                                     sliceIndex,
                                                     underlayVolumeFile);
                                this->drawVolumeAxesCrosshairs(slicePlane,
                                                               selectedVoxelXYZ);
                                const float sliceCoord = (sliceOrigin
                                                          + sliceThickness * sliceIndex);
                                if (showCoordinates) {
                                    const AString coordText = (axisLetter
                                                               + "="
                                                               + AString::number(sliceCoord, 'f', 0)
                                                               + "mm");
                                    glColor3ubv(foregroundRGB);
                                    this->drawTextWindowCoords((vpSizeX - 5),
                                                               5,
                                                               coordText,
                                                               BrainOpenGLTextRenderInterface::X_RIGHT,
                                                               BrainOpenGLTextRenderInterface::Y_BOTTOM);
                                }
                            }
                            sliceIndex += sliceStep;
                        }
                    }
                
                glViewport(viewport[0],
                           viewport[1],
                           viewport[2],
                           viewport[3]);
                this->drawVolumeAxesLabels(slicePlane, 
                                           viewport);
            }
                break;
            case VolumeSliceViewModeEnum::OBLIQUE:
                break;
            case VolumeSliceViewModeEnum::ORTHOGONAL:
            {
                const VolumeSliceViewPlaneEnum::Enum slicePlane = volumeController->getSliceViewPlane(this->windowTabIndex);
                switch (slicePlane) {
                    case VolumeSliceViewPlaneEnum::ALL:
                    {
                        const int halfX = viewport[2] / 2;
                        const int halfY = viewport[3] / 2;
                        
                        const int axialVP[4] = { viewport[0] + halfX, viewport[1], halfX, halfY };
                        this->setViewportAndOrthographicProjection(axialVP,
                                                                   Model::VIEWING_TRANSFORM_NORMAL);
                        this->applyViewingTransformationsVolumeSlice(volumeController, 
                                                                     this->windowTabIndex, 
                                                                     VolumeSliceViewPlaneEnum::AXIAL);
                        this->drawVolumeOrthogonalSliceVolumeViewer(VolumeSliceViewPlaneEnum::AXIAL, 
                                                        selectedSlices->getSliceIndexAxial(underlayVolumeFile),
                                                        volumeDrawInfo);
                        this->drawVolumeSurfaceOutlines(brain, 
                                                        volumeController,
                                                        browserTabContent,
                                                        VolumeSliceViewPlaneEnum::AXIAL, 
                                                        selectedSlices->getSliceIndexAxial(underlayVolumeFile), 
                                                        underlayVolumeFile);
                        this->drawVolumeFoci(brain,
                                             volumeController,
                                             browserTabContent,
                                             VolumeSliceViewPlaneEnum::AXIAL,
                                             selectedSlices->getSliceIndexAxial(underlayVolumeFile),
                                             underlayVolumeFile);
                        this->drawVolumeFibers(brain,
                                               volumeController,
                                               browserTabContent,
                                               VolumeSliceViewPlaneEnum::AXIAL,
                                               selectedSlices->getSliceIndexAxial(underlayVolumeFile),
                                               underlayVolumeFile);
                        this->drawVolumeAxesCrosshairs(VolumeSliceViewPlaneEnum::AXIAL,
                                                       selectedVoxelXYZ);
                        this->drawVolumeAxesLabels(VolumeSliceViewPlaneEnum::AXIAL, 
                                                   axialVP);
                        
                        const int coronalVP[4] = { viewport[0] + halfX, viewport[1] + halfY, halfX, halfY };
                        this->setViewportAndOrthographicProjection(coronalVP,
                                                                   Model::VIEWING_TRANSFORM_NORMAL);
                        this->applyViewingTransformationsVolumeSlice(volumeController, 
                                                                     this->windowTabIndex, 
                                                                     VolumeSliceViewPlaneEnum::CORONAL);
                        this->drawVolumeOrthogonalSliceVolumeViewer(VolumeSliceViewPlaneEnum::CORONAL, 
                                                        selectedSlices->getSliceIndexCoronal(underlayVolumeFile),
                                                        volumeDrawInfo);
                        this->drawVolumeSurfaceOutlines(brain, 
                                                        volumeController,
                                                        browserTabContent,
                                                        VolumeSliceViewPlaneEnum::CORONAL, 
                                                        selectedSlices->getSliceIndexCoronal(underlayVolumeFile), 
                                                        underlayVolumeFile);
                        this->drawVolumeFoci(brain,
                                             volumeController,
                                             browserTabContent,
                                             VolumeSliceViewPlaneEnum::CORONAL,
                                             selectedSlices->getSliceIndexCoronal(underlayVolumeFile),
                                             underlayVolumeFile);
                        this->drawVolumeFibers(brain,
                                               volumeController,
                                               browserTabContent,
                                               VolumeSliceViewPlaneEnum::CORONAL,
                                               selectedSlices->getSliceIndexCoronal(underlayVolumeFile),
                                               underlayVolumeFile);
                       this->drawVolumeAxesCrosshairs(VolumeSliceViewPlaneEnum::CORONAL,
                                                       selectedVoxelXYZ);
                        this->drawVolumeAxesLabels(VolumeSliceViewPlaneEnum::CORONAL, 
                                                   coronalVP);
                        
                        const int parasagittalVP[4] = { viewport[0], viewport[1] + halfY, halfX, halfY };
                        this->setViewportAndOrthographicProjection(parasagittalVP,
                                                                   Model::VIEWING_TRANSFORM_NORMAL);
                        this->applyViewingTransformationsVolumeSlice(volumeController, 
                                                                     this->windowTabIndex, 
                                                                     VolumeSliceViewPlaneEnum::PARASAGITTAL);
                        this->drawVolumeOrthogonalSliceVolumeViewer(VolumeSliceViewPlaneEnum::PARASAGITTAL, 
                                                        selectedSlices->getSliceIndexParasagittal(underlayVolumeFile),
                                                        volumeDrawInfo);
                        this->drawVolumeSurfaceOutlines(brain, 
                                                        volumeController,
                                                        browserTabContent,
                                                        VolumeSliceViewPlaneEnum::PARASAGITTAL, 
                                                        selectedSlices->getSliceIndexParasagittal(underlayVolumeFile), 
                                                        underlayVolumeFile);
                        this->drawVolumeFoci(brain,
                                             volumeController,
                                             browserTabContent,
                                             VolumeSliceViewPlaneEnum::PARASAGITTAL,
                                             selectedSlices->getSliceIndexParasagittal(underlayVolumeFile),
                                             underlayVolumeFile);
                        this->drawVolumeFibers(brain,
                                               volumeController,
                                               browserTabContent,
                                               VolumeSliceViewPlaneEnum::PARASAGITTAL,
                                               selectedSlices->getSliceIndexParasagittal(underlayVolumeFile),
                                               underlayVolumeFile);
                        this->drawVolumeAxesCrosshairs(VolumeSliceViewPlaneEnum::PARASAGITTAL,
                                                       selectedVoxelXYZ);
                        this->drawVolumeAxesLabels(VolumeSliceViewPlaneEnum::PARASAGITTAL, 
                                                   parasagittalVP);
                        
                    }
                        break;
                    case VolumeSliceViewPlaneEnum::AXIAL:
                        this->setViewportAndOrthographicProjection(viewport,
                                                                   Model::VIEWING_TRANSFORM_NORMAL);
                        this->applyViewingTransformationsVolumeSlice(volumeController, 
                                                                     this->windowTabIndex, 
                                                                     VolumeSliceViewPlaneEnum::AXIAL);
                        this->drawVolumeOrthogonalSliceVolumeViewer(slicePlane, 
                                                        selectedSlices->getSliceIndexAxial(underlayVolumeFile),
                                                        volumeDrawInfo);
                        this->drawVolumeSurfaceOutlines(brain, 
                                                        volumeController,
                                                        browserTabContent,
                                                        slicePlane, 
                                                        selectedSlices->getSliceIndexAxial(underlayVolumeFile), 
                                                        underlayVolumeFile);
                        this->drawVolumeFoci(brain,
                                             volumeController,
                                             browserTabContent,
                                             slicePlane,
                                             selectedSlices->getSliceIndexAxial(underlayVolumeFile),
                                             underlayVolumeFile);
                        this->drawVolumeFibers(brain,
                                               volumeController,
                                               browserTabContent,
                                               slicePlane,
                                               selectedSlices->getSliceIndexAxial(underlayVolumeFile),
                                               underlayVolumeFile);
                        this->drawVolumeAxesCrosshairs(slicePlane,
                                                       selectedVoxelXYZ);
                        this->drawVolumeAxesLabels(slicePlane, 
                                                   viewport);
                        break;
                    case VolumeSliceViewPlaneEnum::CORONAL:
                        this->setViewportAndOrthographicProjection(viewport,
                                                                   Model::VIEWING_TRANSFORM_NORMAL);
                        this->applyViewingTransformationsVolumeSlice(volumeController, 
                                                                     this->windowTabIndex, 
                                                                     VolumeSliceViewPlaneEnum::CORONAL);
                        this->drawVolumeOrthogonalSliceVolumeViewer(slicePlane, 
                                                        selectedSlices->getSliceIndexCoronal(underlayVolumeFile),
                                                        volumeDrawInfo);
                        this->drawVolumeSurfaceOutlines(brain, 
                                                        volumeController,
                                                        browserTabContent,
                                                        slicePlane, 
                                                        selectedSlices->getSliceIndexCoronal(underlayVolumeFile), 
                                                        underlayVolumeFile);
                        this->drawVolumeFoci(brain,
                                             volumeController,
                                             browserTabContent,
                                             slicePlane,
                                             selectedSlices->getSliceIndexCoronal(underlayVolumeFile),
                                             underlayVolumeFile);
                        this->drawVolumeFibers(brain,
                                               volumeController,
                                               browserTabContent,
                                               slicePlane,
                                               selectedSlices->getSliceIndexCoronal(underlayVolumeFile),
                                               underlayVolumeFile);
                        this->drawVolumeAxesCrosshairs(slicePlane,
                                                       selectedVoxelXYZ);
                        this->drawVolumeAxesLabels(slicePlane, 
                                                   viewport);
                        break;
                    case VolumeSliceViewPlaneEnum::PARASAGITTAL:
                        this->setViewportAndOrthographicProjection(viewport,
                                                                   Model::VIEWING_TRANSFORM_NORMAL);
                        this->applyViewingTransformationsVolumeSlice(volumeController, 
                                                                     this->windowTabIndex, 
                                                                     VolumeSliceViewPlaneEnum::PARASAGITTAL);
                        this->drawVolumeOrthogonalSliceVolumeViewer(slicePlane, 
                                                        selectedSlices->getSliceIndexParasagittal(underlayVolumeFile),
                                                        volumeDrawInfo);
                        this->drawVolumeSurfaceOutlines(brain, 
                                                        volumeController,
                                                        browserTabContent,
                                                        slicePlane, 
                                                        selectedSlices->getSliceIndexParasagittal(underlayVolumeFile), 
                                                        underlayVolumeFile);
                        this->drawVolumeFoci(brain,
                                             volumeController,
                                             browserTabContent,
                                             slicePlane,
                                             selectedSlices->getSliceIndexParasagittal(underlayVolumeFile),
                                             underlayVolumeFile);
                        this->drawVolumeFibers(brain,
                                               volumeController,
                                               browserTabContent,
                                               slicePlane,
                                               selectedSlices->getSliceIndexParasagittal(underlayVolumeFile),
                                               underlayVolumeFile);
                        this->drawVolumeAxesCrosshairs(slicePlane,
                                                       selectedVoxelXYZ);
                        this->drawVolumeAxesLabels(slicePlane, 
                                                   viewport);
                        break;
                }
            }
            break;
        }
    }
}

/**
 * Draw the volume axes crosshairs.
 * @param brain
 *   Brain that owns the volumes.
 * @param slicePlane
 *   Plane being viewed.
 * @param voxelXYZ
 *   Stereotaxic coordinate of current slice indices.
 */
void 
BrainOpenGLFixedPipeline::drawVolumeAxesCrosshairs(
                                               const VolumeSliceViewPlaneEnum::Enum slicePlane,
                                               const float voxelXYZ[3])
{
    if (SessionManager::get()->getCaretPreferences()->isVolumeAxesCrosshairsDisplayed()) {
        unsigned char red[3]   = { 255, 0, 0 };
        unsigned char green[3] = { 0, 255, 0 };
        unsigned char blue[3]  = { 0, 0, 255 };
        
        const bool drawIt = (slicePlane != VolumeSliceViewPlaneEnum::ALL);
        
        float voxelXYZmod[3] = { voxelXYZ[0], voxelXYZ[1], voxelXYZ[2] };
        
        switch (slicePlane)//prevent disappearing crosshairs when zoomed in by zeroing the into/out of plane coords
        {
            case VolumeSliceViewPlaneEnum::ALL:
                break;
            case VolumeSliceViewPlaneEnum::AXIAL:
                voxelXYZmod[2] = 0.0f;
                break;
            case VolumeSliceViewPlaneEnum::CORONAL:
                voxelXYZmod[1] = 0.0f;
                break;
            case VolumeSliceViewPlaneEnum::PARASAGITTAL:
                voxelXYZmod[0] = 0.0f;
                break;
        }
        
        if (drawIt) {
            const float bigNumber = 10000;
            this->setLineWidth(1.0);
            glColor3ubv(green);
            glBegin(GL_LINES);
            glVertex3f(voxelXYZmod[0], -bigNumber, voxelXYZmod[2]);
            glVertex3f(voxelXYZmod[0],  bigNumber, voxelXYZmod[2]);
            glColor3ubv(red);
            glVertex3f(-bigNumber, voxelXYZmod[1], voxelXYZmod[2]);
            glVertex3f( bigNumber, voxelXYZmod[1], voxelXYZmod[2]);
            glColor3ubv(blue);
            glVertex3f(voxelXYZmod[0], voxelXYZmod[1], -bigNumber);
            glVertex3f(voxelXYZmod[0], voxelXYZmod[1],  bigNumber);
            glEnd();
        }
    }    
}

/**
 * Draw the volume axes labels.
 * @param brain
 *   Brain that owns the volumes.
 * @param slicePlane
 *   Plane being viewed.
 * @param viewport
 *   Viewport of drawing region.
 */
void 
BrainOpenGLFixedPipeline::drawVolumeAxesLabels(
                          const VolumeSliceViewPlaneEnum::Enum slicePlane,
                          const int32_t viewport[4])
{
    if (SessionManager::get()->getCaretPreferences()->isVolumeAxesLabelsDisplayed()) {
        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glOrtho(0, viewport[2], 0, viewport[3], -1.0, 1.0);
        
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadIdentity();
        
        QString orientLeftSideLabel;
        QString orientRightSideLabel;
        QString orientBottomSideLabel;
        QString orientTopSideLabel;
        
        switch(slicePlane) {
            case VolumeSliceViewPlaneEnum::AXIAL:
                orientLeftSideLabel   = "L";
                orientRightSideLabel  = "R";
                orientBottomSideLabel = "P";
                orientTopSideLabel    = "A";
                break;
            case VolumeSliceViewPlaneEnum::CORONAL:
                orientLeftSideLabel   = "L";
                orientRightSideLabel  = "R";
                orientBottomSideLabel = "V";
                orientTopSideLabel    = "D";
                break;
            case VolumeSliceViewPlaneEnum::PARASAGITTAL:
                orientLeftSideLabel   = "A";
                orientRightSideLabel  = "P";
                orientBottomSideLabel = "V";
                orientTopSideLabel    = "D";
                break;
            default:
                break;
        }
        
        /*
         * Switch to the foreground color.
         */
        uint8_t foregroundRGB[3];
        SessionManager::get()->getCaretPreferences()->getColorForeground(foregroundRGB);
        glColor3ubv(foregroundRGB);
        
        this->drawTextWindowCoords(5, 
                                   (viewport[3] / 2), 
                                   orientLeftSideLabel, 
                                   BrainOpenGLTextRenderInterface::X_LEFT,
                                   BrainOpenGLTextRenderInterface::Y_CENTER);

        this->drawTextWindowCoords((viewport[2] - 5), 
                                   (viewport[3] / 2), 
                                   orientRightSideLabel, 
                                   BrainOpenGLTextRenderInterface::X_RIGHT,
                                   BrainOpenGLTextRenderInterface::Y_CENTER);
        
        this->drawTextWindowCoords((viewport[2] / 2), 
                                   5, 
                                   orientBottomSideLabel, 
                                   BrainOpenGLTextRenderInterface::X_CENTER,
                                   BrainOpenGLTextRenderInterface::Y_BOTTOM);
        
        this->drawTextWindowCoords((viewport[2] / 2), 
                                   (viewport[3] - 5), 
                                   orientTopSideLabel, 
                                   BrainOpenGLTextRenderInterface::X_CENTER,
                                   BrainOpenGLTextRenderInterface::Y_TOP);
        
        glPopMatrix();
        
        glMatrixMode(GL_PROJECTION);
        glPopMatrix();
        
        glMatrixMode(GL_MODELVIEW);
    }
}

void
BrainOpenGLFixedPipeline::drawVolumeOrthogonalSliceVolumeViewer(const VolumeSliceViewPlaneEnum::Enum slicePlane,
                                                                const int64_t sliceIndex,
                                                                std::vector<VolumeDrawInfo>& volumeDrawInfo)
{
    const int32_t numberOfVolumesToDraw = static_cast<int32_t>(volumeDrawInfo.size());
    
    SelectionItemVoxel* voxelID =
    m_brain->getSelectionManager()->getVoxelIdentification();
    
    /*
     * Check for a 'selection' type mode
     */
    bool isSelect = false;
    switch (this->mode) {
        case MODE_DRAWING:
            break;
        case MODE_IDENTIFICATION:
            if (voxelID->isEnabledForSelection()) {
                isSelect = true;
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            }
            else {
                return;
            }
            break;
        case MODE_PROJECTION:
            return;
            break;
    }
    
    /*
     * For slices disable culling since want to see both side
     * and set shading to flat so there is no interpolation of
     * colors within a voxel drawn as a quad.  This allows
     * drawing of voxels using quad strips.
     */
    this->disableLighting();
    glDisable(GL_CULL_FACE);
    glShadeModel(GL_FLAT);
    
    /*
     * Enable alpha blending so voxels that are not drawn from higher layers
     * allow voxels from lower layers to be seen.
     */
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    
    /**
     * Holds colors for voxels in the slice
     * Outside of loop to minimize allocations
     * It is faster to make one call to
     * NodeAndVoxelColoring::colorScalarsWithPalette() with
     * all voxels in the slice than it is to call it
     * separately for each voxel.
     */
    std::vector<float> sliceVoxelsValuesVector;
    std::vector<uint8_t> sliceVoxelsRgbaVector;
    
    /*
     * Draw each of the volumes separately so that each
     * is drawn with the correct voxel slices.
     */
    float sliceCoordinate = 0.0;
    for (int32_t iVol = 0; iVol < numberOfVolumesToDraw; iVol++) {
        const VolumeDrawInfo& volInfo = volumeDrawInfo[iVol];
        const VolumeFile* volumeFile = volInfo.volumeFile;
        int64_t dimI, dimJ, dimK, numMaps, numComponents;
        volumeFile->getDimensions(dimI, dimJ, dimK, numMaps, numComponents);
        const int64_t mapIndex = volInfo.mapIndex;
        
        float originX, originY, originZ;
        float x1, y1, z1;
        float lastX, lastY, lastZ;
        volumeFile->indexToSpace(0, 0, 0, originX, originY, originZ);
        volumeFile->indexToSpace(1, 1, 1, x1, y1, z1);
        volumeFile->indexToSpace(dimI - 1, dimJ - 1, dimK - 1, lastX, lastY, lastZ);
        const float voxelStepX = x1 - originX;
        const float voxelStepY = y1 - originY;
        const float voxelStepZ = z1 - originZ;
        
        /*
         * Slice coordinate is from first volume
         */
        int drawingSliceIndex = -1;
        
        if (iVol == 0) {
            switch (slicePlane) {
                case VolumeSliceViewPlaneEnum::ALL:
                    CaretAssert(0);
                    break;
                case VolumeSliceViewPlaneEnum::AXIAL:
                    sliceCoordinate = originZ + voxelStepZ * sliceIndex;
                    break;
                case VolumeSliceViewPlaneEnum::CORONAL:
                    sliceCoordinate = originY + voxelStepY * sliceIndex;
                    break;
                case VolumeSliceViewPlaneEnum::PARASAGITTAL:
                    sliceCoordinate = originX + voxelStepX * sliceIndex;
                    break;
            }
            drawingSliceIndex = sliceIndex;
        }
        else {
            /*
             * Find a voxel in the middle of the volume
             * and then get the index of the slice
             * at the sliceCoordinate.
             */
            const float minX = std::min(originX, lastX);
            const float maxX = std::max(originX, lastX);
            const float minY = std::min(originY, lastY);
            const float maxY = std::max(originY, lastY);
            const float minZ = std::min(originZ, lastZ);
            const float maxZ = std::max(originZ, lastZ);
            
            float midPoint[3] = {
                (minX + maxX) / 2.0,
                (minY + maxY) / 2.0,
                (minZ + maxZ) / 2.0
            };
            
            switch (slicePlane) {
                case VolumeSliceViewPlaneEnum::ALL:
                    CaretAssert(0);
                    break;
                case VolumeSliceViewPlaneEnum::AXIAL:
                    midPoint[2] = sliceCoordinate;
                    break;
                case VolumeSliceViewPlaneEnum::CORONAL:
                    midPoint[1] = sliceCoordinate;
                    break;
                case VolumeSliceViewPlaneEnum::PARASAGITTAL:
                    midPoint[0] = sliceCoordinate;
                    break;
            }
            
            int64_t voxelIndices[3];
            volumeFile->enclosingVoxel(midPoint, voxelIndices);
            if (volumeFile->indexValid(voxelIndices)) {
                switch (slicePlane) {
                    case VolumeSliceViewPlaneEnum::ALL:
                        CaretAssert(0);
                        break;
                    case VolumeSliceViewPlaneEnum::AXIAL:
                        drawingSliceIndex = voxelIndices[2];
                        break;
                    case VolumeSliceViewPlaneEnum::CORONAL:
                        drawingSliceIndex = voxelIndices[1];
                        break;
                    case VolumeSliceViewPlaneEnum::PARASAGITTAL:
                        drawingSliceIndex = voxelIndices[0];
                        break;
                }
            }
        }
        
        if (drawingSliceIndex >= 0) {
            int64_t iStart = 0;
            int64_t iEnd   = dimI - 1;
            int64_t jStart = 0;
            int64_t jEnd   = dimJ - 1;
            int64_t kStart = 0;
            int64_t kEnd   = dimK - 1;
            switch (slicePlane) {
                case VolumeSliceViewPlaneEnum::ALL:
                    CaretAssert(0);
                    break;
                case VolumeSliceViewPlaneEnum::AXIAL:
                    kStart = drawingSliceIndex;
                    kEnd   = drawingSliceIndex;
                    break;
                case VolumeSliceViewPlaneEnum::CORONAL:
                    jStart = drawingSliceIndex;
                    jEnd   = drawingSliceIndex;
                    break;
                case VolumeSliceViewPlaneEnum::PARASAGITTAL:
                    iStart = drawingSliceIndex;
                    iEnd   = drawingSliceIndex;
                    break;
            }
            const int64_t numSliceVoxelsI = iEnd - iStart + 1;
            const int64_t numSliceVoxelsJ = jEnd - jStart + 1;
            const int64_t numSliceVoxelsK = kEnd - kStart + 1;
            
            /*
             * Stores value for each voxel.
             * Use a vector for voxels so no worries about memory being freed.
             */
            const int64_t numVoxelsInSlice = (numSliceVoxelsI * numSliceVoxelsJ * numSliceVoxelsK);
//            if (numVoxelsInSlice > static_cast<int64_t>(sliceVoxelsValuesVector.size())) {
//                sliceVoxelsValuesVector.resize(numVoxelsInSlice);
//            }
//            float* sliceVoxelValues = &sliceVoxelsValuesVector[0];
//            
//            /*
//             * Get all voxel in the slice
//             */
//            for (int64_t k = kStart; k <= kEnd; k++) {
//                for (int64_t j = jStart; j <= jEnd; j++) {
//                    for (int64_t i = iStart; i <= iEnd; i++) {
//                        int64_t voxelOffset = -1;
//                        switch (slicePlane) {
//                            case VolumeSliceViewPlaneEnum::ALL:
//                                CaretAssert(0);
//                                break;
//                            case VolumeSliceViewPlaneEnum::AXIAL:
//                                voxelOffset = (i + (j * dimI));
//                                break;
//                            case VolumeSliceViewPlaneEnum::CORONAL:
//                                voxelOffset = (i + (k * dimI));
//                                break;
//                            case VolumeSliceViewPlaneEnum::PARASAGITTAL:
//                                voxelOffset = (j + (k * dimJ));
//                                break;
//                        }
//                        
//                        const float voxel = volumeFile->getValue(i, j, k, mapIndex);
//                        CaretAssertVectorIndex(sliceVoxelsValuesVector, voxelOffset);
//                        sliceVoxelValues[voxelOffset] = voxel;
//                    }
//                }
//            }
            
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
            volumeFile->getVoxelColorsForSliceInMap(mapIndex,
                                                    slicePlane,
                                                    drawingSliceIndex,
                                                    sliceVoxelsRGBA);
//            this->colorizeVoxels(volInfo,
//                                 sliceVoxelValues,
//                                 sliceVoxelValues,
//                                 numVoxelsInSlice,
//                                 sliceVoxelsRGBA,
//                                 false);
            //            NodeAndVoxelColoring::colorScalarsWithPalette(volInfo.statistics,
            //                                                          volInfo.paletteColorMapping,
            //                                                          volInfo.palette,
            //                                                          sliceVoxelValues,
            //                                                          sliceVoxelValues,
            //                                                          numVoxelsInSlice,
            //                                                          sliceVoxelsRGBA);
            
            /*
             * Voxels not colored will have negative alpha so fix it.
             */
            for (int64_t iVoxel = 0; iVoxel < numVoxelsInSlice; iVoxel++) {
                const int64_t alphaIndex = iVoxel * 4 + 3;
                if (sliceVoxelsRGBA[alphaIndex] <= 0) {
                    if (iVol == 0) {
                        /*
                         * For first drawn volume, use black for voxel that would not be displayed.
                         */
                        sliceVoxelsRGBA[alphaIndex - 3] = 0;
                        sliceVoxelsRGBA[alphaIndex - 2] = 0;
                        sliceVoxelsRGBA[alphaIndex - 1] = 0;
                        sliceVoxelsRGBA[alphaIndex] = 255.0;
                    }
                    else {
                        /*
                         * Do not show voxel
                         */
                        sliceVoxelsRGBA[alphaIndex] = 0;
                    }
                }
                else {
                    /*
                     * Use overlay's opacity
                     */
                    sliceVoxelsRGBA[alphaIndex] = static_cast<int8_t>(volInfo.opacity * 255.0);
                }
            }
            
            /*
             * The voxel coordinates are at the center of the voxel.
             * Shift the minimum voxel coordinates by one-half the
             * size of a voxel so that the rectangles depicting the
             * voxels are drawn with the center of the voxel at the
             * center of the rectangle.
             */
            const float halfVoxelStepX = voxelStepX * 0.5;
            const float halfVoxelStepY = voxelStepY * 0.5;
            const float halfVoxelStepZ = voxelStepZ * 0.5;
            const float minVoxelX = originX - halfVoxelStepX;
            const float minVoxelY = originY - halfVoxelStepY;
            const float minVoxelZ = originZ - halfVoxelStepZ;
            
            bool useQuadStrips = true;
            if (isSelect) {
                useQuadStrips = false;
            }
            if (useQuadStrips) {
                /*
                 * Note on quad strips:
                 *
                 * Each quad receives the color specified at the vertex
                 * 2i +2 (for i = 1..N).
                 *
                 * So, the color used to draw a quad is the color that
                 * is specified at vertex indices 2, 4, 6,.. with the first
                 * vertex index being 0.
                 */
                
                switch (slicePlane) {
                    case VolumeSliceViewPlaneEnum::ALL:
                        CaretAssert(0);
                        break;
                    case VolumeSliceViewPlaneEnum::AXIAL:
                    {
                        const float z = 0.0f;//sliceCoordinate;
                        float x = minVoxelX;
                        for (int64_t i = 0; i < dimI; i++) {
                            glBegin(GL_QUAD_STRIP);
                            {
                                const float x2 = x + voxelStepX;
                                float y = minVoxelY;
                                
                                /*
                                 * Vertices 0 and 1.
                                 */
                                glVertex3f(x, y, z);
                                glVertex3f(x2, y, z);
                                
                                for (int64_t j = 0; j < dimJ; j++) {
                                    const int32_t sliceRgbaOffset = (i + (j * dimI)) * 4;
                                    CaretAssertVectorIndex(sliceVoxelsRgbaVector, sliceRgbaOffset+3);
                                    glColor4ubv(&sliceVoxelsRGBA[sliceRgbaOffset]);
                                    
                                    y += voxelStepY;
                                    glVertex3f(x, y, z);
                                    glVertex3f(x2, y, z);
                                }
                                
                                x += voxelStepX;
                            }
                            glEnd();
                        }
                    }
                        break;
                    case VolumeSliceViewPlaneEnum::CORONAL:
                    {
                        const float y = 0.0f;//sliceCoordinate;
                        float x = minVoxelX;
                        for (int64_t i = 0; i < dimI; i++) {
                            glBegin(GL_QUAD_STRIP);
                            {
                                const float x2 = x + voxelStepX;
                                float z = minVoxelZ;
                                
                                glVertex3f(x, y, z);
                                glVertex3f(x2, y, z);
                                
                                for (int64_t k = 0; k < dimK; k++) {
                                    const int32_t sliceRgbaOffset = (i + (k * dimI)) * 4;
                                    CaretAssertVectorIndex(sliceVoxelsRgbaVector, sliceRgbaOffset+3);
                                    glColor4ubv(&sliceVoxelsRGBA[sliceRgbaOffset]);
                                    
                                    z += voxelStepZ;
                                    glVertex3f(x, y, z);
                                    glVertex3f(x2, y, z);
                                }
                                
                                x += voxelStepX;
                            }
                            glEnd();
                        }
                    }
                        break;
                    case VolumeSliceViewPlaneEnum::PARASAGITTAL:
                    {
                        const float x = 0.0f;//sliceCoordinate;
                        float y = minVoxelY;
                        for (int64_t j = 0; j < dimJ; j++) {
                            glBegin(GL_QUAD_STRIP);
                            {
                                const float y2 = y + voxelStepY;
                                float z = minVoxelZ;
                                
                                glVertex3f(x, y, z);
                                glVertex3f(x, y2, z);
                                
                                for (int64_t k = 0; k < dimK; k++) {
                                    const int32_t sliceRgbaOffset = (j + (k * dimJ)) * 4;
                                    CaretAssertVectorIndex(sliceVoxelsRgbaVector, sliceRgbaOffset+3);
                                    glColor4ubv(&sliceVoxelsRGBA[sliceRgbaOffset]);
                                    
                                    z += voxelStepZ;
                                    glVertex3f(x, y, z);
                                    glVertex3f(x, y2, z);
                                }
                                
                                y += voxelStepY;
                            }
                            glEnd();
                        }
                    }
                        break;
                }
            }
            else {
                uint8_t rgb[3];
                std::vector<float> idVoxelCoordinates;
                int64_t idVoxelCounter = 0;
                if (isSelect) {
                    const int64_t bigDim = std::max(dimI, std::max(dimJ, dimK));
                    idVoxelCoordinates.reserve(bigDim * 3);
                }
                
                glBegin(GL_QUADS);
                switch (slicePlane) {
                    case VolumeSliceViewPlaneEnum::ALL:
                        CaretAssert(0);
                        break;
                    case VolumeSliceViewPlaneEnum::AXIAL:
                    {
                        const float z1 = 0.0f;//sliceCoordinate;
                        for (int64_t i = 0; i < dimI; i++) {
                            const float x1 = minVoxelX + (voxelStepX * i);
                            const float x2 = x1 + voxelStepX;
                            for (int64_t j = 0; j < dimJ; j++) {
                                const float y1 = minVoxelY + (voxelStepY * j);
                                const float y2 = y1 + voxelStepY;
                                if (isSelect) {
                                    this->colorIdentification->addItem(rgb,
                                                                       SelectionItemDataTypeEnum::VOXEL,
                                                                       idVoxelCounter);
                                    glColor3ubv(rgb);
                                    
                                    idVoxelCoordinates.push_back(x1 + halfVoxelStepX);
                                    idVoxelCoordinates.push_back(y1 + halfVoxelStepY);
                                    idVoxelCoordinates.push_back(sliceCoordinate); // coord of slice is not offset by half voxel
                                    idVoxelCounter++;
                                }
                                else {
                                    const int32_t sliceRgbaOffset = (i + (j * dimI)) * 4;
                                    CaretAssertVectorIndex(sliceVoxelsRgbaVector, sliceRgbaOffset+3);
                                    glColor4ubv(&sliceVoxelsRGBA[sliceRgbaOffset]);
                                }
                                glVertex3f(x1, y1, z1);
                                glVertex3f(x2, y1, z1);
                                glVertex3f(x2, y2, z1);
                                glVertex3f(x1, y2, z1);
                            }
                        }
                    }
                        break;
                    case VolumeSliceViewPlaneEnum::CORONAL:
                    {
                        const float y1 = 0.0f;//sliceCoordinate;
                        for (int64_t i = 0; i < dimI; i++) {
                            const float x1 = minVoxelX + (voxelStepX * i);
                            const float x2 = x1 + voxelStepX;
                            for (int64_t k = 0; k < dimK; k++) {
                                const float z1 = minVoxelZ + (voxelStepZ * k);
                                const float z2 = z1 + voxelStepZ;
                                if (isSelect) {
                                    this->colorIdentification->addItem(rgb,
                                                                       SelectionItemDataTypeEnum::VOXEL,
                                                                       idVoxelCounter);
                                    glColor3ubv(rgb);
                                    
                                    idVoxelCoordinates.push_back(x1 + halfVoxelStepX);
                                    idVoxelCoordinates.push_back(sliceCoordinate); // coord of slice is not offset by half voxel
                                    idVoxelCoordinates.push_back(z1 + halfVoxelStepZ);
                                    idVoxelCounter++;
                                }
                                else {
                                    const int32_t sliceRgbaOffset = (i + (k * dimI)) * 4;
                                    CaretAssertVectorIndex(sliceVoxelsRgbaVector, sliceRgbaOffset+3);
                                    glColor4ubv(&sliceVoxelsRGBA[sliceRgbaOffset]);
                                }
                                glVertex3f(x1, y1, z1);
                                glVertex3f(x2, y1, z1);
                                glVertex3f(x2, y1, z2);
                                glVertex3f(x1, y1, z2);
                            }
                        }
                    }
                        break;
                    case VolumeSliceViewPlaneEnum::PARASAGITTAL:
                    {
                        const float x1 = 0.0f;//sliceCoordinate;
                        for (int64_t j = 0; j < dimJ; j++) {
                            const float y1 = minVoxelY + (voxelStepY * j);
                            const float y2 = y1 + voxelStepY;
                            for (int64_t k = 0; k < dimK; k++) {
                                const float z1 = minVoxelZ + (voxelStepZ * k);
                                const float z2 = z1 + voxelStepZ;
                                if (isSelect) {
                                    this->colorIdentification->addItem(rgb,
                                                                       SelectionItemDataTypeEnum::VOXEL,
                                                                       idVoxelCounter);
                                    glColor3ubv(rgb);
                                    
                                    idVoxelCoordinates.push_back(sliceCoordinate); // coord of slice is not offset by half voxel
                                    idVoxelCoordinates.push_back(y1 + halfVoxelStepY);
                                    idVoxelCoordinates.push_back(z1 + halfVoxelStepZ);
                                    idVoxelCounter++;
                                }
                                else {
                                    const int32_t sliceRgbaOffset = (j + (k * dimJ)) * 4;
                                    CaretAssertVectorIndex(sliceVoxelsRgbaVector, sliceRgbaOffset+3);
                                    glColor4ubv(&sliceVoxelsRGBA[sliceRgbaOffset]);
                                }
                                glVertex3f(x1, y1, z1);
                                glVertex3f(x1, y2, z1);
                                glVertex3f(x1, y2, z2);
                                glVertex3f(x1, y1, z2);
                            }
                        }
                    }
                        break;
                }
                glEnd();
                
                /*
                 * If selection enabled, find voxel that was selected.
                 */
                if (isSelect) {
                    int32_t idIndex;
                    float depth = -1.0;
                    this->getIndexFromColorSelection(SelectionItemDataTypeEnum::VOXEL,
                                                     this->mouseX,
                                                     this->mouseY,
                                                     idIndex,
                                                     depth);
                    if (idIndex >= 0) {
                        float voxelCoordinates[3] = {
                            idVoxelCoordinates[idIndex*3],
                            idVoxelCoordinates[idIndex*3+1],
                            idVoxelCoordinates[idIndex*3+2]
                        };
                        
                        for (int32_t iVol = 0; iVol < numberOfVolumesToDraw; iVol++) {
                            VolumeFile* vf = volumeDrawInfo[iVol].volumeFile;
                            int64_t voxelIndices[3];
                            vf->enclosingVoxel(voxelCoordinates,
                                               voxelIndices);
                            if (vf->indexValid(voxelIndices)) {
                                if (voxelID->isOtherScreenDepthCloserToViewer(depth)) {
                                    voxelID->setBrain(volumeDrawInfo[iVol].brain);
                                    voxelID->setVolumeFile(volumeDrawInfo[iVol].volumeFile);
                                    voxelID->setVoxelIJK(voxelIndices);
                                    voxelID->setScreenDepth(depth);
                                    this->setSelectedItemScreenXYZ(voxelID, voxelCoordinates);
                                    CaretLogFine("Selected Voxel: " + AString::fromNumbers(voxelIndices, 3, ","));
                                    break;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    
    glEnable(GL_CULL_FACE);
    glShadeModel(GL_SMOOTH);
    //    glEnable(GL_DEPTH_TEST);
    
    glDisable(GL_BLEND);
}

//void 
//BrainOpenGLFixedPipeline::drawVolumeOrthogonalSliceVolumeViewer(const VolumeSliceViewPlaneEnum::Enum slicePlane,
//                                           const int64_t sliceIndex,
//                                           std::vector<VolumeDrawInfo>& volumeDrawInfo)
//{
//    const int32_t numberOfVolumesToDraw = static_cast<int32_t>(volumeDrawInfo.size());
//    
//    SelectionItemVoxel* voxelID = 
//    m_brain->getSelectionManager()->getVoxelIdentification();
//    
//    /*
//     * Check for a 'selection' type mode
//     */
//    bool isSelect = false;
//    switch (this->mode) {
//        case MODE_DRAWING:
//            break;
//        case MODE_IDENTIFICATION:
//            if (voxelID->isEnabledForSelection()) {
//                isSelect = true;
//                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);            
//            }
//            else {
//                return;
//            }
//            break;
//        case MODE_PROJECTION:
//            return;
//            break;
//    }
//    
//    /*
//     * For slices disable culling since want to see both side
//     * and set shading to flat so there is no interpolation of
//     * colors within a voxel drawn as a quad.  This allows
//     * drawing of voxels using quad strips.
//     */
//    this->disableLighting();
//    glDisable(GL_CULL_FACE);
//    glShadeModel(GL_FLAT);
//
//    /*
//     * Enable alpha blending so voxels that are not drawn from higher layers
//     * allow voxels from lower layers to be seen.
//     */
//    glEnable(GL_BLEND);
//    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//    
//    
//    /**
//     * Holds colors for voxels in the slice
//     * Outside of loop to minimize allocations
//     * It is faster to make one call to
//     * NodeAndVoxelColoring::colorScalarsWithPalette() with
//     * all voxels in the slice than it is to call it
//     * separately for each voxel.
//     */
//    std::vector<float> sliceVoxelsValuesVector;
//    std::vector<float> sliceVoxelsRgbaVector;
//    
//    /*
//     * Draw each of the volumes separately so that each 
//     * is drawn with the correct voxel slices.
//     */
//    float sliceCoordinate = 0.0;
//    for (int32_t iVol = 0; iVol < numberOfVolumesToDraw; iVol++) {
//        const VolumeDrawInfo& volInfo = volumeDrawInfo[iVol];
//        const VolumeFile* volumeFile = volInfo.volumeFile;
//        int64_t dimI, dimJ, dimK, numMaps, numComponents;
//        volumeFile->getDimensions(dimI, dimJ, dimK, numMaps, numComponents);
//        const int64_t mapIndex = volInfo.mapIndex;        
//        
//        float originX, originY, originZ;
//        float x1, y1, z1;
//        float lastX, lastY, lastZ;
//        volumeFile->indexToSpace(0, 0, 0, originX, originY, originZ);
//        volumeFile->indexToSpace(1, 1, 1, x1, y1, z1);
//        volumeFile->indexToSpace(dimI - 1, dimJ - 1, dimK - 1, lastX, lastY, lastZ);
//        const float voxelStepX = x1 - originX;
//        const float voxelStepY = y1 - originY;
//        const float voxelStepZ = z1 - originZ;
//
//        /*
//         * Slice coordinate is from first volume
//         */
//        int drawingSliceIndex = -1;
//        
//        if (iVol == 0) {
//            switch (slicePlane) {
//                case VolumeSliceViewPlaneEnum::ALL:
//                    CaretAssert(0);
//                    break;
//                case VolumeSliceViewPlaneEnum::AXIAL:
//                    sliceCoordinate = originZ + voxelStepZ * sliceIndex;
//                    break;
//                case VolumeSliceViewPlaneEnum::CORONAL:
//                    sliceCoordinate = originY + voxelStepY * sliceIndex;
//                    break;
//                case VolumeSliceViewPlaneEnum::PARASAGITTAL:
//                    sliceCoordinate = originX + voxelStepX * sliceIndex;
//                    break;
//            }
//            drawingSliceIndex = sliceIndex;
//        }
//        else {
//            /*
//             * Find a voxel in the middle of the volume
//             * and then get the index of the slice
//             * at the sliceCoordinate.
//             */
//            const float minX = std::min(originX, lastX);
//            const float maxX = std::max(originX, lastX);
//            const float minY = std::min(originY, lastY);
//            const float maxY = std::max(originY, lastY);
//            const float minZ = std::min(originZ, lastZ);
//            const float maxZ = std::max(originZ, lastZ);
//            
//            float midPoint[3] = { 
//                (minX + maxX) / 2.0,
//                (minY + maxY) / 2.0,
//                (minZ + maxZ) / 2.0
//            };
//            
//            switch (slicePlane) {
//                case VolumeSliceViewPlaneEnum::ALL:
//                    CaretAssert(0);
//                    break;
//                case VolumeSliceViewPlaneEnum::AXIAL:
//                    midPoint[2] = sliceCoordinate;
//                    break;
//                case VolumeSliceViewPlaneEnum::CORONAL:
//                    midPoint[1] = sliceCoordinate;
//                    break;
//                case VolumeSliceViewPlaneEnum::PARASAGITTAL:
//                    midPoint[0] = sliceCoordinate;
//                    break;
//            }
//            
//            int64_t voxelIndices[3];
//            volumeFile->enclosingVoxel(midPoint, voxelIndices);
//            if (volumeFile->indexValid(voxelIndices)) {
//                switch (slicePlane) {
//                    case VolumeSliceViewPlaneEnum::ALL:
//                        CaretAssert(0);
//                        break;
//                    case VolumeSliceViewPlaneEnum::AXIAL:
//                        drawingSliceIndex = voxelIndices[2];
//                        break;
//                    case VolumeSliceViewPlaneEnum::CORONAL:
//                        drawingSliceIndex = voxelIndices[1];
//                        break;
//                    case VolumeSliceViewPlaneEnum::PARASAGITTAL:
//                        drawingSliceIndex = voxelIndices[0];
//                        break;
//                }
//            }
//        }
//        
//        if (drawingSliceIndex >= 0) {
//            int64_t iStart = 0;
//            int64_t iEnd   = dimI - 1;
//            int64_t jStart = 0;
//            int64_t jEnd   = dimJ - 1;
//            int64_t kStart = 0;
//            int64_t kEnd   = dimK - 1;
//            switch (slicePlane) {
//                case VolumeSliceViewPlaneEnum::ALL:
//                    CaretAssert(0);
//                    break;
//                case VolumeSliceViewPlaneEnum::AXIAL:
//                    kStart = drawingSliceIndex;
//                    kEnd   = drawingSliceIndex;
//                    break;
//                case VolumeSliceViewPlaneEnum::CORONAL:
//                    jStart = drawingSliceIndex;
//                    jEnd   = drawingSliceIndex;
//                    break;
//                case VolumeSliceViewPlaneEnum::PARASAGITTAL:
//                    iStart = drawingSliceIndex;
//                    iEnd   = drawingSliceIndex;
//                    break;
//            }
//            const int64_t numSliceVoxelsI = iEnd - iStart + 1;
//            const int64_t numSliceVoxelsJ = jEnd - jStart + 1;
//            const int64_t numSliceVoxelsK = kEnd - kStart + 1;
//            
//            /*
//             * Stores value for each voxel.
//             * Use a vector for voxels so no worries about memory being freed.
//             */
//            const int64_t numVoxelsInSlice = (numSliceVoxelsI * numSliceVoxelsJ * numSliceVoxelsK);
//            if (numVoxelsInSlice > static_cast<int64_t>(sliceVoxelsValuesVector.size())) {
//                sliceVoxelsValuesVector.resize(numVoxelsInSlice);
//            }
//            float* sliceVoxelValues = &sliceVoxelsValuesVector[0];
//            
//            /*
//             * Get all voxel in the slice
//             */
//            for (int64_t k = kStart; k <= kEnd; k++) {                        
//                for (int64_t j = jStart; j <= jEnd; j++) {
//                    for (int64_t i = iStart; i <= iEnd; i++) {
//                        int64_t voxelOffset = -1;
//                        switch (slicePlane) {
//                            case VolumeSliceViewPlaneEnum::ALL:
//                                CaretAssert(0);
//                                break;
//                            case VolumeSliceViewPlaneEnum::AXIAL:
//                                voxelOffset = (i + (j * dimI));
//                                break;
//                            case VolumeSliceViewPlaneEnum::CORONAL:
//                                voxelOffset = (i + (k * dimI));
//                                break;
//                            case VolumeSliceViewPlaneEnum::PARASAGITTAL:
//                                voxelOffset = (j + (k * dimJ));
//                                break;
//                        }
//                        
//                        const float voxel = volumeFile->getValue(i, j, k, mapIndex);
//                        CaretAssertVectorIndex(sliceVoxelsValuesVector, voxelOffset);
//                        sliceVoxelValues[voxelOffset] = voxel;
//                    }
//                }
//            }
//            
//            /*
//             * Stores RGBA values for each voxel.
//             * Use a vector for voxel colors so no worries about memory being freed.
//             */
//            const int64_t numVoxelsInSliceRGBA = numVoxelsInSlice * 4;
//            if (numVoxelsInSliceRGBA > static_cast<int64_t>(sliceVoxelsRgbaVector.size())) {
//                sliceVoxelsRgbaVector.resize(numVoxelsInSliceRGBA);
//            }
//            float* sliceVoxelsRGBA = &sliceVoxelsRgbaVector[0];
//
//            /*
//             * Get colors for all voxels in the slice.
//             */
//            this->colorizeVoxels(volInfo,
//                                 sliceVoxelValues,
//                                 sliceVoxelValues,
//                                 numVoxelsInSlice,
//                                 sliceVoxelsRGBA,
//                                 false);
////            NodeAndVoxelColoring::colorScalarsWithPalette(volInfo.statistics,
////                                                          volInfo.paletteColorMapping,
////                                                          volInfo.palette,
////                                                          sliceVoxelValues,
////                                                          sliceVoxelValues,
////                                                          numVoxelsInSlice,
////                                                          sliceVoxelsRGBA);
//            
//            /*
//             * Voxels not colored will have negative alpha so fix it.
//             */
//            for (int64_t iVoxel = 0; iVoxel < numVoxelsInSlice; iVoxel++) {
//                const int64_t alphaIndex = iVoxel * 4 + 3;
//                if (sliceVoxelsRGBA[alphaIndex] <= 0) {
//                    if (iVol == 0) {
//                        /*
//                         * For first drawn volume, use black for voxel that would not be displayed.
//                         */
//                        sliceVoxelsRGBA[alphaIndex - 3] = 0.0;
//                        sliceVoxelsRGBA[alphaIndex - 2] = 0.0;
//                        sliceVoxelsRGBA[alphaIndex - 1] = 0.0;
//                        sliceVoxelsRGBA[alphaIndex] = 1.0;
//                    }
//                    else {
//                        /*
//                         * Do not show voxel
//                         */
//                        sliceVoxelsRGBA[alphaIndex] = 0.0;
//                    }
//                }
//                else {
//                    /*
//                     * Use overlay's opacity
//                     */
//                    sliceVoxelsRGBA[alphaIndex] = volInfo.opacity;
//                }
//            }
//            
//            /*
//             * The voxel coordinates are at the center of the voxel.  
//             * Shift the minimum voxel coordinates by one-half the 
//             * size of a voxel so that the rectangles depicting the 
//             * voxels are drawn with the center of the voxel at the
//             * center of the rectangle.
//             */
//            const float halfVoxelStepX = voxelStepX * 0.5;
//            const float halfVoxelStepY = voxelStepY * 0.5;
//            const float halfVoxelStepZ = voxelStepZ * 0.5;
//            const float minVoxelX = originX - halfVoxelStepX;
//            const float minVoxelY = originY - halfVoxelStepY;
//            const float minVoxelZ = originZ - halfVoxelStepZ;
//            
//            bool useQuadStrips = true;
//            if (isSelect) {
//                useQuadStrips = false;
//            }
//            if (useQuadStrips) {
//                /*
//                 * Note on quad strips:
//                 *
//                 * Each quad receives the color specified at the vertex
//                 * 2i +2 (for i = 1..N).
//                 *
//                 * So, the color used to draw a quad is the color that
//                 * is specified at vertex indices 2, 4, 6,.. with the first
//                 * vertex index being 0.
//                 */
//                
//                switch (slicePlane) {
//                    case VolumeSliceViewPlaneEnum::ALL:
//                        CaretAssert(0);
//                        break;
//                    case VolumeSliceViewPlaneEnum::AXIAL:
//                    {
//                        const float z = 0.0f;//sliceCoordinate;
//                        float x = minVoxelX;
//                        for (int64_t i = 0; i < dimI; i++) {
//                            glBegin(GL_QUAD_STRIP);
//                            {
//                                const float x2 = x + voxelStepX;
//                                float y = minVoxelY;
//                                
//                                /*
//                                 * Vertices 0 and 1.
//                                 */
//                                glVertex3f(x, y, z);
//                                glVertex3f(x2, y, z);
//                                
//                                for (int64_t j = 0; j < dimJ; j++) {
//                                    const int32_t sliceRgbaOffset = (i + (j * dimI)) * 4;
//                                    CaretAssertVectorIndex(sliceVoxelsRgbaVector, sliceRgbaOffset+3);
//                                    glColor4fv(&sliceVoxelsRGBA[sliceRgbaOffset]);
//
//                                    y += voxelStepY;
//                                    glVertex3f(x, y, z);
//                                    glVertex3f(x2, y, z);
//                                }
//                                
//                                x += voxelStepX;
//                            }
//                            glEnd();
//                        }
//                    }
//                        break;
//                    case VolumeSliceViewPlaneEnum::CORONAL:
//                    {
//                        const float y = 0.0f;//sliceCoordinate;
//                        float x = minVoxelX;
//                        for (int64_t i = 0; i < dimI; i++) {
//                            glBegin(GL_QUAD_STRIP);
//                            {
//                                const float x2 = x + voxelStepX;
//                                float z = minVoxelZ;
//                                
//                                glVertex3f(x, y, z);
//                                glVertex3f(x2, y, z);
//                                
//                                for (int64_t k = 0; k < dimK; k++) {
//                                    const int32_t sliceRgbaOffset = (i + (k * dimI)) * 4;
//                                    CaretAssertVectorIndex(sliceVoxelsRgbaVector, sliceRgbaOffset+3);
//                                    glColor4fv(&sliceVoxelsRGBA[sliceRgbaOffset]);
//                                    
//                                    z += voxelStepZ;
//                                    glVertex3f(x, y, z);
//                                    glVertex3f(x2, y, z);
//                                }
//                                
//                                x += voxelStepX;
//                            }
//                            glEnd();
//                        }
//                    }
//                        break;
//                    case VolumeSliceViewPlaneEnum::PARASAGITTAL:
//                    {
//                        const float x = 0.0f;//sliceCoordinate;
//                        float y = minVoxelY;
//                        for (int64_t j = 0; j < dimJ; j++) {
//                            glBegin(GL_QUAD_STRIP);
//                            {
//                                const float y2 = y + voxelStepY;
//                                float z = minVoxelZ;
//                                
//                                glVertex3f(x, y, z);
//                                glVertex3f(x, y2, z);
//                                
//                                for (int64_t k = 0; k < dimK; k++) {
//                                    const int32_t sliceRgbaOffset = (j + (k * dimJ)) * 4;
//                                    CaretAssertVectorIndex(sliceVoxelsRgbaVector, sliceRgbaOffset+3);
//                                    glColor4fv(&sliceVoxelsRGBA[sliceRgbaOffset]);
//                                    
//                                    z += voxelStepZ;
//                                    glVertex3f(x, y, z);
//                                    glVertex3f(x, y2, z);
//                                }
//                                
//                                y += voxelStepY;
//                            }
//                            glEnd();
//                        }
//                    }
//                        break;
//                }
//            }
//            else {
//                uint8_t rgb[3];
//                std::vector<float> idVoxelCoordinates;
//                int64_t idVoxelCounter = 0;
//                if (isSelect) {
//                    const int64_t bigDim = std::max(dimI, std::max(dimJ, dimK));
//                    idVoxelCoordinates.reserve(bigDim * 3);
//                }
//                
//                glBegin(GL_QUADS);
//                switch (slicePlane) {
//                    case VolumeSliceViewPlaneEnum::ALL:
//                        CaretAssert(0);
//                        break;
//                    case VolumeSliceViewPlaneEnum::AXIAL:
//                    {
//                        const float z1 = 0.0f;//sliceCoordinate;
//                        for (int64_t i = 0; i < dimI; i++) {
//                            const float x1 = minVoxelX + (voxelStepX * i);
//                            const float x2 = x1 + voxelStepX;
//                            for (int64_t j = 0; j < dimJ; j++) {
//                                const float y1 = minVoxelY + (voxelStepY * j);
//                                const float y2 = y1 + voxelStepY;
//                                if (isSelect) {
//                                    this->colorIdentification->addItem(rgb, 
//                                                                       SelectionItemDataTypeEnum::VOXEL, 
//                                                                       idVoxelCounter);
//                                    glColor3ubv(rgb);
//                                    
//                                    idVoxelCoordinates.push_back(x1 + halfVoxelStepX);
//                                    idVoxelCoordinates.push_back(y1 + halfVoxelStepY);
//                                    idVoxelCoordinates.push_back(sliceCoordinate); // coord of slice is not offset by half voxel
//                                    idVoxelCounter++;
//                                }
//                                else {
//                                    const int32_t sliceRgbaOffset = (i + (j * dimI)) * 4;
//                                    CaretAssertVectorIndex(sliceVoxelsRgbaVector, sliceRgbaOffset+3);
//                                    glColor4fv(&sliceVoxelsRGBA[sliceRgbaOffset]);
//                                }
//                                glVertex3f(x1, y1, z1);
//                                glVertex3f(x2, y1, z1);
//                                glVertex3f(x2, y2, z1);
//                                glVertex3f(x1, y2, z1);
//                            }
//                        }
//                    }
//                        break;
//                    case VolumeSliceViewPlaneEnum::CORONAL:
//                    {
//                        const float y1 = 0.0f;//sliceCoordinate;
//                        for (int64_t i = 0; i < dimI; i++) {
//                            const float x1 = minVoxelX + (voxelStepX * i);
//                            const float x2 = x1 + voxelStepX;
//                            for (int64_t k = 0; k < dimK; k++) {
//                                const float z1 = minVoxelZ + (voxelStepZ * k);
//                                const float z2 = z1 + voxelStepZ;
//                                if (isSelect) {
//                                    this->colorIdentification->addItem(rgb, 
//                                                                       SelectionItemDataTypeEnum::VOXEL, 
//                                                                       idVoxelCounter);
//                                    glColor3ubv(rgb);
//                                    
//                                    idVoxelCoordinates.push_back(x1 + halfVoxelStepX);
//                                    idVoxelCoordinates.push_back(sliceCoordinate); // coord of slice is not offset by half voxel
//                                    idVoxelCoordinates.push_back(z1 + halfVoxelStepZ);
//                                    idVoxelCounter++;
//                                }
//                                else {
//                                    const int32_t sliceRgbaOffset = (i + (k * dimI)) * 4;
//                                    CaretAssertVectorIndex(sliceVoxelsRgbaVector, sliceRgbaOffset+3);
//                                    glColor4fv(&sliceVoxelsRGBA[sliceRgbaOffset]);
//                                }
//                                glVertex3f(x1, y1, z1);
//                                glVertex3f(x2, y1, z1);
//                                glVertex3f(x2, y1, z2);
//                                glVertex3f(x1, y1, z2);
//                            }
//                        }
//                    }
//                        break;
//                    case VolumeSliceViewPlaneEnum::PARASAGITTAL:
//                    {
//                        const float x1 = 0.0f;//sliceCoordinate;
//                        for (int64_t j = 0; j < dimJ; j++) {
//                            const float y1 = minVoxelY + (voxelStepY * j);
//                            const float y2 = y1 + voxelStepY;
//                            for (int64_t k = 0; k < dimK; k++) {
//                                const float z1 = minVoxelZ + (voxelStepZ * k);
//                                const float z2 = z1 + voxelStepZ;
//                                if (isSelect) {
//                                    this->colorIdentification->addItem(rgb, 
//                                                                       SelectionItemDataTypeEnum::VOXEL, 
//                                                                       idVoxelCounter);
//                                    glColor3ubv(rgb);
//                                    
//                                    idVoxelCoordinates.push_back(sliceCoordinate); // coord of slice is not offset by half voxel
//                                    idVoxelCoordinates.push_back(y1 + halfVoxelStepY);
//                                    idVoxelCoordinates.push_back(z1 + halfVoxelStepZ);
//                                    idVoxelCounter++;
//                                }
//                                else {
//                                    const int32_t sliceRgbaOffset = (j + (k * dimJ)) * 4;
//                                    CaretAssertVectorIndex(sliceVoxelsRgbaVector, sliceRgbaOffset+3);
//                                    glColor4fv(&sliceVoxelsRGBA[sliceRgbaOffset]);
//                                }
//                                glVertex3f(x1, y1, z1);
//                                glVertex3f(x1, y2, z1);
//                                glVertex3f(x1, y2, z2);
//                                glVertex3f(x1, y1, z2);
//                            }
//                        }
//                    }
//                        break;
//                }
//                glEnd();
//                
//                /*
//                 * If selection enabled, find voxel that was selected.
//                 */
//                if (isSelect) {
//                    int32_t idIndex;
//                    float depth = -1.0;
//                    this->getIndexFromColorSelection(SelectionItemDataTypeEnum::VOXEL, 
//                                                     this->mouseX, 
//                                                     this->mouseY,
//                                                     idIndex,
//                                                     depth);
//                    if (idIndex >= 0) {
//                        float voxelCoordinates[3] = {
//                            idVoxelCoordinates[idIndex*3],
//                            idVoxelCoordinates[idIndex*3+1],
//                            idVoxelCoordinates[idIndex*3+2]
//                        };
//                        
//                        for (int32_t iVol = 0; iVol < numberOfVolumesToDraw; iVol++) {
//                            VolumeFile* vf = volumeDrawInfo[iVol].volumeFile;
//                            int64_t voxelIndices[3];
//                            vf->enclosingVoxel(voxelCoordinates,
//                                             voxelIndices);
//                            if (vf->indexValid(voxelIndices)) {
//                                if (voxelID->isOtherScreenDepthCloserToViewer(depth)) {
//                                    voxelID->setBrain(volumeDrawInfo[iVol].brain);
//                                    voxelID->setVolumeFile(volumeDrawInfo[iVol].volumeFile);
//                                    voxelID->setVoxelIJK(voxelIndices);
//                                    voxelID->setScreenDepth(depth);
//                                    this->setSelectedItemScreenXYZ(voxelID, voxelCoordinates);
//                                    CaretLogFine("Selected Voxel: " + AString::fromNumbers(voxelIndices, 3, ","));  
//                                    break;
//                                }
//                            }
//                        }
//                    }
//                }
//            }
//        }
//    }
//    
//    glEnable(GL_CULL_FACE);
//    glShadeModel(GL_SMOOTH);
////    glEnable(GL_DEPTH_TEST);
//    
//    glDisable(GL_BLEND);
//}


/**
 * Draw a single volume orthogonal slice.
 * @param slicePlane
 *    Plane that is drawn
 * @param sliceIndex
 *    Index of slice in plane.
 * @param volumeDrawInfo
 *    Describes volumes that are drawn.
 */
void 
BrainOpenGLFixedPipeline::drawVolumeOrthogonalSlice(const VolumeSliceViewPlaneEnum::Enum slicePlane,
                                                    const int64_t sliceIndex,
                                                    std::vector<VolumeDrawInfo>& volumeDrawInfo)
{
    const int32_t numberOfVolumesToDraw = static_cast<int32_t>(volumeDrawInfo.size());
    
    SelectionItemVoxel* voxelID = 
    m_brain->getSelectionManager()->getVoxelIdentification();

    /*
     * Check for a 'selection' type mode
     */
    bool isSelect = false;
    switch (this->mode) {
        case MODE_DRAWING:
            break;
        case MODE_IDENTIFICATION:
            if (voxelID->isEnabledForSelection()) {
                isSelect = true;
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);            
            }
            else {
                return;
            }
            break;
        case MODE_PROJECTION:
            return;
            break;
    }
    
    
    /*
     * For slices disable culling since want to see both side
     * and set shading to flat so there is no interpolation of
     * colors within a voxel drawn as a quad.  This allows
     * drawing of voxels using quad strips.
     */
    this->disableLighting();
    glDisable(GL_CULL_FACE);
    glShadeModel(GL_FLAT);
    
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
        const VolumeFile* volumeFile = volumeDrawInfo[i].volumeFile;
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
        
        if (i == 0) {
            switch (slicePlane) {
                case VolumeSliceViewPlaneEnum::ALL:
                    CaretAssert(0);
                    
                    break;
                case VolumeSliceViewPlaneEnum::AXIAL:
                    sliceCoordinate = originZ + dz * sliceIndex;
                    break;
                case VolumeSliceViewPlaneEnum::CORONAL:
                    sliceCoordinate = originY + dy * sliceIndex;
                    break;
                case VolumeSliceViewPlaneEnum::PARASAGITTAL:
                    sliceCoordinate = originX + dx * sliceIndex;
                    break;
            }
        }
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
    
    /*
     * For display slice axis, do only one slice (numVoxels[XYZ]) and set the coordinate
     * of the first voxel in the viewed plane to the coordinate of the slice.
     */
    switch (slicePlane) {
        case VolumeSliceViewPlaneEnum::ALL:
            CaretAssert(0);
            break;
        case VolumeSliceViewPlaneEnum::AXIAL:
            numVoxelsZ = 1;
            minVoxelZ  = sliceCoordinate;
            break;
        case VolumeSliceViewPlaneEnum::CORONAL:
            numVoxelsY = 1;
            minVoxelY  = sliceCoordinate;
            break;
        case VolumeSliceViewPlaneEnum::PARASAGITTAL:
            numVoxelsX = 1;
            minVoxelX  = sliceCoordinate;
            break;
    }
    
    /*
     * Set colors for each drawn voxel.
     * Use a vector for colors so no worries about memory being freed.
     */
    const int64_t numVoxels = numVoxelsX * numVoxelsY * numVoxelsZ;
    std::vector<uint8_t> sliceRgbaVector(numVoxels * 4);
    uint8_t* sliceRGBA = &sliceRgbaVector[0];
    
    for (int64_t i = 0; i < numVoxelsX; i++) {
        for (int64_t j = 0; j < numVoxelsY; j++) {
            for (int64_t k = 0; k < numVoxelsZ; k++) {
                const float x = minVoxelX + i * voxelStepX;
                const float y = minVoxelY + j * voxelStepY;
                const float z = minVoxelZ + k * voxelStepZ;
                int64_t sliceRgbaOffset = -1;
                switch (slicePlane) {
                    case VolumeSliceViewPlaneEnum::ALL:
                        CaretAssert(0);
                        break;
                    case VolumeSliceViewPlaneEnum::AXIAL:
                        sliceRgbaOffset = (i + (j * numVoxelsX)) * 4;
                        break;
                    case VolumeSliceViewPlaneEnum::CORONAL:
                        sliceRgbaOffset = (i + (k * numVoxelsX)) * 4;
                        break;
                    case VolumeSliceViewPlaneEnum::PARASAGITTAL:
                        sliceRgbaOffset = (j + (k * numVoxelsY)) * 4;
                        break;
                }
                
                CaretAssertVectorIndex(sliceRgbaVector, sliceRgbaOffset+3);
                sliceRGBA[sliceRgbaOffset]   = 0;
                sliceRGBA[sliceRgbaOffset+1] = 0;
                sliceRGBA[sliceRgbaOffset+2] = 0;
                sliceRGBA[sliceRgbaOffset+3] = 255;
                
                for (int32_t iVol = 0; iVol < numberOfVolumesToDraw; iVol++) {
                    VolumeDrawInfo& volInfo = volumeDrawInfo[iVol];
                    VolumeFile* vf = volInfo.volumeFile;
                    const int64_t mapIndex = volInfo.mapIndex;
                    bool valid = false;
//                    float voxel = 0;
                        int64_t iVoxel, jVoxel, kVoxel;
                        vf->enclosingVoxel(x, y, z, iVoxel, jVoxel, kVoxel);
                        if (vf->indexValid(iVoxel, jVoxel, kVoxel, mapIndex)) {
//                            voxel = vf->getValue(iVoxel, jVoxel, kVoxel, mapIndex);
                            valid = true;
                        }
                    
                    if (valid) {
                        uint8_t rgba[4];
                        vf->getVoxelColorInMap(iVoxel,
                                               jVoxel,
                                               kVoxel,
                                               mapIndex,
                                               rgba);
//                        this->colorizeVoxels(volInfo,
//                                             &voxel,
//                                             &voxel,
//                                             1,
//                                             rgba,
//                                             false);
//                        NodeAndVoxelColoring::colorScalarsWithPalette(volInfo.statistics,
//                                                                      volInfo.paletteColorMapping,
//                                                                      volInfo.palette,
//                                                                      &voxel,
//                                                                      &voxel,
//                                                                      1,
//                                                                      rgba);
                        if (rgba[3] > 0) {
                            bool useOpacity = false;
                            if (iVol > 0) {
                                if (volInfo.opacity < 1.0) {
                                    useOpacity = true;
                                }
                            }
                            if (useOpacity) {
                                const float oneMinuseOpacity = 1.0 - volInfo.opacity;
                                sliceRGBA[sliceRgbaOffset]   = ((rgba[0] * volInfo.opacity)
                                                                + (sliceRGBA[sliceRgbaOffset] * oneMinuseOpacity));
                                sliceRGBA[sliceRgbaOffset+1] = ((rgba[1] * volInfo.opacity)
                                                                + (sliceRGBA[sliceRgbaOffset+1] * oneMinuseOpacity));
                                sliceRGBA[sliceRgbaOffset+2] = ((rgba[2] * volInfo.opacity)
                                                                + (sliceRGBA[sliceRgbaOffset+2] * oneMinuseOpacity));
                                sliceRGBA[sliceRgbaOffset+3] = 255;
                            }
                            else {
                                sliceRGBA[sliceRgbaOffset]   = rgba[0];
                                sliceRGBA[sliceRgbaOffset+1] = rgba[1];
                                sliceRGBA[sliceRgbaOffset+2] = rgba[2];
                                sliceRGBA[sliceRgbaOffset+3] = 255;
                            }
                        }
                        
                    }
                }
            } 
        }
    }

    /*
     * The voxel coordinates are at the center of the voxel.  
     * Shift the minimum voxel coordinates by one-half the 
     * size of a voxel so that the rectangles depicting the 
     * voxels are drawn with the center of the voxel at the
     * center of the rectangle.
     */
    const float halfVoxelStepX = voxelStepX * 0.5;
    const float halfVoxelStepY = voxelStepY * 0.5;
    const float halfVoxelStepZ = voxelStepZ * 0.5;
    minVoxelX -= halfVoxelStepX;
    minVoxelY -= halfVoxelStepY;
    minVoxelZ -= halfVoxelStepZ;
    
    bool useQuadStrips = true;
    if (isSelect) {
        useQuadStrips = false;
    }
    if (useQuadStrips) {
        /*
         * Note on quad strips:
         *
         * Each quad receives the color specified at the vertex
         * 2i +2 (for i = 1..N).
         *
         * So, the color used to draw a quad is the color that
         * is specified at vertex 3, 5, 7,.. with the first
         * vertex being 1.
         */
        
        switch (slicePlane) {
            case VolumeSliceViewPlaneEnum::ALL:
                CaretAssert(0);
                break;
            case VolumeSliceViewPlaneEnum::AXIAL:
            {
                    const float z = sliceCoordinate;
                    float x = minVoxelX;
                    for (int64_t i = 0; i < numVoxelsX; i++) {
                        glBegin(GL_QUAD_STRIP);
                        {
                            const float x2 = x + voxelStepX;
                            float y = minVoxelY;
                            
                            glVertex3f(x, y, z);
                            glVertex3f(x2, y, z);
                            
                            for (int64_t j = 0; j < numVoxelsY; j++) {
                                const int32_t sliceRgbaOffset = (i + (j * numVoxelsX)) * 4;
                                CaretAssertVectorIndex(sliceRgbaVector, sliceRgbaOffset+3);
                                glColor4ubv(&sliceRGBA[sliceRgbaOffset]);
                                
                                y += voxelStepY;
                                glVertex3f(x, y, z);
                                glVertex3f(x2, y, z);
                            }
                            
                            x += voxelStepX;
                        }
                        glEnd();
                    }
            }
                break;
            case VolumeSliceViewPlaneEnum::CORONAL:
            {
                    const float y = sliceCoordinate;
                    float x = minVoxelX;
                    for (int64_t i = 0; i < numVoxelsX; i++) {
                        glBegin(GL_QUAD_STRIP);
                        {
                            const float x2 = x + voxelStepX;
                            float z = minVoxelZ;
                            
                            glVertex3f(x, y, z);
                            glVertex3f(x2, y, z);
                            
                            for (int64_t k = 0; k < numVoxelsZ; k++) {
                                const int32_t sliceRgbaOffset = (i + (k * numVoxelsX)) * 4;
                                CaretAssertVectorIndex(sliceRgbaVector, sliceRgbaOffset+3);
                                glColor4ubv(&sliceRGBA[sliceRgbaOffset]);
                                
                                z += voxelStepZ;
                                glVertex3f(x, y, z);
                                glVertex3f(x2, y, z);
                            }
                            
                            x += voxelStepX;
                        }
                        glEnd();
                    }
            }
                break;
            case VolumeSliceViewPlaneEnum::PARASAGITTAL:
            {
                    const float x = sliceCoordinate;
                    float y = minVoxelY;
                    for (int64_t j = 0; j < numVoxelsY; j++) {
                        glBegin(GL_QUAD_STRIP);
                        {
                            const float y2 = y + voxelStepY;
                            float z = minVoxelZ;
                            
                            glVertex3f(x, y, z);
                            glVertex3f(x, y2, z);
                            
                            for (int64_t k = 0; k < numVoxelsZ; k++) {
                                const int32_t sliceRgbaOffset = (j + (k * numVoxelsY)) * 4;
                                CaretAssertVectorIndex(sliceRgbaVector, sliceRgbaOffset+3);
                                glColor4ubv(&sliceRGBA[sliceRgbaOffset]);
                                
                                z += voxelStepZ;
                                glVertex3f(x, y, z);
                                glVertex3f(x, y2, z);
                            }
                            
                            y += voxelStepY;
                        }
                        glEnd();
                    }
            }
                break;
        }
    }
    else {
        uint8_t rgb[3];
        std::vector<float> idVoxelCoordinates;
        int64_t idVoxelCounter = 0;
        if (isSelect) {
            const int64_t bigDim = std::max(numVoxelsX, std::max(numVoxelsY, numVoxelsZ));
            idVoxelCoordinates.reserve(bigDim * 3);
        }
        
        glBegin(GL_QUADS);
        switch (slicePlane) {
            case VolumeSliceViewPlaneEnum::ALL:
                CaretAssert(0);
                break;
            case VolumeSliceViewPlaneEnum::AXIAL:
            {
                    const float z1 = sliceCoordinate;
                    for (int64_t i = 0; i < numVoxelsX; i++) {
                        const float x1 = minVoxelX + (voxelStepX * i);
                        const float x2 = x1 + voxelStepX;
                        for (int64_t j = 0; j < numVoxelsY; j++) {
                            const float y1 = minVoxelY + (voxelStepY * j);
                            const float y2 = y1 + voxelStepY;
                            if (isSelect) {
                                this->colorIdentification->addItem(rgb, 
                                                                   SelectionItemDataTypeEnum::VOXEL, 
                                                                   idVoxelCounter);
                                glColor3ubv(rgb);
                                
                                idVoxelCoordinates.push_back(x1 + halfVoxelStepX);
                                idVoxelCoordinates.push_back(y1 + halfVoxelStepY);
                                idVoxelCoordinates.push_back(z1); // coord of slice is not offset by half voxel
                                idVoxelCounter++;
                            }
                            else {
                                const int32_t sliceRgbaOffset = (i + (j * numVoxelsX)) * 4;
                                CaretAssertVectorIndex(sliceRgbaVector, sliceRgbaOffset+3);
                                glColor4ubv(&sliceRGBA[sliceRgbaOffset]);
                            }
                            glVertex3f(x1, y1, z1);
                            glVertex3f(x2, y1, z1);
                            glVertex3f(x2, y2, z1);
                            glVertex3f(x1, y2, z1);
                        }
                    }
            }
                break;
            case VolumeSliceViewPlaneEnum::CORONAL:
            {
                    const float y1 = sliceCoordinate;
                    for (int64_t i = 0; i < numVoxelsX; i++) {
                        const float x1 = minVoxelX + (voxelStepX * i);
                        const float x2 = x1 + voxelStepX;
                        for (int64_t k = 0; k < numVoxelsZ; k++) {
                            const float z1 = minVoxelZ + (voxelStepZ * k);
                            const float z2 = z1 + voxelStepZ;
                            if (isSelect) {
                                this->colorIdentification->addItem(rgb, 
                                                                   SelectionItemDataTypeEnum::VOXEL, 
                                                                   idVoxelCounter);
                                glColor3ubv(rgb);
                                
                                idVoxelCoordinates.push_back(x1 + halfVoxelStepX);
                                idVoxelCoordinates.push_back(y1); // coord of slice is not offset by half voxel
                                idVoxelCoordinates.push_back(z1 + halfVoxelStepZ);
                                idVoxelCounter++;
                            }
                            else {
                                const int32_t sliceRgbaOffset = (i + (k * numVoxelsX)) * 4;
                                CaretAssertVectorIndex(sliceRgbaVector, sliceRgbaOffset+3);
                                glColor4ubv(&sliceRGBA[sliceRgbaOffset]);
                            }
                            glVertex3f(x1, y1, z1);
                            glVertex3f(x2, y1, z1);
                            glVertex3f(x2, y1, z2);
                            glVertex3f(x1, y1, z2);
                        }
                    }
            }
                break;
            case VolumeSliceViewPlaneEnum::PARASAGITTAL:
            {
                    const float x1 = sliceCoordinate;
                    for (int64_t j = 0; j < numVoxelsY; j++) {
                        const float y1 = minVoxelY + (voxelStepY * j);
                        const float y2 = y1 + voxelStepY;
                        for (int64_t k = 0; k < numVoxelsZ; k++) {
                            const float z1 = minVoxelZ + (voxelStepZ * k);
                            const float z2 = z1 + voxelStepZ;
                            if (isSelect) {
                                this->colorIdentification->addItem(rgb, 
                                                                   SelectionItemDataTypeEnum::VOXEL, 
                                                                   idVoxelCounter);
                                glColor3ubv(rgb);
                                
                                idVoxelCoordinates.push_back(x1); // coord of slice is not offset by half voxel
                                idVoxelCoordinates.push_back(y1 + halfVoxelStepY);
                                idVoxelCoordinates.push_back(z1 + halfVoxelStepZ);
                                idVoxelCounter++;
                            }
                            else {
                                const int32_t sliceRgbaOffset = (j + (k * numVoxelsY)) * 4;
                                CaretAssertVectorIndex(sliceRgbaVector, sliceRgbaOffset+3);
                                glColor4ubv(&sliceRGBA[sliceRgbaOffset]);
                            }
                            glVertex3f(x1, y1, z1);
                            glVertex3f(x1, y2, z1);
                            glVertex3f(x1, y2, z2);
                            glVertex3f(x1, y1, z2);
                        }
                    }
            }
                break;
        }
        glEnd();
        
        /*
         * If selection enabled, find voxel that was selected.
         */
        if (isSelect) {
            int32_t idIndex;
            float depth = -1.0;
            this->getIndexFromColorSelection(SelectionItemDataTypeEnum::VOXEL, 
                                             this->mouseX, 
                                             this->mouseY,
                                             idIndex,
                                             depth);
            if (idIndex >= 0) {
                float voxelCoordinates[3] = {
                    idVoxelCoordinates[idIndex*3],
                    idVoxelCoordinates[idIndex*3+1],
                    idVoxelCoordinates[idIndex*3+2]
                };
                
                for (int32_t iVol = 0; iVol < numberOfVolumesToDraw; iVol++) {
                    VolumeFile* vf = volumeDrawInfo[iVol].volumeFile;
                    int64_t voxelIndices[3];
                    vf->enclosingVoxel(voxelCoordinates,
                                     voxelIndices);
                    if (vf->indexValid(voxelIndices)) {
                        if (voxelID->isOtherScreenDepthCloserToViewer(depth)) {
                            voxelID->setVolumeFile(volumeDrawInfo[iVol].volumeFile);
                            voxelID->setVoxelIJK(voxelIndices);
                            voxelID->setScreenDepth(depth);
                            this->setSelectedItemScreenXYZ(voxelID, voxelCoordinates);
                            CaretLogFine("Selected Voxel: " + AString::fromNumbers(voxelIndices, 3, ","));  
                            break;
                        }
                    }
                }
            }
        }
        
    }
    
    glEnable(GL_CULL_FACE);
    glShadeModel(GL_SMOOTH);
}

///**
// * Apply coloring to voxels.
// *
// * @param volumeDrawInfo
// *    Info about volume being drawn.
// * @param scalarValues
// *    Scalar values that are used to assign colors.
// * @param thresholdValues
// *    Scalar values that are used for thresholding.
// * @param numberOfScalars
// *    Number of scalars.
// * @param rgbaOut
// *    Output containing RGBA colors.
// * @param ignoreThresholding
// *    If true, thresolding is ignored.
// */
//void 
//BrainOpenGLFixedPipeline::colorizeVoxels(const VolumeDrawInfo& volumeDrawInfo,
//                                         const float* scalarValues,
//                                         const float* thresholdValues,
//                                         const int32_t numberOfScalars,
//                                         float* rgbaOut,
//                                         const bool ignoreThresholding)
//{
//    bool clearColorsFlag = false;
//    
//    const VolumeFile* vf = volumeDrawInfo.volumeFile;
//    
//    switch (vf->getType()) {
//        case SubvolumeAttributes::UNKNOWN:
//        case SubvolumeAttributes::ANATOMY:
//        case SubvolumeAttributes::FUNCTIONAL:
//            NodeAndVoxelColoring::colorScalarsWithPalette(volumeDrawInfo.statistics,
//                                                          volumeDrawInfo.paletteColorMapping,
//                                                          volumeDrawInfo.palette,
//                                                          scalarValues,
//                                                          thresholdValues,
//                                                          numberOfScalars,
//                                                          rgbaOut,
//                                                          ignoreThresholding);
//            break;
//        case SubvolumeAttributes::LABEL:
//            if (numberOfScalars > 0) {
//                std::vector<int32_t> labelIndices(numberOfScalars);
//                for (int32_t i = 0; i < numberOfScalars; i++) {
//                    labelIndices[i] = static_cast<int32_t>(scalarValues[i]);
//                }
//                
//               NodeAndVoxelColoring::colorIndicesWithLabelTable(vf->getMapLabelTable(volumeDrawInfo.mapIndex), 
//                                                                 &labelIndices[0], 
//                                                                 numberOfScalars, 
//                                                                 rgbaOut);
//            }
//            break;
//        case SubvolumeAttributes::RGB:
//            clearColorsFlag = true;
//            break;
//        case SubvolumeAttributes::SEGMENTATION:
//            clearColorsFlag = true;
//            break;
//        case SubvolumeAttributes::VECTOR:
//            clearColorsFlag = true;
//            break;
//    }
//    
//    if (clearColorsFlag) {
//        for (int32_t i = 0; i < numberOfScalars; i++) {
//            const int32_t i4 = i * 4;
//            rgbaOut[i4]   = 0.0;
//            rgbaOut[i4+1] = 0.0;
//            rgbaOut[i4+2] = 0.0;
//            rgbaOut[i4+3] = 0.0;
//        }
//    }
//    
//    /*
//     NodeAndVoxelColoring::colorScalarsWithPalette(volInfo.statistics,
//     volInfo.paletteColorMapping,
//     volInfo.palette,
//     &voxel,
//     &voxel,
//     1,
//     rgba);
//     */
//}


/**
 * Draw surface outlines on volume slices.
 *
 * @param brain
 *    The brain.
 * @param modelDisplayController
 *    Model display controller in which surface outlines are drawn.
 * @param browserTabContent
 *    Tab content that is being drawn.
 * @param slicePlane
 *    Plane on which surface outlines are drawn.
 * @param sliceIndex
 *    Index of slice.
 * @param underlayVolume
 *    Bottom-most displayed volume.
 */
void 
BrainOpenGLFixedPipeline::drawVolumeSurfaceOutlines(Brain* /*brain*/,
                                                    Model* /*modelDisplayController*/,
                                                    BrowserTabContent* browserTabContent,
                                                    const VolumeSliceViewPlaneEnum::Enum slicePlane,
                                                    const int64_t sliceIndex,
                                                    VolumeFile* underlayVolume)
{
 //   CaretAssert(brain);
    CaretAssert(underlayVolume);
    
    std::vector<int64_t> dim;
    underlayVolume->getDimensions(dim);
    
    /*
     * Find three points on the slice so that the equation for a Plane
     * can be formed.
     */
    float p1[3];
    float p2[3];
    float p3[3];
    switch(slicePlane) {
        case VolumeSliceViewPlaneEnum::ALL:
            return;
            break;
        case VolumeSliceViewPlaneEnum::PARASAGITTAL:
        {
            underlayVolume->indexToSpace(sliceIndex, 0, 0, p1);
            underlayVolume->indexToSpace(sliceIndex, dim[1] - 1, 0, p2);
            underlayVolume->indexToSpace(sliceIndex, dim[1] - 1, dim[2] - 1, p3);
        }
            break;
        case VolumeSliceViewPlaneEnum::CORONAL:
        {
            underlayVolume->indexToSpace(0, sliceIndex, 0, p1);
            underlayVolume->indexToSpace(dim[0] - 1, sliceIndex, 0, p2);
            underlayVolume->indexToSpace(dim[0] - 1, sliceIndex, dim[2] - 1, p3);
        }
            break;
        case VolumeSliceViewPlaneEnum::AXIAL:
        {
            underlayVolume->indexToSpace(0, 0, sliceIndex, p1);
            underlayVolume->indexToSpace(dim[0] - 1, 0, sliceIndex, p2);
            underlayVolume->indexToSpace(dim[0] - 1, dim[1] - 1, sliceIndex, p3);
        }
            break;
    }
    
    Plane plane(p1, p2, p3);
    if (plane.isValidPlane() == false) {
        return;
    }
    
    float intersectionPoint1[3];
    float intersectionPoint2[3];
    
    this->enableLineAntiAliasing();
    
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
                const float lineWidth = this->modelSizeToPixelSize(thickness);
                
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
                    nodeColoringRGBA = this->surfaceNodeColoring->colorSurfaceNodes(NULL, /*modelDisplayController*/
                                                                                    surface, 
                                                                                    colorSourceBrowserTabIndex);
                }
                
                glColor3fv(CaretColorEnum::toRGB(outlineColor));
                this->setLineWidth(lineWidth);
                
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
                        switch(slicePlane) {
                            case VolumeSliceViewPlaneEnum::ALL:
                                break;
                            case VolumeSliceViewPlaneEnum::PARASAGITTAL:
                                glVertex3f(p1[0], intersectionPoint1[1], intersectionPoint1[2]);
                                glVertex3f(p1[0], intersectionPoint2[1], intersectionPoint2[2]);
                                break;
                            case VolumeSliceViewPlaneEnum::CORONAL:
                                glVertex3f(intersectionPoint1[0], p1[1], intersectionPoint1[2]);
                                glVertex3f(intersectionPoint2[0], p1[1], intersectionPoint2[2]);
                                break;
                            case VolumeSliceViewPlaneEnum::AXIAL:
                                glVertex3f(intersectionPoint1[0], intersectionPoint1[1], p1[2]);
                                glVertex3f(intersectionPoint2[0], intersectionPoint2[1], p1[2]);
                                break;
                        }
                    }
                }
                glEnd();
            }
        }
    }
    
    this->disableLineAntiAliasing();
}

/**
 * Draw surface outlines on volume slices.
 *
 * @param brain
 *    The brain.
 * @param modelDisplayController
 *    Model display controller in which surface outlines are drawn.
 * @param browserTabContent
 *    Tab content that is being drawn.
 * @param slicePlane
 *    Plane on which surface outlines are drawn.
 * @param sliceIndex
 *    Index of slice.
 * @param underlayVolume
 *    Bottom-most displayed volume.
 */
void
BrainOpenGLFixedPipeline::drawVolumeFoci(Brain* brain,
                                         ModelVolume* /*modelVolume*/,
                                         BrowserTabContent* /*browserTabContent*/,
                                         const VolumeSliceViewPlaneEnum::Enum slicePlane,
                                         const int64_t sliceIndex,
                                         VolumeFile* underlayVolume)
{
    CaretAssert(brain);
    CaretAssert(underlayVolume);
    
    SelectionItemFocusVolume* idFocus = m_brain->getSelectionManager()->getVolumeFocusIdentification();
    
    /*
     * Check for a 'selection' type mode
     */
    bool isSelect = false;
    switch (this->mode) {
        case MODE_DRAWING:
            break;
        case MODE_IDENTIFICATION:
            if (idFocus->isEnabledForSelection()) {
                isSelect = true;
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            }
            else {
                return;
            }
            break;
        case MODE_PROJECTION:
            return;
            break;
    }
    
    std::vector<int64_t> dim;
    underlayVolume->getDimensions(dim);
    
    /*
     * Slice thicknesses
     */
    float sliceXYZ[3];
    float sliceNextXYZ[3];
    int64_t sliceIJK[3] = { 0, 0, 0 };
    int64_t sliceNextIJK[3] = { 1, 1, 1 };
    underlayVolume->indexToSpace(sliceIJK, sliceXYZ);
    underlayVolume->indexToSpace(sliceNextIJK, sliceNextXYZ);
    const float sliceThicknesses[3] = {
        sliceNextXYZ[0] - sliceXYZ[0],
        sliceNextXYZ[1] - sliceXYZ[1],
        sliceNextXYZ[2] - sliceXYZ[2]
    };
    
    /*
     * Find three points on the slice so that the equation for a Plane
     * can be formed.
     */
    float p1[3];
    float p2[3];
    float p3[3];
    float sliceThickness = 0.0;
    switch(slicePlane) {
        case VolumeSliceViewPlaneEnum::ALL:
            return;
            break;
        case VolumeSliceViewPlaneEnum::PARASAGITTAL:
        {
            underlayVolume->indexToSpace(sliceIndex, 0, 0, p1);
            underlayVolume->indexToSpace(sliceIndex, dim[1] - 1, 0, p2);
            underlayVolume->indexToSpace(sliceIndex, dim[1] - 1, dim[2] - 1, p3);
            sliceThickness = sliceThicknesses[0];
        }
            break;
        case VolumeSliceViewPlaneEnum::CORONAL:
        {
            underlayVolume->indexToSpace(0, sliceIndex, 0, p1);
            underlayVolume->indexToSpace(dim[0] - 1, sliceIndex, 0, p2);
            underlayVolume->indexToSpace(dim[0] - 1, sliceIndex, dim[2] - 1, p3);
            sliceThickness = sliceThicknesses[1];
        }
            break;
        case VolumeSliceViewPlaneEnum::AXIAL:
        {
            underlayVolume->indexToSpace(0, 0, sliceIndex, p1);
            underlayVolume->indexToSpace(dim[0] - 1, 0, sliceIndex, p2);
            underlayVolume->indexToSpace(dim[0] - 1, dim[1] - 1, sliceIndex, p3);
            sliceThickness = sliceThicknesses[2];
        }
            break;
    }
    const float halfSliceThickness = sliceThickness * 0.5;
    
    Plane plane(p1, p2, p3);
    if (plane.isValidPlane() == false) {
        return;
    }
    
    
    const DisplayPropertiesFoci* fociDisplayProperties = brain->getDisplayPropertiesFoci();
    const DisplayGroupEnum::Enum displayGroup = fociDisplayProperties->getDisplayGroupForTab(this->windowTabIndex);
    
    if (fociDisplayProperties->isDisplayed(displayGroup,
                                           this->windowTabIndex) == false) {
        return;
    }
    const float focusRadius = fociDisplayProperties->getFociSize(displayGroup,
                                                                 this->windowTabIndex) / 2.0;
    const FeatureColoringTypeEnum::Enum fociColoringType = fociDisplayProperties->getColoringType(displayGroup,
                                                                                               this->windowTabIndex);
    
    bool drawAsSpheres = false;
    switch (fociDisplayProperties->getDrawingType(displayGroup,
                                                  this->windowTabIndex)) {
        case FociDrawingTypeEnum::DRAW_AS_SPHERES:
            drawAsSpheres = true;
            break;
        case FociDrawingTypeEnum::DRAW_AS_SQUARES:
            break;
    }
    
    /*
     * Process each foci file
     */
    const int32_t numberOfFociFiles = brain->getNumberOfFociFiles();
    for (int32_t iFile = 0; iFile < numberOfFociFiles; iFile++) {
        FociFile* fociFile = brain->getFociFile(iFile);
    
    const GroupAndNameHierarchyModel* classAndNameSelection = fociFile->getGroupAndNameHierarchyModel();
    if (classAndNameSelection->isSelected(displayGroup,
                                          this->windowTabIndex) == false) {
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
                                              this->windowTabIndex) == false) {
                    continue;
                }
            }
//            const int32_t selectionClassKey = focus->getSelectionClassKey();
//            const int32_t selectionNameKey  = focus->getSelectionNameKey();
//            if (classAndNameSelection->isGroupSelected(displayGroup,
//                                                       this->windowTabIndex,
//                                                       selectionClassKey) == false) {
//                continue;
//            }
//            if (classAndNameSelection->isNameSelected(displayGroup,
//                                                      this->windowTabIndex,
//                                                      selectionClassKey,
//                                                      selectionNameKey) == false) {
//                continue;
//            }
            
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
                case FeatureColoringTypeEnum::FEATURE_COLORING_TYPE_COLOR_LIST:
                {
                    const float* colorRGBA = CaretColorEnum::toRGB(focus->getColor());
                    rgba[0] = colorRGBA[0];
                    rgba[1] = colorRGBA[1];
                    rgba[2] = colorRGBA[2];
                    rgba[3] = colorRGBA[3];
                }
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
                            this->colorIdentification->addItem(idRGB,
                                                               SelectionItemDataTypeEnum::FOCUS_VOLUME,
                                                               iFile, // file index
                                                               j, // focus index
                                                               k);// projection index
                            glColor3ubv(idRGB);
                        }
                        
                        glPushMatrix();
                        glTranslatef(xyz[0], xyz[1], xyz[2]);
                        if (drawAsSpheres) {
                            this->drawSphere(focusRadius);
                        }
                        else {
                            this->drawSquare(focusRadius);
                        }
                        glPopMatrix();
                    }
                }
            }
        }
    }
    
    if (isSelect) {
        int32_t fociFileIndex = -1;
        int32_t focusIndex = -1;
        int32_t focusProjectionIndex = -1;
        float depth = -1.0;
        this->getIndexFromColorSelection(SelectionItemDataTypeEnum::FOCUS_VOLUME,
                                         this->mouseX,
                                         this->mouseY,
                                         fociFileIndex,
                                         focusIndex,
                                         focusProjectionIndex,
                                         depth);
        if (fociFileIndex >= 0) {
            if (idFocus->isOtherScreenDepthCloserToViewer(depth)) {
                Focus* focus = brain->getFociFile(fociFileIndex)->getFocus(focusIndex);
                idFocus->setBrain(brain);
                idFocus->setFocus(focus);
                idFocus->setFociFile(brain->getFociFile(fociFileIndex));
                idFocus->setFocusIndex(focusIndex);
                idFocus->setFocusProjectionIndex(focusProjectionIndex);
                idFocus->setVolumeFile(underlayVolume);
                idFocus->setScreenDepth(depth);
                float xyz[3];
                const SurfaceProjectedItem* spi = focus->getProjection(focusProjectionIndex);
                spi->getVolumeXYZ(xyz);
                this->setSelectedItemScreenXYZ(idFocus, xyz);
                CaretLogFine("Selected Volume Focus Identification Symbol: " + QString::number(focusIndex));
            }
        }
    }
}

/**
 * Draw fiber orientations on volume slices.
 *
 * @param brain
 *    The brain.
 * @param modelDisplayController
 *    Model display controller in which surface outlines are drawn.
 * @param browserTabContent
 *    Tab content that is being drawn.
 * @param slicePlane
 *    Plane on which surface outlines are drawn.
 * @param sliceIndex
 *    Index of slice.
 * @param underlayVolume
 *    Bottom-most displayed volume.
 */
void
BrainOpenGLFixedPipeline::drawVolumeFibers(Brain* /*brain*/,
                                           ModelVolume* /*modelVolume*/,
                                           BrowserTabContent* /*browserTabContent*/,
                                           const VolumeSliceViewPlaneEnum::Enum slicePlane,
                                           const int64_t sliceIndex,
                                           VolumeFile* underlayVolume)
{
    std::vector<int64_t> dim;
    underlayVolume->getDimensions(dim);
    
    /*
     * Slice thicknesses
     */
    /*float sliceXYZ[3];
    float sliceNextXYZ[3];
    int64_t sliceIJK[3] = { 0, 0, 0 };
    int64_t sliceNextIJK[3] = { 1, 1, 1 };
    underlayVolume->indexToSpace(sliceIJK, sliceXYZ);
    underlayVolume->indexToSpace(sliceNextIJK, sliceNextXYZ);
    const float sliceThicknesses[3] = {
        sliceNextXYZ[0] - sliceXYZ[0],
        sliceNextXYZ[1] - sliceXYZ[1],
        sliceNextXYZ[2] - sliceXYZ[2]
    };//*///these should not be computed from the underlay volume, commenting out to prevent compiler warning
    
    /*
     * Find three points on the slice so that the equation for a Plane
     * can be formed.
     */
    float p1[3];
    float p2[3];
    float p3[3];
    //float sliceThickness = 0.0;
    switch(slicePlane) {
        case VolumeSliceViewPlaneEnum::ALL:
            return;
            break;
        case VolumeSliceViewPlaneEnum::PARASAGITTAL:
        {
            underlayVolume->indexToSpace(sliceIndex, 0, 0, p1);
            underlayVolume->indexToSpace(sliceIndex, dim[1] - 1, 0, p2);
            underlayVolume->indexToSpace(sliceIndex, dim[1] - 1, dim[2] - 1, p3);
            //sliceThickness = sliceThicknesses[0];
        }
            break;
        case VolumeSliceViewPlaneEnum::CORONAL:
        {
            underlayVolume->indexToSpace(0, sliceIndex, 0, p1);
            underlayVolume->indexToSpace(dim[0] - 1, sliceIndex, 0, p2);
            underlayVolume->indexToSpace(dim[0] - 1, sliceIndex, dim[2] - 1, p3);
            //sliceThickness = sliceThicknesses[1];
        }
            break;
        case VolumeSliceViewPlaneEnum::AXIAL:
        {
            underlayVolume->indexToSpace(0, 0, sliceIndex, p1);
            underlayVolume->indexToSpace(dim[0] - 1, 0, sliceIndex, p2);
            underlayVolume->indexToSpace(dim[0] - 1, dim[1] - 1, sliceIndex, p3);
            //sliceThickness = sliceThicknesses[2];
        }
            break;
    }
    
    Plane plane(p1, p2, p3);
    if (plane.isValidPlane() == false) {
        return;
    }

    drawFiberOrientations(&plane);
    drawFiberTrajectories(&plane);
    disableLighting();
}

void
BrainOpenGLFixedPipeline::setFiberOrientationDisplayInfo(const DisplayPropertiesFiberOrientation* dpfo,
                                                         const DisplayGroupEnum::Enum displayGroup,
                                                         const int32_t tabIndex,
                                                         BoundingBox* boundingBox,
                                                         Plane* plane,
                                    FiberOrientationDisplayInfo& dispInfo)
{
    dispInfo.aboveLimit = dpfo->getAboveLimit(displayGroup, tabIndex);
    dispInfo.belowLimit = dpfo->getBelowLimit(displayGroup, tabIndex);
    dispInfo.boundingBox = boundingBox;
    dispInfo.colorType = dpfo->getColoringType(displayGroup, tabIndex);
    dispInfo.fanMultiplier = dpfo->getFanMultiplier(displayGroup, tabIndex);
    dispInfo.isDrawWithMagnitude = dpfo->isDrawWithMagnitude(displayGroup, tabIndex);
    dispInfo.minimumMagnitude = dpfo->getMinimumMagnitude(displayGroup, tabIndex);
    dispInfo.magnitudeMultiplier = dpfo->getLengthMultiplier(displayGroup, tabIndex);
    dispInfo.plane = plane;
    dispInfo.symbolType = dpfo->getSymbolType(displayGroup, tabIndex);
    dispInfo.fiberOpacities[0] = 1.0;
    dispInfo.fiberOpacities[1] = 1.0;
    dispInfo.fiberOpacities[2] = 1.0;
}

/**
 * Draw fibers for a surface or a volume.
 *
 * @param plane
 *    If not NULL, it is the plane of the volume slice being drawn and
 *    only fibers within the above and below limits from the plane will
 *    be drawn.
 */
void
BrainOpenGLFixedPipeline::drawFiberOrientations(const Plane* plane)
{
    const DisplayPropertiesFiberOrientation* dpfo = m_brain->getDisplayPropertiesFiberOrientation();
    const DisplayGroupEnum::Enum displayGroup = dpfo->getDisplayGroupForTab(this->windowTabIndex);
    if (dpfo->isDisplayed(displayGroup, this->windowTabIndex) == false) {
        return;
    }
//    const float aboveLimit = dpfo->getAboveLimit(displayGroup, this->windowTabIndex);
//    const float belowLimit = dpfo->getBelowLimit(displayGroup, this->windowTabIndex);
//    const float minimumMagnitude = dpfo->getMinimumMagnitude(displayGroup, this->windowTabIndex);
//    const float magnitudeMultiplier = dpfo->getLengthMultiplier(displayGroup, this->windowTabIndex);
//    const float fanMultiplier = dpfo->getFanMultiplier(displayGroup, this->windowTabIndex);
//    const bool isDrawWithMagnitude = dpfo->isDrawWithMagnitude(displayGroup, this->windowTabIndex);
//    const FiberOrientationColoringTypeEnum::Enum colorType = dpfo->getColoringType(displayGroup, this->windowTabIndex);
    const FiberOrientationSymbolTypeEnum::Enum symbolType = dpfo->getSymbolType(displayGroup, this->windowTabIndex);
    
    /*
     * Clipping planes
     */
    BoundingBox clippingBoundingBox;
    clippingBoundingBox.resetWithMaximumExtent();
    
    if (browserTabContent->isClippingPlaneEnabled(0)) {
        const float halfThick = (browserTabContent->getClippingPlaneThickness(0)
                                 * 0.5);
        const float minValue = (browserTabContent->getClippingPlaneCoordinate(0)
                                - halfThick);
        const float maxValue = (browserTabContent->getClippingPlaneCoordinate(0)
                                + halfThick);
        clippingBoundingBox.setMinX(minValue);
        clippingBoundingBox.setMaxX(maxValue);
    }
    if (browserTabContent->isClippingPlaneEnabled(1)) {
        const float halfThick = (browserTabContent->getClippingPlaneThickness(1)
                                 * 0.5);
        const float minValue = (browserTabContent->getClippingPlaneCoordinate(1)
                                - halfThick);
        const float maxValue = (browserTabContent->getClippingPlaneCoordinate(1)
                                + halfThick);
        clippingBoundingBox.setMinY(minValue);
        clippingBoundingBox.setMaxY(maxValue);
    }
    if (browserTabContent->isClippingPlaneEnabled(2)) {
        const float halfThick = (browserTabContent->getClippingPlaneThickness(2)
                                 * 0.5);
        const float minValue = (browserTabContent->getClippingPlaneCoordinate(2)
                                - halfThick);
        const float maxValue = (browserTabContent->getClippingPlaneCoordinate(2)
                                + halfThick);
        clippingBoundingBox.setMinZ(minValue);
        clippingBoundingBox.setMaxZ(maxValue);
    }
    
    /*
     * Save status of clipping and disable clipping.
     * For fibers, the entire fiber symbol is displayed if its
     * origin is within the clipping planes which is tested below.
     */
    GLboolean clipPlanesEnabled[6] = {
        glIsEnabled(GL_CLIP_PLANE0),
        glIsEnabled(GL_CLIP_PLANE1),
        glIsEnabled(GL_CLIP_PLANE2),
        glIsEnabled(GL_CLIP_PLANE3),
        glIsEnabled(GL_CLIP_PLANE4),
        glIsEnabled(GL_CLIP_PLANE5)
    };
    glDisable(GL_CLIP_PLANE0);
    glDisable(GL_CLIP_PLANE1);
    glDisable(GL_CLIP_PLANE2);
    glDisable(GL_CLIP_PLANE3);
    glDisable(GL_CLIP_PLANE4);
    glDisable(GL_CLIP_PLANE5);
    
    /*
     * Fans use lighting but NOT on a volume slice
     */
    disableLighting();
    switch (symbolType) {
        case FiberOrientationSymbolTypeEnum::FIBER_SYMBOL_FANS:
            if (plane == NULL) {
                enableLighting();
            }
            break;
        case FiberOrientationSymbolTypeEnum::FIBER_SYMBOL_LINES:
            break;
    }

    FiberOrientationDisplayInfo fiberOrientDispInfo;
    setFiberOrientationDisplayInfo(dpfo,
                                   displayGroup,
                                   this->windowTabIndex,
                                   &clippingBoundingBox,
                                   const_cast<Plane*>(plane),
                                   fiberOrientDispInfo);
    /*
     * Draw the vectors from each of the connectivity files
     */
    const int32_t numFiberOrienationFiles = m_brain->getNumberOfConnectivityFiberOrientationFiles();
    for (int32_t iFile = 0; iFile < numFiberOrienationFiles; iFile++) {
        CiftiFiberOrientationFile* cfof = m_brain->getConnectivityFiberOrientationFile(iFile);
        if (cfof->isDisplayed(displayGroup,
                              this->windowTabIndex)) {
            /*
             * Draw each of the fiber orientations which may contain multiple fibers
             */
            const int64_t numberOfFiberOrientations = cfof->getNumberOfFiberOrientations();
            for (int64_t i = 0; i < numberOfFiberOrientations; i++) {
                const FiberOrientation* fiberOrientation = cfof->getFiberOrientations(i);
                if (fiberOrientation->m_valid == false) {
                    continue;
                }
                
                addFiberOrientationForDrawing(&fiberOrientDispInfo,
                                              fiberOrientation);
//                drawOneFiberOrientation(&fiberOrientDispInfo,
//                                        fiberOrientation);
            }
        }
    }
    
    drawAllFiberOrientations(&fiberOrientDispInfo,
                             false);
    
    /*
     * Restore status of clipping planes enabled
     */
    if (clipPlanesEnabled[0]) glEnable(GL_CLIP_PLANE0);
    if (clipPlanesEnabled[1]) glEnable(GL_CLIP_PLANE1);
    if (clipPlanesEnabled[2]) glEnable(GL_CLIP_PLANE2);
    if (clipPlanesEnabled[3]) glEnable(GL_CLIP_PLANE3);
    if (clipPlanesEnabled[4]) glEnable(GL_CLIP_PLANE4);
    if (clipPlanesEnabled[5]) glEnable(GL_CLIP_PLANE5);
}

/**
 * Add fiber orientation for drawing.  Note that for alpha blending to
 * work correctly, the fibers must be sorted by depth and drawn from 
 * furthest to nearest.  Some tests will be performed to determine if
 * the fiber should be drawn prior to adding the fiber to the list
 * of fibers that will be drawn.
 *
 * @param fodi
 *    Parameters controlling the drawing of fiber orientations.
 * @param fiberOrientation
 *    The fiber orientation that will be drawn.
 */
void
BrainOpenGLFixedPipeline::addFiberOrientationForDrawing(const FiberOrientationDisplayInfo* fodi,
                                                        const FiberOrientation* fiberOrientation)
{
    /*
     * Test location of fiber orientation for drawing
     */
    if (fodi->plane != NULL) {
        const float distToPlane = fodi->plane->signedDistanceToPlane(fiberOrientation->m_xyz);
        if (distToPlane > fodi->aboveLimit) {
            return;
        }
        if (distToPlane < fodi->belowLimit) {
            return;
        }
    }
    if (fodi->boundingBox->isCoordinateWithinBoundingBox(fiberOrientation->m_xyz) == false) {
        return;
    }
    
    m_fiberOrientationsForDrawing.push_back(const_cast<FiberOrientation*>(fiberOrientation));
}

/*
 * For comparison when sorting that results in furthest fibers drawn first.
 */
static bool
fiberDepthCompare(FiberOrientation* &f1,
                              FiberOrientation* &f2)
{
    return (f1->m_drawingDepth > f2->m_drawingDepth);
}

/**
 * Sort the fiber orientations by depth.
 */
void
BrainOpenGLFixedPipeline::sortFiberOrientationsByDepth()
{
    ElapsedTimer timer;
    timer.start();
    
    /*
     * Create transforms model coordinate to a screen coordinate.
     */
    GLdouble modelMatrixOpenGL[16];
    glGetDoublev(GL_MODELVIEW_MATRIX, modelMatrixOpenGL);
    
    GLdouble projectionMatrixOpenGL[16];
    glGetDoublev(GL_PROJECTION_MATRIX, projectionMatrixOpenGL);
    
//    GLint intViewportOpenGL[4];
//    glGetIntegerv(GL_VIEWPORT, intViewportOpenGL);
    
    Matrix4x4 modelMatrix;
    modelMatrix.setMatrixFromOpenGL(modelMatrixOpenGL);
    
    Matrix4x4 projectionMatrix;
    projectionMatrix.setMatrixFromOpenGL(projectionMatrixOpenGL);
    
    Matrix4x4 modelToScreenMatrix;
    modelToScreenMatrix.setMatrix(projectionMatrix);
    modelToScreenMatrix.premultiply(modelMatrix);
    
    const float m0 = modelToScreenMatrix.getMatrixElement(2, 0);
    const float m1 = modelToScreenMatrix.getMatrixElement(2, 1);
    const float m2 = modelToScreenMatrix.getMatrixElement(2, 2);
    const float m3 = modelToScreenMatrix.getMatrixElement(2, 3);
    
//    float xyz[3];
    for (std::list<FiberOrientation*>::const_iterator iter = m_fiberOrientationsForDrawing.begin();
         iter != m_fiberOrientationsForDrawing.end();
         iter++) {
        const FiberOrientation* fiberOrientation = *iter;
        
//        xyz[0] = fiberOrientation->m_xyz[0];
//        xyz[1] = fiberOrientation->m_xyz[1];
//        xyz[2] = fiberOrientation->m_xyz[2];
//        modelToScreenMatrix.multiplyPoint3(xyz);
//        const float screenDepth = ((xyz[2] + 1.0) / 2.0);
        
        const float rawDepth =(m0 * fiberOrientation->m_xyz[0]
                            + m1 * fiberOrientation->m_xyz[1]
                            + m2 * fiberOrientation->m_xyz[2]
                            + m3);
        const float screenDepth = ((rawDepth + 1.0) / 2.0);

        fiberOrientation->m_drawingDepth = screenDepth;
        
//        double modelXYZ[3] = {
//            fiberOrientation->m_xyz[0],
//            fiberOrientation->m_xyz[1],
//            fiberOrientation->m_xyz[2]
//        };
//        double windowPos[3];
//        if (gluProject(modelXYZ[0],
//                       modelXYZ[1],
//                       modelXYZ[2],
//                       modelMatrixOpenGL,
//                       projectionMatrixOpenGL,
//                       intViewportOpenGL,
//                       &windowPos[0],
//                       &windowPos[1],
//                       &windowPos[2])) {
//            
//            const float diff = std::fabs(screenDepth - windowPos[2]);
//            if (diff > 0.01) {
//                std::cout << "ERROR: Zs: " << screenDepth << " x " << windowPos[2] << " " << qPrintable(AString::fromNumbers(xyz, 3, ",")) <<  std::endl;
//            }
//        }
    }
    
    m_fiberOrientationsForDrawing.sort(fiberDepthCompare);
    
    CaretLogSevere("Time (ms) to compute depth of fibers and sort: "
                   + AString::number(timer.getElapsedTimeMilliseconds()));
    for (std::list<FiberOrientation*>::const_iterator iter = m_fiberOrientationsForDrawing.begin();
         iter != m_fiberOrientationsForDrawing.end();
         iter++) {
        const FiberOrientation* fiberOrientation = *iter;
        std::cout << " " << fiberOrientation->m_drawingDepth;
    }
}

/**
 * Draw all of the fiber orienations.
 *
 * @param fodi
 *    Parameters controlling the drawing of fiber orientations. 
 */
void
BrainOpenGLFixedPipeline::drawAllFiberOrientations(const FiberOrientationDisplayInfo* fodi,
                                                   const bool isSortFibers)
{
    if (isSortFibers) {
        sortFiberOrientationsByDepth();
    }
    
    for (std::list<FiberOrientation*>::const_iterator iter = m_fiberOrientationsForDrawing.begin();
         iter != m_fiberOrientationsForDrawing.end();
         iter++) {
        const FiberOrientation* fiberOrientation = *iter;

        /*
         * Draw each of the fibers
         */
        const int64_t numberOfFibers = fiberOrientation->m_numberOfFibers;
        for (int64_t j = 0; j < numberOfFibers; j++) {
            const Fiber* fiber = fiberOrientation->m_fibers[j];
            
            /*
             * Apply display properties
             */
            bool drawIt = true;
            if (fiber->m_meanF < fodi->minimumMagnitude) {
                drawIt = false;
            }
            
            if (drawIt) {
                float alpha = 1.0;
                if (j < 3) {
                    alpha = fodi->fiberOpacities[j];
                    CaretAssertMessage(((alpha >= 0.0) && (alpha <= 1.0)),
                                       ("Value=" + AString::number(alpha)));
                    if (alpha <= 0.0) {
                        continue;
                    }
                }
                
                /*
                 * Length of vector
                 */
                float vectorLength = fodi->magnitudeMultiplier;
                if (fodi->isDrawWithMagnitude) {
                    vectorLength *= fiber->m_meanF;
                }
                
                /*
                 * Vector with magnitude
                 */
                const float magnitudeVector[3] = {
                    fiber->m_directionUnitVector[0] * vectorLength,
                    fiber->m_directionUnitVector[1] * vectorLength,
                    fiber->m_directionUnitVector[2] * vectorLength
                };
                
                const float halfMagnitudeVector[3] = {
                    magnitudeVector[0] * 0.5,
                    magnitudeVector[1] * 0.5,
                    magnitudeVector[2] * 0.5,
                };
                
                /*
                 * Start of vector
                 */
                float startXYZ[3] = {
                    fiberOrientation->m_xyz[0],
                    fiberOrientation->m_xyz[1],
                    fiberOrientation->m_xyz[2]
                };
                
                /*
                 * When drawing lines, start of vector is offset by
                 * have of the vector length since the vector is
                 * bi-directional.
                 */
                switch (fodi->symbolType) {
                    case FiberOrientationSymbolTypeEnum::FIBER_SYMBOL_FANS:
                        break;
                    case FiberOrientationSymbolTypeEnum::FIBER_SYMBOL_LINES:
                        startXYZ[0] -= halfMagnitudeVector[0];
                        startXYZ[1] -= halfMagnitudeVector[1];
                        startXYZ[2] -= halfMagnitudeVector[2];
                        break;
                }
                
                
                /*
                 * End of vector
                 */
                float endXYZ[3] = { 0.0, 0.0, 0.0 };
                
                /*
                 * When drawing lines, end point is the start
                 * plus the vector with magnitude.
                 *
                 * When drawing fans, there are two endpoints
                 * with the fans starting in the middle.
                 */
                switch (fodi->symbolType) {
                    case FiberOrientationSymbolTypeEnum::FIBER_SYMBOL_FANS:
                        endXYZ[0] = startXYZ[0] + halfMagnitudeVector[0];
                        endXYZ[1] = startXYZ[1] + halfMagnitudeVector[1];
                        endXYZ[2] = startXYZ[2] + halfMagnitudeVector[2];
                        break;
                    case FiberOrientationSymbolTypeEnum::FIBER_SYMBOL_LINES:
                        endXYZ[0] = startXYZ[0] + magnitudeVector[0];
                        endXYZ[1] = startXYZ[1] + magnitudeVector[1];
                        endXYZ[2] = startXYZ[2] + magnitudeVector[2];
                        break;
                }
                
                /*
                 * Color of fiber
                 */
                switch (fodi->colorType) {
                    case FiberOrientationColoringTypeEnum::FIBER_COLORING_FIBER_INDEX_AS_RGB:
                    {
                        const int32_t indx = j % 3;
                        switch (indx) {
                            case 0: // use RED
                                glColor4f(BrainOpenGLFixedPipeline::COLOR_RED[0],
                                          BrainOpenGLFixedPipeline::COLOR_RED[1],
                                          BrainOpenGLFixedPipeline::COLOR_RED[2],
                                          alpha);
                                break;
                            case 1: // use BLUE
                                glColor4f(BrainOpenGLFixedPipeline::COLOR_BLUE[0],
                                          BrainOpenGLFixedPipeline::COLOR_BLUE[1],
                                          BrainOpenGLFixedPipeline::COLOR_BLUE[2],
                                          alpha);
                                break;
                            case 2: // use GREEN
                                glColor4f(BrainOpenGLFixedPipeline::COLOR_GREEN[0],
                                          BrainOpenGLFixedPipeline::COLOR_GREEN[1],
                                          BrainOpenGLFixedPipeline::COLOR_GREEN[2],
                                          alpha);
                                break;
                        }
                    }
                        break;
                    case FiberOrientationColoringTypeEnum::FIBER_COLORING_XYZ_AS_RGB:
                        CaretAssert((fiber->m_directionUnitVectorRGB[0] >= 0.0) && (fiber->m_directionUnitVectorRGB[0] <= 1.0));
                        CaretAssert((fiber->m_directionUnitVectorRGB[1] >= 0.0) && (fiber->m_directionUnitVectorRGB[1] <= 1.0));
                        CaretAssert((fiber->m_directionUnitVectorRGB[2] >= 0.0) && (fiber->m_directionUnitVectorRGB[2] <= 1.0));
                        CaretAssert((alpha >= 0.0) && (alpha <= 1.0));
                        glColor4f(fiber->m_directionUnitVectorRGB[0],
                                  fiber->m_directionUnitVectorRGB[1],
                                  fiber->m_directionUnitVectorRGB[2],
                                  alpha);
                        break;
                }
                
                /*
                 * Draw the fiber
                 */
                switch (fodi->symbolType) {
                    case FiberOrientationSymbolTypeEnum::FIBER_SYMBOL_FANS:
                    {
                        /*
                         * Draw the cones
                         */
                        const float radiansToDegrees = 180.0 / M_PI;
                        const float majorAxis = std::min((vectorLength
                                                          * std::tan(fiber->m_fanningMajorAxisAngle)
                                                          * fodi->fanMultiplier),
                                                         vectorLength);
                        const float minorAxis = std::min((vectorLength
                                                          * std::tan(fiber->m_fanningMinorAxisAngle)
                                                          * fodi->fanMultiplier),
                                                         vectorLength);
                        
                        /*
                         * First cone
                         */
                        glPushMatrix();
                        glTranslatef(startXYZ[0], startXYZ[1], startXYZ[2]);
                        glRotatef(-fiber->m_phi * radiansToDegrees, 0.0, 0.0, 1.0);
                        glRotatef(-fiber->m_theta * radiansToDegrees, 0.0, 1.0, 0.0);
                        glRotatef(-fiber->m_psi * radiansToDegrees, 0.0, 0.0, 1.0);
                        glScalef(majorAxis * 2.0,
                                 minorAxis * 2.0,
                                 vectorLength);
                        m_shapeCone->draw();
                        glPopMatrix();
                        
                        /*
                         * Second cone but pointing in opposite direction
                         */
                        glPushMatrix();
                        glTranslatef(startXYZ[0], startXYZ[1], startXYZ[2]);
                        glRotatef(-fiber->m_phi * radiansToDegrees, 0.0, 0.0, 1.0);
                        glRotatef(180.0 -fiber->m_theta * radiansToDegrees, 0.0, 1.0, 0.0);
                        glRotatef(fiber->m_psi * radiansToDegrees, 0.0, 0.0, 1.0);
                        glScalef(majorAxis * 2.0,
                                 minorAxis * 2.0,
                                 vectorLength);
                        m_shapeCone->draw();
                        glPopMatrix();
                        
                    }
                        break;
                    case FiberOrientationSymbolTypeEnum::FIBER_SYMBOL_LINES:
                    {
                        const float radius = 2.0;
                        setLineWidth(radius);
                        glBegin(GL_LINES);
                        glVertex3fv(startXYZ);
                        glVertex3fv(endXYZ);
                        glEnd();
                    }
                        break;
                }
            }
        }
    }
    
    /*
     * Now clear the list of fiber orientations for drawing.
     */
    m_fiberOrientationsForDrawing.clear();
}

/**
 * Draw one fiber orientation.
 * @param fodi
 *    Parameters controlling the drawing of fiber orientations.
 * @param fiberOrientation
 *    The fiber orientation that will be drawn.
 */
void
BrainOpenGLFixedPipeline::drawOneFiberOrientation(const FiberOrientationDisplayInfo* fodi,
                                                  const FiberOrientation* fiberOrientation)
{
    /*
     * Test location of fiber orientation for drawing
     */
    bool drawFiberOrientation = true;
    if (fodi->plane != NULL) {
        const float distToPlane = fodi->plane->signedDistanceToPlane(fiberOrientation->m_xyz);
        if (distToPlane > fodi->aboveLimit) {
            drawFiberOrientation = false;
        }
        if (distToPlane < fodi->belowLimit) {
            drawFiberOrientation = false;
        }
    }
    if (fodi->boundingBox->isCoordinateWithinBoundingBox(fiberOrientation->m_xyz) == false) {
        drawFiberOrientation = false;
    }
    if (drawFiberOrientation == false) {
        return;
    }
    
    /*
     * Draw each of the fibers
     */
    const int64_t numberOfFibers = fiberOrientation->m_numberOfFibers;
    for (int64_t j = 0; j < numberOfFibers; j++) {
        const Fiber* fiber = fiberOrientation->m_fibers[j];
        
        /*
         * Apply display properties
         */
        bool drawIt = true;
        if (fiber->m_meanF < fodi->minimumMagnitude) {
            drawIt = false;
        }
        
        if (drawIt) {
            float alpha = 1.0;
            if (j < 3) {
                alpha = fodi->fiberOpacities[j];
                CaretAssertMessage(((alpha >= 0.0) && (alpha <= 1.0)),
                                   ("Value=" + AString::number(alpha)));
                if (alpha <= 0.0) {
                    continue;
                }
            }
            
            /*
             * Length of vector
             */
            float vectorLength = fodi->magnitudeMultiplier;
            if (fodi->isDrawWithMagnitude) {
                vectorLength *= fiber->m_meanF;
            }
            
            /*
             * Vector with magnitude
             */
            const float magnitudeVector[3] = {
                fiber->m_directionUnitVector[0] * vectorLength,
                fiber->m_directionUnitVector[1] * vectorLength,
                fiber->m_directionUnitVector[2] * vectorLength
            };
            
            const float halfMagnitudeVector[3] = {
                magnitudeVector[0] * 0.5,
                magnitudeVector[1] * 0.5,
                magnitudeVector[2] * 0.5,
            };
            
            /*
             * Start of vector
             */
            float startXYZ[3] = {
                fiberOrientation->m_xyz[0],
                fiberOrientation->m_xyz[1],
                fiberOrientation->m_xyz[2]
            };
            
            /*
             * When drawing lines, start of vector is offset by
             * have of the vector length since the vector is
             * bi-directional.
             */
            switch (fodi->symbolType) {
                case FiberOrientationSymbolTypeEnum::FIBER_SYMBOL_FANS:
                    break;
                case FiberOrientationSymbolTypeEnum::FIBER_SYMBOL_LINES:
                    startXYZ[0] -= halfMagnitudeVector[0];
                    startXYZ[1] -= halfMagnitudeVector[1];
                    startXYZ[2] -= halfMagnitudeVector[2];
                    break;
            }
            
            
            /*
             * End of vector
             */
            float endXYZ[3] = { 0.0, 0.0, 0.0 };
            //float endTwoXYZ[3] = { 0.0, 0.0, 0.0 };//not used, commenting out to prevent compiler warning
            
            /*
             * When drawing lines, end point is the start
             * plus the vector with magnitude.
             *
             * When drawing fans, there are two endpoints
             * with the fans starting in the middle.
             */
            switch (fodi->symbolType) {
                case FiberOrientationSymbolTypeEnum::FIBER_SYMBOL_FANS:
                    endXYZ[0] = startXYZ[0] + halfMagnitudeVector[0];
                    endXYZ[1] = startXYZ[1] + halfMagnitudeVector[1];
                    endXYZ[2] = startXYZ[2] + halfMagnitudeVector[2];
                    //endTwoXYZ[0] = startXYZ[0] - halfMagnitudeVector[0];
                    //endTwoXYZ[1] = startXYZ[1] - halfMagnitudeVector[1];
                    //endTwoXYZ[2] = startXYZ[2] - halfMagnitudeVector[2];
                    break;
                case FiberOrientationSymbolTypeEnum::FIBER_SYMBOL_LINES:
                    endXYZ[0] = startXYZ[0] + magnitudeVector[0];
                    endXYZ[1] = startXYZ[1] + magnitudeVector[1];
                    endXYZ[2] = startXYZ[2] + magnitudeVector[2];
                    break;
            }
            
            /*
             * Color of fiber
             */
            switch (fodi->colorType) {
                case FiberOrientationColoringTypeEnum::FIBER_COLORING_FIBER_INDEX_AS_RGB:
                {
                    const int32_t indx = j % 3;
                    switch (indx) {
                        case 0: // use RED
                            glColor4f(BrainOpenGLFixedPipeline::COLOR_RED[0],
                                      BrainOpenGLFixedPipeline::COLOR_RED[1],
                                      BrainOpenGLFixedPipeline::COLOR_RED[2],
                                      alpha);
                            break;
                        case 1: // use BLUE
                            glColor4f(BrainOpenGLFixedPipeline::COLOR_BLUE[0],
                                      BrainOpenGLFixedPipeline::COLOR_BLUE[1],
                                      BrainOpenGLFixedPipeline::COLOR_BLUE[2],
                                      alpha);
                            break;
                        case 2: // use GREEN
                            glColor4f(BrainOpenGLFixedPipeline::COLOR_GREEN[0],
                                      BrainOpenGLFixedPipeline::COLOR_GREEN[1],
                                      BrainOpenGLFixedPipeline::COLOR_GREEN[2],
                                      alpha);
                            break;
                    }
                }
                    break;
                case FiberOrientationColoringTypeEnum::FIBER_COLORING_XYZ_AS_RGB:
                    CaretAssert((fiber->m_directionUnitVectorRGB[0] >= 0.0) && (fiber->m_directionUnitVectorRGB[0] <= 1.0));
                    CaretAssert((fiber->m_directionUnitVectorRGB[1] >= 0.0) && (fiber->m_directionUnitVectorRGB[1] <= 1.0));
                    CaretAssert((fiber->m_directionUnitVectorRGB[2] >= 0.0) && (fiber->m_directionUnitVectorRGB[2] <= 1.0));
                    CaretAssert((alpha >= 0.0) && (alpha <= 1.0));
                    glColor4f(fiber->m_directionUnitVectorRGB[0],
                              fiber->m_directionUnitVectorRGB[1],
                              fiber->m_directionUnitVectorRGB[2],
                              alpha);
                    break;
            }
            
            /*
             * Draw the fiber
             */
            switch (fodi->symbolType) {
                case FiberOrientationSymbolTypeEnum::FIBER_SYMBOL_FANS:
                {
                    /*
                     * Draw the cones
                     */
                    const float radiansToDegrees = 180.0 / M_PI;
//                    const float majorAxis = fiber->m_fanningMajorAxisAngle * fodi->fanMultiplier;
//                    const float minorAxis = fiber->m_fanningMinorAxisAngle * fodi->fanMultiplier;
//                    const float maxWidth = z;
                    const float majorAxis = std::min((vectorLength
                                                      * std::tan(fiber->m_fanningMajorAxisAngle)
                                                      * fodi->fanMultiplier),
                                                     vectorLength);
                    const float minorAxis = std::min((vectorLength
                                                      * std::tan(fiber->m_fanningMinorAxisAngle)
                                                      * fodi->fanMultiplier),
                                                     vectorLength);
                   
                    /*
                     * First cone
                     */
                    glPushMatrix();
                    glTranslatef(startXYZ[0], startXYZ[1], startXYZ[2]);
                    glRotatef(-fiber->m_phi * radiansToDegrees, 0.0, 0.0, 1.0);
                    glRotatef(-fiber->m_theta * radiansToDegrees, 0.0, 1.0, 0.0);
                    glRotatef(-fiber->m_psi * radiansToDegrees, 0.0, 0.0, 1.0);
                    glScalef(majorAxis * 2.0,
                             minorAxis * 2.0,
                             vectorLength);
                    m_shapeCone->draw();
                    glPopMatrix();

                    /*
                     * Second cone but pointing in opposite direction
                     */
                    glPushMatrix();
                    glTranslatef(startXYZ[0], startXYZ[1], startXYZ[2]);
                    glRotatef(-fiber->m_phi * radiansToDegrees, 0.0, 0.0, 1.0);
                    glRotatef(180.0 -fiber->m_theta * radiansToDegrees, 0.0, 1.0, 0.0);
                    glRotatef(fiber->m_psi * radiansToDegrees, 0.0, 0.0, 1.0);
                    glScalef(majorAxis * 2.0,
                             minorAxis * 2.0,
                             vectorLength);
                    m_shapeCone->draw();
                    glPopMatrix();
                    
                }
//                    drawEllipticalCone(endXYZ,
//                                       startXYZ,
//                                       fodi->fanMultiplier,
//                                       fiber->m_fanningMajorAxisAngle * fodi->fanMultiplier,
//                                       fiber->m_fanningMinorAxisAngle * fodi->fanMultiplier,
//                                       fiber->m_psi,
//                                       false);
//                    drawEllipticalCone(endXYZ,
//                                       startXYZ,
//                                       fodi->fanMultiplier,
//                                       fiber->m_fanningMajorAxisAngle * fodi->fanMultiplier,
//                                       fiber->m_fanningMinorAxisAngle * fodi->fanMultiplier,
//                                       fiber->m_psi,
//                                       true);
                    break;
                case FiberOrientationSymbolTypeEnum::FIBER_SYMBOL_LINES:
                {
                    const float radius = 2.0;
                    setLineWidth(radius);
                    glBegin(GL_LINES);
                    glVertex3fv(startXYZ);
                    glVertex3fv(endXYZ);
                    glEnd();
                }
                    break;
            }
        }
    }
}

/**
 * Draw fiber trajectories on a surface.
 */
void
BrainOpenGLFixedPipeline::drawSurfaceFiberTrajectories()
{
    drawFiberTrajectories(NULL);
}

/**
 * Draw the fiber trajectories.
 * @param plane
 *    If a volume it is non-NULL and contains the plane of the slice.
 */
void
BrainOpenGLFixedPipeline::drawFiberTrajectories(const Plane* plane)
{
    const DisplayPropertiesFiberTrajectory* dpft = m_brain->getDisplayPropertiesFiberTrajectory();
    const DisplayGroupEnum::Enum displayGroup = dpft->getDisplayGroupForTab(this->windowTabIndex);
    if (dpft->isDisplayed(displayGroup, this->windowTabIndex) == false) {
        return;
    }
    
    const float proportionMinimumOpacity = dpft->getProportionMinimumOpacity(displayGroup,
                                                                      this->windowTabIndex);
    const float proportionMaximumOpacity = dpft->getProportionMaximumOpacity(displayGroup,
                                                                      this->windowTabIndex);
    const float proportionRangeOpacity = proportionMaximumOpacity - proportionMinimumOpacity;
    
    const float countMinimumOpacity = dpft->getCountMinimumOpacity(displayGroup,
                                                                             this->windowTabIndex);
    const float countMaximumOpacity = dpft->getCountMaximumOpacity(displayGroup,
                                                                             this->windowTabIndex);
    const float countRangeOpacity = countMaximumOpacity - countMinimumOpacity;
    
    const float distanceMinimumOpacity = dpft->getDistanceMinimumOpacity(displayGroup,
                                                                   this->windowTabIndex);
    const float distanceMaximumOpacity = dpft->getDistanceMaximumOpacity(displayGroup,
                                                                   this->windowTabIndex);
    const float distanceRangeOpacity = distanceMaximumOpacity - distanceMinimumOpacity;
    
    const FiberTrajectoryDisplayModeEnum::Enum displayMode = dpft->getDisplayMode(displayGroup,
                                                                                  this->windowTabIndex);
    uint32_t streamlineThreshold = std::numeric_limits<uint32_t>::max();
    switch (displayMode) {
        case FiberTrajectoryDisplayModeEnum::FIBER_TRAJECTORY_DISPLAY_ABSOLUTE:
            streamlineThreshold = dpft->getCountStreamline(displayGroup,
                                                           this->windowTabIndex);
            if (countRangeOpacity <= 0.0) {
                return;
            }
            break;
        case FiberTrajectoryDisplayModeEnum::FIBER_TRAJECTORY_DISPLAY_DISTANCE_WEIGHTED:
        case FiberTrajectoryDisplayModeEnum::FIBER_TRAJECTORY_DISPLAY_DISTANCE_WEIGHTED_LOG:
            streamlineThreshold = dpft->getDistanceStreamline(displayGroup,
                                                           this->windowTabIndex);
            if (distanceRangeOpacity <= 0.0) {
                return;
            }
            break;
        case FiberTrajectoryDisplayModeEnum::FIBER_TRAJECTORY_DISPLAY_PROPORTION:
            streamlineThreshold = dpft->getProportionStreamline(displayGroup,
                                                           this->windowTabIndex);
            if (proportionRangeOpacity <= 0.0) {
                return;
            }
            break;
    }
    
    /*
     * Clipping planes
     */
    BoundingBox clippingBoundingBox;
    clippingBoundingBox.resetWithMaximumExtent();
    
    if (browserTabContent->isClippingPlaneEnabled(0)) {
        const float halfThick = (browserTabContent->getClippingPlaneThickness(0)
                                 * 0.5);
        const float minValue = (browserTabContent->getClippingPlaneCoordinate(0)
                                - halfThick);
        const float maxValue = (browserTabContent->getClippingPlaneCoordinate(0)
                                + halfThick);
        clippingBoundingBox.setMinX(minValue);
        clippingBoundingBox.setMaxX(maxValue);
    }
    if (browserTabContent->isClippingPlaneEnabled(1)) {
        const float halfThick = (browserTabContent->getClippingPlaneThickness(1)
                                 * 0.5);
        const float minValue = (browserTabContent->getClippingPlaneCoordinate(1)
                                - halfThick);
        const float maxValue = (browserTabContent->getClippingPlaneCoordinate(1)
                                + halfThick);
        clippingBoundingBox.setMinY(minValue);
        clippingBoundingBox.setMaxY(maxValue);
    }
    if (browserTabContent->isClippingPlaneEnabled(2)) {
        const float halfThick = (browserTabContent->getClippingPlaneThickness(2)
                                 * 0.5);
        const float minValue = (browserTabContent->getClippingPlaneCoordinate(2)
                                - halfThick);
        const float maxValue = (browserTabContent->getClippingPlaneCoordinate(2)
                                + halfThick);
        clippingBoundingBox.setMinZ(minValue);
        clippingBoundingBox.setMaxZ(maxValue);
    }
    
    /*
     * Save status of clipping and disable clipping.
     * For fibers, the entire fiber symbol is displayed if its
     * origin is within the clipping planes which is tested below.
     */
    GLboolean clipPlanesEnabled[6] = {
        glIsEnabled(GL_CLIP_PLANE0),
        glIsEnabled(GL_CLIP_PLANE1),
        glIsEnabled(GL_CLIP_PLANE2),
        glIsEnabled(GL_CLIP_PLANE3),
        glIsEnabled(GL_CLIP_PLANE4),
        glIsEnabled(GL_CLIP_PLANE5)
    };
    glDisable(GL_CLIP_PLANE0);
    glDisable(GL_CLIP_PLANE1);
    glDisable(GL_CLIP_PLANE2);
    glDisable(GL_CLIP_PLANE3);
    glDisable(GL_CLIP_PLANE4);
    glDisable(GL_CLIP_PLANE5);
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    const DisplayPropertiesFiberOrientation* dpfo = m_brain->getDisplayPropertiesFiberOrientation();
    const FiberOrientationSymbolTypeEnum::Enum symbolType = dpfo->getSymbolType(displayGroup, this->windowTabIndex);
    FiberOrientationDisplayInfo fiberOrientDispInfo;
    setFiberOrientationDisplayInfo(dpfo,
                                   displayGroup,
                                   this->windowTabIndex,
                                   &clippingBoundingBox,
                                   const_cast<Plane*>(plane),
                                   fiberOrientDispInfo);
    
    /*
     * Fans use lighting but NOT on a volume slice
     */
    disableLighting();
    switch (symbolType) {
        case FiberOrientationSymbolTypeEnum::FIBER_SYMBOL_FANS:
            if (plane == NULL) {
                enableLighting();
            }
            break;
        case FiberOrientationSymbolTypeEnum::FIBER_SYMBOL_LINES:
            break;
    }
    
    const int32_t numTrajFiles = m_brain->getNumberOfConnectivityFiberTrajectoryFiles();
    for (int32_t iFile = 0; iFile < numTrajFiles; iFile++) {
        const CiftiFiberTrajectoryFile* trajFile = m_brain->getConnectivityFiberTrajectoryFile(iFile);
        if (trajFile->isDisplayed(displayGroup, this->windowTabIndex) == false) {
            continue;
        }
        
        const std::vector<FiberOrientationTrajectory*>& trajectories = trajFile->getLoadedFiberOrientationTrajectories();
        const int64_t numTraj = static_cast<int64_t>(trajectories.size());
        for (int64_t iTraj = 0; iTraj < numTraj; iTraj++) {
            const FiberOrientationTrajectory* fiberTraj = trajectories[iTraj];
            const FiberOrientation* orientation = fiberTraj->m_fiberOrientation;
            const FiberFractions* fiberFractions = fiberTraj->m_fiberFractions;
            
            if (fiberFractions->fiberFractions.size() != 3) {
                CaretLogWarning("Fiber Trajectory index="
                                + AString::number(iTraj)
                                + " has "
                                + AString::number(fiberFractions->fiberFractions.size())
                                + " fibers != 3 from file "
                                + trajFile->getFileNameNoPath());
                
                continue;
            }
            else if (fiberFractions->totalCount < streamlineThreshold) {
                continue;
            }

            float fiberOpacities[3] = { 0.0, 0.0, 0.0 };
            const float fiberCounts[3] = {
                fiberFractions->fiberFractions[0] * fiberFractions->totalCount,
                fiberFractions->fiberFractions[1] * fiberFractions->totalCount,
                fiberFractions->fiberFractions[2] * fiberFractions->totalCount
            };
            
            /*
             * Set opacities for each fiber using mapping of minimum and
             * maximum opacities
             */
            switch (displayMode) {
                case FiberTrajectoryDisplayModeEnum::FIBER_TRAJECTORY_DISPLAY_ABSOLUTE:
                    fiberOpacities[0] = (fiberCounts[0]
                                         - countMinimumOpacity) / countRangeOpacity;
                    fiberOpacities[1] = (fiberCounts[1]
                                         - countMinimumOpacity) / countRangeOpacity;
                    fiberOpacities[2] = (fiberCounts[2]
                                         - countMinimumOpacity) / countRangeOpacity;
                    break;
                case FiberTrajectoryDisplayModeEnum::FIBER_TRAJECTORY_DISPLAY_DISTANCE_WEIGHTED:
                    fiberOpacities[0] = ((fiberCounts[0] * fiberFractions->distance)
                                         - distanceMinimumOpacity) / distanceRangeOpacity;
                    fiberOpacities[1] = ((fiberCounts[1] * fiberFractions->distance)
                                         - distanceMinimumOpacity) / distanceRangeOpacity;
                    fiberOpacities[2] = ((fiberCounts[2] * fiberFractions->distance)
                                         - distanceMinimumOpacity) / distanceRangeOpacity;
                    break;
                case FiberTrajectoryDisplayModeEnum::FIBER_TRAJECTORY_DISPLAY_DISTANCE_WEIGHTED_LOG:
                {
                    const float distanceLog = std::log(fiberFractions->distance);
                    fiberOpacities[0] = ((fiberCounts[0] * distanceLog)
                                         - distanceMinimumOpacity) / distanceRangeOpacity;
                    fiberOpacities[1] = ((fiberCounts[1] * distanceLog)
                                         - distanceMinimumOpacity) / distanceRangeOpacity;
                    fiberOpacities[2] = ((fiberCounts[2] * distanceLog)
                                         - distanceMinimumOpacity) / distanceRangeOpacity;
                }
                    break;
                case FiberTrajectoryDisplayModeEnum::FIBER_TRAJECTORY_DISPLAY_PROPORTION:
                    fiberOpacities[0] = (fiberFractions->fiberFractions[0]
                                         - proportionMinimumOpacity) /proportionRangeOpacity;
                    fiberOpacities[1] = (fiberFractions->fiberFractions[1]
                                         - proportionMinimumOpacity) /proportionRangeOpacity;
                    fiberOpacities[2] = (fiberFractions->fiberFractions[2]
                                         - proportionMinimumOpacity) /proportionRangeOpacity;
                    break;
            }
            int32_t drawCount = 3;
            for (int32_t i = 0; i < 3; i++) {
                if (fiberOpacities[i] > 1.0) {
                    fiberOpacities[i] = 1.0;
                }
                else if (fiberOpacities[i] <= 0.0) {
                    fiberOpacities[i] = 0.0;
                    drawCount--;
                }
            }
            if (drawCount <= 0) {
                continue;
            }
            
            fiberOrientDispInfo.fiberOpacities[0] = fiberOpacities[0];
            fiberOrientDispInfo.fiberOpacities[1] = fiberOpacities[1];
            fiberOrientDispInfo.fiberOpacities[2] = fiberOpacities[2];
            
            addFiberOrientationForDrawing(&fiberOrientDispInfo,
                                          orientation);
//            drawOneFiberOrientation(&fiberOrientDispInfo,
//                                    orientation);
            
//            glColor3f(1.0, 0.0, 0.0);
//            glPushMatrix();
//            glTranslatef(orientation->m_xyz[0],
//                         orientation->m_xyz[1],
//                         orientation->m_xyz[2]);
//            drawSphere(1.0);
//            glPopMatrix();
        }
    }
    
    drawAllFiberOrientations(&fiberOrientDispInfo,
                             true);
    
    glDisable(GL_BLEND);
    
    /*
     * Restore status of clipping planes enabled
     */
    if (clipPlanesEnabled[0]) glEnable(GL_CLIP_PLANE0);
    if (clipPlanesEnabled[1]) glEnable(GL_CLIP_PLANE1);
    if (clipPlanesEnabled[2]) glEnable(GL_CLIP_PLANE2);
    if (clipPlanesEnabled[3]) glEnable(GL_CLIP_PLANE3);
    if (clipPlanesEnabled[4]) glEnable(GL_CLIP_PLANE4);
    if (clipPlanesEnabled[5]) glEnable(GL_CLIP_PLANE5);
}


/**
 * Draw a cone with an elliptical shape.
 * @param baseXYZ
 *    Location of the base (flat wide) part of the cone
 * @param apexXYZ
 *    Location of the pointed end of the cone
 * @param baseRadiusScaling
 *    Scale the base radius by this amount
 * @param baseMajorAngle
 *    Angle for the major axis of the ellipse (units = Radians)
 *    Valid range is [0, Pi/2]
 * @param baseMinorAngle
 *    Angle for the minor axis of the ellipse (units = Radians)
 *    Valid range is [0, Pi/2]
 * @param baseRotationAngle  (units = Radians)
 *    Rotation of major axis from 'up'
 * @param backwardsFlag
 *    If true, draw the cone backwards (rotated 180 degrees).
 */
void
BrainOpenGLFixedPipeline::drawEllipticalCone(const float baseXYZ[3],
                                             const float apexXYZ[3],
                                             const float baseRadiusScaling,
                                             const float baseMajorAngleIn,
                                             const float baseMinorAngleIn,
                                             const float baseRotationAngle,
                                             const bool backwardsFlag)
{
    float x1 = apexXYZ[0];
    float y1 = apexXYZ[1];
    float z1 = apexXYZ[2];
    float vx = baseXYZ[0] - x1;
    float vy = baseXYZ[1] - y1;
    float vz = baseXYZ[2] - z1;
    
    float z = (float)std::sqrt( vx*vx + vy*vy + vz*vz );
    double ax = 0.0f;
    
    const float maxAngle = M_PI_2 * 0.95;
    float baseMajorAngle = baseMajorAngleIn;
    if (baseMajorAngle > maxAngle) {
        baseMajorAngle = maxAngle;
    }
    float baseMinorAngle = baseMinorAngleIn;
    if (baseMinorAngle > maxAngle) {
        baseMinorAngle = maxAngle;
    }
    
    const float maxWidth = z;
    const float majorAxis = std::min(z * std::tan(baseMajorAngle) * baseRadiusScaling,
                                       maxWidth);
    const float minorAxis = std::min(z * std::tan(baseMinorAngle) * baseRadiusScaling,
                                       maxWidth);
    
    double zero = 1.0e-3;
    
    if (std::abs(vz) < zero) {
        ax = 57.2957795*std::acos( vx/z ); // rotation angle in x-y plane
        if ( vx <= 0.0f ) ax = -ax;
    }
    else {
        ax = 57.2957795*std::acos( vz/z ); // rotation angle
        if ( vz <= 0.0f ) ax = -ax;
    }
    
    glPushMatrix();
    glTranslatef( x1, y1, z1 );
    
    float rx = -vy*vz;
    float ry = vx*vz;
    
    if ((std::abs(vx) < zero) && (std::fabs(vz) < zero)) {
        if (vy > 0) {
            ax = 90;
        }
    }
    
    if (std::abs(vz) < zero)  {
        glRotated(90.0, 0.0, 1.0, 0.0); // Rotate & align with x axis
        glRotated(ax, -1.0, 0.0, 0.0); // Rotate to point 2 in x-y plane
    }
    else {
        glRotated(ax, rx, ry, 0.0); // Rotate about rotation vector
    }
    
    glPushMatrix();
    
    if (backwardsFlag) {
        glRotatef(180.0,
                  0.0,
                  1.0,
                  0.0);
        glRotatef(MathFunctions::toDegrees(-baseRotationAngle), 0.0, 0.0, 1.0);
    }
    else {
        /*
         * Rotate around Z-axis using the base rotation angle
         */
        glRotatef(MathFunctions::toDegrees(baseRotationAngle), 0.0, 0.0, 1.0);
    }

    /*
     * Draw the cone
     */
    glScalef(majorAxis * 2.0,
             minorAxis * 2.0,
             z);
    m_shapeCone->draw();
    glPopMatrix();
    glPopMatrix();

}

/**
 * Draw fiber orientations on surface models.
 */
void
BrainOpenGLFixedPipeline::drawSurfaceFiberOrientations()
{
    drawFiberOrientations(NULL);
}

/**
 * Draw the surface montage controller.
 * @param browserTabContent
 *   Content of the window.
 * @param surfaceMontageModel
 *   The surface montage displayed in the window.
 * @param viewport
 *   Region for drawing.
 */
void 
BrainOpenGLFixedPipeline::drawSurfaceMontageModel(BrowserTabContent* browserTabContent,
                                                  ModelSurfaceMontage* surfaceMontageModel,
                                                  const int32_t viewport[4])
{
    GLint savedVP[4];
    glGetIntegerv(GL_VIEWPORT, savedVP);
    
    const int32_t tabIndex = browserTabContent->getTabNumber();
    
    std::vector<Surface*> rowOne;
    if (surfaceMontageModel->isFirstSurfaceEnabled(tabIndex)) {
        if (surfaceMontageModel->isLeftEnabled(tabIndex)) {
            Surface* s = surfaceMontageModel->getLeftSurfaceSelectionModel(tabIndex)->getSurface();
            if (s != NULL) {
                rowOne.push_back(s);
            }
        }
        if (surfaceMontageModel->isRightEnabled(tabIndex)) {
            Surface* s = surfaceMontageModel->getRightSurfaceSelectionModel(tabIndex)->getSurface();
            if (s != NULL) {
                rowOne.push_back(s);
            }
        }
    }
    std::vector<Surface*> rowTwo;
    if (surfaceMontageModel->isSecondSurfaceEnabled(tabIndex)) {
        if (surfaceMontageModel->isLeftEnabled(tabIndex)) {
            Surface* s = surfaceMontageModel->getLeftSecondSurfaceSelectionModel(tabIndex)->getSurface();
            if (s != NULL) {
                rowTwo.push_back(s);
            }
        }
        if (surfaceMontageModel->isRightEnabled(tabIndex)) {
            Surface* s = surfaceMontageModel->getRightSecondSurfaceSelectionModel(tabIndex)->getSurface();
            if (s != NULL) {
                rowTwo.push_back(s);
            }
        }
    }
    
    const int32_t numberOfSurfaces = static_cast<int32_t>(rowOne.size() + rowTwo.size());
    if (rowOne.empty()) {
        rowOne = rowTwo;
        rowTwo.clear();
    }
    
    std::vector<SurfaceMontageViewport> montageViewports;
    
    /*
     * If the first row contains two surfaces and the second row contains
     * zero surfaces, this is a special case for two structures with
     * one surface each.
     */
    if ((rowOne.size() == 2) && (rowTwo.size() == 0)) {
        const int32_t numRows = 2;
        const int32_t numCols = 2;
        const int32_t vpSizeX = viewport[2] / numCols;
        const int32_t vpSizeY = viewport[3] / numRows;
        
        for (int32_t ivp = 0; ivp < 2; ivp++) {
            Surface* surface = rowOne[ivp];
            
            if (surface->getStructure() == StructureEnum::CORTEX_RIGHT) {
                /*
                 * Lateral view of surface
                 */
                SurfaceMontageViewport lateralVP(surface,
                                                 Model::VIEWING_TRANSFORM_SURFACE_MONTAGE_RIGHT,
                                                 viewport[0] + vpSizeX,
                                                 viewport[1] + vpSizeY,
                                                 vpSizeX,
                                                 vpSizeY);
                montageViewports.push_back(lateralVP);
                
                /*
                 * Medial view of surface
                 */
                SurfaceMontageViewport medialVP(surface,
                                                Model::VIEWING_TRANSFORM_SURFACE_MONTAGE_RIGHT_OPPOSITE,
                                                viewport[0] + vpSizeX,
                                                viewport[1],
                                                vpSizeX,
                                                vpSizeY);
                montageViewports.push_back(medialVP);
            }
            else if (surface->getStructure() == StructureEnum::CORTEX_LEFT) {
                /*
                 * Lateral view of surface
                 */
                SurfaceMontageViewport lateralVP(surface,
                                                 Model::VIEWING_TRANSFORM_NORMAL,
                                                 viewport[0],
                                                 viewport[1] + vpSizeY,
                                                 vpSizeX,
                                                 vpSizeY);
                montageViewports.push_back(lateralVP);
                
                /*
                 * Medial view of surface
                 */
                SurfaceMontageViewport medialVP(surface,
                                                Model::VIEWING_TRANSFORM_SURFACE_MONTAGE_LEFT_OPPOSITE,
                                                viewport[0],
                                                viewport[1],
                                                vpSizeX,
                                                vpSizeY);
                montageViewports.push_back(medialVP);
            }
            else {
                CaretLogWarning("Surface is neither left nor right in montage: "
                                + surface->getFileNameNoPath());
                continue;
            }
        }
    }
    else {
        /*
         * Number of rows to display.  Use only one row when there is only
         * one surface to display;
         */
        const int32_t numRows = (numberOfSurfaces == 1) ? 1 : 2;
        const int32_t vpSizeY = viewport[3] / numRows;
        int32_t vpY = viewport[1];
        
        /*
         * Note that row one is on top and row two is on bottom.
         * Note that OpenGL has origin in bottom left corner.
         */
        for (int32_t iRow = 0; iRow < 2; iRow++) {
            std::vector<Surface*> rowContent = (iRow == 0) ? rowTwo : rowOne;
            
            const int32_t numSurfaces = static_cast<int32_t>(rowContent.size());
            if (numSurfaces > 0) {
                int32_t vpX = viewport[0];
                
                for (int32_t i = 0; i < numSurfaces; i++) {
                    const int32_t numCols = (numSurfaces * 2);
                    int32_t vpSizeX = viewport[2] / numCols;
                    
                    Surface* surface = rowContent[i];
                    Model::ViewingTransformIndex lateralView = Model::VIEWING_TRANSFORM_NORMAL;
                    Model::ViewingTransformIndex medialView = Model::VIEWING_TRANSFORM_SURFACE_MONTAGE_LEFT_OPPOSITE;
                    if (surface->getStructure() == StructureEnum::CORTEX_RIGHT) {
                        lateralView = Model::VIEWING_TRANSFORM_SURFACE_MONTAGE_RIGHT;
                        medialView  = Model::VIEWING_TRANSFORM_SURFACE_MONTAGE_RIGHT_OPPOSITE;
                    }
                    else if (surface->getStructure() != StructureEnum::CORTEX_LEFT) {
                        CaretLogWarning("Surface is neither left nor right in montage: "
                                        + surface->getFileNameNoPath());
                        continue;
                    }
                    
                    /*
                     * Lateral view of surface
                     */
                    SurfaceMontageViewport lateralVP(surface,
                                       lateralView,
                                       vpX,
                                       vpY,
                                       vpSizeX,
                                       vpSizeY);
                    montageViewports.push_back(lateralVP);
                    vpX += vpSizeX;
                    
                    /*
                     * Medial view of surface
                     */
                    SurfaceMontageViewport medialVP(surface,
                                       medialView,
                                       vpX,
                                       vpY,
                                       vpSizeX,
                                       vpSizeY);
                    montageViewports.push_back(medialVP);
                    vpX += vpSizeX;
                }
                
                vpY += vpSizeY;
            }
        }
    }
    
    const int32_t numberOfViewports = static_cast<int32_t>(montageViewports.size());
    for (int32_t ivp = 0; ivp < numberOfViewports; ivp++) {
        const SurfaceMontageViewport& mvp = montageViewports[ivp];
        const float* nodeColoringRGBA = this->surfaceNodeColoring->colorSurfaceNodes(surfaceMontageModel,
                                                                                     mvp.surface,
                                                                                     this->windowTabIndex);
        float center[3];
        mvp.surface->getBoundingBox()->getCenter(center);
        
        /*
         * Left surface view
         */
        this->setViewportAndOrthographicProjection(mvp.viewport,
                                                   mvp.viewingMatrixIndex);
        
        this->applyViewingTransformations(surfaceMontageModel,
                                          this->windowTabIndex,
                                          center,
                                          mvp.viewingMatrixIndex);
        this->drawSurface(mvp.surface,
                          nodeColoringRGBA);
        
    }
    
    surfaceMontageModel->setMontageViewports(this->windowTabIndex,
                                             montageViewports);
    
//    int32_t surfaceCount = 0;
//    if (surfaceMontageModel->isRightEnabled(tabIndex)) {
//        if (surfaceMontageModel->isFirstSurfaceEnabled(tabIndex)) {
//            rightSurface = surfaceMontageModel->getRightSurfaceSelectionModel(tabIndex)->getSurface();
//        }
//        if (surfaceMontageModel->isSecondSurfaceEnabled(tabIndex)) {
//            rightSecondSurface = surfaceMontageModel->getRightSecondSurfaceSelectionModel(tabIndex)->getSurface();
//        }
//    }
//    if (surfaceMontageModel->isLeftEnabled(tabIndex)) {
//        if (surfaceMontageModel->isFirstSurfaceEnabled(tabIndex)) {
//            leftSurface = surfaceMontageModel->getLeftSurfaceSelectionModel(tabIndex)->getSurface();
//        }
//        if (surfaceMontageModel->isSecondSurfaceEnabled(tabIndex)) {
//            leftSecondSurface = surfaceMontageModel->getLeftSecondSurfaceSelectionModel(tabIndex)->getSurface();
//        }
//    }
//    
//    const bool haveLeft  = (leftSurface != NULL) || (leftSecondSurface != NULL);
//    const bool haveBothLeft = (leftSurface != NULL) && (leftSecondSurface != NULL);
//    const bool haveRight = (rightSurface != NULL) || (rightSecondSurface != NULL);
//    const bool haveBothRight = (rightSurface != NULL) && (rightSecondSurface != NULL);
//    const bool haveBoth  = (haveLeft && haveRight);
//    const bool haveAll = haveBothLeft && haveBothRight;
//    //const bool haveAny   = (haveLeft || haveRight);//unused, commenting out to prevent compiler warning
//    
//    int leftLateralVP[4];
//    int leftMedialVP[4];
//    int leftSecondLateralVP[4];
//    int leftSecondMedialVP[4];
//    
//    int rightLateralVP[4];
//    int rightMedialVP[4];
//    int rightSecondLateralVP[4];
//    int rightSecondMedialVP[4];
//    
//    /*
//     * Is EVERYTHING displayed?
//     */
//    if ((rowOne.size() == 2)
//        && (rowTwo.size() == 2)) {
//        const int sizeX = viewport[2] / 4;
//        const int sizeY = viewport[3] / 2;
//        
//        leftLateralVP[0] = viewport[0];
//        leftLateralVP[1] = viewport[1] + sizeY;
//        leftLateralVP[2] = sizeX;
//        leftLateralVP[3] = sizeY;
//        
//        leftMedialVP[0] = viewport[0] + sizeX;
//        leftMedialVP[1] = viewport[1] + sizeY;
//        leftMedialVP[2] = sizeX;
//        leftMedialVP[3] = sizeY;        
//        
//        leftSecondLateralVP[0] = viewport[0];
//        leftSecondLateralVP[1] = viewport[1];
//        leftSecondLateralVP[2] = sizeX;
//        leftSecondLateralVP[3] = sizeY;
//        
//        leftSecondMedialVP[0] = viewport[0] + sizeX;
//        leftSecondMedialVP[1] = viewport[1];
//        leftSecondMedialVP[2] = sizeX;
//        leftSecondMedialVP[3] = sizeY;
//        
//        
//        rightLateralVP[0] = viewport[0] + sizeX * 2;
//        rightLateralVP[1] = viewport[1] + sizeY;
//        rightLateralVP[2] = sizeX;
//        rightLateralVP[3] = sizeY;
//        
//        rightMedialVP[0] = viewport[0] + sizeX * 3;
//        rightMedialVP[1] = viewport[1] + sizeY;
//        rightMedialVP[2] = sizeX;
//        rightMedialVP[3] = sizeY;
//        
//        rightSecondLateralVP[0] = viewport[0] + sizeX * 2;
//        rightSecondLateralVP[1] = viewport[1];
//        rightSecondLateralVP[2] = sizeX;
//        rightSecondLateralVP[3] = sizeY;
//        
//        rightSecondMedialVP[0] = viewport[0] + sizeX * 3;
//        rightSecondMedialVP[1] = viewport[1];
//        rightSecondMedialVP[2] = sizeX;
//        rightSecondMedialVP[3] = sizeY;
//    }
//    else {
//        
//    }
//    int vpSizeX = viewport[2];
//    if (haveBoth) {
//        vpSizeX /= 2;
//    }
//    int vpSizeY = viewport[3] / 2;
//    const bool isDualDisplay = surfaceMontageModel->isDualConfigurationEnabled(tabIndex);
//    if (isDualDisplay) {
//        vpSizeX /= 2;
//    }
//    else {
//        leftSecondSurface  = NULL;
//        rightSecondSurface = NULL;
//    }
//    
//    int vpLeftSizeX = 0;
//    if (haveLeft) {
//        vpLeftSizeX = vpSizeX;
//    }
//    int vpRightSizeX = 0;
//    if (haveRight) {
//        vpRightSizeX = vpSizeX;
//    }
//    
//    int sizeXCol1 = 0;
//    int sizeXCol2 = 0;
//    int sizeXCol3 = 0;
//    int sizeXCol4 = 0;
//    if (haveLeft) {
//        sizeXCol1 = vpLeftSizeX;
//        if (isDualDisplay) {
//            sizeXCol2 = vpLeftSizeX;
//        }
//    }
//    if (haveRight) {
//        if (isDualDisplay) {
//            sizeXCol3 = vpRightSizeX;
//            sizeXCol4 = vpRightSizeX;
//        }
//        else {
//            sizeXCol2 = vpRightSizeX;
//        }
//    }
//    
//    /*
//     * Viewports for surfaces
//     * Row 1 is bottom
//     * Column 1 is left
//     */
//    const int vpRow1Col1[4] = { viewport[0],               viewport[1],           sizeXCol1, vpSizeY };
//    const int vpRow1Col2[4] = { vpRow1Col1[0] + sizeXCol1, viewport[1],           sizeXCol2, vpSizeY };
//    const int vpRow1Col3[4] = { vpRow1Col2[0] + sizeXCol2, viewport[1],           sizeXCol3, vpSizeY };
//    const int vpRow1Col4[4] = { vpRow1Col3[0] + sizeXCol3, viewport[1],           sizeXCol4, vpSizeY };
//    const int vpRow2Col1[4] = { viewport[0],               viewport[1] + vpSizeY, sizeXCol1, vpSizeY };
//    const int vpRow2Col2[4] = { vpRow2Col1[0] + sizeXCol1, viewport[1] + vpSizeY, sizeXCol2, vpSizeY };
//    const int vpRow2Col3[4] = { vpRow2Col2[0] + sizeXCol2, viewport[1] + vpSizeY, sizeXCol3, vpSizeY };
//    const int vpRow2Col4[4] = { vpRow2Col3[0] + sizeXCol3, viewport[1] + vpSizeY, sizeXCol4, vpSizeY };
//    
////    const int vpRow1Col1[4] = { viewport[0], viewport[1], vpSizeX, vpSizeY };
////    const int vpRow1Col2[4] = { viewport[0] + vpSizeX, viewport[1], vpSizeX, vpSizeY };
////    const int vpRow1Col3[4] = { viewport[0] + vpSizeX * 2, viewport[1], vpSizeX, vpSizeY };
////    const int vpRow1Col4[4] = { viewport[0] + vpSizeX * 3, viewport[1], vpSizeX, vpSizeY };
////    const int vpRow2Col1[4] = { viewport[0], viewport[1] + vpSizeY, vpSizeX, vpSizeY };
////    const int vpRow2Col2[4] = { viewport[0] + vpSizeX, viewport[1] + vpSizeY, vpSizeX, vpSizeY };
////    const int vpRow2Col3[4] = { viewport[0] + vpSizeX * 2, viewport[1] + vpSizeY, vpSizeX, vpSizeY };
////    const int vpRow2Col4[4] = { viewport[0] + vpSizeX * 3, viewport[1] + vpSizeY, vpSizeX, vpSizeY };
//    
//    /*
//     * Is DUAL CONFIGURATION Enabled?
//     */
//    if (isDualDisplay) {
//        if (leftSurface != NULL) {
//            const float* nodeColoringRGBA = this->surfaceNodeColoring->colorSurfaceNodes(surfaceMontageModel, 
//                                                                                         leftSurface, 
//                                                                                         this->windowTabIndex);
//            float center[3];
//            leftSurface->getBoundingBox()->getCenter(center);
//            
//            /*
//             * Left surface view
//             */
//            this->setViewportAndOrthographicProjection(vpRow2Col1,
//                                                       Model::VIEWING_TRANSFORM_NORMAL);
//            
//            this->applyViewingTransformations(surfaceMontageModel, 
//                                              this->windowTabIndex,
//                                              center,
//                                              Model::VIEWING_TRANSFORM_NORMAL);
//            this->drawSurface(leftSurface,
//                              nodeColoringRGBA);
//            
//            
//            /*
//             * Left Surface lateral/medial view
//             */
//            this->setViewportAndOrthographicProjection(vpRow2Col2,
//                                                       Model::VIEWING_TRANSFORM_SURFACE_MONTAGE_LEFT_OPPOSITE);
//            
//            this->applyViewingTransformations(surfaceMontageModel, 
//                                              this->windowTabIndex,
//                                              center,
//                                              Model::VIEWING_TRANSFORM_SURFACE_MONTAGE_LEFT_OPPOSITE);
//            this->drawSurface(leftSurface,
//                              nodeColoringRGBA);
//            
//            if (leftSecondSurface != NULL) {
//                /*
//                 * Left surface view
//                 */
//                leftSecondSurface->getBoundingBox()->getCenter(center);
//                this->setViewportAndOrthographicProjection(vpRow1Col1,
//                                                           Model::VIEWING_TRANSFORM_NORMAL);
//                
//                this->applyViewingTransformations(surfaceMontageModel, 
//                                                  this->windowTabIndex,
//                                                  center,
//                                                  Model::VIEWING_TRANSFORM_NORMAL);
//                this->drawSurface(leftSecondSurface,
//                                  nodeColoringRGBA);
//                
//                
//                /*
//                 * Left Surface lateral/medial view
//                 */
//                this->setViewportAndOrthographicProjection(vpRow1Col2,
//                                                           Model::VIEWING_TRANSFORM_SURFACE_MONTAGE_LEFT_OPPOSITE);
//                
//                this->applyViewingTransformations(surfaceMontageModel, 
//                                                  this->windowTabIndex,
//                                                  center,
//                                                  Model::VIEWING_TRANSFORM_SURFACE_MONTAGE_LEFT_OPPOSITE);
//                this->drawSurface(leftSecondSurface,
//                                  nodeColoringRGBA);
//            }
//        }
//        
//        if (rightSurface != NULL) {
//            const float* nodeColoringRGBA = this->surfaceNodeColoring->colorSurfaceNodes(surfaceMontageModel, 
//                                                                                         rightSurface, 
//                                                                                         this->windowTabIndex);
//            /*
//             * Right Surface
//             */
//            float center[3];
//            rightSurface->getBoundingBox()->getCenter(center);
//            
//            this->setViewportAndOrthographicProjection(vpRow2Col3,
//                                                       Model::VIEWING_TRANSFORM_SURFACE_MONTAGE_RIGHT); //VIEWING_TRANSFORM_RIGHT_LATERAL_MEDIAL_YOKED);
//            
//            this->applyViewingTransformations(surfaceMontageModel, 
//                                              this->windowTabIndex,
//                                              center,
//                                              Model::VIEWING_TRANSFORM_SURFACE_MONTAGE_RIGHT); // VIEWING_TRANSFORM_RIGHT_LATERAL_MEDIAL_YOKED);
//            this->drawSurface(rightSurface,
//                              nodeColoringRGBA);
//            
//            /*
//             * Right Surface lateral/medial view
//             */
//            this->setViewportAndOrthographicProjection(vpRow2Col4,
//                                                       Model::VIEWING_TRANSFORM_SURFACE_MONTAGE_RIGHT_OPPOSITE);
//            
//            this->applyViewingTransformations(surfaceMontageModel, 
//                                              this->windowTabIndex,
//                                              center,
//                                              Model::VIEWING_TRANSFORM_SURFACE_MONTAGE_RIGHT_OPPOSITE);
//            this->drawSurface(rightSurface,
//                              nodeColoringRGBA);
//            
//            if (rightSecondSurface != NULL) {
//                /*
//                 * Left surface view
//                 */
//                rightSecondSurface->getBoundingBox()->getCenter(center);
//                this->setViewportAndOrthographicProjection(vpRow1Col3,
//                                                           Model::VIEWING_TRANSFORM_SURFACE_MONTAGE_RIGHT);
//                
//                this->applyViewingTransformations(surfaceMontageModel, 
//                                                  this->windowTabIndex,
//                                                  center,
//                                                  Model::VIEWING_TRANSFORM_SURFACE_MONTAGE_RIGHT);
//                this->drawSurface(rightSecondSurface,
//                                  nodeColoringRGBA);
//                
//                
//                /*
//                 * Left Surface lateral/medial view
//                 */
//                this->setViewportAndOrthographicProjection(vpRow1Col4,
//                                                           Model::VIEWING_TRANSFORM_SURFACE_MONTAGE_RIGHT_OPPOSITE);
//                
//                this->applyViewingTransformations(surfaceMontageModel, 
//                                                  this->windowTabIndex,
//                                                  center,
//                                                  Model::VIEWING_TRANSFORM_SURFACE_MONTAGE_RIGHT_OPPOSITE);
//                this->drawSurface(rightSecondSurface,
//                                  nodeColoringRGBA);
//            }
//        }
//    }
//    else {
//        /*
//         * NOT DUAL CONFIGURATION
//         */
//        if (leftSurface != NULL) {
//            const float* nodeColoringRGBA = this->surfaceNodeColoring->colorSurfaceNodes(surfaceMontageModel, 
//                                                                                         leftSurface, 
//                                                                                         this->windowTabIndex);
//            float center[3];
//            leftSurface->getBoundingBox()->getCenter(center);
//            
//            /*
//             * Left surface view
//             */
//            this->setViewportAndOrthographicProjection(vpRow2Col1,
//                                                       Model::VIEWING_TRANSFORM_NORMAL);
//            
//            this->applyViewingTransformations(surfaceMontageModel, 
//                                              this->windowTabIndex,
//                                              center,
//                                              Model::VIEWING_TRANSFORM_NORMAL);
//            this->drawSurface(leftSurface,
//                              nodeColoringRGBA);
//            
//            
//            /*
//             * Left Surface lateral/medial view
//             */
//            this->setViewportAndOrthographicProjection(vpRow1Col1,
//                                                       Model::VIEWING_TRANSFORM_SURFACE_MONTAGE_LEFT_OPPOSITE);
//            
//            this->applyViewingTransformations(surfaceMontageModel, 
//                                              this->windowTabIndex,
//                                              center,
//                                              Model::VIEWING_TRANSFORM_SURFACE_MONTAGE_LEFT_OPPOSITE);
//            this->drawSurface(leftSurface,
//                              nodeColoringRGBA);
//        }
//        
//        if (rightSurface != NULL) {
//            const float* nodeColoringRGBA = this->surfaceNodeColoring->colorSurfaceNodes(surfaceMontageModel, 
//                                                                                         rightSurface, 
//                                                                                         this->windowTabIndex);
//            /*
//             * Right Surface
//             */
//            float center[3];
//            rightSurface->getBoundingBox()->getCenter(center);
//            
//            this->setViewportAndOrthographicProjection(vpRow2Col2,
//                                                       Model::VIEWING_TRANSFORM_SURFACE_MONTAGE_RIGHT); //VIEWING_TRANSFORM_RIGHT_LATERAL_MEDIAL_YOKED);
//            
//            this->applyViewingTransformations(surfaceMontageModel, 
//                                              this->windowTabIndex,
//                                              center,
//                                              Model::VIEWING_TRANSFORM_SURFACE_MONTAGE_RIGHT); // VIEWING_TRANSFORM_RIGHT_LATERAL_MEDIAL_YOKED);
//            this->drawSurface(rightSurface,
//                              nodeColoringRGBA);
//            
//            /*
//             * Right Surface lateral/medial view
//             */
//            this->setViewportAndOrthographicProjection(vpRow1Col2,
//                                                       Model::VIEWING_TRANSFORM_SURFACE_MONTAGE_RIGHT_OPPOSITE);
//            
//            this->applyViewingTransformations(surfaceMontageModel, 
//                                              this->windowTabIndex,
//                                              center,
//                                              Model::VIEWING_TRANSFORM_SURFACE_MONTAGE_RIGHT_OPPOSITE);
//            this->drawSurface(rightSurface,
//                              nodeColoringRGBA);
//        }
//    }
    
    glViewport(savedVP[0], 
               savedVP[1], 
               savedVP[2], 
               savedVP[3]);
}


/**
 * Draw the whole brain.
 * @param browserTabContent
 *    Content of the window.
 * @param wholeBrainController
 *    Controller for whole brain.
 * @param viewport
 *    Region for drawing.
 */
void 
BrainOpenGLFixedPipeline::drawWholeBrainController(BrowserTabContent* browserTabContent,
                                      ModelWholeBrain* wholeBrainController,
                                      const int32_t viewport[4])
{
    const int32_t tabNumberIndex = browserTabContent->getTabNumber();
    
    /*
     * Center using volume, if it is available
     * Otherwise, see if surface is available, but a surface is offset
     * from center so override the X-coordinate to zero.
     */
    float center[3] = { 0.0, 0.0, 0.0 };
    VolumeFile* underlayVolumeFile = wholeBrainController->getUnderlayVolumeFile(tabNumberIndex);
    if (underlayVolumeFile != NULL) {
        const BoundingBox volumeBoundingBox = underlayVolumeFile->getSpaceBoundingBox();
        volumeBoundingBox.getCenter(center);
    }
    else {
        Surface* leftSurface = wholeBrainController->getSelectedSurface(StructureEnum::CORTEX_LEFT,
                                                                       tabNumberIndex);
        if (leftSurface != NULL) {
            leftSurface->getBoundingBox()->getCenter(center);
            center[0] = 0.0;
        }
        else {
            Surface* rightSurface = wholeBrainController->getSelectedSurface(StructureEnum::CORTEX_RIGHT,
                                                                            tabNumberIndex);
            if (rightSurface != NULL) {
                rightSurface->getBoundingBox()->getCenter(center);
                center[0] = 0.0;
            }
        }
    }
    
    this->setViewportAndOrthographicProjection(viewport,
                                               Model::VIEWING_TRANSFORM_NORMAL);
    this->applyViewingTransformations(wholeBrainController,
                                      this->windowTabIndex,
                                      center,
                                      Model::VIEWING_TRANSFORM_NORMAL);
    
    const SurfaceTypeEnum::Enum surfaceType = wholeBrainController->getSelectedSurfaceType(tabNumberIndex);
    
    /*
     * Draw the surfaces. 
     */
    Brain* brain = wholeBrainController->getBrain();
    const int32_t numberOfBrainStructures = brain->getNumberOfBrainStructures();
    for (int32_t i = 0; i < numberOfBrainStructures; i++) {
        BrainStructure* brainStructure = brain->getBrainStructure(i);
        const StructureEnum::Enum structure = brainStructure->getStructure();
        Surface* surface = wholeBrainController->getSelectedSurface(structure, 
                                                                    tabNumberIndex);
        if (surface != NULL) {
            float dx = 0.0;
            float dy = 0.0;
            float dz = 0.0;
            
            bool drawIt = false;
            switch (structure) {
                case StructureEnum::CORTEX_LEFT:
                    drawIt = wholeBrainController->isLeftEnabled(tabNumberIndex);
                    dx = -wholeBrainController->getLeftRightSeparation(tabNumberIndex);
                    if ((surfaceType != SurfaceTypeEnum::ANATOMICAL)
                        && (surfaceType != SurfaceTypeEnum::RECONSTRUCTION)) {
                        dx -= surface->getBoundingBox()->getMaxX();
                    }
                    break;
                case StructureEnum::CORTEX_RIGHT:
                    drawIt = wholeBrainController->isRightEnabled(tabNumberIndex);
                    dx = wholeBrainController->getLeftRightSeparation(tabNumberIndex);
                    if ((surfaceType != SurfaceTypeEnum::ANATOMICAL)
                        && (surfaceType != SurfaceTypeEnum::RECONSTRUCTION)) {
                        dx -= surface->getBoundingBox()->getMinX();
                    }
                    break;
                case StructureEnum::CEREBELLUM:
                    drawIt = wholeBrainController->isCerebellumEnabled(tabNumberIndex);
                    dz = wholeBrainController->getCerebellumSeparation(tabNumberIndex);
                    break;
                default:
                    CaretLogWarning("programmer-issure: Surface type not left/right/cerebellum");
                    break;
            }
            
            const float* nodeColoringRGBA = this->surfaceNodeColoring->colorSurfaceNodes(wholeBrainController, 
                                                                                         surface, 
                                                                                         this->windowTabIndex);
            
            if (drawIt) {
                glPushMatrix();
                glTranslatef(dx, dy, dz);
                this->drawSurface(surface,
                                  nodeColoringRGBA);
                glPopMatrix();
            }
        }
    }

//    if (surfaceType == SurfaceTypeEnum::ANATOMICAL) {
//        drawSurfaceFibers();
//    }

    /*
     * Need depth testing for drawing slices
     */
    glEnable(GL_DEPTH_TEST);

    /*
     * Determine volumes that are to be drawn  
     */
    //VolumeFile* underlayVolumeFile = wholeBrainController->getUnderlayVolumeFile(tabNumberIndex);
    if (underlayVolumeFile != NULL) {
        std::vector<VolumeDrawInfo> volumeDrawInfo;
        this->setupVolumeDrawInfo(browserTabContent,
                                  brain,
                                  volumeDrawInfo);
        if (volumeDrawInfo.empty() == false) {
            const VolumeSliceCoordinateSelection* slices = 
            wholeBrainController->getSelectedVolumeSlices(tabNumberIndex);
            if (slices->isSliceAxialEnabled()) {
                this->drawVolumeOrthogonalSlice(VolumeSliceViewPlaneEnum::AXIAL, 
                                                slices->getSliceIndexAxial(underlayVolumeFile), 
                                                volumeDrawInfo);
                this->drawVolumeSurfaceOutlines(brain, 
                                                wholeBrainController,
                                                browserTabContent,
                                                VolumeSliceViewPlaneEnum::AXIAL, 
                                                slices->getSliceIndexAxial(underlayVolumeFile), 
                                                volumeDrawInfo[0].volumeFile);
            }
            if (slices->isSliceCoronalEnabled()) {
                this->drawVolumeOrthogonalSlice(VolumeSliceViewPlaneEnum::CORONAL, 
                                                slices->getSliceIndexCoronal(underlayVolumeFile), 
                                                volumeDrawInfo);
                this->drawVolumeSurfaceOutlines(brain, 
                                                wholeBrainController,
                                                browserTabContent,
                                                VolumeSliceViewPlaneEnum::CORONAL, 
                                                slices->getSliceIndexCoronal(underlayVolumeFile), 
                                                volumeDrawInfo[0].volumeFile);
            }
            if (slices->isSliceParasagittalEnabled()) {
                this->drawVolumeOrthogonalSlice(VolumeSliceViewPlaneEnum::PARASAGITTAL, 
                                                slices->getSliceIndexParasagittal(underlayVolumeFile), 
                                                volumeDrawInfo);
                this->drawVolumeSurfaceOutlines(brain, 
                                                wholeBrainController,
                                                browserTabContent,
                                                VolumeSliceViewPlaneEnum::PARASAGITTAL, 
                                                slices->getSliceIndexParasagittal(underlayVolumeFile), 
                                                volumeDrawInfo[0].volumeFile);
            }
        }
    }
    if (surfaceType == SurfaceTypeEnum::ANATOMICAL) {
        drawSurfaceFiberOrientations();
        drawSurfaceFiberTrajectories();
    }
}

/**
 * Setup the orthographic projection.
 * @param viewport
 *    The viewport (x, y, width, height)
 * @param rotationMatrixIndex
 *    Viewing rotation matrix index.
 */
void 
BrainOpenGLFixedPipeline::setOrthographicProjection(const int32_t viewport[4],
                                        const Model::ViewingTransformIndex rotationMatrixIndex)
{
    double defaultOrthoWindowSize = BrainOpenGLFixedPipeline::getModelViewingHalfWindowHeight();
    double width = viewport[2];
    double height = viewport[3];
    double aspectRatio = (width / height);
    this->orthographicRight  =    defaultOrthoWindowSize * aspectRatio;
    this->orthographicLeft   =   -defaultOrthoWindowSize * aspectRatio;
    this->orthographicTop    =    defaultOrthoWindowSize;
    this->orthographicBottom =   -defaultOrthoWindowSize;
    this->orthographicNear   = -1000.0; //-500.0; //-10000.0;
    this->orthographicFar    =  1000.0; //500.0; // 10000.0;
    
    //bool viewingFromFarSide = false;//unused, commenting out to prevent compiler warning
    
    switch (rotationMatrixIndex) {
        case Model::VIEWING_TRANSFORM_NORMAL:
            glOrtho(this->orthographicLeft, this->orthographicRight, 
                    this->orthographicBottom, this->orthographicTop, 
                    this->orthographicNear, this->orthographicFar);            
            break;
        case Model::VIEWING_TRANSFORM_SURFACE_MONTAGE_LEFT_OPPOSITE:
            glOrtho(this->orthographicRight, this->orthographicLeft, 
                    this->orthographicBottom, this->orthographicTop, 
                    this->orthographicFar, this->orthographicNear); 
            //viewingFromFarSide = true;
            break;
        case Model::VIEWING_TRANSFORM_SURFACE_MONTAGE_RIGHT:
            glOrtho(this->orthographicRight, this->orthographicLeft, 
                    this->orthographicBottom, this->orthographicTop, 
                    this->orthographicFar, this->orthographicNear);    
            //viewingFromFarSide = true;
            break;
        case Model::VIEWING_TRANSFORM_SURFACE_MONTAGE_RIGHT_OPPOSITE:
            glOrtho(this->orthographicLeft, this->orthographicRight, 
                    this->orthographicBottom, this->orthographicTop, 
                    this->orthographicNear, this->orthographicFar);            
            break;
        case Model::VIEWING_TRANSFORM_RIGHT_LATERAL_MEDIAL_YOKED:
            glOrtho(this->orthographicRight, this->orthographicLeft, 
                    this->orthographicBottom, this->orthographicTop, 
                    this->orthographicFar, this->orthographicNear);    
            //viewingFromFarSide = true;
            break;
        case Model::VIEWING_TRANSFORM_COUNT:
            CaretAssert(0);
            break;
    }
    
    
}

/**
 * check for an OpenGL Error.
 */
void 
BrainOpenGLFixedPipeline::checkForOpenGLError(const Model* modelController,
                                      const AString& msgIn)
{
    GLenum errorCode = glGetError();
    if (errorCode != GL_NO_ERROR) {
        AString msg;
        if (msgIn.isEmpty() == false) {
            msg += (msgIn + "\n");
        }
        msg += ("OpenGL Error: " + AString((char*)gluErrorString(errorCode)) + "\n");
        msg += ("OpenGL Version: " + AString((char*)glGetString(GL_VERSION)) + "\n");
        msg += ("OpenGL Vendor:  " + AString((char*)glGetString(GL_VENDOR)) + "\n");
        if (modelController != NULL) {
            msg += ("While drawing brain model " + modelController->getNameForGUI(true) + "\n");
        }
        msg += ("In tab number " + AString::number(this->windowTabIndex) + "\n");
        GLint nameStackDepth, modelStackDepth, projStackDepth;
        glGetIntegerv(GL_PROJECTION_STACK_DEPTH,
                      &projStackDepth);
        glGetIntegerv(GL_MODELVIEW_STACK_DEPTH,
                      &modelStackDepth);
        glGetIntegerv(GL_NAME_STACK_DEPTH,
                      &nameStackDepth);
        msg += ("Projection Matrix Stack Depth " + AString::number(projStackDepth) + "\n");
        msg += ("Model Matrix Stack Depth " + AString::number(modelStackDepth) + "\n");
        msg += ("Name Matrix Stack Depth " + AString::number(nameStackDepth) + "\n");
        CaretLogSevere(msg);
    }
}

/**
 * Analyze color information to extract identification data.
 * @param dataType
 *    Type of data.
 * @param x
 *    X-coordinate of identification.
 * @param y
 *    X-coordinate of identification.
 * @param indexOut
 *    Index of selected item.
 * @param depthOut
 *    Depth of selected item.
 */
void
BrainOpenGLFixedPipeline::getIndexFromColorSelection(SelectionItemDataTypeEnum::Enum dataType,
                                        const int32_t x,
                                        const int32_t y,
                                        int32_t& indexOut,
                                        float& depthOut)
{
    // Figure out item was picked using color in color buffer
    //
    glReadBuffer(GL_BACK);
    glPixelStorei(GL_PACK_SKIP_ROWS, 0);
    glPixelStorei(GL_PACK_SKIP_PIXELS, 0);
    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    uint8_t pixels[3];
    glReadPixels((int)x,
                    (int)y,
                    1,
                    1,
                    GL_RGB,
                    GL_UNSIGNED_BYTE,
                    pixels);
    
    indexOut = -1;
    depthOut = -1.0;
    
    CaretLogFine("ID color is "
                 + QString::number(pixels[0]) + ", "
                 + QString::number(pixels[1]) + ", "
                 + QString::number(pixels[2]));
    
    this->colorIdentification->getItem(pixels, dataType, &indexOut);
    
    if (indexOut >= 0) {
        /*
         * Get depth from depth buffer
         */
        glPixelStorei(GL_PACK_ALIGNMENT, 4);
        glReadPixels(x,
                     y,
                        1,
                        1,
                        GL_DEPTH_COMPONENT,
                        GL_FLOAT,
                        &depthOut);
    }
    this->colorIdentification->reset();
}

/**
 * Analyze color information to extract identification data.
 * @param dataType
 *    Type of data.
 * @param x
 *    X-coordinate of identification.
 * @param y
 *    X-coordinate of identification.
 * @param indexOut1
 *    First index of selected item.
 * @param indexOut2
 *    Second index of selected item.
 * @param depthOut
 *    Depth of selected item.
 */
void
BrainOpenGLFixedPipeline::getIndexFromColorSelection(SelectionItemDataTypeEnum::Enum dataType,
                                                     const int32_t x,
                                                     const int32_t y,
                                                     int32_t& index1Out,
                                                     int32_t& index2Out,
                                                     float& depthOut)
{
    // Figure out item was picked using color in color buffer
    //
    glReadBuffer(GL_BACK);
    glPixelStorei(GL_PACK_SKIP_ROWS, 0);
    glPixelStorei(GL_PACK_SKIP_PIXELS, 0);
    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    uint8_t pixels[3];
    glReadPixels((int)x,
                 (int)y,
                 1,
                 1,
                 GL_RGB,
                 GL_UNSIGNED_BYTE,
                 pixels);
    
    index1Out = -1;
    index2Out = -1;
    depthOut = -1.0;
    
    CaretLogFine("ID color is "
                 + QString::number(pixels[0]) + ", "
                 + QString::number(pixels[1]) + ", "
                 + QString::number(pixels[2]));
    
    this->colorIdentification->getItem(pixels, dataType, &index1Out, &index2Out);
    
    if (index1Out >= 0) {
        /*
         * Get depth from depth buffer
         */
        glPixelStorei(GL_PACK_ALIGNMENT, 4);
        glReadPixels(x,
                     y,
                     1,
                     1,
                     GL_DEPTH_COMPONENT,
                     GL_FLOAT,
                     &depthOut);
    }
    this->colorIdentification->reset();
}

/**
 * Analyze color information to extract identification data.
 * @param dataType
 *    Type of data.
 * @param x
 *    X-coordinate of identification.
 * @param y
 *    X-coordinate of identification.
 * @param indexOut1
 *    First index of selected item.
 * @param indexOut2
 *    Second index of selected item.
 * @param indexOut3
 *    Third index of selected item.
 * @param depthOut
 *    Depth of selected item.
 */
void
BrainOpenGLFixedPipeline::getIndexFromColorSelection(SelectionItemDataTypeEnum::Enum dataType,
                                                     const int32_t x,
                                                     const int32_t y,
                                                     int32_t& index1Out,
                                                     int32_t& index2Out,
                                                     int32_t& index3Out,
                                                     float& depthOut)
{
    // Figure out item was picked using color in color buffer
    //
    glReadBuffer(GL_BACK);
    glPixelStorei(GL_PACK_SKIP_ROWS, 0);
    glPixelStorei(GL_PACK_SKIP_PIXELS, 0);
    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    uint8_t pixels[3];
    glReadPixels((int)x,
                 (int)y,
                 1,
                 1,
                 GL_RGB,
                 GL_UNSIGNED_BYTE,
                 pixels);
    
    index1Out = -1;
    index2Out = -1;
    index3Out = -1;
    depthOut = -1.0;
    
    CaretLogFine("ID color is "
                 + QString::number(pixels[0]) + ", "
                 + QString::number(pixels[1]) + ", "
                 + QString::number(pixels[2]));
    
    this->colorIdentification->getItem(pixels, dataType, &index1Out, &index2Out, &index3Out);
    
    if (index1Out >= 0) {
        /*
         * Get depth from depth buffer
         */
        glPixelStorei(GL_PACK_ALIGNMENT, 4);
        glReadPixels(x,
                     y,
                     1,
                     1,
                     GL_DEPTH_COMPONENT,
                     GL_FLOAT,
                     &depthOut);
    }
    this->colorIdentification->reset();
}

/**
 * Set the selected item's screen coordinates.
 * @param item
 *    Item that has screen coordinates set.
 * @param itemXYZ
 *    Model's coordinate.
 */
void 
BrainOpenGLFixedPipeline::setSelectedItemScreenXYZ(SelectionItem* item,
                                        const float itemXYZ[3])
{
    GLdouble selectionModelviewMatrix[16];
    glGetDoublev(GL_MODELVIEW_MATRIX, selectionModelviewMatrix);
    
    GLdouble selectionProjectionMatrix[16];
    glGetDoublev(GL_PROJECTION_MATRIX, selectionProjectionMatrix);
    
    GLint selectionViewport[4];
    glGetIntegerv(GL_VIEWPORT, selectionViewport);
    
    const double modelXYZ[3] = {
        itemXYZ[0],
        itemXYZ[1],
        itemXYZ[2]
    };
    
    double windowPos[3];
    if (gluProject(modelXYZ[0], 
                   modelXYZ[1], 
                   modelXYZ[2],
                   selectionModelviewMatrix,
                   selectionProjectionMatrix,
                   selectionViewport,
                   &windowPos[0],
                   &windowPos[1],
                   &windowPos[2])) {
        item->setScreenXYZ(windowPos);
        item->setModelXYZ(modelXYZ);
    }
}

/**
 * Draw sphere.
 */
void 
BrainOpenGLFixedPipeline::drawSphere(const double radius)
{
    glPushMatrix();
    glScaled(radius, radius, radius);
    m_shapeSphere->draw();
    glPopMatrix();
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
BrainOpenGLFixedPipeline::drawSquare(const float size)
{
    if (this->inverseRotationMatrixValid) {
        /*
         * Remove any rotation 
         */
        glMultMatrixd(this->inverseRotationMatrix);

        glScalef(size, size, size);
        
        /*
         * Draw both front and back side since in some instances,
         * such as surface montage, we are viweing from the far
         * side (from back of monitor)
         */
        glBegin(GL_QUADS);
        glNormal3f(0.0, 0.0, 1.0);
        glVertex3f(-0.5, -0.5, 0.0);
        glVertex3f( 0.5, -0.5, 0.0);
        glVertex3f( 0.5,  0.5, 0.0);
        glVertex3f(-0.5,  0.5, 0.0);
        glNormal3f(0.0, 0.0, -1.0);
        glVertex3f(-0.5, -0.5, 0.0);
        glVertex3f(-0.5,  0.5, 0.0);
        glVertex3f( 0.5,  0.5, 0.0);
        glVertex3f( 0.5, -0.5, 0.0);
        glEnd();
    }
}

/**
 * Draw text at the given window coordinates.
 * @param windowX
 *    Window X-coordinate.
 * @param windowY
 *    Window Y-coordinate.
 * @param text
 *    Text that is to be drawn.
 * @param alignment
 *    Alignment of text.
 */
void 
BrainOpenGLFixedPipeline::drawTextWindowCoords(const int windowX,
                                               const int windowY,
                                               const QString& text,
                                               const BrainOpenGLTextRenderInterface::TextAlignmentX alignmentX,
                                               const BrainOpenGLTextRenderInterface::TextAlignmentY alignmentY)
{
    if (this->textRenderer != NULL) {
        GLint vp[4];
        glGetIntegerv(GL_VIEWPORT, vp);
        int viewport[4] = {
            vp[0],
            vp[1],
            vp[2],
            vp[3]
        };
        this->textRenderer->drawTextAtWindowCoords(viewport,
                                                   windowX,
                                                   windowY,
                                                   text.trimmed(),
                                                   alignmentX,
                                                   alignmentY);
    }
}

/**
 * Draw text at the given window coordinates.
 * @param modelX
 *    Model X-coordinate.
 * @param modelY
 *    Model Y-coordinate.
 * @param modelZ
 *    Model Z-coordinate.
 * @param text
 *    Text that is to be drawn.
 * @param alignment
 *    Alignment of text.
 */
void 
BrainOpenGLFixedPipeline::drawTextModelCoords(const double modelX,
                                              const double modelY,
                                              const double modelZ,
                                              const QString& text)
{
    if (this->textRenderer != NULL) {
        this->textRenderer->drawTextAtModelCoords(modelX,
                                                  modelY,
                                                  modelZ,
                                                  text.trimmed());
    }
}

/**
 * Draw the palettes showing how scalars are mapped
 * to colors.
 * @param brain
 *    Brain containing model being drawn.
 * @param viewport
 *    Viewport for the model.
 */
void 
BrainOpenGLFixedPipeline::drawAllPalettes(Brain* brain)
{
    /*
     * Turn off depth testing
     */
    glDisable(GL_DEPTH_TEST);
    

    /*
     * Save the projection matrix, model matrix, and viewport.
     */
    glMatrixMode(GL_PROJECTION);
    GLfloat savedProjectionMatrix[16];
    glGetFloatv(GL_PROJECTION_MATRIX, 
                savedProjectionMatrix);
    glMatrixMode(GL_MODELVIEW);
    GLfloat savedModelviewMatrix[16];
    glGetFloatv(GL_MODELVIEW_MATRIX, 
                savedModelviewMatrix);
    GLint savedViewport[4];
    glGetIntegerv(GL_VIEWPORT, 
                  savedViewport);
    
    CaretAssert(brain);
    
    /*
     * Check for a 'selection' type mode
     */
    bool selectFlag = false;
    switch (this->mode) {
        case MODE_DRAWING:
            break;
        case MODE_IDENTIFICATION:
            selectFlag = true;
            break;
        case MODE_PROJECTION:
            return;
            break;
    }
    if (selectFlag) {
        return;
    }
    
    this->disableLighting();
    
    PaletteFile* paletteFile = brain->getPaletteFile();
    CaretAssert(paletteFile);
    
    std::vector<CaretMappableDataFile*> mapFiles;
    std::vector<int32_t> mapIndices;
    this->browserTabContent->getDisplayedPaletteMapFiles(mapFiles, 
                                                         mapIndices);
    
    /*
     * Each map file has a palette drawn to represent the
     * datas mapping to colors.
     */
    const int32_t numMapFiles = static_cast<int32_t>(mapFiles.size());
    for (int32_t i = 0; i < numMapFiles; i++) {
        const int mapIndex = mapIndices[i];
        const PaletteColorMapping* pcm = mapFiles[i]->getMapPaletteColorMapping(mapIndex);
        const AString paletteName = pcm->getSelectedPaletteName();
        const Palette* palette = paletteFile->getPaletteByName(paletteName);
        if (palette != NULL) {
            const FastStatistics* statistics = mapFiles[i]->getMapFastStatistics(mapIndex);
            this->drawPalette(palette, 
                              pcm, 
                              statistics, 
                              i);
        }
        else {
            CaretLogWarning("Palette named "
                            + paletteName
                            + " not found in palette file.");
        }
    }
    
    /*
     * Restore the projection matrix, model matrix, and viewport.
     */
    glMatrixMode(GL_PROJECTION);
    glLoadMatrixf(savedProjectionMatrix);
    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixf(savedModelviewMatrix);
    glViewport(savedViewport[0],
               savedViewport[1],
               savedViewport[2],
               savedViewport[3]);
}

/**
 * Draw a palette.
 * @param palette
 *    Palette that is drawn.
 * @param paletteColorMapping
 *    Controls mapping of data to colors.
 * @param statistics
 *    Statistics describing the data that is mapped to the palette.
 * @param paletteDrawingIndex
 *    Counts number of palettes being drawn for the Y-position
 */
//void 
//BrainOpenGLFixedPipeline::drawPalette(const Palette* palette,
//                                      const PaletteColorMapping* paletteColorMapping,
//                                      const DescriptiveStatistics* statistics,
//                                      const int paletteDrawingIndex)
//{
//    /*
//     * Save viewport.
//     */
//    GLint modelViewport[4];
//    glGetIntegerv(GL_VIEWPORT, modelViewport);
//    
//    /*
//     * Create a viewport for drawing the palettes in the 
//     * lower left corner of the window.  Allow palette width
//     * to increase as window is made larger
//     */
//    const GLint colorbarViewportWidth = std::max(static_cast<GLint>(modelViewport[2] * 0.25), 
//                                                 (GLint)120);
//    const GLint colorbarViewportHeight = 35;    
//    const GLint colorbarViewportX = modelViewport[0] + 10;
//    
//    GLint colorbarViewportY = (modelViewport[1] + 10 + (paletteDrawingIndex * colorbarViewportHeight));
//    if (paletteDrawingIndex > 0) {
////        colorbarViewportY += 5;
//    }
//    
//    glViewport(colorbarViewportX, 
//               colorbarViewportY, 
//               colorbarViewportWidth, 
//               colorbarViewportHeight);
//    
//    /*
//     * Create an orthographic projection
//     */
//    //const GLdouble halfWidth = static_cast<GLdouble>(colorbarViewportWidth / 2);
//    const GLdouble halfHeight = static_cast<GLdouble>(colorbarViewportHeight / 2);
//    const GLdouble margin = 1.1;
//    const GLdouble orthoWidth = margin;
//    const GLdouble orthoHeight = halfHeight * margin;
//    glMatrixMode(GL_PROJECTION);
//    glLoadIdentity();
//    glOrtho(-orthoWidth,  orthoWidth, 
//            -orthoHeight, orthoHeight, 
//            -1.0, 1.0);
//    
//    glMatrixMode (GL_MODELVIEW);
//    glLoadIdentity();
//
//    /*
//     * Use the background color to fill in a rectangle
//     * for display of palette, hiding anything currently drawn.
//     */
//    uint8_t backgroundRGB[3];
//    CaretPreferences* prefs = SessionManager::get()->getCaretPreferences();
//    prefs->getColorBackground(backgroundRGB);
//    glColor3ubv(backgroundRGB);
//    glRectf(-orthoWidth, -orthoHeight, orthoWidth, orthoHeight);
//    
//    /*
//     * Always interpolate if the palette has only two colors
//     */
//    bool interpolateColor = paletteColorMapping->isInterpolatePaletteFlag();
//    if (palette->getNumberOfScalarsAndColors() <= 2) {
//        interpolateColor = true;
//    }
//    
//    /*
//     * Types of values for display
//     */
//    const bool isPositiveDisplayed = paletteColorMapping->isDisplayPositiveDataFlag();
//    const bool isNegativeDisplayed = paletteColorMapping->isDisplayNegativeDataFlag();
//    const bool isZeroDisplayed     = paletteColorMapping->isDisplayZeroDataFlag();
//    
//    /*
//     * Draw the colorbar starting with the color assigned
//     * to the negative end of the palette.
//     * Colorbar scalars range from -1 to 1.
//     */
//    const int iStart = palette->getNumberOfScalarsAndColors() - 1;
//    const int iEnd = 1;
//    const int iStep = -1;
//    for (int i = iStart; i >= iEnd; i += iStep) {
//        /*
//         * palette data for 'left' side of a color in the palette.
//         */
//        const PaletteScalarAndColor* sc = palette->getScalarAndColor(i);
//        float scalar = sc->getScalar();
//        float rgba[4];
//        sc->getColor(rgba);
//        
//        /*
//         * palette data for 'right' side of a color in the palette.
//         */
//        const PaletteScalarAndColor* nextSC = palette->getScalarAndColor(i - 1);
//        float nextScalar = nextSC->getScalar();
//        float nextRGBA[4];
//        nextSC->getColor(nextRGBA);
//        const bool isNoneColorFlag = nextSC->isNoneColor();
//        
//        /*
//         * Exclude negative regions if not displayed.
//         *
//        if (isNegativeDisplayed == false) {
//            if (nextScalar < 0.0) {
//                continue;
//            }
//            else if (scalar < 0.0) {
//                scalar = 0.0;
//            }
//        }
//        */
//        
//        /*
//         * Exclude positive regions if not displayed.
//         *
//        if (isPositiveDisplayed == false) {
//            if (scalar > 0.0) {
//                continue;
//            }
//            else if (nextScalar > 0.0) {
//                nextScalar = 0.0;
//            }
//        }
//        */
//        
//        /*
//         * Normally, the first entry has a scalar value of -1.
//         * If it does not, use the first color draw from 
//         * -1 to the first scalar value.
//         */
//        if (i == iStart) {
//            if (sc->isNoneColor() == false) {
//                if (scalar > -1.0) {
//                    const float xStart = -1.0;
//                    const float xEnd   = scalar;
//                    glColor3fv(rgba);
//                    glBegin(GL_POLYGON);
//                    glVertex3f(xStart, 0.0, 0.0);
//                    glVertex3f(xStart, -halfHeight, 0.0);
//                    glVertex3f(xEnd, -halfHeight, 0.0);
//                    glVertex3f(xEnd, 0.0, 0.0);
//                    glEnd();
//                }
//            }
//        }
//        
//        /*
//         * If the 'next' color is none, drawing
//         * is skipped to let the background show
//         * throw the 'none' region of the palette.
//         */ 
//        if (isNoneColorFlag == false) {
//            /*
//             * left and right region of an entry in the palette
//             */
//            const float xStart = scalar;
//            const float xEnd   = nextScalar;
//            
//            /*
//             * Unless interpolating, use the 'next' color.
//             */
//            float* startRGBA = nextRGBA;
//            float* endRGBA   = nextRGBA;
//            if (interpolateColor) {
//                startRGBA = rgba;
//            }
//            
//            /*
//             * Draw the region in the palette.
//             */
//            glBegin(GL_POLYGON);
//            glColor3fv(startRGBA);
//            glVertex3f(xStart, 0.0, 0.0);
//            glVertex3f(xStart, -halfHeight, 0.0);
//            glColor3fv(endRGBA);
//            glVertex3f(xEnd, -halfHeight, 0.0);
//            glVertex3f(xEnd, 0.0, 0.0);
//            glEnd();
//            
//            /*
//             * The last scalar value is normally 1.0.  If the last
//             * scalar is less than 1.0, then fill in the rest of 
//             * the palette from the last scalar to 1.0.
//             */
//            if (i == iEnd) {
//                if (nextScalar < 1.0) {
//                    const float xStart = nextScalar;
//                    const float xEnd   = 1.0;
//                    glColor3fv(nextRGBA);
//                    glBegin(GL_POLYGON);
//                    glVertex3f(xStart, 0.0, 0.0);
//                    glVertex3f(xStart, -halfHeight, 0.0);
//                    glVertex3f(xEnd, -halfHeight, 0.0);
//                    glVertex3f(xEnd, 0.0, 0.0);
//                    glEnd();
//                }
//            }
//        }
//    }
//    
//    /*
//     * If positive not displayed, draw over it with background color
//     */
//    if (isPositiveDisplayed == false) {
//        glColor3ubv(backgroundRGB);
//        glRectf(0.0, -orthoHeight, orthoWidth, orthoHeight);
//    }
//    
//    /*
//     * If negative not displayed, draw over it with background color
//     */
//    if (isNegativeDisplayed == false) {
//        glColor3ubv(backgroundRGB);
//        glRectf(-orthoWidth, -orthoHeight, 0.0, orthoHeight);
//    }
//    
//    /*
//     * Draw over thresholded regions with background color
//     */
//    const PaletteThresholdTypeEnum::Enum thresholdType = paletteColorMapping->getThresholdType();
//    if (thresholdType != PaletteThresholdTypeEnum::THRESHOLD_TYPE_OFF) {
//        const float minMaxThresholds[2] = {
//            paletteColorMapping->getThresholdMinimum(thresholdType),
//            paletteColorMapping->getThresholdMaximum(thresholdType)
//        };
//        float normalizedThresholds[2];
//        
//        paletteColorMapping->mapDataToPaletteNormalizedValues(statistics,
//                                                              minMaxThresholds,
//                                                              normalizedThresholds,
//                                                              2);
//        
//        switch (paletteColorMapping->getThresholdTest()) {
//            case PaletteThresholdTestEnum::THRESHOLD_TEST_SHOW_INSIDE:
//                glColor3ubv(backgroundRGB);
//                glRectf(-orthoWidth, -orthoHeight, normalizedThresholds[0], orthoHeight);
//                glRectf(normalizedThresholds[1], -orthoHeight, orthoWidth, orthoHeight);
//                break;
//            case PaletteThresholdTestEnum::THRESHOLD_TEST_SHOW_OUTSIDE:
//                glColor3ubv(backgroundRGB);
//                glRectf(normalizedThresholds[0], -orthoHeight, normalizedThresholds[1], orthoHeight);
//                break;
//        }
//    }
//    
//    /*
//     * If zeros are not displayed, draw a line in the 
//     * background color at zero in the palette.
//     */
//    if (isZeroDisplayed == false) {
//        this->setLineWidth(1.0);
//        glColor3ubv(backgroundRGB);
//        glBegin(GL_LINES);
//        glVertex2f(0.0, -halfHeight);
//        glVertex2f(0.0, 0.0);
//        glEnd();
//    }
//    
//    float minMax[4] = { -1.0, 0.0, 0.0, 1.0 };
//    switch (paletteColorMapping->getScaleMode()) {
//        case PaletteScaleModeEnum::MODE_AUTO_SCALE:
//            minMax[0] = statistics->getMostNegativeValue();
//            minMax[3] = statistics->getMostPositiveValue();
//            break;
//        case PaletteScaleModeEnum::MODE_AUTO_SCALE_PERCENTAGE:
//        {
//            const float negMaxPct = paletteColorMapping->getAutoScalePercentageNegativeMaximum();
//            const float negMinPct = paletteColorMapping->getAutoScalePercentageNegativeMinimum();
//            const float posMinPct = paletteColorMapping->getAutoScalePercentagePositiveMinimum();
//            const float posMaxPct = paletteColorMapping->getAutoScalePercentagePositiveMaximum();
//            
//            minMax[0] = statistics->getNegativePercentile(negMaxPct);
//            minMax[1] = statistics->getNegativePercentile(negMinPct);
//            minMax[2] = statistics->getPositivePercentile(posMinPct);
//            minMax[3] = statistics->getPositivePercentile(posMaxPct);
//        }
//            break;
//        case PaletteScaleModeEnum::MODE_USER_SCALE:
//            minMax[0] = paletteColorMapping->getUserScaleNegativeMaximum();
//            minMax[1] = paletteColorMapping->getUserScaleNegativeMinimum();
//            minMax[2] = paletteColorMapping->getUserScalePositiveMinimum();
//            minMax[3] = paletteColorMapping->getUserScalePositiveMaximum();
//            break;
//    }
//    
//    AString textLeft = AString::number(minMax[0], 'f', 1);
//    AString textCenterNeg = AString::number(minMax[1], 'f', 1);
//    AString textCenterPos = AString::number(minMax[2], 'f', 1);
//    AString textCenter = textCenterPos;
//    if (isNegativeDisplayed && isPositiveDisplayed) {
//        if (textCenterNeg != textCenterPos) {
//            if (textCenterNeg != AString("-" + textCenterPos)) {
//                textCenter = textCenterNeg + "/" + textCenterPos;
//            }
//        }
//    }
//    else if (isNegativeDisplayed) {
//        textCenter = textCenterNeg;
//    }
//    else if (isPositiveDisplayed) {
//        textCenter = textCenterPos;
//    }
//    AString textRight = AString::number(minMax[3], 'f', 1);
//    
//    /*
//     * Reset to the models viewport for drawing text.
//     */
//    glViewport(modelViewport[0], 
//               modelViewport[1], 
//               modelViewport[2], 
//               modelViewport[3]);
//    
//    /*
//     * Switch to the foreground color.
//     */
//    uint8_t foregroundRGB[3];
//    prefs->getColorForeground(foregroundRGB);
//    glColor3ubv(foregroundRGB);
//    
//    /*
//     * Account for margin around colorbar when calculating text locations
//     */
//    const int textCenterX = /*colorbarViewportX +*/ (colorbarViewportWidth / 2);
//    const int textHalfX   = colorbarViewportWidth / (margin * 2);
//    const int textLeftX   = textCenterX - textHalfX;
//    const int textRightX  = textCenterX + textHalfX;
//    
//    const int textY = 2 + colorbarViewportY  - modelViewport[1] + (colorbarViewportHeight / 2);
//    if (isNegativeDisplayed) {
//        this->drawTextWindowCoords(textLeftX, 
//                                   textY, 
//                                   textLeft,
//                                   BrainOpenGLTextRenderInterface::X_LEFT,
//                                   BrainOpenGLTextRenderInterface::Y_BOTTOM);
//    }
//    if (isNegativeDisplayed
//        || isZeroDisplayed
//        || isPositiveDisplayed) {
//        this->drawTextWindowCoords(textCenterX, 
//                                   textY, 
//                                   textCenter,
//                                   BrainOpenGLTextRenderInterface::X_CENTER,
//                                   BrainOpenGLTextRenderInterface::Y_BOTTOM);
//    }
//    if (isPositiveDisplayed) {
//        this->drawTextWindowCoords(textRightX, 
//                                   textY, 
//                                   textRight,
//                                   BrainOpenGLTextRenderInterface::X_RIGHT,
//                                   BrainOpenGLTextRenderInterface::Y_BOTTOM);
//    }
//    
//    return;
//}

/**
 * Draw a palette.
 * @param palette
 *    Palette that is drawn.
 * @param paletteColorMapping
 *    Controls mapping of data to colors.
 * @param statistics
 *    Statistics describing the data that is mapped to the palette.
 * @param paletteDrawingIndex
 *    Counts number of palettes being drawn for the Y-position
 */
void 
BrainOpenGLFixedPipeline::drawPalette(const Palette* palette,
                                      const PaletteColorMapping* paletteColorMapping,
                                      const FastStatistics* statistics,
                                      const int paletteDrawingIndex)
{
    /*
     * Save viewport.
     */
    GLint modelViewport[4];
    glGetIntegerv(GL_VIEWPORT, modelViewport);
    
    /*
     * Create a viewport for drawing the palettes in the 
     * lower left corner of the window.  Try to use
     * 25% of the display's width.
     */
    const GLint colorbarViewportWidth = std::max(static_cast<GLint>(modelViewport[2] * 0.25), 
                                                 (GLint)120);
    const GLint colorbarViewportHeight = 35;    
    const GLint colorbarViewportX = modelViewport[0] + 10;
    GLint colorbarViewportY = (modelViewport[1] + 10 + (paletteDrawingIndex * colorbarViewportHeight));
    glViewport(colorbarViewportX, 
               colorbarViewportY, 
               colorbarViewportWidth, 
               colorbarViewportHeight);
    
    CaretLogFine("Palette " + palette->getName() + " Viewport: ("
                   + AString::number(colorbarViewportX) + ", "
                   + AString::number(colorbarViewportY) + ", "
                   + AString::number(colorbarViewportWidth) + ", "
                   + AString::number(colorbarViewportHeight)
                   + ")\n Model Viewport: "
                   + AString::fromNumbers(modelViewport, 4, ", "));
    /*
     * Types of values for display
     */
    const bool isPositiveDisplayed = paletteColorMapping->isDisplayPositiveDataFlag();
    const bool isNegativeDisplayed = paletteColorMapping->isDisplayNegativeDataFlag();
    const bool isZeroDisplayed     = paletteColorMapping->isDisplayZeroDataFlag();
    const bool isPositiveOnly = (isPositiveDisplayed && (isNegativeDisplayed == false));
    const bool isNegativeOnly = ((isPositiveDisplayed == false) && isNegativeDisplayed);
   
    /*
     * Create an orthographic projection that ranges in X:
     *   (-1, 1) If negative and positive displayed
     *   (-1, 0) If positive is NOT displayed
     *   (0, 1)  If negative is NOT displayed
     */
    const GLdouble halfHeight = static_cast<GLdouble>(colorbarViewportHeight / 2);
    const GLdouble orthoHeight = halfHeight;
    GLdouble orthoLeft = -1.0;
    GLdouble orthoRight = 1.0;    
    if (isPositiveOnly) {
        orthoLeft = 0.0;
    }
    else if (isNegativeOnly) {
        orthoRight = 0.0;
    }
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(orthoLeft,  orthoRight, 
            -orthoHeight, orthoHeight, 
            -1.0, 1.0);
    
    glMatrixMode (GL_MODELVIEW);
    glLoadIdentity();

    /*
     * A little extra so viewport gets filled
     */
    const GLdouble orthoLeftWithExtra = orthoLeft - 0.10;
    const GLdouble orthoRightWithExtra = orthoRight + 0.10;
    
    /*
     * Fill the palette viewport with the background color
     * Add a little to left and right so viewport is filled (excess will get clipped)
     */
    uint8_t backgroundRGB[3];
    CaretPreferences* prefs = SessionManager::get()->getCaretPreferences();
    prefs->getColorBackground(backgroundRGB);
    glColor3ubv(backgroundRGB);
    glRectf(orthoLeftWithExtra, orthoRightWithExtra, -orthoHeight, orthoHeight);
    
    /*
     * Always interpolate if the palette has only two colors
     */
    bool interpolateColor = paletteColorMapping->isInterpolatePaletteFlag();
    if (palette->getNumberOfScalarsAndColors() <= 2) {
        interpolateColor = true;
    }
    
    /*
     * Draw the colorbar starting with the color assigned
     * to the negative end of the palette.
     * Colorbar scalars range from -1 to 1.
     */
    const int iStart = palette->getNumberOfScalarsAndColors() - 1;
    const int iEnd = 1;
    const int iStep = -1;
    for (int i = iStart; i >= iEnd; i += iStep) {
        /*
         * palette data for 'left' side of a color in the palette.
         */
        const PaletteScalarAndColor* sc = palette->getScalarAndColor(i);
        float scalar = sc->getScalar();
        float rgba[4];
        sc->getColor(rgba);
        
        /*
         * palette data for 'right' side of a color in the palette.
         */
        const PaletteScalarAndColor* nextSC = palette->getScalarAndColor(i - 1);
        float nextScalar = nextSC->getScalar();
        float nextRGBA[4];
        nextSC->getColor(nextRGBA);
        const bool isNoneColorFlag = nextSC->isNoneColor();
        
        /*
         * Exclude negative regions if not displayed.
         */
        if (isNegativeDisplayed == false) {
            if (nextScalar < 0.0) {
                continue;
            }
            else if (scalar < 0.0) {
                scalar = 0.0;
            }
        }
        
        /*
         * Exclude positive regions if not displayed.
         */
        if (isPositiveDisplayed == false) {
            if (scalar > 0.0) {
                continue;
            }
            else if (nextScalar > 0.0) {
                nextScalar = 0.0;
            }
        }
        
        /*
         * Normally, the first entry has a scalar value of -1.
         * If it does not, use the first color draw from 
         * -1 to the first scalar value.
         */
        if (i == iStart) {
            if (sc->isNoneColor() == false) {
                if (scalar > -1.0) {
                    const float xStart = -1.0;
                    const float xEnd   = scalar;
                    glColor3fv(rgba);
                    glBegin(GL_POLYGON);
                    glVertex3f(xStart, 0.0, 0.0);
                    glVertex3f(xStart, -halfHeight, 0.0);
                    glVertex3f(xEnd, -halfHeight, 0.0);
                    glVertex3f(xEnd, 0.0, 0.0);
                    glEnd();
                }
            }
        }
        
        /*
         * If the 'next' color is none, drawing
         * is skipped to let the background show
         * throw the 'none' region of the palette.
         */ 
        if (isNoneColorFlag == false) {
            /*
             * left and right region of an entry in the palette
             */
            const float xStart = scalar;
            const float xEnd   = nextScalar;
            
            /*
             * Unless interpolating, use the 'next' color.
             */
            float* startRGBA = nextRGBA;
            float* endRGBA   = nextRGBA;
            if (interpolateColor) {
                startRGBA = rgba;
            }
            
            /*
             * Draw the region in the palette.
             */
            glBegin(GL_POLYGON);
            glColor3fv(startRGBA);
            glVertex3f(xStart, 0.0, 0.0);
            glVertex3f(xStart, -halfHeight, 0.0);
            glColor3fv(endRGBA);
            glVertex3f(xEnd, -halfHeight, 0.0);
            glVertex3f(xEnd, 0.0, 0.0);
            glEnd();
            
            /*
             * The last scalar value is normally 1.0.  If the last
             * scalar is less than 1.0, then fill in the rest of 
             * the palette from the last scalar to 1.0.
             */
            if (i == iEnd) {
                if (nextScalar < 1.0) {
                    const float xStart = nextScalar;
                    const float xEnd   = 1.0;
                    glColor3fv(nextRGBA);
                    glBegin(GL_POLYGON);
                    glVertex3f(xStart, 0.0, 0.0);
                    glVertex3f(xStart, -halfHeight, 0.0);
                    glVertex3f(xEnd, -halfHeight, 0.0);
                    glVertex3f(xEnd, 0.0, 0.0);
                    glEnd();
                }
            }
        }
    }
    
    /*
     * Draw over thresholded regions with background color
     */
    const PaletteThresholdTypeEnum::Enum thresholdType = paletteColorMapping->getThresholdType();
    if (thresholdType != PaletteThresholdTypeEnum::THRESHOLD_TYPE_OFF) {
        const float minMaxThresholds[2] = {
            paletteColorMapping->getThresholdMinimum(thresholdType),
            paletteColorMapping->getThresholdMaximum(thresholdType)
        };
        float normalizedThresholds[2];
        
        paletteColorMapping->mapDataToPaletteNormalizedValues(statistics,
                                                              minMaxThresholds,
                                                              normalizedThresholds,
                                                              2);
        
        switch (paletteColorMapping->getThresholdTest()) {
            case PaletteThresholdTestEnum::THRESHOLD_TEST_SHOW_INSIDE:
                glColor3ubv(backgroundRGB);
                glRectf(orthoLeftWithExtra, -orthoHeight, normalizedThresholds[0], orthoHeight);
                glRectf(normalizedThresholds[1], -orthoHeight, orthoRightWithExtra, orthoHeight);
                break;
            case PaletteThresholdTestEnum::THRESHOLD_TEST_SHOW_OUTSIDE:
                glColor3ubv(backgroundRGB);
                glRectf(normalizedThresholds[0], -orthoHeight, normalizedThresholds[1], orthoHeight);
                break;
        }
    }
    
    /*
     * If zeros are not displayed, draw a line in the 
     * background color at zero in the palette.
     */
    if (isZeroDisplayed == false) {
        this->setLineWidth(1.0);
        glColor3ubv(backgroundRGB);
        glBegin(GL_LINES);
        glVertex2f(0.0, -halfHeight);
        glVertex2f(0.0, 0.0);
        glEnd();
    }
    
    float minMax[4] = { -1.0, 0.0, 0.0, 1.0 };
    switch (paletteColorMapping->getScaleMode()) {
        case PaletteScaleModeEnum::MODE_AUTO_SCALE:
        {
            float dummy;
            statistics->getNonzeroRanges(minMax[0], dummy, dummy, minMax[3]);
        }
            break;
        case PaletteScaleModeEnum::MODE_AUTO_SCALE_PERCENTAGE:
        {
            const float negMaxPct = paletteColorMapping->getAutoScalePercentageNegativeMaximum();
            const float negMinPct = paletteColorMapping->getAutoScalePercentageNegativeMinimum();
            const float posMinPct = paletteColorMapping->getAutoScalePercentagePositiveMinimum();
            const float posMaxPct = paletteColorMapping->getAutoScalePercentagePositiveMaximum();
            
            minMax[0] = statistics->getApproxNegativePercentile(negMaxPct);
            minMax[1] = statistics->getApproxNegativePercentile(negMinPct);
            minMax[2] = statistics->getApproxPositivePercentile(posMinPct);
            minMax[3] = statistics->getApproxPositivePercentile(posMaxPct);
        }
            break;
        case PaletteScaleModeEnum::MODE_USER_SCALE:
            minMax[0] = paletteColorMapping->getUserScaleNegativeMaximum();
            minMax[1] = paletteColorMapping->getUserScaleNegativeMinimum();
            minMax[2] = paletteColorMapping->getUserScalePositiveMinimum();
            minMax[3] = paletteColorMapping->getUserScalePositiveMaximum();
            break;
    }
    
    AString textLeft = AString::number(minMax[0], 'f', 1);
    AString textCenterNeg = AString::number(minMax[1], 'f', 1);
    if (textCenterNeg == "-0.0") {
        textCenterNeg = "0.0";
    }
    AString textCenterPos = AString::number(minMax[2], 'f', 1);
    AString textCenter = textCenterPos;
    if (isNegativeDisplayed && isPositiveDisplayed) {
        if (textCenterNeg != textCenterPos) {
            textCenter = textCenterNeg + "/" + textCenterPos;
        }
    }
    else if (isNegativeDisplayed) {
        textCenter = textCenterNeg;
    }
    else if (isPositiveDisplayed) {
        textCenter = textCenterPos;
    }
    AString textRight = AString::number(minMax[3], 'f', 1);
    
    /*
     * Reset to the models viewport for drawing text.
     */
    glViewport(modelViewport[0], 
               modelViewport[1], 
               modelViewport[2], 
               modelViewport[3]);
    
    /*
     * Switch to the foreground color.
     */
    uint8_t foregroundRGB[3];
    prefs->getColorForeground(foregroundRGB);
    glColor3ubv(foregroundRGB);
    
    /*
     * Account for margin around colorbar when calculating text locations
     */
    int textCenterX = colorbarViewportX - modelViewport[0] + (colorbarViewportWidth / 2);
    const int textLeftX   = colorbarViewportX - modelViewport[0];
    const int textRightX  = (colorbarViewportX  - modelViewport[0] + colorbarViewportWidth);
    if (isPositiveOnly) {
        textCenterX = textLeftX;
    }
    else if (isNegativeOnly) {
        textCenterX = textRightX;
    }
    
    const int textY = 2 + colorbarViewportY  - modelViewport[1] + (colorbarViewportHeight / 2);
    if (isNegativeDisplayed) {
        this->drawTextWindowCoords(textLeftX, 
                                   textY, 
                                   textLeft,
                                   BrainOpenGLTextRenderInterface::X_LEFT,
                                   BrainOpenGLTextRenderInterface::Y_BOTTOM);
    }
    if (isNegativeDisplayed
        || isZeroDisplayed
        || isPositiveDisplayed) {
        BrainOpenGLTextRenderInterface::TextAlignmentX textAlignX = BrainOpenGLTextRenderInterface::X_CENTER;
        if (isNegativeOnly) {
            textAlignX = BrainOpenGLTextRenderInterface::X_RIGHT;
        }
        else if (isPositiveOnly) {
            textAlignX = BrainOpenGLTextRenderInterface::X_LEFT;
        }
        this->drawTextWindowCoords(textCenterX, 
                                   textY, 
                                   textCenter,
                                   textAlignX,
                                   BrainOpenGLTextRenderInterface::Y_BOTTOM);
    }
    if (isPositiveDisplayed) {
        this->drawTextWindowCoords(textRightX, 
                                   textY, 
                                   textRight,
                                   BrainOpenGLTextRenderInterface::X_RIGHT,
                                   BrainOpenGLTextRenderInterface::Y_BOTTOM);
    }
    
    return;
}

/**
 * Since OpenGL draws lines/points in pixels, map window coordinates to
 * model coordinates to estimate the line width or points size in pixels
 * for a line width or point size that is in model coordinates.
 * @param modelSize
 *    Size in model coordinates.
 * @return
 *    Size converted to pixels.
 */
float 
BrainOpenGLFixedPipeline::modelSizeToPixelSize(const float modelSize)
{
    float pixelSize = modelSize;
    
    GLdouble modelview[16];
    GLdouble projection[16];
    glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
    glGetDoublev(GL_PROJECTION_MATRIX, projection);
    
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, 
                  viewport);
    
    GLdouble windowA[3] = { viewport[0], viewport[1], 0.0 };
    GLdouble windowB[3] = { viewport[0] + viewport[2] - 1, viewport[1] + viewport[3] - 1, 0.0 };
    GLdouble modelA[3], modelB[3];
    if (gluUnProject(windowA[0], 
                     windowA[1], 
                     windowA[2], 
                     modelview, 
                     projection, 
                     viewport, 
                     &modelA[0], 
                     &modelA[1], 
                     &modelA[2]) == GL_TRUE) {
        if (gluUnProject(windowB[0], 
                         windowB[1], 
                         windowB[2], 
                         modelview, 
                         projection, 
                         viewport, 
                         &modelB[0], 
                         &modelB[1], 
                         &modelB[2]) == GL_TRUE) {
            const double modelDist = MathFunctions::distance3D(modelA, modelB);
            const double windowDist = MathFunctions::distance3D(windowA, windowB);
            
            const float scaling = windowDist / modelDist;
            
            pixelSize *= scaling;
        }
    }
    return pixelSize;
}

//============================================================================
/**
 * Constructor.
 */
BrainOpenGLFixedPipeline::VolumeDrawInfo::VolumeDrawInfo(VolumeFile* volumeFile,
                                                         Brain* brain,
                                                         PaletteColorMapping* paletteColorMapping,
                                                         const FastStatistics* statistics,
                                                         const int32_t mapIndex,
                                                         const float opacity) 
: statistics(statistics) {
    this->volumeFile = volumeFile;
    this->brain = brain;
    this->volumeType = volumeFile->getType();
    this->paletteColorMapping = paletteColorMapping;
    this->mapIndex = mapIndex;
    this->opacity    = opacity;
}
