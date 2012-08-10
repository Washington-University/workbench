/*LICENSE_START*/ 
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


#include "SurfaceProjector.h"

#include "SurfaceFile.h"
//#include "SurfaceNodeLocator.h"
#include "SurfaceProjectedItem.h"
//#include "SurfaceProjectorBarycentricInformation.h"
#include "TopologyHelper.h"

using namespace caret;

/**
 * Constructor.
 *
 * @param surface - Surface to which projection takes place.
 * @param surfaceTypeHint - Hint at the type of surface that
 *   allows some optimization or higher quality projection.
 * @param projectionsAllowed - Allows the Van Essen
 *   projection which projects to the edge of two triangles.
 *
 */
SurfaceProjector::SurfaceProjector(const SurfaceFile* surface,
                                   const SurfaceHintType surfaceTypeHint,
                                   const ProjectionsAllowedType projectionsAllowed,
                                   const bool surfaceMayGetModifiedFlag)
    : CaretObject()
{
    this->initializeMembersSurfaceProjector();
}

/**
 * Destructor
 */
SurfaceProjector::~SurfaceProjector()
{
}

/**
 * Copy Constructor
 * @param Object that is copied.
 */
SurfaceProjector::SurfaceProjector(const SurfaceProjector& o)
    : CaretObject(o)
{
    this->initializeMembersSurfaceProjector();
    this->copyHelper(o);
}

/**
 * Assignment operator.
 */
SurfaceProjector&
SurfaceProjector::operator=(const SurfaceProjector& o)
{
    if (this != &o) {
        CaretObject::operator=(o);
        this->copyHelper(o);
    };
    return *this;
}

/**
 * Helps with copy constructor and assignment operator.
 */
void
SurfaceProjector::copyHelper(const SurfaceProjector& o)
{
}

void
SurfaceProjector::initializeMembersSurfaceProjector()
{
//    this->surface = NULL_POINTER;
//    this->numberOfNodes = 0;
//    this->surfaceTypeHint = NULL_POINTER;
//    this->projectionsAllowed = NULL_POINTER;
//    this->surfaceNodeLocator = NULL_POINTER;
//    this->topologyHelper = NULL_POINTER;
//    this->nearestNodeToleranceSquared = 0.0f;
//    this->sphericalSurfaceRadius = 0.0f;
//    this->surfaceOffset = 0.0f;
//    this->surfaceOffsetValid = false;
//    this->barycentricProjectionInfo = NULL_POINTER;
//    this->surfaceMayGetModifiedFlag = false;
}
/**
 * Set the desired offset of projected items from the surface.
 *
 * @param surfaceOffset - distance above the surface.
 *
 */
void
SurfaceProjector::setSurfaceOffset(const float surfaceOffset)
{
}

///**
// * Project to the surface
// * @param spi - Item that is to be projected.  Its contents will be
// *    updated to reflect the projection.  This items XYZ coordinate
// *    is used for the projection point.
// * @param positionSource - source of position being projected
// *
// * @throws SurfaceProjectorException  If projecting an item
// *   failed.
// *
// */
//void
//SurfaceProjector::projectToSurface(
//                   const SurfaceProjectedItem* spi,
//                   const ProjectionPositionSourceType positionSource)
//            throw (SurfaceProjectorException)
//{
//}
//
///**
// * Project a coordinate to the surface using a barycentric projection.
// * @param xyz - the coordinate
// * @return The projection status.
// *
// * @throws SurfaceProjectorException  If projecting an item
// *   failed.
// *
// */
//SurfaceProjectorBarycentricInformation
//SurfaceProjector::projectToSurfaceBestTriangle2D(const float xyz[])
//            throw (SurfaceProjectorException)
//{
//}
//
///**
// * Project a coordinate to the surface using a barycentric projection.  The
// * nodes in the barycentric projection are arranged so that the first node
// * is the node closest to the coordinate passed to this method.  
// * @param xyz - the coordinate
// * @return The projection information whose nodes and areas are properly
// * set for both projection success and failure.  When the projection fails,
// * the projections nodes are set to -1.
// *
// * @throws SurfaceProjectorException  If projecting an item
// *   failed.
// *
// */
//SurfaceProjectorBarycentricInformation
//SurfaceProjector::projectToSurfaceForRegistration(const float xyz[])
//            throw (SurfaceProjectorException)
//{
//}
//
///**
// * Project a coordinate to the surface using a barycentric projection.
// * @param xyz - the coordinate
// * @return The projection status.
// *
// * @throws SurfaceProjectorException  If projecting an item
// *   failed.
// *
// */
//SurfaceProjectorBarycentricInformation
//SurfaceProjector::projectToSurface(const float xyz[])
//            throw (SurfaceProjectorException)
//{
//}
//
///**
// * Project a coordinate to the surface.
// * @param xyzIn - the coordinate
// * @return The projection status.
// *
// * @throws SurfaceProjectorException  If projecting an item
// *   failed.
// *
// */
//SurfaceProjectorBarycentricInformation
//SurfaceProjector::projectToSurfaceAux(const float xyzIn[])
//            throw (SurfaceProjectorException)
//{
//}

/**
 * Check the triangles used by this node see if the coordinate
 * is contained in any of the triangles.
 * 
 * @param nearestNode - Node of the triangles.
 * @param xyz - coordinate that is being projected.
 *
 */
void
SurfaceProjector::findEnclosingTriangle(
                   const int32_t nearestNode,
                   const float xyz[3])
{
}

/**
 * See if the coordinate is within the triangle.
 * @param triangleNumber - triangle to check.
 * @param xyz - the coordinate
 *
 */
void
SurfaceProjector::checkItemInTriangle(
                   const int32_t triangleNumber,
                   const float xyz[3])
{
}

/**
 * Compute the signed areas formed by assuming "xyz" is contained in the triangle formed
 * by the points "p1, p2, p3".  "area2" and "area3" may not be set if "xyz" is not
 * within the triangle.
 *
 * @param p1 - point in triangle.
 * @param p2 - point in triangle.
 * @param p3 - point in triangle.
 * @param normal - The normal vector.
 * @param xyz - the coordinate being examined.
 * @param areasOut - barycentric areas of xyz in the triangle OUTPUT.
 * @return
 *    Returns 1 if all areas are positive (point inside the triangle).
 *    Returns -1 if all areas are greater than the tolerance
 *        (point may be on edge or vertex)
 *    Returns 0 if not in the triangle.
 *
 */
int32_t
SurfaceProjector::triangleAreas(
                   const float p1[3],
                   const float p2[3],
                   const float p3[3],
                   const float normal[3],
                   const float xyz[3],
                   const float areasOut[3])
{
    return 0;
}

/**
 * Perform a VanEssen Projection that projects to the edge of two triangles.
 *
 * @param spi - Item for projection.
 * @param nearestTriangleIn - Nearest triangle from barycentric
 *    projection attempt, may be invalid (< 0).
 * @param xyzIn - Location of item for projection.
 * @throws SurfaceProjectorException  If projection failure.
 *
 */
void
SurfaceProjector::projectWithVanEssenAlgorithm(
                   const SurfaceProjectedItem* spi,
                   const int32_t nearestTriangleIn,
                   const float xyzIn[3])
            throw (SurfaceProjectorException)
{
}

/**
 * Find the triangle nearest the coordinate.
 *
 * @param xyz - the coordinate
 *
 */
int32_t
SurfaceProjector::findNearestTriangle(const float xyz[3])
{
    return -1;
}

/**
 * Create a surface node locator.
 * @return  A new surface node locator.
 *
 */
//SurfaceNodeLocator*
//SurfaceProjector::createSurfaceNodeLocator()
//{
//}

