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
/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkBase64Utilities.h,v $

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/



#include <cmath>

#include "MathFunctions.h"

#include "CaretAssert.h"

using namespace caret;
using namespace std;

MathFunctions::MathFunctions()
    : CaretObject()
{
}

/**
 * Destructor
 */
MathFunctions::~MathFunctions()
{
}

/**
 * Calulate the number of combinations for choosing "k" elements
 * from a total of "n" elements.
 * 
 * Formula: [n!/(k!*(n-k!))
 *    If k > (n-k):  [n(n-1)(n-2)...(k+1)] / (n-k)!
 *    If k < (n-k):  [n(n-1)(n-2)...(n-k+1)] / k!
 * 
 * @param n - total number of elements.
 * @param k - number of elements to choose.
 * @return  - number of combinations.
 *
 */
int64_t
MathFunctions::combinations(
                   const int64_t n,
                   const int64_t k)
{
    int64_t denominator = 1;
    int64_t nmk         = n - k;
    int64_t iStart      = 1;
    
    if (k > nmk) {
        iStart = k + 1;
        denominator = MathFunctions::factorial(nmk);
    }
    else {
        iStart = nmk + 1;
        denominator = MathFunctions::factorial(k);
    }
    int64_t numerator   = 1;
    for (int64_t i = iStart; i <= n; i++) {
        numerator *= i;
    }
    
    int64_t numCombosLong = numerator / denominator;
    int64_t numCombos = (int)numCombosLong;
    
    return numCombos;
}

/**
 * Calulate the number of permuations for choosing "k" elements
 * from a total of "n" elements.
 * 
 * Formula: [n!/(n-m)!] = n(n-1)(n-2)...(n-m+1).
 * 
 * @param n - total number of elements.
 * @param k - number of elements to choose.
 * @return  - number of combinations.
 *
 */
int64_t
MathFunctions::permutations(
                   const int64_t n,
                   const int64_t k)
{
    int64_t iStart = n - k + 1;
    int numPerms = 1;
    for (int i = iStart; i <= n; i++) {
        numPerms *= i;
    }
    
    return numPerms;
}

/**
 * Calculate the factorial for a number.
 * 
 * @param n - the number.
 * @return  - its factiorial
 *
 */
int64_t
MathFunctions::factorial(const int64_t n)
{
    int64_t num = 1;
    
    for (int64_t i = 1; i <= n; i++) {
        num *= i;
    }
    
    return num;
}

/**
 * Compute a normal vector from three vertices and make it a unit vector.
 *
 * @param  v1  the first vertex, an array of three floats.
 * @param  v2  the first vertex, an array of three floats.
 * @param  v3  the first vertex, an array of three floats.
 * @param normalVectorOut A three-dimensional array passed into which
 * the normal vector is loaded.
 * @return true if vector is valid (non-zero length).
 *
 */
bool
MathFunctions::normalVector(
                   const float v1[3],
                   const float v2[3],
                   const float v3[3],
                   float normalVectorOut[3])
{
    /*
    * DOUBLE PRECISION is needed when points are a small or sliver triangle.
    */
    double a0 = v3[0] - v2[0];
    double a1 = v3[1] - v2[1];
    double a2 = v3[2] - v2[2];
    
    double b0 = v1[0] - v2[0];
    double b1 = v1[1] - v2[1];
    double b2 = v1[2] - v2[2];
    
    double nv0 = (a1 * b2 - a2 * b1);
    double nv1 = (a2 * b0 - a0 * b2);
    double nv2 = (a0 * b1 - a1 * b0);
    
    double length = std::sqrt(nv0*nv0 + nv1*nv1 + nv2*nv2);
    bool valid = false;
    if (length != 0.0) {
        nv0 /= length;
        nv1 /= length;
        nv2 /= length;
        valid = true;
    }
    
    normalVectorOut[0] = (float)nv0;
    normalVectorOut[1] = (float)nv1;
    normalVectorOut[2] = (float)nv2;
    
    return valid;
}

/**
 * Compute a normal vector from three vertices and make it a unit vector.
 *
 * @param  v1  the first vertex, an array of three floats.
 * @param  v2  the first vertex, an array of three floats.
 * @param  v3  the first vertex, an array of three floats.
 * @param normalVectorOut A three-dimensional array passed into which
 * the normal vector is loaded.
 * @return true if vector is valid (non-zero length).
 *
 */
bool
MathFunctions::normalVector(
                   const double v1[3],
                   const double v2[3],
                   const double v3[3],
                   double normalVectorOut[3])
{
    double a0 = v3[0] - v2[0];
    double a1 = v3[1] - v2[1];
    double a2 = v3[2] - v2[2];
    
    double b0 = v1[0] - v2[0];
    double b1 = v1[1] - v2[1];
    double b2 = v1[2] - v2[2];
    
    double nv0 = (a1 * b2 - a2 * b1);
    double nv1 = (a2 * b0 - a0 * b2);
    double nv2 = (a0 * b1 - a1 * b0);
    
    double length = std::sqrt(nv0*nv0 + nv1*nv1 + nv2*nv2);
    bool valid = false;
    if (length != 0.0) {
        nv0 /= length;
        nv1 /= length;
        nv2 /= length;
        valid = true;
    }
    
    normalVectorOut[0] = nv0;
    normalVectorOut[1] = nv1;
    normalVectorOut[2] = nv2;
    
    return valid;
}

/**
 * Compute a normal vector from three vertices and but the returned
 * vector IS NOT a unit vector.
 *
 * @param  v1  the first vertex, an array of three floats.
 * @param  v2  the first vertex, an array of three floats.
 * @param  v3  the first vertex, an array of three floats.
 * @return     The normal vector, an array of three floats.
 *
 */
void
MathFunctions::normalVectorDirection(
                   const float v1[3],
                   const float v2[3],
                   const float v3[3],
                   float directionOut[3])
{
    float a[] = {
        v3[0] - v2[0],
        v3[1] - v2[1],
        v3[2] - v2[2]
    };
    float b[] = {
        v1[0] - v2[0],
        v1[1] - v2[1],
        v1[2] - v2[2]
    };
    
    directionOut[0] = (a[1] * b[2] - a[2] * b[1]);
    directionOut[1] = (a[2] * b[0] - a[0] * b[2]);
    directionOut[2] = (a[0] * b[1] - a[1] * b[0]);
}

/**
 * Cross product of two 3D vectors.
 * @param  v1  The first vector, an array of three floats.
 * @param  v2  The first vector, an array of three floats.
 * @param resultOut  Output containing the cross product.
 *
 */
void
MathFunctions::crossProduct(
                   const float v1[],
                   const float v2[],
                   float resultOut[])
{
    resultOut[0] = v1[1] * v2[2] - v1[2] * v2[1];
    resultOut[1] = v1[2] * v2[0] - v1[0] * v2[2];
    resultOut[2] = v1[0] * v2[1] - v1[1] * v2[0];
}

/**
 * Cross product of two 3D vectors.
 * @param  v1  The first vector, an array of three floats.
 * @param  v2  The first vector, an array of three floats.
 * @param resultOut  Output containing the cross product.
 *
 */
void
MathFunctions::crossProduct(
                            const double v1[],
                            const double v2[],
                            double resultOut[])
{
    resultOut[0] = v1[1] * v2[2] - v1[2] * v2[1];
    resultOut[1] = v1[2] * v2[0] - v1[0] * v2[2];
    resultOut[2] = v1[0] * v2[1] - v1[1] * v2[0];
}

/**
 * Cross product of two 3D vectors with normalizing both the
 * input and output vectors.
 *
 * @param  x1  The first vector, an array of three floats.
 * @param  x2  The first vector, an array of three floats.
 * @return     The cross product, an array of three floats.
 *
 */
void
MathFunctions::normalizedCrossProduct(
                   const float x1[],
                                      const float x2[],
                                      float resultOut[])
{
    float v1[3] = { x1[0], x1[1], x1[2] };
    MathFunctions::normalizeVector(v1);
    float v2[3] = { x2[0], x2[1], x2[2] };
    MathFunctions::normalizeVector(v2);
    
    MathFunctions::crossProduct(v1, v2, resultOut);
    MathFunctions::normalizeVector(resultOut);
}

/**
 * Normalize a 3D vector (make its length 1.0).
 * 
 * @param  vectorsAll  Array containing the XYZ components
 *    of the vectors.
 * @param offset Offset of the vector's X-component in the
 *    vectorsAll array.
 * @return The length of the vector prior to normalization.
 *
 */
float
MathFunctions::normalizeVector(
                   float vectorsAll[],
                   const int32_t offset)
{
    float len = MathFunctions::vectorLength(vectorsAll, offset);
    if (len != 0.0) {
        vectorsAll[offset] /= len;
        vectorsAll[offset+1] /= len;
        vectorsAll[offset+2] /= len;
    }
    return len;
}

/**
 * Normalize a 3D vector (make its length 1.0).
 * 
 * @param  vectorInOut  vector that is normalized.
 * @return The length of the vector prior to normalization.
 *
 */
float
MathFunctions::normalizeVector(float vector[3])
{
    float len = vectorLength(vector);
    if (len != 0.0) {
        vector[0] /= len;
        vector[1] /= len;
        vector[2] /= len;
    }
    return len;
}

/**
 * Normalize a 3D vector (make its length 1.0).
 *
 * @param  vectorInOut  vector that is normalized.
 * @return The length of the vector prior to normalization.
 *
 */
double
MathFunctions::normalizeVector(double vector[3])
{
    double len = vectorLength(vector);
    if (len != 0.0) {
        vector[0] /= len;
        vector[1] /= len;
        vector[2] /= len;
    }
    return len;
}

/**
 * Get length of vector.
 * 
 * @param  vector  Vector whose length is computed.
 *
 * @return  The length of the vector.
 *
 */
float
MathFunctions::vectorLength(const float vector[3])
{
    float len =
    (float)std::sqrt(vector[0]*vector[0] +
                     vector[1]*vector[1] +
                     vector[2]*vector[2]);
    return len;
}

/**
 * Get length of vector.
 * 
 * @param  vectorsAll  Array containing three-dimensional vectors.
 * @param offset   Offset of vector's X-component in vectorsAll array.
 *
 * @return  The length of the vector.
 *
 */
float
MathFunctions::vectorLength(
                   const float vectorsAll[],
                   const int32_t offset)
{
    float len =
    (float)std::sqrt(vectorsAll[offset]*vectorsAll[offset] +
                     vectorsAll[offset+1]*vectorsAll[offset+1] +
                     vectorsAll[offset+2]*vectorsAll[offset+2]);
    return len;
}

/**
 * Get length of vector.
 *
 * @param  vector  Vector whose length is computed.
 *
 * @return  The length of the vector.
 *
 */
double
MathFunctions::vectorLength(const double vector[3])
{
    double len = std::sqrt(vector[0]*vector[0] +
                     vector[1]*vector[1] +
                     vector[2]*vector[2]);
    return len;
}

/**
 * Get the squared distance between two 3D points.
 * 
 * @param  p1   Point 1 (3 element array)
 * @param  p2   Point 2 (3 element array)
 * @return Distance squared between the two points.
 *
 */
float
MathFunctions::distanceSquared3D(
                   const float p1[3],
                   const float p2[3])
{
    float dx = p1[0] - p2[0];
    float dy = p1[1] - p2[1];
    float dz = p1[2] - p2[2];
    float distSQ = dx*dx + dy*dy + dz*dz;
    return distSQ;
}

/**
 * Get the squared distance between two 3D coordinates.
 * 
 * @param xyzAll Array containing all of the XYZ coordinates.
 * @param  offsetCoord1 Offset of the first coordinates X-coordinate.
 * @param  offsetCoord2 Offset of the second coordinates X-coordinate.
 * @return Distance squared between the two coordinates.
 *
 */
float
MathFunctions::distanceSquared3D(
                   const float xyzAll[],
                   const int32_t offsetCoord1,
                   const int32_t offsetCoord2)
{
    float dx = xyzAll[offsetCoord1]   - xyzAll[offsetCoord2];
    float dy = xyzAll[offsetCoord1+1] - xyzAll[offsetCoord2+1];
    float dz = xyzAll[offsetCoord1+2] - xyzAll[offsetCoord2+2];
    float distSQ = dx*dx + dy*dy + dz*dz;
    return distSQ;
}

/**
 * Get the distance between two 3D points.
 * 
 * @param  p1   Point 1 (3 element array)
 * @param  p2   Point 2 (3 element array)
 * @return Distance between the two points.
 *
 */
float
MathFunctions::distance3D(
                   const float p1[3],
                   const float p2[3])
{
    float dist = distanceSquared3D(p1, p2);
    if (dist != 0.0f) {
        dist = (float)std::sqrt(dist);
    }
    return dist;
}

/**
 * Get the squared distance between two 3D points.
 *
 * @param  p1   Point 1 (3 element array)
 * @param  p2   Point 2 (3 element array)
 * @return Distance squared between the two points.
 *
 */
double
MathFunctions::distanceSquared3D(
                   const double p1[3],
                   const double p2[3])
{
    double dx = p1[0] - p2[0];
    double dy = p1[1] - p2[1];
    double dz = p1[2] - p2[2];
    double distSQ = dx*dx + dy*dy + dz*dz;
    return distSQ;
//    double dist = distanceSquared3D(p1, p2);
//    if (dist != 0.0f) {
//        dist = std::sqrt(dist);
//    }
//    return dist;
}

/**
 * Get the distance between two 3D points.
 *
 * @param  p1   Point 1 (3 element array)
 * @param  p2   Point 2 (3 element array)
 * @return Distance between the two points.
 *
 */
double
MathFunctions::distance3D(
                   const double p1[3],
                   const double p2[3])
{
    double dist = distanceSquared3D(p1, p2);
    if (dist != 0.0f) {
        dist = std::sqrt(dist);
    }
    return dist;
}

/**
 * subtract vectors (3d)  result = v1 - v2.
 * @param v1  1st vector input
 * @param v2  2nd vector input
 * @return  3D vector containing result of subtraction.
 *
 */
void
MathFunctions::subtractVectors(
                   const float v1[3],
                   const float v2[3],
                               float resultOut[3])
{
    resultOut[0] = v1[0] - v2[0];
    resultOut[1] = v1[1] - v2[1];
    resultOut[2] = v1[2] - v2[2];
}

/**
 * subtract vectors (3d)  result = v1 - v2.
 * @param v1  1st vector input
 * @param v2  2nd vector input
 * @return  3D vector containing result of subtraction.
 *
 */
void
MathFunctions::subtractVectors(const double v1[3],
                               const double v2[3],
                               double resultOut[3])
{
    resultOut[0] = v1[0] - v2[0];
    resultOut[1] = v1[1] - v2[1];
    resultOut[2] = v1[2] - v2[2];
}

void MathFunctions::addVectors(const float v1[3], const float v2[3], float resultOut[3])
{
    resultOut[0] = v1[0] + v2[0];
    resultOut[1] = v1[1] + v2[1];
    resultOut[2] = v1[2] + v2[2];
}

/**
 * Create the unit vector for a vector that starts at startXYZ and
 * ends at endXYZ.
 * 
 * @param startXYZ - Starting position of vector.
 * @param endXYZ - Ending position of vector.
 * @param  Unit vector starting at startXYZ and pointing to endXYZ.
 *
 */
void
MathFunctions::createUnitVector(
                   const float startXYZ[3],
                   const float endXYZ[3],
                                float resultOut[3])
{
    resultOut[0] = endXYZ[0] - startXYZ[0];
    resultOut[1] = endXYZ[1] - startXYZ[1];
    resultOut[2] = endXYZ[2] - startXYZ[2];
    
    MathFunctions::normalizeVector(resultOut);
}

/**
 * Create the unit vector for a vector that starts at startXYZ and
 * ends at endXYZ.
 *
 * @param startXYZ - Starting position of vector.
 * @param endXYZ - Ending position of vector.
 * @param  Unit vector starting at startXYZ and pointing to endXYZ.
 *
 */
void
MathFunctions::createUnitVector(
                                const double startXYZ[3],
                                const double endXYZ[3],
                                double resultOut[3])
{
    resultOut[0] = endXYZ[0] - startXYZ[0];
    resultOut[1] = endXYZ[1] - startXYZ[1];
    resultOut[2] = endXYZ[2] - startXYZ[2];
    
    MathFunctions::normalizeVector(resultOut);
}

/**
 * Dot produce of three dimensional vectors.
 * @param p1   vector 1
 * @param p2   vector 2
 * @return     Dot product of the two vectors.
 *
 */
float
MathFunctions::dotProduct(
                   const float p1[3],
                   const float p2[3])
{
    float dot = p1[0]*p2[0] + p1[1]*p2[1] + p1[2]*p2[2];
    return dot;
}

/**
 * Dot produce of three dimensional vectors.
 * @param p1   vector 1
 * @param p2   vector 2
 * @return     Dot product of the two vectors.
 *
 */
float
MathFunctions::dotProduct(
                          const double p1[3],
                          const double p2[3])
{
    float dot = p1[0]*p2[0] + p1[1]*p2[1] + p1[2]*p2[2];
    return dot;
}

/**
 * Calculate the area for a triangle.
 * @param v1 - XYZ coordinates for vertex 1
 * @param v2 - XYZ coordinates for vertex 2
 * @param v3 - XYZ coordinates for vertex 3
 * 
 * @return Area of triangle.
 *
 */
float
MathFunctions::triangleArea(
                   const float v1[3],
                   const float v2[3],
                   const float v3[3])
{
    /*
     * Using doubles for the intermediate calculations
     * produces results different from that if floats
     * were used in the "area" equation.  I'm
     * assuming double is more accurate (JWH).
     */
    double a = MathFunctions::distanceSquared3D(v1,v2);
    double b = MathFunctions::distanceSquared3D(v2,v3);
    double c = MathFunctions::distanceSquared3D(v3,v1);
    float area =
    (float)(0.25f* std::sqrt(std::abs(4.0*a*c - (a-b+c)*(a-b+c))));
    return area;
}

/**
 * Calculate the area for a triangle (with doubles)
 * @param v1 - XYZ coordinates for vertex 1
 * @param v2 - XYZ coordinates for vertex 2
 * @param v3 - XYZ coordinates for vertex 3
 * 
 * @return Area of triangle.
 *
 */
float
MathFunctions::triangleArea(const double v1[3],
                            const double v2[3],
                            const double v3[3])
{
    /*
     * Using doubles for the intermediate calculations
     * produces results different from that if floats
     * were used in the "area" equation.  I'm
     * assuming double is more accurate (JWH).
     */
    double a = MathFunctions::distanceSquared3D(v1,v2);
    double b = MathFunctions::distanceSquared3D(v2,v3);
    double c = MathFunctions::distanceSquared3D(v3,v1);
    float area =
    (float)(0.25f* std::sqrt(std::abs(4.0*a*c - (a-b+c)*(a-b+c))));
    return area;
}

/**
 * Calculate the area of a triangle formed by 3 coordinates.
 * @param xyzAll One-dimensional array containing the XYZ coordinates.
 * @param offsetCoord1  Offset of node 1's X-coordinate which is
 *    followed by the Y- and Z-coordinates.
 * @param offsetCoord2  Offset of node 2's X-coordinate which is
 *    followed by the Y- and Z-coordinates.
 * @param offsetCoord3  Offset of node 3's X-coordinate which is
 *    followed by the Y- and Z-coordinates.
 * @return  Area of the triangle formed by the coordinates.
 *
 */
float
MathFunctions::triangleArea(
                   const float xyzAll[],
                   const int32_t offsetCoord1,
                   const int32_t offsetCoord2,
                   const int32_t offsetCoord3)
{
    /*
     * Using doubles for the intermediate calculations
     * produces results different from that if floats
     * were used in the "area" equation.  I'm
     * assuming double is more accurate (JWH).
     */
    double a = MathFunctions::distanceSquared3D(xyzAll, offsetCoord1, offsetCoord2);
    double b = MathFunctions::distanceSquared3D(xyzAll, offsetCoord2, offsetCoord3);
    double c = MathFunctions::distanceSquared3D(xyzAll, offsetCoord3, offsetCoord1);
    float area =
        (float)(0.25f* std::sqrt(std::abs(4.0*a*c - (a-b+c)*(a-b+c))));
    return area;
}

/**
 * Compute the signed area of a triangle in 2D.
 * @param p1 - 1st coordinate of triangle
 * @param p2 - 2nd coordinate of triangle
 * @param p3 - 3rd coordinate of triangle
 * @return Signed area of triangle which is positive if the vertices
 *    are in counter-clockwise orientation or negative if the vertices
 *    are in clockwise orientation.
 *
 */
float
MathFunctions::triangleAreaSigned2D(
                   const float p1[2],
                   const float p2[2],
                   const float p3[2])
{
    float area = (  p1[0]*p2[1] + p2[0]*p3[1] + p3[0]*p1[1]
                  - p1[1]*p2[0] - p2[1]*p3[0] - p3[1]*p1[0] ) * 0.5f;
    return area;
}

/**
 * Compute the signed area of a triangle in 3D.
 * @param referenceNormal - Normal vector.
 * @param p1 - 1st coordinate of triangle
 * @param p2 - 2nd coordinate of triangle
 * @param p3 - 3rd coordinate of triangle
 * @return Signed area of triangle which is positive if the vertices
 *    are in counter-clockwise orientation or negative if the vertices
 *    are in clockwise orientation.
 *
 */
float
MathFunctions::triangleAreaSigned3D(
                   const float referenceNormal[3],
                   const float p1[3],
                   const float p2[3],
                   const float p3[3])
{
    //
    // Area of the triangle formed by the three points
    //
    float area = triangleArea(p1, p2, p3);
    
    //
    // Normal for the three points
    //
    float triangleNormal[3];
    MathFunctions::normalVector(p1, p2, p3, triangleNormal);
    
    //
    // Dot Product is the cosine of the angle between the two normals.  When this value is less
    // than zero, the absolute angle between the normals is greater than 90 degrees.
    //
    float dot = MathFunctions::dotProduct(referenceNormal, triangleNormal);
    if (dot < 0.0) {
        area = -area;
    }
    
    return area;
}

void
MathFunctions::vtkLinearSolve3x3(
                   const float A[3][3],
                   const float x[3],
                   float y[3])
{
    int index[3];
    float B[3][3];
    for (int i = 0; i < 3; i++)
    {
        B[i][0] = A[i][0];
        B[i][1] = A[i][1];
        B[i][2] = A[i][2];
        y[i] = x[i];
    }
    
    vtkLUFactor3x3(B,index);
    vtkLUSolve3x3(B,index,y);
}

void
MathFunctions::vtkLUSolve3x3(
                   const float A[3][3],
                   const int32_t index[3],
                   float x[3])
{
    float sum;
    
    // forward substitution
    
    sum = x[index[0]];
    x[index[0]] = x[0];
    x[0] = sum;
    
    sum = x[index[1]];
    x[index[1]] = x[1];
    x[1] = sum - A[1][0]*x[0];
    
    sum = x[index[2]];
    x[index[2]] = x[2];
    x[2] = sum - A[2][0]*x[0] - A[2][1]*x[1];
    
    // back substitution
    
    x[2] = x[2]*A[2][2];
    x[1] = (x[1] - A[1][2]*x[2])*A[1][1];
    x[0] = (x[0] - A[0][1]*x[1] - A[0][2]*x[2])*A[0][0];
}

void
MathFunctions::vtkLUFactor3x3(
                   float A[3][3],
                   int32_t index[3])
{
    int i,maxI;
    float tmp,largest;
    float scale[3];
    
    // Loop over rows to get implicit scaling information
    
    for ( i = 0; i < 3; i++ )
    {
        largest =  std::abs(A[i][0]);
        if ((tmp = std::abs(A[i][1])) > largest)
        {
            largest = tmp;
        }
        if ((tmp = std::abs(A[i][2])) > largest)
        {
            largest = tmp;
        }
        scale[i] = (1.0f)/largest;
    }
    
    // Loop over all columns using Crout's method
    
    // first column
    largest = scale[0]*std::abs(A[0][0]);
    maxI = 0;
    if ((tmp = scale[1]*std::abs(A[1][0])) >= largest)
    {
        largest = tmp;
        maxI = 1;
    }
    if ((tmp = scale[2]*std::abs(A[2][0])) >= largest)
    {
        maxI = 2;
    }
    if (maxI != 0)
    {
        //vtkSwapVectors3(A[maxI],A[0]);
        float tmpSwap[3] = { A[maxI][0], A[maxI][1], A[maxI][2] };
        A[maxI][0] = A[0][0];
        A[maxI][1] = A[0][1];
        A[maxI][2] = A[0][2];
        A[0][0] = tmpSwap[0];
        A[0][1] = tmpSwap[1];
        A[0][2] = tmpSwap[2];
        scale[maxI] = scale[0];
    }
    index[0] = maxI;
    
    A[0][0] = (1.0f)/A[0][0];
    A[1][0] *= A[0][0];
    A[2][0] *= A[0][0];
    
    // second column
    A[1][1] -= A[1][0]*A[0][1];
    A[2][1] -= A[2][0]*A[0][1];
    largest = scale[1]*std::abs(A[1][1]);
    maxI = 1;
    if ((tmp = scale[2]*std::abs(A[2][1])) >= largest)
    {
        maxI = 2;
        //vtkSwapVectors3(A[2],A[1]);
        float tmpSwap[3] =  { A[2][0], A[2][1], A[2][2] };
        A[2][0] = A[1][0];
        A[2][1] = A[1][1];
        A[2][2] = A[1][2];
        A[1][0] = tmpSwap[0];
        A[1][1] = tmpSwap[1];
        A[1][2] = tmpSwap[2];
        scale[2] = scale[1];
    }
    index[1] = maxI;
    A[1][1] = (1.0f)/A[1][1];
    A[2][1] *= A[1][1];
    
    // third column
    A[1][2] -= A[1][0]*A[0][2];
    A[2][2] -= A[2][0]*A[0][2] + A[2][1]*A[1][2];
    //largest = scale[2]*std::abs(A[2][2]);
    index[2] = 2;
    A[2][2] = (1.0f)/A[2][2];
}

/**
 * Determine if 2D line segments intersect.
 * Algorithm from http://mathworld.wolfram.com/Line-LineIntersection.html
 *
 * @param p1 Line 1 end point 1.
 * @param p2 Line 1 end point 2.
 * @param q1 Line 2 end point 1.
 * @param q2 Line 2 end point 2.
 * @param tolerance  Tolerance around the vertices (essentially
 *    lengthens lines by this quantity).  Caret5 set this
 *    parameter to 0.01.
 * @param intersectionOut Location of intersection.
 * @return  true if the line segments intersect else false.
 *
 */
bool
MathFunctions::lineIntersection2D(
                   const float p1[2],
                   const float p2[2],
                   const float q1[2],
                   const float q2[2],
                   const float tolerance,
                   float intersectionOut[2])
{
    double tol = tolerance;
    double x1 = p1[0];
    double y1 = p1[1];
    double x2 = p2[0];
    double y2 = p2[1];
    
    double x3 = q1[0];
    double y3 = q1[1];
    double x4 = q2[0];
    double y4 = q2[1];
    
    double denom = ((x1 - x2) * (y3 - y4)) - ((x3 - x4) * (y1 - y2));
    
    if (denom != 0.0) {
        double a = (x1 * y2) - (x2 * y1);
        double c = (x3 * y4) - (x4 * y3);
        double x = ((a * (x3 - x4)) - (c * (x1 - x2))) / denom;
        double y = ((a * (y3 - y4)) - (c * (y1 - y2))) / denom;
        
        double pxMax = std::max(x1, x2) + tol;
        double pxMin = std::min(x1, x2) - tol;
        double pyMax = std::max(y1, y2) + tol;
        double pyMin = std::min(y1, y2) - tol;
        
        double qxMax = std::max(x3, x4) + tol;
        double qxMin = std::min(x3, x4) - tol;
        double qyMax = std::max(y3, y4) + tol;
        double qyMin = std::min(y3, y4) - tol;
        
        intersectionOut[0] = (float)x;
        intersectionOut[1] = (float)y;
        if ((x >= pxMin) && (x <= pxMax) && (x >= qxMin) && (x <= qxMax) &&
            (y >= pyMin) && (y <= pyMax) && (y >= qyMin) && (y <= qyMax)) {
            return true;
        }
    }
    
    return false;
}

/**
 * Determine if a ray intersects a plane.
 * @param p1 - 1st point defining the plane
 * @param p2 - 2nd point defining the plane
 * @param p3 - 3rd point defining the plane
 * @param rayOrigin - origin of the ray
 * @param rayVector - vector defining the ray
 * @param intersectionXYZandDistance - An array of four that will contain
 *    the XYZ or the intersection point and the distance from the plane.
 * @return  true if the ray intersects the plane, else false.
 *
 */
bool
MathFunctions::rayIntersectPlane(
                   const float p1[3],
                   const float p2[3],
                   const float p3[3],
                   const float rayOrigin[3],
                   const float rayVector[3],
                   float intersectionXYZandDistance[4])
{
    // Convert the ray into a unit vector
    //
    double ray[3] = { rayVector[0], rayVector[1], rayVector[2] };
    MathFunctions::normalizeVector(ray);
    
    //
    // Normal of plane
    //
    float normal[3]; 
    MathFunctions::normalVector(p1, p2, p3, normal);
    
    //
    // Compute the plane equation
    //
    double A = normal[0];
    double B = normal[1];
    double C = normal[2];
    double D = -(A*p1[0] + B*p1[1] + C*p1[2]);
    
    //
    // Parametric coordinate of where ray intersects plane
    //
    double denom = A * ray[0] + B * ray[1] + C * ray[2];
    if (denom != 0) {
        const double t = -(A * rayOrigin[0] + B * rayOrigin[1] + C * rayOrigin[2] + D) / denom;
        
        intersectionXYZandDistance[0] = (float)(rayOrigin[0] + ray[0] * t);
        intersectionXYZandDistance[1] = (float)(rayOrigin[1] + ray[1] * t);
        intersectionXYZandDistance[2] = (float)(rayOrigin[2] + ray[2] * t);
        
        intersectionXYZandDistance[3] = (float)t;
        
        return true;
    }
    
    return false;
}

/**
 * Project a point to a plane.
 * @param pt - the point to project.
 * @param origin - point in the plane.
 * @param normal - normal vector of plane.
 * @return  The projected position of "pt" on the plane.
 *
 */
void
MathFunctions::projectPoint(
                   const float pt[3],
                   const float origin[3],
                   const float normal[3],
                            float projectedPointOut[3])
{
    float xo[3] = {
        pt[0] - origin[0],
        pt[1] - origin[1],
        pt[2] - origin[2]
    };
    
    float t = MathFunctions::dotProduct(normal, xo);
    
    projectedPointOut[0] = pt[0] - t * normal[0];
    projectedPointOut[1] = pt[1] - t * normal[1];
    projectedPointOut[2] = pt[2] - t * normal[2];
}

/**
 * Get the signed distance from the plane to the "queryPoint".  
 * A positive distance indicates "queryPoint" is above the plane
 * and a negative distance indicates "queryPoint" is below
 * the plane.
 * 
 * @param planeNormal - plane's normal vector.
 * @param pointInPlane - point on the plane.
 * @param queryPoint - the query point for which distance to plane is sought.
 * 
 * @return Distance from the plane to the query point.
 *
 */
float
MathFunctions::signedDistanceFromPlane(
                   const float planeNormal[3],
                   const float pointInPlane[3],
                   const float queryPoint[3])
{
    // Find out where query point projects on the plane
    //
    float queryPointProjectedOntoPlane[3];
    MathFunctions::projectPoint(queryPoint, pointInPlane, planeNormal, queryPointProjectedOntoPlane);
    float dx = planeNormal[0]
        * (queryPoint[0] - queryPointProjectedOntoPlane[0]);
    float dy = planeNormal[1]
        * (queryPoint[1] - queryPointProjectedOntoPlane[1]);
    float dz = planeNormal[2]
        * (queryPoint[2] - queryPointProjectedOntoPlane[2]);
    float dist = dx + dy + dz;
    
    return dist;
}

/**
 * Limit the "value" to be inclusively between the minimum and maximum.
 * @param value - Value for testing.
 * @param minimumValue - Minimum inclusive value.
 * @param maximumValue - Maximum inclusive value.
 * @return Value limited inclusively to the minimum and maximum values.
 *
 */
int32_t
MathFunctions::limitRange(
                   const int32_t value,
                   const int32_t minimumValue,
                   const int32_t maximumValue)
{
    if (value < minimumValue) {
        return minimumValue;
    }
    
    if (value > maximumValue) {
        return maximumValue;
    }
    
    return value;
}

/**
 * Limit the "value" to be inclusively between the minimum and maximum.
 * @param value - Value for testing.
 * @param minimumValue - Minimum inclusive value.
 * @param maximumValue - Maximum inclusive value.
 * @return Value limited inclusively to the minimum and maximum values.
 *
 */
float
MathFunctions::limitRange(
                   const float value,
                   const float minimumValue,
                   const float maximumValue)
{
    if (value < minimumValue) {
        return minimumValue;
    }
    
    if (value > maximumValue) {
        return maximumValue;
    }
    
    return value;
}

/**
 * Limit the "value" to be inclusively between the minimum and maximum.
 * @param value - Value for testing.
 * @param minimumValue - Minimum inclusive value.
 * @param maximumValue - Maximum inclusive value.
 * @return Value limited inclusively to the minimum and maximum values.
 *
 */
double
MathFunctions::limitRange(
                   const double value,
                   const double minimumValue,
                   const double maximumValue)
{
    if (value < minimumValue) {
        return minimumValue;
    }
    
    if (value > maximumValue) {
        return maximumValue;
    }
    
    return value;
}

/**
 * Find the distance from the point to the line defined by p1 and p2.
 * Formula is from
 *    "http://mathworld.wolfram.com/Point-LineDistance3-Dimensional.html".
 *
 * @param p1 - First coordinate in line.
 * @param p2 - Second coordinate in line.
 * @param point - coordinate for which distance to line is sought.
 * @return Distance from point to the line (p1, p2).
 *
 */
float
MathFunctions::distanceToLine3D(
                   const float p1[3],
                   const float p2[3],
                   const float point[3])
{
    float dv2v1[3];
    MathFunctions::subtractVectors(p2, p1, dv2v1);
    float dv1pt[3];
    MathFunctions::subtractVectors(p1, point, dv1pt);
    
    float crossed[3];
    MathFunctions::crossProduct(dv2v1, dv1pt, crossed);
    
    float numerator = MathFunctions::vectorLength(crossed);
    float denomenator = MathFunctions::vectorLength(dv2v1);
    
    float dist = numerator / denomenator;
    
    return dist;
}

/**
 * Determine if two arrays are equal, same number of elements and
 * corresponding elements equal.
 *
 * @param a - first array.
 * @param b - second array.
 * @return true if arrays are equal, else false.
 *
 */
bool
MathFunctions::arraysEqual(
                   const float a[],
                   const float b[],
                           const int numElements)
{
    for (int i = 0; i < numElements; i++) {
        if (a[i] != b[i]) {
            return false;
        }
    }
    
    return true;
}

/**
 * Get the average of two coordinates.
 * @param c1 - coordinate 1
 * @param c2 - coordinate 2
 * @param outputAverage A three-dimensional array into
 * which the average of the two coordinates is
 * placed.
 *
 */
void
MathFunctions::averageOfTwoCoordinates(const float c1[3],
                                       const float c2[3],
                                       float outputAverage[3])
{
    outputAverage[0] = (c1[0] + c2[0]) / 2.0f;
    outputAverage[1] = (c1[1] + c2[1]) / 2.0f;
    outputAverage[2] = (c1[2] + c2[2]) / 2.0f;
}


/**
 * Get the average of three coordinates.
 * @param c1 - coordinate 1
 * @param c2 - coordinate 2
 * @param c3 - coordinate 3
 * @param outputAverage A three-dimensional array into 
 * which the average of the three coordinates is 
 * placed.
 *
 */
void
MathFunctions::averageOfThreeCoordinates(
                   const float c1[3],
                   const float c2[3],
                   const float c3[3],
                   float outputAverage[3])
{
    outputAverage[0] = (c1[0] + c2[0] + c3[0]) / 3.0f;
    outputAverage[1] = (c1[1] + c2[1] + c3[1]) / 3.0f;
    outputAverage[2] = (c1[2] + c2[2] + c3[2]) / 3.0f;
}

/**
 * Calculate the average of 3 coordinates.
 * @param xyzAll One-dimensional array containing the XYZ coordinates.
 * @param offsetCoord1  Offset of node 1's X-coordinate which is
 *    followed by the Y- and Z-coordinates.
 * @param offsetCoord2  Offset of node 2's X-coordinate which is
 *    followed by the Y- and Z-coordinates.
 * @param offsetCoord3  Offset of node 3's X-coordinate which is
 *    followed by the Y- and Z-coordinates.
 * @param outputAverage  3 dimensional array passed in, into which
 * the average is placed.
 * @param outputOffset  Offset of average into outputAverage array.
 *
 */
void
MathFunctions::averageOfThreeCoordinates(
                   const float xyzAll[],
                   const int32_t offsetCoord1,
                   const int32_t offsetCoord2,
                   const int32_t offsetCoord3,
                   float outputAverage[],
                   const int32_t outputOffset)
{
    outputAverage[outputOffset] = (xyzAll[offsetCoord1]   + xyzAll[offsetCoord2]   + xyzAll[offsetCoord3]) / 3.0f;
    outputAverage[outputOffset+1] = (xyzAll[offsetCoord1+1] + xyzAll[offsetCoord2+1] + xyzAll[offsetCoord3+1]) / 3.0f;
    outputAverage[outputOffset+2] = (xyzAll[offsetCoord1+2] + xyzAll[offsetCoord2+2] + xyzAll[offsetCoord3+2]) / 3.0f;
}

/**
 * Angle formed by p1, p2, p3 (angle at p2).  Returned angle is in radians.
 * This method uses Java Math.acos() and produces highly accurate results.
 * @param p1 - point.
 * @param p2 - point.
 * @param p3 - point.
 * @return Angle formed by points.
 *
 */
float
MathFunctions::angle(
                   const float p1[3],
                   const float p2[3],
                   const float p3[3])
{
    //
    // Vector from P2 to P1
    //
    float v21[3] = { p1[0] - p2[0], p1[1] - p2[1], p1[2] - p2[2] };
    
    //
    // Vector from P2 to P3
    //
    float v23[3] = { p3[0] - p2[0], p3[1] - p2[1], p3[2] - p2[2] };
    
    //
    // Normalize the vectors
    //
    float v21len = MathFunctions::normalizeVector(v21);
    float v23len = MathFunctions::normalizeVector(v23);
    
    float angleOut = 0.0f;
    if ((v21len > 0.0) && (v23len > 0.0)) {          //
        // angle is inverse cosine of the dot product
        // and be sure to handle numerical errors.
        //
        float dot = MathFunctions::dotProduct(v21, v23);
        if (dot > 1.0f) dot = 1.0f;
        else if (dot < -1.0f) dot = -1.0f;
        angleOut = (float)std::acos(dot);
    }
    
    return angleOut;
}

/**
 * Signed angle for "jik".
 * @param pi - point.
 * @param pj - point.
 * @param pk - point.
 * @param n - normal
 * @return signed angle formed by the points.
 *
 */
float
MathFunctions::signedAngle(
                   const float pi[3],
                   const float pj[3],
                   const float pk[3],
                   const float n[3])
{
    float x1 = pj[0] - pi[0];
    float y1 = pj[1] - pi[1];
    float z1 = pj[2] - pi[2];
    float x2 = pk[0] - pi[0];
    float y2 = pk[1] - pi[1];
    float z2 = pk[2] - pi[2];
    
    /* s = |(ji)||(ki)| sin(phi) by cross product */
    float dx = y1*z2 - y2*z1;
    float dy = x2*z1 - x1*z2;
    float dz = x1*y2 - x2*y1;
    float t = (dx*n[0]) + (dy*n[1]) + (dz*n[2]);
    float s = (float)std::sqrt((dx*dx) + (dy*dy) + (dz*dz));
    if (t < 0.0f) {
        s = -s;
    }
    
    /* c = |(ji)||(ki)| cos(phi) by inner product */
    float c = x1*x2 + y1*y2 + z1*z2;
    float phi = (float)std::atan2(s,c);
    return phi;
}

/**
 * Determine if an integer is an odd number.
 * @param number Integer to test.
 * @return  true if integer is odd, else false.
 *
 */
bool
MathFunctions::isOddNumber(const int32_t number)
{
    bool result = ((number & 1) != 0);
    return result;
}

/**
 * Determine if an integer is an odd number.
 * @param number Integer to test.
 * @return  true if integer is odd, else false.
 *
 */
bool
MathFunctions::isEvenNumber(const int32_t number)
{
    bool result = ((number & 1) == 0);
    return result;
}

/**
 * Determine if two arrays are equal.
 * @param a1 First array.
 * @param a2 Second array.
 * @param tolerance  Allowable difference in elements at same index.
 * @return  true if arrays are of same length and corresponding
 * elements have a difference less than tolerance.
 *
 */
bool
MathFunctions::compareArrays(
                   const float a1[],
                   const float a2[],
                   const int numElements,
                   const float tolerance)
{
    for (int i = 0; i < numElements; i++) {
        float diff = a1[i] - a2[i];
        if (diff < 0.0f) diff = -diff;
        if (diff > tolerance) {
            return false;
        }
    }
    
    return true;
}

/**
 * Clamp a value to the range minimum to maximum.
 * @param value  Value for clamping.
 * @param minimum  Minimum allowed value.
 * @param maximum  Maximum allowed value.
 * @return Value clamped to minimum and maximum.
 *
 */
int32_t
MathFunctions::clamp(
                   const int32_t value,
                   const int32_t minimum,
                   const int32_t maximum)
{
    return MathFunctions::limitRange(value, minimum, maximum);
}

/**
 * Clamp a value to the range minimum to maximum.
 * @param value  Value for clamping.
 * @param minimum  Minimum allowed value.
 * @param maximum  Maximum allowed value.
 * @return Value clamped to minimum and maximum.
 *
 */
float
MathFunctions::clamp(
                   const float value,
                   const float minimum,
                   const float maximum)
{
    return MathFunctions::limitRange(value, minimum, maximum);
}

/**
 * convert degrees to radians.
 * @param 
 *    degrees value converted to radians.
 * @return
 *    the corresponding radians value.
 */
float 
MathFunctions::toRadians(float degrees)
{
    float radians = degrees * (M_PI / 180.0f);
    return radians;
}
    
/**
 * convert radians to degrees.
 * @param 
 *    degrees value converted to degrees.
 * @return
 *    the corresponding degrees value.
 */
float 
MathFunctions::toDegrees(float radians)
{
    float degrees = radians * (180.0f / M_PI);
    return degrees;
}

/**
 * Distance SQUARED from (x1, y1) to (x2, y2)
 * @param X-coordinate of first point.
 * @param Y-coordinate of first point.
 * @param X-coordinate of second point.
 * @param Y-coordinate of second point.
 * @return  Distance squared between the points.
 */
double 
MathFunctions::distanceSquared2D(const double x1,
                                 const double y1,
                                 const double x2,
                                 const double y2)
{
    const double dx = x2 - x1;
    const double dy = y2 - y1;
    const double d = (dx*dx) + (dy*dy);
    return d;
}

uint32_t MathFunctions::gcd(uint32_t num1, uint32_t num2)
{
    if (num1 == 0 || num2 == 0)
    {//catch zeros
        return 0;//gcd(0,x)=gcd(x,0)=0, seems less confusing than returning x
    }
    //modulus method for good worst-case asymptotic performance
    uint32_t temp;
    if (num2 > num1)//num1 kept as the larger number to simplify the code
    {
        temp = num1;
        num1 = num2;
        num2 = temp;
    }
    while (num2)
    {//maintain num2 as the smaller number
        temp = num1 % num2;//modulus to reduce the larger as much as possible, result will be smaller than num2
        num1 = num2;//so, we need to swap them
        num2 = temp;//when result becomes zero, num1 is our gcd
    }
    return num1;
}

bool MathFunctions::isInf(const float number)
{
    return (abs(number) > 1.0f && number * 2.0f == number);
}

bool MathFunctions::isNaN(const float number)
{
    return (number != number);
}

bool MathFunctions::isNegInf(const float number)
{
    return (number < -1.0f && number * 2.0f == number);
}

bool MathFunctions::isNumeric(const float number)
{
    return (!isNaN(number) && !isInf(number));
}

bool MathFunctions::isPosInf(const float number)
{
    return (number > 1.0f && number * 2.0f == number);
}

void MathFunctions::quaternToMatrix(const float cijk[4], float matrix[3][3])
{//formula from http://en.wikipedia.org/wiki/Rotation_matrix#Quaternion
    double qlengthsqr = cijk[0] * cijk[0] + cijk[1] * cijk[1] + cijk[2] * cijk[2] + cijk[3] * cijk[3];
    double mult = 0.0;
    if (qlengthsqr > 0.0f)
    {
        mult = 2.0f / qlengthsqr;
    }
    double ijkmult[4] = { cijk[1] * mult, cijk[2] * mult, cijk[3] * mult };
    double wX = cijk[0] * ijkmult[0], wY = cijk[0] * ijkmult[1], wZ = cijk[0] * ijkmult[2];
    double xX = cijk[1] * ijkmult[0], xY = cijk[1] * ijkmult[1], xZ = cijk[1] * ijkmult[2];
    double yY = cijk[2] * ijkmult[1], yZ = cijk[2] * ijkmult[2];
    double zZ = cijk[3] * ijkmult[2];
    matrix[0][0] = 1.0 - (yY + zZ);//equals nifti1 formula because for unit quaternion, a*a + b*b + c*c + d*d = 1, and yY = 2 * c*c
    matrix[0][1] = xY - wZ;
    matrix[0][2] = xZ + wY;
    matrix[1][0] = xY + wZ;
    matrix[1][1] = 1.0 - (xX + zZ);
    matrix[1][2] = yZ - wX;
    matrix[2][0] = xZ - wY;
    matrix[2][1] = yZ + wX;
    matrix[2][2] = 1.0 - (xX + yY);
}

void MathFunctions::quaternToMatrix(const double cijk[4], double matrix[3][3])
{//formula from http://en.wikipedia.org/wiki/Rotation_matrix#Quaternion
    double qlengthsqr = cijk[0] * cijk[0] + cijk[1] * cijk[1] + cijk[2] * cijk[2] + cijk[3] * cijk[3];
    double mult = 0.0;
    if (qlengthsqr > 0.0f)
    {
        mult = 2.0f / qlengthsqr;
    }
    double ijkmult[4] = { cijk[1] * mult, cijk[2] * mult, cijk[3] * mult };
    double wX = cijk[0] * ijkmult[0], wY = cijk[0] * ijkmult[1], wZ = cijk[0] * ijkmult[2];
    double xX = cijk[1] * ijkmult[0], xY = cijk[1] * ijkmult[1], xZ = cijk[1] * ijkmult[2];
    double yY = cijk[2] * ijkmult[1], yZ = cijk[2] * ijkmult[2];
    double zZ = cijk[3] * ijkmult[2];
    matrix[0][0] = 1.0 - (yY + zZ);//equals nifti1 formula because for unit quaternion, a*a + b*b + c*c + d*d = 1, and yY = 2 * c*c
    matrix[0][1] = xY - wZ;
    matrix[0][2] = xZ + wY;
    matrix[1][0] = xY + wZ;
    matrix[1][1] = 1.0 - (xX + zZ);
    matrix[1][2] = yZ - wX;
    matrix[2][0] = xZ - wY;
    matrix[2][1] = yZ + wX;
    matrix[2][2] = 1.0 - (xX + yY);
}

bool MathFunctions::matrixToQuatern(const float matrix[3][3], float cijk[4])
{//formulas from http://en.wikipedia.org/wiki/Rotation_matrix#Quaternion
    const float toler = 0.0001f;
    float ivec[3] = { matrix[0][0], matrix[1][0], matrix[2][0] };
    float jvec[3] = { matrix[0][1], matrix[1][1], matrix[2][1] };
    float kvec[3] = { matrix[0][2], matrix[1][2], matrix[2][2] };
    if (!(std::abs(1.0f - normalizeVector(ivec)) <= toler)) return false;//use the "not less than or equal to" trick to catch NaNs
    if (!(std::abs(1.0f - normalizeVector(jvec)) <= toler)) return false;
    if (!(std::abs(1.0f - normalizeVector(kvec)) <= toler)) return false;
    if (!(dotProduct(ivec, jvec) <= toler)) return false;
    if (!(dotProduct(ivec, kvec) <= toler)) return false;
    if (!(dotProduct(jvec, kvec) <= toler)) return false;
    float tempvec[3];
    crossProduct(ivec, jvec, tempvec);
    if (!(dotProduct(tempvec, kvec) >= 0.9f)) return false;//i cross j must be k, otherwise it contains a flip
    int method = 0;
    double trace = matrix[0][0] + matrix[1][1] + matrix[2][2];
    if (trace < 0.0)
    {
        method = 1;
        float tempf = matrix[0][0];
        if (matrix[1][1] > tempf)
        {
            method = 2;
            tempf = matrix[1][1];
        }
        if (matrix[2][2] > tempf)
        {
            method = 3;
        }
    }
    switch (method)
    {
        case 0:
            {
                double r = std::sqrt(1.0 + trace);
                double s = 0.5 / r;
                cijk[0] = 0.5 * r;
                cijk[1] = (matrix[2][1] - matrix[1][2]) * s;
                cijk[2] = (matrix[0][2] - matrix[2][0]) * s;
                cijk[3] = (matrix[1][0] - matrix[0][1]) * s;
            }
        break;
        case 1:
            {
                double r = std::sqrt(1.0 + matrix[0][0] - matrix[1][1] - matrix[2][2]);
                double s = 0.5 / r;
                cijk[0] = (matrix[2][1] - matrix[1][2]) * s;
                cijk[1] = 0.5 * r;
                cijk[2] = (matrix[0][1] + matrix[1][0]) * s;
                cijk[3] = (matrix[2][0] + matrix[0][2]) * s;
            }
        break;
        case 2:
            {//DISCLAIMER: these last two were worked out by pattern since they aren't on wikipedia
                double r = std::sqrt(1.0 - matrix[0][0] + matrix[1][1] - matrix[2][2]);
                double s = 0.5 / r;
                cijk[0] = (matrix[0][2] - matrix[2][0]) * s;
                cijk[1] = (matrix[0][1] + matrix[1][0]) * s;
                cijk[2] = 0.5 * r;
                cijk[3] = (matrix[1][2] + matrix[2][1]) * s;
            }
        break;
        case 3:
            {
                double r = std::sqrt(1.0 - matrix[0][0] - matrix[1][1] + matrix[2][2]);
                double s = 0.5 / r;
                cijk[0] = (matrix[1][0] - matrix[0][1]) * s;
                cijk[1] = (matrix[2][0] + matrix[0][2]) * s;
                cijk[2] = (matrix[1][2] + matrix[2][1]) * s;
                cijk[3] = 0.5 * r;
            }
        break;
        default:
            return false;
    }
    if (cijk[0] < 0.0f)
    {
        cijk[0] = -cijk[0];
        cijk[1] = -cijk[1];
        cijk[2] = -cijk[2];
        cijk[3] = -cijk[3];
    }
    return true;
}

bool MathFunctions::matrixToQuatern(const double matrix[3][3], double cijk[4])
{//formulas from http://en.wikipedia.org/wiki/Rotation_matrix#Quaternion
    const float toler = 0.0001f;
    float ivec[3] = { matrix[0][0], matrix[1][0], matrix[2][0] };
    float jvec[3] = { matrix[0][1], matrix[1][1], matrix[2][1] };
    float kvec[3] = { matrix[0][2], matrix[1][2], matrix[2][2] };
    if (!(std::abs(1.0f - normalizeVector(ivec)) <= toler)) return false;//use the "not less than or equal to" trick to catch NaNs
    if (!(std::abs(1.0f - normalizeVector(jvec)) <= toler)) return false;
    if (!(std::abs(1.0f - normalizeVector(kvec)) <= toler)) return false;
    if (!(dotProduct(ivec, jvec) <= toler)) return false;
    if (!(dotProduct(ivec, kvec) <= toler)) return false;
    if (!(dotProduct(jvec, kvec) <= toler)) return false;
    float tempvec[3];
    crossProduct(ivec, jvec, tempvec);
    if (!(dotProduct(tempvec, kvec) >= 0.9f)) return false;//i cross j must be k, otherwise it contains a flip
    int method = 0;
    double trace = matrix[0][0] + matrix[1][1] + matrix[2][2];
    if (trace < 0.0)
    {
        method = 1;
        float tempf = matrix[0][0];
        if (matrix[1][1] > tempf)
        {
            method = 2;
            tempf = matrix[1][1];
        }
        if (matrix[2][2] > tempf)
        {
            method = 3;
        }
    }
    switch (method)
    {
        case 0:
        {
            double r = std::sqrt(1.0 + trace);
            double s = 0.5 / r;
            cijk[0] = 0.5 * r;
            cijk[1] = (matrix[2][1] - matrix[1][2]) * s;
            cijk[2] = (matrix[0][2] - matrix[2][0]) * s;
            cijk[3] = (matrix[1][0] - matrix[0][1]) * s;
        }
            break;
        case 1:
        {
            double r = std::sqrt(1.0 + matrix[0][0] - matrix[1][1] - matrix[2][2]);
            double s = 0.5 / r;
            cijk[0] = (matrix[2][1] - matrix[1][2]) * s;
            cijk[1] = 0.5 * r;
            cijk[2] = (matrix[0][1] + matrix[1][0]) * s;
            cijk[3] = (matrix[2][0] + matrix[0][2]) * s;
        }
            break;
        case 2:
        {//DISCLAIMER: these last two were worked out by pattern since they aren't on wikipedia
            double r = std::sqrt(1.0 - matrix[0][0] + matrix[1][1] - matrix[2][2]);
            double s = 0.5 / r;
            cijk[0] = (matrix[0][2] - matrix[2][0]) * s;
            cijk[1] = (matrix[0][1] + matrix[1][0]) * s;
            cijk[2] = 0.5 * r;
            cijk[3] = (matrix[1][2] + matrix[2][1]) * s;
        }
            break;
        case 3:
        {
            double r = std::sqrt(1.0 - matrix[0][0] - matrix[1][1] + matrix[2][2]);
            double s = 0.5 / r;
            cijk[0] = (matrix[1][0] - matrix[0][1]) * s;
            cijk[1] = (matrix[2][0] + matrix[0][2]) * s;
            cijk[2] = (matrix[1][2] + matrix[2][1]) * s;
            cijk[3] = 0.5 * r;
        }
            break;
        default:
            return false;
    }
    if (cijk[0] < 0.0f)
    {
        cijk[0] = -cijk[0];
        cijk[1] = -cijk[1];
        cijk[2] = -cijk[2];
        cijk[3] = -cijk[3];
    }
    return true;
}

/**
 * Return the remainder from the resulting division using the given values.
 * 
 * This method is written to match the result produced by the remainder() 
 * function that is part of C99 but not supported on all platforms.
 *
 * Code may appear verbose but it avoid functions calls to fabs(), ceil(),
 * and floor().
 *
 *     Note: X is the numerator.
 *           Y is the denominator.
 *
 *     The remainder() functions compute the value r such that r = x - n*y,
 *     where n is the integer nearest the exact value of x/y.
 *     
 *     If there are two integers closest to x/y, n shall be the even one. 
 *
 * @param numerator
 *    The numerator.
 * @param denominator
 *    The denominator.
 * @return
 *    The remainder from numerator divided by denominator.
 */
double
MathFunctions::remainder(const double numerator,
                         const double denominator)
{
    if (denominator == 0.0) {
        return 0.0;
    }
    
    const double quotient = numerator / denominator;
    
    /*
     * Integer value greater than or equal to the quotient
     * and its difference with the quotient (ceiling)
     */
    const int64_t nearestIntegerOne = static_cast<int64_t>(quotient + 0.5);
    double diffOne = quotient - nearestIntegerOne;
    if (diffOne < 0.0) diffOne = -diffOne;

    /*
     * Integer value less than or equal to the quotient
     * and its difference with the quotient (floor)
     */
    const int64_t nearestIntegerTwo = static_cast<int64_t>(quotient - 0.5);
    double diffTwo = quotient - nearestIntegerTwo;
    if (diffTwo < 0.0) diffTwo = -diffTwo;

    /*
     * Helps determine if the two integer value are the same
     * distance from the quotient (value will be very close
     * to zero).
     */
    double diffOneTwo = diffOne - diffTwo;
    if (diffOneTwo < 0.0) diffOneTwo = -diffOneTwo;

    int64_t nearestInteger = 0;
    
    /*
     * If the two integer values are the same distance from zero
     */
    if (diffOneTwo < 0.000001) {
        /*
         * Use the integer that is even.
         * Note that if an integer is even, first bit is zero.
         */
        if ((nearestIntegerOne & 1) == 0) {
            nearestInteger = nearestIntegerOne;
        }
        else {
            nearestInteger = nearestIntegerTwo;
        }
    }
    else if (diffOne < diffTwo) {
        nearestInteger = nearestIntegerOne;
    }
    else {
        nearestInteger = nearestIntegerTwo;
    }
    
    const double remainderValue = numerator - nearestInteger * denominator;
    return remainderValue;
}

/**
 * Return the value rounded to the nearest integral (integer) value.
 * 
 * @param value
 *    Value that is rounded.
 * @return
 *    Value rounded to nearest integral value.
 */
double
MathFunctions::round(const double value)
{
    if (value < 0.0) {
        return std::ceil(value - 0.5f);
    }
    return std::floor(value + 0.5f);
}

float MathFunctions::q_func(const float& x)
{//when using c++11 or later, could use erfc instead of this approximation
    if (x == 0.0f) return 0.5f;//below approximation is NaN for 0!
    if (isInf(x))
    {
        if (x > 0.0f) return 0;//inf
        return 1;//-inf
    }
    float ret;
    if (x < 0.0f)
    {
        ret = 1.0f - (1.0f - exp(1.4f * x)) * exp(-x * x / 2) / (x * -1.135f * sqrt(2 * 3.1415926f));
    } else {
        ret = (1.0f - exp(-1.4f * x)) * exp(-x * x / 2) / (x * 1.135f * sqrt(2 * 3.1415926f));
    }
    //formula from http://en.wikipedia.org/wiki/Q-function
    //references http://users.auth.gr/users/9/3/028239/public_html/pdf/Q_Approxim.pdf
    //which gives formula and constants for erfc, need to substitute and simplify
    //however, is wrong for negatives, so we substitute -x and subtract from 1
    if (!isNumeric(ret))
    {
        CaretAssert(abs(x) < 0.00001f);//should only be possible for very small inputs, so check before returning the answer for 0
        return 0.5f;
    }
    return ret;
}
