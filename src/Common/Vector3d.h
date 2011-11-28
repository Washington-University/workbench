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
    
    struct Vector3d
    {
        float m_vec[3];
        //vector functions
        float dot(const Vector3d& right) const;
        Vector3d cross(const Vector3d& right) const;
        Vector3d normal() const;
        float length() const;
        //constructors
        Vector3d();
        Vector3d(const float* right);
        //compatibility operators
        float& operator[](const int& index);
        const float& operator[](const int& index) const;
        Vector3d& operator=(const float* right);
        //numerical operators
        Vector3d& operator+=(const Vector3d& right);
        Vector3d& operator-=(const Vector3d& right);
        Vector3d& operator*=(const float& right);
        Vector3d& operator/=(const float& right);
        Vector3d operator+(const Vector3d& right) const;
        Vector3d operator-(const Vector3d& right) const;
        Vector3d operator*(const float& right) const;
        Vector3d operator/(const float& right) const;//NOTE: doesn't really make sense to have the other division, unlike multiplication
    };
    
    Vector3d operator*(const float& left, const Vector3d& right);

}
#endif //__VECTOR_3D_H__