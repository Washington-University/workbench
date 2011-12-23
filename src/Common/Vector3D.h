#ifndef __VECTOR_3D_H__
#define __VECTOR_3D_H__

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
#include "CaretAssert.h"

namespace caret {
    
    struct Vector3D
    {
        float m_vec[3];
        //vector functions
        float dot(const Vector3D& right) const;
        Vector3D cross(const Vector3D& right) const;
        Vector3D normal(float* origLength = NULL) const;
        float length() const;
        //constructors
        Vector3D();
        Vector3D(const float* right);
        //compatibility operators
        float& operator[](const int& index);
        const float& operator[](const int& index) const;
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
    };
    
    Vector3D operator*(const float& left, const Vector3D& right);

}
#endif //__VECTOR_3D_H__