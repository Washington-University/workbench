
/*LICENSE_START*/
/*
 *  Copyright (C) 2024 Washington University School of Medicine
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

#define __BRAIN_OPEN_G_L_VOLUME_SURFACE_CLIPPED_OUTLINE_DRAWING_DECLARE__
#include "BrainOpenGLVolumeSurfaceClippedOutlineDrawing.h"
#undef __BRAIN_OPEN_G_L_VOLUME_SURFACE_CLIPPED_OUTLINE_DRAWING_DECLARE__

#include "CaretAssert.h"
#include "CaretLogger.h"
#include "BrainOpenGLFixedPipeline.h"
#include "HistologySlice.h"
#include "Plane.h"
#include "Surface.h"
#include "SurfaceNodeColoring.h"
#include "VolumeSurfaceOutlineColorOrTabModel.h"
#include "VolumeSurfaceOutlineModel.h"
#include "VolumeSurfaceOutlineSetModel.h"

using namespace caret;


    
/**
 * \class caret::BrainOpenGLVolumeSurfaceClippedOutlineDrawing 
 * \brief Draw surface outlnes by clipping a surface
 * \ingroup Brain
 */

/**
 * Constructor.
 */
BrainOpenGLVolumeSurfaceClippedOutlineDrawing::BrainOpenGLVolumeSurfaceClippedOutlineDrawing(const HistologySlice* histologySlice,
                                                                                             const Plane& plane,
                                                                                             const Vector3D& pointOnPlane,
                                                                                             const VolumeSurfaceOutlineModel* outlineModel,
                                                                                             BrainOpenGLFixedPipeline* fixedPipelineDrawing)
: CaretObject(),
m_histologySlice(histologySlice),
m_plane(plane),
m_pointOnPlane(pointOnPlane),
m_outlineModel(outlineModel),
m_fixedPipelineDrawing(fixedPipelineDrawing)
{
    CaretAssert(outlineModel);
    CaretAssert(fixedPipelineDrawing);

}

/**
 * Destructor.
 */
BrainOpenGLVolumeSurfaceClippedOutlineDrawing::~BrainOpenGLVolumeSurfaceClippedOutlineDrawing()
{
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
BrainOpenGLVolumeSurfaceClippedOutlineDrawing::toString() const
{
    return "BrainOpenGLVolumeSurfaceClippedOutlineDrawing";
}

/**
 * Draw surface outlines on the given plane
 * @param plane
 *    The plane
 * @param outlineSet
 *    The outline set
 * @param fixedPipelineDrawing
 *    Fixed pipeline drawing
 */
void
BrainOpenGLVolumeSurfaceClippedOutlineDrawing::drawSurfaceOutline()
{
    if ( ! m_plane.isValidPlane()) {
        CaretLogWarning("Invalid plane for clipped outline drawing");
        return;
    }
    
    glPushAttrib(GL_ENABLE_BIT
                 | GL_TRANSFORM_BIT);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    
    std::vector<GraphicsPrimitive*> contourPrimitives;
    
    if (m_outlineModel->isDisplayed()
        && m_outlineModel->isDrawSurfaceModeSelected()) {
        const Surface* surface = m_outlineModel->getSurface();
        if (surface != NULL) {
            CaretColorEnum::Enum outlineColor = CaretColorEnum::BLACK;
            int32_t colorSourceBrowserTabIndex = -1;
            
            const VolumeSurfaceOutlineColorOrTabModel* colorOrTabModel = m_outlineModel->getColorOrTabModel();
            const VolumeSurfaceOutlineColorOrTabModel::Item* selectedColorOrTabItem = colorOrTabModel->getSelectedItem();
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
            
            float slicePlaneDepth(m_outlineModel->getSlicePlaneDepth());
            if (slicePlaneDepth <= 0.0001) {
                slicePlaneDepth = VolumeSurfaceOutlineModel::getDefaultSurfaceDepthMillimeters();
            }
            
            Plane planeOne;
            Plane planeTwo;
            getClippingPlanes(slicePlaneDepth,
                              planeOne,
                              planeTwo);
            
            bool validPlanesFlag(true);
            if ( ! planeOne.isValidPlane()) {
                CaretLogWarning("Failed to create clipping plane one");
                validPlanesFlag = false;
            }
            if ( ! planeTwo.isValidPlane()) {
                CaretLogWarning("Failed to create clipping plane two");
                validPlanesFlag = false;
            }
            if (validPlanesFlag) {
                const auto planeEquationOne(planeOne.getPlaneEquation());
                const auto planeEquationTwo(planeTwo.getPlaneEquation());
                
                glClipPlane(GL_CLIP_PLANE0,
                            planeEquationOne.data());
                glEnable(GL_CLIP_PLANE0);
                glClipPlane(GL_CLIP_PLANE1,
                            planeEquationTwo.data());
                glEnable(GL_CLIP_PLANE1);
                
                Surface* surface(const_cast<Surface*>(m_outlineModel->getSurface()));
                if (surface) {
                    float solidRGBA[4] { 0.0f, 0.0f, 0.0f, 0.0f };
                    float* nodeColoringRGBA = NULL;
                    if (surfaceColorFlag) {
                        nodeColoringRGBA = m_fixedPipelineDrawing->surfaceNodeColoring->colorSurfaceNodes(NULL,
                                                                                                          surface,
                                                                                                          colorSourceBrowserTabIndex);
                    }
                    else {
                        CaretColorEnum::toRGBAFloat(outlineColor,
                                                    solidRGBA);
                    }
                    
                    const float* surfaceCoordinateXYZ(surfaceCoordinateXYZ = surface->getCoordinate(0));
                    
                    std::vector<float> surfaceTransformedXYZ;
                    if (m_histologySlice != NULL) {
                        const int32_t numXYZ(surface->getNumberOfNodes());
                        surfaceTransformedXYZ.resize(numXYZ * 3);
                        for (int32_t i = 0; i < numXYZ; i++) {
                            Vector3D xyz;
                            m_histologySlice->stereotaxicXyzToPlaneXyz(surface->getCoordinate(i),
                                                                       xyz);
                            surfaceTransformedXYZ[i*3]     = xyz[0];
                            surfaceTransformedXYZ[i*3 + 1] = xyz[1];
                            surfaceTransformedXYZ[i*3 + 2] = xyz[2];
                        }
                        
                        surfaceCoordinateXYZ = &surfaceTransformedXYZ[0];
                    }
                    drawSurfaceTrianglesWithVertexArrays(surface,
                                                         surfaceCoordinateXYZ,
                                                         nodeColoringRGBA,
                                                         solidRGBA);
                }
                else {
                    CaretLogWarning("Surface is invalid");
                }
            }
        }
    }

    glPopAttrib();
}

/**
 * Draw a surface triangles with vertex arrays.
 * @param surface
 *    Surface that is drawn.
 * @param surfaceCoordinateXYZ
 *    The surface's  coordinates
 * @param nodeColoringRGBA
 *    RGBA coloring for the nodes.
 * @param solidRGBA
 *    Solid color RGBA used if nodeColoringRGBA is invalid
 */
void
BrainOpenGLVolumeSurfaceClippedOutlineDrawing::drawSurfaceTrianglesWithVertexArrays(const Surface* surface,
                                                                                    const float* surfaceCoordinateXYZ,
                                                                                    const float* nodeColoringRGBA,
                                                                                    const float solidRGBA[4]) const
{
    glEnableClientState(GL_VERTEX_ARRAY);
    if (nodeColoringRGBA != NULL) {
        glEnableClientState(GL_COLOR_ARRAY);
    }
    glEnableClientState(GL_NORMAL_ARRAY);
    glVertexPointer(3,
                    GL_FLOAT,
                    0,
                    reinterpret_cast<const GLvoid*>(surfaceCoordinateXYZ));
    if (nodeColoringRGBA != NULL) {
        glColorPointer(4,
                       GL_FLOAT,
                       0,
                       reinterpret_cast<const GLvoid*>(nodeColoringRGBA));
    }
    else {
        glColor3fv(solidRGBA);
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
 * Create the planes for clipping the surface
 * @param slicePlaneDepth
 *    Depth of surface outline
 * @param planeOneOut
 *    Output with first plane for clipping
 * @param planeTwoOut
 *    Outline with second plane for clipping
 */
void
BrainOpenGLVolumeSurfaceClippedOutlineDrawing::getClippingPlanes(const float slicePlaneDepth,
                                                                 Plane& planeOneOut,
                                                                 Plane& planeTwoOut) const
{
    const float halfDepth(slicePlaneDepth / 2.0);
    
    const Vector3D normalVector(m_plane.getNormalVector());
    const Vector3D offsetXYZ(normalVector * halfDepth);
    
    const Vector3D pointOnPlaneOneXYZ(m_pointOnPlane - offsetXYZ);
    planeOneOut = Plane(normalVector,
                        pointOnPlaneOneXYZ);
    
    const Vector3D pointOnPlaneTwoXYZ(m_pointOnPlane + offsetXYZ);
    planeTwoOut = Plane(-normalVector,
                        pointOnPlaneTwoXYZ);
}
