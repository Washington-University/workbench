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

#include "VectorOperation.h"

#include "CaretAssert.h"

using namespace std;
using namespace caret;

VectorOperation::Operation VectorOperation::stringToOperation(const AString& string, bool& ok)
{
    ok = true;
    if (string == "DOT")
    {
        return DOT;
    } else if (string == "CROSS") {
        return CROSS;
    } else if (string == "ADD") {
        return ADD;
    } else if (string == "SUBTRACT") {
        return SUBTRACT;
    }
    ok = false;
    return DOT;//have to return something
}

AString VectorOperation::operationToString(const VectorOperation::Operation& myOp)
{
    switch (myOp)
    {
        case DOT:
            return "DOT";
        case CROSS:
            return "CROSS";
        case ADD:
            return "ADD";
        case SUBTRACT:
            return "SUBTRACT";
    }
    return "";
}

vector<VectorOperation::Operation> VectorOperation::getAllOperations()
{
    vector<Operation> ret;
    ret.push_back(DOT);
    ret.push_back(CROSS);
    ret.push_back(ADD);
    ret.push_back(SUBTRACT);
    return ret;
}

bool VectorOperation::operationReturnsScalar(const VectorOperation::Operation& myOp)
{
    switch (myOp)
    {
        case DOT:
            return true;
        default:
            return false;
    }
}

Vector3D VectorOperation::doVectorOperation(const Vector3D& first, const Vector3D& second, const Operation& myOp)
{
    switch (myOp)
    {
        case CROSS:
            return first.cross(second);
        case ADD:
            return first + second;
        case SUBTRACT:
            return first - second;
        default:
            CaretAssert(false);
            return Vector3D();
    }
}

float VectorOperation::doScalarOperation(const Vector3D& first, const Vector3D& second, const VectorOperation::Operation& myOp)
{
    switch (myOp)
    {
        case DOT:
            return first.dot(second);
        default:
            CaretAssert(false);
            return 0.0f;
    }
}
