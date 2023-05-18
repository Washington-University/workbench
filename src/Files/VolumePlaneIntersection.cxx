
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
        m_constructorErrorMessage = "Volume is invalid (NULL)";
        return;
    }
    
    std::vector<int64_t> dims;
    m_volume->getDimensions(dims);
    if (dims.size() < 3) {
        m_constructorErrorMessage = "Volume dimensions are less than three";
        return;
    }
    if ((dims[0] < 2) || (dims[1] < 2) || (dims[2] < 2)) {
        m_constructorErrorMessage = "At least one dimension is less than two";
        return;
    }
    
    const float maxI(dims[0] - 1);
    const float maxJ(dims[1] - 1);
    const float maxK(dims[2] - 1);
    
    /*
     * Coordinates at volume's 8 corners
     */
    Vector3D xyz000(m_volume->indexToSpace( 0.0,  0.0, 0.0));
    Vector3D xyzI00(m_volume->indexToSpace(maxI,  0.0, 0.0));
    Vector3D xyzIJ0(m_volume->indexToSpace(maxI, maxJ, 0.0));
    Vector3D xyz0J0(m_volume->indexToSpace( 0.0, maxJ, 0.0));
    
    Vector3D xyz00K(m_volume->indexToSpace( 0.0,  0.0, maxK));
    Vector3D xyzI0K(m_volume->indexToSpace(maxI,  0.0, maxK));
    Vector3D xyzIJK(m_volume->indexToSpace(maxI, maxJ, maxK));
    Vector3D xyz0JK(m_volume->indexToSpace( 0.0, maxJ, maxK));
    
    /*
     * Transform the coordinates at the volume's corners
     */
    matrix.multiplyPoint3(xyz000);
    matrix.multiplyPoint3(xyzI00);
    matrix.multiplyPoint3(xyzIJ0);
    matrix.multiplyPoint3(xyz0J0);
    
    matrix.multiplyPoint3(xyz00K);
    matrix.multiplyPoint3(xyzI0K);
    matrix.multiplyPoint3(xyzIJK);
    matrix.multiplyPoint3(xyz0JK);
    
    /*
     * Line segments for the 12 edges
     */
    m_edges.emplace_back(xyz000, xyzI00);
    m_edges.emplace_back(xyzI00, xyzIJ0);
    m_edges.emplace_back(xyzIJ0, xyz0J0);
    m_edges.emplace_back(xyz0J0, xyz000);
    
    m_edges.emplace_back(xyz00K, xyzI0K);
    m_edges.emplace_back(xyzI0K, xyzIJK);
    m_edges.emplace_back(xyzIJK, xyz0JK);
    m_edges.emplace_back(xyz0JK, xyz00K);
    
    m_edges.emplace_back(xyz000, xyz00K);
    m_edges.emplace_back(xyzI00, xyzI0K);
    m_edges.emplace_back(xyzIJ0, xyzIJK);
    m_edges.emplace_back(xyz0J0, xyz0JK);
    
    CaretAssert(m_edges.size() == 12);
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

    if ( ! m_constructorErrorMessage.isEmpty()) {
        errorMessageOut = m_constructorErrorMessage;
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
    
    return true;
}

/**
 * Sort the intersection points so that they are in a counter-clockwise order
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




