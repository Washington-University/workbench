
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
BrainOpenGLVolumeSurfaceClippedOutlineDrawing::BrainOpenGLVolumeSurfaceClippedOutlineDrawing(const Plane& plane,
                                                                                             const Vector3D& pointOnPlane,
                                                                                             const VolumeSurfaceOutlineSetModel* outlineSet,
                                                                                             BrainOpenGLFixedPipeline* fixedPipelineDrawing)
: CaretObject(),
m_plane(plane),
m_pointOnPlane(pointOnPlane),
m_outlineSet(outlineSet),
m_fixedPipelineDrawing(fixedPipelineDrawing)
{
    CaretAssert(outlineSet);
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
    
    /*
     * Process each surface outline
     * As of 24 May, "zero" is on top so draw in reverse order
     */
    const int32_t numberOfOutlines = m_outlineSet->getNumberOfDislayedVolumeSurfaceOutlines();
    for (int32_t io = (numberOfOutlines - 1);
         io >= 0;
         io--) {
        std::vector<GraphicsPrimitive*> contourPrimitives;
        
        const VolumeSurfaceOutlineModel* outline = m_outlineSet->getVolumeSurfaceOutlineModel(io);
        if (outline->isDisplayed()
            && outline->isDrawSurfaceModeSelected()) {
            const Surface* surface = outline->getSurface();
            if (surface != NULL) {
                CaretColorEnum::Enum outlineColor = CaretColorEnum::BLACK;
                int32_t colorSourceBrowserTabIndex = -1;
                
                const VolumeSurfaceOutlineColorOrTabModel* colorOrTabModel = outline->getColorOrTabModel();
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

                const float slicePlaneDepth(outline->getSlicePlaneDepth());

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
                if ( ! validPlanesFlag) {
                    continue;
                }

                const auto planeEquationOne(planeOne.getPlaneEquation());
                const auto planeEquationTwo(planeTwo.getPlaneEquation());
                
                glClipPlane(GL_CLIP_PLANE0,
                            planeEquationOne.data());
                glEnable(GL_CLIP_PLANE0);
                glClipPlane(GL_CLIP_PLANE1,
                            planeEquationTwo.data());
                glEnable(GL_CLIP_PLANE1);

                Surface* surface(const_cast<Surface*>(outline->getSurface()));
                if ( ! surface) {
                    CaretLogWarning("Surface is invalid");
                    continue;
                }
                
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
                
                drawSurfaceTrianglesWithVertexArrays(surface,
                                                     nodeColoringRGBA,
                                                     solidRGBA);
                
//                createContours(surface,
//                               plane,
//                               underlayVolume->getMaximumVoxelSpacing(),
//                               outlineColor,
//                               nodeColoringRGBA,
//                               thicknessPercentage,
//                               slicePlaneDepth,
//                               outline->getUserOutlineSlicePlaneDepthSeparation(),
//                               contourPrimitives);
            }
        }
        
//        /**
//         * Draw the contours.
//         */
//        for (auto primitive : contourPrimitives) {
//            CaretAssert(primitive);
//            if (useNegativePolygonOffsetFlag) {
//                glPolygonOffset(-1.0, -1.0);
//            }
//            else {
//                glPolygonOffset(1.0, 1.0);
//            }
//            glEnable(GL_POLYGON_OFFSET_FILL);
//
//            if (displayTransformMatrixValidFlag) {
//                const int32_t numVerts(primitive->getNumberOfVertices());
//                for (int32_t i = 0; i < numVerts; i++) {
//                    Vector3D xyz;
//                    primitive->getVertexFloatXYZ(i, xyz);
//                    displayTransformMatrix.multiplyPoint3(xyz);
//                    primitive->replaceVertexFloatXYZ(i, xyz);
//                }
//            }
//
//            GraphicsEngineDataOpenGL::draw(primitive);
//            delete primitive;
//
//            glDisable(GL_POLYGON_OFFSET_FILL);
//        }
    }
    
    glPopAttrib();
}

/**
 * Draw a surface triangles with vertex arrays.
 * @param surface
 *    Surface that is drawn.
 * @param nodeColoringRGBA
 *    RGBA coloring for the nodes.
 * @param solidRGBA
 *    Solid color RGBA used if nodeColoringRGBA is invalid
 */
void
BrainOpenGLVolumeSurfaceClippedOutlineDrawing::drawSurfaceTrianglesWithVertexArrays(const Surface* surface,
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
                    reinterpret_cast<const GLvoid*>(surface->getCoordinate(0)));
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
