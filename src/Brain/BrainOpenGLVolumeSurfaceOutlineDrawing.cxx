
/*LICENSE_START*/
/*
 *  Copyright (C) 2022 Washington University School of Medicine
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

#define __BRAIN_OPEN_G_L_VOLUME_SURFACE_OUTLINE_DRAWING_DECLARE__
#include "BrainOpenGLVolumeSurfaceOutlineDrawing.h"
#undef __BRAIN_OPEN_G_L_VOLUME_SURFACE_OUTLINE_DRAWING_DECLARE__

#include <cmath>

#include "CaretAssert.h"
#include "CaretLogger.h"
#include "CaretOMP.h"
#include "CaretPreferences.h"
#include "BrainOpenGLFixedPipeline.h"
#include "BrainOpenGLVolumeSurfaceClippedOutlineDrawing.h"
#include "ElapsedTimer.h"
#include "GraphicsEngineDataOpenGL.h"
#include "GraphicsPrimitive.h"
#include "GraphicsUtilitiesOpenGL.h"
#include "HistologySlice.h"
#include "HistologySlicesFile.h"
#include "MathFunctions.h"
#include "Plane.h"
#include "SessionManager.h"
#include "Surface.h"
#include "SurfaceNodeColoring.h"
#include "SurfacePlaneIntersectionToContour.h"
#include "VolumeMappableInterface.h"
#include "VolumeSurfaceOutlineModel.h"
#include "VolumeSurfaceOutlineModelCacheKey.h"
#include "VolumeSurfaceOutlineSetModel.h"

using namespace caret;


    
/**
 * \class caret::BrainOpenGLVolumeSurfaceOutlineDrawing 
 * \brief Draws volume surface outlines
 * \ingroup Brain
 */

/**
 * Constructor.
 */
BrainOpenGLVolumeSurfaceOutlineDrawing::BrainOpenGLVolumeSurfaceOutlineDrawing()
: CaretObject()
{
    
}

/**
 * Destructor.
 */
BrainOpenGLVolumeSurfaceOutlineDrawing::~BrainOpenGLVolumeSurfaceOutlineDrawing()
{
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
BrainOpenGLVolumeSurfaceOutlineDrawing::toString() const
{
    return "BrainOpenGLVolumeSurfaceOutlineDrawing";
}

/**
 * Draw surface outlines on the volume slices
 *
 * @param histologySlicesFile
 *    File containing the histology slice
 * @param histologySlice
 *    The histology slice
 * @param sliceXYZ
 *    Coordinates of slices
 * @param outlineSet
 *    The surface outline set.
 * @param fixedPipelineDrawing
 *    The fixed pipeline drawing.
 * @param useNegativePolygonOffsetFlag
 *    If true, use a negative offset for polygon offset
 */
void
BrainOpenGLVolumeSurfaceOutlineDrawing::drawSurfaceOutline(const HistologySlicesFile* histologySlicesFile,
                                                           const HistologySlice* histologySlice,
                                                           const Vector3D& sliceXYZ,
                                                           VolumeSurfaceOutlineSetModel* outlineSet,
                                                           BrainOpenGLFixedPipeline* fixedPipelineDrawing,
                                                           const bool useNegativePolygonOffsetFlag)
{
    const VolumeMappableInterface* underlayVolume(NULL);
    const Plane stereotaxicPlane(histologySlice->getStereotaxicPlane());
    VolumeSurfaceOutlineModelCacheKey outlineCacheKey(histologySlice,
                                                      histologySlice->getSliceIndex());
    /*
     * Process each surface outline
     * As of 24 May, "zero" is on top so draw in reverse order
     */
    const int32_t numberOfOutlines = outlineSet->getNumberOfDislayedVolumeSurfaceOutlines();
    for (int32_t io = (numberOfOutlines - 1);
         io >= 0;
         io--) {
        VolumeSurfaceOutlineModel* outlineModel = outlineSet->getVolumeSurfaceOutlineModel(io);
        if (outlineModel->isDisplayed()) {
            if (outlineModel->isDrawSurfaceModeSelected()) {
                const bool supportedFlag(false);
                if (supportedFlag) {
                    /*
                     * Note: Histology slice is used to transform XYZ to plane
                     * BUT DOES NOT WORK !!!!
                     */
                    const Plane plane(histologySlice->getStereotaxicPlane());
                    BrainOpenGLVolumeSurfaceClippedOutlineDrawing clippedDrawing(histologySlice,
                                                                                 plane,
                                                                                 sliceXYZ,
                                                                                 outlineModel,
                                                                                 fixedPipelineDrawing);
                    clippedDrawing.drawSurfaceOutline();
                }
                else {
                    const AString msg("Surface clipped outlines is not supported on histology slices.  "
                                      "Slices are drawn in plane coordinates and a transform is needed "
                                      "to transform the surface's stereotaxic coordinates to the histology's "
                                      "plane coordinates.");
                    CaretLogSevere(msg);
                }
            }
            
            if (outlineModel->isDrawLinesModeSelected()) {
                drawSurfaceOutlineCached(histologySlicesFile,
                                         histologySlice,
                                         underlayVolume,
                                         ModelTypeEnum::MODEL_TYPE_HISTOLOGY,
                                         stereotaxicPlane,
                                         outlineCacheKey,
                                         outlineModel,
                                         fixedPipelineDrawing,
                                         useNegativePolygonOffsetFlag);
            }
        }
    }
}

/**
 * Draw surface outlines on the volume slices
 *
 * @param underlayVolume
 *    The underlay volume
 * @param modelType
 *    Type of model being drawn.
 * @param sliceProjectionType
 *    Projection type (oblique/orthogonal)
 * @param sliceViewPlane
 *    Slice view plane (axial, coronal, parasagittal)
 * @param sliceXYZ
 *    Coordinates of slices
 * @param plane
 *    Plane of the volume slice on which surface outlines are drawn.
 * @param outlineSet
 *    The surface outline set.
 * @param fixedPipelineDrawing
 *    The fixed pipeline drawing.
 * @param useNegativePolygonOffsetFlag
 *    If true, use a negative offset for polygon offset
 */
void
BrainOpenGLVolumeSurfaceOutlineDrawing::drawSurfaceOutline(const VolumeMappableInterface* underlayVolume,
                                                           const ModelTypeEnum::Enum modelType,
                                                           const VolumeSliceProjectionTypeEnum::Enum sliceProjectionType,
                                                           const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
                                                           const float sliceXYZ[3],
                                                           const Plane& plane,
                                                           const Matrix4x4& displayTransformMatrix,
                                                           const bool displayTransformMatrixValidFlag,
                                                           VolumeSurfaceOutlineSetModel* outlineSet,
                                                           BrainOpenGLFixedPipeline* fixedPipelineDrawing,
                                                           const bool useNegativePolygonOffsetFlag)
{
    bool drawCachedFlag(true);
    
    switch (sliceProjectionType) {
        case VolumeSliceProjectionTypeEnum::VOLUME_SLICE_PROJECTION_ORTHOGONAL:
            break;
        case VolumeSliceProjectionTypeEnum::VOLUME_SLICE_PROJECTION_OBLIQUE:
            break;
        case VolumeSliceProjectionTypeEnum::VOLUME_SLICE_PROJECTION_MPR:
            drawCachedFlag = false;
            break;
        case VolumeSliceProjectionTypeEnum::VOLUME_SLICE_PROJECTION_MPR_THREE:
            break;
    }

    /*
     * Process each surface outline
     * As of 24 May, "zero" is on top so draw in reverse order
     */
    const int32_t numberOfOutlines = outlineSet->getNumberOfDislayedVolumeSurfaceOutlines();
    for (int32_t io = (numberOfOutlines - 1);
         io >= 0;
         io--) {
        VolumeSurfaceOutlineModel* outlineModel = outlineSet->getVolumeSurfaceOutlineModel(io);
        if (outlineModel->isDisplayed()) {
            if (outlineModel->isDrawSurfaceModeSelected()) {
                const bool testFlag(true);
                if (testFlag) {
                    const HistologySlice* histologySlice(NULL);
                    BrainOpenGLVolumeSurfaceClippedOutlineDrawing clippedDrawing(histologySlice,
                                                                                 plane,
                                                                                 sliceXYZ,
                                                                                 outlineModel,
                                                                                 fixedPipelineDrawing);
                    clippedDrawing.drawSurfaceOutline();
                }
            }
        
            if (outlineModel->isDrawLinesModeSelected()) {
                /*
                 * Code still here to allow comparison with
                 * previous algorithm
                 */
                if (drawCachedFlag) {
                    drawSurfaceOutlineCachedOnVolume(underlayVolume,
                                                     modelType,
                                                     sliceProjectionType,
                                                     sliceViewPlane,
                                                     sliceXYZ,
                                                     plane,
                                                     outlineModel,
                                                     fixedPipelineDrawing,
                                                     useNegativePolygonOffsetFlag);
                }
                else {
                    drawSurfaceOutlineNotCached(underlayVolume,
                                                modelType,
                                                plane,
                                                displayTransformMatrix,
                                                displayTransformMatrixValidFlag,
                                                outlineModel,
                                                fixedPipelineDrawing,
                                                useNegativePolygonOffsetFlag);
                }
            }
        }
    }
}

/**
 * Draw surface outlines on the volume slices
 *
 * @param underlayVolume
 *    The underlay volume
 * @param modelType
 *    Type of model being drawn.
 * @param sliceProjectionType
 Type of slice projection
 * @param sliceProjectionType
 *    Type of slice projection
 * @param sliceViewPlane
 *    Slice view plane (axial, coronal, parasagittal)
 * @param sliceXYZ
 *    Coordinates of slices
 * @param plane
 *    Plane of the volume slice on which surface outlines are drawn.
 * @param outlineModel
 *    The surface outline model.
 * @param fixedPipelineDrawing
 *    The fixed pipeline drawing.
 * @param useNegativePolygonOffsetFlag
 *    If true, use a negative offset for polygon offset
 */
void
BrainOpenGLVolumeSurfaceOutlineDrawing::drawSurfaceOutlineCachedOnVolume(const VolumeMappableInterface* underlayVolume,
                                                        const ModelTypeEnum::Enum modelType,
                                                        const VolumeSliceProjectionTypeEnum::Enum sliceProjectionType,
                                                        const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
                                                        const float sliceXYZ[3],
                                                        const Plane& plane,
                                                        VolumeSurfaceOutlineModel* outlineModel,
                                                        BrainOpenGLFixedPipeline* fixedPipelineDrawing,
                                                        const bool useNegativePolygonOffsetFlag)
{
    
    float sliceCoordinate(0.0);
    switch (sliceViewPlane) {
        case VolumeSliceViewPlaneEnum::ALL:
            break;
        case VolumeSliceViewPlaneEnum::AXIAL:
            sliceCoordinate = sliceXYZ[2];
            break;
        case VolumeSliceViewPlaneEnum::CORONAL:
            sliceCoordinate = sliceXYZ[1];
            break;
        case VolumeSliceViewPlaneEnum::PARASAGITTAL:
            sliceCoordinate = sliceXYZ[0];
            break;
    }
    
    /*
     * Key for outline cache
     */
    VolumeSurfaceOutlineModelCacheKey outlineCacheKey(underlayVolume,
                                                      sliceViewPlane,
                                                      sliceCoordinate);
    switch (sliceProjectionType) {
        case VolumeSliceProjectionTypeEnum::VOLUME_SLICE_PROJECTION_OBLIQUE:
            outlineCacheKey = VolumeSurfaceOutlineModelCacheKey(underlayVolume,
                                                                plane);
            break;
        case VolumeSliceProjectionTypeEnum::VOLUME_SLICE_PROJECTION_ORTHOGONAL:
            break;
        case VolumeSliceProjectionTypeEnum::VOLUME_SLICE_PROJECTION_MPR:
            break;
        case VolumeSliceProjectionTypeEnum::VOLUME_SLICE_PROJECTION_MPR_THREE:
            outlineCacheKey = VolumeSurfaceOutlineModelCacheKey(underlayVolume,
                                                                plane);
            break;
    }
    
    const HistologySlicesFile* histologySlicesFile(NULL);
    const HistologySlice* histologySlice(NULL);
    drawSurfaceOutlineCached(histologySlicesFile,
                             histologySlice,
                             underlayVolume,
                             modelType,
                             plane,
                             outlineCacheKey,
                             outlineModel,
                             fixedPipelineDrawing,
                             useNegativePolygonOffsetFlag);
}

/**
 * Draw surface outlines on the volume slices
 *
 * @param histologySlicesFile
 *    File containing the histology slice
 * @param underlayVolume
 *    The underlay volume
 * @param modelType
 *    Type of model on which outlines are drawn
 * @param plane
 *    Plane of the volume slice on which surface outlines are drawn.
 * @param outlineCacheKey
 *    Key for outline cache
 * @param outlineModel
 *    The surface outline model.
 * @param fixedPipelineDrawing
 *    The fixed pipeline drawing.
 * @param useNegativePolygonOffsetFlag
 *    If true, use a negative offset for polygon offset
 */
void
BrainOpenGLVolumeSurfaceOutlineDrawing::drawSurfaceOutlineCached(const HistologySlicesFile* histologySlicesFile,
                                                                 const HistologySlice* histologySlice,
                                                                 const VolumeMappableInterface* underlayVolume,
                                                                 const ModelTypeEnum::Enum modelType,
                                                                 const Plane& plane,
                                                                 VolumeSurfaceOutlineModelCacheKey& outlineCacheKey,
                                                                 VolumeSurfaceOutlineModel* outlineModel,
                                                                 BrainOpenGLFixedPipeline* fixedPipelineDrawing,
                                                                 const bool useNegativePolygonOffsetFlag)
{
    glPushAttrib(GL_ENABLE_BIT);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    
    switch (modelType) {
        case ModelTypeEnum::MODEL_TYPE_CHART:
            break;
        case ModelTypeEnum::MODEL_TYPE_CHART_TWO:
            break;
        case ModelTypeEnum::MODEL_TYPE_INVALID:
            break;
        case ModelTypeEnum::MODEL_TYPE_HISTOLOGY:
            break;
        case  ModelTypeEnum::MODEL_TYPE_MULTI_MEDIA:
            break;
        case ModelTypeEnum::MODEL_TYPE_SURFACE:
            break;
        case ModelTypeEnum::MODEL_TYPE_SURFACE_MONTAGE:
            break;
        case ModelTypeEnum::MODEL_TYPE_VOLUME_SLICES:
            break;
        case ModelTypeEnum::MODEL_TYPE_WHOLE_BRAIN:
            /*
             * Enable depth so outlines in front or in back
             * of the slices.  Without this the volume surface
             * outlines "behind" the slices are visible and
             * it looks weird
             */
            glEnable(GL_DEPTH_TEST);
            break;
    }
    
    std::vector<GraphicsPrimitive*> contourPrimitives;

    if (outlineModel->isDisplayed()
        && outlineModel->isDrawLinesModeSelected()) {
        Surface* surface = outlineModel->getSurface();
        if (surface != NULL) {
            float thicknessPercentage = outlineModel->getThicknessPercentageViewportHeight();
            const float thicknessPixels = outlineModel->getThicknessPixelsObsolete();
            
            /*
             * Thickness was changed from pixels to percentage viewport height on Feb 02, 2018
             * If thickness percentage is negative, it was not present in an old
             * scene so convert pixels to percentage using viewports dimensions
             */
            if (thicknessPercentage < 0.0f) {
                thicknessPercentage = GraphicsUtilitiesOpenGL::convertPixelsToPercentageOfViewportHeight(thicknessPixels);
                if (thicknessPercentage > 0.0f) {
                    outlineModel->setThicknessPercentageViewportHeight(thicknessPercentage);
                }
            }
            
            if (outlineModel->getOutlineCachePrimitives(histologySlice,
                                                        underlayVolume,
                                                        outlineCacheKey,
                                                        contourPrimitives)) {
                /* OK, have cached primitives to draw */
            }
            else {
                CaretColorEnum::Enum outlineColor = CaretColorEnum::BLACK;
                int32_t colorSourceBrowserTabIndex = -1;
                
                VolumeSurfaceOutlineColorOrTabModel* colorOrTabModel = outlineModel->getColorOrTabModel();
                VolumeSurfaceOutlineColorOrTabModel::Item* selectedColorOrTabItem = colorOrTabModel->getSelectedItem();
                switch (selectedColorOrTabItem->getItemType()) {
                    case VolumeSurfaceOutlineColorOrTabModel::Item::ITEM_TYPE_BROWSER_TAB:
                        colorSourceBrowserTabIndex = selectedColorOrTabItem->getBrowserTabIndex();
                        outlineColor = CaretColorEnum::CUSTOM;
                        break;
                    case VolumeSurfaceOutlineColorOrTabModel::Item::ITEM_TYPE_COLOR:
                        outlineColor = selectedColorOrTabItem->getColor();
                        break;
                }
                const bool surfaceColorFlag = (colorSourceBrowserTabIndex >= 0);
                
                float* nodeColoringRGBA = NULL;
                if (surfaceColorFlag) {
                    nodeColoringRGBA = fixedPipelineDrawing->surfaceNodeColoring->colorSurfaceNodes(NULL,
                                                                                                    surface,
                                                                                                    colorSourceBrowserTabIndex);
                }
                
                float sliceSpacing(1.0);
                if (underlayVolume != NULL) {
                    sliceSpacing = underlayVolume->getMaximumVoxelSpacing();
                }
                else if (histologySlicesFile != NULL) {
                    sliceSpacing = histologySlicesFile->getSliceSpacing();
                }
                const float slicePlaneDepth(outlineModel->getSlicePlaneDepth());
                createContours(surface,
                               plane,
                               sliceSpacing,
                               outlineColor,
                               nodeColoringRGBA,
                               thicknessPercentage,
                               slicePlaneDepth,
                               getSeparation(outlineModel),
                               contourPrimitives);
                
                if (histologySlice != NULL) {
                    projectContoursToHistologySlice(histologySlice,
                                                    contourPrimitives);
                }
                
                outlineModel->setOutlineCachePrimitives(histologySlice,
                                                        underlayVolume,
                                                        outlineCacheKey,
                                                        contourPrimitives);
            }
        }
    }
    
    /**
     * Draw the contours.
     * Note: The primitives are now cached so DO NOT delete them.
     */
    for (auto primitive : contourPrimitives) {
        if (useNegativePolygonOffsetFlag) {
            glPolygonOffset(-1.0, -1.0);
        }
        else {
            glPolygonOffset(1.0, 1.0);
        }
        glEnable(GL_POLYGON_OFFSET_FILL);
        
        GraphicsEngineDataOpenGL::draw(primitive);
        
        glDisable(GL_POLYGON_OFFSET_FILL);
    }

    glPopAttrib();
}

/**
 * Project the contours that are in sterotaxic space to plane coordinates on the histology slice
 * @param histologySlice
 *    The histology slice
 * @param contourPrimitives
 *    Primitives containing the contours
 */
void
BrainOpenGLVolumeSurfaceOutlineDrawing::projectContoursToHistologySlice(const HistologySlice* histologySlice,
                                                                        std::vector<GraphicsPrimitive*>& contourPrimitives)
{
    for (GraphicsPrimitive* primitive : contourPrimitives) {
        CaretAssert(primitive);
        const std::vector<float>& coordinates(primitive->getFloatXYZ());
        const int32_t numCoordinates(primitive->getNumberOfVertices());
        std::vector<float> planeCoordinates;
        planeCoordinates.reserve(numCoordinates * 3);
        for (int32_t i = 0; i < numCoordinates; i++) {
            Vector3D planeXYZ;
            histologySlice->stereotaxicXyzToPlaneXyz(&coordinates[i * 3],
                                                     planeXYZ);
            planeCoordinates.insert(planeCoordinates.end(),
                                    &planeXYZ[0], &planeXYZ[0] + 3);
        }
        
        CaretAssert(planeCoordinates.size() == coordinates.size());
        primitive->replaceFloatXYZ(planeCoordinates);
    }
}

/**
 * Draw surface outlines on the volume slices WITHOUT caching
 *
 * @param underlayVolume
 *    The intersection volume (NULL if not drawing on a volume)
 * @param modelType
 *    Type of model being drawn.
 * @param plane
 *    Plane of the volume slice on which surface outlines are drawn.
 * @param outlineModel
 *    The surface outline model.
 * @param fixedPipelineDrawing
 *    The fixed pipeline drawing.
 * @param useNegativePolygonOffsetFlag
 *    If true, use a negative offset for polygon offset
 */
void
BrainOpenGLVolumeSurfaceOutlineDrawing::drawSurfaceOutlineNotCached(const VolumeMappableInterface* underlayVolume,
                                                                    const ModelTypeEnum::Enum modelType,
                                                                    const Plane& plane,
                                                                    const Matrix4x4& displayTransformMatrix,
                                                                    const bool displayTransformMatrixValidFlag,
                                                                    VolumeSurfaceOutlineModel* outlineModel,
                                                                    BrainOpenGLFixedPipeline* fixedPipelineDrawing,
                                                                    const bool useNegativePolygonOffsetFlag)
{
    glPushAttrib(GL_ENABLE_BIT);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    
    switch (modelType) {
        case ModelTypeEnum::MODEL_TYPE_CHART:
            break;
        case ModelTypeEnum::MODEL_TYPE_CHART_TWO:
            break;
        case ModelTypeEnum::MODEL_TYPE_INVALID:
            break;
        case ModelTypeEnum::MODEL_TYPE_HISTOLOGY:
            break;
        case  ModelTypeEnum::MODEL_TYPE_MULTI_MEDIA:
            break;
        case ModelTypeEnum::MODEL_TYPE_SURFACE:
            break;
        case ModelTypeEnum::MODEL_TYPE_SURFACE_MONTAGE:
            break;
        case ModelTypeEnum::MODEL_TYPE_VOLUME_SLICES:
            break;
        case ModelTypeEnum::MODEL_TYPE_WHOLE_BRAIN:
            /*
             * Enable depth so outlines in front or in back
             * of the slices.  Without this the volume surface
             * outlines "behind" the slices are visible and
             * it looks weird
             */
            glEnable(GL_DEPTH_TEST);
            break;
    }
    
    std::vector<GraphicsPrimitive*> contourPrimitives;
    
    if (outlineModel->isDisplayed()
        && outlineModel->isDrawLinesModeSelected()) {
        Surface* surface = outlineModel->getSurface();
        if (surface != NULL) {
            float thicknessPercentage = outlineModel->getThicknessPercentageViewportHeight();
            const float thicknessPixels = outlineModel->getThicknessPixelsObsolete();
            
            /*
             * Thickness was changed from pixels to percentage viewport height on Feb 02, 2018
             * If thickness percentage is negative, it was not present in an old
             * scene so convert pixels to percentage using viewports dimensions
             */
            if (thicknessPercentage < 0.0f) {
                thicknessPercentage = GraphicsUtilitiesOpenGL::convertPixelsToPercentageOfViewportHeight(thicknessPixels);
                if (thicknessPercentage > 0.0f) {
                    outlineModel->setThicknessPercentageViewportHeight(thicknessPercentage);
                }
            }
            
            CaretColorEnum::Enum outlineColor = CaretColorEnum::BLACK;
            int32_t colorSourceBrowserTabIndex = -1;
            
            VolumeSurfaceOutlineColorOrTabModel* colorOrTabModel = outlineModel->getColorOrTabModel();
            VolumeSurfaceOutlineColorOrTabModel::Item* selectedColorOrTabItem = colorOrTabModel->getSelectedItem();
            switch (selectedColorOrTabItem->getItemType()) {
                case VolumeSurfaceOutlineColorOrTabModel::Item::ITEM_TYPE_BROWSER_TAB:
                    colorSourceBrowserTabIndex = selectedColorOrTabItem->getBrowserTabIndex();
                    outlineColor = CaretColorEnum::CUSTOM;
                    break;
                case VolumeSurfaceOutlineColorOrTabModel::Item::ITEM_TYPE_COLOR:
                    outlineColor = selectedColorOrTabItem->getColor();
                    break;
            }
            const bool surfaceColorFlag = (colorSourceBrowserTabIndex >= 0);
            
            float* nodeColoringRGBA = NULL;
            if (surfaceColorFlag) {
                nodeColoringRGBA = fixedPipelineDrawing->surfaceNodeColoring->colorSurfaceNodes(NULL,
                                                                                                surface,
                                                                                                colorSourceBrowserTabIndex);
            }
            
            const float slicePlaneDepth(outlineModel->getSlicePlaneDepth());
            createContours(surface,
                           plane,
                           underlayVolume->getMaximumVoxelSpacing(),
                           outlineColor,
                           nodeColoringRGBA,
                           thicknessPercentage,
                           slicePlaneDepth,
                           getSeparation(outlineModel),
                           contourPrimitives);
        }
    }
    
    /**
     * Draw the contours.
     */
    for (auto primitive : contourPrimitives) {
        CaretAssert(primitive);
        if (useNegativePolygonOffsetFlag) {
            glPolygonOffset(-1.0, -1.0);
        }
        else {
            glPolygonOffset(1.0, 1.0);
        }
        glEnable(GL_POLYGON_OFFSET_FILL);
        
        if (displayTransformMatrixValidFlag) {
            const int32_t numVerts(primitive->getNumberOfVertices());
            for (int32_t i = 0; i < numVerts; i++) {
                Vector3D xyz;
                primitive->getVertexFloatXYZ(i, xyz);
                displayTransformMatrix.multiplyPoint3(xyz);
                primitive->replaceVertexFloatXYZ(i, xyz);
            }
        }
        
        GraphicsEngineDataOpenGL::draw(primitive);
        delete primitive;
        
        glDisable(GL_POLYGON_OFFSET_FILL);
    }

    glPopAttrib();
}

/**
 * Constructor.
 *
 * @param surface
 *     The surface .
 * @param plane
 *     Plane intersected with the surface.
 * @param sliceSpacingMM
 *     Spacing of slices in millimeters
 * @param outlineColor
 *     outline coloring or, if value is CUSTOM, use the vertex coloring
 * @param nodeColoringRGBA
 *     The per-vertex coloring if 'caretColor' is CUSTOM
 * @param thicknessPercentage
 *     Thickness of outlines percentage of viewport height
 * @param slicePlaneDepth
 *     Depth that slice plane along normal vector
 * @param userOutlineSeparation
 *     User override for outline separation when depth is greater than zero
 * @param contourPrimitivesOut
 *     Output with contour primitives
 */

void
BrainOpenGLVolumeSurfaceOutlineDrawing::createContours(const SurfaceFile* surface,
                                                       const Plane& plane,
                                                       const float sliceSpacingMM,
                                                       const CaretColorEnum::Enum outlineColor,
                                                       const float* nodeColoringRGBA,
                                                       const float thicknessPercentage,
                                                       const float slicePlaneDepth,
                                                       const float userOutlineSeparation,
                                                       std::vector<GraphicsPrimitive*>& contourPrimitivesOut)
{
    contourPrimitivesOut.clear();
    
    const bool timingFlag(false);
    ElapsedTimer timer;
    if (timingFlag) {
        timer.start();
    }
    if (slicePlaneDepth > 0.0) {
        int32_t numSteps(0);
        float depthStart(0.0);
        float depthStepSize(0.0);
        computeDepthNumStepsAndStepSize(sliceSpacingMM,
                                        slicePlaneDepth,
                                        userOutlineSeparation,
                                        numSteps,
                                        depthStart,
                                        depthStepSize);
        const bool debugFlag(false);
        if (debugFlag) {
            std::cout << "SlicePlaneDepth: " << slicePlaneDepth
            << " Start: " << depthStart
            << " Steps: " << numSteps
            << " Size: " << depthStepSize << std::endl;
        }
#pragma omp CARET_PARFOR schedule(dynamic)
        for (int32_t i = 0; i < numSteps; i++) {
            const float depthOffset(depthStart +
                                    depthStepSize * static_cast<float>(i));
            Plane intersectionPlane(plane);
            intersectionPlane.shiftPlane(depthOffset);
            const Plane drawOnPlane(plane);
            if (debugFlag) {
                std::cout << i << "   Intersect Plane: " << intersectionPlane.toString() << std::endl;
                std::cout << i << "   Draw On Plane: " << drawOnPlane.toString() << std::endl;
            }
            SurfacePlaneIntersectionToContour contour(surface,
                                                      intersectionPlane,
                                                      drawOnPlane,
                                                      outlineColor,
                                                      nodeColoringRGBA,
                                                      thicknessPercentage);
            AString errorMessage;
            std::vector<GraphicsPrimitive*> primitives;
            if (contour.createContours(primitives,
                                       errorMessage)) {
#pragma omp critical
                {
                    contourPrimitivesOut.insert(contourPrimitivesOut.end(),
                                                primitives.begin(),
                                                primitives.end());
                }
            }
            else {
#pragma omp critical
                {
                    CaretLogSevere(errorMessage);
                }
            }
        }
    }
    else {
        SurfacePlaneIntersectionToContour contour(surface,
                                                  plane,
                                                  outlineColor,
                                                  nodeColoringRGBA,
                                                  thicknessPercentage);
        AString errorMessage;
        if ( ! contour.createContours(contourPrimitivesOut,
                                      errorMessage)) {
            CaretLogSevere(errorMessage);
        }
    }
    if (timingFlag) {
        std::cout << "Time to compute all contours: " << timer.getElapsedTimeMilliseconds() << "ms" << std::endl;
    }
}

/**
 * Compute the number of steps and step size for slice plane depth
 * @param sliceSpacingMM
 *    Spacing of the slices
 * @param slicePlaneDepth
 *    Slice plane depth set by user
 * @param userOutlineSeparation
 *     User override for outline separation when depth is greater than zero
 * @param numStepsOut
 *    Number of steps output
 * @param depthStartOut
 *    Starting depth value output
 * @param depthStepSizeOut
 *    Depth step size output
 */
void
BrainOpenGLVolumeSurfaceOutlineDrawing::computeDepthNumStepsAndStepSize(const float sliceSpacingMM,
                                                                        const float slicePlaneDepth,
                                                                        const float userOutlineSeparation,
                                                                        int32_t& numStepsOut,
                                                                        float& depthStartOut,
                                                                        float& depthStepSizeOut)
{
    /*
     * Default to normal (infinitely thin slice)
     */
    numStepsOut      = 1;
    depthStartOut    = 0.0;
    depthStepSizeOut = 0.0;
    if (slicePlaneDepth <= 0.0) {
        return ;
    }
    
    const float outlineSeparation(userOutlineSeparation);

    /*
     * If spacing valid, use 1/2 spacing for step size; else 0.5mm
     */
    depthStepSizeOut = (sliceSpacingMM / 2.0);
    if (outlineSeparation > 0.0) {
        depthStepSizeOut = outlineSeparation;
    }
    if (depthStepSizeOut <= 0.0) {
        depthStepSizeOut = 0.5;
    }
    
    /*
     * Set number of steps
     */
    numStepsOut = static_cast<int32_t>(std::ceil(slicePlaneDepth / depthStepSizeOut));
    if (numStepsOut <= 1) {
        numStepsOut      = 1;
        depthStartOut    = 0.0;
        depthStepSizeOut = 0.0;
        return;
    }
    
    /*
     * Limit number of steps in event user sets depth very
     * large and separation very small
     */
    const int32_t maxSteps(51);
    if (numStepsOut > maxSteps) {
        numStepsOut = maxSteps;
    }
    
    /*
     * Always make number of steps odd so that we
     * do an intersection on the plane of the displayed
     * histology or volume slice
     */
    if (MathFunctions::isEvenNumber(numStepsOut)) {
        ++numStepsOut;
    }
    
    /*
     * Reset step size using number of steps but use
     * number of steps minus one so that the middle step
     * is centered on the intersection plane.
     *
     * For example: If there are three steps, this places
     * an intersections at:
     * % at minus half depth
     * % at the interection plane
     * % at plus half depth
     */
    CaretAssert(MathFunctions::isOddNumber(numStepsOut));
    CaretAssert(numStepsOut >= 3);
    depthStepSizeOut = (slicePlaneDepth / (numStepsOut - 1));
    depthStartOut = -slicePlaneDepth / 2.0;
}

/**
 * @return The outline separation for the given outline model
 * @param outline
 *    The outline model.
 */
float
BrainOpenGLVolumeSurfaceOutlineDrawing::getSeparation(const VolumeSurfaceOutlineModel* /*outline*/) const
{
    CaretPreferences* prefs(SessionManager::get()->getCaretPreferences());
    CaretAssert(prefs);
    return prefs->getVolumeSurfaceOutlineSeparation();
}

