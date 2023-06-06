
/*LICENSE_START*/
/*
 *  Copyright (C) 2023 Washington University School of Medicine
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

#define __VOLUME_PLANE_INTERSECTION_DECLARE__
#include "VolumePlaneIntersection.h"
#undef __VOLUME_PLANE_INTERSECTION_DECLARE__

#include <cmath>
#include <map>

#include "CaretAssert.h"
#include "CaretLogger.h"
#include "MathFunctions.h"
#include "Plane.h"
#include "VolumeMappableInterface.h"

using namespace caret;


    
/**
 * \class caret::VolumePlaneIntersection 
 * \brief Find intersections of a plane and a volume
 * \ingroup Files
 */

/**
 * Constructor.
 * @param volume
 *    The volume
 */
VolumePlaneIntersection::VolumePlaneIntersection(const VolumeMappableInterface* volume)
: VolumePlaneIntersection(volume,
                          Matrix4x4())
{
}

/**
 * Constructor with matrix that transforms volume's coordinates
 * @param volume
 *    The volume
 * @param matrix
 *    Matrix that transforms the volume's coordinates
 */
VolumePlaneIntersection::VolumePlaneIntersection(const VolumeMappableInterface* volume,
                                                 const Matrix4x4& matrix)
: CaretObject(),
m_volume(volume),
m_matrix(matrix)
{

    CaretAssert(volume);
    if (volume == NULL) {
        CaretLogSevere("Volume is invalid (NULL)");
        return;
    }
    
    std::vector<int64_t> dims;
    m_volume->getDimensions(dims);
    if (dims.size() < 3) {
        CaretLogSevere("Volume dimensions are less than three");
        return;
    }
    if ((dims[0] < 2) || (dims[1] < 2) || (dims[2] < 2)) {
        CaretLogSevere("At least one dimension is less than two");
        return;
    }
    
    m_validFlag = true;
    
    const float maxI(dims[0] - 1);
    const float maxJ(dims[1] - 1);
    const float maxK(dims[2] - 1);
    
    /*
     * Coordinates at volume's 8 corners
     */
    m_xyz000 = m_volume->indexToSpace( 0.0,  0.0, 0.0);
    m_xyzI00 = m_volume->indexToSpace(maxI,  0.0, 0.0);
    m_xyzIJ0 = m_volume->indexToSpace(maxI, maxJ, 0.0);
    m_xyz0J0 = m_volume->indexToSpace( 0.0, maxJ, 0.0);
    
    m_xyz00K = m_volume->indexToSpace( 0.0,  0.0, maxK);
    m_xyzI0K = m_volume->indexToSpace(maxI,  0.0, maxK);
    m_xyzIJK = m_volume->indexToSpace(maxI, maxJ, maxK);
    m_xyz0JK = m_volume->indexToSpace( 0.0, maxJ, maxK);
    
    /*
     * Transform the coordinates at the volume's corners
     */
    matrix.multiplyPoint3(m_xyz000);
    matrix.multiplyPoint3(m_xyzI00);
    matrix.multiplyPoint3(m_xyzIJ0);
    matrix.multiplyPoint3(m_xyz0J0);
    
    matrix.multiplyPoint3(m_xyz00K);
    matrix.multiplyPoint3(m_xyzI0K);
    matrix.multiplyPoint3(m_xyzIJK);
    matrix.multiplyPoint3(m_xyz0JK);
    
    /*
     * Line segments for the 12 edges
     */
    m_edges.emplace_back(m_xyz000, m_xyzI00);
    m_edges.emplace_back(m_xyzI00, m_xyzIJ0);
    m_edges.emplace_back(m_xyzIJ0, m_xyz0J0);
    m_edges.emplace_back(m_xyz0J0, m_xyz000);
    
    m_edges.emplace_back(m_xyz00K, m_xyzI0K);
    m_edges.emplace_back(m_xyzI0K, m_xyzIJK);
    m_edges.emplace_back(m_xyzIJK, m_xyz0JK);
    m_edges.emplace_back(m_xyz0JK, m_xyz00K);
    
    m_edges.emplace_back(m_xyz000, m_xyz00K);
    m_edges.emplace_back(m_xyzI00, m_xyzI0K);
    m_edges.emplace_back(m_xyzIJ0, m_xyzIJK);
    m_edges.emplace_back(m_xyz0J0, m_xyz0JK);
    
    CaretAssert(m_edges.size() == 12);
}

/**
 * Create the faces of the volume
 */
void
VolumePlaneIntersection::createFaces() const
{
    if (m_facesValidFlag) {
        return;
    }
    m_facesValidFlag = true;
        
    /* Bottom */
    m_faces.emplace_back(m_xyz000, m_xyzI00, m_xyzIJ0, m_xyz0J0);
    
    /* Top */
    m_faces.emplace_back(m_xyz00K, m_xyzI0K, m_xyzIJK, m_xyz0JK);
    
    /* Left */
    m_faces.emplace_back(m_xyz000, m_xyz00K, m_xyz0JK, m_xyz0J0);
    
    /* Right */
    m_faces.emplace_back(m_xyzI00, m_xyzI0K, m_xyzIJK, m_xyzIJ0);
    
    /* Near */
    m_faces.emplace_back(m_xyz000, m_xyzI00, m_xyzI0K, m_xyz00K);
    
    /* Far */
    m_faces.emplace_back(m_xyz0J0, m_xyzIJ0, m_xyzIJK, m_xyz0JK);
    
    if (s_debugFlag) {
        for (auto& f : m_faces) {
            std::cout << "Face: " << f.m_v1.toString() << std::endl;
            std::cout << "      " << f.m_v2.toString() << std::endl;
            std::cout << "      " << f.m_v3.toString() << std::endl;
            std::cout << "      " << f.m_v4.toString() << std::endl;
        }
        std::cout << std::endl;
        std::cout << std::endl;
    }

    CaretAssert(m_faces.size() == 6);
}
/**
 * Destructor.
 */
VolumePlaneIntersection::~VolumePlaneIntersection()
{
}

/**
 * Intersect the plane with the volume
 * @param plane
 *    The plane
 * @param centerOut
 *    Contains point around which points were sorted (can be used for OpenGL Triangle Fan)
 * @param intersectionPointsOut
 *    Contains all intersection points that were found
 * @param errorMessageOut
 *    Contains message if error
 * @return True if intersection is successful, else false.
 */
bool
VolumePlaneIntersection::intersectWithPlane(const Plane& plane,
                                            Vector3D& centerOut,
                                            std::vector<Vector3D>& intersectionPointsOut,
                                            AString& errorMessageOut) const
{
    centerOut.fill(0.0);
    intersectionPointsOut.clear();
    errorMessageOut.clear();

    if ( ! m_validFlag) {
        return false;
    }
    
    if ( ! plane.isValidPlane()) {
        errorMessageOut = "Plane is invalid";
        return false;
    }
    
    /*
     * Test for intersection of plane with each
     * of the volume's edges
     */
    for (const auto& lineSegment : m_edges) {
        Vector3D intersectionXYZ;
        if (plane.lineSegmentIntersectPlane(lineSegment.v1(),
                                            lineSegment.v2(),
                                            intersectionXYZ)) {
            intersectionPointsOut.push_back(intersectionXYZ);
        }
    }
    
    sortIntersectionPoints(plane,
                           centerOut,
                           intersectionPointsOut);
    
    orientIntersectionPoints(plane,
                             centerOut,
                             intersectionPointsOut);
    
    return true;
}

/**
 * Sort the intersection points around the center
 * @param plane
 *    The plane
 * @param centerOut
 *    Output containing the center of gravity of the intersection points
 * @param intersectionPoints
 *    Input/output Intersection points that are sorted around the center
 */
void
VolumePlaneIntersection::sortIntersectionPoints(const Plane& plane,
                                                Vector3D& centerOut,
                                                std::vector<Vector3D>& intersectionPoints) const
{
    const int32_t numPoints(intersectionPoints.size());
    if (numPoints < 1) {
        return;
    }
    
    for (int32_t i = 0; i < numPoints; i++) {
        CaretAssertVectorIndex(intersectionPoints, i);
        centerOut += intersectionPoints[i];
    }
    CaretAssert(numPoints >= 1);
    centerOut /= static_cast<float>(numPoints);

    if (numPoints <= 2) {
        return;
    }
    
    /*
     * Use a map (angle, point) for sorting
     */
    std::map<float, Vector3D> pointsSorted;

    Vector3D normalVector;
    plane.getNormalVector(normalVector);
    
    CaretAssertVectorIndex(intersectionPoints, 0);
    const Vector3D referenceVector((intersectionPoints[0] - centerOut).normal());
    for (auto& p : intersectionPoints) {
        const Vector3D vec((p - centerOut).normal());
        const float angle(referenceVector.signedAngleRadians(vec,
                                                             normalVector));
        pointsSorted.insert(std::make_pair(angle, p));
    }
    
    intersectionPoints.clear();
    for (auto& p : pointsSorted) {
        intersectionPoints.push_back(p.second);
    }
    
    CaretAssert(numPoints == static_cast<int32_t>(intersectionPoints.size()));
}

/**
 * Orient the intersection points so that they are in a counter-clockwise order
 * about the center
 * @param plane
 *    The plane
 * @param centerOut
 *    Center of gravity of the intersection points
 * @param intersectionPoints
 *    Input/output Intersection points that are oriented counter-clockwise
 */
void
VolumePlaneIntersection::orientIntersectionPoints(const Plane& plane,
                                                  const Vector3D& center,
                                                  std::vector<Vector3D>& intersectionPoints) const
{
    const int32_t numPoints(intersectionPoints.size());
    if (numPoints < 2) {
        return;
    }
    
    CaretAssertVectorIndex(intersectionPoints, 1);
    const Vector3D p1(intersectionPoints[0]);
    const Vector3D p2(intersectionPoints[1]);

    Vector3D triangleNormalVector;
    MathFunctions::normalVector(center, p1, p2, triangleNormalVector);

    Vector3D planeNormalVector;
    plane.getNormalVector(planeNormalVector);
    
    float dotProduct(triangleNormalVector.dot(planeNormalVector));
    
    /*
     * The plane vector and vector from first triangle
     * should either be the same (1.0) or opposite (-1.0).
     * If not, something weird has happened.
     */
    const float nearOne(0.9);
    if (dotProduct > nearOne) {
        /* OK, vectors are aligned */
    }
    else if (dotProduct < -nearOne) {
        /* Vectors are opposite; triangle orientation needs to be flipped */
        std::reverse(intersectionPoints.begin(),
                     intersectionPoints.end());
        CaretLogWarning("FYI, Reorienting triangles (OK !)");
    }
    else {
        AString pointsStr;
        for (const auto& v : intersectionPoints) {
            pointsStr.append(" " + v.toString());
        }
        const AString msg("Invalid normal vectors when orientation triangles.  Dot="
                          + AString::number(dotProduct)
                          + " plane normal="
                          + planeNormalVector.toString()
                          + " triangle normal="
                          + triangleNormalVector.toString()
                          + " Center="
                          + center.toString()
                          + " Intersections="
                          + pointsStr);
        CaretLogSevere(msg);
        CaretAssertMessage(0, msg);
    }
}

/**
 * Intersect a ray with the volume
 * @param rayOrigin
 *    Origin of the ray
 * @param rayDirectionVector
 *    Direction vector of the ray
 * @param centerOut
 *    Contains point around which points were sorted (can be used for OpenGL Triangle Fan)
 * @param intersectionPointsOut
 *    Contains all intersection points that were found.  These points are ordered by
 *    nearest distance to farest distance from the ray's origin.
 * @param errorMessageOut
 *    Contains message if error
 * @return True if intersection is successful, else false.
 */
bool
VolumePlaneIntersection::intersectWithRay(const Vector3D& rayOriginXYZ,
                                          const Vector3D& rayDirectionVector,
                                          std::vector<Vector3D>& intersectionPointsOut,
                                          AString& errorMessageOut) const
{
    intersectionPointsOut.clear();
    errorMessageOut.clear();

    if ( ! m_validFlag) {
        return false;
    }

    createFaces();
    
    /*
     * Map that sorts intersection by distance from
     * the ray's origin
     */
    std::map<float, Vector3D> distanceIntersectionXYZ;
    
    for (const auto& face : m_faces) {
        Vector3D intersectionXYZ;
        float distanceFromRayOrigin(0.0);
        if (face.m_plane.rayIntersection(rayOriginXYZ,
                                         rayDirectionVector,
                                         intersectionXYZ,
                                         distanceFromRayOrigin)) {
            const float degenerateTolerance(0.0001);
            bool insideFlag(false);
            
            /*
             * Is point in triangle (half of side)?
             * Use barycentric areas.  Orientation of triangle is unknown so if point
             * is inside triangle all areas will be either negative or positive.
             */
            const float area1 = MathFunctions::triangleAreaSigned3D(rayDirectionVector, face.m_v1, face.m_v2, intersectionXYZ);
            const float area2 = MathFunctions::triangleAreaSigned3D(rayDirectionVector, face.m_v2, face.m_v3, intersectionXYZ);
            const float area3 = MathFunctions::triangleAreaSigned3D(rayDirectionVector, face.m_v3, face.m_v1, intersectionXYZ);
            if ((area1 > -degenerateTolerance)
                && (area2 > -degenerateTolerance)
                && (area3 > -degenerateTolerance)) {
                insideFlag = true;
            }
            if ((area1 < degenerateTolerance)
                && (area2 < degenerateTolerance)
                && (area3 < degenerateTolerance)) {
                insideFlag = true;
            }
            if (insideFlag) {
                distanceIntersectionXYZ.insert(std::make_pair(distanceFromRayOrigin,
                                                              intersectionXYZ));
            }
            
            if ( ! insideFlag) {
                const float area4 = MathFunctions::triangleAreaSigned3D(rayDirectionVector, face.m_v1, face.m_v3, intersectionXYZ);
                const float area5 = MathFunctions::triangleAreaSigned3D(rayDirectionVector, face.m_v3, face.m_v4, intersectionXYZ);
                const float area6 = MathFunctions::triangleAreaSigned3D(rayDirectionVector, face.m_v4, face.m_v1, intersectionXYZ);
                if ((area4 > -degenerateTolerance)
                    && (area5 > -degenerateTolerance)
                    && (area6 > -degenerateTolerance)) {
                    insideFlag = true;
                }
                if ((area4 < degenerateTolerance)
                    && (area5 < degenerateTolerance)
                    && (area6 < degenerateTolerance)) {
                    insideFlag = true;
                }
                if (insideFlag) {
                    distanceIntersectionXYZ.insert(std::make_pair(distanceFromRayOrigin,
                                                                  intersectionXYZ));
                }
            }
        }
    }
    
    for (auto& distXYZ: distanceIntersectionXYZ) {
        intersectionPointsOut.push_back(distXYZ.second);
    }
    
    return ( ! intersectionPointsOut.empty());
}


