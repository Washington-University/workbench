

/*LICENSE_START*/
/*
 *  Copyright (C) 2014  Washington University School of Medicine
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

#include <cstdlib>

#include "CaretOpenGLInclude.h"

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
#include "BrainOpenGLChartDrawingFixedPipeline.h"
#include "BrainOpenGLPrimitiveDrawing.h"
#include "BrainOpenGLVolumeSliceDrawing.h"
#include "BrainOpenGLShapeCone.h"
#include "BrainOpenGLShapeCube.h"
#include "BrainOpenGLShapeCylinder.h"
#include "BrainOpenGLShapeSphere.h"
#include "BrainOpenGLViewportContent.h"
#include "BrainStructure.h"
#include "BrowserTabContent.h"
#include "BoundingBox.h"
#include "CaretAssert.h"
#include "CaretLogger.h"
#include "CaretMappableDataFile.h"
#include "CaretPreferences.h"
#include "ChartableMatrixFileSelectionModel.h"
#include "ChartModelDataSeries.h"
#include "ChartModelTimeSeries.h"
#include "CiftiBrainordinateLabelFile.h"
#include "CiftiFiberOrientationFile.h"
#include "CiftiFiberTrajectoryFile.h"
#include "ClippingPlaneGroup.h"
#include "DescriptiveStatistics.h"
#include "DisplayGroupEnum.h"
#include "DisplayPropertiesBorders.h"
#include "DisplayPropertiesFiberOrientation.h"
#include "DisplayPropertiesFoci.h"
#include "DisplayPropertiesLabels.h"
#include "DisplayPropertiesSurface.h"
#include "DisplayPropertiesVolume.h"
#include "ElapsedTimer.h"
#include "EventManager.h"
#include "EventModelSurfaceGet.h"
#include "EventNodeIdentificationColorsGetFromCharts.h"
#include "FastStatistics.h"
#include "Fiber.h"
#include "FiberOrientation.h"
#include "FiberOrientationTrajectory.h"
#include "FiberTrajectoryMapProperties.h"
#include "FociFile.h"
#include "Focus.h"
#include "GiftiLabel.h"
#include "GiftiLabelTable.h"
#include "GroupAndNameHierarchyModel.h"
#include "IdentifiedItemNode.h"
#include "IdentificationManager.h"
#include "LabelDrawingTypeEnum.h"
#include "Matrix4x4.h"
#include "SelectionItemBorderSurface.h"
#include "SelectionItemFocusSurface.h"
#include "SelectionItemFocusVolume.h"
#include "SelectionItemSurfaceNode.h"
#include "SelectionItemSurfaceNodeIdentificationSymbol.h"
#include "SelectionItemSurfaceTriangle.h"
#include "SelectionItemVoxel.h"
#include "SurfaceMontageConfigurationCerebellar.h"
#include "SurfaceMontageConfigurationCerebral.h"
#include "SurfaceMontageConfigurationFlatMaps.h"
#include "IdentificationWithColor.h"
#include "SelectionManager.h"
#include "MathFunctions.h"
#include "ModelChart.h"
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
#include "VolumeMappableInterface.h"
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
    m_shapeCylinder = NULL;
    m_shapeCube   = NULL;
    m_shapeCubeRounded = NULL;
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
    if (m_shapeCylinder != NULL) {
        delete m_shapeCylinder;
        m_shapeCylinder = NULL;
    }
    if (m_shapeCube != NULL) {
        delete m_shapeCube;
        m_shapeCube = NULL;
    }
    if (m_shapeCubeRounded != NULL) {
        delete m_shapeCubeRounded;
        m_shapeCubeRounded = NULL;
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
    
    for (int32_t i = 0; i < static_cast<int32_t>(viewportContents.size()); i++) {
        /*
         * Viewport of window.
         */
        BrainOpenGLViewportContent* vpContent = viewportContents[i];
        const int* windowVP = vpContent->getWindowViewport();
        glViewport(windowVP[0], windowVP[1], windowVP[2], windowVP[3]);
        
        CaretLogFinest("Drawing Model "
                       + AString::number(i)
                       + ": "
                       + AString::fromNumbers(vpContent->getModelViewport(), 4, ", "));
        m_brain = vpContent->getBrain();
        CaretAssert(m_brain);
        this->drawModelInternal(MODE_DRAWING,
                                vpContent);
        
        /*
         * Draw border in foreground color around tab that is highlighted
         * in Tile Tabs when user selects a tab.
         */
        if (vpContent->isTabHighlighted()) {
            glMatrixMode(GL_PROJECTION);
            glPushMatrix();
            glLoadIdentity();
            const float width = windowVP[2];
            const float height = windowVP[3];
            glOrtho(0.0, windowVP[2], 0.0, windowVP[3], -100.0, 100.0);
            
            glMatrixMode(GL_MODELVIEW);
            glPushMatrix();
            glLoadIdentity();
            
            const CaretPreferences* prefs = SessionManager::get()->getCaretPreferences();
            float foregroundColor[4];
            prefs->getColorForeground(foregroundColor);
            glColor3fv(foregroundColor);
            
            const float thickness = 10;
            
            /*
             * Left Side
             */
            glBegin(GL_QUADS);
            glVertex3f(0.0, 0.0, 0.0);
            glVertex3f(thickness, 0.0, 0.0);
            glVertex3f(thickness, height, 0.0);
            glVertex3f(0.0, height, 0.0);
            glEnd();
            
            /*
             * Right Side
             */
            glBegin(GL_QUADS);
            glVertex3f(width - thickness, 0.0, 0.0);
            glVertex3f(width, 0.0, 0.0);
            glVertex3f(width, height, 0.0);
            glVertex3f(width - thickness, height, 0.0);
            glEnd();
            
            /*
             * Bottom Side
             */
            glBegin(GL_QUADS);
            glVertex3f(0.0, 0.0, 0.0);
            glVertex3f(width, 0.0, 0.0);
            glVertex3f(width, thickness, 0.0);
            glVertex3f(0.0, thickness, 0.0);
            glEnd();
            
            /*
             * Top Side
             */
            glBegin(GL_QUADS);
            glVertex3f(0.0, height - thickness, 0.0);
            glVertex3f(width, height - thickness, 0.0);
            glVertex3f(width, height, 0.0);
            glVertex3f(0.0, height, 0.0);
            glEnd();
            
            glPopMatrix();
            glMatrixMode(GL_PROJECTION);
            glPopMatrix();
            
            glMatrixMode(GL_MODELVIEW);
        }
        
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
    Model* model = NULL;
    
    if (this->browserTabContent != NULL) {
        Model* model = this->browserTabContent->getModelForDisplay();
        this->windowTabIndex = this->browserTabContent->getTabNumber();
        int viewport[4];
        viewportContent->getModelViewport(viewport);
        
        
        this->mode = mode;
        
        this->checkForOpenGLError(model, "At beginning of drawModelInternal()");
        
        if(model != NULL) {
            CaretAssert((this->windowTabIndex >= 0) && (this->windowTabIndex < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS));
            
            bool modelAllowsPalettes = true;
            
            ModelChart* modelChart = dynamic_cast<ModelChart*>(model);
            ModelSurface* surfaceModel = dynamic_cast<ModelSurface*>(model);
            ModelSurfaceMontage* surfaceMontageModel = dynamic_cast<ModelSurfaceMontage*>(model);
            ModelVolume* volumeModel = dynamic_cast<ModelVolume*>(model);
            ModelWholeBrain* wholeBrainModel = dynamic_cast<ModelWholeBrain*>(model);
            if (modelChart != NULL) {
                drawChartData(browserTabContent, modelChart, viewport);
                modelAllowsPalettes = false;
            }
            else if (surfaceModel != NULL) {
                this->drawSurfaceModel(surfaceModel, viewport);
            }
            else if (surfaceMontageModel != NULL) {
                this->drawSurfaceMontageModel(browserTabContent, 
                                              surfaceMontageModel, 
                                              viewport);
            }
            else if (volumeModel != NULL) {
                this->drawVolumeModel(browserTabContent,
                                           volumeModel, viewport);
            }
            else if (wholeBrainModel != NULL) {
                this->drawWholeBrainModel(browserTabContent,
                                               wholeBrainModel, viewport);
            }
            else {
                modelAllowsPalettes = false;
                CaretAssertMessage(0, "Unknown type of model for drawing");
            }
            
            if (modelAllowsPalettes) {
                int viewport[4];
                viewportContent->getModelViewport(viewport);
                glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);
                this->drawAllPalettes(model->getBrain());
            }
        }
    }
    
    glFlush();
    
    this->checkForOpenGLError(model, "At end of drawModelInternal()");
    
    if (model != NULL) {
        CaretLogFine("Time to draw " 
                    + model->getNameForGUI(false)
                    + " was "
                    + AString::number(et.getElapsedTimeSeconds())
                    + " seconds");
    }
}

/**
 * Set the viewport.
 *
 * @param viewport
 *   Values for viewport (x, y, x-size, y-size)
 * @param projectionType
 *    Type of view projection.
 */
void
BrainOpenGLFixedPipeline::setViewportAndOrthographicProjection(const int32_t viewport[4],
                                          const  ProjectionViewTypeEnum::Enum projectionType)
{
    glViewport(viewport[0],
               viewport[1],
               viewport[2],
               viewport[3]);
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    this->setOrthographicProjection(viewport,
                                    projectionType);
    glMatrixMode(GL_MODELVIEW);
}

/**
 * Set the viewport for a volume.
 *
 * @param viewport
 *   Values for viewport (x, y, x-size, y-size)
 * @param projectionType
 *    Type of view projection.
 * @param volume
 *    Volume for use in setting orthographic projection.
 */
void
BrainOpenGLFixedPipeline::setViewportAndOrthographicProjectionForVolume(const int32_t viewport[4],
                                                                        const  ProjectionViewTypeEnum::Enum projectionType,
                                                                        const VolumeMappableInterface* volume)
{
    CaretAssert(volume);
    glViewport(viewport[0],
               viewport[1],
               viewport[2],
               viewport[3]);
    
    BoundingBox boundingBox;
    volume->getVoxelSpaceBoundingBox(boundingBox);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    setOrthographicProjectionForWithBoundingBox(viewport,
                                                projectionType,
                                                &boundingBox);
    glMatrixMode(GL_MODELVIEW);
}

/**
 * Set the viewport for a surface file.
 *
 * @param viewport
 *   Values for viewport (x, y, x-size, y-size)
 * @param projectionType
 *    Type of view projection.
 * @param surfaceFile
 *    Surface file for use in setting orthographic projection.
 */
void
BrainOpenGLFixedPipeline::setViewportAndOrthographicProjectionForSurfaceFile(const int32_t viewport[4],
                                                                             const  ProjectionViewTypeEnum::Enum projectionType,
                                                                             const SurfaceFile* surfaceFile)
{
    CaretAssert(surfaceFile);
    glViewport(viewport[0],
               viewport[1],
               viewport[2],
               viewport[3]);
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    setOrthographicProjectionForWithBoundingBox(viewport,
                                            projectionType,
                                            surfaceFile->getBoundingBox());
    glMatrixMode(GL_MODELVIEW);
}

/**
 * Disable clipping planes.
 */
void
BrainOpenGLFixedPipeline::disableClippingPlanes()
{
    glDisable(GL_CLIP_PLANE0);
    glDisable(GL_CLIP_PLANE1);
    glDisable(GL_CLIP_PLANE2);
    glDisable(GL_CLIP_PLANE3);
    glDisable(GL_CLIP_PLANE4);
    glDisable(GL_CLIP_PLANE5);
}

/**
 * Apply the clipping planes for the given data type and structure.
 *
 * @param clippingDataType
 *    Type of data that is being clipped.
 * @param structure
 *    The structure.
 */
void
BrainOpenGLFixedPipeline::applyClippingPlanes(const ClippingDataType clippingDataType,
                                              const StructureEnum::Enum structure)
{
    disableClippingPlanes();
    
    if (browserTabContent == NULL) {
        return;
    }
    const ClippingPlaneGroup* clippingPlaneGroup = browserTabContent->getClippingPlaneGroup();
    float rotation[3];
    clippingPlaneGroup->getRotationAngles(rotation);

    float panning[3];
    clippingPlaneGroup->getTranslationForStructure(structure,
                                                   panning);
    
    float thickness[3];
    clippingPlaneGroup->getThickness(thickness);

    const bool showClippingBounds = false;
    if (showClippingBounds) {
        glColor3f(1.0, 0.0, 0.0);
        glLineWidth(2.0);
        glPushMatrix();
        glTranslatef(panning[0], panning[1], panning[2]);
        float minX = -thickness[0] / 2.0;
        float maxX =  thickness[0] / 2.0;
        float minY = -thickness[1] / 2.0;
        float maxY =  thickness[1] / 2.0;
        float minZ = -thickness[2] / 2.0;
        float maxZ =  thickness[2] / 2.0;
        
        glBegin(GL_LINE_LOOP);
        glVertex3f(minX, minY, minZ);
        glVertex3f(maxX, minY, minZ);
        glVertex3f(maxX, maxY, minZ);
        glVertex3f(minX, maxY, minZ);
        glEnd();
        
        glBegin(GL_LINE_LOOP);
        glVertex3f(minX, minY, maxZ);
        glVertex3f(maxX, minY, maxZ);
        glVertex3f(maxX, maxY, maxZ);
        glVertex3f(minX, maxY, maxZ);
        glEnd();
        
        glBegin(GL_LINES);
        glVertex3f(minX, minY, minZ);
        glVertex3f(minX, minY, maxZ);
        glVertex3f(maxX, minY, minZ);
        glVertex3f(maxX, minY, maxZ);
        glVertex3f(maxX, maxY, minZ);
        glVertex3f(maxX, maxY, maxZ);
        glVertex3f(minX, maxY, minZ);
        glVertex3f(minX, maxY, maxZ);
        glEnd();
        glPopMatrix();
    }
    
    switch (clippingDataType) {
        case CLIPPING_DATA_TYPE_FEATURES:
            if (! clippingPlaneGroup->isFeaturesSelected()) {
                return;
            }
            break;
        case CLIPPING_DATA_TYPE_SURFACE:
            if (! clippingPlaneGroup->isSurfaceSelected()) {
                return;
            }
            break;
        case CLIPPING_DATA_TYPE_VOLUME:
            if (! clippingPlaneGroup->isVolumeSelected()) {
                return;
            }
            break;
    }
    
    std::vector<Plane> planes = clippingPlaneGroup->getActiveClippingPlanesForStructure(structure);
    const int32_t numPlanes = static_cast<int32_t>(planes.size());
    for (int32_t i = 0; i < numPlanes; i++) {
        const Plane& p = planes[i];
        double a, b, c, d;
        p.getPlane(a, b, c, d);
        const GLdouble abcd[4] = { a, b, c, d };
        glClipPlane(GL_CLIP_PLANE0 + i,
                    abcd);
        glEnable(GL_CLIP_PLANE0 + i);
    }
}

/**
 * Apply the viewing transformations for the content of the browser tab.
 *
 * @param objectCenterXYZ
 *    If not NULL, contains center of object about
 *    which rotation should take place.
 * @param projectionViewType
 *    Projection view type.
 */
void
BrainOpenGLFixedPipeline::applyViewingTransformations(const float objectCenterXYZ[3],
                                                      const ProjectionViewTypeEnum::Enum projectionViewType)
{
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    float eyeX = 0.0;
    float eyeY = 0.0;
    float eyeZ = 0.0;
    
    float centerX = 0.0;
    float centerY = 0.0;
    float centerZ = 0.0;
    
    float upX = 0.0;
    float upY = 0.0;
    float upZ = 0.0;
    
    bool useGluLookAt = false;
    
    switch (projectionViewType) {
//        case ProjectionViewTypeEnum::PROJECTION_VIEW_CEREBELLUM_ANTERIOR:
//            eyeY = 5.0;
//            upZ  = 1.0;
//            useGluLookAt = true;
//            break;
        case ProjectionViewTypeEnum::PROJECTION_VIEW_CEREBELLUM_ANTERIOR:
        case ProjectionViewTypeEnum::PROJECTION_VIEW_CEREBELLUM_DORSAL:
        case ProjectionViewTypeEnum::PROJECTION_VIEW_CEREBELLUM_POSTERIOR:
        case ProjectionViewTypeEnum::PROJECTION_VIEW_CEREBELLUM_VENTRAL:
            eyeZ = 5.0;
            upY  = 1.0;
            useGluLookAt = true;
            break;
//        case ProjectionViewTypeEnum::PROJECTION_VIEW_CEREBELLUM_POSTERIOR:
//            eyeY = -5.0;
//            upZ  =  1.0;
//            useGluLookAt = true;
//            break;
//        case ProjectionViewTypeEnum::PROJECTION_VIEW_CEREBELLUM_VENTRAL:
//            eyeZ = -5.0;
//            upY  = -1.0;
//            useGluLookAt = true;
//            break;
        case ProjectionViewTypeEnum::PROJECTION_VIEW_CEREBELLUM_FLAT_SURFACE:
            eyeZ = 5.0;
            upY  = 1.0;
            useGluLookAt = true;
            break;
        case ProjectionViewTypeEnum::PROJECTION_VIEW_LEFT_LATERAL:
            break;
        case ProjectionViewTypeEnum::PROJECTION_VIEW_LEFT_MEDIAL:
            break;
        case ProjectionViewTypeEnum::PROJECTION_VIEW_LEFT_FLAT_SURFACE:
            break;
        case ProjectionViewTypeEnum::PROJECTION_VIEW_RIGHT_LATERAL:
            break;
        case ProjectionViewTypeEnum::PROJECTION_VIEW_RIGHT_MEDIAL:
            break;
        case ProjectionViewTypeEnum::PROJECTION_VIEW_RIGHT_FLAT_SURFACE:
            break;
    }

    if (useGluLookAt) {
        gluLookAt(eyeX, eyeY, eyeZ,
                  centerX, centerY, centerZ,
                  upX, upY, upZ);
    }

    
    float translation[3];
    double rotationMatrixElements[16];
    float scaling;
    browserTabContent->getTransformationsForOpenGLDrawing(projectionViewType,
                                                          translation,
                                                          rotationMatrixElements,
                                                          scaling);
    
    glTranslatef(translation[0],
                 translation[1],
                 translation[2]);
    
    
    glMultMatrixd(rotationMatrixElements);
    
    /*
     * Save the inverse rotation matrix which may be used
     * later by some drawing functions.
     */
    Matrix4x4 inverseMatrix;
    inverseMatrix.setMatrixFromOpenGL(rotationMatrixElements);
    this->inverseRotationMatrixValid = inverseMatrix.invert();
    if (this->inverseRotationMatrixValid) {
        inverseMatrix.getMatrixForOpenGL(this->inverseRotationMatrix);
    }
    
    glScalef(scaling,
             scaling,
             scaling);
    
    if (objectCenterXYZ != NULL) {
        /*
         * Place center of surface at origin.
         */
        glTranslatef(-objectCenterXYZ[0], -objectCenterXYZ[1], -objectCenterXYZ[2]);
    }
}

/**
 * For a volume, get translation and scaling so that the volume 'fills' 
 * the window.
 *
 * @param volume
 *   The volume.
 * @param sliceViewPlane
 *   The slice viewing plane.
 * @param orthographicExtent
 *   The orthographic bounds
 * @param translationOut
 *   Output of translation.
 * @param scalingOut
 *   Output of scaling.
 *
 */
void
BrainOpenGLFixedPipeline::getVolumeFitToWindowScalingAndTranslation(const VolumeMappableInterface* volume,
                                                                    const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
                                                                    const double orthographicExtent[6],
                                                                    float translationOut[3],
                                                                    float& scalingOut) const
{
    /*
     * Apply some scaling and translation so that the volume slice, by default
     * is not larger than the window in which it is being viewed.
     */
    scalingOut = 1.0;
    translationOut[0] = 0.0;
    translationOut[1] = 0.0;
    translationOut[2] = 0.0;
    
    if (volume != NULL) {
        BoundingBox boundingBox;
        volume->getVoxelSpaceBoundingBox(boundingBox);
        
        int64_t dimI, dimJ, dimK, numMaps, numComponents;
        volume->getDimensions(dimI, dimJ, dimK, numMaps, numComponents);
        if ((dimI > 2)
            && (dimJ > 2)
            && (dimK > 2)) {
            
            float volumeCenter[3] = { (boundingBox.getMinX() + boundingBox.getMaxX()) / 2,
                (boundingBox.getMinY() + boundingBox.getMaxY()) / 2,
                (boundingBox.getMinZ() + boundingBox.getMaxZ()) / 2 };
            
            /*
             * Translate so that the center voxel (by dimenisons)
             * is at the center of the screen.
             */
            translationOut[0] = -volumeCenter[0];
            translationOut[1] = -volumeCenter[1];
            translationOut[2] = -volumeCenter[2];
            switch (sliceViewPlane) {
                case VolumeSliceViewPlaneEnum::ALL:
                    break;
                case VolumeSliceViewPlaneEnum::AXIAL:
                    translationOut[2] = 0.0;
                    break;
                case VolumeSliceViewPlaneEnum::CORONAL:
                    translationOut[1] = 0.0;
                    break;
                case VolumeSliceViewPlaneEnum::PARASAGITTAL:
                    translationOut[0] = 0.0;
                    break;
            }
            
            
            /*
             * Scale so volume fills, but does not extend out of window.
             */
            const float xExtent = (boundingBox.getMaxX() - boundingBox.getMinX()) / 2;
            const float yExtent = (boundingBox.getMaxY() - boundingBox.getMinY()) / 2;
            const float zExtent = (boundingBox.getMaxZ() - boundingBox.getMinZ()) / 2;
            
//            const float orthoExtentX = std::min(std::fabs(this->orthographicRight),
//                                                std::fabs(this->orthographicLeft));
//            const float orthoExtentY = std::min(std::fabs(this->orthographicTop),
//                                                std::fabs(this->orthographicBottom));
            const float orthoExtentX = std::min(std::fabs(orthographicExtent[0]),
                                                std::fabs(orthographicExtent[1]));
            const float orthoExtentY = std::min(std::fabs(orthographicExtent[2]),
                                                std::fabs(orthographicExtent[3]));
            
            float temp;
            float scaleWindowX = (orthoExtentX / xExtent);
            temp = (orthoExtentX / yExtent);//parasaggital y is screen x
            if (temp < scaleWindowX) scaleWindowX = temp;
            float scaleWindowY = (orthoExtentY / zExtent);
            temp = (orthoExtentY / yExtent);//axial y is screen y
            if (temp < scaleWindowY) scaleWindowY = temp;
            scalingOut = std::min(scaleWindowX,
                                          scaleWindowY);
            scalingOut *= 0.98;
        }
    }
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
                                                   0.5);
    }
    if (m_shapeCone == NULL) {
        m_shapeCone = new BrainOpenGLShapeCone(8);
    }
    
    if (m_shapeCylinder == NULL) {
        m_shapeCylinder = new BrainOpenGLShapeCylinder(8);
    }
    
    if (m_shapeCube == NULL) {
        m_shapeCube = new BrainOpenGLShapeCube(1.0,
                                               BrainOpenGLShapeCube::NORMAL);
    }
    if (m_shapeCubeRounded == NULL) {
        m_shapeCubeRounded = new BrainOpenGLShapeCube(1.0,
                                                      BrainOpenGLShapeCube::ROUNDED);
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
 * Draw contents of a surface model.
 * @param surfaceModel
 *    Model that is drawn.
 * @param viewport
 *    Viewport for drawing region.
 */
void 
BrainOpenGLFixedPipeline::drawSurfaceModel(ModelSurface* surfaceModel,
                                   const int32_t viewport[4])
{
    Surface* surface = surfaceModel->getSurface();
    float center[3];
    surface->getBoundingBox()->getCenter(center);

    this->setViewportAndOrthographicProjectionForSurfaceFile(viewport,
                                                             browserTabContent->getProjectionViewType(),
                                                             surface);
    
    this->applyViewingTransformations(center,
                                      browserTabContent->getProjectionViewType());
    
    const float* nodeColoringRGBA = this->surfaceNodeColoring->colorSurfaceNodes(surfaceModel, 
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
 *
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
    
    applyClippingPlanes(BrainOpenGLFixedPipeline::CLIPPING_DATA_TYPE_SURFACE,
                        surface->getStructure());
    
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
                    /*
                     * Enable alpha blending so that surface transparency
                     * (using first overlay opacity) will function.
                     */
                    GLboolean blendingEnabled = false;
                    glGetBooleanv(GL_BLEND, &blendingEnabled);
                    glEnable(GL_BLEND);
                    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//                    std::cout << "Surface " << surface->getFileNameNoPath() << std::endl;
//                    std::cout << qPrintable(getStateOfOpenGL()) << std::endl;
                    
                    this->drawSurfaceTrianglesWithVertexArrays(surface,
                                                               nodeColoringRGBA);
                    if (blendingEnabled == false) {
                        glDisable(GL_BLEND);
                    }
                    break;
            }
            
            this->disableClippingPlanes();
            
            if (dps->isDisplayNormalVectors()) {
                drawSurfaceNormalVectors(surface);
            }
            this->drawSurfaceBorders(surface);
            this->drawSurfaceFoci(surface);
            this->drawSurfaceNodeAttributes(surface);
            this->drawSurfaceBorderBeingDrawn(surface);
            break;
        case MODE_IDENTIFICATION:
            /*
             * Disable shading since ID info is encoded in rgba coloring
             */
            glShadeModel(GL_FLAT); 
            if (drawingType != SurfaceDrawingTypeEnum::DRAW_HIDE) {
                this->drawSurfaceNodes(surface,
                                       nodeColoringRGBA);
                this->drawSurfaceTriangles(surface,
                                           nodeColoringRGBA);
            }

            this->disableClippingPlanes();
            
            if (dps->isDisplayNormalVectors()) {
                drawSurfaceNormalVectors(surface);
            }
            this->drawSurfaceBorders(surface);
            this->drawSurfaceFoci(surface);
            this->drawSurfaceNodeAttributes(surface);
            /*
             * Re-enable shading since ID info is encoded in rgba coloring
             */
            glShadeModel(GL_SMOOTH);
            break;
        case MODE_PROJECTION:
            /*
             * Disable shading since ID info is encoded in rgba coloring
             */
            glShadeModel(GL_FLAT);
            if (drawingType != SurfaceDrawingTypeEnum::DRAW_HIDE) {
                this->drawSurfaceTriangles(surface,
                                           nodeColoringRGBA);
            }
            /*
             * Re-enable shading since ID info is encoded in rgba coloring
             */
            glShadeModel(GL_SMOOTH);
            break;
    }
    
    this->disableLighting();
    
    this->disableClippingPlanes();
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
    
    uint8_t rgba[4];
    
    glBegin(GL_TRIANGLES);
    for (int32_t i = 0; i < numTriangles; i++) {
        const int32_t i3 = i * 3;
        const int32_t n1 = triangles[i3];
        const int32_t n2 = triangles[i3+1];
        const int32_t n3 = triangles[i3+2];
        
        if (isSelect) {
            this->colorIdentification->addItem(rgba, SelectionItemDataTypeEnum::SURFACE_TRIANGLE, i);
            glColor3ubv(rgba);
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
    
    uint8_t rgba[4];

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
            this->colorIdentification->addItem(rgba, SelectionItemDataTypeEnum::SURFACE_NODE, i);
            glColor3ubv(rgba);
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
    
    const std::vector<IdentifiedItemNode> identifiedNodes = idManager->getNodeIdentifiedItemsForSurface(structure,
                                                                                                        numNodes);
    std::vector<int32_t> identifiedNodeIndices;
    for (std::vector<IdentifiedItemNode>::const_iterator iter = identifiedNodes.begin();
         iter != identifiedNodes.end();
         iter++) {
        const IdentifiedItemNode& nodeID = *iter;
        
        identifiedNodeIndices.push_back(nodeID.getNodeIndex());
    }
    
    EventNodeIdentificationColorsGetFromCharts colorsFromChartsEvent(structure,
                                                                     this->windowTabIndex,
                                                                     identifiedNodeIndices);
    
    /*
     * Check for a 'selection' type mode
     */
    bool isSelect = false;
    switch (this->mode) {
        case MODE_DRAWING:
            EventManager::get()->sendEvent(colorsFromChartsEvent.getPointer());
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
    
    uint8_t idRGBA[4];
    
    for (std::vector<IdentifiedItemNode>::const_iterator iter = identifiedNodes.begin();
         iter != identifiedNodes.end();
         iter++) {
        const IdentifiedItemNode& nodeID = *iter;
        
        const int32_t nodeIndex = nodeID.getNodeIndex();
        const float symbolDiameter = nodeID.getSymbolSize();
        
        if (isSelect) {
            this->colorIdentification->addItem(idRGBA,
                                               SelectionItemDataTypeEnum::SURFACE_NODE_IDENTIFICATION_SYMBOL,
                                               nodeIndex);
        }
        else {
            if (structure == nodeID.getStructure()) {
                nodeID.getSymbolRGBA(idRGBA);
                
                colorsFromChartsEvent.applyChartColorToNode(nodeIndex,
                                                            idRGBA);
            }
            else {
                nodeID.getContralateralSymbolRGB(idRGBA);
            }
        }
        idRGBA[3] = 255;
        
        const int32_t i3 = nodeIndex * 3;
        glPushMatrix();
        glTranslatef(coordinates[i3], coordinates[i3+1], coordinates[i3+2]);
        this->drawSphereWithDiameter(idRGBA,
                                     symbolDiameter);
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
 * @param borderDrawInfo
 *   Info about border being drawn.
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
BrainOpenGLFixedPipeline::drawBorder(const BorderDrawInfo& borderDrawInfo)
{
    CaretAssert(borderDrawInfo.surface);
    CaretAssert(borderDrawInfo.border);

    const StructureEnum::Enum surfaceStructure = borderDrawInfo.surface->getStructure();
    const StructureEnum::Enum contralateralSurfaceStructure = StructureEnum::getContralateralStructure(surfaceStructure);
    const int32_t numBorderPoints = borderDrawInfo.border->getNumberOfPoints();
    const bool isHighlightEndPoints = borderDrawInfo.isHighlightEndPoints;
    
    float pointDiameter = 2.0;
    float lineWidth  = 2.0;
    BorderDrawingTypeEnum::Enum drawType = BorderDrawingTypeEnum::DRAW_AS_POINTS_SPHERES;
    if (borderDrawInfo.borderFileIndex >= 0) {
        const BrainStructure* bs = borderDrawInfo.surface->getBrainStructure();
        const Brain* brain = bs->getBrain();
        const DisplayPropertiesBorders* dpb = brain->getDisplayPropertiesBorders();
        const DisplayGroupEnum::Enum displayGroup = dpb->getDisplayGroupForTab(this->windowTabIndex);
        pointDiameter = dpb->getPointSize(displayGroup,
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
    
    const bool flatSurfaceFlag = (borderDrawInfo.surface->getSurfaceType() == SurfaceTypeEnum::FLAT);
    const float drawAtDistanceAboveSurface = 0.0;

    std::vector<float> pointXYZ;
    std::vector<int32_t> pointIndex;
    
    /*
     * Find points valid for this surface
     */
    for (int32_t i = 0; i < numBorderPoints; i++) {
        const SurfaceProjectedItem* p = borderDrawInfo.border->getPoint(i);
        
        /*
         * If surface structure does not match the point's structure,
         * check to see if contralateral display is enabled and 
         * compare contralateral surface structure to point's structure.
         */
        const StructureEnum::Enum pointStructure = p->getStructure();
        bool structureMatches = true;
        if (surfaceStructure != pointStructure) {
            structureMatches = false;
            if (borderDrawInfo.isContralateralEnabled) {
                if (contralateralSurfaceStructure == pointStructure) {
                    structureMatches = true;
                }
            }
        }
        if (structureMatches == false) {
            continue;
        }
        
        float xyz[3];
        const bool isXyzValid = p->getProjectedPositionAboveSurface(*borderDrawInfo.surface, 
                                                                    xyz,
                                                                    drawAtDistanceAboveSurface);
        
        if (isXyzValid) {
            pointXYZ.push_back(xyz[0]);
            pointXYZ.push_back(xyz[1]);
            pointXYZ.push_back(xyz[2]);
            pointIndex.push_back(i);
        }
    }    

    const ClippingPlaneGroup* clippingPlaneGroup = this->browserTabContent->getClippingPlaneGroup();
    const bool doClipping = clippingPlaneGroup->isFeaturesAndAnyAxisSelected();
    
    const int32_t numPointsToDraw = static_cast<int32_t>(pointXYZ.size() / 3);
    
    /*
     * Draw points
     */
    if (drawSphericalPoints
        || drawSquarePoints) {
        for (int32_t i = 0; i < numPointsToDraw; i++) {
            const int32_t i3 = i * 3;
            
            const float* xyz = &pointXYZ[i3];
            
            if (doClipping) {
                if ( ! clippingPlaneGroup->isCoordinateInsideClippingPlanesForStructure(surfaceStructure,
                                                                                        xyz)) {
                    continue;
                }
            }
            
            glPushMatrix();
            glTranslatef(xyz[0], xyz[1], xyz[2]);
            
            if (borderDrawInfo.isSelect) {
                uint8_t idRGBA[4];
                this->colorIdentification->addItem(idRGBA,
                                                   SelectionItemDataTypeEnum::BORDER_SURFACE, 
                                                   borderDrawInfo.borderFileIndex,
                                                   borderDrawInfo.borderIndex,
                                                   pointIndex[i]);
                idRGBA[3] = 255;
                if (drawSphericalPoints) {
                    this->drawSphereWithDiameter(idRGBA,
                                                 pointDiameter);
                }
                else {
                    this->drawSquare(idRGBA,
                                     pointDiameter);
                }
            }
            else {
                float rgba[4] = {
                    borderDrawInfo.rgba[0],
                    borderDrawInfo.rgba[1],
                    borderDrawInfo.rgba[2],
                    borderDrawInfo.rgba[3],
                };
                if (isHighlightEndPoints) {
                    if (i == 0) {
                        rgba[0] = 0.0;
                        rgba[1] = 1.0;
                        rgba[2] = 0.0;
                        rgba[3] = 1.0;
                    }
                    else if (i == (numPointsToDraw - 1)) {
                        rgba[0] = 0.0;
                        rgba[1] = 0.75;
                        rgba[2] = 0.0;
                        rgba[3] = 1.0;
                    }
                }
                if (drawSphericalPoints) {
                    this->drawSphereWithDiameter(rgba,
                                                 pointDiameter);
                }
                else {
                    this->drawSquare(rgba,
                                     pointDiameter);
                }
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
        
        if (borderDrawInfo.isSelect) {
            /*
             * Start at one, since need two points for each line
             */
            for (int32_t i = 1; i < numPointsToDraw; i++) {
                /*
                 * On a flat surface, do not draw a line segment if it is
                 * from non-consecutive border points.  This occurs when
                 * a border point does not project to the flat surface 
                 * due to a cut or removal of the medial wall.  If helps
                 * prevent long border lines stretching from one edge of the
                 * surface to a far away edge.
                 */
                if (flatSurfaceFlag) {
                    if (pointIndex[i] != (pointIndex[i-1] + 1)) {
                        continue;
                    }
                }
                
                const int32_t i3 = i * 3;
                uint8_t idRGBA[4];
                this->colorIdentification->addItem(idRGBA,
                                                   SelectionItemDataTypeEnum::BORDER_SURFACE, 
                                                   borderDrawInfo.borderFileIndex,
                                                   borderDrawInfo.borderIndex,
                                                   pointIndex[i]);
                glColor3ubv(idRGBA);
                
                const float* xyz1 = &pointXYZ[i3 - 3];
                const float* xyz2 = &pointXYZ[i3];
                
                bool drawIt = true;
                if (doClipping) {
                    if (clippingPlaneGroup->isCoordinateInsideClippingPlanesForStructure(surfaceStructure,
                                                                                         xyz1)
                        && (clippingPlaneGroup->isCoordinateInsideClippingPlanesForStructure(surfaceStructure,
                                                                                             xyz2)) ) {
                        /* nothing */
                    }
                    else {
                        drawIt = false;
                    }
                }
                
                if (drawIt) {
                    glBegin(GL_LINES);
                    glVertex3fv(xyz1);
                    glVertex3fv(xyz2);
                    glEnd();
                }
            }
        }
        else {
            glColor3fv(borderDrawInfo.rgba);
            
            /*
             * Start at one, since need two points for each line
             */
            glBegin(GL_LINES);
            for (int32_t i = 1; i < numPointsToDraw; i++) {
                /*
                 * On a flat surface, do not draw a line segment if it is
                 * from non-consecutive border points.  This occurs when
                 * a border point does not project to the flat surface
                 * due to a cut or removal of the medial wall.  If helps
                 * prevent long border lines stretching from one edge of the
                 * surface to a far away edge.
                 */
                if (flatSurfaceFlag) {
                    if (pointIndex[i] != (pointIndex[i-1] + 1)) {
                        continue;
                    }
                }
                
                const int32_t i3 = i * 3;
                const float* xyz1 = &pointXYZ[i3 - 3];
                const float* xyz2 = &pointXYZ[i3];
                
                bool drawIt = true;
                if (doClipping) {
                    if (clippingPlaneGroup->isCoordinateInsideClippingPlanesForStructure(surfaceStructure,
                                                                                         xyz1)
                        && (clippingPlaneGroup->isCoordinateInsideClippingPlanesForStructure(surfaceStructure,
                                                                                             xyz2)) ) {
                        /* nothing */
                    }
                    else {
                        drawIt = false;
                    }
                }
                
                if (drawIt) {
                    glVertex3fv(xyz1);
                    glVertex3fv(xyz2);
                }
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
    const float focusDiameter = fociDisplayProperties->getFociSize(displayGroup,
                                                                 this->windowTabIndex);
    const FeatureColoringTypeEnum::Enum fociColoringType = fociDisplayProperties->getColoringType(displayGroup,
                                                                                               this->windowTabIndex);
    
    const StructureEnum::Enum surfaceStructure = surface->getStructure();
    const StructureEnum::Enum surfaceContralateralStructure = StructureEnum::getContralateralStructure(surfaceStructure);
    
    const ClippingPlaneGroup* clippingPlaneGroup = this->browserTabContent->getClippingPlaneGroup();
    const bool doClipping = clippingPlaneGroup->isFeaturesAndAnyAxisSelected();
    
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

                    if (doClipping) {
                        if ( ! clippingPlaneGroup->isCoordinateInsideClippingPlanesForStructure(surfaceStructure,
                                                                                             xyz)) {
                            drawIt = false;
                        }
                    }
                    
                    if (drawIt) {
                        glPushMatrix();
                        glTranslatef(xyz[0], xyz[1], xyz[2]);
                        
                        if (isSelect) {
                            uint8_t idRGBA[4];
                            this->colorIdentification->addItem(idRGBA,
                                                               SelectionItemDataTypeEnum::FOCUS_SURFACE, 
                                                               i, // file index
                                                               j, // focus index
                                                               k);// projection index
                            idRGBA[3] = 255;
                            if (drawAsSpheres) {
                                this->drawSphereWithDiameter(idRGBA,
                                                             focusDiameter);
                            }
                            else {
                                this->drawSquare(idRGBA,
                                                 focusDiameter);
                            }
                        }
                        else {
                            if (drawAsSpheres) {
                                this->drawSphereWithDiameter(rgba,
                                                             focusDiameter);
                            }
                            else {
                                this->drawSquare(rgba,
                                                 focusDiameter);
                            }
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
            
            BorderDrawInfo borderDrawInfo;
            borderDrawInfo.surface = surface;
            borderDrawInfo.border = border;
            borderDrawInfo.rgba[0] = rgba[0];
            borderDrawInfo.rgba[1] = rgba[1];
            borderDrawInfo.rgba[2] = rgba[2];
            borderDrawInfo.rgba[3] = rgba[3];
            borderDrawInfo.borderFileIndex = i;
            borderDrawInfo.borderIndex = j;
            borderDrawInfo.isSelect = isSelect;
            borderDrawInfo.isContralateralEnabled = isContralateralEnabled;
            borderDrawInfo.isHighlightEndPoints = m_drawHighlightedEndPoints;
            this->drawBorder(borderDrawInfo);
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
        BorderDrawInfo borderDrawInfo;
        borderDrawInfo.surface = const_cast<Surface*>(surface);
        borderDrawInfo.border = this->borderBeingDrawn;
        borderDrawInfo.rgba[0] = 1.0;
        borderDrawInfo.rgba[1] = 0.0;
        borderDrawInfo.rgba[2] = 0.0;
        borderDrawInfo.rgba[3] = 1.0;
        borderDrawInfo.borderFileIndex = -1;
        borderDrawInfo.borderIndex = -1;
        borderDrawInfo.isSelect = false;
        borderDrawInfo.isContralateralEnabled = false;
        borderDrawInfo.isHighlightEndPoints = false;
        this->drawBorder(borderDrawInfo);
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
                    VolumeMappableInterface* vf = dynamic_cast<VolumeMappableInterface*>(mapFile);
                    if (vf != NULL) {
                        float opacity = overlay->getOpacity();
                        if (volumeDrawInfoOut.empty()) {
                            opacity = 1.0;
                        }
                        
                        WholeBrainVoxelDrawingMode::Enum wholeBrainVoxelDrawingMode = overlay->getWholeBrainVoxelDrawingMode();
                        
                        if (mapFile->isMappedWithPalette()) {
                            const FastStatistics* statistics = mapFile->getMapFastStatistics(mapIndex);
                            PaletteColorMapping* paletteColorMapping = mapFile->getMapPaletteColorMapping(mapIndex);
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
                                    
                                        VolumeDrawInfo vdi(mapFile,
                                                           vf,
                                                           brain,
                                                           paletteColorMapping,
                                                           statistics,
                                                           wholeBrainVoxelDrawingMode,
                                                           mapIndex,
                                                           opacity);
                                        volumeDrawInfoOut.push_back(vdi);
                                }
                            }
                            else {
                                CaretLogWarning("No valid palette for drawing volume file: "
                                                + mapFile->getFileNameNoPath());
                            }
                        }
                        else {
                            VolumeDrawInfo vdi(mapFile,
                                               vf,
                                               brain,
                                               NULL,
                                               NULL,
                                               wholeBrainVoxelDrawingMode,
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
 * @param volumeModel
 *    Model for slices.
 * @param viewport
 *    Region of drawing.
 */
void 
BrainOpenGLFixedPipeline::drawVolumeModel(BrowserTabContent* browserTabContent,
                                  ModelVolume* volumeModel,
                                  const int32_t viewport[4])
{
    /*
     * Determine volumes that are to be drawn
     */
    const int32_t tabNumber = browserTabContent->getTabNumber();
    volumeModel->updateModel(tabNumber);
    Brain* brain = volumeModel->getBrain();
    std::vector<VolumeDrawInfo> volumeDrawInfo;
    this->setupVolumeDrawInfo(browserTabContent,
                              brain,
                              volumeDrawInfo);
    
    const VolumeSliceViewModeEnum::Enum sliceViewMode = browserTabContent->getSliceViewMode();
    
    BrainOpenGLVolumeSliceDrawing volumeSliceDrawing;
    volumeSliceDrawing.draw(this,
                        browserTabContent,
                        volumeDrawInfo,
                        sliceViewMode,
                        viewport);
}

///**
// * Draw the volume axes crosshairs.
// * @param brain
// *   Brain that owns the volumes.
// * @param slicePlane
// *   Plane being viewed.
// * @param voxelXYZ
// *   Stereotaxic coordinate of current slice indices.
// */
//void 
//BrainOpenGLFixedPipeline::drawVolumeAxesCrosshairs(
//                                               const VolumeSliceViewPlaneEnum::Enum slicePlane,
//                                               const float voxelXYZ[3])
//{
//    if (SessionManager::get()->getCaretPreferences()->isVolumeAxesCrosshairsDisplayed()) {
//        unsigned char red[3]   = { 255, 0, 0 };
//        unsigned char green[3] = { 0, 255, 0 };
//        unsigned char blue[3]  = { 0, 0, 255 };
//        
//        const bool drawIt = (slicePlane != VolumeSliceViewPlaneEnum::ALL);
//        
//        float voxelXYZmod[3] = { voxelXYZ[0], voxelXYZ[1], voxelXYZ[2] };
//        
//        switch (slicePlane)//prevent disappearing crosshairs when zoomed in by zeroing the into/out of plane coords
//        {
//            case VolumeSliceViewPlaneEnum::ALL:
//                break;
//            case VolumeSliceViewPlaneEnum::AXIAL:
//                voxelXYZmod[2] = 0.0f;
//                break;
//            case VolumeSliceViewPlaneEnum::CORONAL:
//                voxelXYZmod[1] = 0.0f;
//                break;
//            case VolumeSliceViewPlaneEnum::PARASAGITTAL:
//                voxelXYZmod[0] = 0.0f;
//                break;
//        }
//        
//        if (drawIt) {
//            const float bigNumber = 10000;
//            this->setLineWidth(1.0);
//            glColor3ubv(green);
//            glBegin(GL_LINES);
//            glVertex3f(voxelXYZmod[0], -bigNumber, voxelXYZmod[2]);
//            glVertex3f(voxelXYZmod[0],  bigNumber, voxelXYZmod[2]);
//            glColor3ubv(red);
//            glVertex3f(-bigNumber, voxelXYZmod[1], voxelXYZmod[2]);
//            glVertex3f( bigNumber, voxelXYZmod[1], voxelXYZmod[2]);
//            glColor3ubv(blue);
//            glVertex3f(voxelXYZmod[0], voxelXYZmod[1], -bigNumber);
//            glVertex3f(voxelXYZmod[0], voxelXYZmod[1],  bigNumber);
//            glEnd();
//        }
//    }    
//}

///**
// * Draw the volume axes labels.
// * @param brain
// *   Brain that owns the volumes.
// * @param slicePlane
// *   Plane being viewed.
// * @param viewport
// *   Viewport of drawing region.
// */
//void 
//BrainOpenGLFixedPipeline::drawVolumeAxesLabels(
//                          const VolumeSliceViewPlaneEnum::Enum slicePlane,
//                          const int32_t viewport[4])
//{
//    if (SessionManager::get()->getCaretPreferences()->isVolumeAxesLabelsDisplayed()) {
//        glMatrixMode(GL_PROJECTION);
//        glPushMatrix();
//        glOrtho(0, viewport[2], 0, viewport[3], -1.0, 1.0);
//        
//        glMatrixMode(GL_MODELVIEW);
//        glPushMatrix();
//        glLoadIdentity();
//        
//        QString orientLeftSideLabel;
//        QString orientRightSideLabel;
//        QString orientBottomSideLabel;
//        QString orientTopSideLabel;
//        
//        switch(slicePlane) {
//            case VolumeSliceViewPlaneEnum::AXIAL:
//                orientLeftSideLabel   = "L";
//                orientRightSideLabel  = "R";
//                orientBottomSideLabel = "P";
//                orientTopSideLabel    = "A";
//                break;
//            case VolumeSliceViewPlaneEnum::CORONAL:
//                orientLeftSideLabel   = "L";
//                orientRightSideLabel  = "R";
//                orientBottomSideLabel = "V";
//                orientTopSideLabel    = "D";
//                break;
//            case VolumeSliceViewPlaneEnum::PARASAGITTAL:
//                orientLeftSideLabel   = "A";
//                orientRightSideLabel  = "P";
//                orientBottomSideLabel = "V";
//                orientTopSideLabel    = "D";
//                break;
//            default:
//                break;
//        }
//        
//        /*
//         * Switch to the foreground color.
//         */
//        uint8_t foregroundRGB[3];
//        SessionManager::get()->getCaretPreferences()->getColorForeground(foregroundRGB);
//        glColor3ubv(foregroundRGB);
//        
//        const int fontHeight = 18;
//        this->drawBoldTextWindowCoords(5,
//                                   (viewport[3] / 2), 
//                                   orientLeftSideLabel, 
//                                   BrainOpenGLTextRenderInterface::X_LEFT,
//                                   BrainOpenGLTextRenderInterface::Y_CENTER,
//                                   fontHeight);
//
//        this->drawBoldTextWindowCoords((viewport[2] - 5), 
//                                   (viewport[3] / 2), 
//                                   orientRightSideLabel, 
//                                   BrainOpenGLTextRenderInterface::X_RIGHT,
//                                   BrainOpenGLTextRenderInterface::Y_CENTER,
//                                   fontHeight);
//        
//        this->drawBoldTextWindowCoords((viewport[2] / 2), 
//                                   5, 
//                                   orientBottomSideLabel, 
//                                   BrainOpenGLTextRenderInterface::X_CENTER,
//                                   BrainOpenGLTextRenderInterface::Y_BOTTOM,
//                                   fontHeight);
//        
//        this->drawBoldTextWindowCoords((viewport[2] / 2), 
//                                   (viewport[3] - 5), 
//                                   orientTopSideLabel, 
//                                   BrainOpenGLTextRenderInterface::X_CENTER,
//                                   BrainOpenGLTextRenderInterface::Y_TOP,
//                                   fontHeight);
//        
//        glPopMatrix();
//        
//        glMatrixMode(GL_PROJECTION);
//        glPopMatrix();
//        
//        glMatrixMode(GL_MODELVIEW);
//    }
//}

//void
//BrainOpenGLFixedPipeline::drawVolumeOrthogonalSliceVolumeViewer(const VolumeSliceViewPlaneEnum::Enum slicePlane,
//                                                                const int64_t sliceIndex,
//                                                                std::vector<VolumeDrawInfo>& volumeDrawInfo)
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
//    const int32_t browserTabIndex = browserTabContent->getTabNumber();
//    const DisplayPropertiesLabels* displayPropertiesLabels = m_brain->getDisplayPropertiesLabels();
//    const DisplayGroupEnum::Enum displayGroup = displayPropertiesLabels->getDisplayGroupForTab(browserTabIndex);
//    const LabelDrawingTypeEnum::Enum labelDrawingType = displayPropertiesLabels->getDrawingType(displayGroup,
//                                                                                                browserTabIndex);
//    bool isOutlineMode = false;
//    switch (labelDrawingType) {
//        case LabelDrawingTypeEnum::DRAW_FILLED:
//            break;
//        case LabelDrawingTypeEnum::DRAW_OUTLINE:
//            isOutlineMode = true;
//            break;
//    }
//    
//    /**
//     * Holds colors for voxels in the slice
//     * Outside of loop to minimize allocations
//     * It is faster to make one call to
//     * NodeAndVoxelColoring::colorScalarsWithPalette() with
//     * all voxels in the slice than it is to call it
//     * separately for each voxel.
//     */
//    std::vector<uint8_t> sliceVoxelsRgbaVector;
//    
//    /*
//     * Draw each of the volumes separately so that each
//     * is drawn with the correct voxel slices.
//     */
//    float sliceCoordinate = 0.0;
//    for (int32_t iVol = 0; iVol < numberOfVolumesToDraw; iVol++) {
//        const VolumeDrawInfo& volInfo = volumeDrawInfo[iVol];
//        const VolumeMappableInterface* volumeFile = volInfo.volumeFile;
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
//            int64_t voxelI, voxelJ, voxelK;
//            volumeFile->enclosingVoxel(midPoint[0], midPoint[1], midPoint[2], voxelI, voxelJ, voxelK);
//            if (volumeFile->indexValid(voxelI, voxelJ, voxelK)) {
//                switch (slicePlane) {
//                    case VolumeSliceViewPlaneEnum::ALL:
//                        CaretAssert(0);
//                        break;
//                    case VolumeSliceViewPlaneEnum::AXIAL:
//                        drawingSliceIndex = voxelK;
//                        break;
//                    case VolumeSliceViewPlaneEnum::CORONAL:
//                        drawingSliceIndex = voxelJ;
//                        break;
//                    case VolumeSliceViewPlaneEnum::PARASAGITTAL:
//                        drawingSliceIndex = voxelI;
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
//            
//            /*
//             * Stores RGBA values for each voxel.
//             * Use a vector for voxel colors so no worries about memory being freed.
//             */
//            const int64_t numVoxelsInSliceRGBA = numVoxelsInSlice * 4;
//            if (numVoxelsInSliceRGBA > static_cast<int64_t>(sliceVoxelsRgbaVector.size())) {
//                sliceVoxelsRgbaVector.resize(numVoxelsInSliceRGBA);
//            }
//            uint8_t* sliceVoxelsRGBA = &sliceVoxelsRgbaVector[0];
//            
//            /*
//             * Get colors for all voxels in the slice.
//             */
//            volumeFile->getVoxelColorsForSliceInMap(m_brain->getPaletteFile(),
//                                                    mapIndex,
//                                                    slicePlane,
//                                                    drawingSliceIndex,
//                                                    displayGroup,
//                                                    browserTabIndex,
//                                                    sliceVoxelsRGBA);
//            
//            /*
//             * Is label outline mode?
//             */
//            if (volumeDrawInfo[iVol].mapFile->isMappedWithLabelTable()) {
//                if (isOutlineMode) {
//                    int64_t xdim = 0;
//                    int64_t ydim = 0;
//                    switch (slicePlane) {
//                        case VolumeSliceViewPlaneEnum::ALL:
//                            CaretAssert(0);
//                            break;
//                        case VolumeSliceViewPlaneEnum::AXIAL:
//                            xdim = dimI;
//                            ydim = dimJ;
//                            break;
//                        case VolumeSliceViewPlaneEnum::CORONAL:
//                            xdim = dimI;
//                            ydim = dimK;
//                            break;
//                        case VolumeSliceViewPlaneEnum::PARASAGITTAL:
//                            xdim = dimJ;
//                            ydim = dimK;
//                            break;
//                    }
//                    
//                    NodeAndVoxelColoring::convertSliceColoringToOutlineMode(sliceVoxelsRGBA,
//                                                                            xdim,
//                                                                            ydim);
//                }
//            }
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
//                        sliceVoxelsRGBA[alphaIndex - 3] = 0;
//                        sliceVoxelsRGBA[alphaIndex - 2] = 0;
//                        sliceVoxelsRGBA[alphaIndex - 1] = 0;
//                        sliceVoxelsRGBA[alphaIndex] = 255.0;
//                    }
//                    else {
//                        /*
//                         * Do not show voxel
//                         */
//                        sliceVoxelsRGBA[alphaIndex] = 0;
//                    }
//                }
//                else {
//                    /*
//                     * Use overlay's opacity
//                     */
//                    sliceVoxelsRGBA[alphaIndex] = static_cast<int8_t>(volInfo.opacity * 255.0);
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
//                                    glColor4ubv(&sliceVoxelsRGBA[sliceRgbaOffset]);
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
//                                    glColor4ubv(&sliceVoxelsRGBA[sliceRgbaOffset]);
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
//                                    glColor4ubv(&sliceVoxelsRGBA[sliceRgbaOffset]);
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
//                uint8_t rgba[4];
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
//                                    this->colorIdentification->addItem(rgba,
//                                                                       SelectionItemDataTypeEnum::VOXEL,
//                                                                       idVoxelCounter);
//                                    glColor3ubv(rgba);
//                                    
//                                    idVoxelCoordinates.push_back(x1 + halfVoxelStepX);
//                                    idVoxelCoordinates.push_back(y1 + halfVoxelStepY);
//                                    idVoxelCoordinates.push_back(sliceCoordinate); // coord of slice is not offset by half voxel
//                                    idVoxelCounter++;
//                                }
//                                else {
//                                    const int32_t sliceRgbaOffset = (i + (j * dimI)) * 4;
//                                    CaretAssertVectorIndex(sliceVoxelsRgbaVector, sliceRgbaOffset+3);
//                                    glColor4ubv(&sliceVoxelsRGBA[sliceRgbaOffset]);
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
//                                    this->colorIdentification->addItem(rgba,
//                                                                       SelectionItemDataTypeEnum::VOXEL,
//                                                                       idVoxelCounter);
//                                    glColor3ubv(rgba);
//                                    
//                                    idVoxelCoordinates.push_back(x1 + halfVoxelStepX);
//                                    idVoxelCoordinates.push_back(sliceCoordinate); // coord of slice is not offset by half voxel
//                                    idVoxelCoordinates.push_back(z1 + halfVoxelStepZ);
//                                    idVoxelCounter++;
//                                }
//                                else {
//                                    const int32_t sliceRgbaOffset = (i + (k * dimI)) * 4;
//                                    CaretAssertVectorIndex(sliceVoxelsRgbaVector, sliceRgbaOffset+3);
//                                    glColor4ubv(&sliceVoxelsRGBA[sliceRgbaOffset]);
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
//                                    this->colorIdentification->addItem(rgba,
//                                                                       SelectionItemDataTypeEnum::VOXEL,
//                                                                       idVoxelCounter);
//                                    glColor3ubv(rgba);
//                                    
//                                    idVoxelCoordinates.push_back(sliceCoordinate); // coord of slice is not offset by half voxel
//                                    idVoxelCoordinates.push_back(y1 + halfVoxelStepY);
//                                    idVoxelCoordinates.push_back(z1 + halfVoxelStepZ);
//                                    idVoxelCounter++;
//                                }
//                                else {
//                                    const int32_t sliceRgbaOffset = (j + (k * dimJ)) * 4;
//                                    CaretAssertVectorIndex(sliceVoxelsRgbaVector, sliceRgbaOffset+3);
//                                    glColor4ubv(&sliceVoxelsRGBA[sliceRgbaOffset]);
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
//                            VolumeMappableInterface* vf = volumeDrawInfo[iVol].volumeFile;
//                            CaretMappableDataFile* mapFile = dynamic_cast<CaretMappableDataFile*>(vf);
//                            CaretAssert(mapFile);
//                            
//                            int64_t voxelIndices[3];
//                            vf->enclosingVoxel(voxelCoordinates[0], voxelCoordinates[1], voxelCoordinates[2],
//                                               voxelIndices[0], voxelIndices[1], voxelIndices[2]);
//                            if (vf->indexValid(voxelIndices[0], voxelIndices[1], voxelIndices[2])) {
//                                if (voxelID->isOtherScreenDepthCloserToViewer(depth)) {
//                                    voxelID->setBrain(volumeDrawInfo[iVol].brain);
//                                    voxelID->setVolumeFile(volumeDrawInfo[iVol].volumeFile);
//                                    voxelID->setVoxelIJK(voxelIndices);
//                                    voxelID->setScreenDepth(depth);
//                                    this->setSelectedItemScreenXYZ(voxelID, voxelCoordinates);
//                                    float voxelXYZ[3];
//                                    vf->indexToSpace(voxelIndices,
//                                                     voxelXYZ);
//                                    CaretLogFine("Selected Voxel from file name/IJK/coords/ID-coords: "
//                                                 + mapFile->getFileNameNoPath()
//                                                 + AString::fromNumbers(voxelIndices, 3, ",")
//                                                   + "   "
//                                                   + AString::fromNumbers(voxelXYZ, 3, " ")
//                                                   + "   "
//                                                   + AString::fromNumbers(voxelCoordinates, 3, " "));
//
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
//    //    glEnable(GL_DEPTH_TEST); olumeFile
//    
//    glDisable(GL_BLEND);
//}


/**
 * Draw volumes a voxel cubes for whole brain view.
 *
 * @param volumeDrawInfoIn
 *    Describes volumes that are drawn.
 */
void
BrainOpenGLFixedPipeline::drawVolumeVoxelsAsCubesWholeBrain(std::vector<VolumeDrawInfo>& volumeDrawInfoIn)
{
    /*
     * Filter volumes for drawing and only draw those volumes that
     * are to be drawn as 3D Voxel Cubes.
     */
    std::vector<VolumeDrawInfo> volumeDrawInfo;
    for (std::vector<VolumeDrawInfo>::iterator iter = volumeDrawInfoIn.begin();
         iter != volumeDrawInfoIn.end();
         iter++) {
        bool useIt = false;
        VolumeDrawInfo& vdi = *iter;
        switch (vdi.wholeBrainVoxelDrawingMode) {
            case WholeBrainVoxelDrawingMode::DRAW_VOXELS_AS_THREE_D_CUBES:
            case WholeBrainVoxelDrawingMode::DRAW_VOXELS_AS_ROUNDED_THREE_D_CUBES:
                useIt = true;
                break;
            case WholeBrainVoxelDrawingMode::DRAW_VOXELS_ON_TWO_D_SLICES:
                break;
        }
        if (useIt) {
            volumeDrawInfo.push_back(vdi);
        }
    }
    
    const int32_t numberOfVolumesToDraw = static_cast<int32_t>(volumeDrawInfo.size());
    if (numberOfVolumesToDraw <= 0) {
        return;
    }
    
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
     * When selecting turn on lighting and shading since
     * colors are used for identification.
     */
    if (isSelect) {
        this->disableLighting();
        glShadeModel(GL_FLAT);
    }
    else {
        this->enableLighting();
        glEnable(GL_CULL_FACE);
        glShadeModel(GL_SMOOTH);
    }
    
    glEnable(GL_CULL_FACE);
    
    const DisplayPropertiesLabels* dsl = m_brain->getDisplayPropertiesLabels();
    const DisplayGroupEnum::Enum displayGroup = dsl->getDisplayGroupForTab(this->windowTabIndex);
    
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

    PaletteFile* paletteFile = m_brain->getPaletteFile();
    
    for (int32_t iVol = 0; iVol < numberOfVolumesToDraw; iVol++) {
        VolumeDrawInfo& volInfo = volumeDrawInfo[iVol];
        if (volInfo.opacity < 1.0) {
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        }
        else {
            glDisable(GL_BLEND);
        }
        const VolumeMappableInterface* volumeFile = volInfo.volumeFile;
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
        
        /*
         * Cube size for voxel drawing.  Some volumes may have a right to left
         * orientation in which case dx may be negative.
         */
        const float cubeSizeDX = std::fabs(dx);
        const float cubeSizeDY = std::fabs(dy);
        const float cubeSizeDZ = std::fabs(dz);
        
        std::vector<float> labelMapData;
        const CiftiBrainordinateLabelFile* ciftiLabelFile = dynamic_cast<const CiftiBrainordinateLabelFile*>(volumeFile);
        if (ciftiLabelFile != NULL) {
            ciftiLabelFile->getMapData(volInfo.mapIndex,
                                       labelMapData);
        }
        
        uint8_t rgba[4];
        for (int64_t iVoxel = 0; iVoxel < dimI; iVoxel++) {
            for (int64_t jVoxel = 0; jVoxel < dimJ; jVoxel++) {
                for (int64_t kVoxel = 0; kVoxel < dimK; kVoxel++) {
                    if (ciftiLabelFile != NULL) {
                        ciftiLabelFile->getVoxelColorInMapForLabelData(paletteFile,
                                                                       labelMapData,
                                                                       iVoxel,
                                                                       jVoxel,
                                                                       kVoxel,
                                                                       volInfo.mapIndex,
                                                                       displayGroup,
                                                                       this->windowTabIndex,
                                                                       rgba);
                    }
                    else {
                        volumeFile->getVoxelColorInMap(paletteFile,
                                                       iVoxel,
                                                       jVoxel,
                                                       kVoxel,
                                                       volInfo.mapIndex,
                                                       displayGroup,
                                                       this->windowTabIndex,
                                                       rgba);
                    }
                    if (rgba[3] > 0) {
                        if (volInfo.opacity < 1.0) {
                            rgba[3] *= volInfo.opacity;
                        }
                        if (rgba[3] > 0) {
                            if (isSelect) {
                                const int32_t idIndex = identificationIndices.size() / idPerVoxelCount;
                                this->colorIdentification->addItem(rgba,
                                                                   SelectionItemDataTypeEnum::VOXEL,
                                                                   idIndex);
                                identificationIndices.push_back(iVol);
                                identificationIndices.push_back(volInfo.mapIndex);
                                identificationIndices.push_back(iVoxel);
                                identificationIndices.push_back(jVoxel);
                                identificationIndices.push_back(kVoxel);
                            }
                            
                            const float x = iVoxel * dx + originX;
                            const float y = jVoxel * dy + originY;
                            const float z = kVoxel * dz + originZ;
                            glPushMatrix();
                            glTranslatef(x, y, z);
                            switch (volInfo.wholeBrainVoxelDrawingMode) {
                                case WholeBrainVoxelDrawingMode::DRAW_VOXELS_AS_THREE_D_CUBES:
                                    drawCuboid(rgba, cubeSizeDX, cubeSizeDY, cubeSizeDZ);
                                    break;
                                case WholeBrainVoxelDrawingMode::DRAW_VOXELS_AS_ROUNDED_THREE_D_CUBES:
                                    drawRoundedCuboid(rgba, cubeSizeDX, cubeSizeDY, cubeSizeDZ);
                                    break;
                                case WholeBrainVoxelDrawingMode::DRAW_VOXELS_ON_TWO_D_SLICES:
                                    break;
                            }
                            glPopMatrix();
                        }
                    }
                }
            }
        }
    }
    
    if (isSelect) {
        int32_t identifiedItemIndex;
        float depth = -1.0;
        this->getIndexFromColorSelection(SelectionItemDataTypeEnum::VOXEL,
                                         this->mouseX,
                                         this->mouseY,
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
                
                this->setSelectedItemScreenXYZ(voxelID,
                                               voxelCoordinates);
                CaretLogFine("Selected Voxel (3D): " + AString::fromNumbers(voxelIndices, 3, ","));
            }
        }
    }
    
    glShadeModel(GL_SMOOTH);
    glDisable(GL_BLEND);
}

void
BrainOpenGLFixedPipeline::setFiberOrientationDisplayInfo(const DisplayPropertiesFiberOrientation* dpfo,
                                                         const DisplayGroupEnum::Enum displayGroup,
                                                         const int32_t tabIndex,
                                                         const ClippingPlaneGroup* clippingPlaneGroup,
                                                         Plane* plane,
                                                         const StructureEnum::Enum structure,
                                                         FiberTrajectoryColorModel::Item* colorSource,
                                    FiberOrientationDisplayInfo& dispInfo)
{
    dispInfo.aboveLimit = dpfo->getAboveLimit(displayGroup, tabIndex);
    dispInfo.belowLimit = dpfo->getBelowLimit(displayGroup, tabIndex);
    dispInfo.clippingPlaneGroup = clippingPlaneGroup;
    dispInfo.colorSource = colorSource;
    dispInfo.fiberOrientationColorType = dpfo->getColoringType(displayGroup, tabIndex);
    dispInfo.fanMultiplier = dpfo->getFanMultiplier(displayGroup, tabIndex);
    dispInfo.isDrawWithMagnitude = dpfo->isDrawWithMagnitude(displayGroup, tabIndex);
    dispInfo.minimumMagnitude = dpfo->getMinimumMagnitude(displayGroup, tabIndex);
    dispInfo.magnitudeMultiplier = dpfo->getLengthMultiplier(displayGroup, tabIndex);
    dispInfo.plane = plane;
    dispInfo.structure = structure;
    dispInfo.symbolType = dpfo->getSymbolType(displayGroup, tabIndex);
}

/**
 * Draw fibers for a surface or a volume.
 *
 * @param plane
 *    If not NULL, it is the plane of the volume slice being drawn and
 *    only fibers within the above and below limits from the plane will
 *    be drawn.
 * @param structure
 *    The structure.
 */
void
BrainOpenGLFixedPipeline::drawFiberOrientations(const Plane* plane,
                                                const StructureEnum::Enum structure)
{
    const DisplayPropertiesFiberOrientation* dpfo = m_brain->getDisplayPropertiesFiberOrientation();
    const DisplayGroupEnum::Enum displayGroup = dpfo->getDisplayGroupForTab(this->windowTabIndex);
    if (dpfo->isDisplayed(displayGroup, this->windowTabIndex) == false) {
        return;
    }
    const FiberOrientationSymbolTypeEnum::Enum symbolType = dpfo->getSymbolType(displayGroup, this->windowTabIndex);
    
//    /*
//     * Clipping planes
//     */
//    BoundingBox clippingBoundingBox;
//    clippingBoundingBox.resetWithMaximumExtent();
//    
//    if (browserTabContent->isClippingPlaneEnabled(0)) {
//        const float halfThick = (browserTabContent->getClippingPlaneThickness(0)
//                                 * 0.5);
//        const float minValue = (browserTabContent->getClippingPlaneCoordinate(0)
//                                - halfThick);
//        const float maxValue = (browserTabContent->getClippingPlaneCoordinate(0)
//                                + halfThick);
//        clippingBoundingBox.setMinX(minValue);
//        clippingBoundingBox.setMaxX(maxValue);
//    }
//    if (browserTabContent->isClippingPlaneEnabled(1)) {
//        const float halfThick = (browserTabContent->getClippingPlaneThickness(1)
//                                 * 0.5);
//        const float minValue = (browserTabContent->getClippingPlaneCoordinate(1)
//                                - halfThick);
//        const float maxValue = (browserTabContent->getClippingPlaneCoordinate(1)
//                                + halfThick);
//        clippingBoundingBox.setMinY(minValue);
//        clippingBoundingBox.setMaxY(maxValue);
//    }
//    if (browserTabContent->isClippingPlaneEnabled(2)) {
//        const float halfThick = (browserTabContent->getClippingPlaneThickness(2)
//                                 * 0.5);
//        const float minValue = (browserTabContent->getClippingPlaneCoordinate(2)
//                                - halfThick);
//        const float maxValue = (browserTabContent->getClippingPlaneCoordinate(2)
//                                + halfThick);
//        clippingBoundingBox.setMinZ(minValue);
//        clippingBoundingBox.setMaxZ(maxValue);
//    }
    
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

    
    /*
     * Default constructor is color by fiber orientation settings XYZ/123 as RGB
     */
    FiberTrajectoryColorModel::Item colorUseFiber;
    
    FiberOrientationDisplayInfo fiberOrientDispInfo;
    setFiberOrientationDisplayInfo(dpfo,
                                   displayGroup,
                                   this->windowTabIndex,
                                   this->browserTabContent->getClippingPlaneGroup(),
                                   const_cast<Plane*>(plane),
                                   structure,
                                   &colorUseFiber,
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
                
                for (int32_t ifi = 0; ifi < fiberOrientation->m_numberOfFibers; ifi++) {
                    fiberOrientation->m_fibers[ifi]->m_opacityForDrawing = 1.0;
                }
                
                addFiberOrientationForDrawing(&fiberOrientDispInfo,
                                              fiberOrientation);
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
    if (fodi->clippingPlaneGroup->isFeaturesAndAnyAxisSelected()) {
        if ( ! fodi->clippingPlaneGroup->isCoordinateInsideClippingPlanesForStructure(fodi->structure,
                                                                                      fiberOrientation->m_xyz)) {
            return;
        }
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
    
    for (std::list<FiberOrientation*>::const_iterator iter = m_fiberOrientationsForDrawing.begin();
         iter != m_fiberOrientationsForDrawing.end();
         iter++) {
        const FiberOrientation* fiberOrientation = *iter;
        
        const float rawDepth =(m0 * fiberOrientation->m_xyz[0]
                            + m1 * fiberOrientation->m_xyz[1]
                            + m2 * fiberOrientation->m_xyz[2]
                            + m3);
        const float screenDepth = ((rawDepth + 1.0) / 2.0);

        fiberOrientation->m_drawingDepth = screenDepth;
        
    }
    
    m_fiberOrientationsForDrawing.sort(fiberDepthCompare);
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
                    alpha = fiber->m_opacityForDrawing;
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
                
                float fiberRGBA[4] = { 0.0, 0.0, 0.0, 0.0 };
                
                /*
                 * Color of fiber
                 */
                switch (fodi->colorSource->getItemType()) {
                    case FiberTrajectoryColorModel::Item::ITEM_TYPE_FIBER_ORIENTATION_COLORING_TYPE:
                        switch (fodi->fiberOrientationColorType) {
                            case FiberOrientationColoringTypeEnum::FIBER_COLORING_FIBER_INDEX_AS_RGB:
                            {
                                const int32_t indx = j % 3;
                                switch (indx) {
                                    case 0: // use RED
                                        glColor4f(BrainOpenGLFixedPipeline::COLOR_RED[0],
                                                  BrainOpenGLFixedPipeline::COLOR_RED[1],
                                                  BrainOpenGLFixedPipeline::COLOR_RED[2],
                                                  alpha);
                                        fiberRGBA[0] = BrainOpenGLFixedPipeline::COLOR_RED[0];
                                        fiberRGBA[1] = BrainOpenGLFixedPipeline::COLOR_RED[1];
                                        fiberRGBA[2] = BrainOpenGLFixedPipeline::COLOR_RED[2];
                                        fiberRGBA[3] = alpha;
                                        break;
                                    case 1: // use BLUE
                                        glColor4f(BrainOpenGLFixedPipeline::COLOR_BLUE[0],
                                                  BrainOpenGLFixedPipeline::COLOR_BLUE[1],
                                                  BrainOpenGLFixedPipeline::COLOR_BLUE[2],
                                                  alpha);
                                        fiberRGBA[0] = BrainOpenGLFixedPipeline::COLOR_BLUE[0];
                                        fiberRGBA[1] = BrainOpenGLFixedPipeline::COLOR_BLUE[1];
                                        fiberRGBA[2] = BrainOpenGLFixedPipeline::COLOR_BLUE[2];
                                        fiberRGBA[3] = alpha;
                                        break;
                                    case 2: // use GREEN
                                        glColor4f(BrainOpenGLFixedPipeline::COLOR_GREEN[0],
                                                  BrainOpenGLFixedPipeline::COLOR_GREEN[1],
                                                  BrainOpenGLFixedPipeline::COLOR_GREEN[2],
                                                  alpha);
                                        fiberRGBA[0] = BrainOpenGLFixedPipeline::COLOR_GREEN[0];
                                        fiberRGBA[1] = BrainOpenGLFixedPipeline::COLOR_GREEN[1];
                                        fiberRGBA[2] = BrainOpenGLFixedPipeline::COLOR_GREEN[2];
                                        fiberRGBA[3] = alpha;
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
                                fiberRGBA[0] = fiber->m_directionUnitVectorRGB[0];
                                fiberRGBA[1] = fiber->m_directionUnitVectorRGB[1];
                                fiberRGBA[2] = fiber->m_directionUnitVectorRGB[2];
                                fiberRGBA[3] = alpha;
                                break;
                        }
                        break;
                    case FiberTrajectoryColorModel::Item::ITEM_TYPE_CARET_COLOR:
                    {
                        const CaretColorEnum::Enum caretColor = fodi->colorSource->getCaretColor();
                        const float* rgb = CaretColorEnum::toRGB(caretColor);
                        glColor4f(rgb[0],
                                  rgb[1],
                                  rgb[2],
                                  alpha);
                        fiberRGBA[0] = rgb[0];
                        fiberRGBA[1] = rgb[1];
                        fiberRGBA[2] = rgb[2];
                        fiberRGBA[3] = alpha;
                    }
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
                        m_shapeCone->draw(fiberRGBA);
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
                        m_shapeCone->draw(fiberRGBA);
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
 * Draw fiber trajectories on a surface.
 */
void
BrainOpenGLFixedPipeline::drawSurfaceFiberTrajectories(const StructureEnum::Enum structure)
{
    drawFiberTrajectories(NULL,
                          structure);
}

/**
 * Draw the fiber trajectories.
 * @param plane
 *    If a volume it is non-NULL and contains the plane of the slice.
 * @param structure
 *    The structure.
 */
void
BrainOpenGLFixedPipeline::drawFiberTrajectories(const Plane* plane,
                                                const StructureEnum::Enum structure)
{
//    /*
//     * Clipping planes
//     */
//    BoundingBox clippingBoundingBox;
//    clippingBoundingBox.resetWithMaximumExtent();
//    
//    if (browserTabContent->isClippingPlaneEnabled(0)) {
//        const float halfThick = (browserTabContent->getClippingPlaneThickness(0)
//                                 * 0.5);
//        const float minValue = (browserTabContent->getClippingPlaneCoordinate(0)
//                                - halfThick);
//        const float maxValue = (browserTabContent->getClippingPlaneCoordinate(0)
//                                + halfThick);
//        clippingBoundingBox.setMinX(minValue);
//        clippingBoundingBox.setMaxX(maxValue);
//    }
//    if (browserTabContent->isClippingPlaneEnabled(1)) {
//        const float halfThick = (browserTabContent->getClippingPlaneThickness(1)
//                                 * 0.5);
//        const float minValue = (browserTabContent->getClippingPlaneCoordinate(1)
//                                - halfThick);
//        const float maxValue = (browserTabContent->getClippingPlaneCoordinate(1)
//                                + halfThick);
//        clippingBoundingBox.setMinY(minValue);
//        clippingBoundingBox.setMaxY(maxValue);
//    }
//    if (browserTabContent->isClippingPlaneEnabled(2)) {
//        const float halfThick = (browserTabContent->getClippingPlaneThickness(2)
//                                 * 0.5);
//        const float minValue = (browserTabContent->getClippingPlaneCoordinate(2)
//                                - halfThick);
//        const float maxValue = (browserTabContent->getClippingPlaneCoordinate(2)
//                                + halfThick);
//        clippingBoundingBox.setMinZ(minValue);
//        clippingBoundingBox.setMaxZ(maxValue);
//    }
    
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
    
    CaretAssert(this->browserTabContent);
    OverlaySet* overlaySet = this->browserTabContent->getOverlaySet();
    const int32_t numberOfDisplayedOverlays = overlaySet->getNumberOfDisplayedOverlays();
    for (int32_t iOver = 0; iOver < numberOfDisplayedOverlays; iOver++) {
        Overlay* overlay = overlaySet->getOverlay(iOver);
        if (overlay->isEnabled() == false) {
            continue;
        }
        
        CaretMappableDataFile* caretMappableDataFile = NULL;
        int32_t mapIndex = -1;
        overlay->getSelectionData(caretMappableDataFile,
                                  mapIndex);
        if (caretMappableDataFile == NULL) {
            continue;
        }
        CiftiFiberTrajectoryFile* trajFile = dynamic_cast<CiftiFiberTrajectoryFile*>(caretMappableDataFile);
        if (trajFile == NULL) {
            continue;
        }
        
        FiberTrajectoryMapProperties* ftmp = trajFile->getFiberTrajectoryMapProperties();
        
        const float proportionMinimumOpacity = ftmp->getProportionMinimumOpacity();
        const float proportionMaximumOpacity = ftmp->getProportionMaximumOpacity();
        const float proportionRangeOpacity = proportionMaximumOpacity - proportionMinimumOpacity;
        
        const float countMinimumOpacity = ftmp->getCountMinimumOpacity();
        const float countMaximumOpacity = ftmp->getCountMaximumOpacity();
        const float countRangeOpacity = countMaximumOpacity - countMinimumOpacity;
        
        const float distanceMinimumOpacity = ftmp->getDistanceMinimumOpacity();
        const float distanceMaximumOpacity = ftmp->getDistanceMaximumOpacity();
        const float distanceRangeOpacity = distanceMaximumOpacity - distanceMinimumOpacity;
        
        const FiberTrajectoryDisplayModeEnum::Enum displayMode = ftmp->getDisplayMode();
        float streamlineThreshold = std::numeric_limits<float>::max();
        switch (displayMode) {
            case FiberTrajectoryDisplayModeEnum::FIBER_TRAJECTORY_DISPLAY_ABSOLUTE:
                streamlineThreshold = ftmp->getCountStreamline();
                if (countRangeOpacity <= 0.0) {
                    continue;
                }
                break;
            case FiberTrajectoryDisplayModeEnum::FIBER_TRAJECTORY_DISPLAY_DISTANCE_WEIGHTED:
            case FiberTrajectoryDisplayModeEnum::FIBER_TRAJECTORY_DISPLAY_DISTANCE_WEIGHTED_LOG:
                streamlineThreshold = ftmp->getDistanceStreamline();
                if (distanceRangeOpacity <= 0.0) {
                    continue;
                }
                break;
            case FiberTrajectoryDisplayModeEnum::FIBER_TRAJECTORY_DISPLAY_PROPORTION:
                streamlineThreshold = ftmp->getProportionStreamline();
                if (proportionRangeOpacity <= 0.0) {
                    continue;
                }
                break;
        }
        
        DisplayPropertiesFiberOrientation* dpfo = m_brain->getDisplayPropertiesFiberOrientation();
        const DisplayGroupEnum::Enum displayGroup = dpfo->getDisplayGroupForTab(this->windowTabIndex);
        const FiberOrientationSymbolTypeEnum::Enum symbolType = dpfo->getSymbolType(displayGroup,
                                                                                    this->windowTabIndex);
        FiberOrientationDisplayInfo fiberOrientDispInfo;
        setFiberOrientationDisplayInfo(dpfo,
                                       displayGroup,
                                       this->windowTabIndex,
                                       browserTabContent->getClippingPlaneGroup(),
                                       const_cast<Plane*>(plane),
                                       structure,
                                       ftmp->getFiberTrajectoryColorModel()->getSelectedItem(),
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
        
        
        
        const std::vector<FiberOrientationTrajectory*>& trajectories = trajFile->getLoadedFiberOrientationTrajectories();
        const int64_t numTraj = static_cast<int64_t>(trajectories.size());
        for (int64_t iTraj = 0; iTraj < numTraj; iTraj++) {
            const FiberOrientationTrajectory* fiberTraj = trajectories[iTraj];
            const FiberOrientation* orientation = fiberTraj->getFiberOrientation();
            
            const float fiberFractionTotalCount = fiberTraj->getFiberFractionTotalCount();
            
            const std::vector<float>& fiberFractions = fiberTraj->getFiberFractions();
            if (fiberFractions.size() != 3) {
                CaretLogFinest("Fiber Trajectory index="
                                + AString::number(iTraj)
                                + " has "
                                + AString::number(fiberFractions.size())
                                + " fibers != 3 from file "
                                + trajFile->getFileNameNoPath());
                
                continue;
            }
            else if (fiberFractionTotalCount < streamlineThreshold) {
                continue;
            }
            float fiberOpacities[3] = { 0.0, 0.0, 0.0 };
            const float fiberCounts[3] = {
                fiberFractions[0] * fiberFractionTotalCount,
                fiberFractions[1] * fiberFractionTotalCount,
                fiberFractions[2] * fiberFractionTotalCount
            };
            
            const float fiberFractionDistance = fiberTraj->getFiberFractionDistance();
            
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
                    fiberOpacities[0] = ((fiberCounts[0] * fiberFractionDistance)
                                         - distanceMinimumOpacity) / distanceRangeOpacity;
                    fiberOpacities[1] = ((fiberCounts[1] * fiberFractionDistance)
                                         - distanceMinimumOpacity) / distanceRangeOpacity;
                    fiberOpacities[2] = ((fiberCounts[2] * fiberFractionDistance)
                                         - distanceMinimumOpacity) / distanceRangeOpacity;
                    break;
                case FiberTrajectoryDisplayModeEnum::FIBER_TRAJECTORY_DISPLAY_DISTANCE_WEIGHTED_LOG:
                {
                    const float distanceLog = std::log(fiberFractionDistance);
                    fiberOpacities[0] = ((fiberCounts[0] * distanceLog)
                                         - distanceMinimumOpacity) / distanceRangeOpacity;
                    fiberOpacities[1] = ((fiberCounts[1] * distanceLog)
                                         - distanceMinimumOpacity) / distanceRangeOpacity;
                    fiberOpacities[2] = ((fiberCounts[2] * distanceLog)
                                         - distanceMinimumOpacity) / distanceRangeOpacity;
                }
                    break;
                case FiberTrajectoryDisplayModeEnum::FIBER_TRAJECTORY_DISPLAY_PROPORTION:
                    fiberOpacities[0] = (fiberFractions[0]
                                         - proportionMinimumOpacity) /proportionRangeOpacity;
                    fiberOpacities[1] = (fiberFractions[1]
                                         - proportionMinimumOpacity) /proportionRangeOpacity;
                    fiberOpacities[2] = (fiberFractions[2]
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
            if (drawCount > 0) {
                orientation->m_fibers[0]->m_opacityForDrawing = fiberOpacities[0];
                orientation->m_fibers[1]->m_opacityForDrawing = fiberOpacities[1];
                orientation->m_fibers[2]->m_opacityForDrawing = fiberOpacities[2];
                
                addFiberOrientationForDrawing(&fiberOrientDispInfo,
                                              orientation);
            }
        }
        
        drawAllFiberOrientations(&fiberOrientDispInfo,
                                 true);
    }
    
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
 * @param rgba
 *    Color of cone.
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
BrainOpenGLFixedPipeline::drawEllipticalCone(const float rgba[4],
                                             const float baseXYZ[3],
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
    m_shapeCone->draw(rgba);
    glPopMatrix();
    glPopMatrix();
}

/**
 * Draw a cone with an elliptical shape.
 * @param rgba
 *    Color of cone.
 * @param bottomXYZ
 *    Location of the bottom of the cylinder.
 * @param topXYZ
 *    Location of the top of the cylinder.
 * @param radius
 *    Radius of the cylinder.
 */
void
BrainOpenGLFixedPipeline::drawCylinder(const float rgba[4],
                                       const float bottomXYZ[3],
                                       const float topXYZ[3],
                                       const float radius)
{
    float x1 = topXYZ[0];
    float y1 = topXYZ[1];
    float z1 = topXYZ[2];
    float vx = bottomXYZ[0] - x1;
    float vy = bottomXYZ[1] - y1;
    float vz = bottomXYZ[2] - z1;
    
    float z = (float)std::sqrt( vx*vx + vy*vy + vz*vz );
    double ax = 0.0f;
    
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
    
    /*
     * Draw the cone
     */
    glScalef(radius * 2.0,
             radius * 2.0,
             z);
    m_shapeCylinder->draw(rgba);
    glPopMatrix();
    glPopMatrix();    
}


/**
 * Draw fiber orientations on surface models.
 *
 * @param structure
 *    The structure.
 */
void
BrainOpenGLFixedPipeline::drawSurfaceFiberOrientations(const StructureEnum::Enum structure)
{
    drawFiberOrientations(NULL,
                          structure);
}

/**
 * Draw the surface montage model.
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
    const int32_t tabIndex = browserTabContent->getTabNumber();
    
    std::vector<SurfaceMontageViewport*> montageViewports;
    surfaceMontageModel->getSurfaceMontageViewportsForDrawing(tabIndex,
                                                              montageViewports);
    if (montageViewports.empty()) {
        return;
    }
    
    GLint savedVP[4];
    glGetIntegerv(GL_VIEWPORT,
                  savedVP);
    
    
    int32_t numberOfRows = 0;
    int32_t numberOfColumns = 0;
    SurfaceMontageViewport::getNumberOfRowsAndColumns(montageViewports,
                                                      numberOfRows,
                                                      numberOfColumns);
    
    const int32_t vpSizeX = viewport[2] / numberOfColumns;
    const int32_t vpSizeY = viewport[3] / numberOfRows;
    
    const int32_t numberOfViewports = static_cast<int32_t>(montageViewports.size());
    for (int32_t ivp = 0; ivp < numberOfViewports; ivp++) {
        SurfaceMontageViewport* mvp = montageViewports[ivp];
        const float* nodeColoringRGBA = this->surfaceNodeColoring->colorSurfaceNodes(surfaceMontageModel,
                                                                                     mvp->getSurface(),
                                                                                     this->windowTabIndex);
        float center[3];
        mvp->getSurface()->getBoundingBox()->getCenter(center);
        
        const int32_t rowFromTop    = mvp->getRow();
        const int32_t rowFromBottom = (numberOfRows - rowFromTop - 1);
        const int32_t column = mvp->getColumn();
        
        const int32_t surfaceViewport[4] = {
            (viewport[0] + (column * vpSizeX)),
            (viewport[1] + (rowFromBottom * vpSizeY)),
            vpSizeX,
            vpSizeY
        };
        mvp->setViewport(surfaceViewport);
        
        this->setViewportAndOrthographicProjectionForSurfaceFile(surfaceViewport,
                                                                 mvp->getProjectionViewType(),
                                                                 mvp->getSurface());
        
        this->applyViewingTransformations(center,
                                          mvp->getProjectionViewType());
        this->drawSurface(mvp->getSurface(),
                          nodeColoringRGBA);
        
    }
    
    glViewport(savedVP[0], 
               savedVP[1], 
               savedVP[2], 
               savedVP[3]);
}

/**
 * Draw the whole brain.
 * @param browserTabContent
 *    Content of the window.
 * @param wholeBrainModel
 *    Model for whole brain.
 * @param viewport
 *    Region for drawing.
 */
void 
BrainOpenGLFixedPipeline::drawWholeBrainModel(BrowserTabContent* browserTabContent,
                                      ModelWholeBrain* wholeBrainModel,
                                      const int32_t viewport[4])
{
    const int32_t tabNumberIndex = browserTabContent->getTabNumber();
    
    Surface* leftSurface = wholeBrainModel->getSelectedSurface(StructureEnum::CORTEX_LEFT,
                                                                    tabNumberIndex);
    Surface* rightSurface = wholeBrainModel->getSelectedSurface(StructureEnum::CORTEX_RIGHT,
                                                                     tabNumberIndex);
    /*
     * Center using volume, if it is available
     * Otherwise, see if surface is available, but a surface is offset
     * from center so override the X-coordinate to zero.
     */
    float center[3] = { 0.0, 0.0, 0.0 };
    VolumeMappableInterface* underlayVolumeFile = wholeBrainModel->getUnderlayVolumeFile(tabNumberIndex);
    if (underlayVolumeFile != NULL) {
        BoundingBox volumeBoundingBox;
        underlayVolumeFile->getVoxelSpaceBoundingBox(volumeBoundingBox);
        volumeBoundingBox.getCenter(center);
    }
//    else {
        if (leftSurface != NULL) {
            leftSurface->getBoundingBox()->getCenter(center);
            center[0] = 0.0;
        }
        else {
            if (rightSurface != NULL) {
                rightSurface->getBoundingBox()->getCenter(center);
                center[0] = 0.0;
            }
        }
//    }

    /*
     * Use a surface (if available) to set the orthographic projection size
     */
    Surface* anySurface = NULL;
    if (leftSurface != NULL) {
        anySurface = leftSurface;
    }
    else if (rightSurface != NULL) {
        anySurface = rightSurface;
    }
    
    if (anySurface != NULL) {
        this->setViewportAndOrthographicProjectionForSurfaceFile(viewport,
                                                                 ProjectionViewTypeEnum::PROJECTION_VIEW_LEFT_LATERAL,
                                                                 anySurface);
    }
    else if (underlayVolumeFile != NULL) {
        this->setViewportAndOrthographicProjectionForVolume(viewport,
                                                            ProjectionViewTypeEnum::PROJECTION_VIEW_LEFT_LATERAL,
                                                            underlayVolumeFile);
    }
    else {
        this->setViewportAndOrthographicProjection(viewport,
                                                   ProjectionViewTypeEnum::PROJECTION_VIEW_LEFT_LATERAL);
    }
    this->applyViewingTransformations(center,
                                      ProjectionViewTypeEnum::PROJECTION_VIEW_LEFT_LATERAL);
    
    const SurfaceTypeEnum::Enum surfaceType = wholeBrainModel->getSelectedSurfaceType(tabNumberIndex);
    
    Brain* brain = wholeBrainModel->getBrain();

    /*
     * Need depth testing for drawing slices
     */
    glEnable(GL_DEPTH_TEST);

    /*
     * Determine volumes that are to be drawn  
     */
    if (underlayVolumeFile != NULL) {
        std::vector<VolumeDrawInfo> volumeDrawInfo;
        this->setupVolumeDrawInfo(browserTabContent,
                                  brain,
                                  volumeDrawInfo);
        if (volumeDrawInfo.empty() == false) {
            /*
             * Voxels as 3D
             */
            drawVolumeVoxelsAsCubesWholeBrain(volumeDrawInfo);

            /*
             * Filter volumes for drawing and only draw those volumes that
             * are to be drawn as 2D volume slices.
             */
            std::vector<VolumeDrawInfo> twoDimSliceDrawVolumeDrawInfo;
            for (std::vector<VolumeDrawInfo>::iterator iter = volumeDrawInfo.begin();
                 iter != volumeDrawInfo.end();
                 iter++) {
                bool useIt = false;
                VolumeDrawInfo& vdi = *iter;
                switch (vdi.wholeBrainVoxelDrawingMode) {
                    case WholeBrainVoxelDrawingMode::DRAW_VOXELS_AS_THREE_D_CUBES:
                    case WholeBrainVoxelDrawingMode::DRAW_VOXELS_AS_ROUNDED_THREE_D_CUBES:
                        break;
                    case WholeBrainVoxelDrawingMode::DRAW_VOXELS_ON_TWO_D_SLICES:
                        useIt = true;
                        break;
                }
                if (useIt) {
                    twoDimSliceDrawVolumeDrawInfo.push_back(vdi);
                }
            }
            /*
             * Check for oblique slice drawing
             */
            const bool doObliqueDrawing = (browserTabContent->getSliceViewMode() == VolumeSliceViewModeEnum::OBLIQUE);
            if (doObliqueDrawing) {
                
                BrainOpenGLVolumeSliceDrawing volumeSliceDrawing;
                volumeSliceDrawing.draw(this,
                                    browserTabContent,
                                    twoDimSliceDrawVolumeDrawInfo,
                                    VolumeSliceViewModeEnum::OBLIQUE,
                                    viewport);
            }
            else {
                BrainOpenGLVolumeSliceDrawing volumeSliceDrawing;
                volumeSliceDrawing.draw(this,
                                    browserTabContent,
                                    twoDimSliceDrawVolumeDrawInfo,
                                    VolumeSliceViewModeEnum::ORTHOGONAL,
                                    viewport);
            }
        }
    }
    drawSurfaceFiberOrientations(StructureEnum::ALL);
    drawSurfaceFiberTrajectories(StructureEnum::ALL);
    
    /*
     * Draw surfaces last so that opacity works.
     */
    const int32_t numberOfBrainStructures = brain->getNumberOfBrainStructures();
    for (int32_t i = 0; i < numberOfBrainStructures; i++) {
        BrainStructure* brainStructure = brain->getBrainStructure(i);
        const StructureEnum::Enum structure = brainStructure->getStructure();
        Surface* surface = wholeBrainModel->getSelectedSurface(structure,
                                                                    tabNumberIndex);
        if (surface != NULL) {
            float dx = 0.0;
            float dy = 0.0;
            float dz = 0.0;
            
            bool drawIt = false;
            switch (structure) {
                case StructureEnum::CORTEX_LEFT:
                    drawIt = browserTabContent->isWholeBrainLeftEnabled();
                    dx = -browserTabContent->getWholeBrainLeftRightSeparation();
                    if ((surfaceType != SurfaceTypeEnum::ANATOMICAL)
                        && (surfaceType != SurfaceTypeEnum::RECONSTRUCTION)) {
                        dx -= surface->getBoundingBox()->getMaxX();
                    }
                    break;
                case StructureEnum::CORTEX_RIGHT:
                    drawIt = browserTabContent->isWholeBrainRightEnabled();
                    dx = browserTabContent->getWholeBrainLeftRightSeparation();
                    if ((surfaceType != SurfaceTypeEnum::ANATOMICAL)
                        && (surfaceType != SurfaceTypeEnum::RECONSTRUCTION)) {
                        dx -= surface->getBoundingBox()->getMinX();
                    }
                    break;
                case StructureEnum::CEREBELLUM:
                    drawIt = browserTabContent->isWholeBrainCerebellumEnabled();
                    dz = browserTabContent->getWholeBrainCerebellumSeparation();
                    break;
                default:
                    CaretLogWarning("programmer-issure: Surface type not left/right/cerebellum");
                    break;
            }
            
            const float* nodeColoringRGBA = this->surfaceNodeColoring->colorSurfaceNodes(wholeBrainModel,
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
}

/**
 * Draw a chart model.
 *
 * @param browserTabContent
 *    Content of browser tab.
 * @param chartModel
 *    The chart model.
 * @param viewport
 *    The viewport (x, y, width, height)
 */
void
BrainOpenGLFixedPipeline::drawChartData(BrowserTabContent* browserTabContent,
                    ModelChart* chartModel,
                    const int32_t viewport[4])
{
    CaretAssert(browserTabContent);
    CaretAssert(chartModel);
    
    const int32_t tabIndex = browserTabContent->getTabNumber();
    ChartModelCartesian* cartesianChart = NULL;
    ChartableMatrixInterface* matrixChartFile = NULL;
    const ChartDataTypeEnum::Enum chartDataType = chartModel->getSelectedChartDataType(tabIndex);

    SelectionItemDataTypeEnum::Enum selectionItemDataType = SelectionItemDataTypeEnum::INVALID;
    
    switch (chartDataType) {
        case ChartDataTypeEnum::CHART_DATA_TYPE_INVALID:
            break;
        case ChartDataTypeEnum::CHART_DATA_TYPE_DATA_SERIES:
            cartesianChart = chartModel->getSelectedDataSeriesChartModel(tabIndex);
            selectionItemDataType = SelectionItemDataTypeEnum::CHART_DATA_SERIES;
            break;
        case ChartDataTypeEnum::CHART_DATA_TYPE_MATRIX:
        {
            ChartableMatrixFileSelectionModel* matrixFileSelector = chartModel->getChartableMatrixFileSelectionModel(tabIndex);
            matrixChartFile = matrixFileSelector->getSelectedFile();
            selectionItemDataType = SelectionItemDataTypeEnum::CHART_MATRIX;
        }
            break;
        case ChartDataTypeEnum::CHART_DATA_TYPE_TIME_SERIES:
            cartesianChart = chartModel->getSelectedTimeSeriesChartModel(tabIndex);
            selectionItemDataType = SelectionItemDataTypeEnum::CHART_TIME_SERIES;
            break;
    }
    
    if (cartesianChart != NULL) {
        BrainOpenGLChartDrawingFixedPipeline chartDrawing;
        chartDrawing.drawCartesianChart(m_brain,
                                        this,
                                        viewport,
                                        this->textRenderer,
                                        cartesianChart,
                                        selectionItemDataType,
                                        this->windowTabIndex);
    }
    else if (matrixChartFile != NULL) {
        BrainOpenGLChartDrawingFixedPipeline chartDrawing;
        chartDrawing.drawMatrixChart(m_brain,
                                     this,
                                     viewport,
                                     this->textRenderer,
                                     matrixChartFile,
                                     selectionItemDataType,
                                     this->windowTabIndex);
    }
}

/**
 * Setup the orthographic projection.
 * @param viewport
 *    The viewport (x, y, width, height)
 * @param projectionType
 *    Type of view projection.
 */
void
BrainOpenGLFixedPipeline::setOrthographicProjection(const int32_t viewport[4],
                                                    const ProjectionViewTypeEnum::Enum projectionType)
{
    setOrthographicProjectionWithHeight(viewport,
                                        projectionType,
                                        getModelViewingHalfWindowHeight());
    
//    const BoundingBox* boundingBox = m_brain->getSpatialFilesBoundingBox();
//    std::cout << ("Spatial: "
//                  + boundingBox->toString()) << std::endl;
//    float bounds[6];
//    boundingBox->getBounds(bounds);
//    
//    const float modelHalfHeight = std::max(std::max(boundingBox->getDifferenceX(),
//                                                    boundingBox->getDifferenceY()),
//                                           boundingBox->getDifferenceZ()) / 2.0;
//    const float orthoHeight = modelHalfHeight * 1.02;
//
//    
////    double defaultOrthoWindowSize = BrainOpenGLFixedPipeline::getModelViewingHalfWindowHeight();
//    double defaultOrthoWindowSize = orthoHeight;
//    double width = viewport[2];
//    double height = viewport[3];
//    double aspectRatio = (width / height);
//    this->orthographicRight  =    defaultOrthoWindowSize * aspectRatio;
//    this->orthographicLeft   =   -defaultOrthoWindowSize * aspectRatio;
//    this->orthographicTop    =    defaultOrthoWindowSize;
//    this->orthographicBottom =   -defaultOrthoWindowSize;
//    this->orthographicNear   = -1000.0; //-500.0; //-10000.0;
//    this->orthographicFar    =  1000.0; //500.0; // 10000.0;
//    
//    switch (projectionType) {
//        case ProjectionViewTypeEnum::PROJECTION_VIEW_LEFT_LATERAL:
//        case ProjectionViewTypeEnum::PROJECTION_VIEW_LEFT_FLAT_SURFACE:
//        case ProjectionViewTypeEnum::PROJECTION_VIEW_RIGHT_MEDIAL:
//            glOrtho(this->orthographicLeft, this->orthographicRight,
//                    this->orthographicBottom, this->orthographicTop,
//                    this->orthographicNear, this->orthographicFar);
//            break;
//        case ProjectionViewTypeEnum::PROJECTION_VIEW_RIGHT_LATERAL:
//        case ProjectionViewTypeEnum::PROJECTION_VIEW_LEFT_MEDIAL:
//        case ProjectionViewTypeEnum::PROJECTION_VIEW_RIGHT_FLAT_SURFACE:
//            glOrtho(this->orthographicRight, this->orthographicLeft,
//                    this->orthographicBottom, this->orthographicTop,
//                    this->orthographicFar, this->orthographicNear);
//            break;
//    }
}

/**
 * Setup the orthographic projection for the given surface file.
 *
 * @param viewport
 *    The viewport (x, y, width, height)
 * @param projectionType
 *    Type of view projection.
 * @param boundingBox
 *    The bounding box used for maximum spatial extent.
 */
void
BrainOpenGLFixedPipeline::setOrthographicProjectionForWithBoundingBox(const int32_t viewport[4],
                                                 const ProjectionViewTypeEnum::Enum projectionType,
                                                 const BoundingBox* boundingBox)
{
    CaretAssert(boundingBox);
    
    const float modelHalfHeight = std::max(std::max(boundingBox->getDifferenceX(),
                                                    boundingBox->getDifferenceY()),
                                           boundingBox->getDifferenceZ()) / 2.0;
    const float orthoHeight = modelHalfHeight * 1.02;
    
    setOrthographicProjectionWithHeight(viewport,
                                        projectionType,
                                        orthoHeight);
}

///**
// * Setup the orthographic projection for the given surface file.
// *
// * @param viewport
// *    The viewport (x, y, width, height)
// * @param projectionType
// *    Type of view projection.
// */
//void
//BrainOpenGLFixedPipeline::setOrthographicProjectionForSurfaceFile(const int32_t viewport[4],
//                                             const ProjectionViewTypeEnum::Enum projectionType,
//                                             const SurfaceFile* surfaceFile)
//{
//    const BoundingBox* boundingBox = surfaceFile->getBoundingBox();
//    const float modelHalfHeight = std::max(std::max(boundingBox->getDifferenceX(),
//                                                    boundingBox->getDifferenceY()),
//                                           boundingBox->getDifferenceZ()) / 2.0;
//    const float orthoHeight = modelHalfHeight * 1.02;
//    
//    setOrthographicProjectionWithHeight(viewport,
//                                        projectionType,
//                                        orthoHeight);
//}

/**
 * Setup the orthographic projection with the given window height.
 *
 * @param viewport
 *    The viewport (x, y, width, height)
 * @param projectionType
 *    Type of view projection.
 * @param halfWindowHeight
 *    Half of window height for model.
 */
void
BrainOpenGLFixedPipeline::setOrthographicProjectionWithHeight(const int32_t viewport[4],
                                                              const ProjectionViewTypeEnum::Enum projectionType,
                                                              const float halfWindowHeight)
{
    double width = viewport[2];
    double height = viewport[3];
    double aspectRatio = (width / height);
    this->orthographicRight  =    halfWindowHeight * aspectRatio;
    this->orthographicLeft   =   -halfWindowHeight * aspectRatio;
    this->orthographicTop    =    halfWindowHeight;
    this->orthographicBottom =   -halfWindowHeight;
    this->orthographicNear   = -1000.0; //-500.0; //-10000.0;
    this->orthographicFar    =  1000.0; //500.0; // 10000.0;
    
    switch (projectionType) {
        case ProjectionViewTypeEnum::PROJECTION_VIEW_CEREBELLUM_ANTERIOR:
        case ProjectionViewTypeEnum::PROJECTION_VIEW_CEREBELLUM_DORSAL:
        case ProjectionViewTypeEnum::PROJECTION_VIEW_CEREBELLUM_POSTERIOR:
        case ProjectionViewTypeEnum::PROJECTION_VIEW_CEREBELLUM_VENTRAL:
            glOrtho(this->orthographicLeft, this->orthographicRight,
                    this->orthographicBottom, this->orthographicTop,
                    this->orthographicNear, this->orthographicFar);
            break;
//        case ProjectionViewTypeEnum::PROJECTION_VIEW_CEREBELLUM_DORSAL:
//            glOrtho(this->orthographicLeft, this->orthographicRight,
//                    this->orthographicBottom, this->orthographicTop,
//                    this->orthographicNear, this->orthographicFar);
//            break;
//        case ProjectionViewTypeEnum::PROJECTION_VIEW_CEREBELLUM_POSTERIOR:
//            glOrtho(this->orthographicRight, this->orthographicLeft,
//                    this->orthographicBottom, this->orthographicTop,
//                    this->orthographicFar, this->orthographicNear);
//            break;
//        case ProjectionViewTypeEnum::PROJECTION_VIEW_CEREBELLUM_VENTRAL:
//            glOrtho(this->orthographicRight, this->orthographicLeft,
//                    this->orthographicBottom, this->orthographicTop,
//                    this->orthographicFar, this->orthographicNear);
//            break;
        case ProjectionViewTypeEnum::PROJECTION_VIEW_CEREBELLUM_FLAT_SURFACE:
            glOrtho(this->orthographicLeft, this->orthographicRight,
                    this->orthographicBottom, this->orthographicTop,
                    this->orthographicNear, this->orthographicFar);
            break;
        case ProjectionViewTypeEnum::PROJECTION_VIEW_LEFT_LATERAL:
        case ProjectionViewTypeEnum::PROJECTION_VIEW_LEFT_FLAT_SURFACE:
        case ProjectionViewTypeEnum::PROJECTION_VIEW_RIGHT_MEDIAL:
            glOrtho(this->orthographicLeft, this->orthographicRight,
                    this->orthographicBottom, this->orthographicTop,
                    this->orthographicNear, this->orthographicFar);
            break;
        case ProjectionViewTypeEnum::PROJECTION_VIEW_RIGHT_LATERAL:
        case ProjectionViewTypeEnum::PROJECTION_VIEW_LEFT_MEDIAL:
        case ProjectionViewTypeEnum::PROJECTION_VIEW_RIGHT_FLAT_SURFACE:
            glOrtho(this->orthographicRight, this->orthographicLeft,
                    this->orthographicBottom, this->orthographicTop,
                    this->orthographicFar, this->orthographicNear);
            break;
    }
}


/**
 * check for an OpenGL Error.
 */
void
BrainOpenGLFixedPipeline::checkForOpenGLError(const Model* model,
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
        if (model != NULL) {
            msg += ("While drawing brain model " + model->getNameForGUI(true) + "\n");
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
 *
 * @param rgba
 *    Color for drawing.
 * @param diameter
 *    Diameter of the sphere.
 */
void 
BrainOpenGLFixedPipeline::drawSphereWithDiameter(const float rgba[4],
                                                 const double diameter)
{
    glPushMatrix();
    glScaled(diameter, diameter, diameter);
    m_shapeSphere->draw(rgba);
    glPopMatrix();
}

/**
 * Draw sphere.
 *
 * @param rgba
 *    Color for drawing.
 * @param diameter
 *    Diameter of the sphere.
 */
void
BrainOpenGLFixedPipeline::drawSphereWithDiameter(const uint8_t rgba[4],
                                                 const double diameter)
{
    glPushMatrix();
    glScaled(diameter, diameter, diameter);
    m_shapeSphere->draw(rgba);
    glPopMatrix();
}

/**
 * Draw cube.
 *
 * @param rgba
 *    Color for drawing.
 * @param cubeSize
 *    Size of the cube (distance from one face to its opposite face).
 */
void
BrainOpenGLFixedPipeline::drawCube(const float rgba[4],
                                   const double cubeSize)
{
    glPushMatrix();
    glScaled(cubeSize, cubeSize, cubeSize);
    m_shapeCube->draw(rgba);
    glPopMatrix();
}

/**
 * Draw a cuboid (3D Box)
 *
 * @param rgba
 *    Color for drawing.
 * @param sizeX
 *    X-Size of the cube (distance from -X face to its +X face).
 * @param sizeY
 *    Y-Size of the cube (distance from -Y face to its +Y face).
 * @param sizeZ
 *    Z-Size of the cube (distance from -Z face to its +X face).
 */
void
BrainOpenGLFixedPipeline::drawCuboid(const uint8_t rgba[4],
                                     const double sizeX,
                                     const double sizeY,
                                     const double sizeZ)
{
    glPushMatrix();
    glScaled(sizeX, sizeY, sizeZ);
    m_shapeCube->draw(rgba);
    glPopMatrix();
}

/**
 * Draw cube.
 *
 * @param rgba
 *    Color for drawing.
 * @param cubeSize
 *    Size of the cube (distance from one face to its opposite face).
 */
void
BrainOpenGLFixedPipeline::drawRoundedCube(const float rgba[4],
                                          const double cubeSize)
{
    glPushMatrix();
    glScaled(cubeSize, cubeSize, cubeSize);
    m_shapeCubeRounded->draw(rgba);
    glPopMatrix();
}

/**
 * Draw a cuboid (3D Box)
 *
 * @param rgba
 *    Color for drawing.
 * @param sizeX
 *    X-Size of the cube (distance from -X face to its +X face).
 * @param sizeY
 *    Y-Size of the cube (distance from -Y face to its +Y face).
 * @param sizeZ
 *    Z-Size of the cube (distance from -Z face to its +X face).
 */
void
BrainOpenGLFixedPipeline::drawRoundedCuboid(const uint8_t rgba[4],
                                            const double sizeX,
                                     const double sizeY,
                                     const double sizeZ)
{
    glPushMatrix();
    glScaled(sizeX, sizeY, sizeZ);
    m_shapeCubeRounded->draw(rgba);
    glPopMatrix();
}

/**
 * Draw a one millimeter square facing the user.
 * NOTE: This method will alter the current
 * modelviewing matrices so caller may need
 * to enclose the call to this method within
 * glPushMatrix() and glPopMatrix().
 *
 * @param rgba
 *     RGBA coloring ranging 0.0 to 1.0.
 * @param size
 *     Size of square.
 */
void 
BrainOpenGLFixedPipeline::drawSquare(const float rgba[4],
                                     const float size)
{
    if (this->inverseRotationMatrixValid) {
        glColor4fv(rgba);
        
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
 * Draw a one millimeter square facing the user.
 * NOTE: This method will alter the current
 * modelviewing matrices so caller may need
 * to enclose the call to this method within
 * glPushMatrix() and glPopMatrix().
 *
 * @param rgba
 *     RGBA coloring ranging 0 to 255.
 * @param size
 *     Size of square.
 */
void
BrainOpenGLFixedPipeline::drawSquare(const uint8_t rgba[4],
                                     const float size)
{
    if (this->inverseRotationMatrixValid) {
        glColor4ubv(rgba);
        
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
 *
 * @param windowX
 *    Window X-coordinate.
 * @param windowY
 *    Window Y-coordinate.
 * @param text
 *    Text that is to be drawn.
 * @param alignmentX
 *    X-alignment of text.
 * @param alignmentY
 *    Y-alignment of text.
 * @param textStyle
 *    Style of text.
 * @param fontHeight
 *    Height of font.  If negative, default is used.
 */
void 
BrainOpenGLFixedPipeline::drawTextWindowCoords(const int windowX,
                                               const int windowY,
                                               const QString& text,
                                               const BrainOpenGLTextRenderInterface::TextAlignmentX alignmentX,
                                               const BrainOpenGLTextRenderInterface::TextAlignmentY alignmentY,
                                               const BrainOpenGLTextRenderInterface::TextStyle textStyle,
                                               const int fontHeight)
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
                                                   alignmentY,
                                                   textStyle,
                                                   fontHeight);
    }
}

/**
 * Draw text at the given window coordinates and occlude anything under
 * the text by drawing the text region with a background.
 *
 * @param windowX
 *    Window X-coordinate.
 * @param windowY
 *    Window Y-coordinate.
 * @param text
 *    Text that is to be drawn.
 * @param alignmentX
 *    X-alignment of text.
 * @param alignmentY
 *    Y-alignment of text.
 * @param textStyle
 *    Style of text.
 * @param fontHeight
 *    Height of font.  If negative, default is used.
 */
void
BrainOpenGLFixedPipeline::drawTextWindowCoordsWithBackground(const int windowX,
                                                             const int windowY,
                                                             const QString& text,
                                                             const BrainOpenGLTextRenderInterface::TextAlignmentX alignmentX,
                                                             const BrainOpenGLTextRenderInterface::TextAlignmentY alignmentY,
                                                             const BrainOpenGLTextRenderInterface::TextStyle textStyle,
                                                             const int fontHeight)
{
    if (this->textRenderer != NULL) {
        
        int32_t textWidth  = 0;
        int32_t textHeight = 0;
        this->textRenderer->getTextBoundsInPixels(text,
                                                  textStyle,
                                                  fontHeight,
                                                  "",
                                                  textWidth,
                                                  textHeight);
        //std::cout << "Text bounds: " << qPrintable(text) << ": " << textWidth << ", " << textHeight << std::endl;
        
        const int textCenter[2] = {
            windowX,
            windowY
        };
//        const int halfFontSize = fontHeight / 2;
        
        CaretPreferences* prefs = SessionManager::get()->getCaretPreferences();
        uint8_t backgroundRGBA[4];
        prefs->getColorBackground(backgroundRGBA);
        backgroundRGBA[3] = 255;
        
        GLint savedViewport[4];
        glGetIntegerv(GL_VIEWPORT, savedViewport);
        
        const int halfWidth = (textWidth + 3) / 2.0;
        const int halfHeight = (textHeight + 3) / 2.0;
        
        int vpLeftX   = savedViewport[0] + textCenter[0] - halfWidth;
        int vpRightX  = savedViewport[0] + textCenter[0] + halfWidth;
        int vpBottomY = savedViewport[1] + textCenter[1] - halfHeight;
        int vpTopY    = savedViewport[1] + textCenter[1] + halfHeight;
//        int vpLeftX   = savedViewport[0] + textCenter[0] - halfFontSize;
//        int vpRightX  = savedViewport[0] + textCenter[0] + halfFontSize;
//        int vpBottomY = savedViewport[1] + textCenter[1] - halfFontSize;
//        int vpTopY    = savedViewport[1] + textCenter[1] + halfFontSize;
        MathFunctions::limitRange(vpLeftX,
                                  savedViewport[0],
                                  savedViewport[0] + savedViewport[2]);
        MathFunctions::limitRange(vpRightX,
                                  savedViewport[0],
                                  savedViewport[0] + savedViewport[2]);
        MathFunctions::limitRange(vpBottomY,
                                  savedViewport[1],
                                  savedViewport[1] + savedViewport[3]);
        MathFunctions::limitRange(vpTopY,
                                  savedViewport[1],
                                  savedViewport[1] + savedViewport[3]);
        
        const int vpSizeX = vpRightX - vpLeftX;
        const int vpSizeY = vpTopY - vpBottomY;
        glViewport(vpLeftX, vpBottomY, vpSizeX, vpSizeY);
        
        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glLoadIdentity();
        glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);
        
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadIdentity();
        
        std::vector<uint8_t> rgba;
        std::vector<float> coords, normals;
        
        coords.push_back(-1.0);
        coords.push_back(-1.0);
        coords.push_back( 0.0);
        normals.push_back(0.0);
        normals.push_back(0.0);
        normals.push_back(1.0);
        rgba.push_back(backgroundRGBA[0]);
        rgba.push_back(backgroundRGBA[1]);
        rgba.push_back(backgroundRGBA[2]);
        rgba.push_back(backgroundRGBA[3]);
        
        coords.push_back( 1.0);
        coords.push_back(-1.0);
        coords.push_back( 0.0);
        normals.push_back(0.0);
        normals.push_back(0.0);
        normals.push_back(1.0);
        rgba.push_back(backgroundRGBA[0]);
        rgba.push_back(backgroundRGBA[1]);
        rgba.push_back(backgroundRGBA[2]);
        rgba.push_back(backgroundRGBA[3]);
        
        coords.push_back( 1.0);
        coords.push_back( 1.0);
        coords.push_back( 0.0);
        normals.push_back(0.0);
        normals.push_back(0.0);
        normals.push_back(1.0);
        rgba.push_back(backgroundRGBA[0]);
        rgba.push_back(backgroundRGBA[1]);
        rgba.push_back(backgroundRGBA[2]);
        rgba.push_back(backgroundRGBA[3]);
        
        coords.push_back(-1.0);
        coords.push_back( 1.0);
        coords.push_back( 0.0);
        normals.push_back(0.0);
        normals.push_back(0.0);
        normals.push_back(1.0);
        rgba.push_back(backgroundRGBA[0]);
        rgba.push_back(backgroundRGBA[1]);
        rgba.push_back(backgroundRGBA[2]);
        rgba.push_back(backgroundRGBA[3]);
        
        
        BrainOpenGLPrimitiveDrawing::drawQuads(coords,
                                               normals,
                                               rgba);
        
        glPopMatrix();
        
        glMatrixMode(GL_PROJECTION);
        glPopMatrix();
        glMatrixMode(GL_MODELVIEW);
        
        glViewport(savedViewport[0],
                   savedViewport[1],
                   savedViewport[2],
                   savedViewport[3]);
        drawTextWindowCoords(windowX,
                             windowY,
                             text,
                             alignmentX,
                             alignmentY,
                             textStyle,
                             fontHeight);
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
 * Draw text at the given window coordinates.
 * @param modelXYZ
 *    Model XYZ coordinate.
 * @param text
 *    Text that is to be drawn.
 */
void
BrainOpenGLFixedPipeline::drawTextModelCoords(const double modelXYZ[3],
                                              const QString& text)
{
    drawTextModelCoords(modelXYZ[0],
                        modelXYZ[1],
                        modelXYZ[2],
                        text);
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
        if (pcm != NULL) {
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
                                   BrainOpenGLTextRenderInterface::Y_BOTTOM,
                                   BrainOpenGLTextRenderInterface::NORMAL,
                                   12);
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
                                   BrainOpenGLTextRenderInterface::Y_BOTTOM,
                                   BrainOpenGLTextRenderInterface::NORMAL,
                                   12);
    }
    if (isPositiveDisplayed) {
        this->drawTextWindowCoords(textRightX,
                                   textY, 
                                   textRight,
                                   BrainOpenGLTextRenderInterface::X_RIGHT,
                                   BrainOpenGLTextRenderInterface::Y_BOTTOM,
                                   BrainOpenGLTextRenderInterface::NORMAL,
                                   12);
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

/**
 * Convert pixels into model size.
 * 
 * @param pixelSize
 *    The pixel size
 * @return
 *    Model size coordinate space value
 */
float
BrainOpenGLFixedPipeline::pixelSizeToModelSize(const float pixelSize)
{
    float modelSize = 1.0;
    
    const float value = modelSizeToPixelSize(pixelSize);
    if (value > 0) {
        modelSize /= value;
    }
    
    return modelSize;
}

/**
 * @return A string containing the state of OpenGL (depth testing, lighting, etc.)
 */
AString
BrainOpenGLFixedPipeline::getStateOfOpenGL() const
{
    AString s = BrainOpenGL::getStateOfOpenGL();
    
    s.appendWithNewLine("Fixed Pipeline State:");
    s.appendWithNewLine("   " + getOpenGLEnabledEnumAsText("GL_BLEND", GL_BLEND));
    s.appendWithNewLine("   " + getOpenGLEnabledEnumAsText("GL_CLIP_PLANE0", GL_CLIP_PLANE0));
    s.appendWithNewLine("   " + getOpenGLEnabledEnumAsText("GL_CLIP_PLANE1", GL_CLIP_PLANE1));
    s.appendWithNewLine("   " + getOpenGLEnabledEnumAsText("GL_CLIP_PLANE2", GL_CLIP_PLANE2));
    s.appendWithNewLine("   " + getOpenGLEnabledEnumAsText("GL_CLIP_PLANE3", GL_CLIP_PLANE3));
    s.appendWithNewLine("   " + getOpenGLEnabledEnumAsText("GL_CLIP_PLANE4", GL_CLIP_PLANE4));
    s.appendWithNewLine("   " + getOpenGLEnabledEnumAsText("GL_CLIP_PLANE5", GL_CLIP_PLANE5));
    s.appendWithNewLine("   " + getOpenGLEnabledEnumAsText("GL_COLOR_MATERIAL", GL_COLOR_MATERIAL));
    s.appendWithNewLine("   " + getOpenGLEnabledEnumAsText("GL_CULL_FACE", GL_CULL_FACE));
    s.appendWithNewLine("   " + getOpenGLEnabledEnumAsText("GL_DEPTH_TEST", GL_DEPTH_TEST));
    s.appendWithNewLine("   " + getOpenGLBooleanAsText("GL_LIGHT_MODEL_LOCAL_VIEWER", GL_LIGHT_MODEL_LOCAL_VIEWER));
    s.appendWithNewLine("   " + getOpenGLBooleanAsText("GL_LIGHT_MODEL_TWO_SIDE", GL_LIGHT_MODEL_TWO_SIDE));
    s.appendWithNewLine("   " + getOpenGLEnabledEnumAsText("GL_LIGHTING", GL_LIGHTING));
    s.appendWithNewLine("   " + getOpenGLEnabledEnumAsText("GL_LIGHT0", GL_LIGHT0));
    s.appendWithNewLine("   " + getOpenGLEnabledEnumAsText("GL_LIGHT1", GL_LIGHT1));
    s.appendWithNewLine("   " + getOpenGLEnabledEnumAsText("GL_LIGHT2", GL_LIGHT2));
    s.appendWithNewLine("   " + getOpenGLEnabledEnumAsText("GL_LIGHT3", GL_LIGHT3));
    s.appendWithNewLine("   " + getOpenGLEnabledEnumAsText("GL_LIGHT4", GL_LIGHT4));
    s.appendWithNewLine("   " + getOpenGLEnabledEnumAsText("GL_LIGHT5", GL_LIGHT5));
    s.appendWithNewLine("   " + getOpenGLLightAsText("GL_LIGHT0, GL_DIFFUSE", GL_LIGHT0, GL_DIFFUSE, 4));
    s.appendWithNewLine("   " + getOpenGLLightAsText("GL_LIGHT0, GL_POSITION", GL_LIGHT0, GL_POSITION, 4));
    s.appendWithNewLine("   " + getOpenGLLightAsText("GL_LIGHT1, GL_DIFFUSE", GL_LIGHT1, GL_DIFFUSE, 4));
    s.appendWithNewLine("   " + getOpenGLLightAsText("GL_LIGHT1, GL_POSITION", GL_LIGHT1, GL_POSITION, 4));
    s.appendWithNewLine("   " + getOpenGLFloatAsText("GL_LIGHT_MODEL_AMBIENT", GL_LIGHT_MODEL_AMBIENT, 4));
    s.appendWithNewLine("   " + getOpenGLEnabledEnumAsText("GL_LINE_SMOOTH", GL_LINE_SMOOTH));
    s.appendWithNewLine("   " + getOpenGLEnabledEnumAsText("GL_NORMALIZE", GL_NORMALIZE));
    s.appendWithNewLine("   " + getOpenGLEnabledEnumAsText("GL_POLYGON_OFFSET_FILL", GL_POLYGON_OFFSET_FILL));
    
    GLint frontFace;
    glGetIntegerv(GL_FRONT_FACE, &frontFace);
    AString frontFaceValue;
    switch (frontFace) {
        case GL_CW:
            frontFaceValue = "GL_CW";
            break;
        case GL_CCW:
            frontFaceValue = "GL_CCW";
            break;
        default:
            frontFaceValue = "INVALID";
            break;
    }
    s.appendWithNewLine("   Front Face " + frontFaceValue);
    
    GLint cullFace;
    glGetIntegerv(GL_FRONT_FACE, &cullFace);
    AString cullFaceValue;
    switch (cullFace) {
        case GL_FRONT:
            cullFaceValue = "GL_FRONT";
            break;
        case GL_BACK:
            cullFaceValue = "GL_BACK";
            break;
        case GL_FRONT_AND_BACK:
            cullFaceValue = "GL_FRONT_AND_BACK";
            break;
        default:
            cullFaceValue = "INVALID";
            break;
    }
    s.appendWithNewLine("   Cull Face " + cullFaceValue);
    
    return s;
}



//============================================================================
/**
 * Constructor.
 */
BrainOpenGLFixedPipeline::VolumeDrawInfo::VolumeDrawInfo(CaretMappableDataFile* mapFile,
                                                         VolumeMappableInterface* volumeFile,
                                                         Brain* brain,
                                                         PaletteColorMapping* paletteColorMapping,
                                                         const FastStatistics* statistics,
                                                         const WholeBrainVoxelDrawingMode::Enum wholeBrainVoxelDrawingMode,
                                                         const int32_t mapIndex,
                                                         const float opacity) 
: statistics(statistics) {
    this->mapFile = mapFile;
    this->volumeFile = volumeFile;
    this->brain = brain;
//    this->volumeType = volumeFile->getVolumeType();
    this->paletteColorMapping = paletteColorMapping;
    this->wholeBrainVoxelDrawingMode = wholeBrainVoxelDrawingMode;
    this->mapIndex = mapIndex;
    this->opacity    = opacity;
}
