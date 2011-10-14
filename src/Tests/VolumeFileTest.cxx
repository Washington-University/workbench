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
#include "VolumeFileTest.h"

#include "FloatMatrix.h"
#include "VolumeFile.h"

#include <ctime>
#include <cstdlib>

using namespace caret;
using namespace std;

VolumeFileTest::VolumeFileTest(const AString& identifier) : TestInterface(identifier)
{
}

void VolumeFileTest::execute()
{
    VolumeFile myTestVol;
    vector<int64_t> myDims;
    myDims.push_back(20);//i
    myDims.push_back(20);//j
    myDims.push_back(20);//k
    myDims.push_back(10);//time
    const int64_t numComponents = 3;//simulate rgb
    FloatMatrix indexSpace = FloatMatrix::zeros(4,4);//start with zeros as a 4x4 style matrix
    indexSpace[3][3] = 1.1f;//test a wrong element, because it SHOULD ignore it
    indexSpace[0][2] = 1.0f;//give it an odd orientation
    indexSpace[1][0] = 1.0f;//this is yzx, ie, PIL
    indexSpace[2][1] = 1.0f;
    myTestVol.reinitialize(myDims, indexSpace.getMatrix(), numComponents);
    float outIndex1, outIndex2, outIndex3;
    const float testAllowance = 0.00001f;//allow it to be wrong by this much
    myTestVol.spaceToIndex(1.0f, 0.0f, 0.0f, outIndex1, outIndex2, outIndex3);
    if (outIndex1 < 0.0f - testAllowance || outIndex1 > 0.0f + testAllowance)
    {
        setFailed(AString("index 1 failed: needed 0.0, got ") + AString::number(outIndex1));
    }
    if (outIndex2 < 0.0f - testAllowance || outIndex2 > 0.0f + testAllowance)
    {
        setFailed(AString("index 2 failed: needed 0.0, got ") + AString::number(outIndex2));
    }
    if (outIndex3 < 1.0f - testAllowance || outIndex3 > 1.0f + testAllowance)
    {
        setFailed(AString("index 3 failed: needed 0.0, got ") + AString::number(outIndex3));
    }
    time_t myseed = time(NULL);
    srand(myseed);//initialize RNG to fill, so we can compare afterwards
    int64_t i, j, k, t, c, testIndex;
    for (t = 0; t < myDims[3]; ++t)
    {
        for (k = 0; k < myDims[2]; ++k)
        {
            for (j = 0; j < myDims[1]; ++j)
            {
                for (i = 0; i < myDims[0]; ++i)
                {
                    for (c = 0; c < numComponents; ++c)
                    {
                        testIndex = myTestVol.getIndex(i, j, k, t, c);
                        if (testIndex != i + myDims[0] * (j + myDims[1] * (k + myDims[2] * (t + myDims[3] * c))))
                        {
                            setFailed(AString("index of (") + AString::number(i) + ", " + AString::number(j) + ", " + AString::number(k) + ", " + AString::number(t) + ", " + AString::number(c) + ") should be " +
                                AString::number(i + myDims[0] * (j + myDims[1] * (k + myDims[2] * (t + myDims[3] * c)))) + " but was " + AString::number(testIndex));
                            return;
                        }
                        myTestVol.setValue((float)rand(), i, j, k, t, c);
                    }
                }
            }
        }
    }
    srand(myseed);//now we compare
    for (t = 0; t < myDims[3]; ++t)
    {
        for (k = 0; k < myDims[2]; ++k)
        {
            for (j = 0; j < myDims[1]; ++j)
            {
                for (i = 0; i < myDims[0]; ++i)
                {
                    for (c = 0; c < numComponents; ++c)
                    {
                        if (myTestVol.getValue(i, j, k, t, c) != (float)rand())
                        {
                            setFailed(AString("data was not consistent starting at (") + AString::number(i) + ", " + AString::number(j) + ", " + AString::number(k) + ", " + AString::number(t) + ", " + AString::number(c) + ")");
                            return;
                        }
                    }
                }
            }
        }
    }
}
