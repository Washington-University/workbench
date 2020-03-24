#ifndef __VECTOR_3D_H__
#define __VECTOR_3D_H__

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

#include "CaretAssert.h"

namespace caret {
    
    class Vector3D
    {
        float m_vec[3];
    public:
        //vector functions
        float dot(const Vector3D& right) const;
        Vector3D cross(const Vector3D& right) const;
        Vector3D normal(float* origLength = NULL) const;
        float length() const;
        float lengthsquared() const;
        //geometry functions
        float distToLine(const Vector3D& p1, const Vector3D& p2, Vector3D* closePointOut = NULL) const;
        float distToLineSegment(const Vector3D& p1, const Vector3D& p2, Vector3D* closePointOut = NULL) const;
        //constructors
        Vector3D();
        Vector3D(const float& x, const float& y, const float& z);
        Vector3D(const float* right);
        //compatibility operators
        float& operator[](const int64_t& index);
        const float& operator[](const int64_t& index) const;
        float& operator[](const int32_t& index);
        const float& operator[](const int32_t& index) const;
        Vector3D& operator=(const float* right);
        //numerical operators
        Vector3D& operator+=(const Vector3D& right);
        Vector3D& operator-=(const Vector3D& right);
        Vector3D& operator*=(const float& right);
        Vector3D& operator/=(const float& right);
        Vector3D operator+(const Vector3D& right) const;
        Vector3D operator-(const Vector3D& right) const;
        Vector3D operator-() const;
        Vector3D operator*(const float& right) const;
        Vector3D operator/(const float& right) const;//NOTE: doesn't really make sense to have the other division, unlike multiplication
        inline operator float*() { return m_vec; }
        inline operator const float*() const { return m_vec; }
    };
    
    Vector3D operator*(const float& left, const Vector3D& right);

}
#endif //__VECTOR_3D_H__
