#ifndef __MATHFUNCTIONS_H__
#define __MATHFUNCTIONS_H__

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

    static void crossProduct(
                             const double v1[],
                             const double v2[],
                             double resultOut[]);
    
    static void normalizedCrossProduct(
                    const float x1[],
                                 const float x2[],
                                 float resultOut[]);

    static float normalizeVector(
                    float vectorsAll[],
                    const int32_t offset);

    static float normalizeVector(float vectorInOut[3]);

    static double normalizeVector(double vectorInOut[3]);

    static float vectorLength(const float vector[3]);

    static float vectorLength(
                    const float vectorsAll[],
                    const int32_t offset);

    static double vectorLength(const double vector[3]);

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
    
    static void subtractVectors(const float v1[3],
                                const float v2[3],
                                float resultOut[3]);

    static void subtractVectors(const double v1[3],
                                const double v2[3],
                                double resultOut[3]);
    static void addVectors(
                    const float v1[3],
                    const float v2[3],
                    float resultOut[3]);
    
    static void createUnitVector(
                    const float startXYZ[3],
                    const float endXYZ[3],
                    float unitVectorOut[3]);

    static void createUnitVector(
                                 const double startXYZ[3],
                                 const double endXYZ[3],
                                 double unitVectorOut[3]);
    
    static float dotProduct(
                    const float p1[3],
                    const float p2[3]);

    static double dotProduct(
                            const double p1[3],
                            const double p2[3]);
    
    static void addOffsetToVector(double v[3],
                                  const double offset[3]);
    
    static void subtractOffsetFromVector(double v[3],
                                         const double offset[3]);
    
    static float triangleArea(
                    const float v1[3],
                    const float v2[3],
                    const float v3[3]);

    static float triangleArea(const double v1[3],
                              const double v2[3],
                              const double v3[3]);
    
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

    static void vtkInvert3x3(const double A[3][3],
                             double AI[3][3]);
    
    static void vtkMultiply3x3(const double A[3][3],
                               const double B[3][3],
                               double C[3][3]);
    
    static int vtkJacobiN(double **a,
                       int n,
                       double *w,
                       double **v);

    static void vtkPerpendiculars(const double x[3],
                                  double y[3],
                                  double z[3],
                                  double theta);
    
    static double vtkDeterminant2x2(double a, double b, double c, double d);

    static bool lineIntersection2D(
                    const float p1[3],
                    const float p2[3],
                    const float q1[3],
                    const float q2[3],
                    const float tolerance,
                    float intersectionOut[3]);

    static bool vectorIntersection2D(
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

    static void nearestPointOnLine3D(const float p1[3],
                                     const float p2[3],
                                     const float point[3],
                                     float pointOnLineOut[3],
                                     float& p1ToPointOnLineNormalizedDistanceOut,
                                     float& distanceFromPointToPointOnLine);
    
    static bool arraysEqual(
                    const float a[],
                    const float b[],
                    const int32_t numElements);

    static void averageOfTwoCoordinates(
                    const float c1[3],
                    const float c2[3],
                    float outputAverage[3]);

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
                    float outputAverage[],
                    const int32_t outputOffset);

    static void averageOfFourCoordinates(const float c1[3],
                                         const float c2[3],
                                         const float c3[3],
                                         const float c4[3],
                                         float outputAverage[3]);
    
    static float angle(
                    const float p1[3],
                    const float p2[3],
                    const float p3[3]);

    static float signedAngle(
                    const float pi[3],
                    const float pj[3],
                    const float pk[3],
                    const float n[3]);

    static float angleInDegreesBetweenVectors(const float u[3], const float v[3]);
    
    static bool isOddNumber(const int32_t number);

    static bool isEvenNumber(const int32_t number);
    
    static bool isNaN(const float number);
    
    static bool isPosInf(const float number);
    
    static bool isNegInf(const float number);
    
    ///true if either inf or -inf
    static bool isInf(const float number);
    
    ///true only if not NaN, inf, or -inf
    static bool isNumeric(const float number);

    static bool compareArrays(
                    const float a1[],
                    const float a2[],
                    const int32_t numElemets,
                    const float tolerance);

    static bool compareValuesEqual(const float* data,
                                   const int32_t numberOfElements,
                                   const float value,
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
    
    ///greatest common divisor
    static uint32_t gcd(uint32_t num1, uint32_t num2);
    
    /**
     * Is the value very, very close to zero?
     * @param value
     *    Value to test.
     * @return true if approximately zero, else false.
     */
    static inline bool isZero(const float value) {
        if (value >  0.00001) return false;
        if (value < -0.00001) return false;
        return true;
    }
    
    ///convert quaternion to rotation matrix
    static void quaternToMatrix(const float cijk[4], float matrix[3][3]);
    
    ///convert quaternion to rotation matrix
    static void quaternToMatrix(const double cijk[4], double matrix[3][3]);
    
    ///try to convert 3x3 matrix to quaternion (return false if not a rotation matrix)
    static bool matrixToQuatern(const float matrix[3][3], float cijk[4]);

    ///try to convert 3x3 matrix to quaternion (return false if not a rotation matrix)
    static bool matrixToQuatern(const double matrix[3][3], double cijk[4]);
    
    static double remainder(const double numerator,
                            const double denominator);
    
    static double round(const double value);
    
    ///one minus cdf of standard normal distribution
    static float q_func(const float& x);
    
    static void expandBox(float bottomLeft[3],
                          float bottomRight[3],
                          float topRight[3],
                          float topLeft[3],
                          const float extraSpaceX,
                          const float extraSpaceY);
    
    static void expandBoxPixels3D(double bottomLeft[3],
                                  double bottomRight[3],
                                  double topRight[3],
                                  double topLeft[3],
                                  const double extraSpacePixels);
    
    static void expandBoxPercentage3D(float bottomLeft[3],
                                      float bottomRight[3],
                                      float topRight[3],
                                      float topLeft[3],
                                      const float extraSpacePercentage);
    
    static void expandLinePercentage3D(float u[3],
                                       float v[3],
                                       const float extraSpacePercent);
    
    static void expandLinePixels3D(double u[3],
                                   double v[3],
                                   const double extraSpacePixels);
    
};

} // namespace

#endif // __MATHFUNCTIONS_H__
