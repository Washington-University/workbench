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

#include <cmath>

#include <QStringList>

#include "MathFunctions.h"
#include "Vector3D.h"

using namespace std;
using namespace caret;

Vector3D Vector3D::cross(const Vector3D& right) const
{
    Vector3D ret;
    MathFunctions::crossProduct(m_vec, right.m_vec, ret.m_vec);
    return ret;
}

float Vector3D::dot(const Vector3D& right) const
{
    return MathFunctions::dotProduct(m_vec, right.m_vec);
}

float Vector3D::length() const
{
    return MathFunctions::vectorLength(m_vec);
}

float Vector3D::lengthsquared() const
{
    return m_vec[0] * m_vec[0] + m_vec[1] * m_vec[1] + m_vec[2] * m_vec[2];
}

Vector3D Vector3D::normal(float* origLength) const
{
    Vector3D ret = *this;
    if (origLength != NULL)
    {
        *origLength = MathFunctions::normalizeVector(ret.m_vec);
    } else {
        MathFunctions::normalizeVector(ret.m_vec);
    }
    return ret;
}

float Vector3D::distToLine(const Vector3D& p1, const Vector3D& p2, Vector3D* closePointOut) const
{
    Vector3D diff = p2 - p1;
    float origLength;
    Vector3D diffHat = diff.normal(&origLength);
    if (origLength == 0.0f)//line is degenerate, return distance to point - we could return zero, but this seems like it would be better behaved
    {
        if (closePointOut != NULL) *closePointOut = p1;
        return (*this - p1).length();
    }
    float distAlong = diffHat.dot(*this - p1);
    Vector3D closePoint = p1 + diffHat * distAlong;
    if (closePointOut != NULL) *closePointOut = closePoint;
    return (*this - closePoint).length();
}

float Vector3D::distToLineSegment(const Vector3D& p1, const Vector3D& p2, Vector3D* closePointOut) const
{
    float origLength;
    Vector3D diffHat = (p2 - p1).normal(&origLength);
    if (origLength == 0.0f)//line segment is degenerate, return distance to point
    {
        if (closePointOut != NULL) *closePointOut = p1;
        return (*this - p1).length();
    }
    float distAlong = diffHat.dot(*this - p1);
    if (distAlong < 0.0f) distAlong = 0.0f;
    if (distAlong > origLength) distAlong = origLength;
    Vector3D closePoint = p1 + diffHat * distAlong;
    if (closePointOut != NULL) *closePointOut = closePoint;
    return (*this - closePoint).length();
}

Vector3D::Vector3D()
{
    m_vec[0] = 0.0f;
    m_vec[1] = 0.0f;
    m_vec[2] = 0.0f;
}

Vector3D::Vector3D(const float& x, const float& y, const float& z)
{
    m_vec[0] = x;
    m_vec[1] = y;
    m_vec[2] = z;
}

Vector3D::Vector3D(const float* right)
{
    m_vec[0] = right[0];
    m_vec[1] = right[1];
    m_vec[2] = right[2];
}

float& Vector3D::operator[](const int64_t& index)
{
    CaretAssert(index > -1 && index < 3);
    return m_vec[index];
}

const float& Vector3D::operator[](const int64_t& index) const
{
    CaretAssert(index > -1 && index < 3);
    return m_vec[index];
}

float& Vector3D::operator[](const int32_t& index)
{
    CaretAssert(index > -1 && index < 3);
    return m_vec[index];
}

const float& Vector3D::operator[](const int32_t& index) const
{
    CaretAssert(index > -1 && index < 3);
    return m_vec[index];
}

Vector3D Vector3D::operator*(const float& right) const
{
    Vector3D ret = *this;
    ret *= right;
    return ret;
}

Vector3D& Vector3D::operator*=(const float& right)
{
    m_vec[0] *= right;
    m_vec[1] *= right;
    m_vec[2] *= right;
    return *this;
}

Vector3D caret::operator*(const float& left, const Vector3D& right)
{
    return right * left;
}

Vector3D Vector3D::operator+(const Vector3D& right) const
{
    Vector3D ret = *this;
    ret += right;
    return ret;
}

Vector3D& Vector3D::operator+=(const Vector3D& right)
{
    m_vec[0] += right.m_vec[0];
    m_vec[1] += right.m_vec[1];
    m_vec[2] += right.m_vec[2];
    return *this;
}

Vector3D Vector3D::operator-(const Vector3D& right) const
{
    Vector3D ret = *this;
    ret -= right;
    return ret;
}

Vector3D Vector3D::operator-() const
{
    Vector3D ret;
    ret.m_vec[0] = -m_vec[0];
    ret.m_vec[1] = -m_vec[1];
    ret.m_vec[2] = -m_vec[2];
    return ret;
}

Vector3D& Vector3D::operator-=(const Vector3D& right)
{
    m_vec[0] -= right.m_vec[0];
    m_vec[1] -= right.m_vec[1];
    m_vec[2] -= right.m_vec[2];
    return *this;
}

Vector3D Vector3D::operator/(const float& right) const
{
    Vector3D ret = *this;
    ret /= right;
    return ret;
}

Vector3D& Vector3D::operator/=(const float& right)
{
    m_vec[0] /= right;
    m_vec[1] /= right;
    m_vec[2] /= right;
    return *this;
}

Vector3D& Vector3D::operator=(const float* right)
{
    m_vec[0] = right[0];
    m_vec[1] = right[1];
    m_vec[2] = right[2];
    return *this;
}

void Vector3D::fill(const float value)
{
    m_vec[0] = value;
    m_vec[1] = value;
    m_vec[2] = value;
}

void Vector3D::set(const float x, const float y, const float z)
{
    m_vec[0] = x;
    m_vec[1] = y;
    m_vec[2] = z;
}

/**
 * Compute the signed angle between 'this' and 'right'.
 * Both 'this' and 'right' must lie in the plane with the normal vector 'normal'
 * @param right
 *    Vector that has angle computed between 'this'
 * @param normal
 *    Normal vector of plane in which both vector lie
 * @return
 *    The signed angle between 'this' and 'right'
 *
 * NOTE: Code is adapted from vtkMath::SignedAngleBetweenVectors()
 * https://vtk.org/doc/nightly/html/classvtkMath.html
 *
 *  =========================================================================
 *
 *  Program:   Visualization Toolkit
 *  Module:    vtkMath.h
 *
 *  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
 *  All rights reserved.
 *  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.
 *
 *  This software is distributed WITHOUT ANY WARRANTY; without even
 *  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 *  PURPOSE.  See the above copyright notice for more information.
 *
 *  =========================================================================
 *  Copyright 2011 Sandia Corporation.
 *  Under the terms of Contract DE-AC04-94AL85000, there is a non-exclusive
 *  license for use of this work by or on behalf of the
 *  U.S. Government. Redistribution and use in source and binary forms, with
 *  or without modification, are permitted provided that this Notice and any
 *  statement of authorship are reproduced on all copies.
 *
 *  Contact: pppebay@sandia.gov,dcthomp@sandia.gov
 *
 * =========================================================================
 *
 */
float
Vector3D::signedAngleRadians(const Vector3D& right,
                             const Vector3D& normal) const
{
    /*
     * Code from vtkMath::SignedAngleBetweenVectors()
     *
     * double cross[3];
     * vtkMath::Cross(v1, v2, cross);
     * double angle = atan2(vtkMath::Norm(cross), vtkMath::Dot(v1, v2));
     * return vtkMath::Dot(cross, vn) >= 0 ? angle : -angle;
     */
    
    Vector3D cross(this->cross(right));
    double angle(atan2(cross.length(),
                       this->dot(right)));
    angle = (cross.dot(normal) >= 0) ? angle : -angle;
    return angle;
}

/**
 * @return vector as a string separated by commas and enclosed in parenthesis
 * @param precision
 *    Digits right of decimal
 */
AString
Vector3D::toString(const int32_t precision) const
{
    return AString("("
                   + AString::fromNumbers(m_vec,
                                          3,    /* number of elements */
                                          ", ", /* separator */
                                          'f',  /* format */
                                          precision)
                   + ")");

}

/**
 * Read three values from a string and numeric values in Vector3D
 * @param s
 *    The string with values separated by a comma
 * @param validFlag
 *   If not NULL, will be set to true if conversion from string to
 *   Vector3D was successful.
 */
Vector3D
Vector3D::fromString(const AString& s,
                     bool* validFlag)
{
    if (validFlag != NULL) {
        *validFlag = false;
    }
    
    Vector3D xyz;
    xyz.fill(0.0);
    
    const QStringList stringList(s.split(",",
                                         Qt::SkipEmptyParts));
    if (stringList.size() == 3) {
        bool xValid(false), yValid(false), zValid(false);
        xyz[0] = stringList.at(0).toFloat(&xValid);
        xyz[1] = stringList.at(1).toFloat(&yValid);
        xyz[2] = stringList.at(2).toFloat(&zValid);
        
        if (validFlag != NULL) {
            *validFlag = (xValid && yValid && zValid);
        }
    }

    return xyz;
}
