#ifndef __VECTOR_OPERATION_H__
#define __VECTOR_OPERATION_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2015  Washington University School of Medicine
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

#include "AString.h"
#include "Vector3D.h"

#include <vector>

namespace caret
{
    
    ///helper class for doing user-specified operations on 3D vectors
    class VectorOperation
    {
    public:
        enum Operation
        {
            DOT,
            CROSS,
            ADD,
            SUBTRACT
        };
        static Operation stringToOperation(const AString& string, bool& ok);
        static AString operationToString(const Operation& myOp);
        static std::vector<Operation> getAllOperations();
        static bool operationReturnsScalar(const Operation& myOp);
        static Vector3D doVectorOperation(const Vector3D& first, const Vector3D& second, const Operation& myOp);
        static float doScalarOperation(const Vector3D& first, const Vector3D& second, const Operation& myOp);
    };
    
}

#endif //__VECTOR_OPERATION_H__
