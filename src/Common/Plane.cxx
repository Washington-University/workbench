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

#include "MathFunctions.h"
#include "Plane.h"


using namespace caret;

/**
 * Constructor.
 * @param p1 Point on plane.
 * @param p2 Point on plane.
 * @param p3 Point on plane.
 *
 */
Plane::Plane(const float p1[3],
             const float p2[3],
             const float p3[3])
    : CaretObject()
{
    this->p1[0] = p1[0];
    this->p1[1] = p1[1];
    this->p1[2] = p1[2];
    this->p2[0] = p2[0];
    this->p2[1] = p2[1];
    this->p2[2] = p2[2];
    this->p3[0] = p3[0];
    this->p3[1] = p3[1];
    this->p3[2] = p3[2];
    
    this->validPlaneFlag =  MathFunctions::normalVector(this->p1, 
                                                        this->p2, 
                                                        this->p3, 
                                                        this->normalVector);
    
    //
    // Compute the plane equation
    //
    this->A = this->normalVector[0];
    this->B = this->normalVector[1];
    this->C = this->normalVector[2];
    this->D = -(A*p1[0] + B*p1[1] + C*p1[2]);
}

/**
 * Destructor
 */
Plane::~Plane()
{
}

/**
 * Is the plane valid?
 * @return  true if plane is valid, else false.
 *
 */
bool
Plane::isValidPlane() const
{
    return this->validPlaneFlag;
}

/**
 * Find the points where a triangle intersects the plane.
 * @param t1  First point in triangle.
 * @param t2  Second point in triangle.
 * @param t3  Third point in triangle.
 * @param intersectionPointOut1 If there is intersection,
 * this will be loaded with one intersection point.
 * @param intersectionPointOut2 If there is intersection,
 * this will be loaded with other intersection point.
 * @return  true if any two edges of the triangle intersect
 * the plane, else false.
 *
 */
bool
Plane::triangleIntersectPlane(
                   const float t1[3],
                   const float t2[3],
                   const float t3[3],
                   float intersectionPointOut1[3],
                   float intersectionPointOut2[3])
{
    float* intersection = intersectionPointOut1;
    int count = 0;
    if (this->lineSegmentIntersectPlane(t1, t2, intersection)) {
        count++;
        intersection = intersectionPointOut2;
    }
    if (this->lineSegmentIntersectPlane(t2, t3, intersection)) {
        count++;
        if (count == 2) {
            return true;
        }
        intersection = intersectionPointOut2;
    }
    if (this->lineSegmentIntersectPlane(t3, t1, intersection)) {
        count++;
        if (count == 2) {
            return true;
        }
    }
    
    return false;
}

/**
 * Get distance of point from the plane.
 * @param p Point.
 * @return  Distance of "p" from plane.
 *
 */
double
Plane::absDistanceToPlane(const float p[3])
{
    double dist = (this->A * p[0] + this->B * p[1] + this->C * p[2] + this->D);
    if (dist < 0.0f) dist = -dist;
    return dist;
}

/**
 * Find the intersection of a line segment with the plane.
 * @param lp1 start of line.
 * @param lp2 end of line.
 * @param intersectionOut If the line intersects the plane,
 * the point of intersection will be loaded into this.  If 
 * there is no intersection, the values of this array will 
 * not be altered.
 * @return  true if the line segment intersects the plane,
 * else false.
 *
 */
bool
Plane::lineSegmentIntersectPlane(
                   const float lp1[3],
                   const float lp2[3],
                   float intersectionOut[3])
{
    /*
     * Ray formed by lp1 ==> lp2 (NOT a unit vector; do not normalize)
     */
    double r0 = lp2[0] - lp1[0];
    double r1 = lp2[1] - lp1[1];
    double r2 = lp2[2] - lp1[2];
    
    /*
     * Denominator is dot product of the plane's normal
     * and the ray.  If it is zero, that means the ray
     * is parallel to the plane.
     * 
     * Perhaps should test for value near zero, and,
     * if it is, see if line end points are very 
     * close to plane, and, if so, use the line
     * endpoints, projected to the plane, as the
     * intersection.
     */
    double denominator = 
    (this->A * r0)
    + (this->B * r1) 
    + (this->C * r2);
    if (denominator == 0.0) {
        return false;
    }
    
    /*
     * "t" is the normalized distance of plane from the line origin to
     * the endpoint of the line.  
     * 
     * (0 <= t <= 1): The line intersects the plane with one point
     * above the plane and one point below the plane.
     * 
     * (t > 1): Vector (P1, P2) point towards plane but both are
     * on same side of the plane.
     * 
     * (t < 0): Vector (P1, P2) point away from plane but both are
     * on same side of the plane. 
     */
    double numerator = -((this->A * lp1[0]) + (this->B * lp1[1]) + (this->C * lp1[2]) + this->D);
    double t = numerator / denominator;
    
    if ((t >= 0.0f) && (t <= 1.0f)) {
        intersectionOut[0] = (float)(lp1[0] + r0 * t);
        intersectionOut[1] = (float)(lp1[1] + r1 * t);
        intersectionOut[2] = (float)(lp1[2] + r2 * t);
        return true;
    }
    
    return false;
}

/**
 * Unit test the class.
 *
 */
void
Plane::unitTest(std::ostream& stream,
                const bool /*isVerbose*/)
{
    stream << "Plane::unitTest is starting" << std::endl;
    Plane::unitTest1(stream);
    Plane::unitTest2(stream);
    Plane::unitTest3(stream);
    stream << "Plane::unitTest has ended" << std::endl;
}

/**
 * Unit test a plane and line intersection.
 * @param testName  Name of test.
 * @param p1  Plane Point 1.
 * @param p2  Plane Point 2.
 * @param p3  Plane Point 3.
 * @param l1  Line End Point 1.
 * @param l2  Line End Point 2.
 * @param correctIntersectionPoint
 * @param intersectionValid
 *
 */
void
Plane::unitTestLineIntersectPlane(std::ostream& stream,
                   const AString& testName,
                   const float p1[3],
                   const float p2[3],
                   const float p3[3],
                   const float l1[3],
                   const float l2[3],
                   const float correctIntersectionPoint[3],
                   const bool intersectionValid)
{
    float intersection[3];
    Plane p(p1, p2, p3);
    bool result = p.lineSegmentIntersectPlane(l1, l2, intersection);
    
    AString sb;
    if (intersectionValid && result) {
        if (MathFunctions::compareArrays(correctIntersectionPoint, intersection, 3, 0.001f)) {
            return;
        }
        sb.append("Intersection should be " 
                  + AString::fromNumbers(correctIntersectionPoint, 3, ","));
        sb.append(" but is " + AString::fromNumbers(intersection, 3, ","));
    }
    else if (intersectionValid != result) {
        sb.append("intersection " + AString::fromBool(result) + " but should be " + AString::fromBool(intersectionValid));
    }
    if (sb.length() > 0) {
        sb = ("Line/Plane Intersection Test " + testName + " FAILED: "
              + sb);
        stream << sb.toStdString() << std::endl;
    }
}

void
Plane::unitTest1(std::ostream& stream)
{
    float p1[] = { -50.0f, -60.0f, 2.0f };
    float p2[] = {  50.0f, -60.0f, 2.0f };
    float p3[] = {  50.0f,  60.0f, 2.0f };
    float l1[] = { 25.0f, 10.0f, -3.0f };
    float l2[] = { 25.0f, 10.0f,  7.0f };
    float correctIntersectionPoint[] = {
        25.0f, 10.0f, 2.0f      
    };
    Plane::unitTestLineIntersectPlane(stream, "1", p1, p2, p3, l1, l2, correctIntersectionPoint, true);
}

void
Plane::unitTest2(std::ostream& stream)
{
    float p1[] = { -50.0f, -60.0f, 2.0f };
    float p2[] = {  50.0f, -60.0f, 2.0f };
    float p3[] = {  50.0f,  60.0f, 2.0f };
    float l1[] = { 25.0f, 10.0f,  7.0f };
    float l2[] = { 25.0f, 10.0f,  9.0f };
    float correctIntersectionPoint[3];
    Plane::unitTestLineIntersectPlane(stream, "2", p1, p2, p3, l1, l2, correctIntersectionPoint, false);}

void
Plane::unitTest3(std::ostream& stream)
{
    float p1[] = { -50.0f, -60.0f, 2.0f };
    float p2[] = {  50.0f, -60.0f, 2.0f };
    float p3[] = {  50.0f,  60.0f, 2.0f };
    float l1[] = { 25.0f, 10.0f, -5.0f };
    float l2[] = { 25.0f, 10.0f, -3.0f };
    float correctIntersectionPoint[3];
    Plane::unitTestLineIntersectPlane(stream, "3", p1, p2, p3, l1, l2, correctIntersectionPoint, false);
}

