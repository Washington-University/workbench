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

#include <cmath>
#include <limits>

#define __SURFACE_PROJECTOR_DEFINE__
#include "SurfaceProjector.h"
#undef __SURFACE_PROJECTOR_DEFINE__

#include "CaretLogger.h"
#include "DescriptiveStatistics.h"
#include "FociFile.h"
#include "Focus.h"
#include "MathFunctions.h"
#include "SurfaceFile.h"
#include "SurfaceProjectedItem.h"
#include "SurfaceProjectionBarycentric.h"
#include "SurfaceProjectionVanEssen.h"
#include "TopologyHelper.h"

using namespace caret;

/**
 * \class caret::SurfaceProjector
 * \brief Project points to a surface.
 */

/**
 * Constructor for projection to a given surface.
 *
 * @param surfaceFile
 *    Surface to which projection takes place.  For proper
 *    projection (particularly flat and spherical) surfaces, it is important
 *    that the surface's type is correctly set.
 */
SurfaceProjector::SurfaceProjector(const SurfaceFile* surfaceFile)
    : CaretObject()
{
    CaretAssert(surfaceFile);
    m_surfaceFiles.push_back(surfaceFile);
    initializeMembersSurfaceProjector();
}

/**
 * Constructor for projection to closest of a group of surfaces.
 *
 * @param surfaceFiles
 *    Vector of Surfaces to which projection takes place.  For proper
 *    projection (particularly flat and spherical) surfaces, it is important
 *    that the surface's type is correctly set.
 */
SurfaceProjector::SurfaceProjector(const std::vector<const SurfaceFile*>& surfaceFiles)
: CaretObject()
{
    const int32_t numberOfSurfaces = static_cast<int32_t>(surfaceFiles.size());
    for (int32_t i = 0; i < numberOfSurfaces; i++) {
        m_surfaceFiles.push_back(surfaceFiles[i]);
    }
    initializeMembersSurfaceProjector();
}

/**
 * Destructor
 */
SurfaceProjector::~SurfaceProjector()
{
}

/**
 * Initialize members of this instance.
 */
void
SurfaceProjector::initializeMembersSurfaceProjector()
{
    m_surfaceOffset = 0.0;
    m_surfaceOffsetValid = false;
    computeSurfaceNearestNodeTolerances();
}


/**
 * Set the desired offset of projected items from the surface->
 *
 * @param surfaceOffset - distance above the surface->
 *
 */
void
SurfaceProjector::setSurfaceOffset(const float surfaceOffset)
{
    m_surfaceOffset = surfaceOffset;
    m_surfaceOffsetValid = true;
}

/**
 * Project all foci in a foci file.
 * @param fociFile
 *     The foci file.
 * @throws SurfaceProjectorException
 *      If projecting an item failed.
 */
void
SurfaceProjector::projectFociFile(FociFile* fociFile) throw (SurfaceProjectorException)
{
    bool TEST_FLAG = false;
    CaretAssert(fociFile);
    const int32_t numberOfFoci = fociFile->getNumberOfFoci();
    
    AString validateString;
    bool validateFlag = true;
    AString errorMessage = "";
    for (int32_t i = 0; i < numberOfFoci; i++) {
        Focus* focus = fociFile->getFocus(i);
        try {
            if (TEST_FLAG) {
                if (focus->getProjection(0)->isStereotaxicXYZValid()) {
                    const float* xyz = focus->getProjection(0)->getStereotaxicXYZ();
                    const int32_t node = m_surfaceFiles[0]->closestNode(xyz);
                    CaretAssert((node >= 0) && (node < m_surfaceFiles[0]->getNumberOfNodes()));
                }
            }
            else {
                projectFocus(focus);
                
                if (validateFlag) {
                    SurfaceProjectedItem* spi = focus->getProjection(0);
                    if (spi->getBarycentricProjection()->isValid()
                        || spi->getVanEssenProjection()->isValid()) {
                        for (std::vector<const SurfaceFile*>::const_iterator iter = m_surfaceFiles.begin();
                             iter != m_surfaceFiles.end();
                             iter++) {
                            const SurfaceFile* sf = *iter;
                            if (sf->getStructure() == spi->getStructure()) {
                                float projXYZ[3];
                                spi->getProjectedPosition(*sf, projXYZ, false);
                                float stereoXYZ[3];
                                spi->getStereotaxicXYZ(stereoXYZ);
                                const float dist = MathFunctions::distance3D(projXYZ, stereoXYZ);
                                if (dist > 1.0) {
                                    bool degenString = "";
                                    if (spi->getBarycentricProjection()->isValid()) {
                                        if (spi->getBarycentricProjection()->isDegenerate()) {
                                            degenString = " DEGENERATE";
                                        }
                                    }
                                    validateString += (focus->getName()
                                                     + " Index="
                                                     + AString::number(i)
                                                     + ": stereo/proj positions differ by "
                                                     + AString::number(dist, 'f', 3)
                                                     + degenString
                                                     + "\n");
                                }
                            }
                        }
                    }
                    else {
                        validateString += (focus->getName()
                                         + " Index="
                                         + AString::number(i)
                                         + " failed to project\n");
                    }
                }
                
                
            }
        }
        catch (const SurfaceProjectorException& spe) {
            if (errorMessage.isEmpty() == false) {
                errorMessage += "\n";
            }
            errorMessage += (focus->getName()
                             + ", index="
                             + AString::number(i)
                             + ": "
                             + spe.whatString());
        }
    }
    
    if (validateString.isEmpty() == false) {
        std::cout << qPrintable(validateString) << std::endl;
    }
    
    if (errorMessage.isEmpty() == false) {
        throw SurfaceProjectorException(errorMessage);
    }
}

/**
 * Project a focus.
 * @param focus
 *    The focus.
 * @throws SurfaceProjectorException
 *      If projecting an item failed.
 */
void
SurfaceProjector::projectFocus(Focus* focus) throw (SurfaceProjectorException)
{
    const int32_t numberOfProjections = focus->getNumberOfProjections();
    CaretAssert(numberOfProjections > 0);
    if (numberOfProjections < 0) {
        throw SurfaceProjectorException("Focus has no projections, no stereotaxic coordinate.");
    }
    focus->removeExtraProjections();
    SurfaceProjectedItem* spi = focus->getProjection(0);
    projectItemToTriangleOrEdge(spi);
}

/**
 * Project to the surface(s) triangles (barycentric projection)
 *
 * @param spi
 *    Item that is to be projected.  Its contents will be
 *    updated to reflect the projection.  This item's stereotaxic coordinate
 *    is used for the projection point.
 *
 * @throws SurfaceProjectorException
 *      If projecting an item failed.
 */
void
SurfaceProjector::projectItemToTriangle(SurfaceProjectedItem* spi) throw (SurfaceProjectorException)
{
    CaretAssert(spi);
    m_allowEdgeProjection = false;
    projectItem(spi);
}

/**
 * Project to the surface(s) triangles (barycentric projection)
 * or edges (van-essen projection).
 *
 * @param spi
 *    Item that is to be projected.  Its contents will be
 *    updated to reflect the projection.  This item's stereotaxic coordinate
 *    is used for the projection point.
 *
 * @throws SurfaceProjectorException
 *      If projecting an item failed.
 */
void
SurfaceProjector::projectItemToTriangleOrEdge(SurfaceProjectedItem* spi) throw (SurfaceProjectorException)
{
    CaretAssert(spi);
    m_allowEdgeProjection = true;
    projectItem(spi);
}


/**
 * Project to the surface(s)
 *
 * @param spi
 *    Item that is to be projected.  Its contents will be
 *    updated to reflect the projection.  This item's stereotaxic coordinate
 *    is used for the projection point.
 *
 * @throws SurfaceProjectorException  
 *      If projecting an item failed.
 */
void
SurfaceProjector::projectItem(SurfaceProjectedItem* spi) throw (SurfaceProjectorException)
{
    const int32_t numberOfSurfaceFiles = static_cast<int32_t>(m_surfaceFiles.size());
    if (numberOfSurfaceFiles <= 0) {
        throw SurfaceProjectorException("No surface for projection!");
    }
    
    /*
     * Get position of item.
     */
    float xyz[3];
    if (spi->isStereotaxicXYZValid() == false) {
        throw new SurfaceProjectorException( "Stereotaxic position is invalid, cannot project.");
    }
    spi->getStereotaxicXYZ(xyz);
    
    if (numberOfSurfaceFiles == 1) {
        projectToSurface(m_surfaceFiles[0],
                         xyz,
                         spi);
    }
    else {
        /*
         * Find surface closest to node.
         */
        int32_t nearestSurfaceIndex = 0;
        float nearestDistance = std::numeric_limits<float>::max();
        for (int32_t i = 0; i < numberOfSurfaceFiles; i++) {
            const SurfaceFile* sf = m_surfaceFiles[i];
            const int32_t node = sf->closestNode(xyz);
            CaretAssertArrayIndex("nodes", sf->getNumberOfNodes(), node);
            if (node >= 0) {
                const float* nodeXYZ = sf->getCoordinate(node);
                const float dist = MathFunctions::distanceSquared3D(xyz,
                                                                    nodeXYZ);
                if (dist < nearestDistance) {
                    nearestDistance = dist;
                    nearestSurfaceIndex = i;
                }
            }
        }
        
        if (nearestSurfaceIndex >= 0) {
            CaretAssertVectorIndex(m_surfaceNearestNodeToleranceSquared, nearestSurfaceIndex);
            m_nearestNodeToleranceSquared = m_surfaceNearestNodeToleranceSquared[nearestSurfaceIndex];
            
            const SurfaceFile* sf = m_surfaceFiles[nearestSurfaceIndex];
            projectToSurface(sf,
                             xyz,
                             spi);
        }
    }
}

/**
 * Compute the nearest node tolerances for each surface.
 */
void
SurfaceProjector::computeSurfaceNearestNodeTolerances()
{
    m_surfaceNearestNodeToleranceSquared.clear();
    
    const int32_t numberOfSurfaceFiles = static_cast<int32_t>(m_surfaceFiles.size());
    for (int32_t i = 0; i < numberOfSurfaceFiles; i++) {
        DescriptiveStatistics stats;
        m_surfaceFiles[i]->getNodesSpacingStatistics(stats);
        
        //const float value = 2.0 * stats.getMean() * stats.getMean();
        const float value = 0.01;
        m_surfaceNearestNodeToleranceSquared.push_back(value);
    }
    CaretAssert(m_surfaceFiles.size() == m_surfaceNearestNodeToleranceSquared.size());
}

/**
 * Project to the surface
 * @param spi 
 *    Item that is to be projected.  Its contents will be
 *    updated to reflect the projection.  This items XYZ coordinate
 *    is used for the projection point.
 *
 * @throws SurfaceProjectorException  If projecting an item
 *   failed.
 */
void
SurfaceProjector::projectToSurface(const SurfaceFile* surfaceFile,
                                   const float xyz[3],
                                   SurfaceProjectedItem* spi)
            throw (SurfaceProjectorException)
{
    //
    // If needed, create node locator
    //
    if (surfaceFile->getNumberOfNodes() <= 0) {
        throw SurfaceProjectorException("Surface file contains no nodes: "
                                        + surfaceFile->getFileNameNoPath());
    }
    
    if (surfaceFile->getNumberOfTriangles() <= 0) {
        throw SurfaceProjectorException("Surface topology contains no triangles: "
                                        + surfaceFile->getFileNameNoPath());
    }
    
    m_searchedTriangleFlags.resize(surfaceFile->getNumberOfTriangles(),
                                   false);
    m_sphericalSurfaceRadius = 0.0;
    m_surfaceTypeHint = SURFACE_HINT_THREE_DIMENSIONAL;
    switch (surfaceFile->getSurfaceType()) {
        case SurfaceTypeEnum::FLAT:
            m_surfaceTypeHint = SURFACE_HINT_FLAT;
            break;
        case SurfaceTypeEnum::SPHERICAL:
            m_surfaceTypeHint = SURFACE_HINT_SPHERE;
            break;
        default:
            m_surfaceTypeHint = SURFACE_HINT_THREE_DIMENSIONAL;
            break;
    }
    m_sphericalSurfaceRadius = surfaceFile->getSphericalRadius();
    
    //
    // Default to invalid projection
    //
    SurfaceProjectionBarycentric* baryProj = spi->getBarycentricProjection();
    baryProj->setValid(false);
    SurfaceProjectionVanEssen* vanEssenProj = spi->getVanEssenProjection();
    vanEssenProj->setValid(false);
        
    //
    // Project item
    //
    projectToSurfaceTriangle(surfaceFile,
                     xyz,
                     spi->getBarycentricProjection());
    
    /*
     * Should a van-essen projection be attempted?
     */
    bool tryVanEssenProjection = false;
    if (m_allowEdgeProjection) {
        tryVanEssenProjection = true;
        if (baryProj->isValid()) {
            if (baryProj->isDegenerate() == false) {
                /*
                 * Since barycentric fully successful,
                 * no need to try van essen projection.
                 */
                tryVanEssenProjection = false;
            }
        }
    }
    
    if (tryVanEssenProjection) {
        const int32_t nearestTriangle = findNearestTriangle(surfaceFile,
                                                            xyz);
        if (nearestTriangle >= 0) {
            SurfaceProjectionBarycentric bp;
            m_searchedTriangleFlags[nearestTriangle] = false;
            float savedTol = s_triangleAreaTolerance;
            s_triangleAreaTolerance = -std::numeric_limits<float>::max();
            checkItemInTriangle(surfaceFile, nearestTriangle, xyz, &bp);
            if (bp.isValid()) {
                *baryProj = bp;
                tryVanEssenProjection = false;
            }
            s_triangleAreaTolerance = savedTol;
        }
    }
    
    if (tryVanEssenProjection) {
        const int32_t nearestTriangle = findNearestTriangle(surfaceFile,
                                                            xyz);
        if (nearestTriangle < 0) {
            throw SurfaceProjectorException("Unable to find nearest triangle for VanEssen Projection");
        }
        vanEssenProj->setPosAnatomical(xyz);
        projectWithVanEssenAlgorithm(surfaceFile,
                                     nearestTriangle,
                                     xyz,
                                     vanEssenProj);
        if (vanEssenProj->isValid() == false) {
            if (baryProj->isValid() == false) {
                throw SurfaceProjectorException("Van Essen projection failed.");
            }
        }
    }
    else if (baryProj->isValid() == false) {
        throw SurfaceProjectorException("Barycentric projection failed.");
    }

    if (baryProj->isValid()
        || vanEssenProj->isValid()) {
        spi->setStructure(surfaceFile->getStructure());
    }
}

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

/**
 * Project a coordinate to the surface using a barycentric projection.
 * @param xyz
 *    The coordinate being projected.
 * @param baryProj
 *    The barycentric projection that will be updated.
 * @return The projection status.
 *
 * @throws SurfaceProjectorException  If projecting an item
 *   failed.
 *
 */
void
SurfaceProjector::projectToSurfaceTriangle(const SurfaceFile* surfaceFile,
                                   const float xyz[3],
                                   SurfaceProjectionBarycentric* baryProj)
                                            throw (SurfaceProjectorException)
{
    /*
     * At one time, there was a need to 'perturb' (slightly move) the
     * surface, probably for registration.
     */
    
    
    projectToSurfaceAux(surfaceFile,
                        xyz,
                        baryProj);
    
    if (baryProj->isValid()) {
        if (m_surfaceOffsetValid) {
            baryProj->setSignedDistanceAboveSurface(m_surfaceOffset);
        }
    }
}

/**
 * Project a coordinate to the surface
 * @param xyzIn
 *    The coordinate
 * @param baryProj
 *    The barycentric projection that will be set.
 * @return
 *    The node nearest the coordinate.
 * @throws SurfaceProjectorException
 *    If projecting an item failed.
 */
int32_t
SurfaceProjector::projectToSurfaceAux(const SurfaceFile* surfaceFile,
                                      const float xyzIn[3],
                                      SurfaceProjectionBarycentric* baryProj)
                                             throw (SurfaceProjectorException)
{
    float xyz[3] = { xyzIn[0], xyzIn[1], xyzIn[2] };

    /*
     * If spherical surface, place point on the sphere
     */
    if (m_surfaceTypeHint == SURFACE_HINT_SPHERE) {
        if (m_sphericalSurfaceRadius > 0.0f) {
            MathFunctions::normalizeVector(xyz);
            xyz[0] *= m_sphericalSurfaceRadius;
            xyz[1] *= m_sphericalSurfaceRadius;
            xyz[2] *= m_sphericalSurfaceRadius;
        }
    }
    
    //
    // Initialize projection information
    //
    baryProj->reset();
    
    //
    // Get the nearest node
    //
    int32_t nearestNode = surfaceFile->closestNode(xyz);
    if (nearestNode < 0) {
        throw SurfaceProjectorException("Failed to find nearest node in surface.");
    }
    CaretAssertArrayIndex("nodes", surfaceFile->getNumberOfNodes(), nearestNode);
    
    //
    // Check the triangles used by the nearest node
    //
    findEnclosingTriangle(surfaceFile,
                          nearestNode,
                          xyz,
                          baryProj);
    
    //
    // If not found in triangles used by nearest nodes, check triangles
    // used by neighbors of nearest node
    //
    bool checkOtherTriangles = true;
    if (baryProj->isValid()) {
        if (baryProj->isDegenerate() == false) {
            checkOtherTriangles = false;
        }
    }
    if (checkOtherTriangles) {
        /*
         * Look for non-degenerate point in triangle
         */
        int32_t numberOfNeighbors = 0;
        CaretPointer<TopologyHelper> topoHelper = surfaceFile->getTopologyHelper();
        const int32_t* nodeNeighbors = topoHelper->getNodeNeighbors(nearestNode, numberOfNeighbors);
        for (int32_t i = 0; i < numberOfNeighbors; i++) {
            const int32_t node = nodeNeighbors[i];
            findEnclosingTriangle(surfaceFile, node, xyz, baryProj);
            if (baryProj->isValid()) {
                if (baryProj->isDegenerate() == false) {
                    break;
                }
            }
        }
    }
    
    /*
     * If still not found, see if "on" the nearest node
     */
    bool checkOnNodes = true;
    if (baryProj->isValid()) {
        if (baryProj->isDegenerate() == false) {
            checkOnNodes = false;
        }
    }
    if (checkOnNodes) {
        float distanceSquared =
        MathFunctions::distanceSquared3D(xyz,
                                        surfaceFile->getCoordinate(nearestNode));
        if (distanceSquared <= m_nearestNodeToleranceSquared) {
            baryProj->setValid(true);
            baryProj->setDegenerate(false);
            const float areas[3] = { 1.0, 0.0, 0.0 };
            baryProj->setTriangleAreas(areas);
            const int32_t nodes[3] = { nearestNode, nearestNode, nearestNode };
            baryProj->setTriangleNodes(nodes);
        }
        else {
            CaretLogInfo("Nearest node tolerance="
                         + AString::number(std::sqrt(m_nearestNodeToleranceSquared))
                                         + " failed for distance="
                         + AString::number(std::sqrt(distanceSquared)));
        }
    }
    
//    if (this.barycentricProjectionInfo.getProjectionStatus() ==
//        SurfaceProjectorBarycentricInformation.ProjectionStatus.NOT_FOUND) {
//        // do nothing
//    }
//    else {
//        this.barycentricProjectionInfo.setStructure(
//                                                    this.surface.getStructure());
//    }
    
    return nearestNode;
}

/**
 * Check the triangles used by this node see if the coordinate
 * is contained in any of the triangles.
 * 
 * @param nearestNode - Node of the triangles.
 * @param xyz - coordinate that is being projected.
 * @return 
 *     Index of enclosing triangle or -1 if none found.
 *
 */
void
SurfaceProjector::findEnclosingTriangle(const SurfaceFile* surfaceFile,
                                        const int32_t nearestNode,
                                        const float xyz[3],
                                        SurfaceProjectionBarycentric* baryProj)
{
    //
    // Examine the triangles used by the node
    //
    CaretPointer<TopologyHelper> topoHelper = surfaceFile->getTopologyHelper();
    int32_t numTriangles = 0;
    const int32_t* trianglesArray = topoHelper->getNodeTiles(nearestNode, numTriangles);
    for (int j = 0; j < numTriangles; j++) {
        int triangle = trianglesArray[j];
        /*
         * Was it in a triangle AND not degenerate?
         * Otherwise, keep looking.
         */
        checkItemInTriangle(surfaceFile,
                            triangle,
                            xyz,
                            baryProj);
        
        if (baryProj->isValid()) {
            if (baryProj->isDegenerate() == false) {
                /*
                 * If not degenerate inside triangle, then done.
                 * Else keep looking for non-degenerate.
                 */
                return;
            }
        }
    }
}

/**
 * See if the coordinate is within the triangle.
 * @param triangleNumber
 *    Triangle to check.
 * @param xyz
 *    The coordinate
 * @param spb
 *    Barycentric projection into triangle.
 * @return
 *    true if within triangle.
 */
void
SurfaceProjector::checkItemInTriangle(const SurfaceFile* surfaceFile,
                                      const int32_t triangleNumber,
                                      const float xyz[3],
                                      SurfaceProjectionBarycentric* baryProj)
{
    //
    // Triangle already examined?
    //
    CaretAssertVectorIndex(m_searchedTriangleFlags, triangleNumber);
    if (m_searchedTriangleFlags[triangleNumber]) {
        return;
    }
    m_searchedTriangleFlags[triangleNumber] = true;
    
    //
    // Vertices of the triangle
    //
    const int32_t* tn = surfaceFile->getTriangle(triangleNumber);
    const float* v1 = surfaceFile->getCoordinate(tn[0]);
    const float* v2 = surfaceFile->getCoordinate(tn[1]);
    const float* v3 = surfaceFile->getCoordinate(tn[2]);
    
    //
    // coordinate that may be pushed to a plane depending upon surfac type
    //
    float queryXYZ[3] = {
        xyz[0],
        xyz[1],
        xyz[2]
    };
    
    //
    // Initialize normal vector to normal of triangle
    //
    float normal[3];
    MathFunctions::normalVector(v1, v2, v3, normal);
    
    //
    // Adjust the query coordinate based upon the surface type
    //
    switch (m_surfaceTypeHint) {
        case SURFACE_HINT_FLAT:
            //
            // Override normal with flat surface normal
            //
            normal[0] = 0.0f;
            normal[1] = 0.0f;
            normal[2] = 1.0f;
            queryXYZ[2] = 0.0f; // place on plane
            break;
        case SURFACE_HINT_SPHERE:
        {
            float origin[3] = { 0.0f, 0.0f, 0.0f };
            float xyzDistance[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
            if (MathFunctions::rayIntersectPlane(v1, v2, v3,
                                                origin, queryXYZ,
                                                xyzDistance) == false) {
                //
                // Ray does not intersect, must be parallel to plane
                //
                return;
            }
            
            //
            // Use intersection point
            //
            queryXYZ[0] = xyzDistance[0];
            queryXYZ[1] = xyzDistance[1];
            queryXYZ[2] = xyzDistance[2];
        }
            break;
        case SURFACE_HINT_THREE_DIMENSIONAL:
        {
            //
            // Project point to the triangle
            //
            float xyzOnPlane[3];
            MathFunctions::projectPoint(queryXYZ, v1, normal, xyzOnPlane);
            queryXYZ[0] = xyzOnPlane[0];
            queryXYZ[1] = xyzOnPlane[1];
            queryXYZ[2] = xyzOnPlane[2];
        }
            break;
    }
    
    //
    // Note that if tolerance is a small negative number (which is done to handle
    // degenerate cases - projected point on vertex or edge of triangle) an area may
    // be negative and we continue searching tiles.  If all areas are positive
    // then there is no need to continue searching.
    //
    float areas[3] = { 0.0f, 0.0f, 0.0f };
    int result = triangleAreas(v1, v2, v3, normal, queryXYZ, areas);
    if (result != 0) {
        baryProj->setValid(true);
        if (result < 0) {
            baryProj->setDegenerate(true);
        }
        float signedDistanceToTriangle =
        MathFunctions::signedDistanceFromPlane(normal, v1, xyz);
         
        baryProj->setTriangleAreas(areas);
        baryProj->setTriangleNodes(tn);
        baryProj->setSignedDistanceAboveSurface(signedDistanceToTriangle);
        baryProj->setProjectionSurfaceNumberOfNodes(surfaceFile->getNumberOfNodes());
    }
}

/**
 * Compute the signed areas formed by assuming "xyz" is contained in the triangle formed
 * by the points "p1, p2, p3".  "area2" and "area3" may not be set if "xyz" is not
 * within the triangle.
 *
 * @param p1
 *    Coordinate of triangle node1.
 * @param p2
 *    Coordinate of triangle node2.
 * @param p3
 *    Coordinate of triangle node3.
 * @param normal
 *    Triangle's normal vector.
 * @param xyz
 *    The coordinate being examined.
 * @param areasOut
 *    Output barycentric areas of xyz in the triangle OUTPUT.
 * @return
 *    Returns 1 if all areas are positive (point32_t inside the triangle).
 *    Returns -1 if all areas are greater than the tolerance
 *        (point32_t may be on edge or vertex)
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
                   float areasOut[3])
{
    float area1 = 0.0f;
    float area2 = 0.0f;
    float area3 = 0.0f;
    int32_t result = 0;
    float triangleArea = 0.0f;
    bool inside = false;
    
    switch (m_surfaceTypeHint) {
        case SURFACE_HINT_FLAT:
            //            area1 = MathFunctions::triangleAreaSigned2D(p1, p2, xyz);
            //            if (area1 > triangleAreaTolerance) {
            //               area2 = MathFunctions::triangleAreaSigned2D(p2, p3, xyz);
            //               if (area2 > triangleAreaTolerance) {
            //                  area3 = MathFunctions::triangleAreaSigned2D(p3, p1, xyz);
            //                  if (area3 > triangleAreaTolerance) {
            //                     inside = true;
            //                     triangleArea = MathFunctions::triangleAreaSigned2D(p1,
            //                                                                       p2,
            //                                                                       p3);
            //                  }
            //               }
            //            }
            area1 = MathFunctions::triangleAreaSigned2D(p2, p3, xyz);
            if (area1 > s_triangleAreaTolerance) {
                area2 = MathFunctions::triangleAreaSigned2D(p3, p1, xyz);
                if (area2 > s_triangleAreaTolerance) {
                    area3 = MathFunctions::triangleAreaSigned2D(p1, p2, xyz);
                    if (area3 > s_triangleAreaTolerance) {
                        inside = true;
                        triangleArea = MathFunctions::triangleAreaSigned2D(p1,
                                                                          p2,
                                                                          p3);
                    }
                }
            }
            break;
        case SURFACE_HINT_SPHERE:
        case SURFACE_HINT_THREE_DIMENSIONAL:
            /*
             area1 = MathFunctions::triangleAreaSigned3D(normal, p1, p2, xyz);
             if (area1 >= triangleAreaTolerance) {
             area2 = MathFunctions::triangleAreaSigned3D(normal, p2, p3, xyz);
             if (area2 >= triangleAreaTolerance) {
             area3 = MathFunctions::triangleAreaSigned3D(normal, p3,p1,xyz);
             if (area3 >= triangleAreaTolerance) {
             inside = true;
             triangleArea = MathFunctions::triangleArea(p1, p2, p3);
             }
             }
             }
             */
            area1 = MathFunctions::triangleAreaSigned3D(normal, p2, p3, xyz);
            if (area1 >= s_triangleAreaTolerance) {
                area2 = MathFunctions::triangleAreaSigned3D(normal, p3, p1, xyz);
                if (area2 >= s_triangleAreaTolerance) {
                    area3 = MathFunctions::triangleAreaSigned3D(normal, p1,p2,xyz);
                    if (area3 >= s_triangleAreaTolerance) {
                        inside = true;
                        triangleArea = MathFunctions::triangleArea(p1, p2, p3);
                    }
                }
            }
            break;
    }
    
    if (inside) {
        if ((area1 > 0.0) && (area2 > 0.0) && (area3 > 0.0)) {
            result = 1;
        }
        else {
            result = -1;
        }
//        if (area1 < 0.0) area1 = -area1;
//        if (area2 < 0.0) area2 = -area2;
//        if (area3 < 0.0) area3 = -area3;
        
        if (triangleArea > 0.0) {
            //area1 /= triangleArea;
            //area2 /= triangleArea;
            //area3 /= triangleArea;
        }
        else {
            area1 = 1.0f;
            area2 = 0.0f;
            area3 = 0.0f;
        }
    }
    
    areasOut[0] = area1;
    areasOut[1] = area2;
    areasOut[2] = area3;
    
    return result;
}

/**
 * Perform a VanEssen Projection that projects to the edge of two triangles.
 *
 * @param nearestTriangleIn 
 *    Nearest triangle from barycentric
 *    projection attempt, may be invalid (< 0).
 * @param xyzIn 
 *    Location of item for projection.
 * @param spve
 *    The Van Essen Projection that is setup.
 * @throws SurfaceProjectorException  If projection failure.
 *
 */
void
SurfaceProjector::projectWithVanEssenAlgorithm(const SurfaceFile* surfaceFile,
                                               const int32_t nearestTriangleIn,
                                               const float xyzIn[3],
                                               SurfaceProjectionVanEssen* spve)
            throw (SurfaceProjectorException)
{
    float xyz[3] = {
        xyzIn[0],
        xyzIn[1],
        xyzIn[2]
    };
    
    //
    // Find nearest triangle to coordinate
    //
    int32_t nearestTriangle = nearestTriangleIn;
    if (nearestTriangle < 0) {
        nearestTriangle = findNearestTriangle(surfaceFile,
                                              xyz);
    }
    if (nearestTriangle < 0) {
        throw new SurfaceProjectorException(
                                            "Unable to find nearest triangle for VanEssen projection.");
    }
    
    //
    // Get triangle nodes and their coordinates
    //
    const int32_t* tn = surfaceFile->getTriangle(nearestTriangle);
    int32_t n1 = tn[0];
    int32_t n2 = tn[1];
    int32_t n3 = tn[2];
    const float* p1 = surfaceFile->getCoordinate(n1);
    const float* p2 = surfaceFile->getCoordinate(n2);
    const float* p3 = surfaceFile->getCoordinate(n3);
    
    //
    // Project the coordinate to the plane of nearest triangle
    //
    float planeNormal[3];
    MathFunctions::normalVector(p1, p2, p3, planeNormal);
    float xyzOnPlane[3];
    MathFunctions::projectPoint(xyz, p1, planeNormal, xyzOnPlane);
    
    //
    // Adjust for surface offset
    //
    if (m_surfaceOffsetValid) {
        for (int32_t i = 0; i < 3; i++) {
            xyz[i] = xyzOnPlane[i] + planeNormal[i] * m_surfaceOffset;
        }
    }
    
    //
    // Find edge of triangle closest to coordinate projected to triangle
    //
    float dist1 = MathFunctions::distanceToLine3D(p1, p2, xyz);
    float dist2 = MathFunctions::distanceToLine3D(p2, p3, xyz);
    float dist3 = MathFunctions::distanceToLine3D(p3, p1, xyz);
    int32_t closestVertices[2] = { -1, -1 };
    if ((dist1 < dist2) &&
        (dist1 < dist3)) {
        closestVertices[0] = n1;
        closestVertices[1] = n2;
    }
    else if ((dist2 < dist1) &&
             (dist2 < dist3)) {
        closestVertices[0] = n2;
        closestVertices[1] = n3;
    }
    else {
        closestVertices[0] = n3;
        closestVertices[1] = n1;
    }
    
    int32_t iR = closestVertices[0];
    int32_t jR = closestVertices[1];
    int32_t triA = nearestTriangle;
    int32_t triB = surfaceFile->getTriangleThatSharesEdge(iR, jR, triA);
    
    const float* coordJR = surfaceFile->getCoordinate(jR);
    const float* coordIR = surfaceFile->getCoordinate(iR);
    
    float normalA[3];
    surfaceFile->getTriangleNormalVector(triA, normalA);
    
    //
    // Second triangle might not be found if topology is open or cut.
    //
    float normalB[3] = { 0.0f, 0.0f, 0.0f };
    if (triB >= 0) {
        surfaceFile->getTriangleNormalVector(triB, normalB);
    }
    else {
        float dR =
        (float)std::sqrt(MathFunctions::distance3D(xyzOnPlane, xyz));
        
        float v[3];
        MathFunctions::subtractVectors(coordJR, coordIR, v);
        
        float t1[3];
        MathFunctions::subtractVectors(xyz, coordIR, t1);
        float t2 = MathFunctions::dotProduct(v, v);
        float t3 = MathFunctions::dotProduct(t1, v);
        
        float QR[3] = { 0.0f, 0.0f, 0.0f };
        for (int32_t j = 0; j < 3; j++) {
            QR[j] = coordIR[j] + ((t3/t2) * v[j]);
        }
        MathFunctions::subtractVectors(coordJR, coordIR, v);
        t2 = MathFunctions::vectorLength(v);
        MathFunctions::subtractVectors(QR, coordIR, t1);
        t3 = MathFunctions::vectorLength(t1);
        float fracRI = 0.0f;
        if (t2 > 0.0f) {
            fracRI = t3/t2;
        }
        
        MathFunctions::subtractVectors(coordIR, coordJR, v);
        t2 = MathFunctions::vectorLength(v);
        MathFunctions::subtractVectors(QR, coordJR, t1);
        t3 = MathFunctions::vectorLength(t1);
        float fracRJ = 0.0f;
        if (t2 > 0.0f) {
            fracRJ = t3/t2;
        }
        else {
            fracRI = 0.0f;  // uses fracRI seems wrong but like this in OLD code
        }
        
        if (fracRI > 1.0f) {
            for (int32_t j = 0; j < 3; j++) {
                QR[j] = coordJR[j];
            }
        }
        if (fracRJ > 1.0f) {
            for (int32_t j = 0; j < 3; j++) {
                QR[j] = coordIR[j];
            }
        }
        
        MathFunctions::subtractVectors(xyz, xyzOnPlane, t1);
        t2 = MathFunctions::vectorLength(t1);
        if (t2 > 0.0f) {
            for (int32_t j = 0; j < 3; j++) {
                t1[j] = t1[j]/t2;
            }
        }
        t3 = MathFunctions::dotProduct(t1, normalA);
        for (int32_t j = 0; j < 3; j++) {
            xyz[j] = QR[j] + (dR * t3 * normalA[j]);
        }
    }
    
    float v[3];
    MathFunctions::subtractVectors(coordJR, coordIR, v);
    float t1[3];
    MathFunctions::subtractVectors(xyz, coordIR, t1);
    float t2 = MathFunctions::dotProduct(v, v);
    float t3 = MathFunctions::dotProduct(t1, v);
    float QR[3] = { 0.0f, 0.0f, 0.0f };
    for (int32_t j = 0; j < 3; j++) {
        QR[j] = coordIR[j] + ((t3/t2) * v[j]);
    }
    
    if ((triA >= 0) && (triB >= 0)) {
        t2 = MathFunctions::dotProduct(normalA, normalB);
        t2 = std::min(t2, 1.0f);
        spve->setPhiR((float)std::acos(t2));
    }
    else {
        spve->setPhiR(0.0f);
    }
    
    MathFunctions::subtractVectors(xyz, QR, t1);
    t2 = MathFunctions::vectorLength(t1);
    if (t2 > 0.0f) {
        for (int32_t j = 0; j < 3; j++) {
            t1[j] = t1[j] / t2;
        }
    }
    t3 = MathFunctions::dotProduct(normalA, t1);
    if (t3 > 0.0f) {
        spve->setThetaR((float)std::acos(t3 * (t3/std::fabs(t3))));
    }
    else {
        spve->setThetaR(0.0f);
    }
    
    MathFunctions::subtractVectors(coordJR, coordIR, v);
    t2 = MathFunctions::vectorLength(v);
    MathFunctions::subtractVectors(QR, coordIR, t1);
    t3 = MathFunctions::vectorLength(t1);
    if (t2 > 0.0f) {
        spve->setFracRI(t3/t2);
    }
    else {
        spve->setFracRI(0.0f);
    }
    
    MathFunctions::subtractVectors(coordIR, coordJR, v);
    t2 = MathFunctions::vectorLength(v);
    MathFunctions::subtractVectors(QR, coordJR, t1);
    t3 = MathFunctions::vectorLength(t1);
    if (t2 > 0.0f) {
        spve->setFracRJ(t3/t2);
    }
    else {
        spve->setFracRJ(0.0f);
    }
    
    spve->setDR(MathFunctions::distance3D(QR, xyz));
    const int32_t* triANodes = surfaceFile->getTriangle(triA);
    int32_t nodesA[3] = {
        triANodes[0],
        triANodes[1],
        triANodes[2]
    };
    int32_t swapA = nodesA[0];
    nodesA[0] = nodesA[2];
    nodesA[2] = swapA;
    spve->setTriVertices(0, nodesA);
    spve->setTriAnatomical(0,0,surfaceFile->getCoordinate(nodesA[0]));
    spve->setTriAnatomical(0,1,surfaceFile->getCoordinate(nodesA[1]));
    spve->setTriAnatomical(0,2,surfaceFile->getCoordinate(nodesA[2]));
    
    
    if (triB >= 0) {
        const int32_t* triBNodes = surfaceFile->getTriangle(triB);
        int32_t nodesB[3] = {
            triBNodes[0],
            triBNodes[1],
            triBNodes[2]
        };
        int32_t swapB = nodesB[0];
        nodesB[0] = nodesB[2];
        nodesB[2] = swapB;
        spve->setTriVertices(1, nodesB);
        spve->setTriAnatomical(1,0,surfaceFile->getCoordinate(nodesB[0]));
        spve->setTriAnatomical(1,1,surfaceFile->getCoordinate(nodesB[1]));
        spve->setTriAnatomical(1,2,surfaceFile->getCoordinate(nodesB[2]));
    }
    else {
        int32_t intZeros[3] = { 0, 0, 0 };
        spve->setTriVertices(1, intZeros);
        float zeros[3] = { 0.0f, 0.0f, 0.0f };
        spve->setTriAnatomical(1, 0, zeros);
        spve->setTriAnatomical(1, 1, zeros);
        spve->setTriAnatomical(1, 2, zeros);
    }
    
    spve->setVertexAnatomical(0, coordIR);
    spve->setVertexAnatomical(1, coordJR);
    
    spve->setVertex(0, iR);
    spve->setVertex(1, jR);

    spve->setProjectionSurfaceNumberOfNodes(surfaceFile->getNumberOfNodes());
    spve->setValid(true);
}

/**
 * Find the triangle nearest the coordinate.
 *
 * @param xyz
 *    The coordinate
 * @return
 *    Index of triangle nearest coordinate.
 *    Returns -1 if none found.
 */
int32_t
SurfaceProjector::findNearestTriangle(const SurfaceFile* surfaceFile,
                                      const float xyz[3])
{
    //
    // Get nearest node
    //
    int32_t nearestNode = surfaceFile->closestNode(xyz);
    if (nearestNode < 0) {
        return -1;
    }
    CaretAssertArrayIndex("nodes", surfaceFile->getNumberOfNodes(), nearestNode);
    
    
    int32_t nearestTriangle = -1;
    float nearestTriangleDistance = std::numeric_limits<float>::max();
    
    //
    // Search triangles of nearest node
    //
    CaretPointer<TopologyHelper> topoHelp = surfaceFile->getTopologyHelper();
    const std::vector<int32_t> triangles = topoHelp->getNodeTiles(nearestNode);
    int32_t numT = triangles.size();
    for (int32_t j = 0; j < numT; j++) {
        int32_t triangleNumber = triangles[j];
        //
        // Get triangle and its nodes and their coordinates
        //
        const int32_t* tn = surfaceFile->getTriangle(triangleNumber);
        const float* p1 = surfaceFile->getCoordinate(tn[0]);
        const float* p2 = surfaceFile->getCoordinate(tn[1]);
        const float* p3 = surfaceFile->getCoordinate(tn[2]);
        
        //
        // Initialize normal vector to normal of triangle
        //
        float triangleNormal[3];
        MathFunctions::normalVector(p1, p2, p3, triangleNormal);
        
        //
        // Find distance from coordinate to triangle
        //
        float distanceToTriangle = std::fabs(MathFunctions::signedDistanceFromPlane(triangleNormal, p1, xyz));
        
        //
        // Is closer?
        //
        if (distanceToTriangle < nearestTriangleDistance) {
            nearestTriangle = triangleNumber;
            nearestTriangleDistance = distanceToTriangle;
        }
    }
    
    return nearestTriangle;
}

