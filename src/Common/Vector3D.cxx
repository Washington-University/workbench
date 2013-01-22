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

Vector3D::Vector3D()
{
    m_vec[0] = 0.0f;
    m_vec[1] = 0.0f;
    m_vec[2] = 0.0f;
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
