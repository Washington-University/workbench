#ifndef __MATHFUNCTIONS_H__
#define __MATHFUNCTIONS_H__

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



#include "CaretObject.h"


#include <stdint.h>


namespace caret {

/**
 * Various mathematical functions.
 */
class MathFunctions : public CaretObject {

private:
    MathFunctions();

public:
    virtual ~MathFunctions();

public:
    static int64_t combinations(
                    const int64_t n,
                    const int64_t k);

    static int64_t permutations(
                    const int64_t n,
                    const int64_t k);

    static int64_t factorial(const int64_t n);

    static bool normalVector(
                    const float v1[3],
                    const float v2[3],
                    const float v3[3],
                    float normalVectorOut[3]);

    static bool normalVector(
                    const double v1[3],
                    const double v2[3],
                    const double v3[3],
                    double normalVectorOut[3]);

    static void normalVectorDirection(
                    const float v1[3],
                    const float v2[3],
                    const float v3[3],
                    float directionOut[3]);

    static void crossProduct(
                    const float v1[],
                    const float v2[],
                    float resultOut[]);

    static void normalizedCrossProduct(
                    const float x1[],
                                 const float x2[],
                                 float resultOut[]);

    static float normalizeVector(
                    float vectorsAll[],
                    const int32_t offset);

    static float normalizeVector(float* vectorInOut);

    static double normalizeVector(double* vectorInOut);

    static float vectorLength(const float* vector);

    static float vectorLength(
                    const float vectorsAll[],
                    const int32_t offset);

    static double vectorLength(const double* vector);

    static float distanceSquared3D(
                    const float p1[3],
                    const float p2[3]);

    static float distanceSquared3D(
                    const float xyzAll[],
                    const int32_t offsetCoord1,
                    const int32_t offsetCoord2);

    static float distance3D(
                    const float p1[3],
                    const float p2[3]);

    static double distanceSquared3D(
                    const double p1[3],
                    const double p2[3]);

    static double distance3D(
                    const double p1[3],
                    const double p2[3]);

    static double distanceSquared2D(const double x1,
                                    const double y1,
                                    const double x2,
                                    const double y2);
    
    static void subtractVectors(
                    const float v1[],
                    const float v2[],
                    float result[3]);

    static void createUnitVector(
                    const float startXYZ[3],
                    const float endXYZ[3],
                    float unitVectorOut[3]);

    static float dotProduct(
                    const float p1[3],
                    const float p2[3]);

    static float triangleArea(
                    const float v1[3],
                    const float v2[3],
                    const float v3[3]);

    static float triangleArea(
                    const float xyzAll[],
                    const int32_t offsetCoord1,
                    const int32_t offsetCoord2,
                    const int32_t offsetCoord3);

    static float triangleAreaSigned2D(
                    const float p1[3],
                    const float p2[3],
                    const float p3[3]);

    static float triangleAreaSigned3D(
                    const float referenceNormal[3],
                    const float p1[3],
                    const float p2[3],
                    const float p3[3]);

    static void vtkLinearSolve3x3(
                    const float A[3][3],
                    const float x[],
                    float y[]);

    static void vtkLUSolve3x3(
                    const float A[3][3],
                    const int32_t index[],
                    float x[]);

    static void vtkLUFactor3x3(
                    float A[3][3],
                    int32_t index[]);

    static bool lineIntersection2D(
                    const float p1[3],
                    const float p2[3],
                    const float q1[3],
                    const float q2[3],
                    const float tolerance,
                    float intersectionOut[3]);

    static bool rayIntersectPlane(
                    const float p1[3],
                    const float p2[3],
                    const float p3[3],
                    const float rayOrigin[3],
                    const float rayVector[3],
                    float intersectionXYZandDistance[3]);

    static void projectPoint(
                    const float pt[3],
                    const float origin[3],
                    const float normal[3],
                              float projectedPointOut[3]);

    static float signedDistanceFromPlane(
                    const float planeNormal[3],
                    const float pointInPlane[3],
                    const float queryPoint[3]);

    static int32_t limitRange(
                    const int32_t value,
                    const int32_t minimumValue,
                    const int32_t maximumValue);

    static float limitRange(
                    const float value,
                    const float minimumValue,
                    const float maximumValue);

    static double limitRange(
                    const double value,
                    const double minimumValue,
                    const double maximumValue);

    static float distanceToLine3D(
                    const float p1[3],
                    const float p2[3],
                    const float point[3]);

    static bool arraysEqual(
                    const float a[],
                    const float b[],
                    const int32_t numElements);

    static void averageOfThreeCoordinates(
                    const float c1[3],
                    const float c2[3],
                    const float c3[3],
                    float outputAverage[3]);

    static void averageOfThreeCoordinates(
                    const float xyzAll[],
                    const int32_t offsetCoord1,
                    const int32_t offsetCoord2,
                    const int32_t offsetCoord3,
                    float outputAverage[3],
                    const int32_t outputOffset);

    static float angle(
                    const float p1[3],
                    const float p2[3],
                    const float p3[3]);

    static float signedAngle(
                    const float pi[3],
                    const float pj[3],
                    const float pk[3],
                    const float n[3]);

    static bool isOddNumber(const int32_t number);

    static bool isEvenNumber(const int32_t number);

    static bool compareArrays(
                    const float a1[],
                    const float a2[],
                    const int32_t numElemets,
                    const float tolerance);

    static int32_t clamp(
                    const int32_t value,
                    const int32_t minimum,
                    const int32_t maximum);

    static float clamp(
                    const float value,
                    const float minimum,
                    const float maximum);

    static float toRadians(float angle);
    
    static float toDegrees(float radians);
};

} // namespace

#endif // __MATHFUNCTIONS_H__
