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

#include "Vector3d.h"

using namespace std;
using namespace caret;

Vector3d Vector3d::cross(const Vector3d& right) const
{
    Vector3d ret;
    MathFunctions::crossProduct(m_vec, right.m_vec, ret.m_vec);
    return ret;
}

float Vector3d::dot(const Vector3d& right) const
{
    return MathFunctions::dotProduct(m_vec, right.m_vec);
}

float Vector3d::length() const
{
    return MathFunctions::vectorLength(m_vec);
}

Vector3d Vector3d::normal() const
{
    Vector3d ret = *this;
    MathFunctions::normalizeVector(ret.m_vec);
    return ret;
}

Vector3d::Vector3d()
{
    m_vec[0] = 0.0f;
    m_vec[1] = 0.0f;
    m_vec[2] = 0.0f;
}

Vector3d::Vector3d(const float* right)
{
    m_vec[0] = right[0];
    m_vec[1] = right[1];
    m_vec[2] = right[2];
}

float& Vector3d::operator[](const int& index)
{
    CaretAssert(index > -1 && index < 3);
    return m_vec[index];
}

const float& Vector3d::operator[](const int& index) const
{
    CaretAssert(index > -1 && index < 3);
    return m_vec[index];
}

Vector3d Vector3d::operator*(const float& right) const
{
    Vector3d ret = *this;
    ret *= right;
    return ret;
}

Vector3d& Vector3d::operator*=(const float& right)
{
    m_vec[0] *= right;
    m_vec[1] *= right;
    m_vec[2] *= right;
    return *this;
}

Vector3d operator*(const float& left, const Vector3d& right)
{
    return right * left;
}

Vector3d Vector3d::operator+(const Vector3d& right) const
{
    Vector3d ret = *this;
    ret += right;
    return ret;
}

Vector3d& Vector3d::operator+=(const Vector3d& right)
{
    m_vec[0] += right.m_vec[0];
    m_vec[1] += right.m_vec[1];
    m_vec[2] += right.m_vec[2];
    return *this;
}

Vector3d Vector3d::operator-(const Vector3d& right) const
{
    Vector3d ret = *this;
    ret -= right;
    return ret;
}

Vector3d& Vector3d::operator-=(const Vector3d& right)
{
    m_vec[0] -= right.m_vec[0];
    m_vec[1] -= right.m_vec[1];
    m_vec[2] -= right.m_vec[2];
    return *this;
}

Vector3d Vector3d::operator/(const float& right) const
{
    Vector3d ret = *this;
    ret /= right;
    return ret;
}

Vector3d& Vector3d::operator/=(const float& right)
{
    m_vec[0] /= right;
    m_vec[1] /= right;
    m_vec[2] /= right;
    return *this;
}

Vector3d& Vector3d::operator=(const float* right)
{
    m_vec[0] = right[0];
    m_vec[1] = right[1];
    m_vec[2] = right[2];
    return *this;
}
