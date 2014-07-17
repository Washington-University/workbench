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
#include "QuatTest.h"
#include "MathFunctions.h"
#include "Vector3D.h"
#include <cstdlib>
#include <cmath>

using namespace caret;
using namespace std;

QuatTest::QuatTest(const AString& identifier) : TestInterface(identifier)
{
}

bool QuatTest::matrixMatches(const float a[3][3], const float b[3][3])
{
    const float toler = 0.0001f;
    for (int i = 0; i < 3; ++i)
        for (int j = 0; j < 3; ++j)
            if (abs(a[i][j] - b[i][j]) > toler) return false;
    return true;
}

bool QuatTest::quatMatches(const float a[4], const float b[4])
{
    const float toler = 0.0001f;
    for (int i = 0; i < 4; ++i)
        if (abs(a[i] - b[i]) > toler) return false;
    return true;
}

AString QuatTest::showMat(const float a[3][3])
{
    AString ret = "\n";
    for (int i = 0; i < 3; ++i)
    {
        ret += AString::number(a[i][0]);
        for (int j = 1; j < 3; ++j)
        {
            ret += "\t" + AString::number(a[i][j]);
        }
        ret += "\n";
    }
    return ret;
}

AString QuatTest::showQuat(const float a[4])
{
    AString ret = AString::number(a[0]);
    for (int i = 1; i < 3; ++i)
        ret += "  " + AString::number(a[i]);
    return ret;
}

void QuatTest::execute()
{
    const int ITERS = 1000;
    for (int i = 0; i < ITERS; ++i)
    {
        float startquat[4] = { (rand() & 32767) / 32767.0f * 2.0f - 1.0f, (rand() & 32767) / 32767.0f * 2.0f - 1.0f,
                            (rand() & 32767) / 32767.0f * 2.0f - 1.0f, (rand() & 32767) / 32767.0f * 2.0f - 1.0f };
        float mat[3][3], endquat[4];
        float startlen = sqrt(startquat[0] * startquat[0] + startquat[1] * startquat[1] + startquat[2] * startquat[2] + startquat[3] * startquat[3]);
        if (startlen < 0.01f) continue;
        if (startquat[0] < 0.0f) startlen = -startlen;
        startquat[0] /= startlen;
        startquat[1] /= startlen;
        startquat[2] /= startlen;
        startquat[3] /= startlen;
        MathFunctions::quaternToMatrix(startquat, mat);
        if (!MathFunctions::matrixToQuatern(mat, endquat))
        {
            setFailed("matrix from quaternToMatrix failed to convert to quaternion for quat: " + showQuat(startquat));
        }
        if (!quatMatches(startquat, endquat))
        {
            setFailed("quaternion " + showQuat(startquat) + " produced nonmatching quaternion " + showQuat(endquat));
        }
    }
    for (int i = 0; i < ITERS; ++i)
    {
        float ivecf[3] = { (rand() & 32767) / 32767.0f * 2.0f - 1.0f, (rand() & 32767) / 32767.0f * 2.0f - 1.0f, (rand() & 32767) / 32767.0f * 2.0f - 1.0f };
        Vector3D ivec = ivecf;
        if (ivec.length() < 0.01f) continue;
        float jvecf[3] = { (rand() & 32767) / 32767.0f * 2.0f - 1.0f, (rand() & 32767) / 32767.0f * 2.0f - 1.0f, (rand() & 32767) / 32767.0f * 2.0f - 1.0f };
        Vector3D jvec = jvecf;
        ivec = ivec.normal();
        jvec = jvec - ivec * jvec.dot(ivec);//makes jvec orthogonal to ivec
        if (jvec.length() < 0.01f) continue;
        jvec = jvec.normal();
        Vector3D kvec = ivec.cross(jvec);//should be a unit vector, orthogonal to both
        float startmat[3][3], quat[4], endmat[3][3];
        startmat[0][0] = ivec[0];
        startmat[1][0] = ivec[1];
        startmat[2][0] = ivec[2];
        startmat[0][1] = jvec[0];
        startmat[1][1] = jvec[1];
        startmat[2][1] = jvec[2];
        startmat[0][2] = kvec[0];
        startmat[1][2] = kvec[1];
        startmat[2][2] = kvec[2];
        if (!MathFunctions::matrixToQuatern(startmat, quat))
        {
            setFailed("failed to convert matrix to quaternion:" + showMat(startmat));
            continue;
        }
        MathFunctions::quaternToMatrix(quat, endmat);
        if (!matrixMatches(startmat, endmat))
        {
            setFailed("matrix:" + showMat(startmat) + "produced nonmatching matrix:" + showMat(endmat));
        }
    }
}
